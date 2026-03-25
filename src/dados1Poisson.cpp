/*
 * dados1Poisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */

#include "dados1Poisson.h"

dadosP::dadosP(string nomeArquivoEntrada, double vCondGlob,double vCondLoc, double vhE, double vhInt,
		double vTint, double vTamb,double vdiamI, double vdiamE){
	iniciaVariaveis();
	lerPoisson(nomeArquivoEntrada, vCondGlob,vCondLoc,vhE,vhInt,
			vTint,vTamb,vdiamI,vdiamE);
}

dadosP::dadosP(){
	iniciaVariaveis();
}

dadosP::dadosP(const dadosP& vPoisson){
	iniciaVariaveis();
	lerPoisson(vPoisson.entrada,vPoisson.condGlob,vPoisson.condLoc,vPoisson.hE,vPoisson.hI,vPoisson.tInt,vPoisson.tAmb,
			vPoisson.diamInt,vPoisson.diamExt);
}

dadosP& dadosP::operator =(const dadosP& vPoisson) {
	if (this != &vPoisson) {
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
		if(noZero>0)delete[] vecsra;
		if(noZero>0)delete[] veclm;
		if(noZero>0)delete[] vecfnz;

		iniciaVariaveis();
		if(vPoisson.nele!=0)
		lerPoisson(vPoisson.entrada,vPoisson.condGlob,vPoisson.condLoc,vPoisson.hE,vPoisson.hI,vPoisson.tInt,vPoisson.tAmb,
				vPoisson.diamInt,vPoisson.diamExt);
	}

	return *this;
}

void dadosP::iniciaVariaveis(){

	//dyH=0;
	//dxH=0;

	nno=0;
	nele=0;
	noZero=0;
	noEle=0;
	atributo=0;
	tipo=0;
	vecsra=0;
	veclm=0;
	vecfnz=0;
	vecsra=0;
	veclm=0;
	vecfnz=0;
	xcoor=0;

	tempo=0;
	acop=1;
	angAcop=2.*M_PI;
	tParede=0;
	tInt=0;
	tAmb=0;

	hE=0;
	hI=0;
	condGlob=0;
	condLoc=0;
	qDesacop=0;
	qAcop=0;
	qTotal=0;
	tInt0=tInt;

	diamInt=0;
	diamExt=0;

	//centroX=0;
	//centroY=0;
	//nxMH=0;
	//nyMH=0;
	xmin=0;
	xmax=0;
	ymin=0;
	ymax=0;

	temp=detTempoPoisson();
	prop=detPropPoisson();
	CI=detCIPoisson();
	CC=detCCPoisson();

	unv=0;
	elearq="default.ele";
	noarq="default.node";
	polyarq="default.poly";
	entrada="parametros.json";

	dimExt=-1;

	nthrdLocal=1;

	rankLU=-1;
	colore=0;
	solverMat=0;
}

