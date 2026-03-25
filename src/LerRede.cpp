/*
 * LerRede.cpp
 *
 *  Created on: 23 de jun de 2017
 *      Author: Eduardo
 */
#include "LerRede.h"

using namespace rapidjson;

Rede::Rede() {
  entrada = "lixo";
  entradaLog="lixo";
  validacaoJson = tipoValidacaoJson_t::json;
  tipoSimulacao = tipoSimulacao_t::rede;
  vg1dSP=0;
  impfiles=0;
  malha=0;
  conexFR=0;
  nsisprod=0;
  nfonteR=0;
  chutHol=0;
  relax=0;
  tipoRede=0;
  redeParalela=0;
  anelGL=0;
  injec=0;
  chaveredeT=0;
  fluidoRede=0;
  tabelaDinamica=0;
  TmaxR=0;//alteracao4
  apenasPreProc=0;
  chute=0;
  nthrRede=0;
  limConverge=0.001;
  tipoModeloDrift=1;

  tramoPrimario=-1;
  tramoSecundario=-1;
}

Rede::Rede(const string IMPFILE, const string ARQUIVO_LOG, const tipoValidacaoJson_t VALIDACAO, const tipoSimulacao_t SIMULACAO,varGlob1D* Vvg1dSP) {
  entrada = IMPFILE;
  entradaLog=ARQUIVO_LOG;
  validacaoJson = VALIDACAO;
  tipoSimulacao = SIMULACAO;
  vg1dSP=Vvg1dSP;
  logger.changeResultadoSimulacao(entrada, entradaLog);
  lerArq();
}

Rede::Rede(const Rede& vrede) {
  entrada = vrede.entrada;
  entradaLog=vrede.entradaLog;
  vg1dSP=vrede.vg1dSP;
  //logger.changeResultadoSimulacao(entrada, entradaLog);
  lerArq();
}

Rede& Rede::operator =(const Rede& vrede) {
	if (this != &vrede) {
	 if(nsisprod>0){
		 if(anelGL==0){
		    for(int i=0; i<this->nsisprod;i++){
			  if(malha[i].ncoleta>0)delete [] malha[i].coleta;
			  if(malha[i].nafluente>0)delete [] malha[i].afluente;
		    }
		 }
		 if(malha!=0)delete [] malha;
		 if(conexFR!=0)delete [] conexFR;
		 delete [] impfiles;
	}
    entrada = vrede.entrada;
    entradaLog=vrede.entradaLog;
    validacaoJson = vrede.validacaoJson;
    tipoSimulacao = vrede.tipoSimulacao;
    vg1dSP=vrede.vg1dSP;
    //logger.changeResultadoSimulacao(entrada, entradaLog);
    lerArq();
  }
  return *this;
}

/*
 * Criar arquivo do schema Rede.
 */
