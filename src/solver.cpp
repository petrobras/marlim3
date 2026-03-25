/*
 * solver.cpp
 *
 *  Created on: 3 de dez. de 2023
 *      Author: Eduardo
 */

#include "solver.h"

void solv2D::parse_materiais(Value& material_json) {
	// obter o tamanho do elemento do Json
	(*vg1dSP).nmaterialVF = material_json.Size();
	// caso o tamanho do elemento seja maior que zero
		if ((*vg1dSP).nmaterialVF > 0) {
			// vetor de materiais
			matVF = new materialVF[(*vg1dSP).nmaterialVF];
			// loop para carga do vetor do JSON
			for (int i = 0; i < (*vg1dSP).nmaterialVF; i++) {

				matVF[i].cond =0;
				matVF[i].cp =0;
				matVF[i].rho =0;
				matVF[i].tipo =0;
				matVF[i].visc = 0;
				matVF[i].beta = 0;
				matVF[i].tipo = material_json[i]["tipo"].GetInt();

				// de-para do elemento "material" do json para o struct material
				if (matVF[i].tipo <= 1) {
					matVF[i].cond = material_json[i]["condutividade"].GetDouble();
					matVF[i].cp = material_json[i]["calorEspecifico"].GetDouble();
					matVF[i].rho = material_json[i]["rho"].GetDouble();
				}
				if(matVF[i].tipo== 1) {
					if(material_json[i].HasMember("visc"))matVF[i].visc = material_json[i]["visc"].GetDouble();
					if(material_json[i].HasMember("beta"))matVF[i].visc = material_json[i]["beta"].GetDouble();
				}
			}
		}
}

void solv2D::parse_corte(Value& corte_json) {
			// de-para do elemento do json para o struct
	corteVF.a = corte_json["diametroInterno"].GetDouble();
	corteVF.b = corteVF.a;
	corteVF.anul = 0;
	corteVF.rug = corteVF.a/10000.;//corte_json["rugosidade"].GetDouble();
	corteVF.ncam = corte_json["camadas"].Size();
	corteVF.diam = new double[corteVF.ncam];
	corteVF.indmat = new int[corteVF.ncam];
	corteVF.discre = new int[corteVF.ncam];
			// percorrer a lista de camadas da seção transversal
	for (int j = 0; j < corteVF.ncam; j++) {
		double esp=corte_json["camadas"][j]["espessura"].GetDouble();
		if(j==0)corteVF.diam[j]=corteVF.a+2.*esp;
		else corteVF.diam[j]=corteVF.diam[j-1]+2.*esp;

		corteVF.discre[j] =
				corte_json["camadas"][j]["discretizacao"].GetInt();
		corteVF.indmat[j] =
				corte_json["camadas"][j]["idMaterial"].GetInt();
	}
}

void solv2D::parse_fluido_complementar(Value& fluido_json) {
	// caso a propriedade "ativo" esteja habilitada
		// de-para do elemento "fluido_complementar" do json para o struct fluc
	int tipoflui=0;
	//if (fluido_json.HasMember("tipoF"))
			//tipoflui=fluido_json["tipoF"].GetInt();
	double masesp=0.;
	double compP = 0.;
	double compT = 0.;
	double tensup = 0.;
	double calesp = 0.;
	double condut = 0.;
	double templ = 0.;
	double lvisl = 0.;
	double temph = 0.;
	double lvish = 0.;
	double sal=0.;
	double rhogStd=0.7;
	double rhog=20.;
	//if (fluido_json.HasMember("salinidade"))
		//sal= fluido_json["salinidade"].GetDouble();
	//if(tipoflui!=1){
		//masesp =fluido_json["massaEspecifica"].GetDouble();
		//compP = fluido_json["compP"].GetDouble();
		//compT = fluido_json["compT"].GetDouble();
		//tensup = fluido_json["tensup"].GetDouble();
		//calesp = fluido_json["calorEspecifico"].GetDouble();
		//condut = fluido_json["condutividade"].GetDouble();
	    if (fluido_json.HasMember("rhogStd"))
		rhogStd = fluido_json["rhogStd"].GetDouble();
	    if (fluido_json.HasMember("temp1"))
		templ = fluido_json["temp1"].GetDouble();
	    if (fluido_json.HasMember("visc1"))
		lvisl = fluido_json["visc1"].GetDouble();
	    if (fluido_json.HasMember("temp2"))
		temph = fluido_json["temp2"].GetDouble();
	    if (fluido_json.HasMember("visc2"))
		lvish = fluido_json["visc2"].GetDouble();

	    int tabVisc=0;
	    int naoN=0;
	    int ndeform=1;
	    int ntemp=0;
	    double** viscTempDef;
		if (fluido_json.HasMember("tabelaViscosidade"))
		 tabVisc= fluido_json["tabelaViscosidade"].GetBool();
		if(tabVisc==1){
			if (fluido_json.HasMember("naoNewton"))
			 naoN= fluido_json["naoNewton"].GetBool();

			if(naoN==0){
				ndeform=1;
				ntemp=fluido_json["valorTemp"].Size();
				viscTempDef=new double*[ntemp + 1];
				for (int k = 0; k < ntemp + 1; k++)
					viscTempDef[k] = new double[2];
				viscTempDef[0][0]=0.;
				viscTempDef[0][1]=0.;
				for(int i=0;i<ntemp;i++){
					std::vector<double> viscTempLocal;
					viscTempDef[i+1][0]=fluido_json["valorTemp"][i].GetDouble();
					viscTempDef[i+1][1]=fluido_json["valorVisc"][i].GetDouble();
				}
			}
			else{
				ndeform=fluido_json["valorDeforma"].Size();
				ntemp=fluido_json["valorTemp"].Size();
				viscTempDef=new double*[ntemp + 1];
				for (int k = 0; k < ntemp + 1; k++)
					viscTempDef[k] = new double[ndeform+1];
				viscTempDef[0][0]=0.;
				for(int i=0;i<ndeform;i++){
					viscTempDef[0][i+1]=fluido_json["valorDeforma"][i].GetDouble();
				}
				for(int i=0;i<ntemp;i++){
					viscTempDef[i+1][0]=fluido_json["valorTemp"][i].GetDouble();
				}
				for(int i=0;i<ntemp;i++){
					for(int j=0;j<ndeform;j++){
						viscTempDef[i+1][j+1]=fluido_json["valorVisc"][i][j].GetDouble();
					}
				}
			}
		}
	//}

	flucVF = ProFluColVF(masesp, compP, compT, tensup, calesp,
				condut, sal, templ, lvisl, temph, lvish,0, tipoflui,rhogStd, rhog,
				tabVisc, ntemp, ndeform,naoN);
	if(tabVisc==1){
		for(int i=0;i<flucVF.nTemp+1; i++){
			for(int j=0;j<flucVF.nDeform+1; j++){
				flucVF.viscNaoNew[i][j]=viscTempDef[i][j];
			}
		}
		for(int i=0;i<ntemp+1;i++) delete[] viscTempDef[i];
		delete[] viscTempDef;
	}
	flucVF.npontos = 0;
}

void solv2D::parse_tempo(
		Value& tempo_json) {
	string chaveJson("#/configuracaoInicial");
/*
 * 	int perm;
	int trans;
	double relax;
	int parserie;
	double* dtmax;
	double* tempo;
 */

	tempVF.perm=1;
	tempVF.trans=0;
	tempVF.relax=0.5;
	tempVF.relaxP=0.01;
	tempVF.relaxT=0.01;
	impliAcopTerm=0;
	iteraTerm=200;
	tempVF.nvfHR=0;
	tempVF.parserie=0;
	tempVF.dtmax=0;
	tempVF.tempoDT=0;
	tempVF.tmax=0.;
	tempVF.tempoImp=0;
	tempVF.parserieImp=0;
	tempVF.confinado=0;
	tempVF.pRef=1.*98066.52;

	tempVF.errP=0.00001;
	tempVF.errV=0.0001;
	tempVF.errT=0.0000001;
	tempVF.erroRes=1.e-3;
	tempVF.maxIt=100;

	tempVF.tendTemp=10.;
	equilterm=0;
	if (tempo_json.HasMember("equilibrioTermico"))
		equilterm = tempo_json["equilibrioTermico"].GetBool();

	if (tempo_json.HasMember("nPrime"))
		nPrime = tempo_json["nPrime"].GetInt();

	rankLU=-1;
	if (tempo_json.HasMember("rankLU"))
		rankLU = tempo_json["rankLU"].GetInt();
	colore=0;
	if(rankLU>=0){
		if (tempo_json.HasMember("corLU"))
			colore = tempo_json["corLU"].GetBool();
	}
	solverMat=0;
	if (tempo_json.HasMember("solverMat"))
		solverMat = tempo_json["solverMat"].GetInt();

	(*vg1dSP).atrasaHR=0;
	if(tempo_json.HasMember("implicitoHR"))(*vg1dSP).atrasaHR = 1-tempo_json["implicitoHR"].GetBool();

	dtSegur=10.;
	cicloSegur=100;
	if (tempo_json.HasMember("razDtMinimo"))
		dtSegur = tempo_json["razDtMinimo"].GetDouble();
	if (tempo_json.HasMember("cicloSeguranca"))
		cicloSegur = tempo_json["cicloSeguranca"].GetInt();

	if (tempo_json.HasMember("threadLocal"))
		nthrdLocal = tempo_json["threadLocal"].GetInt();

	nthrdMatriz=nthrdLocal;

	if (tempo_json.HasMember("tendImpresTemp"))
		tempVF.tendTemp = tempo_json["tendImpresTemp"].GetDouble();

	if (tempo_json.HasMember("acop"))
		(*vg1dSP).acop = tempo_json["acop"].GetInt();
	if((*vg1dSP).acop==1){
		(*vg1dSP).tAmb = tempo_json["tempAmbiente"].GetDouble();
		(*vg1dSP).vAmb = tempo_json["velAmbiente"].GetDouble();
		(*vg1dSP).amb= tempo_json["Ambiente"].GetInt();
		//angAcop= tempo_json["anguloAcoplamento"].GetDouble();
		//(*vg1dSP).centroX=tempo_json["centroX"].GetDouble();
		//(*vg1dSP).centroY=tempo_json["centroY"].GetDouble();
		(*vg1dSP).centroX= 0.;//tempo_json["centroX"].GetDouble();
		(*vg1dSP).centroY= 0.;//tempo_json["centroY"].GetDouble();
	}

	if (tempo_json.HasMember("errP"))
		tempVF.errP =
				tempo_json["errP"].GetDouble();
	if (tempo_json.HasMember("errV"))
		tempVF.errV =
				tempo_json["errV"].GetDouble();
	if (tempo_json.HasMember("errT"))
		tempVF.errT =
				tempo_json["errT"].GetDouble();
	if (tempo_json.HasMember("acoplamentoImplicitoTermico"))
		impliAcopTerm =
				tempo_json["acoplamentoImplicitoTermico"].GetBool();
	if (tempo_json.HasMember("iteraTermico"))
		iteraTerm =
				tempo_json["iteraTermico"].GetInt();
	if (tempo_json.HasMember("limiteResiduo"))
		tempVF.erroRes =
				tempo_json["limiteResiduo"].GetDouble();
	if (tempo_json.HasMember("maxIt"))
		tempVF.maxIt =
				tempo_json["maxIt"].GetInt();

	if (tempo_json.HasMember("acopPV"))
		(*vg1dSP).metodoAcopPV =
				tempo_json["acopPV"].GetInt();
	if (tempo_json.HasMember("atrasaHR"))
		(*vg1dSP).atrasaHR =
				tempo_json["atrasaHR"].GetInt();

	if (tempo_json.HasMember("tmax"))
		tempVF.tmax =
				tempo_json["tmax"].GetDouble();
	if (tempo_json.HasMember("anguloY"))
		(*vg1dSP).angY =
				tempo_json["anguloY"].GetDouble();
	//if (tempo_json.HasMember("anguloX"))
		//angX =
				//tempo_json["anguloX"].GetDouble();
	if (tempo_json.HasMember("confinado")){
		tempVF.confinado =tempo_json["confinado"].GetInt();
		tempVF.pRef =tempo_json["pressaoReferencia"].GetDouble()*98066.52;
	}
	if (tempo_json.HasMember("perm"))
		tempVF.perm =
				tempo_json["perm"].GetBool();
	if (tempo_json.HasMember("trans"))
		tempVF.trans =
				tempo_json["trans"].GetBool();
	if (tempo_json.HasMember("relax"))
		tempVF.relax =
				tempo_json["relax"].GetDouble();
	if (tempo_json.HasMember("relaxExpli"))
		(*vg1dSP).relaxVFExpli =
				tempo_json["relaxExpli"].GetDouble();
	if((*vg1dSP).metodoAcopPV==2)tempVF.relax=1.;
	if (tempo_json.HasMember("relaxP"))
		tempVF.relaxP =
				tempo_json["relaxP"].GetDouble();
	if (tempo_json.HasMember("relaxT"))
		tempVF.relaxT =
				tempo_json["relaxT"].GetDouble();
	if (tempo_json.HasMember("nvfHR"))
		tempVF.nvfHR =
				tempo_json["nvfHR"].GetInt();
	if (tempo_json.HasMember("nvfHRVOF"))
		(*vg1dSP).nvfHRHol =
				tempo_json["(*vg1dSP).nvfHRHol"].GetInt();

	if (tempo_json.HasMember("dtMax"))
		tempVF.parserie =
				tempo_json["dtMax"].Size();
	else if(tempVF.trans==1)logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'dtMax' inexistente",chaveJson, "condicao de tempo");
	if(tempo_json["dtMax"].Size()!=tempo_json["tempo"].Size()){
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Chaves 'dtMax', 'tempo' com dimensoes diferentes",
				chaveJson, "Condicoes de tempo");
	}
	tempVF.dtmax=new double [tempVF.parserie];
	tempVF.tempoDT=new double [tempVF.parserie];
	for(int i=0; i<tempVF.parserie; i++){
		tempVF.dtmax[i]=tempo_json["dtMax"][i].GetDouble();
		tempVF.tempoDT[i]=tempo_json["tempo"][i].GetDouble();
	}


	if (tempo_json.HasMember("tempoImp"))
		tempVF.parserieImp =
				tempo_json["tempoImp"].Size();
	else if(tempVF.trans==1)logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempoImp' inexistente para impressão do processo transiente",chaveJson, "condicao de tempo");
	tempVF.tempoImp=new double [tempVF.parserieImp];

	if (tempo_json.HasMember("residuo"))
		(*vg1dSP).tipoRes =
				tempo_json["residuo"].GetInt();
	for(int i=0; i<tempVF.parserieImp; i++){
		tempVF.tempoImp[i]=tempo_json["tempoImp"][i].GetDouble();
	}

	if (tempo_json.HasMember("CFL"))
		(*vg1dSP).cflG =
				tempo_json["CFL"].GetDouble();

	if (tempo_json.HasMember("corrigeDistorcao"))
	 (*vg1dSP).corrigeDistor=tempo_json["corrigeDistorcao"].GetDouble();

	if (tempo_json.HasMember("forcaCorpoCompleta"))
		(*vg1dSP).mulFC=tempo_json["forcaCorpoCompleta"].GetDouble();

	if (tempo_json.HasMember("VOF"))
		(*vg1dSP).aplicaVOF=tempo_json["VOF"].GetInt();
	if (tempo_json.HasMember("corrForcCorp"))
		(*vg1dSP).correcForcCorp=tempo_json["corrForcCorp"].GetInt();
	if (tempo_json.HasMember("nThread"))
		(*vg1dSP).ntrd=tempo_json["nThread"].GetInt();

}

