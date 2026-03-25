/*
 * LerRede.cpp
 *
 *  Created on: 23 de jun de 2017
 *      Author: Eduardo
 */
#include "LerAS.h"

using namespace rapidjson;

int chrisao=0;

ASens::ASens(varGlob1D* Vvg1dSP,const string IMPFILE,int vncel, detcelp* vcelp, ProFlu* vflup, detBCS* vbcs, detFONGAS* vfonteg) {
  entrada = IMPFILE;
  vg1dSP=Vvg1dSP;
  ncel=vncel;
  celp=vcelp;
  flup=vflup;
  bcs=vbcs;
  fonteg=vfonteg;
  lerArq();
}

ASens::ASens(const ASens& vAS) {
  entrada = vAS.entrada;
  vg1dSP=vAS.vg1dSP;
  ncel=vAS.ncel;
  celp=vAS.celp;
  flup=vAS.flup;
  bcs=vAS.bcs;
  fonteg=vAS.fonteg;
  lerArq();
}

ASens& ASens::operator =(const ASens& vAS) {
  if (this != &vAS) {
		if(listaV.vbcs==1){
		    /*for(int i=0; i<this->nASBCS;i++){
			  if(ASBCS[i].parserieEstag>0)delete [] ASBCS[i].nestag;
			  if(ASBCS[i].parserieFreq>0)delete [] ASBCS[i].freq;
		    }*/
			delete [] ASBCS;
		}

		if(listaV.vipr ==1){
		    /*for(int i=0; i<this->nASIPR;i++){
			  if(ASIPR[i].parserieFlup>0)delete [] ASIPR[i].indfluP;
			  if(ASIPR[i].parserieIP>0)delete [] ASIPR[i].ip;
			  if(ASIPR[i].parserieJP>0)delete [] ASIPR[i].jp;
			  if(ASIPR[i].parseriePres>0)delete [] ASIPR[i].pres;
			  if(ASIPR[i].parserieTemp>0)delete [] ASIPR[i].temp;
			  if(ASIPR[i].parserieqMax>0)delete [] ASIPR[i].qMax;
		    }*/
			delete [] ASIPR;
		}

		if(listaV.vfonliq ==1){
		    /*for(int i=0; i<this->nASFL;i++){
			  if(ASFonLiq[i].parserieFlu>0)delete [] ASFonLiq[i].indfluP;
			  if(ASFonLiq[i].parserieBet>0)delete [] ASFonLiq[i].bet;
			  if(ASFonLiq[i].parserieTemp>0)delete [] ASFonLiq[i].temp;
			  if(ASFonLiq[i].parserieVL>0)delete [] ASFonLiq[i].vazliq;
		    }*/
			delete [] ASFonLiq;
		}

		if(listaV.vfongas ==1){
		    /*for(int i=0; i<this->nASFG;i++){
			  if(ASFonGas[i].parserieFluP>0)delete [] ASFonGas[i].indfluP;
			  if(ASFonGas[i].parserieTemp>0)delete [] ASFonGas[i].temp;
			  if(ASFonGas[i].parserieVazG>0)delete [] ASFonGas[i].vazgas;
			  if(ASFonGas[i].parserieVazC>0)delete [] ASFonGas[i].vazcomp;
		    }*/
			delete [] ASFonGas;
		}

		if(listaV.vfonmas ==1){
		    /*for(int i=0; i<this->nASFG;i++){
			  if(ASFonMas[i].parserieFlu>0)delete [] ASFonMas[i].indfluP;
			  if(ASFonMas[i].parserieMC>0)delete [] ASFonMas[i].vazMasC;
			  if(ASFonMas[i].parserieMG>0)delete [] ASFonMas[i].vazMasG;
			  if(ASFonMas[i].parserieMT>0)delete [] ASFonMas[i].vazMasT;
			  if(ASFonMas[i].parserieTemp>0)delete [] ASFonMas[i].temp;
		    }*/
			delete [] ASFonMas;
		}

		if(listaV.vvalv ==1){
		    /*for(int i=0; i<this->nASV;i++){
			  if(ASValv[i].parserieAbre>0)delete [] ASValv[i].abertura;
			  if(ASValv[i].parserieCD>0)delete [] ASValv[i].cd;
		    }*/
			delete ASValv;
		}

		if(listaV.vbvol ==1){
		    /*for(int i=0; i<this->nASBV;i++){
			  if(ASBVOL[i].parserieFreq>0)delete [] ASBVOL[i].freq;
			  if(ASBVOL[i].parserieCap>0)delete [] ASBVOL[i].capacidade;
			  if(ASBVOL[i].parserieNPoli>0)delete [] ASBVOL[i].npoli;
		    }*/
			delete [] ASBVOL;
		}

		if(listaV.vdp ==1){
		    /*for(int i=0; i<this->nASDP;i++){
			  if(ASDP[i].parserieDP>0)delete [] ASDP[i].dp;
		    }*/
			delete [] ASDP;
		}

		if(listaV.vdpH ==1){
			delete [] ASdPdLH;
		}

		if(listaV.vdpF ==1){
			delete [] ASdPdLF;
		}

		if(listaV.vdt ==1){
			delete [] ASdTdL;
		}


		if(listaV.vfuro ==1){
		    /*for(int i=0; i<this->nASFuro;i++){
			  if(ASFuro[i].parserieAbre>0)delete [] ASFuro[i].abertura;
			  if(ASFuro[i].parserieBeta>0)delete [] ASFuro[i].beta;
			  if(ASFuro[i].parserieCD>0)delete [] ASFuro[i].cd;
			  if(ASFuro[i].parserieFlu>0)delete [] ASFuro[i].indFlu;
			  if(ASFuro[i].parseriePres>0)delete [] ASFuro[i].pres;
			  if(ASFuro[i].parserieTemp>0)delete [] ASFuro[i].temp;
		    }*/
			delete [] ASFuro;
		}

		if(listaV.diam ==1){
			delete[] ASGeom;
		}


		if(listaV.kequiv ==1){
			delete[] ASCondEquiv;
		}

		if(nVariaveis>1){
			delete [] sequenciaAS;
			delete [] genericoAS;
			delete [] vecParSerie;
		}

		if(nVariaveis>1 && dim>0){
			for(int i=0;i<nVariaveis;i++){
				delete [] saidaBHP[i];
				delete [] saidaVazLiq[i];
			}
			delete [] saidaBHP;
			delete [] saidaVazLiq;
		}

    entrada = vAS.entrada;
    vg1dSP=vAS.vg1dSP;
    ncel=vAS.ncel;
    celp=vAS.celp;
    flup=vAS.flup;
    bcs=vAS.bcs;
    fonteg=vAS.fonteg;
    lerArq();
  }
  return *this;
}

Document ASens::parseEntrada() {
  // criar string para mensagem de falha
  char mensagemFalha[5000];
  // declarar o documento raiz do arquivo entrada
  Document jsonDoc;
  // definir arquivo Rede de entrada da simulação
  FILE *ASInFile = NULL;
  // definir buffer de entrada da simulação
  char ASInBuf[65536];
  // realizar a leitura do arquivo de rede
  try {
    // atualizar a estrutura de resultado do parse do arquivo de entrada da simulação
    logger.setNomeArqEntrada(entrada);
    // abrir arquivo de entrada da simulação
    ASInFile = fopen(entrada.c_str(), "r");
    // criar stream para o arquivo de entrada da simulação
    FileReadStream ASInStream(ASInFile, ASInBuf, sizeof(ASInBuf));
    // realizar o parse de AS de entrada
    jsonDoc.ParseStream(ASInStream);
    // fechar o arquivo
    fclose(ASInFile);
    // caso parse de AS de entrada com erro
    if (jsonDoc.HasParseError()) {
//cout << "ErrorOffset: " << determinarLinhaErro(jsonDoc.GetErrorOffset()) << endl;
      // transpor os dados  para a mensagem
      sprintf(mensagemFalha, "Posicao = %u", jsonDoc.GetErrorOffset());
      // incluir falha no processo de parse
      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_JSON_FORMAT_VALIDATION, "Verificar formato JSON de Analise de Sensibibildade", mensagemFalha,
          GetParseError_Pt_BR(jsonDoc.GetParseError()), 0);
      // gerar o arquivo de saida da simulacao
      logger.writeOutputLog();
      // encerrar a aplicacao
      exit(EXIT_SUCCESS);
    }
  } catch (exception& e) {
    // incluir falha
    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Verificar formato JSON de Analise de Sensibibildade", "", e.what());
    // gerar o arquivo de saida da simulacao
    logger.writeOutputLog();
    // encerrar a aplicacao
    exit(EXIT_SUCCESS);
  }
  // incluir info de conclusao do parse de AS no log
  logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED, "Verificar formato JSON de Analise de Sensibibildade", "", "SUCESSO");
  // retorna o documento de AS
  return jsonDoc;
}

void ASens::parse_variaveis(Value& variaveis_json) {
	nVariaveis=1;
	tipoAS=0;
	vfp=0;
	listaV.vbcs=0;
	listaV.vbvol=0;
	listaV.vchk=0;
	listaV.vdp=0;
	listaV.vdpH=0;
	listaV.vdpF=0;
	listaV.vdt=0;
	listaV.vfongas=0;
	listaV.vfonliq=0;
	listaV.vfonmas=0;
	listaV.vfuro=0;
	listaV.vgasinj=0;
	listaV.vipr=0;
	listaV.vpocinj=0;
	listaV.vpresent=0;
	listaV.vpsep=0;
	listaV.vvalv=0;
	listaV.vvazpresent=0;
    listaV.diam=0;
    listaV.kequiv=0;
}