void Rede::writeSchemaRede() {
  remove(ARQUIVO_SCHEMA_REDE_JSON);
  // criar arquivo de schema Rede
  ofstream schemaRedeStream(ARQUIVO_SCHEMA_REDE_JSON, ios_base::out);
  schemaRedeStream << "{" << endl;
  schemaRedeStream << "\"$schema\": \"http://json-schema.org/draft-04/schema#\"," << endl;
  schemaRedeStream << "\"id\": \"http://transiente.cenpes.petrobras.com/schemas/" << ARQUIVO_SCHEMA_REDE_JSON << "\"," << endl;
  schemaRedeStream << "\"definitions\": {" << endl;
  schemaRedeStream << "\"t_versao\": {\"type\": \"string\", \"pattern\": \"[0-9]{1,2}.[0-9]{1,2}\"}," << endl;
  schemaRedeStream << "\"t_configuracaoInicial\": {" << endl;
  schemaRedeStream << "\"type\": \"object\"," << endl;
  schemaRedeStream << "\"properties\": {" << endl;
  schemaRedeStream << "\"Rede\": {\"type\": \"boolean\"}," << endl;
  schemaRedeStream << "\"Injecao\": {\"type\": \"boolean\"}," << endl;
  schemaRedeStream << "\"ParametroInicial\": {\"type\": \"number\"}," << endl;
  schemaRedeStream << "\"Relaxacao\": {\"type\": \"number\"}," << endl;
  schemaRedeStream << "\"Transiente\": {\"type\": \"boolean\"}," << endl;
  schemaRedeStream << "\"TempoSimulacao\": {\"type\": \"number\"} }," << endl;
  schemaRedeStream << "\"required\": [\"ParametroInicial\", \"Relaxacao\"] }," << endl;
  schemaRedeStream << "\"p_arquivos\" : {\"type\": \"array\", \"uniqueItems\": false, \"items\": {\"type\": \"string\"} }," << endl;
  schemaRedeStream << "\"p_conexao\": {\"type\": \"object\", \"properties\": {" << endl;
  //schemaRedeStream << "\"sisprod\": {\"type\": \"integer\", \"minimum\": 0}," << endl;
  schemaRedeStream << "\"permanente\": {\"type\": \"integer\", \"pattern\": \"[0|1]\"}," << endl;
  schemaRedeStream << "\"coletores\": {\"type\": \"array\", \"uniqueItems\": true, \"items\": {\"type\": \"integer\"} }," << endl;
  schemaRedeStream << "\"afluentes\": {\"type\": \"array\", \"uniqueItems\": true, \"items\": {\"type\": \"integer\"} } }," << endl;
  schemaRedeStream << "\"required\": [\"coletores\", \"afluentes\"]" << endl;
  schemaRedeStream << "}," << endl;
  schemaRedeStream << "\"t_conexao\": {\"type\": \"array\", \"items\": {\"$ref\": \"#/definitions/p_conexao\"} }" << endl;
  schemaRedeStream << "}," << endl;
  schemaRedeStream << "\"type\": \"object\"," << endl;
  schemaRedeStream << "\"properties\": {" << endl;
  schemaRedeStream << "\"versao\": {\"$ref\": \"#/definitions/t_versao\"}," << endl;
  schemaRedeStream << "\"configuracaoInicial\": {\"$ref\": \"#/definitions/t_configuracaoInicial\"}," << endl;
  schemaRedeStream << "\"Arquivos\": {\"$ref\": \"#/definitions/p_arquivos\"}," << endl;
  schemaRedeStream << "\"Conexao\": {\"$ref\": \"#/definitions/t_conexao\"} }," << endl;
  schemaRedeStream << "\"required\": [\"versao\", \"configuracaoInicial\", \"Arquivos\", \"Conexao\"]" << endl;
  schemaRedeStream << "}" << endl;
  // fechar o stream
  schemaRedeStream.close();
}

Document Rede::parseEntrada() {
  // criar string para mensagem de falha
  char mensagemFalha[5000];
  // declarar o documento raiz do arquivo entrada
  Document jsonDoc;
  // definir arquivo Rede de entrada da simulação
  FILE *redeInFile = NULL;
  // definir buffer de entrada da simulação
  char redeInBuf[65536];
  // realizar a leitura do arquivo de rede
  try {
    // atualizar a estrutura de resultado do parse do arquivo de entrada da simulação
	  logger.setNomeArqEntrada(entrada);
    // abrir arquivo de entrada da simulação
    redeInFile = fopen(entrada.c_str(), "r");
    // criar stream para o arquivo de entrada da simulação
    FileReadStream redeInStream(redeInFile, redeInBuf, sizeof(redeInBuf));
    // realizar o parse do Rede de entrada
    jsonDoc.ParseStream(redeInStream);
    // fechar o arquivo
    fclose(redeInFile);
    // caso parse do Rede de entrada com erro
    if (jsonDoc.HasParseError()) {
//cout << "ErrorOffset: " << determinarLinhaErro(jsonDoc.GetErrorOffset()) << endl;
      // transpor os dados  para a mensagem
      sprintf(mensagemFalha, "Posicao = %u", jsonDoc.GetErrorOffset());
      // incluir falha no processo de parse
      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_JSON_FORMAT_VALIDATION, "Verificar formato JSON", mensagemFalha,
          GetParseError_Pt_BR(jsonDoc.GetParseError()), 0);
      // gerar o arquivo de saida da simulacao
      logger.writeOutputLog();
      // encerrar a aplicacao
      exit(EXIT_SUCCESS);
    }
  } catch (exception& e) {
    // incluir falha
    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Verificar formato JSON", "", e.what());
    // gerar o arquivo de saida da simulacao
    logger.writeOutputLog();
    // encerrar a aplicacao
    exit(EXIT_SUCCESS);
  }
  // incluir info de conclusao do parse do rede no log
  logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED, "Verificar formato JSON", "", "SUCESSO");
  // retorna o documento do Rede
  return jsonDoc;
}