void solv2D::parse_Prop(Value& prop_json){
	string chaveJson("#/prop");
	prop.parserie=prop_json.Size();
	prop.cond=new double [prop.parserie];
	prop.rho=new double [prop.parserie];
	prop.visc=new double [prop.parserie];
	prop.beta=new double [prop.parserie];
	prop.tRef=new double [prop.parserie];
	prop.cp=new double [prop.parserie];
	for(int i=0;i<prop.parserie;i++){
		if (prop_json[i].HasMember("cond")) prop.cond[i]=prop_json[i]["cond"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'cond', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("cp")) prop.cp[i]=prop_json[i]["cp"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'cp', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("rho")) prop.rho[i]=prop_json[i]["rho"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'rho', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("visc")) prop.visc[i]=prop_json[i]["visc"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'visc', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("beta")) prop.beta[i]=prop_json[i]["beta"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'beta', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("tRef")) prop.tRef[i]=prop_json[i]["tRef"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'tRef', ", chaveJson, "faltante");

	}
}

void solv2D::parse_CI(Value& CI_json){
	string chaveJson("#/CI");
	CI.parserie=CI_json.Size();
	CI.xmax=new double [CI.parserie];
	CI.xmin=new double [CI.parserie];
	CI.ymax=new double [CI.parserie];
	CI.ymin=new double [CI.parserie];
	CI.temp=new double [CI.parserie];
	CI.valU=new double [CI.parserie];
	CI.valV=new double [CI.parserie];
	CI.pres=new double [CI.parserie];
	for(int i=0;i<CI.parserie;i++){
		if (CI_json[i].HasMember("xmax")) CI.xmax[i]=CI_json[i]["xmax"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'xmax', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("xmin")) CI.xmin[i]=CI_json[i]["xmin"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'xmin',", chaveJson, "faltante");

		if (CI_json[i].HasMember("ymax")) CI.ymax[i]=CI_json[i]["ymax"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'ymax', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("ymin")) CI.ymin[i]=CI_json[i]["ymin"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'ymin', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("temp")) CI.temp[i]=CI_json[i]["temp"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'temp', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("velU")) CI.valU[i]=CI_json[i]["velU"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'velU', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("velV")) CI.valV[i]=CI_json[i]["velV"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'velV', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("pres")) CI.pres[i]=CI_json[i]["pres"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'pres', ", chaveJson, "faltante");

	}
}

void solv2D::parse_CC(
		Value& CC_json) {
	string chaveJson("#/CC");
/*
 * 	int nDiri;
	int nRic;
	int nVN;
	detDiri* ccDir;
	detRic* ccRic;
	detVN* ccVN;
 */

	CC.nInl=0;
	CC.ccInl=0;
	CC.nOut=0;
	CC.ccPres=0;
	CC.nSim=0;
	CC.ccSim=0;
	CC.nWall=0;
	CC.ccWall=0;
	/////////////////////////////////////////////////////////////////////////////////////////////////
	CC.nDiri=0;
	CC.ccDir=0;
	CC.nRic=0;
	CC.ccRic=0;
	CC.nVN=0;
	CC.ccVN=0;

	CC.rotuloAcop=-1;

	if (CC_json.HasMember("Inlet")){
		CC.nInl =
				CC_json["Inlet"].Size();
		CC.ccInl=new detInl [CC.nInl];
	}
	if (CC_json.HasMember("Outlet")){
		CC.nOut =
				CC_json["Outlet"].Size();
		CC.ccPres=new detPres [CC.nOut];
	}
	if (CC_json.HasMember("Simetria")){
		CC.nSim =
				CC_json["Simetria"].Size();
		CC.ccSim=new detSim [CC.nSim];
	}
	if (CC_json.HasMember("Wall")){
		CC.nWall =
				CC_json["Wall"].Size();
		CC.ccWall=new detWall [CC.nWall];
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if (CC_json.HasMember("Dirichlet")){
		CC.nDiri =
				CC_json["Dirichlet"].Size();
		CC.ccDir=new detDiri [CC.nDiri];
	}
	if (CC_json.HasMember("Richardson")){
		CC.nRic =
				CC_json["Richardson"].Size();
		CC.ccRic=new detRic [CC.nRic];
	}
	if (CC_json.HasMember("VonNewman")){
		CC.nVN =
				CC_json["VonNewman"].Size();
		CC.ccVN=new detVN [CC.nVN];
	}
	if((*vg1dSP).acop==1){
		CC.rotuloAcop=CC_json["acoplamento"].GetInt();
	}

	for(int i=0; i<CC.nInl; i++){
		/*
		 * 	int rotulo;
			double* val;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccInl[i].rotulo=CC_json["Inlet"][i]["rotulo"].GetInt();
		else CC.ccInl[i].rotuloSTR=CC_json["Inlet"][i]["rotuloSTR"].GetString();
		CC.ccInl[i].nserie=CC_json["Inlet"][i]["velU"].Size();
		if(CC_json["Inlet"][i]["velU"].Size()!=CC_json["Inlet"][i]["tempo"].Size()){
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'valor', 'tempo' com dimensoes diferentes",
					chaveJson, "Condicoes de Contorno Inlet");
		}
		CC.ccInl[i].tempo=new double [CC.ccInl[i].nserie];
		CC.ccInl[i].valU=new double [CC.ccInl[i].nserie];
		CC.ccInl[i].valV=new double [CC.ccInl[i].nserie];
		for(int j=0; j<CC.ccInl[i].nserie; j++){
			CC.ccInl[i].tempo[j]=CC_json["Inlet"][i]["tempo"][j].GetDouble();
			CC.ccInl[i].valU[j]=CC_json["Inlet"][i]["velU"][j].GetDouble();
			CC.ccInl[i].valV[j]=CC_json["Inlet"][i]["velV"][j].GetDouble();
		}
	}

	for(int i=0; i<CC.nOut; i++){
		/*
		 * 	int rotulo;
			double* val;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccPres[i].rotulo=CC_json["Outlet"][i]["rotulo"].GetInt();
		else CC.ccPres[i].rotuloSTR=CC_json["Outlet"][i]["rotuloSTR"].GetString();
		CC.ccPres[i].nserie=CC_json["Outlet"][i]["pres"].Size();
		if(CC_json["Outlet"][i]["pres"].Size()!=CC_json["Outlet"][i]["pres"].Size()){
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'pres', 'tempo' com dimensoes diferentes",
					chaveJson, "Condicoes de Contorno Outlet");
		}
		CC.ccPres[i].tempo=new double [CC.ccPres[i].nserie];
		CC.ccPres[i].val=new double [CC.ccPres[i].nserie];
		for(int j=0; j<CC.ccPres[i].nserie; j++){
			CC.ccPres[i].tempo[j]=CC_json["Outlet"][i]["tempo"][j].GetDouble();
			CC.ccPres[i].val[j]=CC_json["Outlet"][i]["pres"][j].GetDouble()*98066.52;
		}
	}


	for(int i=0; i<CC.nSim; i++){
		/*
		 * 	int rotulo;
			double* valAmb;
			double* hAmb;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccSim[i].rotulo=CC_json["Simetria"][i]["rotulo"].GetInt();
		else CC.ccSim[i].rotuloSTR=CC_json["Simetria"][i]["rotuloSTR"].GetString();
	}

	for(int i=0; i<CC.nWall; i++){
		/*
		 * 	int rotulo;
			double* val;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccWall[i].rotulo=CC_json["Wall"][i]["rotulo"].GetInt();
		else CC.ccWall[i].rotuloSTR=CC_json["Wall"][i]["rotuloSTR"].GetString();
		CC.ccWall[i].nserie=CC_json["Wall"][i]["vel"].Size();
		if(CC_json["Wall"][i]["tempo"].Size()!=CC_json["Wall"][i]["vel"].Size()){
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'vel', 'tempo' com dimensoes diferentes",
					chaveJson, "Condicoes de Contorno Wall");
		}
		CC.ccWall[i].tempo=new double [CC.ccWall[i].nserie];
		CC.ccWall[i].velW=new double [CC.ccWall[i].nserie];
		for(int j=0; j<CC.ccWall[i].nserie; j++){
			CC.ccWall[i].tempo[j]=CC_json["Wall"][i]["tempo"][j].GetDouble();
			CC.ccWall[i].velW[j]=CC_json["Wall"][i]["vel"][j].GetDouble();
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int i=0; i<CC.nDiri; i++){
		/*
		 * 	int rotulo;
			double* val;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccDir[i].rotulo=CC_json["Dirichlet"][i]["rotulo"].GetInt();
		else CC.ccDir[i].rotuloSTR=CC_json["Dirichlet"][i]["rotuloSTR"].GetString();
		CC.ccDir[i].nserie=CC_json["Dirichlet"][i]["valor"].Size();
		if(CC_json["Dirichlet"][i]["valor"].Size()!=CC_json["Dirichlet"][i]["tempo"].Size()){
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'valor', 'tempo' com dimensoes diferentes",
					chaveJson, "Condicoes de Contorno Dirichlet");
		}
		CC.ccDir[i].tempo=new double [CC.ccDir[i].nserie];
		CC.ccDir[i].val=new double [CC.ccDir[i].nserie];
		for(int j=0; j<CC.ccDir[i].nserie; j++){
			CC.ccDir[i].tempo[j]=CC_json["Dirichlet"][i]["tempo"][j].GetDouble();
			CC.ccDir[i].val[j]=CC_json["Dirichlet"][i]["valor"][j].GetDouble();
		}
	}

	for(int i=0; i<CC.nVN; i++){
		/*
		 * 	int rotulo;
			double* val;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccVN[i].rotulo=CC_json["VonNewman"][i]["rotulo"].GetInt();
		else CC.ccVN[i].rotuloSTR=CC_json["VonNewman"][i]["rotuloSTR"].GetString();
		CC.ccVN[i].nserie=CC_json["VonNewman"][i]["valor"].Size();
		if(CC_json["VonNewman"][i]["valor"].Size()!=CC_json["VonNewman"][i]["tempo"].Size()){
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'valor', 'tempo' com dimensoes diferentes",
					chaveJson, "Condicoes de Contorno Newman");
		}
		CC.ccVN[i].tempo=new double [CC.ccVN[i].nserie];
		CC.ccVN[i].val=new double [CC.ccVN[i].nserie];
		for(int j=0; j<CC.ccVN[i].nserie; j++){
			CC.ccVN[i].tempo[j]=CC_json["VonNewman"][i]["tempo"][j].GetDouble();
			CC.ccVN[i].val[j]=CC_json["VonNewman"][i]["valor"][j].GetDouble();
		}
	}


	for(int i=0; i<CC.nRic; i++){
		/*
		 * 	int rotulo;
			double* valAmb;
			double* hAmb;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccRic[i].rotulo=CC_json["Richardson"][i]["rotulo"].GetInt();
		else CC.ccRic[i].rotuloSTR=CC_json["Richardson"][i]["rotuloSTR"].GetString();
		CC.ccRic[i].nserie=CC_json["Richardson"][i]["valorAmb"].Size();
		if(CC_json["Richardson"][i]["valorAmb"].Size()!=CC_json["Richardson"][i]["tempo"].Size()){
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'valor', 'tempo' com dimensoes diferentes",
					chaveJson, "Condicoes de Contorno Richardson");
		}
		CC.ccRic[i].tempo=new double [CC.ccRic[i].nserie];
		CC.ccRic[i].valAmb=new double [CC.ccRic[i].nserie];
		CC.ccRic[i].hAmb=new double [CC.ccRic[i].nserie];
		for(int j=0; j<CC.ccRic[i].nserie; j++){
			CC.ccRic[i].tempo[j]=CC_json["Richardson"][i]["tempo"][j].GetDouble();
			CC.ccRic[i].valAmb[j]=CC_json["Richardson"][i]["valorAmb"][j].GetDouble();
			CC.ccRic[i].hAmb[j]=CC_json["Richardson"][i]["hAmb"][j].GetDouble();
		}
	}
}


void solv2D::parse_malha(
		Value& malha_json, double& dx, double& dy,string& elearq,string& noarq,string& polyarq) {
	string chaveJson("#/malha");


	if (malha_json.HasMember("tipoArquivo"))unv=malha_json["tipoArquivo"].GetInt();
	if(unv==0){
		if (malha_json.HasMember("elemento"))
			elearq = pathArqExtEntrada + string(malha_json["elemento"].GetString());
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'arquivo de elemento' inexistente",chaveJson, "malha");

		if (malha_json.HasMember("no"))
			noarq = pathArqExtEntrada + string(malha_json["no"].GetString());
		else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'arquivo de no' inexistente",chaveJson, "malha");

		if (malha_json.HasMember("poly"))
			polyarq = pathArqExtEntrada + string(malha_json["poly"].GetString());
		else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'arquivo de poly' inexistente",chaveJson, "malha");
	}
	else{
		if (malha_json.HasMember("elemento"))
			elearq = pathArqExtEntrada + string(malha_json["elemento"].GetString());
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'arquivo de elemento' inexistente",chaveJson, "malha");
	}

	if (malha_json.HasMember("dxMalhaRegular"))
		dx = malha_json["dxMalhaRegular"].GetDouble();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'dxMalhaRegular' inexistente",chaveJson, "malha");

	if (malha_json.HasMember("dyMalhaRegular"))
		dy = malha_json["dyMalhaRegular"].GetDouble();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'dyMalhaRegular' inexistente",chaveJson, "malha");

}

void solv2D::parse_Interface(
		Value& Interface_json) {
	string chaveJson("#/interface");
	interface.nserie =
			Interface_json["xcoor"].Size();
	interface.xcoor=new double [interface.nserie];
	interface.ycoor=new double [interface.nserie];
	interface.xmax=-1e5;
	interface.ymax=-1e5;
	interface.xmin=1e5;
	interface.ymin=1e5;
	for(int i=0;i<interface.nserie;i++){
		interface.xcoor[i]=Interface_json["xcoor"][i].GetDouble();
		interface.ycoor[i]=Interface_json["ycoor"][i].GetDouble();
		if(interface.xcoor[i]>interface.xmax)interface.xmax=interface.xcoor[i];
		if(interface.xcoor[i]<interface.xmin)interface.xmin=interface.xcoor[i];
		if(interface.ycoor[i]>interface.ymax)interface.ymax=interface.ycoor[i];
		if(interface.ycoor[i]<interface.ymin)interface.ymin=interface.ycoor[i];
	}
}

void solv2D::calcYInter(double x, double* y){
	int iInter=0;
	double val;
	y[0]=-1e5;
	y[1]=-1e5;
	int iy=0;
	for(int i=0; i<interface.nserie-1;i++){
		if((x>=interface.xcoor[i] && x<=interface.xcoor[i+1]) || (x>=interface.xcoor[i+1] && x<=interface.xcoor[i])){
			double raz=(x-interface.xcoor[i])/(interface.xcoor[i+1]-interface.xcoor[i]);
			y[iy]=raz*interface.ycoor[i+1]+(1.-raz)*interface.ycoor[i+1];
			iy++;
		}
		if(iy>1)break;
	}
	if(iy==1)y[1]=y[0];
	if(y[1]<y[0]){
		double val=y[0];
		y[0]=y[1];
		y[1]=val;
	}
}

double solv2D::calcYInterUno(double x,double ymed){
	int iInter=0;
	double val;
	double y[2];
	y[0]=-1e5;
	y[1]=-1e5;
	int iy=0;
	for(int i=0; i<interface.nserie-1;i++){
		if((x>=interface.xcoor[i] && x<=interface.xcoor[i+1]) || (x>=interface.xcoor[i+1] && x<=interface.xcoor[i])){
			double raz=(x-interface.xcoor[i])/(interface.xcoor[i+1]-interface.xcoor[i]);
			y[iy]=raz*interface.ycoor[i+1]+(1.-raz)*interface.ycoor[i+1];
			iy++;
		}
		if(iy>1)break;
	}
	if(iy==1)y[1]=y[0];
	if(y[1]<y[0]){
		double val=y[0];
		y[0]=y[1];
		y[1]=val;
	}
	if(fabsl(y[0]-y[1])>1e-15){
		if(fabsl(y[0]-ymed)>fabsl(y[1]-ymed))y[0]=y[1];
	}
	return y[0];
}

void solv2D::parse_mapProp(
		Value& mapProp_json) {
	string chaveJson("#/mapProp");
	mapprop.parserie =
			mapProp_json.Size();
	//mapprop.xcoor=new double [interface.nserie];
	//mapprop.ymax=new double [interface.nserie];
	//mapprop.ymin=new double [interface.nserie];
	mapprop.indProp=new int [interface.nserie];
	mapprop.interno=new int [interface.nserie];
	mapprop.inferior=new int [interface.nserie];
	double yInterf[2];
	for(int i=0;i<mapprop.parserie;i++){
		if (mapProp_json[i].HasMember("interno"))
			mapprop.interno[i]=mapProp_json[i]["interno"].GetBool();
		else mapprop.interno[i]=-1;
		if(mapprop.interno[i]==-1)mapprop.inferior[i]=mapProp_json[i]["inferior"].GetBool();
		else mapprop.inferior[i]=-1;
		/*mapprop.xcoor[i]=mapProp_json["xCoor"][i].GetDouble();
		calcYInter(mapprop.xcoor[i], yInterf);
		if(mapprop.interno[i]==1){
			if(yInterf[0]<=yInterf[1]){
				mapprop.ymax[i]=yInterf[0];
				mapprop.ymin[i]=yInterf[1];
			}
			else{
				mapprop.ymax[i]=yInterf[1];
				mapprop.ymin[i]=yInterf[0];
			}
		}
		else if(mapprop.inferior==1){
			if(yInterf[0]<=yInterf[1]){
				mapprop.ymax[i]=yInterf[0];
				mapprop.ymin[i]=-1e5;
			}
			else{
				mapprop.ymax[i]=yInterf[1];
				mapprop.ymin[i]=-1e5;
			}
		}
		else{
			if(yInterf[0]<=yInterf[1]){
				mapprop.ymax[i]=1e5;
				mapprop.ymin[i]=yInterf[1];
			}
			else{
				mapprop.ymax[i]=1e5;
				mapprop.ymin[i]=yInterf[0];
			}
		}*/
		mapprop.indProp[i]=mapProp_json[i]["indProp"].GetInt();
	}
}

double solv2D::SIGN(double a, double b) {
  return (b >= 0 ? 1.0 : -1.0) * fabsl(a);
}
double  solv2D::zriddr(double x1,double x2,double a,double b,double ymed){
	double xacc=1e-5;
	int maxit=100;
	double fmin;
	double xmin;
	double fl=a*x1+b-calcYInterUno(x1,ymed);
	double fh=a*x2+b-calcYInterUno(x2,ymed);
	if(fabsl(fh)<fabsl(fl)){
		fmin=fh;
		xmin=x2;
	}
	else{
		fmin=fl;
		xmin=x1;
	}
	if((fl>0.0 && fh<0.0) || (fl<0.0 && fh>0.0)){
		double xl=x1;
		double xh=x2;
		double ans=-1.e20;
		for(int j=0;j<maxit;j++){
			double xm=0.5*(xl+xh);
			double fm=a*xm+b-calcYInterUno(xm,ymed);
			if(fabsl(fm)<fabsl(fmin)){
				fmin=fm;
				xmin=xm;
			}
			double s=sqrtl(fm*fm-fl*fh);
			if(s==0.0){
				fmin=a*xmin+b-calcYInterUno(xmin,ymed);
				return xmin;
			}
			double xnew=xm+(xm-xl)*((fl>=fh ? 1.0 : -1.0)*fm/s);
			if(fabsl(xnew-ans)<=xacc ){
				fmin=a*xmin+b-calcYInterUno(xmin,ymed);
				return xmin;
			}
			ans=xnew;
			double fnew=a*ans+b-calcYInterUno(ans,ymed);
			if(fabsl(fnew)<fabsl(fmin)){
				fmin=fnew;
				xmin=ans;
			}
			if(fabsl(fnew)<=xacc ){
				fmin=a*xmin+b-calcYInterUno(xmin,ymed);
				return xmin;
			}
			if(SIGN(fm, fnew)!=fm){
				xl=xm;
				fl=fm;
				xh=ans;
				fh=fnew;
			}
			else if(SIGN(fl,fnew)!=fl){
				xh=ans;
				fh=fnew;
			}
			else if(SIGN(fh,fnew)!=fh){
				xl=ans;
				fl=fnew;
			}
			else return -100000.;
			if(fabsl(xh-xl)<=xacc ){
				fmin=a*xmin+b-calcYInterUno(xmin,ymed);
				return xmin;
			}

		}
		 return 100000.;
	}
	else {
		if(fabsl(fl)<=xacc) return x1;
		if(fabsl(fh)<=xacc) return x2;
		return -10000.;
	}
}

void solv2D::iniciaProp(malha2dVF& malha){
	for(int iEle=0; iEle<malha.nele;iEle++){
		double holGenerico=1.;
		if(mapprop.parserie>1){
			int nvert=3;
			double xN[nvert];
			double yN[nvert];
			double yInterf[2];
			double xInterf[2];
			int cheio=1;
			int teste=pow(cheio,nvert);
			int tipoNo[nvert];
			int iLiq;
			if(prop.rho[mapprop.indProp[0]]>prop.rho[mapprop.indProp[1]])iLiq=0;
			else iLiq=1;
			for(int k=0;k<nvert;k++){
				xN[k]=malha.mlh2d[iEle].cel2D.coordVert[k][0];
				yN[k]=malha.mlh2d[iEle].cel2D.coordVert[k][1];
				calcYInter(xN[k], yInterf);
				if(yN[k]<yInterf[0] && yInterf[0]==yInterf[1]){
					cheio*=-1;
					tipoNo[k]=-1;
				}
				else if(yN[k]>yInterf[1] && yInterf[0]==yInterf[1]){
					cheio*=-2;
					tipoNo[k]=-2;
				}
				else if(yN[k]>yInterf[0] && yN[k]<yInterf[1]){
					cheio*=-3;
					tipoNo[k]=-3;
				}
				else{
					cheio*=-4;
					tipoNo[k]=-4;
				}
			}
			int flui=0;
			if(cheio==-1){
				for(int i=0;i<mapprop.parserie;i++){
					if(mapprop.inferior[i]==1){
						flui=i;
						if(flui==iLiq) holGenerico=1.;
						else  holGenerico=0.;
						malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
						break;
					}
				}
			}
			else if(cheio==-8){
				for(int i=0;i<mapprop.parserie;i++){
					if(mapprop.inferior[i]==0){
						flui=i;
						if(flui==iLiq) holGenerico=1.;
						else  holGenerico=0.;
						malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
						break;
					}
				}
			}
			else if(cheio==-27){
				for(int i=0;i<mapprop.parserie;i++){
					if(mapprop.interno[i]==1){
						flui=i;
						if(flui==iLiq) holGenerico=1.;
						else  holGenerico=0.;
						malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
						break;
					}
				}
			}
			else if(cheio==-64){
				for(int i=0;i<mapprop.parserie;i++){
					if(mapprop.interno[i]==0){
						flui=i;
						if(flui==iLiq) holGenerico=1.;
						else  holGenerico=0.;
						malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
						break;
					}
				}
			}
			else if(cheio==-2){
				int noSup=0;
				int noInf[2];
				double xInter[2];
				double yInter[2];
				int konta=0;
				for(int k=0;k<nvert;k++){
					if(tipoNo[k]==-2){
						noSup=k;
					}
					else{
						noInf[konta]=k;
						konta++;
					}
				}
				for(int k=0;k<nvert-1;k++){
					double ycres[2];
					double xcres[2];
					if(xN[noInf[k]]>=xN[noSup]){
						xcres[0]=xN[noSup];
						xcres[1]=xN[noInf[k]];
						ycres[0]=yN[noSup];
						ycres[1]=yN[noInf[k]];
					}
					else{
						xcres[1]=xN[noSup];
						xcres[0]=xN[noInf[k]];
						ycres[1]=yN[noSup];
						ycres[0]=yN[noInf[k]];
					}
					double a=(ycres[1]-ycres[0])/(xcres[1]-xcres[0]);
					double b=ycres[0]-xcres[0]*a;
					double xmed=0.5*(xcres[1]+xcres[0]);
					double ymed=a*xmed+b;
					xInterf[k]=zriddr(xcres[0],xcres[1],a,b,ymed);
					yInterf[k]=calcYInterUno(xInterf[k],ymed);
				}
				double xsup[3];
				double ysup[3];
				xsup[0]=xN[noSup];
				ysup[0]=yN[noSup];
				for(int k=0;k<nvert-1;k++){
					xsup[k+1]=xInterf[k];
					ysup[k+1]=yInterf[k];
				}
    			double subVElem=0.5*fabsl((xsup[1]-xsup[0])*
					(ysup[2]-ysup[0])-(xsup[2]-xsup[0])*
					(ysup[1]-ysup[0]));
    			double holInf=1.-subVElem/malha.mlh2d[iEle].cel2D.vElem;
    			//holInf=0.5;
    			int indSup;
    			int indInf;
    			if(mapprop.inferior[0]==0){
    				indSup=mapprop.indProp[0];
    				indInf=mapprop.indProp[1];
    				if(iLiq==1)holGenerico=holInf;
    				else holGenerico=1.-holInf;
    			}
    			else{
       				indSup=mapprop.indProp[1];
        			indInf=mapprop.indProp[0];
    				if(iLiq==0)holGenerico=holInf;
    				else holGenerico=1.-holInf;
    			}
    			malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
				malha.mlh2d[iEle].cel2D.cond=(1.-holInf)*prop.cond[indSup]+holInf*prop.cond[indInf];
				malha.mlh2d[iEle].cel2D.cp=(1.-holInf)*prop.cp[indSup]+holInf*prop.cp[indInf];
				malha.mlh2d[iEle].cel2D.rho=(1.-holInf)*prop.rho[indSup]+holInf*prop.rho[indInf];
				malha.mlh2d[iEle].cel2D.visc=(1.-holInf)*prop.visc[indSup]+holInf*prop.visc[indInf];
				malha.mlh2d[iEle].cel2D.beta=(1.-holInf)*prop.beta[indSup]+holInf*prop.beta[indInf];
				malha.mlh2d[iEle].cel2D.tempRef=(1.-holInf)*prop.tRef[indSup]+holInf*prop.tRef[indInf];
			}
			else if(cheio==-4){
				int noInf=0;
				int noSup[2];
				double xInter[2];
				double yInter[2];
				int konta=0;
				for(int k=0;k<nvert;k++){
					if(tipoNo[k]==-1){
						noInf=k;
					}
					else{
						noSup[konta]=k;
						konta++;
					}
				}
				for(int k=0;k<nvert-1;k++){
					double ycres[2];
					double xcres[2];
					if(xN[noSup[k]]>=xN[noInf]){
						xcres[0]=xN[noInf];
						xcres[1]=xN[noSup[k]];
						ycres[0]=yN[noInf];
						ycres[1]=yN[noSup[k]];
					}
					else{
						xcres[1]=xN[noInf];
						xcres[0]=xN[noSup[k]];
						ycres[1]=yN[noInf];
						ycres[0]=yN[noSup[k]];
					}
					double a=(ycres[1]-ycres[0])/(xcres[1]-xcres[0]);
					double b=ycres[0]-xcres[0]*a;
					double xmed=0.5*(xcres[1]+xcres[0]);
					double ymed=a*xmed+b;
					xInterf[k]=zriddr(xcres[0],xcres[1],a,b,ymed);
					yInterf[k]=calcYInterUno(xInterf[k],ymed);
				}
				double xinf[3];
				double yinf[3];
				xinf[0]=xN[noInf];
				yinf[0]=yN[noInf];
				for(int k=0;k<nvert-1;k++){
					xinf[k+1]=xInterf[k];
					yinf[k+1]=yInterf[k];
				}
    			double subVElem=0.5*fabsl((xinf[1]-xinf[0])*
					(yinf[2]-yinf[0])-(xinf[2]-xinf[0])*
					(yinf[1]-yinf[0]));
    			double holInf=subVElem/malha.mlh2d[iEle].cel2D.vElem;
    			//holInf=0.5;
    			int indSup;
    			int indInf;
    			if(mapprop.inferior[0]==0){
    				indSup=mapprop.indProp[0];
    				indInf=mapprop.indProp[1];
    				if(iLiq==1)holGenerico=holInf;
    				else holGenerico=1.-holInf;
    			}
    			else{
       				indSup=mapprop.indProp[1];
        			indInf=mapprop.indProp[0];
    				if(iLiq==0)holGenerico=holInf;
    				else holGenerico=1.-holInf;
    			}
    			malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
				malha.mlh2d[iEle].cel2D.cond=(1.-holInf)*prop.cond[indSup]+holInf*prop.cond[indInf];
				malha.mlh2d[iEle].cel2D.cp=(1.-holInf)*prop.cp[indSup]+holInf*prop.cp[indInf];
				malha.mlh2d[iEle].cel2D.rho=(1.-holInf)*prop.rho[indSup]+holInf*prop.rho[indInf];
				malha.mlh2d[iEle].cel2D.visc=(1.-holInf)*prop.visc[indSup]+holInf*prop.visc[indInf];
				malha.mlh2d[iEle].cel2D.beta=(1.-holInf)*prop.beta[indSup]+holInf*prop.beta[indInf];
				malha.mlh2d[iEle].cel2D.tempRef=(1.-holInf)*prop.tRef[indSup]+holInf*prop.tRef[indInf];
			}
			else if(cheio==-36){
				int noExt=0;
				int noInt[2];
				double xInter[2];
				double yInter[2];
				int konta=0;
				for(int k=0;k<nvert;k++){
					if(tipoNo[k]==-4){
						noExt=k;
					}
					else{
						noInt[konta]=k;
						konta++;
					}
				}
				for(int k=0;k<nvert-1;k++){
					double ycres[2];
					double xcres[2];
					if(xN[noInt[k]]>=xN[noExt]){
						xcres[0]=xN[noExt];
						xcres[1]=xN[noInt[k]];
						ycres[0]=yN[noExt];
						ycres[1]=yN[noInt[k]];
					}
					else{
						xcres[1]=xN[noExt];
						xcres[0]=xN[noInt[k]];
						ycres[1]=yN[noExt];
						ycres[0]=yN[noInt[k]];
					}
					double a=(ycres[1]-ycres[0])/(xcres[1]-xcres[0]);
					double b=ycres[0]-xcres[0]*a;
					double xmed=0.5*(xcres[1]+xcres[0]);
					double ymed=a*xmed+b;
					xInterf[k]=zriddr(xcres[0],xcres[1],a,b,ymed);
					yInterf[k]=calcYInterUno(xInterf[k],ymed);
				}
				double xext[3];
				double yext[3];
				xext[0]=xN[noExt];
				yext[0]=yN[noExt];
				for(int k=0;k<nvert-1;k++){
					xext[k+1]=xInterf[k];
					yext[k+1]=yInterf[k];
				}
    			double subVElem=0.5*fabsl((xext[1]-xext[0])*
					(yext[2]-yext[0])-(xext[2]-xext[0])*
					(yext[1]-yext[0]));
    			double holInt=1.-subVElem/malha.mlh2d[iEle].cel2D.vElem;
    			//holInt=0.5;
    			int indExt;
    			int indInt;
    			if(mapprop.interno[0]==1){
    				indInt=mapprop.indProp[0];
    				indExt=mapprop.indProp[1];
    				if(iLiq==0)holGenerico=holInt;
    				else holGenerico=1.-holInt;
    			}
    			else{
       				indInt=mapprop.indProp[1];
        			indExt=mapprop.indProp[0];
    				if(iLiq==1)holGenerico=holInt;
    				else holGenerico=1.-holInt;
    			}
    			malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
				malha.mlh2d[iEle].cel2D.cond=(1.-holInt)*prop.cond[indExt]+holInt*prop.cond[indInt];
				malha.mlh2d[iEle].cel2D.cp=(1.-holInt)*prop.cp[indExt]+holInt*prop.cp[indInt];
				malha.mlh2d[iEle].cel2D.rho=(1.-holInt)*prop.rho[indExt]+holInt*prop.rho[indInt];
				malha.mlh2d[iEle].cel2D.visc=(1.-holInt)*prop.visc[indExt]+holInt*prop.visc[indInt];
				malha.mlh2d[iEle].cel2D.beta=(1.-holInt)*prop.beta[indExt]+holInt*prop.beta[indInt];
				malha.mlh2d[iEle].cel2D.tempRef=(1.-holInt)*prop.tRef[indExt]+holInt*prop.tRef[indInt];
			}
			else if(cheio==-64){
				int noInt;
				int noExt[2];
				double xInter[2];
				double yInter[2];
				int konta=0;
				for(int k=0;k<nvert;k++){
					if(tipoNo[k]==-3){
						noInt=k;
					}
					else{
						noExt[konta]=k;
						konta++;
					}
				}
				for(int k=0;k<nvert-1;k++){
					double ycres[2];
					double xcres[2];
					if(xN[noExt[k]]>=xN[noInt]){
						xcres[0]=xN[noInt];
						xcres[1]=xN[noExt[k]];
						ycres[0]=yN[noInt];
						ycres[1]=yN[noExt[k]];
					}
					else{
						xcres[1]=xN[noInt];
						xcres[0]=xN[noExt[k]];
						ycres[1]=yN[noInt];
						ycres[0]=yN[noExt[k]];
					}
					double a=(ycres[1]-ycres[0])/(xcres[1]-xcres[0]);
					double b=ycres[0]-xcres[0]*a;
					double xmed=0.5*(xcres[1]+xcres[0]);
					double ymed=a*xmed+b;
					xInterf[k]=zriddr(xcres[0],xcres[1],a,b,ymed);
					yInterf[k]=calcYInterUno(xInterf[k],ymed);
				}
				double xint[3];
				double yint[3];
				xint[0]=xN[noInt];
				yint[0]=yN[noInt];
				for(int k=0;k<nvert-1;k++){
					xint[k+1]=xInterf[k];
					yint[k+1]=yInterf[k];
				}
    			double subVElem=0.5*fabsl((xint[1]-xint[0])*
					(yint[2]-yint[0])-(xint[2]-xint[0])*
					(yint[1]-yint[0]));
    			double holInt=subVElem/malha.mlh2d[iEle].cel2D.vElem;
    			//holInt=0.5;
    			int indExt;
    			int indInt;
    			if(mapprop.interno[0]==1){
    				indInt=mapprop.indProp[0];
    				indExt=mapprop.indProp[1];
    				if(iLiq==0)holGenerico=holInt;
    				else holGenerico=1.-holInt;
    			}
    			else{
       				indInt=mapprop.indProp[1];
        			indExt=mapprop.indProp[0];
    				if(iLiq==1)holGenerico=holInt;
    				else holGenerico=1.-holInt;
    			}
    			malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=holGenerico;
				malha.mlh2d[iEle].cel2D.cond=(1.-holInt)*prop.cond[indExt]+holInt*prop.cond[indInt];
				malha.mlh2d[iEle].cel2D.cp=(1.-holInt)*prop.cp[indExt]+holInt*prop.cp[indInt];
				malha.mlh2d[iEle].cel2D.rho=(1.-holInt)*prop.rho[indExt]+holInt*prop.rho[indInt];
				malha.mlh2d[iEle].cel2D.visc=(1.-holInt)*prop.visc[indExt]+holInt*prop.visc[indInt];
				malha.mlh2d[iEle].cel2D.beta=(1.-holInt)*prop.beta[indExt]+holInt*prop.beta[indInt];
				malha.mlh2d[iEle].cel2D.tempRef=(1.-holInt)*prop.tRef[indExt]+holInt*prop.tRef[indInt];
			}
			if(cheio==-1 || cheio==-8 || cheio==-27 || cheio==-64/*||(cheio==-2 || cheio==-4)*/){
				double holint=holGenerico;
				int iflui=mapprop.indProp[flui];
				malha.mlh2d[iEle].cel2D.cond=prop.cond[iflui];
				malha.mlh2d[iEle].cel2D.cp=prop.cp[iflui];
				malha.mlh2d[iEle].cel2D.rho=prop.rho[iflui];
				malha.mlh2d[iEle].cel2D.visc=prop.visc[iflui];
				malha.mlh2d[iEle].cel2D.beta=prop.beta[iflui];
				malha.mlh2d[iEle].cel2D.tempRef=prop.tRef[iflui];
			}
		}
		else{
			double holint=holGenerico;
			int iflui=mapprop.indProp[0];
			malha.mlh2d[iEle].cel2D.cond=prop.cond[iflui];
			malha.mlh2d[iEle].cel2D.cp=prop.cp[iflui];
			malha.mlh2d[iEle].cel2D.rho=prop.rho[iflui];
			malha.mlh2d[iEle].cel2D.visc=prop.visc[iflui];
			malha.mlh2d[iEle].cel2D.beta=prop.beta[iflui];
			malha.mlh2d[iEle].cel2D.tempRef=prop.tRef[iflui];
		}
	}
}



void solv2D::indrazT(int& ind, double& raz) {
	for (int i = 0; i <= tempVF.parserie - 1; i++) {
		if (i < tempVF.parserie - 1) {
			if ((*vg1dSP).tempo >= tempVF.tempoDT[i] && (*vg1dSP).tempo < tempVF.tempoDT[i + 1]) {
				ind = i;
				raz = 1
						- ((*vg1dSP).tempo - tempVF.tempoDT[i])
								/ (tempVF.tempoDT[i + 1] - tempVF.tempoDT[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == tempVF.parserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void solv2D::malhaRegular(double& dx, double& dy, malha2dVF& malha){
	(*vg1dSP).xmin=malha.mlh2d[0].cel2D.centroideElem[0];
	(*vg1dSP).xmax=malha.mlh2d[0].cel2D.centroideElem[0];
	(*vg1dSP).ymin=malha.mlh2d[0].cel2D.centroideElem[1];
	(*vg1dSP).ymax=malha.mlh2d[0].cel2D.centroideElem[1];
	int nvert=3;
	for(int i=1;i<malha.nele;i++){
		for(int k=0;k<nvert;k++){
			if(malha.mlh2d[i].cel2D.coordVert[k][0]<(*vg1dSP).xmin)(*vg1dSP).xmin=malha.mlh2d[i].cel2D.coordVert[k][0];
			if(malha.mlh2d[i].cel2D.coordVert[k][0]>(*vg1dSP).xmax)(*vg1dSP).xmax=malha.mlh2d[i].cel2D.coordVert[k][0];
			if(malha.mlh2d[i].cel2D.coordVert[k][1]<(*vg1dSP).ymin)(*vg1dSP).ymin=malha.mlh2d[i].cel2D.coordVert[k][1];
			if(malha.mlh2d[i].cel2D.coordVert[k][1]>(*vg1dSP).ymax)(*vg1dSP).ymax=malha.mlh2d[i].cel2D.coordVert[k][1];
		}
	}
	(*vg1dSP).nxMH=floor(((*vg1dSP).xmax-(*vg1dSP).xmin)/dx);
	(*vg1dSP).nyMH=floor(((*vg1dSP).ymax-(*vg1dSP).ymin)/dy);
	malhaH=new detMalhaHom* [(*vg1dSP).nxMH];
	for(int i=0; i<(*vg1dSP).nxMH;i++)malhaH[i]=new detMalhaHom [(*vg1dSP).nyMH];
	for(int i=0; i<(*vg1dSP).nxMH;i++)
		for(int j=0; j<(*vg1dSP).nyMH;j++)malhaH[i][j].buraco=1;
	dx=((*vg1dSP).xmax-(*vg1dSP).xmin)/(*vg1dSP).nxMH;
	dy=((*vg1dSP).ymax-(*vg1dSP).ymin)/(*vg1dSP).nyMH;
	double xcoor=(*vg1dSP).xmin;
	double ycoor=(*vg1dSP).ymin;
	int interior=-1;
	int iele=0;
	double ylim[nvert];
	double xlim[nvert];
	////#pragma omp parallel for num_threads((*vg1dSP).ntrd)
	for(int ix=0; ix<(*vg1dSP).nxMH; ix++){
		ycoor=(*vg1dSP).ymin;
		for(int iy=0; iy<(*vg1dSP).nyMH; iy++){
			iele=0;
			interior=-1;
			while(interior==-1 && iele<malha.nele){
				int xinterno[3];
				int yinterno[3];
				for(int ivert=0;ivert<nvert;ivert++){
					xinterno[ivert]=0;
					yinterno[ivert]=0;
					if(ivert<nvert-1){
						double delx=malha.mlh2d[iele].cel2D.coordVert[ivert+1][0]-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dely=malha.mlh2d[iele].cel2D.coordVert[ivert+1][1]-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						double dxteste=xcoor-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dyteste=ycoor-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						if(fabsl(delx)>1e-15)
							ylim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][1]+(dely/delx)*dxteste;
						else ylim[ivert]=1e20;
						if(fabsl(dely)>1e-15)
							xlim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][0]+(delx/dely)*dyteste;
						else xlim[ivert]=1e20;
						if((ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert+1][1]) ||
						   (ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert+1][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][1]))
							yinterno[ivert]=1;
						if((xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert+1][0]) ||
						   (xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert+1][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][0]))
							xinterno[ivert]=1;
					}
					else{
						double delx=malha.mlh2d[iele].cel2D.coordVert[0][0]-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dely=malha.mlh2d[iele].cel2D.coordVert[0][1]-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						double dxteste=xcoor-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dyteste=ycoor-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						if(fabsl(dely)>1e-15)
							ylim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][1]+(dely/delx)*dxteste;
						else ylim[ivert]=1e20;
						if(fabsl(dely)>1e-15)
							xlim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][0]+(delx/dely)*dyteste;
						else xlim[ivert]=1e20;
						if((ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[0][1]) ||
						   (ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[0][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][1]))
							yinterno[ivert]=1;
						if((xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[0][0]) ||
						   (xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[0][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][0]))
							xinterno[ivert]=1;
					}
				}
				int totalxyinterno=0;
				for(int ivert=0;ivert<nvert;ivert++){
					totalxyinterno+=(yinterno[ivert]+xinterno[ivert]);

				}
				if(totalxyinterno==4){
					malhaH[ix][iy].buraco=0;
					malhaH[ix][iy].indEle=iele;
					malhaH[ix][iy].xcoor=xcoor;
					malhaH[ix][iy].ycoor=ycoor;
					malhaH[ix][iy].delx=xcoor-malha.mlh2d[iele].cel2D.centroideElem[0];
					malhaH[ix][iy].dely=ycoor-malha.mlh2d[iele].cel2D.centroideElem[1];
					interior=0;
				}
				iele++;
				if(iele==malha.nele){
					int para;
					para=0;
				}
			}
			ycoor+=dy;
		}
		xcoor+=dx;
	}
	//for(int ix=0;ix<nx;ix++){
		//for(int iy=0;iy<ny;iy++){

		//}
	//}
}

void solv2D::imprimeMalhaRegular(malha2dVF& malha, int var){
	FullMtx<double> flut((*vg1dSP).nxMH,(*vg1dSP).nyMH);
	int numero = roundl((*vg1dSP).tempo);
	double ymaxWall=malha.mlh2d[(*vg1dSP).eleYmaxWall].cel2D.centroideElem[1];
	if(var==1){
		#pragma omp parallel for num_threads(nthrdLocal)
		for(int i=0;i<(*vg1dSP).nxMH;i++){
			for(int j=0;j<(*vg1dSP).nyMH;j++){
				if(malhaH[i][j].buraco==0){
					double dh=ymaxWall-malhaH[i][j].ycoor;
					double forcaCorpo=dh*(malha.mlh2d[malhaH[i][j].indEle].cel2D.rho)*(*vg1dSP).gravVF*sinl((*vg1dSP).angY);
					flut[i][j]=(malha.mlh2d[malhaH[i][j].indEle].cel2D.presC+
							0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenPres[0]+
							0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenPres[1]+
							(1.-(*vg1dSP).mulFC)*forcaCorpo)/98066.52;
				}
				else flut[i][j]=0.;
			}
		}
		ostringstream saida;
		saida << pathPrefixoArqSaida << "PerfisPresRegular" << "-" << numero
					<< ".dat";
		string tmp = saida.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << flut;
		escreveIni.close();
	}
	else if(var==2){
        #pragma omp parallel for num_threads(nthrdLocal)
		for(int i=0;i<(*vg1dSP).nxMH;i++){
			for(int j=0;j<(*vg1dSP).nyMH;j++){
				if(malhaH[i][j].buraco==0){
					flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.uC+
						0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenU[0]+
						0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenU[1];
				}
				else flut[i][j]=0.;
			}
		}
		ostringstream saida;
		saida << pathPrefixoArqSaida << "PerfisURegular" << "-" << numero
					<< ".dat";
		string tmp = saida.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << flut;
		escreveIni.close();
	}
	else if(var==3){
		#pragma omp parallel for num_threads(nthrdLocal)
		for(int i=0;i<(*vg1dSP).nxMH;i++){
			for(int j=0;j<(*vg1dSP).nyMH;j++){
				if(malhaH[i][j].buraco==0){
					flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.vC+
						0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenV[0]+
						0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenV[1];
				}
				else flut[i][j]=0.;
			}
		}
		ostringstream saida;
		saida << pathPrefixoArqSaida << "PerfisVRegular" << "-" << numero
					<< ".dat";
		string tmp = saida.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << flut;
		escreveIni.close();
	}
	else if(var==4){
		#pragma omp parallel for num_threads(nthrdLocal)
		for(int i=0;i<(*vg1dSP).nxMH;i++){
			for(int j=0;j<(*vg1dSP).nyMH;j++){
				if(malhaH[i][j].buraco==0){
					flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.tempC+
						0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenTemp[0]+
						0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenTemp[1];
				}
				else flut[i][j]=0.;
			}
		}
		ostringstream saida;
		saida << pathPrefixoArqSaida << "PerfisTempRegular" << "-" << numero
					<< ".dat";
		string tmp = saida.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << flut;
		escreveIni.close();
	}
	else if(var==5){
		#pragma omp parallel for num_threads(nthrdLocal)
		for(int i=0;i<(*vg1dSP).nxMH;i++){
			for(int j=0;j<(*vg1dSP).nyMH;j++){
				if(malhaH[i][j].buraco==0){
					double utemp=malha.mlh2d[malhaH[i][j].indEle].cel2D.uC+
						0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenU[0]+
						0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenU[1];
					double vtemp=malha.mlh2d[malhaH[i][j].indEle].cel2D.vC+
						0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenV[0]+
						0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenV[1];
					flut[i][j]=sqrtl(utemp*utemp+vtemp*vtemp);
				}
				else flut[i][j]=0.;
			}
		}
		ostringstream saida;
		saida << pathPrefixoArqSaida << "PerfisModVelRegular" << "-" << numero
					<< ".dat";
		string tmp = saida.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << flut;
		escreveIni.close();
	}
	else if(var==6){
		#pragma omp parallel for num_threads(nthrdLocal)
		for(int i=0;i<(*vg1dSP).nxMH;i++){
			for(int j=0;j<(*vg1dSP).nyMH;j++){
				if(malhaH[i][j].buraco==0){
					flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.holC+
						0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenHol[0]+
						0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenHol[1];
				}
				else flut[i][j]=0.;
			}
		}
		ostringstream saida;
		saida << pathPrefixoArqSaida << "PerfisHoldupRegular" << "-" << numero
					<< ".dat";
		string tmp = saida.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << flut;
		escreveIni.close();
	}
}


double solv2D::fluxMed(malha2dVF& malha){
	double varTemp=0.;
	double comprimento=0;
#pragma omp parallel for reduction(+:varTemp,comprimento) num_threads(nthrdLocal)
	for(int i=0;i<malha.nele;i++){
	    for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    	if(malha.mlh2d[i].kvizinho[j]<0){
				int kcc=0;
				while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
					//varTemp+=malha.mlh2d[i].cel2D.tempC*malha.mlh2d[i].cel2D.sFaceMod[j];
					varTemp+=malha.mlh2d[i].ccTVN[j]*malha.mlh2d[i].cel2D.sFaceMod[j];
					comprimento+=malha.mlh2d[i].cel2D.sFaceMod[j];
				}
	    	}
	    }

	}

	//return varTemp/(dutosMRT.a*M_PI);
	return varTemp/comprimento;
}
double solv2D::tempMed(malha2dVF& malha){
	double varTemp=0.;
	double varCapTerm=0;
#pragma omp parallel for reduction(+:varTemp,varCapTerm) num_threads(nthrdLocal)
	for(int i=1;i<malha.nele;i++){
		double capLocal=malha.mlh2d[i].cel2D.rho*malha.mlh2d[i].cel2D.cp*malha.mlh2d[i].cel2D.vElem;
		varTemp+=capLocal*(malha.mlh2d[i].cel2D.tempC+273.16);
		varCapTerm+=capLocal;
	}

	return varTemp/varCapTerm-273.16;
}
double solv2D::viscMed(malha2dVF& malha){
	double varTemp=0.;
	double varCapTerm=0;
#pragma omp parallel for reduction(+:varTemp,varCapTerm) num_threads(nthrdLocal)
	for(int i=1;i<malha.nele;i++){
		double capLocal=malha.mlh2d[i].cel2D.vElem;
		varTemp+=capLocal*(malha.mlh2d[i].cel2D.visc);
		varCapTerm+=capLocal;
	}

	return varTemp/varCapTerm;
}
double solv2D::taxaMed(malha2dVF& malha){
	double varTemp=0.;
	double varCapTerm=0;
#pragma omp parallel for reduction(+:varTemp,varCapTerm) num_threads(nthrdLocal)
	for(int i=1;i<malha.nele;i++){
		double capLocal=malha.mlh2d[i].cel2D.vElem;
		varTemp+=capLocal*(malha.mlh2d[i].cel2D.taxaDeform);
		varCapTerm+=capLocal;
	}

	return varTemp/varCapTerm;
}
double solv2D::velMed(malha2dVF& malha){
	double varTemp=0.;
	double varCapTerm=0;
#pragma omp parallel for reduction(+:varTemp,varCapTerm) num_threads(nthrdLocal)
	for(int i=1;i<malha.nele;i++){
		double capLocal=malha.mlh2d[i].cel2D.vElem;
		varTemp+=capLocal*sqrtl(malha.mlh2d[i].cel2D.uC*malha.mlh2d[i].cel2D.uC+malha.mlh2d[i].cel2D.vC*malha.mlh2d[i].cel2D.vC);
		varCapTerm+=capLocal;
	}

	return varTemp/varCapTerm;
}


