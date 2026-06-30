/*
 * LerRede.cpp
 *
 *  Created on: 23 de jun de 2017
 *      Author: Eduardo
 */
#include "LerAP.h"

using namespace rapidjson;

int chrisao = 0;

APara::APara(varGlob1D *Vvg1dSP, const string IMPFILE, int vncel, detcelp *vcelp, ProFlu *vflup, detBCS *vbcs, detFONGAS *vfonteg) {
    entrada = IMPFILE;
    vg1dSP = Vvg1dSP;
    ncel = vncel;
    celp = vcelp;
    flup = vflup;
    bcs = vbcs;
    fonteg = vfonteg;
    lerArq();
}

APara::APara(const APara &vAP) {
    entrada = vAP.entrada;
    vg1dSP = vAP.vg1dSP;
    ncel = vAP.ncel;
    celp = vAP.celp;
    flup = vAP.flup;
    bcs = vAP.bcs;
    fonteg = vAP.fonteg;
    lerArq();
}

APara &APara::operator=(const APara &vAP) {
    if (this != &vAP) {
        if (listaV.vbcs == 1) {
            delete[] APBCS;
        }

        if (listaV.vipr == 1) {
            delete[] APIPR;
        }

        if (listaV.vfonliq == 1) {
            delete[] APFonLiq;
        }

        if (listaV.vfongas == 1) {
            delete[] APFonGas;
        }

        if (listaV.vfonmas == 1) {
            delete[] APFonMas;
        }

        if (listaV.vvalv == 1) {
            delete APValv;
        }

        if (listaV.vbvol == 1) {
            delete[] APBVOL;
        }

        if (listaV.vdp == 1) {
            delete[] APDP;
        }

        if (listaV.vdpH == 1) {
            delete[] APdPdLH;
        }

        if (listaV.vdpF == 1) {
            delete[] APdPdLF;
        }

        if (listaV.vdt == 1) {
            delete[] APdTdL;
        }

        if (listaV.vfuro == 1) {
            delete[] APFuro;
        }

        if (listaV.diam == 1) {
            delete[] APGeom;
        }

        if (listaV.kequiv == 1) {
            delete[] APCondEquiv;
        }

        if (nVariaveis > 1) {
            delete[] sequenciaAP;
            delete[] genericoAP;
            delete[] vecParSerie;
        }

        if (nVariaveis > 1 && dim > 0) {
            for (int i = 0; i < nVariaveis; i++) {
                delete[] saidaBHP[i];
                delete[] saidaVazLiq[i];
            }
            delete[] saidaBHP;
            delete[] saidaVazLiq;
        }

        entrada = vAP.entrada;
        vg1dSP = vAP.vg1dSP;
        ncel = vAP.ncel;
        celp = vAP.celp;
        flup = vAP.flup;
        bcs = vAP.bcs;
        fonteg = vAP.fonteg;
        lerArq();
    }
    return *this;
}

Document APara::parseEntrada() {
    // criar string para mensagem de falha
    char mensagemFalha[5000];
    // declarar o documento raiz do arquivo entrada
    Document jsonDoc;
    // definir arquivo Rede de entrada da simulação
    FILE *APInFile = NULL;
    // definir buffer de entrada da simulação
    char APInBuf[65536];
    // realizar a leitura do arquivo de rede
    try {
        // atualizar a estrutura de resultado do parse do arquivo de entrada da simulação
        logger.setNomeArqEntrada(entrada);
        // abrir arquivo de entrada da simulação
        APInFile = fopen(entrada.c_str(), "r");
        // criar stream para o arquivo de entrada da simulação
        FileReadStream APInStream(APInFile, APInBuf, sizeof(APInBuf));
        // realizar o parse de AP de entrada
        jsonDoc.ParseStream(APInStream);
        // fechar o arquivo
        fclose(APInFile);
        // caso parse de AP de entrada com erro
        if (jsonDoc.HasParseError()) {
            // transpor os dados  para a mensagem
            sprintf(mensagemFalha, "Posicao = %zu", jsonDoc.GetErrorOffset());
            // incluir falha no processo de parse
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_JSON_FORMAT_VALIDATION, "Verificar formato JSON de Analise Parametrica", mensagemFalha,
                       GetParseError_Pt_BR(jsonDoc.GetParseError()), 0);
            // gerar o arquivo de saida da simulacao
            logger.writeOutputLog();
            // encerrar a aplicacao
            exit(EXIT_SUCCESS);
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Verificar formato JSON de Analise Parametrica", "", e.what());
        // gerar o arquivo de saida da simulacao
        logger.writeOutputLog();
        // encerrar a aplicacao
        exit(EXIT_SUCCESS);
    }
    // incluir info de conclusao do parse de AP no log
    logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED, "Verificar formato JSON de Analise de Parametrica", "", "SUCESSO");
    // retorna o documento de AP
    return jsonDoc;
}

void APara::parse_variaveis(Value &variaveis_json) {
    nVariaveis = 1;
    tipoAP = 0;
    vfp = 0;
    listaV.vbcs = 0;
    listaV.vbvol = 0;
    listaV.vchk = 0;
    listaV.vdp = 0;
    listaV.vdpH = 0;
    listaV.vdpF = 0;
    listaV.vdt = 0;
    listaV.vfongas = 0;
    listaV.vfonliq = 0;
    listaV.vfonmas = 0;
    listaV.vfuro = 0;
    listaV.vgasinj = 0;
    listaV.vipr = 0;
    listaV.vpocinj = 0;
    listaV.vpresent = 0;
    listaV.vpsep = 0;
    listaV.vvalv = 0;
    listaV.vvazpresent = 0;
    listaV.diam = 0;
    listaV.kequiv = 0;
}

void APara::parse_IPR(Value &IPR_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/IPR");

    nAPIPR = 0;
    int total = 0;
    APIPR = 0;
    try {
        nAPIPR = IPR_json.Size();
        if (nAPIPR < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'IPR-AP' possui menos de um elemento",
                       "#/IPR", "Quantidade de IPR na AP = " + to_string(nAPIPR));
        } else {
            APIPR = new detIPRAP[nAPIPR];
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                APIPR[iIPR].indIPR = IPR_json[iIPR]["indiceIPR"].GetInt();
                if (IPR_json[iIPR].HasMember("presEstatica")) {
                    dim++;
                    APIPR[iIPR].parseriePres = IPR_json[iIPR]["presEstatica"].Size();
                    for (int ipesta = 0; ipesta < APIPR[iIPR].parseriePres; ipesta++) {
                        APIPR[iIPR].pres.push_back(IPR_json[iIPR]["presEstatica"][ipesta].GetDouble());
                    }
                    sort(APIPR[iIPR].pres.begin(), APIPR[iIPR].pres.end());
                } else
                    APIPR[iIPR].parseriePres = 0;
                if (APIPR[iIPR].parseriePres > 0)
                    nVariaveis *= APIPR[iIPR].parseriePres;
                if (IPR_json[iIPR].HasMember("tempRes")) {
                    dim++;
                    APIPR[iIPR].parserieTemp = IPR_json[iIPR]["tempRes"].Size();
                    for (int ipesta = 0; ipesta < APIPR[iIPR].parserieTemp; ipesta++) {
                        APIPR[iIPR].temp.push_back(IPR_json[iIPR]["tempRes"][ipesta].GetDouble());
                    }
                } else
                    APIPR[iIPR].parserieTemp = 0;
                if (APIPR[iIPR].parserieTemp > 0)
                    nVariaveis *= APIPR[iIPR].parserieTemp;
                if (IPR_json[iIPR].HasMember("IP")) {
                    dim++;
                    APIPR[iIPR].parserieIP = IPR_json[iIPR]["IP"].Size();
                    for (int ipesta = 0; ipesta < APIPR[iIPR].parserieIP; ipesta++) {
                        APIPR[iIPR].ip.push_back(IPR_json[iIPR]["IP"][ipesta].GetDouble());
                    }
                    sort(APIPR[iIPR].ip.begin(), APIPR[iIPR].ip.end());
                } else
                    APIPR[iIPR].parserieIP = 0;
                if (APIPR[iIPR].parserieIP > 0)
                    nVariaveis *= APIPR[iIPR].parserieIP;
                if (IPR_json[iIPR].HasMember("qMax")) {
                    dim++;
                    APIPR[iIPR].parserieqMax = IPR_json[iIPR]["qMax"].Size();
                    for (int ipesta = 0; ipesta < APIPR[iIPR].parserieqMax; ipesta++) {
                        APIPR[iIPR].qMax.push_back(IPR_json[iIPR]["qMax"][ipesta].GetDouble());
                    }
                    sort(APIPR[iIPR].qMax.begin(), APIPR[iIPR].qMax.end());
                } else
                    APIPR[iIPR].parserieqMax = 0;
                if (IPR_json[iIPR].HasMember("II")) {
                    dim++;
                    APIPR[iIPR].parserieJP = IPR_json[iIPR]["II"].Size();
                    for (int ipesta = 0; ipesta < APIPR[iIPR].parserieJP; ipesta++) {
                        APIPR[iIPR].jp.push_back(IPR_json[iIPR]["II"][ipesta].GetDouble());
                    }
                    sort(APIPR[iIPR].jp.begin(), APIPR[iIPR].jp.end());
                } else
                    APIPR[iIPR].parserieJP = 0;
                if (APIPR[iIPR].parserieJP > 0)
                    nVariaveis *= APIPR[iIPR].parserieJP;
                if (IPR_json[iIPR].HasMember("fluido")) {
                    dim++;
                    APIPR[iIPR].parserieFlup = IPR_json[iIPR]["fluido"].Size();
                    for (int ipesta = 0; ipesta < APIPR[iIPR].parserieFlup; ipesta++) {
                        APIPR[iIPR].indfluP.push_back(IPR_json[iIPR]["fluido"][ipesta].GetInt());
                    }
                } else
                    APIPR[iIPR].parserieFlup = 0;
                if (APIPR[iIPR].parserieFlup > 0)
                    nVariaveis *= APIPR[iIPR].parserieFlup;

                total = APIPR[iIPR].parserieIP + APIPR[iIPR].parserieFlup + APIPR[iIPR].parserieJP +
                        APIPR[iIPR].parseriePres + APIPR[iIPR].parserieTemp + APIPR[iIPR].parserieqMax;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'IPR-AP' sem variavel fluido ou IP ou II ou presestatica ou t reservatorio ou qmax",
                               "#/IPR", "Indice de IPR na AP = " + to_string(iIPR));
                }
            }
            listaV.vipr = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_FonLiq(Value &FonLiq_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/FonteLiquido");

    nAPFL = 0;
    APFonLiq = 0;
    int total = 0;
    try {
        nAPFL = FonLiq_json.Size();
        if (nAPFL < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'FonteLiquido-AP' possui menos de um elemento",
                       "#/FonteLiquido", "Quantidade de Fonte Liquido na AP = " + to_string(nAPFL));
        } else {
            APFonLiq = new detFONLIQAP[nAPFL];
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                APFonLiq[iFL].indFL = FonLiq_json[iFL]["indiceFL"].GetInt();
                if (FonLiq_json[iFL].HasMember("temperatura")) {
                    dim++;
                    APFonLiq[iFL].parserieTemp = FonLiq_json[iFL]["temperatura"].Size();
                    for (int ifreq = 0; ifreq < APFonLiq[iFL].parserieTemp; ifreq++) {
                        APFonLiq[iFL].temp.push_back(FonLiq_json[iFL]["temperatura"][ifreq].GetDouble());
                    }
                    sort(APFonLiq[iFL].temp.begin(), APFonLiq[iFL].temp.end());
                } else
                    APFonLiq[iFL].parserieTemp = 0;
                if (APFonLiq[iFL].parserieTemp > 0)
                    nVariaveis *= APFonLiq[iFL].parserieTemp;
                if (FonLiq_json[iFL].HasMember("vazLiq")) {
                    dim++;
                    APFonLiq[iFL].parserieVL = FonLiq_json[iFL]["vazLiq"].Size();
                    for (int ifreq = 0; ifreq < APFonLiq[iFL].parserieVL; ifreq++) {
                        APFonLiq[iFL].vazliq.push_back(FonLiq_json[iFL]["vazLiq"][ifreq].GetDouble());
                    }
                    sort(APFonLiq[iFL].vazliq.begin(), APFonLiq[iFL].vazliq.end());
                } else
                    APFonLiq[iFL].parserieVL = 0;
                if (APFonLiq[iFL].parserieVL > 0)
                    nVariaveis *= APFonLiq[iFL].parserieVL;
                if (FonLiq_json[iFL].HasMember("beta")) {
                    dim++;
                    APFonLiq[iFL].parserieBet = FonLiq_json[iFL]["beta"].Size();
                    for (int ifreq = 0; ifreq < APFonLiq[iFL].parserieBet; ifreq++) {
                        APFonLiq[iFL].bet.push_back(FonLiq_json[iFL]["beta"][ifreq].GetDouble());
                    }
                    sort(APFonLiq[iFL].bet.begin(), APFonLiq[iFL].bet.end());
                } else
                    APFonLiq[iFL].parserieBet = 0;
                if (APFonLiq[iFL].parserieBet > 0)
                    nVariaveis *= APFonLiq[iFL].parserieBet;
                if (FonLiq_json[iFL].HasMember("fluido")) {
                    dim++;
                    APFonLiq[iFL].parserieFlu = FonLiq_json[iFL]["fluido"].Size();
                    for (int ifreq = 0; ifreq < APFonLiq[iFL].parserieFlu; ifreq++) {
                        APFonLiq[iFL].indfluP.push_back(FonLiq_json[iFL]["fluido"][ifreq].GetInt());
                    }
                } else
                    APFonLiq[iFL].parserieFlu = 0;
                if (APFonLiq[iFL].parserieFlu > 0)
                    nVariaveis *= APFonLiq[iFL].parserieFlu;
                total = APFonLiq[iFL].parserieTemp + APFonLiq[iFL].parserieBet + APFonLiq[iFL].parserieVL + APFonLiq[iFL].parserieFlu;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'FonteLiq-AP' sem variavel temperatura ou vazão de líquido, beta ou fluido",
                               "#/FonteLiq", "Indice de FonteLiq na AP = " + to_string(iFL));
                }
            }
            listaV.vfonliq = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_FonGas(Value &FonGas_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/FonteGas");

    nAPFG = 0;
    APFonGas = 0;
    int total = 0;
    try {
        nAPFG = FonGas_json.Size();
        if (nAPFG < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'FonteGas-AP' possui menos de um elemento",
                       "#/FonteGas", "Quantidade de Fonte Gas na AP = " + to_string(nAPFG));
        } else {
            APFonGas = new detFONGASAP[nAPFG];
            for (int iFG = 0; iFG < nAPFG; iFG++) {
                APFonGas[iFG].indFG = FonGas_json[iFG]["indiceFG"].GetInt();
                if (FonGas_json[iFG].HasMember("temperatura")) {
                    dim++;
                    APFonGas[iFG].parserieTemp = FonGas_json[iFG]["temperatura"].Size();
                    for (int ifreq = 0; ifreq < APFonGas[iFG].parserieTemp; ifreq++) {
                        APFonGas[iFG].temp.push_back(FonGas_json[iFG]["temperatura"][ifreq].GetDouble());
                    }
                    sort(APFonGas[iFG].temp.begin(), APFonGas[iFG].temp.end());
                } else
                    APFonGas[iFG].parserieTemp = 0;
                if (APFonGas[iFG].parserieTemp > 0)
                    nVariaveis *= APFonGas[iFG].parserieTemp;
                if (FonGas_json[iFG].HasMember("vazGas")) {
                    dim++;
                    APFonGas[iFG].parserieVazG = FonGas_json[iFG]["vazGas"].Size();
                    for (int ifreq = 0; ifreq < APFonGas[iFG].parserieVazG; ifreq++) {
                        APFonGas[iFG].vazgas.push_back(FonGas_json[iFG]["vazGas"][ifreq].GetDouble());
                    }
                    sort(APFonGas[iFG].vazgas.begin(), APFonGas[iFG].vazgas.end());
                } else
                    APFonGas[iFG].parserieVazG = 0;
                if (APFonGas[iFG].parserieVazG > 0)
                    nVariaveis *= APFonGas[iFG].parserieVazG;
                if (FonGas_json[iFG].HasMember("vazComp")) {
                    dim++;
                    APFonGas[iFG].parserieVazC = FonGas_json[iFG]["vazComp"].Size();
                    for (int ifreq = 0; ifreq < APFonGas[iFG].parserieVazC; ifreq++) {
                        APFonGas[iFG].vazcomp.push_back(FonGas_json[iFG]["vazComp"][ifreq].GetDouble());
                    }
                    sort(APFonGas[iFG].vazcomp.begin(), APFonGas[iFG].vazcomp.end());
                } else
                    APFonGas[iFG].parserieVazC = 0;
                if (APFonGas[iFG].parserieVazC > 0)
                    nVariaveis *= APFonGas[iFG].parserieVazC;
                total = APFonGas[iFG].parserieTemp + APFonGas[iFG].parserieVazC + APFonGas[iFG].parserieVazG;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'FonteGas-AP' sem variavel temperatura ou vazão de gás ou vazão complementar",
                               "#/FonteGas", "Indice de FonteGas na AP = " + to_string(iFG));
                }
            }
            listaV.vfongas = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_FonMas(Value &FonMas_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/FonteMassa");

    nAPFM = 0;
    APFonMas = 0;
    try {
        nAPFM = FonMas_json.Size();
        if (nAPFM < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'FonteMass-AP' possui menos de um elemento",
                       "#/FonteMass", "Quantidade de Fonte Massa na AP = " + to_string(nAPFM));
        } else {
            APFonMas = new detFONMASSAP[nAPFM];
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                APFonMas[iFM].indFM = FonMas_json[iFM]["indiceFM"].GetInt();
                if (FonMas_json[iFM].HasMember("temperatura")) {
                    dim++;
                    APFonMas[iFM].parserieTemp = FonMas_json[iFM]["temperatura"].Size();
                    for (int ifreq = 0; ifreq < APFonMas[iFM].parserieTemp; ifreq++) {
                        APFonMas[iFM].temp.push_back(FonMas_json[iFM]["temperatura"][ifreq].GetDouble());
                    }
                    sort(APFonMas[iFM].temp.begin(), APFonMas[iFM].temp.end());
                } else
                    APFonMas[iFM].parserieTemp = 0;
                if (APFonMas[iFM].parserieTemp > 0)
                    nVariaveis *= APFonMas[iFM].parserieTemp;
                if (FonMas_json[iFM].HasMember("vazaoProd")) {
                    dim++;
                    APFonMas[iFM].parserieMP = FonMas_json[iFM]["vazaoProd"].Size();
                    for (int ifreq = 0; ifreq < APFonMas[iFM].parserieMP; ifreq++) {
                        APFonMas[iFM].vazMasP.push_back(FonMas_json[iFM]["vazaoTotal"][ifreq].GetDouble());
                    }
                    sort(APFonMas[iFM].vazMasP.begin(), APFonMas[iFM].vazMasP.end());
                } else
                    APFonMas[iFM].parserieMP = 0;
                if (APFonMas[iFM].parserieMP > 0)
                    nVariaveis *= APFonMas[iFM].parserieMP;
                if (FonMas_json[iFM].HasMember("VazaoComp")) {
                    dim++;
                    APFonMas[iFM].parserieMC = FonMas_json[iFM]["VazaoComp"].Size();
                    for (int ifreq = 0; ifreq < APFonMas[iFM].parserieMC; ifreq++) {
                        APFonMas[iFM].vazMasC.push_back(FonMas_json[iFM]["VazaoComp"][ifreq].GetDouble());
                    }
                    sort(APFonMas[iFM].vazMasC.begin(), APFonMas[iFM].vazMasC.end());
                } else
                    APFonMas[iFM].parserieMC = 0;
                if (APFonMas[iFM].parserieMC > 0)
                    nVariaveis *= APFonMas[iFM].parserieMC;
                if (FonMas_json[iFM].HasMember("VazaoGas")) {
                    dim++;
                    APFonMas[iFM].parserieMG = FonMas_json[iFM]["VazaoGas"].Size();
                    for (int ifreq = 0; ifreq < APFonMas[iFM].parserieMG; ifreq++) {
                        APFonMas[iFM].vazMasG.push_back(FonMas_json[iFM]["VazaoGas"][ifreq].GetDouble());
                    }
                    sort(APFonMas[iFM].vazMasG.begin(), APFonMas[iFM].vazMasG.end());
                } else
                    APFonMas[iFM].parserieMG = 0;
                if (APFonMas[iFM].parserieMG > 0)
                    nVariaveis *= APFonMas[iFM].parserieMG;
                if (FonMas_json[iFM].HasMember("fluido")) {
                    dim++;
                    APFonMas[iFM].parserieFlu = FonMas_json[iFM]["fluido"].Size();
                    for (int ifreq = 0; ifreq < APFonLiq[iFM].parserieFlu; ifreq++) {
                        APFonMas[iFM].indfluP.push_back(FonMas_json[iFM]["fluido"][ifreq].GetInt());
                    }
                } else
                    APFonMas[iFM].parserieFlu = 0;
                if (APFonMas[iFM].parserieFlu > 0)
                    nVariaveis *= APFonMas[iFM].parserieFlu;
                int total = APFonMas[iFM].parserieTemp + APFonMas[iFM].parserieMP + APFonMas[iFM].parserieMC + APFonMas[iFM].parserieMG + APFonMas[iFM].parserieFlu;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'FonteMas-AP' sem variavel temperatura ou vazão tptal, ou complementar ou gas ou fluido",
                               "#/FonteMas", "Indice de FonteMas na AP = " + to_string(iFM));
                }
            }
            listaV.vfonmas = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_BCS(Value &BCS_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/BCS");

    nAPBCS = 0;
    APBCS = 0;
    try {
        nAPBCS = BCS_json.Size();
        if (nAPBCS < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'BCS-AP' possui menos de um elemento",
                       "#/BCS", "Quantidade de BCS na AP = " + to_string(nAPBCS));
        } else {
            APBCS = new detBCSAP[nAPBCS];
            for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
                APBCS[indBCS].indBCS = BCS_json[indBCS]["indiceBCS"].GetInt();
                if (BCS_json[indBCS].HasMember("frequencia")) {
                    dim++;
                    APBCS[indBCS].parserieFreq = BCS_json[indBCS]["frequencia"].Size();
                    for (int ifreq = 0; ifreq < APBCS[indBCS].parserieFreq; ifreq++) {
                        APBCS[indBCS].freq.push_back(BCS_json[indBCS]["frequencia"][ifreq].GetDouble());
                    }
                    sort(APBCS[indBCS].freq.begin(), APBCS[indBCS].freq.end());
                } else
                    APBCS[indBCS].parserieFreq = 0;
                if (APBCS[indBCS].parserieFreq > 0)
                    nVariaveis *= APBCS[indBCS].parserieFreq;
                if (BCS_json[indBCS].HasMember("estagio")) {
                    dim++;
                    APBCS[indBCS].parserieEstag = BCS_json[indBCS]["estagio"].Size();
                    for (int iestag = 0; iestag < APBCS[indBCS].parserieEstag; iestag++) {
                        APBCS[indBCS].nestag.push_back(BCS_json[indBCS]["estagio"][iestag].GetInt());
                    }
                    sort(APBCS[indBCS].nestag.begin(), APBCS[indBCS].nestag.end());
                } else
                    APBCS[indBCS].parserieEstag = 0;
                if (APBCS[indBCS].parserieEstag > 0)
                    nVariaveis *= APBCS[indBCS].parserieEstag;
                int total = APBCS[indBCS].parserieFreq + APBCS[indBCS].parserieEstag;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'BCS-AP' sem variavel estagio ou frequencia",
                               "#/BCS", "Indice de BCS na AP = " + to_string(indBCS));
                }
            }
            listaV.vbcs = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_BVol(Value &BVol_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/BVol");

    nAPBV = 0;
    APBVOL = 0;
    try {
        nAPBV = BVol_json.Size();
        if (nAPBV < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'BVol-AP' possui menos de um elemento",
                       "#/BVol", "Quantidade de BVol na AP = " + to_string(nAPBV));
        } else {
            APBVOL = new detBVOLAP[nAPBV];
            for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
                APBVOL[indBCS].indBV = BVol_json[indBCS]["indiceBVol"].GetInt();
                if (BVol_json[indBCS].HasMember("frequencia")) {
                    dim++;
                    APBVOL[indBCS].parserieFreq = BVol_json[indBCS]["frequencia"].Size();
                    for (int ifreq = 0; ifreq < APBVOL[indBCS].parserieFreq; ifreq++) {
                        APBVOL[indBCS].freq.push_back(BVol_json[indBCS]["frequencia"][ifreq].GetDouble());
                    }
                    sort(APBVOL[indBCS].freq.begin(), APBVOL[indBCS].freq.end());
                } else
                    APBVOL[indBCS].parserieFreq = 0;
                if (APBVOL[indBCS].parserieFreq > 0)
                    nVariaveis *= APBVOL[indBCS].parserieFreq;
                if (BVol_json[indBCS].HasMember("capacidade")) {
                    dim++;
                    APBVOL[indBCS].parserieCap = BVol_json[indBCS]["capacidade"].Size();
                    for (int iestag = 0; iestag < APBVOL[indBCS].parserieCap; iestag++) {
                        APBVOL[indBCS].capacidade.push_back(BVol_json[indBCS]["capacidade"][iestag].GetDouble());
                    }
                    sort(APBVOL[indBCS].capacidade.begin(), APBVOL[indBCS].capacidade.end());
                } else
                    APBVOL[indBCS].parserieCap = 0;
                if (APBVOL[indBCS].parserieCap > 0)
                    nVariaveis *= APBVOL[indBCS].parserieCap;
                if (BVol_json[indBCS].HasMember("fatorPoli")) {
                    dim++;
                    APBVOL[indBCS].parserieNPoli = BVol_json[indBCS]["fatorPoli"].Size();
                    for (int iestag = 0; iestag < APBVOL[indBCS].parserieNPoli; iestag++) {
                        APBVOL[indBCS].npoli.push_back(BVol_json[indBCS]["fatorPoli"][iestag].GetDouble());
                    }
                    sort(APBVOL[indBCS].npoli.begin(), APBVOL[indBCS].npoli.end());
                } else
                    APBVOL[indBCS].parserieNPoli = 0;
                if (APBVOL[indBCS].parserieNPoli > 0)
                    nVariaveis *= APBVOL[indBCS].parserieNPoli;
                int total = APBVOL[indBCS].parserieFreq + APBVOL[indBCS].parserieCap + APBVOL[indBCS].parserieNPoli;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'BVOL-AP' sem variavel capacidade ou frequencia ou fator politropico",
                               "#/BVOL", "Indice de BVOL na AP = " + to_string(indBCS));
                }
            }
            listaV.vbvol = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_DP(Value &DP_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/DP");

    nAPDP = 0;
    APDP = 0;
    try {
        nAPDP = DP_json.Size();
        if (nAPDP < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'DP-AP' possui menos de um elemento",
                       "#/DP", "Quantidade de DP na AP = " + to_string(nAPDP));
        } else {
            APDP = new detDPREQAP[nAPDP];
            for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
                APDP[indBCS].indDP = DP_json[indBCS]["indiceDP"].GetInt();
                if (DP_json[indBCS].HasMember("deltapressao")) {
                    dim++;
                    APDP[indBCS].parserieDP = DP_json[indBCS]["deltapressao"].Size();
                    for (int ifreq = 0; ifreq < APDP[indBCS].parserieDP; ifreq++) {
                        APDP[indBCS].dp.push_back(DP_json[indBCS]["deltapressao"][ifreq].GetDouble());
                    }
                    sort(APDP[indBCS].dp.begin(), APDP[indBCS].dp.end());
                } else
                    APDP[indBCS].parserieDP = 0;
                if (APDP[indBCS].parserieDP > 0)
                    nVariaveis *= APDP[indBCS].parserieDP;
                int total = APDP[indBCS].parserieDP;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'DP-AP' sem variavel delpres",
                               "#/DP", "Indice de DP na AP = " + to_string(indBCS));
                }
            }
            listaV.vdp = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_dPdLH(Value &dPdLHidro_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/dPdLHidro");

    nAPdPdLH = 0;
    APdPdLH = 0;
    try {
        nAPdPdLH = dPdLHidro_json.Size();
        if (nAPdPdLH < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'dPdLHidro-AP' possui menos de um elemento",
                       "#/dPdLHidro", "Quantidade de dPdLHidro na AP = " + to_string(nAPdPdLH));
        } else {
            APdPdLH = new detDPHidro[nAPdPdLH];
            for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
                APdPdLH[indBCS].indDPHidro = dPdLHidro_json[indBCS]["indicedPdLHidro"].GetInt();
                if (dPdLHidro_json[indBCS].HasMember("fator")) {
                    dim++;
                    APdPdLH[indBCS].parserieDPHidro = dPdLHidro_json[indBCS]["fator"].Size();
                    for (int ifreq = 0; ifreq < APdPdLH[indBCS].parserieDPHidro; ifreq++) {
                        APdPdLH[indBCS].dPHidro.push_back(dPdLHidro_json[indBCS]["fator"][ifreq].GetDouble());
                    }
                    sort(APdPdLH[indBCS].dPHidro.begin(), APdPdLH[indBCS].dPHidro.end());
                } else
                    APdPdLH[indBCS].parserieDPHidro = 0;
                if (APdPdLH[indBCS].parserieDPHidro > 0)
                    nVariaveis *= APdPdLH[indBCS].parserieDPHidro;
                int total = APdPdLH[indBCS].parserieDPHidro;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'dPdLHidro-AP' sem variavel de correcao",
                               "#/dPdLHidro", "Indice de dPdLHidro na AP = " + to_string(indBCS));
                }
            }
            listaV.vdpH = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_dPdLF(Value &dPdLFric_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/dPdLFric");

    nAPdPdLF = 0;
    APdPdLF = 0;
    try {
        nAPdPdLF = dPdLFric_json.Size();
        if (nAPdPdLF < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'dPdLFric-AP' possui menos de um elemento",
                       "#/dPdLFric", "Quantidade de dPdLFric na AP = " + to_string(nAPdPdLF));
        } else {
            APdPdLF = new detDPFric[nAPdPdLF];
            for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
                APdPdLF[indBCS].indDPFric = dPdLFric_json[indBCS]["indicedPdLFric"].GetInt();
                if (dPdLFric_json[indBCS].HasMember("fator")) {
                    dim++;
                    APdPdLF[indBCS].parserieDPFric = dPdLFric_json[indBCS]["fator"].Size();
                    for (int ifreq = 0; ifreq < APdPdLF[indBCS].parserieDPFric; ifreq++) {
                        APdPdLF[indBCS].dPFric.push_back(dPdLFric_json[indBCS]["fator"][ifreq].GetDouble());
                    }
                    sort(APdPdLF[indBCS].dPFric.begin(), APdPdLF[indBCS].dPFric.end());
                } else
                    APdPdLF[indBCS].parserieDPFric = 0;
                if (APdPdLF[indBCS].parserieDPFric > 0)
                    nVariaveis *= APdPdLF[indBCS].parserieDPFric;
                int total = APdPdLF[indBCS].parserieDPFric;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'dPdLFric-AP' sem variavel de correcao",
                               "#/dPdLFric", "Indice de dPdLFric na AP = " + to_string(indBCS));
                }
            }
            listaV.vdpF = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_dTdL(Value &dTdL_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/dTdL");

    nAPdTdL = 0;
    APdTdL = 0;
    try {
        nAPdTdL = dTdL_json.Size();
        if (nAPdTdL < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'dTdL-AP' possui menos de um elemento",
                       "#/dTdL", "Quantidade de dTdL na AP = " + to_string(nAPdTdL));
        } else {
            APdTdL = new detDT[nAPdTdL];
            for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
                APdTdL[indBCS].indDT = dTdL_json[indBCS]["indicedTdL"].GetInt();
                if (dTdL_json[indBCS].HasMember("fator")) {
                    dim++;
                    APdTdL[indBCS].parserieDT = dTdL_json[indBCS]["fator"].Size();
                    for (int ifreq = 0; ifreq < APdTdL[indBCS].parserieDT; ifreq++) {
                        APdTdL[indBCS].dT.push_back(dTdL_json[indBCS]["fator"][ifreq].GetDouble());
                    }
                    sort(APdTdL[indBCS].dT.begin(), APdTdL[indBCS].dT.end());
                } else
                    APdTdL[indBCS].parserieDT = 0;
                if (APdTdL[indBCS].parserieDT > 0)
                    nVariaveis *= APdTdL[indBCS].parserieDT;
                int total = APdTdL[indBCS].parserieDT;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'dTdL-AP' sem variavel de correcao",
                               "#/dTdL", "Indice de dTdL na AP = " + to_string(indBCS));
                }
            }
            listaV.vdt = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_Valv(Value &Valv_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/Valvula");

    nAPV = 0;
    APValv = 0;
    try {
        nAPV = Valv_json.Size();
        if (nAPV < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'Valvula-AP' possui menos de um elemento",
                       "#/Valvula", "Quantidade de Valvula na AP = " + to_string(nAPV));
        } else {
            APValv = new detValvAP[nAPV];
            for (int indBCS = 0; indBCS < nAPV; indBCS++) {
                APValv[indBCS].indV = Valv_json[indBCS]["indiceValvula"].GetInt();
                if (Valv_json[indBCS].HasMember("abertura")) {
                    dim++;
                    APValv[indBCS].parserieAbre = Valv_json[indBCS]["abertura"].Size();
                    for (int ifreq = 0; ifreq < APValv[indBCS].parserieAbre; ifreq++) {
                        APValv[indBCS].abertura.push_back(Valv_json[indBCS]["abertura"][ifreq].GetDouble());
                    }
                    sort(APValv[indBCS].abertura.begin(), APValv[indBCS].abertura.end());
                } else
                    APValv[indBCS].parserieAbre = 0;
                if (APValv[indBCS].parserieAbre > 0)
                    nVariaveis *= APValv[indBCS].parserieAbre;
                if (Valv_json[indBCS].HasMember("CD")) {
                    dim++;
                    APValv[indBCS].parserieCD = Valv_json[indBCS]["CD"].Size();
                    for (int ifreq = 0; ifreq < APValv[indBCS].parserieCD; ifreq++) {
                        APValv[indBCS].cd.push_back(Valv_json[indBCS]["CD"][ifreq].GetDouble());
                    }
                    sort(APValv[indBCS].cd.begin(), APValv[indBCS].cd.end());
                } else
                    APValv[indBCS].parserieCD = 0;
                if (APValv[indBCS].parserieCD > 0)
                    nVariaveis *= APValv[indBCS].parserieCD;
                int total = APValv[indBCS].parserieAbre + APValv[indBCS].parserieCD;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'Valvula-AP' sem variavel abertura ou CD",
                               "#/Valvula", "Indice de Valvula na AP = " + to_string(indBCS));
                }
            }
            listaV.vvalv = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_Furo(Value &Furo_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/Vazamento");

    nAPFuro = 0;
    APFuro = 0;
    try {
        nAPFuro = Furo_json.Size();
        if (nAPFuro < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'Vazamento-AP' possui menos de um elemento",
                       "#/Vazamento", "Quantidade de Vazamento na AP = " + to_string(nAPFuro));
        } else {
            APFuro = new detFUROAP[nAPFuro];
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                APFuro[indBCS].indFuro = Furo_json[indBCS]["indiceVazamento"].GetInt();
                if (Furo_json[indBCS].HasMember("abertura")) {
                    dim++;
                    APFuro[indBCS].parserieAbre = Furo_json[indBCS]["abertura"].Size();
                    for (int ifreq = 0; ifreq < APFuro[indBCS].parserieAbre; ifreq++) {
                        APFuro[indBCS].abertura.push_back(Furo_json[indBCS]["abertura"][ifreq].GetDouble());
                    }
                    sort(APFuro[indBCS].abertura.begin(), APFuro[indBCS].abertura.end());
                } else
                    APFuro[indBCS].parserieAbre = 0;
                if (APFuro[indBCS].parserieAbre > 0)
                    nVariaveis *= APFuro[indBCS].parserieAbre;
                if (Furo_json[indBCS].HasMember("CD")) {
                    dim++;
                    APFuro[indBCS].parserieCD = Furo_json[indBCS]["CD"].Size();
                    for (int ifreq = 0; ifreq < APFuro[indBCS].parserieCD; ifreq++) {
                        APFuro[indBCS].cd.push_back(Furo_json[indBCS]["CD"][ifreq].GetDouble());
                    }
                    sort(APFuro[indBCS].cd.begin(), APFuro[indBCS].cd.end());
                } else
                    APFuro[indBCS].parserieCD = 0;
                if (APFuro[indBCS].parserieCD > 0)
                    nVariaveis *= APFuro[indBCS].parserieCD;
                if (Furo_json[indBCS].HasMember("beta")) {
                    APFuro[indBCS].parserieBeta = Furo_json[indBCS]["beta"].Size();
                    for (int ifreq = 0; ifreq < APFuro[indBCS].parserieBeta; ifreq++) {
                        APFuro[indBCS].beta.push_back(Furo_json[indBCS]["beta"][ifreq].GetDouble());
                    }
                    sort(APFuro[indBCS].beta.begin(), APFuro[indBCS].beta.end());
                } else
                    APFuro[indBCS].parserieBeta = 0;
                if (APFuro[indBCS].parserieBeta > 0)
                    nVariaveis *= APFuro[indBCS].parserieBeta;
                if (Furo_json[indBCS].HasMember("pressao")) {
                    dim++;
                    APFuro[indBCS].parseriePres = Furo_json[indBCS]["pressao"].Size();
                    for (int ifreq = 0; ifreq < APFuro[indBCS].parseriePres; ifreq++) {
                        APFuro[indBCS].pres.push_back(Furo_json[indBCS]["pressao"][ifreq].GetDouble());
                    }
                    sort(APFuro[indBCS].pres.begin(), APFuro[indBCS].pres.end());
                } else
                    APFuro[indBCS].parseriePres = 0;
                if (APFuro[indBCS].parseriePres > 0)
                    nVariaveis *= APFuro[indBCS].parseriePres;
                if (Furo_json[indBCS].HasMember("temperatura")) {
                    dim++;
                    APFuro[indBCS].parserieTemp = Furo_json[indBCS]["tempertatura"].Size();
                    for (int ifreq = 0; ifreq < APFuro[indBCS].parserieTemp; ifreq++) {
                        APFuro[indBCS].temp.push_back(Furo_json[indBCS]["temperatura"][ifreq].GetDouble());
                    }
                    sort(APFuro[indBCS].temp.begin(), APFuro[indBCS].temp.end());
                } else
                    APFuro[indBCS].parserieTemp = 0;
                if (APFuro[indBCS].parserieTemp > 0)
                    nVariaveis *= APFuro[indBCS].parserieTemp;
                if (Furo_json[indBCS].HasMember("fluido")) {
                    dim++;
                    APFuro[indBCS].parserieFlu = Furo_json[indBCS]["fluido"].Size();
                    for (int ifreq = 0; ifreq < APFuro[indBCS].parserieFlu; ifreq++) {
                        APFuro[indBCS].indFlu.push_back(Furo_json[indBCS]["fluido"][ifreq].GetInt());
                    }
                } else
                    APFuro[indBCS].parserieFlu = 0;
                if (APFuro[indBCS].parserieFlu > 0)
                    nVariaveis *= APFuro[indBCS].parserieFlu;
                int total = APFuro[indBCS].parserieAbre + APFuro[indBCS].parserieCD + APFuro[indBCS].parserieBeta + APFuro[indBCS].parseriePres +
                            APFuro[indBCS].parserieTemp + APFuro[indBCS].parserieFlu;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'Vazamento-AP' sem variavel abertura ou CD ou pressao ou temperatrura ou beta ou fluido",
                               "#/Vazamento", "Indice de Vazamento na AP = " + to_string(indBCS));
                }
            }
            listaV.vfuro = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_Diam(Value &secaoTransversal_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/secaoTransversal");

    nAPGeom = 0;
    APGeom = 0;
    int total = 0;
    try {
        nAPGeom = secaoTransversal_json.Size();
        if (nAPGeom < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'secaoTransversal-AP' possui menos de um elemento",
                       "#/secaoTransversal", "Quantidade de secaoTransversals na AP = " + to_string(nAPGeom));
        } else {
            APGeom = new detDiamRug[nAPGeom];
            for (int iG = 0; iG < nAPGeom; iG++) {
                APGeom[iG].indGeom = secaoTransversal_json[iG]["indiceSec"].GetInt();
                if (secaoTransversal_json[iG].HasMember("DiamIntMaior")) {
                    dim++;
                    APGeom[iG].parserieA = secaoTransversal_json[iG]["DiamIntMaior"].Size();
                    for (int ia = 0; ia < APGeom[iG].parserieA; ia++) {
                        APGeom[iG].dA.push_back(secaoTransversal_json[iG]["DiamIntMaior"][ia].GetDouble());
                    }
                    sort(APGeom[iG].dA.begin(), APGeom[iG].dA.end());
                } else
                    APGeom[iG].parserieA = 0;
                if (APGeom[iG].parserieA > 0)
                    nVariaveis *= APGeom[iG].parserieA;
                if (secaoTransversal_json[iG].HasMember("DiamIntMenor")) {
                    dim++;
                    APGeom[iG].parserieB = secaoTransversal_json[iG]["DiamIntMenor"].Size();
                    for (int ib = 0; ib < APGeom[iG].parserieB; ib++) {
                        APGeom[iG].dB.push_back(secaoTransversal_json[iG]["DiamIntMenor"][ib].GetDouble());
                    }
                    sort(APGeom[iG].dB.begin(), APGeom[iG].dB.end());
                } else
                    APGeom[iG].parserieB = 0;
                if (APGeom[iG].parserieB > 0)
                    nVariaveis *= APGeom[iG].parserieB;
                if (secaoTransversal_json[iG].HasMember("Rugosidade")) {
                    dim++;
                    APGeom[iG].parserieRug = secaoTransversal_json[iG]["Rugosidade"].Size();
                    for (int ir = 0; ir < APGeom[iG].parserieRug; ir++) {
                        APGeom[iG].rug.push_back(secaoTransversal_json[iG]["Rugosidade"][ir].GetDouble());
                    }
                    sort(APGeom[iG].rug.begin(), APGeom[iG].rug.end());
                } else
                    APGeom[iG].parserieRug = 0;
                if (APGeom[iG].parserieRug > 0)
                    nVariaveis *= APGeom[iG].parserieRug;
                total = APGeom[iG].parserieA + APGeom[iG].parserieB + APGeom[iG].parserieRug;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'secaoTransversal-AP' sem variavel Diametro menor ou Diametro Maior ou Rugosidade",
                               "#/secaoTransversal", "Indice de secaoTransversal na AP = " + to_string(iG));
                }
            }
            listaV.diam = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_CondEquiv(Value &CondEquiv_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/Condutividade");

    nAPCondEquiv = 0;
    APCondEquiv = 0;
    int total = 0;
    try {
        nAPCondEquiv = CondEquiv_json.Size();
        if (nAPCondEquiv < 1) {
            logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Chave 'Condutividade-AP' possui menos de um elemento",
                       "#/Condutividade", "Quantidade de Condutividades na AP = " + to_string(nAPCondEquiv));
        } else {
            APCondEquiv = new detCondEquiv[nAPCondEquiv];
            for (int ik = 0; ik < nAPCondEquiv; ik++) {
                APCondEquiv[ik].indMat = CondEquiv_json[ik]["indiceMat"].GetInt();
                if (CondEquiv_json[ik].HasMember("condutividade")) {
                    dim++;
                    APCondEquiv[ik].parserieK = CondEquiv_json[ik]["condutividade"].Size();
                    for (int ia = 0; ia < APCondEquiv[ik].parserieK; ia++) {
                        APCondEquiv[ik].cond.push_back(CondEquiv_json[ik]["condutividade"][ia].GetDouble());
                    }
                    sort(APCondEquiv[ik].cond.begin(), APCondEquiv[ik].cond.end());
                } else
                    APCondEquiv[ik].parserieK = 0;
                if (APCondEquiv[ik].parserieK > 0)
                    nVariaveis *= APCondEquiv[ik].parserieK;
                total = APCondEquiv[ik].parserieK;
                if (total == 0) {
                    logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Chave 'Condutividade-AP' sem variavel condutividade",
                               "#/Condutividade", "Indice de condutividade na AP = " + to_string(ik));
                }
            }
            listaV.kequiv = 1;
        }
    } catch (exception &e) {
        // incluir falha
        logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
    }
}