Document Rede::parseSchema() {
  // criar string para mensagem de falha
  char mensagemFalha[5000];
  // declarar o documento para o schema de validacao do formato do Rede
  Document schemaDoc;
  // criar arquivo de schema de validacao do formato do Rede
  FILE *schemaRedeInFile = NULL;
  // definir buffer de entrada para leitura do arquivo json
  char schemaRedeInBuf[65536];
  // realizar a leitura do arquivo de schema do Rede
  try {
    // atualizar logger com arquivo de schema do rede
    logger.setNomeArqEntrada(ARQUIVO_SCHEMA_REDE_JSON);
    // criar arquivo de schema de validacao do formato do Rede
    schemaRedeInFile = fopen(ARQUIVO_SCHEMA_REDE_JSON, "r");
    // criar stream do schema de validacao do formato do Rede
    FileReadStream schemaRedeInStream(schemaRedeInFile, schemaRedeInBuf, sizeof(schemaRedeInBuf));
    // realizar o parse do schema de validacao do formato do Rede
    schemaDoc.ParseStream(schemaRedeInStream);
    // fechar o arquivo
    fclose(schemaRedeInFile);
    // remover o arquivo de schema
    remove(ARQUIVO_SCHEMA_REDE_JSON);
    // caso haja erros de parse
    if (schemaDoc.HasParseError()) {
//cout << "ErrorOffset: " << determinarLinhaErro(schemaDoc.GetErrorOffset()) << endl;
      // transpor os dados da falha para a mensagem
      sprintf(mensagemFalha, "Posicao = %u", schemaDoc.GetErrorOffset());
      // incluir falha no processo de parse
      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_JSON_FORMAT_VALIDATION, "Parse Schema-Rede", mensagemFalha,
          GetParseError_Pt_BR(schemaDoc.GetParseError()), 0);
      cout << " parse error: " << GetParseError_Pt_BR(schemaDoc.GetParseError()) << " - " << mensagemFalha << " - "
          << schemaDoc.GetErrorOffset() << endl;
      // gerar o arquivo de saida da simulacao
      logger.writeOutputLog();
      // encerrar a aplicacao
      exit(EXIT_SUCCESS);
    }
  } catch (exception& e) {
    // incluir falha no processo de parse
    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Parse Schema-Rede", "", e.what());
    // gerar o arquivo de saida da simulacao
    logger.writeOutputLog();
    // encerrar a aplicacao
    exit(EXIT_SUCCESS);
  }
  // retorna o documento do schema do Rede
  return schemaDoc;
}

void Rede::validateVsSchema(Document *schemaDoc, Document *jsonDoc) {
  // atualizar logger com arquivo Rede de entrda
  logger.setNomeArqEntrada(entrada);
  // realizar a validacao do arquivo Rede segundo o schema
  try {
    // compilar o documento do schema Rede para um SchemaDocument
    SchemaDocument schemaRede(*schemaDoc);
    // criar validador a partir do SchemaDocument definido para o Rede
    SchemaValidator schemaValidator(schemaRede);
    // caso o documento Rede lido nao seja validado
    if (!jsonDoc->Accept(schemaValidator)) {
      // definir buffer para validacao do Rede
      StringBuffer schemaValidationStrBuf;
      // obter a propriedade origem da falha
      schemaValidator.GetInvalidDocumentPointer().StringifyUriFragment(schemaValidationStrBuf);
//cout << "linha: " << determinarLinhaErro(schemaValidationStrBuf.GetString()) << endl;
      // incluir falha no processo de parse
      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_SCHEMA_VALIDATION, "Validar Rede x Schema-Rede",
          schemaValidationStrBuf.GetString(), GetParseErrorCause_Pt_BR(schemaValidator.GetInvalidSchemaKeyword()),
          0);
      cout << schemaValidationStrBuf.GetString() << " - " << schemaValidator.GetInvalidSchemaKeyword() << endl;
      // gerar o arquivo de saida da simulacao
      logger.writeOutputLog();
      // encerrar a aplicacao
      exit(EXIT_SUCCESS);
    }
  } catch (exception& e) {
    // incluir falha
    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Validar Rede x Schema-Rede", "", e.what());
    // gerar o arquivo de saida da simulacao
    logger.writeOutputLog();
    // encerrar a aplicacao
    exit(EXIT_SUCCESS);
  }
  // incluir info de conclusao do parse do rede no log
  logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED, "Validar Rede x Schema-Rede", "", "SUCESSO");
}