void solv2D::paredeContorno(malha2dVF& malha, unordered_map<int, int> & indPar, unordered_map<int, int>& indPar2, int& nPar){
	nPar=0;
	for(int i=0;i<malha.nele;i++){
	    for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    	if(malha.mlh2d[i].kvizinho[j]<0){
				int kcc=0;
				while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
					indPar[i]=nPar;
					indPar2[nPar]=i;
					nPar++;
				}
	    	}
	    }

	}
}

double solv2D::tempMedParede(malha2dVF& malha){
	double varTemp=0.;
	double comprimento=0;
#pragma omp parallel for reduction(+:varTemp,comprimento) num_threads(nthrdLocal)
	for(int i=0;i<malha.nele;i++){
	    for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    	if(malha.mlh2d[i].kvizinho[j]<0){
				int kcc=0;
				while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
					//varTemp+=malha.mlh2d[i].cel2D.tempC*malha.mlh2d[i].cel2D.sFaceMod[j];
					varTemp+=malha.mlh2d[i].tempF[j]*malha.mlh2d[i].cel2D.sFaceMod[j];
					comprimento+=malha.mlh2d[i].cel2D.sFaceMod[j];
				}
	    	}
	    }

	}

	//return varTemp/(dutosMRT.a*M_PI);
	return varTemp/comprimento;
}