void ASens::parse_IPR(Value& IPR_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/IPR");

	  nASIPR=0;
	  int total=0;
	  ASIPR=0;
	  try {
	    nASIPR = IPR_json.Size();
	    if (nASIPR < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'IPR-AS' possui menos de um elemento",
	        "#/IPR", "Quantidade de IPR na AS = " + to_string(nASIPR));
	    } else {
	    	ASIPR = new detIPRAS[nASIPR];
	      for (int iIPR = 0; iIPR < nASIPR; iIPR++) {
	    	  ASIPR[iIPR].indIPR=IPR_json[iIPR]["indiceIPR"].GetInt();
	    	  if (IPR_json[iIPR].HasMember("presEstatica")){
	    		  dim++;
	    		  ASIPR[iIPR].parseriePres = IPR_json[iIPR]["presEstatica"].Size();
	    		  for(int ipesta=0;ipesta<ASIPR[iIPR].parseriePres;ipesta++){
	    			  ASIPR[iIPR].pres.push_back(IPR_json[iIPR]["presEstatica"][ipesta].GetDouble());
	    		  }
	    		  sort(ASIPR[iIPR].pres.begin(), ASIPR[iIPR].pres.end());
	    	  }
	    	  else ASIPR[iIPR].parseriePres = 0;
	    	  if(ASIPR[iIPR].parseriePres>0)nVariaveis*=ASIPR[iIPR].parseriePres;
	    	  if (IPR_json[iIPR].HasMember("tempRes")){
	    		  dim++;
	    		  ASIPR[iIPR].parserieTemp = IPR_json[iIPR]["tempRes"].Size();
	    		  for(int ipesta=0;ipesta<ASIPR[iIPR].parserieTemp;ipesta++){
	    			  ASIPR[iIPR].temp.push_back(IPR_json[iIPR]["tempRes"][ipesta].GetDouble());
	    		  }
	    	  }
	    	  else ASIPR[iIPR].parserieTemp = 0;
	    	  if(ASIPR[iIPR].parserieTemp>0)nVariaveis*=ASIPR[iIPR].parserieTemp;
	    	  if (IPR_json[iIPR].HasMember("IP")){
	    		  dim++;
	    		  ASIPR[iIPR].parserieIP = IPR_json[iIPR]["IP"].Size();
	    		  for(int ipesta=0;ipesta<ASIPR[iIPR].parserieIP;ipesta++){
	    			  ASIPR[iIPR].ip.push_back(IPR_json[iIPR]["IP"][ipesta].GetDouble());
	    		  }
	    		  sort(ASIPR[iIPR].ip.begin(), ASIPR[iIPR].ip.end());
	    	  }
	    	  else ASIPR[iIPR].parserieIP = 0;
	    	  if(ASIPR[iIPR].parserieIP>0)nVariaveis*=ASIPR[iIPR].parserieIP;
	    	  if (IPR_json[iIPR].HasMember("qMax")){
	    		  dim++;
	    		  ASIPR[iIPR].parserieqMax = IPR_json[iIPR]["qMax"].Size();
	    		  for(int ipesta=0;ipesta<ASIPR[iIPR].parserieqMax;ipesta++){
	    			  ASIPR[iIPR].qMax.push_back(IPR_json[iIPR]["qMax"][ipesta].GetDouble());
	    		  }
	    		  sort(ASIPR[iIPR].qMax.begin(), ASIPR[iIPR].qMax.end());
	    	  }
	    	  else ASIPR[iIPR].parserieqMax = 0;
	    	  if (IPR_json[iIPR].HasMember("II")){
	    		  dim++;
	    		  ASIPR[iIPR].parserieJP = IPR_json[iIPR]["II"].Size();
	    		  for(int ipesta=0;ipesta<ASIPR[iIPR].parserieJP;ipesta++){
	    			  ASIPR[iIPR].jp.push_back(IPR_json[iIPR]["II"][ipesta].GetDouble());
	    		  }
	    		  sort(ASIPR[iIPR].jp.begin(), ASIPR[iIPR].jp.end());
	    	  }
	    	  else ASIPR[iIPR].parserieJP = 0;
	    	  if(ASIPR[iIPR].parserieJP>0)nVariaveis*=ASIPR[iIPR].parserieJP;
	    	  if (IPR_json[iIPR].HasMember("fluido")){
	    		  dim++;
	    		  ASIPR[iIPR].parserieFlup = IPR_json[iIPR]["fluido"].Size();
	    		  for(int ipesta=0;ipesta<ASIPR[iIPR].parserieFlup;ipesta++){
	    			  ASIPR[iIPR].indfluP.push_back(IPR_json[iIPR]["fluido"][ipesta].GetInt());
	    		  }
	    	  }
	    	  else ASIPR[iIPR].parserieFlup = 0;
	    	  if(ASIPR[iIPR].parserieFlup>0)nVariaveis*=ASIPR[iIPR].parserieFlup;

	    	  total=ASIPR[iIPR].parserieIP+ASIPR[iIPR].parserieFlup+ASIPR[iIPR].parserieJP+
	    			  ASIPR[iIPR].parseriePres+ASIPR[iIPR].parserieTemp+ASIPR[iIPR].parserieqMax;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'IPR-AS' sem variavel fluido ou IP ou II ou presestatica ou t reservatorio ou qmax",
	    	        "#/IPR", "Indice de IPR na AS = " + to_string(iIPR));
	    	  }
	      }
	      listaV.vipr=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_FonLiq(Value& FonLiq_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/FonteLiquido");

	  nASFL=0;
	  ASFonLiq =0;
	  int total=0;
	  try {
	    nASFL = FonLiq_json.Size();
	    if (nASFL < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'FonteLiquido-AS' possui menos de um elemento",
	        "#/FonteLiquido", "Quantidade de Fonte Liquido na AS = " + to_string(nASFL));
	    } else {
	    	ASFonLiq = new detFONLIQAS[nASFL];
	      for (int iFL = 0; iFL < nASFL; iFL++) {
	    	  ASFonLiq[iFL].indFL=FonLiq_json[iFL]["indiceFL"].GetInt();
	    	  if (FonLiq_json[iFL].HasMember("temperatura")){
	    		  dim++;
	    		  ASFonLiq[iFL].parserieTemp = FonLiq_json[iFL]["temperatura"].Size();
	    		  for(int ifreq=0;ifreq<ASFonLiq[iFL].parserieTemp;ifreq++){
	    			  ASFonLiq[iFL].temp.push_back(FonLiq_json[iFL]["temperatura"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonLiq[iFL].temp.begin(), ASFonLiq[iFL].temp.end());
	    	  }
	    	  else ASFonLiq[iFL].parserieTemp = 0;
	    	  if(ASFonLiq[iFL].parserieTemp>0)nVariaveis*=ASFonLiq[iFL].parserieTemp;
	    	  if (FonLiq_json[iFL].HasMember("vazLiq")){
	    		  dim++;
	    		  ASFonLiq[iFL].parserieVL = FonLiq_json[iFL]["vazLiq"].Size();
	    		  for(int ifreq=0;ifreq<ASFonLiq[iFL].parserieVL;ifreq++){
	    			  ASFonLiq[iFL].vazliq.push_back(FonLiq_json[iFL]["vazLiq"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonLiq[iFL].vazliq.begin(), ASFonLiq[iFL].vazliq.end());
	    	  }
	    	  else ASFonLiq[iFL].parserieVL = 0;
	    	  if(ASFonLiq[iFL].parserieVL>0)nVariaveis*=ASFonLiq[iFL].parserieVL;
	    	  if (FonLiq_json[iFL].HasMember("beta")){
	    		  dim++;
	    		  ASFonLiq[iFL].parserieBet = FonLiq_json[iFL]["beta"].Size();
	    		  for(int ifreq=0;ifreq<ASFonLiq[iFL].parserieBet;ifreq++){
	    			  ASFonLiq[iFL].bet.push_back(FonLiq_json[iFL]["beta"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonLiq[iFL].bet.begin(), ASFonLiq[iFL].bet.end());
	    	  }
	    	  else ASFonLiq[iFL].parserieBet = 0;
	    	  if(ASFonLiq[iFL].parserieBet>0)nVariaveis*=ASFonLiq[iFL].parserieBet;
	    	  if (FonLiq_json[iFL].HasMember("fluido")){
	    		  dim++;
	    		  ASFonLiq[iFL].parserieFlu = FonLiq_json[iFL]["fluido"].Size();
	    		  for(int ifreq=0;ifreq<ASFonLiq[iFL].parserieFlu;ifreq++){
	    			  ASFonLiq[iFL].indfluP.push_back(FonLiq_json[iFL]["fluido"][ifreq].GetInt());
	    		  }
	    	  }
	    	  else ASFonLiq[iFL].parserieFlu = 0;
	    	  if(ASFonLiq[iFL].parserieFlu>0)nVariaveis*=ASFonLiq[iFL].parserieFlu;
	    	  total=ASFonLiq[iFL].parserieTemp+ASFonLiq[iFL].parserieBet+ASFonLiq[iFL].parserieVL+ASFonLiq[iFL].parserieFlu;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'FonteLiq-AS' sem variavel temperatura ou vazão de líquido, beta ou fluido",
	    	        "#/FonteLiq", "Indice de FonteLiq na AS = " + to_string(iFL));
	    	  }
	      }
	      listaV.vfonliq=1;
	    }
	    //nVariaveis+=total;
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_FonGas(Value& FonGas_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/FonteGas");

	  nASFG=0;
	  ASFonGas = 0;
	  int total=0;
	  try {
	    nASFG = FonGas_json.Size();
	    if (nASFG < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'FonteGas-AS' possui menos de um elemento",
	        "#/FonteGas", "Quantidade de Fonte Gas na AS = " + to_string(nASFG));
	    } else {
	    	ASFonGas = new detFONGASAS[nASFG];
	      for (int iFG = 0; iFG < nASFG; iFG++) {
	    	  ASFonGas[iFG].indFG=FonGas_json[iFG]["indiceFG"].GetInt();
	    	  if (FonGas_json[iFG].HasMember("temperatura")){
	    		  dim++;
	    		  ASFonGas[iFG].parserieTemp = FonGas_json[iFG]["temperatura"].Size();
	    		  for(int ifreq=0;ifreq<ASFonGas[iFG].parserieTemp;ifreq++){
	    			  ASFonGas[iFG].temp.push_back(FonGas_json[iFG]["temperatura"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonGas[iFG].temp.begin(), ASFonGas[iFG].temp.end());
	    	  }
	    	  else ASFonGas[iFG].parserieTemp = 0;
	    	  if(ASFonGas[iFG].parserieTemp>0)nVariaveis*=ASFonGas[iFG].parserieTemp;
	    	  if (FonGas_json[iFG].HasMember("vazGas")){
	    		  dim++;
	    		  ASFonGas[iFG].parserieVazG = FonGas_json[iFG]["vazGas"].Size();
	    		  for(int ifreq=0;ifreq<ASFonGas[iFG].parserieVazG;ifreq++){
	    			  ASFonGas[iFG].vazgas.push_back(FonGas_json[iFG]["vazGas"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonGas[iFG].vazgas.begin(), ASFonGas[iFG].vazgas.end());
	    	  }
	    	  else ASFonGas[iFG].parserieVazG = 0;
	    	  if(ASFonGas[iFG].parserieVazG>0)nVariaveis*=ASFonGas[iFG].parserieVazG;
	    	  if (FonGas_json[iFG].HasMember("vazComp")){
	    		  dim++;
	    		  ASFonGas[iFG].parserieVazC = FonGas_json[iFG]["vazComp"].Size();
	    		  for(int ifreq=0;ifreq<ASFonGas[iFG].parserieVazC;ifreq++){
	    			  ASFonGas[iFG].vazcomp.push_back(FonGas_json[iFG]["vazComp"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonGas[iFG].vazcomp.begin(), ASFonGas[iFG].vazcomp.end());
	    	  }
	    	  else ASFonGas[iFG].parserieVazC = 0;
	    	  if(ASFonGas[iFG].parserieVazC>0)nVariaveis*=ASFonGas[iFG].parserieVazC;
	    	  total=ASFonGas[iFG].parserieTemp+ASFonGas[iFG].parserieVazC+ASFonGas[iFG].parserieVazG;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'FonteGas-AS' sem variavel temperatura ou vazão de gás ou vazão complementar",
	    	        "#/FonteGas", "Indice de FonteGas na AS = " + to_string(iFG));
	    	  }
	      }
	      listaV.vfongas=1;
	    }
	    //nVariaveis+=total;
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_FonMas(Value& FonMas_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/FonteMassa");

	  nASFM=0;
	  ASFonMas = 0;
	  try {
	    nASFM = FonMas_json.Size();
	    if (nASFM < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'FonteMass-AS' possui menos de um elemento",
	        "#/FonteMass", "Quantidade de Fonte Massa na AS = " + to_string(nASFM));
	    } else {
	    	ASFonMas = new detFONMASSAS[nASFM];
	      for (int iFM = 0; iFM < nASFM; iFM++) {
	    	  ASFonMas[iFM].indFM=FonMas_json[iFM]["indiceFM"].GetInt();
	    	  if (FonMas_json[iFM].HasMember("temperatura")){
	    		  dim++;
	    		  ASFonMas[iFM].parserieTemp = FonMas_json[iFM]["temperatura"].Size();
	    		  for(int ifreq=0;ifreq<ASFonMas[iFM].parserieTemp;ifreq++){
	    			  ASFonMas[iFM].temp.push_back(FonMas_json[iFM]["temperatura"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonMas[iFM].temp.begin(), ASFonMas[iFM].temp.end());
	    	  }
	    	  else ASFonMas[iFM].parserieTemp = 0;
	    	  if(ASFonMas[iFM].parserieTemp>0)nVariaveis*=ASFonMas[iFM].parserieTemp;
	    	  if (FonMas_json[iFM].HasMember("vazaoProd")){
	    		  dim++;
	    		  ASFonMas[iFM].parserieMP = FonMas_json[iFM]["vazaoProd"].Size();
	    		  for(int ifreq=0;ifreq<ASFonMas[iFM].parserieMP;ifreq++){
	    			  ASFonMas[iFM].vazMasP.push_back(FonMas_json[iFM]["vazaoTotal"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonMas[iFM].vazMasP.begin(), ASFonMas[iFM].vazMasP.end());
	    	  }
	    	  else ASFonMas[iFM].parserieMP = 0;
	    	  if(ASFonMas[iFM].parserieMP>0)nVariaveis*=ASFonMas[iFM].parserieMP;
	    	  if (FonMas_json[iFM].HasMember("VazaoComp")){
	    		  dim++;
	    		  ASFonMas[iFM].parserieMC = FonMas_json[iFM]["VazaoComp"].Size();
	    		  for(int ifreq=0;ifreq<ASFonMas[iFM].parserieMC;ifreq++){
	    			  ASFonMas[iFM].vazMasC.push_back(FonMas_json[iFM]["VazaoComp"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonMas[iFM].vazMasC.begin(), ASFonMas[iFM].vazMasC.end());
	    	  }
	    	  else ASFonMas[iFM].parserieMC = 0;
	    	  if(ASFonMas[iFM].parserieMC>0)nVariaveis*=ASFonMas[iFM].parserieMC;
	    	  if (FonMas_json[iFM].HasMember("VazaoGas")){
	    		  dim++;
	    		  ASFonMas[iFM].parserieMG = FonMas_json[iFM]["VazaoGas"].Size();
	    		  for(int ifreq=0;ifreq<ASFonMas[iFM].parserieMG;ifreq++){
	    			  ASFonMas[iFM].vazMasG.push_back(FonMas_json[iFM]["VazaoGas"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFonMas[iFM].vazMasG.begin(), ASFonMas[iFM].vazMasG.end());
	    	  }
	    	  else ASFonMas[iFM].parserieMG = 0;
	    	  if(ASFonMas[iFM].parserieMG>0)nVariaveis*=ASFonMas[iFM].parserieMG;
	    	  if (FonMas_json[iFM].HasMember("fluido")){
	    		  dim++;
	    		  ASFonMas[iFM].parserieFlu = FonMas_json[iFM]["fluido"].Size();
	    		  for(int ifreq=0;ifreq<ASFonLiq[iFM].parserieFlu;ifreq++){
	    			  ASFonMas[iFM].indfluP.push_back(FonMas_json[iFM]["fluido"][ifreq].GetInt());
	    		  }
	    	  }
	    	  else ASFonMas[iFM].parserieFlu = 0;
	    	  if(ASFonMas[iFM].parserieFlu>0)nVariaveis*=ASFonMas[iFM].parserieFlu;
	    	  int total=ASFonMas[iFM].parserieTemp+ASFonMas[iFM].parserieMP+ASFonMas[iFM].parserieMC+ASFonMas[iFM].parserieMG+ASFonMas[iFM].parserieFlu;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'FonteMas-AS' sem variavel temperatura ou vazão tptal, ou complementar ou gas ou fluido",
	    	        "#/FonteMas", "Indice de FonteMas na AS = " + to_string(iFM));
	    	  }
	      }
	      listaV.vfonmas=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_BCS(Value& BCS_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/BCS");

	  nASBCS=0;
	  ASBCS = 0;
	  try {
	    nASBCS = BCS_json.Size();
	    if (nASBCS < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'BCS-AS' possui menos de um elemento",
	        "#/BCS", "Quantidade de BCS na AS = " + to_string(nASBCS));
	    } else {
	    	ASBCS = new detBCSAS[nASBCS];
	      for (int indBCS = 0; indBCS < nASBCS; indBCS++) {
	    	  ASBCS[indBCS].indBCS=BCS_json[indBCS]["indiceBCS"].GetInt();
	    	  if (BCS_json[indBCS].HasMember("frequencia")){
	    		  dim++;
	    		  ASBCS[indBCS].parserieFreq = BCS_json[indBCS]["frequencia"].Size();
	    		  for(int ifreq=0;ifreq<ASBCS[indBCS].parserieFreq;ifreq++){
	    			  ASBCS[indBCS].freq.push_back(BCS_json[indBCS]["frequencia"][ifreq].GetDouble());
	    		  }
	    		  sort(ASBCS[indBCS].freq.begin(), ASBCS[indBCS].freq.end());
	    	  }
	    	  else ASBCS[indBCS].parserieFreq = 0;
	    	  if(ASBCS[indBCS].parserieFreq>0)nVariaveis*=ASBCS[indBCS].parserieFreq;
	    	  if (BCS_json[indBCS].HasMember("estagio")){
	    		  dim++;
	    		  ASBCS[indBCS].parserieEstag = BCS_json[indBCS]["estagio"].Size();
	    		  for(int iestag=0;iestag<ASBCS[indBCS].parserieEstag;iestag++){
	    			  ASBCS[indBCS].nestag.push_back(BCS_json[indBCS]["estagio"][iestag].GetInt());
	    		  }
	    		  sort(ASBCS[indBCS].nestag.begin(), ASBCS[indBCS].nestag.end());
	    	  }
	    	  else ASBCS[indBCS].parserieEstag = 0;
	    	  if(ASBCS[indBCS].parserieEstag>0)nVariaveis*=ASBCS[indBCS].parserieEstag;
	    	  int total=ASBCS[indBCS].parserieFreq+ASBCS[indBCS].parserieEstag;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'BCS-AS' sem variavel estagio ou frequencia",
	    	        "#/BCS", "Indice de BCS na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vbcs=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_BVol(Value& BVol_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/BVol");

	  nASBV=0;
	  ASBVOL = 0;
	  try {
	    nASBV = BVol_json.Size();
	    if (nASBV < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'BVol-AS' possui menos de um elemento",
	        "#/BVol", "Quantidade de BVol na AS = " + to_string(nASBV));
	    } else {
	    	ASBVOL = new detBVOLAS[nASBV];
	      for (int indBCS = 0; indBCS < nASBV; indBCS++) {
	    	  ASBVOL[indBCS].indBV=BVol_json[indBCS]["indiceBVol"].GetInt();
	    	  if (BVol_json[indBCS].HasMember("frequencia")){
	    		  dim++;
	    		  ASBVOL[indBCS].parserieFreq = BVol_json[indBCS]["frequencia"].Size();
	    		  for(int ifreq=0;ifreq<ASBVOL[indBCS].parserieFreq;ifreq++){
	    			  ASBVOL[indBCS].freq.push_back(BVol_json[indBCS]["frequencia"][ifreq].GetDouble());
	    		  }
	    		  sort(ASBVOL[indBCS].freq.begin(), ASBVOL[indBCS].freq.end());
	    	  }
	    	  else ASBVOL[indBCS].parserieFreq = 0;
	    	  if(ASBVOL[indBCS].parserieFreq>0)nVariaveis*=ASBVOL[indBCS].parserieFreq;
	    	  if (BVol_json[indBCS].HasMember("capacidade")){
	    		  dim++;
	    		  ASBVOL[indBCS].parserieCap = BVol_json[indBCS]["capacidade"].Size();
	    		  for(int iestag=0;iestag<ASBVOL[indBCS].parserieCap;iestag++){
	    			  ASBVOL[indBCS].capacidade.push_back(BVol_json[indBCS]["capacidade"][iestag].GetDouble());
	    		  }
	    		  sort(ASBVOL[indBCS].capacidade.begin(), ASBVOL[indBCS].capacidade.end());
	    	  }
	    	  else ASBVOL[indBCS].parserieCap = 0;
	    	  if(ASBVOL[indBCS].parserieCap>0)nVariaveis*=ASBVOL[indBCS].parserieCap;
	    	  if (BVol_json[indBCS].HasMember("fatorPoli")){
	    		  dim++;
	    		  ASBVOL[indBCS].parserieNPoli = BVol_json[indBCS]["fatorPoli"].Size();
	    		  for(int iestag=0;iestag<ASBVOL[indBCS].parserieNPoli;iestag++){
	    			  ASBVOL[indBCS].npoli.push_back(BVol_json[indBCS]["fatorPoli"][iestag].GetDouble());
	    		  }
	    		  sort(ASBVOL[indBCS].npoli.begin(), ASBVOL[indBCS].npoli.end());
	    	  }
	    	  else ASBVOL[indBCS].parserieNPoli = 0;
	    	  if(ASBVOL[indBCS].parserieNPoli>0)nVariaveis*=ASBVOL[indBCS].parserieNPoli;
	    	  int total=ASBVOL[indBCS].parserieFreq+ASBVOL[indBCS].parserieCap+ASBVOL[indBCS].parserieNPoli;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'BVOL-AS' sem variavel capacidade ou frequencia ou fator politropico",
	    	        "#/BVOL", "Indice de BVOL na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vbvol=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_DP(Value& DP_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/DP");

	  nASDP=0;
	  ASDP = 0;
	  try {
	    nASDP = DP_json.Size();
	    if (nASDP < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'DP-AS' possui menos de um elemento",
	        "#/DP", "Quantidade de DP na AS = " + to_string(nASDP));
	    } else {
	    	ASDP = new detDPREQAS[nASDP];
	      for (int indBCS = 0; indBCS < nASDP; indBCS++) {
	    	  ASDP[indBCS].indDP=DP_json[indBCS]["indiceDP"].GetInt();
	    	  if (DP_json[indBCS].HasMember("deltapressao")){
	    		  dim++;
	    		  ASDP[indBCS].parserieDP = DP_json[indBCS]["deltapressao"].Size();
	    		  for(int ifreq=0;ifreq<ASDP[indBCS].parserieDP;ifreq++){
	    			  ASDP[indBCS].dp.push_back(DP_json[indBCS]["deltapressao"][ifreq].GetDouble());
	    		  }
	    		  sort(ASDP[indBCS].dp.begin(), ASDP[indBCS].dp.end());
	    	  }
	    	  else ASDP[indBCS].parserieDP = 0;
	    	  if(ASDP[indBCS].parserieDP>0)nVariaveis*=ASDP[indBCS].parserieDP;
	    	  int total=ASDP[indBCS].parserieDP;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'DP-AS' sem variavel delpres",
	    	        "#/DP", "Indice de DP na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vdp=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_dPdLH(Value& dPdLHidro_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/dPdLHidro");

	  nASdPdLH=0;
	  ASdPdLH = 0;
	  try {
	    nASdPdLH = dPdLHidro_json.Size();
	    if (nASdPdLH < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'dPdLHidro-AS' possui menos de um elemento",
	        "#/dPdLHidro", "Quantidade de dPdLHidro na AS = " + to_string(nASdPdLH));
	    } else {
	    	ASdPdLH = new detDPHidro[nASdPdLH];
	      for (int indBCS = 0; indBCS < nASdPdLH; indBCS++) {
	    	  ASdPdLH[indBCS].indDPHidro=dPdLHidro_json[indBCS]["indicedPdLHidro"].GetInt();
	    	  if (dPdLHidro_json[indBCS].HasMember("fator")){
	    		  dim++;
	    		  ASdPdLH[indBCS].parserieDPHidro = dPdLHidro_json[indBCS]["fator"].Size();
	    		  for(int ifreq=0;ifreq<ASdPdLH[indBCS].parserieDPHidro;ifreq++){
	    			  ASdPdLH[indBCS].dPHidro.push_back(dPdLHidro_json[indBCS]["fator"][ifreq].GetDouble());
	    		  }
	    		  sort(ASdPdLH[indBCS].dPHidro.begin(), ASdPdLH[indBCS].dPHidro.end());
	    	  }
	    	  else ASdPdLH[indBCS].parserieDPHidro = 0;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0)nVariaveis*=ASdPdLH[indBCS].parserieDPHidro;
	    	  int total=ASdPdLH[indBCS].parserieDPHidro;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'dPdLHidro-AS' sem variavel de correcao",
	    	        "#/dPdLHidro", "Indice de dPdLHidro na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vdpH=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_dPdLF(Value& dPdLFric_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/dPdLFric");

	  nASdPdLF=0;
	  ASdPdLF = 0;
	  try {
	    nASdPdLF = dPdLFric_json.Size();
	    if (nASdPdLF < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'dPdLFric-AS' possui menos de um elemento",
	        "#/dPdLFric", "Quantidade de dPdLFric na AS = " + to_string(nASdPdLF));
	    } else {
	    	ASdPdLF = new detDPFric[nASdPdLF];
	      for (int indBCS = 0; indBCS < nASdPdLF; indBCS++) {
	    	  ASdPdLF[indBCS].indDPFric=dPdLFric_json[indBCS]["indicedPdLFric"].GetInt();
	    	  if (dPdLFric_json[indBCS].HasMember("fator")){
	    		  dim++;
	    		  ASdPdLF[indBCS].parserieDPFric = dPdLFric_json[indBCS]["fator"].Size();
	    		  for(int ifreq=0;ifreq<ASdPdLF[indBCS].parserieDPFric;ifreq++){
	    			  ASdPdLF[indBCS].dPFric.push_back(dPdLFric_json[indBCS]["fator"][ifreq].GetDouble());
	    		  }
	    		  sort(ASdPdLF[indBCS].dPFric.begin(), ASdPdLF[indBCS].dPFric.end());
	    	  }
	    	  else ASdPdLF[indBCS].parserieDPFric = 0;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0)nVariaveis*=ASdPdLF[indBCS].parserieDPFric;
	    	  int total=ASdPdLF[indBCS].parserieDPFric;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'dPdLFric-AS' sem variavel de correcao",
	    	        "#/dPdLFric", "Indice de dPdLFric na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vdpF=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_dTdL(Value& dTdL_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/dTdL");

	  nASdTdL=0;
	  ASdTdL = 0;
	  try {
	    nASdTdL = dTdL_json.Size();
	    if (nASdTdL < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'dTdL-AS' possui menos de um elemento",
	        "#/dTdL", "Quantidade de dTdL na AS = " + to_string(nASdTdL));
	    } else {
	    	ASdTdL = new detDT[nASdTdL];
	      for (int indBCS = 0; indBCS < nASdTdL; indBCS++) {
	    	  ASdTdL[indBCS].indDT=dTdL_json[indBCS]["indicedTdL"].GetInt();
	    	  if (dTdL_json[indBCS].HasMember("fator")){
	    		  dim++;
	    		  ASdTdL[indBCS].parserieDT = dTdL_json[indBCS]["fator"].Size();
	    		  for(int ifreq=0;ifreq<ASdTdL[indBCS].parserieDT;ifreq++){
	    			  ASdTdL[indBCS].dT.push_back(dTdL_json[indBCS]["fator"][ifreq].GetDouble());
	    		  }
	    		  sort(ASdTdL[indBCS].dT.begin(), ASdTdL[indBCS].dT.end());
	    	  }
	    	  else ASdTdL[indBCS].parserieDT = 0;
	    	  if(ASdTdL[indBCS].parserieDT>0)nVariaveis*=ASdTdL[indBCS].parserieDT;
	    	  int total=ASdTdL[indBCS].parserieDT;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'dTdL-AS' sem variavel de correcao",
	    	        "#/dTdL", "Indice de dTdL na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vdt=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_Valv(Value& Valv_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/Valvula");

	  nASV=0;
	  ASValv = 0;
	  try {
	    nASV = Valv_json.Size();
	    if (nASV < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'Valvula-AS' possui menos de um elemento",
	        "#/Valvula", "Quantidade de Valvula na AS = " + to_string(nASV));
	    } else {
	    	ASValv = new detValvAS[nASV];
	      for (int indBCS = 0; indBCS < nASV; indBCS++) {
	    	  ASValv[indBCS].indV=Valv_json[indBCS]["indiceValvula"].GetInt();
	    	  if (Valv_json[indBCS].HasMember("abertura")){
	    		  dim++;
	    		  ASValv[indBCS].parserieAbre = Valv_json[indBCS]["abertura"].Size();
	    		  for(int ifreq=0;ifreq<ASValv[indBCS].parserieAbre;ifreq++){
	    			  ASValv[indBCS].abertura.push_back(Valv_json[indBCS]["abertura"][ifreq].GetDouble());
	    		  }
	    		  sort(ASValv[indBCS].abertura.begin(), ASValv[indBCS].abertura.end());
	    	  }
	    	  else ASValv[indBCS].parserieAbre = 0;
	    	  if(ASValv[indBCS].parserieAbre>0)nVariaveis*=ASValv[indBCS].parserieAbre;
	    	  if (Valv_json[indBCS].HasMember("CD")){
	    		  dim++;
	    		  ASValv[indBCS].parserieCD = Valv_json[indBCS]["CD"].Size();
	    		  for(int ifreq=0;ifreq<ASValv[indBCS].parserieCD;ifreq++){
	    			  ASValv[indBCS].cd.push_back(Valv_json[indBCS]["CD"][ifreq].GetDouble());
	    		  }
	    		  sort(ASValv[indBCS].cd.begin(), ASValv[indBCS].cd.end());
	    	  }
	    	  else ASValv[indBCS].parserieCD = 0;
	    	  if(ASValv[indBCS].parserieCD>0)nVariaveis*=ASValv[indBCS].parserieCD;
	    	  int total=ASValv[indBCS].parserieAbre+ASValv[indBCS].parserieCD;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'Valvula-AS' sem variavel abertura ou CD",
	    	        "#/Valvula", "Indice de Valvula na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vvalv=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_Furo(Value& Furo_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/Vazamento");

	  nASFuro=0;
	  ASFuro = 0;
	  try {
	    nASFuro = Furo_json.Size();
	    if (nASFuro < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'Vazamento-AS' possui menos de um elemento",
	        "#/Vazamento", "Quantidade de Vazamento na AS = " + to_string(nASFuro));
	    } else {
	    	ASFuro = new detFUROAS[nASFuro];
	      for (int indBCS = 0; indBCS < nASFuro; indBCS++) {
	    	  ASFuro[indBCS].indFuro=Furo_json[indBCS]["indiceVazamento"].GetInt();
	    	  if (Furo_json[indBCS].HasMember("abertura")){
	    		  dim++;
	    		  ASFuro[indBCS].parserieAbre = Furo_json[indBCS]["abertura"].Size();
	    		  for(int ifreq=0;ifreq<ASFuro[indBCS].parserieAbre;ifreq++){
	    			  ASFuro[indBCS].abertura.push_back(Furo_json[indBCS]["abertura"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFuro[indBCS].abertura.begin(), ASFuro[indBCS].abertura.end());
	    	  }
	    	  else ASFuro[indBCS].parserieAbre = 0;
	    	  if(ASFuro[indBCS].parserieAbre>0)nVariaveis*=ASFuro[indBCS].parserieAbre;
	    	  if (Furo_json[indBCS].HasMember("CD")){
	    		  dim++;
	    		  ASFuro[indBCS].parserieCD = Furo_json[indBCS]["CD"].Size();
	    		  for(int ifreq=0;ifreq<ASFuro[indBCS].parserieCD;ifreq++){
	    			  ASFuro[indBCS].cd.push_back(Furo_json[indBCS]["CD"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFuro[indBCS].cd.begin(), ASFuro[indBCS].cd.end());
	    	  }
	    	  else ASFuro[indBCS].parserieCD = 0;
	    	  if(ASFuro[indBCS].parserieCD>0)nVariaveis*=ASFuro[indBCS].parserieCD;
	    	  if (Furo_json[indBCS].HasMember("beta")){
	    		  ASFuro[indBCS].parserieBeta = Furo_json[indBCS]["beta"].Size();
	    		  for(int ifreq=0;ifreq<ASFuro[indBCS].parserieBeta;ifreq++){
	    			  ASFuro[indBCS].beta.push_back(Furo_json[indBCS]["beta"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFuro[indBCS].beta.begin(), ASFuro[indBCS].beta.end());
	    	  }
	    	  else ASFuro[indBCS].parserieBeta = 0;
	    	  if(ASFuro[indBCS].parserieBeta>0)nVariaveis*=ASFuro[indBCS].parserieBeta;
	    	  if (Furo_json[indBCS].HasMember("pressao")){
	    		  dim++;
	    		  ASFuro[indBCS].parseriePres = Furo_json[indBCS]["pressao"].Size();
	    		  for(int ifreq=0;ifreq<ASFuro[indBCS].parseriePres;ifreq++){
	    			  ASFuro[indBCS].pres.push_back(Furo_json[indBCS]["pressao"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFuro[indBCS].pres.begin(), ASFuro[indBCS].pres.end());
	    	  }
	    	  else ASFuro[indBCS].parseriePres = 0;
	    	  if(ASFuro[indBCS].parseriePres>0)nVariaveis*=ASFuro[indBCS].parseriePres;
	    	  if (Furo_json[indBCS].HasMember("temperatura")){
	    		  dim++;
	    		  ASFuro[indBCS].parserieTemp = Furo_json[indBCS]["tempertatura"].Size();
	    		  for(int ifreq=0;ifreq<ASFuro[indBCS].parserieTemp;ifreq++){
	    			  ASFuro[indBCS].temp.push_back(Furo_json[indBCS]["temperatura"][ifreq].GetDouble());
	    		  }
	    		  sort(ASFuro[indBCS].temp.begin(), ASFuro[indBCS].temp.end());
	    	  }
	    	  else ASFuro[indBCS].parserieTemp = 0;
	    	  if(ASFuro[indBCS].parserieTemp>0)nVariaveis*=ASFuro[indBCS].parserieTemp;
	    	  if (Furo_json[indBCS].HasMember("fluido")){
	    		  dim++;
	    		  ASFuro[indBCS].parserieFlu = Furo_json[indBCS]["fluido"].Size();
	    		  for(int ifreq=0;ifreq<ASFuro[indBCS].parserieFlu;ifreq++){
	    			  ASFuro[indBCS].indFlu.push_back(Furo_json[indBCS]["fluido"][ifreq].GetInt());
	    		  }
	    	  }
	    	  else ASFuro[indBCS].parserieFlu = 0;
	    	  if(ASFuro[indBCS].parserieFlu>0)nVariaveis*=ASFuro[indBCS].parserieFlu;
	    	  int total=ASFuro[indBCS].parserieAbre+ASFuro[indBCS].parserieCD+ASFuro[indBCS].parserieBeta+ASFuro[indBCS].parseriePres+
	    			  ASFuro[indBCS].parserieTemp+ASFuro[indBCS].parserieFlu;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'Vazamento-AS' sem variavel abertura ou CD ou pressao ou temperatrura ou beta ou fluido",
	    	        "#/Vazamento", "Indice de Vazamento na AS = " + to_string(indBCS));
	    	  }
	      }
	      listaV.vfuro=1;
	    }
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_Diam(Value& secaoTransversal_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/secaoTransversal");

	  nASGeom=0;
	  ASGeom = 0;
	  int total=0;
	  try {
	    nASGeom = secaoTransversal_json.Size();
	    if (nASGeom < 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'secaoTransversal-AS' possui menos de um elemento",
	        "#/secaoTransversal", "Quantidade de secaoTransversals na AS = " + to_string(nASGeom));
	    } else {
	    	ASGeom = new detDiamRug[nASGeom];
	      for (int iG = 0; iG < nASGeom; iG++) {
	    	  ASGeom[iG].indGeom=secaoTransversal_json[iG]["indiceSec"].GetInt();
	    	  if (secaoTransversal_json[iG].HasMember("DiamIntMaior")){
	    		  dim++;
	    		  ASGeom[iG].parserieA = secaoTransversal_json[iG]["DiamIntMaior"].Size();
	    		  for(int ia=0;ia<ASGeom[iG].parserieA;ia++){
	    			  ASGeom[iG].dA.push_back(secaoTransversal_json[iG]["DiamIntMaior"][ia].GetDouble());
	    		  }
	    		  sort(ASGeom[iG].dA.begin(), ASGeom[iG].dA.end());
	    	  }
	    	  else ASGeom[iG].parserieA = 0;
	    	  if(ASGeom[iG].parserieA>0)nVariaveis*=ASGeom[iG].parserieA;
	    	  if (secaoTransversal_json[iG].HasMember("DiamIntMenor")){
	    		  dim++;
	    		  ASGeom[iG].parserieB = secaoTransversal_json[iG]["DiamIntMenor"].Size();
	    		  for(int ib=0;ib<ASGeom[iG].parserieB;ib++){
	    			  ASGeom[iG].dB.push_back(secaoTransversal_json[iG]["DiamIntMenor"][ib].GetDouble());
	    		  }
	    		  sort(ASGeom[iG].dB.begin(), ASGeom[iG].dB.end());
	    	  }
	    	  else ASGeom[iG].parserieB = 0;
	    	  if(ASGeom[iG].parserieB>0)nVariaveis*=ASGeom[iG].parserieB;
	    	  if (secaoTransversal_json[iG].HasMember("Rugosidade")){
	    		  dim++;
	    		  ASGeom[iG].parserieRug = secaoTransversal_json[iG]["Rugosidade"].Size();
	    		  for(int ir=0;ir<ASGeom[iG].parserieRug;ir++){
	    			  ASGeom[iG].rug.push_back(secaoTransversal_json[iG]["Rugosidade"][ir].GetDouble());
	    		  }
	    		  sort(ASGeom[iG].rug.begin(), ASGeom[iG].rug.end());
	    	  }
	    	  else ASGeom[iG].parserieRug = 0;
	    	  if(ASGeom[iG].parserieRug>0)nVariaveis*=ASGeom[iG].parserieRug;
	    	  total=ASGeom[iG].parserieA+ASGeom[iG].parserieB+ASGeom[iG].parserieRug;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'secaoTransversal-AS' sem variavel Diametro menor ou Diametro Maior ou Rugosidade",
	    	        "#/secaoTransversal", "Indice de secaoTransversal na AS = " + to_string(iG));
	    	  }
	      }
	      listaV.diam=1;
	    }
	    //nVariaveis+=total;
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}

void ASens::parse_CondEquiv(Value& CondEquiv_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/Condutividade");

	  nASCondEquiv=0;
	  ASCondEquiv = 0;
	  int total=0;
	  try {
	    nASCondEquiv = CondEquiv_json.Size();
	    if (nASCondEquiv< 1) {
	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'Condutividade-AS' possui menos de um elemento",
	        "#/Condutividade", "Quantidade de Condutividades na AS = " + to_string(nASCondEquiv));
	    } else {
	    	ASCondEquiv = new detCondEquiv[nASCondEquiv];
	      for (int ik = 0; ik < nASCondEquiv; ik++) {
	    	  ASCondEquiv[ik].indMat=CondEquiv_json[ik]["indiceMat"].GetInt();
	    	  if (CondEquiv_json[ik].HasMember("condutividade")){
	    		  dim++;
	    		  ASCondEquiv[ik].parserieK = CondEquiv_json[ik]["condutividade"].Size();
	    		  for(int ia=0;ia<ASCondEquiv[ik].parserieK;ia++){
	    			  ASCondEquiv[ik].cond.push_back(CondEquiv_json[ik]["condutividade"][ia].GetDouble());
	    		  }
	    		  sort(ASCondEquiv[ik].cond.begin(), ASCondEquiv[ik].cond.end());
	    	  }
	    	  else ASCondEquiv[ik].parserieK = 0;
	    	  if(ASCondEquiv[ik].parserieK>0)nVariaveis*=ASCondEquiv[ik].parserieK;
	    	  total=ASCondEquiv[ik].parserieK;
	    	  //dim+=total;
	    	  if(total==0){
	    	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
	    	    		  "Chave 'Condutividade-AS' sem variavel condutividade",
	    	        "#/Condutividade", "Indice de condutividade na AS = " + to_string(ik));
	    	  }
	      }
	      listaV.kequiv=1;
	    }
	    //nVariaveis+=total;
	  } catch (exception& e) {
	    // incluir falha
	    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	  }
}


void ASens::parse_Psep(Value& Psep_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/psep");

   	  if (Psep_json.HasMember("pressao")){
   		dim++;
		  ASPsep.parseriePres = Psep_json["pressao"].Size();
   		  for(int ifreq=0;ifreq<ASPsep.parseriePres;ifreq++){
   			  ASPsep.pres.push_back(Psep_json["pressao"][ifreq].GetDouble());
   		  }
   		sort(ASPsep.pres.begin(), ASPsep.pres.end());
   	  }
   	  else ASPsep.parseriePres = 0;
      if(ASPsep.parseriePres>0)nVariaveis*=ASPsep.parseriePres;
   	  int total=ASPsep.parseriePres;
      //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'Psep-AS' sem variavel pressao",
    	        "#/psep","Pressao");
   	  }

     listaV.vpsep=1;

}

void ASens::parse_RGO(Value& RGO_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/RGO-fluido0");

   	  if (RGO_json.HasMember("RGO")){
   		dim++;
		  ASRGO.parserieRGO = RGO_json["RGO"].Size();
   		  for(int ifreq=0;ifreq<ASRGO.parserieRGO;ifreq++){
   			  ASRGO.RGO.push_back(RGO_json["RGO"][ifreq].GetDouble());
   		  }
   		sort(ASRGO.RGO.begin(), ASRGO.RGO.end());
   	  }
   	  else ASRGO.parserieRGO = 0;
      if(ASRGO.parserieRGO>0)nVariaveis*=ASRGO.parserieRGO;
   	  int total=ASRGO.parserieRGO;
      //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'RGO-AS' sem variavel RGO",
    	        "#/RGO-fluido0","RGO");
   	  }

     listaV.vRGO=1;

}

void ASens::parse_BSW(Value& BSW_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/BSW-fluido0");

   	  if (BSW_json.HasMember("BSW")){
   		dim++;
		  ASBSW.parserieBSW = BSW_json["BSW"].Size();
   		  for(int ifreq=0;ifreq<ASBSW.parserieBSW;ifreq++){
   			  ASBSW.BSW.push_back(BSW_json["BSW"][ifreq].GetDouble());
   		  }
   		sort(ASBSW.BSW.begin(), ASBSW.BSW.end());
   	  }
   	  else ASBSW.parserieBSW = 0;
      if(ASBSW.parserieBSW>0)nVariaveis*=ASBSW.parserieBSW;
   	  int total=ASBSW.parserieBSW;
      //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'BSW-AS' sem variavel BSW",
    	        "#/BSW-fluido0","BSW");
   	  }

     listaV.vBSW=1;

}

void ASens::parse_GasInj(Value& GasInj_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/GasLift");

   	  if (GasInj_json.HasMember("presInj")){
   		dim++;
		  ASGasInj.parseriePresInj = GasInj_json["presInj"].Size();
   		  for(int ifreq=0;ifreq<ASGasInj.parseriePresInj;ifreq++){
   			ASGasInj.presinj.push_back(GasInj_json["presInj"][ifreq].GetDouble());
   		  }
   		sort(ASGasInj.presinj.begin(), ASGasInj.presinj.end());
   	  }
   	  else ASGasInj.parseriePresInj = 0;
   	  if(ASGasInj.parseriePresInj>0)nVariaveis*=ASGasInj.parseriePresInj;
   	  if (GasInj_json.HasMember("temperatura")){
   		dim++;
		  ASGasInj.parserieTemp = GasInj_json["temperatura"].Size();
   		  for(int ifreq=0;ifreq<ASGasInj.parserieTemp;ifreq++){
   			ASGasInj.temperatura.push_back(GasInj_json["temperatura"][ifreq].GetDouble());
   		  }
   		sort(ASGasInj.temperatura.begin(), ASGasInj.temperatura.end());
   	  }
   	  else ASGasInj.parserieTemp = 0;
   	  if(ASGasInj.parserieTemp>0)nVariaveis*=ASGasInj.parserieTemp;
   	  if (GasInj_json.HasMember("vazGas")){
   		dim++;
		  ASGasInj.parserieVazGas = GasInj_json["vazGas"].Size();
   		  for(int ifreq=0;ifreq<ASGasInj.parserieVazGas;ifreq++){
   			ASGasInj.vazgas.push_back(GasInj_json["vazGas"][ifreq].GetDouble());
   		  }
   		sort(ASGasInj.vazgas.begin(), ASGasInj.vazgas.end());
   	  }
   	  else ASGasInj.parserieVazGas = 0;
   	  if(ASGasInj.parserieVazGas>0)nVariaveis*=ASGasInj.parserieVazGas;
   	  int total=ASGasInj.parseriePresInj+ASGasInj.parseriePresInj+ASGasInj.parserieVazGas;
   	  //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'GasInj-AS' sem variavel pressao ou temperatura ou vazao de gas",
    	        "#/GasLift","multiplas variaveis");
   	  }

     listaV.vgasinj=1;

}

void ASens::parse_CHK(Value& CHK_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/choke");

   	  if (CHK_json.HasMember("abertura")){
   		dim++;
		  ASCHK.parserieAbre = CHK_json["abertura"].Size();
   		  for(int ifreq=0;ifreq<ASCHK.parserieAbre;ifreq++){
   			  ASCHK.abertura.push_back(CHK_json["abertura"][ifreq].GetDouble());
   		  }
   		  sort(ASCHK.abertura.begin(), ASCHK.abertura.end());
   	  }
   	  else ASCHK.parserieAbre = 0;
   	  if(ASCHK.parserieAbre>0)nVariaveis*=ASCHK.parserieAbre;
   	  if (CHK_json.HasMember("CD")){
   		dim++;
		  ASCHK.parserieCD = CHK_json["CD"].Size();
   		  for(int ifreq=0;ifreq<ASCHK.parserieCD;ifreq++){
   			  ASCHK.cd.push_back(CHK_json["CD"][ifreq].GetDouble());
   		  }
   		sort(ASCHK.cd.begin(), ASCHK.cd.end());
   	  }
   	  else ASCHK.parserieCD = 0;
   	  if(ASCHK.parserieCD>0)nVariaveis*=ASCHK.parserieCD;
   	  int total=ASCHK.parserieAbre+ASCHK.parserieCD;
      //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'Choke-AS' sem variavel abertura ou CD",
    	        "#/CHK","Choke");
   	  }

     listaV.vchk=1;

}

void ASens::parse_Pent(Value& Pent_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/pentrada");

   	  if (Pent_json.HasMember("pressao")){
   		dim++;
   		  ASpEntrada.parseriePres = Pent_json["pressao"].Size();
   		  for(int ifreq=0;ifreq<ASpEntrada.parseriePres;ifreq++){
   			ASpEntrada.pres.push_back(Pent_json["pressao"][ifreq].GetDouble());
   		  }
   		  sort(ASpEntrada.pres.begin(), ASpEntrada.pres.end());
   	  }
   	  else ASpEntrada.parseriePres = 0;
   	  if(ASpEntrada.parseriePres>0)nVariaveis*=ASpEntrada.parseriePres;
   	  if (Pent_json.HasMember("temperatura")){
   		dim++;
   		  ASpEntrada.parserieTemp = Pent_json["temperatura"].Size();
   		  for(int ifreq=0;ifreq<ASpEntrada.parserieTemp;ifreq++){
   			ASpEntrada.temperatura.push_back(Pent_json["temperatura"][ifreq].GetDouble());
   		  }
   		  sort(ASpEntrada.temperatura.begin(), ASpEntrada.temperatura.end());
   	  }
   	  else ASpEntrada.parserieTemp = 0;
   	  if(ASpEntrada.parserieTemp>0)nVariaveis*=ASpEntrada.parserieTemp;
   	  if (Pent_json.HasMember("titulo")){
   		dim++;
   		  ASpEntrada.parserieTit= Pent_json["titulo"].Size();
   		  for(int ifreq=0;ifreq<ASpEntrada.parserieTit;ifreq++){
   			ASpEntrada.tit.push_back(Pent_json["titulo"][ifreq].GetDouble());
   		  }
   		  sort(ASpEntrada.tit.begin(), ASpEntrada.tit.end());
   	  }
   	  else ASpEntrada.parserieTit = 0;
   	  if(ASpEntrada.parserieTit>0)nVariaveis*=ASpEntrada.parserieTit;
   	  if (Pent_json.HasMember("beta")){
   		dim++;
   		  ASpEntrada.parserieBet= Pent_json["beta"].Size();
   		  for(int ifreq=0;ifreq<ASpEntrada.parserieBet;ifreq++){
   			ASpEntrada.bet.push_back(Pent_json["beta"][ifreq].GetDouble());
   		  }
   		  sort(ASpEntrada.bet.begin(), ASpEntrada.bet.end());
   	  }
   	  else ASpEntrada.parserieBet = 0;
   	  if(ASpEntrada.parserieBet>0)nVariaveis*=ASpEntrada.parserieBet;
   	  int total=ASpEntrada.parseriePres+ASpEntrada.parserieTemp+ASpEntrada.parserieTit+ASpEntrada.parserieBet;
   	  //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'Presentrada-AS' sem variavel pressao ou temperatura ou titulo ou beta",
    	        "#/pentrada","multiplas variaveis");
   	  }

     listaV.vpresent=1;

}

void ASens::parse_VPent(Value& VPent_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/vazpentrada");

   	  if (VPent_json.HasMember("pressao")){
   		dim++;
   		  ASvpEntrada.parseriePres = VPent_json["pressao"].Size();
   		  for(int ifreq=0;ifreq<ASvpEntrada.parseriePres;ifreq++){
   			ASvpEntrada.pres.push_back(VPent_json["pressao"][ifreq].GetDouble());
   		  }
   		  sort(ASvpEntrada.pres.begin(), ASvpEntrada.pres.end());
   	  }
   	  else ASvpEntrada.parseriePres = 0;
   	  if(ASvpEntrada.parseriePres>0)nVariaveis*=ASvpEntrada.parseriePres;
   	  if (VPent_json.HasMember("temperatura")){
   		dim++;
   		  ASvpEntrada.parserieTemp = VPent_json["temperatura"].Size();
   		  for(int ifreq=0;ifreq<ASvpEntrada.parserieTemp;ifreq++){
   			ASvpEntrada.temperatura.push_back(VPent_json["temperatura"][ifreq].GetDouble());
   		  }
   		  sort(ASvpEntrada.temperatura.begin(), ASvpEntrada.temperatura.end());
   	  }
   	  else ASvpEntrada.parserieTemp = 0;
   	  if(ASvpEntrada.parserieTemp>0)nVariaveis*=ASvpEntrada.parserieTemp;
   	  if (VPent_json.HasMember("vazMass")){
   		dim++;
   		  ASvpEntrada.parserieMass= VPent_json["vazMass"].Size();
   		  for(int ifreq=0;ifreq<ASvpEntrada.parserieMass;ifreq++){
   			ASvpEntrada.mass.push_back(VPent_json["vazMass"][ifreq].GetDouble());
   		  }
   		  sort(ASvpEntrada.mass.begin(), ASvpEntrada.mass.end());
   	  }
   	  else ASvpEntrada.parserieMass = 0;
   	  if(ASvpEntrada.parserieMass>0)nVariaveis*=ASvpEntrada.parserieMass;
   	  if (VPent_json.HasMember("beta")){
   		dim++;
   		  ASvpEntrada.parserieBet= VPent_json["beta"].Size();
   		  for(int ifreq=0;ifreq<ASvpEntrada.parserieBet;ifreq++){
   			ASvpEntrada.bet.push_back(VPent_json["beta"][ifreq].GetDouble());
   		  }
   		  sort(ASvpEntrada.bet.begin(), ASvpEntrada.bet.end());
   	  }
   	  else ASvpEntrada.parserieBet = 0;
   	  if(ASvpEntrada.parserieBet>0)nVariaveis*=ASvpEntrada.parserieBet;
   	  int total=ASvpEntrada.parseriePres+ASvpEntrada.parserieTemp+ASvpEntrada.parserieMass+ASvpEntrada.parserieBet;
      //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'VazPresentrada-AS' sem variavel pressao ou temperatura ou vazao massica ou beta",
    	        "#/vpentrada","multiplas variaveis");
   	  }

     listaV.vvazpresent=1;

}

void ASens::parse_injecPoco(Value& injecP_json) {
	  // criar variavel para o nome da propriedade json em processo de parse
	  string chaveJson("#/pocoInjetor");

   	  if (injecP_json.HasMember("pressaoInj")){
   		dim++;
   		  ASPInj.parseriePresI = injecP_json["pressaoInj"].Size();
   		  for(int ifreq=0;ifreq<ASPInj.parseriePresI;ifreq++){
   			ASPInj.presinj.push_back(injecP_json["pressaoInj"][ifreq].GetDouble());
   		  }
   		  sort(ASPInj.presinj.begin(), ASPInj.presinj.end());
   	  }
   	  else ASPInj.parseriePresI = 0;
   	  if(ASPInj.parseriePresI>0)nVariaveis*=ASPInj.parseriePresI;
  	  if (injecP_json.HasMember("pressaoFinal")){
  		dim++;
   		  ASPInj.parseriePresFundo = injecP_json["pressaoFinal"].Size();
   		  for(int ifreq=0;ifreq<ASPInj.parseriePresFundo;ifreq++){
   			ASPInj.presfundo.push_back(injecP_json["pressaoFinal"][ifreq].GetDouble());
   		  }
   		  sort(ASPInj.presfundo.begin(), ASPInj.presfundo.end());
   	  }
   	  else ASPInj.parseriePresFundo = 0;
  	  if(ASPInj.parseriePresFundo>0)nVariaveis*=ASPInj.parseriePresFundo;
  	  if (injecP_json.HasMember("temperatura")){
  		dim++;
   		  ASPInj.parserieTemp = injecP_json["temperatura"].Size();
   		  for(int ifreq=0;ifreq<ASPInj.parserieTemp;ifreq++){
   			ASPInj.tempinj.push_back(injecP_json["temperatura"][ifreq].GetDouble());
   		  }
   		  sort(ASPInj.tempinj.begin(), ASPInj.tempinj.end());
   	  }
   	  else ASPInj.parserieTemp = 0;
  	  if(ASPInj.parserieTemp>0)nVariaveis*=ASPInj.parserieTemp;
 	  if (injecP_json.HasMember("temperatura")){
 		 dim++;
   		  ASPInj.parserieVaz = injecP_json["vazao"].Size();
   		  for(int ifreq=0;ifreq<ASPInj.parserieVaz;ifreq++){
   			ASPInj.vazinj.push_back(injecP_json["vazao"][ifreq].GetDouble());
   		  }
   		  sort(ASPInj.vazinj.begin(), ASPInj.vazinj.end());
   	  }
   	  else ASPInj.parserieVaz= 0;
 	  if(ASPInj.parserieVaz>0)nVariaveis*=ASPInj.parserieVaz;
   	  int total=ASPInj.parseriePresI+ASPInj.parseriePresFundo+ASPInj.parserieTemp+ASPInj.parserieVaz;
   	  //dim+=total;
   	  if(total==0){
  	      logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
   	    		  "Chave 'PocoInjetor-AS' sem variavel pressao de injecao ou temperatura ou vazao ou pressao final",
    	        "#/pocoInjetorEntrada","multiplas variaveis");
   	  }

     listaV.vvazpresent=1;

}


void ASens::lerArq() {
	   // Caso o arquivo de entrada inexista
	    if (!ifstream(entrada)) {
	      // incluir log de falha
	      logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", "", "Arquivo de analise de sensiblidade inexistente");
	      // gerar o arquivo de saida da simulacao
	      logger.writeOutputLog();
	      // encerrar a aplicacao
	      exit(EXIT_SUCCESS);
	    }

	    // parse do documento raiz do arquivo de entrada
	    Document jsonDoc = parseEntrada();

	    //parse_variaveis(jsonDoc["variaveis"]);
	    dim=0;
	    nthrdAS=1;

		nVariaveis=1;
		tipoAS=0;
		vfp=0;
		listaV.vbcs=0;
		listaV.vbvol=0;
		listaV.vchk=0;
		listaV.vdp=0;
		listaV.vdpH=0;
		listaV.vdpF=0;
		listaV.vdt=0;
		listaV.vfongas=0;
		listaV.vfonliq=0;
		listaV.vfonmas=0;
		listaV.vfuro=0;
		listaV.diam=0;
		listaV.vgasinj=0;
		listaV.vipr=0;
		listaV.vpocinj=0;
		listaV.vpresent=0;
		listaV.vpsep=0;
		listaV.vRGO=0;
		listaV.vBSW=0;
		listaV.vvalv=0;
		listaV.vvazpresent=0;
		listaV.diam=0;
		listaV.kequiv=0;

	    /*
	     string chaveJson("#/IPR");
	     string chaveJson("#/FonteLiquido");
	     string chaveJson("#/FonteGas");
	     string chaveJson("#/FonteMassa");
	     string chaveJson("#/FonteMassa");
	     string chaveJson("#/BCS");
	     string chaveJson("#/BVol");
	     string chaveJson("#/DP");
	     string chaveJson("#/Valvula");
	     string chaveJson("#/Vazamento");
	     string chaveJson("#/psep");
	     string chaveJson("#/GasLift");
	     string chaveJson("#/choke");
	     string chaveJson("#/pentrada");
	     string chaveJson("#/vazpentrada");
	     string chaveJson("#/pocoInjetor");
	     */


		if (jsonDoc.HasMember("nthread"))
			nthrdAS=jsonDoc["nthread"].GetInt();
		if (jsonDoc.HasMember("tipoAS"))
			tipoAS=jsonDoc["tipoAS"].GetInt();

		//chris alteracao VFPs novo
		/*if (jsonDoc.HasMember("vfp"))
		vfp=jsonDoc["vfp"].GetInt();*/
		if (jsonDoc.HasMember("vfp") && jsonDoc["vfp"].IsInt()) {
			this->vfp = jsonDoc["vfp"].GetInt(); // aceita 0
		} else {
			this->vfp = 1; // default
		}

		//std::cout << "[DEBUG] vfp lido = " << this->vfp << "\n";

	    ASIPR=0;
	    if (jsonDoc.HasMember("IPR")){
	    		parse_IPR(jsonDoc["IPR"]);
	    		//nVariaveis+=listaV.vipr;
	    }
	    ASFonLiq =0;
	    if (jsonDoc.HasMember("FonteLiquido")){
	    		parse_FonLiq(jsonDoc["FonteLiquido"]);
	    		//nVariaveis+=listaV.vfonliq;
	    }
	    ASFonGas = 0;
	    if (jsonDoc.HasMember("FonteGas")){
	    		parse_FonGas(jsonDoc["FonteGas"]);
	    		//nVariaveis+=listaV.vfongas;
	    }
	    ASFonMas = 0;
	    if (jsonDoc.HasMember("FonteMassa")){
	    		parse_FonMas(jsonDoc["FonteMassa"]);
	    		//nVariaveis+=listaV.vfonmas;
	    }
	    ASBCS = 0;
	    if (jsonDoc.HasMember("BCS")){
	    		parse_BCS(jsonDoc["BCS"]);
	    		//nVariaveis+=listaV.vbcs;
	    }
	    ASBVOL = 0;
	    if (jsonDoc.HasMember("BVol")){
	    		parse_BVol(jsonDoc["BVol"]);
	    		//nVariaveis+=listaV.vbvol;
	    }
	    ASDP = 0;
	    if (jsonDoc.HasMember("DP")){
	    		parse_DP(jsonDoc["DP"]);
	    		//nVariaveis+=listaV.vdp;
	    }
	    ASdPdLH = 0;
	    if (jsonDoc.HasMember("dPdLHidro")){
	    		parse_dPdLH(jsonDoc["dPdLHidro"]);
	    		//nVariaveis+=listaV.vdp;
	    }
	    ASdPdLF = 0;
	    if (jsonDoc.HasMember("dPdLFric")){
	    		parse_dPdLF(jsonDoc["dPdLFric"]);
	    		//nVariaveis+=listaV.vdp;
	    }
	    ASdTdL = 0;
	    if (jsonDoc.HasMember("dTdL")){
	    		parse_dTdL(jsonDoc["dTdL"]);
	    		//nVariaveis+=listaV.vdp;
	    }
	    ASValv = 0;
	    if (jsonDoc.HasMember("Valvula")){
	    		parse_Valv(jsonDoc["Valvula"]);
	    		//nVariaveis+=listaV.vvalv;
	    }
	    ASFuro = 0;
	    if (jsonDoc.HasMember("Vazamento")){
	    		parse_Furo(jsonDoc["Vazamento"]);
	    		//nVariaveis+=listaV.vfuro;
	    }
	    ASGeom = 0;
	    if (jsonDoc.HasMember("secaoTransversal")){
	    		parse_Diam(jsonDoc["secaoTransversal"]);
	    		//nVariaveis+=listaV.vfuro;
	    }
	    ASCondEquiv = 0;
	    if (jsonDoc.HasMember("Condutividade")){
	    		parse_CondEquiv(jsonDoc["Condutividade"]);
	    		//nVariaveis+=listaV.vfuro;
	    }
	    if (jsonDoc.HasMember("psep")){
	    		parse_Psep(jsonDoc["psep"]);
	    		//nVariaveis+=listaV.vpsep;
	    }
	    if (jsonDoc.HasMember("RGO-fluido0")){
	    		parse_RGO(jsonDoc["RGO-fluido0"]);
	    		//nVariaveis+=listaV.vpsep;
	    }
	    if (jsonDoc.HasMember("BSW-fluido0")){
	    		parse_BSW(jsonDoc["BSW-fluido0"]);
	    		//nVariaveis+=listaV.vpsep;
	    }
	    if (jsonDoc.HasMember("GasLift")){
	    		parse_GasInj(jsonDoc["GasLift"]);
	    		//nVariaveis+=listaV.vgasinj;
	    }
	    if (jsonDoc.HasMember("choke")){
	    		parse_CHK(jsonDoc["choke"]);
	    		//nVariaveis+=listaV.vchk;
	    }
	    if (jsonDoc.HasMember("pentrada")){
	    		parse_Pent(jsonDoc["pentrada"]);
	    		//nVariaveis+=listaV.vpresent;
	    }
	    if (jsonDoc.HasMember("vazpentrada")){
	       		parse_VPent(jsonDoc["vazpentrada"]);
	       		//nVariaveis+=listaV.vvazpresent;
	    }
	    if (jsonDoc.HasMember("pocoInjetor")){
	    	    parse_injecPoco(jsonDoc["pocoInjetor"]);
	    	    //nVariaveis+=listaV.vpocinj;
	    }


		varSeq[0]=listaV.vpsep;
		varSeq[1]=listaV.vRGO;
		varSeq[2]=listaV.vBSW;
		varSeq[3]=listaV.vfonliq;
		varSeq[4]=listaV.vfongas;
		varSeq[5]=listaV.vipr;
		varSeq[6]=listaV.vfonmas;
		varSeq[7]=listaV.vbcs;
		varSeq[8]=listaV.vdp;
		varSeq[9]=listaV.vdpH;
		varSeq[10]=listaV.vdpF;
		varSeq[11]=listaV.vdt;
		varSeq[12]=listaV.vbvol;
		varSeq[13]=listaV.vvalv;
		varSeq[14]=listaV.vfuro;
		varSeq[15]=listaV.diam;
		varSeq[16]=listaV.kequiv;
		varSeq[17]=listaV.vgasinj;
		varSeq[18]=listaV.vpresent;
		varSeq[19]=listaV.vvazpresent;
		varSeq[20]=listaV.vchk;
		varSeq[21]=listaV.vpocinj;

	    sequenciaAS=0;
	    if(nVariaveis>1){
	    	sequenciaAS= new casoVEC[nVariaveis];
	    	genericoAS= new genericoVEC[nVariaveis];
	    	vecParSerie= new int[dim];
	    }
		ostringstream saidaP;
		saidaP << "relacaoAS.dat";

		string tmp = saidaP.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni <<"sequencias da Analise de Sensibilidade para um Tramo "<< "\n";
		escreveIni.close();

	    if(vfp==1){
	    	constroiVecParSerie();
	    	inicializaSequen(0,0);
	    	traduzSeq();
	    }
	    else if(vfp==0){
	    	constroiVecParSerieImex();
	    	inicializaSequen(0,0);
	    	traduzSeqImex();
	    }

	    if(tipoAS==1){
	    	saidaBHP=new double*[nVariaveis];
	    	saidaVazLiq=new double*[nVariaveis];
	    	for(int i=0;i<nVariaveis;i++){
	    		saidaBHP[i]=new double[dim+1];
	    		saidaVazLiq[i]=new double[ASFonLiq[0].parserieVL];
	    	}

	    	tabelaGenericaCabecalho();
	    }
	    else{
	    	saidaBHP=0;
	    	saidaVazLiq=0;
	    }

	    int teste;
	    /*teste=sequenciaAS[0].PSEPpres;
	    teste=sequenciaAS[1].PSEPpres;
	    teste=sequenciaAS[2].PSEPpres;
	    teste=sequenciaAS[3].PSEPpres;
	    teste=sequenciaAS[4].PSEPpres;
	    teste=sequenciaAS[5].PSEPpres;
	    teste=sequenciaAS[6].PSEPpres;
	    teste=sequenciaAS[7].PSEPpres;
	    teste=sequenciaAS[0].IPRip[0];
	    teste=sequenciaAS[1].IPRip[0];
	    teste=sequenciaAS[2].IPRip[0];
	    teste=sequenciaAS[3].IPRip[0];
	    teste=sequenciaAS[4].IPRip[0];
	    teste=sequenciaAS[5].IPRip[0];
	    teste=sequenciaAS[6].IPRip[0];
	    teste=sequenciaAS[7].IPRip[0];
	    teste=sequenciaAS[0].INJGvazgas;
	    teste=sequenciaAS[1].INJGvazgas;
	    teste=sequenciaAS[2].INJGvazgas;
	    teste=sequenciaAS[3].INJGvazgas;
	    teste=sequenciaAS[4].INJGvazgas;
	    teste=sequenciaAS[5].INJGvazgas;
	    teste=sequenciaAS[6].INJGvazgas;
	    teste=sequenciaAS[7].INJGvazgas;*/
}

void ASens::constroiVecParSerie(){
  int locdim=0;
  //while(locdim<dim){
		if(listaV.vpsep==1){
			vecParSerie[locdim]=ASPsep.parseriePres;
			locdim++;
		}
		if(listaV.vBSW==1){
			vecParSerie[locdim]=ASBSW.parserieBSW;
			locdim++;
		}
		if(listaV.vRGO==1){
			vecParSerie[locdim]=ASRGO.parserieRGO;
			locdim++;
		}
		if(listaV.vfongas==1){
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieTemp>0){
					vecParSerie[locdim]=ASFonGas[iFG].parserieTemp;
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazG>0){
					vecParSerie[locdim]=ASFonGas[iFG].parserieVazG;
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazC>0){
					vecParSerie[locdim]=ASFonGas[iFG].parserieVazC;
					locdim++;
				}
			}
		}
	    if(listaV.vbcs==1){
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieFreq>0){
					vecParSerie[locdim]=ASBCS[indBCS].parserieFreq;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieEstag>0){
					vecParSerie[locdim]=ASBCS[indBCS].parserieEstag;
					locdim++;
				}
	    	}
	    }
		if(listaV.vfonliq==1){
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieTemp>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieTemp;
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieVL>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieVL;
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieBet>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieBet;
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieFlu>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieFlu;
					locdim++;
				}
			}
		}
		if(listaV.vipr==1){
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieTemp>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieTemp;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parseriePres>0){
					vecParSerie[locdim]=ASIPR[iIPR].parseriePres;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieIP>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieIP;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieJP>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieJP;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieqMax>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieqMax;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieFlup>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieFlup;
					locdim++;
				}
			}
		}

		if(listaV.vfonmas==1){
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieTemp>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieTemp;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMP>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieMP;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMC>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieMC;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMG>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieMG;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieFlu>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieFlu;
					locdim++;
				}
			}
		}
	   /* if(listaV.vbcs==1){
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieFreq>0){
					vecParSerie[locdim]=ASBCS[indBCS].parserieFreq;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieEstag>0){
					vecParSerie[locdim]=ASBCS[indBCS].parserieEstag;
					locdim++;
				}
	    	}
	    }*/
	    if(listaV.vdp==1){
	    	for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    		 if(ASDP[indBCS].parserieDP>0){
					vecParSerie[locdim]=ASDP[indBCS].parserieDP;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpH==1){
	    	for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    		 if(ASdPdLH[indBCS].parserieDPHidro>0){
					vecParSerie[locdim]=ASdPdLH[indBCS].parserieDPHidro;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpF==1){
	    	for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    		 if(ASdPdLF[indBCS].parserieDPFric>0){
					vecParSerie[locdim]=ASdPdLF[indBCS].parserieDPFric;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdt==1){
	    	for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    		 if(ASdTdL[indBCS].parserieDT>0){
					vecParSerie[locdim]=ASdTdL[indBCS].parserieDT;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vbvol==1){
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieFreq>0){
					vecParSerie[locdim]=ASBVOL[indBCS].parserieFreq;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieCap>0){
					vecParSerie[locdim]=ASBVOL[indBCS].parserieCap;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieNPoli>0){
					vecParSerie[locdim]=ASBVOL[indBCS].parserieNPoli;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vvalv==1){
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieAbre>0){
					vecParSerie[locdim]=ASValv[indBCS].parserieAbre;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieCD>0){
					vecParSerie[locdim]=ASValv[indBCS].parserieCD;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vfuro==1){
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieAbre>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieAbre;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieCD>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieCD;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieBeta>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieBeta;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parseriePres>0){
					vecParSerie[locdim]=ASFuro[indBCS].parseriePres;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieTemp>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieTemp;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieFlu>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieFlu;
					locdim++;
				}
	    	}
	    }
		if(listaV.diam==1){
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieA>0){
					vecParSerie[locdim]=ASGeom[iG].parserieA;
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieB>0){
					vecParSerie[locdim]=ASGeom[iG].parserieB;
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieRug>0){
					vecParSerie[locdim]=ASGeom[iG].parserieRug;
					locdim++;
				}
			}
		}
		if(listaV.kequiv==1){
			for (int ik = 0; ik < nASCondEquiv; ik++){
				if(ASCondEquiv[ik].parserieK>0){
					vecParSerie[locdim]=ASCondEquiv[ik].parserieK;
					locdim++;
				}
			}
		}
		if(listaV.vgasinj==1){
			if(ASGasInj.parseriePresInj>0){
				vecParSerie[locdim]=ASGasInj.parseriePresInj;
				locdim++;
			}
			if(ASGasInj.parserieTemp>0){
				vecParSerie[locdim]=ASGasInj.parserieTemp;
				locdim++;
			}
			if(ASGasInj.parserieVazGas>0){
				vecParSerie[locdim]=ASGasInj.parserieVazGas;
				locdim++;
			}
		}
		if(listaV.vpresent==1){
			if(ASpEntrada.parseriePres>0){
				vecParSerie[locdim]=ASpEntrada.parseriePres;
				locdim++;
			}
			if(ASpEntrada.parserieBet>0){
				vecParSerie[locdim]=ASpEntrada.parserieBet;
				locdim++;
			}
			if(ASpEntrada.parserieTemp>0){
				vecParSerie[locdim]=ASpEntrada.parserieTemp;
				locdim++;
			}
			if(ASpEntrada.parserieTit>0){
				vecParSerie[locdim]=ASpEntrada.parserieTit;
				locdim++;
			}
		}
		if(listaV.vvazpresent==1){
			if(ASvpEntrada.parseriePres>0){
				vecParSerie[locdim]=ASvpEntrada.parseriePres;
				locdim++;
			}
			if(ASvpEntrada.parserieBet>0){
				vecParSerie[locdim]=ASvpEntrada.parserieBet;
				locdim++;
			}
			if(ASvpEntrada.parserieMass>0){
				vecParSerie[locdim]=ASvpEntrada.parserieMass;
				locdim++;
			}
			if(ASvpEntrada.parserieTemp>0){
				vecParSerie[locdim]=ASvpEntrada.parserieTemp;
				locdim++;
			}
		}
		if(listaV.vchk==1){
		  	  if(ASCHK.parserieAbre>0){
					vecParSerie[locdim]=ASCHK.parserieAbre;
					locdim++;
		  	  }
		  	  if(ASCHK.parserieCD>0){
					vecParSerie[locdim]=ASCHK.parserieCD;
					locdim++;
		  	  }
		}
		if(listaV.vpocinj==1){
			if(ASPInj.parseriePresI>0){
				vecParSerie[locdim]=ASPInj.parseriePresI;
				locdim++;
			}
			if(ASPInj.parseriePresFundo>0){
				vecParSerie[locdim]=ASPInj.parseriePresFundo;
				locdim++;
			}
			if(ASPInj.parserieTemp>0){
				vecParSerie[locdim]=ASPInj.parserieTemp;
				locdim++;
			}
			if(ASPInj.parserieVaz>0){
				vecParSerie[locdim]=ASPInj.parserieVaz;
				locdim++;
			}
		}
  //}
}

void ASens::constroiVecParSerieImex(){
  int locdim=0;
  //while(locdim<dim){
		if(listaV.vRGO==1){
			vecParSerie[locdim]=ASRGO.parserieRGO;
			locdim++;
		}
		if(listaV.vBSW==1){
			vecParSerie[locdim]=ASBSW.parserieBSW;
			locdim++;
		}
		if(listaV.vfonliq==1){
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieTemp>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieTemp;
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieVL>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieVL;
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieBet>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieBet;
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieFlu>0){
					vecParSerie[locdim]=ASFonLiq[iFL].parserieFlu;
					locdim++;
				}
			}
		}
		if(listaV.vfongas==1){
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieTemp>0){
					vecParSerie[locdim]=ASFonGas[iFG].parserieTemp;
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazG>0){
					vecParSerie[locdim]=ASFonGas[iFG].parserieVazG;
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazC>0){
					vecParSerie[locdim]=ASFonGas[iFG].parserieVazC;
					locdim++;
				}
			}
		}
		if(listaV.vpsep==1){
			vecParSerie[locdim]=ASPsep.parseriePres;
			locdim++;
		}
		if(listaV.vipr==1){
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieTemp>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieTemp;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parseriePres>0){
					vecParSerie[locdim]=ASIPR[iIPR].parseriePres;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieIP>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieIP;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieJP>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieJP;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieqMax>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieqMax;
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieFlup>0){
					vecParSerie[locdim]=ASIPR[iIPR].parserieFlup;
					locdim++;
				}
			}
		}
		if(listaV.vfonmas==1){
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieTemp>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieTemp;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMP>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieMP;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMC>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieMC;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMG>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieMG;
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieFlu>0){
					vecParSerie[locdim]=ASFonMas[iFM].parserieFlu;
					locdim++;
				}
			}
		}
	    if(listaV.vbcs==1){
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieFreq>0){
					vecParSerie[locdim]=ASBCS[indBCS].parserieFreq;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieEstag>0){
					vecParSerie[locdim]=ASBCS[indBCS].parserieEstag;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdp==1){
	    	for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    		 if(ASDP[indBCS].parserieDP>0){
					vecParSerie[locdim]=ASDP[indBCS].parserieDP;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpH==1){
	    	for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    		 if(ASdPdLH[indBCS].parserieDPHidro>0){
					vecParSerie[locdim]=ASdPdLH[indBCS].parserieDPHidro;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpF==1){
	    	for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    		 if(ASdPdLF[indBCS].parserieDPFric>0){
					vecParSerie[locdim]=ASdPdLF[indBCS].parserieDPFric;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdt==1){
	    	for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    		 if(ASdTdL[indBCS].parserieDT>0){
					vecParSerie[locdim]=ASdTdL[indBCS].parserieDT;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vbvol==1){
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieFreq>0){
					vecParSerie[locdim]=ASBVOL[indBCS].parserieFreq;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieCap>0){
					vecParSerie[locdim]=ASBVOL[indBCS].parserieCap;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieNPoli>0){
					vecParSerie[locdim]=ASBVOL[indBCS].parserieNPoli;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vvalv==1){
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieAbre>0){
					vecParSerie[locdim]=ASValv[indBCS].parserieAbre;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieCD>0){
					vecParSerie[locdim]=ASValv[indBCS].parserieCD;
					locdim++;
				}
	    	}
	    }
	    if(listaV.vfuro==1){
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieAbre>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieAbre;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieCD>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieCD;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieBeta>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieBeta;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parseriePres>0){
					vecParSerie[locdim]=ASFuro[indBCS].parseriePres;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieTemp>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieTemp;
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieFlu>0){
					vecParSerie[locdim]=ASFuro[indBCS].parserieFlu;
					locdim++;
				}
	    	}
	    }
		if(listaV.diam==1){
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieA>0){
					vecParSerie[locdim]=ASGeom[iG].parserieA;
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieB>0){
					vecParSerie[locdim]=ASGeom[iG].parserieB;
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieRug>0){
					vecParSerie[locdim]=ASGeom[iG].parserieRug;
					locdim++;
				}
			}
		}
		if(listaV.kequiv==1){
			for (int ik = 0; ik < nASCondEquiv; ik++){
				if(ASCondEquiv[ik].parserieK>0){
					vecParSerie[locdim]=ASCondEquiv[ik].parserieK;
					locdim++;
				}
			}
		}
		if(listaV.vgasinj==1){
			if(ASGasInj.parseriePresInj>0){
				vecParSerie[locdim]=ASGasInj.parseriePresInj;
				locdim++;
			}
			if(ASGasInj.parserieTemp>0){
				vecParSerie[locdim]=ASGasInj.parserieTemp;
				locdim++;
			}
			if(ASGasInj.parserieVazGas>0){
				vecParSerie[locdim]=ASGasInj.parserieVazGas;
				locdim++;
			}
		}
		if(listaV.vpresent==1){
			if(ASpEntrada.parseriePres>0){
				vecParSerie[locdim]=ASpEntrada.parseriePres;
				locdim++;
			}
			if(ASpEntrada.parserieBet>0){
				vecParSerie[locdim]=ASpEntrada.parserieBet;
				locdim++;
			}
			if(ASpEntrada.parserieTemp>0){
				vecParSerie[locdim]=ASpEntrada.parserieTemp;
				locdim++;
			}
			if(ASpEntrada.parserieTit>0){
				vecParSerie[locdim]=ASpEntrada.parserieTit;
				locdim++;
			}
		}
		if(listaV.vvazpresent==1){
			if(ASvpEntrada.parseriePres>0){
				vecParSerie[locdim]=ASvpEntrada.parseriePres;
				locdim++;
			}
			if(ASvpEntrada.parserieBet>0){
				vecParSerie[locdim]=ASvpEntrada.parserieBet;
				locdim++;
			}
			if(ASvpEntrada.parserieMass>0){
				vecParSerie[locdim]=ASvpEntrada.parserieMass;
				locdim++;
			}
			if(ASvpEntrada.parserieTemp>0){
				vecParSerie[locdim]=ASvpEntrada.parserieTemp;
				locdim++;
			}
		}
		if(listaV.vchk==1){
		  	  if(ASCHK.parserieAbre>0){
					vecParSerie[locdim]=ASCHK.parserieAbre;
					locdim++;
		  	  }
		  	  if(ASCHK.parserieCD>0){
					vecParSerie[locdim]=ASCHK.parserieCD;
					locdim++;
		  	  }
		}
		if(listaV.vpocinj==1){
			if(ASPInj.parseriePresI>0){
				vecParSerie[locdim]=ASPInj.parseriePresI;
				locdim++;
			}
			if(ASPInj.parseriePresFundo>0){
				vecParSerie[locdim]=ASPInj.parseriePresFundo;
				locdim++;
			}
			if(ASPInj.parserieTemp>0){
				vecParSerie[locdim]=ASPInj.parserieTemp;
				locdim++;
			}
			if(ASPInj.parserieVaz>0){
				vecParSerie[locdim]=ASPInj.parserieVaz;
				locdim++;
			}
		}
  //}
}