/*
 * Converter o elemento "configuracaoInicial" do arquivo Json do Rede em struct ::configuracao_inicial_t.
 *
 * Entrada: Value&
 * Retorna: ::configuracao_inicial_t
 */
void Rede::parse_configuracao_inicial(Value& configuracao_inicial_json) {
  /*
   "configuracaoInicial":{
   "Rede":false,
   "Injecao":true,
   "ParametroInicial":0.5
   }
   */

  // criar variavel para o nome da propriedade json em processo de parse
  string chaveJson("#/configuracaoInicial");

  tipoRede=0;
  redeParalela=0;
  injec=0;
  anelGL=0;
  relax=0.5;
  fluidoRede=1;
  tabelaDinamica=0;
  apenasPreProc=0;
  chute=0;
  nthrRede=1;
  limConverge=0.001;
  tipoModeloDrift=1;
  try {
	//isto deve ser melhoradop, a entrada correta seria o tipoRede, mas por enquanto será mantido assim para não atrapalhar os casos de rede que já são utilizados atualmente
    if (configuracao_inicial_json.HasMember("Injecao")){
      injec = configuracao_inicial_json["Injecao"].GetBool();
      if(injec==1)tipoRede=1;
    }
    if (configuracao_inicial_json.HasMember("AnelGL")){
      anelGL = configuracao_inicial_json["AnelGL"].GetBool();
      if(anelGL==1)tipoRede=2;
    }
    if (configuracao_inicial_json.HasMember("fonteRedeParalela")){
    	redeParalela = configuracao_inicial_json["fonteRedeParalela"].GetBool();
      if(redeParalela==1)tipoRede=3;
    }
    if(anelGL==1)injec=0;
	if (configuracao_inicial_json.HasMember("tipoModeloDrift"))
			tipoModeloDrift=configuracao_inicial_json["tipoModeloDrift"].GetBool();//alteracao7
	chaveredeT=0;//alteracao7
	if (tipoModeloDrift==1 && configuracao_inicial_json.HasMember("Transiente"))
			chaveredeT=configuracao_inicial_json["Transiente"].GetBool();//alteracao7
	if (chaveredeT==1)
			TmaxR=configuracao_inicial_json["TempoSimulacao"].GetDouble();//alteracao7
    chutHol = configuracao_inicial_json["ParametroInicial"].GetDouble();
	if (configuracao_inicial_json.HasMember("Relaxacao"))
		  relax = configuracao_inicial_json["Relaxacao"].GetDouble();
	if (configuracao_inicial_json.HasMember("fluidoRede"))
	      fluidoRede = configuracao_inicial_json["fluidoRede"].GetInt();
    if (configuracao_inicial_json.HasMember("modeloTabelaDinamica"))
      tabelaDinamica = configuracao_inicial_json["modeloTabelaDinamica"].GetBool();
    if (configuracao_inicial_json.HasMember("apenasPreProc"))
      apenasPreProc = configuracao_inicial_json["apenasPreProc"].GetBool();
    if (configuracao_inicial_json.HasMember("ChuteNos"))
      chute = configuracao_inicial_json["ChuteNos"].GetBool();
    if (configuracao_inicial_json.HasMember("threadRede"))
    	nthrRede = configuracao_inicial_json["threadRede"].GetInt();
    if (configuracao_inicial_json.HasMember("limiteConvergencia"))
    	limConverge = configuracao_inicial_json["limiteConvergencia"].GetDouble();
  } catch (exception& e) {
    // incluir falha
    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
  }
}

/*
 * Converter o elemento "Arquivos" do arquivo Json do Rede em struct ::Arquivos_t.
 *
 * Entrada: Value&
 * Retorna: ::Arquivos_t
 */