void APara::parse_Psep(Value &Psep_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/psep");

    if (Psep_json.HasMember("pressao")) {
        dim++;
        APPsep.parseriePres = Psep_json["pressao"].Size();
        for (int ifreq = 0; ifreq < APPsep.parseriePres; ifreq++) {
            APPsep.pres.push_back(Psep_json["pressao"][ifreq].GetDouble());
        }
        sort(APPsep.pres.begin(), APPsep.pres.end());
    } else
        APPsep.parseriePres = 0;
    if (APPsep.parseriePres > 0)
        nVariaveis *= APPsep.parseriePres;
    int total = APPsep.parseriePres;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'Psep-AP' sem variavel pressao",
                   "#/psep", "Pressao");
    }

    listaV.vpsep = 1;
}

void APara::parse_RGO(Value &RGO_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/RGO-fluido0");

    if (RGO_json.HasMember("RGO")) {
        dim++;
        APRGO.parserieRGO = RGO_json["RGO"].Size();
        for (int ifreq = 0; ifreq < APRGO.parserieRGO; ifreq++) {
            APRGO.RGO.push_back(RGO_json["RGO"][ifreq].GetDouble());
        }
        sort(APRGO.RGO.begin(), APRGO.RGO.end());
    } else
        APRGO.parserieRGO = 0;
    if (APRGO.parserieRGO > 0)
        nVariaveis *= APRGO.parserieRGO;
    int total = APRGO.parserieRGO;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'RGO-AP' sem variavel RGO",
                   "#/RGO-fluido0", "RGO");
    }

    listaV.vRGO = 1;
}

void APara::parse_BSW(Value &BSW_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/BSW-fluido0");

    if (BSW_json.HasMember("BSW")) {
        dim++;
        APBSW.parserieBSW = BSW_json["BSW"].Size();
        for (int ifreq = 0; ifreq < APBSW.parserieBSW; ifreq++) {
            APBSW.BSW.push_back(BSW_json["BSW"][ifreq].GetDouble());
        }
        sort(APBSW.BSW.begin(), APBSW.BSW.end());
    } else
        APBSW.parserieBSW = 0;
    if (APBSW.parserieBSW > 0)
        nVariaveis *= APBSW.parserieBSW;
    int total = APBSW.parserieBSW;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'BSW-AP' sem variavel BSW",
                   "#/BSW-fluido0", "BSW");
    }

    listaV.vBSW = 1;
}

void APara::parse_GasInj(Value &GasInj_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/GasLift");

    if (GasInj_json.HasMember("presInj")) {
        dim++;
        APGasInj.parseriePresInj = GasInj_json["presInj"].Size();
        for (int ifreq = 0; ifreq < APGasInj.parseriePresInj; ifreq++) {
            APGasInj.presinj.push_back(GasInj_json["presInj"][ifreq].GetDouble());
        }
        sort(APGasInj.presinj.begin(), APGasInj.presinj.end());
    } else
        APGasInj.parseriePresInj = 0;
    if (APGasInj.parseriePresInj > 0)
        nVariaveis *= APGasInj.parseriePresInj;
    if (GasInj_json.HasMember("temperatura")) {
        dim++;
        APGasInj.parserieTemp = GasInj_json["temperatura"].Size();
        for (int ifreq = 0; ifreq < APGasInj.parserieTemp; ifreq++) {
            APGasInj.temperatura.push_back(GasInj_json["temperatura"][ifreq].GetDouble());
        }
        sort(APGasInj.temperatura.begin(), APGasInj.temperatura.end());
    } else
        APGasInj.parserieTemp = 0;
    if (APGasInj.parserieTemp > 0)
        nVariaveis *= APGasInj.parserieTemp;
    if (GasInj_json.HasMember("vazGas")) {
        dim++;
        APGasInj.parserieVazGas = GasInj_json["vazGas"].Size();
        for (int ifreq = 0; ifreq < APGasInj.parserieVazGas; ifreq++) {
            APGasInj.vazgas.push_back(GasInj_json["vazGas"][ifreq].GetDouble());
        }
        sort(APGasInj.vazgas.begin(), APGasInj.vazgas.end());
    } else
        APGasInj.parserieVazGas = 0;
    if (APGasInj.parserieVazGas > 0)
        nVariaveis *= APGasInj.parserieVazGas;
    int total = APGasInj.parseriePresInj + APGasInj.parseriePresInj + APGasInj.parserieVazGas;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'GasInj-AP' sem variavel pressao ou temperatura ou vazao de gas",
                   "#/GasLift", "multiplas variaveis");
    }

    listaV.vgasinj = 1;
}

void APara::parse_CHK(Value &CHK_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/choke");

    if (CHK_json.HasMember("abertura")) {
        dim++;
        APCHK.parserieAbre = CHK_json["abertura"].Size();
        for (int ifreq = 0; ifreq < APCHK.parserieAbre; ifreq++) {
            APCHK.abertura.push_back(CHK_json["abertura"][ifreq].GetDouble());
        }
        sort(APCHK.abertura.begin(), APCHK.abertura.end());
    } else
        APCHK.parserieAbre = 0;
    if (APCHK.parserieAbre > 0)
        nVariaveis *= APCHK.parserieAbre;
    if (CHK_json.HasMember("CD")) {
        dim++;
        APCHK.parserieCD = CHK_json["CD"].Size();
        for (int ifreq = 0; ifreq < APCHK.parserieCD; ifreq++) {
            APCHK.cd.push_back(CHK_json["CD"][ifreq].GetDouble());
        }
        sort(APCHK.cd.begin(), APCHK.cd.end());
    } else
        APCHK.parserieCD = 0;
    if (APCHK.parserieCD > 0)
        nVariaveis *= APCHK.parserieCD;
    int total = APCHK.parserieAbre + APCHK.parserieCD;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'Choke-AP' sem variavel abertura ou CD",
                   "#/CHK", "Choke");
    }

    listaV.vchk = 1;
}

void APara::parse_Pent(Value &Pent_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/pentrada");

    if (Pent_json.HasMember("pressao")) {
        dim++;
        APpEntrada.parseriePres = Pent_json["pressao"].Size();
        for (int ifreq = 0; ifreq < APpEntrada.parseriePres; ifreq++) {
            APpEntrada.pres.push_back(Pent_json["pressao"][ifreq].GetDouble());
        }
        sort(APpEntrada.pres.begin(), APpEntrada.pres.end());
    } else
        APpEntrada.parseriePres = 0;
    if (APpEntrada.parseriePres > 0)
        nVariaveis *= APpEntrada.parseriePres;
    if (Pent_json.HasMember("temperatura")) {
        dim++;
        APpEntrada.parserieTemp = Pent_json["temperatura"].Size();
        for (int ifreq = 0; ifreq < APpEntrada.parserieTemp; ifreq++) {
            APpEntrada.temperatura.push_back(Pent_json["temperatura"][ifreq].GetDouble());
        }
        sort(APpEntrada.temperatura.begin(), APpEntrada.temperatura.end());
    } else
        APpEntrada.parserieTemp = 0;
    if (APpEntrada.parserieTemp > 0)
        nVariaveis *= APpEntrada.parserieTemp;
    if (Pent_json.HasMember("titulo")) {
        dim++;
        APpEntrada.parserieTit = Pent_json["titulo"].Size();
        for (int ifreq = 0; ifreq < APpEntrada.parserieTit; ifreq++) {
            APpEntrada.tit.push_back(Pent_json["titulo"][ifreq].GetDouble());
        }
        sort(APpEntrada.tit.begin(), APpEntrada.tit.end());
    } else
        APpEntrada.parserieTit = 0;
    if (APpEntrada.parserieTit > 0)
        nVariaveis *= APpEntrada.parserieTit;
    if (Pent_json.HasMember("beta")) {
        dim++;
        APpEntrada.parserieBet = Pent_json["beta"].Size();
        for (int ifreq = 0; ifreq < APpEntrada.parserieBet; ifreq++) {
            APpEntrada.bet.push_back(Pent_json["beta"][ifreq].GetDouble());
        }
        sort(APpEntrada.bet.begin(), APpEntrada.bet.end());
    } else
        APpEntrada.parserieBet = 0;
    if (APpEntrada.parserieBet > 0)
        nVariaveis *= APpEntrada.parserieBet;
    int total = APpEntrada.parseriePres + APpEntrada.parserieTemp + APpEntrada.parserieTit + APpEntrada.parserieBet;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'Presentrada-AP' sem variavel pressao ou temperatura ou titulo ou beta",
                   "#/pentrada", "multiplas variaveis");
    }

    listaV.vpresent = 1;
}

void APara::parse_VPent(Value &VPent_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/vazpentrada");

    if (VPent_json.HasMember("pressao")) {
        dim++;
        APvpEntrada.parseriePres = VPent_json["pressao"].Size();
        for (int ifreq = 0; ifreq < APvpEntrada.parseriePres; ifreq++) {
            APvpEntrada.pres.push_back(VPent_json["pressao"][ifreq].GetDouble());
        }
        sort(APvpEntrada.pres.begin(), APvpEntrada.pres.end());
    } else
        APvpEntrada.parseriePres = 0;
    if (APvpEntrada.parseriePres > 0)
        nVariaveis *= APvpEntrada.parseriePres;
    if (VPent_json.HasMember("temperatura")) {
        dim++;
        APvpEntrada.parserieTemp = VPent_json["temperatura"].Size();
        for (int ifreq = 0; ifreq < APvpEntrada.parserieTemp; ifreq++) {
            APvpEntrada.temperatura.push_back(VPent_json["temperatura"][ifreq].GetDouble());
        }
        sort(APvpEntrada.temperatura.begin(), APvpEntrada.temperatura.end());
    } else
        APvpEntrada.parserieTemp = 0;
    if (APvpEntrada.parserieTemp > 0)
        nVariaveis *= APvpEntrada.parserieTemp;
    if (VPent_json.HasMember("vazMass")) {
        dim++;
        APvpEntrada.parserieMass = VPent_json["vazMass"].Size();
        for (int ifreq = 0; ifreq < APvpEntrada.parserieMass; ifreq++) {
            APvpEntrada.mass.push_back(VPent_json["vazMass"][ifreq].GetDouble());
        }
        sort(APvpEntrada.mass.begin(), APvpEntrada.mass.end());
    } else
        APvpEntrada.parserieMass = 0;
    if (APvpEntrada.parserieMass > 0)
        nVariaveis *= APvpEntrada.parserieMass;
    if (VPent_json.HasMember("beta")) {
        dim++;
        APvpEntrada.parserieBet = VPent_json["beta"].Size();
        for (int ifreq = 0; ifreq < APvpEntrada.parserieBet; ifreq++) {
            APvpEntrada.bet.push_back(VPent_json["beta"][ifreq].GetDouble());
        }
        sort(APvpEntrada.bet.begin(), APvpEntrada.bet.end());
    } else
        APvpEntrada.parserieBet = 0;
    if (APvpEntrada.parserieBet > 0)
        nVariaveis *= APvpEntrada.parserieBet;
    int total = APvpEntrada.parseriePres + APvpEntrada.parserieTemp + APvpEntrada.parserieMass + APvpEntrada.parserieBet;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'VazPresentrada-AP' sem variavel pressao ou temperatura ou vazao massica ou beta",
                   "#/vpentrada", "multiplas variaveis");
    }

    listaV.vvazpresent = 1;
}

void APara::parse_injecPoco(Value &injecP_json) {
    // criar variavel para o nome da propriedade json em processo de parse
    string chaveJson("#/pocoInjetor");

    if (injecP_json.HasMember("pressaoInj")) {
        dim++;
        APPInj.parseriePresI = injecP_json["pressaoInj"].Size();
        for (int ifreq = 0; ifreq < APPInj.parseriePresI; ifreq++) {
            APPInj.presinj.push_back(injecP_json["pressaoInj"][ifreq].GetDouble());
        }
        sort(APPInj.presinj.begin(), APPInj.presinj.end());
    } else
        APPInj.parseriePresI = 0;
    if (APPInj.parseriePresI > 0)
        nVariaveis *= APPInj.parseriePresI;
    if (injecP_json.HasMember("pressaoFinal")) {
        dim++;
        APPInj.parseriePresFundo = injecP_json["pressaoFinal"].Size();
        for (int ifreq = 0; ifreq < APPInj.parseriePresFundo; ifreq++) {
            APPInj.presfundo.push_back(injecP_json["pressaoFinal"][ifreq].GetDouble());
        }
        sort(APPInj.presfundo.begin(), APPInj.presfundo.end());
    } else
        APPInj.parseriePresFundo = 0;
    if (APPInj.parseriePresFundo > 0)
        nVariaveis *= APPInj.parseriePresFundo;
    if (injecP_json.HasMember("temperatura")) {
        dim++;
        APPInj.parserieTemp = injecP_json["temperatura"].Size();
        for (int ifreq = 0; ifreq < APPInj.parserieTemp; ifreq++) {
            APPInj.tempinj.push_back(injecP_json["temperatura"][ifreq].GetDouble());
        }
        sort(APPInj.tempinj.begin(), APPInj.tempinj.end());
    } else
        APPInj.parserieTemp = 0;
    if (APPInj.parserieTemp > 0)
        nVariaveis *= APPInj.parserieTemp;
    if (injecP_json.HasMember("temperatura")) {
        dim++;
        APPInj.parserieVaz = injecP_json["vazao"].Size();
        for (int ifreq = 0; ifreq < APPInj.parserieVaz; ifreq++) {
            APPInj.vazinj.push_back(injecP_json["vazao"][ifreq].GetDouble());
        }
        sort(APPInj.vazinj.begin(), APPInj.vazinj.end());
    } else
        APPInj.parserieVaz = 0;
    if (APPInj.parserieVaz > 0)
        nVariaveis *= APPInj.parserieVaz;
    int total = APPInj.parseriePresI + APPInj.parseriePresFundo + APPInj.parserieTemp + APPInj.parserieVaz;
    if (total == 0) {
        logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'PocoInjetor-AP' sem variavel pressao de injecao ou temperatura ou vazao ou pressao final",
                   "#/pocoInjetorEntrada", "multiplas variaveis");
    }

    listaV.vvazpresent = 1;
}