double solv2D::areaMedParede(malha2dVF& malha, double& condMed){
	double varArea=0.;
	condMed=0;
#pragma omp parallel for reduction(+:varArea,condMed) num_threads(nthrdLocal)
	for(int i=0;i<malha.nele;i++){
	    for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    	if(malha.mlh2d[i].kvizinho[j]<0){
				int kcc=0;
				while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
					//varTemp+=malha.mlh2d[i].cel2D.tempC*malha.mlh2d[i].cel2D.sFaceMod[j];
					varArea+=0.5*malha.mlh2d[i].cel2D.vElem;
					condMed+=malha.mlh2d[i].cel2D.vElem*malha.mlh2d[i].cel2D.cond;
				}
	    	}
	    }

	}
	condMed/=(2*varArea);

	//return varTemp/(dutosMRT.a*M_PI);
	return varArea;
}

void solv2D::resolve(){

	  string elearq;
	  string noarq;
	  string polyarq;


	  FILE *fp = fopen(nomeArquivoEntrada.c_str(), "r");
		//FILE *fp = fopen("/home/bwz1/teste2.json", "r");
	  char readBuffer[125536];
	  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	  Document elementoRaiz;
	  elementoRaiz.ParseStream(is);
	  fclose(fp);

	  double dx;
      double dy;
	  assert(elementoRaiz.HasMember("malha"));

	  if (elementoRaiz.HasMember("malha")){
				parse_malha(elementoRaiz["malha"],dx,dy,elearq,noarq,polyarq);
	  } else {
				exit(-1);
	  }
	  parse_tempo(elementoRaiz["configuracaoInicial"]);
	  parse_CC(elementoRaiz["CC"]);
	  parse_Prop(elementoRaiz["prop"]);
	  parse_CI(elementoRaiz["CI"]);
	  if(elementoRaiz.HasMember("interface") && prop.parserie>1){
		  parse_Interface(elementoRaiz["interface"]);
		  (*vg1dSP).temInterface=1;
	  }
	  else{
		  interface.nserie=0;
		  interface.xcoor=0;
		  interface.ycoor=0;
	  }
	  if(elementoRaiz.HasMember("mapProp"))
		  parse_mapProp(elementoRaiz["mapProp"]);
	  else{
			mapprop.parserie =1;
			mapprop.indProp=0;
			mapprop.interno=0;
			mapprop.inferior=0;
	  }
	  (*vg1dSP).iliq=0;
	  (*vg1dSP).igas=1;
	  if((*vg1dSP).temInterface==1&&prop.rho[0]<prop.rho[1]){
		  (*vg1dSP).iliq=1;
		  (*vg1dSP).igas=0;
	  }
	  else if((*vg1dSP).temInterface==0){
		  (*vg1dSP).iliq=0;
		  (*vg1dSP).igas=0;
	  }
	  int viscVar=0;
	  if(elementoRaiz.HasMember("fluido")){
		  viscVar=1;
		  parse_fluido_complementar(elementoRaiz["fluido"]);
	  }
	  flucVF.cond=prop.cond[(*vg1dSP).iliq];
	  flucVF.cp=prop.cp[(*vg1dSP).iliq];
	  flucVF.rholStd=prop.rho[(*vg1dSP).iliq];
	  flucVF.compresT=prop.beta[(*vg1dSP).iliq];
	  if((*vg1dSP).temInterface==1){
		  flucVF.rhog=prop.rho[(*vg1dSP).igas];
	  }
	  double fluxcal=0.;
	  DadosGeo dutosMRT;
	  TransCal transfer;
	  TransCal transfer2;
	  if((*vg1dSP).acop==1){
		  parse_materiais(elementoRaiz["material"]);
		  /*parse_fluido_complementar(elementoRaiz["fluido"]);
		  fluc.cond=prop.cond[(*vg1dSP).iliq];
		  fluc.cp=prop.cp[(*vg1dSP).iliq];
		  fluc.rholStd=prop.rho[(*vg1dSP).iliq];
		  fluc.compresT=prop.beta[(*vg1dSP).iliq];
		  if((*vg1dSP).temInterface==1){
			  fluc.rhog=prop.rho[(*vg1dSP).igas];
		  }*/
		  parse_corte(elementoRaiz["secaoTransversal"]);
		  ////////////////////////////////////////////////////////////////////////
		  int cam = corteVF.ncam;
		  double* vk;
		  vk = new double[cam];
		  for (int j = 0; j < cam; j++)
				vk[j] = matVF[corteVF.indmat[j]].cond;
		  double* vcp;
		  vcp = new double[cam];
		  for (int j = 0; j < cam; j++)
				vcp[j] = matVF[corteVF.indmat[j]].cp;
		  double* vrhoc;
		  vrhoc = new double[cam];
		  for (int j = 0; j < cam; j++)
				vrhoc[j] = matVF[corteVF.indmat[j]].rho;
		  double* vvisc;
		  vvisc = new double[cam];
		  for (int j = 0; j < cam; j++)
				vvisc[j] = matVF[corteVF.indmat[j]].visc;
		  double* vbeta;
		  vbeta = new double[cam];
		  for (int j = 0; j < cam; j++)
				vbeta[j] = matVF[corteVF.indmat[j]].beta;
		  int* vtipomat;
		  vtipomat = new int[cam];
		  for (int j = 0; j < cam; j++)
				vtipomat[j] = matVF[corteVF.indmat[j]].tipo;
		  int* vindmat;
		  vindmat = new int[cam];
		  for (int j = 0; j < cam; j++)
				vindmat[j] = corteVF.indmat[j];

		  dutosMRT = DadosGeo(corteVF.a, corteVF.b,	0., corteVF.rug, corteVF.anul,
					corteVF.ncam, vk, corteVF.diam, vcp, vrhoc, vvisc,
					vbeta,vtipomat,vindmat);
		  delete[] vk;
		  delete[] vcp;
		  delete[] vrhoc;
		  delete[] vvisc;
		  delete[] vbeta;
		  delete[] vtipomat;
		  delete[] vindmat;
		  //////////////////////////////////////////////////////////////////////////////////////////
		  int* vncamada;
			vncamada = new int[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				if (dutosMRT.tipomat[i] != 0)
					vncamada[i] = 1;
				else
					vncamada[i] = corteVF.discre[i];
			}
			double* vdrcamada;
			vdrcamada = new double[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				if (i > 0)
					vdrcamada[i] = 0.5 * (dutosMRT.diamC[i] - dutosMRT.diamC[i - 1])
							/ vncamada[i];
				else
					vdrcamada[0] = 0.5 * (dutosMRT.diamC[0] - dutosMRT.a) / vncamada[0];
			}
			double** vTcamada;
			vTcamada = new double*[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				vTcamada[i] = new double[vncamada[i] + 1];
			}

			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				for (int j = 0; j <= vncamada[i]; j++)
					//vTcamada[i][j] = (*vg1dSP).tAmb;
					vTcamada[i][j] = CI.temp[0];
			}
			int perm = -1;

			int kL = -1;  //alteracao2

			double tInt=CI.temp[0];
			double pInt=tempVF.pRef;
			double ki=flucVF.CondLiq(pInt, tInt);
			double cpi=flucVF.CalorLiq(pInt, tInt);
			double rhoi=flucVF.MasEspFlu(pInt, tInt);
			double visci;
			if(viscVar==1){
				  if(flucVF.naoN==0) visci=flucVF.VisFlu(pInt, tInt)/1000.;
				  else{
					  double taxaDef=flucVF.viscNaoNew[0][1];
					  visci=flucVF.VisFlu(pInt, tInt,taxaDef)/1000.;
				  }
			}
			else visci=prop.visc[(*vg1dSP).iliq];
			if((*vg1dSP).temInterface==1){
				if(viscVar==1){
					double visciG=flucVF.VisGas(pInt, tInt)/1000.;
					prop.visc[(*vg1dSP).igas]=visciG;
				}
			}

			transfer = TransCal(vg1dSP,dutosMRT, perm, vncamada, vdrcamada, vTcamada,
					tInt, (*vg1dSP).tAmb, 0., 0, (*vg1dSP).vAmb, 0, 1, ki, cpi, rhoi, visci, 0.,
					0., 0., 0., 0, (*vg1dSP).amb, 0,
					0., 0., 0., 1.); // alteracao2 // alteracao0
			transfer2 = TransCal(vg1dSP,dutosMRT, perm, vncamada, vdrcamada, vTcamada,
					tInt, (*vg1dSP).tAmb, 0., 0, (*vg1dSP).vAmb, 0, 1, ki, cpi, rhoi, visci, 0.,
					0., 0., 0., 0, (*vg1dSP).amb, 0,
					0., 0., 0., 1.); // alteracao2 // alteracao0
			transfer.condiTparede=1;
			transfer2.condiTparede=1;
			if (equilterm == 1){
				transfer.transperm();
				transfer2.transperm();
			}
			else{
				transfer2.transperm();
			}
			transfer2.condiTparede=0;
			delete[] vncamada;
			delete[] vdrcamada;
			for (int i = 0; i < dutosMRT.ncamadas; i++)
				delete[] vTcamada[i];
			delete[] vTcamada;
			/////////////////////////////////////////////////////////////////////////////////////////////////////

			//fluxcal=transfer.transtrans()/(M_PI*dutosMRT.a);
			if(equilterm==1)fluxcal=transfer.fluxFim/(M_PI*dutosMRT.a);
			else fluxcal=0.;

	  }
	  //if(elementoRaiz.HasMember("relax"))
			//relax =elementoRaiz["relax"].GetDouble();
	  //else relax=0.001;


  /*string impfile;
  impfile="parametros.txt";
  ifstream lendoimp(impfile.c_str(), ios_base::in);
  string elefile,nofile,volfile;
  lendoimp>>elefile;//nome do arquivo com os dados dos elementos
  lendoimp>>nofile;//nome do arquivo com os dados dos n�s discretizados
  lendoimp.close();

  //elefile="elemento.ele";
  ifstream lendoele(elefile.c_str(), ios_base::in);
  //nofile="no.node";
  ifstream lendono(nofile.c_str(), ios_base::in);*/


	/*string elefile,nofile,volfile,polyfile;
	elefile=elearq;
	ifstream lendoele(elefile.c_str(), ios_base::in);
	nofile=noarq;
	ifstream lendono(nofile.c_str(), ios_base::in);
	polyfile=polyarq;
	ifstream lendopoly(polyfile.c_str(), ios_base::in);


  string val;
  int nno=0;//vari�vel inteira indicando o n�mero de n�s da malha
  lendono>>nno;
  lendono>>val;
  lendono>>val;
  lendono>>val;

  double** xcoor;//vetor de coordenadas de cada n�, em cada termo,
  //teremos os valores de x e y para cada n�, por exemplo, para o n� 53:
  //x=xcoor[53][0] e y=xcoor[53][1]
  xcoor = new double* [nno];
  int nvert=3;
  for(int i=0;i<nno;i++)xcoor[i]=new double [nvert-1];
  double* atributo;
  atributo = new double [nno];
  int* tipo;
  tipo = new int [nno];

  for(int j=0;j<nno;j++){
      lendono>>val;
      lendono>>xcoor[j][0];
      lendono>>xcoor[j][1];
      lendono>>atributo[j];
      lendono>>tipo[j];
  }

  lendono.close();

  int nele=0;
  lendoele>>nele;
  lendoele>>val;
  lendoele>>val;

  int** noEle;
  noEle = new int* [nele];
  for(int i=0;i<nele;i++){
  	noEle[i]=new int [nvert];
  }

  for(int j=0;j<nele;j++){
      lendoele>>val;
      for(int k=0;k<nvert;k++){
      	lendoele>>noEle[j][k];
      	noEle[j][k]--;
      }
  }
  lendoele.close();

  int npoly;
  int nburaco=0;
  for(int i=0;i<4;i++)lendopoly>>val;
  lendopoly>>npoly;
  lendopoly>>val;
  for(int i=0;i<npoly;i++){
  	for(int i=0;i<4;i++)lendopoly>>val;
  }
  lendopoly>>nburaco;

  double** noBuraco;
  noBuraco = new double* [nburaco];
  for(int i=0;i<nburaco;i++)noBuraco[i]=new double [nvert-1];

  for(int j=0;j<nburaco;j++){
      lendopoly>>val;
      for(int k=0;k<nvert-1;k++){
      	lendopoly>>noBuraco[j][k];
      }
  }
  lendopoly.close();*/
	int nno=0;
	int nele=0;
	double** xcoor;
	double* atributo;
	int* tipo;
	int** noEle;
	int nvert=3;
	if(unv==0){
		string elefile,nofile,volfile,polyfile;
		elefile=elearq;
		ifstream lendoele(elefile.c_str(), ios_base::in);
		nofile=noarq;
		ifstream lendono(nofile.c_str(), ios_base::in);
			//polyfile=polyarq;
			//ifstream lendopoly(polyfile.c_str(), ios_base::in);


		string val;
		lendono>>nno;
		lendono>>val;
		lendono>>val;
		lendono>>val;

			//double** xcoor;//vetor de coordenadas de cada n�, em cada termo,
			//teremos os valores de x e y para cada n�, por exemplo, para o n� 53:
			//x=xcoor[53][0] e y=xcoor[53][1]
		xcoor = new double* [nno];
		for(int i=0;i<nno;i++)xcoor[i]=new double [nvert-1];
			//double* atributo;
		atributo = new double [nno];
			//int* tipo;
		tipo = new int [nno];

		for(int j=0;j<nno;j++){
			lendono>>val;
			lendono>>xcoor[j][0];
			lendono>>xcoor[j][1];
			lendono>>atributo[j];
			lendono>>tipo[j];
		}

		lendono.close();

		lendoele>>nele;
		lendoele>>val;
		lendoele>>val;

		noEle = new int* [nele];
		for(int i=0;i<nele;i++){
			noEle[i]=new int [nvert];
		}

		for(int j=0;j<nele;j++){
			lendoele>>val;
			for(int k=0;k<nvert;k++){
				lendoele>>noEle[j][k];
				noEle[j][k]--;
			}
		}
		lendoele.close();
	}
	else{
		vector<string> strCC;
		string eleUNV;
		eleUNV=elearq;
		ifstream lendoUNV(eleUNV.c_str(), ios_base::in);

		string chave;
		char* tenta;
		//tenta = new char[400];
		struct cartesiano{
			int ind;
			double x[3];
			int tipo;
			int vertice;
			string condCont;
		};
		vector<cartesiano> vecXY;

		double testatok;
		char line[4000];
		lendoUNV.get(line, 4000);
		tenta = strtok(line, " ,()=");
		lendoUNV >> chave;
		while (chave != "2411") {
			lendoUNV >> chave;
		}

		double chaveN[3]={0,0,0};

		cartesiano temp;
		for(int i=0;i<3;i++)lendoUNV>>chaveN[i];
		temp.ind=chaveN[0];
		temp.tipo=0;
		temp.condCont="interno";
		while(chaveN[0]!=-1 && chaveN[1]!=-1 && chaveN[2]!=2412){

			for(int i=1;i<3;i++){
				chaveN[i-1]=chaveN[i];
			}
			lendoUNV>>chaveN[2];
			for(int i=0;i<3;i++)lendoUNV>>temp.x[i];
			vecXY.push_back(temp);
			for(int i=0;i<3;i++)lendoUNV>>chaveN[i];
			temp.ind=chaveN[0];
			temp.tipo=0;
			temp.vertice=0;
			temp.condCont="interno";
		}

		struct faceNo{
			int ind;
			int no[2];
		};
		vector<faceNo> faces;
		faceNo tempFace;

		int chaveFace[8]={0,0,0,0,0,0,0,0};
		int chaveInt;
		while(chaveFace[4]!=3){
			lendoUNV>>tempFace.ind;
			for(int i=0;i<8;i++)lendoUNV>>chaveFace[i];
			if(chaveFace[4]!=3){
				for(int i=0;i<2;i++)lendoUNV>>tempFace.no[i];
				faces.push_back(tempFace);
			}
		}

		struct eleNo{
			int ind;
			int no[3];
		};
		vector<eleNo> elemento;
		eleNo tempEle;

		tempEle.ind=tempFace.ind;
		for(int i=0;i<3;i++)tempEle.no[i]=chaveFace[i+5];
		elemento.push_back(tempEle);
		int chaveEle[5]={0,0,0,0,0};
		lendoUNV>>tempEle.ind;
		for(int i=0;i<2;i++) lendoUNV>>chaveEle[i];
		while(tempEle.ind!=-1 && chaveEle[0]!=-1 && chaveEle[1]!=2467){
			for(int i=2;i<5;i++)lendoUNV>>chaveEle[i];
			for(int i=0;i<3;i++)lendoUNV>>tempEle.no[i];
			elemento.push_back(tempEle);
			lendoUNV>>tempEle.ind;
			for(int i=0;i<2;i++) lendoUNV>>chaveEle[i];
		}
		string condicaoContorno;

		lendoUNV>>chaveInt;
		int inCC=1;
		while(chaveInt!=-1){
			for(int i=0;i<7;i++){
				lendoUNV>>chaveInt;
			}
			//lendoUNV>>chaveInt;
			lendoUNV>>condicaoContorno;
			strCC.push_back(condicaoContorno);
			int indCont;
			int konta=0;
			int nface=faces.size();
			while(konta<chaveInt){
				lendoUNV>>indCont;
				lendoUNV>>indCont;
				for(int i=0;i<nface;i++){
					int queNo0=-1;
					int queNo1=-1;
					if(faces[i].ind==indCont){
						queNo0=faces[i].no[0];
						queNo1=faces[i].no[1];
						if(vecXY[queNo0].tipo==0 && vecXY[queNo1].tipo==0){
							vecXY[queNo0].vertice=0;
							vecXY[queNo1].vertice=0;
							vecXY[queNo0].tipo=inCC;
							vecXY[queNo0].condCont=condicaoContorno;
							vecXY[queNo1].tipo=inCC;
							vecXY[queNo1].condCont=condicaoContorno;
						}
						else if(vecXY[queNo0].tipo==0 && vecXY[queNo1].tipo>0){
							if(vecXY[queNo1].tipo==inCC){
								vecXY[queNo0].vertice=0;
								vecXY[queNo1].vertice=0;
								vecXY[queNo0].tipo=inCC;
								vecXY[queNo0].condCont=condicaoContorno;
							}
							else{
								vecXY[queNo0].vertice=0;
								vecXY[queNo1].vertice=1;
								vecXY[queNo0].tipo=inCC;
								vecXY[queNo0].condCont=condicaoContorno;
							}
						}
						else if(vecXY[queNo1].tipo==0 && vecXY[queNo0].tipo>0){
							if(vecXY[queNo0].tipo==inCC){
								vecXY[queNo1].vertice=0;
								vecXY[queNo0].vertice=0;
								vecXY[queNo1].tipo=inCC;
								vecXY[queNo1].condCont=condicaoContorno;
							}
							else{
								vecXY[queNo1].vertice=0;
								vecXY[queNo0].vertice=1;
								vecXY[queNo1].tipo=inCC;
								vecXY[queNo1].condCont=condicaoContorno;
							}
						}
					}
					if(queNo0>=0 && queNo1>=0)break;
				}
				for(int i=0;i<2;i++)lendoUNV>>indCont;
				konta++;
			}
			lendoUNV>>chaveInt;
			inCC++;
		}

		int nstr=strCC.size();
		for(int i=0; i<CC.nDiri; i++){
			for(int j=0;j<nstr;j++){
				if(CC.ccDir[i].rotuloSTR==strCC[j]){
					CC.ccDir[i].rotulo=j+1;
					break;
				}
			}
		}
		for(int i=0; i<CC.nVN; i++){
			for(int j=0;j<nstr;j++){
				if(CC.ccVN[i].rotuloSTR==strCC[j]){
					CC.ccVN[i].rotulo=j+1;
					break;
				}
			}
		}
		for(int i=0; i<CC.nRic; i++){
			for(int j=0;j<nstr;j++){
				if(CC.ccRic[i].rotuloSTR==strCC[j]){
					CC.ccRic[i].rotulo=j+1;
					break;
				}
			}
		}
		for(int i=0; i<CC.nInl; i++){
			for(int j=0;j<nstr;j++){
				if(CC.ccInl[i].rotuloSTR==strCC[j]){
					CC.ccInl[i].rotulo=j+1;
					break;
				}
			}
		}
		for(int i=0; i<CC.nOut; i++){
			for(int j=0;j<nstr;j++){
				if(CC.ccPres[i].rotuloSTR==strCC[j]){
					CC.ccPres[i].rotulo=j+1;
					break;
				}
			}
		}
		for(int i=0; i<CC.nSim; i++){
			for(int j=0;j<nstr;j++){
				if(CC.ccSim[i].rotuloSTR==strCC[j]){
					CC.ccSim[i].rotulo=j+1;
					break;
				}
			}
		}
		for(int i=0; i<CC.nWall; i++){
			for(int j=0;j<nstr;j++){
				if(CC.ccWall[i].rotuloSTR==strCC[j]){
					CC.ccWall[i].rotulo=j+1;
					break;
				}
			}
		}

		int nno=vecXY.size();
		xcoor = new double* [nno];
		for(int i=0;i<nno;i++)xcoor[i]=new double [nvert-1];
		atributo = new double [nno];
		tipo = new int [nno];

		for(int j=0;j<nno;j++){
		      xcoor[j][0]=vecXY[j].x[0];
		      xcoor[j][1]=vecXY[j].x[1];
		      atributo[j]=vecXY[j].vertice;
		      tipo[j]=vecXY[j].tipo;
		}

		int nele=elemento.size();

		noEle = new int* [nele];
		for(int i=0;i<nele;i++){
			noEle[i]=new int [nvert];
		}

		for(int j=0;j<nele;j++){
		    for(int k=0;k<nvert;k++){
		        noEle[j][k]=elemento[j].no[nvert-1-k];
		        noEle[j][k]--;
		    }
		}

		lendoUNV.close();
		//delete [] tenta;
	}



  (*vg1dSP).tempo=0.;
  double holdupGlob=0.;
  double areaTotal=0.;
  malha2dVF malha(xcoor, noEle, tipo, atributo, nvert, nele, nno,tempVF.dtmax[0],tempVF.perm,tempVF.trans,vg1dSP,flucVF);
	int confinado=tempVF.confinado;
	double yMaxWall=-10000000.;
  for(int i=0;i<malha.nele;i++){
  	if(malha.mlh2d[i].cel2D.centroideElem[1]>=yMaxWall){
  		yMaxWall=malha.mlh2d[i].cel2D.centroideElem[1];
  		(*vg1dSP).eleYmaxWall=i;
  	}
  }
  if((*vg1dSP).temInterface==1){
  	iniciaProp(malha);
  	for(int i=0;i<malha.nele;i++){
  		holdupGlob+=malha.mlh2d[i].cel2D.holC*malha.mlh2d[i].cel2D.vElem;
  		areaTotal+=malha.mlh2d[i].cel2D.vElem;
  	}
  	holdupGlob/=areaTotal;

		transfer2.kint =transfer.kint =holdupGlob*prop.cond[(*vg1dSP).iliq]+(1.-holdupGlob)*prop.cond[(*vg1dSP).igas];
		transfer2.cpint =transfer.cpint = holdupGlob*prop.cp[(*vg1dSP).iliq]+(1.-holdupGlob)*prop.cp[(*vg1dSP).igas];
		transfer2.rhoint =transfer.rhoint = holdupGlob*prop.rho[(*vg1dSP).iliq]+(1.-holdupGlob)*prop.rho[(*vg1dSP).igas];
		transfer2.viscint =transfer.viscint = holdupGlob*prop.visc[(*vg1dSP).iliq]+(1.-holdupGlob)*prop.visc[(*vg1dSP).igas];
		transfer2.betint=transfer.betint= holdupGlob*prop.beta[(*vg1dSP).iliq]+(1.-holdupGlob)*prop.beta[(*vg1dSP).igas];

		//transfer.transperm();
		//transfer2.transperm();
  }
  else{
  	for(int iEle=0;iEle<malha.nele;iEle++){
  			malha.mlh2d[iEle].cel2D.holC=malha.mlh2d[iEle].cel2D.holC0=1.;
  			malha.mlh2d[iEle].cel2D.cond=prop.cond[0];
  			malha.mlh2d[iEle].cel2D.cp=prop.cp[0];
  			malha.mlh2d[iEle].cel2D.rho=prop.rho[0];
  			malha.mlh2d[iEle].cel2D.visc=prop.visc[0];
  			malha.mlh2d[iEle].cel2D.beta=prop.beta[0];
  			malha.mlh2d[iEle].cel2D.tempRef=prop.tRef[0];
  	}
  }
  for(int i=0;i<malha.nele;i++){
  	/*for(int j=0; j<prop.parserie;j++){
  		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
  		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
  		if((xc<=prop.xmax[j] && xc>=prop.(*vg1dSP).xmin[j])&&(yc<=prop.ymax[j] && yc>=prop.ymin[j])){
  			malha.mlh2d[i].cel2D.cond=prop.cond[j];
  			malha.mlh2d[i].cel2D.cp=prop.cp[j];
  			malha.mlh2d[i].cel2D.rho=prop.rho[j];
  			malha.mlh2d[i].cel2D.visc=prop.visc[j];
  			malha.mlh2d[i].cel2D.beta=prop.beta[j];
  			malha.mlh2d[i].cel2D.tempRef=prop.tRef[j];
  			break;
  		}
  	}*/
  	for(int j=0; j<CI.parserie;j++){
  		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
  		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
  		if((xc<=CI.xmax[j] && xc>=CI.xmin[j])&&(yc<=CI.ymax[j] && yc>=CI.ymin[j])){
  			if((*vg1dSP).acop==0){
  				malha.mlh2d[i].cel2D.tempC=CI.temp[j];
  				malha.mlh2d[i].cel2D.tempCI=CI.temp[j];
  				malha.mlh2d[i].cel2D.tempC0=CI.temp[j];
  			}
  			else{
  				malha.mlh2d[i].cel2D.tempC=CI.temp[0];
  				malha.mlh2d[i].cel2D.tempCI=CI.temp[0];
  				malha.mlh2d[i].cel2D.tempC0=CI.temp[0];
  			}
  			malha.mlh2d[i].cel2D.uC=CI.valU[j];
  			malha.mlh2d[i].cel2D.uCI=CI.valU[j];
  			malha.mlh2d[i].cel2D.uC0=CI.valU[j];
  			malha.mlh2d[i].cel2D.vC=CI.valV[j];
  			malha.mlh2d[i].cel2D.vCI=CI.valV[j];
  			malha.mlh2d[i].cel2D.vC0=CI.valV[j];
  			double dh=yMaxWall-malha.mlh2d[i].cel2D.centroideElem[1];
  			double forcaCorpo=dh*(*vg1dSP).gravVF*sinl((*vg1dSP).angY)*
  					malha.mlh2d[i].cel2D.rho*((*vg1dSP).mulFC-1.*malha.mlh2d[i].cel2D.beta*
  							(malha.mlh2d[i].cel2D.tempC-malha.mlh2d[i].cel2D.tempRef));
  			malha.mlh2d[i].cel2D.presC=CI.pres[j]*98066.52+forcaCorpo;
  			malha.mlh2d[i].cel2D.presCI=malha.mlh2d[i].cel2D.presC;
  			malha.mlh2d[i].cel2D.presC0=malha.mlh2d[i].cel2D.presC;
  			break;
  		}
  	}
  	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
  		if(malha.mlh2d[i].kvizinho[j]<0){
  			malha.mlh2d[i].atualizaCC(j);
  			malha.mlh2d[i].atualizaCCTemp(j);

  		}
  	}
  	if((*vg1dSP).acop==1){
  	    for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
  	    	if(malha.mlh2d[i].kvizinho[j]<0){
  				int kcc=0;
  				while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
  				if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
  					malha.mlh2d[i].ccTVN[j]=fluxcal;
  				}
  	    	}
  	    }
  	}
  }

  int noZero=0;
  for(int i=0;i<malha.nele;i++){
  	noZero++;
  	for(int j=0;j<nvert;j++){
  		if(malha.mlh2d[i].kvizinho[j]>=0){
  			noZero++;
  		}
  	}
  }

  double* vecsraU;
  vecsraU=new double [noZero];
  int* veclm;
  veclm=new int [noZero];
  int* vecfnz;
  vecfnz=new int [malha.nele+1];

  Vcr<double> termolivreU(malha.nele);//vetor livre/solucao do sistema linear ;

  double* vecsraV;
  vecsraV=new double [noZero];

  Vcr<double> termolivreV(malha.nele);//vetor livre/solucao do sistema linear ;

  double* vecsraPCor;
  vecsraPCor=new double [noZero];

  Vcr<double> termolivrePCor(malha.nele);//vetor livre/solucao do sistema linear ;

  double* vecsraTemp;
  vecsraTemp=new double [noZero];

  Vcr<double> termolivreTemp(malha.nele);//vetor livre/solucao do sistema linear ;

  int indsra=0;
  for(int i=0;i<malha.nele;i++){
  	int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
  	//vecfnz[i]=malha.mlh2d[i].cel2D.indVizCres[0];
  	vecfnz[i]=indsra;
  	for(int j=0;j<dimNoZero;j++){
  		int ind=malha.mlh2d[i].cel2D.indVizCres[j];
  		vecsraU[indsra+j]=malha.mlh2d[i].localU.mx[0][j];
  		vecsraV[indsra+j]=malha.mlh2d[i].localV.mx[0][j];
  		vecsraPCor[indsra+j]=malha.mlh2d[i].localPCor.mx[0][j];
  		vecsraTemp[indsra+j]=malha.mlh2d[i].localT.mx[0][j];
  		veclm[indsra+j]=ind;
  	}
  	indsra+=dimNoZero;
  	termolivreU[i]=malha.mlh2d[i].TLU[0];
  }
  vecfnz[malha.nele]=noZero;

  SparseMtx<double> matglobU(malha.nele,noZero,vecsraU,veclm,vecfnz);//matriz global;
  SparseMtx<double> matglobV(malha.nele,noZero,vecsraV,veclm,vecfnz);//matriz global;
  SparseMtx<double> matglobP(malha.nele,noZero,vecsraPCor,veclm,vecfnz);//matriz global;
  SparseMtx<double> matglobT(malha.nele,noZero,vecsraTemp,veclm,vecfnz);//matriz global;

  if(rankLU>=0){
	  matglobU.rank=rankLU;
	  if(rankLU>0){
	    	matglobU.geraILUrank();
	    	matglobU.sraILU=new double[matglobU.noZeroIlu];
	  }
	  matglobU.colorido=colore;
	  if(colore)matglobU.geraILUcor();

	  matglobV.rank=rankLU;
	  if(rankLU>0){
	    	matglobV.geraILUrank();
	    	matglobV.sraILU=new double[matglobV.noZeroIlu];
	  }
	  matglobV.colorido=colore;
	  if(colore)matglobV.geraILUcor();

	  matglobP.rank=rankLU;
	  if(rankLU>0){
	    	matglobP.geraILUrank();
	    	matglobP.sraILU=new double[matglobP.noZeroIlu];
	  }
	  matglobP.colorido=colore;
	  if(colore)matglobP.geraILUcor();

	  matglobT.rank=rankLU;
	  if(rankLU>0){
	    	matglobT.geraILUrank();
	    	matglobT.sraILU=new double[matglobT.noZeroIlu];
	  }
	  matglobT.colorido=colore;
	  if(colore)matglobT.geraILUcor();
  }
	int precn=1;
	if(rankLU>=0)precn=4;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ind=0;
	double raz=0;
	double delt;
	int kontaTempo=0;
	double norma0;
	double norma1Antiga;
	double norma1;
	int dimNoZero;
	double eps;
	int iter;
	int iterV;
	int iterU;
	int retorna;
	(*vg1dSP).relaxVF=tempVF.relax;
	(*vg1dSP).relaxVFPcor=tempVF.relaxP;
	(*vg1dSP).nvfHR=tempVF.nvfHR;


	for(int i=0; i<malha.nele;i++){
		for(int face=0;face<malha.mlh2d[i].cel2D.nvert;face++){
			if(malha.mlh2d[i].kvizinho[face]>=0){
				malha.mlh2d[i].uRC[face]=malha.mlh2d[i].cel2D.fatG[face]*malha.mlh2d[i].cel2D.uC+
						(1-malha.mlh2d[i].cel2D.fatG[face])*(*malha.mlh2d[i].vizinho[face]).uC;
				malha.mlh2d[i].uRC0[face]=malha.mlh2d[i].uRC[face];
				malha.mlh2d[i].vRC[face]=malha.mlh2d[i].cel2D.fatG[face]*malha.mlh2d[i].cel2D.vC+
						(1-malha.mlh2d[i].cel2D.fatG[face])*(*malha.mlh2d[i].vizinho[face]).vC;
				malha.mlh2d[i].vRC0[face]=malha.mlh2d[i].vRC[face];
				malha.mlh2d[i].tempF[face]=malha.mlh2d[i].cel2D.fatG[face]*malha.mlh2d[i].cel2D.tempC+
						(1-malha.mlh2d[i].cel2D.fatG[face])*(*malha.mlh2d[i].vizinho[face]).tempC;
				if((*vg1dSP).temInterface==1){
					if(malha.mlh2d[i].cel2D.holC==(*malha.mlh2d[i].vizinho[face]).holC)malha.mlh2d[i].holF[face]=malha.mlh2d[i].cel2D.holC;
					else if((*malha.mlh2d[i].vizinho[face]).holC>1.-1e-15 || (*malha.mlh2d[i].vizinho[face]).holC<1e-15){
						double vazTemp=malha.mlh2d[i].cel2D.sFace[face][0]*malha.mlh2d[i].uRC[face]+
								malha.mlh2d[i].cel2D.sFace[face][1]*malha.mlh2d[i].vRC[face];
						if(malha.mlh2d[i].cel2D.holC<1.-1e-15 && malha.mlh2d[i].cel2D.holC>1e-15)
							malha.mlh2d[i].holF[face]=(*malha.mlh2d[i].vizinho[face]).holC;
						else if(vazTemp<=0.) malha.mlh2d[i].holF[face]=(*malha.mlh2d[i].vizinho[face]).holC;
						else if(vazTemp>0.) malha.mlh2d[i].holF[face]=malha.mlh2d[i].cel2D.holC;
					}
					else{
						malha.mlh2d[i].holF[face]=malha.mlh2d[i].cel2D.fatG[face]*malha.mlh2d[i].cel2D.holC+
								(1-malha.mlh2d[i].cel2D.fatG[face])*(*malha.mlh2d[i].vizinho[face]).holC;
					}
				}
				else{
					malha.mlh2d[i].holF[face]=1.;
				}
			}
		}
  	for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
  			malha.mlh2d[i].cel2D.gradGreenUI[j]=malha.mlh2d[i].cel2D.gradGreenU[j];
  			malha.mlh2d[i].cel2D.gradGreenVI[j]=malha.mlh2d[i].cel2D.gradGreenV[j];
  			malha.mlh2d[i].cel2D.gradGreenPresCorI[j]=malha.mlh2d[i].cel2D.gradGreenPresCor[j];
  			malha.mlh2d[i].cel2D.gradGreenTempI[j]=malha.mlh2d[i].cel2D.gradGreenTemp[j];
  			malha.mlh2d[i].cel2D.gradGreenHolI[j]=malha.mlh2d[i].cel2D.gradGreenHol[j];
  			malha.mlh2d[i].cel2D.gradGreenPresI[j]=malha.mlh2d[i].cel2D.gradGreenPres[j];
  	}
		malha.mlh2d[i].calcGradGreenTemp(1);
		malha.mlh2d[i].calcGradGreenHol(1);
		malha.mlh2d[i].calcForcCorp();
		malha.mlh2d[i].vazMass((*vg1dSP).relaxVF);
		malha.mlh2d[i].calcGradGreenPresCor();
	}

	//dx=0.1/50.;
	//dy=0.1/50.;
	malhaRegular(dx, dy, malha);

	//double erroPres=0.000001;
	//double erroV=0.0001;
	double erroPres=tempVF.errP;
	double erroV=tempVF.errV;
	double erroT=tempVF.errT;
	int iterMax=tempVF.maxIt;
	int iterMin=3;
	double maxresi=0.;
	double maxresiT=0.;
	double maxdeno;
	double maxdenoT;
	double tempMedPar=0.;
	ostringstream saidaTempMed;
	saidaTempMed << pathPrefixoArqSaida << "tempMed2D"<< ".dat";
	string tmp = saidaTempMed.str();
	double resetTrend=0.;
	if((*vg1dSP).acop==1){
		tempMedPar=transfer.Tint;
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << "tempo (s)  ;  "<< "temperatura media (C)  ;  "<<" temperatura media parede  "<<"temperatura media (C)1D  ;  "<<
		" temperatura media parede 1D  ; "<<"fluxo de calor medio (J/(s.m2)  ;  "<<"viscodiadade media (Pa.s)  ;  "<<"taxa de deformação media (1/s)  ;  "<<"velocidade media (C)  ;  "<<endl;
		escreveIni << 0 <<"   ;   "<< tempMedPar<<"  ;   "<<tempMedPar<<"   ;   "<< tempMedPar<<"  ;   "<<tempMedPar<<endl;
		escreveIni.close();
	}


	int nPar=0;
	//vector<int> indPar;
	unordered_map<int, int> indPar;
	unordered_map<int, int> indPar2;
	TransCal* vecTransfer;
	double* vecFluxcal;
	if((*vg1dSP).acop==1){
		paredeContorno(malha, indPar,indPar2, nPar);
		vecTransfer = new TransCal[nPar];
		vecFluxcal = new double[nPar];
		for(int i=0;i<nPar;i++){
			vecTransfer[i] =transfer;
			vecFluxcal[i]=fluxcal;
		}
	}


	//parse_fluido_complementar(elementoRaiz["fluido"]);

	int nvfHRBuffer=(*vg1dSP).nvfHR;
	double velmax;
	int kontaTempoProf=0;
	int contaCicloSeg=-1;
	int totalIter=0;
	double deltCFL;
	double multCiclo=1.;
	double condMed;
	double drInt;
	double Pi=3.14159265359;
	double condTermLiq;
	double areaLiq;
	double fluxCalMed=0.;
	while((*vg1dSP).tempo<=tempVF.tmax){
		int multEps=1.0;
		chrono::steady_clock::time_point begin, end;
		begin = chrono::steady_clock::now();
		//(*vg1dSP).mulFC=tempo/1.;
		//if((*vg1dSP).mulFC>1.)(*vg1dSP).mulFC=1.;
		//if(tempo<10.)nvfHR=0;
		//else nvfHR=nvfHRBuffer;
		if(totalIter>25 || contaCicloSeg>=0){
			if(totalIter>25 && contaCicloSeg>=0){
				contaCicloSeg=-1;
				multCiclo*=2.;
			}
			contaCicloSeg++;
		}
		if(contaCicloSeg>cicloSegur){
			contaCicloSeg=-1;
			multCiclo=1.;
		}
		velmax=0.;
		(*vg1dSP).reiniciaVF=0;
		indrazT(ind, raz);
		double inf = tempVF.dtmax[ind];
		double sup;
		if (ind < tempVF.parserie - 1){
			sup = tempVF.dtmax[ind + 1];
			delt = (inf * raz + (1 - raz) * sup);
		}
		else
			delt = tempVF.dtmax[ind];
		if(contaCicloSeg>=0)delt/=(dtSegur*multCiclo);
		deltCFL=100000.;
		for(int i=0; i<malha.nele;i++){
			if((*vg1dSP).temInterface==1 && (*vg1dSP).aplicaVOF==1){
				double vazespecif=0.;
				for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					double vXS=malha.mlh2d[i].massF[j];
					if(vXS<0.)vXS=0.;
					vazespecif+=vXS;
				}
				vazespecif/=(malha.mlh2d[i].cel2D.vElem*malha.mlh2d[i].cel2D.rho);
				double testeCFL=vazespecif*delt;
				int mono=1;
				if(malha.mlh2d[i].cel2D.holC>1e-15 || malha.mlh2d[i].cel2D.holC<1-1e-15)mono=0;
				else{
					for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
						if(malha.mlh2d[i].kvizinho[j]<0)mono=0;
						else if((*(malha.mlh2d[i].vizinho[j])).holC!=malha.mlh2d[i].cel2D.holC)mono=0;
					}
				}
				if(mono==0 && testeCFL>(*vg1dSP).cflG && fabsl(vazespecif)>1e-15)delt=(*vg1dSP).cflG/vazespecif;
			}
			else{
				malha.mlh2d[i].dt=delt;
				malha.mlh2d[i].CFLC=(*vg1dSP).cflG;

				double vazespecif=0.;
				for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					double vXS=malha.mlh2d[i].massF[j];
					if(vXS<0.)vXS=0.;
					vazespecif+=vXS;
				}
				vazespecif/=(malha.mlh2d[i].cel2D.vElem*malha.mlh2d[i].cel2D.rho);
				if((*vg1dSP).cflG/vazespecif<deltCFL)deltCFL=(*vg1dSP).cflG/vazespecif;
				for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					malha.mlh2d[i].dtCFL[j]=malha.mlh2d[i].dt;
					malha.mlh2d[i].CFL[j]=(*vg1dSP).cflG;
				}
			}
		}
		if((*vg1dSP).temInterface==0  && (*vg1dSP).aplicaVOF==0){
			if(delt>deltCFL)delt=deltCFL;
			for(int i=0; i<malha.nele;i++){
				malha.mlh2d[i].dt=delt;
				for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					malha.mlh2d[i].dtCFL[j]=malha.mlh2d[i].dt;
				}
			}
		}
		if((*vg1dSP).temInterface==1 && (*vg1dSP).aplicaVOF==1){
			for(int i=0; i<malha.nele;i++){
				malha.mlh2d[i].dt=delt;
				double vazespecif=0.;
				for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					double vXS=malha.mlh2d[i].massF[j];
					if(vXS<0.)vXS=0.;
					vazespecif+=vXS;
					malha.mlh2d[i].dtCFL[j]=delt;
					malha.mlh2d[i].CFL[j]=malha.mlh2d[i].dt*vXS/(malha.mlh2d[i].cel2D.vElem*malha.mlh2d[i].cel2D.rho);
				}
				vazespecif/=(malha.mlh2d[i].cel2D.vElem*malha.mlh2d[i].cel2D.rho);
				malha.mlh2d[i].CFLC=vazespecif*delt;
			}
			for(int i=0; i<malha.nele;i++)malha.mlh2d[i].explicitHol();
			if((*vg1dSP).reiniciaVF==1){
				int menori=0;
				double menorDt=malha.mlh2d[menori].dt;
				for(int i=0; i<malha.nele;i++){
					if(menorDt>malha.mlh2d[i].dt){
						menori=i;
						menorDt=malha.mlh2d[menori].dt;
					}
				}
				delt=menorDt;
				for(int i=0; i<malha.nele;i++){
					malha.mlh2d[i].dt=menorDt;
					double vazespecif=0.;
					for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
						double vXS=malha.mlh2d[i].massF[j];
						if(vXS<0.)vXS=0.;
						vazespecif+=vXS;
						malha.mlh2d[i].dtCFL[j]=delt;
						malha.mlh2d[i].CFL[j]=malha.mlh2d[i].dt*vXS/(malha.mlh2d[i].cel2D.vElem*malha.mlh2d[i].cel2D.rho);
					}
					vazespecif/=(malha.mlh2d[i].cel2D.vElem*malha.mlh2d[i].cel2D.rho);
					malha.mlh2d[i].CFLC=vazespecif*delt;
				}
				for(int i=0; i<malha.nele;i++)malha.mlh2d[i].explicitHol();
			}
			if((*vg1dSP).temInterface==1){
				for(int i=0; i<malha.nele;i++){
					if(fabsl(malha.mlh2d[i].cel2D.holC0-malha.mlh2d[i].cel2D.holC)>1e-15){
						malha.mlh2d[i].cel2D.rho=malha.mlh2d[i].cel2D.holC*prop.rho[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.rho[(*vg1dSP).igas];
						if((*vg1dSP).acop==0 || viscVar==0)
							malha.mlh2d[i].cel2D.visc=malha.mlh2d[i].cel2D.holC*prop.visc[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.visc[(*vg1dSP).igas];
						/*if(viscVar==1){
							double pres=malha.mlh2d[i].cel2D.presC;
							double temp=malha.mlh2d[i].cel2D.tempC;
							malha.mlh2d[i].cel2D.visc=malha.mlh2d[i].cel2D.holC*flucVF.VisFlu(pres, temp,malha.mlh2d[i].cel2D.taxaDeform)/1000.+
								(1.-malha.mlh2d[i].cel2D.holC)*flucVF.VisGas(pres, temp)/1000.;
						}
						else{
							malha.mlh2d[i].cel2D.visc=malha.mlh2d[i].cel2D.holC*prop.visc[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.visc[(*vg1dSP).igas];
						}*/
						malha.mlh2d[i].cel2D.beta=malha.mlh2d[i].cel2D.holC*prop.beta[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.beta[(*vg1dSP).igas];
						malha.mlh2d[i].cel2D.cond=malha.mlh2d[i].cel2D.holC*prop.cond[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.cond[(*vg1dSP).igas];
						malha.mlh2d[i].cel2D.cp=malha.mlh2d[i].cel2D.holC*prop.cp[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.cp[(*vg1dSP).igas];
						malha.mlh2d[i].cel2D.tempRef=malha.mlh2d[i].cel2D.holC*prop.tRef[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.tRef[(*vg1dSP).igas];
					}
				}
			}

		}
		//acoplamento fluxo de calor
		if((*vg1dSP).acop==1){
			//areaLiq=areaMedParede(malha,condMed);
			//drInt=sqrtl(dutosMRT.a*dutosMRT.a-areaLiq/Pi);
			//condTermLiq=2*Pi*condMed/logl(dutosMRT.a/drInt);
			//transfer.novoHi=condTermLiq;
			//transfer.novoHi=0.;
			for(int i=0;i<nPar;i++)vecTransfer[i].novoHi=0.;
		}
		//acoplamento fluxo de calor
		if((*vg1dSP).acop==1){
			//transfer.dt=delt;
			//fluxcal=transfer.transtrans()/(M_PI*dutosMRT.a);
			//fluxcal=transfer.transperm()/(M_PI*dutosMRT.a);
			for(int i=0;i<nPar;i++){
				vecTransfer[i].dt=delt;
			}
			for(int i=0;i<nPar;i++){
				vecFluxcal[i]=vecTransfer[i].transtrans()/(M_PI*dutosMRT.a);
			}
		}
/*		if((*vg1dSP).acop==1){
			transfer.dt=delt;
			fluxcal=transfer.transtrans()/(M_PI*dutosMRT.a);
			for(int i=0; i<malha.nele;i++){
				if((*vg1dSP).acop==1 && (*vg1dSP).temInterface==0){
					double pres=malha.mlh2d[i].cel2D.presC;
					double temp=malha.mlh2d[i].cel2D.tempC;
					malha.mlh2d[i].cel2D.visc=flucVF.VisFlu(pres, temp,malha.mlh2d[i].cel2D.taxaDeform)/1000.;
				}
				for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
					if(malha.mlh2d[i].kvizinho[j]<0){
						int kcc=0;
						while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
						if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
							if(tempMedPar!=transfer.Textern1)malha.mlh2d[i].ccTVN[j]=fluxcal*fabsl(malha.mlh2d[i].tempF[j]-transfer.Textern1)/
									fabsl(tempMedPar-transfer.Textern1);
							else malha.mlh2d[i].ccTVN[j]=0.;
							if(fluxcal<0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)<0.)malha.mlh2d[i].ccTVN[j]=0.;
							else if(fluxcal>0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)>0.)malha.mlh2d[i].ccTVN[j]=0.;
							//malha.mlh2d[i].ccTVN[j]=fluxcal;
						}
					}
				}
			}
		}*/

		/*for(int i=0; i<malha.nele;i++){
			double pres=malha.mlh2d[i].cel2D.presC;
			double temp=malha.mlh2d[i].cel2D.tempC;
			malha.mlh2d[i].cel2D.visc=fluc.VisFlu(pres, temp)/1000.;
		}*/

		for(int i=0;i<malha.nele;i++){
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
	    			malha.mlh2d[i].atualizaCC(j);
	    			malha.mlh2d[i].atualizaCCTemp(j);
	    		}
	    	}
		}

		norma0=1e5;
		norma1Antiga=1e6;
		norma1=1e5;
		int iterTempo=0;
		int iterUV=0;
		double norma0Piso;
		double norma1Piso;
		double velmedPiso;
		double normaTemp=1e5;

		//while(((norma0>erroPres || norma1>erroV || normaTemp>erroT) && iterTempo<iterMax)||
				//iterTempo<iterMin){
		int totalizaIterTemp=0;
		double multiRes=1.;
		int multIter=1;
		while((((maxresi>tempVF.erroRes*multiRes) &&
				(norma0>erroPres || norma1>erroV)) && iterTempo<iterMax)|| iterTempo<multIter*iterMin){

			//if(iterTempo>5)multEps=0.1;
			//if(iterTempo>10)multEps=0.01;
			//if(iterTempo>20)multEps=0.01;

			//acoplamento fluxo de calor
			if((*vg1dSP).acop==1){
				for(int i=0; i<malha.nele;i++){
					if(viscVar==1){
						double pres=malha.mlh2d[i].cel2D.presC;
						double temp=malha.mlh2d[i].cel2D.tempC;
						if((*vg1dSP).temInterface==1){
							malha.mlh2d[i].cel2D.visc=malha.mlh2d[i].cel2D.holC*flucVF.VisFlu(pres, temp,malha.mlh2d[i].cel2D.taxaDeform)/1000.+
								(1.-malha.mlh2d[i].cel2D.holC)*flucVF.VisGas(pres, temp)/1000.;
						}
						else malha.mlh2d[i].cel2D.visc=flucVF.VisFlu(pres, temp,malha.mlh2d[i].cel2D.taxaDeform)/1000.;
					}
					else{
						malha.mlh2d[i].cel2D.visc=malha.mlh2d[i].cel2D.holC*prop.visc[(*vg1dSP).iliq]+(1.-malha.mlh2d[i].cel2D.holC)*prop.visc[(*vg1dSP).igas];
					}
					for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
						if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
								/*if(tempMedPar!=transfer.Textern1)malha.mlh2d[i].ccTVN[j]=fluxcal*fabsl(malha.mlh2d[i].tempF[j]-transfer.Textern1)/
										fabsl(tempMedPar-transfer.Textern1);
								else malha.mlh2d[i].ccTVN[j]=0.;
								//malha.mlh2d[i].ccTVN[j]=fluxcal*malha.mlh2d[i].tempF[j]/transfer2.Tcamada[0][0];
								if(fluxcal<0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)<0.)malha.mlh2d[i].ccTVN[j]=0.;
								else if(fluxcal>0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)>0.)malha.mlh2d[i].ccTVN[j]=0.;
								//malha.mlh2d[i].ccTVN[j]=fluxcal;*/
								int iFlux=indPar[i];
								malha.mlh2d[i].ccTVN[j]=-0*800*fabsl(malha.mlh2d[i].tempF[j]-transfer.Textern1)/
										fabsl(60-transfer.Textern1)+1*vecFluxcal[iFlux];
								if(fluxcal<0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)<0.)malha.mlh2d[i].ccTVN[j]=0.;
								else if(fluxcal>0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)>0.)malha.mlh2d[i].ccTVN[j]=0.;
							}
						}
					}
				}
			}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////        PASSO SIMPLE        ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			/*for(int i=0; i<malha.nele;i++){
		    	malha.mlh2d[i].calcGradGreenUV();
		    }*/

			/*if(norma1Antiga<norma1){
			    for(int i=0; i<malha.nele;i++){
			    	malha.mlh2d[i].cel2D.presC=malha.mlh2d[i].cel2D.presCI;
			    	malha.mlh2d[i].cel2D.uC=malha.mlh2d[i].cel2D.uCI;
			    	malha.mlh2d[i].cel2D.vC=malha.mlh2d[i].cel2D.vCI;
			    }
			}*/
			#pragma omp parallel for num_threads(nthrdLocal)
	    	for(int i=0; i<malha.nele;i++){
	    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
	    			malha.mlh2d[i].cel2D.gradGreenPresI[j]=malha.mlh2d[i].cel2D.gradGreenPres[j];
	    		}
	    	}
			#pragma omp parallel for num_threads(nthrdLocal)
		    for(int i=0; i<malha.nele;i++){
		    	malha.mlh2d[i].calcGradGreenPres();
		    }

		    //norma1=1e5;
		    //iterUV=0;
		    //while((norma1>0.0001 && iterUV<50)|| iterUV<2){
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
		    			malha.mlh2d[i].cel2D.gradGreenUI[j]=malha.mlh2d[i].cel2D.gradGreenU[j];
		    			malha.mlh2d[i].cel2D.gradGreenVI[j]=malha.mlh2d[i].cel2D.gradGreenV[j];
		    		}
		    	}
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		malha.mlh2d[i].calcGradGreenUV();
		    	}
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){

		    		if((*vg1dSP).metodoAcopPV==-1){
		    			malha.mlh2d[i].cel2D.uCI=1.0*(malha.mlh2d[i].cel2D.uC);
		    			malha.mlh2d[i].cel2D.vCI=1.0*(malha.mlh2d[i].cel2D.vC);
		    		}

		    		//malha.mlh2d[i].GeraLocalUV((*vg1dSP).relaxVF);
		    	}
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++)malha.mlh2d[i].GeraLocalUV((*vg1dSP).relaxVF);
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		malha.mlh2d[i].calSupDifu();
		    		if((*vg1dSP).metodoAcopPV!=-1){
		    			malha.mlh2d[i].cel2D.uCI=malha.mlh2d[i].cel2D.uC;
		    			malha.mlh2d[i].cel2D.vCI=malha.mlh2d[i].cel2D.vC;
		    		}


		    	}

		    	maxresi=-1.;
		    	maxdeno=-1.;
		    	double totalRes=0.;
				//#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		if(maxdeno<fabsl(malha.mlh2d[i].denoResU))
		    			maxdeno=fabsl(malha.mlh2d[i].denoResU);
		    		if(maxdeno<fabsl(malha.mlh2d[i].denoResV))
		    			maxdeno=fabsl(malha.mlh2d[i].denoResV);
		    	}
				//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
		    	double kontaFora=0.;
		    	for(int i=0; i<malha.nele;i++){
		    		double resiteste=maxdeno;
		    		if(fabsl(resiteste<=1e-20))resiteste=1.;
		    		if(maxresi<fabsl(malha.mlh2d[i].residuoU/resiteste))
		    			maxresi=fabsl(malha.mlh2d[i].residuoU/resiteste);
		    		if(fabsl(malha.mlh2d[i].denoResU)>1e-10)totalRes+=fabsl(malha.mlh2d[i].residuoU)/fabsl(malha.mlh2d[i].denoResU);
		    		if(maxresi<fabsl(malha.mlh2d[i].residuoV/resiteste))
		    			maxresi=fabsl(malha.mlh2d[i].residuoV/resiteste);
		    		if(fabsl(malha.mlh2d[i].denoResV)>1e-10)totalRes+=fabsl(malha.mlh2d[i].residuoV)/fabsl(malha.mlh2d[i].denoResV);
		    	}
		    	maxresi=totalRes/malha.nele;

		    	if((*vg1dSP).metodoAcopPV!=2){
		    		indsra=0;
					//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
		    		for(int i=0;i<malha.nele;i++){
		    			dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
		    			for(int j=0;j<dimNoZero;j++)
		    				matglobU.sra[indsra+j]=malha.mlh2d[i].localU.mx[0][j];
		    			termolivreU[i]=malha.mlh2d[i].TLU[0];
						//#pragma omp atomic
		    				indsra+=dimNoZero;
		    		}
		    		eps=0.00000001*multEps;
		    		iterU=100;
		    		if(iterU<malha.nele)iterU=malha.nele;
					#pragma omp parallel for num_threads(nthrdLocal)
		    		for(int i=0;i<malha.nele;i++){
		    			malha.vecSolv[i]=malha.mlh2d[i].cel2D.uCEx;
		    		//malha.vecSolv[i]=malha.mlh2d[i].cel2D.uC;
		    		}
		    		if(solverMat==0)retorna=matglobU.GMRES(malha.vecSolv,termolivreU,eps,iterU,precn,40+0*malha.nele-1);
		    		else if(solverMat==1)retorna=matglobU.FGMRES(malha.vecSolv,termolivreU,eps,iterU,precn,40+0*malha.nele-1);
		    		else retorna=matglobU.BiCGStab(malha.vecSolv,termolivreU,eps,iterU,precn);
		    		if(retorna==1){
		    			//cout << "Preoblema no GMRES " << retorna<<endl;
		    			//break;
		    		}
		    	}
		    	norma1Antiga=norma1;
		    	norma1=0.;
		    	double velmed=0.;
				//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
		    	for(int i=0;i<malha.nele;i++){
		    		if((*vg1dSP).metodoAcopPV!=2)malha.mlh2d[i].cel2D.uC=malha.vecSolv[i];
		    		else malha.mlh2d[i].cel2D.uC=(*vg1dSP).relaxVF*malha.mlh2d[i].cel2D.uCEx+(1-(*vg1dSP).relaxVF)*malha.mlh2d[i].cel2D.uCI;
		    		norma1+=fabsl(malha.mlh2d[i].cel2D.uC-malha.mlh2d[i].cel2D.uCI);
		    		velmed+=fabsl(malha.mlh2d[i].cel2D.uC);
		    		if(fabsl(malha.mlh2d[i].cel2D.uC)>velmax)velmax=fabsl(malha.mlh2d[i].cel2D.uC);
		    	}

		    	if((*vg1dSP).metodoAcopPV!=2){
		    		indsra=0;
					//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
		    		for(int i=0;i<malha.nele;i++){
		    			dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
		    			for(int j=0;j<dimNoZero;j++)
		    				matglobV.sra[indsra+j]=malha.mlh2d[i].localV.mx[0][j];
		    			indsra+=dimNoZero;
		    			termolivreV[i]=malha.mlh2d[i].TLV[0];
		    		}
		    		eps=0.00000001*multEps;
		    		iterV=100;
		    		if(iterV<malha.nele)iterV=malha.nele;
					#pragma omp parallel for num_threads(nthrdLocal)
		    		for(int i=0;i<malha.nele;i++){
		    			malha.vecSolv[i]=malha.mlh2d[i].cel2D.vCEx;
		    			//malha.vecSolv[i]=malha.mlh2d[i].cel2D.vC;
		    		}
		    		if(solverMat==0)retorna=matglobV.GMRES(malha.vecSolv,termolivreV,eps,iterV,precn,40+0*malha.nele-1);
		    		else if(solverMat==1)retorna=matglobV.FGMRES(malha.vecSolv,termolivreV,eps,iterV,precn,40+0*malha.nele-1);
		    		else retorna=matglobV.BiCGStab(malha.vecSolv,termolivreV,eps,iterV,precn);
		    		if(retorna==1){
		    			//cout << "Preoblema no GMRES " << retorna<<endl;
		    			//break;
		    		}
		    	}
				#pragma omp parallel for reduction(+:norma1, velmed) num_threads(nthrdLocal)
		    	for(int i=0;i<malha.nele;i++){
		    		if((*vg1dSP).metodoAcopPV!=2) malha.mlh2d[i].cel2D.vC=malha.vecSolv[i];
		    		else malha.mlh2d[i].cel2D.vC=(*vg1dSP).relaxVF*malha.mlh2d[i].cel2D.vCEx+(1-(*vg1dSP).relaxVF)*malha.mlh2d[i].cel2D.vCI;
		    		norma1+=fabsl(malha.mlh2d[i].cel2D.vC-malha.mlh2d[i].cel2D.vCI);
		    		velmed+=fabsl(malha.mlh2d[i].cel2D.vC);
		    		//if(fabsl(malha.mlh2d[i].cel2D.uC)>velmax)velmax=fabsl(malha.mlh2d[i].cel2D.uC);
		    	}
		    	for(int i=0;i<malha.nele;i++) if(fabsl(malha.mlh2d[i].cel2D.uC)>velmax)velmax=fabsl(malha.mlh2d[i].cel2D.uC);
		    	velmed/=(2.*malha.nele);
		    	norma1/=malha.nele;
		    	norma1/=2.;
		    	norma1/=velmed;
		    	iterUV++;
		    //}
			#pragma omp parallel for num_threads(nthrdLocal)
			for(int i=0; i<malha.nele;i++){
	    		//malha.mlh2d[i].cel2D.uC=relax*malha.mlh2d[i].cel2D.uC+(1-relax)*malha.mlh2d[i].cel2D.uCI;
	    		//malha.mlh2d[i].cel2D.vC=relax*malha.mlh2d[i].cel2D.vC+(1-relax)*malha.mlh2d[i].cel2D.vCI;
			    malha.mlh2d[i].vazMass((*vg1dSP).relaxVF);
			}
			#pragma omp parallel for num_threads(nthrdLocal)
			for(int i=0; i<malha.nele;i++) malha.mlh2d[i].cel2D.presCI=malha.mlh2d[i].cel2D.presC;
			#pragma omp parallel for num_threads(nthrdLocal)
		    for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocalPcor();
			#pragma omp parallel for num_threads(nthrdLocal)
		    for(int i=0; i<malha.nele;i++)
		    	malha.mlh2d[i].cel2D.presCIcor=malha.mlh2d[i].cel2D.presCcor;

		    indsra=0;
			//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
		    for(int i=0;i<malha.nele;i++){
		    	dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
		    	for(int j=0;j<dimNoZero;j++)
		    		matglobP.sra[indsra+j]=malha.mlh2d[i].localPCor.mx[0][j]/10000.;
		    	indsra+=dimNoZero;
		    	termolivrePCor[i]=malha.mlh2d[i].TLPCor[0];
		    }
		    eps=0.0000001*multEps;
		    iter=100;
		    if(iter<malha.nele)iter=malha.nele;
		    if((*vg1dSP).tempo>1e-15){
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presCcor*10000.;
		    }
		    else{
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=(malha.mlh2d[i].cel2D.presC)*10000./100000.;
		    }
		    //if(iterTempo==0)for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presC/100000.;
		    //else for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presCEx;
		    if(solverMat==0)retorna=matglobP.GMRES(malha.vecSolv,termolivrePCor,eps,iter,precn,40+0*malha.nele-1);
		    else if(solverMat==1)retorna=matglobP.FGMRES(malha.vecSolv,termolivrePCor,eps,iter,precn,40+0*malha.nele-1);
		   // retorna=matglobP.GMRES(malha.vecSolv,termolivrePCor,eps,iter,1,40+0*malha.nele-1);
		    else retorna=matglobP.BiCGStab(malha.vecSolv,termolivrePCor,eps,iter,precn);
		    //retorna=matglobP.CG(malha.vecSolv,termolivrePCor,eps,iter,1);
		    if(retorna==1){
		    	//cout << "Preoblema no GMRES " << retorna<<endl;
		    	//break;
		    }
			#pragma omp parallel for num_threads(nthrdLocal)
		    for(int i=0;i<malha.nele;i++){
		    	malha.mlh2d[i].cel2D.presCcor=malha.vecSolv[i]/10000.;
		    }

	    	norma0=0.;
	    	double presMed=0.;
			//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
