/*
 * LerRede.h
 *
 * Created on: June 23, 2017
 *     Author: Eduardo
 */

#ifndef LERREDE_H_
#define LERREDE_H_
#define _USE_MATH_DEFINES // Required for M_PI
#define ARQUIVO_SCHEMA_REDE_JSON "schema_rede_1_0_0.json"

#include "Log.h"
#include "SisProd.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

using namespace rapidjson;

// Application logger object
extern Logger logger;

extern int logRede;

struct conexao {
    int sistemaprod;
    int perm;
    int ativo;
    int ncoleta;
    int nafluente;
    int nbloqueio;
    int *coleta;
    int *afluente;
    double compfonte;
    int tipoanel;
    int presimposta;
    int bloqueio;
    int principal;
    int reverso;
    double presJus;
    double presMon;
    int derivaPrincipal;
    int tramoPrimario;
};

struct conexRedeParalela {
    int noP;
    int noS;
};

class Rede {
  public:
    int nsisprod;
    int nfonteR;
    string entrada;
    string entradaLog;
    string *impfiles;
    conexao *malha;
    conexRedeParalela *conexFR;
    double chutHol;
    double relax;
    int tipoRede; // 0 -> standard; 1 -> injection; 2 -> gas-lift ring; 3 -> parallel network
    int redeParalela;
    int anelGL;
    int injec;
    int tipoModeloDrift;
    int chaveredeT;
    int fluidoRede;
    int tabelaDinamica;
    double TmaxR;
    int apenasPreProc;
    int chute;
    int nthrRede;
    double limConverge;
    int tramoPrimario;
    int tramoSecundario;
    varGlob1D *vg1dSP;
    Rede();
    Rede(const string IMPFILE, const string ARQUIVO_LOG, const tipoValidacaoJson_t VALIDACAO, const tipoSimulacao_t SIMULACAO, varGlob1D *Vvg1dSP);
    Rede(const Rede &); // Copy constructor
    Rede &operator=(const Rede &);
    ~Rede() { // Destructor

        if (nsisprod > 0) {
            if (anelGL == 0) {
                for (int i = 0; i < this->nsisprod; i++) {
                    if (malha[i].ncoleta > 0)
                        delete[] malha[i].coleta;
                    if (malha[i].nafluente > 0)
                        delete[] malha[i].afluente;
                }
            }
            if (malha != 0)
                delete[] malha;
            if (conexFR != 0)
                delete[] conexFR;
            delete[] impfiles;
        }
    }

    void lerArq();

  private:
    tipoValidacaoJson_t validacaoJson;
    tipoSimulacao_t tipoSimulacao;
    void writeSchemaRede();
    Document parseSchema();
    Document parseEntrada();
    void validateVsSchema(Document *schemaMrtDoc, Document *mrtDoc);
    void parse_configuracao_inicial(Value &configuracao_inicial_json);
    void parse_arquivos(Value &arquivos_json);
    void parse_conexao(Value &conexao_json);
    void parse_fonteReciproca(Value &fonteRedeParalela_json);
};

#endif /* LERREDE_H_ */