void APara::lerArq() {
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

    dim = 0;
    nthrdAP = 1;

    nVariaveis = 1;
    tipoAP = 0;
    vfp = 0;
    listaV.vbcs = 0;
    listaV.vbvol = 0;
    listaV.vchk = 0;
    listaV.vdp = 0;
    listaV.vdpH = 0;
    listaV.vdpF = 0;
    listaV.vdt = 0;
    listaV.vfongas = 0;
    listaV.vfonliq = 0;
    listaV.vfonmas = 0;
    listaV.vfuro = 0;
    listaV.diam = 0;
    listaV.vgasinj = 0;
    listaV.vipr = 0;
    listaV.vpocinj = 0;
    listaV.vpresent = 0;
    listaV.vpsep = 0;
    listaV.vRGO = 0;
    listaV.vBSW = 0;
    listaV.vvalv = 0;
    listaV.vvazpresent = 0;
    listaV.diam = 0;
    listaV.kequiv = 0;

    if (jsonDoc.HasMember("nthread"))
        nthrdAP = jsonDoc["nthread"].GetInt();
    if (jsonDoc.HasMember("tipoAP"))
        tipoAP = jsonDoc["tipoAP"].GetInt();

    if (jsonDoc.HasMember("vfp") && jsonDoc["vfp"].IsInt()) {
        this->vfp = jsonDoc["vfp"].GetInt(); // aceita 0
    } else {
        this->vfp = 1; // default
    }

    APIPR = 0;
    if (jsonDoc.HasMember("IPR")) {
        parse_IPR(jsonDoc["IPR"]);
    }
    APFonLiq = 0;
    if (jsonDoc.HasMember("FonteLiquido")) {
        parse_FonLiq(jsonDoc["FonteLiquido"]);
    }
    APFonGas = 0;
    if (jsonDoc.HasMember("FonteGas")) {
        parse_FonGas(jsonDoc["FonteGas"]);
    }
    APFonMas = 0;
    if (jsonDoc.HasMember("FonteMassa")) {
        parse_FonMas(jsonDoc["FonteMassa"]);
    }
    APBCS = 0;
    if (jsonDoc.HasMember("BCS")) {
        parse_BCS(jsonDoc["BCS"]);
    }
    APBVOL = 0;
    if (jsonDoc.HasMember("BVol")) {
        parse_BVol(jsonDoc["BVol"]);
    }
    APDP = 0;
    if (jsonDoc.HasMember("DP")) {
        parse_DP(jsonDoc["DP"]);
    }
    APdPdLH = 0;
    if (jsonDoc.HasMember("dPdLHidro")) {
        parse_dPdLH(jsonDoc["dPdLHidro"]);
    }
    APdPdLF = 0;
    if (jsonDoc.HasMember("dPdLFric")) {
        parse_dPdLF(jsonDoc["dPdLFric"]);
    }
    APdTdL = 0;
    if (jsonDoc.HasMember("dTdL")) {
        parse_dTdL(jsonDoc["dTdL"]);
    }
    APValv = 0;
    if (jsonDoc.HasMember("Valvula")) {
        parse_Valv(jsonDoc["Valvula"]);
    }
    APFuro = 0;
    if (jsonDoc.HasMember("Vazamento")) {
        parse_Furo(jsonDoc["Vazamento"]);
    }
    APGeom = 0;
    if (jsonDoc.HasMember("secaoTransversal")) {
        parse_Diam(jsonDoc["secaoTransversal"]);
    }
    APCondEquiv = 0;
    if (jsonDoc.HasMember("Condutividade")) {
        parse_CondEquiv(jsonDoc["Condutividade"]);
    }
    if (jsonDoc.HasMember("psep")) {
        parse_Psep(jsonDoc["psep"]);
    }
    if (jsonDoc.HasMember("RGO-fluido0")) {
        parse_RGO(jsonDoc["RGO-fluido0"]);
    }
    if (jsonDoc.HasMember("BSW-fluido0")) {
        parse_BSW(jsonDoc["BSW-fluido0"]);
    }
    if (jsonDoc.HasMember("GasLift")) {
        parse_GasInj(jsonDoc["GasLift"]);
    }
    if (jsonDoc.HasMember("choke")) {
        parse_CHK(jsonDoc["choke"]);
    }
    if (jsonDoc.HasMember("pentrada")) {
        parse_Pent(jsonDoc["pentrada"]);
    }
    if (jsonDoc.HasMember("vazpentrada")) {
        parse_VPent(jsonDoc["vazpentrada"]);
    }
    if (jsonDoc.HasMember("pocoInjetor")) {
        parse_injecPoco(jsonDoc["pocoInjetor"]);
    }

    varSeq[0] = listaV.vpsep;
    varSeq[1] = listaV.vRGO;
    varSeq[2] = listaV.vBSW;
    varSeq[3] = listaV.vfonliq;
    varSeq[4] = listaV.vfongas;
    varSeq[5] = listaV.vipr;
    varSeq[6] = listaV.vfonmas;
    varSeq[7] = listaV.vbcs;
    varSeq[8] = listaV.vdp;
    varSeq[9] = listaV.vdpH;
    varSeq[10] = listaV.vdpF;
    varSeq[11] = listaV.vdt;
    varSeq[12] = listaV.vbvol;
    varSeq[13] = listaV.vvalv;
    varSeq[14] = listaV.vfuro;
    varSeq[15] = listaV.diam;
    varSeq[16] = listaV.kequiv;
    varSeq[17] = listaV.vgasinj;
    varSeq[18] = listaV.vpresent;
    varSeq[19] = listaV.vvazpresent;
    varSeq[20] = listaV.vchk;
    varSeq[21] = listaV.vpocinj;

    sequenciaAP = 0;
    if (nVariaveis > 1) {
        sequenciaAP = new casoVEC[nVariaveis];
        genericoAP = new genericoVEC[nVariaveis];
        vecParSerie = new int[dim];
    }
    ostringstream saidaP;
    saidaP << "relacaoAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::out);
    escreveIni << "sequencias da Analise Parametrica para um Tramo " << "\n";
    escreveIni.close();

    if (vfp == 1) {
        constroiVecParSerie();
        inicializaSequen(0, 0);
        traduzSeq();
    } else if (vfp == 0) {
        constroiVecParSerieImex();
        inicializaSequen(0, 0);
        traduzSeqImex();
    }

    if (tipoAP == 1) {
        saidaBHP = new double *[nVariaveis];
        saidaVazLiq = new double *[nVariaveis];
        for (int i = 0; i < nVariaveis; i++) {
            saidaBHP[i] = new double[dim + 1];
            saidaVazLiq[i] = new double[APFonLiq[0].parserieVL];
        }

        tabelaGenericaCabecalho();
    } else {
        saidaBHP = 0;
        saidaVazLiq = 0;
    }
}

void APara::constroiVecParSerie() {
    int locdim = 0;
    if (listaV.vpsep == 1) {
        vecParSerie[locdim] = APPsep.parseriePres;
        locdim++;
    }
    if (listaV.vBSW == 1) {
        vecParSerie[locdim] = APBSW.parserieBSW;
        locdim++;
    }
    if (listaV.vRGO == 1) {
        vecParSerie[locdim] = APRGO.parserieRGO;
        locdim++;
    }
    if (listaV.vfongas == 1) {
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieTemp > 0) {
                vecParSerie[locdim] = APFonGas[iFG].parserieTemp;
                locdim++;
            }
        }
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieVazG > 0) {
                vecParSerie[locdim] = APFonGas[iFG].parserieVazG;
                locdim++;
            }
        }
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieVazC > 0) {
                vecParSerie[locdim] = APFonGas[iFG].parserieVazC;
                locdim++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            if (APBCS[indBCS].parserieFreq > 0) {
                vecParSerie[locdim] = APBCS[indBCS].parserieFreq;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            if (APBCS[indBCS].parserieEstag > 0) {
                vecParSerie[locdim] = APBCS[indBCS].parserieEstag;
                locdim++;
            }
        }
    }
    if (listaV.vfonliq == 1) {
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieTemp > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieTemp;
                locdim++;
            }
        }
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieVL > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieVL;
                locdim++;
            }
        }
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieBet > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieBet;
                locdim++;
            }
        }
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieFlu > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieFlu;
                locdim++;
            }
        }
    }
    if (listaV.vipr == 1) {
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieTemp > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieTemp;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parseriePres > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parseriePres;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieIP > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieIP;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieJP > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieJP;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieqMax > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieqMax;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieFlup > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieFlup;
                locdim++;
            }
        }
    }

    if (listaV.vfonmas == 1) {
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieTemp > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieTemp;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieMP > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieMP;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieMC > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieMC;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieMG > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieMG;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieFlu > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieFlu;
                locdim++;
            }
        }
    }
    if (listaV.vdp == 1) {
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            if (APDP[indBCS].parserieDP > 0) {
                vecParSerie[locdim] = APDP[indBCS].parserieDP;
                locdim++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                vecParSerie[locdim] = APdPdLH[indBCS].parserieDPHidro;
                locdim++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                vecParSerie[locdim] = APdPdLF[indBCS].parserieDPFric;
                locdim++;
            }
        }
    }
    if (listaV.vdt == 1) {
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            if (APdTdL[indBCS].parserieDT > 0) {
                vecParSerie[locdim] = APdTdL[indBCS].parserieDT;
                locdim++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieFreq > 0) {
                vecParSerie[locdim] = APBVOL[indBCS].parserieFreq;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieCap > 0) {
                vecParSerie[locdim] = APBVOL[indBCS].parserieCap;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieNPoli > 0) {
                vecParSerie[locdim] = APBVOL[indBCS].parserieNPoli;
                locdim++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            if (APValv[indBCS].parserieAbre > 0) {
                vecParSerie[locdim] = APValv[indBCS].parserieAbre;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            if (APValv[indBCS].parserieCD > 0) {
                vecParSerie[locdim] = APValv[indBCS].parserieCD;
                locdim++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieAbre > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieAbre;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieCD > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieCD;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieBeta > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieBeta;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parseriePres > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parseriePres;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieTemp > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieTemp;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieFlu > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieFlu;
                locdim++;
            }
        }
    }
    if (listaV.diam == 1) {
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                vecParSerie[locdim] = APGeom[iG].parserieA;
                locdim++;
            }
        }
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieB > 0) {
                vecParSerie[locdim] = APGeom[iG].parserieB;
                locdim++;
            }
        }
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieRug > 0) {
                vecParSerie[locdim] = APGeom[iG].parserieRug;
                locdim++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                vecParSerie[locdim] = APCondEquiv[ik].parserieK;
                locdim++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            vecParSerie[locdim] = APGasInj.parseriePresInj;
            locdim++;
        }
        if (APGasInj.parserieTemp > 0) {
            vecParSerie[locdim] = APGasInj.parserieTemp;
            locdim++;
        }
        if (APGasInj.parserieVazGas > 0) {
            vecParSerie[locdim] = APGasInj.parserieVazGas;
            locdim++;
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            vecParSerie[locdim] = APpEntrada.parseriePres;
            locdim++;
        }
        if (APpEntrada.parserieBet > 0) {
            vecParSerie[locdim] = APpEntrada.parserieBet;
            locdim++;
        }
        if (APpEntrada.parserieTemp > 0) {
            vecParSerie[locdim] = APpEntrada.parserieTemp;
            locdim++;
        }
        if (APpEntrada.parserieTit > 0) {
            vecParSerie[locdim] = APpEntrada.parserieTit;
            locdim++;
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            vecParSerie[locdim] = APvpEntrada.parseriePres;
            locdim++;
        }
        if (APvpEntrada.parserieBet > 0) {
            vecParSerie[locdim] = APvpEntrada.parserieBet;
            locdim++;
        }
        if (APvpEntrada.parserieMass > 0) {
            vecParSerie[locdim] = APvpEntrada.parserieMass;
            locdim++;
        }
        if (APvpEntrada.parserieTemp > 0) {
            vecParSerie[locdim] = APvpEntrada.parserieTemp;
            locdim++;
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            vecParSerie[locdim] = APCHK.parserieAbre;
            locdim++;
        }
        if (APCHK.parserieCD > 0) {
            vecParSerie[locdim] = APCHK.parserieCD;
            locdim++;
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            vecParSerie[locdim] = APPInj.parseriePresI;
            locdim++;
        }
        if (APPInj.parseriePresFundo > 0) {
            vecParSerie[locdim] = APPInj.parseriePresFundo;
            locdim++;
        }
        if (APPInj.parserieTemp > 0) {
            vecParSerie[locdim] = APPInj.parserieTemp;
            locdim++;
        }
        if (APPInj.parserieVaz > 0) {
            vecParSerie[locdim] = APPInj.parserieVaz;
            locdim++;
        }
    }
}

void APara::constroiVecParSerieImex() {
    int locdim = 0;
    if (listaV.vRGO == 1) {
        vecParSerie[locdim] = APRGO.parserieRGO;
        locdim++;
    }
    if (listaV.vBSW == 1) {
        vecParSerie[locdim] = APBSW.parserieBSW;
        locdim++;
    }
    if (listaV.vfonliq == 1) {
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieTemp > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieTemp;
                locdim++;
            }
        }
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieVL > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieVL;
                locdim++;
            }
        }
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieBet > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieBet;
                locdim++;
            }
        }
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieFlu > 0) {
                vecParSerie[locdim] = APFonLiq[iFL].parserieFlu;
                locdim++;
            }
        }
    }
    if (listaV.vfongas == 1) {
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieTemp > 0) {
                vecParSerie[locdim] = APFonGas[iFG].parserieTemp;
                locdim++;
            }
        }
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieVazG > 0) {
                vecParSerie[locdim] = APFonGas[iFG].parserieVazG;
                locdim++;
            }
        }
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieVazC > 0) {
                vecParSerie[locdim] = APFonGas[iFG].parserieVazC;
                locdim++;
            }
        }
    }
    if (listaV.vpsep == 1) {
        vecParSerie[locdim] = APPsep.parseriePres;
        locdim++;
    }
    if (listaV.vipr == 1) {
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieTemp > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieTemp;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parseriePres > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parseriePres;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieIP > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieIP;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieJP > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieJP;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieqMax > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieqMax;
                locdim++;
            }
        }
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieFlup > 0) {
                vecParSerie[locdim] = APIPR[iIPR].parserieFlup;
                locdim++;
            }
        }
    }
    if (listaV.vfonmas == 1) {
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieTemp > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieTemp;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieMP > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieMP;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieMC > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieMC;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieMG > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieMG;
                locdim++;
            }
        }
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieFlu > 0) {
                vecParSerie[locdim] = APFonMas[iFM].parserieFlu;
                locdim++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            if (APBCS[indBCS].parserieFreq > 0) {
                vecParSerie[locdim] = APBCS[indBCS].parserieFreq;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            if (APBCS[indBCS].parserieEstag > 0) {
                vecParSerie[locdim] = APBCS[indBCS].parserieEstag;
                locdim++;
            }
        }
    }
    if (listaV.vdp == 1) {
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            if (APDP[indBCS].parserieDP > 0) {
                vecParSerie[locdim] = APDP[indBCS].parserieDP;
                locdim++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                vecParSerie[locdim] = APdPdLH[indBCS].parserieDPHidro;
                locdim++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                vecParSerie[locdim] = APdPdLF[indBCS].parserieDPFric;
                locdim++;
            }
        }
    }
    if (listaV.vdt == 1) {
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            if (APdTdL[indBCS].parserieDT > 0) {
                vecParSerie[locdim] = APdTdL[indBCS].parserieDT;
                locdim++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieFreq > 0) {
                vecParSerie[locdim] = APBVOL[indBCS].parserieFreq;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieCap > 0) {
                vecParSerie[locdim] = APBVOL[indBCS].parserieCap;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieNPoli > 0) {
                vecParSerie[locdim] = APBVOL[indBCS].parserieNPoli;
                locdim++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            if (APValv[indBCS].parserieAbre > 0) {
                vecParSerie[locdim] = APValv[indBCS].parserieAbre;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            if (APValv[indBCS].parserieCD > 0) {
                vecParSerie[locdim] = APValv[indBCS].parserieCD;
                locdim++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieAbre > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieAbre;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieCD > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieCD;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieBeta > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieBeta;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parseriePres > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parseriePres;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieTemp > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieTemp;
                locdim++;
            }
        }
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieFlu > 0) {
                vecParSerie[locdim] = APFuro[indBCS].parserieFlu;
                locdim++;
            }
        }
    }
    if (listaV.diam == 1) {
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                vecParSerie[locdim] = APGeom[iG].parserieA;
                locdim++;
            }
        }
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieB > 0) {
                vecParSerie[locdim] = APGeom[iG].parserieB;
                locdim++;
            }
        }
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieRug > 0) {
                vecParSerie[locdim] = APGeom[iG].parserieRug;
                locdim++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                vecParSerie[locdim] = APCondEquiv[ik].parserieK;
                locdim++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            vecParSerie[locdim] = APGasInj.parseriePresInj;
            locdim++;
        }
        if (APGasInj.parserieTemp > 0) {
            vecParSerie[locdim] = APGasInj.parserieTemp;
            locdim++;
        }
        if (APGasInj.parserieVazGas > 0) {
            vecParSerie[locdim] = APGasInj.parserieVazGas;
            locdim++;
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            vecParSerie[locdim] = APpEntrada.parseriePres;
            locdim++;
        }
        if (APpEntrada.parserieBet > 0) {
            vecParSerie[locdim] = APpEntrada.parserieBet;
            locdim++;
        }
        if (APpEntrada.parserieTemp > 0) {
            vecParSerie[locdim] = APpEntrada.parserieTemp;
            locdim++;
        }
        if (APpEntrada.parserieTit > 0) {
            vecParSerie[locdim] = APpEntrada.parserieTit;
            locdim++;
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            vecParSerie[locdim] = APvpEntrada.parseriePres;
            locdim++;
        }
        if (APvpEntrada.parserieBet > 0) {
            vecParSerie[locdim] = APvpEntrada.parserieBet;
            locdim++;
        }
        if (APvpEntrada.parserieMass > 0) {
            vecParSerie[locdim] = APvpEntrada.parserieMass;
            locdim++;
        }
        if (APvpEntrada.parserieTemp > 0) {
            vecParSerie[locdim] = APvpEntrada.parserieTemp;
            locdim++;
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            vecParSerie[locdim] = APCHK.parserieAbre;
            locdim++;
        }
        if (APCHK.parserieCD > 0) {
            vecParSerie[locdim] = APCHK.parserieCD;
            locdim++;
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            vecParSerie[locdim] = APPInj.parseriePresI;
            locdim++;
        }
        if (APPInj.parseriePresFundo > 0) {
            vecParSerie[locdim] = APPInj.parseriePresFundo;
            locdim++;
        }
        if (APPInj.parserieTemp > 0) {
            vecParSerie[locdim] = APPInj.parserieTemp;
            locdim++;
        }
        if (APPInj.parserieVaz > 0) {
            vecParSerie[locdim] = APPInj.parserieVaz;
            locdim++;
        }
    }
}

