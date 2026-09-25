/*
 * dados1Poisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */
#include "dados1PoissonAxiSim.h"

dadosPAxiSim::dadosPAxiSim(string nomeArquivoEntrada, double* vresGlob,
               double* vTint, double* vdiamI, double* dxCel, int vncel,
			   double* vgeoterm) {
    iniciaVariaveis();
    lerPoisson(nomeArquivoEntrada, vresGlob,
               vTint, vdiamI, dxCel,
			   vncel, vgeoterm);
}

dadosPAxiSim::dadosPAxiSim() {
    iniciaVariaveis();
}

dadosPAxiSim::dadosPAxiSim(const dadosPAxiSim &vPoisson) {
    iniciaVariaveis();
    lerPoisson(vPoisson.entrada, vPoisson.resGlob, vPoisson.tInt,
               vPoisson.diamInt,
			   vPoisson.dxPoco, vPoisson.ncelPoco, vPoisson.perfiGeoterm);
}

dadosPAxiSim &dadosPAxiSim::operator=(const dadosPAxiSim &vPoisson) {
    if (this != &vPoisson) {

        if (nmaterial > 0)
            delete[] mat;

        if (nzonas > 0)
            delete[] zonaProf;

        if(ncelPoco>0){
        	delete[] dxPoco;
        	delete[] perfiGeoterm;
        	delete[] resGlob;
        	delete[] tInt;
        	delete[] tInt0;
        	delete[] diamInt;
        	delete[] qAcop;
        }

        if (ncorte > 0) {
            for (int i = 0; i < this->ncorte; i++) {
                delete[] corte[i].diam;
                delete[] corte[i].indmat;
                delete[] corte[i].discre;
            }
            delete[] corte;
        }


        if (nele > 0) {
            for (int i = 0; i < nele; i++)
                delete[] noEle[i];
            delete[] noEle;
        }
        if (nno > 0) {
            for (int i = 0; i < nno; i++)
                delete[] xcoor[i];
            delete[] xcoor;
            delete[] atributo;
            delete[] tipo;
        }
        if (noZero > 0)
            delete[] vecsra;
        if (noZero > 0)
            delete[] veclm;
        if (noZero > 0)
            delete[] vecfnz;

        iniciaVariaveis();
        if (vPoisson.nele != 0)
            lerPoisson(vPoisson.entrada, vPoisson.resGlob, vPoisson.tInt,
                       vPoisson.diamInt,
					   vPoisson.dxPoco, vPoisson.ncelPoco, vPoisson.perfiGeoterm);
    }

    return *this;
}

void dadosPAxiSim::iniciaVariaveis() {

    ndR=0;
    nVert=0;
	nno = 0;
    nele = 0;
    noZero = 0;
    noEle = 0;
    atributo = 0;
    tipo = 0;
    vecsra = 0;
    veclm = 0;
    vecfnz = 0;
    vecsra = 0;
    veclm = 0;
    vecfnz = 0;
    xcoor = 0;

    tempo = 0;
    acop = 1;
    tParede = 0;
    tInt = 0;
    tAmb = 0;
    valAmb=0.;

    hE = 0;
    resGlob = 0;
    qAcop = 0;
    tInt0 = 0;

    diamInt = 0;
    diamExt = 0;

    xmin = 0;
    xmax = 0;
    ymin = 0;
    ymax = 0;

    temp = detTempoPoisson();
    CC = detCCPoisson();

    entrada = "parametros.json";

    dimExt = -1;

    nthrdLocal = 1;

    rankLU = -1;
    colore = 0;
    solverMat = 0;

    nmaterial=0;
    ncorte=0;
	mat=0;
    corte=0;
    nzonas=0;
    zonaProf=0;
    profIni=0;
    profFim=0;
    ncelPoco=0;
    dxPoco=0;
    perfiGeoterm=0;
}


