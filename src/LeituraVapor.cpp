/*
 * LeituraVapor.cpp
 *
 *  Created on: 20 de mai de 2019
 *      Author: eduardo
 */



/*
 * Leitura.cpp
 *
 *  Created on: 18/03/2015
 *      Author: bwz1
 */
//#include "Acidentes2.h"
#include "LeituraVapor.h"

using namespace rapidjson;

LerVap::LerVap(const string IMPFILE,varGlob1D* Vvg1dSP) {
	vg1dSP=Vvg1dSP;
	dtmaxserie.parserie=0;
	ConContEntrada=0;//alteracao4
	nmaterial=0;
	ncorte=0;
	nduto=0;
	nunidadep=0;
	ncelp=0;
	nipr=0;
	ninjliq=0;
	nbcs=0;
	nbvol=0;
	ndpreq=0;
	npig=0;
	master1.parserie=0;
	psep.parserie=0;
	chokep.parserie=0;
	nperfisp=0;
	nperfistransp=0;
	ntendp=0;
	ntendtransp=0;
	nduto=0;
	ntela=0;

	nform=0;//alteracao2
	formacPoc=0;//alteracao2

	mat=0;
	corte=0;
	duto=0;
	unidadeP=0;
	celp=0;
	IPRS=0;
	fontel=0;
	bcs=0;
	bvol=0;
	dpreq=0;
	fontechk=0;
	pig=0;
	trendp=0;
	nvartrendp=0;
	trendtransp=0;
	tela=0;
	Tevento=0;
	Teventof=0;
	TeventoPXO=0;

	impfile = IMPFILE;
	lerArq();
}

LerVap::LerVap(const LerVap& vler) {
	vg1dSP=vler.vg1dSP;
	dtmaxserie.parserie=0;
	ConContEntrada=0;//alteracao4
	nmaterial=0;
	ncorte=0;
	nduto=0;
	nunidadep=0;
	ncelp=0;
	nipr=0;
	ninjliq=0;
	nbcs=0;
	nbvol=0;
	ndpreq=0;
	npig=0;
	master1.parserie=0;
	psep.parserie=0;
	chokep.parserie=0;
	nperfisp=0;
	nperfistransp=0;
	ntendp=0;
	ntendtransp=0;
	nduto=0;
	ntela=0;

	nform=0;//alteracao2
	formacPoc=0;//alteracao2

	mat=0;
	corte=0;
	duto=0;
	unidadeP=0;
	celp=0;
	IPRS=0;
	fontel=0;
	bcs=0;
	bvol=0;
	dpreq=0;
	fontechk=0;
	pig=0;
	trendp=0;
	nvartrendp=0;
	trendtransp=0;
	tela=0;
	Tevento=0;
	Teventof=0;
	TeventoPXO=0;


	impfile = vler.impfile;
	lerArq();
}

LerVap::LerVap() {
	vg1dSP=0;
	mono=0;
	ConContEntrada=0;//alteracao4
	eventoabrePXO=0;
	tfinal=0;
	transiente=0;
	ordperm=0;
	dutosMRT=0;
	chkv=0;
	dtmax=0;
	critcond=0;
	areagargPXO=0;
	equilterm=0;
	eventofecha=0;
	anmP=0;
	transmass=0;
	snaps=0;
	referencia=0;
	perm=0;
	eventoabre=0;
	nvarprofp=0;

	dtmaxserie.parserie=0;
	nmaterial=0;
	ncorte=0;
	nduto=0;
	nunidadep=0;
	ncelp=0;
	nipr=0;
	ninjliq=0;
	nbcs=0;
	nbvol=0;
	ndpreq=0;
	npig=0;
	master1.parserie=0;
	psep.parserie=0;
	chokep.parserie=0;
	nperfisp=0;
	nperfistransp=0;
	ntendp=0;
	ntendtransp=0;
	nduto=0;
	ntela=0;

	nform=0;//alteracao2
	formacPoc=0;//alteracao2

	mat=0;
	corte=0;
	duto=0;
	unidadeP=0;
	celp=0;
	IPRS=0;
	fontel=0;
	bcs=0;
	bvol=0;
	dpreq=0;
	fontechk=0;
	pig=0;
	trendp=0;
	nvartrendp=0;
	trendtransp=0;
	tela=0;
	Tevento=0;
	Teventof=0;
	TeventoPXO=0;

}

LerVap& LerVap::operator =(const LerVap& vler){
 if (this != &vler) {
	if(dtmaxserie.parserie>0){
		delete [] dtmaxserie.tempo;
		delete [] dtmaxserie.valor;
	}

	if(nform>0) delete [] formacPoc;//aletracao2

	if(ConContEntrada>0){//alteracao4
		delete [] CCPres.temperatura;
		delete [] CCPres.pres;
		delete [] CCPres.tit;
		delete [] CCPres.tempo;
	}//alteracao4

	//delete [] varRGO.tempo;
	//delete [] varRGO.valor;

	if(nmaterial>0) delete [] mat;

	if(ncorte>0){
	  for(int i=0; i<this->ncorte;i++){
		  delete [] corte[i].diam;
		  delete [] corte[i].indmat;
		  delete [] corte[i].discre;
	  }
	  delete [] corte;
	}

	if(nduto>0) delete [] duto;

	if(nunidadep>0){
		for(int i=0; i<nunidadep;i++){
			delete [] unidadeP[i].dx;
			delete [] unidadeP[i].dxVar;
			for(int j=0;j<11;j++)delete [] unidadeP[i].var[j];
			delete [] unidadeP[i].var;
		}
		delete [] unidadeP;
	}

	if(ncelp>0) delete [] celp;

	if(nipr>0){
	  for(int i=0; i<this->nipr;i++){
		  delete [] IPRS[i].pres;
		  delete [] IPRS[i].tpres;
		  delete [] IPRS[i].temp;
		  delete [] IPRS[i].ttemp;
		  delete [] IPRS[i].jp;
		  delete [] IPRS[i].tjp;

	  }
	  delete [] IPRS;
	}
	if( ConContEntrada==1){
        delete [] CCPres.temperatura;
        delete [] CCPres.pres;
        delete [] CCPres.tit;
        delete [] CCPres.tempo;
	}

	if(ninjliq>0){
	  for(int i=0; i<this->ninjliq;i++){
		  delete [] fontel[i].temp;
		  delete [] fontel[i].tempo;
	  }
	  delete [] fontel;
	}

	if(nbcs>0){
	  for(int i=0; i<this->nbcs;i++){
		  delete [] bcs[i].tempo;
		  delete [] bcs[i].freq;
		  delete [] bcs[i].vaz;
		  delete [] bcs[i].head;
		  delete [] bcs[i].power;
		  delete [] bcs[i].efic;
	  }
	  delete [] bcs;
	}

	if(nbvol>0){
	  for(int i=0; i<this->nbvol;i++){
		  delete [] bvol[i].tempo;
		  delete [] bvol[i].freq;
	  }
	  delete [] bvol;
	}

	if(ndpreq>0){
	  for(int i=0; i<this->ndpreq;i++){
		  delete [] dpreq[i].dp;
		  delete [] dpreq[i].tempo;
	  }
	  delete [] dpreq;
	}

	if(npig>0) delete [] pig;

	int nevento=master1.parserie;
	if(master1.parserie>0)delete [] master1.tempo;
	if(master1.parserie>0)delete [] master1.abertura;

	if(psep.parserie>0){
		delete [] psep.tempo;
		delete [] psep.pres;
	}

	if(chokep.parserie>0){
	    delete [] chokep.tempo;
	    delete [] chokep.abertura;
	}

	if(nperfisp>0)	delete [] profp.tempo;
	if(nperfistransp>0){
		delete [] proftransp.posic;
		delete [] proftransp.tempo;
	}
	if(ntendp>0){
		delete [] trendp;
	    delete [] nvartrendp;
	}
	if(ntendtransp>0)	delete [] trendtransp;
	if(nduto>0) delete [] dutosMRT;

	if(nevento>0) delete [] Tevento;
	if(nevento>0) delete [] Teventof;
	//if(TeventoPXO) delete [] TeventoPXO;

	if(ntela>0) delete [] tela;
	vg1dSP=vler.vg1dSP;
	impfile = vler.impfile;
	lerArq();
 }

	return *this;
}
/*
 * Converter o elemento "configuracaoInicial" do arquivo Json do MRT em struct mrtjson::configuracao_inicial_mrt_t.
 *
 * Entrada: Value&
 * Retorna: mrtjson::configuracao_inicial_mrt_t
 */
void LerVap::parse_configuracao_inicial(
		Value& configuracao_inicial_json) {
	/*
	 "configuracaoInicial": {
	 "linhaGas": true,
	 "equilibrioTermico": true,
	 "latente": true,
	 "modeloCp": true,
	 "pvtsimArq": "PVTSIM-MARLIM.tab",
	 "tabP": true,
	 "tabG": true,
	 "trackRgo": true,
	 "trackDensidadeGas": true,
	 "permanente": true,
	 "ordemperm" : 1,
	 "transiente": true,
	 "transferenciaMassa": 0,
	 "iniFluidoP": 1,
	 "pocinjec" : false
	 }
	 */
// obter os valores dos atributos do elemento "configuracaoInicial" do Json
// Verificar se o arquivo JSON é para simulação de poço injetor


	  equilterm = configuracao_inicial_json["equilibrioTermico"].GetBool();

	  snaps=0;
	  if(configuracao_inicial_json.HasMember("SnapShot")){
		  snaps=configuracao_inicial_json["SnapShot"].GetBool();
          if (configuracao_inicial_json.HasMember("SnapShotArq"))
            snapshotArqIn = configuracao_inicial_json["SnapShotArq"].GetString();
          else snapshotArqIn = "teste1.snp";
	  }
	  if(snaps==0){
	    perm = configuracao_inicial_json["permanente"].GetBool();
	    if (perm == 1) {
		  ordperm = configuracao_inicial_json["ordemperm"].GetInt();
	    } else {
		  ordperm = 0;
	    }
	  }
	  else perm=2;
	  transiente = configuracao_inicial_json["transiente"].GetBool();
      transmass = configuracao_inicial_json["transferenciaMassa"].GetInt();
      if(configuracao_inicial_json.HasMember("CheckValve"))chkv=configuracao_inicial_json["CheckValve"].GetInt();
      else chkv=0;
      referencia=1;//alteracao2
      if(configuracao_inicial_json.HasMember("RefPosicao"))referencia=configuracao_inicial_json["RefPosicao"].GetInt();//alteracao2
	if(configuracao_inicial_json.HasMember("Formacao")){
		nform=configuracao_inicial_json["Formacao"]["Propriedades"].Size();//alteracao2
		double tempProd=configuracao_inicial_json["Formacao"]["TempoProducao"].GetDouble();//alteracao2
		formacPoc=new detFormacaoVap[nform];//alteracao2
		//formacPoc.tempo=configuracao_inicial_json["Formacao"]["tempo"].GetDouble();//alteracao2
		//formacPoc.cond=configuracao_inicial_json["Formacao"]["condutividade"].GetDouble();//alteracao2
		//formacPoc.cp=configuracao_inicial_json["Formacao"]["calorEspecifico"].GetDouble();//alteracao2
		//formacPoc.rho=configuracao_inicial_json["Formacao"]["massaEspecifica"].GetDouble();//alteracao2
		for(int i=0;i<nform;i++){//alteracao2
			formacPoc[i].tempo=tempProd;//alteracao2
			formacPoc[i].cond=configuracao_inicial_json["Formacao"]["Propriedades"][i]["condutividade"].GetDouble();//alteracao2
			formacPoc[i].cp=configuracao_inicial_json["Formacao"]["Propriedades"][i]["calorEspecifico"].GetDouble();//alteracao2
			formacPoc[i].rho=configuracao_inicial_json["Formacao"]["Propriedades"][i]["massaEspecifica"].GetDouble();//alteracao2
		}//alteracao2
	}
	else{
		nform=1;//alteracao2
		formacPoc=new detFormacaoVap[nform];//alteracao2
		formacPoc[0].tempo=-100.;//altercacao2
		formacPoc[0].cond=0.;//altercacao2
		formacPoc[0].cp=0.;//altercacao2
		formacPoc[0].rho=0.;//altercacao2
		nform=0;//altercacao2
	}
	if(configuracao_inicial_json.HasMember("Avancado")){
		mono=configuracao_inicial_json["Avancado"]["CriterioMonofasico"].GetDouble();
		critcond=configuracao_inicial_json["Avancado"]["CriterioCondensacao"].GetDouble();
	}
	else{
		mono=1e-9;
		critcond=1e-9;
	}

	   ConContEntrada = 0;        //alteracao4
	    if (configuracao_inicial_json.HasMember("condicaoPressao")) {
	      ConContEntrada = 1;
	        CCPres.parserie = configuracao_inicial_json["condicaoPressao"]["pressao"].Size();
	        CCPres.temperatura = new double[CCPres.parserie];
	        CCPres.pres = new double[CCPres.parserie];
	        CCPres.tit = new double[CCPres.parserie];
	        CCPres.tempo = new double[CCPres.parserie];
	        for (int i = 0; i < CCPres.parserie; i++)
	          CCPres.temperatura[i] = configuracao_inicial_json["condicaoPressao"]["temperatura"][i].GetDouble();
	        for (int i = 0; i < CCPres.parserie; i++)
	          CCPres.pres[i] = configuracao_inicial_json["condicaoPressao"]["pressao"][i].GetDouble();
	        for (int i = 0; i < CCPres.parserie; i++)
	          CCPres.tit[i] = configuracao_inicial_json["condicaoPressao"]["titulo"][i].GetDouble();
	        for (int i = 0; i < CCPres.parserie; i++)
	          CCPres.tempo[i] = configuracao_inicial_json["condicaoPressao"]["tempo"][i].GetDouble();
	      }

}
/*
 * Converter o elemento "CondicaoContPocInjec" do arquivo Json do MRT em struct mrtjson::CONDCONPOCINJEC_mrt_t.
 *
 * Entrada: Value&
 * Retorna: mrtjson::CONDCONPOCINJEC_mrt_t
 */
/*
 * Converter o elemento "tempo" do arquivo Json do MRT em struct detTMAX.
 *
 * Entrada: Value& correspondente a detTMAX
 */
void LerVap::parse_tempo(Value& tempo_json) {
	/*
 "tempo": {
    "tempoFinal": 50001,
    "tempos": [0 , 1000.0 , 1060 , 29000 , 29060 , 31000 , 31060],
    "dtmax": [4 , 4 , 4 , 4 , 4 , 4 , 4]
  }
	 */
	tfinal=tempo_json["tempoFinal"].GetDouble();
	dtmaxserie.parserie=tempo_json["tempos"].Size();
	dtmaxserie.tempo = new double[dtmaxserie.parserie];
	dtmaxserie.valor = new double[dtmaxserie.parserie];

	for (int i = 0; i < dtmaxserie.parserie; i++) {
		dtmaxserie.tempo[i] = tempo_json["tempos"][i].GetDouble();
		dtmaxserie.valor[i] = tempo_json["dtmax"][i].GetDouble();
	}
	dtmax = dtmaxserie.valor[0];

}