void APara::traduzSeq() {
    int locdim;
    int iseq = 0;
    while (iseq < nVariaveis) {
        locdim = 0;
        if (listaV.vpsep == 1) {
            sequenciaAP[iseq].PSEPpres = genericoAP[iseq].generico[locdim];
            locdim++;
        }
        if (listaV.vBSW == 1) {
            sequenciaAP[iseq].BSW = genericoAP[iseq].generico[locdim];
            locdim++;
        }
        if (listaV.vRGO == 1) {
            sequenciaAP[iseq].RGO = genericoAP[iseq].generico[locdim];
            locdim++;
        }
        if (listaV.vfongas == 1) { // chrisao; alterei a ordem com a QLIQ
            for (int iFG = 0; iFG < nAPFG; iFG++) {
                if (APFonGas[iFG].parserieTemp > 0) {
                    sequenciaAP[iseq].FGtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFG = 0; iFG < nAPFG; iFG++) {
                if (APFonGas[iFG].parserieVazG > 0) {
                    sequenciaAP[iseq].FGvazgas.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFG = 0; iFG < nAPFG; iFG++) {
                if (APFonGas[iFG].parserieVazC > 0) {
                    sequenciaAP[iseq].FGvazcomp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vbcs == 1) {
            for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
                if (APBCS[indBCS].parserieFreq > 0) {
                    sequenciaAP[iseq].BCSfreq.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
                if (APBCS[indBCS].parserieEstag > 0) {
                    sequenciaAP[iseq].BCSnestag.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }

        if (listaV.vfonliq == 1) {
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieTemp > 0) {
                    sequenciaAP[iseq].FLtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieVL > 0) {
                    sequenciaAP[iseq].FLvazliq.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieBet > 0) {
                    sequenciaAP[iseq].FLbet.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieFlu > 0) {
                    sequenciaAP[iseq].FLindfluP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vipr == 1) {
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieTemp > 0) {
                    sequenciaAP[iseq].IPRtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parseriePres > 0) {
                    sequenciaAP[iseq].IPRpres.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieIP > 0) {
                    sequenciaAP[iseq].IPRip.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieJP > 0) {
                    sequenciaAP[iseq].IPRjp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieqMax > 0) {
                    sequenciaAP[iseq].IPRqMax.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieFlup > 0) {
                    sequenciaAP[iseq].IPRindfluP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }

        if (listaV.vfonmas == 1) {
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieTemp > 0) {
                    sequenciaAP[iseq].FMtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieMP > 0) {
                    sequenciaAP[iseq].FMvazMasP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieMC > 0) {
                    sequenciaAP[iseq].FMvazMasC.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieMG > 0) {
                    sequenciaAP[iseq].FMvazMasG.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieFlu > 0) {
                    sequenciaAP[iseq].FMindfluP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdp == 1) {
            for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
                if (APDP[indBCS].parserieDP > 0) {
                    sequenciaAP[iseq].DPdp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdpH == 1) {
            for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
                if (APdPdLH[indBCS].parserieDPHidro > 0) {
                    sequenciaAP[iseq].DPdLH.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdpF == 1) {
            for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
                if (APdPdLF[indBCS].parserieDPFric > 0) {
                    sequenciaAP[iseq].DPdLF.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdt == 1) {
            for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
                if (APdTdL[indBCS].parserieDT > 0) {
                    sequenciaAP[iseq].DTdL.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vbvol == 1) {
            for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
                if (APBVOL[indBCS].parserieFreq > 0) {
                    sequenciaAP[iseq].BVOLfreq.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
                if (APBVOL[indBCS].parserieCap > 0) {
                    sequenciaAP[iseq].BVOLcapacidade.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
                if (APBVOL[indBCS].parserieNPoli > 0) {
                    sequenciaAP[iseq].BVOLnpoli.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vvalv == 1) {
            for (int indBCS = 0; indBCS < nAPV; indBCS++) {
                if (APValv[indBCS].parserieAbre > 0) {
                    sequenciaAP[iseq].VALVabertura.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPV; indBCS++) {
                if (APValv[indBCS].parserieCD > 0) {
                    sequenciaAP[iseq].VALVcd.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vfuro == 1) {
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieAbre > 0) {
                    sequenciaAP[iseq].FUROabertura.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieCD > 0) {
                    sequenciaAP[iseq].FUROcd.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieBeta > 0) {
                    sequenciaAP[iseq].FURObeta.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parseriePres > 0) {
                    sequenciaAP[iseq].FUROpres.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieTemp > 0) {
                    sequenciaAP[iseq].FUROtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieFlu > 0) {
                    sequenciaAP[iseq].FUROindFlu.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.diam == 1) {
            for (int iG = 0; iG < nAPGeom; iG++) {
                if (APGeom[iG].parserieA > 0) {
                    sequenciaAP[iseq].GeomdA.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iG = 0; iG < nAPGeom; iG++) {
                if (APGeom[iG].parserieB > 0) {
                    sequenciaAP[iseq].GeomdB.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iG = 0; iG < nAPGeom; iG++) {
                if (APGeom[iG].parserieRug > 0) {
                    sequenciaAP[iseq].GeomRug.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.kequiv == 1) {
            for (int ik = 0; ik < nAPCondEquiv; ik++) {
                if (APCondEquiv[ik].parserieK > 0) {
                    sequenciaAP[iseq].condEqui.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vgasinj == 1) {
            if (APGasInj.parseriePresInj > 0) {
                sequenciaAP[iseq].INJGpresinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APGasInj.parserieTemp > 0) {
                sequenciaAP[iseq].INJGtemperatura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APGasInj.parserieVazGas > 0) {
                sequenciaAP[iseq].INJGvazgas = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vpresent == 1) {
            if (APpEntrada.parseriePres > 0) {
                sequenciaAP[iseq].PEpres = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APpEntrada.parserieBet > 0) {
                sequenciaAP[iseq].PEbet = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APpEntrada.parserieTemp > 0) {
                sequenciaAP[iseq].PEtemperatura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APpEntrada.parserieTit > 0) {
                sequenciaAP[iseq].PEtit = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vvazpresent == 1) {
            if (APvpEntrada.parseriePres > 0) {
                sequenciaAP[iseq].VPEpres = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APvpEntrada.parserieBet > 0) {
                sequenciaAP[iseq].VPEbet = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APvpEntrada.parserieMass > 0) {
                sequenciaAP[iseq].VPEmass = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APvpEntrada.parserieTemp > 0) {
                sequenciaAP[iseq].VPEtemperatura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vchk == 1) {
            if (APCHK.parserieAbre > 0) {
                sequenciaAP[iseq].CHKabertura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APCHK.parserieCD > 0) {
                sequenciaAP[iseq].CHKcd = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vpocinj == 1) {
            if (APPInj.parseriePresI > 0) {
                sequenciaAP[iseq].PIpresinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APPInj.parseriePresFundo > 0) {
                sequenciaAP[iseq].PIpresfundo = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APPInj.parserieTemp > 0) {
                sequenciaAP[iseq].PItempinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APPInj.parserieVaz > 0) {
                sequenciaAP[iseq].PIvazinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        iseq++;
    }
}

void APara::traduzSeqImex() {
    int locdim;
    int iseq = 0;
    while (iseq < nVariaveis) {
        locdim = 0;

        if (listaV.vRGO == 1) {
            sequenciaAP[iseq].RGO = genericoAP[iseq].generico[locdim];
            locdim++;
        }

        if (listaV.vBSW == 1) {
            sequenciaAP[iseq].BSW = genericoAP[iseq].generico[locdim];
            locdim++;
        }

        if (listaV.vfonliq == 1) {
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieTemp > 0) {
                    sequenciaAP[iseq].FLtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieVL > 0) {
                    sequenciaAP[iseq].FLvazliq.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieBet > 0) {
                    sequenciaAP[iseq].FLbet.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFL = 0; iFL < nAPFL; iFL++) {
                if (APFonLiq[iFL].parserieFlu > 0) {
                    sequenciaAP[iseq].FLindfluP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vfongas == 1) { // chrisao; alterei a ordem com a QLIQ
            for (int iFG = 0; iFG < nAPFG; iFG++) {
                if (APFonGas[iFG].parserieTemp > 0) {
                    sequenciaAP[iseq].FGtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFG = 0; iFG < nAPFG; iFG++) {
                if (APFonGas[iFG].parserieVazG > 0) {
                    sequenciaAP[iseq].FGvazgas.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFG = 0; iFG < nAPFG; iFG++) {
                if (APFonGas[iFG].parserieVazC > 0) {
                    sequenciaAP[iseq].FGvazcomp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vpsep == 1) {
            sequenciaAP[iseq].PSEPpres = genericoAP[iseq].generico[locdim];
            locdim++;
        }
        if (listaV.vipr == 1) {
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieTemp > 0) {
                    sequenciaAP[iseq].IPRtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parseriePres > 0) {
                    sequenciaAP[iseq].IPRpres.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieIP > 0) {
                    sequenciaAP[iseq].IPRip.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieJP > 0) {
                    sequenciaAP[iseq].IPRjp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieqMax > 0) {
                    sequenciaAP[iseq].IPRqMax.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
                if (APIPR[iIPR].parserieFlup > 0) {
                    sequenciaAP[iseq].IPRindfluP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vfonmas == 1) {
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieTemp > 0) {
                    sequenciaAP[iseq].FMtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieMP > 0) {
                    sequenciaAP[iseq].FMvazMasP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieMC > 0) {
                    sequenciaAP[iseq].FMvazMasC.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieMG > 0) {
                    sequenciaAP[iseq].FMvazMasG.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iFM = 0; iFM < nAPFM; iFM++) {
                if (APFonMas[iFM].parserieFlu > 0) {
                    sequenciaAP[iseq].FMindfluP.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vbcs == 1) {
            for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
                if (APBCS[indBCS].parserieFreq > 0) {
                    sequenciaAP[iseq].BCSfreq.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
                if (APBCS[indBCS].parserieEstag > 0) {
                    sequenciaAP[iseq].BCSnestag.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdp == 1) {
            for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
                if (APDP[indBCS].parserieDP > 0) {
                    sequenciaAP[iseq].DPdp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdpH == 1) {
            for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
                if (APdPdLH[indBCS].parserieDPHidro > 0) {
                    sequenciaAP[iseq].DPdLH.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdpF == 1) {
            for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
                if (APdPdLF[indBCS].parserieDPFric > 0) {
                    sequenciaAP[iseq].DPdLF.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vdt == 1) {
            for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
                if (APdTdL[indBCS].parserieDT > 0) {
                    sequenciaAP[iseq].DTdL.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vbvol == 1) {
            for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
                if (APBVOL[indBCS].parserieFreq > 0) {
                    sequenciaAP[iseq].BVOLfreq.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
                if (APBVOL[indBCS].parserieCap > 0) {
                    sequenciaAP[iseq].BVOLcapacidade.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
                if (APBVOL[indBCS].parserieNPoli > 0) {
                    sequenciaAP[iseq].BVOLnpoli.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vvalv == 1) {
            for (int indBCS = 0; indBCS < nAPV; indBCS++) {
                if (APValv[indBCS].parserieAbre > 0) {
                    sequenciaAP[iseq].VALVabertura.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPV; indBCS++) {
                if (APValv[indBCS].parserieCD > 0) {
                    sequenciaAP[iseq].VALVcd.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vfuro == 1) {
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieAbre > 0) {
                    sequenciaAP[iseq].FUROabertura.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieCD > 0) {
                    sequenciaAP[iseq].FUROcd.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieBeta > 0) {
                    sequenciaAP[iseq].FURObeta.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parseriePres > 0) {
                    sequenciaAP[iseq].FUROpres.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieTemp > 0) {
                    sequenciaAP[iseq].FUROtemp.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
                if (APFuro[indBCS].parserieFlu > 0) {
                    sequenciaAP[iseq].FUROindFlu.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.diam == 1) {
            for (int iG = 0; iG < nAPGeom; iG++) {
                if (APGeom[iG].parserieA > 0) {
                    sequenciaAP[iseq].GeomdA.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iG = 0; iG < nAPGeom; iG++) {
                if (APGeom[iG].parserieB > 0) {
                    sequenciaAP[iseq].GeomdB.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
            for (int iG = 0; iG < nAPGeom; iG++) {
                if (APGeom[iG].parserieRug > 0) {
                    sequenciaAP[iseq].GeomRug.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.kequiv == 1) {
            for (int ik = 0; ik < nAPCondEquiv; ik++) {
                if (APCondEquiv[ik].parserieK > 0) {
                    sequenciaAP[iseq].condEqui.push_back(genericoAP[iseq].generico[locdim]);
                    locdim++;
                }
            }
        }
        if (listaV.vgasinj == 1) {
            if (APGasInj.parseriePresInj > 0) {
                sequenciaAP[iseq].INJGpresinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APGasInj.parserieTemp > 0) {
                sequenciaAP[iseq].INJGtemperatura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APGasInj.parserieVazGas > 0) {
                sequenciaAP[iseq].INJGvazgas = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vpresent == 1) {
            if (APpEntrada.parseriePres > 0) {
                sequenciaAP[iseq].PEpres = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APpEntrada.parserieBet > 0) {
                sequenciaAP[iseq].PEbet = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APpEntrada.parserieTemp > 0) {
                sequenciaAP[iseq].PEtemperatura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APpEntrada.parserieTit > 0) {
                sequenciaAP[iseq].PEtit = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vvazpresent == 1) {
            if (APvpEntrada.parseriePres > 0) {
                sequenciaAP[iseq].VPEpres = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APvpEntrada.parserieBet > 0) {
                sequenciaAP[iseq].VPEbet = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APvpEntrada.parserieMass > 0) {
                sequenciaAP[iseq].VPEmass = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APvpEntrada.parserieTemp > 0) {
                sequenciaAP[iseq].VPEtemperatura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vchk == 1) {
            if (APCHK.parserieAbre > 0) {
                sequenciaAP[iseq].CHKabertura = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APCHK.parserieCD > 0) {
                sequenciaAP[iseq].CHKcd = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        if (listaV.vpocinj == 1) {
            if (APPInj.parseriePresI > 0) {
                sequenciaAP[iseq].PIpresinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APPInj.parseriePresFundo > 0) {
                sequenciaAP[iseq].PIpresfundo = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APPInj.parserieTemp > 0) {
                sequenciaAP[iseq].PItempinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
            if (APPInj.parserieVaz > 0) {
                sequenciaAP[iseq].PIvazinj = genericoAP[iseq].generico[locdim];
                locdim++;
            }
        }
        iseq++;
    }
}

int APara::inicializaSequen(int iSeq, int tipo) {

    int locDim = 0;
    int nvar = 0;
    int kontaDim = 0;
    int iVaria[dim] = {};
    while (locDim < nVariaveis) {
        for (int idim = 0; idim < dim; idim++) {
            if (vecParSerie[idim] > 0) {
                genericoAP[locDim].generico.push_back(iVaria[idim]);
                nvar++;
                kontaDim++;
                if (kontaDim == dim) {
                    locDim++;
                    kontaDim = 0;
                    iVaria[idim]++;
                    if (iVaria[idim] == vecParSerie[idim]) {
                        iVaria[idim] = 0;
                        int iOK = 0;
                        int decres = idim - 1;
                        while (iOK == 0) {
                            if (decres >= 0) {
                                iVaria[decres]++;
                                if (iVaria[decres] == vecParSerie[decres]) {
                                    iVaria[decres] = 0;
                                    decres--;
                                } else
                                    iOK = 1;
                                if (decres < 0)
                                    iOK = 1;
                            }
                            if (decres < 0)
                                iOK = 1;
                        }
                    }
                }
                if (locDim == nVariaveis)
                    break;
            }
        }
    }
    return 1;
}

void APara::atualizaGeom(int ncelG, Cel *celula, CelG *celulaG, int iduto, int tipovar, double val) {
    for (int i = 0; i < ncel; i++) {
        if (celula[i].indGeom == iduto) {
            if (tipovar == 0) {
                celula[i].duto.a = val;
                celula[i].duto.renovaD();
                celula[i].calor.geom.a = val;
                celula[i].calor.geom.renovaD();
                celula[i].dutoL.a = val;
                celula[i].dutoL.renovaD();
                celula[i].dutoR.a = val;
                celula[i].dutoR.renovaD();
            } else if (tipovar == 1) {
                celula[i].duto.b = val;
                celula[i].calor.geom.b = val;
                celula[i].dutoL.b = val;
                celula[i].dutoR.b = val;
            } else {
                celula[i].duto.rug = val;
                celula[i].calor.geom.rug = val;
                celula[i].dutoL.rug = val;
                celula[i].dutoR.rug = val;
            }
        }
    }
    for (int i = 0; i < ncelG; i++) {
        if (celulaG[i].indGeom == iduto) {
            if (tipovar == 0) {
                celulaG[i].duto.a = val;
                celulaG[i].duto.renovaD();
                celulaG[i].calor.geom.a = val;
                celulaG[i].calor.geom.renovaD();
                celulaG[i].dutoL.a = val;
                celulaG[i].dutoL.renovaD();
                celulaG[i].dutoR.a = val;
                celulaG[i].dutoR.renovaD();
            } else if (tipovar == 1) {
                celulaG[i].duto.b = val;
                celulaG[i].calor.geom.b = val;
                celulaG[i].dutoL.b = val;
                celulaG[i].dutoR.b = val;
            } else {
                celulaG[i].duto.rug = val;
                celulaG[i].calor.geom.rug = val;
                celulaG[i].dutoL.rug = val;
                celulaG[i].dutoR.rug = val;
            }
        }
    }
}

void APara::atualizaMat(int ncelG, Cel *celula, CelG *celulaG, int imat, double val) {
    for (int i = 0; i < ncel; i++) {
        for (int k = 0; k < celula[i].calor.geom.ncamadas; k++) {
            if (celula[i].calor.geom.indmat[k] == imat) {
                celula[i].calor.geom.cond[k] = val;
            }
        }
    }
    for (int i = 0; i < ncelG; i++) {
        for (int k = 0; k < celulaG[i].calor.geom.ncamadas; k++) {
            if (celulaG[i].calor.geom.indmat[k] == imat) {
                celulaG[i].calor.geom.cond[k] = val;
            }
        }
    }
}

void APara::atualizaCompRGO(double rgo, ProFlu &flui) {

    int npseudo = flui.npseudo;
    double *GivenInitialLiqComposition = new double[npseudo];
    for (int k = 0; k < npseudo; k++)
        GivenInitialLiqComposition[k] = 0;

    double *GivenInitialVapComposition = new double[npseudo];
    for (int k = 0; k < npseudo; k++)
        GivenInitialVapComposition[k] = 0;

    double *oGORAdjustedGlobalComp = new double[npseudo];
    double dAdjustedGlobalCompGOR;
    int iier;

    Marlim_AdjustGlobalCompositionForTargetGOR(flui.npseudo, flui.fracMol,
                                               flui.masMol, flui.tempCrit, flui.presCrit,
                                               flui.fatAcent, flui.TIndepPeneloux, flui.kij,
                                               flui.lij, flui.liqModel, flui.liqModel,
                                               flui.liqModel,
                                               flui.liqModel, 1.03322745279996,
                                               20., rgo, -1.0,
                                               GivenInitialLiqComposition, GivenInitialVapComposition,
                                               oGORAdjustedGlobalComp, &dAdjustedGlobalCompGOR,
                                               &iier);
    for (int k = 0; k < npseudo; k++)
        flui.fracMol[k] = oGORAdjustedGlobalComp[k];
    flui.atualizaPropCompStandard();
    delete[] GivenInitialLiqComposition;
    delete[] GivenInitialVapComposition;
    delete[] oGORAdjustedGlobalComp;
}

void APara::atualizaRGO(Cel *celula, double val) {
    if (celula[0].flui.flashCompleto != 2) {
        celula[0].flui.RGO = val;
        celula[0].flui.RenovaFluido();
        if (celula[0].acsr.tipo == 1) {
            celula[0].acsr.injg.FluidoPro.RGO = val;
            celula[0].acsr.injg.FluidoPro.RenovaFluido();
        } else if (celula[0].acsr.tipo == 2) {
            celula[0].acsr.injl.FluidoPro.RGO = val;
            celula[0].acsr.injl.FluidoPro.RenovaFluido();
        } else if (celula[0].acsr.tipo == 3) {
            celula[0].acsr.ipr.FluidoPro.RGO = val;
            celula[0].acsr.ipr.FluidoPro.RenovaFluido();
        } else if (celula[0].acsr.tipo == 10) {
            celula[0].acsr.injm.FluidoPro.RGO = val;
            celula[0].acsr.injm.FluidoPro.RenovaFluido();
        }
    } else {
        atualizaCompRGO(val, celula[0].flui);
        if (celula[0].acsr.tipo == 1) {
            celula[0].acsr.injg.FluidoPro.RGO = val;
            celula[0].acsr.injg.FluidoPro.RenovaFluido();
        } else if (celula[0].acsr.tipo == 2) {
            atualizaCompRGO(val, celula[0].acsr.injl.FluidoPro);
        } else if (celula[0].acsr.tipo == 3) {
            atualizaCompRGO(val, celula[0].acsr.ipr.FluidoPro);
        } else if (celula[0].acsr.tipo == 10) {
            atualizaCompRGO(val, celula[0].acsr.injm.FluidoPro);
        }
    }
}

void APara::atualizaBSW(Cel *celula, double val) {
    for (int i = 0; i < ncel; i++) {
        celula[i].flui.BSW = val;
        celula[i].flui.RenovaFluido();
    }
    if (celula[0].acsr.tipo == 1) {
        celula[0].acsr.injg.FluidoPro.BSW = val;
        celula[0].acsr.injg.FluidoPro.RenovaFluido();
    } else if (celula[0].acsr.tipo == 2) {
        celula[0].acsr.injl.FluidoPro.BSW = val;
        celula[0].acsr.injl.FluidoPro.RenovaFluido();
    } else if (celula[0].acsr.tipo == 3) {
        celula[0].acsr.ipr.FluidoPro.BSW = val;
        celula[0].acsr.ipr.FluidoPro.RenovaFluido();
    } else if (celula[0].acsr.tipo == 10) {
        celula[0].acsr.injm.FluidoPro.BSW = val;
        celula[0].acsr.injm.FluidoPro.RenovaFluido();
    }
}

void APara::atualizaCorrecao(int ncelG, Cel *celula, CelG *celulaG, double *vdPdLH,
                             double *vdPdLF, double *vdTdL) {
    for (int i = 0; i < ncel; i++) {
        if (celula[i].inddPdLHidro >= 0)
            celula[i].dPdLHidro = vdPdLH[celula[i].inddPdLHidro];
        if (celula[i].inddPdLFric >= 0)
            celula[i].dPdLFric = vdPdLF[celula[i].inddPdLFric];
        if (celula[i].inddTdLCor >= 0)
            celula[i].dTdLCor = vdTdL[celula[i].inddTdLCor];
    }
    for (int i = 0; i < ncelG; i++) {
        if (celulaG[i].inddPdLHidro >= 0)
            celulaG[i].dPdLHidro = vdPdLH[celulaG[i].inddPdLHidro];
        if (celulaG[i].inddPdLFric >= 0)
            celulaG[i].dPdLFric = vdPdLF[celulaG[i].inddPdLFric];
        if (celulaG[i].inddTdLCor >= 0)
            celulaG[i].dTdLCor = vdTdL[celulaG[i].inddTdLCor];
    }
}

void APara::cabecalhoAP(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                        detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                        detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq) {

    ostringstream saidaP;
    saidaP << "variaveisInteresseAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::out);

    escreveIni << " indice AP; " << " Sucesso; " << " Pressao Fundo; " << " Temperatura Plataforma; ";

    if (listaV.vpsep == 1) {
        escreveIni << " Pressao Separador; ";
    }
    if (listaV.vRGO == 1) {
        escreveIni << " RGO fluido 0; ";
    }
    if (listaV.vBSW == 1) {
        escreveIni << " BSW fluido 0; ";
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            int icel = fonteg[APFonGas[iFG].indFG].posicP;
            if (APFonGas[iFG].parserieTemp > 0) {
                escreveIni << " Fonte de Gas " << icel << " Temperatura; ";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                escreveIni << " Fonte de Gas " << icel << " Vazao Gas; ";
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                escreveIni << " Fonte de Gas " << icel << " Vazao Liq Complementar; ";
                konta3++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                escreveIni << " BCS " << icel << " Frequencia; ";
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                escreveIni << " BCS " << icel << " N. Estagios; ";
                konta2++;
            }
        }
    }

    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            int icel = fontel[APFonLiq[iFL].indFL].posicP;
            if (APFonLiq[iFL].parserieTemp > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Temperatura; ";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Vazao de Liquido; ";
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Beta; ";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Fluido; ";
                konta4++;
            }
        }
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            int icel = IPRS[APIPR[iIPR].indIPR].indcel;
            if (APIPR[iIPR].parserieTemp > 0) {
                escreveIni << " IPR " << icel << " Temperatura; ";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                escreveIni << " IPR " << icel << " P Estatica; ";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                escreveIni << " IPR " << icel << " IP; ";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                escreveIni << " IPR " << icel << " II; ";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                escreveIni << " IPR " << icel << " qMax; ";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                escreveIni << " IPR " << icel << " fluido; ";
                konta6++;
            }
        }
    }

    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            int icel = fontem[APFonMas[iFM].indFM].posicP;
            if (APFonMas[iFM].parserieTemp > 0) {
                escreveIni << " Fonte de Massa " << icel << " Temperatura; ";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                escreveIni << " Fonte de Massa " << icel << " Vazao Liquido; ";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                escreveIni << " Fonte de Massa " << icel << " Vazao Liquido Complementar; ";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                escreveIni << " Fonte de Massa " << icel << " Vazao Gas; ";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                escreveIni << " Fonte de Massa " << icel << " Fluido; ";
                konta5++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            int icel = dpreq[APDP[indBCS].indDP].posicP;
            if (APDP[indBCS].parserieDP > 0) {
                escreveIni << " Delta Pressao " << icel << " Del. Pres.; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                escreveIni << " Corr. Pres. Hidro; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                escreveIni << " Corr. Pres. Fric; ";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            if (APdTdL[indBCS].parserieDT > 0) {
                escreveIni << " Corr. Temperatura; ";
                konta1++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            int icel = bvol[APBVOL[indBCS].indBV].posicP;
            if (APBVOL[indBCS].parserieFreq > 0) {
                escreveIni << " B. Volumetrica " << icel << " Frequencia; ";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                escreveIni << " B. Volumetrica " << icel << " Capacidade; ";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                escreveIni << " B. Volumetrica " << icel << " Fator Politropico; ";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            int icel = valv[APValv[indBCS].indV].posicP;
            if (APValv[indBCS].parserieAbre > 0) {
                escreveIni << " Valvula " << icel << " Abertura; ";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                escreveIni << " Valvula " << icel << " CD; ";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            int icel = furo[APFuro[indBCS].indFuro].posicP;
            if (APFuro[indBCS].parserieAbre > 0) {
                escreveIni << " Vazamento " << icel << " Abertura; ";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                escreveIni << " Vazamento " << icel << " CD; ";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                escreveIni << " Valvula " << icel << " Beta; ";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                escreveIni << " Valvula " << icel << " Pressao; ";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                escreveIni << " Valvula " << icel << " Temperatura; ";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                escreveIni << " Valvula " << icel << " Fluido; ";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                int igeo = APGeom[iG].indGeom;
                escreveIni << " Secao Transversal " << igeo << " Maior Diam Interno; ";
                konta1++;
            }
            if (APGeom[iG].parserieB > 0) {
                int igeo = APGeom[iG].indGeom;
                escreveIni << " Secao Transversal " << igeo << " Menor Diam Interno; ";
                konta2++;
            }
            if (APGeom[iG].parserieRug > 0) {
                int igeo = APGeom[iG].indGeom;
                escreveIni << " Secao Transversal " << igeo << " Rugosidade; ";
                konta3++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                int imat = APCondEquiv[ik].indMat;
                escreveIni << " Material " << imat << " Condutividade; ";
                konta1++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            escreveIni << " Pressao de Injecao; ";
        }
        if (APGasInj.parserieTemp > 0) {
            escreveIni << " Temperatura de Injecao; ";
        }
        if (APGasInj.parserieVazGas > 0) {
            escreveIni << " Vazao de Injecao; ";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            escreveIni << " Pressao de Contorno; ";
        }
        if (APpEntrada.parserieBet > 0) {
            escreveIni << " Beta de Contorno; ";
        }
        if (APpEntrada.parserieTemp > 0) {
            escreveIni << " Temperatura de Contorno; ";
        }
        if (APpEntrada.parserieTit > 0) {
            escreveIni << " Titulo de Contorno; ";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            escreveIni << " Pressao de Contorno; ";
        }
        if (APvpEntrada.parserieBet > 0) {
            escreveIni << " Beta de Contorno; ";
        }
        if (APvpEntrada.parserieMass > 0) {
            escreveIni << " Vazao de Contorno; ";
        }
        if (APvpEntrada.parserieTemp > 0) {
            escreveIni << " Temperatura de Contorno; ";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            escreveIni << " Abertura de Choke; ";
        }
        if (APCHK.parserieCD > 0) {
            escreveIni << " CD de Choke; ";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            escreveIni << " Pressao a Jusante; ";
        }
        if (APPInj.parseriePresFundo > 0) {
            escreveIni << " Pressao a Montante; ";
        }
        if (APPInj.parserieTemp > 0) {
            escreveIni << " Temperatura de Injecao; ";
        }
        if (APPInj.parserieVaz > 0) {
            escreveIni << " Vazao de Injecao; ";
        }
    }
    escreveIni << endl;
    escreveIni.close();
}

void APara::cabecalhoAPImex(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                            detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                            detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq) {

    ostringstream saidaP;
    saidaP << "variaveisInteresseAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::out);

    escreveIni << " indice AP; " << " Sucesso; " << " Pressao Fundo; " << " Temperatura Plataforma; ";

    if (listaV.vRGO == 1) {
        escreveIni << " RGO fluido 0; ";
    }
    if (listaV.vBSW == 1) {
        escreveIni << " BSW fluido 0; ";
    }
    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            int icel = fontel[APFonLiq[iFL].indFL].posicP;
            if (APFonLiq[iFL].parserieTemp > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Temperatura; ";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Vazao de Liquido; ";
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Beta; ";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                escreveIni << " Fonte de Liquido " << icel << " Fluido; ";
                konta4++;
            }
        }
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            int icel = fonteg[APFonGas[iFG].indFG].posicP;
            if (APFonGas[iFG].parserieTemp > 0) {
                escreveIni << " Fonte de Gas " << icel << " Temperatura; ";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                escreveIni << " Fonte de Gas " << icel << " Vazao Gas; ";
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                escreveIni << " Fonte de Gas " << icel << " Vazao Liq Complementar; ";
                konta3++;
            }
        }
    }
    if (listaV.vpsep == 1) {
        escreveIni << " Pressao Separador; ";
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            int icel = IPRS[APIPR[iIPR].indIPR].indcel;
            if (APIPR[iIPR].parserieTemp > 0) {
                escreveIni << " IPR " << icel << " Temperatura; ";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                escreveIni << " IPR " << icel << " P Estatica; ";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                escreveIni << " IPR " << icel << " IP; ";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                escreveIni << " IPR " << icel << " II; ";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                escreveIni << " IPR " << icel << " qMax; ";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                escreveIni << " IPR " << icel << " fluido; ";
                konta6++;
            }
        }
    }
    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            int icel = fontem[APFonMas[iFM].indFM].posicP;
            if (APFonMas[iFM].parserieTemp > 0) {
                escreveIni << " Fonte de Massa " << icel << " Temperatura; ";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                escreveIni << " Fonte de Massa " << icel << " Vazao Liquido; ";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                escreveIni << " Fonte de Massa " << icel << " Vazao Liquido Complementar; ";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                escreveIni << " Fonte de Massa " << icel << " Vazao Gas; ";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                escreveIni << " Fonte de Massa " << icel << " Fluido; ";
                konta5++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                escreveIni << " BCS " << icel << " Frequencia; ";
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                escreveIni << " BCS " << icel << " N. Estagios; ";
                konta2++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            int icel = dpreq[APDP[indBCS].indDP].posicP;
            if (APDP[indBCS].parserieDP > 0) {
                escreveIni << " Delta Pressao " << icel << " Del. Pres.; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                escreveIni << " Corr. Pres. Hidro; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                escreveIni << " Corr. Pres. Fric; ";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            if (APdTdL[indBCS].parserieDT > 0) {
                escreveIni << " Corr. Temperatura; ";
                konta1++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            int icel = bvol[APBVOL[indBCS].indBV].posicP;
            if (APBVOL[indBCS].parserieFreq > 0) {
                escreveIni << " B. Volumetrica " << icel << " Frequencia; ";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                escreveIni << " B. Volumetrica " << icel << " Capacidade; ";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                escreveIni << " B. Volumetrica " << icel << " Fator Politropico; ";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            int icel = valv[APValv[indBCS].indV].posicP;
            if (APValv[indBCS].parserieAbre > 0) {
                escreveIni << " Valvula " << icel << " Abertura; ";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                escreveIni << " Valvula " << icel << " CD; ";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            int icel = furo[APFuro[indBCS].indFuro].posicP;
            if (APFuro[indBCS].parserieAbre > 0) {
                escreveIni << " Vazamento " << icel << " Abertura; ";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                escreveIni << " Vazamento " << icel << " CD; ";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                escreveIni << " Valvula " << icel << " Beta; ";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                escreveIni << " Valvula " << icel << " Pressao; ";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                escreveIni << " Valvula " << icel << " Temperatura; ";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                escreveIni << " Valvula " << icel << " Fluido; ";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                int igeo = APGeom[iG].indGeom;
                escreveIni << " Secao Transversal " << igeo << " Maior Diam Interno; ";
                konta1++;
            }
            if (APGeom[iG].parserieB > 0) {
                int igeo = APGeom[iG].indGeom;
                escreveIni << " Secao Transversal " << igeo << " Menor Diam Interno; ";
                konta2++;
            }
            if (APGeom[iG].parserieRug > 0) {
                int igeo = APGeom[iG].indGeom;
                escreveIni << " Secao Transversal " << igeo << " Rugosidade; ";
                konta3++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                int imat = APCondEquiv[ik].indMat;
                escreveIni << " Material " << imat << " Condutividade; ";
                konta1++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            escreveIni << " Pressao de Injecao; ";
        }
        if (APGasInj.parserieTemp > 0) {
            escreveIni << " Temperatura de Injecao; ";
        }
        if (APGasInj.parserieVazGas > 0) {
            escreveIni << " Vazao de Injecao; ";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            escreveIni << " Pressao de Contorno; ";
        }
        if (APpEntrada.parserieBet > 0) {
            escreveIni << " Beta de Contorno; ";
        }
        if (APpEntrada.parserieTemp > 0) {
            escreveIni << " Temperatura de Contorno; ";
        }
        if (APpEntrada.parserieTit > 0) {
            escreveIni << " Titulo de Contorno; ";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            escreveIni << " Pressao de Contorno; ";
        }
        if (APvpEntrada.parserieBet > 0) {
            escreveIni << " Beta de Contorno; ";
        }
        if (APvpEntrada.parserieMass > 0) {
            escreveIni << " Vazao de Contorno; ";
        }
        if (APvpEntrada.parserieTemp > 0) {
            escreveIni << " Temperatura de Contorno; ";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            escreveIni << " Abertura de Choke; ";
        }
        if (APCHK.parserieCD > 0) {
            escreveIni << " CD de Choke; ";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            escreveIni << " Pressao a Jusante; ";
        }
        if (APPInj.parseriePresFundo > 0) {
            escreveIni << " Pressao a Montante; ";
        }
        if (APPInj.parserieTemp > 0) {
            escreveIni << " Temperatura de Injecao; ";
        }
        if (APPInj.parserieVaz > 0) {
            escreveIni << " Vazao de Injecao; ";
        }
    }
    escreveIni << endl;
    escreveIni.close();
}

void APara::imprimeVarInteresseAP(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                                  detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                                  detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq, int seq) {

    ostringstream saidaP;
    saidaP << "variaveisInteresseAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::app);

    int ind;
    if (listaV.vpsep == 1) {
        ind = sequenciaAP[seq].PSEPpres;
        escreveIni << "   " << APPsep.pres[ind] << " ; ";
    }
    if (listaV.vRGO == 1) {
        ind = sequenciaAP[seq].RGO;
        escreveIni << "   " << APRGO.RGO[ind] << " ; ";
    }
    if (listaV.vBSW == 1) {
        ind = sequenciaAP[seq].BSW;
        flup[0].BSW = APBSW.BSW[ind];
        flup[0].RenovaFluido();
        atualizaBSW(celula, APBSW.BSW[ind]);
        escreveIni << "   " << APBSW.BSW[ind] << " ; ";
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieTemp > 0) {
                ind = sequenciaAP[seq].FGtemp[konta1];
                escreveIni << "   " << APFonGas[iFG].temp[ind] << " ; ";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                ind = sequenciaAP[seq].FGvazgas[konta2];
                escreveIni << "   " << APFonGas[iFG].vazgas[ind] << " ; ";
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                ind = sequenciaAP[seq].FGvazcomp[konta3];
                escreveIni << "   " << APFonGas[iFG].vazcomp[ind] << " ; ";
                konta3++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BCSfreq[konta1];
                escreveIni << "   " << APBCS[indBCS].freq[ind] << " ; ";
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                ind = sequenciaAP[seq].BCSnestag[konta2];
                celula[icel].acsr.bcs.nestag = APBCS[indBCS].nestag[ind];
                escreveIni << seq << " : " << " indice BCS = " << konta2 << " celula Fonte " << icel << " indice N. Estagios= " << ind << " N. Estagios= " << APBCS[indBCS].nestag[ind] << "\n";
                konta2++;
            }
        }
    }

    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieTemp > 0) {
                ind = sequenciaAP[seq].FLtemp[konta1];
                escreveIni << "  " << APFonLiq[iFL].temp[ind] << " ; ";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                ind = sequenciaAP[seq].FLvazliq[konta2];
                escreveIni << "   " << APFonLiq[iFL].vazliq[ind] << " ; ";
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                ind = sequenciaAP[seq].FLbet[konta3];
                escreveIni << "   " << APFonLiq[iFL].bet[ind] << " ; ";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                ind = sequenciaAP[seq].FLindfluP[konta4];
                int indFlu = APFonLiq[iFL].indfluP[ind];
                escreveIni << "   " << indFlu << " ; ";
                konta4++;
            }
        }
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieTemp > 0) {
                ind = sequenciaAP[seq].IPRtemp[konta1];
                escreveIni << "   " << APIPR[iIPR].temp[ind] << " ; ";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                ind = sequenciaAP[seq].IPRpres[konta2];
                escreveIni << "   " << APIPR[iIPR].pres[ind] << " ; ";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                ind = sequenciaAP[seq].IPRip[konta3];
                escreveIni << seq << "   " << APIPR[iIPR].ip[ind] << " ; ";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                ind = sequenciaAP[seq].IPRjp[konta4];
                escreveIni << "   " << APIPR[iIPR].jp[ind] << " ; ";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                ind = sequenciaAP[seq].IPRqMax[konta5];
                escreveIni << "   " << APIPR[iIPR].qMax[ind] << " ; ";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                ind = sequenciaAP[seq].IPRindfluP[konta6];
                escreveIni << "   " << APIPR[iIPR].indfluP[ind] << " ; ";
                konta6++;
            }
        }
    }

    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieTemp > 0) {
                ind = sequenciaAP[seq].FMtemp[konta1];
                escreveIni << "   " << APFonMas[iFM].temp[ind] << " ; ";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                ind = sequenciaAP[seq].FMvazMasP[konta2];
                escreveIni << "   " << APFonMas[iFM].vazMasP[ind] << " ; ";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                ind = sequenciaAP[seq].FMvazMasC[konta3];
                escreveIni << "   " << APFonMas[iFM].vazMasC[ind] << " ; ";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                ind = sequenciaAP[seq].FMvazMasG[konta4];
                escreveIni << "   " << APFonMas[iFM].vazMasG[ind] << " ; ";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                ind = sequenciaAP[seq].FMindfluP[konta5];
                escreveIni << "   " << APFonMas[iFM].indfluP[ind] << " ; ";
                konta5++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                escreveIni << "   " << APDP[indBCS].dp[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                ind = sequenciaAP[seq].DPdLH[konta1];
                escreveIni << "   " << APdPdLH[indBCS].dPHidro[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                ind = sequenciaAP[seq].DPdLF[konta1];
                escreveIni << "   " << APdPdLF[indBCS].dPFric[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            if (APdTdL[indBCS].parserieDT > 0) {
                ind = sequenciaAP[seq].DTdL[konta1];
                escreveIni << "   " << APdTdL[indBCS].dT[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BVOLfreq[konta1];
                escreveIni << "   " << APBVOL[indBCS].freq[ind] << " ; ";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                ind = sequenciaAP[seq].BVOLcapacidade[konta2];
                escreveIni << "   " << APBVOL[indBCS].capacidade[ind] << " ; ";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                ind = sequenciaAP[seq].BVOLnpoli[konta3];
                escreveIni << "   " << APBVOL[indBCS].npoli[ind] << " ; ";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            if (APValv[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].VALVabertura[konta1];
                escreveIni << "   " << APValv[indBCS].abertura[ind] << " ; ";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].VALVcd[konta2];
                escreveIni << " : " << APValv[indBCS].cd[ind] << " ; ";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].FUROabertura[konta1];
                escreveIni << "   " << APFuro[indBCS].abertura[ind] << " ; ";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].FUROcd[konta2];
                escreveIni << "   " << APFuro[indBCS].cd[ind] << " ; ";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                ind = sequenciaAP[seq].FURObeta[konta3];
                escreveIni << "   " << APFuro[indBCS].beta[ind] << " ; ";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                ind = sequenciaAP[seq].FUROpres[konta4];
                escreveIni << "   " << APFuro[indBCS].pres[ind] << " ; ";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                ind = sequenciaAP[seq].FUROtemp[konta5];
                escreveIni << "   " << " Temperatura= " << APFuro[indBCS].temp[ind] << " ; ";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                ind = sequenciaAP[seq].FUROindFlu[konta6];
                int indFlu = APFuro[indBCS].indFlu[ind];
                escreveIni << "   " << indFlu << " ; ";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                ind = sequenciaAP[seq].GeomdA[konta1];
                escreveIni << "   " << APGeom[iG].dA[ind] << " ; ";
                konta1++;
            }
            if (APGeom[iG].parserieB > 0) {
                ind = sequenciaAP[seq].GeomdB[konta2];
                escreveIni << "   " << APGeom[iG].dB[ind] << " ; ";
                konta2++;
            }
            if (APGeom[iG].parserieRug > 0) {
                ind = sequenciaAP[seq].GeomdB[konta3];
                escreveIni << "   " << APGeom[iG].rug[ind] << " ; ";
                konta3++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                ind = sequenciaAP[seq].condEqui[konta1];
                escreveIni << "   " << APCondEquiv[ik].cond[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            ind = sequenciaAP[seq].INJGpresinj;
            escreveIni << "   " << APGasInj.presinj[ind] << " ; ";
        }
        if (APGasInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].INJGtemperatura;
            escreveIni << "   " << APGasInj.temperatura[ind] << " ; ";
        }
        if (APGasInj.parserieVazGas > 0) {
            ind = sequenciaAP[seq].INJGvazgas;
            escreveIni << "   " << APGasInj.vazgas[ind] << " ; ";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].PEpres;
            escreveIni << "   " << APpEntrada.pres[ind] << " ; ";
        }
        if (APpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].PEbet;
            escreveIni << "   " << APpEntrada.bet[ind] << " ; ";
        }
        if (APpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].PEtemperatura;
            escreveIni << "   " << APpEntrada.temperatura[ind] << " ; ";
        }
        if (APpEntrada.parserieTit > 0) {
            ind = sequenciaAP[seq].PEtit;
            escreveIni << "   " << APpEntrada.tit[ind] << " ; ";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].VPEpres;
            escreveIni << "   " << APvpEntrada.pres[ind] << " ; ";
        }
        if (APvpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].VPEbet;
            escreveIni << "   " << APvpEntrada.bet[ind] << " ; ";
        }
        if (APvpEntrada.parserieMass > 0) {
            ind = sequenciaAP[seq].VPEmass;
            escreveIni << "   " << APvpEntrada.mass[ind] << " ; ";
        }
        if (APvpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].VPEtemperatura;
            escreveIni << "   " << APvpEntrada.temperatura[ind] << " ; ";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            ind = sequenciaAP[seq].CHKabertura;
            escreveIni << "   " << APCHK.abertura[ind] << " ; ";
        }
        if (APCHK.parserieCD > 0) {
            ind = sequenciaAP[seq].CHKcd;
            escreveIni << "   " << chokeSup.cdchk << " ; ";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            ind = sequenciaAP[seq].PIpresinj;
            escreveIni << " : " << APPInj.presfundo[ind] << " ; ";
        }
        if (APPInj.parseriePresFundo > 0) {
            ind = sequenciaAP[seq].PIpresfundo;
            escreveIni << " : " << APPInj.presinj[ind] << " ; ";
        }
        if (APPInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].PItempinj;
            escreveIni << " : " << APPInj.tempinj[ind] << " ; ";
        }
        if (APPInj.parserieVaz > 0) {
            ind = sequenciaAP[seq].PIvazinj;
            escreveIni << " : " << APPInj.vazinj[ind] << " ; ";
        }
    }
    escreveIni << endl;

    escreveIni.close();
}

void APara::selecaoAP(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                      detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                      detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                      double &pGSup, double &temperatura, double &presiniG, double &tempiniG, double &vazgasG,
                      double &presE, double &tempE, double &titE, double &betaE, double &vazE, int seq, int &indCHK,
                      double *vdPdLH, double *vdPdLF, double *vdTdL, int imprime) {

    ostringstream saidaP;
    saidaP << "relacaoAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::app);

    indCHK = 0;
    int ind;
    if (listaV.vpsep == 1) {
        ind = sequenciaAP[seq].PSEPpres;
        pGSup = APPsep.pres[ind];
        if (imprime == 1) {
            escreveIni << seq << " : " << " indice PSep = " << ind << " Pressao Separador= " << pGSup << "\n";
        }
    }
    if (listaV.vRGO == 1) {
        ind = sequenciaAP[seq].RGO;
        if (flup[0].flashCompleto != 2) {
            flup[0].RGO = APRGO.RGO[ind];
            flup[0].RenovaFluido();
        } else
            atualizaCompRGO(APRGO.RGO[ind], flup[0]);
        atualizaRGO(celula, APRGO.RGO[ind]);
        if (imprime == 1) {
            escreveIni << seq << " : " << " indice RGO = " << ind << " RGO fluido 0= " << flup[0].RGO << "\n";
        }
    }
    if (listaV.vBSW == 1) {
        ind = sequenciaAP[seq].BSW;
        flup[0].BSW = APBSW.BSW[ind];
        flup[0].RenovaFluido();
        atualizaBSW(celula, APBSW.BSW[ind]);
        if (imprime == 1)
            escreveIni << seq << " : " << " indice BSW = " << ind << " BSW fluido 0= " << flup[0].BSW << "\n";
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            int icel = fonteg[APFonGas[iFG].indFG].posicP;
            if (APFonGas[iFG].parserieTemp > 0) {
                ind = sequenciaAP[seq].FGtemp[konta1];
                celula[icel].acsr.injg.temp = APFonGas[iFG].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Gas = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APFonGas[iFG].temp[ind] << "\n";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                ind = sequenciaAP[seq].FGvazgas[konta2];
                celula[icel].acsr.injg.QGas = APFonGas[iFG].vazgas[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Gas = " << konta2 << " celula Fonte " << icel << " indice Vazao Gas = " << ind << " Vazao Gas= " << APFonGas[iFG].vazgas[ind] << "\n";
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                ind = sequenciaAP[seq].FGvazcomp[konta3];
                celula[icel].acsr.injg.razCompGas = APFonGas[iFG].vazcomp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Gas = " << konta3 << " celula Fonte " << icel << " indice Vazao Liq Complementar = " << ind << " Vazao Liq Complementar= " << APFonGas[iFG].vazcomp[ind] << "\n";
                konta3++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BCSfreq[konta1];
                celula[icel].acsr.bcs.freqnova = APBCS[indBCS].freq[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice BCS = " << konta1 << " celula Fonte " << icel << " indice Frequencia= " << ind << " Frequencia= " << APBCS[indBCS].freq[ind] << "\n";
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                ind = sequenciaAP[seq].BCSnestag[konta2];
                celula[icel].acsr.bcs.nestag = APBCS[indBCS].nestag[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice BCS = " << konta2 << " celula Fonte " << icel << " indice N. Estagios= " << ind << " N. Estagios= " << APBCS[indBCS].nestag[ind] << "\n";
                konta2++;
            }
        }
    }

    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            int icel = fontel[APFonLiq[iFL].indFL].posicP;
            if (APFonLiq[iFL].parserieTemp > 0) {
                ind = sequenciaAP[seq].FLtemp[konta1];
                celula[icel].acsr.injl.temp = APFonLiq[iFL].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APFonLiq[iFL].temp[ind] << "\n";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                ind = sequenciaAP[seq].FLvazliq[konta2];
                celula[icel].acsr.injl.QLiq = APFonLiq[iFL].vazliq[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta2 << " celula Fonte " << icel << " indice Vazao Liq = " << ind << " Vazao de Liquido= " << APFonLiq[iFL].vazliq[ind] << "\n";
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                ind = sequenciaAP[seq].FLbet[konta3];
                celula[icel].acsr.injl.bet = APFonLiq[iFL].bet[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta3 << " celula Fonte " << icel << " indice Beta = " << ind << " Beta= " << APFonLiq[iFL].bet[ind] << "\n";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                ind = sequenciaAP[seq].FLindfluP[konta4];
                int indFlu = APFonLiq[iFL].indfluP[ind];
                celula[icel].acsr.injl.FluidoPro = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta4 << " celula Fonte " << icel << " indice Fluido = " << ind << " Fluido= " << indFlu << "\n";
                konta4++;
            }
        }
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            int icel = IPRS[APIPR[iIPR].indIPR].indcel;
            if (APIPR[iIPR].parserieTemp > 0) {
                ind = sequenciaAP[seq].IPRtemp[konta1];
                celula[icel].acsr.ipr.Tres = APIPR[iIPR].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APIPR[iIPR].temp[ind] << "\n";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                ind = sequenciaAP[seq].IPRpres[konta2];
                celula[icel].acsr.ipr.Pres = APIPR[iIPR].pres[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta2 << " celula Fonte " << icel << " indice P Estatica = " << ind << " P Estatica= " << APIPR[iIPR].pres[ind] << "\n";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                ind = sequenciaAP[seq].IPRip[konta3];
                celula[icel].acsr.ipr.ip = APIPR[iIPR].ip[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta3 << " celula Fonte " << icel << " indice IP = " << ind << " IP= " << APIPR[iIPR].ip[ind] << "\n";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                ind = sequenciaAP[seq].IPRjp[konta4];
                celula[icel].acsr.ipr.ij = APIPR[iIPR].jp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta4 << " celula Fonte " << icel << " indice II = " << ind << " II= " << APIPR[iIPR].jp[ind] << "\n";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                ind = sequenciaAP[seq].IPRqMax[konta5];
                celula[icel].acsr.ipr.qMax = APIPR[iIPR].qMax[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta5 << " celula Fonte " << icel << " indice qMax = " << ind << " qMax= " << APIPR[iIPR].qMax[ind] << "\n";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                ind = sequenciaAP[seq].IPRindfluP[konta6];
                int indFlu = APIPR[iIPR].indfluP[ind];
                celula[icel].acsr.ipr.FluidoPro = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta6 << " celula Fonte " << icel << " indice Fluido = " << ind << " fluido= " << APIPR[iIPR].indfluP[ind] << "\n";
                konta6++;
            }
        }
    }

    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            int icel = fontem[APFonMas[iFM].indFM].posicP;
            if (APFonMas[iFM].parserieTemp > 0) {
                ind = sequenciaAP[seq].FMtemp[konta1];
                celula[icel].acsr.injm.temp = APFonMas[iFM].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APFonMas[iFM].temp[ind] << "\n";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                ind = sequenciaAP[seq].FMvazMasP[konta2];
                celula[icel].acsr.injm.MassP = APFonMas[iFM].vazMasP[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta2 << " celula Fonte " << icel << " indice Vazao Liquido= " << ind << " Vazao Liquido= " << APFonMas[iFM].vazMasP[ind] << "\n";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                ind = sequenciaAP[seq].FMvazMasC[konta3];
                celula[icel].acsr.injm.MassC = APFonMas[iFM].vazMasC[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta3 << " celula Fonte " << icel << " indice Vazao Liquido Complementar= " << ind << " Vazao Liquido Complementar= " << APFonMas[iFM].vazMasC[ind] << "\n";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                ind = sequenciaAP[seq].FMvazMasG[konta4];
                celula[icel].acsr.injm.MassG = APFonMas[iFM].vazMasG[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta4 << " celula Fonte " << icel << " indice Vazao Gas= " << ind << " Vazao Gas= " << APFonMas[iFM].vazMasG[ind] << "\n";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                ind = sequenciaAP[seq].FMindfluP[konta5];
                int indFlu = APFonMas[iFM].indfluP[ind];
                celula[icel].acsr.injm.FluidoPro = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta5 << " celula Fonte " << icel << " indice Fluido= " << ind << " Fluido= " << APFonMas[iFM].indfluP[ind] << "\n";
                konta5++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            int icel = dpreq[APDP[indBCS].indDP].posicP;
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                celula[icel].acsr.delp = APDP[indBCS].dp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Del. Pres, = " << konta1 << " celula Fonte " << icel << " indice Del. Pres.= " << ind << " Del. Pres.= " << APDP[indBCS].dp[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            int icel = APdPdLH[indBCS].indDPHidro;
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                ind = sequenciaAP[seq].DPdLH[konta1];
                vdPdLH[icel] = APdPdLH[indBCS].dPHidro[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Corr. Pres. Hidro, = " << konta1 << " indice da colecao de fatores de correcao " << icel << " indice na AP.= " << ind << " Corr. Pres. Hidro= " << APdPdLH[indBCS].dPHidro[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            int icel = APdPdLF[indBCS].indDPFric;
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                ind = sequenciaAP[seq].DPdLF[konta1];
                vdPdLF[icel] = APdPdLF[indBCS].dPFric[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Corr. Pres. Fric., = " << konta1 << " indice da colecao de fatores de correcao " << icel << " indice na AP.= " << ind << " Corr. Pres. Fric.= " << APdPdLF[indBCS].dPFric[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            int icel = APdTdL[indBCS].indDT;
            if (APdTdL[indBCS].parserieDT > 0) {
                ind = sequenciaAP[seq].DTdL[konta1];
                vdTdL[icel] = APdTdL[indBCS].dT[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Corr. Temperatura, = " << konta1 << " indice da colecao de fatores de correcao " << icel << " indice na AP.= " << ind << " Corr. Temperatura= " << APdTdL[indBCS].dT[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1 || listaV.vdpF == 1 || listaV.vdt == 1)
        atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            int icel = bvol[APBVOL[indBCS].indBV].posicP;
            if (APBVOL[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BVOLfreq[konta1];
                celula[icel].acsr.bvol.freq = APBVOL[indBCS].freq[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Bomba Vol., = " << konta1 << " celula Fonte " << icel << " indice Frequencia= " << ind << " Frequencia= " << APBVOL[indBCS].freq[ind] << "\n";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                ind = sequenciaAP[seq].BVOLcapacidade[konta2];
                celula[icel].acsr.bvol.capacidade = APBVOL[indBCS].capacidade[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Bomba Vol., = " << konta2 << " celula Fonte " << icel << " indice Capacidade= " << ind << " Capacidade= " << APBVOL[indBCS].capacidade[ind] << "\n";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                ind = sequenciaAP[seq].BVOLnpoli[konta3];
                celula[icel].acsr.bvol.npoli = APBVOL[indBCS].npoli[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Bomba Vol., = " << konta3 << " celula Fonte " << icel << " indice Fator Politropico= " << ind << " Fator Politropico= " << APBVOL[indBCS].npoli[ind] << "\n";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            int icel = valv[APValv[indBCS].indV].posicP;
            if (APValv[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].VALVabertura[konta1];
                celula[icel].acsr.chk.AreaGarg = APValv[indBCS].abertura[ind] * celula[icel].acsr.chk.AreaTub;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Valvula, = " << konta1 << " celula Fonte " << icel << " indice Abertura= " << ind << " Abertura= " << APValv[indBCS].abertura[ind] << "\n";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].VALVcd[konta2];
                celula[icel].acsr.chk.cdchk = APValv[indBCS].cd[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Valvula, = " << konta2 << " celula Fonte " << icel << " indice CD= " << ind << " CD= " << APValv[indBCS].cd[ind] << "\n";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            int icel = furo[APFuro[indBCS].indFuro].posicP;
            if (APFuro[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].FUROabertura[konta1];
                celula[icel].acsr.fontechk.abertura = APFuro[indBCS].abertura[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta1 << " celula Fonte " << icel << " indice Abertura= " << ind << " Abertura= " << APFuro[indBCS].abertura[ind] << "\n";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].FUROcd[konta2];
                celula[icel].acsr.fontechk.cd = APFuro[indBCS].cd[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta2 << " celula Fonte " << icel << " indice CD= " << ind << " CD= " << APFuro[indBCS].cd[ind] << "\n";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                ind = sequenciaAP[seq].FURObeta[konta3];
                celula[icel].acsr.fontechk.bet = APFuro[indBCS].beta[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta3 << " celula Fonte " << icel << " indice Beta= " << ind << " Beta= " << APFuro[indBCS].beta[ind] << "\n";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                ind = sequenciaAP[seq].FUROpres[konta4];
                celula[icel].acsr.fontechk.pamb = APFuro[indBCS].pres[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta4 << " celula Fonte " << icel << " indice Pressao= " << ind << " Pressao= " << APFuro[indBCS].pres[ind] << "\n";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                ind = sequenciaAP[seq].FUROtemp[konta5];
                celula[icel].acsr.fontechk.tempT = APFuro[indBCS].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta5 << " celula Fonte " << icel << " indice Temperatura= " << ind << " Temperatura= " << APFuro[indBCS].temp[ind] << "\n";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                ind = sequenciaAP[seq].FUROindFlu[konta6];
                int indFlu = APFuro[indBCS].indFlu[ind];
                celula[icel].acsr.fontechk.fluidoPamb = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta6 << " celula Fonte " << icel << " indice Fluido= " << ind << " Fluido= " << indFlu << "\n";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                ind = sequenciaAP[seq].GeomdA[konta1];
                int igeo = APGeom[iG].indGeom;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Secao Transversal = " << konta1 << " secao transversal " << igeo << " indice Maior Diametro = " << ind << " Maior Diam Interno= " << APGeom[iG].dA[ind] << "\n";
                konta1++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 0, APGeom[iG].dA[ind]);
            }
            if (APGeom[iG].parserieB > 0) {
                ind = sequenciaAP[seq].GeomdB[konta2];
                int igeo = APGeom[iG].indGeom;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Secao Transversal = " << konta2 << " secao transversal " << igeo << " indice Menor Diametro = " << ind << " Menor Diam Interno= " << APGeom[iG].dB[ind] << "\n";
                konta2++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 1, APGeom[iG].dB[ind]);
            }
            if (APGeom[iG].parserieRug > 0) {
                ind = sequenciaAP[seq].GeomdB[konta3];
                int igeo = APGeom[iG].indGeom;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Secao Transversal = " << konta3 << " secao transversal " << igeo << " indice Rugosidade = " << ind << " Rugosidade= " << APGeom[iG].rug[ind] << "\n";
                konta3++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 2, APGeom[iG].rug[ind]);
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                ind = sequenciaAP[seq].condEqui[konta1];
                int imat = APCondEquiv[ik].indMat;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Material = " << konta1 << " material " << imat << " indice Condutividade = " << ind << " Condutividade= " << APCondEquiv[ik].cond[ind] << "\n";
                konta1++;
                atualizaMat(ncelG, celula, celulaG, imat, APCondEquiv[ik].cond[ind]);
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            ind = sequenciaAP[seq].INJGpresinj;
            presiniG = APGasInj.presinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pressao Injecao = " << ind << " Pressao Injecao= " << APGasInj.presinj[ind] << "\n";
        }
        if (APGasInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].INJGtemperatura;
            tempiniG = APGasInj.temperatura[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp Injecao = " << ind << " Temp Injecao= " << APGasInj.temperatura[ind] << "\n";
        }
        if (APGasInj.parserieVazGas > 0) {
            ind = sequenciaAP[seq].INJGvazgas;
            vazgasG = APGasInj.vazgas[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Vaz Injecao = " << ind << " Vaz Injecao= " << APGasInj.vazgas[ind] << "\n";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].PEpres;
            presE = APpEntrada.pres[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Contorno = " << ind << " Pres. Contorno= " << presE << "\n";
        }
        if (APpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].PEbet;
            betaE = APpEntrada.bet[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Beta Contorno = " << ind << " Beta Contorno= " << betaE << "\n";
        }
        if (APpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].PEtemperatura;
            tempE = APpEntrada.temperatura[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp. Contorno = " << ind << " Temp. Contorno= " << tempE << "\n";
        }
        if (APpEntrada.parserieTit > 0) {
            ind = sequenciaAP[seq].PEtit;
            titE = APpEntrada.tit[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Titulo Contorno = " << ind << " Titulo Contorno= " << titE << "\n";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].VPEpres;
            presE = APvpEntrada.pres[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Contorno = " << ind << " Pres. Contorno= " << presE << "\n";
        }
        if (APvpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].VPEbet;
            betaE = APvpEntrada.bet[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Beta Contorno = " << ind << " Beta Contorno= " << betaE << "\n";
        }
        if (APvpEntrada.parserieMass > 0) {
            ind = sequenciaAP[seq].VPEmass;
            vazE = APvpEntrada.mass[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Vazao Contorno = " << ind << " Vazao Contorno= " << vazE << "\n";
        }
        if (APvpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].VPEtemperatura;
            tempE = APvpEntrada.temperatura[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp. Contorno = " << ind << " Temp. Contorno= " << tempE << "\n";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            ind = sequenciaAP[seq].CHKabertura;
            indCHK = ind;
            chokeSup.AreaGarg = APCHK.abertura[ind] * chokeSup.AreaTub;
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Abertura Choke = " << ind << " Abertura Choke= " << APCHK.abertura[ind] << "\n";
        }
        if (APCHK.parserieCD > 0) {
            ind = sequenciaAP[seq].CHKcd;
            chokeSup.cdchk = APCHK.cd[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice CD Choke = " << ind << " CD Choke= " << chokeSup.cdchk << "\n";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            ind = sequenciaAP[seq].PIpresinj;
            presE = APPInj.presfundo[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Jusante = " << ind << " Pres Jusante= " << presE << "\n";
        }
        if (APPInj.parseriePresFundo > 0) {
            ind = sequenciaAP[seq].PIpresfundo;
            pGSup = APPInj.presinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Montante = " << ind << " Pres Montante= " << pGSup << "\n";
        }
        if (APPInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].PItempinj;
            tempE = APPInj.tempinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp. Injecao = " << ind << " Temp. Injecao= " << tempE << "\n";
        }
        if (APPInj.parserieVaz > 0) {
            ind = sequenciaAP[seq].PIvazinj;
            vazE = APPInj.vazinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Vazao Poco Injecao = " << ind << " Vazao Poco Injecao= " << vazE << "\n";
        }
    }

    escreveIni.close();
}

void APara::selecaoAPsemImpre(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                              detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                              detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                              double &pGSup, double &temperatura, double &presiniG, double &tempiniG, double &vazgasG,
                              double &presE, double &tempE, double &titE, double &betaE, double &vazE, int seq, int &indCHK,
                              double *vdPdLH, double *vdPdLF, double *vdTdL) {

    indCHK = 0;
    int ind;
    if (listaV.vpsep == 1) {
        ind = sequenciaAP[seq].PSEPpres;
        pGSup = APPsep.pres[ind];
    }
    if (listaV.vRGO == 1) {
        ind = sequenciaAP[seq].RGO;
        if (flup[0].flashCompleto != 2) {
            flup[0].RGO = APRGO.RGO[ind];
            flup[0].RenovaFluido();
        } else
            atualizaCompRGO(APRGO.RGO[ind], flup[0]);
        atualizaRGO(celula, APRGO.RGO[ind]);
    }
    if (listaV.vBSW == 1) {
        ind = sequenciaAP[seq].BSW;
        flup[0].BSW = APBSW.BSW[ind];
        flup[0].RenovaFluido();
        atualizaBSW(celula, APBSW.BSW[ind]);
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            int icel = fonteg[APFonGas[iFG].indFG].posicP;
            if (APFonGas[iFG].parserieTemp > 0) {
                ind = sequenciaAP[seq].FGtemp[konta1];
                celula[icel].acsr.injg.temp = APFonGas[iFG].temp[ind];
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                ind = sequenciaAP[seq].FGvazgas[konta2];
                celula[icel].acsr.injg.QGas = APFonGas[iFG].vazgas[ind];
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                ind = sequenciaAP[seq].FGvazcomp[konta3];
                celula[icel].acsr.injg.razCompGas = APFonGas[iFG].vazcomp[ind];
                konta3++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BCSfreq[konta1];
                celula[icel].acsr.bcs.freqnova = APBCS[indBCS].freq[ind];
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                ind = sequenciaAP[seq].BCSnestag[konta2];
                celula[icel].acsr.bcs.nestag = APBCS[indBCS].nestag[ind];
                konta2++;
            }
        }
    }

    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            int icel = fontel[APFonLiq[iFL].indFL].posicP;
            if (APFonLiq[iFL].parserieTemp > 0) {
                ind = sequenciaAP[seq].FLtemp[konta1];
                celula[icel].acsr.injl.temp = APFonLiq[iFL].temp[ind];
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                ind = sequenciaAP[seq].FLvazliq[konta2];
                celula[icel].acsr.injl.QLiq = APFonLiq[iFL].vazliq[ind];
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                ind = sequenciaAP[seq].FLbet[konta3];
                celula[icel].acsr.injl.bet = APFonLiq[iFL].bet[ind];
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                ind = sequenciaAP[seq].FLindfluP[konta4];
                int indFlu = APFonLiq[iFL].indfluP[ind];
                celula[icel].acsr.injl.FluidoPro = flup[indFlu];
                konta4++;
            }
        }
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            int icel = IPRS[APIPR[iIPR].indIPR].indcel;
            if (APIPR[iIPR].parserieTemp > 0) {
                ind = sequenciaAP[seq].IPRtemp[konta1];
                celula[icel].acsr.ipr.Tres = APIPR[iIPR].temp[ind];
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                ind = sequenciaAP[seq].IPRpres[konta2];
                celula[icel].acsr.ipr.Pres = APIPR[iIPR].pres[ind];
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                ind = sequenciaAP[seq].IPRip[konta3];
                celula[icel].acsr.ipr.ip = APIPR[iIPR].ip[ind];
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                ind = sequenciaAP[seq].IPRjp[konta4];
                celula[icel].acsr.ipr.ij = APIPR[iIPR].jp[ind];
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                ind = sequenciaAP[seq].IPRqMax[konta5];
                celula[icel].acsr.ipr.qMax = APIPR[iIPR].qMax[ind];
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                ind = sequenciaAP[seq].IPRindfluP[konta6];
                int indFlu = APIPR[iIPR].indfluP[ind];
                celula[icel].acsr.ipr.FluidoPro = flup[indFlu];
                konta6++;
            }
        }
    }

    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            int icel = fontem[APFonMas[iFM].indFM].posicP;
            if (APFonMas[iFM].parserieTemp > 0) {
                ind = sequenciaAP[seq].FMtemp[konta1];
                celula[icel].acsr.injm.temp = APFonMas[iFM].temp[ind];
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                ind = sequenciaAP[seq].FMvazMasP[konta2];
                celula[icel].acsr.injm.MassP = APFonMas[iFM].vazMasP[ind];
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                ind = sequenciaAP[seq].FMvazMasC[konta3];
                celula[icel].acsr.injm.MassC = APFonMas[iFM].vazMasC[ind];
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                ind = sequenciaAP[seq].FMvazMasG[konta4];
                celula[icel].acsr.injm.MassG = APFonMas[iFM].vazMasG[ind];
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                ind = sequenciaAP[seq].FMindfluP[konta5];
                int indFlu = APFonMas[iFM].indfluP[ind];
                celula[icel].acsr.injm.FluidoPro = flup[indFlu];
                konta5++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            int icel = dpreq[APDP[indBCS].indDP].posicP;
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                celula[icel].acsr.delp = APDP[indBCS].dp[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            int icel = APdPdLH[indBCS].indDPHidro;
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                ind = sequenciaAP[seq].DPdLH[konta1];
                vdPdLH[icel] = APdPdLH[indBCS].dPHidro[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            int icel = APdPdLF[indBCS].indDPFric;
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                ind = sequenciaAP[seq].DPdLF[konta1];
                vdPdLF[icel] = APdPdLF[indBCS].dPFric[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            int icel = APdTdL[indBCS].indDT;
            if (APdTdL[indBCS].parserieDT > 0) {
                ind = sequenciaAP[seq].DTdL[konta1];
                vdTdL[icel] = APdTdL[indBCS].dT[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1 || listaV.vdpF == 1 || listaV.vdt == 1)
        atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            int icel = bvol[APBVOL[indBCS].indBV].posicP;
            if (APBVOL[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BVOLfreq[konta1];
                celula[icel].acsr.bvol.freq = APBVOL[indBCS].freq[ind];
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                ind = sequenciaAP[seq].BVOLcapacidade[konta2];
                celula[icel].acsr.bvol.capacidade = APBVOL[indBCS].capacidade[ind];
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                ind = sequenciaAP[seq].BVOLnpoli[konta3];
                celula[icel].acsr.bvol.npoli = APBVOL[indBCS].npoli[ind];
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            int icel = valv[APValv[indBCS].indV].posicP;
            if (APValv[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].VALVabertura[konta1];
                celula[icel].acsr.chk.AreaGarg = APValv[indBCS].abertura[ind] * celula[icel].acsr.chk.AreaTub;
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].VALVcd[konta2];
                celula[icel].acsr.chk.cdchk = APValv[indBCS].cd[ind];
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            int icel = furo[APFuro[indBCS].indFuro].posicP;
            if (APFuro[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].FUROabertura[konta1];
                celula[icel].acsr.fontechk.abertura = APFuro[indBCS].abertura[ind];
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].FUROcd[konta2];
                celula[icel].acsr.fontechk.cd = APFuro[indBCS].cd[ind];
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                ind = sequenciaAP[seq].FURObeta[konta3];
                celula[icel].acsr.fontechk.bet = APFuro[indBCS].beta[ind];
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                ind = sequenciaAP[seq].FUROpres[konta4];
                celula[icel].acsr.fontechk.pamb = APFuro[indBCS].pres[ind];
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                ind = sequenciaAP[seq].FUROtemp[konta5];
                celula[icel].acsr.fontechk.tempT = APFuro[indBCS].temp[ind];
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                ind = sequenciaAP[seq].FUROindFlu[konta6];
                int indFlu = APFuro[indBCS].indFlu[ind];
                celula[icel].acsr.fontechk.fluidoPamb = flup[indFlu];
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                ind = sequenciaAP[seq].GeomdA[konta1];
                int igeo = APGeom[iG].indGeom;
                konta1++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 0, APGeom[iG].dA[ind]);
            }
            if (APGeom[iG].parserieB > 0) {
                ind = sequenciaAP[seq].GeomdB[konta2];
                int igeo = APGeom[iG].indGeom;
                konta2++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 1, APGeom[iG].dB[ind]);
            }
            if (APGeom[iG].parserieRug > 0) {
                ind = sequenciaAP[seq].GeomdB[konta3];
                int igeo = APGeom[iG].indGeom;
                konta3++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 2, APGeom[iG].rug[ind]);
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                ind = sequenciaAP[seq].condEqui[konta1];
                int imat = APCondEquiv[ik].indMat;
                konta1++;
                atualizaMat(ncelG, celula, celulaG, imat, APCondEquiv[ik].cond[ind]);
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            ind = sequenciaAP[seq].INJGpresinj;
            presiniG = APGasInj.presinj[ind];
        }
        if (APGasInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].INJGtemperatura;
            tempiniG = APGasInj.temperatura[ind];
        }
        if (APGasInj.parserieVazGas > 0) {
            ind = sequenciaAP[seq].INJGvazgas;
            vazgasG = APGasInj.vazgas[ind];
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].PEpres;
            presE = APpEntrada.pres[ind];
        }
        if (APpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].PEbet;
            betaE = APpEntrada.bet[ind];
        }
        if (APpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].PEtemperatura;
            tempE = APpEntrada.temperatura[ind];
        }
        if (APpEntrada.parserieTit > 0) {
            ind = sequenciaAP[seq].PEtit;
            titE = APpEntrada.tit[ind];
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].VPEpres;
            presE = APvpEntrada.pres[ind];
        }
        if (APvpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].VPEbet;
            betaE = APvpEntrada.bet[ind];
        }
        if (APvpEntrada.parserieMass > 0) {
            ind = sequenciaAP[seq].VPEmass;
            vazE = APvpEntrada.mass[ind];
        }
        if (APvpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].VPEtemperatura;
            tempE = APvpEntrada.temperatura[ind];
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            ind = sequenciaAP[seq].CHKabertura;
            indCHK = ind;
            chokeSup.AreaGarg = APCHK.abertura[ind] * chokeSup.AreaTub;
        }
        if (APCHK.parserieCD > 0) {
            ind = sequenciaAP[seq].CHKcd;
            chokeSup.cdchk = APCHK.cd[ind];
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            ind = sequenciaAP[seq].PIpresinj;
            presE = APPInj.presfundo[ind];
        }
        if (APPInj.parseriePresFundo > 0) {
            ind = sequenciaAP[seq].PIpresfundo;
            pGSup = APPInj.presinj[ind];
        }
        if (APPInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].PItempinj;
            tempE = APPInj.tempinj[ind];
        }
        if (APPInj.parserieVaz > 0) {
            ind = sequenciaAP[seq].PIvazinj;
            vazE = APPInj.vazinj[ind];
        }
    }
}

void APara::imprimeVarInteresseAPImex(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                                      detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                                      detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq, int seq) {

    ostringstream saidaP;
    saidaP << "variaveisInteresseAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::app);

    int ind;
    if (listaV.vRGO == 1) {
        ind = sequenciaAP[seq].RGO;
        escreveIni << "   " << APRGO.RGO[ind] << " ; ";
    }
    if (listaV.vBSW == 1) {
        ind = sequenciaAP[seq].BSW;
        flup[0].BSW = APBSW.BSW[ind];
        flup[0].RenovaFluido();
        atualizaBSW(celula, APBSW.BSW[ind]);
        escreveIni << "   " << APBSW.BSW[ind] << " ; ";
    }
    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieTemp > 0) {
                ind = sequenciaAP[seq].FLtemp[konta1];
                escreveIni << "  " << APFonLiq[iFL].temp[ind] << " ; ";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                ind = sequenciaAP[seq].FLvazliq[konta2];
                escreveIni << "   " << APFonLiq[iFL].vazliq[ind] << " ; ";
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                ind = sequenciaAP[seq].FLbet[konta3];
                escreveIni << "   " << APFonLiq[iFL].bet[ind] << " ; ";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                ind = sequenciaAP[seq].FLindfluP[konta4];
                int indFlu = APFonLiq[iFL].indfluP[ind];
                escreveIni << "   " << indFlu << " ; ";
                konta4++;
            }
        }
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieTemp > 0) {
                ind = sequenciaAP[seq].FGtemp[konta1];
                escreveIni << "   " << APFonGas[iFG].temp[ind] << " ; ";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                ind = sequenciaAP[seq].FGvazgas[konta2];
                escreveIni << "   " << APFonGas[iFG].vazgas[ind] << " ; ";
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                ind = sequenciaAP[seq].FGvazcomp[konta3];
                escreveIni << "   " << APFonGas[iFG].vazcomp[ind] << " ; ";
                konta3++;
            }
        }
    }
    if (listaV.vpsep == 1) {
        ind = sequenciaAP[seq].PSEPpres;
        escreveIni << "   " << APPsep.pres[ind] << " ; ";
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieTemp > 0) {
                ind = sequenciaAP[seq].IPRtemp[konta1];
                escreveIni << "   " << APIPR[iIPR].temp[ind] << " ; ";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                ind = sequenciaAP[seq].IPRpres[konta2];
                escreveIni << "   " << APIPR[iIPR].pres[ind] << " ; ";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                ind = sequenciaAP[seq].IPRip[konta3];
                escreveIni << seq << "   " << APIPR[iIPR].ip[ind] << " ; ";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                ind = sequenciaAP[seq].IPRjp[konta4];
                escreveIni << "   " << APIPR[iIPR].jp[ind] << " ; ";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                ind = sequenciaAP[seq].IPRqMax[konta5];
                escreveIni << "   " << APIPR[iIPR].qMax[ind] << " ; ";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                ind = sequenciaAP[seq].IPRindfluP[konta6];
                escreveIni << "   " << APIPR[iIPR].indfluP[ind] << " ; ";
                konta6++;
            }
        }
    }
    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieTemp > 0) {
                ind = sequenciaAP[seq].FMtemp[konta1];
                escreveIni << "   " << APFonMas[iFM].temp[ind] << " ; ";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                ind = sequenciaAP[seq].FMvazMasP[konta2];
                escreveIni << "   " << APFonMas[iFM].vazMasP[ind] << " ; ";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                ind = sequenciaAP[seq].FMvazMasC[konta3];
                escreveIni << "   " << APFonMas[iFM].vazMasC[ind] << " ; ";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                ind = sequenciaAP[seq].FMvazMasG[konta4];
                escreveIni << "   " << APFonMas[iFM].vazMasG[ind] << " ; ";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                ind = sequenciaAP[seq].FMindfluP[konta5];
                escreveIni << "   " << APFonMas[iFM].indfluP[ind] << " ; ";
                konta5++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BCSfreq[konta1];
                escreveIni << "   " << APBCS[indBCS].freq[ind] << " ; ";
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                ind = sequenciaAP[seq].BCSnestag[konta2];
                celula[icel].acsr.bcs.nestag = APBCS[indBCS].nestag[ind];
                escreveIni << seq << " : " << " indice BCS = " << konta2 << " celula Fonte " << icel << " indice N. Estagios= " << ind << " N. Estagios= " << APBCS[indBCS].nestag[ind] << "\n";
                konta2++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                escreveIni << "   " << APDP[indBCS].dp[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                escreveIni << "   " << APDP[indBCS].dp[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                ind = sequenciaAP[seq].DPdLF[konta1];
                escreveIni << "   " << APdPdLF[indBCS].dPFric[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            if (APdTdL[indBCS].parserieDT > 0) {
                ind = sequenciaAP[seq].DTdL[konta1];
                escreveIni << "   " << APdTdL[indBCS].dT[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            if (APBVOL[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BVOLfreq[konta1];
                escreveIni << "   " << APBVOL[indBCS].freq[ind] << " ; ";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                ind = sequenciaAP[seq].BVOLcapacidade[konta2];
                escreveIni << "   " << APBVOL[indBCS].capacidade[ind] << " ; ";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                ind = sequenciaAP[seq].BVOLnpoli[konta3];
                escreveIni << "   " << APBVOL[indBCS].npoli[ind] << " ; ";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            if (APValv[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].VALVabertura[konta1];
                escreveIni << "   " << APValv[indBCS].abertura[ind] << " ; ";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].VALVcd[konta2];
                escreveIni << " : " << APValv[indBCS].cd[ind] << " ; ";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].FUROabertura[konta1];
                escreveIni << "   " << APFuro[indBCS].abertura[ind] << " ; ";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].FUROcd[konta2];
                escreveIni << "   " << APFuro[indBCS].cd[ind] << " ; ";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                ind = sequenciaAP[seq].FURObeta[konta3];
                escreveIni << "   " << APFuro[indBCS].beta[ind] << " ; ";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                ind = sequenciaAP[seq].FUROpres[konta4];
                escreveIni << "   " << APFuro[indBCS].pres[ind] << " ; ";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                ind = sequenciaAP[seq].FUROtemp[konta5];
                escreveIni << "   " << " Temperatura= " << APFuro[indBCS].temp[ind] << " ; ";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                ind = sequenciaAP[seq].FUROindFlu[konta6];
                int indFlu = APFuro[indBCS].indFlu[ind];
                escreveIni << "   " << indFlu << " ; ";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                ind = sequenciaAP[seq].GeomdA[konta1];
                escreveIni << "   " << APGeom[iG].dA[ind] << " ; ";
                konta1++;
            }
            if (APGeom[iG].parserieB > 0) {
                ind = sequenciaAP[seq].GeomdB[konta2];
                escreveIni << "   " << APGeom[iG].dB[ind] << " ; ";
                konta2++;
            }
            if (APGeom[iG].parserieRug > 0) {
                ind = sequenciaAP[seq].GeomdB[konta3];
                escreveIni << "   " << APGeom[iG].rug[ind] << " ; ";
                konta3++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                ind = sequenciaAP[seq].condEqui[konta1];
                escreveIni << "   " << APCondEquiv[ik].cond[ind] << " ; ";
                konta1++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            ind = sequenciaAP[seq].INJGpresinj;
            escreveIni << "   " << APGasInj.presinj[ind] << " ; ";
        }
        if (APGasInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].INJGtemperatura;
            escreveIni << "   " << APGasInj.temperatura[ind] << " ; ";
        }
        if (APGasInj.parserieVazGas > 0) {
            ind = sequenciaAP[seq].INJGvazgas;
            escreveIni << "   " << APGasInj.vazgas[ind] << " ; ";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].PEpres;
            escreveIni << "   " << APpEntrada.pres[ind] << " ; ";
        }
        if (APpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].PEbet;
            escreveIni << "   " << APpEntrada.bet[ind] << " ; ";
        }
        if (APpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].PEtemperatura;
            escreveIni << "   " << APpEntrada.temperatura[ind] << " ; ";
        }
        if (APpEntrada.parserieTit > 0) {
            ind = sequenciaAP[seq].PEtit;
            escreveIni << "   " << APpEntrada.tit[ind] << " ; ";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].VPEpres;
            escreveIni << "   " << APvpEntrada.pres[ind] << " ; ";
        }
        if (APvpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].VPEbet;
            escreveIni << "   " << APvpEntrada.bet[ind] << " ; ";
        }
        if (APvpEntrada.parserieMass > 0) {
            ind = sequenciaAP[seq].VPEmass;
            escreveIni << "   " << APvpEntrada.mass[ind] << " ; ";
        }
        if (APvpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].VPEtemperatura;
            escreveIni << "   " << APvpEntrada.temperatura[ind] << " ; ";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            ind = sequenciaAP[seq].CHKabertura;
            escreveIni << "   " << APCHK.abertura[ind] << " ; ";
        }
        if (APCHK.parserieCD > 0) {
            ind = sequenciaAP[seq].CHKcd;
            escreveIni << "   " << chokeSup.cdchk << " ; ";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            ind = sequenciaAP[seq].PIpresinj;
            escreveIni << " : " << APPInj.presfundo[ind] << " ; ";
        }
        if (APPInj.parseriePresFundo > 0) {
            ind = sequenciaAP[seq].PIpresfundo;
            escreveIni << " : " << APPInj.presinj[ind] << " ; ";
        }
        if (APPInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].PItempinj;
            escreveIni << " : " << APPInj.tempinj[ind] << " ; ";
        }
        if (APPInj.parserieVaz > 0) {
            ind = sequenciaAP[seq].PIvazinj;
            escreveIni << " : " << APPInj.vazinj[ind] << " ; ";
        }
    }
    escreveIni << endl;
    escreveIni.close();
}

void APara::selecaoAPImex(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                          detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                          detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                          double &pGSup, double &temperatura, double &presiniG, double &tempiniG, double &vazgasG,
                          double &presE, double &tempE, double &titE, double &betaE, double &vazE, int seq, int &indCHK,
                          double *vdPdLH, double *vdPdLF, double *vdTdL, int imprime) {

    ostringstream saidaP;
    saidaP << "relacaoAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::app);

    indCHK = 0;
    int ind;
    if (listaV.vRGO == 1) {
        ind = sequenciaAP[seq].RGO;
        if (flup[0].flashCompleto != 2) {
            flup[0].RGO = APRGO.RGO[ind];
            flup[0].RenovaFluido();
        } else
            atualizaCompRGO(APRGO.RGO[ind], flup[0]);
        atualizaRGO(celula, APRGO.RGO[ind]);
        if (imprime == 1)
            escreveIni << seq << " : " << " indice RGO = " << ind << " RGO fluido 0= " << flup[0].RGO << "\n";
    }
    if (listaV.vBSW == 1) {
        ind = sequenciaAP[seq].BSW;
        flup[0].BSW = APBSW.BSW[ind];
        flup[0].RenovaFluido();
        atualizaBSW(celula, APBSW.BSW[ind]);
        if (imprime == 1)
            escreveIni << seq << " : " << " indice BSW = " << ind << " BSW fluido 0= " << flup[0].BSW << "\n";
    }
    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            int icel = fontel[APFonLiq[iFL].indFL].posicP;
            if (APFonLiq[iFL].parserieTemp > 0) {
                ind = sequenciaAP[seq].FLtemp[konta1];
                celula[icel].acsr.injl.temp = APFonLiq[iFL].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APFonLiq[iFL].temp[ind] << "\n";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                ind = sequenciaAP[seq].FLvazliq[konta2];
                celula[icel].acsr.injl.QLiq = APFonLiq[iFL].vazliq[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta2 << " celula Fonte " << icel << " indice Vazao Liq = " << ind << " Vazao de Liquido= " << APFonLiq[iFL].vazliq[ind] << "\n";
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                ind = sequenciaAP[seq].FLbet[konta3];
                celula[icel].acsr.injl.bet = APFonLiq[iFL].bet[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta3 << " celula Fonte " << icel << " indice Beta = " << ind << " Beta= " << APFonLiq[iFL].bet[ind] << "\n";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                ind = sequenciaAP[seq].FLindfluP[konta4];
                int indFlu = APFonLiq[iFL].indfluP[ind];
                celula[icel].acsr.injl.FluidoPro = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Liquido = " << konta4 << " celula Fonte " << icel << " indice Fluido = " << ind << " Fluido= " << indFlu << "\n";
                konta4++;
            }
        }
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            int icel = fonteg[APFonGas[iFG].indFG].posicP;
            if (APFonGas[iFG].parserieTemp > 0) {
                ind = sequenciaAP[seq].FGtemp[konta1];
                celula[icel].acsr.injg.temp = APFonGas[iFG].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Gas = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APFonGas[iFG].temp[ind] << "\n";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                ind = sequenciaAP[seq].FGvazgas[konta2];
                celula[icel].acsr.injg.QGas = APFonGas[iFG].vazgas[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Gas = " << konta2 << " celula Fonte " << icel << " indice Vazao Gas = " << ind << " Vazao Gas= " << APFonGas[iFG].vazgas[ind] << "\n";
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                ind = sequenciaAP[seq].FGvazcomp[konta3];
                celula[icel].acsr.injg.razCompGas = APFonGas[iFG].vazcomp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Gas = " << konta3 << " celula Fonte " << icel << " indice Vazao Liq Complementar = " << ind << " Vazao Liq Complementar= " << APFonGas[iFG].vazcomp[ind] << "\n";
                konta3++;
            }
        }
    }
    if (listaV.vpsep == 1) {
        ind = sequenciaAP[seq].PSEPpres;
        pGSup = APPsep.pres[ind];
        if (imprime == 1)
            escreveIni << seq << " : " << " indice PSep = " << ind << " Pressao Separador= " << pGSup << "\n";
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            int icel = IPRS[APIPR[iIPR].indIPR].indcel;
            if (APIPR[iIPR].parserieTemp > 0) {
                ind = sequenciaAP[seq].IPRtemp[konta1];
                celula[icel].acsr.ipr.Tres = APIPR[iIPR].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APIPR[iIPR].temp[ind] << "\n";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                ind = sequenciaAP[seq].IPRpres[konta2];
                celula[icel].acsr.ipr.Pres = APIPR[iIPR].pres[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta2 << " celula Fonte " << icel << " indice P Estatica = " << ind << " P Estatica= " << APIPR[iIPR].pres[ind] << "\n";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                ind = sequenciaAP[seq].IPRip[konta3];
                celula[icel].acsr.ipr.ip = APIPR[iIPR].ip[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta3 << " celula Fonte " << icel << " indice IP = " << ind << " IP= " << APIPR[iIPR].ip[ind] << "\n";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                ind = sequenciaAP[seq].IPRjp[konta4];
                celula[icel].acsr.ipr.ij = APIPR[iIPR].jp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta4 << " celula Fonte " << icel << " indice II = " << ind << " II= " << APIPR[iIPR].jp[ind] << "\n";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                ind = sequenciaAP[seq].IPRqMax[konta5];
                celula[icel].acsr.ipr.qMax = APIPR[iIPR].qMax[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta5 << " celula Fonte " << icel << " indice qMax = " << ind << " qMax= " << APIPR[iIPR].qMax[ind] << "\n";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                ind = sequenciaAP[seq].IPRindfluP[konta6];
                int indFlu = APIPR[iIPR].indfluP[ind];
                celula[icel].acsr.ipr.FluidoPro = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice IPR = " << konta6 << " celula Fonte " << icel << " indice Fluido = " << ind << " fluido= " << APIPR[iIPR].indfluP[ind] << "\n";
                konta6++;
            }
        }
    }
    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            int icel = fontem[APFonMas[iFM].indFM].posicP;
            if (APFonMas[iFM].parserieTemp > 0) {
                ind = sequenciaAP[seq].FMtemp[konta1];
                celula[icel].acsr.injm.temp = APFonMas[iFM].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta1 << " celula Fonte " << icel << " indice Temperatura = " << ind << " Temperatura= " << APFonMas[iFM].temp[ind] << "\n";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                ind = sequenciaAP[seq].FMvazMasP[konta2];
                celula[icel].acsr.injm.MassP = APFonMas[iFM].vazMasP[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta2 << " celula Fonte " << icel << " indice Vazao Liquido= " << ind << " Vazao Liquido= " << APFonMas[iFM].vazMasP[ind] << "\n";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                ind = sequenciaAP[seq].FMvazMasC[konta3];
                celula[icel].acsr.injm.MassC = APFonMas[iFM].vazMasC[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta3 << " celula Fonte " << icel << " indice Vazao Liquido Complementar= " << ind << " Vazao Liquido Complementar= " << APFonMas[iFM].vazMasC[ind] << "\n";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                ind = sequenciaAP[seq].FMvazMasG[konta4];
                celula[icel].acsr.injm.MassG = APFonMas[iFM].vazMasG[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta4 << " celula Fonte " << icel << " indice Vazao Gas= " << ind << " Vazao Gas= " << APFonMas[iFM].vazMasG[ind] << "\n";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                ind = sequenciaAP[seq].FMindfluP[konta5];
                int indFlu = APFonMas[iFM].indfluP[ind];
                celula[icel].acsr.injm.FluidoPro = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Fonte Massa = " << konta5 << " celula Fonte " << icel << " indice Fluido= " << ind << " Fluido= " << APFonMas[iFM].indfluP[ind] << "\n";
                konta5++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BCSfreq[konta1];
                celula[icel].acsr.bcs.freqnova = APBCS[indBCS].freq[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice BCS = " << konta1 << " celula Fonte " << icel << " indice Frequencia= " << ind << " Frequencia= " << APBCS[indBCS].freq[ind] << "\n";
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                ind = sequenciaAP[seq].BCSnestag[konta2];
                celula[icel].acsr.bcs.nestag = APBCS[indBCS].nestag[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice BCS = " << konta2 << " celula Fonte " << icel << " indice N. Estagios= " << ind << " N. Estagios= " << APBCS[indBCS].nestag[ind] << "\n";
                konta2++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            int icel = dpreq[APDP[indBCS].indDP].posicP;
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                celula[icel].acsr.delp = APDP[indBCS].dp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Del. Pres, = " << konta1 << " celula Fonte " << icel << " indice Del. Pres.= " << ind << " Del. Pres.= " << APDP[indBCS].dp[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            int icel = APdPdLH[indBCS].indDPHidro;
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                ind = sequenciaAP[seq].DPdLH[konta1];
                vdPdLH[icel] = APdPdLH[indBCS].dPHidro[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Corr. Pres. Hidro, = " << konta1 << " indice da colecao de fatores de correcao " << icel << " indice na AP.= " << ind << " Corr. Pres. Hidro= " << APdPdLH[indBCS].dPHidro[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            int icel = APdPdLF[indBCS].indDPFric;
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                ind = sequenciaAP[seq].DPdLF[konta1];
                vdPdLF[icel] = APdPdLF[indBCS].dPFric[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Corr. Pres. Fric., = " << konta1 << " indice da colecao de fatores de correcao " << icel << " indice na AP.= " << ind << " Corr. Pres. Fric.= " << APdPdLF[indBCS].dPFric[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            int icel = APdTdL[indBCS].indDT;
            if (APdTdL[indBCS].parserieDT > 0) {
                ind = sequenciaAP[seq].DTdL[konta1];
                vdTdL[icel] = APdTdL[indBCS].dT[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Corr. Temperatura, = " << konta1 << " indice da colecao de fatores de correcao " << icel << " indice na AP.= " << ind << " Corr. Temperatura= " << APdTdL[indBCS].dT[ind] << "\n";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1 || listaV.vdpF == 1 || listaV.vdt == 1)
        atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            int icel = bvol[APBVOL[indBCS].indBV].posicP;
            if (APBVOL[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BVOLfreq[konta1];
                celula[icel].acsr.bvol.freq = APBVOL[indBCS].freq[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Bomba Vol., = " << konta1 << " celula Fonte " << icel << " indice Frequencia= " << ind << " Frequencia= " << APBVOL[indBCS].freq[ind] << "\n";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                ind = sequenciaAP[seq].BVOLcapacidade[konta2];
                celula[icel].acsr.bvol.capacidade = APBVOL[indBCS].capacidade[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Bomba Vol., = " << konta2 << " celula Fonte " << icel << " indice Capacidade= " << ind << " Capacidade= " << APBVOL[indBCS].capacidade[ind] << "\n";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                ind = sequenciaAP[seq].BVOLnpoli[konta3];
                celula[icel].acsr.bvol.npoli = APBVOL[indBCS].npoli[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Bomba Vol., = " << konta3 << " celula Fonte " << icel << " indice Fator Politropico= " << ind << " Fator Politropico= " << APBVOL[indBCS].npoli[ind] << "\n";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            int icel = valv[APValv[indBCS].indV].posicP;
            if (APValv[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].VALVabertura[konta1];
                celula[icel].acsr.chk.AreaGarg = APValv[indBCS].abertura[ind] * celula[icel].acsr.chk.AreaTub;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Valvula, = " << konta1 << " celula Fonte " << icel << " indice Abertura= " << ind << " Abertura= " << APValv[indBCS].abertura[ind] << "\n";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].VALVcd[konta2];
                celula[icel].acsr.chk.cdchk = APValv[indBCS].cd[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Valvula, = " << konta2 << " celula Fonte " << icel << " indice CD= " << ind << " CD= " << APValv[indBCS].cd[ind] << "\n";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            int icel = furo[APFuro[indBCS].indFuro].posicP;
            if (APFuro[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].FUROabertura[konta1];
                celula[icel].acsr.fontechk.abertura = APFuro[indBCS].abertura[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta1 << " celula Fonte " << icel << " indice Abertura= " << ind << " Abertura= " << APFuro[indBCS].abertura[ind] << "\n";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].FUROcd[konta2];
                celula[icel].acsr.fontechk.cd = APFuro[indBCS].cd[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta2 << " celula Fonte " << icel << " indice CD= " << ind << " CD= " << APFuro[indBCS].cd[ind] << "\n";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                ind = sequenciaAP[seq].FURObeta[konta3];
                celula[icel].acsr.fontechk.bet = APFuro[indBCS].beta[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta3 << " celula Fonte " << icel << " indice Beta= " << ind << " Beta= " << APFuro[indBCS].beta[ind] << "\n";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                ind = sequenciaAP[seq].FUROpres[konta4];
                celula[icel].acsr.fontechk.pamb = APFuro[indBCS].pres[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta4 << " celula Fonte " << icel << " indice Pressao= " << ind << " Pressao= " << APFuro[indBCS].pres[ind] << "\n";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                ind = sequenciaAP[seq].FUROtemp[konta5];
                celula[icel].acsr.fontechk.tempT = APFuro[indBCS].temp[ind];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta5 << " celula Fonte " << icel << " indice Temperatura= " << ind << " Temperatura= " << APFuro[indBCS].temp[ind] << "\n";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                ind = sequenciaAP[seq].FUROindFlu[konta6];
                int indFlu = APFuro[indBCS].indFlu[ind];
                celula[icel].acsr.fontechk.fluidoPamb = flup[indFlu];
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Vazamento = " << konta6 << " celula Fonte " << icel << " indice Fluido= " << ind << " Fluido= " << indFlu << "\n";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                ind = sequenciaAP[seq].GeomdA[konta1];
                int igeo = APGeom[iG].indGeom;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Secao Transversal = " << konta1 << " secao transversal " << igeo << " indice Maior Diametro = " << ind << " Maior Diam Interno= " << APGeom[iG].dA[ind] << "\n";
                konta1++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 0, APGeom[iG].dA[ind]);
            }
            if (APGeom[iG].parserieB > 0) {
                ind = sequenciaAP[seq].GeomdB[konta2];
                int igeo = APGeom[iG].indGeom;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Secao Transversal = " << konta2 << " secao transversal " << igeo << " indice Menor Diametro = " << ind << " Menor Diam Interno= " << APGeom[iG].dB[ind] << "\n";
                konta2++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 1, APGeom[iG].dB[ind]);
            }
            if (APGeom[iG].parserieRug > 0) {
                ind = sequenciaAP[seq].GeomdB[konta3];
                int igeo = APGeom[iG].indGeom;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Secao Transversal = " << konta3 << " secao transversal " << igeo << " indice Rugosidade = " << ind << " Rugosidade= " << APGeom[iG].rug[ind] << "\n";
                konta3++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 2, APGeom[iG].rug[ind]);
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                ind = sequenciaAP[seq].condEqui[konta1];
                int imat = APCondEquiv[ik].indMat;
                if (imprime == 1)
                    escreveIni << seq << " : " << " indice Material = " << konta1 << " material " << imat << " indice Condutividade = " << ind << " Condutividade= " << APCondEquiv[ik].cond[ind] << "\n";
                konta1++;
                atualizaMat(ncelG, celula, celulaG, imat, APCondEquiv[ik].cond[ind]);
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            ind = sequenciaAP[seq].INJGpresinj;
            presiniG = APGasInj.presinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pressao Injecao = " << ind << " Pressao Injecao= " << APGasInj.presinj[ind] << "\n";
        }
        if (APGasInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].INJGtemperatura;
            tempiniG = APGasInj.temperatura[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp Injecao = " << ind << " Temp Injecao= " << APGasInj.temperatura[ind] << "\n";
        }
        if (APGasInj.parserieVazGas > 0) {
            ind = sequenciaAP[seq].INJGvazgas;
            vazgasG = APGasInj.vazgas[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Vaz Injecao = " << ind << " Vaz Injecao= " << APGasInj.vazgas[ind] << "\n";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].PEpres;
            presE = APpEntrada.pres[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Contorno = " << ind << " Pres. Contorno= " << presE << "\n";
        }
        if (APpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].PEbet;
            betaE = APpEntrada.bet[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Beta Contorno = " << ind << " Beta Contorno= " << betaE << "\n";
        }
        if (APpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].PEtemperatura;
            tempE = APpEntrada.temperatura[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp. Contorno = " << ind << " Temp. Contorno= " << tempE << "\n";
        }
        if (APpEntrada.parserieTit > 0) {
            ind = sequenciaAP[seq].PEtit;
            titE = APpEntrada.tit[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Titulo Contorno = " << ind << " Titulo Contorno= " << titE << "\n";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].VPEpres;
            presE = APvpEntrada.pres[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Contorno = " << ind << " Pres. Contorno= " << presE << "\n";
        }
        if (APvpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].VPEbet;
            betaE = APvpEntrada.bet[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Beta Contorno = " << ind << " Beta Contorno= " << betaE << "\n";
        }
        if (APvpEntrada.parserieMass > 0) {
            ind = sequenciaAP[seq].VPEmass;
            vazE = APvpEntrada.mass[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Vazao Contorno = " << ind << " Vazao Contorno= " << vazE << "\n";
        }
        if (APvpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].VPEtemperatura;
            tempE = APvpEntrada.temperatura[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp. Contorno = " << ind << " Temp. Contorno= " << tempE << "\n";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            ind = sequenciaAP[seq].CHKabertura;
            indCHK = ind;
            chokeSup.AreaGarg = APCHK.abertura[ind] * chokeSup.AreaTub;
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Abertura Choke = " << ind << " Abertura Choke= " << APCHK.abertura[ind] << "\n";
        }
        if (APCHK.parserieCD > 0) {
            ind = sequenciaAP[seq].CHKcd;
            chokeSup.cdchk = APCHK.cd[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice CD Choke = " << ind << " CD Choke= " << chokeSup.cdchk << "\n";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            ind = sequenciaAP[seq].PIpresinj;
            presE = APPInj.presfundo[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Jusante = " << ind << " Pres Jusante= " << presE << "\n";
        }
        if (APPInj.parseriePresFundo > 0) {
            ind = sequenciaAP[seq].PIpresfundo;
            pGSup = APPInj.presinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Pres. Montante = " << ind << " Pres Montante= " << pGSup << "\n";
        }
        if (APPInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].PItempinj;
            tempE = APPInj.tempinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Temp. Injecao = " << ind << " Temp. Injecao= " << tempE << "\n";
        }
        if (APPInj.parserieVaz > 0) {
            ind = sequenciaAP[seq].PIvazinj;
            vazE = APPInj.vazinj[ind];
            if (imprime == 1)
                escreveIni << seq << " : " << " indice Vazao Poco Injecao = " << ind << " Vazao Poco Injecao= " << vazE << "\n";
        }
    }

    escreveIni.close();
}

void APara::selecaoAPImexsemImpre(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                                  detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                                  detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                                  double &pGSup, double &temperatura, double &presiniG, double &tempiniG, double &vazgasG,
                                  double &presE, double &tempE, double &titE, double &betaE, double &vazE, int seq, int &indCHK,
                                  double *vdPdLH, double *vdPdLF, double *vdTdL) {

    indCHK = 0;
    int ind;
    if (listaV.vRGO == 1) {
        ind = sequenciaAP[seq].RGO;
        if (flup[0].flashCompleto != 2) {
            flup[0].RGO = APRGO.RGO[ind];
            flup[0].RenovaFluido();
        } else
            atualizaCompRGO(APRGO.RGO[ind], flup[0]);
        atualizaRGO(celula, APRGO.RGO[ind]);
    }
    if (listaV.vBSW == 1) {
        ind = sequenciaAP[seq].BSW;
        flup[0].BSW = APBSW.BSW[ind];
        flup[0].RenovaFluido();
        atualizaBSW(celula, APBSW.BSW[ind]);
    }
    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            int icel = fontel[APFonLiq[iFL].indFL].posicP;
            if (APFonLiq[iFL].parserieTemp > 0) {
                ind = sequenciaAP[seq].FLtemp[konta1];
                celula[icel].acsr.injl.temp = APFonLiq[iFL].temp[ind];
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                ind = sequenciaAP[seq].FLvazliq[konta2];
                celula[icel].acsr.injl.QLiq = APFonLiq[iFL].vazliq[ind];
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                ind = sequenciaAP[seq].FLbet[konta3];
                celula[icel].acsr.injl.bet = APFonLiq[iFL].bet[ind];
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                ind = sequenciaAP[seq].FLindfluP[konta4];
                int indFlu = APFonLiq[iFL].indfluP[ind];
                celula[icel].acsr.injl.FluidoPro = flup[indFlu];
                konta4++;
            }
        }
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            int icel = fonteg[APFonGas[iFG].indFG].posicP;
            if (APFonGas[iFG].parserieTemp > 0) {
                ind = sequenciaAP[seq].FGtemp[konta1];
                celula[icel].acsr.injg.temp = APFonGas[iFG].temp[ind];
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                ind = sequenciaAP[seq].FGvazgas[konta2];
                celula[icel].acsr.injg.QGas = APFonGas[iFG].vazgas[ind];
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                ind = sequenciaAP[seq].FGvazcomp[konta3];
                celula[icel].acsr.injg.razCompGas = APFonGas[iFG].vazcomp[ind];
                konta3++;
            }
        }
    }
    if (listaV.vpsep == 1) {
        ind = sequenciaAP[seq].PSEPpres;
        pGSup = APPsep.pres[ind];
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            int icel = IPRS[APIPR[iIPR].indIPR].indcel;
            if (APIPR[iIPR].parserieTemp > 0) {
                ind = sequenciaAP[seq].IPRtemp[konta1];
                celula[icel].acsr.ipr.Tres = APIPR[iIPR].temp[ind];
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                ind = sequenciaAP[seq].IPRpres[konta2];
                celula[icel].acsr.ipr.Pres = APIPR[iIPR].pres[ind];
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                ind = sequenciaAP[seq].IPRip[konta3];
                celula[icel].acsr.ipr.ip = APIPR[iIPR].ip[ind];
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                ind = sequenciaAP[seq].IPRjp[konta4];
                celula[icel].acsr.ipr.ij = APIPR[iIPR].jp[ind];
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                ind = sequenciaAP[seq].IPRqMax[konta5];
                celula[icel].acsr.ipr.qMax = APIPR[iIPR].qMax[ind];
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                ind = sequenciaAP[seq].IPRindfluP[konta6];
                int indFlu = APIPR[iIPR].indfluP[ind];
                celula[icel].acsr.ipr.FluidoPro = flup[indFlu];
                konta6++;
            }
        }
    }
    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            int icel = fontem[APFonMas[iFM].indFM].posicP;
            if (APFonMas[iFM].parserieTemp > 0) {
                ind = sequenciaAP[seq].FMtemp[konta1];
                celula[icel].acsr.injm.temp = APFonMas[iFM].temp[ind];
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                ind = sequenciaAP[seq].FMvazMasP[konta2];
                celula[icel].acsr.injm.MassP = APFonMas[iFM].vazMasP[ind];
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                ind = sequenciaAP[seq].FMvazMasC[konta3];
                celula[icel].acsr.injm.MassC = APFonMas[iFM].vazMasC[ind];
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                ind = sequenciaAP[seq].FMvazMasG[konta4];
                celula[icel].acsr.injm.MassG = APFonMas[iFM].vazMasG[ind];
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                ind = sequenciaAP[seq].FMindfluP[konta5];
                int indFlu = APFonMas[iFM].indfluP[ind];
                celula[icel].acsr.injm.FluidoPro = flup[indFlu];
                konta5++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BCSfreq[konta1];
                celula[icel].acsr.bcs.freqnova = APBCS[indBCS].freq[ind];
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                ind = sequenciaAP[seq].BCSnestag[konta2];
                celula[icel].acsr.bcs.nestag = APBCS[indBCS].nestag[ind];
                konta2++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            int icel = dpreq[APDP[indBCS].indDP].posicP;
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                celula[icel].acsr.delp = APDP[indBCS].dp[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            int icel = APdPdLH[indBCS].indDPHidro;
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                ind = sequenciaAP[seq].DPdLH[konta1];
                vdPdLH[icel] = APdPdLH[indBCS].dPHidro[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            int icel = APdPdLF[indBCS].indDPFric;
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                ind = sequenciaAP[seq].DPdLF[konta1];
                vdPdLF[icel] = APdPdLF[indBCS].dPFric[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            int icel = APdTdL[indBCS].indDT;
            if (APdTdL[indBCS].parserieDT > 0) {
                ind = sequenciaAP[seq].DTdL[konta1];
                vdTdL[icel] = APdTdL[indBCS].dT[ind];
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1 || listaV.vdpF == 1 || listaV.vdt == 1)
        atualizaCorrecao(ncelG, celula, celulaG, vdPdLH, vdPdLF, vdTdL);
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            int icel = bvol[APBVOL[indBCS].indBV].posicP;
            if (APBVOL[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BVOLfreq[konta1];
                celula[icel].acsr.bvol.freq = APBVOL[indBCS].freq[ind];
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                ind = sequenciaAP[seq].BVOLcapacidade[konta2];
                celula[icel].acsr.bvol.capacidade = APBVOL[indBCS].capacidade[ind];
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                ind = sequenciaAP[seq].BVOLnpoli[konta3];
                celula[icel].acsr.bvol.npoli = APBVOL[indBCS].npoli[ind];
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            int icel = valv[APValv[indBCS].indV].posicP;
            if (APValv[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].VALVabertura[konta1];
                celula[icel].acsr.chk.AreaGarg = APValv[indBCS].abertura[ind] * celula[icel].acsr.chk.AreaTub;
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].VALVcd[konta2];
                celula[icel].acsr.chk.cdchk = APValv[indBCS].cd[ind];
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            int icel = furo[APFuro[indBCS].indFuro].posicP;
            if (APFuro[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].FUROabertura[konta1];
                celula[icel].acsr.fontechk.abertura = APFuro[indBCS].abertura[ind];
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].FUROcd[konta2];
                celula[icel].acsr.fontechk.cd = APFuro[indBCS].cd[ind];
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                ind = sequenciaAP[seq].FURObeta[konta3];
                celula[icel].acsr.fontechk.bet = APFuro[indBCS].beta[ind];
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                ind = sequenciaAP[seq].FUROpres[konta4];
                celula[icel].acsr.fontechk.pamb = APFuro[indBCS].pres[ind];
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                ind = sequenciaAP[seq].FUROtemp[konta5];
                celula[icel].acsr.fontechk.tempT = APFuro[indBCS].temp[ind];
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                ind = sequenciaAP[seq].FUROindFlu[konta6];
                int indFlu = APFuro[indBCS].indFlu[ind];
                celula[icel].acsr.fontechk.fluidoPamb = flup[indFlu];
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                ind = sequenciaAP[seq].GeomdA[konta1];
                int igeo = APGeom[iG].indGeom;
                konta1++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 0, APGeom[iG].dA[ind]);
            }
            if (APGeom[iG].parserieB > 0) {
                ind = sequenciaAP[seq].GeomdB[konta2];
                int igeo = APGeom[iG].indGeom;
                konta2++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 1, APGeom[iG].dB[ind]);
            }
            if (APGeom[iG].parserieRug > 0) {
                ind = sequenciaAP[seq].GeomdB[konta3];
                int igeo = APGeom[iG].indGeom;
                konta3++;
                atualizaGeom(ncelG, celula, celulaG, igeo, 2, APGeom[iG].rug[ind]);
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                ind = sequenciaAP[seq].condEqui[konta1];
                int imat = APCondEquiv[ik].indMat;
                konta1++;
                atualizaMat(ncelG, celula, celulaG, imat, APCondEquiv[ik].cond[ind]);
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            ind = sequenciaAP[seq].INJGpresinj;
            presiniG = APGasInj.presinj[ind];
        }
        if (APGasInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].INJGtemperatura;
            tempiniG = APGasInj.temperatura[ind];
        }
        if (APGasInj.parserieVazGas > 0) {
            ind = sequenciaAP[seq].INJGvazgas;
            vazgasG = APGasInj.vazgas[ind];
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].PEpres;
            presE = APpEntrada.pres[ind];
        }
        if (APpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].PEbet;
            betaE = APpEntrada.bet[ind];
        }
        if (APpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].PEtemperatura;
            tempE = APpEntrada.temperatura[ind];
        }
        if (APpEntrada.parserieTit > 0) {
            ind = sequenciaAP[seq].PEtit;
            titE = APpEntrada.tit[ind];
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].VPEpres;
            presE = APvpEntrada.pres[ind];
        }
        if (APvpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].VPEbet;
            betaE = APvpEntrada.bet[ind];
        }
        if (APvpEntrada.parserieMass > 0) {
            ind = sequenciaAP[seq].VPEmass;
            vazE = APvpEntrada.mass[ind];
        }
        if (APvpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].VPEtemperatura;
            tempE = APvpEntrada.temperatura[ind];
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            ind = sequenciaAP[seq].CHKabertura;
            indCHK = ind;
            chokeSup.AreaGarg = APCHK.abertura[ind] * chokeSup.AreaTub;
        }
        if (APCHK.parserieCD > 0) {
            ind = sequenciaAP[seq].CHKcd;
            chokeSup.cdchk = APCHK.cd[ind];
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            ind = sequenciaAP[seq].PIpresinj;
            presE = APPInj.presfundo[ind];
        }
        if (APPInj.parseriePresFundo > 0) {
            ind = sequenciaAP[seq].PIpresfundo;
            pGSup = APPInj.presinj[ind];
        }
        if (APPInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].PItempinj;
            tempE = APPInj.tempinj[ind];
        }
        if (APPInj.parserieVaz > 0) {
            ind = sequenciaAP[seq].PIvazinj;
            vazE = APPInj.vazinj[ind];
        }
    }
}

void APara::tabelaGenericaCabecalho() {

    // Saídas inerentes das BHPs - IMEX e Eclipse

    // variável vfp (deve-se entrar no .json com os valores de 0 ou 1, descritos como segue)
    // 0-para solver IMEX
    // 1-para solver Eclipse
    // vfp=0; //chris

    ostringstream saidaP1;
    if (vfp == 0)
        saidaP1 << "bhpsIMEX.imx";
    string tmp1 = saidaP1.str();
    ofstream escreveIni1(tmp1.c_str(), ios_base::out);

    ostringstream saidaP2;
    if (vfp == 1)
        saidaP2 << "bhpsEclipse.ecp";
    string tmp2 = saidaP2.str();
    ofstream escreveIni2(tmp2.c_str(), ios_base::out);

    if (vfp == 0) {
        escreveIni1 << "** Relatorio do IMEX gerado pelo Marlim 3 em " << __DATE__ << " " << __TIME__ << endl; // add data e horário such as: 12/23/2015 2:47:47 PM
        escreveIni1 << "\n"
                    << endl;
        escreveIni1 << "** IDENTIFICACAO :" << " CSantim10" << endl;
        escreveIni1 << "** Titulo....... :" << endl; // verificar se é necessário string de entrada
        escreveIni1 << "** Responsavel.. :" << endl; // verificar se é necessário string de entrada
        // escreveIni1 << "** Data......... :" << endl; //19092007   //verificar se é necessário string de entrada
        escreveIni1 << "** Comentario... :" << endl; // verificar se é necessário string de entrada
        escreveIni1 << "\n"
                    << endl;

        // celp[i].profundiM e celp[i].profundiF

        escreveIni1 << "** Dados do Fluido " << endl;
        escreveIni1 << "** Grau API do oleo...... : " << flup[0].API << endl;                                    // add entrada 21.3
        escreveIni1 << "** Densidade da agua .... : " << flup[0].MasEspAgua(celp[0].pres, celp[0].temp) << endl; // add entrada 1.030
        escreveIni1 << "** Densidade do gas  .... : " << flup[0].MasEspGas(celp[0].pres, celp[0].temp) << endl;  // add entrada 0.000
        // escreveIni1 << "** Correlações :" << endl; //add entrada Analise PVT
        escreveIni1 << "\n"
                    << endl;

        // profundidade da BCS i

        if (nAPBCS > 0 && nAPBCS < 10) {
            escreveIni1 << "** Modo de operacao :" << " Elevacao por BCS " << endl; // add entrada QGI_VALV  = 225600.0
            for (int i = 0; i < nAPBCS; ++i) {
                escreveIni1 << "** Profundidade BCS: " << celp[bcs[APBCS[i].indBCS].posicP].profundiM << endl;
                escreveIni1 << "** Frequencia da BCS (Hz): " << APBCS->freq[i] << endl;
            }
            escreveIni1 << "\n"
                        << endl;
        }

        // profundidade da Fonte de Gás i
        if (nAPFG > 0 && nAPFG < 10) {
            escreveIni1 << "** Modo de operacao :" << " Elevacao por Injecao de Gas " << endl; // add entrada QGI_VALV  = 225600.0
            for (int i = 0; i < nAPFG; ++i) {
                escreveIni1 << "** Profundidade FG: " << celp[fonteg[APFonGas[i].indFG].posicP].profundiM << endl;
                for (int i = 0; i < APFonGas->parserieVazG; ++i) {
                    escreveIni1 << "** Vazao de Gas (m3/d): " << APFonGas->vazgas[i] << endl;
                }
            }
            escreveIni1 << "\n"
                        << endl;
        }

        escreveIni1 << "** CURVA DE FVM PARA INJECAO NO FUNDO, COLUNA NOVA" << endl;
        escreveIni1 << "*PTUBE *LIQ 1" << endl;                 // add entrada 1
        escreveIni1 << "*DEPTH  " << celp[0].profundiM << endl; // add entrada  2688.6
        escreveIni1 << "  " << endl;

        // valores devem vir da entrada   0.00   0.10   0.20   0.30   0.40   0.50   0.60   0.70   0.80   0.99   /
        escreveIni1 << "*GOR " << endl; //" tamanho vetor " << APRGO.parserieRGO << endl;
        for (int i = 0; i < APRGO.parserieRGO; ++i) {
            escreveIni1 << APRGO.RGO[i];
            if (i < APRGO.parserieRGO - 1) {
                escreveIni1 << " "; //
            }
        }
        escreveIni1 << "  " << endl;

        escreveIni1 << "*QLIQ " << endl; //" tamanho vetor " << APFonLiq->parserieVL << endl;
        for (int i = 0; i < APFonLiq->parserieVL; ++i) {
            escreveIni1 << APFonLiq->vazliq[i];
            if (i < APFonLiq->parserieVL - 1) {
                escreveIni1 << " ";
            }
        }
        escreveIni1 << "  " << endl;
        // valores devem vir da entrada 10.0 250.0 500.0 750.0 1000.0 1500.0 2000.0   /

        escreveIni1 << "*WCUT " << endl; //" tamanho vetor " << APBSW.parserieBSW << endl;
        for (int i = 0; i < APBSW.parserieBSW; ++i) {
            escreveIni1 << APBSW.BSW[i];
            if (i < APBSW.parserieBSW - 1) {
                escreveIni1 << " ";
            }
        }
        escreveIni1 << "  " << endl;

        escreveIni1 << "*WHP " << endl; // " tamanho vetor " << APPsep.parseriePres << endl; //PSEP
        for (int i = 0; i < APPsep.parseriePres; ++i) {
            escreveIni1 << APPsep.pres[i];
            if (i < APPsep.parseriePres - 1) {
                escreveIni1 << " ";
            }
        }
        escreveIni1 << "  " << endl;
        // valores devem vir da entrada  10.82  15.72  25.53  50.05   /

        escreveIni1 << "*" << "BHPTO" << endl;
        // escreveIni1 << "** " << "QO  " << "WCUT  " << "GOR  " <<  endl; //add BHP1 e no final [kgf/cm2 abs]

    }

    else if (vfp == 1) {

        escreveIni2 << "** Relatorio do ECLIPSE gerado pelo Marlim 3 em " << __DATE__ << " " << __TIME__ << endl; // add data e horário, such as: 4/9/2009 15:25:21
        escreveIni2 << "\n";
        escreveIni2 << "-- Tabela de BHPs para utilizacao no simulador Eclipse." << endl;
        escreveIni2 << "-- Tabela gerada pelo simulador Marlim 3." << endl;

        cout << "nAPBCS: " << nAPBCS << endl;
        if (nAPBCS > 0 && nAPBCS < 10) {
            for (int i = 0; i < nAPBCS; ++i) {
                escreveIni2 << "-- Profundidade Medida BCS: " << celp[bcs[APBCS[i].indBCS].posicP].profundiM << endl;
            }
        }

        if (nAPFG > 0 && nAPFG < 10) {
            for (int i = 0; i < nAPFG; ++i) {
                escreveIni2 << "-- Profundidade Medida de injecao de gas (m): " << celp[fonteg[APFonGas[i].indFG].posicP].profundiM << endl;
            }
        }
        // escreveIni2 << "-- Profundidade Medida de injeção de gás (m):" << endl; //add profundidade 2035
        escreveIni2 << "VFPPROD   --------------" << endl;

        escreveIni2 << "\n"
                    << endl;
        escreveIni2 << "-- N   profundidade canhoneado (m)" << endl;
        // verificar o que é vetor e o que é entrada   1      2655.0     'LIQ' 'WCT' 'GOR' 'THP' 'GRAT' 'METRIC' 'BHP'   /
        escreveIni2 << " 1 " << celp[0].profundiM << " 'LIQ' " << " 'WCT' " << " 'GOR' " << " 'THP' " << " 'GRAT' " << " 'METRIC' " << "'BHP'" << "   /" << endl;
        escreveIni2 << "-- QLIQ (Sm3/d)" << endl; //" tamanho vetor " << APFonLiq->parserieVL << endl;
        for (int i = 0; i < APFonLiq->parserieVL; ++i) {
            escreveIni2 << APFonLiq->vazliq[i];
            if (i < APFonLiq->parserieVL - 1) {
                escreveIni2 << " ";
            }
        }
        escreveIni2 << "   /" << endl;
        // valores devem vir da entrada 10.0 250.0 500.0 750.0 1000.0 1500.0 2000.0   /
        escreveIni2 << "-- THP (kgf/cm2)" << endl; //" tamanho vetor " << APPsep.parseriePres << endl;
        for (int i = 0; i < APPsep.parseriePres; ++i) {
            escreveIni2 << APPsep.pres[i];
            if (i < APPsep.parseriePres - 1) {
                escreveIni2 << " ";
            }
        }
        escreveIni2 << "   /" << endl;
        // valores devem vir da entrada  10.82  15.72  25.53  50.05   /

        escreveIni2 << "-- WCUT (-)" << endl; // " tamanho vetor " << APBSW.parserieBSW << endl;
        for (int i = 0; i < APBSW.parserieBSW; ++i) {
            escreveIni2 << APBSW.BSW[i];
            if (i < APBSW.parserieBSW - 1) {
                escreveIni2 << "   ";
            }
        }
        escreveIni2 << "   /" << endl;

        escreveIni2 << "-- RGO (Sm3/Sm3)" << endl; // " tamanho vetor " << APRGO.parserieRGO << endl;
        for (int i = 0; i < APRGO.parserieRGO; ++i) {
            escreveIni2 << APRGO.RGO[i];
            if (i < APRGO.parserieRGO - 1) {
                escreveIni2 << "   "; //
            }
        }
        escreveIni2 << "   /" << endl;

        // valores devem vir da entrada   0.00   0.10   0.20   0.30   0.40   0.50   0.60   0.70   0.80   0.99   /

        if (nAPBCS > 0 && nAPBCS < 10) {
            escreveIni2 << "-- GRAT (Hz)" << endl;
            for (int i = 0; i < APBCS->parserieFreq; ++i) {
                escreveIni2 << APBCS->freq[i];
                if (i < APBCS->parserieFreq - 1) {
                    escreveIni2 << " ";
                }
            }
            escreveIni2 << "   /" << endl;
        }
        if (nAPFG > 0 && nAPFG < 10) {
            escreveIni2 << "-- GRAT (m3/d)" << endl;
            for (int i = 0; i < APFonGas->parserieVazG; ++i) {
                escreveIni2 << APFonGas->vazgas[i];
                if (i < APFonGas->parserieVazG - 1) {
                    escreveIni2 << " ";
                }
            }
            escreveIni2 << "   /" << endl;
        }

        // número de colunas da tabela do Eclipse (solver de Reservatório) depende do número de variáveis de entrada e de como elas
        escreveIni2 << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

    } else {
        cout << "O valor de vfp deve ser 0-IMEX ou 1-Eclipse" << endl;
    }

    ostringstream saidaP;
    saidaP << "tabelaGenericaAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::out);

    if (vfp == 0)
        escreveIni1 << "** ";
    if (vfp == 1)
        escreveIni2 << "-- ";

    if (listaV.vpsep == 1) {
        escreveIni << " Pressao Separador ;";
        if (vfp == 1)
            escreveIni2 << "nthp ";
    }
    if (listaV.vBSW == 1) {
        escreveIni << " BSW fluido 0 ;";
        if (vfp == 0)
            escreveIni1 << "  GOR  ";
        if (vfp == 1)
            escreveIni2 << " nwc ";
    }
    if (listaV.vRGO == 1) {
        escreveIni << " RGO fluido 0 ;";
        if (vfp == 0)
            escreveIni1 << " WCUT  ";
        if (vfp == 1)
            escreveIni2 << "  nrgo  ";
    }
    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            if (APFonLiq[iFL].parserieTemp > 0) {
                escreveIni << " indice Fonte Liquido = " << konta1 << " Temperatura ;";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {

                escreveIni << " indice Fonte Liquido = " << konta2 << " Vazao de Liquido ;";

                if (vfp == 0) {
                    escreveIni1 << " QLIQ ";
                }

                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                escreveIni << " indice Fonte Liquido = " << konta3 << " Beta ;";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                escreveIni << " indice Fonte Liquido = " << konta4 << " Fluido ;";
                konta4++;
            }
        }
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            if (APFonGas[iFG].parserieTemp > 0) {
                escreveIni << " indice Fonte Gas = " << konta1 << " Temperatura ;";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                escreveIni << " indice Fonte Gas = " << konta2 << " Vazao Gas ;";
                if (vfp == 0) {
                }

                if (vfp == 1) {
                    escreveIni2 << "nalq  ";
                }
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                escreveIni << " indice Fonte Gas = " << konta3 << " Vazao Liq Complementar ;";
                konta3++;
            }
        }
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            if (APIPR[iIPR].parserieTemp > 0) {
                escreveIni << " indice IPR = " << konta1 << " Temperatura ;";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                escreveIni << " indice IPR = " << konta2 << " P Estatica ;";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                escreveIni << " indice IPR = " << konta3 << " indice IP ;";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                escreveIni << " indice IPR = " << konta4 << " II ;";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                escreveIni << " indice IPR = " << konta5 << " qMax ;";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                escreveIni << " indice IPR = " << konta6 << " fluido ;";
                konta6++;
            }
        }
    }
    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            if (APFonMas[iFM].parserieTemp > 0) {
                escreveIni << " indice Fonte Massa = " << konta1 << " Temperatura ;";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                escreveIni << " indice Fonte Massa = " << konta2 << " Vazao Liquido ;";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                escreveIni << " indice Fonte Massa = " << konta3 << " Vazao Liquido Complementar ;";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                escreveIni << " indice Fonte Massa = " << konta4 << " Vazao Gas ;";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                escreveIni << " indice Fonte Massa = " << konta5 << " Fluido ;";
                konta5++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            if (APBCS[indBCS].parserieFreq > 0) {
                escreveIni << " indice BCS = " << konta1 << " Frequencia ;";
                if (vfp == 1) {
                    escreveIni2 << "nfreq  ";
                }
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                escreveIni << " indice BCS = " << konta2 << " N Estagios ;";
                konta2++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            if (APDP[indBCS].parserieDP > 0) {
                escreveIni << " indice Del Pres = " << konta1 << " Del Pres ;";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                escreveIni << " indice Corr Pres Hidro = " << konta1 << " Corr Pres Hidro ;";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                escreveIni << " indice Corr Pres Fric = " << konta1 << " Corr Pre Fric ;";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            if (APdTdL[indBCS].parserieDT > 0) {
                escreveIni << " indice Corr Temperatura = " << konta1 << " Corr Temperatura ;";
                konta1++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {

            if (APBVOL[indBCS].parserieFreq > 0) {
                escreveIni << " indice Bomba Vol = " << konta1 << " Frequencia ;";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                escreveIni << " indice Bomba Vol = " << konta2 << " Capacidade ;";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                escreveIni << " indice Bomba Vol = " << konta3 << " Fator Politropico ;";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            if (APValv[indBCS].parserieAbre > 0) {
                escreveIni << " indice Valvula = " << konta1 << " Abertura ;";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                escreveIni << " indice Valvula = " << konta2 << " CD ;";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            if (APFuro[indBCS].parserieAbre > 0) {
                escreveIni << " indice Vazamento = " << konta1 << " Abertura ;";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                escreveIni << " indice Vazamento = " << konta2 << " CD ;";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                escreveIni << " indice Vazamento = " << konta3 << " Beta ;";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                escreveIni << " indice Vazamento = " << konta4 << " Pressao ;";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                escreveIni << " indice Vazamento = " << konta5 << " Temperatura ;";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                escreveIni << " indice Vazamento = " << konta6 << " Fluido ;";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                escreveIni << " indice Secao Transversal = " << konta1 << " Maior Diam Interno ;";
                konta1++;
            }
            if (APGeom[iG].parserieB > 0) {
                escreveIni << " indice Secao Transversal = " << konta2 << " Menor Diam Interno ;";
                konta2++;
            }
            if (APGeom[iG].parserieRug > 0) {
                escreveIni << " indice Secao Transversal = " << konta3 << " Rugosidade ;";
                konta3++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                escreveIni << " indice Material = " << konta1 << " Condutividade ;";
                konta1++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            escreveIni << " Pressao Injecao ;";
        }
        if (APGasInj.parserieTemp > 0) {

            escreveIni << " Temp Injecao ;";
        }
        if (APGasInj.parserieVazGas > 0) {
            escreveIni << " Vaz Injecao ;";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            escreveIni << " Pres. Contorno ;";
        }
        if (APpEntrada.parserieBet > 0) {
            escreveIni << " Beta Contorno ;";
        }
        if (APpEntrada.parserieTemp > 0) {
            escreveIni << " Temp. Contorno ;";
        }
        if (APpEntrada.parserieTit > 0) {
            escreveIni << " Titulo Contorno ;";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            escreveIni << " Pres. Contorno ;";
        }
        if (APvpEntrada.parserieBet > 0) {
            escreveIni << " Beta Contorno ;";
        }
        if (APvpEntrada.parserieMass > 0) {
            escreveIni << " Vazao Contorno ;";
        }
        if (APvpEntrada.parserieTemp > 0) {
            escreveIni << " Temp. Contorno ;";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            escreveIni << " Abertura Choke ;";
        }
        if (APCHK.parserieCD > 0) {
            escreveIni << " CD Choke ;";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            escreveIni << " Pres Jusante ;";
        }
        if (APPInj.parseriePresFundo > 0) {
            escreveIni << " Pres Montante ;";
        }
        if (APPInj.parserieTemp > 0) {
            escreveIni << " Temp. Injecao ;";
        }
        if (APPInj.parserieVaz > 0) {
            escreveIni << " Vazao Poco Injecao ;";
        }
    }

    int dimensionQL;
    int dimensionPSEP;
    dimensionQL = APFonLiq->parserieVL;
    dimensionPSEP = APPsep.parseriePres;
    if (vfp == 0) {
        for (int i = 0; i < dimensionPSEP; i++) {
            escreveIni1 << " BHP" << i + 1 << "   ";
        }
        escreveIni1 << " [kgf/cm2 absoluta]" << endl;
    }

    if (vfp == 1) {
        for (int i = 0; i < dimensionQL; i++) {
            escreveIni2 << "  BHP" << i + 1 << "    ";
        }
    }

    // escreveIni2 << "\n" << "--                            kgf/cm2 abs " << endl; //kgf/cm2 abs
    escreveIni2 << "\n"
                << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

    escreveIni.close();

    escreveIni1.close();

    escreveIni2.close();
}

void APara::tabelaGenerica(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                           detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                           detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                           double &pGSup, double &temperatura, double &presiniG, double &tempiniG, double &vazgasG,
                           double &presE, double &tempE, double &titE, double &betaE, double &vazE, int seq, int &indCHK,
                           double *vdPdLH, double *vdPdLF, double *vdTdL, double BHP) {

    ostringstream saidaP1;
    if (vfp == 0)
        saidaP1 << "bhpsIMEX.imx";
    string tmp1 = saidaP1.str();
    ofstream escreveIni1(tmp1.c_str(), ios_base::app);

    ostringstream saidaP2;
    if (vfp == 1)
        saidaP2 << "bhpsEclipse.ecp";
    string tmp2 = saidaP2.str();
    ofstream escreveIni2(tmp2.c_str(), ios_base::app);

    ostringstream saidaP;
    saidaP << "tabelaGenericaAP.dat";

    string tmp = saidaP.str();
    ofstream escreveIni(tmp.c_str(), ios_base::app);

    indCHK = 0;
    int ind;
    int coluna = 0;
    if (listaV.vpsep == 1) {
        ind = sequenciaAP[seq].PSEPpres;
        pGSup = APPsep.pres[ind];
        saidaBHP[seq][coluna] = pGSup;
        coluna++;
        escreveIni << pGSup << " ;";
        if (vfp == 1 && chrisao == 0)
            escreveIni2 << "     " << ind + 1 << " ";
    }
    if (listaV.vBSW == 1) {
        ind = sequenciaAP[seq].BSW;
        flup[0].BSW = APBSW.BSW[ind];
        flup[0].RenovaFluido();
        atualizaBSW(celula, APBSW.BSW[ind]);
        saidaBHP[seq][coluna] = flup[0].BSW;
        coluna++;
        escreveIni << flup[0].BSW << " ;";
        if (vfp == 0 && chrisao == 0)
            escreveIni1 << "    " << ind + 1 << "  ";
        if (vfp == 1 && chrisao == 0)
            escreveIni2 << "    " << ind + 1 << " ";
    }
    if (listaV.vRGO == 1) {
        ind = sequenciaAP[seq].RGO;
        if (flup[0].flashCompleto != 2) {
            flup[0].RGO = APRGO.RGO[ind];
            flup[0].RenovaFluido();
        } else
            atualizaCompRGO(APRGO.RGO[ind], flup[0]);
        atualizaRGO(celula, APRGO.RGO[ind]);
        saidaBHP[seq][coluna] = flup[0].RGO;
        coluna++;
        escreveIni << flup[0].RGO << " ;";
        if (vfp == 0 && chrisao == 0)
            escreveIni1 << "    " << ind + 1 << "  ";
        if (vfp == 1 && chrisao == 0)
            escreveIni2 << "    " << ind + 1 << " ";
    }
    if (listaV.vfonliq == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        for (int iFL = 0; iFL < nAPFL; iFL++) {
            int icel = fontel[APFonLiq[iFL].indFL].posicP;
            if (APFonLiq[iFL].parserieTemp > 0) {
                ind = sequenciaAP[seq].FLtemp[konta1];
                celula[icel].acsr.injl.temp = APFonLiq[iFL].temp[ind];
                saidaBHP[seq][coluna] = APFonLiq[iFL].temp[ind];
                coluna++;
                escreveIni << APFonLiq[iFL].temp[ind] << " ;";
                konta1++;
            }
            if (APFonLiq[iFL].parserieVL > 0) {
                ind = sequenciaAP[seq].FLvazliq[konta2];
                celula[icel].acsr.injl.QLiq = APFonLiq[iFL].vazliq[ind];
                saidaBHP[seq][coluna] = APFonLiq[iFL].vazliq[ind];
                if (iFL == 0) {
                    saidaVazLiq[seq][ind] = BHP;
                }
                coluna++;
                escreveIni << APFonLiq[iFL].vazliq[ind] << " ;";
                if (vfp == 0 && chrisao == 0)
                    escreveIni1 << "     " << ind + 1 << "     ";
                konta2++;
            }
            if (APFonLiq[iFL].parserieBet > 0) {
                ind = sequenciaAP[seq].FLbet[konta3];
                celula[icel].acsr.injl.bet = APFonLiq[iFL].bet[ind];
                saidaBHP[seq][coluna] = APFonLiq[iFL].bet[ind];
                coluna++;
                escreveIni << APFonLiq[iFL].bet[ind] << " ;";
                konta3++;
            }
            if (APFonLiq[iFL].parserieFlu > 0) {
                ind = sequenciaAP[seq].FLindfluP[konta4];
                int indFlu = APFonLiq[iFL].indfluP[ind];
                celula[icel].acsr.injl.FluidoPro = flup[indFlu];
                saidaBHP[seq][coluna] = indFlu;
                coluna++;
                escreveIni << indFlu << " ;";
                konta4++;
            }
        }
    }
    if (listaV.vfongas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iFG = 0; iFG < nAPFG; iFG++) {
            int icel = fonteg[APFonGas[iFG].indFG].posicP;
            if (APFonGas[iFG].parserieTemp > 0) {
                ind = sequenciaAP[seq].FGtemp[konta1];
                celula[icel].acsr.injg.temp = APFonGas[iFG].temp[ind];
                saidaBHP[seq][coluna] = APFonGas[iFG].temp[ind];
                coluna++;
                escreveIni << APFonGas[iFG].temp[ind] << " ;";
                konta1++;
            }
            if (APFonGas[iFG].parserieVazG > 0) {
                ind = sequenciaAP[seq].FGvazgas[konta2];
                celula[icel].acsr.injg.QGas = APFonGas[iFG].vazgas[ind];
                saidaBHP[seq][coluna] = APFonGas[iFG].vazgas[ind];
                coluna++;
                escreveIni << APFonGas[iFG].vazgas[ind] << " ;";
                if (vfp == 1 && chrisao == 0)
                    escreveIni2 << "    " << ind + 1 << "     ";
                konta2++;
            }
            if (APFonGas[iFG].parserieVazC > 0) {
                ind = sequenciaAP[seq].FGvazcomp[konta3];
                celula[icel].acsr.injg.razCompGas = APFonGas[iFG].vazcomp[ind];
                saidaBHP[seq][coluna] = APFonGas[iFG].vazcomp[ind];
                coluna++;
                escreveIni << APFonGas[iFG].vazcomp[ind] << " ;";
                konta3++;
            }
        }
    }
    if (listaV.vbcs == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPBCS; indBCS++) {
            int icel = bcs[APBCS[indBCS].indBCS].posicP;
            if (APBCS[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BCSfreq[konta1];
                celula[icel].acsr.bcs.freq = APBCS[indBCS].freq[ind];
                saidaBHP[seq][coluna] = APBCS[indBCS].freq[ind];
                coluna++;
                escreveIni << APBCS[indBCS].freq[ind] << " ;";
                if (vfp == 1 && chrisao == 0)
                    escreveIni2 << "    " << ind + 1 << "     ";
                konta1++;
            }
            if (APBCS[indBCS].parserieEstag > 0) {
                ind = sequenciaAP[seq].BCSnestag[konta2];
                celula[icel].acsr.bcs.nestag = APBCS[indBCS].nestag[ind];
                saidaBHP[seq][coluna] = APBCS[indBCS].nestag[ind];
                coluna++;
                escreveIni << APBCS[indBCS].nestag[ind] << " ;";
                konta2++;
            }
        }
    }
    if (listaV.vipr == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int iIPR = 0; iIPR < nAPIPR; iIPR++) {
            int icel = IPRS[APIPR[iIPR].indIPR].indcel;
            if (APIPR[iIPR].parserieTemp > 0) {
                ind = sequenciaAP[seq].IPRtemp[konta1];
                celula[icel].acsr.ipr.Tres = APIPR[iIPR].temp[ind];
                saidaBHP[seq][coluna] = APIPR[iIPR].temp[ind];
                coluna++;
                escreveIni << APIPR[iIPR].temp[ind] << " ;";
                konta1++;
            }
            if (APIPR[iIPR].parseriePres > 0) {
                ind = sequenciaAP[seq].IPRpres[konta2];
                celula[icel].acsr.ipr.Pres = APIPR[iIPR].pres[ind];
                saidaBHP[seq][coluna] = APIPR[iIPR].pres[ind];
                coluna++;
                escreveIni << APIPR[iIPR].pres[ind] << " ;";
                konta2++;
            }
            if (APIPR[iIPR].parserieIP > 0) {
                ind = sequenciaAP[seq].IPRip[konta3];
                celula[icel].acsr.ipr.ip = APIPR[iIPR].ip[ind];
                saidaBHP[seq][coluna] = APIPR[iIPR].ip[ind];
                coluna++;
                escreveIni << APIPR[iIPR].ip[ind] << " ;";
                konta3++;
            }
            if (APIPR[iIPR].parserieJP > 0) {
                ind = sequenciaAP[seq].IPRjp[konta4];
                celula[icel].acsr.ipr.ij = APIPR[iIPR].jp[ind];
                saidaBHP[seq][coluna] = APIPR[iIPR].jp[ind];
                coluna++;
                escreveIni << APIPR[iIPR].jp[ind] << " ;";
                konta4++;
            }
            if (APIPR[iIPR].parserieqMax > 0) {
                ind = sequenciaAP[seq].IPRqMax[konta5];
                celula[icel].acsr.ipr.qMax = APIPR[iIPR].qMax[ind];
                saidaBHP[seq][coluna] = APIPR[iIPR].qMax[ind];
                coluna++;
                escreveIni << APIPR[iIPR].qMax[ind] << " ;";
                konta5++;
            }
            if (APIPR[iIPR].parserieFlup > 0) {
                ind = sequenciaAP[seq].IPRindfluP[konta6];
                int indFlu = APIPR[iIPR].indfluP[ind];
                celula[icel].acsr.ipr.FluidoPro = flup[indFlu];
                saidaBHP[seq][coluna] = APIPR[iIPR].indfluP[ind];
                coluna++;
                escreveIni << APIPR[iIPR].indfluP[ind] << " ;";
                konta6++;
            }
        }
    }
    if (listaV.vfonmas == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        for (int iFM = 0; iFM < nAPFM; iFM++) {
            int icel = fontem[APFonMas[iFM].indFM].posicP;
            if (APFonMas[iFM].parserieTemp > 0) {
                ind = sequenciaAP[seq].FMtemp[konta1];
                celula[icel].acsr.injm.temp = APFonMas[iFM].temp[ind];
                saidaBHP[seq][coluna] = APFonMas[iFM].temp[ind];
                coluna++;
                escreveIni << APFonMas[iFM].temp[ind] << " ;";
                konta1++;
            }
            if (APFonMas[iFM].parserieMP > 0) {
                ind = sequenciaAP[seq].FMvazMasP[konta2];
                celula[icel].acsr.injm.MassP = APFonMas[iFM].vazMasP[ind];
                saidaBHP[seq][coluna] = APFonMas[iFM].vazMasP[ind];
                coluna++;
                escreveIni << APFonMas[iFM].vazMasP[ind] << " ;";
                konta2++;
            }
            if (APFonMas[iFM].parserieMC > 0) {
                ind = sequenciaAP[seq].FMvazMasC[konta3];
                celula[icel].acsr.injm.MassC = APFonMas[iFM].vazMasC[ind];
                saidaBHP[seq][coluna] = APFonMas[iFM].vazMasC[ind];
                coluna++;
                escreveIni << APFonMas[iFM].vazMasC[ind] << " ;";
                konta3++;
            }
            if (APFonMas[iFM].parserieMG > 0) {
                ind = sequenciaAP[seq].FMvazMasG[konta4];
                celula[icel].acsr.injm.MassG = APFonMas[iFM].vazMasG[ind];
                saidaBHP[seq][coluna] = APFonMas[iFM].vazMasG[ind];
                coluna++;
                escreveIni << APFonMas[iFM].vazMasG[ind] << " ;";
                konta4++;
            }
            if (APFonMas[iFM].parserieFlu > 0) {
                ind = sequenciaAP[seq].FMindfluP[konta5];
                int indFlu = APFonMas[iFM].indfluP[ind];
                celula[icel].acsr.injm.FluidoPro = flup[indFlu];
                saidaBHP[seq][coluna] = APFonMas[iFM].indfluP[ind];
                coluna++;
                escreveIni << APFonMas[iFM].indfluP[ind] << " ;";
                konta5++;
            }
        }
    }
    if (listaV.vdp == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPDP; indBCS++) {
            int icel = dpreq[APDP[indBCS].indDP].posicP;
            if (APDP[indBCS].parserieDP > 0) {
                ind = sequenciaAP[seq].DPdp[konta1];
                celula[icel].acsr.delp = APDP[indBCS].dp[ind];
                saidaBHP[seq][coluna] = APDP[indBCS].dp[ind];
                coluna++;
                escreveIni << APDP[indBCS].dp[ind] << " ;";
                konta1++;
            }
        }
    }
    if (listaV.vdpH == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLH; indBCS++) {
            int icel = APdPdLH[indBCS].indDPHidro;
            if (APdPdLH[indBCS].parserieDPHidro > 0) {
                ind = sequenciaAP[seq].DPdLH[konta1];
                vdPdLH[icel] = APdPdLH[indBCS].dPHidro[ind];
                saidaBHP[seq][coluna] = APdPdLH[indBCS].dPHidro[ind];
                coluna++;
                escreveIni << APdPdLH[indBCS].dPHidro[ind] << " ;";
                konta1++;
            }
        }
    }
    if (listaV.vdpF == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdPdLF; indBCS++) {
            int icel = APdPdLF[indBCS].indDPFric;
            if (APdPdLF[indBCS].parserieDPFric > 0) {
                ind = sequenciaAP[seq].DPdLF[konta1];
                vdPdLF[icel] = APdPdLF[indBCS].dPFric[ind];
                saidaBHP[seq][coluna] = APdPdLF[indBCS].dPFric[ind];
                coluna++;
                escreveIni << APdPdLF[indBCS].dPFric[ind] << " ;";
                konta1++;
            }
        }
    }
    if (listaV.vdt == 1) {
        int konta1 = 0;
        for (int indBCS = 0; indBCS < nAPdTdL; indBCS++) {
            int icel = APdTdL[indBCS].indDT;
            if (APdTdL[indBCS].parserieDT > 0) {
                ind = sequenciaAP[seq].DTdL[konta1];
                vdTdL[icel] = APdTdL[indBCS].dT[ind];
                saidaBHP[seq][coluna] = APdTdL[indBCS].dT[ind];
                coluna++;
                escreveIni << APdTdL[indBCS].dT[ind] << " ;";
                konta1++;
            }
        }
    }
    if (listaV.vbvol == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int indBCS = 0; indBCS < nAPBV; indBCS++) {
            int icel = bvol[APBVOL[indBCS].indBV].posicP;
            if (APBVOL[indBCS].parserieFreq > 0) {
                ind = sequenciaAP[seq].BVOLfreq[konta1];
                celula[icel].acsr.bvol.freq = APBVOL[indBCS].freq[ind];
                saidaBHP[seq][coluna] = APBVOL[indBCS].freq[ind];
                coluna++;
                escreveIni << APBVOL[indBCS].freq[ind] << " ;";
                konta1++;
            }
            if (APBVOL[indBCS].parserieCap > 0) {
                ind = sequenciaAP[seq].BVOLcapacidade[konta2];
                celula[icel].acsr.bvol.capacidade = APBVOL[indBCS].capacidade[ind];
                saidaBHP[seq][coluna] = APBVOL[indBCS].capacidade[ind];
                coluna++;
                escreveIni << APBVOL[indBCS].capacidade[ind] << " ;";
                konta2++;
            }
            if (APBVOL[indBCS].parserieNPoli > 0) {
                ind = sequenciaAP[seq].BVOLnpoli[konta3];
                celula[icel].acsr.bvol.npoli = APBVOL[indBCS].npoli[ind];
                saidaBHP[seq][coluna] = APBVOL[indBCS].npoli[ind];
                coluna++;
                escreveIni << APBVOL[indBCS].npoli[ind] << " ;";
                konta3++;
            }
        }
    }
    if (listaV.vvalv == 1) {
        int konta1 = 0;
        int konta2 = 0;
        for (int indBCS = 0; indBCS < nAPV; indBCS++) {
            int icel = valv[APValv[indBCS].indV].posicP;
            if (APValv[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].VALVabertura[konta1];
                celula[icel].acsr.chk.AreaGarg = APValv[indBCS].abertura[ind] * celula[icel].acsr.chk.AreaTub;
                saidaBHP[seq][coluna] = APValv[indBCS].abertura[ind];
                coluna++;
                escreveIni << APValv[indBCS].abertura[ind] << " ;";
                konta1++;
            }
            if (APValv[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].VALVcd[konta2];
                celula[icel].acsr.chk.cdchk = APValv[indBCS].cd[ind];
                saidaBHP[seq][coluna] = APValv[indBCS].cd[ind];
                coluna++;
                escreveIni << APValv[indBCS].cd[ind] << " ;";
                konta2++;
            }
        }
    }
    if (listaV.vfuro == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        int konta4 = 0;
        int konta5 = 0;
        int konta6 = 0;
        for (int indBCS = 0; indBCS < nAPFuro; indBCS++) {
            int icel = furo[APFuro[indBCS].indFuro].posicP;
            if (APFuro[indBCS].parserieAbre > 0) {
                ind = sequenciaAP[seq].FUROabertura[konta1];
                celula[icel].acsr.fontechk.abertura = APFuro[indBCS].abertura[ind];
                saidaBHP[seq][coluna] = APFuro[indBCS].abertura[ind];
                coluna++;
                escreveIni << APFuro[indBCS].abertura[ind] << " ;";
                konta1++;
            }
            if (APFuro[indBCS].parserieCD > 0) {
                ind = sequenciaAP[seq].FUROcd[konta2];
                celula[icel].acsr.fontechk.cd = APFuro[indBCS].cd[ind];
                saidaBHP[seq][coluna] = APFuro[indBCS].cd[ind];
                coluna++;
                escreveIni << APFuro[indBCS].cd[ind] << " ;";
                konta2++;
            }
            if (APFuro[indBCS].parserieBeta > 0) {
                ind = sequenciaAP[seq].FURObeta[konta3];
                celula[icel].acsr.fontechk.bet = APFuro[indBCS].beta[ind];
                saidaBHP[seq][coluna] = APFuro[indBCS].beta[ind];
                coluna++;
                escreveIni << APFuro[indBCS].beta[ind] << " ;";
                konta3++;
            }
            if (APFuro[indBCS].parseriePres) {
                ind = sequenciaAP[seq].FUROpres[konta4];
                celula[icel].acsr.fontechk.pamb = APFuro[indBCS].pres[ind];
                saidaBHP[seq][coluna] = APFuro[indBCS].pres[ind];
                coluna++;
                escreveIni << APFuro[indBCS].pres[ind] << " ;";
                konta4++;
            }
            if (APFuro[indBCS].parserieTemp > 0) {
                ind = sequenciaAP[seq].FUROtemp[konta5];
                celula[icel].acsr.fontechk.tempT = APFuro[indBCS].temp[ind];
                saidaBHP[seq][coluna] = APFuro[indBCS].temp[ind];
                coluna++;
                escreveIni << APFuro[indBCS].temp[ind] << " ;";
                konta5++;
            }
            if (APFuro[indBCS].parserieFlu > 0) {
                ind = sequenciaAP[seq].FUROindFlu[konta6];
                int indFlu = APFuro[indBCS].indFlu[ind];
                celula[icel].acsr.fontechk.fluidoPamb = flup[indFlu];
                saidaBHP[seq][coluna] = indFlu;
                coluna++;
                escreveIni << indFlu << " ;";
                konta6++;
            }
        }
    }
    if (listaV.diam == 1) {
        int konta1 = 0;
        int konta2 = 0;
        int konta3 = 0;
        for (int iG = 0; iG < nAPGeom; iG++) {
            if (APGeom[iG].parserieA > 0) {
                ind = sequenciaAP[seq].GeomdA[konta1];
                saidaBHP[seq][coluna] = APGeom[iG].dA[ind];
                coluna++;
                escreveIni << APGeom[iG].dA[ind] << " ;";
                konta1++;
            }
            if (APGeom[iG].parserieB > 0) {
                ind = sequenciaAP[seq].GeomdB[konta2];
                saidaBHP[seq][coluna] = APGeom[iG].dB[ind];
                coluna++;
                escreveIni << APGeom[iG].dB[ind] << " ;";
                konta2++;
            }
            if (APGeom[iG].parserieRug > 0) {
                ind = sequenciaAP[seq].GeomdB[konta3];
                saidaBHP[seq][coluna] = APGeom[iG].rug[ind];
                coluna++;
                escreveIni << APGeom[iG].rug[ind] << " ;";
                konta3++;
            }
        }
    }
    if (listaV.kequiv == 1) {
        int konta1 = 0;
        for (int ik = 0; ik < nAPCondEquiv; ik++) {
            if (APCondEquiv[ik].parserieK > 0) {
                ind = sequenciaAP[seq].condEqui[konta1];
                saidaBHP[seq][coluna] = APCondEquiv[ik].cond[ind];
                coluna++;
                escreveIni << APCondEquiv[ik].cond[ind] << " ;";
                konta1++;
            }
        }
    }
    if (listaV.vgasinj == 1) {
        if (APGasInj.parseriePresInj > 0) {
            ind = sequenciaAP[seq].INJGpresinj;
            presiniG = APGasInj.presinj[ind];
            saidaBHP[seq][coluna] = APGasInj.presinj[ind];
            coluna++;
            escreveIni << APGasInj.presinj[ind] << " ;";
        }
        if (APGasInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].INJGtemperatura;
            tempiniG = APGasInj.temperatura[ind];
            saidaBHP[seq][coluna] = APGasInj.temperatura[ind];
            coluna++;
            escreveIni << APGasInj.temperatura[ind] << " ;";
        }
        if (APGasInj.parserieVazGas > 0) {
            ind = sequenciaAP[seq].INJGvazgas;
            vazgasG = APGasInj.vazgas[ind];
            saidaBHP[seq][coluna] = APGasInj.vazgas[ind];
            coluna++;
            escreveIni << APGasInj.vazgas[ind] << " ;";
        }
    }
    if (listaV.vpresent == 1) {
        if (APpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].PEpres;
            presE = APpEntrada.pres[ind];
            saidaBHP[seq][coluna] = presE;
            coluna++;
            escreveIni << presE << " ;";
        }
        if (APpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].PEbet;
            betaE = APpEntrada.bet[ind];
            saidaBHP[seq][coluna] = betaE;
            coluna++;
            escreveIni << betaE << " ;";
        }
        if (APpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].PEtemperatura;
            tempE = APpEntrada.temperatura[ind];
            saidaBHP[seq][coluna] = tempE;
            coluna++;
            escreveIni << tempE << " ;";
        }
        if (APpEntrada.parserieTit > 0) {
            ind = sequenciaAP[seq].PEtit;
            titE = APpEntrada.tit[ind];
            saidaBHP[seq][coluna] = titE;
            coluna++;
            escreveIni << titE << " ;";
        }
    }
    if (listaV.vvazpresent == 1) {
        if (APvpEntrada.parseriePres > 0) {
            ind = sequenciaAP[seq].VPEpres;
            presE = APvpEntrada.pres[ind];
            saidaBHP[seq][coluna] = presE;
            coluna++;
            escreveIni << presE << " ;";
        }
        if (APvpEntrada.parserieBet > 0) {
            ind = sequenciaAP[seq].VPEbet;
            betaE = APvpEntrada.bet[ind];
            saidaBHP[seq][coluna] = betaE;
            coluna++;
            escreveIni << betaE << " ;";
        }
        if (APvpEntrada.parserieMass > 0) {
            ind = sequenciaAP[seq].VPEmass;
            vazE = APvpEntrada.mass[ind];
            saidaBHP[seq][coluna] = vazE;
            coluna++;
            escreveIni << vazE << " ;";
        }
        if (APvpEntrada.parserieTemp > 0) {
            ind = sequenciaAP[seq].VPEtemperatura;
            tempE = APvpEntrada.temperatura[ind];
            saidaBHP[seq][coluna] = tempE;
            coluna++;
            escreveIni << tempE << " ;";
        }
    }
    if (listaV.vchk == 1) {
        if (APCHK.parserieAbre > 0) {
            ind = sequenciaAP[seq].CHKabertura;
            indCHK = ind;
            chokeSup.AreaGarg = APCHK.abertura[ind] * chokeSup.AreaTub;
            saidaBHP[seq][coluna] = APCHK.abertura[ind];
            coluna++;
            escreveIni << APCHK.abertura[ind] << " ;";
        }
        if (APCHK.parserieCD > 0) {
            ind = sequenciaAP[seq].CHKcd;
            chokeSup.cdchk = APCHK.cd[ind];
            saidaBHP[seq][coluna] = chokeSup.cdchk;
            coluna++;
            escreveIni << chokeSup.cdchk << " ;";
        }
    }
    if (listaV.vpocinj == 1) {
        if (APPInj.parseriePresI > 0) {
            ind = sequenciaAP[seq].PIpresinj;
            presE = APPInj.presfundo[ind];
            saidaBHP[seq][coluna] = presE;
            coluna++;
            escreveIni << presE << " ;";
        }
        if (APPInj.parseriePresFundo > 0) {
            ind = sequenciaAP[seq].PIpresfundo;
            pGSup = APPInj.presinj[ind];
            saidaBHP[seq][coluna] = pGSup;
            coluna++;
            escreveIni << pGSup << " ;";
        }
        if (APPInj.parserieTemp > 0) {
            ind = sequenciaAP[seq].PItempinj;
            tempE = APPInj.tempinj[ind];
            saidaBHP[seq][coluna] = tempE;
            coluna++;
            escreveIni << tempE << " ;";
        }
        if (APPInj.parserieVaz > 0) {
            ind = sequenciaAP[seq].PIvazinj;
            vazE = APPInj.vazinj[ind];
            saidaBHP[seq][coluna] = vazE;
            coluna++;
            escreveIni << vazE << " ;";
        }
    }

    saidaBHP[seq][coluna] = BHP;
    escreveIni << BHP << "\n";
    escreveIni.close();

    chrisao++;

    if (vfp == 0) {

        for (int cont = 0; cont < APPsep.parseriePres; ++cont) { //< APFonLiq->parserieVL

            if (chrisao == cont)
                escreveIni1 << BHP << "   ";
        }

        if (chrisao == APPsep.parseriePres)
            escreveIni1 << BHP << "   " << "\n";
        if (chrisao == APPsep.parseriePres)
            chrisao = 0;
    }
    escreveIni1.close();

    if (vfp == 1) {

        for (int cont = 0; cont < APFonLiq->parserieVL; ++cont) { //< APFonLiq->parserieVL

            if (chrisao == cont)
                escreveIni2 << BHP << "   ";
        }

        if (chrisao == APFonLiq->parserieVL)
            escreveIni2 << BHP << "   /" << "\n";
        if (chrisao == APFonLiq->parserieVL)
            chrisao = 0;
    }
    escreveIni2.close();
}