void dadosPAxiSim::parse_materiais(Value& material_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/material");
		// obter o tamanho do elemento do Json
		nmaterial = material_json.Size();

		// criar vetor de inteiros para armazenar os ids
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		// caso o tamanho do elemento seja maior que zero
		if (nmaterial > 0) {
			// vetor de materiais
			mat = new materialAxiSim[nmaterial];
			for (int i = 0; i < nmaterial; i++) {
				mat[i].cond = 0;
				mat[i].cp = 0;
				mat[i].rho = 0;
				if (!material_json[i].HasMember("condutividade")
					|| !material_json[i].HasMember("calorEspecifico")
					|| !material_json[i].HasMember("massaEspecifica")) {
						// RN-050: Chaves "condutividade", "calorEspecifico", "rho" requeridas para tipo de material solido ou fluido informado pelo usuario (tipo = 0, 1)
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'condutividade', 'calorEspecifico', 'massaEspecifica' requeridas",
								chaveJson, "tipo modelo Axissimetrico");
					} else {
						// caso as chaves "condutividade", "calorEspecifico", "rho", "visc" existam
						// de-para do elemento "material" do json para o struct material
						mat[i].cond =
								material_json[i]["condutividade"].GetDouble();
						mat[i].cp =
								material_json[i]["calorEspecifico"].GetDouble();
						mat[i].rho = material_json[i]["massaEspecifica"].GetDouble();
					}
					// caso fluido informado pelo usuario e a chave "visc" inexista
				}
			}
}

void dadosPAxiSim::parse_corte(Value& corte_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/secaoTransversal");
		// obter o tamanho do elemento do Json

		if (corte_json.HasMember("zona")){
			ncorte = corte_json["zona"].Size();
		}
		else
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Sem detalhamento de zona de formacao para a discretizacao", "Material da secao",
				"Detalhamento Axissimetrico");


		// criar vetor de inteiros para armazenar os ids
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		// caso o tamanho do elemento seja maior que zero
		if (ncorte > 0) {
			// vetor de seções transversais
			corte = new corteAxiSim[ncorte];
			for (int i = 0; i < ncorte; i++) {
					corte[i].a =
							corte_json["diametroInterno"].GetDouble();
				corte[i].ncam = (int) corte_json["camada"].Size();
				// alocar memória para os elementos das camadas
				corte[i].diam = new double[corte[i].ncam];
				corte[i].indmat = new int[corte[i].ncam];
				corte[i].discre = new int[corte[i].ncam];
				// percorrer a lista de camadas da secao transversal
				for (int j = 0; j < corte[i].ncam; j++) {
					// indicador do tipo de medicao das camadas (espessura ou diametro)
					int medicaoCamadasDiametro = 1;
					if (corte_json["camada"][j].HasMember("tipoMedicaoCamada")) {
						string tipoMedicaoCamada(corte_json["camada"]["tipoMedicaoCamada"].GetString());
						transform(tipoMedicaoCamada.begin(),
								tipoMedicaoCamada.end(),
								tipoMedicaoCamada.begin(), ::toupper);
						if (tipoMedicaoCamada.compare("ESPESSURA") == 0)
							medicaoCamadasDiametro = 0;
					}
					string chaveCamada(chaveJson + "/camada/" + to_string(j));
					if (medicaoCamadasDiametro == 1) {
						if (corte_json["camada"][j].HasMember("diametro")) {
							corte[i].diam[j] =
									corte_json["camada"][j]["diametro"].GetDouble();
						} else {
							// RN-046: Chave "diametro" requerida
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chave diametro requerida", chaveCamada,
									"tipoMedicaoCamada=DIAMETRO Discretizacao Axissimetrica");
						}
					} else {
						if (corte_json["camada"][j].HasMember("espessura")) {
							double espessura =
									corte_json["camada"][j]["espessura"].GetDouble();
							if (j > 0)
								corte[i].diam[j] = corte[i].diam[j - 1]
										+ 2 * espessura;
							else
								corte[i].diam[j] = corte[i].a + 2 * espessura;
						} else {
							// RN-047: Chave "espessura" requerida
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chave espessura requerida", chaveCamada,
									"tipoMedicaoCamada=ESPESSURA Discretizacao Axissimetrica");
						}
					}
					if (corte_json["camada"][j].HasMember("discretizacao"))
					corte[i].discre[j] =
							corte_json["camada"][j]["discretizacao"].GetInt();
					else corte[i].discre[j] =1;
					if(corte[i].discre[j] ==0)corte[i].discre[j]=1;
					if(corte[i].discre[j] <=0)
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Sem discretizacao valida da camada<=0", chaveCamada,
								"Discretizacao Axissimetrica");
				}
			}

			if(corte_json.HasMember("zona")){
				nzonas=corte_json["zona"].Size();
				zonaProf = new double[nzonas];
				for(int i = 0; i < ncorte; i++){
					int nMat=corte_json["zona"][i]["idMaterial"].Size();
					if(nMat!=corte[i].ncam){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Chave Material", "zona de troca termica axisimetrica",
								"vetor de materiais diferente do numero de camadas");
					}
					else{
						for (int j = 0; j < corte[i].ncam; j++) {
							corte[i].indmat[j] =corte_json["zona"][i]["idMaterial"][j].GetInt();
						}
					}
					if(corte_json["zona"][i].HasMember("profundidade")){
						zonaProf[i]=corte_json["zona"][i]["profundidade"].GetDouble();
					}
					else{
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Chave profundidade", "zona de troca termica axissimetrica",
								"sem valor de profundidade da zona de troca termica");
					}
				}
			}
			else{
				logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Sem zona de formacao", "zonas de troca termica",
						"Detalhamento Axissimetrico");
			}
			if(fabs(fabs(zonaProf[nzonas])-fabs(profFim))<1e-10)
				logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Profundidade final da zona de troca termica diferente da profundiade do poco",
							"zonas de troca termica",
							"Detalhamento Axissimetrico");

		}
}