/*
 * Converter o elemento "material" do arquivo Json do MRT em struct material.
 *
 * Entrada: Value& correspondente ao material
 */
void LerVap::parse_materiais(Value& material_json) {
	/*
	 "material": [
	 {
	 "id": 0,
	 "rotulo": "aco1",
	 "condutividade": 55.0,
	 "calorEspecifico": 465.0,
	 "rho": 7833.0,
	 "tipo": 0,
	 "visc": 0.0
	 }
	 ]
	 */
// obter o tamanho do elemento do Json
	nmaterial = material_json.Size();
// caso o tamanho do elemento seja maior que zero
	if (nmaterial > 0) {
		// vetor de materiais
		mat = new materialVap[nmaterial];
		// loop para carga do vetor do JSON
		for (int i = 0; i < nmaterial; i++) {
			// de-para do elemento "material" do json para o struct material
			mat[i].tipo = material_json[i]["tipo"].GetInt();// alteracao0
			if(mat[i].tipo<2){// alteracao2
			  mat[i].cond = material_json[i]["condutividade"].GetDouble();
			  mat[i].cp = material_json[i]["calorEspecifico"].GetDouble();
			  mat[i].rho = material_json[i]["rho"].GetDouble();
			  //mat[i].tipo = material_json[i]["tipo"].GetInt();// alteracao0
			  mat[i].visc = material_json[i]["visc"].GetDouble();
			}// alteracao0
			else{// alteracao0
				  mat[i].cond =0.;
				  mat[i].cp =0.;
				  mat[i].rho = 0.;
				  mat[i].visc = 0.;
			}// alteracao0

		}
	}
}
/*
 * Converter o elemento "secaoTransversal" do arquivo Json do MRT em struct material.
 *
 * Entrada: Value& correspondente a secão transversal / corte
 */
void LerVap::parse_corte(Value& corte_json) {
	/*
	 "sessaoTransversal": [
	 {
	 "id": 0,
	 "rotulo": "corte1",
	 "anular": false,
	 "diametroExterno": 0.12573,
	 "diametroInterno": 0.12573,
	 "rugosidade": 0.0001,
	 "camadas": [{"diametro": 0.1397,"idMaterial": 0,"discretizacao": 1}]
	 }
	 ]
	 */
// obter o tamanho do elemento do Json
	ncorte = corte_json.Size();
// caso o tamanho do elemento seja maior que zero
	if (ncorte > 0) {
		// vetor de seções transversais
		corte = new cortedutoVap[ncorte];
		// loop para carga do vetor do JSON
		for (int i = 0; i < ncorte; i++) {
			// de-para do elemento do json para o struct
			corte[i].anul = corte_json[i]["anular"].GetBool();
			corte[i].a = corte_json[i]["diametroExterno"].GetDouble();
			corte[i].b = corte_json[i]["diametroInterno"].GetDouble();
			corte[i].rug = corte_json[i]["rugosidade"].GetDouble();
			// obter vetor de camadas da seção transversal
			corte[i].ncam = corte_json[i]["camadas"].Size();
			// alocar memória para os elementos das camadas
			corte[i].diam = new double[corte[i].ncam];
			corte[i].indmat = new int[corte[i].ncam];
			corte[i].discre = new int[corte[i].ncam];
			// percorrer a lista de camadas da seção transversal
			for (int j = 0; j < corte[i].ncam; j++) {
				corte[i].diam[j] =
						corte_json[i]["camadas"][j]["diametro"].GetDouble();
				corte[i].discre[j] =
						corte_json[i]["camadas"][j]["discretizacao"].GetInt();
				corte[i].indmat[j] =
						corte_json[i]["camadas"][j]["idMaterial"].GetInt();
			}
		}
	}
}
/*
 * Converter os elementos "dutosProducao" e "dutosServico" do arquivo Json do MRT em struct detduto.
 *
 * Entrada: Value& correspondente à fluidos produção
 */
void LerVap::parse_unidades_producao(
		Value& dutos_producao_json) {

// loop para carga do vetor de unidades de produção do JSON
	for (int i = 0; i < nunidadep; i++) {
		// de-para do elemento "dutosProducao" do json para o struct detduto
		duto[i].ang = dutos_producao_json[i]["angulo"].GetDouble();
		duto[i].servico = 0;
		duto[i].indcorte = dutos_producao_json[i]["idCorte"].GetInt();
	}

	/*
	 * UNIDADEP
	 */

	unidadeP = new detalhaPVap[nunidadep];
	ncelp = 0;
// loop para carga do vetor de unidades de produção do JSON
	for (int i = 0; i < nunidadep; i++) {
		// de-para do elemento "dutosProducao" do json para o struct detalhaP
		unidadeP[i].ind = i;
		unidadeP[i].duto = i;
		unidadeP[i].formacDuto=0;
		unidadeP[i].lito=0;//alteracao2
		if(dutos_producao_json[i].HasMember("formacao") && dutos_producao_json[i]["formacao"]==true){//alteracao2
			unidadeP[i].formacDuto=1;
			if(nform>1 && dutos_producao_json[i].HasMember("Litologia"))unidadeP[i].lito=dutos_producao_json[i]["Litologia"].GetInt();
		}//alteracao2
		unidadeP[i].ambext=0;// alteracao0
		if(dutos_producao_json[i].HasMember("AmbienteExterno"))unidadeP[i].ambext=dutos_producao_json[i]["AmbienteExterno"].GetInt();// alteracao0
		if (i == 0)
			unidadeP[i].Lini = 0.;
		else
			unidadeP[i].Lini = unidadeP[i - 1].Lini + unidadeP[i - 1].comp;

		unidadeP[i].dirconv =
				dutos_producao_json[i]["direcaoConveccao"].GetInt();
		unidadeP[i].acopcol =
				dutos_producao_json[i]["acoplamentoTermico"].GetInt();

		unidadeP[i].ncel = 0;
		for (int j = 0; j < dutos_producao_json[i]["discretizacao"].Size();
				j++) {
			unidadeP[i].ncel +=
					dutos_producao_json[i]["discretizacao"][j]["nCelulas"].GetInt();
		}
		// quantidade de células total da unidade de produção
		ncelp += unidadeP[i].ncel;
		// vetor de comprimentos de cada célula desta unidade de produção
		unidadeP[i].dx = new double[unidadeP[i].ncel];
		int ndx;
		double tempdx;
		int totaliza = 0;
		// percorre o vetor de discretizações
		for (int kontadiscre = 0;
				kontadiscre < dutos_producao_json[i]["discretizacao"].Size();
				kontadiscre++) {
			ndx =
					dutos_producao_json[i]["discretizacao"][kontadiscre]["nCelulas"].GetInt();
			tempdx =
					dutos_producao_json[i]["discretizacao"][kontadiscre]["comprimento"].GetDouble();
			for (int j = totaliza; j < totaliza + ndx; j++)
				unidadeP[i].dx[j] = tempdx;
			totaliza += ndx;
		}
		// comprimento da unidade
		unidadeP[i].comp = 0.;
		for (int j = 0; j < unidadeP[i].ncel; j++)
			unidadeP[i].comp += unidadeP[i].dx[j];

		// tamanho do vetor das condições iniciais
		unidadeP[i].nponts = dutos_producao_json[i]["condicoesIniciais"]["compInter"].Size();
		unidadeP[i].dxVar = new double[unidadeP[i].nponts];
		// percorrer vetor de pontos no comprimento do duto para interpolação
		for (int j = 0; j < unidadeP[i].nponts; j++)
			unidadeP[i].dxVar[j] =
					dutos_producao_json[i]["condicoesIniciais"]["compInter"][j].GetDouble();

		unidadeP[i].var = new double*[11];
		for (int j = 0; j < 11; j++)
			unidadeP[i].var[j] = new double[unidadeP[i].nponts];
		for (int j = 0; j < unidadeP[i].nponts; j++) {// alteracao2

				unidadeP[i].var[0][j] =
						dutos_producao_json[i]["condicoesIniciais"]["temp"][j].GetDouble();
				unidadeP[i].var[1][j] =
						dutos_producao_json[i]["condicoesIniciais"]["pressao"][j].GetDouble();
				unidadeP[i].var[2][j] =
						dutos_producao_json[i]["condicoesIniciais"]["holdup"][j].GetDouble();
				unidadeP[i].var[3][j] =
						dutos_producao_json[i]["condicoesIniciais"]["uls"][j].GetDouble();
				unidadeP[i].var[4][j] =
						dutos_producao_json[i]["condicoesIniciais"]["ugs"][j].GetDouble();

			unidadeP[i].var[5][j] =
					dutos_producao_json[i]["condicoesIniciais"]["tempExterna"][j].GetDouble();
			if(unidadeP[i].formacDuto==0){// alteracao2

				  unidadeP[i].var[6][j] =
			    	dutos_producao_json[i]["condicoesIniciais"]["velExterna"][j].GetDouble();
				  if(unidadeP[i].ambext==0){
			      unidadeP[i].var[7][j] =
					dutos_producao_json[i]["condicoesIniciais"]["kExterna"][j].GetDouble();
			      unidadeP[i].var[8][j] =
					dutos_producao_json[i]["condicoesIniciais"]["calorEspecificoExterno"][j].GetDouble();
			      unidadeP[i].var[9][j] =
					dutos_producao_json[i]["condicoesIniciais"]["rhoExterno"][j].GetDouble();
			      unidadeP[i].var[10][j] =
					dutos_producao_json[i]["condicoesIniciais"]["viscExterna"][j].GetDouble();
				  }
				  else{
					  unidadeP[i].var[6][j] =100.;
					  unidadeP[i].var[7][j] =0.;
					  unidadeP[i].var[8][j] =0.;
					  unidadeP[i].var[9][j] =0.;
					  unidadeP[i].var[10][j] =0.;
				  }
			}
			else{
				  unidadeP[i].var[6][j] =0.;
				  unidadeP[i].var[7][j] =0.;
				  unidadeP[i].var[8][j] =0.;
				  unidadeP[i].var[9][j] =0.;
				  unidadeP[i].var[10][j] =0.;
			}
		}
	}
}

/*
 * Converter o elemento "ipr" do arquivo Json do MRT em struct ipr.
 *
 * Entrada: Value& correspondente a dutos de serviço
 */
void LerVap::parse_ipr(Value& ipr_json) {
	/*
	 "ipr": [
	 {
	 "id": 0,
	 "comprimentoMedido": 10.0,
	 "pressaoEstatica": [240.0],
	 "tempoPressaoEstatica": [0.0],
	 "temperaturas": [70.0],
	 "tempoTemperaturas":[0.0],
	 "ip": [20.0,20.0],
	 "tempoip": [0.0,1000.0],
	 "jp": [20.0,20.0],
	 "tempojp": [0.0,1000.0],
	 "indFluidoPro": 0
	 }
	 ],
	 */

// tamanho do vetor ipr
	nipr = ipr_json.Size();
	if (nipr > 0) {
		// declarar o vetor ipr
		IPRS = new detIPRVap[nipr];
		// loop para parse das estruturas do ipr
		for (int i = 0; i < nipr; i++) {
			IPRS[i].comp = ipr_json[i]["comprimentoMedido"].GetDouble();
			if(referencia==0)IPRS[i].comp=compTotal-IPRS[i].comp;//alteracao2
			double lverif = IPRS[i].comp;
			IPRS[i].indcel = buscaIndiceMeioP(lverif);
			// parse do vetor de pressões estáticas
			IPRS[i].seriep = ipr_json[i]["pressaoEstatica"].Size();
			IPRS[i].pres = new double[IPRS[i].seriep];
			IPRS[i].tpres = new double[IPRS[i].seriep];
			// loop para parse do vetor de pressões estáticas
			for (int j = 0; j < IPRS[i].seriep; j++) {
				IPRS[i].pres[j] = ipr_json[i]["pressaoEstatica"][j].GetDouble();
				IPRS[i].tpres[j] =
						ipr_json[i]["tempoPressaoEstatica"][j].GetDouble();
			}
			// parse do vetor de temperaturas
			IPRS[i].seriet = ipr_json[i]["temperaturas"].Size();
			IPRS[i].temp = new double[IPRS[i].seriet];
			IPRS[i].ttemp = new double[IPRS[i].seriet];
			// loop para parse do vetor de temperaturas
			for (int j = 0; j < IPRS[i].seriet; j++) {
				IPRS[i].temp[j] = ipr_json[i]["temperaturas"][j].GetDouble();
				IPRS[i].ttemp[j] =
						ipr_json[i]["tempoTemperaturas"][j].GetDouble();
			}
			// parse do vetor de ip
			// parse do vetor de jp
			IPRS[i].seriejp = ipr_json[i]["ii"].Size();
			IPRS[i].jp = new double[IPRS[i].seriejp];
			IPRS[i].tjp = new double[IPRS[i].seriejp];
			// loop para parse do vetor de jp / ii
			for (int j = 0; j < IPRS[i].seriejp; j++) {
				IPRS[i].jp[j] = ipr_json[i]["ii"][j].GetDouble();
				IPRS[i].tjp[j] = ipr_json[i]["tempoii"][j].GetDouble();
			}
		}
	}
}


/*
 * Converter o elemento "fonteLiquido" do arquivo Json do MRT em struct .
 *
 * Entrada: Value& correspondente à fonte de liquido
 */
void LerVap::parse_fonte_liquido(Value& fonte_liquido_json) {
	/*
	 "fonteLiquido": [
	 {
	 "comprimentoMedido": 2010.0,
	 "temperatura": [40.0,40.0,40.0,40.0,40.0],
	 "beta": [0.0,0.0,1.0,1.0,0.0],
	 "vazaoliquido": [0.0,0.0,1000.0,1000.0,0.0],
	 "tempo": [0.0,15000.0,15060.0,16060.0,16120.0],
	 "indiFluidoPro": 0
	 }
	 ]
	 */

// tamanho do vetor fonte_liquido
	if(!fonte_liquido_json.Empty())	ninjliq = fonte_liquido_json.Size();
	if (ninjliq > 0) {
		int iIni=0;
			//ninjliq++;
			// declarar o vetor fontel

		// loop para parse das estruturas do fontel
		for (int i = 0; i < ninjliq-iIni; i++) {
			fontel[i+iIni].comp =
					fonte_liquido_json[i]["comprimentoMedido"].GetDouble();
			if(referencia==0)fontel[i+iIni].comp =compTotal-fontel[i+iIni].comp;//alteracao2
			double lverif = fontel[i+iIni].comp;
			fontel[i+iIni].posicP = buscaIndiceMeioP(lverif);

			fontel[i+iIni].parserie = fonte_liquido_json[i]["tempo"].Size();
			fontel[i+iIni].temp = new double[fontel[i+iIni].parserie];
			fontel[i+iIni].vazmas = new double[fontel[i+iIni].parserie];
			fontel[i+iIni].tempo = new double[fontel[i+iIni].parserie];

			// parse does vetores
			for (int j = 0; j < fontel[i+iIni].parserie; j++) {
				fontel[i+iIni].temp[j] =
						fonte_liquido_json[i]["temperatura"][j].GetDouble();
				fontel[i+iIni].vazmas[j] =
						fonte_liquido_json[i]["vazaoliquido"][j].GetDouble();
				fontel[i+iIni].tempo[j] =
						fonte_liquido_json[i]["tempo"][j].GetDouble();
			}
		}
	}
}
/* Converter os elementos "bcs" do arquivo Json do MRT em struct bcs.
 *
 * Entrada: Value& correspondente a bcs.
 */