void Rede::parse_arquivos(Value& arquivos_json) {
  /*
   "Arquivos": ["sis-Injec-tronco1.json" , "sis-inj-choke-1.json" , "sis-inj-choke-1.json" , "sis-Injec-tronco2.json",
   "sis-Injec-ramo1a.json",  "sis-Injec-ramo1b.json" , "sis-inj-choke-2.json" , "sis-inj-choke-2.json",
   "sis-inj-choke-2.json",   "sis-Injec-tronco3.json","sis-Injec-ramo2a.json","sis-Injec-ramo2b.json",
   "sis-Injec-ramo2c.json",  "sis-Injec-ramo3a.json","sis-Injec-ramo3a.json"]
   */

  // criar variavel para o nome da propriedade json em processo de parse
  string chaveJson("#/Arquivos");

  impfiles=0;

  try {
    // quantidade de arquivos da rede
    nsisprod = arquivos_json.Capacity();
    // caso a quantidade de arquivos seja inferior a dois, i.e., nao indica simulacao em rede
    if (nsisprod < 1) {
      // RN-001: Chave 'arquivos' possui menos de dois arquivos, nao indicando simulacao em rede
      // incluir falha
    		logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'arquivos' possui menos de um elementos",
    				"#/Arquivos", "Quantidade de arquivos = " + to_string(nsisprod));
    } else {
      // vetor dos arquivos da rede
      impfiles = new string[nsisprod];
      // percorrer a lista de elementos da rede
      for (int indArquivo = 0; indArquivo < nsisprod; indArquivo++) {
        // arquivos
        impfiles[indArquivo] = arquivos_json[indArquivo].GetString();
      }
    }
  } catch (exception& e) {
    // incluir falha
		  logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
  }
}

/*
 * Converter o elemento "Conexao" do arquivo Json do Rede em struct ::Conexao_t.
 *
 * Entrada: Value&
 * Retorna: ::Conexao_t
 */