#pragma omp parallel for reduction(+:norma0, presMed) num_threads(nthrdLocal)
	    	for(int i=0;i<malha.nele;i++){
	    		norma0+=fabsl(malha.mlh2d[i].cel2D.presCcor);
	    		presMed+=fabsl(malha.mlh2d[i].cel2D.presC);
	    	}
	    	presMed/=malha.nele;
	    	norma0/=malha.nele;
	    	norma0/=presMed;
			#pragma omp parallel for num_threads(nthrdLocal)
	    	for(int i=0; i<malha.nele;i++){
	    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
	    			malha.mlh2d[i].cel2D.gradGreenPresCorI[j]=malha.mlh2d[i].cel2D.gradGreenPresCor[j];
	    		}
	    	}
			#pragma omp parallel for num_threads(nthrdLocal)
	    	for(int i=0; i<malha.nele;i++) malha.mlh2d[i].calcGradGreenPresCor();
			#pragma omp parallel for num_threads(nthrdLocal)
	    	for(int i=0; i<malha.nele;i++) malha.mlh2d[i].atualizaCor();
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    	//////////////////////////////////////////////////////////// Fim Passo SIMPLE  ////////////////////////////////////////////////////
	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	    	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    	////////////////////////   Passo correcao - PISO/////////////////////////////////////////////////////////////
	    	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    	double relaxPiso=(*vg1dSP).relaxVFExpli;
	    	if((*vg1dSP).metodoAcopPV==4)relaxPiso=(*vg1dSP).relaxVFExpli;
	    	if(((*vg1dSP).metodoAcopPV==3 ||(*vg1dSP).metodoAcopPV==4)  && (norma1Antiga>norma1 && (norma0>erroPres || norma1>erroV))){
	    		for(int kontaPiso=0;kontaPiso<=nPrime;kontaPiso++){
	    			norma0Piso=0;
	    			norma1Piso=0;
	    			velmedPiso=0.;
					#pragma omp parallel for num_threads(nthrdLocal)
	    	    	for(int i=0; i<malha.nele;i++){
	    	    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
	    	    			malha.mlh2d[i].cel2D.gradGreenPresI[j]=malha.mlh2d[i].cel2D.gradGreenPres[j];
	    	    		}
	    	    	}
					#pragma omp parallel for num_threads(nthrdLocal)
	    			for(int i=0; i<malha.nele;i++){
	    				malha.mlh2d[i].calcGradGreenPres();
	    			}
					#pragma omp parallel for num_threads(nthrdLocal)
			    	for(int i=0; i<malha.nele;i++){
			    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
			    			malha.mlh2d[i].cel2D.gradGreenUI[j]=malha.mlh2d[i].cel2D.gradGreenU[j];
			    			malha.mlh2d[i].cel2D.gradGreenVI[j]=malha.mlh2d[i].cel2D.gradGreenV[j];
			    		}
			    	}
					#pragma omp parallel for num_threads(nthrdLocal)
	    			for(int i=0; i<malha.nele;i++){
	    				malha.mlh2d[i].calcGradGreenUV();
	    			}
					#pragma omp parallel for num_threads(nthrdLocal)
	    			for(int i=0; i<malha.nele;i++){
	    				malha.mlh2d[i].GeraLocalUV(relaxPiso);
	    			}
					#pragma omp parallel for num_threads(nthrdLocal)
	    			for(int i=0; i<malha.nele;i++){
	    				malha.mlh2d[i].calSupDifu();
	    			}