void LerVap::parse_bcs(Value& bcs_json) {
	/*
	 "bcs": [
	 {
	 "comprimentoMedido": 100.0,
	 "tempo": [0, 500 , 501 , 7000 , 7005 , 30060, 30120 ],
	 "frequencia": [ 60, 60, 60, 60, 0, 0, 60 ],
	 "vazao": [ 0 , 1500 , 3200 , 4000 , 4760 , 5500 ],
	 "head": [ 53.5 , 47 , 39 , 34 , 26 , 17 ],
	 "potencia": [ 1.2 , 1.3 , 1.5 , 1.5 , 1.5 , 1.4 ],
	 "eficiencia": [ 0 , 40 , 64 , 68 , 64 , 50 ],
	 "freqref": 60,
	 "nestag": 60
	 }
	 ]
	 */

	nbcs = bcs_json.Size();
	if (nbcs > 0) {
		bcs = new detBCSVap[nbcs];
		for (int i = 0; i < nbcs; i++) {
			bcs[i].comp = bcs_json[i]["comprimentoMedido"].GetDouble();
			if(referencia==0)bcs[i].comp=compTotal-bcs[i].comp;//alteracao2
			double lverif = bcs[i].comp;
			bcs[i].posicP = buscaIndiceFrontP(lverif);

			bcs[i].parserie = bcs_json[i]["tempo"].Size();
			bcs[i].tempo = new double[bcs[i].parserie];
			bcs[i].freq = new double[bcs[i].parserie];
			for (int j = 0; j < bcs[i].parserie; j++) {
				bcs[i].tempo[j] = bcs_json[i]["tempo"][j].GetDouble();
				bcs[i].freq[j] = bcs_json[i]["frequencia"][j].GetDouble();
			}

			bcs[i].ncurva = bcs_json[i]["vazao"].Size();
			bcs[i].vaz = new double[bcs[i].ncurva];
			bcs[i].head = new double[bcs[i].ncurva];
			bcs[i].power = new double[bcs[i].ncurva];
			bcs[i].efic = new double[bcs[i].ncurva];
			for (int j = 0; j < bcs[i].ncurva; j++) {
				bcs[i].vaz[j] = bcs_json[i]["vazao"][j].GetDouble();
				bcs[i].head[j] = bcs_json[i]["head"][j].GetDouble();
				bcs[i].power[j] = bcs_json[i]["potencia"][j].GetDouble();
				bcs[i].efic[j] = bcs_json[i]["eficiencia"][j].GetDouble();
			}
			bcs[i].freqref = bcs_json[i]["freqref"].GetDouble();
			bcs[i].nestag = bcs_json[i]["nestag"].GetInt();
			if(bcs_json[i].HasMember("EficienciaMotor"))bcs[i].eficM=bcs_json[i]["EficienciaMotor"].GetDouble();
			else bcs[i].eficM=100.;
			if(bcs_json[i].HasMember("FrequenciaMinima"))bcs[i].freqMinima=bcs_json[i]["FrequenciaMinima"].GetDouble();
			else bcs[i].freqMinima=0.;
		}
	}
}
/*
 * Converter os elementos "bombaVolumetrica" do arquivo Json do MRT em struct bvol.
 *
 * Entrada: Value& correspondente a bombaVolumetrica.
 */
void LerVap::parse_bomba_volumetrica(
		Value& bomba_volumetrica_json) {
	/*
	 "bombaVolumetrica": [
	 {
	 "comprimentoMedido": 4020.0,
	 "tempo": [0.0,500.0,560.0,7000.0,7005.0,30500.0,30560.0],
	 "frequencia": [60.0,60.0,60.0,60.0,0.0,0.0,60.0],
	 "capacidade": 2000.0,
	 "fatorpoli": 1.4
	 }
	 ]
	 */

	nbvol = bomba_volumetrica_json.Size();
	if (nbvol > 0) {
		bvol = new detBVOLVap[nbvol];
		for (int i = 0; i < nbvol; i++) {
			bvol[i].comp =
					bomba_volumetrica_json[i]["comprimentoMedido"].GetDouble();
			if(referencia==0)bvol[i].comp=compTotal-bvol[i].comp;//alteracao2
			double lverif = bvol[i].comp;
			bvol[i].posicP = buscaIndiceFrontP(lverif);

			bvol[i].parserie = bomba_volumetrica_json[i]["tempo"].Size();
			bvol[i].tempo = new double[bvol[i].parserie];
			bvol[i].freq = new double[bvol[i].parserie];
			for (int j = 0; j < bvol[i].parserie; j++) {
				bvol[i].tempo[j] =
						bomba_volumetrica_json[i]["tempo"][j].GetDouble();
				bvol[i].freq[j] =
						bomba_volumetrica_json[i]["frequencia"][j].GetDouble();
			}

			bvol[i].capacidade = bomba_volumetrica_json[i]["capacidade"].GetDouble();
			bvol[i].capacidade/=1e6;
			bvol[i].npoli = bomba_volumetrica_json[i]["fatorpoli"].GetDouble();
		}

	}
}
/*
 * Converter os elementos "deltaPressao" do arquivo Json do MRT em struct dpreq.
 *
 * Entrada: Value& correspondente a deltaPressao.
 */
void LerVap::parse_delta_pressao(Value& delta_pressao_json) {
	/*
	 "deltaPressao": [
	 {
	 "comprimentoMedido": 4020.0,
	 "deltaPressao": [50.0,50.0,0.0,0.0,50.0],
	 "tempo": [0.0,7000.0,7005.0,30500.0,30560.0]
	 }
	 ]
	 */

	ndpreq = delta_pressao_json.Size();
	if (ndpreq > 0) {
		dpreq = new detDPREQVap[ndpreq];
		for (int i = 0; i < ndpreq; i++) {
			dpreq[i].comp =
					delta_pressao_json[i]["comprimentoMedido"].GetDouble();
			if(referencia==0)dpreq[i].comp=compTotal-dpreq[i].comp;//alteracao2
			double lverif = dpreq[i].comp;
			dpreq[i].posicP = buscaIndiceFrontP(lverif);

			dpreq[i].parserie = delta_pressao_json[i]["tempo"].Size();
			dpreq[i].tempo = new double[dpreq[i].parserie];
			dpreq[i].dp = new double[dpreq[i].parserie];
			for (int j = 0; j < dpreq[i].parserie; j++) {
				dpreq[i].tempo[j] =
						delta_pressao_json[i]["tempo"][j].GetDouble();
				dpreq[i].dp[j] =
						delta_pressao_json[i]["deltaPressao"][j].GetDouble();
			}
		}
	}
}
/*
 * Converter os elementos "master1" do arquivo Json do MRT em struct master1.
 *
 * Entrada: Value& correspondente a master1.
 */
void LerVap::parse_master1(Value& master1_json) {
	/*
	 "master1": {
	 "comprimentoMedido": 1980.0,
	 "abertura": [1.0,1.0,0.0,0.0,1.0],
	 "tempo": [0.0,7000.0,7065.0,30000.0,30060.0]
	 }
	 */

	master1.razareaativ=0.5;//alteracao4
	if(master1_json.HasMember("RazaoAreaAtiva")) master1.razareaativ=master1_json["RazaoAreaAtiva"].GetDouble();//alteracao4

	master1.parserie = master1_json["abertura"].Size();
	master1.abertura = new double[master1.parserie];
	master1.tempo = new double[master1.parserie];
	master1.comp = master1_json["comprimentoMedido"].GetDouble();
	if(referencia==0)master1.comp=compTotal-master1.comp;//alteracao2

	double lverif = master1.comp;
	master1.posic = buscaIndiceFrontP(lverif);

	for (int i = 0; i < master1.parserie; i++) {
		master1.abertura[i] = master1_json["abertura"][i].GetDouble();
		master1.tempo[i] = master1_json["tempo"][i].GetDouble();
	}

	eventoabre = 0;
	eventofecha = 0;
	Tevento = new double[master1.parserie];
	Teventof = new double[master1.parserie];
}
/*
 * Converter os elementos "separador" do arquivo Json do MRT em struct PSEP.
 *
 * Entrada: Value& correspondente a separador.
 */
void LerVap::parse_separador(Value& separador_json) {
	/*
	 "separador": {
	 "pressao": [10.0,10.0,6.0,6.0,10.0],
	 "tempo": [0.0,500.0,560.0,4000.0,4060.0]
	 }
	 */
	psep.parserie = separador_json["pressao"].Size();
	psep.pres = new double[psep.parserie];
	psep.tempo = new double[psep.parserie];

	for (int i = 0; i < psep.parserie; i++) {
		psep.pres[i] = separador_json["pressao"][i].GetDouble();
		psep.tempo[i] = separador_json["tempo"][i].GetDouble();
	}

}
/*
 * Converter os elementos "chokeSup" do arquivo Json do MRT em struct chokep.
 *
 * Entrada: Value& correspondente a chokeSup
 */
void LerVap::parse_chokeSup(Value& chokeSup_json) {
	/*
	 "chokeSup": {
	 "coeficienteDescarga": 0.84,
	 "modelo": 0,
	 "abertura": [0.3,0.3,0.0,,0.0,1.0,1.0,1.0,1.0,0.3],
	 "tempo": [0.0,7000.0,7065.0,8000.0,8065.0,8500.0,8565.0,30000.0,30065.0]
	 }
	 */
	chokep.parserie = chokeSup_json["abertura"].Size();
	chokep.abertura = new double[chokep.parserie];
	chokep.tempo = new double[chokep.parserie];
	chokep.cd = chokeSup_json["coeficienteDescarga"].GetDouble();
	chokep.modelo = chokeSup_json["modelo"].GetInt();

	for (int i = 0; i < chokep.parserie; i++) {
		chokep.abertura[i] = chokeSup_json["abertura"][i].GetDouble();
		chokep.tempo[i] = chokeSup_json["tempo"][i].GetDouble();
	}

}

/*
 * Converter os elementos "pig" do arquivo Json do MRT em struct pig.
 *
 * Entrada: Value& correspondente a pig.
 */
void LerVap::parse_pig(Value& pig_json) {
	/*
	 "pig": [
	 {
	 "lancador": 2420.0,
	 "recebedor": 7940.0,
	 "tempo": 25100.0
	 }
	 ],
	 */

	npig = pig_json.Size();
	if (npig > 0) {
		pig = new detPigVap[npig];
		for (int i = 0; i < npig; i++) {
			pig[i].compL = pig_json[i]["lancador"].GetDouble();
			if(referencia==0)pig[i].compL=compTotal-pig[i].compL;//alteracao2
			pig[i].compR = pig_json[i]["recebedor"].GetDouble();
			if(referencia==0)pig[i].compR=compTotal-pig[i].compR;//alteracao2
			pig[i].tempo = pig_json[i]["tempo"].GetDouble();

			double lverif = pig[i].compL;
			pig[i].lanca = buscaIndiceMeioP(lverif);
			lverif = pig[i].compR;
			pig[i].receb = buscaIndiceMeioP(lverif);
			pig[i].razarea=0.;//alteracao2
			if(pig_json[i].HasMember("Folga de Area (razao)"))pig[i].razarea=pig_json[i]["Folga de Area (razao)"].GetDouble();//alteracao2
			pig[i].delpres=0.;//alteracao2
			if(pig_json[i].HasMember("Delta Pressao"))pig[i].delpres=pig_json[i]["Delta Pressao"].GetDouble();//alteracao2
			pig[i].cdPig=1.;//alteracao2
			if(pig_json[i].HasMember("cd Pig"))pig[i].cdPig=pig_json[i]["cd Pig"].GetDouble();//alteracao2
		}
	}
}
/*
 * Converter os elementos "perfil_producao" do arquivo Json do MRT em struct fonte.
 *
 * Entrada: Value& correspondente a perfil_producao.
 */