void ASens::traduzSeq(){
	int locdim;
	int iseq=0;
	while(iseq<nVariaveis){
		locdim=0;
		if(listaV.vpsep==1){
			sequenciaAS[iseq].PSEPpres=genericoAS[iseq].generico[locdim];
			locdim++;
		}
		if(listaV.vBSW==1){
			sequenciaAS[iseq].BSW=genericoAS[iseq].generico[locdim];
			locdim++;
		}
		if(listaV.vRGO==1){
			sequenciaAS[iseq].RGO=genericoAS[iseq].generico[locdim];
			locdim++;
		}
		if(listaV.vfongas==1){ //chrisao; alterei a ordem com a QLIQ
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieTemp>0){
					sequenciaAS[iseq].FGtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazG>0){
					sequenciaAS[iseq].FGvazgas.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazC>0){
					sequenciaAS[iseq].FGvazcomp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
	    if(listaV.vbcs==1){
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieFreq>0){
	    			 sequenciaAS[iseq].BCSfreq.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieEstag>0){
	    			 sequenciaAS[iseq].BCSnestag.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }

		if(listaV.vfonliq==1){
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieTemp>0){
					sequenciaAS[iseq].FLtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieVL>0){
					sequenciaAS[iseq].FLvazliq.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieBet>0){
					sequenciaAS[iseq].FLbet.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieFlu>0){
					sequenciaAS[iseq].FLindfluP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.vipr==1){
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieTemp>0){
					sequenciaAS[iseq].IPRtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parseriePres>0){
					sequenciaAS[iseq].IPRpres.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieIP>0){
					sequenciaAS[iseq].IPRip.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieJP>0){
					sequenciaAS[iseq].IPRjp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieqMax>0){
					sequenciaAS[iseq].IPRqMax.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieFlup>0){
					sequenciaAS[iseq].IPRindfluP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}

		if(listaV.vfonmas==1){
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieTemp>0){
					sequenciaAS[iseq].FMtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMP>0){
					sequenciaAS[iseq].FMvazMasP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMC>0){
					sequenciaAS[iseq].FMvazMasC.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMG>0){
					sequenciaAS[iseq].FMvazMasG.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieFlu>0){
					sequenciaAS[iseq].FMindfluP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
	    /*if(listaV.vbcs==1){
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieFreq>0){
	    			 sequenciaAS[iseq].BCSfreq.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieEstag>0){
	    			 sequenciaAS[iseq].BCSnestag.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }*/
	    if(listaV.vdp==1){
	    	for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    		 if(ASDP[indBCS].parserieDP>0){
	    			 sequenciaAS[iseq].DPdp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpH==1){
	    	for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    		 if(ASdPdLH[indBCS].parserieDPHidro>0){
	    			 sequenciaAS[iseq].DPdLH.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpF==1){
	    	for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    		 if(ASdPdLF[indBCS].parserieDPFric>0){
	    			 sequenciaAS[iseq].DPdLF.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdt==1){
	    	for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    		 if(ASdTdL[indBCS].parserieDT>0){
	    			 sequenciaAS[iseq].DTdL.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vbvol==1){
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieFreq>0){
	    			sequenciaAS[iseq].BVOLfreq.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieCap>0){
	    			 sequenciaAS[iseq].BVOLcapacidade.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieNPoli>0){
	    			 sequenciaAS[iseq].BVOLnpoli.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vvalv==1){
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieAbre>0){
	    			 sequenciaAS[iseq].VALVabertura.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieCD>0){
	    			 sequenciaAS[iseq].VALVcd.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vfuro==1){
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieAbre>0){
	    			 sequenciaAS[iseq].FUROabertura.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieCD>0){
	    			 sequenciaAS[iseq].FUROcd.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieBeta>0){
	    			 sequenciaAS[iseq].FURObeta.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parseriePres>0){
	    			 sequenciaAS[iseq].FUROpres.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieTemp>0){
	    			 sequenciaAS[iseq].FUROtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieFlu>0){
	    			 sequenciaAS[iseq].FUROindFlu.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
		if(listaV.diam==1){
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieA>0){
					sequenciaAS[iseq].GeomdA.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieB>0){
					sequenciaAS[iseq].GeomdB.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieRug>0){
					sequenciaAS[iseq].GeomRug.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.kequiv==1){
			for (int ik = 0; ik < nASCondEquiv; ik++){
				if(ASCondEquiv[ik].parserieK>0){
					sequenciaAS[iseq].condEqui.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.vgasinj==1){
			if(ASGasInj.parseriePresInj>0){
				sequenciaAS[iseq].INJGpresinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASGasInj.parserieTemp>0){
				sequenciaAS[iseq].INJGtemperatura=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASGasInj.parserieVazGas>0){
				sequenciaAS[iseq].INJGvazgas=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		if(listaV.vpresent==1){
			if(ASpEntrada.parseriePres>0){
				sequenciaAS[iseq].PEpres=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASpEntrada.parserieBet>0){
				sequenciaAS[iseq].PEbet=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASpEntrada.parserieTemp>0){
				sequenciaAS[iseq].PEtemperatura=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASpEntrada.parserieTit>0){
				sequenciaAS[iseq].PEtit=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		if(listaV.vvazpresent==1){
			if(ASvpEntrada.parseriePres>0){
				sequenciaAS[iseq].VPEpres=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASvpEntrada.parserieBet>0){
				sequenciaAS[iseq].VPEbet=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASvpEntrada.parserieMass>0){
				sequenciaAS[iseq].VPEmass=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASvpEntrada.parserieTemp>0){
				sequenciaAS[iseq].VPEtemperatura=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		if(listaV.vchk==1){
		  	  if(ASCHK.parserieAbre>0){
		  		sequenciaAS[iseq].CHKabertura=genericoAS[iseq].generico[locdim];
					locdim++;
		  	  }
		  	  if(ASCHK.parserieCD>0){
		  		sequenciaAS[iseq].CHKcd=genericoAS[iseq].generico[locdim];
					locdim++;
		  	  }
		}
		if(listaV.vpocinj==1){
			if(ASPInj.parseriePresI>0){
				sequenciaAS[iseq].PIpresinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASPInj.parseriePresFundo>0){
				sequenciaAS[iseq].PIpresfundo=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASPInj.parserieTemp>0){
				sequenciaAS[iseq].PItempinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASPInj.parserieVaz>0){
				sequenciaAS[iseq].PIvazinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		iseq++;
	}
}

void ASens::traduzSeqImex(){
	int locdim;
	int iseq=0;
	while(iseq<nVariaveis){
		locdim=0;

		if(listaV.vRGO==1){  //chris new -> troquei a ordem para o IMEX
			sequenciaAS[iseq].RGO=genericoAS[iseq].generico[locdim];
			locdim++;
		}

		if(listaV.vBSW==1){  //chris new -> troquei a ordem para o IMEX
			sequenciaAS[iseq].BSW=genericoAS[iseq].generico[locdim];
			locdim++;
		}

		if(listaV.vfonliq==1){
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieTemp>0){
					sequenciaAS[iseq].FLtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieVL>0){
					sequenciaAS[iseq].FLvazliq.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieBet>0){
					sequenciaAS[iseq].FLbet.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFL = 0; iFL < nASFL; iFL++){
				if(ASFonLiq[iFL].parserieFlu>0){
					sequenciaAS[iseq].FLindfluP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.vfongas==1){ //chrisao; alterei a ordem com a QLIQ
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieTemp>0){
					sequenciaAS[iseq].FGtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazG>0){
					sequenciaAS[iseq].FGvazgas.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFG = 0; iFG < nASFG; iFG++){
				if(ASFonGas[iFG].parserieVazC>0){
					sequenciaAS[iseq].FGvazcomp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.vpsep==1){
			sequenciaAS[iseq].PSEPpres=genericoAS[iseq].generico[locdim];
			locdim++;
		}
		if(listaV.vipr==1){
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieTemp>0){
					sequenciaAS[iseq].IPRtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parseriePres>0){
					sequenciaAS[iseq].IPRpres.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieIP>0){
					sequenciaAS[iseq].IPRip.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieJP>0){
					sequenciaAS[iseq].IPRjp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieqMax>0){
					sequenciaAS[iseq].IPRqMax.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(ASIPR[iIPR].parserieFlup>0){
					sequenciaAS[iseq].IPRindfluP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.vfonmas==1){
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieTemp>0){
					sequenciaAS[iseq].FMtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMP>0){
					sequenciaAS[iseq].FMvazMasP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMC>0){
					sequenciaAS[iseq].FMvazMasC.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieMG>0){
					sequenciaAS[iseq].FMvazMasG.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iFM = 0; iFM < nASFM; iFM++){
				if(ASFonMas[iFM].parserieFlu>0){
					sequenciaAS[iseq].FMindfluP.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
	    if(listaV.vbcs==1){
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieFreq>0){
	    			 sequenciaAS[iseq].BCSfreq.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    		 if(ASBCS[indBCS].parserieEstag>0){
	    			 sequenciaAS[iseq].BCSnestag.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdp==1){
	    	for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    		 if(ASDP[indBCS].parserieDP>0){
	    			 sequenciaAS[iseq].DPdp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpH==1){
	    	for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    		 if(ASdPdLH[indBCS].parserieDPHidro>0){
	    			 sequenciaAS[iseq].DPdLH.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdpF==1){
	    	for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    		 if(ASdPdLF[indBCS].parserieDPFric>0){
	    			 sequenciaAS[iseq].DPdLF.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vdt==1){
	    	for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    		 if(ASdTdL[indBCS].parserieDT>0){
	    			 sequenciaAS[iseq].DTdL.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vbvol==1){
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieFreq>0){
	    			sequenciaAS[iseq].BVOLfreq.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieCap>0){
	    			 sequenciaAS[iseq].BVOLcapacidade.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    		 if(ASBVOL[indBCS].parserieNPoli>0){
	    			 sequenciaAS[iseq].BVOLnpoli.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vvalv==1){
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieAbre>0){
	    			 sequenciaAS[iseq].VALVabertura.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASV; indBCS++){
	    		 if(ASValv[indBCS].parserieCD>0){
	    			 sequenciaAS[iseq].VALVcd.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
	    if(listaV.vfuro==1){
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieAbre>0){
	    			 sequenciaAS[iseq].FUROabertura.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieCD>0){
	    			 sequenciaAS[iseq].FUROcd.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieBeta>0){
	    			 sequenciaAS[iseq].FURObeta.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parseriePres>0){
	    			 sequenciaAS[iseq].FUROpres.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieTemp>0){
	    			 sequenciaAS[iseq].FUROtemp.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    	for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    		 if(ASFuro[indBCS].parserieFlu>0){
	    			 sequenciaAS[iseq].FUROindFlu.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
	    	}
	    }
		if(listaV.diam==1){
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieA>0){
					sequenciaAS[iseq].GeomdA.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieB>0){
					sequenciaAS[iseq].GeomdB.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
			for (int iG = 0; iG < nASGeom; iG++){
				if(ASGeom[iG].parserieRug>0){
					sequenciaAS[iseq].GeomRug.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.kequiv==1){
			for (int ik = 0; ik < nASCondEquiv; ik++){
				if(ASCondEquiv[ik].parserieK>0){
					sequenciaAS[iseq].condEqui.push_back(genericoAS[iseq].generico[locdim]);
					locdim++;
				}
			}
		}
		if(listaV.vgasinj==1){
			if(ASGasInj.parseriePresInj>0){
				sequenciaAS[iseq].INJGpresinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASGasInj.parserieTemp>0){
				sequenciaAS[iseq].INJGtemperatura=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASGasInj.parserieVazGas>0){
				sequenciaAS[iseq].INJGvazgas=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		if(listaV.vpresent==1){
			if(ASpEntrada.parseriePres>0){
				sequenciaAS[iseq].PEpres=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASpEntrada.parserieBet>0){
				sequenciaAS[iseq].PEbet=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASpEntrada.parserieTemp>0){
				sequenciaAS[iseq].PEtemperatura=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASpEntrada.parserieTit>0){
				sequenciaAS[iseq].PEtit=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		if(listaV.vvazpresent==1){
			if(ASvpEntrada.parseriePres>0){
				sequenciaAS[iseq].VPEpres=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASvpEntrada.parserieBet>0){
				sequenciaAS[iseq].VPEbet=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASvpEntrada.parserieMass>0){
				sequenciaAS[iseq].VPEmass=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASvpEntrada.parserieTemp>0){
				sequenciaAS[iseq].VPEtemperatura=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		if(listaV.vchk==1){
		  	  if(ASCHK.parserieAbre>0){
		  		sequenciaAS[iseq].CHKabertura=genericoAS[iseq].generico[locdim];
					locdim++;
		  	  }
		  	  if(ASCHK.parserieCD>0){
		  		sequenciaAS[iseq].CHKcd=genericoAS[iseq].generico[locdim];
					locdim++;
		  	  }
		}
		if(listaV.vpocinj==1){
			if(ASPInj.parseriePresI>0){
				sequenciaAS[iseq].PIpresinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASPInj.parseriePresFundo>0){
				sequenciaAS[iseq].PIpresfundo=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASPInj.parserieTemp>0){
				sequenciaAS[iseq].PItempinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
			if(ASPInj.parserieVaz>0){
				sequenciaAS[iseq].PIvazinj=genericoAS[iseq].generico[locdim];
				locdim++;
			}
		}
		iseq++;
	}
}


int ASens::inicializaSequen(int iSeq,int tipo){

	int locDim=0;
	int nvar=0;
	int kontaDim=0;
	//////////////////////////////////////////////////////////////////////////////////////
	int iVaria[dim]={};
	/////////////////////////////////////////////////////////////////////////////////////
	while(locDim<nVariaveis){
		for (int idim = 0; idim < dim; idim++){
			   if(vecParSerie[idim]>0){
			    	genericoAS[locDim].generico.push_back(iVaria[idim]);
			    	nvar++;
			    	kontaDim++;
			    	if(kontaDim==dim){
			    		locDim++;
			    		kontaDim=0;
			    		iVaria[idim]++;
			    		if(iVaria[idim]==vecParSerie[idim]){
			    			iVaria[idim]=0;
			    			int iOK=0;
			    			int decres=idim-1;
			    			while(iOK==0){
			    				if(decres>=0){
			    					iVaria[decres]++;
			    					if(iVaria[decres]==vecParSerie[decres]){
			    						iVaria[decres]=0;
			    						decres--;
			    					}
			    					else iOK=1;
			    					if(decres<0)iOK=1;
			    				}
			    				if(decres<0)iOK=1;
			    			}
			    		}
			    	}
			    	if(locDim==nVariaveis) break;
			    }
			}

	}
	return 1;
}

/*int ASens::inicializaSequen(int iSeq,int tipo){

	int locDim=0;
	int nvar=0;
	int kontaDim=0;
	//////////////////////////////////////////////////////////////////////////////////////
	int ipsep=0;
	int iIPRip[nASIPR]={};
	int iIPRpres[nASIPR]={};
	/////////////////////////////////////////////////////////////////////////////////////
	while(locDim<nVariaveis){
		if(listaV.vpsep==1){
				sequenciaAS[locDim].PSEPpres=ipsep;
				nvar++;
				kontaDim++;
				if(kontaDim==dim){
					locDim++;
					ipsep++;
					kontaDim=0;
				}
				if(locDim==nVariaveis) break;
		}
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
				if(listaV.vipr==1){
			    	  if(ASIPR[iIPR].parserieIP>0){
			    			  sequenciaAS[locDim].IPRip.push_back(iIPRip[iIPR]);
			    			  nvar++;
			    			  kontaDim++;
			    			  if(kontaDim==dim){
			    				  locDim++;
			    				  kontaDim=0;
			    				  iIPRip[iIPR]++;
			    				  if(iIPRip[iIPR]==ASIPR[iIPR].parserieIP){
			    					  iIPRip[iIPR]=0;
			    					  int iOK=0;
			    					  int decres=iIPR-1;
			    					  while(iOK==0){
			    						  if(decres>=0){
			    							  iIPRip[decres]++;
			    							  if(iIPRip[decres]==ASIPR[decres].parserieIP){
			    								  iIPRip[decres]=0;
			    								  decres--;
			    							  }
			    							  else iOK=1;
			    							  if(decres<0)iOK=1;
			    						  }
			    						  if(decres<0)iOK=1;
			    					  }
			    					  if(decres<0){
			    						  ipsep++;
			    						  if(ipsep==ASPsep.parseriePres)ipsep=0;
			    					  }
			    				  }
			    			  }
			    			  if(locDim==nVariaveis) break;
			    	  }
				}
			}

	}
	return 1;
}*/

void ASens::atualizaGeom(int ncelG,  Cel* celula, CelG* celulaG, int iduto,int tipovar, double val){
	for(int i=0;i<ncel;i++){
		if(celula[i].indGeom==iduto){
			if(tipovar==0){
				celula[i].duto.a=val;
				celula[i].duto.renovaD();
				celula[i].calor.geom.a=val;
				celula[i].calor.geom.renovaD();
				celula[i].dutoL.a=val;
				celula[i].dutoL.renovaD();
				celula[i].dutoR.a=val;
				celula[i].dutoR.renovaD();
			}
			else if(tipovar==1){
				celula[i].duto.b=val;
				celula[i].calor.geom.b=val;
				celula[i].dutoL.b=val;
				celula[i].dutoR.b=val;
			}
			else{
				celula[i].duto.rug=val;
				celula[i].calor.geom.rug=val;
				celula[i].dutoL.rug=val;
				celula[i].dutoR.rug=val;
			}
		}
	}
	for(int i=0;i<ncelG;i++){
		if(celulaG[i].indGeom==iduto){
			if(tipovar==0){
				celulaG[i].duto.a=val;
				celulaG[i].duto.renovaD();
				celulaG[i].calor.geom.a=val;
				celulaG[i].calor.geom.renovaD();
				celulaG[i].dutoL.a=val;
				celulaG[i].dutoL.renovaD();
				celulaG[i].dutoR.a=val;
				celulaG[i].dutoR.renovaD();
			}
			else if(tipovar==1){
				celulaG[i].duto.b=val;
				celulaG[i].calor.geom.b=val;
				celulaG[i].dutoL.b=val;
				celulaG[i].dutoR.b=val;
			}
			else{
				celulaG[i].duto.rug=val;
				celulaG[i].calor.geom.rug=val;
				celulaG[i].dutoL.rug=val;
				celulaG[i].dutoR.rug=val;
			}
		}
	}
}

void ASens::atualizaMat(int ncelG,  Cel* celula, CelG* celulaG, int imat,double val){
	for(int i=0;i<ncel;i++){
		for(int k=0;k<celula[i].calor.geom.ncamadas;k++){
			if(celula[i].calor.geom.indmat[k]==imat){
				celula[i].calor.geom.cond[k]=val;
			}
		}
	}
	for(int i=0;i<ncelG;i++){
		for(int k=0;k<celulaG[i].calor.geom.ncamadas;k++){
			if(celulaG[i].calor.geom.indmat[k]==imat){
				celulaG[i].calor.geom.cond[k]=val;
			}
		}
	}
}

void ASens::atualizaCompRGO(double rgo, ProFlu& flui){

	int npseudo=flui.npseudo;
	double* GivenInitialLiqComposition = new double [npseudo];
    for(int k=0;k<npseudo;k++)GivenInitialLiqComposition[k]=0;

	double* GivenInitialVapComposition = new double [npseudo];
	for(int k=0;k<npseudo;k++)GivenInitialVapComposition[k]=0;

	double* oGORAdjustedGlobalComp = new double [npseudo];
	double dAdjustedGlobalCompGOR;
	int iier;


	Marlim_AdjustGlobalCompositionForTargetGOR(flui.npseudo, flui.fracMol,
			flui.masMol,flui.tempCrit, flui.presCrit,
			flui.fatAcent, flui.TIndepPeneloux, flui.kij,
			flui.lij, flui.liqModel, flui.liqModel,
			flui.liqModel,
			flui.liqModel, 1.03322745279996,
			20., rgo, -1.0,
			GivenInitialLiqComposition, GivenInitialVapComposition,
			oGORAdjustedGlobalComp, &dAdjustedGlobalCompGOR,
	        &iier);
	 for(int k=0;k<npseudo;k++)flui.fracMol[k]=oGORAdjustedGlobalComp[k];
	 flui.atualizaPropCompStandard();
     delete GivenInitialLiqComposition;
     delete GivenInitialVapComposition;
     delete oGORAdjustedGlobalComp;

}

void ASens::atualizaRGO(Cel* celula, double val){
	if(celula[0].flui.flashCompleto!=2){
		celula[0].flui.RGO=val;
		celula[0].flui.RenovaFluido();
		if(celula[0].acsr.tipo==1){
			celula[0].acsr.injg.FluidoPro.RGO=val;
			celula[0].acsr.injg.FluidoPro.RenovaFluido();
		}
		else if(celula[0].acsr.tipo==2){
			celula[0].acsr.injl.FluidoPro.RGO=val;
			celula[0].acsr.injl.FluidoPro.RenovaFluido();
		}
		else if(celula[0].acsr.tipo==3){
			celula[0].acsr.ipr.FluidoPro.RGO=val;
			celula[0].acsr.ipr.FluidoPro.RenovaFluido();
		}
		else if(celula[0].acsr.tipo==10){
			celula[0].acsr.injm.FluidoPro.RGO=val;
			celula[0].acsr.injm.FluidoPro.RenovaFluido();
		}
	}
	else{
		atualizaCompRGO(val, celula[0].flui);
		if(celula[0].acsr.tipo==1){
			celula[0].acsr.injg.FluidoPro.RGO=val;
			celula[0].acsr.injg.FluidoPro.RenovaFluido();
		}
		else if(celula[0].acsr.tipo==2){
			atualizaCompRGO(val, celula[0].acsr.injl.FluidoPro);
		}
		else if(celula[0].acsr.tipo==3){
			atualizaCompRGO(val, celula[0].acsr.ipr.FluidoPro);
		}
		else if(celula[0].acsr.tipo==10){
			atualizaCompRGO(val, celula[0].acsr.injm.FluidoPro);
		}
	}

}

void ASens::atualizaBSW(Cel* celula, double val){
	for(int i=0;i<ncel;i++){
		celula[i].flui.BSW=val;
		celula[i].flui.RenovaFluido();
	}
	if(celula[0].acsr.tipo==1){
		celula[0].acsr.injg.FluidoPro.BSW=val;
		celula[0].acsr.injg.FluidoPro.RenovaFluido();
	}
	else if(celula[0].acsr.tipo==2){
		celula[0].acsr.injl.FluidoPro.BSW=val;
		celula[0].acsr.injl.FluidoPro.RenovaFluido();
	}
	else if(celula[0].acsr.tipo==3){
		celula[0].acsr.ipr.FluidoPro.BSW=val;
		celula[0].acsr.ipr.FluidoPro.RenovaFluido();
	}
	else if(celula[0].acsr.tipo==10){
		celula[0].acsr.injm.FluidoPro.BSW=val;
		celula[0].acsr.injm.FluidoPro.RenovaFluido();
	}

}

void ASens::atualizaCorrecao(int ncelG,  Cel* celula, CelG* celulaG, double* vdPdLH,
		double* vdPdLF, double* vdTdL){
	for(int i=0;i<ncel;i++){
		if(celula[i].inddPdLHidro>=0)celula[i].dPdLHidro=vdPdLH[celula[i].inddPdLHidro];
		if(celula[i].inddPdLFric>=0)celula[i].dPdLFric=vdPdLF[celula[i].inddPdLFric];
		if(celula[i].inddTdLCor>=0)celula[i].dTdLCor=vdTdL[celula[i].inddTdLCor];
	}
	for(int i=0;i<ncelG;i++){
		if(celulaG[i].inddPdLHidro>=0)celulaG[i].dPdLHidro=vdPdLH[celulaG[i].inddPdLHidro];
		if(celulaG[i].inddPdLFric>=0)celulaG[i].dPdLFric=vdPdLF[celulaG[i].inddPdLFric];
		if(celulaG[i].inddTdLCor>=0)celulaG[i].dTdLCor=vdTdL[celulaG[i].inddTdLCor];
	}
}

void ASens::cabecalhoAS(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq){

	ostringstream saidaP;
	saidaP << "variaveisInteresseAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);

	escreveIni <<" indice AS; "<<" Sucesso; "<<" Pressao Fundo; "<<" Temperatura Plataforma; ";

	int ind;
	if(listaV.vpsep==1){
		escreveIni <<" Pressao Separador; ";
	}
	if(listaV.vRGO==1){
		escreveIni <<" RGO fluido 0; ";
	}
	if(listaV.vBSW==1){
		escreveIni <<" BSW fluido 0; ";
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  escreveIni <<" Fonte de Gas "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  escreveIni <<" Fonte de Gas "<< icel<<" Vazao Gas; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  escreveIni <<" Fonte de Gas "<< icel<<" Vazao Liq Complementar; ";
	    		  konta3++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  escreveIni <<" BCS "<< icel<<" Frequencia; ";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  escreveIni <<" BCS "<< icel<<" N. Estagios; ";
	    		  konta2++;
	    	  }
	      }
	}

	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Vazao de Liquido; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Beta; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Fluido; ";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  escreveIni <<" IPR "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  escreveIni <<" IPR "<< icel<<" P Estatica; ";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  escreveIni <<" IPR "<< icel<<" IP; ";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  escreveIni <<" IPR "<< icel<<" II; ";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  escreveIni <<" IPR "<< icel<<" qMax; ";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  escreveIni <<" IPR "<< icel<<" fluido; ";
	    		  konta6++;
	    	  }
	      }
	}

	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Vazao Liquido; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Vazao Liquido Complementar; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Vazao Gas; ";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Fluido; ";
	    		  konta5++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  escreveIni <<" Delta Pressao "<< icel<<" Del. Pres.; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  escreveIni <<" Corr. Pres. Hidro; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  escreveIni <<" Corr. Pres. Fric; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  escreveIni <<" Corr. Temperatura; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  escreveIni <<" B. Volumetrica "<< icel<<" Frequencia; ";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  escreveIni <<" B. Volumetrica "<< icel<<" Capacidade; ";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  escreveIni <<" B. Volumetrica "<< icel<<" Fator Politropico; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  escreveIni <<" Valvula "<< icel<<" Abertura; ";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  escreveIni <<" Valvula "<< icel<<" CD; ";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  escreveIni <<" Vazamento "<< icel<<" Abertura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  escreveIni <<" Vazamento "<< icel<<" CD; ";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  escreveIni <<" Valvula "<< icel<<" Beta; ";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  escreveIni <<" Valvula "<< icel<<" Pressao; ";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  escreveIni <<" Valvula "<< icel<<" Temperatura; ";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  escreveIni <<" Valvula "<< icel<<" Fluido; ";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  int igeo=ASGeom[iG].indGeom;
	    		  escreveIni <<" Secao Transversal "<< igeo<<" Maior Diam Interno; ";
	    		  konta1++;
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  int igeo=ASGeom[iG].indGeom;
	    		  escreveIni <<" Secao Transversal "<< igeo<<" Menor Diam Interno; ";
	    		  konta2++;
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  int igeo=ASGeom[iG].indGeom;
	    		  escreveIni <<" Secao Transversal "<< igeo<<" Rugosidade; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  int imat=ASCondEquiv[ik].indMat;
	    		  escreveIni <<" Material "<< imat<<" Condutividade; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			escreveIni <<" Pressao de Injecao; ";
		}
		if(ASGasInj.parserieTemp>0){
			escreveIni <<" Temperatura de Injecao; ";
		}
		if(ASGasInj.parserieVazGas>0){
			escreveIni <<" Vazao de Injecao; ";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		escreveIni <<" Pressao de Contorno; ";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		escreveIni <<" Beta de Contorno; ";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		escreveIni <<" Temperatura de Contorno; ";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		escreveIni <<" Titulo de Contorno; ";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		escreveIni <<" Pressao de Contorno; ";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		escreveIni <<" Beta de Contorno; ";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		escreveIni <<" Vazao de Contorno; ";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		escreveIni <<" Temperatura de Contorno; ";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		escreveIni <<" Abertura de Choke; ";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		escreveIni <<" CD de Choke; ";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		escreveIni <<" Pressao a Jusante; ";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		escreveIni <<" Pressao a Montante; ";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		escreveIni <<" Temperatura de Injecao; ";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		escreveIni <<" Vazao de Injecao; ";
	  	  }
	}
	escreveIni << endl;
	escreveIni.close();
}

void ASens::cabecalhoASImex(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq){

	ostringstream saidaP;
	saidaP << "variaveisInteresseAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);

	escreveIni <<" indice AS; "<<" Sucesso; "<<" Pressao Fundo; "<<" Temperatura Plataforma; ";

	int ind;
	if(listaV.vRGO==1){
		escreveIni <<" RGO fluido 0; ";
	}
	if(listaV.vBSW==1){
		escreveIni <<" BSW fluido 0; ";
	}
	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Vazao de Liquido; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Beta; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  escreveIni <<" Fonte de Liquido "<< icel<<" Fluido; ";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  escreveIni <<" Fonte de Gas "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  escreveIni <<" Fonte de Gas "<< icel<<" Vazao Gas; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  escreveIni <<" Fonte de Gas "<< icel<<" Vazao Liq Complementar; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vpsep==1){
		escreveIni <<" Pressao Separador; ";
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  escreveIni <<" IPR "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  escreveIni <<" IPR "<< icel<<" P Estatica; ";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  escreveIni <<" IPR "<< icel<<" IP; ";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  escreveIni <<" IPR "<< icel<<" II; ";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  escreveIni <<" IPR "<< icel<<" qMax; ";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  escreveIni <<" IPR "<< icel<<" fluido; ";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Temperatura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Vazao Liquido; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Vazao Liquido Complementar; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Vazao Gas; ";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  escreveIni <<" Fonte de Massa "<< icel<<" Fluido; ";
	    		  konta5++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  escreveIni <<" BCS "<< icel<<" Frequencia; ";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  escreveIni <<" BCS "<< icel<<" N. Estagios; ";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  escreveIni <<" Delta Pressao "<< icel<<" Del. Pres.; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  escreveIni <<" Corr. Pres. Hidro; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  escreveIni <<" Corr. Pres. Fric; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  escreveIni <<" Corr. Temperatura; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  escreveIni <<" B. Volumetrica "<< icel<<" Frequencia; ";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  escreveIni <<" B. Volumetrica "<< icel<<" Capacidade; ";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  escreveIni <<" B. Volumetrica "<< icel<<" Fator Politropico; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  escreveIni <<" Valvula "<< icel<<" Abertura; ";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  escreveIni <<" Valvula "<< icel<<" CD; ";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  escreveIni <<" Vazamento "<< icel<<" Abertura; ";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  escreveIni <<" Vazamento "<< icel<<" CD; ";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  escreveIni <<" Valvula "<< icel<<" Beta; ";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  escreveIni <<" Valvula "<< icel<<" Pressao; ";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  escreveIni <<" Valvula "<< icel<<" Temperatura; ";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  escreveIni <<" Valvula "<< icel<<" Fluido; ";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  int igeo=ASGeom[iG].indGeom;
	    		  escreveIni <<" Secao Transversal "<< igeo<<" Maior Diam Interno; ";
	    		  konta1++;
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  int igeo=ASGeom[iG].indGeom;
	    		  escreveIni <<" Secao Transversal "<< igeo<<" Menor Diam Interno; ";
	    		  konta2++;
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  int igeo=ASGeom[iG].indGeom;
	    		  escreveIni <<" Secao Transversal "<< igeo<<" Rugosidade; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  int imat=ASCondEquiv[ik].indMat;
	    		  escreveIni <<" Material "<< imat<<" Condutividade; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			escreveIni <<" Pressao de Injecao; ";
		}
		if(ASGasInj.parserieTemp>0){
			escreveIni <<" Temperatura de Injecao; ";
		}
		if(ASGasInj.parserieVazGas>0){
			escreveIni <<" Vazao de Injecao; ";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		escreveIni <<" Pressao de Contorno; ";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		escreveIni <<" Beta de Contorno; ";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		escreveIni <<" Temperatura de Contorno; ";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		escreveIni <<" Titulo de Contorno; ";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		escreveIni <<" Pressao de Contorno; ";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		escreveIni <<" Beta de Contorno; ";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		escreveIni <<" Vazao de Contorno; ";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		escreveIni <<" Temperatura de Contorno; ";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		escreveIni <<" Abertura de Choke; ";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		escreveIni <<" CD de Choke; ";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		escreveIni <<" Pressao a Jusante; ";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		escreveIni <<" Pressao a Montante; ";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		escreveIni <<" Temperatura de Injecao; ";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		escreveIni <<" Vazao de Injecao; ";
	  	  }
	}
	escreveIni << endl;
	escreveIni.close();
}

void ASens::imprimeVarInteresseAS(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq, int seq){

	ostringstream saidaP;
	saidaP << "variaveisInteresseAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::app);

	int ind;
	if(listaV.vpsep==1){
		ind=sequenciaAS[seq].PSEPpres;
		escreveIni <<"   "<<ASPsep.pres[ind]<<" ; ";
	}
	if(listaV.vRGO==1){
		ind=sequenciaAS[seq].RGO;
		escreveIni <<"   "<<ASRGO.RGO[ind]<< " ; ";
	}
	if(listaV.vBSW==1){
		ind=sequenciaAS[seq].BSW;
		flup[0].BSW=ASBSW.BSW[ind];
		flup[0].RenovaFluido();
		atualizaBSW(celula, ASBSW.BSW[ind]);
		escreveIni <<"   "<<ASBSW.BSW[ind]<< " ; ";
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FGtemp[konta1];
	    		  escreveIni <<"   " << ASFonGas[iFG].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  ind=sequenciaAS[seq].FGvazgas[konta2];
	    		  escreveIni <<"   " <<ASFonGas[iFG].vazgas[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  ind=sequenciaAS[seq].FGvazcomp[konta3];
	    		  escreveIni <<"   " <<ASFonGas[iFG].vazcomp[ind]<< " ; ";
	    		  konta3++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  escreveIni <<"   " << ASBCS[indBCS].freq[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  escreveIni <<seq<<" : " << " indice BCS = " << konta2<<" celula Fonte "<< icel<<
					" indice N. Estagios= "<<ind<<" N. Estagios= "<<ASBCS[indBCS].nestag[ind]<< "\n";
	    		  konta2++;
	    	  }
	      }
	}

	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FLtemp[konta1];
	    		  escreveIni <<"  " << ASFonLiq[iFL].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  ind=sequenciaAS[seq].FLvazliq[konta2];
	    		  escreveIni <<"   " <<ASFonLiq[iFL].vazliq[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  ind=sequenciaAS[seq].FLbet[konta3];
	    		  escreveIni <<"   " <<ASFonLiq[iFL].bet[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FLindfluP[konta4];
	    		  int indFlu=ASFonLiq[iFL].indfluP[ind];
	    		  escreveIni <<"   " << indFlu<< " ; ";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  ind=sequenciaAS[seq].IPRtemp[konta1];
	    		  escreveIni <<"   " << ASIPR[iIPR].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  ind=sequenciaAS[seq].IPRpres[konta2];
	    		  escreveIni <<"   " <<ASIPR[iIPR].pres[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  ind=sequenciaAS[seq].IPRip[konta3];
	    		  escreveIni <<seq<<"   " << ASIPR[iIPR].ip[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  ind=sequenciaAS[seq].IPRjp[konta4];
	    		  escreveIni <<"   " << ASIPR[iIPR].jp[ind]<< " ; ";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  ind=sequenciaAS[seq].IPRqMax[konta5];
	    		  escreveIni <<"   " << ASIPR[iIPR].qMax[ind]<< " ; ";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  ind=sequenciaAS[seq].IPRindfluP[konta6];
	    		  escreveIni <<"   " << ASIPR[iIPR].indfluP[ind]<< " ; ";
	    		  konta6++;
	    	  }
	      }
	}

	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FMtemp[konta1];
	    		  escreveIni <<"   " << ASFonMas[iFM].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  ind=sequenciaAS[seq].FMvazMasP[konta2];
	    		  escreveIni <<"   "<<ASFonMas[iFM].vazMasP[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  ind=sequenciaAS[seq].FMvazMasC[konta3];
	    		  escreveIni <<"   " <<ASFonMas[iFM].vazMasC[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  ind=sequenciaAS[seq].FMvazMasG[konta4];
				  escreveIni <<"   " << ASFonMas[iFM].vazMasG[ind]<< " ; ";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FMindfluP[konta5];
	    		  escreveIni <<"   " << ASFonMas[iFM].indfluP[ind]<< " ; ";
	    		  konta5++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  escreveIni <<"   " << ASDP[indBCS].dp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  ind=sequenciaAS[seq].DPdLH[konta1];
	    		  escreveIni <<"   " <<ASdPdLH[indBCS].dPHidro[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  ind=sequenciaAS[seq].DPdLF[konta1];
	    		  escreveIni <<"   " <<ASdPdLF[indBCS].dPFric[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  ind=sequenciaAS[seq].DTdL[konta1];
	    		  escreveIni <<"   " <<ASdTdL[indBCS].dT[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BVOLfreq[konta1];
	    		  escreveIni <<"   " <<ASBVOL[indBCS].freq[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  ind=sequenciaAS[seq].BVOLcapacidade[konta2];
	    		  escreveIni <<"   " << ASBVOL[indBCS].capacidade[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  ind=sequenciaAS[seq].BVOLnpoli[konta3];
	    		  escreveIni <<"   " <<ASBVOL[indBCS].npoli[ind]<< " ; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].VALVabertura[konta1];
	    		  escreveIni << "   " << ASValv[indBCS].abertura[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].VALVcd[konta2];
	    		  escreveIni <<" : " << ASValv[indBCS].cd[ind]<< " ; ";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].FUROabertura[konta1];
	    		  escreveIni <<"   " << ASFuro[indBCS].abertura[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].FUROcd[konta2];
	    		  escreveIni <<"   " << ASFuro[indBCS].cd[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  ind=sequenciaAS[seq].FURObeta[konta3];
	    		  escreveIni <<"   " <<ASFuro[indBCS].beta[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  ind=sequenciaAS[seq].FUROpres[konta4];
	    		  escreveIni <<"   " << ASFuro[indBCS].pres[ind]<< " ; ";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FUROtemp[konta5];
	    		  escreveIni <<"   " << " Temperatura= "<<ASFuro[indBCS].temp[ind]<< " ; ";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FUROindFlu[konta6];
	    		  int indFlu=ASFuro[indBCS].indFlu[ind];
	    		  escreveIni <<"   " <<indFlu<< " ; ";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  ind=sequenciaAS[seq].GeomdA[konta1];
	    		  escreveIni <<"   " << ASGeom[iG].dA[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta2];
	    		  escreveIni <<"   " <<ASGeom[iG].dB[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta3];
	    		  escreveIni <<"   " << ASGeom[iG].rug[ind]<< " ; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  ind=sequenciaAS[seq].condEqui[konta1];
	    		  escreveIni <<"   " << ASCondEquiv[ik].cond[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			ind=sequenciaAS[seq].INJGpresinj;
			escreveIni <<"   "<< ASGasInj.presinj[ind]<< " ; ";
		}
		if(ASGasInj.parserieTemp>0){
			ind=sequenciaAS[seq].INJGtemperatura;
			escreveIni <<"   "<< ASGasInj.temperatura[ind]<< " ; ";
		}
		if(ASGasInj.parserieVazGas>0){
			ind=sequenciaAS[seq].INJGvazgas;
			escreveIni <<"   "<< ASGasInj.vazgas[ind]<< " ; ";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].PEpres;
	  		escreveIni <<"   "<< ASpEntrada.pres[ind]<< " ; ";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].PEbet;
	  		escreveIni <<"   "<<ASpEntrada.bet[ind]<< " ; ";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].PEtemperatura;
	  		escreveIni <<"   "<< ASpEntrada.temperatura[ind]<< " ; ";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		ind=sequenciaAS[seq].PEtit;
	  		escreveIni <<"   "<< ASpEntrada.tit[ind]<< " ; ";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].VPEpres;
	  		escreveIni <<"   "<< ASvpEntrada.pres[ind]<< " ; ";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].VPEbet;
	  		escreveIni <<"   "<<ASvpEntrada.bet[ind]<< " ; ";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		ind=sequenciaAS[seq].VPEmass;
	  		escreveIni <<"   "<<ASvpEntrada.mass[ind]<< " ; ";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].VPEtemperatura;
	  		escreveIni <<"   "<<ASvpEntrada.temperatura[ind]<< " ; ";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		ind=sequenciaAS[seq].CHKabertura;
	  		escreveIni <<"   "<<ASCHK.abertura[ind]<< " ; ";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		ind=sequenciaAS[seq].CHKcd;
	  		escreveIni <<"   "<<chokeSup.cdchk<< " ; ";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		ind=sequenciaAS[seq].PIpresinj;
	  		escreveIni <<" : "<< ASPInj.presfundo[ind]<< " ; ";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		ind=sequenciaAS[seq].PIpresfundo;
	  		escreveIni <<" : "<<ASPInj.presinj[ind]<< " ; ";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		ind=sequenciaAS[seq].PItempinj;
	  		escreveIni <<" : "<< ASPInj.tempinj[ind]<< " ; ";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		ind=sequenciaAS[seq].PIvazinj;
	  		escreveIni <<" : "<< ASPInj.vazinj[ind]<< " ; ";
	  	  }
	}
	escreveIni << endl;

	escreveIni.close();
}


void ASens::selecaoAS(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
		 double& pGSup,double& temperatura,double& presiniG, double& tempiniG, double& vazgasG,
		 double& presE, double& tempE,double& titE, double& betaE, double& vazE, int seq, int& indCHK,
		 double* vdPdLH, double* vdPdLF, double* vdTdL,int imprime){

	ostringstream saidaP;
	saidaP << "relacaoAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::app);

	indCHK=0;
	int ind;
	if(listaV.vpsep==1){
		ind=sequenciaAS[seq].PSEPpres;
		pGSup=ASPsep.pres[ind];
		if(imprime==1){
			escreveIni <<seq<<" : "<< " indice PSep = " << ind <<" Pressao Separador= "<<pGSup<< "\n";
		}
	}
	if(listaV.vRGO==1){
		ind=sequenciaAS[seq].RGO;
		if(flup[0].flashCompleto!=2){
			flup[0].RGO=ASRGO.RGO[ind];
			flup[0].RenovaFluido();
		}
		else atualizaCompRGO(ASRGO.RGO[ind], flup[0]);
		atualizaRGO(celula, ASRGO.RGO[ind]);
		if(imprime==1){
		escreveIni <<seq<<" : "<< " indice RGO = " << ind <<" RGO fluido 0= "<<flup[0].RGO<< "\n";
		}
	}
	if(listaV.vBSW==1){
		ind=sequenciaAS[seq].BSW;
		flup[0].BSW=ASBSW.BSW[ind];
		flup[0].RenovaFluido();
		atualizaBSW(celula, ASBSW.BSW[ind]);
		if(imprime==1) escreveIni <<seq<<" : "<< " indice BSW = " << ind <<" BSW fluido 0= "<<flup[0].BSW<< "\n";
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FGtemp[konta1];
	    		  celula[icel].acsr.injg.temp=ASFonGas[iFG].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Gas = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASFonGas[iFG].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  ind=sequenciaAS[seq].FGvazgas[konta2];
	    		  celula[icel].acsr.injg.QGas=ASFonGas[iFG].vazgas[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Gas = " << konta2<<" celula Fonte "<< icel<<
					" indice Vazao Gas = "<<ind<<" Vazao Gas= "<<ASFonGas[iFG].vazgas[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  ind=sequenciaAS[seq].FGvazcomp[konta3];
	    		  celula[icel].acsr.injg.razCompGas=ASFonGas[iFG].vazcomp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Gas = " << konta3<<" celula Fonte "<< icel<<
					" indice Vazao Liq Complementar = "<<ind<<" Vazao Liq Complementar= "<<ASFonGas[iFG].vazcomp[ind]<< "\n";
	    		  konta3++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  //celula[icel].acsr.bcs.freq=ASBCS[indBCS].freq[ind];
	    		  celula[icel].acsr.bcs.freqnova=ASBCS[indBCS].freq[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice BCS = " << konta1<<" celula Fonte "<< icel<<
					" indice Frequencia= "<<ind<<" Frequencia= "<<ASBCS[indBCS].freq[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice BCS = " << konta2<<" celula Fonte "<< icel<<
					" indice N. Estagios= "<<ind<<" N. Estagios= "<<ASBCS[indBCS].nestag[ind]<< "\n";
	    		  konta2++;
	    	  }
	      }
	}

	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FLtemp[konta1];
	    		  celula[icel].acsr.injl.temp=ASFonLiq[iFL].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASFonLiq[iFL].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  ind=sequenciaAS[seq].FLvazliq[konta2];
	    		  celula[icel].acsr.injl.QLiq=ASFonLiq[iFL].vazliq[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta2<<" celula Fonte "<< icel<<
					" indice Vazao Liq = "<<ind<<" Vazao de Liquido= "<<ASFonLiq[iFL].vazliq[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  ind=sequenciaAS[seq].FLbet[konta3];
	    		  celula[icel].acsr.injl.bet=ASFonLiq[iFL].bet[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta3<<" celula Fonte "<< icel<<
					" indice Beta = "<<ind<<" Beta= "<<ASFonLiq[iFL].bet[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FLindfluP[konta4];
	    		  int indFlu=ASFonLiq[iFL].indfluP[ind];
	    		  celula[icel].acsr.injl.FluidoPro=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta4<<" celula Fonte "<< icel<<
					" indice Fluido = "<<ind<<" Fluido= "<<indFlu<< "\n";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  ind=sequenciaAS[seq].IPRtemp[konta1];
	    		  celula[icel].acsr.ipr.Tres=ASIPR[iIPR].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASIPR[iIPR].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  ind=sequenciaAS[seq].IPRpres[konta2];
	    		  celula[icel].acsr.ipr.Pres=ASIPR[iIPR].pres[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta2<<" celula Fonte "<< icel<<
					" indice P Estatica = "<<ind<<" P Estatica= "<<ASIPR[iIPR].pres[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  ind=sequenciaAS[seq].IPRip[konta3];
	    		  celula[icel].acsr.ipr.ip=ASIPR[iIPR].ip[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta3<<" celula Fonte "<< icel<<
					" indice IP = "<<ind<<" IP= "<<ASIPR[iIPR].ip[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  ind=sequenciaAS[seq].IPRjp[konta4];
	    		  celula[icel].acsr.ipr.ij=ASIPR[iIPR].jp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta4<<" celula Fonte "<< icel<<
					" indice II = "<<ind<<" II= "<<ASIPR[iIPR].jp[ind]<< "\n";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  ind=sequenciaAS[seq].IPRqMax[konta5];
	    		  celula[icel].acsr.ipr.qMax=ASIPR[iIPR].qMax[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta5<<" celula Fonte "<< icel<<
					" indice qMax = "<<ind<<" qMax= "<<ASIPR[iIPR].qMax[ind]<< "\n";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  ind=sequenciaAS[seq].IPRindfluP[konta6];
	    		  int indFlu=ASIPR[iIPR].indfluP[ind];
	    		  celula[icel].acsr.ipr.FluidoPro=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta6<<" celula Fonte "<< icel<<
					" indice Fluido = "<<ind<<" fluido= "<<ASIPR[iIPR].indfluP[ind]<< "\n";
	    		  konta6++;
	    	  }
	      }
	}

	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FMtemp[konta1];
	    		  celula[icel].acsr.injm.temp=ASFonMas[iFM].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASFonMas[iFM].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  ind=sequenciaAS[seq].FMvazMasP[konta2];
	    		  celula[icel].acsr.injm.MassP=ASFonMas[iFM].vazMasP[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta2<<" celula Fonte "<< icel<<
					" indice Vazao Liquido= "<<ind<<" Vazao Liquido= "<<ASFonMas[iFM].vazMasP[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  ind=sequenciaAS[seq].FMvazMasC[konta3];
	    		  celula[icel].acsr.injm.MassC=ASFonMas[iFM].vazMasC[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta3<<" celula Fonte "<< icel<<
					" indice Vazao Liquido Complementar= "<<ind<<" Vazao Liquido Complementar= "<<ASFonMas[iFM].vazMasC[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  ind=sequenciaAS[seq].FMvazMasG[konta4];
	    		  celula[icel].acsr.injm.MassG=ASFonMas[iFM].vazMasG[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta4<<" celula Fonte "<< icel<<
					" indice Vazao Gas= "<<ind<<" Vazao Gas= "<<ASFonMas[iFM].vazMasG[ind]<< "\n";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FMindfluP[konta5];
	    		  int indFlu=ASFonMas[iFM].indfluP[ind];
	    		  celula[icel].acsr.injm.FluidoPro=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta5<<" celula Fonte "<< icel<<
					" indice Fluido= "<<ind<<" Fluido= "<<ASFonMas[iFM].indfluP[ind]<< "\n";
	    		  konta5++;
	    	  }
	      }
	}
/*    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  celula[icel].acsr.bcs.freq=ASBCS[indBCS].freq[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice BCS = " << konta1<<" celula Fonte "<< icel<<
					" indice Frequencia= "<<ind<<" Frequencia= "<<ASBCS[indBCS].freq[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice BCS = " << konta2<<" celula Fonte "<< icel<<
					" indice N. Estagios= "<<ind<<" N. Estagios= "<<ASBCS[indBCS].nestag[ind]<< "\n";
	    		  konta2++;
	    	  }
	      }
	}*/
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  celula[icel].acsr.delp=ASDP[indBCS].dp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Del. Pres, = " << konta1<<" celula Fonte "<< icel<<
					" indice Del. Pres.= "<<ind<<" Del. Pres.= "<<ASDP[indBCS].dp[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  ind=sequenciaAS[seq].DPdLH[konta1];
	    		  vdPdLH[icel]=ASdPdLH[indBCS].dPHidro[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Corr. Pres. Hidro, = " << konta1<<" indice da colecao de fatores de correcao "<< icel<<
					" indice na AS.= "<<ind<<" Corr. Pres. Hidro= "<<ASdPdLH[indBCS].dPHidro[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  ind=sequenciaAS[seq].DPdLF[konta1];
	    		  vdPdLF[icel]=ASdPdLF[indBCS].dPFric[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Corr. Pres. Fric., = " << konta1<<" indice da colecao de fatores de correcao "<< icel<<
					" indice na AS.= "<<ind<<" Corr. Pres. Fric.= "<<ASdPdLF[indBCS].dPFric[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  ind=sequenciaAS[seq].DTdL[konta1];
	    		  vdTdL[icel]=ASdTdL[indBCS].dT[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Corr. Temperatura, = " << konta1<<" indice da colecao de fatores de correcao "<< icel<<
					" indice na AS.= "<<ind<<" Corr. Temperatura= "<<ASdTdL[indBCS].dT[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1 || listaV.vdpF==1 || listaV.vdt==1)atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BVOLfreq[konta1];
	    		  celula[icel].acsr.bvol.freq=ASBVOL[indBCS].freq[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Bomba Vol., = " << konta1<<" celula Fonte "<< icel<<
					" indice Frequencia= "<<ind<<" Frequencia= "<<ASBVOL[indBCS].freq[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  ind=sequenciaAS[seq].BVOLcapacidade[konta2];
	    		  celula[icel].acsr.bvol.capacidade=ASBVOL[indBCS].capacidade[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Bomba Vol., = " << konta2<<" celula Fonte "<< icel<<
					" indice Capacidade= "<<ind<<" Capacidade= "<<ASBVOL[indBCS].capacidade[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  ind=sequenciaAS[seq].BVOLnpoli[konta3];
	    		  celula[icel].acsr.bvol.npoli=ASBVOL[indBCS].npoli[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Bomba Vol., = " << konta3<<" celula Fonte "<< icel<<
					" indice Fator Politropico= "<<ind<<" Fator Politropico= "<<ASBVOL[indBCS].npoli[ind]<< "\n";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].VALVabertura[konta1];
	    		  celula[icel].acsr.chk.AreaGarg=ASValv[indBCS].abertura[ind]*celula[icel].acsr.chk.AreaTub;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Valvula, = " << konta1<<" celula Fonte "<< icel<<
					" indice Abertura= "<<ind<<" Abertura= "<<ASValv[indBCS].abertura[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].VALVcd[konta2];
	    		  celula[icel].acsr.chk.cdchk=ASValv[indBCS].cd[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Valvula, = " << konta2<<" celula Fonte "<< icel<<
					" indice CD= "<<ind<<" CD= "<<ASValv[indBCS].cd[ind]<< "\n";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].FUROabertura[konta1];
	    		  celula[icel].acsr.fontechk.abertura=ASFuro[indBCS].abertura[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta1<<" celula Fonte "<< icel<<
					" indice Abertura= "<<ind<<" Abertura= "<<ASFuro[indBCS].abertura[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].FUROcd[konta2];
	    		  celula[icel].acsr.fontechk.cd=ASFuro[indBCS].cd[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta2<<" celula Fonte "<< icel<<
					" indice CD= "<<ind<<" CD= "<<ASFuro[indBCS].cd[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  ind=sequenciaAS[seq].FURObeta[konta3];
	    		  celula[icel].acsr.fontechk.bet=ASFuro[indBCS].beta[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta3<<" celula Fonte "<< icel<<
					" indice Beta= "<<ind<<" Beta= "<<ASFuro[indBCS].beta[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  ind=sequenciaAS[seq].FUROpres[konta4];
	    		  celula[icel].acsr.fontechk.pamb=ASFuro[indBCS].pres[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta4<<" celula Fonte "<< icel<<
					" indice Pressao= "<<ind<<" Pressao= "<<ASFuro[indBCS].pres[ind]<< "\n";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FUROtemp[konta5];
	    		  celula[icel].acsr.fontechk.tempT=ASFuro[indBCS].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta5<<" celula Fonte "<< icel<<
					" indice Temperatura= "<<ind<<" Temperatura= "<<ASFuro[indBCS].temp[ind]<< "\n";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FUROindFlu[konta6];
	    		  int indFlu=ASFuro[indBCS].indFlu[ind];
	    		  celula[icel].acsr.fontechk.fluidoPamb=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta6<<" celula Fonte "<< icel<<
					" indice Fluido= "<<ind<<" Fluido= "<<indFlu<< "\n";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  ind=sequenciaAS[seq].GeomdA[konta1];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Secao Transversal = " << konta1<<" secao transversal "<< igeo<<
					" indice Maior Diametro = "<<ind<<" Maior Diam Interno= "<<ASGeom[iG].dA[ind]<< "\n";
	    		  konta1++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,0, ASGeom[iG].dA[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta2];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Secao Transversal = " << konta2<<" secao transversal "<< igeo<<
					" indice Menor Diametro = "<<ind<<" Menor Diam Interno= "<<ASGeom[iG].dB[ind]<< "\n";
	    		  konta2++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,1, ASGeom[iG].dB[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta3];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Secao Transversal = " << konta3<<" secao transversal "<< igeo<<
					" indice Rugosidade = "<<ind<<" Rugosidade= "<<ASGeom[iG].rug[ind]<< "\n";
	    		  konta3++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,2, ASGeom[iG].rug[ind]);
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  ind=sequenciaAS[seq].condEqui[konta1];
	    		  int imat=ASCondEquiv[ik].indMat;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Material = " << konta1<<" material "<< imat<<
					" indice Condutividade = "<<ind<<" Condutividade= "<<ASCondEquiv[ik].cond[ind]<< "\n";
	    		  konta1++;
	    		  atualizaMat(ncelG, celula, celulaG, imat, ASCondEquiv[ik].cond[ind]);
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			ind=sequenciaAS[seq].INJGpresinj;
			presiniG=ASGasInj.presinj[ind];
			if(imprime==1)escreveIni <<seq<<" : "<< " indice Pressao Injecao = " << ind <<" Pressao Injecao= "<<ASGasInj.presinj[ind]<< "\n";
		}
		if(ASGasInj.parserieTemp>0){
			ind=sequenciaAS[seq].INJGtemperatura;
			tempiniG=ASGasInj.temperatura[ind];
			if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp Injecao = " << ind <<" Temp Injecao= "<<ASGasInj.temperatura[ind]<< "\n";
		}
		if(ASGasInj.parserieVazGas>0){
			ind=sequenciaAS[seq].INJGvazgas;
			vazgasG=ASGasInj.vazgas[ind];
			if(imprime==1)escreveIni <<seq<<" : "<< " indice Vaz Injecao = " << ind <<" Vaz Injecao= "<<ASGasInj.vazgas[ind]<< "\n";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].PEpres;
	  		presE=ASpEntrada.pres[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Contorno = " << ind <<" Pres. Contorno= "<<presE<< "\n";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].PEbet;
	  		betaE=ASpEntrada.bet[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Beta Contorno = " << ind <<" Beta Contorno= "<<betaE<< "\n";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].PEtemperatura;
	  		tempE=ASpEntrada.temperatura[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp. Contorno = " << ind <<" Temp. Contorno= "<<tempE<< "\n";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		ind=sequenciaAS[seq].PEtit;
	  		titE=ASpEntrada.tit[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Titulo Contorno = " << ind <<" Titulo Contorno= "<<titE<< "\n";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].VPEpres;
	  		presE=ASvpEntrada.pres[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Contorno = " << ind <<" Pres. Contorno= "<<presE<< "\n";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].VPEbet;
	  		betaE=ASvpEntrada.bet[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Beta Contorno = " << ind <<" Beta Contorno= "<<betaE<< "\n";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		ind=sequenciaAS[seq].VPEmass;
	  		vazE=ASvpEntrada.mass[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Vazao Contorno = " << ind <<" Vazao Contorno= "<<vazE<< "\n";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].VPEtemperatura;
	  		tempE=ASvpEntrada.temperatura[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp. Contorno = " << ind <<" Temp. Contorno= "<<tempE<< "\n";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		ind=sequenciaAS[seq].CHKabertura;
	  		indCHK=ind;
	  		chokeSup.AreaGarg=ASCHK.abertura[ind]*chokeSup.AreaTub;
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Abertura Choke = " << ind <<" Abertura Choke= "<<ASCHK.abertura[ind]<< "\n";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		ind=sequenciaAS[seq].CHKcd;
	  		chokeSup.cdchk=ASCHK.cd[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice CD Choke = " << ind <<" CD Choke= "<<chokeSup.cdchk<< "\n";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		ind=sequenciaAS[seq].PIpresinj;
	  		presE=ASPInj.presfundo[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Jusante = " << ind <<" Pres Jusante= "<<presE<< "\n";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		ind=sequenciaAS[seq].PIpresfundo;
	  		pGSup=ASPInj.presinj[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Montante = " << ind <<" Pres Montante= "<<pGSup<< "\n";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		ind=sequenciaAS[seq].PItempinj;
	  		tempE=ASPInj.tempinj[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp. Injecao = " << ind <<" Temp. Injecao= "<<tempE<< "\n";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		ind=sequenciaAS[seq].PIvazinj;
	  		vazE=ASPInj.vazinj[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Vazao Poco Injecao = " << ind <<" Vazao Poco Injecao= "<<vazE<< "\n";
	  	  }
	}

	escreveIni.close();
}

void ASens::selecaoASsemImpre(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
		 double& pGSup,double& temperatura,double& presiniG, double& tempiniG, double& vazgasG,
		 double& presE, double& tempE,double& titE, double& betaE, double& vazE, int seq, int& indCHK,
		 double* vdPdLH, double* vdPdLF, double* vdTdL){

	indCHK=0;
	int ind;
	if(listaV.vpsep==1){
		ind=sequenciaAS[seq].PSEPpres;
		pGSup=ASPsep.pres[ind];
	}
	if(listaV.vRGO==1){
		ind=sequenciaAS[seq].RGO;
		if(flup[0].flashCompleto!=2){
			flup[0].RGO=ASRGO.RGO[ind];
			flup[0].RenovaFluido();
		}
		else atualizaCompRGO(ASRGO.RGO[ind], flup[0]);
		atualizaRGO(celula, ASRGO.RGO[ind]);
	}
	if(listaV.vBSW==1){
		ind=sequenciaAS[seq].BSW;
		flup[0].BSW=ASBSW.BSW[ind];
		flup[0].RenovaFluido();
		atualizaBSW(celula, ASBSW.BSW[ind]);
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FGtemp[konta1];
	    		  celula[icel].acsr.injg.temp=ASFonGas[iFG].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  ind=sequenciaAS[seq].FGvazgas[konta2];
	    		  celula[icel].acsr.injg.QGas=ASFonGas[iFG].vazgas[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  ind=sequenciaAS[seq].FGvazcomp[konta3];
	    		  celula[icel].acsr.injg.razCompGas=ASFonGas[iFG].vazcomp[ind];
	    		  konta3++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  //celula[icel].acsr.bcs.freq=ASBCS[indBCS].freq[ind];
	    		  celula[icel].acsr.bcs.freqnova=ASBCS[indBCS].freq[ind];
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  konta2++;
	    	  }
	      }
	}

	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FLtemp[konta1];
	    		  celula[icel].acsr.injl.temp=ASFonLiq[iFL].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  ind=sequenciaAS[seq].FLvazliq[konta2];
	    		  celula[icel].acsr.injl.QLiq=ASFonLiq[iFL].vazliq[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  ind=sequenciaAS[seq].FLbet[konta3];
	    		  celula[icel].acsr.injl.bet=ASFonLiq[iFL].bet[ind];
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FLindfluP[konta4];
	    		  int indFlu=ASFonLiq[iFL].indfluP[ind];
	    		  celula[icel].acsr.injl.FluidoPro=flup[indFlu];
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  ind=sequenciaAS[seq].IPRtemp[konta1];
	    		  celula[icel].acsr.ipr.Tres=ASIPR[iIPR].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  ind=sequenciaAS[seq].IPRpres[konta2];
	    		  celula[icel].acsr.ipr.Pres=ASIPR[iIPR].pres[ind];
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  ind=sequenciaAS[seq].IPRip[konta3];
	    		  celula[icel].acsr.ipr.ip=ASIPR[iIPR].ip[ind];
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  ind=sequenciaAS[seq].IPRjp[konta4];
	    		  celula[icel].acsr.ipr.ij=ASIPR[iIPR].jp[ind];
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  ind=sequenciaAS[seq].IPRqMax[konta5];
	    		  celula[icel].acsr.ipr.qMax=ASIPR[iIPR].qMax[ind];
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  ind=sequenciaAS[seq].IPRindfluP[konta6];
	    		  int indFlu=ASIPR[iIPR].indfluP[ind];
	    		  celula[icel].acsr.ipr.FluidoPro=flup[indFlu];
	    		  konta6++;
	    	  }
	      }
	}

	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FMtemp[konta1];
	    		  celula[icel].acsr.injm.temp=ASFonMas[iFM].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  ind=sequenciaAS[seq].FMvazMasP[konta2];
	    		  celula[icel].acsr.injm.MassP=ASFonMas[iFM].vazMasP[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  ind=sequenciaAS[seq].FMvazMasC[konta3];
	    		  celula[icel].acsr.injm.MassC=ASFonMas[iFM].vazMasC[ind];
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  ind=sequenciaAS[seq].FMvazMasG[konta4];
	    		  celula[icel].acsr.injm.MassG=ASFonMas[iFM].vazMasG[ind];
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FMindfluP[konta5];
	    		  int indFlu=ASFonMas[iFM].indfluP[ind];
	    		  celula[icel].acsr.injm.FluidoPro=flup[indFlu];
	    		  konta5++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  celula[icel].acsr.delp=ASDP[indBCS].dp[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  ind=sequenciaAS[seq].DPdLH[konta1];
	    		  vdPdLH[icel]=ASdPdLH[indBCS].dPHidro[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  ind=sequenciaAS[seq].DPdLF[konta1];
	    		  vdPdLF[icel]=ASdPdLF[indBCS].dPFric[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  ind=sequenciaAS[seq].DTdL[konta1];
	    		  vdTdL[icel]=ASdTdL[indBCS].dT[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1 || listaV.vdpF==1 || listaV.vdt==1)atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BVOLfreq[konta1];
	    		  celula[icel].acsr.bvol.freq=ASBVOL[indBCS].freq[ind];
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  ind=sequenciaAS[seq].BVOLcapacidade[konta2];
	    		  celula[icel].acsr.bvol.capacidade=ASBVOL[indBCS].capacidade[ind];
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  ind=sequenciaAS[seq].BVOLnpoli[konta3];
	    		  celula[icel].acsr.bvol.npoli=ASBVOL[indBCS].npoli[ind];
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].VALVabertura[konta1];
	    		  celula[icel].acsr.chk.AreaGarg=ASValv[indBCS].abertura[ind]*celula[icel].acsr.chk.AreaTub;
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].VALVcd[konta2];
	    		  celula[icel].acsr.chk.cdchk=ASValv[indBCS].cd[ind];
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].FUROabertura[konta1];
	    		  celula[icel].acsr.fontechk.abertura=ASFuro[indBCS].abertura[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].FUROcd[konta2];
	    		  celula[icel].acsr.fontechk.cd=ASFuro[indBCS].cd[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  ind=sequenciaAS[seq].FURObeta[konta3];
	    		  celula[icel].acsr.fontechk.bet=ASFuro[indBCS].beta[ind];
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  ind=sequenciaAS[seq].FUROpres[konta4];
	    		  celula[icel].acsr.fontechk.pamb=ASFuro[indBCS].pres[ind];
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FUROtemp[konta5];
	    		  celula[icel].acsr.fontechk.tempT=ASFuro[indBCS].temp[ind];
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FUROindFlu[konta6];
	    		  int indFlu=ASFuro[indBCS].indFlu[ind];
	    		  celula[icel].acsr.fontechk.fluidoPamb=flup[indFlu];
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  ind=sequenciaAS[seq].GeomdA[konta1];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  konta1++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,0, ASGeom[iG].dA[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta2];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  konta2++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,1, ASGeom[iG].dB[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta3];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  konta3++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,2, ASGeom[iG].rug[ind]);
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  ind=sequenciaAS[seq].condEqui[konta1];
	    		  int imat=ASCondEquiv[ik].indMat;
	    		  konta1++;
	    		  atualizaMat(ncelG, celula, celulaG, imat, ASCondEquiv[ik].cond[ind]);
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			ind=sequenciaAS[seq].INJGpresinj;
			presiniG=ASGasInj.presinj[ind];
		}
		if(ASGasInj.parserieTemp>0){
			ind=sequenciaAS[seq].INJGtemperatura;
			tempiniG=ASGasInj.temperatura[ind];
		}
		if(ASGasInj.parserieVazGas>0){
			ind=sequenciaAS[seq].INJGvazgas;
			vazgasG=ASGasInj.vazgas[ind];
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].PEpres;
	  		presE=ASpEntrada.pres[ind];
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].PEbet;
	  		betaE=ASpEntrada.bet[ind];
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].PEtemperatura;
	  		tempE=ASpEntrada.temperatura[ind];
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		ind=sequenciaAS[seq].PEtit;
	  		titE=ASpEntrada.tit[ind];
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].VPEpres;
	  		presE=ASvpEntrada.pres[ind];
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].VPEbet;
	  		betaE=ASvpEntrada.bet[ind];
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		ind=sequenciaAS[seq].VPEmass;
	  		vazE=ASvpEntrada.mass[ind];
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].VPEtemperatura;
	  		tempE=ASvpEntrada.temperatura[ind];
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		ind=sequenciaAS[seq].CHKabertura;
	  		indCHK=ind;
	  		chokeSup.AreaGarg=ASCHK.abertura[ind]*chokeSup.AreaTub;
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		ind=sequenciaAS[seq].CHKcd;
	  		chokeSup.cdchk=ASCHK.cd[ind];
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		ind=sequenciaAS[seq].PIpresinj;
	  		presE=ASPInj.presfundo[ind];
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		ind=sequenciaAS[seq].PIpresfundo;
	  		pGSup=ASPInj.presinj[ind];
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		ind=sequenciaAS[seq].PItempinj;
	  		tempE=ASPInj.tempinj[ind];
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		ind=sequenciaAS[seq].PIvazinj;
	  		vazE=ASPInj.vazinj[ind];
	  	  }
	}

}


void ASens::imprimeVarInteresseASImex(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq, int seq){

	ostringstream saidaP;
	saidaP << "variaveisInteresseAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::app);

	int ind;
	if(listaV.vRGO==1){
		ind=sequenciaAS[seq].RGO;
		escreveIni <<"   "<<ASRGO.RGO[ind]<< " ; ";
	}
	if(listaV.vBSW==1){
		ind=sequenciaAS[seq].BSW;
		flup[0].BSW=ASBSW.BSW[ind];
		flup[0].RenovaFluido();
		atualizaBSW(celula, ASBSW.BSW[ind]);
		escreveIni <<"   "<<ASBSW.BSW[ind]<< " ; ";
	}
	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FLtemp[konta1];
	    		  escreveIni <<"  " << ASFonLiq[iFL].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  ind=sequenciaAS[seq].FLvazliq[konta2];
	    		  escreveIni <<"   " <<ASFonLiq[iFL].vazliq[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  ind=sequenciaAS[seq].FLbet[konta3];
	    		  escreveIni <<"   " <<ASFonLiq[iFL].bet[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FLindfluP[konta4];
	    		  int indFlu=ASFonLiq[iFL].indfluP[ind];
	    		  escreveIni <<"   " << indFlu<< " ; ";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FGtemp[konta1];
	    		  escreveIni <<"   " << ASFonGas[iFG].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  ind=sequenciaAS[seq].FGvazgas[konta2];
	    		  escreveIni <<"   " <<ASFonGas[iFG].vazgas[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  ind=sequenciaAS[seq].FGvazcomp[konta3];
	    		  escreveIni <<"   " <<ASFonGas[iFG].vazcomp[ind]<< " ; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vpsep==1){
		ind=sequenciaAS[seq].PSEPpres;
		escreveIni <<"   "<<ASPsep.pres[ind]<<" ; ";
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  ind=sequenciaAS[seq].IPRtemp[konta1];
	    		  escreveIni <<"   " << ASIPR[iIPR].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  ind=sequenciaAS[seq].IPRpres[konta2];
	    		  escreveIni <<"   " <<ASIPR[iIPR].pres[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  ind=sequenciaAS[seq].IPRip[konta3];
	    		  escreveIni <<seq<<"   " << ASIPR[iIPR].ip[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  ind=sequenciaAS[seq].IPRjp[konta4];
	    		  escreveIni <<"   " << ASIPR[iIPR].jp[ind]<< " ; ";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  ind=sequenciaAS[seq].IPRqMax[konta5];
	    		  escreveIni <<"   " << ASIPR[iIPR].qMax[ind]<< " ; ";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  ind=sequenciaAS[seq].IPRindfluP[konta6];
	    		  escreveIni <<"   " << ASIPR[iIPR].indfluP[ind]<< " ; ";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FMtemp[konta1];
	    		  escreveIni <<"   " << ASFonMas[iFM].temp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  ind=sequenciaAS[seq].FMvazMasP[konta2];
	    		  escreveIni <<"   "<<ASFonMas[iFM].vazMasP[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  ind=sequenciaAS[seq].FMvazMasC[konta3];
	    		  escreveIni <<"   " <<ASFonMas[iFM].vazMasC[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  ind=sequenciaAS[seq].FMvazMasG[konta4];
				  escreveIni <<"   " << ASFonMas[iFM].vazMasG[ind]<< " ; ";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FMindfluP[konta5];
	    		  escreveIni <<"   " << ASFonMas[iFM].indfluP[ind]<< " ; ";
	    		  konta5++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  escreveIni <<"   " << ASBCS[indBCS].freq[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  escreveIni <<seq<<" : " << " indice BCS = " << konta2<<" celula Fonte "<< icel<<
					" indice N. Estagios= "<<ind<<" N. Estagios= "<<ASBCS[indBCS].nestag[ind]<< "\n";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  escreveIni <<"   " << ASDP[indBCS].dp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  escreveIni <<"   " << ASDP[indBCS].dp[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  ind=sequenciaAS[seq].DPdLF[konta1];
	    		  escreveIni <<"   " <<ASdPdLF[indBCS].dPFric[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  ind=sequenciaAS[seq].DTdL[konta1];
	    		  escreveIni <<"   " <<ASdTdL[indBCS].dT[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BVOLfreq[konta1];
	    		  escreveIni <<"   " <<ASBVOL[indBCS].freq[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  ind=sequenciaAS[seq].BVOLcapacidade[konta2];
	    		  escreveIni <<"   " << ASBVOL[indBCS].capacidade[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  ind=sequenciaAS[seq].BVOLnpoli[konta3];
	    		  escreveIni <<"   " <<ASBVOL[indBCS].npoli[ind]<< " ; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].VALVabertura[konta1];
	    		  escreveIni << "   " << ASValv[indBCS].abertura[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].VALVcd[konta2];
	    		  escreveIni <<" : " << ASValv[indBCS].cd[ind]<< " ; ";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].FUROabertura[konta1];
	    		  escreveIni <<"   " << ASFuro[indBCS].abertura[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].FUROcd[konta2];
	    		  escreveIni <<"   " << ASFuro[indBCS].cd[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  ind=sequenciaAS[seq].FURObeta[konta3];
	    		  escreveIni <<"   " <<ASFuro[indBCS].beta[ind]<< " ; ";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  ind=sequenciaAS[seq].FUROpres[konta4];
	    		  escreveIni <<"   " << ASFuro[indBCS].pres[ind]<< " ; ";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FUROtemp[konta5];
	    		  escreveIni <<"   " << " Temperatura= "<<ASFuro[indBCS].temp[ind]<< " ; ";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FUROindFlu[konta6];
	    		  int indFlu=ASFuro[indBCS].indFlu[ind];
	    		  escreveIni <<"   " <<indFlu<< " ; ";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  ind=sequenciaAS[seq].GeomdA[konta1];
	    		  escreveIni <<"   " << ASGeom[iG].dA[ind]<< " ; ";
	    		  konta1++;
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta2];
	    		  escreveIni <<"   " <<ASGeom[iG].dB[ind]<< " ; ";
	    		  konta2++;
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta3];
	    		  escreveIni <<"   " << ASGeom[iG].rug[ind]<< " ; ";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  ind=sequenciaAS[seq].condEqui[konta1];
	    		  escreveIni <<"   " << ASCondEquiv[ik].cond[ind]<< " ; ";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			ind=sequenciaAS[seq].INJGpresinj;
			escreveIni <<"   "<< ASGasInj.presinj[ind]<< " ; ";
		}
		if(ASGasInj.parserieTemp>0){
			ind=sequenciaAS[seq].INJGtemperatura;
			escreveIni <<"   "<< ASGasInj.temperatura[ind]<< " ; ";
		}
		if(ASGasInj.parserieVazGas>0){
			ind=sequenciaAS[seq].INJGvazgas;
			escreveIni <<"   "<< ASGasInj.vazgas[ind]<< " ; ";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].PEpres;
	  		escreveIni <<"   "<< ASpEntrada.pres[ind]<< " ; ";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].PEbet;
	  		escreveIni <<"   "<<ASpEntrada.bet[ind]<< " ; ";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].PEtemperatura;
	  		escreveIni <<"   "<< ASpEntrada.temperatura[ind]<< " ; ";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		ind=sequenciaAS[seq].PEtit;
	  		escreveIni <<"   "<< ASpEntrada.tit[ind]<< " ; ";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].VPEpres;
	  		escreveIni <<"   "<< ASvpEntrada.pres[ind]<< " ; ";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].VPEbet;
	  		escreveIni <<"   "<<ASvpEntrada.bet[ind]<< " ; ";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		ind=sequenciaAS[seq].VPEmass;
	  		escreveIni <<"   "<<ASvpEntrada.mass[ind]<< " ; ";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].VPEtemperatura;
	  		escreveIni <<"   "<<ASvpEntrada.temperatura[ind]<< " ; ";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		ind=sequenciaAS[seq].CHKabertura;
	  		escreveIni <<"   "<<ASCHK.abertura[ind]<< " ; ";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		ind=sequenciaAS[seq].CHKcd;
	  		escreveIni <<"   "<<chokeSup.cdchk<< " ; ";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		ind=sequenciaAS[seq].PIpresinj;
	  		escreveIni <<" : "<< ASPInj.presfundo[ind]<< " ; ";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		ind=sequenciaAS[seq].PIpresfundo;
	  		escreveIni <<" : "<<ASPInj.presinj[ind]<< " ; ";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		ind=sequenciaAS[seq].PItempinj;
	  		escreveIni <<" : "<< ASPInj.tempinj[ind]<< " ; ";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		ind=sequenciaAS[seq].PIvazinj;
	  		escreveIni <<" : "<< ASPInj.vazinj[ind]<< " ; ";
	  	  }
	}
	escreveIni << endl;
	escreveIni.close();
}


void ASens::selecaoASImex(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
		 double& pGSup,double& temperatura,double& presiniG, double& tempiniG, double& vazgasG,
		 double& presE, double& tempE,double& titE, double& betaE, double& vazE, int seq, int& indCHK,
		 double* vdPdLH, double* vdPdLF, double* vdTdL,int imprime){

	ostringstream saidaP;
	saidaP << "relacaoAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::app);

	indCHK=0;
	int ind;
	if(listaV.vRGO==1){
		ind=sequenciaAS[seq].RGO;
		if(flup[0].flashCompleto!=2){
			flup[0].RGO=ASRGO.RGO[ind];
			flup[0].RenovaFluido();
		}
		else atualizaCompRGO(ASRGO.RGO[ind], flup[0]);
		atualizaRGO(celula, ASRGO.RGO[ind]);
		if(imprime==1)escreveIni <<seq<<" : "<< " indice RGO = " << ind <<" RGO fluido 0= "<<flup[0].RGO<< "\n";
	}
	if(listaV.vBSW==1){
		ind=sequenciaAS[seq].BSW;
		flup[0].BSW=ASBSW.BSW[ind];
		flup[0].RenovaFluido();
		atualizaBSW(celula, ASBSW.BSW[ind]);
		if(imprime==1)escreveIni <<seq<<" : "<< " indice BSW = " << ind <<" BSW fluido 0= "<<flup[0].BSW<< "\n";
	}
	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FLtemp[konta1];
	    		  celula[icel].acsr.injl.temp=ASFonLiq[iFL].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASFonLiq[iFL].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  ind=sequenciaAS[seq].FLvazliq[konta2];
	    		  celula[icel].acsr.injl.QLiq=ASFonLiq[iFL].vazliq[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta2<<" celula Fonte "<< icel<<
					" indice Vazao Liq = "<<ind<<" Vazao de Liquido= "<<ASFonLiq[iFL].vazliq[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  ind=sequenciaAS[seq].FLbet[konta3];
	    		  celula[icel].acsr.injl.bet=ASFonLiq[iFL].bet[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta3<<" celula Fonte "<< icel<<
					" indice Beta = "<<ind<<" Beta= "<<ASFonLiq[iFL].bet[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FLindfluP[konta4];
	    		  int indFlu=ASFonLiq[iFL].indfluP[ind];
	    		  celula[icel].acsr.injl.FluidoPro=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Liquido = " << konta4<<" celula Fonte "<< icel<<
					" indice Fluido = "<<ind<<" Fluido= "<<indFlu<< "\n";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FGtemp[konta1];
	    		  celula[icel].acsr.injg.temp=ASFonGas[iFG].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Gas = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASFonGas[iFG].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  ind=sequenciaAS[seq].FGvazgas[konta2];
	    		  celula[icel].acsr.injg.QGas=ASFonGas[iFG].vazgas[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Gas = " << konta2<<" celula Fonte "<< icel<<
					" indice Vazao Gas = "<<ind<<" Vazao Gas= "<<ASFonGas[iFG].vazgas[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  ind=sequenciaAS[seq].FGvazcomp[konta3];
	    		  celula[icel].acsr.injg.razCompGas=ASFonGas[iFG].vazcomp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Gas = " << konta3<<" celula Fonte "<< icel<<
					" indice Vazao Liq Complementar = "<<ind<<" Vazao Liq Complementar= "<<ASFonGas[iFG].vazcomp[ind]<< "\n";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vpsep==1){
		ind=sequenciaAS[seq].PSEPpres;
		pGSup=ASPsep.pres[ind];
		if(imprime==1)escreveIni <<seq<<" : "<< " indice PSep = " << ind <<" Pressao Separador= "<<pGSup<< "\n";
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  ind=sequenciaAS[seq].IPRtemp[konta1];
	    		  celula[icel].acsr.ipr.Tres=ASIPR[iIPR].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASIPR[iIPR].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  ind=sequenciaAS[seq].IPRpres[konta2];
	    		  celula[icel].acsr.ipr.Pres=ASIPR[iIPR].pres[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta2<<" celula Fonte "<< icel<<
					" indice P Estatica = "<<ind<<" P Estatica= "<<ASIPR[iIPR].pres[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  ind=sequenciaAS[seq].IPRip[konta3];
	    		  celula[icel].acsr.ipr.ip=ASIPR[iIPR].ip[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta3<<" celula Fonte "<< icel<<
					" indice IP = "<<ind<<" IP= "<<ASIPR[iIPR].ip[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  ind=sequenciaAS[seq].IPRjp[konta4];
	    		  celula[icel].acsr.ipr.ij=ASIPR[iIPR].jp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta4<<" celula Fonte "<< icel<<
					" indice II = "<<ind<<" II= "<<ASIPR[iIPR].jp[ind]<< "\n";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  ind=sequenciaAS[seq].IPRqMax[konta5];
	    		  celula[icel].acsr.ipr.qMax=ASIPR[iIPR].qMax[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta5<<" celula Fonte "<< icel<<
					" indice qMax = "<<ind<<" qMax= "<<ASIPR[iIPR].qMax[ind]<< "\n";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  ind=sequenciaAS[seq].IPRindfluP[konta6];
	    		  int indFlu=ASIPR[iIPR].indfluP[ind];
	    		  celula[icel].acsr.ipr.FluidoPro=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice IPR = " << konta6<<" celula Fonte "<< icel<<
					" indice Fluido = "<<ind<<" fluido= "<<ASIPR[iIPR].indfluP[ind]<< "\n";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FMtemp[konta1];
	    		  celula[icel].acsr.injm.temp=ASFonMas[iFM].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta1<<" celula Fonte "<< icel<<
					" indice Temperatura = "<<ind<<" Temperatura= "<<ASFonMas[iFM].temp[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  ind=sequenciaAS[seq].FMvazMasP[konta2];
	    		  celula[icel].acsr.injm.MassP=ASFonMas[iFM].vazMasP[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta2<<" celula Fonte "<< icel<<
					" indice Vazao Liquido= "<<ind<<" Vazao Liquido= "<<ASFonMas[iFM].vazMasP[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  ind=sequenciaAS[seq].FMvazMasC[konta3];
	    		  celula[icel].acsr.injm.MassC=ASFonMas[iFM].vazMasC[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta3<<" celula Fonte "<< icel<<
					" indice Vazao Liquido Complementar= "<<ind<<" Vazao Liquido Complementar= "<<ASFonMas[iFM].vazMasC[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  ind=sequenciaAS[seq].FMvazMasG[konta4];
	    		  celula[icel].acsr.injm.MassG=ASFonMas[iFM].vazMasG[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta4<<" celula Fonte "<< icel<<
					" indice Vazao Gas= "<<ind<<" Vazao Gas= "<<ASFonMas[iFM].vazMasG[ind]<< "\n";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FMindfluP[konta5];
	    		  int indFlu=ASFonMas[iFM].indfluP[ind];
	    		  celula[icel].acsr.injm.FluidoPro=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Fonte Massa = " << konta5<<" celula Fonte "<< icel<<
					" indice Fluido= "<<ind<<" Fluido= "<<ASFonMas[iFM].indfluP[ind]<< "\n";
	    		  konta5++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  //celula[icel].acsr.bcs.freq=ASBCS[indBCS].freq[ind];
	    		  celula[icel].acsr.bcs.freqnova=ASBCS[indBCS].freq[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice BCS = " << konta1<<" celula Fonte "<< icel<<
					" indice Frequencia= "<<ind<<" Frequencia= "<<ASBCS[indBCS].freq[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice BCS = " << konta2<<" celula Fonte "<< icel<<
					" indice N. Estagios= "<<ind<<" N. Estagios= "<<ASBCS[indBCS].nestag[ind]<< "\n";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  celula[icel].acsr.delp=ASDP[indBCS].dp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Del. Pres, = " << konta1<<" celula Fonte "<< icel<<
					" indice Del. Pres.= "<<ind<<" Del. Pres.= "<<ASDP[indBCS].dp[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  ind=sequenciaAS[seq].DPdLH[konta1];
	    		  vdPdLH[icel]=ASdPdLH[indBCS].dPHidro[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Corr. Pres. Hidro, = " << konta1<<" indice da colecao de fatores de correcao "<< icel<<
					" indice na AS.= "<<ind<<" Corr. Pres. Hidro= "<<ASdPdLH[indBCS].dPHidro[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  ind=sequenciaAS[seq].DPdLF[konta1];
	    		  vdPdLF[icel]=ASdPdLF[indBCS].dPFric[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Corr. Pres. Fric., = " << konta1<<" indice da colecao de fatores de correcao "<< icel<<
					" indice na AS.= "<<ind<<" Corr. Pres. Fric.= "<<ASdPdLF[indBCS].dPFric[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  ind=sequenciaAS[seq].DTdL[konta1];
	    		  vdTdL[icel]=ASdTdL[indBCS].dT[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Corr. Temperatura, = " << konta1<<" indice da colecao de fatores de correcao "<< icel<<
					" indice na AS.= "<<ind<<" Corr. Temperatura= "<<ASdTdL[indBCS].dT[ind]<< "\n";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1 || listaV.vdpF==1 || listaV.vdt==1)atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BVOLfreq[konta1];
	    		  celula[icel].acsr.bvol.freq=ASBVOL[indBCS].freq[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Bomba Vol., = " << konta1<<" celula Fonte "<< icel<<
					" indice Frequencia= "<<ind<<" Frequencia= "<<ASBVOL[indBCS].freq[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  ind=sequenciaAS[seq].BVOLcapacidade[konta2];
	    		  celula[icel].acsr.bvol.capacidade=ASBVOL[indBCS].capacidade[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Bomba Vol., = " << konta2<<" celula Fonte "<< icel<<
					" indice Capacidade= "<<ind<<" Capacidade= "<<ASBVOL[indBCS].capacidade[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  ind=sequenciaAS[seq].BVOLnpoli[konta3];
	    		  celula[icel].acsr.bvol.npoli=ASBVOL[indBCS].npoli[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Bomba Vol., = " << konta3<<" celula Fonte "<< icel<<
					" indice Fator Politropico= "<<ind<<" Fator Politropico= "<<ASBVOL[indBCS].npoli[ind]<< "\n";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].VALVabertura[konta1];
	    		  celula[icel].acsr.chk.AreaGarg=ASValv[indBCS].abertura[ind]*celula[icel].acsr.chk.AreaTub;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Valvula, = " << konta1<<" celula Fonte "<< icel<<
					" indice Abertura= "<<ind<<" Abertura= "<<ASValv[indBCS].abertura[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].VALVcd[konta2];
	    		  celula[icel].acsr.chk.cdchk=ASValv[indBCS].cd[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Valvula, = " << konta2<<" celula Fonte "<< icel<<
					" indice CD= "<<ind<<" CD= "<<ASValv[indBCS].cd[ind]<< "\n";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].FUROabertura[konta1];
	    		  celula[icel].acsr.fontechk.abertura=ASFuro[indBCS].abertura[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta1<<" celula Fonte "<< icel<<
					" indice Abertura= "<<ind<<" Abertura= "<<ASFuro[indBCS].abertura[ind]<< "\n";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].FUROcd[konta2];
	    		  celula[icel].acsr.fontechk.cd=ASFuro[indBCS].cd[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta2<<" celula Fonte "<< icel<<
					" indice CD= "<<ind<<" CD= "<<ASFuro[indBCS].cd[ind]<< "\n";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  ind=sequenciaAS[seq].FURObeta[konta3];
	    		  celula[icel].acsr.fontechk.bet=ASFuro[indBCS].beta[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta3<<" celula Fonte "<< icel<<
					" indice Beta= "<<ind<<" Beta= "<<ASFuro[indBCS].beta[ind]<< "\n";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  ind=sequenciaAS[seq].FUROpres[konta4];
	    		  celula[icel].acsr.fontechk.pamb=ASFuro[indBCS].pres[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta4<<" celula Fonte "<< icel<<
					" indice Pressao= "<<ind<<" Pressao= "<<ASFuro[indBCS].pres[ind]<< "\n";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FUROtemp[konta5];
	    		  celula[icel].acsr.fontechk.tempT=ASFuro[indBCS].temp[ind];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta5<<" celula Fonte "<< icel<<
					" indice Temperatura= "<<ind<<" Temperatura= "<<ASFuro[indBCS].temp[ind]<< "\n";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FUROindFlu[konta6];
	    		  int indFlu=ASFuro[indBCS].indFlu[ind];
	    		  celula[icel].acsr.fontechk.fluidoPamb=flup[indFlu];
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Vazamento = " << konta6<<" celula Fonte "<< icel<<
					" indice Fluido= "<<ind<<" Fluido= "<<indFlu<< "\n";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  ind=sequenciaAS[seq].GeomdA[konta1];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Secao Transversal = " << konta1<<" secao transversal "<< igeo<<
					" indice Maior Diametro = "<<ind<<" Maior Diam Interno= "<<ASGeom[iG].dA[ind]<< "\n";
	    		  konta1++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,0, ASGeom[iG].dA[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta2];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Secao Transversal = " << konta2<<" secao transversal "<< igeo<<
					" indice Menor Diametro = "<<ind<<" Menor Diam Interno= "<<ASGeom[iG].dB[ind]<< "\n";
	    		  konta2++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,1, ASGeom[iG].dB[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta3];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Secao Transversal = " << konta3<<" secao transversal "<< igeo<<
					" indice Rugosidade = "<<ind<<" Rugosidade= "<<ASGeom[iG].rug[ind]<< "\n";
	    		  konta3++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,2, ASGeom[iG].rug[ind]);
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  ind=sequenciaAS[seq].condEqui[konta1];
	    		  int imat=ASCondEquiv[ik].indMat;
	    		  if(imprime==1)escreveIni <<seq<<" : " << " indice Material = " << konta1<<" material "<< imat<<
					" indice Condutividade = "<<ind<<" Condutividade= "<<ASCondEquiv[ik].cond[ind]<< "\n";
	    		  konta1++;
	    		  atualizaMat(ncelG, celula, celulaG, imat, ASCondEquiv[ik].cond[ind]);
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			ind=sequenciaAS[seq].INJGpresinj;
			presiniG=ASGasInj.presinj[ind];
			if(imprime==1)escreveIni <<seq<<" : "<< " indice Pressao Injecao = " << ind <<" Pressao Injecao= "<<ASGasInj.presinj[ind]<< "\n";
		}
		if(ASGasInj.parserieTemp>0){
			ind=sequenciaAS[seq].INJGtemperatura;
			tempiniG=ASGasInj.temperatura[ind];
			if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp Injecao = " << ind <<" Temp Injecao= "<<ASGasInj.temperatura[ind]<< "\n";
		}
		if(ASGasInj.parserieVazGas>0){
			ind=sequenciaAS[seq].INJGvazgas;
			vazgasG=ASGasInj.vazgas[ind];
			if(imprime==1)escreveIni <<seq<<" : "<< " indice Vaz Injecao = " << ind <<" Vaz Injecao= "<<ASGasInj.vazgas[ind]<< "\n";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].PEpres;
	  		presE=ASpEntrada.pres[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Contorno = " << ind <<" Pres. Contorno= "<<presE<< "\n";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].PEbet;
	  		betaE=ASpEntrada.bet[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Beta Contorno = " << ind <<" Beta Contorno= "<<betaE<< "\n";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].PEtemperatura;
	  		tempE=ASpEntrada.temperatura[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp. Contorno = " << ind <<" Temp. Contorno= "<<tempE<< "\n";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		ind=sequenciaAS[seq].PEtit;
	  		titE=ASpEntrada.tit[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Titulo Contorno = " << ind <<" Titulo Contorno= "<<titE<< "\n";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].VPEpres;
	  		presE=ASvpEntrada.pres[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Contorno = " << ind <<" Pres. Contorno= "<<presE<< "\n";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].VPEbet;
	  		betaE=ASvpEntrada.bet[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Beta Contorno = " << ind <<" Beta Contorno= "<<betaE<< "\n";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		ind=sequenciaAS[seq].VPEmass;
	  		vazE=ASvpEntrada.mass[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Vazao Contorno = " << ind <<" Vazao Contorno= "<<vazE<< "\n";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].VPEtemperatura;
	  		tempE=ASvpEntrada.temperatura[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp. Contorno = " << ind <<" Temp. Contorno= "<<tempE<< "\n";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		ind=sequenciaAS[seq].CHKabertura;
	  		indCHK=ind;
	  		chokeSup.AreaGarg=ASCHK.abertura[ind]*chokeSup.AreaTub;
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Abertura Choke = " << ind <<" Abertura Choke= "<<ASCHK.abertura[ind]<< "\n";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		ind=sequenciaAS[seq].CHKcd;
	  		chokeSup.cdchk=ASCHK.cd[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice CD Choke = " << ind <<" CD Choke= "<<chokeSup.cdchk<< "\n";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		ind=sequenciaAS[seq].PIpresinj;
	  		presE=ASPInj.presfundo[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Jusante = " << ind <<" Pres Jusante= "<<presE<< "\n";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		ind=sequenciaAS[seq].PIpresfundo;
	  		pGSup=ASPInj.presinj[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Pres. Montante = " << ind <<" Pres Montante= "<<pGSup<< "\n";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		ind=sequenciaAS[seq].PItempinj;
	  		tempE=ASPInj.tempinj[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Temp. Injecao = " << ind <<" Temp. Injecao= "<<tempE<< "\n";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		ind=sequenciaAS[seq].PIvazinj;
	  		vazE=ASPInj.vazinj[ind];
	  		if(imprime==1)escreveIni <<seq<<" : "<< " indice Vazao Poco Injecao = " << ind <<" Vazao Poco Injecao= "<<vazE<< "\n";
	  	  }
	}

	escreveIni.close();
}

void ASens::selecaoASImexsemImpre(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
		 double& pGSup,double& temperatura,double& presiniG, double& tempiniG, double& vazgasG,
		 double& presE, double& tempE,double& titE, double& betaE, double& vazE, int seq, int& indCHK,
		 double* vdPdLH, double* vdPdLF, double* vdTdL){

	indCHK=0;
	int ind;
	if(listaV.vRGO==1){
		ind=sequenciaAS[seq].RGO;
		if(flup[0].flashCompleto!=2){
			flup[0].RGO=ASRGO.RGO[ind];
			flup[0].RenovaFluido();
		}
		else atualizaCompRGO(ASRGO.RGO[ind], flup[0]);
		atualizaRGO(celula, ASRGO.RGO[ind]);
	}
	if(listaV.vBSW==1){
		ind=sequenciaAS[seq].BSW;
		flup[0].BSW=ASBSW.BSW[ind];
		flup[0].RenovaFluido();
		atualizaBSW(celula, ASBSW.BSW[ind]);
	}
	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FLtemp[konta1];
	    		  celula[icel].acsr.injl.temp=ASFonLiq[iFL].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  ind=sequenciaAS[seq].FLvazliq[konta2];
	    		  celula[icel].acsr.injl.QLiq=ASFonLiq[iFL].vazliq[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  ind=sequenciaAS[seq].FLbet[konta3];
	    		  celula[icel].acsr.injl.bet=ASFonLiq[iFL].bet[ind];
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FLindfluP[konta4];
	    		  int indFlu=ASFonLiq[iFL].indfluP[ind];
	    		  celula[icel].acsr.injl.FluidoPro=flup[indFlu];
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FGtemp[konta1];
	    		  celula[icel].acsr.injg.temp=ASFonGas[iFG].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  ind=sequenciaAS[seq].FGvazgas[konta2];
	    		  celula[icel].acsr.injg.QGas=ASFonGas[iFG].vazgas[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  ind=sequenciaAS[seq].FGvazcomp[konta3];
	    		  celula[icel].acsr.injg.razCompGas=ASFonGas[iFG].vazcomp[ind];
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vpsep==1){
		ind=sequenciaAS[seq].PSEPpres;
		pGSup=ASPsep.pres[ind];
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  ind=sequenciaAS[seq].IPRtemp[konta1];
	    		  celula[icel].acsr.ipr.Tres=ASIPR[iIPR].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  ind=sequenciaAS[seq].IPRpres[konta2];
	    		  celula[icel].acsr.ipr.Pres=ASIPR[iIPR].pres[ind];
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  ind=sequenciaAS[seq].IPRip[konta3];
	    		  celula[icel].acsr.ipr.ip=ASIPR[iIPR].ip[ind];
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  ind=sequenciaAS[seq].IPRjp[konta4];
	    		  celula[icel].acsr.ipr.ij=ASIPR[iIPR].jp[ind];
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  ind=sequenciaAS[seq].IPRqMax[konta5];
	    		  celula[icel].acsr.ipr.qMax=ASIPR[iIPR].qMax[ind];
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  ind=sequenciaAS[seq].IPRindfluP[konta6];
	    		  int indFlu=ASIPR[iIPR].indfluP[ind];
	    		  celula[icel].acsr.ipr.FluidoPro=flup[indFlu];
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FMtemp[konta1];
	    		  celula[icel].acsr.injm.temp=ASFonMas[iFM].temp[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  ind=sequenciaAS[seq].FMvazMasP[konta2];
	    		  celula[icel].acsr.injm.MassP=ASFonMas[iFM].vazMasP[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  ind=sequenciaAS[seq].FMvazMasC[konta3];
	    		  celula[icel].acsr.injm.MassC=ASFonMas[iFM].vazMasC[ind];
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  ind=sequenciaAS[seq].FMvazMasG[konta4];
	    		  celula[icel].acsr.injm.MassG=ASFonMas[iFM].vazMasG[ind];
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FMindfluP[konta5];
	    		  int indFlu=ASFonMas[iFM].indfluP[ind];
	    		  celula[icel].acsr.injm.FluidoPro=flup[indFlu];
	    		  konta5++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  //celula[icel].acsr.bcs.freq=ASBCS[indBCS].freq[ind];
	    		  celula[icel].acsr.bcs.freqnova=ASBCS[indBCS].freq[ind];
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  celula[icel].acsr.delp=ASDP[indBCS].dp[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  ind=sequenciaAS[seq].DPdLH[konta1];
	    		  vdPdLH[icel]=ASdPdLH[indBCS].dPHidro[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  ind=sequenciaAS[seq].DPdLF[konta1];
	    		  vdPdLF[icel]=ASdPdLF[indBCS].dPFric[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  ind=sequenciaAS[seq].DTdL[konta1];
	    		  vdTdL[icel]=ASdTdL[indBCS].dT[ind];
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1 || listaV.vdpF==1 || listaV.vdt==1)atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BVOLfreq[konta1];
	    		  celula[icel].acsr.bvol.freq=ASBVOL[indBCS].freq[ind];
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  ind=sequenciaAS[seq].BVOLcapacidade[konta2];
	    		  celula[icel].acsr.bvol.capacidade=ASBVOL[indBCS].capacidade[ind];
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  ind=sequenciaAS[seq].BVOLnpoli[konta3];
	    		  celula[icel].acsr.bvol.npoli=ASBVOL[indBCS].npoli[ind];
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].VALVabertura[konta1];
	    		  celula[icel].acsr.chk.AreaGarg=ASValv[indBCS].abertura[ind]*celula[icel].acsr.chk.AreaTub;
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].VALVcd[konta2];
	    		  celula[icel].acsr.chk.cdchk=ASValv[indBCS].cd[ind];
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].FUROabertura[konta1];
	    		  celula[icel].acsr.fontechk.abertura=ASFuro[indBCS].abertura[ind];
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].FUROcd[konta2];
	    		  celula[icel].acsr.fontechk.cd=ASFuro[indBCS].cd[ind];
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  ind=sequenciaAS[seq].FURObeta[konta3];
	    		  celula[icel].acsr.fontechk.bet=ASFuro[indBCS].beta[ind];
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  ind=sequenciaAS[seq].FUROpres[konta4];
	    		  celula[icel].acsr.fontechk.pamb=ASFuro[indBCS].pres[ind];
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FUROtemp[konta5];
	    		  celula[icel].acsr.fontechk.tempT=ASFuro[indBCS].temp[ind];
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FUROindFlu[konta6];
	    		  int indFlu=ASFuro[indBCS].indFlu[ind];
	    		  celula[icel].acsr.fontechk.fluidoPamb=flup[indFlu];
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  ind=sequenciaAS[seq].GeomdA[konta1];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  konta1++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,0, ASGeom[iG].dA[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta2];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  konta2++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,1, ASGeom[iG].dB[ind]);
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta3];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  konta3++;
	    		  atualizaGeom(ncelG, celula, celulaG, igeo,2, ASGeom[iG].rug[ind]);
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  ind=sequenciaAS[seq].condEqui[konta1];
	    		  int imat=ASCondEquiv[ik].indMat;
	    		  konta1++;
	    		  atualizaMat(ncelG, celula, celulaG, imat, ASCondEquiv[ik].cond[ind]);
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			ind=sequenciaAS[seq].INJGpresinj;
			presiniG=ASGasInj.presinj[ind];
		}
		if(ASGasInj.parserieTemp>0){
			ind=sequenciaAS[seq].INJGtemperatura;
			tempiniG=ASGasInj.temperatura[ind];
		}
		if(ASGasInj.parserieVazGas>0){
			ind=sequenciaAS[seq].INJGvazgas;
			vazgasG=ASGasInj.vazgas[ind];
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].PEpres;
	  		presE=ASpEntrada.pres[ind];
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].PEbet;
	  		betaE=ASpEntrada.bet[ind];
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].PEtemperatura;
	  		tempE=ASpEntrada.temperatura[ind];
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		ind=sequenciaAS[seq].PEtit;
	  		titE=ASpEntrada.tit[ind];
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].VPEpres;
	  		presE=ASvpEntrada.pres[ind];
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].VPEbet;
	  		betaE=ASvpEntrada.bet[ind];
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		ind=sequenciaAS[seq].VPEmass;
	  		vazE=ASvpEntrada.mass[ind];
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].VPEtemperatura;
	  		tempE=ASvpEntrada.temperatura[ind];
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		ind=sequenciaAS[seq].CHKabertura;
	  		indCHK=ind;
	  		chokeSup.AreaGarg=ASCHK.abertura[ind]*chokeSup.AreaTub;
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		ind=sequenciaAS[seq].CHKcd;
	  		chokeSup.cdchk=ASCHK.cd[ind];
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		ind=sequenciaAS[seq].PIpresinj;
	  		presE=ASPInj.presfundo[ind];
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		ind=sequenciaAS[seq].PIpresfundo;
	  		pGSup=ASPInj.presinj[ind];
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		ind=sequenciaAS[seq].PItempinj;
	  		tempE=ASPInj.tempinj[ind];
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		ind=sequenciaAS[seq].PIvazinj;
	  		vazE=ASPInj.vazinj[ind];
	  	  }
	}
}


void ASens::tabelaGenericaCabecalho(){
//void ASens::tabelaGenericaCabecalho(int ncel, Cel* celula){

    //chris
		//Saídas inerentes das BHPs - IMEX e Eclipse

		//variável vfp (deve-se entrar no .json com os valores de 0 ou 1, descritos como segue)
		//0-para solver IMEX
		//1-para solver Eclipse
//vfp=0; //chris

	ostringstream saidaP1;
	if (vfp==0)		saidaP1 << "bhpsIMEX.imx";
//	if (vfp==0)		saidaP1 << "bhpsIMEX.IMX";
	string tmp1 = saidaP1.str();
ofstream escreveIni1(tmp1.c_str(), ios_base::out);

	ostringstream saidaP2;
	if (vfp==1)		saidaP2 << "bhpsEclipse.ecp";
//	if (vfp==1)		saidaP2 << "bhpsEclipse.ECP";
	string tmp2 = saidaP2.str();
ofstream escreveIni2(tmp2.c_str(), ios_base::out);

		//int vfp1=1;
		if (vfp==0) {
		//escreveIni1 <<"sequencias da Analise de Sensibilidade para um Tramo "<< "\n";
		escreveIni1 << "** Relatorio do IMEX gerado pelo Marlim 3 em " << __DATE__ << " " << __TIME__ << endl; //add data e horário such as: 12/23/2015 2:47:47 PM
		escreveIni1 << "\n" << endl;
		escreveIni1 << "** IDENTIFICACAO :" << " CSantim10" << endl;
		escreveIni1 << "** Titulo....... :" << endl; //verificar se é necessário string de entrada
		escreveIni1 << "** Responsavel.. :" << endl; //verificar se é necessário string de entrada
		//escreveIni1 << "** Data......... :" << endl; //19092007   //verificar se é necessário string de entrada
		escreveIni1 << "** Comentario... :" << endl;   //verificar se é necessário string de entrada
		escreveIni1 << "\n" << endl;
		/*escreveIni1 << "** GEOMETRIA" << endl;
		escreveIni1 << "** UN Natureza    Comprim     Diametro       Angulo     Fator     Correlaçao" << endl;
		escreveIni1 << "**                 Medido    Tubo    Rev               dP    dL   Escoamento" << endl;
		escreveIni1 << "** *************************************************************************************" << endl;
		escreveIni1 << "** " << endl; // add termos such as 1 Marítima      754.0    8.000   0.000    79.3    1.10  0.80  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 2 Marítima      310.0    8.000   0.000    63.1    1.10  0.80  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 3 Marítima      157.0    8.000   0.000    26.6    1.10  0.80  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 4 Marítima      279.0    8.000   0.000     1.0    1.10  0.80  DUKLER_EATON_FLANIGAN
		escreveIni1 << "** " << endl; // add termos such as 5 Marítima      600.0    8.000   0.000     1.0    1.10  0.80  DUKLER_EATON_FLANIGAN
		escreveIni1 << "** " << endl; // add termos such as 6 Marítima     1890.0    8.000   0.000     1.0    1.10  0.80  DUKLER_EATON_FLANIGAN
		escreveIni1 << "** " << endl; // add termos such as 7 Marítima      100.0    6.000   0.000     1.0    1.10  0.80  DUKLER_EATON_FLANIGAN
		escreveIni1 << "** " << endl; // add termos such as 8 Poço c/ col   776.0    5.921   8.681    90.0    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 9 Poço c/ col   308.0    5.921   8.681    77.5    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 10 Poço c/ col   177.0    5.921   8.681    78.4    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 11 Poço c/ col     5.0    5.921   8.681    78.4    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 12 Poço c/ col   212.0    5.921   8.681    59.9    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 13 Poço c/ col    58.0    5.921   8.681    29.9    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 14 Poço c/ col    26.1    5.921   8.681    29.9    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 15 Poço c/ col    95.0    3.812   8.681    29.9    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 16 Poço c/ col    13.5    3.812   8.681    29.9    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 17 Poço c/ col    77.0    4.892   8.681    12.4    1.10  1.25  HAGEDORN_BROWN
		escreveIni1 << "** " << endl; // add termos such as 18 Poço s/ col   538.0    4.892   4.892    -0.4    1.10  1.25  DUKLER_EATON_FLANIGAN
		escreveIni1 << "** " << endl; // add termos such as 19 Poço s/ col   101.0    4.892   4.892     1.5    1.10  1.25  DUKLER_EATON_FLANIGAN
		escreveIni1 << "\n" << endl;*/

		//celp[i].profundiM e celp[i].profundiF

		escreveIni1 << "** Dados do Fluido " << endl;
		escreveIni1 << "** Grau API do oleo...... : " << flup[0].API << endl; //add entrada 21.3
		escreveIni1 << "** Densidade da agua .... : " << flup[0].MasEspAgua(celp[0].pres, celp[0].temp) << endl; //add entrada 1.030
		escreveIni1 << "** Densidade do gas  .... : " << flup[0].MasEspGas(celp[0].pres, celp[0].temp) << endl; //add entrada 0.000
		//escreveIni1 << "** Correlações :" << endl; //add entrada Analise PVT
		escreveIni1 << "\n" << endl;

		//for (int i; ncel; i++) {
		//if (BSW_json.HasMember("BSW")) {
		//if (listaV.vBSW==1) {
		//if (ASBCS->freq[i]!=0) {
		//escreveIni1 << "** Válvula injetora" << endl;
		//int indcelbcs=ASBCS->indBCS;
		// profundidade da BCS i

		//cout << " nASBCS " << nASBCS << endl;
		//cout << " nASFG " << nASFG << endl;
		if (nASBCS>0&&nASBCS<10) {
			escreveIni1 << "** Modo de operacao :" << " Elevacao por BCS " << endl; //add entrada QGI_VALV  = 225600.0
		for (int i = 0; i < nASBCS; ++i) {
		   // int icel = bcs[ ASBCS[i].indBCS ].posicP;  // mapeia índice da AS → posição na malha
		    escreveIni1 << "** Profundidade BCS: " << celp[bcs[ ASBCS[i].indBCS ].posicP].profundiM << endl;
		    escreveIni1 << "** Frequencia da BCS (Hz): " << ASBCS->freq[i] << endl;
		}
		escreveIni1 << "\n" << endl;
		}

//		escreveIni1 << "\n" << endl;

		//}
		//if (ASFonGas->vazgas[i]!=0) {
		//escreveIni1 << "** Válvula injetora" << endl;
		//int icel=fonteg[ASFonGas[iFG].indFG].posicP;
		//int indcelfongas=ASFonGas->indFG;
		// profundidade da Fonte de Gás i
		if (nASFG>0&&nASFG<10) {
			escreveIni1 << "** Modo de operacao :" << " Elevacao por Injecao de Gas " << endl; //add entrada QGI_VALV  = 225600.0
		for (int i = 0; i < nASFG; ++i) {
		    //int icel = fonteg[ ASFonGas[i].indFG ].posicP;
		    escreveIni1 << "** Profundidade FG: " << celp[fonteg[ ASFonGas[i].indFG ].posicP].profundiM << endl;
		    for (int i = 0; i < ASFonGas->parserieVazG; ++i) {
		        escreveIni1 << "** Vazao de Gas (m3/d): " << ASFonGas->vazgas[i] << endl;
		       /* if (i < ASFonGas->parserieVazG - 1) {
		            escreveIni2 << " ";
		        }*/
		    }
		}
		escreveIni1 << "\n" << endl;
		}
//		escreveIni1 << "\n" << endl;
		//}
		//}

		escreveIni1 << "** CURVA DE FVM PARA INJECAO NO FUNDO, COLUNA NOVA" << endl;
		escreveIni1 << "*PTUBE *LIQ 1" << endl; //add entrada 1
		escreveIni1 << "*DEPTH  " << celp[0].profundiM << endl; //add entrada  2688.6
		/*for (int i=0;ncel;i++) {
		if (celula[i].acsr.tipo==0)
		{
		escreveIni1 << "** Modo de operação :" << " Sem Acessório " << endl; //add entrada QGI_VALV  = 225600.0
		escreveIni1 << "** Válvula injetora" << endl;
		escreveIni1 << "** Profundidade     :" << " 0.0 "<< endl; //add entrada 2400.00
		escreveIni1 << "\n" << endl;
		escreveIni1 << "** CURVA DE FVM PARA INJEÇÃO NO FUNDO, COLUNA NOVA" << endl;
		escreveIni1 << "*PTUBE *LIQ 1" << endl; //add entrada 1
		escreveIni1 << "*DEPTH  " << "comprimento poço" << endl; //add entrada  2688.6
		}
		if (celula[i].acsr.tipo==1)
		{
		escreveIni1 << "** Modo de operação :" << " Fonte de Gás " << endl; //add entrada QGI_VALV  = 225600.0
		escreveIni1 << "** Válvula injetora" << endl;
		escreveIni1 << "** Profundidade (posic celula)     :" << celula[i].posic << endl; //add entrada 2400.00
		escreveIni1 << "\n" << endl;
		escreveIni1 << "** CURVA DE FVM PARA INJEÇÃO NO FUNDO, COLUNA NOVA" << endl;
		escreveIni1 << "*PTUBE *LIQ 1" << endl; //add entrada 1
		escreveIni1 << "*DEPTH  " << "comprimento poço" << endl; //add entrada  2688.6
		}
		if (celula[i].acsr.tipo==2)
		{
		escreveIni1 << "** Modo de operação :" << " Fonte de Líquido " << endl; //add entrada QGI_VALV  = 225600.0
		escreveIni1 << "** Válvula injetora" << endl;
		escreveIni1 << "** Profundidade (posic celula)     :" << celula[i].posic << endl; //add entrada 2400.00
		escreveIni1 << "\n" << endl;
		escreveIni1 << "** CURVA DE FVM PARA INJEÇÃO NO FUNDO, COLUNA NOVA" << endl;
		escreveIni1 << "*PTUBE *LIQ 1" << endl; //add entrada 1
		escreveIni1 << "*DEPTH  " << "comprimento poço" << endl; //add entrada  2688.6
		}
		if (celula[i].acsr.tipo==4)
		{
		escreveIni1 << "** Modo de operação :" << " BCS " << endl; //add entrada QGI_VALV  = 225600.0
		escreveIni1 << "** Válvula injetora" << endl;
		escreveIni1 << "** Profundidade (posic celula)     :" << celula[i].posic << endl; //add entrada 2400.00
		escreveIni1 << "\n" << endl;
		escreveIni1 << "** CURVA DE FVM PARA INJEÇÃO NO FUNDO, COLUNA NOVA" << endl;
		escreveIni1 << "*PTUBE *LIQ 1" << endl; //add entrada 1
		escreveIni1 << "*DEPTH  " << "comprimento poço" << endl; //add entrada  2688.6
		}
		}*/
	    escreveIni1 << "  " << endl;

	    //chris new -> troquei a ordem para o IMEX

		//valores devem vir da entrada   0.00   0.10   0.20   0.30   0.40   0.50   0.60   0.70   0.80   0.99   /
		escreveIni1 << "*GOR " << endl; //" tamanho vetor " << ASRGO.parserieRGO << endl;
	    for (int i = 0; i < ASRGO.parserieRGO; ++i) {
	        escreveIni1 << ASRGO.RGO[i];
	        if (i < ASRGO.parserieRGO - 1) {
	            escreveIni1 << " "; //
	        }
	    }
	    escreveIni1 << "  " << endl;

		escreveIni1 << "*QLIQ " << endl; //" tamanho vetor " << ASFonLiq->parserieVL << endl;
	    for (int i = 0; i < ASFonLiq->parserieVL; ++i) {
	        escreveIni1 << ASFonLiq->vazliq[i];
	        if (i < ASFonLiq->parserieVL - 1) {
	            escreveIni1 << " ";
	        }
	    }
	    escreveIni1 << "  " << endl;
	    //escreveIni1 << "/" << "\n" << endl;
		//valores devem vir da entrada 10.0 250.0 500.0 750.0 1000.0 1500.0 2000.0   /

		//escreveIni1 << "-- WCUT (0 a 1)" << " tamanho vetor " << ASBSW.parserieBSW << " " << ASBSW.BSW[0] << "," << ASBSW.BSW[1] << endl;
		escreveIni1 << "*WCUT " << endl; //" tamanho vetor " << ASBSW.parserieBSW << endl;
	    for (int i = 0; i < ASBSW.parserieBSW; ++i) {
	        escreveIni1 << ASBSW.BSW[i];
	        if (i < ASBSW.parserieBSW - 1) {
	            escreveIni1 << " ";
	        }
	    }
	    escreveIni1 << "  " << endl;

		escreveIni1 << "*WHP " << endl; // " tamanho vetor " << ASPsep.parseriePres << endl; //PSEP
	    for (int i = 0; i < ASPsep.parseriePres; ++i) {
	        escreveIni1 << ASPsep.pres[i];
	        if (i < ASPsep.parseriePres - 1) {
	            escreveIni1 << " ";
	        }
	    }
	    escreveIni1 << "  " << endl;
		//valores devem vir da entrada  10.82  15.72  25.53  50.05   /



	   //alteracao 18/03/2024 - cabecalho Imex excluindo a lista de QG para o Imex
	    /*escreveIni1 << "*QG (m3/d)" << " tamanho vetor " << ASFonGas->parserieVazG << endl;
	    for (int i = 0; i < ASFonGas->parserieVazG; ++i) {
	        escreveIni1 << ASFonGas->vazgas[i];
	        if (i < ASFonGas->parserieVazG - 1) {
	            escreveIni1 << " ";
	        }
	    }
	    escreveIni1 << "  " << endl;*/

	    //escreveIni1 << std::endl;
	    //escreveIni1 << "/" << "\n" <<endl;
	    escreveIni1 << "*" << "BHPTO" << endl;
	    //escreveIni1 << "** " << "QO  " << "WCUT  " << "GOR  " <<  endl; //add BHP1 e no final [kgf/cm2 abs]

		}


		else if (vfp==1) {


		escreveIni2 << "** Relatorio do ECLIPSE gerado pelo Marlim 3 em " << __DATE__ << " " << __TIME__ << endl;  //add data e horário, such as: 4/9/2009 15:25:21
		escreveIni2 << "\n";
		escreveIni2 << "-- Tabela de BHPs para utilizacao no simulador Eclipse." << endl;
		escreveIni2 << "-- Tabela gerada pelo simulador Marlim 3." << endl;

		cout << "nASBCS: " << nASBCS << endl;
		if (nASBCS>0&&nASBCS<10) {
		for (int i = 0; i < nASBCS; ++i) {
			escreveIni2 << "-- Profundidade Medida BCS: " << celp[bcs[ ASBCS[i].indBCS ].posicP].profundiM << endl;
		}
		}

		if (nASFG>0&&nASFG<10) {
		for (int i = 0; i < nASFG; ++i) {
		    escreveIni2 << "-- Profundidade Medida de injecao de gas (m): " << celp[fonteg[ ASFonGas[i].indFG ].posicP].profundiM << endl;
		}
		}
		//escreveIni2 << "-- Profundidade Medida de injeção de gás (m):" << endl; //add profundidade 2035
		//add tal variable VFPPROD   --------------
		escreveIni2 << "VFPPROD   --------------" << endl;

		escreveIni2 << "\n" << endl;
		escreveIni2 << "-- N   profundidade canhoneado (m)" << endl;
		//verificar o que é vetor e o que é entrada   1      2655.0     'LIQ' 'WCT' 'GOR' 'THP' 'GRAT' 'METRIC' 'BHP'   /
		escreveIni2 << " 1 " << celp[0].profundiM << " 'LIQ' " << " 'WCT' " << " 'GOR' " << " 'THP' " << " 'GRAT' " << " 'METRIC' " <<"'BHP'" << "   /" << endl;
		//escreveIni2 << " 1 " << celp[0].profundiM << " 'LIQ' " << " 'WCT' " << " 'GOR' " << " 'THP' " << " 'GRAT' " << " 'METRIC' " <<"'BHP'" << "   /" << endl;
		escreveIni2 << "-- QLIQ (Sm3/d)" << endl; //" tamanho vetor " << ASFonLiq->parserieVL << endl;
	    for (int i = 0; i < ASFonLiq->parserieVL; ++i) {
	        escreveIni2 << ASFonLiq->vazliq[i];
	        if (i < ASFonLiq->parserieVL - 1) {
	            escreveIni2 << " ";
	        }
	    }
	   // escreveIni2 << std::endl;
	    escreveIni2 << "   /" << endl;
		//valores devem vir da entrada 10.0 250.0 500.0 750.0 1000.0 1500.0 2000.0   /
		escreveIni2 << "-- THP (kgf/cm2)" << endl; //" tamanho vetor " << ASPsep.parseriePres << endl;
	    for (int i = 0; i < ASPsep.parseriePres; ++i) {
	        escreveIni2 << ASPsep.pres[i];
	        if (i < ASPsep.parseriePres - 1) {
	            escreveIni2 << " ";
	        }
	    }
	    escreveIni2 << "   /" << endl;
	    //escreveIni2 << std::endl;
	    //escreveIni2 << "/" << "\n" <<endl;
		//valores devem vir da entrada  10.82  15.72  25.53  50.05   /

		//escreveIni2 << "-- WCUT (0 a 1)" << " tamanho vetor " << ASBSW.parserieBSW << " " << ASBSW.BSW[0] << "," << ASBSW.BSW[1] << endl;
		escreveIni2 << "-- WCUT (-)" << endl; // " tamanho vetor " << ASBSW.parserieBSW << endl;
	    for (int i = 0; i < ASBSW.parserieBSW; ++i) {
	        escreveIni2 << ASBSW.BSW[i];
	        if (i < ASBSW.parserieBSW - 1) {
	            escreveIni2 << "   ";
	        }
	    }
	    escreveIni2 << "   /" << endl;

		escreveIni2 << "-- RGO (Sm3/Sm3)" << endl; // " tamanho vetor " << ASRGO.parserieRGO << endl;
	    for (int i = 0; i < ASRGO.parserieRGO; ++i) {
	        escreveIni2 << ASRGO.RGO[i];
	        if (i < ASRGO.parserieRGO - 1) {
	            escreveIni2 << "   "; //
	        }
	    }
	    escreveIni2 << "   /" << endl;

	    //escreveIni2 << std::endl;
	   // escreveIni2 << "/" << "\n" <<endl;
		//valores devem vir da entrada   0.00   0.10   0.20   0.30   0.40   0.50   0.60   0.70   0.80   0.99   /

	    if (nASBCS>0&&nASBCS<10) {
	    	escreveIni2 << "-- GRAT (Hz)" << endl;
		    for (int i = 0; i < ASBCS->parserieFreq; ++i) {
		        escreveIni2 << ASBCS->freq[i];
		        if (i < ASBCS->parserieFreq - 1) {
		            escreveIni2 << " ";
		        }
		    }
    	    escreveIni2 << "   /" << endl;
	    }
	    if (nASFG>0&&nASFG<10) {
	    	escreveIni2 << "-- GRAT (m3/d)" << endl;
		    for (int i = 0; i < ASFonGas->parserieVazG; ++i) {
		        escreveIni2 << ASFonGas->vazgas[i];
		        if (i < ASFonGas->parserieVazG - 1) {
		            escreveIni2 << " ";
		        }
		    }
    	    escreveIni2 << "   /" << endl;
	    }

	    //escreveIni2 << "   /" << endl;

	    //escreveIni2 << "   /" << endl;


		/*escreveIni2 << "-- Obs:1)Se os ranges das correlações do programa MARLIM 3 forem ultrapassados," << endl;
		escreveIni2 << "-- o cálculo não será possível e, de acordo com o manual do Eclipse, um" << endl;
		escreveIni2 << "-- valor padrão BHP=1.0E+10 é impresso." << endl;*/

		//número de colunas da tabela do Eclipse (solver de Reservatório) depende do número de variáveis de entrada e de como elas
		escreveIni2 << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;





//escreveIni2.close();

		} else {
			cout << "O valor de vfp deve ser 0-IMEX ou 1-Eclipse" << endl;
		}


		//system("pause");

	ostringstream saidaP;
	saidaP << "tabelaGenericaAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);


	if (vfp==0) escreveIni1 <<"** ";
	if (vfp==1) escreveIni2 <<"-- ";

	if(listaV.vpsep==1){
		escreveIni <<" Pressao Separador ;";
	//	if (vfp==0) escreveIni1 <<"WHP ";
		if (vfp==1) escreveIni2 <<"nthp ";
	}
	if(listaV.vBSW==1){
		escreveIni <<" BSW fluido 0 ;";
		if (vfp==0) escreveIni1 <<"  GOR  "; //chris new
		if (vfp==1) escreveIni2 <<" nwc ";
	}
	if(listaV.vRGO==1){
		escreveIni<<" RGO fluido 0 ;";
		if (vfp==0) escreveIni1 <<" WCUT  "; //chris new
		if (vfp==1) escreveIni2 <<"  nrgo  ";
	}
	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  if(ASFonLiq[iFL].parserieTemp>0){
				  escreveIni << " indice Fonte Liquido = " << konta1<<
					" Temperatura ;";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){

				  escreveIni << " indice Fonte Liquido = " << konta2<<
					" Vazao de Liquido ;";

				/*  if (vfp==0)
									  {
									  escreveIni1 << " QO ";
									  }*/


				  if (vfp==0)
					  {
					  escreveIni1 << " QLIQ ";
					  }

				 // system("pause");
				/*  if (vfp==1)
					  {
					  escreveIni2 << "nalq  ";
					  }*/
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
				  escreveIni <<" indice Fonte Liquido = " << konta3<<
					" Beta ;";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
				  escreveIni << " indice Fonte Liquido = " << konta4<<
					" Fluido ;";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  if(ASFonGas[iFG].parserieTemp>0){
				  escreveIni<< " indice Fonte Gas = " << konta1<<
					" Temperatura ;";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
				  escreveIni << " indice Fonte Gas = " << konta2<<
					" Vazao Gas ;";
						  if (vfp==0)
							  {
							  //escreveIni1 << " QG ";
							  }

						 // system("pause");
						  if (vfp==1)
							  {
							  escreveIni2 << "nalq  ";
							  }
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
				  escreveIni <<" indice Fonte Gas = " << konta3<<
					" Vazao Liq Complementar ;";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  if(ASIPR[iIPR].parserieTemp>0){
				  escreveIni <<" indice IPR = " << konta1<<
					" Temperatura ;";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
				  escreveIni <<" indice IPR = " << konta2<<
					" P Estatica ;";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
				  escreveIni <<" indice IPR = " << konta3<<
					" indice IP ;";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
				  escreveIni<< " indice IPR = " << konta4<<
					" II ;";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
				  escreveIni <<" indice IPR = " <<konta5<<
					" qMax ;";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
				  escreveIni <<" indice IPR = " << konta6<<
					" fluido ;";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  if(ASFonMas[iFM].parserieTemp>0){
				  escreveIni<< " indice Fonte Massa = " << konta1<<
					" Temperatura ;";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
				  escreveIni <<" indice Fonte Massa = " << konta2<<
					" Vazao Liquido ;";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
				  escreveIni <<" indice Fonte Massa = " << konta3<<
					" Vazao Liquido Complementar ;";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
				  escreveIni << " indice Fonte Massa = " << konta4<<
					" Vazao Gas ;";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
				  escreveIni << " indice Fonte Massa = " << konta5<<
					" Fluido ;";
	    		  konta5++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  if(ASBCS[indBCS].parserieFreq>0){
				  escreveIni << " indice BCS = " << konta1<<
					" Frequencia ;";
				  if (vfp==1)
					  {
					  escreveIni2 << "nfreq  ";
					  }
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
				  escreveIni<< " indice BCS = " << konta2<<
					" N Estagios ;";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  if(ASDP[indBCS].parserieDP>0){
				  escreveIni << " indice Del Pres = " << konta1<<
					" Del Pres ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
				  escreveIni<< " indice Corr Pres Hidro = " << konta1<<
					" Corr Pres Hidro ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
				  escreveIni<< " indice Corr Pres Fric = " << konta1<<
					" Corr Pre Fric ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  if(ASdTdL[indBCS].parserieDT>0){
				  escreveIni << " indice Corr Temperatura = " << konta1<<
					" Corr Temperatura ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){

	    	  if(ASBVOL[indBCS].parserieFreq>0){
				  escreveIni<< " indice Bomba Vol = " << konta1<<
					" Frequencia ;";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
				  escreveIni << " indice Bomba Vol = " << konta2<<
					" Capacidade ;";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
				  escreveIni << " indice Bomba Vol = " << konta3<<
					" Fator Politropico ;";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  if(ASValv[indBCS].parserieAbre>0){
				  escreveIni <<" indice Valvula = " << konta1<<
					" Abertura ;";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
				  escreveIni << " indice Valvula = " << konta2<<
					" CD ;";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  if(ASFuro[indBCS].parserieAbre>0){
				  escreveIni << " indice Vazamento = " << konta1<<" Abertura ;";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
				  escreveIni << " indice Vazamento = " << konta2<<
					" CD ;";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
				  escreveIni << " indice Vazamento = " << konta3<<
					" Beta ;";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
				  escreveIni << " indice Vazamento = " << konta4<<
					" Pressao ;";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
				  escreveIni <<" indice Vazamento = " << konta5<<
					" Temperatura ;";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
				  escreveIni <<" indice Vazamento = " << konta6<<
					" Fluido ;";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
				  escreveIni <<" indice Secao Transversal = " << konta1<<
					" Maior Diam Interno ;";
	    		  konta1++;
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
				  escreveIni<< " indice Secao Transversal = " << konta2<<
					" Menor Diam Interno ;";
	    		  konta2++;
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
				  escreveIni <<" indice Secao Transversal = " << konta3<<
					" Rugosidade ;";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
				  escreveIni << " indice Material = " << konta1<<
					" Condutividade ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			escreveIni <<" Pressao Injecao ;";
		}
		if(ASGasInj.parserieTemp>0){

			escreveIni <<" Temp Injecao ;";
		}
		if(ASGasInj.parserieVazGas>0){
			escreveIni <<" Vaz Injecao ;";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		escreveIni <<" Pres. Contorno ;";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		escreveIni <<" Beta Contorno ;";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		escreveIni <<" Temp. Contorno ;";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		escreveIni <<" Titulo Contorno ;";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		escreveIni <<" Pres. Contorno ;";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		escreveIni <<" Beta Contorno ;";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		escreveIni <<" Vazao Contorno ;";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		escreveIni <<" Temp. Contorno ;";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		escreveIni <<" Abertura Choke ;";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		escreveIni <<" CD Choke ;";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		escreveIni <<" Pres Jusante ;";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		escreveIni <<" Pres Montante ;";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		escreveIni <<" Temp. Injecao ;";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		escreveIni <<" Vazao Poco Injecao ;";
	  	  }
	}

	int dimensionQL;
	int dimensionPSEP;
	dimensionQL=ASFonLiq->parserieVL;
	dimensionPSEP=ASPsep.parseriePres;
	if(vfp==0) {
	 // for (int i=0;i<dimensionPSEP;i++) {
		 for (int i=0;i<dimensionPSEP;i++) {
		  escreveIni1 << " BHP" << i+1 << "   ";
		 // cout << "i" << endl;
	  }
	  escreveIni1 << " [kgf/cm2 absoluta]" << endl;
	}

	if(vfp==1) {
	  for (int i=0;i<dimensionQL;i++) {
		  escreveIni2 << "  BHP" << i+1 << "    ";
		  //cout << "i" << endl;
	  }
	}

	//escreveIni2 << "\n" << "--                            kgf/cm2 abs " << endl; //kgf/cm2 abs
	escreveIni2 << "\n" << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

	//escreveIni <<"\n";
	escreveIni.close();

	//escreveIni1 <<"\n";
	escreveIni1.close();

	//escreveIni2 <<"\n";
	escreveIni2.close();


//system("pause");

}


void ASens::tabelaGenerica(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG,ProFlu* flup,
	    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
		detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
		 double& pGSup,double& temperatura,double& presiniG, double& tempiniG, double& vazgasG,
		 double& presE, double& tempE,double& titE, double& betaE, double& vazE, int seq, int& indCHK,
		 double* vdPdLH, double* vdPdLF, double* vdTdL,double BHP){

	ostringstream saidaP1;
	if (vfp==0)		saidaP1 << "bhpsIMEX.imx";
//	if (vfp==0)		saidaP1 << "bhpsIMEX.IMX";
string tmp1 = saidaP1.str();
ofstream escreveIni1(tmp1.c_str(), ios_base::app);

//cout << "vfp: " << vfp;
//system("pause");

	ostringstream saidaP2;
	if (vfp==1)		saidaP2 << "bhpsEclipse.ecp";
//	if (vfp==1)		saidaP2 << "bhpsEclipse.ECP";
string tmp2 = saidaP2.str();
ofstream escreveIni2(tmp2.c_str(), ios_base::app);

	ostringstream saidaP;
	saidaP << "tabelaGenericaAS.dat";

	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::app);

	indCHK=0;
	int ind;
	int coluna=0;
	if(listaV.vpsep==1){
		ind=sequenciaAS[seq].PSEPpres;
		pGSup=ASPsep.pres[ind];
		saidaBHP[seq][coluna]=pGSup;
		coluna++;
		escreveIni <<pGSup<< " ;";
	//	if (vfp==0&&chrisao==0) escreveIni1 << "     " << ind+1 <<" ";
		if (vfp==1&&chrisao==0) escreveIni2 << "     " << ind+1 <<" ";
	}
	if(listaV.vBSW==1){
		ind=sequenciaAS[seq].BSW;
		flup[0].BSW=ASBSW.BSW[ind];
		flup[0].RenovaFluido();
		atualizaBSW(celula, ASBSW.BSW[ind]);
		saidaBHP[seq][coluna]=flup[0].BSW;
		coluna++;
		escreveIni <<flup[0].BSW<< " ;";
		if (vfp==0&&chrisao==0) escreveIni1 << "    " << ind+1 <<"  ";
		if (vfp==1&&chrisao==0) escreveIni2 << "    " << ind+1 <<" ";
	}
	if(listaV.vRGO==1){
		ind=sequenciaAS[seq].RGO;
		if(flup[0].flashCompleto!=2){
			flup[0].RGO=ASRGO.RGO[ind];
			flup[0].RenovaFluido();
		}
		else atualizaCompRGO(ASRGO.RGO[ind], flup[0]);
		atualizaRGO(celula, ASRGO.RGO[ind]);
		saidaBHP[seq][coluna]=flup[0].RGO;
		coluna++;
		escreveIni <<flup[0].RGO<< " ;";
		if (vfp==0&&chrisao==0) escreveIni1 << "    " << ind+1 <<"  ";
		if (vfp==1&&chrisao==0) escreveIni2 << "    " << ind+1 <<" ";
	}
	if(listaV.vfonliq==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
	      for (int iFL = 0; iFL < nASFL; iFL++){
	    	  int icel=fontel[ASFonLiq[iFL].indFL].posicP;
	    	  if(ASFonLiq[iFL].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FLtemp[konta1];
	    		  celula[icel].acsr.injl.temp=ASFonLiq[iFL].temp[ind];
	    		  saidaBHP[seq][coluna]=ASFonLiq[iFL].temp[ind];
	    		  coluna++;
				  escreveIni <<ASFonLiq[iFL].temp[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieVL>0){
	    		  ind=sequenciaAS[seq].FLvazliq[konta2];
	    		  celula[icel].acsr.injl.QLiq=ASFonLiq[iFL].vazliq[ind];
	    		  saidaBHP[seq][coluna]=ASFonLiq[iFL].vazliq[ind];
	    		  if(iFL==0){
	    			  saidaVazLiq[seq][ind]=BHP;
	    		  }
	    		  coluna++;
				  escreveIni <<ASFonLiq[iFL].vazliq[ind]<< " ;";
					if (vfp==0&&chrisao==0) escreveIni1 << "     " << ind+1 <<"     ";
				//	if (vfp==1&&chrisao==0) escreveIni2 << "    " << ind+1 <<"     ";
	    		  konta2++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieBet>0){
	    		  ind=sequenciaAS[seq].FLbet[konta3];
	    		  celula[icel].acsr.injl.bet=ASFonLiq[iFL].bet[ind];
	    		  saidaBHP[seq][coluna]=ASFonLiq[iFL].bet[ind];
	    		  coluna++;
				  escreveIni <<ASFonLiq[iFL].bet[ind]<< " ;";
	    		  konta3++;
	    	  }
	    	  if(ASFonLiq[iFL].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FLindfluP[konta4];
	    		  int indFlu=ASFonLiq[iFL].indfluP[ind];
	    		  celula[icel].acsr.injl.FluidoPro=flup[indFlu];
	    		  saidaBHP[seq][coluna]=indFlu;
	    		  coluna++;
				  escreveIni <<indFlu<< " ;";
	    		  konta4++;
	    	  }
	      }
	}
	if(listaV.vfongas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iFG = 0; iFG < nASFG; iFG++){
	    	  int icel=fonteg[ASFonGas[iFG].indFG].posicP;
	    	  if(ASFonGas[iFG].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FGtemp[konta1];
	    		  celula[icel].acsr.injg.temp=ASFonGas[iFG].temp[ind];
	    		  saidaBHP[seq][coluna]=ASFonGas[iFG].temp[ind];
	    		  coluna++;
				  escreveIni <<ASFonGas[iFG].temp[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazG>0){
	    		  ind=sequenciaAS[seq].FGvazgas[konta2];
	    		  celula[icel].acsr.injg.QGas=ASFonGas[iFG].vazgas[ind];
	    		  saidaBHP[seq][coluna]=ASFonGas[iFG].vazgas[ind];
	    		  coluna++;
				  escreveIni <<ASFonGas[iFG].vazgas[ind]<< " ;";
					//if (vfp==0&&chrisao==0) escreveIni1 << "    " << ind+1 <<"     ";
					if (vfp==1&&chrisao==0) escreveIni2 << "    " << ind+1 <<"     ";
	    		  konta2++;
	    	  }
	    	  if(ASFonGas[iFG].parserieVazC>0){
	    		  ind=sequenciaAS[seq].FGvazcomp[konta3];
	    		  celula[icel].acsr.injg.razCompGas=ASFonGas[iFG].vazcomp[ind];
	    		  saidaBHP[seq][coluna]=ASFonGas[iFG].vazcomp[ind];
	    		  coluna++;
				  escreveIni <<ASFonGas[iFG].vazcomp[ind]<< " ;";
	    		  konta3++;
	    	  }
	      }
	}
    if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  celula[icel].acsr.bcs.freq=ASBCS[indBCS].freq[ind];
	    		  saidaBHP[seq][coluna]=ASBCS[indBCS].freq[ind];
	    		  coluna++;
				  escreveIni <<ASBCS[indBCS].freq[ind]<< " ;";
				if (vfp==1&&chrisao==0) escreveIni2 << "    " << ind+1 <<"     ";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  saidaBHP[seq][coluna]=ASBCS[indBCS].nestag[ind];
	    		  coluna++;
				  escreveIni <<ASBCS[indBCS].nestag[ind]<< " ;";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vipr==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int iIPR = 0; iIPR < nASIPR; iIPR++){
	    	  int icel=IPRS[ASIPR[iIPR].indIPR].indcel;
	    	  if(ASIPR[iIPR].parserieTemp>0){
	    		  ind=sequenciaAS[seq].IPRtemp[konta1];
	    		  celula[icel].acsr.ipr.Tres=ASIPR[iIPR].temp[ind];
	    		  saidaBHP[seq][coluna]=ASIPR[iIPR].temp[ind];
	    		  coluna++;
				  escreveIni <<ASIPR[iIPR].temp[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASIPR[iIPR].parseriePres>0){
	    		  ind=sequenciaAS[seq].IPRpres[konta2];
	    		  celula[icel].acsr.ipr.Pres=ASIPR[iIPR].pres[ind];
	    		  saidaBHP[seq][coluna]=ASIPR[iIPR].pres[ind];
	    		  coluna++;
				  escreveIni <<ASIPR[iIPR].pres[ind]<< " ;";
	    		  konta2++;
	    	  }
	    	  if(ASIPR[iIPR].parserieIP>0){
	    		  ind=sequenciaAS[seq].IPRip[konta3];
	    		  celula[icel].acsr.ipr.ip=ASIPR[iIPR].ip[ind];
	    		  saidaBHP[seq][coluna]=ASIPR[iIPR].ip[ind];
	    		  coluna++;
				  escreveIni <<ASIPR[iIPR].ip[ind]<< " ;";
	    		  konta3++;
	    	  }
	    	  if(ASIPR[iIPR].parserieJP>0){
	    		  ind=sequenciaAS[seq].IPRjp[konta4];
	    		  celula[icel].acsr.ipr.ij=ASIPR[iIPR].jp[ind];
	    		  saidaBHP[seq][coluna]=ASIPR[iIPR].jp[ind];
	    		  coluna++;
				  escreveIni <<ASIPR[iIPR].jp[ind]<< " ;";
	    		  konta4++;
	    	  }
	    	  if(ASIPR[iIPR].parserieqMax>0){
	    		  ind=sequenciaAS[seq].IPRqMax[konta5];
	    		  celula[icel].acsr.ipr.qMax=ASIPR[iIPR].qMax[ind];
	    		  saidaBHP[seq][coluna]=ASIPR[iIPR].qMax[ind];
	    		  coluna++;
				  escreveIni <<ASIPR[iIPR].qMax[ind]<< " ;";
	    		  konta5++;
	    	  }
	    	  if(ASIPR[iIPR].parserieFlup>0){
	    		  ind=sequenciaAS[seq].IPRindfluP[konta6];
	    		  int indFlu=ASIPR[iIPR].indfluP[ind];
	    		  celula[icel].acsr.ipr.FluidoPro=flup[indFlu];
	    		  saidaBHP[seq][coluna]=ASIPR[iIPR].indfluP[ind];
	    		  coluna++;
				  escreveIni <<ASIPR[iIPR].indfluP[ind]<< " ;";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.vfonmas==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		for (int iFM = 0; iFM < nASFM; iFM++){
	    	  int icel=fontem[ASFonMas[iFM].indFM].posicP;
	    	  if(ASFonMas[iFM].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FMtemp[konta1];
	    		  celula[icel].acsr.injm.temp=ASFonMas[iFM].temp[ind];
	    		  saidaBHP[seq][coluna]=ASFonMas[iFM].temp[ind];
	    		  coluna++;
				  escreveIni <<ASFonMas[iFM].temp[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMP>0){
	    		  ind=sequenciaAS[seq].FMvazMasP[konta2];
	    		  celula[icel].acsr.injm.MassP=ASFonMas[iFM].vazMasP[ind];
	    		  saidaBHP[seq][coluna]=ASFonMas[iFM].vazMasP[ind];
	    		  coluna++;
				  escreveIni <<ASFonMas[iFM].vazMasP[ind]<< " ;";
	    		  konta2++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMC>0){
	    		  ind=sequenciaAS[seq].FMvazMasC[konta3];
	    		  celula[icel].acsr.injm.MassC=ASFonMas[iFM].vazMasC[ind];
	    		  saidaBHP[seq][coluna]=ASFonMas[iFM].vazMasC[ind];
	    		  coluna++;
				  escreveIni <<ASFonMas[iFM].vazMasC[ind]<< " ;";
	    		  konta3++;
	    	  }
	    	  if(ASFonMas[iFM].parserieMG>0){
	    		  ind=sequenciaAS[seq].FMvazMasG[konta4];
	    		  celula[icel].acsr.injm.MassG=ASFonMas[iFM].vazMasG[ind];
	    		  saidaBHP[seq][coluna]=ASFonMas[iFM].vazMasG[ind];
	    		  coluna++;
				  escreveIni <<ASFonMas[iFM].vazMasG[ind]<< " ;";
	    		  konta4++;
	    	  }
	    	  if(ASFonMas[iFM].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FMindfluP[konta5];
	    		  int indFlu=ASFonMas[iFM].indfluP[ind];
	    		  celula[icel].acsr.injm.FluidoPro=flup[indFlu];
	    		  saidaBHP[seq][coluna]=ASFonMas[iFM].indfluP[ind];
	    		  coluna++;
				  escreveIni <<ASFonMas[iFM].indfluP[ind]<< " ;";
	    		  konta5++;
	    	  }
	      }
	}
   /* if(listaV.vbcs==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASBCS; indBCS++){
	    	  int icel=bcs[ASBCS[indBCS].indBCS].posicP;
	    	  if(ASBCS[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BCSfreq[konta1];
	    		  celula[icel].acsr.bcs.freq=ASBCS[indBCS].freq[ind];
	    		  saidaBHP[seq][coluna]=ASBCS[indBCS].freq[ind];
	    		  coluna++;
				  escreveIni <<ASBCS[indBCS].freq[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASBCS[indBCS].parserieEstag>0){
	    		  ind=sequenciaAS[seq].BCSnestag[konta2];
	    		  celula[icel].acsr.bcs.nestag=ASBCS[indBCS].nestag[ind];
	    		  saidaBHP[seq][coluna]=ASBCS[indBCS].nestag[ind];
	    		  coluna++;
				  escreveIni <<ASBCS[indBCS].nestag[ind]<< " ;";
	    		  konta2++;
	    	  }
	      }
	}*/
	if(listaV.vdp==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASDP; indBCS++){
	    	  int icel=dpreq[ASDP[indBCS].indDP].posicP;
	    	  if(ASDP[indBCS].parserieDP>0){
	    		  ind=sequenciaAS[seq].DPdp[konta1];
	    		  celula[icel].acsr.delp=ASDP[indBCS].dp[ind];
	    		  saidaBHP[seq][coluna]=ASDP[indBCS].dp[ind];
	    		  coluna++;
				  escreveIni <<ASDP[indBCS].dp[ind]<< " ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpH==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLH; indBCS++){
	    	  int icel=ASdPdLH[indBCS].indDPHidro;
	    	  if(ASdPdLH[indBCS].parserieDPHidro>0){
	    		  ind=sequenciaAS[seq].DPdLH[konta1];
	    		  vdPdLH[icel]=ASdPdLH[indBCS].dPHidro[ind];
	    		  saidaBHP[seq][coluna]=ASdPdLH[indBCS].dPHidro[ind];
	    		  coluna++;
				  escreveIni <<ASdPdLH[indBCS].dPHidro[ind]<< " ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdpF==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdPdLF; indBCS++){
	    	  int icel=ASdPdLF[indBCS].indDPFric;
	    	  if(ASdPdLF[indBCS].parserieDPFric>0){
	    		  ind=sequenciaAS[seq].DPdLF[konta1];
	    		  vdPdLF[icel]=ASdPdLF[indBCS].dPFric[ind];
	    		  saidaBHP[seq][coluna]=ASdPdLF[indBCS].dPFric[ind];
	    		  coluna++;
				  escreveIni <<ASdPdLF[indBCS].dPFric[ind]<< " ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vdt==1){
		int konta1=0;
		for (int indBCS = 0; indBCS < nASdTdL; indBCS++){
	    	  int icel=ASdTdL[indBCS].indDT;
	    	  if(ASdTdL[indBCS].parserieDT>0){
	    		  ind=sequenciaAS[seq].DTdL[konta1];
	    		  vdTdL[icel]=ASdTdL[indBCS].dT[ind];
	    		  saidaBHP[seq][coluna]=ASdTdL[indBCS].dT[ind];
	    		  coluna++;
				  escreveIni <<ASdTdL[indBCS].dT[ind]<< " ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vbvol==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int indBCS = 0; indBCS < nASBV; indBCS++){
	    	  int icel=bvol[ASBVOL[indBCS].indBV].posicP;
	    	  if(ASBVOL[indBCS].parserieFreq>0){
	    		  ind=sequenciaAS[seq].BVOLfreq[konta1];
	    		  celula[icel].acsr.bvol.freq=ASBVOL[indBCS].freq[ind];
	    		  saidaBHP[seq][coluna]=ASBVOL[indBCS].freq[ind];
	    		  coluna++;
				  escreveIni <<ASBVOL[indBCS].freq[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieCap>0){
	    		  ind=sequenciaAS[seq].BVOLcapacidade[konta2];
	    		  celula[icel].acsr.bvol.capacidade=ASBVOL[indBCS].capacidade[ind];
	    		  saidaBHP[seq][coluna]=ASBVOL[indBCS].capacidade[ind];
	    		  coluna++;
				  escreveIni <<ASBVOL[indBCS].capacidade[ind]<< " ;";
	    		  konta2++;
	    	  }
	    	  if(ASBVOL[indBCS].parserieNPoli>0){
	    		  ind=sequenciaAS[seq].BVOLnpoli[konta3];
	    		  celula[icel].acsr.bvol.npoli=ASBVOL[indBCS].npoli[ind];
	    		  saidaBHP[seq][coluna]=ASBVOL[indBCS].npoli[ind];
	    		  coluna++;
				  escreveIni <<ASBVOL[indBCS].npoli[ind]<< " ;";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.vvalv==1){
		int konta1=0;
		int konta2=0;
		for (int indBCS = 0; indBCS < nASV; indBCS++){
	    	  int icel=valv[ASValv[indBCS].indV].posicP;
	    	  if(ASValv[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].VALVabertura[konta1];
	    		  celula[icel].acsr.chk.AreaGarg=ASValv[indBCS].abertura[ind]*celula[icel].acsr.chk.AreaTub;
	    		  saidaBHP[seq][coluna]=ASValv[indBCS].abertura[ind];
	    		  coluna++;
				  escreveIni <<ASValv[indBCS].abertura[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASValv[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].VALVcd[konta2];
	    		  celula[icel].acsr.chk.cdchk=ASValv[indBCS].cd[ind];
	    		  saidaBHP[seq][coluna]=ASValv[indBCS].cd[ind];
	    		  coluna++;
				  escreveIni <<ASValv[indBCS].cd[ind]<< " ;";
	    		  konta2++;
	    	  }
	      }
	}
	if(listaV.vfuro==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		int konta4=0;
		int konta5=0;
		int konta6=0;
		for (int indBCS = 0; indBCS < nASFuro; indBCS++){
	    	  int icel=furo[ASFuro[indBCS].indFuro].posicP;
	    	  if(ASFuro[indBCS].parserieAbre>0){
	    		  ind=sequenciaAS[seq].FUROabertura[konta1];
	    		  celula[icel].acsr.fontechk.abertura=ASFuro[indBCS].abertura[ind];
	    		  saidaBHP[seq][coluna]=ASFuro[indBCS].abertura[ind];
	    		  coluna++;
				  escreveIni <<ASFuro[indBCS].abertura[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASFuro[indBCS].parserieCD>0){
	    		  ind=sequenciaAS[seq].FUROcd[konta2];
	    		  celula[icel].acsr.fontechk.cd=ASFuro[indBCS].cd[ind];
	    		  saidaBHP[seq][coluna]=ASFuro[indBCS].cd[ind];
	    		  coluna++;
				  escreveIni <<ASFuro[indBCS].cd[ind]<< " ;";
	    		  konta2++;
	    	  }
	    	  if(ASFuro[indBCS].parserieBeta>0){
	    		  ind=sequenciaAS[seq].FURObeta[konta3];
	    		  celula[icel].acsr.fontechk.bet=ASFuro[indBCS].beta[ind];
	    		  saidaBHP[seq][coluna]=ASFuro[indBCS].beta[ind];
	    		  coluna++;
				  escreveIni <<ASFuro[indBCS].beta[ind]<< " ;";
	    		  konta3++;
	    	  }
	    	  if(ASFuro[indBCS].parseriePres){
	    		  ind=sequenciaAS[seq].FUROpres[konta4];
	    		  celula[icel].acsr.fontechk.pamb=ASFuro[indBCS].pres[ind];
	    		  saidaBHP[seq][coluna]=ASFuro[indBCS].pres[ind];
	    		  coluna++;
				  escreveIni <<ASFuro[indBCS].pres[ind]<< " ;";
	    		  konta4++;
	    	  }
	    	  if(ASFuro[indBCS].parserieTemp>0){
	    		  ind=sequenciaAS[seq].FUROtemp[konta5];
	    		  celula[icel].acsr.fontechk.tempT=ASFuro[indBCS].temp[ind];
	    		  saidaBHP[seq][coluna]=ASFuro[indBCS].temp[ind];
	    		  coluna++;
				  escreveIni <<ASFuro[indBCS].temp[ind]<< " ;";
	    		  konta5++;
	    	  }
	    	  if(ASFuro[indBCS].parserieFlu>0){
	    		  ind=sequenciaAS[seq].FUROindFlu[konta6];
	    		  int indFlu=ASFuro[indBCS].indFlu[ind];
	    		  celula[icel].acsr.fontechk.fluidoPamb=flup[indFlu];
	    		  saidaBHP[seq][coluna]=indFlu;
	    		  coluna++;
				  escreveIni <<indFlu<< " ;";
	    		  konta6++;
	    	  }
	      }
	}
	if(listaV.diam==1){
		int konta1=0;
		int konta2=0;
		int konta3=0;
		for (int iG = 0; iG < nASGeom; iG++){
	    	  if(ASGeom[iG].parserieA>0){
	    		  ind=sequenciaAS[seq].GeomdA[konta1];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  saidaBHP[seq][coluna]=ASGeom[iG].dA[ind];
	    		  coluna++;
				  escreveIni <<ASGeom[iG].dA[ind]<< " ;";
	    		  konta1++;
	    	  }
	    	  if(ASGeom[iG].parserieB>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta2];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  saidaBHP[seq][coluna]=ASGeom[iG].dB[ind];
	    		  coluna++;
				  escreveIni <<ASGeom[iG].dB[ind]<< " ;";
	    		  konta2++;
	    	  }
	    	  if(ASGeom[iG].parserieRug>0){
	    		  ind=sequenciaAS[seq].GeomdB[konta3];
	    		  int igeo=ASGeom[iG].indGeom;
	    		  saidaBHP[seq][coluna]=ASGeom[iG].rug[ind];
	    		  coluna++;
				  escreveIni <<ASGeom[iG].rug[ind]<< " ;";
	    		  konta3++;
	    	  }
	      }
	}
	if(listaV.kequiv==1){
		int konta1=0;
		for (int ik = 0; ik < nASCondEquiv; ik++){
	    	  if(ASCondEquiv[ik].parserieK>0){
	    		  ind=sequenciaAS[seq].condEqui[konta1];
	    		  int imat=ASCondEquiv[ik].indMat;
	    		  saidaBHP[seq][coluna]=ASCondEquiv[ik].cond[ind];
	    		  coluna++;
				  escreveIni <<ASCondEquiv[ik].cond[ind]<< " ;";
	    		  konta1++;
	    	  }
	      }
	}
	if(listaV.vgasinj==1){
		if(ASGasInj.parseriePresInj>0){
			ind=sequenciaAS[seq].INJGpresinj;
			presiniG=ASGasInj.presinj[ind];
  		    saidaBHP[seq][coluna]=ASGasInj.presinj[ind];
  		    coluna++;
			escreveIni <<ASGasInj.presinj[ind]<< " ;";
		}
		if(ASGasInj.parserieTemp>0){
			ind=sequenciaAS[seq].INJGtemperatura;
			tempiniG=ASGasInj.temperatura[ind];
  		    saidaBHP[seq][coluna]=ASGasInj.temperatura[ind];
  		    coluna++;
			escreveIni <<ASGasInj.temperatura[ind]<< " ;";
		}
		if(ASGasInj.parserieVazGas>0){
			ind=sequenciaAS[seq].INJGvazgas;
			vazgasG=ASGasInj.vazgas[ind];
  		    saidaBHP[seq][coluna]=ASGasInj.vazgas[ind];
  		    coluna++;
			escreveIni << ASGasInj.vazgas[ind]<< " ;";
		}
	}
	if(listaV.vpresent==1){
	  	  if(ASpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].PEpres;
	  		presE=ASpEntrada.pres[ind];
  		    saidaBHP[seq][coluna]=presE;
  		    coluna++;
	  		escreveIni <<presE<< " ;";
	  	  }
	  	  if(ASpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].PEbet;
	  		betaE=ASpEntrada.bet[ind];
  		    saidaBHP[seq][coluna]=betaE;
  		    coluna++;
	  		escreveIni <<betaE<< " ;";
	  	  }
	  	  if(ASpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].PEtemperatura;
	  		tempE=ASpEntrada.temperatura[ind];
  		    saidaBHP[seq][coluna]=tempE;
  		    coluna++;
	  		escreveIni <<tempE<< " ;";
	  	  }
	  	  if(ASpEntrada.parserieTit>0){
	  		ind=sequenciaAS[seq].PEtit;
	  		titE=ASpEntrada.tit[ind];
  		    saidaBHP[seq][coluna]=titE;
  		    coluna++;
	  		escreveIni <<titE<< " ;";
	  	  }
	}
	if(listaV.vvazpresent==1){
	  	  if(ASvpEntrada.parseriePres>0){
	  		ind=sequenciaAS[seq].VPEpres;
	  		presE=ASvpEntrada.pres[ind];
  		    saidaBHP[seq][coluna]=presE;
  		    coluna++;
	  		escreveIni <<presE<< " ;";
	  	  }
	  	  if(ASvpEntrada.parserieBet>0){
	  		ind=sequenciaAS[seq].VPEbet;
	  		betaE=ASvpEntrada.bet[ind];
  		    saidaBHP[seq][coluna]=betaE;
  		    coluna++;
	  		escreveIni <<betaE<< " ;";
	  	  }
	  	  if(ASvpEntrada.parserieMass>0){
	  		ind=sequenciaAS[seq].VPEmass;
	  		vazE=ASvpEntrada.mass[ind];
  		    saidaBHP[seq][coluna]=vazE;
  		    coluna++;
	  		escreveIni <<vazE<< " ;";
	  	  }
	  	  if(ASvpEntrada.parserieTemp>0){
	  		ind=sequenciaAS[seq].VPEtemperatura;
	  		tempE=ASvpEntrada.temperatura[ind];
  		    saidaBHP[seq][coluna]=tempE;
  		    coluna++;
	  		escreveIni <<tempE<< " ;";
	  	  }
	}
	if(listaV.vchk==1){
	  	  if(ASCHK.parserieAbre>0){
	  		ind=sequenciaAS[seq].CHKabertura;
	  		indCHK=ind;
	  		chokeSup.AreaGarg=ASCHK.abertura[ind]*chokeSup.AreaTub;
  		    saidaBHP[seq][coluna]=ASCHK.abertura[ind];
  		    coluna++;
	  		escreveIni <<ASCHK.abertura[ind]<< " ;";
	  	  }
	  	  if(ASCHK.parserieCD>0){
	  		ind=sequenciaAS[seq].CHKcd;
	  		chokeSup.cdchk=ASCHK.cd[ind];
  		    saidaBHP[seq][coluna]=chokeSup.cdchk;
  		    coluna++;
	  		escreveIni <<chokeSup.cdchk<< " ;";
	  	  }
	}
	if(listaV.vpocinj==1){
	  	  if(ASPInj.parseriePresI>0){
	  		ind=sequenciaAS[seq].PIpresinj;
	  		presE=ASPInj.presfundo[ind];
  		    saidaBHP[seq][coluna]=presE;
  		    coluna++;
	  		escreveIni <<presE<< " ;";
	  	  }
	  	  if(ASPInj.parseriePresFundo>0){
	  		ind=sequenciaAS[seq].PIpresfundo;
	  		pGSup=ASPInj.presinj[ind];
  		    saidaBHP[seq][coluna]=pGSup;
  		    coluna++;
	  		escreveIni <<pGSup<< " ;";
	  	  }
	  	  if(ASPInj.parserieTemp>0){
	  		ind=sequenciaAS[seq].PItempinj;
	  		tempE=ASPInj.tempinj[ind];
  		    saidaBHP[seq][coluna]=tempE;
  		    coluna++;
	  		escreveIni <<tempE<< " ;";
	  	  }
	  	  if(ASPInj.parserieVaz>0){
	  		ind=sequenciaAS[seq].PIvazinj;
	  		vazE=ASPInj.vazinj[ind];
  		    saidaBHP[seq][coluna]=vazE;
  		    coluna++;
	  		escreveIni <<vazE<< " ;";
	  	  }
	}


	saidaBHP[seq][coluna]=BHP;
	escreveIni <<BHP<< "\n";
	escreveIni.close();


	//não deletar !!!
	/*double BHP00, BHP11, BHP22, BHP33;
	double BHP0, BHP1, BHP2, BHP3;
	if (vfp==0&&chrisao==0) BHP0=saidaBHP[seq][coluna];
	if (vfp==0&&chrisao==1) BHP1=saidaBHP[seq][coluna];
	if (vfp==0&&chrisao==2) BHP2=saidaBHP[seq][coluna];
	if (vfp==0&&chrisao==3) BHP3=saidaBHP[seq][coluna];

	//if (vfp==0&chrisao==0) escreveIni1 <<BHP<< " ";
	//if (vfp==0&chrisao==1) escreveIni1 <<BHP<< " ";
	//if (vfp==0&chrisao==2) escreveIni1 <<BHP<< " ";
	if (vfp==0&&chrisao==3) escreveIni1 <<BHP0<< " " << BHP1 << " " << BHP2 << " " << BHP3 << "\n";
	escreveIni1.close();


	if (vfp==1&&chrisao==0) BHP0=saidaBHP[seq][coluna];
	if (vfp==1&&chrisao==1) BHP1=saidaBHP[seq][coluna];
	if (vfp==1&&chrisao==2) BHP2=saidaBHP[seq][coluna];
	if (vfp==1&&chrisao==3) BHP3=saidaBHP[seq][coluna];
	//BHP00=BHP0;
	//cout << "BHP00" << BHP00 << endl;
	//cout << "BHP0" << BHP0 << endl;

	system("pause");
	//if (vfp==1&chrisao==0) escreveIni2 <<BHP<< " ";
	//if (vfp==1&chrisao==1) escreveIni2 <<BHP<< " ";
	//if (vfp==1&chrisao==2) escreveIni2 <<BHP<< " ";
	if (vfp==1&&chrisao==3) escreveIni2 <<BHP0<< " " << BHP1 << " " << BHP2 << " " << BHP3 << "\n";
	escreveIni2.close();*/

	//if (vfp==0&&chrisao==0) escreveIni1 <<BHP<< "   ";
	//if (vfp==0&&chrisao==1) escreveIni1 <<BHP<< "   ";
	//if (vfp==0&&chrisao==2) escreveIni1 <<BHP<< "   ";
	//if (vfp==0&&chrisao==3) escreveIni1 <<BHP<< "   /" << "\n";
	//escreveIni1.close();

	//if (vfp == 1) {
		//int contador=0;




	chrisao++;

	//if (chrisao==4) chrisao=0;

	if (vfp == 0) {

	   for (int cont = 0; cont < ASPsep.parseriePres; ++cont) { //< ASFonLiq->parserieVL
		// for (int cont = 0; cont < ASPsep.parseriePres; ++cont) { //< ASFonLiq->parserieVL

	        //escreveIni1 << ASFonLiq->vazliq[chrisao];
	    	if (chrisao==cont) escreveIni1 << BHP << "   ";
	    	//contador=contador+1;
	      //  if (i < ASFonLiq->parserieVL - 1) {
	       //     escreveIni1 << " ";
	       // }
	    }

	   if (chrisao == ASPsep.parseriePres) escreveIni1 << BHP <<  "   " << "\n";
		if (chrisao==ASPsep.parseriePres) chrisao=0;
	//	 if (chrisao == ASPsep.parseriePres) escreveIni1 << BHP <<  "   " << "\n";
		//		if (chrisao == ASPsep.parseriePres) chrisao=0;

	    //escreveIni2 << "   /" << "\n";
		//}
		/*if (vfp==1&&chrisao==0) escreveIni2 <<BHP<< "   ";
		if (vfp==1&&chrisao==1) escreveIni2 <<BHP<< "   ";
		if (vfp==1&&chrisao==2) escreveIni2 <<BHP<< "   ";
		if (vfp==1&&chrisao==3) escreveIni2 <<BHP<< "   /" << "\n"; */

		//escreveIni1.close();

	}
	//escreveIni1 << "/" << endl;
	escreveIni1.close();


	if (vfp == 1) {

	   for (int cont = 0; cont < ASFonLiq->parserieVL; ++cont) { //< ASFonLiq->parserieVL
	  // for (int cont = 0; cont < ASPsep.parseriePres; ++cont) { //< ASFonLiq->parserieVL

	        //escreveIni1 << ASFonLiq->vazliq[chrisao];
	    	if (chrisao==cont) escreveIni2 << BHP << "   ";
	    	//contador=contador+1;
	      //  if (i < ASFonLiq->parserieVL - 1) {
	       //     escreveIni1 << " ";
	       // }
	    }

	   if (chrisao == ASFonLiq->parserieVL) escreveIni2 << BHP <<  "   /" << "\n";
	   if (chrisao==ASFonLiq->parserieVL) chrisao=0;

	    //escreveIni2 << "   /" << "\n";
		//}
		/*if (vfp==1&&chrisao==0) escreveIni2 <<BHP<< "   ";
		if (vfp==1&&chrisao==1) escreveIni2 <<BHP<< "   ";
		if (vfp==1&&chrisao==2) escreveIni2 <<BHP<< "   ";
		if (vfp==1&&chrisao==3) escreveIni2 <<BHP<< "   /" << "\n"; */

		//escreveIni2.close();

	}
	escreveIni2.close();

	//cout << chrisao << " " << ind << endl;
	//system("pause");


}