#pragma omp parallel for reduction(+:norma1Piso, velmedPiso) num_threads(nthrdLocal)
	    			for(int i=0;i<malha.nele;i++){
	    				norma1Piso+=(fabsl(malha.mlh2d[i].cel2D.uC-malha.mlh2d[i].cel2D.uCEx)+
	    						fabsl(malha.mlh2d[i].cel2D.vC-malha.mlh2d[i].cel2D.vCEx));
	    				velmedPiso+=fabsl(malha.mlh2d[i].cel2D.uC)+fabsl(malha.mlh2d[i].cel2D.vC);
	    				malha.mlh2d[i].cel2D.uC=malha.mlh2d[i].cel2D.uCEx;
	    			}
	    			velmedPiso/=(2.*malha.nele);
	    			norma1Piso/=malha.nele;
	    			norma1Piso/=2.;
	    			norma1Piso/=velmedPiso;
	    			if(norma1Piso<=norma1){
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0;i<malha.nele;i++){
	    					malha.mlh2d[i].cel2D.uC=malha.mlh2d[i].cel2D.uCEx;
	    				}
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0;i<malha.nele;i++){
	    					malha.mlh2d[i].cel2D.vC=malha.mlh2d[i].cel2D.vCEx;
	    				}
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0; i<malha.nele;i++){
	    					malha.mlh2d[i].vazMass(relaxPiso,0);
	    				}
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocalPcor();
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0; i<malha.nele;i++)
	    					malha.mlh2d[i].cel2D.presCIcor=malha.mlh2d[i].cel2D.presCcor;
	    				indsra=0;
						//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
	    				for(int i=0;i<malha.nele;i++){
	    					dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
	    					for(int j=0;j<dimNoZero;j++)
	    						matglobP.sra[indsra+j]=malha.mlh2d[i].localPCor.mx[0][j]/10000.;
	    					indsra+=dimNoZero;
	    					termolivrePCor[i]=malha.mlh2d[i].TLPCor[0];
	    				}
	    				eps=0.00000001*multEps;
	    				iter=100;
	    				if(iter<malha.nele)iter=malha.nele;
	    				if((*vg1dSP).tempo>1e-15){
							#pragma omp parallel for num_threads(nthrdLocal)
	    					for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presCcor*10000.;
	    				}
	    				else{
							#pragma omp parallel for num_threads(nthrdLocal)
	    					for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presC*10000./100000.;
	    				}
	    				if(solverMat==0) retorna=matglobP.GMRES(malha.vecSolv,termolivrePCor,eps,iter,precn,40+0*malha.nele-1);
	    				else  if(solverMat==1)retorna=matglobP.FGMRES(malha.vecSolv,termolivrePCor,eps,iter,precn,40+0*malha.nele-1);
	    				else retorna=matglobP.BiCGStab(malha.vecSolv,termolivrePCor,eps,iter,precn);
	    				//retorna=matglobP.CG(malha.vecSolv,termolivrePCor,eps,iter,1);
	    				if(retorna==1){
	    					//cout << "Preoblema no GMRES " << retorna<<endl;
	    					//break;
	    				}
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0;i<malha.nele;i++){
	    					malha.mlh2d[i].cel2D.presCcor=malha.vecSolv[i]/10000.;
	    				}

	    				/*
	    				norma0=0.;
  					double presMed=0.;
  					for(int i=0;i<malha.nele;i++){
  						norma0+=fabsl(malha.mlh2d[i].cel2D.presCcor);
  						presMed+=fabsl(malha.mlh2d[i].cel2D.presC);
  					}
  					presMed/=malha.nele;
  					norma0/=malha.nele;
  					norma0/=presMed;
	    				 */
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0; i<malha.nele;i++){
	    		    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
	    		    			malha.mlh2d[i].cel2D.gradGreenPresCorI[j]=malha.mlh2d[i].cel2D.gradGreenPresCor[j];
	    		    		}
	    		    	}
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].calcGradGreenPresCor();
						#pragma omp parallel for num_threads(nthrdLocal)
	    				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].atualizaCor();
	    			}
	    		}
	    	}

	    	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    	////////////////////////   FIM Passo correcao - PISO/////////////////////////////////////////////////////////////
	    	/////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////Energia/////////////////////////////////////////////////////////////////////////

	    	//acoplamento fluxo de calor
    		if(impliAcopTerm==0){
    			if((*vg1dSP).acop==1){
    				/*transfer.FeiticoDoTempo();
    				transfer.Tint=tempMedPar=tempMedParede(malha);
    				fluxcal= transfer.transtrans()/(M_PI*dutosMRT.a);
    				//fluxcal= transfer.transperm()/(M_PI*dutosMRT.a);*/
    				tempMedPar=tempMedParede(malha);
    				double dtp;
    				for(int i=0;i<nPar;i++){
    					int iFlux2=indPar2[i];

    					vecTransfer[i].FeiticoDoTempo();
    					for(int j=0; j<malha.mlh2d[iFlux2].cel2D.nvert;j++){
    						if(malha.mlh2d[iFlux2].kvizinho[j]<0){
    							int kcc=0;
    							while(kcc<1 && malha.mlh2d[iFlux2].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
    							if(kcc<1 && malha.mlh2d[iFlux2].cel2D.ccFace[j]==CC.rotuloAcop){
    								double contTemp=1.;
    								if((*vg1dSP).tempo>1 && (*vg1dSP).tempo<10){
    									contTemp=1-((*vg1dSP).tempo-1.)/9;
    								}
    								else if((*vg1dSP).tempo>10)contTemp=0.;
    								vecTransfer[i].Tint=contTemp*malha.mlh2d[iFlux2].cel2D.tempC+(1-contTemp)*malha.mlh2d[iFlux2].tempF[j];
    								dtp=-0.0001*vecTransfer[i].Tint;
    							}
    						}
    					}
    					vecTransfer[i].Tint+=dtp;
    					double fluxTemp=vecTransfer[i].transtrans()/(M_PI*dutosMRT.a);
    					vecTransfer[i].FeiticoDoTempo();
    					vecTransfer[i].Tint-=dtp;
	    				vecFluxcal[i]= vecTransfer[i].transtrans()/(M_PI*dutosMRT.a);
	    				malha.mlh2d[iFlux2].DCCN=(vecFluxcal[i]-fluxTemp)/(-dtp);

    				}
    				for(int i=0; i<malha.nele;i++){
    					for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
    						if(malha.mlh2d[i].kvizinho[j]<0){
    							int kcc=0;
    							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
    							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
    								/*if(tempMedPar!=transfer.Textern1)malha.mlh2d[i].ccTVN[j]=fluxcal*fabsl(malha.mlh2d[i].tempF[j]-transfer.Textern1)/
    										fabsl(tempMedPar-transfer.Textern1);
    								else malha.mlh2d[i].ccTVN[j]=0.;
    								//malha.mlh2d[i].ccTVN[j]=fluxcal*malha.mlh2d[i].tempF[j]/transfer2.Tcamada[0][0];
    								if(fluxcal<0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)<0.)malha.mlh2d[i].ccTVN[j]=0.;
    								else if(fluxcal>0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)>0.)malha.mlh2d[i].ccTVN[j]=0.;
    								//malha.mlh2d[i].ccTVN[j]=fluxcal;*/
    								int iFlux=indPar[i];
    								malha.mlh2d[i].ccTVN[j]=-0*800*fabsl(malha.mlh2d[i].tempF[j]-transfer.Textern1)/
    										fabsl(60-transfer.Textern1)+1*vecFluxcal[iFlux];
    								if(fluxcal<0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)<0.)malha.mlh2d[i].ccTVN[j]=0.;
    								else if(fluxcal>0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)>0.)malha.mlh2d[i].ccTVN[j]=0.;
    							}
    						}
    					}
    				}
    			}
    		}

	    	int iterTemp=0;
	    	normaTemp=1e5;
	    	while((normaTemp>erroT*tempVF.relaxT || maxresiT>tempVF.erroRes) && iterTemp<200){
				#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
		    			malha.mlh2d[i].cel2D.gradGreenTempI[j]=malha.mlh2d[i].cel2D.gradGreenTemp[j];
		    		}
		    	}
				#pragma omp parallel for num_threads(nthrdLocal)
	    		for(int i=0; i<malha.nele;i++) malha.mlh2d[i].calcGradGreenTemp();
				#pragma omp parallel for num_threads(nthrdLocal)
	    		for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocalT(tempVF.relaxT+0*(*vg1dSP).relaxVF);


		    	maxresiT=-1.;
		    	maxdenoT=-1.;
		    	double totalResT=0.;
				//#pragma omp parallel for num_threads(nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		if(maxdenoT<fabsl(malha.mlh2d[i].denoResT))
		    			maxdenoT=fabsl(malha.mlh2d[i].denoResT);
		    	}
				//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
		    	for(int i=0; i<malha.nele;i++){
		    		double resitesteT=maxdenoT;
		    		if(fabsl(resitesteT<=1e-20))resitesteT=1.;
		    		if(maxresiT<fabsl(malha.mlh2d[i].residuoT/resitesteT))
		    			maxresiT=fabsl(malha.mlh2d[i].residuoT/resitesteT);
		    		if(fabsl(malha.mlh2d[i].denoResT)>1e-15)totalResT+=fabsl(malha.mlh2d[i].residuoT)/fabsl(malha.mlh2d[i].denoResT);
		    	}
		    	if((*vg1dSP).tipoRes==1)maxresiT=totalResT/malha.nele;


	    		indsra=0;
				//#pragma omp parallel for num_threads((*vg1dSP).ntrd)
	    		for(int i=0;i<malha.nele;i++){
	    			int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
	    			for(int j=0;j<dimNoZero;j++)
	    				matglobT.sra[indsra+j]=malha.mlh2d[i].localT.mx[0][j];
	    			indsra+=dimNoZero;
	    			termolivreTemp[i]=malha.mlh2d[i].TLT[0];
	    		}
	    		double eps=0.0000001;
	    		iter=100;
	    		if(iter<malha.nele)iter=malha.nele;
	    		int retorna;
				#pragma omp parallel for num_threads(nthrdLocal)
	    		for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.tempC;
	    		if(solverMat==0) retorna=matglobT.GMRES(malha.vecSolv,termolivreTemp,eps,iter,precn,40+0*malha.nele-1);
	    		else if(solverMat==1)retorna=matglobT.FGMRES(malha.vecSolv,termolivreTemp,eps,iter,precn,40+0*malha.nele-1);
	    		else retorna=matglobT.BiCGStab(malha.vecSolv,termolivreTemp,eps,iter,precn);

	    		if(retorna==1){
	    			//cout << "Problema no GMRES " << retorna<<endl;
	    			//break;
	    		}
	    		normaTemp=0.;