void LerVap::parse_perfis_producao(Value& perfis_producao_json) {
	/*
	 "perfilProducao": {
	 "pressao": true,
	 "temperatura": true,
	 "holdup": true,
	 "bet": true,
	 "ugs": true,
	 "uls": true,
	 "ug": false,
	 "ul": false,
	 "arra": true,
	 "viscosidadeLiquido": false,
	 "viscosidadeGas": false,
	 "rhog": false,
	 "rhol": 0.5,
	 "vasaoMassicaGas": true,
	 "vasaoMassicaLiquido": true,
	 "c0": true,
	 "ud": true,
	 "RGO": true,
	 "deng": true,
	 "yco2": true,
	 "calor": false,
	 "masstrans": false,
	 "cpgas": true,
	 "cpliq": true,
	 "QLstd": true,
	 "QGstd": true,
	 "api": true,
	 "bsw": true,
	 "tempo": [0.0,1000.0,7000.0,7100.0,7500.0,8000.0,10000.0,15000.0,20000.0,30000.0,30300.0,31000.0,31500.0,32000.0,33000.0,35000.0,40000.0,45000.0,48000.0,50000.0]
	 }
	 */

	nvarprofp = 0;
	nperfisp = perfis_producao_json["tempo"].Size();
	if (nperfisp > 0) {
		profp.n = perfis_producao_json["tempo"].Size();
		profp.tempo = new double[profp.n];

		profp.pres = perfis_producao_json["pressao"].GetBool();
		if (profp.pres == 1)
			nvarprofp++;

		profp.temp = perfis_producao_json["temperatura"].GetBool();
		if (profp.temp == 1)
			nvarprofp++;

		profp.hol = perfis_producao_json["holdup"].GetBool();
		if (profp.hol == 1)
			nvarprofp++;

		profp.ugs = perfis_producao_json["ugs"].GetBool();
		if (profp.ugs == 1)
			nvarprofp++;

		profp.uls = perfis_producao_json["uls"].GetBool();
		if (profp.uls == 1)
			nvarprofp++;

		profp.ug = perfis_producao_json["ug"].GetBool();
		if (profp.ug == 1)
			nvarprofp++;

		profp.ul = perfis_producao_json["ul"].GetBool();
		if (profp.ul == 1)
			nvarprofp++;

		profp.arra = perfis_producao_json["arra"].GetBool();
		if (profp.arra == 1)
			nvarprofp++;

		profp.viscl = perfis_producao_json["viscosidadeLiquido"].GetBool();
		if (profp.viscl == 1)
			nvarprofp++;

		profp.viscg = perfis_producao_json["viscosidadeGas"].GetBool();
		if (profp.viscg == 1)
			nvarprofp++;

		profp.rhog = perfis_producao_json["rhog"].GetBool();
		if (profp.rhog == 1)
			nvarprofp++;

		profp.rhol = perfis_producao_json["rhol"].GetBool();
		if (profp.rhol == 1)
			nvarprofp++;

		profp.masg = perfis_producao_json["vazaoMassicaGas"].GetBool();
		if (profp.masg == 1)
			nvarprofp++;

		profp.masl = perfis_producao_json["vazaoMassicaLiquido"].GetBool();
		if (profp.masl == 1)
			nvarprofp++;

		profp.c0 = perfis_producao_json["c0"].GetBool();
		if (profp.c0 == 1)
			nvarprofp++;

		profp.ud = perfis_producao_json["ud"].GetBool();
		if (profp.ud == 1)
			nvarprofp++;

		profp.calor = perfis_producao_json["calor"].GetBool();
		if (profp.calor == 1)
			nvarprofp++;

		profp.masstrans = perfis_producao_json["masstrans"].GetBool();
		if (profp.masstrans == 1)
			nvarprofp++;

		profp.cpg = perfis_producao_json["cpgas"].GetBool();
		if (profp.cpg == 1)
			nvarprofp++;

		profp.cpl = perfis_producao_json["cpliq"].GetBool();
		if (profp.cpl == 1)
			nvarprofp++;


		for (int i = 0; i < profp.n; i++) {
			profp.tempo[i] = perfis_producao_json["tempo"][i].GetDouble();
		}

	}
}

/*
 * Converter os elementos "tendencia_producao" do arquivo Json do MRT em struct trendp.
 *
 * Entrada: Value& correspondente a tendencia_producao.
 */
void LerVap::parse_tendencia_producao(
		Value& tendencia_producao_json) {

	/*
	 "tendP": [
	 {
	 "celula": 2,
	 "dt": 20.0,
	 "pressao": true,
	 "temperatura": true,
	 "holdup": true,
	 "bet": true,
	 "ugs": true,
	 "uls": true,
	 "ug": true,
	 "ul": true,
	 "arra": true,
	 "viscosidadeLiquido": true,
	 "viscosidadeGas": true,
	 "rhog": true,
	 "rhol": true,
	 "vazaoMassicaGas": true,
	 "vazaoMassicaLiquido": true,
	 "c0": true,
	 "ud": true,
	 "RGO": true,
	 "deng": true,
	 "yco2": true,
	 "calor": true,
	 "masstrans": true,
	 "QLstd": true,
	 "QGstd": true,
	 "api": true,
	 "bsw": true,
	 "rotulo": "Fundo"
	 }
	 */

	ntendp = tendencia_producao_json.Size();
	if (ntendp > 0) {
		trendp = new detTRENDPVap[ntendp];
		nvartrendp = new int[ntendp];
		for (int i = 0; i < ntendp; i++)
			nvartrendp[i] = 0;
		for (int i = 0; i < ntendp; i++) {
			trendp[i].posic = tendencia_producao_json[i]["celula"].GetInt()-1;
			if(trendp[i].posic<0)trendp[i].posic=0;
			trendp[i].dt = tendencia_producao_json[i]["dt"].GetDouble();

			trendp[i].pres = tendencia_producao_json[i]["pressao"].GetBool();
			if (trendp[i].pres == 1)
				nvartrendp[i]++;

			trendp[i].temp =
					tendencia_producao_json[i]["temperatura"].GetBool();
			if (trendp[i].temp == 1)
				nvartrendp[i]++;

			trendp[i].hol = tendencia_producao_json[i]["holdup"].GetBool();
			if (trendp[i].hol == 1)
				nvartrendp[i]++;

			trendp[i].ugs = tendencia_producao_json[i]["ugs"].GetBool();
			if (trendp[i].ugs == 1)
				nvartrendp[i]++;

			trendp[i].uls = tendencia_producao_json[i]["uls"].GetBool();
			if (trendp[i].uls == 1)
				nvartrendp[i]++;

			trendp[i].ug = tendencia_producao_json[i]["ug"].GetBool();
			if (trendp[i].ug == 1)
				nvartrendp[i]++;

			trendp[i].ul = tendencia_producao_json[i]["ul"].GetBool();
			if (trendp[i].ul == 1)
				nvartrendp[i]++;

			trendp[i].arra = tendencia_producao_json[i]["arra"].GetBool();
			if (trendp[i].arra == 1)
				nvartrendp[i]++;

			trendp[i].viscl =
					tendencia_producao_json[i]["viscosidadeLiquido"].GetBool();
			if (trendp[i].viscl == 1)
				nvartrendp[i]++;

			trendp[i].viscg =
					tendencia_producao_json[i]["viscosidadeGas"].GetBool();
			if (trendp[i].viscg == 1)
				nvartrendp[i]++;

			trendp[i].rhog = tendencia_producao_json[i]["rhog"].GetBool();
			if (trendp[i].rhog == 1)
				nvartrendp[i]++;

			trendp[i].rhol = tendencia_producao_json[i]["rhol"].GetBool();
			if (trendp[i].rhol == 1)
				nvartrendp[i]++;

			trendp[i].masg =
					tendencia_producao_json[i]["vazaoMassicaGas"].GetBool();
			if (trendp[i].masg == 1)
				nvartrendp[i]++;

			trendp[i].masl =
					tendencia_producao_json[i]["vazaoMassicaLiquido"].GetBool();
			if (trendp[i].masl == 1)
				nvartrendp[i]++;

			trendp[i].c0 = tendencia_producao_json[i]["c0"].GetBool();
			if (trendp[i].c0 == 1)
				nvartrendp[i]++;

			trendp[i].ud = tendencia_producao_json[i]["ud"].GetBool();
			if (trendp[i].ud == 1)
				nvartrendp[i]++;

			trendp[i].calor = tendencia_producao_json[i]["calor"].GetBool();
			if (trendp[i].calor == 1)
				nvartrendp[i]++;

			trendp[i].masstrans =
					tendencia_producao_json[i]["masstrans"].GetBool();
			if (trendp[i].masstrans == 1)
				nvartrendp[i]++;

			trendp[i].rotulo = tendencia_producao_json[i]["rotulo"].GetString();
		}
	}
}

/*
 * Converter os elementos "perfis_trans_p_json" do arquivo Json do MRT em struct proftransp.
 *
 * Entrada: Value& correspondente a perfis_trans_p_json.
 */
void LerVap::parse_perfis_trans_producao(	Value& perfis_trans_producao_json) {
	/*
	 "perfisTransP": {
	 "posicao": [1,150],
	 "tempo": [0.0,1000.0,3000.0,7000.0,9500.0,10000.0,10500.0,15000.0,20000.0,30000.0,30010.0,30050.0,30100.0,31000.0,31500.0,32000.0,35000.0,40000.0,45000.0,50000.0]
	 }
	 */

	nperfistransp = perfis_trans_producao_json["posicao"].Size();
	if (nperfistransp > 0) {
		proftransp.posic = new int[nperfistransp];
		proftransp.n = perfis_trans_producao_json["tempo"].Size();
		proftransp.tempo = new double[proftransp.n];

		for (int i = 0; i < nperfistransp; i++) {
			proftransp.posic[i] =
					perfis_trans_producao_json["posicao"][i].GetDouble();
		}

		for (int i = 0; i < proftransp.n; i++) {
			proftransp.tempo[i] =
					perfis_trans_producao_json["tempo"][i].GetDouble();
		}
	}
}
/*
 * Converter os elementos "tendencia_trans_p_json" do arquivo Json do MRT em struct trendtransg.
 *
 * Entrada: Value& correspondente a tendencia_trans_p_json.
 */
void LerVap::parse_tendencia_trans_producao(
		Value& tendencia_trans_producao_json) {
	/*
	 "tendTransP": [
	 {
	 "celula": 2,
	 "camada": 2,
	 "discretizacao": 1,
	 "dt": 20.0,
	 "rotulo": "FundoTemp"
	 }
	 ]
	 */
	ntendtransp = tendencia_trans_producao_json.Size();
	if (ntendtransp > 0) {
		trendtransp = new detTRENDTransVap[ntendtransp];
		for (int i = 0; i < ntendtransp; i++) {
			trendtransp[i].posic =
					tendencia_trans_producao_json[i]["celula"].GetInt()-1;
			if(trendtransp[i].posic<0)trendtransp[i].posic=0;
			trendtransp[i].camada =
					tendencia_trans_producao_json[i]["camada"].GetInt()-1;
			if(trendtransp[i].camada<0)trendtransp[i].camada=0;
			trendtransp[i].discre =
					tendencia_trans_producao_json[i]["discretizacao"].GetInt();
			trendtransp[i].dt =
					tendencia_trans_producao_json[i]["dt"].GetDouble();
			trendtransp[i].rotulo =
					tendencia_trans_producao_json[i]["rotulo"].GetString();
		}
	}

}

/*
 * Converter os elementos "tela" do arquivo Json do MRT em struct tela.
 *
 * Entrada: Value& correspondente a tela.
 */
void LerVap::parse_tela(Value& tela_json) {

 // "tela": [
 // {
 //"coluna": 1,
 // "celula": 0,
 // "variavel": 1
 // }
 // ]

	ntela = tela_json.Size();
	if (ntela > 0) {
		tela = new detTelaVap[ntela];
		for (int i = 0; i < ntela; i++) {
			tela[i].col = tela_json[i]["coluna"].GetBool();
			tela[i].posic =tela_json[i]["celula"].GetInt();
			tela[i].var =tela_json[i]["variavel"].GetInt();
		}
	}

 }