void Rede::parse_conexao(Value& conexao_json) {
  /*
   "Conexao":[
   {
   "sisprod": 0,
   "ncoletor": 3,
   "nafluente": 0,
   "coletores": [1,2,3],
   "afluentes":[]
   }
   ]
   */

  // criar variavel para o nome da propriedade json em processo de parse
  string chaveJson("#/Conexao");

  malha=0;
  conexFR=0;

  try {
    // vetor das estruturas de conexao da rede
	  malha = new conexao[nsisprod];
    // percorrer a lista de elementos da rede
	int maxConex=nsisprod;
	if(tipoRede==3)maxConex=2;
    for (int indConexao = 0; indConexao < maxConex; indConexao++) {
      malha[indConexao].ncoleta = 0;
      malha[indConexao].nafluente = 0;
      malha[indConexao].coleta = 0;
      malha[indConexao].afluente = 0;
      malha[indConexao].compfonte = 0.;
      malha[indConexao].tipoanel = 0;
      malha[indConexao].presimposta = 0;
      malha[indConexao].nbloqueio = 0;
      malha[indConexao].bloqueio = -1;
      malha[indConexao].principal=0;
      malha[indConexao].presMon=0.;
      malha[indConexao].presJus=0.;
      malha[indConexao].reverso=0.;
      malha[indConexao].derivaPrincipal=0.;
      malha[indConexao].tramoPrimario=0;

      // identificador da conexao
      //malha[indConexao].sistemaprod = conexao_json[indConexao]["sisprod"].GetInt();
	  malha[indConexao].perm=1;//alteracao7
	  malha[indConexao].ativo=1;
	  if (conexao_json[indConexao].HasMember("permanente"))//alteracao4
				malha[indConexao].perm=conexao_json[indConexao]["permanente"].GetInt();//alteracao7
	  if (conexao_json[indConexao].HasMember("ativo"))//alteracao4
				malha[indConexao].ativo=conexao_json[indConexao]["ativo"].GetBool();//alteracao7
      // quantidade de coletores
	  if(tipoRede<2){
         malha[indConexao].ncoleta = conexao_json[indConexao]["coletores"].Capacity();
         // caso a quantidade de coletores maior que zero
         if (malha[indConexao].ncoleta > 0) {
        // alocar vetor de coletores da conexao
        	 malha[indConexao].coleta = new int[malha[indConexao].ncoleta];
        // percorrer vetor de coletores da conexao
        	 for (int indColetor = 0; indColetor < malha[indConexao].ncoleta; indColetor++) {
          // numero do coletor
        		 int numeroColetor = conexao_json[indConexao]["coletores"][indColetor].GetInt();
          // caso o indice do coletor não exista no vetor de conexoes
        		 if (numeroColetor > nsisprod || numeroColetor < 0) {
            // RN-003: Elemento de 'coletores' inexistente
            // incluir falha
        			 logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Coletor '" + to_string(numeroColetor) + "' da conexao '" + to_string(indConexao) + "' inexistente",
        					 chaveJson, "Total de arquivos: " + nsisprod);
        		 } else {
            // atribuir coletor
        			 malha[indConexao].coleta[indColetor] = numeroColetor;
        		 }
        	 }
 			if (conexao_json[indConexao].HasMember("PressaoImposta"))//alteracao4
 				malha[indConexao].presimposta=conexao_json[indConexao]["PressaoImposta"].GetBool();//alteracao7
         }
			if (conexao_json[indConexao].HasMember("derivaPrincipal"))//alteracao4
				malha[indConexao].derivaPrincipal=conexao_json[indConexao]["derivaPrincipal"].GetInt();//alteracao7
      // quantidade de afluentes
      malha[indConexao].nafluente = conexao_json[indConexao]["afluentes"].Capacity();
      // caso a quantidade de afluentes maior que zero
      if (malha[indConexao].nafluente > 0) {
        // alocar vetor de afluentes da conexao
        malha[indConexao].afluente = new int[malha[indConexao].nafluente];
        // percorrer vetor de afluentes da conexao
        for (int indAfluente = 0; indAfluente < malha[indConexao].nafluente; indAfluente++) {
          // numero do afluente
          int numeroAfluente = conexao_json[indConexao]["afluentes"][indAfluente].GetInt();
          // caso o indice do afluente não exista no vetor de conexoes
          if (numeroAfluente > nsisprod || numeroAfluente < 0) {
            // RN-005: Elemento de 'afluentes' inexistente
            // incluir falha
                logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Afluente '" + to_string(numeroAfluente) + "' da conexao '" + to_string(indConexao) + "' inexistente",
                chaveJson, "Total de arquivos: " + nsisprod);
          } else {
            // atribuir afluente
            malha[indConexao].afluente[indAfluente] = numeroAfluente;
          }
        }
      }
      if(chute==1){
    	  malha[indConexao].presMon=conexao_json[indConexao]["PressaoMontante"].GetDouble();
    	  if(malha[indConexao].ncoleta>0)malha[indConexao].presJus=conexao_json[indConexao]["PressaoJusante"].GetDouble();
      }
      if (conexao_json[indConexao].HasMember("bloqueio")){
    	  //if (malha[indConexao].nbloqueio > 0) {
    	        //for (int indbloqueio = 0; indbloqueio < malha[indConexao].nbloqueio; indbloqueio++) {
    	          // numero do afluente
    	          int numerobloqueio = conexao_json[indConexao]["bloqueio"].GetInt();
    	          if(numerobloqueio>=0){
    	        	  malha[indConexao].nbloqueio = 1;
    	        	  if (malha[indConexao].ncoleta>2) {
    	        		  // RN-005: Elemento de 'afluentes' inexistente
    	        		  // incluir falha
    	        		  logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "bloqueio '" +
    	                	  to_string(numerobloqueio) + "' da conexao '" + to_string(indConexao) + "' para mais de 2 coletores",
    	                	  chaveJson, "Total de arquivos: " + nsisprod);
    	        	  }
    	        	  // caso o indice do afluente não exista no vetor de conexoes
    	        	  else if (numerobloqueio != malha[indConexao].coleta[0] && numerobloqueio != malha[indConexao].coleta[1]) {
    	        		  // RN-005: Elemento de 'afluentes' inexistente
    	        		  // incluir falha
    	        		  logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "bloqueio '" +
    	            		to_string(numerobloqueio) + "' da conexao '" + to_string(indConexao) + "' inexistente",
							chaveJson, "Total de arquivos: " + nsisprod);
    	        	  }
    	        	  else {
    	        		  // atribuir afluente
    	        		  malha[indConexao].bloqueio = numerobloqueio;
    	        	  }
    	          }
    	     //}
    	  //}
      }
      if (conexao_json[indConexao].HasMember("reverso"))malha[indConexao].reverso=conexao_json[indConexao]["reverso"].GetInt();
     }
	  else{
		  if(tipoRede==2){
			  malha[indConexao].tipoanel = conexao_json[indConexao]["Anel"].GetBool();
			  if(malha[indConexao].tipoanel==0)
				  malha[indConexao].compfonte = conexao_json[indConexao]["ComprimentoAnel"].GetFloat();
			  else malha[indConexao].compfonte=-1;
		  }
		  else if(tipoRede==3){
			  if (conexao_json[indConexao].HasMember("tramoPrimario"))
			  malha[indConexao].tramoPrimario=conexao_json[indConexao]["tramoPrimario"].GetBool();
			  if(malha[indConexao].tramoPrimario==1)tramoPrimario=indConexao;
			  else tramoSecundario=indConexao;
			  if(indConexao==1 && malha[0].tramoPrimario==malha[1].tramoPrimario){
        		  logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "definicao de tramo principal em rede paralela inconsistente",
					chaveJson, "Total de arquivos: " + nsisprod);
			  }
		  }
	  }
   }
  } catch (exception& e) {
    // incluir falha
		  logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
  }
}