void dadosPAxiSim::lerPoisson(string nomeArquivoEntrada, double* vresGlob,
		double* vTint, double* vdiamI, double* dxCel,
		int vncel, double* vgeoterm) {

	ostringstream arquivotemp;
	arquivotemp<<pathArqEntrada<<nomeArquivoEntrada;
	entrada = arquivotemp.str();
    FILE *fp = fopen(entrada.c_str(), "r");

	if (!fp) {
		logger.log(LOGGER_FALHA,LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Arquivo não encontrado: ",entrada, "leitura de json");
		return;
	}

    char readBuffer[125536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document elementoPoissonRaiz;
    elementoPoissonRaiz.ParseStream(is);
    fclose(fp);

	// DIAGNÓSTICO: Verifica se o JSON foi parseado corretamente
	/*if (elementoPoissonRaiz.HasParseError()) {
		logger.log(LOGGER_FALHA,LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "Erro ao parsear JSON",entrada, "leitura de json");
		return;
	}*/

	// DIAGNÓSTICO: Verifica se é um objeto
	/*if (!elementoPoissonRaiz.IsObject()) {
		logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,"JSON raiz não é um objeto",entrada, "leitura de json");
		return;
	}*/

	// DIAGNÓSTICO: Lista as chaves disponíveis
	StringBuffer sb;
	Writer<StringBuffer> writer(sb);
	elementoPoissonRaiz.Accept(writer);
	logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED,
			"JSON parseado: ", string(sb.GetString()), "SUCESSO");

    ncelPoco=vncel;
    dxPoco=new double[vncel] ;
    perfiGeoterm=new double[vncel] ;
    qAcop=new double[vncel] ;
    for(int i=0; i<vncel;i++){
    	dxPoco[i]=dxCel[i];
    	perfiGeoterm[i]=vgeoterm[i];
    	qAcop[i]=0;
    }

    profIni=0.;
    profFim=0.;
    for(int i=0; i<vncel;i++)profFim+=dxCel[i];

    assert(elementoPoissonRaiz.HasMember("secaoTransversal"));
    parse_materiais(elementoPoissonRaiz["material"]);
    parse_corte(elementoPoissonRaiz["secaoTransversal"]);

    nno=1;
    for(int icam=0; icam<corte[0].ncam;icam++){
    	nno+=corte[0].discre[icam];
    }
    nno*=(vncel+1);
    xcoor = new double *[nno];
    int nvert = 4;
    for (int i = 0; i < nno; i++)
        xcoor[i] = new double[2];
    atributo = new double[nno];
    tipo = new int[nno];
    double prof=0.;
    int ncoor=0;
    ndR=0;
	for(int icam=0; icam<corte[0].ncam;icam++)ndR+=corte[0].discre[icam];
    for(int iprof=0;iprof<=vncel;iprof++){
    	xcoor[ncoor][0]=0.5*corte[0].a;
    	xcoor[ncoor][1]=prof;
    	double delR;
    	for(int icam=0; icam<corte[0].ncam;icam++){

    		if(icam==0)delR=0.5*(corte[0].diam[icam]-corte[0].a)/corte[0].discre[icam];
    		else delR=0.5*(corte[0].diam[icam]-corte[0].diam[icam-1])/corte[0].discre[icam];
    		int limDiscre=corte[0].discre[icam];
    		if(icam==corte[0].ncam-1)limDiscre=corte[0].discre[icam]+1;
    		for(int idiscre=0;idiscre<limDiscre;idiscre++){
            	atributo[ncoor]=0;
            	if(iprof==0 && (icam==0 && idiscre==0))
            		atributo[ncoor]=1;
            	else if(iprof==0 && (icam==corte[0].ncam-1 && idiscre==corte[0].discre[icam]))
            		atributo[ncoor]=1;
            	else if(iprof==vncel && (icam==0 && idiscre==0))
            		atributo[ncoor]=1;
            	else if(iprof==vncel && (icam==corte[0].ncam-1 && idiscre==corte[0].discre[icam]))
            		atributo[ncoor]=1;
    			if(iprof==0)tipo[ncoor]=1;
            	else if(iprof==vncel)tipo[ncoor]=3;
            	else if(icam==0 && idiscre==0) tipo[ncoor]=4;
            	else if(icam==corte[0].ncam-1 && idiscre==corte[0].discre[icam]) tipo[ncoor]=2;
            	else tipo[ncoor]=0;
    			if(idiscre<corte[0].discre[icam]){
    				xcoor[ncoor+1][0]=xcoor[ncoor][0]+delR;
    				xcoor[ncoor+1][1]=prof;
    			}
    	    	ncoor++;
    		}
    	}
    	prof-=dxCel[iprof];
    }
    nVert=4;
    nele=ndR*vncel;
    noEle = new int *[nele];
    for (int i = 0; i < nele; i++) {
        noEle[i] = new int[nvert];
    }
    int contaEle=0;
    int contaNo=0;
    for(int iP=0; iP<vncel; iP++){
    	for(int iR=0; iR<ndR; iR++){
    		noEle[contaEle][0]=contaNo;
    		noEle[contaEle][1]=contaNo+1;
    		noEle[contaEle][2]=contaNo+ndR+2;
    		noEle[contaEle][3]=contaNo+ndR+1;
    		contaEle++;
    		contaNo++;
    	}
    	contaNo++;
    }


    parse_configIni(elementoPoissonRaiz["configuracaoInicial"]);
    parse_CC(/*elementoPoissonRaiz["CC"]*/);
    resGlob=new double[ncelPoco];
    tInt=new double[ncelPoco];
    tInt0=new double[ncelPoco];
    diamInt=new double[ncelPoco];
    for(int iPoc=0;iPoc<ncelPoco; iPoc++){
    	resGlob[iPoc]=vresGlob[iPoc];
    	tInt[iPoc]=vTint[iPoc];
    	tInt0[iPoc]=vTint[iPoc];
    	diamInt[iPoc] = vdiamI[iPoc];
    }
    diamExt = corte[0].a;

    noZero = 0;
    for (int elem = 0; elem < nele; elem++) {
        int face;
        noZero++;
        for (int i = 0; i < nvert; i++) {
            face = -1;
            int v1 = noEle[elem][i];
            int v2;
            if (i < nvert - 1)
                v2 = noEle[elem][i + 1];
            else
                v2 = noEle[elem][0];
            int match = 0;
            for (int j = 0; j < nele; j++) {
                if (j != elem) {
                    for (int k = 0; k < nvert; k++) {
                        int v1V = noEle[j][k];
                        int v2V;
                        if (k < nvert - 1)
                            v2V = noEle[j][k + 1];
                        else
                            v2V = noEle[j][0];
                        if ((v1V == v1 || v1V == v2) && (v2V == v1 || v2V == v2)) {
                            match = 1;
                            face = j;
                        }
                        if (match == 1)
                            break;
                    }
                }
                if (match == 1)
                    break;
            }
            if (face >= 0)
                noZero++;
        }
    }
    vecsra = new double[noZero];
    veclm = new int[noZero];
    if (noZero > 0) {
        vecfnz = new int[nele + 1];
        vecfnz[nele] = noZero;
    }
}

void dadosPAxiSim::parse_configIni(Value &configuracaoInicial_json) {
    string chaveJson("#/configuracaoInicial");

    temp.perm = 1;
    temp.trans = 1;
    temp.relax = 1.;
    temp.parserie = 0;
    temp.dtmax = 0;
    temp.tempo = 0;
    temp.tmax = 0.;
    temp.tempoImp = 0;
    temp.parserieImp = 0;
    acopDirichlet=0;

    if (configuracaoInicial_json.HasMember("temperaturaAmbiente"))
        tAmb =
            configuracaoInicial_json["temperaturaAmbiente"].GetDouble();
    else
        logger.log(LOGGER_FALHA,
                   LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'temperaturaAmbiente'", chaveJson, "configuracao inicial");

    if (configuracaoInicial_json.HasMember("hAmbiente"))
        hE =
            configuracaoInicial_json["hAmbiente"].GetDouble();
    else
        logger.log(LOGGER_FALHA,
                   LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'hAmbiente'", chaveJson, "configuracao inicial");



    if (configuracaoInicial_json.HasMember("threadLocal"))
        nthrdLocal = configuracaoInicial_json["threadLocal"].GetInt();

    acop=1;
    if (configuracaoInicial_json.HasMember("acop"))
        acop = configuracaoInicial_json["acop"].GetInt();

    if (configuracaoInicial_json.HasMember("acopDirichlet"))
        acopDirichlet = configuracaoInicial_json["acopDirichlet"].GetInt();

    if (configuracaoInicial_json.HasMember("relax"))
        temp.relax =
            configuracaoInicial_json["relax"].GetDouble();

    temp.dtmax = 0;
    temp.tempo = 0;

    if (configuracaoInicial_json.HasMember("tempoImp"))
        temp.parserieImp =
            configuracaoInicial_json["tempoImp"].Size();
    else if (temp.trans == 1)
        logger.log(LOGGER_FALHA,
                   LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                   "Chave 'tempoImp' inexistente para impressão do processo transiente", chaveJson, "condicao de tempo");
    temp.tempoImp = new double[temp.parserieImp];
    for (int i = 0; i < temp.parserieImp; i++) {
        temp.tempoImp[i] = configuracaoInicial_json["tempoImp"][i].GetDouble();
    }

    if (configuracaoInicial_json.HasMember("DimensaoReferenciaExterna"))
        dimExt = configuracaoInicial_json["DimensaoReferenciaExterna"].GetDouble();

    solverMat = 0;
    if (configuracaoInicial_json.HasMember("solverMat"))
        solverMat = configuracaoInicial_json["solverMat"].GetInt();
    rankLU = -1;
    if (configuracaoInicial_json.HasMember("rankLU"))
        rankLU = configuracaoInicial_json["rankLU"].GetInt();
    colore = 0;
    if (rankLU >= 0) {
        if (configuracaoInicial_json.HasMember("corLU"))
            colore = configuracaoInicial_json["corLU"].GetBool();
    }
}
void dadosPAxiSim::parse_CC(/*Value &CC_json*/) {
    //string chaveJson("#/CC");

    CC.nDiri = 0;
    CC.ccDir = 0;
    CC.nRic = 0;
    CC.ccRic = 0;
    CC.nVN = 0;
    CC.ccVN = 0;
    CC.rotuloAcop = -1;

    //if (CC_json.HasMember("Dirichlet")) {
        CC.nDiri = 1;
           // CC_json["Dirichlet"].Size();
        CC.ccDir = new detDiriPoisson[CC.nDiri];
    //}
   // if (CC_json.HasMember("Richardson")) {
        CC.nRic =2;
            //CC_json["Richardson"].Size();
        CC.ccRic = new detRicPoisson[CC.nRic];
   // }
    //if (CC_json.HasMember("VonNewman")) {
       // CC.nVN =
          //  CC_json["VonNewman"].Size();
      //  CC.ccVN = new detVNPoisson[CC.nVN];
    //}
    //if (acop == 1) {
        CC.rotuloAcop = 4;//CC_json["acoplamento"].GetInt();
    //}

    for (int i = 0; i < CC.nDiri; i++) {
        //if(i==0)CC.ccDir[i].rotulo = 2;//CC_json["Dirichlet"][i]["rotulo"].GetInt();
        //else
    	CC.ccDir[i].rotulo = 3;
        CC.ccDir[i].nserie = 1;// CC_json["Dirichlet"][i]["valor"].Size();
        //if (CC_json["Dirichlet"][i]["valor"].Size() != CC_json["Dirichlet"][i]["tempo"].Size()) {
          //  logger.log(LOGGER_FALHA,
                      // LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                      // "Chaves 'valor', 'tempo' com dimensoes diferentes",
                      // chaveJson, "Condicoes de Contorno Dirichlet");
       // }
        CC.ccDir[i].tempo = new double[CC.ccDir[i].nserie];
        CC.ccDir[i].val = new double[CC.ccDir[i].nserie];
        for (int j = 0; j < CC.ccDir[i].nserie; j++) {
            CC.ccDir[i].tempo[j] = 0.;// CC_json["Dirichlet"][i]["tempo"][j].GetDouble();
            CC.ccDir[i].val[j] = 0.;//CC_json["Dirichlet"][i]["valor"][j].GetDouble();
        }
    }

   // for (int i = 0; i < CC.nVN; i++) {
     //   CC.ccVN[i].rotulo = CC_json["VonNewman"][i]["rotulo"].GetInt();
      //  CC.ccVN[i].nserie = CC_json["VonNewman"][i]["valor"].Size();
      //  if (CC_json["VonNewman"][i]["valor"].Size() != CC_json["VonNewman"][i]["tempo"].Size()) {
         //   logger.log(LOGGER_FALHA,
             //          LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
           //            "Chaves 'valor', 'tempo' com dimensoes diferentes",
             //          chaveJson, "Condicoes de Contorno Newman");
        //}
       // CC.ccVN[i].tempo = new double[CC.ccVN[i].nserie];
       // CC.ccVN[i].val = new double[CC.ccVN[i].nserie];
       // for (int j = 0; j < CC.ccVN[i].nserie; j++) {
         //   CC.ccVN[i].tempo[j] = CC_json["VonNewman"][i]["tempo"][j].GetDouble();
          //  CC.ccVN[i].val[j] = CC_json["VonNewman"][i]["valor"][j].GetDouble();
        //}
   // }

    for (int i = 0; i < CC.nRic; i++) {
    	if(i==0)CC.ccRic[i].rotulo = 1;//CC_json["Richardson"][i]["rotulo"].GetInt();
    	else CC.ccRic[i].rotulo = 2;
        CC.ccRic[i].nserie = 1;
        CC.ccRic[i].tempo = new double[CC.ccRic[i].nserie];
        CC.ccRic[i].valAmb = new double[CC.ccRic[i].nserie];
        CC.ccRic[i].hAmb = new double[CC.ccRic[i].nserie];
        for (int j = 0; j < CC.ccRic[i].nserie; j++) {
            CC.ccRic[i].tempo[j] = 0.;  // CC_json["Richardson"][i]["tempo"][j].GetDouble();
            CC.ccRic[i].valAmb[j] = 0.; // CC_json["Richardson"][i]["valorAmb"][j].GetDouble();
            CC.ccRic[i].hAmb[j] = 0.;   // CC_json["Richardson"][i]["hAmb"][j].GetDouble();
        }
    }
}