void LerVap::lerArq() {

	FILE *fp = fopen(impfile.c_str(), "r");
	//FILE *fp = fopen("/home/bwz1/teste2.json", "r");
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document elementoRaiz;
	elementoRaiz.ParseStream(is);
	fclose(fp);
//   StringBuffer buffer;
//   Writer<StringBuffer> writer(buffer);
//   elementoRaiz.Accept(writer);
//   cout << "..." << buffer.GetString() << "...\n";

   assert(elementoRaiz.HasMember("configuracaoInicial"));

   if (elementoRaiz.HasMember("configuracaoInicial")){
		parse_configuracao_inicial(elementoRaiz["configuracaoInicial"]);
	} else {
		exit(-1);
	}


// obter os valores dos atributos do elemento "tempo" do Json

		 tfinal = 0.;
		 dtmaxserie.parserie = 1;
		 dtmaxserie.tempo = new double[dtmaxserie.parserie];
		 dtmaxserie.valor = new double[dtmaxserie.parserie];
		 dtmaxserie.tempo[0] = 0;
		 dtmaxserie.valor[0] = 1.;
		 dtmax = dtmaxserie.valor[0];


// obter os valores dos atributos do elemento "material" do Json
	parse_materiais(elementoRaiz["material"]);
// obter os valores dos atributos do elemento "secaoTransversal" do Json
	parse_corte(elementoRaiz["secaoTransversal"]);
// calcular a quantidade de dutos de produção e serviço
	nunidadep = elementoRaiz["dutosProducao"].Size();

	duto = new detdutoVap[nduto];

// parse unidades de produção
	parse_unidades_producao(elementoRaiz["dutosProducao"]);

// gerar células de produção
	compTotal=0;//alteracao2
	celp = new detcelpVap[ncelp];
	int tempncel = 0;
	anmP = 0 * unidadeP[0].dx[0];
	int para = 0;
	for (int i = 0; i < nunidadep; i++) {
		double posiccentro = 0.;
		double posicfronteira = 0.;
		int iniSegF = 0;
		int iniSegC = 0;
		for (int j = tempncel; j < (tempncel + unidadeP[i].ncel); j++) {
			int kontauni = j - tempncel;
			celp[j].duto = unidadeP[i].duto;
			celp[j].dirconv = unidadeP[i].dirconv;
			celp[j].acopcol = unidadeP[i].acopcol;
			celp[j].dx = unidadeP[i].dx[kontauni];
			compTotal+=celp[j].dx;//alteracao2
			celp[j].formacCel=unidadeP[i].formacDuto;
			celp[j].lito=unidadeP[i].lito;//alteracao2
			celp[j].ambext=unidadeP[i].ambext;// alteracao0
			posiccentro = posicfronteira + celp[j].dx / 2.;
			posicfronteira += celp[j].dx;
			iniSegF = buscaIndiceUnidade(iniSegF, i, posicfronteira);
			iniSegC = buscaIndiceUnidade(iniSegC, i, posiccentro);
			double compC = (unidadeP[i].dxVar[iniSegC + 1]
					- unidadeP[i].dxVar[iniSegC]) * unidadeP[i].comp;
			double compF = (unidadeP[i].dxVar[iniSegF + 1]
					- unidadeP[i].dxVar[iniSegF]) * unidadeP[i].comp;
			celp[j].temp = unidadeP[i].var[0][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[0][iniSegC + 1]
									- unidadeP[i].var[0][iniSegC]) / compC;
			celp[j].pres = unidadeP[i].var[1][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[1][iniSegC + 1]
									- unidadeP[i].var[1][iniSegC]) / compC;
			celp[j].hol = unidadeP[i].var[2][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[2][iniSegC + 1]
									- unidadeP[i].var[2][iniSegC]) / compC;
			celp[j].uls = unidadeP[i].var[3][iniSegF]
					+ posicfronteira
							* (unidadeP[i].var[3][iniSegF + 1]
									- unidadeP[i].var[3][iniSegF]) / compF;
			celp[j].ugs = unidadeP[i].var[4][iniSegF]
					+ posicfronteira
							* (unidadeP[i].var[4][iniSegF + 1]
									- unidadeP[i].var[4][iniSegF]) / compF;
			celp[j].textern = unidadeP[i].var[5][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[5][iniSegC + 1]
									- unidadeP[i].var[5][iniSegC]) / compC;
			celp[j].vextern = unidadeP[i].var[6][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[6][iniSegC + 1]
									- unidadeP[i].var[6][iniSegC]) / compC;
			celp[j].kextern = unidadeP[i].var[7][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[7][iniSegC + 1]
									- unidadeP[i].var[7][iniSegC]) / compC;
			celp[j].cpextern = unidadeP[i].var[8][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[8][iniSegC + 1]
									- unidadeP[i].var[8][iniSegC]) / compC;
			celp[j].rhoextern = unidadeP[i].var[9][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[9][iniSegC + 1]
									- unidadeP[i].var[9][iniSegC]) / compC;
			celp[j].viscextern = unidadeP[i].var[10][iniSegC]
					+ posiccentro
							* (unidadeP[i].var[10][iniSegC + 1]
									- unidadeP[i].var[10][iniSegC]) / compC;
		}
		tempncel += unidadeP[i].ncel;
	}

	celp[ncelp-1].profundiF=0;//alteracao2
	for(int i=ncelp-1; i>=0; i--){//alteracao2
		int iDu=celp[i].duto;
		celp[i].profundiM=celp[i].profundiF+0.5*celp[i].dx*sinl(duto[iDu].ang);
		if(i>0)celp[i-1].profundiF=celp[i].profundiF+celp[i].dx*sinl(duto[iDu].ang);
	}//alteracao2

// parse ipr
	nipr=0;
	if (elementoRaiz.HasMember("ipr")) {
	    parse_ipr(elementoRaiz["ipr"]);
	}

	// parse fonte de liquido
	ninjliq=0;
	if((elementoRaiz.HasMember("fonteLiquido") && !elementoRaiz["fonteLiquido"].Empty())){
	   parse_fonte_liquido(elementoRaiz["fonteLiquido"]);
	}

	// parser do elemento "bcs"
	nbcs=0;
	if (elementoRaiz.HasMember("bcs") && !elementoRaiz["bcs"].Empty()) {
		parse_bcs(elementoRaiz["bcs"]);
	}

	// parser do elemento "bombaVolumetrica"
	nbvol=0;
	if (elementoRaiz.HasMember("bombaVolumetrica") && !elementoRaiz["bombaVolumetrica"].Empty()) {
		parse_bomba_volumetrica(elementoRaiz["bombaVolumetrica"]);
	}

	// parser do elemento "deltaPressao"
	ndpreq=0;
	if (elementoRaiz.HasMember("deltaPressao") && !elementoRaiz["deltaPressao"].Empty()) {
		parse_delta_pressao(elementoRaiz["deltaPressao"]);
	}

	// parse master1
	if (elementoRaiz.HasMember("master1")) {
		parse_master1(elementoRaiz["master1"]);
	}
	else{
	 	master1.parserie = 1;
		master1.abertura = new double[master1.parserie];
		master1.tempo = new double[master1.parserie];
		master1.posic = -1;
		master1.comp = -0.1;
		master1.abertura[0] = 1;
		master1.tempo[0] = 0;
		eventoabre = 0;
		eventofecha = 0;
		Tevento = new double[master1.parserie];
		Teventof = new double[master1.parserie];
	}

   // parse separador
	    parse_separador(elementoRaiz["separador"]);

	 if (elementoRaiz.HasMember("chokeSup"))parse_chokeSup(elementoRaiz["chokeSup"]);
	else{
		 chokep.parserie = 1;
		 chokep.abertura = new double[chokep.parserie];
		 chokep.tempo = new double[chokep.parserie];
		 chokep.cd = 1;
		 chokep.modelo = 0;
		 chokep.abertura[0] = 1;
		 chokep.tempo[0] = 1;
	}

	// parser do elemento "pig"
	npig=0;
	if (elementoRaiz.HasMember("pig") && !elementoRaiz["pig"].Empty()) {
		parse_pig(elementoRaiz["pig"]);
	}

	// parser do elemento "perfilProducao"
	nperfisp=0;
	if (elementoRaiz.HasMember("perfilProducao")) {
		parse_perfis_producao(elementoRaiz["perfilProducao"]);
	}

	// parser do elemento "perfilTransP"
	nperfistransp=0;
	if (elementoRaiz.HasMember("perfisTransP")) {
	   parse_perfis_trans_producao(elementoRaiz["perfisTransP"]);
	}

	// parser do elemento "tendenciaProducao"
	ntendp=0;
	if (elementoRaiz.HasMember("tendP") && !elementoRaiz["tendP"].Empty()) {
		parse_tendencia_producao(elementoRaiz["tendP"]);
	}

	// parser do elemento "tendenciaTransProducao"
	ntendtransp=0;
	if (elementoRaiz.HasMember("tendTransP") && !elementoRaiz["tendTransP"].Empty()) {
		parse_tendencia_trans_producao(elementoRaiz["tendTransP"]);
	}

	// parser do elemento "Tela"
	ntela=0;
	if (elementoRaiz.HasMember("tela") && !elementoRaiz["tela"].Empty()) {
		parse_tela(elementoRaiz["tela"]);
	}
}



int LerVap::buscaIndiceMeioP(double lverif) {

double lteste = 0.;
int kontacel = 0;
while ((lteste + celp[kontacel].dx/2.) < lverif) {
	lteste += celp[kontacel].dx;
	kontacel++;
	if (kontacel >= ncelp)
		//exit(-1);
		return ncelp-1;
}
if (kontacel == 0)
	return kontacel;
else {
	if (fabsl(lteste + celp[kontacel].dx / 2. - lverif)
			< fabsl(lteste - celp[kontacel - 1].dx / 2. - lverif))
		return kontacel;
	else
		return kontacel - 1;
}
}

int LerVap::buscaIndiceFrontP(double lverif) {

double lteste = 0.;
int kontacel = 0;
while ((lteste + celp[kontacel].dx) < lverif) {
	lteste += celp[kontacel].dx;
	kontacel++;
	if (kontacel >= ncelp)
		//exit(-1);
	return ncelp-1;
}
if (kontacel == 0)
	return kontacel;
else {
	if (fabsl(lteste + celp[kontacel].dx - lverif)
			< fabsl(lteste - celp[kontacel - 1].dx - lverif))
		return kontacel;
	else
		return kontacel - 1;
}
}


int LerVap::buscaIndiceUnidade(int iniSeg, int nuni,
	double comp) {

while (comp
		- (unidadeP[nuni].dxVar[iniSeg + 1] - unidadeP[nuni].dxVar[0])
				* unidadeP[nuni].comp > 1e-5) {
	iniSeg++;
//if(fabsl(comp-(unidadeP[nuni].dxVar[iniSeg+1]-unidadeP[nuni].dxVar[0]))<1e-5 && iniSeg>=unidadeP[nuni].nponts-1)
	if (iniSeg >= unidadeP[nuni].nponts - 1)
		exit(-1);
}
return iniSeg;
}


void LerVap::listaevento(int inic,int extrem) {
if (nipr > 0) {
	for (int i = 0; i < nipr; i++) {
		for (int j = 0; j < IPRS[i].seriep; j++)
			evento.push_back(IPRS[i].tpres[j]);
		for (int j = 0; j < IPRS[i].seriet; j++)
			evento.push_back(IPRS[i].ttemp[j]);
		for (int j = 0; j < IPRS[i].seriejp; j++)
			evento.push_back(IPRS[i].tjp[j]);
	}
}

if (ninjliq > 0) {
	for (int i = 0; i < ninjliq; i++)
		for (int j = 0; j < fontel[i].parserie; j++)
			evento.push_back(fontel[i].tempo[j]);
}
for (int i = 0; i < master1.parserie; i++)
	evento.push_back(master1.tempo[i]);

for (int i = 0; i < psep.parserie; i++)
	evento.push_back(psep.tempo[i]);
for (int i = 0; i < chokep.parserie; i++)
	evento.push_back(chokep.tempo[i]);
if (nbcs > 0) {
	for (int i = 0; i < nbcs; i++)
		for (int j = 0; j < bcs[i].parserie; j++)
			evento.push_back(bcs[i].tempo[j]);
}
if (nbvol > 0) {
	for (int i = 0; i < nbvol; i++)
		for (int j = 0; j < bvol[i].parserie; j++)
			evento.push_back(bvol[i].tempo[j]);
}
if (ndpreq > 0) {
	for (int i = 0; i < ndpreq; i++)
		for (int j = 0; j < dpreq[i].parserie; j++)
			evento.push_back(dpreq[i].tempo[j]);
}
if (npig > 0) {
	for (int i = 0; i < npig; i++)
		evento.push_back(pig[i].tempo);
}
//if(nperfisp>0)for(int i=0; i<profp.n; i++)evento.push_back(profp.tempo[i]);
//if(nperfisg>0 && lingas>0)for(int i=0; i<profg.n; i++)evento.push_back(profg.tempo[i]);
if (evento.size() > 0)
	sort(evento.begin(), evento.end());
if (evento.size() > 1) {
	double replica = evento[0];
	for (int i = 1; i < evento.size(); i++) {
		if (fabsl(replica - evento[i]) < 1e-5) {
			evento.erase(evento.begin() + i);
			i--;
		} else
			replica = evento[i];
	}
}

}