void Rede::parse_fonteReciproca(Value& fonteRedeParalela_json) {
	string chaveJson("#/fonteRedeParalela");
	conexFR=0;
	try {
	   	nfonteR=fonteRedeParalela_json.Size();
	    conexFR = new conexRedeParalela[nfonteR];
		for (int indConexao = 0; indConexao < nfonteR; indConexao++){
			   conexFR[indConexao].noP= fonteRedeParalela_json[indConexao]["idNoPrimario"].GetInt();
			   conexFR[indConexao].noS= fonteRedeParalela_json[indConexao]["idNoSecundario"].GetInt();
		}
	} catch (exception& e) {
		// incluir falha
		  logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Rede::lerArq() {
  try {
    // Caso o arquivo de entrada inexista
    if (!ifstream(entrada)) {
      // incluir log de falha
      logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", "", "Arquivo de entrada inexistente");
      // gerar o arquivo de saida da simulacao
      logger.writeOutputLog();
      // encerrar a aplicacao
      exit(EXIT_SUCCESS);
    }

    int i = 0;
    // parse do documento raiz do arquivo de entrada
    Document jsonDoc = parseEntrada();
    // caso configurada opcao de termino apos validacao do formato do arquivo JSON
    if (validacaoJson == tipoValidacaoJson_t::json && (*vg1dSP).ntrdGlob==1) {
      // gerar o arquivo de saida da simulacao
      logger.writeOutputLog();
      // encerrar a aplicacao
      exit(EXIT_SUCCESS);
    }
    // caso a validacao esteja ligada
    if (validacaoJson != tipoValidacaoJson_t::off) {
      // schema para simulacao transiente
      writeSchemaRede();
      // parse do schema do Rede
      Document schemaDoc = parseSchema();
      // realizar a validacao do arquivo Rede segundo o schema
      validateVsSchema(&schemaDoc, &jsonDoc);
    }

    // caso configurada opcao de termino apos validacao do formato do arquivo JSON
    if (validacaoJson == tipoValidacaoJson_t::schema && (*vg1dSP).ntrdGlob==1) {
      // gravar arquivo de log
      logger.writeOutputLog();
      // abandonar a simulacao
      exit(EXIT_SUCCESS);
    }

    (*vg1dSP).chaverede = 1;

    // parse da chave configuracaoInicial
    parse_configuracao_inicial(jsonDoc["configuracaoInicial"]);
    (*vg1dSP).tipoModeloDrift = tipoModeloDrift;
    // parse da chave Arquivos
    parse_arquivos(jsonDoc["Arquivos"]);
    // caso a quantidade de arquivos indique simulacao em rede
    if (nsisprod > 0) {
      // parse da chave Conexao
      parse_conexao(jsonDoc["Conexao"]);
    }
    if(tipoRede==3){
    	parse_fonteReciproca(jsonDoc["fonteRedeParalela"]);
    }
  } catch (exception& e) {
		  // incluir falha
		  logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Validar Rede x Schema-Rede", "", e.what());
		  // gerar o arquivo de saida da simulacao
		  logger.writeOutputLog();
    // encerrar a aplicacao
    exit(EXIT_SUCCESS);
  }
}

//template class Rede;