void dadosP::lerPoisson(string nomeArquivoEntrada, double vCondGlob,double vCondLoc, double vhE, double vhInt,
		double vTint, double vTamb,double vdiamI, double vdiamE){


	entrada=nomeArquivoEntrada;
	FILE *fp = fopen(nomeArquivoEntrada.c_str(), "r");
		//FILE *fp = fopen("/home/bwz1/teste2.json", "r");
	char readBuffer[125536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document elementoPoissonRaiz;
	elementoPoissonRaiz.ParseStream(is);
	fclose(fp);

	assert(elementoPoissonRaiz.HasMember("malha"));

	if (elementoPoissonRaiz.HasMember("malha")){
				parse_malha(elementoPoissonRaiz["malha"]);
	} else {
				exit(-1);
	}
	parse_configIni(elementoPoissonRaiz["configuracaoInicial"]);
	parse_CC(elementoPoissonRaiz["CC"]);
	parse_Prop(elementoPoissonRaiz["prop"]);
	parse_CI(elementoPoissonRaiz["CI"]);
	hE=vhE;
	hI=vhInt;
	condGlob=vCondGlob;
	condLoc=vCondLoc;
	tInt=vTint;
	tAmb=vTamb;
	diamInt=vdiamI;
	diamExt=vdiamE;

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

	noZero=0;
	for(int elem=0;elem<nele;elem++){
		int face;
		noZero++;
		for(int i=0;i<nvert;i++){
			face=-1;
			int v1=noEle[elem][i];
			int v2;
			if(i<nvert-1) v2=noEle[elem][i+1];
			else v2=noEle[elem][0];
			int match=0;
			for(int j=0;j<nele;j++){
				if(j!=elem){
					for(int k=0;k<nvert;k++){
						int v1V=noEle[j][k];
						int v2V;
						if(k<nvert-1) v2V=noEle[j][k+1];
						else v2V=noEle[j][0];
						if((v1V==v1 || v1V==v2) && (v2V==v1 || v2V==v2)){
							match=1;
							face=j;
						}
						if(match==1)break;
					}
				}
				if(match==1)break;
			}
			if(face>=0)noZero++;
		}
	}
	vecsra=new double [noZero];
	veclm=new int [noZero];
	if(noZero>0){
		vecfnz=new int [nele+1];
		vecfnz[nele]=noZero;
	}

}


void dadosP::parse_configIni(Value& configuracaoInicial_json) {
	string chaveJson("#/configuracaoInicial");
/*
 * 	int perm;
	int trans;
	double relax;
	int parserie;
	double* dtmax;
	double* tempo;
 */

	temp.perm=1;
	temp.trans=1;
	temp.relax=1.;
	temp.parserie=0;
	temp.dtmax=0;
	temp.tempo=0;
	temp.tmax=0.;
	temp.tempoImp=0;
	temp.parserieImp=0;
	//centroX=-1000.;
	//centroY=-1000.;

	if (configuracaoInicial_json.HasMember("threadLocal"))
		nthrdLocal = configuracaoInicial_json["threadLocal"].GetInt();

	if (configuracaoInicial_json.HasMember("acop"))
		acop = configuracaoInicial_json["acop"].GetInt();
	if(acop==1){
		//tInt = tempo_json["tempInterna"].GetDouble();
		//tAmb = tempo_json["tempAmbiente"].GetDouble();
		angAcop= configuracaoInicial_json["anguloAcoplamento"].GetDouble();
		//centroX= configuracaoInicial_json["centroX"].GetDouble();
		//centroY= configuracaoInicial_json["centroY"].GetDouble();
	}

	//if (tempo_json.HasMember("tmax"))
		//temp.tmax =
				//tempo_json["tmax"].GetDouble();
	//if (configuracaoInicial_json.HasMember("perm"))
		//temp.perm =
				//configuracaoInicial_json["perm"].GetBool();
	//if (configuracaoInicial_json.HasMember("trans"))
		//temp.trans =
				//configuracaoInicial_json["trans"].GetBool();
	if (configuracaoInicial_json.HasMember("relax"))
		temp.relax =
				configuracaoInicial_json["relax"].GetDouble();

	//if (tempo_json.HasMember("dtMax"))
		//temp.parserie =
			//	tempo_json["dtMax"].Size();
	//else if(temp.trans==1)logger.log(LOGGER_FALHA,
			//LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					//"Chave 'dtMax' inexistente",chaveJson, "condicao de tempo");
	//if(tempo_json["dtMax"].Size()!=tempo_json["tempo"].Size()){
		//logger.log(LOGGER_FALHA,
		//LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				//"Chaves 'dtMax', 'tempo' com dimensoes diferentes",
				//chaveJson, "Condicoes de tempo");
	//}
	temp.dtmax=0;//new double [temp.parserie];
	temp.tempo=0;//new double [temp.parserie];
	//for(int i=0; i<temp.parserie; i++){
		//temp.dtmax[i]=0.;
		//temp.tempo[i]=configuracaoInicial_json["tempo"][i].GetDouble();
	//}


	if (configuracaoInicial_json.HasMember("tempoImp"))
		temp.parserieImp =
				configuracaoInicial_json["tempoImp"].Size();
	else if(temp.trans==1)logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempoImp' inexistente para impressão do processo transiente",chaveJson, "condicao de tempo");
	temp.tempoImp=new double [temp.parserieImp];
	for(int i=0; i<temp.parserieImp; i++){
		temp.tempoImp[i]=configuracaoInicial_json["tempoImp"][i].GetDouble();
	}

	if (configuracaoInicial_json.HasMember("DimensaoReferenciaExterna"))
		dimExt =configuracaoInicial_json["DimensaoReferenciaExterna"].GetDouble();


	solverMat=0;
	if (configuracaoInicial_json.HasMember("solverMat"))
		solverMat = configuracaoInicial_json["solverMat"].GetInt();
	rankLU=-1;
	if (configuracaoInicial_json.HasMember("rankLU"))
		rankLU = configuracaoInicial_json["rankLU"].GetInt();
	colore=0;
	if(rankLU>=0){
		if (configuracaoInicial_json.HasMember("corLU"))
			colore = configuracaoInicial_json["corLU"].GetBool();
	}
}

void dadosP::parse_Prop(Value& prop_json){
	string chaveJson("#/prop");
	prop.parserie=prop_json.Size();
	prop.xmax=new double [prop.parserie];
	prop.xmin=new double [prop.parserie];
	prop.ymax=new double [prop.parserie];
	prop.ymin=new double [prop.parserie];
	prop.cond=new double [prop.parserie];
	prop.rho=new double [prop.parserie];
	prop.cp=new double [prop.parserie];
	for(int i=0;i<prop.parserie;i++){
		if (prop_json[i].HasMember("xmax")) prop.xmax[i]=prop_json[i]["xmax"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'xmax', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("xmin")) prop.xmin[i]=prop_json[i]["xmin"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'xmin',", chaveJson, "faltante");

		if (prop_json[i].HasMember("ymax")) prop.ymax[i]=prop_json[i]["ymax"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'ymax', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("ymin")) prop.ymin[i]=prop_json[i]["ymin"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'ymin', ", chaveJson, "faltante");
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

	}
}

void dadosP::parse_CI(Value& CI_json){
	string chaveJson("#/CI");
	CI.parserie=CI_json.Size();
	CI.xmax=new double [CI.parserie];
	CI.xmin=new double [CI.parserie];
	CI.ymax=new double [CI.parserie];
	CI.ymin=new double [CI.parserie];
	CI.temp=new double [CI.parserie];
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

	}
}

void dadosP::parse_CC(Value& CC_json) {
	string chaveJson("#/CC");
/*
 * 	int nDiri;
	int nRic;
	int nVN;
	detDiriPoisson* ccDir;
	detRicPoisson* ccRic;
	detVNPoisson* ccVN;
 */

	CC.nDiri=0;
	CC.ccDir=0;
	CC.nRic=0;
	CC.ccRic=0;
	CC.nVN=0;
	CC.ccVN=0;
	CC.rotuloAcop=-1;

	if (CC_json.HasMember("Dirichlet")){
		CC.nDiri =
				CC_json["Dirichlet"].Size();
		CC.ccDir=new detDiriPoisson [CC.nDiri];
	}
	if (CC_json.HasMember("Richardson")){
		CC.nRic =
				CC_json["Richardson"].Size();
		CC.ccRic=new detRicPoisson [CC.nRic];
	}
	if (CC_json.HasMember("VonNewman")){
		CC.nVN =
				CC_json["VonNewman"].Size();
		CC.ccVN=new detVNPoisson [CC.nVN];
	}
	if(acop==1){
		CC.rotuloAcop=CC_json["acoplamento"].GetInt();
	}

	for(int i=0; i<CC.nDiri; i++){
		/*
		 * 	int rotulo;
			double* val;
			double* tempo;
			int nserie;
		 */
		if(unv==0) CC.ccDir[i].rotulo=CC_json["Dirichlet"][i]["rotulo"].GetInt();
		else  CC.ccDir[i].rotuloSTR=CC_json["Dirichlet"][i]["rotuloSTR"].GetString();
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
		else  CC.ccVN[i].rotuloSTR=CC_json["VonNewman"][i]["rotuloSTR"].GetString();
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
		//CC.ccRic[i].nserie=CC_json["Richardson"][i]["valorAmb"].Size();
		CC.ccRic[i].nserie=1;
		/*if(CC_json["Richardson"][i]["valorAmb"].Size()!=CC_json["Richardson"][i]["tempo"].Size()){
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'valor', 'tempo' com dimensoes diferentes",
					chaveJson, "Condicoes de Contorno Richardson");
		}*/
		CC.ccRic[i].tempo=new double [CC.ccRic[i].nserie];
		CC.ccRic[i].valAmb=new double [CC.ccRic[i].nserie];
		CC.ccRic[i].hAmb=new double [CC.ccRic[i].nserie];
		for(int j=0; j<CC.ccRic[i].nserie; j++){
			CC.ccRic[i].tempo[j]=0.;//CC_json["Richardson"][i]["tempo"][j].GetDouble();
			CC.ccRic[i].valAmb[j]=0.;//CC_json["Richardson"][i]["valorAmb"][j].GetDouble();
			CC.ccRic[i].hAmb[j]=0.;//CC_json["Richardson"][i]["hAmb"][j].GetDouble();
		}
	}
}

void dadosP::parse_malha(Value& malha_json) {
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
	}
	else{
		if (malha_json.HasMember("elemento"))
			elearq = pathArqExtEntrada + string(malha_json["elemento"].GetString());
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'arquivo de elemento' inexistente",chaveJson, "malha");
	}

	/*if (malha_json.HasMember("poly"))
		polyarq = pathArqExtEntrada + string(malha_json["poly"].GetString());
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'arquivo de poly' inexistente",chaveJson, "malha");*/

	//if (malha_json.HasMember("dxMalhaRegular"))
		//dxH = malha_json["dxMalhaRegular"].GetDouble();
	//else logger.log(LOGGER_FALHA,
			//LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					//"Chave 'dxMalhaRegular' inexistente",chaveJson, "malha");

	//if (malha_json.HasMember("dyMalhaRegular"))
		//dyH = malha_json["dyMalhaRegular"].GetDouble();
	//else logger.log(LOGGER_FALHA,
			//LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					//"Chave 'dyMalhaRegular' inexistente",chaveJson, "malha");

}