#pragma omp parallel for reduction(+:normaTemp) num_threads(nthrdLocal)
	    		for(int i=0;i<malha.nele;i++){
	    				malha.mlh2d[i].cel2D.tempCI=malha.mlh2d[i].cel2D.tempC;
	    				malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
	    				normaTemp+=fabsl(malha.mlh2d[i].cel2D.tempC-malha.mlh2d[i].cel2D.tempCI);
	    		}
	    		normaTemp/=malha.nele;
	    		iterTemp++;
	    		//acoplamento fluxo de calor
	    		if(impliAcopTerm==1){
	    			if((*vg1dSP).acop==1){
	    				/*transfer.FeiticoDoTempo();
	    				transfer.Tint=tempMedPar=tempMedParede(malha);
	    				fluxcal= transfer.transtrans()/(M_PI*dutosMRT.a);
	    				//fluxcal= transfer.transperm()/(M_PI*dutosMRT.a);*/
	    				tempMedPar=tempMedParede(malha);
	    				double dtp;
	    				for(int i=0;i<nPar;i++){
	    					int iFlux2=indPar2[i];

	    					vecTransfer[i].FeiticoDoTempo();
	    					for(int j=0; j<malha.mlh2d[iFlux2].cel2D.nvert;j++){
	    						if(malha.mlh2d[iFlux2].kvizinho[j]<0){
	    							int kcc=0;
	    							while(kcc<1 && malha.mlh2d[iFlux2].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
	    							if(kcc<1 && malha.mlh2d[iFlux2].cel2D.ccFace[j]==CC.rotuloAcop){
	    								double contTemp=0.;
	    								vecTransfer[i].Tint=contTemp*malha.mlh2d[iFlux2].cel2D.tempC+(1-contTemp)*malha.mlh2d[iFlux2].tempF[j];
	    								//vecTransfer[i].Tint=malha.mlh2d[iFlux2].cel2D.tempC+0*malha.mlh2d[iFlux2].extrapSuaveCCN;
	    								dtp=-0.0001*vecTransfer[i].Tint;
	    							}
	    						}
	    					}
	    					/*vecTransfer[i].Tint+=dtp;
	    					double fluxTemp=vecTransfer[i].transtrans()/(M_PI*dutosMRT.a);
	    					vecTransfer[i].FeiticoDoTempo();
	    					vecTransfer[i].Tint-=dtp;*/
		    				vecFluxcal[i]=-600+0* vecTransfer[i].transtrans()/(M_PI*dutosMRT.a);
		    				//malha.mlh2d[iFlux2].DCCN=(vecFluxcal[i]-fluxTemp)/(-dtp);

	    				}
	    				for(int i=0; i<malha.nele;i++){
	    					for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    						if(malha.mlh2d[i].kvizinho[j]<0){
	    							int kcc=0;
	    							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
	    							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
	    								/*if(tempMedPar!=transfer.Textern1)malha.mlh2d[i].ccTVN[j]=fluxcal*fabsl(malha.mlh2d[i].tempF[j]-transfer.Textern1)/
	    										fabsl(tempMedPar-transfer.Textern1);
	    								else malha.mlh2d[i].ccTVN[j]=0.;
	    								//malha.mlh2d[i].ccTVN[j]=fluxcal*malha.mlh2d[i].tempF[j]/transfer2.Tcamada[0][0];
	    								if(fluxcal<0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)<0.)malha.mlh2d[i].ccTVN[j]=0.;
	    								else if(fluxcal>0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)>0.)malha.mlh2d[i].ccTVN[j]=0.;
	    								//malha.mlh2d[i].ccTVN[j]=fluxcal;*/
	    								int iFlux=indPar[i];
	    								malha.mlh2d[i].ccTVN[j]=-0*800*fabsl(malha.mlh2d[i].tempF[j]-transfer.Textern1)/
	    										fabsl(60-transfer.Textern1)+1*vecFluxcal[iFlux];
	    								if(fluxcal<0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)<0.)malha.mlh2d[i].ccTVN[j]=0.;
	    								else if(fluxcal>0. && (malha.mlh2d[i].tempF[j]-transfer.Textern1)>0.)malha.mlh2d[i].ccTVN[j]=0.;
	    							}
	    						}
	    					}
	    				}
	    			}
	    		}
	    	}
	    	totalizaIterTemp+=iterTemp;
	    	for(int i=0; i<malha.nele;i++) malha.mlh2d[i].calcForcCorp();