void LerVap::geraevento(int inic,int extrem) {
LogEventoVap e;
if (nipr > 0) {
	for (int i = 0; i < nipr; i++) {
		if (IPRS[i].seriep > 1) {
			for (int j = 0; j < IPRS[i].seriep; j++) {
				if (j < IPRS[i].seriep - 1) {
					if (fabsl(IPRS[i].pres[j] - IPRS[i].pres[j + 1]) > 1e-15) {
						e.instante = IPRS[i].tpres[j];
						e.duracao = IPRS[i].tpres[j + 1] - IPRS[i].tpres[j];
						if (IPRS[i].pres[j] > IPRS[i].pres[j + 1])
							e.descricao = "Diminuindo Pressao Estatica";
						else
							e.descricao = "Aumentando Pressao Estatica";
						e.estIni = IPRS[i].pres[j];
						e.estFim = IPRS[i].pres[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
		if (IPRS[i].seriet > 1) {
			for (int j = 0; j < IPRS[i].seriet; j++) {
				if (j < IPRS[i].seriet - 1) {
					if (fabsl(IPRS[i].temp[j] - IPRS[i].temp[j + 1]) > 1e-15) {
						e.instante = IPRS[i].ttemp[j];
						e.duracao = IPRS[i].ttemp[j + 1] - IPRS[i].ttemp[j];
						if (IPRS[i].temp[j] > IPRS[i].temp[j + 1])
							e.descricao =
									"Diminuindo Temperatura de Reservatorio";
						else
							e.descricao =
									"Aumentando Temperatura de Reservatorio";
						e.estIni = IPRS[i].temp[j];
						e.estFim = IPRS[i].temp[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
      	if (IPRS[i].seriejp > 1) {
			for (int j = 0; j < IPRS[i].seriejp; j++) {
				if (j < IPRS[i].seriejp - 1) {
					if (fabsl(IPRS[i].jp[j] - IPRS[i].jp[j + 1]) > 1e-15) {
						e.instante = IPRS[i].tjp[j];
						e.duracao = IPRS[i].tjp[j + 1] - IPRS[i].tjp[j];
						if (IPRS[i].jp[j] > IPRS[i].jp[j + 1])
							e.descricao = "Diminuindo indice de Injetividade";
						else
							e.descricao = "Aumentando indice de Injetividade";
						e.estIni = IPRS[i].jp[j];
						e.estFim = IPRS[i].jp[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
}
if (ninjliq > 0) {
	for (int i = 0; i < ninjliq; i++) {
		if (fontel[i].parserie > 1) {
			for (int j = 0; j < fontel[i].parserie; j++) {
				if (j < fontel[i].parserie - 1) {
					if (fabsl(fontel[i].vazmas[j] - fontel[i].vazmas[j + 1])
							> 1e-15) {
						e.instante = fontel[i].tempo[j];
						e.duracao = fontel[i].tempo[j + 1] - fontel[i].tempo[j];
						if (fontel[i].vazmas[j] > fontel[i].vazmas[j + 1])
							e.descricao = "Diminuindo Vazao de Liquido, Fonte";
						else
							e.descricao = "Aumentando Vazao de Liquido, Fonte";
						e.estIni = fontel[i].vazmas[j];
						e.estFim = fontel[i].vazmas[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
}
if (master1.parserie > 1) {
	for (int j = 0; j < master1.parserie; j++) {
		if (j < master1.parserie - 1) {
			if (fabsl(master1.abertura[j] - master1.abertura[j + 1]) > 1e-15) {
				e.instante = master1.tempo[j];
				e.duracao = master1.tempo[j + 1] - master1.tempo[j];
				if (master1.abertura[j] > master1.abertura[j + 1])
					e.descricao = "Fechando Master1";
				else
					e.descricao = "Abrindo Master1";
				e.estIni = master1.abertura[j];
				e.estFim = master1.abertura[j + 1];
				logevento.push_back(e);
				logeventoEstat.push_back(e);
			}
		}
	}
}
if (psep.parserie > 1) {
	for (int j = 0; j < psep.parserie; j++) {
		if (j < psep.parserie - 1) {
			if (fabsl(psep.pres[j] - psep.pres[j + 1]) > 1e-15) {
				e.instante = psep.tempo[j];
				e.duracao = psep.tempo[j + 1] - psep.tempo[j];
				if (psep.pres[j] > psep.pres[j + 1])
					e.descricao = "Diminuindo Pressao do Separador";
				else
					e.descricao = "Aumentando Pressao do Separador";
				e.estIni = psep.pres[j];
				e.estFim = psep.pres[j + 1];
				logevento.push_back(e);
				logeventoEstat.push_back(e);
			}
		}
	}

if (chokep.parserie > 1) {
	for (int j = 0; j < chokep.parserie; j++) {
		if (j < chokep.parserie - 1) {
			if (fabsl(chokep.abertura[j + 1] - chokep.abertura[j]) > 1e-15) {
				e.instante = chokep.tempo[j];
				e.duracao = chokep.tempo[j + 1] - chokep.tempo[j];
				if (chokep.abertura[j] > chokep.abertura[j + 1])
					e.descricao = "Fechando Choke de Superficie";
				else
					e.descricao = "Abrindo Choke de Superficie";
				e.estIni = chokep.abertura[j];
				e.estFim = chokep.abertura[j + 1];
				logevento.push_back(e);
				logeventoEstat.push_back(e);
			}
		}
	}
}
}
if (nbcs > 0) {
	for (int i = 0; i < nbcs; i++) {
		if (bcs[i].parserie > 1) {
			for (int j = 0; j < bcs[i].parserie; j++) {
				if (j < bcs[i].parserie - 1) {
					if (fabsl(bcs[i].freq[j + 1] - bcs[i].freq[j]) > 1e-15) {
						e.instante = bcs[i].tempo[j];
						e.duracao = bcs[i].tempo[j + 1] - bcs[i].tempo[j];
						if (bcs[i].freq[j] > bcs[i].freq[j + 1])
							e.descricao = "Desacelerando BCS";
						else
							e.descricao = "Acelerando BCS";
						e.estIni = bcs[i].freq[j];
						e.estFim = bcs[i].freq[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
}
if (nbvol > 0) {
	for (int i = 0; i < nbvol; i++) {
		if (bvol[i].parserie > 1) {
			for (int j = 0; j < bvol[i].parserie; j++) {
				if (j < bvol[i].parserie - 1) {
					if (fabsl(bvol[i].freq[j + 1] - bvol[i].freq[j]) > 1e-15) {
						e.instante = bvol[i].tempo[j];
						e.duracao = bvol[i].tempo[j + 1] - bvol[i].tempo[j];
						if (bvol[i].freq[j] > bvol[i].freq[j + 1])
							e.descricao = "Desacelerando Bomba Volumetrica";
						else
							e.descricao = "Acelerando Bomba Volumetrica";
						e.estIni = bvol[i].freq[j];
						e.estFim = bvol[i].freq[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
}
if (ndpreq > 0) {
	for (int i = 0; i < ndpreq; i++) {
		if (dpreq[i].parserie > 1) {
			for (int j = 0; j < dpreq[i].parserie; j++) {
				if (j < dpreq[i].parserie - 1) {
					if (fabsl(dpreq[i].dp[j] - dpreq[i].dp[j + 1]) > 1e-15) {
						e.instante = dpreq[i].tempo[j];
						e.duracao = dpreq[i].tempo[j + 1] - dpreq[i].tempo[j];
						if (dpreq[i].dp[j] > dpreq[i].dp[j + 1])
							e.descricao =
									"Diminuindo Delta P (pressao requerida)";
						else
							e.descricao =
									"Aumentando Delta P (pressao requerida)";
						e.estIni = dpreq[i].dp[j];
						e.estFim = dpreq[i].dp[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
}
if (npig > 0) {
	for (int i = 0; i < npig; i++) {
		e.instante = pig[i].tempo;
		e.duracao = 0.;
		e.descricao = "Lancando PIG";
		e.estIni = 0.;
		e.estFim = 0.;
		logevento.push_back(e);
		logeventoEstat.push_back(e);
	}
}
if (nperfisp > 0) {
	for (int i = 0; i < nperfisp; i++) {
		e.instante = profp.tempo[i];
		e.duracao = 0.;
		e.descricao = "Gravando Perfil Linha de Producao";
		e.estIni = 0.;
		e.estFim = 0.;
		logevento.push_back(e);
	}
}
if (logevento.size() > 0)
	sort(logevento.begin(), logevento.end());
if (logeventoEstat.size() > 0)
	sort(logeventoEstat.begin(), logeventoEstat.end());

ostringstream saidaLog;
saidaLog << "LogEvento" << ".dat";
string tmp = saidaLog.str();
ofstream escreveIni(tmp.c_str(), ios_base::out);
/* escreveIni<<" Tempo ; ";
 escreveIni<<" Duracao ; ";
 escreveIni<<" Estado Inicial ; ";
 escreveIni<<" Estado Final ; ";
 escreveIni<<" Evento ; "<<"\n";*/
escreveIni.close();

int neve = logeventoEstat.size();
ostringstream saidaExt;
saidaExt << "EventosExternos" << ".dat";
tmp = saidaExt.str();
ofstream escreveExt(tmp.c_str(), ios_base::out);
escreveExt << " Tempo ; ";
escreveExt << " Duracao ; ";
escreveExt << " Estado Inicial ; ";
escreveExt << " Estado Final ; ";
escreveExt << " Evento ; " << "\n";
if (neve > 0) {
	for (int i = 0; i < neve; i++) {
		escreveExt << logeventoEstat[i].instante << " ; ";
		escreveExt << logeventoEstat[i].duracao << " ; ";
		escreveExt << logeventoEstat[i].estIni << " ; ";
		escreveExt << logeventoEstat[i].estFim << " ; ";
		escreveExt << logeventoEstat[i].descricao << " ; " << "\n";
	}
}
escreveExt.close();

}

void LerVap::geraduto() {
dutosMRT = new DadosGeo [nduto];

for (int i = 0; i < nduto; i++) {
	int cam = corte[duto[i].indcorte].ncam;
	int icor = duto[i].indcorte;

	double* vk;
	vk = new double[cam];
	for (int j = 0; j < cam; j++)
		vk[j] = mat[corte[icor].indmat[j]].cond;
	double* vcp;
	vcp = new double[cam];
	for (int j = 0; j < cam; j++)
		vcp[j] = mat[corte[icor].indmat[j]].cp;
	double* vrhoc;
	vrhoc = new double[cam];
	for (int j = 0; j < cam; j++)
		vrhoc[j] = mat[corte[icor].indmat[j]].rho;
	double* vvisc;
	vvisc = new double[cam];
	for (int j = 0; j < cam; j++)
		vvisc[j] = mat[corte[icor].indmat[j]].visc;
	double* vbeta;
	vbeta = new double[cam];
	for (int j = 0; j < cam; j++)
		vbeta[j] = 1.;
	int* vtipomat;
	vtipomat = new int[cam];
	for (int j = 0; j < cam; j++)
		vtipomat[j] = mat[corte[icor].indmat[j]].tipo;

	dutosMRT[i] = DadosGeo(corte[icor].a, corte[icor].b,
			duto[i].ang, corte[icor].rug, corte[icor].anul, corte[icor].ncam,
			vk, corte[icor].diam, vcp, vrhoc, vvisc,vbeta, vtipomat);
	delete[] vk;
	delete[] vcp;
	delete[] vrhoc;
	delete[] vvisc;
	delete[] vbeta;
	delete[] vtipomat;
}
}


void LerVap::novatrans(TransCal& transfer,
	DadosGeo& geom, int* discre, double tint,
	double text, double vext, double vint, long dircon,
	double dt, double ki, double cpi, double rhoi,
	double visci, double ke, double cpe, double rhoe,
	double visce, int vformacao,int litologia, int ambext,double pres) {// alteracao0//alteracao2

int* vncamada;
vncamada = new int[geom.ncamadas];
for (int i = 0; i < geom.ncamadas; i++) {
	if (geom.tipomat[i] != 0)
		vncamada[i] = 1;
	else
		vncamada[i] = discre[i];
}
double* vdrcamada;
vdrcamada = new double[geom.ncamadas];
for (int i = 0; i < geom.ncamadas; i++) {
	if (i > 0)
		vdrcamada[i] = 0.5 * (geom.diamC[i] - geom.diamC[i - 1]) / vncamada[i];
	else
		vdrcamada[0] = 0.5 * (geom.diamC[0] - geom.a) / vncamada[0];
}
double** vTcamada;
vTcamada = new double*[geom.ncamadas];
for (int i = 0; i < geom.ncamadas; i++) {
	vTcamada[i] = new double[vncamada[i] + 1];
}

for (int i = 0; i < geom.ncamadas; i++) {
	for (int j = 0; j <= vncamada[i]; j++)
		//vTcamada[i][j] = tint;// alteracao0
		vTcamada[i][j] = text;
}
int perm = -1;

if(ambext==1){
	int para;
	para=0;
}

int kL=litologia;//alteracao2


transfer = TransCal(vg1dSP,geom, perm, vncamada, vdrcamada, vTcamada,
		tint, text, 0., vint, vext, dircon, dt, ki, cpi, rhoi, visci, ki, cpe,
		rhoe, visce, vformacao,ambext,formacPoc[kL].tempo,formacPoc[kL].cond,
		formacPoc[kL].cp,formacPoc[kL].rho,pres);//alteracao2// alteracao0
if (equilterm == 1)
	transfer.transperm();
delete vncamada;
delete vdrcamada;
for (int i = 0; i < geom.ncamadas; i++)
	delete[] vTcamada[i];
delete[] vTcamada;

}

void LerVap::geracelp(CelVap* celula) {

//int ColunaAnulaIni=99;
//int ColunaAnulaFim=9;
for (int i = 0; i < ncelp; i++) {

	int iduto = celp[i].duto;
	int idutoL;
	if (i > 0)
		idutoL = celp[i - 1].duto;
	else
		idutoL = celp[i].duto;
	int idutoR;
	if (i < ncelp - 1)
		idutoR = celp[i + 1].duto;
	else
		idutoR = celp[i].duto;
	int icorte = duto[iduto].indcorte;
	double vdx;
	double vdxL;
	double vdxR;
	double vazio;
	double vazioL;
	double vazioR;
	double bet;
	double betL;
	double betR;
	double bet0;
	double betN;
	double betP;
	double pmed = celp[i].pres;
	double tmed = celp[i].temp;
	double pmedL;
	double tmedL;
	double pmedR;
	double tmedR;
	double presaux;
	double presauxL;
	double presauxR;
	double taux;
	double tauxL;
	double tauxR;
	double razdx;
	double razdxL;
	double razdxR;
	double vQL;
	double vQG;
	double vMC;
	double vMliq;
	double vML;
	double vMliqL;
	double vMR;
	double vMliqR;
	double vrhol;
	double vrhog;
    ProVap flup;
	if (i > 1 && i < ncelp - 1) {
		vdx = celp[i].dx;
		vdxL = celp[i - 1].dx;
		vdxR = celp[i + 1].dx;
		razdx = celp[i].dx / (celp[i].dx + celp[i - 1].dx);
		razdxL = celp[i - 1].dx / (celp[i - 1].dx + celp[i - 2].dx);
		razdxR = celp[i + 1].dx / (celp[i + 1].dx + celp[i].dx);
		pmedL = celp[i - 1].pres;
		tmedL = celp[i - 1].temp;
		pmedR = celp[i + 1].pres;
		tmedR = celp[i + 1].temp;
		presaux = razdx * celp[i].pres + (1. - razdx) * celp[i - 1].pres;
		taux = razdx * celp[i].temp + (1. - razdx) * celp[i - 1].temp;
		presauxL = razdxL * celp[i - 1].pres + (1. - razdxL) * celp[i - 2].pres;
		tauxL = razdxL * celp[i - 1].temp + (1. - razdxL) * celp[i - 2].temp;
		presauxR = razdxR * celp[i + 1].pres + (1. - razdxR) * celp[i].pres;
		tauxR = razdxR * celp[i + 1].temp + (1. - razdxR) * celp[i].temp;
		vQL = celp[i].uls * dutosMRT[iduto].area;
		vQG = celp[i].ugs * dutosMRT[iduto].area;
		vazio = 1 - celp[i].hol;
		vazioL = 1 - celp[i - 1].hol;
		vazioR = 1 - celp[i + 1].hol;
		vrhol = flup.MasEspLiq(presaux, taux,TEMP);
		vrhog = flup.MasEspGas(presaux, taux,TEMP);
		vMC = vrhol * vQL + vrhog * vQG;
		vMliq = vrhol * vQL;
		vML = ((flup.MasEspLiq(presauxL, tauxL,TEMP)) * celp[i - 1].uls
				+ flup.MasEspGas(presauxL, tauxL,TEMP) * celp[i - 1].ugs)
				* dutosMRT[idutoL].area;
		vMliqL = (flup.MasEspLiq(presauxL, tauxL,TEMP)) * celp[i - 1].uls
				* dutosMRT[idutoL].area;
		vMR = (flup.MasEspLiq(presauxR, tauxR,TEMP)* celp[i + 1].uls)
				* dutosMRT[idutoR].area;
		vMliqR = (flup.MasEspLiq(presauxR, tauxR,TEMP)) * celp[i + 1].uls
				* dutosMRT[idutoR].area;
	} else if (i == 1) {
		vdx = celp[i].dx;
		vdxL = celp[i - 1].dx;
		vdxR = celp[i + 1].dx;
		razdx = celp[i].dx / (celp[i].dx + celp[i - 1].dx);
		razdxL = 1.;
		razdxR = celp[i + 1].dx / (celp[i + 1].dx + celp[i].dx);
		pmedL = celp[i - 1].pres;
		tmedL = celp[i - 1].temp;
		pmedR = celp[i + 1].pres;
		tmedR = celp[i + 1].temp;
		presaux = razdx * celp[i].pres + (1. - razdx) * celp[i - 1].pres;
		taux = razdx * celp[i].temp + (1. - razdx) * celp[i - 1].temp;
		presauxL = razdxL * celp[i - 1].pres;
		tauxL = razdxL * celp[i - 1].temp;
		presauxR = razdxR * celp[i + 1].pres + (1. - razdxR) * celp[i].pres;
		tauxR = razdxR * celp[i + 1].temp + (1. - razdxR) * celp[i].temp;
		vQL = celp[i].uls * dutosMRT[iduto].area;
		vQG = celp[i].ugs * dutosMRT[iduto].area;
		vazio = 1 - celp[i].hol;
		vazioL = 1 - celp[i - 1].hol;
		vazioR = 1 - celp[i + 1].hol;
		bet0 = betL;
		if (vQL <= 0)
			bet0 = bet;
		betN = betL;
		betP = bet;
		if (celp[i + 1].uls <= 0)
			betP = betR;
		vrhol = flup.MasEspLiq(presaux, taux,TEMP);
		vrhog = flup.MasEspGas(presaux, taux,TEMP);
		vMC = vrhol * vQL + vrhog * vQG;
		vMliq = vrhol * vQL;
		vML = ((flup.MasEspLiq(presauxL, tauxL,TEMP)) * celp[i - 1].uls
				+ flup.MasEspGas(presauxL, tauxL,TEMP) * celp[i - 1].ugs)
				* dutosMRT[idutoL].area;
		vMliqL = (flup.MasEspLiq(presauxL, tauxL,TEMP)) * celp[i - 1].uls
				* dutosMRT[idutoL].area;
		vMR = ((flup.MasEspLiq(presauxR, tauxR,TEMP)) * celp[i + 1].uls
				+ flup.MasEspGas(presauxR, tauxR,TEMP) * celp[i + 1].ugs)
				* dutosMRT[idutoR].area;
		vMliqR = (flup.MasEspLiq(presauxR, tauxR,TEMP)) * celp[i + 1].uls
				* dutosMRT[idutoR].area;
	} else if (i == 0) {
		vdx = celp[i].dx;
		vdxL = celp[i].dx;
		vdxR = celp[i + 1].dx;
		razdx = 1.;
		razdxL = 1.;
		razdxR = celp[i + 1].dx / (celp[i + 1].dx + celp[i].dx);
		pmedL = celp[i].pres;
		tmedL = celp[i].temp;
		pmedR = celp[i + 1].pres;
		tmedR = celp[i + 1].temp;
		presaux = razdx * celp[i].pres;
		taux = razdx * celp[i].temp;
		presauxL = razdxL * celp[i].pres;
		tauxL = razdxL * celp[i].temp;
		presauxR = razdxR * celp[i + 1].pres + (1. - razdxR) * celp[i].pres;
		tauxR = razdxR * celp[i + 1].temp + (1. - razdxR) * celp[i].temp;
		vQL = celp[i].uls * dutosMRT[iduto].area;
		vQG = celp[i].ugs * dutosMRT[iduto].area;
		vazio = 1 - celp[i].hol;
		vazioL = 1 - celp[i].hol;
		vazioR = 1 - celp[i + 1].hol;
		bet0 = betL;
		if (vQL <= 0)
			bet0 = bet;
		betN = betL;
		betP = bet;
		if (celp[i + 1].uls <= 0)
			betP = betR;
		vrhol = flup.MasEspLiq(presaux, taux,TEMP);
		vrhog = flup.MasEspGas(presaux, taux,TEMP);
		vMC = vrhol * vQL + vrhog * vQG;
		vMliq = vrhol * vQL;
		vML = ((flup.MasEspLiq(presauxL, tauxL,TEMP)) * 0.
				+ flup.MasEspGas(presauxL,tauxL ,TEMP) * 0.)
				* dutosMRT[idutoL].area;
		vMliqL = ( flup.MasEspLiq(presauxL, tauxL,TEMP)) * 0
				* dutosMRT[idutoL].area;
		vMR = ((flup.MasEspLiq(presauxR, tauxR,TEMP)) * celp[i + 1].uls
				+ flup.MasEspGas(presauxR, tauxR,TEMP) * celp[i + 1].ugs)
				* dutosMRT[idutoR].area;
		vMliqR = (flup.MasEspLiq(presauxR, tauxR,TEMP)) * celp[i + 1].uls
				* dutosMRT[idutoR].area;
	} else {
		vdx = celp[i].dx;
		vdxL = celp[i - 1].dx;
		vdxR = celp[i].dx;
		razdx = celp[i].dx / (celp[i].dx + celp[i - 1].dx);
		razdxL = celp[i - 1].dx / (celp[i - 1].dx + celp[i - 2].dx);
		razdxR = 0.;
		pmedL = celp[i - 1].pres;
		tmedL = celp[i - 1].temp;
		pmedR = celp[i].pres;
		tmedR = celp[i].temp;
		presaux = razdx * celp[i].pres + (1. - razdx) * celp[i - 1].pres;
		taux = razdx * celp[i].temp + (1. - razdx) * celp[i - 1].temp;
		presauxL = razdxL * celp[i - 1].pres + (1. - razdxL) * celp[i - 2].pres;
		tauxL = razdxL * celp[i - 1].temp + (1. - razdxL) * celp[i - 2].temp;
		presauxR = (1. - razdxR) * celp[i].pres;
		tauxR = (1. - razdxR) * celp[i].temp;
		vQL = celp[i].uls * dutosMRT[iduto].area;
		vQG = celp[i].ugs * dutosMRT[iduto].area;
		vazio = 1 - celp[i].hol;
		vazioL = 1 - celp[i - 1].hol;
		vazioR = 1 - celp[i].hol;
		if (vQL <= 0)
			bet0 = bet;
		betN = betL;
		betP = bet;
		if (celp[i + 1].uls <= 0)
			betP = betR;
		vrhol = flup.MasEspLiq(presaux, taux,TEMP);
		vrhog = flup.MasEspGas(presaux, taux,TEMP);
		vMC = vrhol * vQL + vrhog * vQG;
		vMliq = vrhol * vQL;
		vML = ((flup.MasEspLiq(presauxL, tauxL,TEMP)) * celp[i - 1].uls
				+ flup.MasEspGas(presauxL, tauxL,TEMP) * celp[i - 1].ugs)
				* dutosMRT[idutoL].area;
		vMliqL = (flup.MasEspLiq(presauxL, tauxL,TEMP)) * celp[i - 1].uls
				* dutosMRT[idutoL].area;
		vMR = ((flup.MasEspLiq(presauxR, tauxR,TEMP)) * 0
				+ flup.MasEspGas(presauxR, tauxR,TEMP) * 0)
				* dutosMRT[idutoR].area;
		vMliqR = (flup.MasEspLiq(presauxR, tauxR,TEMP)) * 0
				* dutosMRT[idutoR].area;
	}
	double jmix;
	jmix = celp[i].ugs + celp[i].uls;
	TransCal entrada;

	double vhol = celp[i].hol;
	vrhol = flup.MasEspLiq(celp[i].pres, celp[i].temp,TEMP);
	double vrhomix = celp[i].hol * vrhol + (1 - celp[i].hol) * vrhog;
	double vkint = ( flup.CondLiq(celp[i].pres, celp[i].temp)) * vhol
			+ flup.CondGas(celp[i].pres, celp[i].temp) * (1 - vhol);
	double vcpint;
	if (fabsl(jmix) > 1e-10)
		vcpint = ((flup.CPlFunc(celp[i].pres, celp[i].temp))
				* fabsl(celp[i].uls) * vrhol
				+ flup.CPgFunc(celp[i].pres, celp[i].temp)
						* fabsl(celp[i].ugs) * vrhog)
				/ (fabsl(celp[i].uls) * vrhol + fabsl(celp[i].ugs) * vrhog);
	else
		vcpint = (flup.CPlFunc(celp[i].pres, celp[i].temp)) * vhol
				+ flup.CPgFunc(celp[i].pres, celp[i].temp)
						* (1. - vhol);
	double vviscint = (flup.ViscLiq(celp[i].pres, celp[i].temp)) * vhol
			+ flup.ViscGas(celp[i].pres, celp[i].temp)
					* (1. - vhol);

	if(i>120){
		int para;
		para=0;
	}

	novatrans(entrada, dutosMRT[iduto], corte[icorte].discre, celp[i].temp,
			celp[i].textern, celp[i].vextern, jmix, celp[i].dirconv, dtmax,
			vkint, vcpint, vrhomix, vviscint, celp[i].kextern, celp[i].cpextern,
			celp[i].rhoextern, celp[i].viscextern, celp[i].formacCel,celp[i].lito,celp[i].ambext,
			1.0197+celp[i].profundiM*1000*9.81/98066.52);
	celula[i] = CelVap(vg1dSP,dutosMRT[idutoL], dutosMRT[iduto],
			dutosMRT[idutoR], flup, tmedL, tmed, tmedR, tmed,
			pmedL, pmed, pmedR, vML, vMC, vMR, vMliqL, vMliq, vMliqR, vazioL,
			vazio, vazioR, vdxL, vdx, vdxR, dtmax, i, entrada);

}

//for(int i=ncelp-2; i<ncelp;i++) celula[i].flui.tab=0;

}

void LerVap::geraipr(CelVap* celula) {
//IPR iprMRT();

for (int i = 0; i < nipr; i++) {
	IPRVap iprMRT(IPRS[i].pres[0], IPRS[i].temp[0],
			IPRS[i].jp[0]);
	celula[IPRS[i].indcel].acsr.tipo = 11;
	celula[IPRS[i].indcel].acsr.iprvap = iprMRT;
	if(IPRS[i].indcel<(ncelp-1))
	celula[IPRS[i].indcel + 1].acsrL = &celula[IPRS[i].indcel].acsr;
}

}


void LerVap::gerafliqFonte(CelVap* celula) {
//InjGas injgasMRT();

for (int i = 0; i < ninjliq; i++) {
	int iposp = fontel[i].posicP;
	InjMultVap injliqMRT(fontel[i].vazmas[0], fontel[i].temp[0]);
	celula[iposp].acsr.tipo = 12;
	celula[iposp].acsr.injmvap = injliqMRT;
}

}

void LerVap::gerafBCS(CelVap* celula) {
//InjGas injgasMRT();

for (int i = 0; i < nbcs; i++) {
	int iposp = bcs[i].posicP;
	BomCentSub bcsMRT(bcs[i].ncurva, bcs[i].vaz, bcs[i].head,
			bcs[i].power, bcs[i].efic, bcs[i].freqref, bcs[i].nestag, bcs[i].eficM, bcs[i].freqMinima);
	celula[iposp].acsr.tipo = 4;
	celula[iposp].acsr.bcs = bcsMRT;
}

}

void LerVap::gerafBVOL(CelVap* celula) {
//InjGas injgasMRT();

for (int i = 0; i < nbvol; i++) {
	int iposp = bvol[i].posicP;
	BomVol bvolMRT(bvol[i].capacidade, bvol[i].freq[0],
			bvol[i].npoli);
	celula[iposp].acsr.tipo = 8;
	celula[iposp].acsr.bvol = bvolMRT;
	celula[iposp + 1].acsrL = &celula[iposp].acsr;
}

}

void LerVap::geraDPReq(CelVap* celula) {
//InjGas injgasMRT();

for (int i = 0; i < ndpreq; i++) {
	int iposp = dpreq[i].posicP;
	celula[iposp].acsr.tipo = 7;
	celula[iposp].acsr.delp = dpreq[i].dp[0];
}

}

void LerVap::geraMaster1(CelVap* celula) {

int posicM = master1.posic;
double area = celula[posicM].duto.area;
//celula[posicM].flui.tab=0;
//celula[posicM+1].flui.tab=0;
choke master(area, 1. * area);
celula[posicM].acsr.tipo = 5;
celula[posicM].acsr.chk = master;
celula[posicM + 1].acsrL = &celula[posicM].acsr;
double abre = master1.abertura[0];
for (int i = 1; i < master1.parserie; i++) {
	if (abre == 0 && master1.abertura[i] == 1) {
		Tevento[eventoabre] = master1.tempo[i - 1];
		eventoabre++;
	}
	abre = master1.abertura[i];
}
if (eventoabre == 0)
	Tevento[0] = -1000.;

double fecha = master1.abertura[0];
for (int i = 1; i < master1.parserie; i++) {
	if (fecha == 1 && master1.abertura[i] == 0) {
		Teventof[eventofecha] = master1.tempo[i - 1];
		eventofecha++;
	}
	fecha = master1.abertura[i];
}
if (eventofecha == 0)
	Teventof[0] = -1000.;

}

void LerVap::gerapresfim(double& presfim,
	double& pGSup) {

pGSup = psep.pres[0];
presfim = psep.pres[0];

}

void LerVap::gerachokesup(choke& chokeSup) {
int indduto = celp[ncelp - 1].duto;
int indcorte = duto[indduto].indcorte;
double dia = corte[indcorte].a;
//celp[ncelp].
double area = M_PI * dia * dia / 4;
chokeSup.AreaGarg = chokep.abertura[0] * area;
chokeSup.AreaTub = area;
chokeSup.cdchk = chokep.cd;
}

void LerVap::indraz(int& ind, double& raz,
	double tempo, int parserie, double* serietemp) {

for (int i = 0; i <= parserie - 1; i++) {
	if (i < parserie - 1) {
		if (tempo >= serietemp[i] && tempo < serietemp[i + 1]) {
			ind = i;
			raz = 1
					- (tempo - serietemp[i])
							/ (serietemp[i + 1] - serietemp[i]);
			break;
		}
		//else if(i==parserie-2){
		//ind=i+1;
		//raz=1;
		//}
	} else if (i == parserie - 1) {
		ind = i;
		raz = 1;
	}
}
}

void LerVap::atualiza(int inicio, int extrem,
	choke& chokeSup, CelVap* celula,
	double& pGSup, double& temperatura,
	double& presE, double& tempE,
	  double& titE, double tempo) {//alteracao4

int ind;
double raz;
int celpos = master1.posic;
indraz(ind, raz, tempo, master1.parserie, master1.tempo);
double aberinf = master1.abertura[ind];
double abersup;
if (ind < master1.parserie - 1)
	abersup = master1.abertura[ind + 1];
else
	abersup = master1.abertura[ind];
celula[celpos].acsr.chk.AreaGarg = (aberinf * raz + (1 - raz) * abersup)
		* celula[celpos].duto.area;
//if(raz<0.99999999)celula[celpos].acsr.chk.AreaGarg=abersup*celula[celpos].duto.area;

if(ConContEntrada>0){//alteracao4
	indraz(ind, raz, tempo, CCPres.parserie, CCPres.tempo);
	double inf = CCPres.pres[ind];
	double sup;
	if (ind < CCPres.parserie - 1)
		sup = CCPres.pres[ind + 1];
	else
	    sup = CCPres.pres[ind];
	presE = (inf * raz + (1 - raz) * sup);

	inf = CCPres.temperatura[ind];
	if (ind < CCPres.parserie - 1)
		sup = CCPres.temperatura[ind + 1];
	else
	    sup = CCPres.temperatura[ind];
	tempE = (inf * raz + (1 - raz) * sup);

    inf = CCPres.tit[ind];
	if (ind < CCPres.parserie - 1)
		sup = CCPres.tit[ind + 1];
	else
	    sup = CCPres.tit[ind];
	titE = (inf * raz + (1 - raz) * sup);


}


if (tempo > 7950.) {
	int para;
	para = 0;
}
indraz(ind, raz, tempo, dtmaxserie.parserie, dtmaxserie.tempo);
double Tinf = dtmaxserie.valor[ind];
double Tsup;
if (ind < dtmaxserie.parserie - 1)
	Tsup = dtmaxserie.valor[ind + 1];
else
	Tsup = dtmaxserie.valor[ind];
dtmax = Tinf * raz + (1 - raz) * Tsup;

/*indraz(ind, raz, tempo, varRGO.parserie, varRGO.tempo);
 double RGOinf=varRGO.valor[ind];
 double RGOsup;
 if(ind<varRGO.parserie-1)RGOsup=varRGO.valor[ind+1];
 else RGOsup=varRGO.valor[ind];
 trackRGO=RGOinf*raz+(1-raz)*RGOsup;*/
if (ninjliq > 0) {
	double vazinf;
	double vazsup;
	double betinf;
	double betsup;
	double tempinf;
	double tempsup;
	for (int i = 0; i < ninjliq; i++) {
		celpos = fontel[i].posicP;
		indraz(ind, raz, tempo, fontel[i].parserie, fontel[i].tempo);
		vazinf = fontel[i].vazmas[ind];
		tempinf = fontel[i].temp[ind];
		if (ind < fontel[i].parserie - 1) {
			vazsup = fontel[i].vazmas[ind + 1];
			tempsup = fontel[i].temp[ind + 1];
		} else {
			vazsup = fontel[i].vazmas[ind];
			tempsup = fontel[i].temp[ind];
		}
		celula[celpos].acsr.injl.QLiq = 1 * (vazinf * raz + (1 - raz) * vazsup);
		celula[celpos].acsr.injl.bet = 1 * (betinf * raz + (1 - raz) * betsup);
		celula[celpos].acsr.injl.temp = 1
				* (tempinf * raz + (1 - raz) * tempsup);
		if (celpos == 0 && celula[celpos].acsr.tipo==2)
			temperatura = celula[celpos].acsr.injl.temp;

	}
}

if (nbcs > 0) {
	double freqinf;
	double freqsup;
	for (int i = 0; i < nbcs; i++) {
		celpos = bcs[i].posicP;
		indraz(ind, raz, tempo, bcs[i].parserie, bcs[i].tempo);
		freqinf = bcs[i].freq[ind];
		if (ind < bcs[i].parserie - 1)
			freqsup = bcs[i].freq[ind + 1];
		else
			freqsup = bcs[i].freq[ind];
		celula[celpos].acsr.bcs.freqnova = freqinf * raz + (1 - raz) * freqsup;
	}
}

if (nbvol > 0) {
	double freqinf;
	double freqsup;
	for (int i = 0; i < nbvol; i++) {
		celpos = bvol[i].posicP;
		indraz(ind, raz, tempo, bvol[i].parserie, bvol[i].tempo);
		freqinf = bvol[i].freq[ind];
		if (ind < bvol[i].parserie - 1)
			freqsup = bvol[i].freq[ind + 1];
		else
			freqsup = bvol[i].freq[ind];
		celula[celpos].acsr.bvol.freq = freqinf * raz + (1 - raz) * freqsup;
	}
}

if (ndpreq > 0) {
	double dpinf;
	double dpsup;
	for (int i = 0; i < ndpreq; i++) {
		celpos = dpreq[i].posicP;
		indraz(ind, raz, tempo, dpreq[i].parserie, dpreq[i].tempo);
		dpinf = dpreq[i].dp[ind];
		if (ind < dpreq[i].parserie - 1)
			dpsup = dpreq[i].dp[ind + 1];
		else
			dpsup = dpreq[i].dp[ind];
		celula[celpos].acsr.delp = dpinf * raz + (1 - raz) * dpsup;
	}
}

if (npig > 0) {
	for (int i = 0; i < npig; i++) {
		if (tempo < (pig[i].tempo + 0.1)
				&& tempo > (pig[i].tempo - (*vg1dSP).localtiny)) {
			int celpos = pig[i].lanca;
			celula[celpos].estadoPig = 1;
			celula[celpos].razPig = 0.5;
			celula[celpos].razPigini = 0.5;
			celula[celpos].velPig = (celula[celpos].QL + celula[celpos].QG)
					/ celula[celpos].duto.area;
			celula[celpos].velPigini = celula[celpos].velPig;
			celula[celpos].indpig=i;//alteracao2
		}
	}
}

celpos = ncelp - 1;
indraz(ind, raz, tempo, chokep.parserie, chokep.tempo);
aberinf = chokep.abertura[ind];
if (ind < chokep.parserie - 1)
	abersup = chokep.abertura[ind + 1];
else
	abersup = chokep.abertura[ind];
chokeSup.AreaGarg = (aberinf * raz + (1 - raz) * abersup)
		* celula[celpos].duto.area;


if((*vg1dSP).chaverede==0 || extrem==1){//alteracao4
  indraz(ind, raz, tempo, psep.parserie, psep.tempo);
  double presinf = psep.pres[ind];
  double pressup;
  if (ind < psep.parserie - 1)
	pressup = psep.pres[ind + 1];
}
//pGSup=10.;

for (int i = 0; i < nipr; i++) {
	celpos = IPRS[i].indcel;

	indraz(ind, raz, tempo, IPRS[i].seriep, IPRS[i].tpres);
	double inf = IPRS[i].pres[ind];
	double sup;
	if (ind < IPRS[i].seriep - 1)
		sup = IPRS[i].pres[ind + 1];
	else
		sup = IPRS[i].pres[ind];
	celula[celpos].acsr.ipr.Pres = inf * raz + (1 - raz) * sup;

	indraz(ind, raz, tempo, IPRS[i].seriet, IPRS[i].ttemp);
	inf = IPRS[i].temp[ind];
	if (ind < IPRS[i].seriet - 1)
		sup = IPRS[i].temp[ind + 1];
	else
		sup = IPRS[i].temp[ind];
	celula[celpos].acsr.ipr.Tres = inf * raz + (1 - raz) * sup;

	indraz(ind, raz, tempo, IPRS[i].seriejp, IPRS[i].tjp);
	inf = IPRS[i].jp[ind];
	if (ind < IPRS[i].seriejp - 1)
		sup = IPRS[i].jp[ind + 1];
	else
		sup = IPRS[i].jp[ind];
	celula[celpos].acsr.ipr.ij = inf * raz + (1 - raz) * sup;
	if (celpos == 0)
		temperatura = celula[celpos].acsr.ipr.Tres;

}

}

void LerVap::imprimeProfile(CelVap* const celula,
	FullMtx<double>& flut, double tempo, int indTramo) {
if (nperfisp > 0) {
	double comp = 0.;
	double altura = 0.;
	int unidade = 0;
	for (int i = 0; i < ncelp; i++) {
		flut[i][0] = comp;
		flut[i][1] = tempo;
		int k = 2;

		if (profp.pres == 1) {
			flut[i][k] = celula[i].pres;
			k++;
		}
		if (profp.temp == 1) {
			flut[i][k] = celula[i].temp;
			k++;
		}
		if (profp.hol == 1) {
			flut[i][k] = 1. - celula[i].alf;
			k++;
		}
		if (profp.ugs == 1) {
			flut[i][k] = celula[i].QG / celula[i].duto.area;
			k++;
		}
		if (profp.uls == 1) {
			flut[i][k] = celula[i].QL / celula[i].duto.area;
			k++;
		}
		if (profp.ug == 1) {
			if (celula[i].alf > (*vg1dSP).localtiny)
				flut[i][k] = celula[i].QG
						/ (celula[i].duto.area * celula[i].alf);
			else
				flut[i][k] = 0;
			k++;
		}
		if (profp.ul == 1) {
			if (celula[i].alf < 1. - (*vg1dSP).localtiny)
				flut[i][k] = celula[i].QL
						/ (celula[i].duto.area * (1. - celula[i].alf));
			else
				flut[i][k] = 0;
			k++;
		}
		if (profp.arra == 1) {
			flut[i][k] = celula[i].arranjo;
			k++;
		}
		if (profp.viscl == 1) {
			flut[i][k] = celula[i].flui.ViscLiq(celula[i].pres,
					celula[i].temp);
			k++;
		}
		if (profp.viscg == 1) {
			flut[i][k] = celula[i].flui.ViscGas(celula[i].pres, celula[i].temp);
			k++;
		}
		if (profp.rhog == 1) {
			flut[i][k] = celula[i].flui.MasEspGas(celula[i].pres,
					celula[i].temp,TEMP);
			//flut[i][k] = celula[i].flui.CalorGas(celula[i].pres,celula[i].temp);
			k++;
		}
		if (profp.rhol == 1) {
			flut[i][k] = celula[i].flui.MasEspLiq(celula[i].pres,
					celula[i].temp,TEMP);
			//flut[i][k] = celula[i].flui.CalorLiq(celula[i].pres,celula[i].temp);
			k++;
		}
		if (profp.masg == 1) {
			flut[i][k] = celula[i].MC - celula[i].Mliqini;
			k++;
		}
		if (profp.masl == 1) {
			flut[i][k] = celula[i].Mliqini;
			k++;
		}
		if (profp.c0 == 1) {
			flut[i][k] = celula[i].c0;
			k++;
		}
		if (profp.ud == 1) {
			flut[i][k] = celula[i].ud;
			k++;
		}
		if (profp.calor == 1) {
			flut[i][k] = celula[i].calor.fluxIni;
			k++;
		}
		if (profp.masstrans == 1) {
			flut[i][k] = celula[i].transmassL;
			k++;
		}
		if (unidade < nunidadep - 1) {
			if (comp >= unidadeP[unidade + 1].Lini)
				unidade++;
		}
		flut[i][k] = unidade;
		k++;
		flut[i][k]=altura;
		k++;
		comp += celula[i].dx;
		altura+=celula[i].dx*sinl(celula[i].duto.teta);

	}
	ostringstream saidaP;
	int numero = roundl(tempo);
	if(indTramo<0)
	saidaP << "PERFISP" << "-" << numero << ".dat";
	else
    saidaP<< "Tramo"<<indTramo<<"-" << "PERFISP" << "-" << numero << ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << "# Passo de tempo = " << (*vg1dSP).contador << "\n";
	escreveIni << " Comprimento (m) ;";
	escreveIni << " Tempo (s) ;";
	if (profp.pres == 1)
		escreveIni << " Pressao (kgf/cm2) ;";
	if (profp.temp == 1)
		escreveIni << " Temperatura (C) ;";
	if (profp.hol == 1)
		escreveIni << " Holdup (-) ;";
	if (profp.ugs == 1)
		escreveIni << " Usg (m/s) ;";
	if (profp.uls == 1)
		escreveIni << " Uls (m/s) ;";
	if (profp.ug == 1)
		escreveIni << " Ug (m/s) ;";
	if (profp.ul == 1)
		escreveIni << " Ul (m/s) ;";
	if (profp.arra == 1)
		escreveIni << " Arranjo (-) ;";
	if (profp.viscl == 1)
		escreveIni << " Viscosidade Liquido (cP) ;";
	if (profp.viscg == 1)
		escreveIni << " Viscosidade Gas (cP) ;";
	if (profp.rhog == 1)
		escreveIni << " Massa Especifica Gas (kg/m3) ;";
	if (profp.rhol == 1)
		escreveIni << " Massa Especifica Liquido (kg/m3) ;";
	if (profp.masg == 1)
		escreveIni << " Vazao Massica Gas (kg/s) ;";
	if (profp.masl == 1)
		escreveIni << " Vazao Massica Liquido (kg/s) ;";
	if (profp.c0 == 1)
		escreveIni << " C0 (-) ;";
	if (profp.ud == 1)
		escreveIni << " Ud (m/s) ;";
	if (profp.RGO == 1)
		escreveIni << " RGO (Sm3/Sm3) ;";
	if (profp.deng == 1)
		escreveIni << " Densidade Gas (-) ;";
	if (profp.calor == 1)
		escreveIni << " Fluxo de Calor (W/m) ;";
	if (profp.masstrans == 1)
		escreveIni << " Transferencia de Massa entre Fases (kgf/(s m)) ;";
	if (profp.cpg == 1)
		escreveIni << " Calor Especifico a press�o constante Gas (J/(kg C)) ;";
	if (profp.cpl == 1)
		escreveIni
				<< " Calor Especifico a press�o constante Liquido (J/(kg C)) ;";
	escreveIni << " Unidade de Producao;";
	escreveIni << " Elevação (m);";
	escreveIni << "\n";

	escreveIni << flut;
	escreveIni.close();
}
}


void LerVap::imprimeProfileTrans(
	CelVap* const celula, int* length, double tempo, int indTramo) {
if (nperfistransp > 0) {
	for (int i = 0; i < nperfistransp; i++) {
		int n = length[i];
		int posicn = proftransp.posic[i];
		double comprimento = 0;
		for (int k = 0; k <= posicn; k++)
			comprimento += celp[k].dx;
		FullMtx<double> saida(n, 2);
		saida = celula[posicn].calor.perfil();
		ostringstream saidaP;
		int numero = roundl(tempo);
		if(indTramo<0)
		saidaP << "PERFISTRANSP" << "-" << numero << "-" << proftransp.posic[i]
			   << ".dat";
		else
		saidaP <<"Tramo"<<indTramo<<"-" <<"PERFISTRANSP" << "-" << numero << "-" << proftransp.posic[i]
			   << ".dat";
		string tmp = saidaP.str();

		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << "# Passo de tempo = " << (*vg1dSP).contador << "\n";
		escreveIni << "# Tempo (s) = " << tempo << "\n";
		escreveIni << "# Comprimento a partir do Fundo de Poco(m) = "
				<< comprimento << "\n";
		escreveIni << " Raio (m) ;";
		escreveIni << " Raio (Polegada) ;";
		escreveIni << " Temperatura (C) ;";
		escreveIni << "\n";
		escreveIni << saida;
		escreveIni.close();

	}
}
}


void LerVap::imprimeTrend(CelVap* const celula,
	double** flut, double tempo, int trend, int linha) {
if (ntendp > 0) {

	flut[linha][0] = tempo;
	int k = 1;

	int i = trendp[trend].posic;
	if (trendp[trend].pres == 1) {
		flut[linha][k] = celula[i].pres;
		k++;
	}
	if (trendp[trend].temp == 1) {
		flut[linha][k] = celula[i].temp;
		k++;
	}
	if (trendp[trend].hol == 1) {
		flut[linha][k] = 1. - celula[i].alf;
		k++;
	}
	if (trendp[trend].ugs == 1) {
		flut[linha][k] = celula[i].QG / celula[i].duto.area;
		k++;
	}
	if (trendp[trend].uls == 1) {
		flut[linha][k] = celula[i].QL / celula[i].duto.area;
		k++;
	}
	if (trendp[trend].ug == 1) {
		if (celula[i].alf > (*vg1dSP).localtiny)
			flut[linha][k] = celula[i].QG
					/ (celula[i].duto.area * celula[i].alf);
		else
			flut[linha][k] = 0;
		k++;
	}
	if (trendp[trend].ul == 1) {
		if (celula[i].alf < 1. - (*vg1dSP).localtiny)
			flut[linha][k] = celula[i].QL
					/ (celula[i].duto.area * (1. - celula[i].alf));
		else
			flut[linha][k] = 0;
		k++;
	}
	if (trendp[trend].arra == 1) {
		flut[linha][k] = celula[i].arranjo;
		k++;
	}
	if (trendp[trend].viscl == 1) {
		flut[linha][k] = celula[i].flui.ViscLiq(celula[i].pres,
				celula[i].temp);
		k++;
	}
	if (trendp[trend].viscg == 1) {
		flut[linha][k] = celula[i].flui.ViscGas(celula[i].pres, celula[i].temp);
		k++;
	}
	if (trendp[trend].rhog == 1) {
		flut[linha][k] = celula[i].flui.MasEspGas(celula[i].pres,
				celula[i].temp,TEMP);
		k++;
	}
	if (trendp[trend].rhol == 1) {
		flut[linha][k] = celula[i].flui.MasEspLiq(celula[i].pres,
				celula[i].temp,TEMP);
		k++;
	}
	if (trendp[trend].masg == 1) {
		flut[linha][k] = celula[i].MC - celula[i].Mliqini;
		k++;
	}
	if (trendp[trend].masl == 1) {
		flut[linha][k] = celula[i].Mliqini;
		k++;
	}
	if (trendp[trend].c0 == 1) {
		flut[linha][k] = celula[i].c0;
		k++;
	}
	if (trendp[trend].ud == 1) {
		flut[linha][k] = celula[i].ud;
		k++;
	}
	if (trendp[trend].calor == 1) {
		flut[linha][k] = celula[i].calor.fluxIni;
		k++;
	}
	if (trendp[trend].masstrans == 1) {
		flut[linha][k] = celula[i].transmassL;
		k++;
	}


}
}