///////////////////////////////////////////////      FIM ENERGIA           ////////////////////////////////////////////////////////////////////



  		iterTempo++;

  		if(velmed<0.001){
  			multEps=1.;
  			multiRes=10.;
  			multIter=1;
  		}
  		else if(velmed<0.005){
  			multEps=1.;
  			multiRes=1.;
  			multIter=1;
  		}
  		else if(velmed<0.01){
  			multEps=1.;
  			multiRes=1.;
  		}
  		else{
  			multEps=1.;
  			multiRes=1.;
  		}

  		/*if(acop==1){
  			transfer.FeiticoDoTempo();
  			transfer.dt=delt;
  			fluxcal=transfer.transtrans()/(M_PI*dutosMRT.a);
  			for(int i=0; i<malha.nele;i++){
  				if(acop==1 && (*vg1dSP).temInterface==0){
  					double pres=malha.mlh2d[i].cel2D.presC;
  					double temp=malha.mlh2d[i].cel2D.tempC;
  					malha.mlh2d[i].cel2D.visc=fluc.VisFlu(pres, temp)/1000.;
  				}
  				for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
  					if(malha.mlh2d[i].kvizinho[j]<0){
  						int kcc=0;
  						while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
  						if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
  							if(tempMedPar!=transfer.Textern1)malha.mlh2d[i].ccTVN[j]=fluxcal*fabsl(malha.mlh2d[i].cel2D.tempC-transfer.Textern1)/
  									fabsl(tempMedPar-transfer.Textern1);
  							else malha.mlh2d[i].ccTVN[j]=0.;
  						}
  					}
  				}
  			}
  		}*/
	    	//normaTemp=0;////temporario
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		/*int iterTemp=0;
		while(normaTemp>erroT && iterTemp<100){
			for(int i=0; i<malha.nele;i++) malha.mlh2d[i].calcGradGreenTemp();
			for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocalT(relax);
			indsra=0;
			for(int i=0;i<malha.nele;i++){
				int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
				for(int j=0;j<dimNoZero;j++)
					matglobT.sra[indsra+j]=malha.mlh2d[i].localT.mx[0][j];
				indsra+=dimNoZero;
				termolivreTemp[i]=malha.mlh2d[i].TLT[0];
			}
			double eps=0.0000001;
			iter=100;
			if(iter<malha.nele)iter=malha.nele;
			int retorna;
			for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.tempC;
			retorna=matglobT.GMRES(malha.vecSolv,termolivreTemp,eps,iter,1,malha.nele-1);
			if(retorna==1)break;
			normaTemp=0.;
			for(int i=0;i<malha.nele;i++){
				malha.mlh2d[i].cel2D.tempCI=malha.mlh2d[i].cel2D.tempC;
				malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
				normaTemp+=fabsl(malha.mlh2d[i].cel2D.tempC-malha.mlh2d[i].cel2D.tempCI);
			}
			normaTemp/=malha.nele;
			iterTemp++;
		}*/
		if(confinado==1){
			double pres0=malha.mlh2d[(*vg1dSP).eleYmaxWall].cel2D.presC;
			double delp=1.*(tempVF.pRef-pres0);
#pragma omp parallel for num_threads(nthrdLocal)
			for(int i=0; i<malha.nele;i++){
				malha.mlh2d[i].cel2D.presC=malha.mlh2d[i].cel2D.presC+delp;
			}
		}
#pragma omp parallel for num_threads(nthrdLocal)
  	for(int i=0; i<malha.nele;i++){
  		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
  			malha.mlh2d[i].cel2D.gradGreenHolI[j]=malha.mlh2d[i].cel2D.gradGreenHol[j];
  		}
  	}
		//calculando o Gradiente de holdup e os valores de holdup nas faces do elemento
//#pragma omp parallel for num_threads(nthrdLocal)
		for(int i=0; i<malha.nele;i++){
			malha.mlh2d[i].calcGradGreenHol();
			malha.mlh2d[i].cel2D.holC0=malha.mlh2d[i].cel2D.holC;
		}

		double tempfluidmed;
		double viscfluidmed;
		double taxafluidmed;
		double velfluidmed;

		resetTrend+=delt;
		if((*vg1dSP).acop==1){
			fluxCalMed=fluxMed(malha);
			//tempMedPar=transfer.Tint=tempMedParede(malha);
			tempMedPar=tempMedParede(malha);
			tempfluidmed=tempMed(malha);
			viscfluidmed=viscMed(malha);
			taxafluidmed=taxaMed(malha);
			velfluidmed=velMed(malha);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			  double JmixRef=0.;
			  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			  transfer2.Vint = 0.;
			  transfer2.dt = delt;
			  double condliq = prop.cond[0];
			  transfer2.kint =prop.cond[0];
			  transfer2.cpint = prop.cp[0];
			  transfer2.rhoint = prop.rho[0];
			  double visci;
			  if(viscVar==1){
				  if(flucVF.naoN==0) visci=flucVF.VisFlu(1.,transfer2.Tint)/1000.;
				  else{
					  double taxaDef=flucVF.viscNaoNew[0][1];
					  visci=flucVF.VisFlu(1.,transfer2.Tint,taxaDef)/1000.;
				  }
			  }
			  else visci=prop.visc[0];
			  transfer2.viscint = visci;
			  transfer2.betint= prop.beta[0];
			  double fluxcal;
			  fluxcal= transfer2.transtrans();
			  //fluxcal= transfer2.transperm();
			  double dia = dutosMRT.a;
			  double area = 0.25 * M_PI * dia * dia;
			  double coefTempo = (transfer2.rhoint * transfer2.cpint) * area;
			  transfer2.Tint = transfer2.Tint+ fluxcal/ (coefTempo / delt);
			///////////////////////////////////////////////////////////////////////////////////////////////////////////

			if(resetTrend>tempVF.tendTemp){
				ofstream escreveIni(tmp.c_str(), ios_base::app);
				escreveIni << (*vg1dSP).tempo <<"   ;   "<< tempfluidmed<<"  ;   "<<
						tempMedPar<<"   ;   "<< transfer2.Tint<<"  ;   "<<transfer2.Tcamada[0][0]<<
						"  ;   "<<fluxCalMed<<"  ;   "<<viscfluidmed<<"  ;   "<<taxafluidmed<<"  ;   "<<velfluidmed<<endl;
				escreveIni.close();
			}
		}

		if(resetTrend>tempVF.tendTemp)resetTrend=0.;


		if(isnan(maxresi))NumError("residuo NaN");
		if(isnan(norma0))NumError("norma0 NaN");
		if(isnan(norma1))NumError("norma1 NaN");
		if(isnan(maxresiT))NumError("maxresiT NaN");

		totalIter=iterTempo;
		end = chrono::steady_clock::now();
		cout << "Tempo= " << (*vg1dSP).tempo;
		cout << endl;
		cout << "Residuo= " << maxresi<<"; norma0= "<<norma0<<"; norma1= "<<norma1<<"; normaTemp= "<<normaTemp;
		cout << endl;
		cout <<"iterTempo= "<<iterTempo<<"; iterTemp= "<<totalizaIterTemp <<"; iter= "<<iter<<"; iterU= "<<iterU<<"; iterV= "<<iterV;
		cout << endl;
		cout << "ResiduoT= " << maxresiT<<" CFL= "<<deltCFL;
		cout << endl;
	    cout << " duracao avanco:  " << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	    cout << endl;
		cout <<"/////////////////////////////////////////////////////////////////////////////////////////";
		cout << endl;

		if (tempVF.parserieImp > 0) {
		    if (((*vg1dSP).tempo> 1e-15 && (*vg1dSP).tempo <= tempVF.tempoImp[kontaTempoProf] &&
		    		(*vg1dSP).tempo + delt >= tempVF.tempoImp[kontaTempoProf])) {


				malha.imprimeP((*vg1dSP).tempo);
				malha.imprimeU((*vg1dSP).tempo);
				malha.imprimeV((*vg1dSP).tempo);
				malha.imprimeT((*vg1dSP).tempo);
				malha.imprimePCor((*vg1dSP).tempo);
		        if((*vg1dSP).temInterface==1)
				malha.imprimeHol((*vg1dSP).tempo);
				malha.imprimeTaxaDeform((*vg1dSP).tempo);
				malha.imprimeVisc((*vg1dSP).tempo);
				malha.imprimeQcont((*vg1dSP).tempo, nPar);
				malha.imprimeTcont((*vg1dSP).tempo, nPar);
				//imprimeMalhaRegular(malha, 1);
				//imprimeMalhaRegular(malha, 2);
				//imprimeMalhaRegular(malha, 3);
				//imprimeMalhaRegular(malha, 4);
				//imprimeMalhaRegular(malha, 5);
				//if((*vg1dSP).temInterface==1)
				//imprimeMalhaRegular(malha, 6);
		    	tempVF.tempoImp[kontaTempoProf] = (*vg1dSP).tempo;
		    	kontaTempoProf++;
		    	if (kontaTempoProf >= tempVF.parserieImp) kontaTempoProf--;
		    }
		}

  	for(int i=0; i<malha.nele;i++){
  		malha.mlh2d[i].cel2D.uC0=malha.mlh2d[i].cel2D.uC;
  		malha.mlh2d[i].cel2D.vC0=malha.mlh2d[i].cel2D.vC;
  		malha.mlh2d[i].cel2D.uCI=malha.mlh2d[i].cel2D.uC;
  		malha.mlh2d[i].cel2D.vCI=malha.mlh2d[i].cel2D.vC;
			malha.mlh2d[i].cel2D.tempC0=malha.mlh2d[i].cel2D.tempC;
			malha.mlh2d[i].cel2D.presC0=malha.mlh2d[i].cel2D.presC;
  		for(int face=0;face<nvert;face++){
  			malha.mlh2d[i].uRC0[face]=malha.mlh2d[i].uRC[face];
  			malha.mlh2d[i].vRC0[face]=malha.mlh2d[i].vRC[face];
  		}
  		malha.mlh2d[i].calcForcCorp();
  	}
  	(*vg1dSP).tempo+=delt;
	}

    if(nele>0){
    	for(int i=0;i<nele;i++) delete[] noEle[i];
    	delete[] noEle;
    }
    if(nno>0){
    	for(int i=0;i<nno;i++) delete[] xcoor[i];
    	delete[] xcoor;
    	delete[] atributo;
    	delete[] tipo;
    }
    /*if(nburaco>0){
    	for(int i=0;i<nburaco;i++) delete[] noBuraco[i];
    	delete[] noBuraco;
    }*/
   /* if(nxMH>0){
    	for(int i=0;i<nxMH;i++) delete[] malhaH[i];
    	delete[] malhaH;
    }*/
    if(nele>0)delete[] vecsraU;
    if(nele>0)delete[] vecsraV;
    if(nele>0)delete[] vecsraPCor;
    if(nele>0)delete[] vecsraTemp;
    if(nele>0) delete[] veclm;
    if(nele>0)delete[] vecfnz;
    if(nPar>0){
    	delete[] vecTransfer;
    	delete[] vecFluxcal;
    }

    /*if(acop==1){
    	delete[] matVF;
		delete[] corteVF.diam;
		delete[] corteVF.indmat;
		delete[] corteVF.discre;
    }*/
}
