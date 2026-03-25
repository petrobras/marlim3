/*
 * dados1Poroso.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */

#include "dados1Poroso.h"

dadosPoro::dadosPoro(varGlob1D* Vvg1dSP,string nomeArquivoEntrada){
	vg1dSP=Vvg1dSP;
	iniciaVariaveis();
	lerPoroso(nomeArquivoEntrada);
}

dadosPoro::dadosPoro(){
	vg1dSP=0;
	iniciaVariaveis();
}

dadosPoro::dadosPoro(const dadosPoro& vPoroso){
	vg1dSP=vPoroso.vg1dSP;
	iniciaVariaveis();
	lerPoroso(vPoroso.entrada);
}

dadosPoro& dadosPoro::operator =(const dadosPoro& vPoroso) {
	if (this != &vPoroso) {
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
		if(acop==1){
			if(mat!=0)delete[] mat;
			if(corte.diam!=0)delete[] corte.diam;
			if(corte.indmat!=0)delete[] corte.indmat;
			if(corte.discre!=0)delete[] corte.discre;
		}


	    if(tabp>0){
			  for(int i=0; i<npontos+2;i++) delete[] zdranP[i];
			  delete [] zdranP;
			  for(int i=0; i<npontos+2;i++) delete[] dzdpP[i];
			  delete [] dzdpP;
			  for(int i=0; i<npontos+2;i++) delete[] dzdtP[i];
			  delete [] dzdtP;
	     }

	    if(flashCompleto==1){
		   for(int j=0; j<tabent.npont+1;j++) delete [] flash.rholF[j];
		   delete [] flash.rholF;
		   for(int j=0; j<tabent.npont+1;j++) delete[] flash.rhogF[j];
		   delete [] flash.rhogF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDpF[j];
	       delete [] flash.DrholDpF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDpF[j];
	       delete [] flash.DrhogDpF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDtF[j];
	       delete [] flash.DrholDtF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDtF[j];
	       delete [] flash.DrhogDtF;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhowDtF[j];
	       delete [] flash.DrhowDtF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.RSF[j];
	       delete [] flash.RSF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cplF[j];
	       delete [] flash.cplF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpgF[j];
	       delete [] flash.cpgF;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpwF[j];
	       delete [] flash.cpwF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HlF[j];
	       delete [] flash.HlF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HgF[j];
	       delete [] flash.HgF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigOGF[j];
	       delete [] flash.sigOGF;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigWGF[j];
	       delete [] flash.sigWGF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscO[j];//alteracao6
	       delete [] flash.viscO;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscG[j];//alteracao6
	       delete [] flash.viscG;

	       delete [] flash.PBF;
	       delete [] flash.TBF;
	   }

	   if(nvecEmul>0){
	    	  delete [] BSWVec;
	    	  delete [] emulVec;
	    }
		if(flup.corrOM==7){
			if(tabVisc.parserie>0){
				delete [] tabVisc.visc;
				delete [] tabVisc.temp;
			}
		}
		if(nsatAPoc>0){
			delete [] satAPoc;
			delete [] tempoSatAPoc;
		}
		if(nsatLRes>0){
			delete [] satLRes;
			delete [] tempoSatLRes;
		}
		vg1dSP=vPoroso.vg1dSP;
		iniciaVariaveis();
		lerPoroso(vPoroso.entrada);
	}

	return *this;
}

void dadosPoro::iniciaVariaveis(){
	zdranP=0;
	tabelaDinamica=0;
	emulVec=0;
	BSWVec=0;
	tabp=0;
	miniTabDp=0;
	nvecEmul=0;
	dzdtP=0;
	usaTabela=0;
	ippoco=0;
	miniTabDt=0;
	pParede=0;
	satAconat=0;
	dzdpP=0;
	miniTabAtraso=0;
	flashCompleto=0;

	mat=0;//vetor com os materiais cadastrados no Json

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
	nmaterial=0;
	acop=0;
	angAcop=0;
	pInt=0;
	pInt0=pInt;
	pRes=0.;
	tRes=0.;
	espessuraRes=0.;
	zDatumRef=0.;


	hE=0;
	hI=0;
	condGlob=0;
	condLoc=0;
	qDesacop=0;
	qAcop=0;
	qTotal=0;
	qTotalW=0.;
	qTotalO=0.;
	qTotalG=0.;

	centroX=0;
	centroY=0;
	nxMH=0;
	nyMH=0;
	xmin=0;
	xmax=0;
	ymin=0;
	ymax=0;

	dxH=0;
	dyH=0;

	satAPoc=0;
	satLRes=0;
	tempoSatAPoc=0;
	tempoSatLRes=0;
	nsatAPoc=0;
	nsatLRes=0;

	unv=0;
	elearq="default.ele";
	noarq="default.node";
	polyarq="default.poly";
	entrada="parametros.json";

	pBolha=1000.;
    BSW=0;
	tipoModelOleo=1;
	compOleo=1e-5;
	rhoRef=900;

	transfer=PorosRad();
	dutosMRT=DadosGeoPoro();
	corte=cortePoroso();
	CC=detCCPoroso();
	pW=detDiriPoroso();
	CI=detCIPoroso();
	prop=detPropPoroso();
	temp=detTempoPoroso();

	kRelOA=tabelaPemRelOA();
	kRelOG=tabelaPemRelOG();
	pcOA=tabelaPresCapOA();
	pcGO=tabelaPresCapGO();

	nthrdLocal=1;

	rankLU=-1;
	colore=0;
	solverMat=0;
}

void dadosPoro::parse_materiais(Value& material_json) {
		// obter o tamanho do elementoPoroso do Json
			nmaterial = material_json.Size();
		// caso o tamanho do elementoPoroso seja maior que zero
			if (nmaterial > 0) {
				// vetor de materiais
				mat = new materialPoroso[nmaterial];
				// loop para carga do vetor do JSON
				for (int i = 0; i < nmaterial; i++) {

					mat[i].kX =0;
					mat[i].kY =0;
					mat[i].poro =0;
					mat[i].compRoc =0;

					mat[i].kX = material_json[i]["permeabilidadeY"].GetDouble()*(9.869233/(1e16));
					mat[i].kY = material_json[i]["permeabilidadeY"].GetDouble()*(9.869233/(1e16));
					mat[i].poro = material_json[i]["porosidade"].GetDouble();
					mat[i].compRoc = material_json[i]["compressibilidadeRocha"].GetDouble();
				}
			}
}

void dadosPoro::lerPoroso(string nomeArquivoEntrada){


	entrada=nomeArquivoEntrada;
	FILE *fp = fopen(nomeArquivoEntrada.c_str(), "r");
		//FILE *fp = fopen("/home/bwz1/teste2.json", "r");
	  char readBuffer[125536];
	  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	  Document elementoPorosoRaiz;
	  elementoPorosoRaiz.ParseStream(is);
	  fclose(fp);

	  assert(elementoPorosoRaiz.HasMember("malha"));

	  parse_tempo(elementoPorosoRaiz["tempo"]);

	  if (elementoPorosoRaiz.HasMember("malha")){
				parse_malha(elementoPorosoRaiz["malha"]);
	  } else {
				exit(-1);
	  }
	  //parse_tempo(elementoRaiz["tempo"]);
	  //parse_PW(elementoPorosoRaiz["pW"]);

	  parse_kRelOA(elementoPorosoRaiz["kRelOA"]);
	  parse_kRelOG(elementoPorosoRaiz["kRelOG"]);
	  parse_pcOA(elementoPorosoRaiz["pcOA"]);
	  //parse_pcGO(elementoRaiz["pcGO"]);

	  if(elementoPorosoRaiz.HasMember("fluidoComplementar"))parse_fluido_complementar(elementoPorosoRaiz["fluidoComplementar"]);
	  else{
			int tipoflui=0;
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
			double sal =0.;
			fluc = ProFluCol(masesp, compP, compT, tensup, calesp,
						condut, sal, templ, lvisl, temph, lvish,0, tipoflui);
			fluc.npontos = 0;
	  }
	  if(tabp>0){
			// realizar o parse da chave tabela
			parse_tabela(elementoPorosoRaiz["tabelaZ"]);
			zdranP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
					zdranP[i] = new double[npontos + 2];
			dzdpP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
					dzdpP[i] = new double[npontos + 2];
			dzdtP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
					dzdtP[i] = new double[npontos + 2];

	  }
	  parse_fluido_producao(elementoPorosoRaiz["fluidosProducao"]);

	  if(tabp>0){
			double TCMin = 0;  //15-06-2018
			double TCMax = 0;  //15-06-2018
			double PCMin = 0;  //15-06-2018
			double PCMax = 0;  //15-06-2018
			if(corrDeng==0){
				TCMin = flup.TC;  //15-06-2018
				TCMax = flup.TC;  //15-06-2018
				PCMin = flup.PC;  //15-06-2018
				PCMax = flup.PC;  //15-06-2018
			}
			else{
				double PC= 708.75 - 57.5* 0.5538;
				double TC= 169 + 314.0* 0.5538;
				TCMin = TC;
				TCMax = TC;
				PCMin = PC;
				PCMax = PC;
			}
			if (flup.TC < TCMin)
							TCMin = flup.TC;
			if (flup.TC > TCMax)
							TCMax = flup.TC;
			if (flup.PC < PCMin)
							PCMin = flup.PC;
			if (flup.PC > PCMax)
							PCMax = flup.PC;
			tabentCrit.npont = tabent.npont;  //15-06-2018
			tabentCrit.tmin = (tabent.tmin + 273.15)
							/ ((TCMax - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
			tabentCrit.tmax = (tabent.tmax + 273.15)
							/ ((TCMin - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
			tabentCrit.pmin = tabent.pmin * 0.9678411 * 14.69595 / PCMax; //15-06-2018
			tabentCrit.pmax = tabent.pmax * 0.9678411 * 14.69595 / PCMin; //15-06-2018

			flup.tabela(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
							tabentCrit.tmin);
			flup.tabelaDZDP(tabentCrit.pmax, tabentCrit.tmax,
							tabentCrit.pmin, tabentCrit.tmin);
			flup.tabelaDZDT(tabentCrit.pmax, tabentCrit.tmax,
							tabentCrit.pmin, tabentCrit.tmin);
	  }
	  pBolha=flup.PB(1., tRes)/(0.9678411*14.69595);
	  if(tipoModelOleo==0){
		  rhoRef=flup.MasEspoleo(pRes, tRes);
		  double dpPres=0.01*pRes;
		  compOleo=(1/rhoRef)*(flup.MasEspoleo(pRes+dpPres, tRes)-rhoRef)/dpPres;
	  }
	  parse_CC(elementoPorosoRaiz["CC"]);
	  parse_Prop(elementoPorosoRaiz["prop"]);
	  parse_CI(elementoPorosoRaiz["CI"]);
	  double ki;
	  double cpi;
	  double rhoi;
	  double visci;
	  if(acop==1){
		  parse_materiais(elementoPorosoRaiz["material"]);
		  parse_corte(elementoPorosoRaiz["secaoTransversal"]);
		  parse_PW(elementoPorosoRaiz["pW"]);
		  ////////////////////////////////////////////////////////////////////////
		  int cam = corte.ncam;
		  double* vky;
		  vky = new double[cam];
		  for (int j = 0; j < cam; j++)
				vky[j] = mat[corte.indmat[j]].kY;
		  double* vkx;
		  vkx = new double[cam];
		  for (int j = 0; j < cam; j++)
				vkx[j] = mat[corte.indmat[j]].kX;
		  double* vporo;
		  vporo = new double[cam];
		  for (int j = 0; j < cam; j++)
				vporo[j] = mat[corte.indmat[j]].poro;
		  double* vcompRoc;
		  vcompRoc = new double[cam];
		  for (int j = 0; j < cam; j++)
				vcompRoc[j] = mat[corte.indmat[j]].compRoc;

		  dutosMRT = DadosGeoPoro(corte.a, corte.b,	corte.ncam, vkx, corte.diam, vky, vporo, vcompRoc);
		  delete[] vkx;
		  delete[] vky;
		  delete[] vporo;
		  delete[] vcompRoc;
		  //////////////////////////////////////////////////////////////////////////////////////////
		  int* vncamada;
			vncamada = new int[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) vncamada[i] = corte.discre[i];
			double* vdrcamada;
			vdrcamada = new double[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				if (i > 0)
					vdrcamada[i] = 0.5 * (dutosMRT.diamC[i] - dutosMRT.diamC[i - 1])
							/ vncamada[i];
				else
					vdrcamada[0] = 0.5 * (dutosMRT.diamC[0] - dutosMRT.a) / vncamada[0];
			}

			double** vPcamada;
			vPcamada = new double*[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				vPcamada[i] = new double[vncamada[i] + 1];
			}

			int equilterm=1;

			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				for (int j = 0; j <= vncamada[i]; j++)
					vPcamada[i][j] = pRes;
			}

			double** vsO;
			vsO = new double*[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				vsO[i] = new double[vncamada[i] + 1];
			}


			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				for (int j = 0; j <= vncamada[i]; j++)
					vsO[i][j] = satLRes[0];
			}

			double** vsW;
			vsW = new double*[dutosMRT.ncamadas];
			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				vsW[i] = new double[vncamada[i] + 1];
			}


			for (int i = 0; i < dutosMRT.ncamadas; i++) {
				for (int j = 0; j <= vncamada[i]; j++)
					vsW[i][j] = satAPoc[0];
			}

			int perm = -1;

			pInt=pW.val[0];
			transfer = PorosRad(dutosMRT,vg1dSP, perm, vncamada, vdrcamada, vPcamada,
					pInt, pRes, tRes,temp.dtmax[0],pW.val[0],corte.zDatum0-zDatumRef, corte.zDatum1-zDatumRef,espessuraRes,
					vsO,vsW,satLRes[0],satAPoc[0],satAPoc[0],satAconat,ippoco,
					kRelOA,kRelOG,pcOA,pcGO,fluc,flup); // alteracao2 // alteracao0
			transfer.condiTparede=1;
			transfer.pBolha=pBolha;
			transfer.tipoModelOleo=tipoModelOleo;
			transfer.compOleo=compOleo;
			transfer.rhoRef=rhoRef;
			for(int i=0;i<transfer.ncel;i++){
				transfer.celula[i].pBolha=pBolha;
				transfer.celula[i].tipoModelOleo=tipoModelOleo;
				transfer.celula[i].compOleo=compOleo;
				transfer.celula[i].rhoRef=rhoRef;
			}

			//if (equilterm == 1)
				//transfer.transperm();
			delete[] vncamada;
			delete[] vdrcamada;
			for (int i = 0; i < dutosMRT.ncamadas; i++)
				delete[] vPcamada[i];
			delete[] vPcamada;
			delete[] vsO;
			delete[] vsW;

			int ind;
			double raz;
			double delt;
			double presW;
			int kontaTempoImp=0;
			double satAResAtual;
			double satOResAtual;
			/////////////////////////////////////////////////////////////////////////////////////////////////////


	  }


	/*string elefile,nofile,volfile,polyfile;
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
  int nvert=3;
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
  lendoele.close();*/
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
  vecfnz=new int [nele+1];
  vecfnz[nele]=noZero;

}

void dadosPoro::parse_corte(Value& corte_json) {
	// de-para do elementoPoroso do json para o struct
	corte.a = corte_json["diametroInterno"].GetDouble();
	corte.b = corte.a;
	corte.ncam = corte_json["camadas"].Size();
	corte.zDatum1 = corte_json["zInterno"].GetDouble();
	corte.zDatum0 = corte_json["zPoco"].GetDouble();
	corte.diam = new double[corte.ncam];
	corte.indmat = new int[corte.ncam];
	corte.discre = new int[corte.ncam];
	// percorrer a lista de camadas da seção transversal
	for (int j = 0; j < corte.ncam; j++) {
		double esp=corte_json["camadas"][j]["espessura"].GetDouble();
		if(j==0)corte.diam[j]=corte.a+2.*esp;
		else corte.diam[j]=corte.diam[j-1]+2.*esp;

		corte.discre[j] =
				corte_json["camadas"][j]["discretizacao"].GetInt();
		corte.indmat[j] =
				corte_json["camadas"][j]["idMaterial"].GetInt();
	}
}

void dadosPoro::parse_tabela(Value& tabela_json) {
	/*
	 * "tabela": { "ativo": false, "nPontos": 21, "pressaoMaxima": 600, "pressaominima": 2, "temperaturaMaxima": 200, "temperaturaMinima": -100 }
	 */

		// de-para do elemento "tabela" obtido do json para o struct correspondente
		// de-para do elemento "tabela" obtido do json para o struct correspondente
		tabent.npont = tabela_json["nPontos"].GetInt();
		tabent.pmax = tabela_json["pressaoMaxima"].GetDouble();
		tabent.pmin = tabela_json["pressaoMinima"].GetDouble();
		tabent.tmax = tabela_json["temperaturaMaxima"].GetDouble();
		tabent.tmin = tabela_json["temperaturaMinima"].GetDouble();
		npontos = tabent.npont;
}

void dadosPoro::parse_kRelOA(Value& kRelOA_json) {
	kRelOA.npont= kRelOA_json["satA"].Size();
	kRelOA.permRelOW=new double[kRelOA.npont];
	kRelOA.permRelW=new double[kRelOA.npont];
	kRelOA.satW=new double[kRelOA.npont];
	for(int i=0;i<kRelOA.npont;i++){
		kRelOA.satW[i]=kRelOA_json["satA"][i].GetDouble();
		kRelOA.permRelW[i]=kRelOA_json["kRelA"][i].GetDouble();
		kRelOA.permRelOW[i]=kRelOA_json["kRelOA"][i].GetDouble();
	}

	/*kRelOG.npont= 1;
		kRelOG.permRelG=new double[kRelOG.npont];
		kRelOG.permRelOG=new double[kRelOG.npont];
		kRelOG.satG=new double[kRelOG.npont];
		for(int i=0;i<kRelOG.npont;i++){
			kRelOG.satG[i]=0.;
			kRelOG.permRelG[i]=0.;
			kRelOG.permRelOG[i]=0.;
		}*/
}

void dadosPoro::parse_kRelOG(Value& kRelOG_json) {
	kRelOG.npont= kRelOG_json["satG"].Size();
	kRelOG.permRelG=new double[kRelOG.npont];
	kRelOG.permRelOG=new double[kRelOG.npont];
	kRelOG.satG=new double[kRelOG.npont];
	for(int i=0;i<kRelOG.npont;i++){
		kRelOG.satG[i]=kRelOG_json["satG"][i].GetDouble();
		kRelOG.permRelG[i]=kRelOG_json["kRelG"][i].GetDouble();
		kRelOG.permRelOG[i]=kRelOG_json["kRelOG"][i].GetDouble();
	}
	/*kRelOG.npont= 1;
		kRelOG.permRelG=new double[kRelOG.npont];
		kRelOG.permRelOG=new double[kRelOG.npont];
		kRelOG.satG=new double[kRelOG.npont];
		for(int i=0;i<kRelOG.npont;i++){
			kRelOG.satG[i]=0.;
			kRelOG.permRelG[i]=0.;
			kRelOG.permRelOG[i]=0.;
		}*/
}

void dadosPoro::parse_pcOA(Value& pcOA_json) {
	//psi
	pcOA.npont= pcOA_json["satA"].Size();
	pcOA.satW=new double[pcOA.npont];
	pcOA.presCapOW=new double[pcOA.npont];
	for(int i=0;i<pcOA.npont;i++){
		pcOA.satW[i]=pcOA_json["satA"][i].GetDouble();
		pcOA.presCapOW[i]=pcOA_json["pcOA"][i].GetDouble()/(14.69595*0.9678411);
	}

	pcGO.npont= 1;
	    pcGO.satG=new double[pcGO.npont];
		pcGO.presCapGO=new double[pcGO.npont];
		for(int i=0;i<pcGO.npont;i++){
			pcGO.satG[i]=0.;
			pcGO.presCapGO[i]=0.;
		}
}

void dadosPoro::parse_pcGO(Value& pcGO_json) {
	//psi
	/*pcGO.npont= pcGO_json["satG"].Size();
    pcGO.satG=new double[pcGO.npont];
	pcGO.presCapGO=new double[pcGO.npont];
	for(int i=0;i<pcGO.npont;i++){
		pcGO.satG[i]=pcGO_json["satG"][i].GetDouble();
		pcGO.presCapGO[i]=pcGO_json["pcGO"][i].GetDouble()/(14.69595*0.9678411);
	}*/
	pcGO.npont= 1;
	    pcGO.satG=new double[pcGO.npont];
		pcGO.presCapGO=new double[pcGO.npont];
		for(int i=0;i<pcGO.npont;i++){
			pcGO.satG[i]=0.;
			pcGO.presCapGO[i]=0.;
		}
}


void dadosPoro::geraTabFlash(int var) {

	string pvtfile;
	pvtfile = pvtsimarq;
	string dadosMR = pvtfile;
	ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
	string chave;
	char* tenta;
	//tenta = new char[400];
	double testatok;
	int ndiv = tabent.npont - 1;

	Vcr<double> presPVTSim(ndiv + 1);
	Vcr<double> tempPVTSim(ndiv + 1);

	FullMtx<double> VarTemp(ndiv + 2, ndiv + 2);

	char line[4000];
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	while (tenta != nullptr &&  strcmp(tenta, "PHASE") != 0) {
		tenta = strtok(NULL, " ,()=");
	}
	tenta = strtok(NULL, " ,()=");
	int lacoleitura;

	int threeOrtwo = 0;
	if (strcmp(tenta, "THREE") == 0) {
		threeOrtwo = 1;
		if (var == 1)
			lacoleitura = 1;  //rhog
		else if (var == 2)
			lacoleitura = 2;  //rhol
		else if (var == 3)
			lacoleitura = 4;  //drhogdp
		else if (var == 4)
			lacoleitura = 5;  //drholdp
		else if (var == 5)
			lacoleitura = 7;  //dhhogdt
		else if (var == 6)
			lacoleitura = 8;  //drholdt
		else if (var == 7)
			lacoleitura = 9;  //drhowdt
		else if (var == 8)
			lacoleitura = 10;  //RS
		else if (var == 9)
			lacoleitura = 15;  //Cpg
		else if (var == 10)
			lacoleitura = 16;  //Cpl
		else if (var == 11)
			lacoleitura = 17;  //Cpw
		else if (var == 12)
			lacoleitura = 18;  //Hg
		else if (var == 13)
			lacoleitura = 19;  //Hl
		else if (var == 14)
			lacoleitura = 24;  //Sig g-l
		else if (var == 15)
			lacoleitura = 25;  //Sig g-w
		else if (var == 16)
			lacoleitura = 13;  //visc oleo//alteracao6
		else if (var == 17)
			lacoleitura = 12;  //visc gas//alteracao6
	} else {
		if (var == 1)
			lacoleitura = 1;  //rhog
		else if (var == 2)
			lacoleitura = 2;  //rhol
		else if (var == 3)
			lacoleitura = 3;  //drhogdp
		else if (var == 4)
			lacoleitura = 4;  //drholdp
		else if (var == 5)
			lacoleitura = 5;  //dhhogdt
		else if (var == 6)
			lacoleitura = 6;  //drholdt
		else if (var == 8)
			lacoleitura = 7;  //RS
		else if (var == 9)
			lacoleitura = 10;  //Cpg
		else if (var == 10)
			lacoleitura = 11;  //Cpl
		else if (var == 12)
			lacoleitura = 12;  //Hg
		else if (var == 13)
			lacoleitura = 13;  //Hl
		else if (var == 14)
			lacoleitura = 16;  //Sig g-l
		else if (var == 16)
			lacoleitura = 9;  //visc oleo//alteracao6
		else if (var == 17)
			lacoleitura = 8;  //visc gas//alteracao6
	}

	while (chave != "PRESSURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	presPVTSim[0] = atof(tenta) * 1.01971621e-5;
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		presPVTSim[kontaPVT] = atof(tenta) * 1.01971621e-5;
	}
	while (chave != "TEMPERATURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	tempPVTSim[0] = atof(tenta);
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		tempPVTSim[kontaPVT] = atof(tenta);
	}
	if(var==1){
		tabent.pmin=presPVTSim[0];
		tabent.pmax=presPVTSim[ndiv];
		tabent.tmin=tempPVTSim[0];
		tabent.tmax=tempPVTSim[ndiv];
	}

	for (int i = 1; i <= ndiv + 1; i++) {
		VarTemp[i][0] = presPVTSim[i - 1];
		for (int j = 1; j <= ndiv + 1; j++) {
			VarTemp[0][j] = tempPVTSim[j - 1];
			//HLatTemp[i][j]=arq.flup.EntalpGasIsoT(pteste,tteste)-arq.flup.EntalpLiqIsoT(pteste,tteste);
			while (chave != "POINT")
				lendoPVTSim >> chave;
			lendoPVTSim.get(line, 4000);
			tenta = strtok(line, " ,()=");
			for (int kontaPVT = 0; kontaPVT < lacoleitura; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			VarTemp[i][j] = testatok;
			//tenta = strtok(NULL, " ,");
			//testatok = atof(tenta);
			//VarTemp[i][j] -= testatok;

			if (i == ndiv + 1 && j == ndiv + 1)
				break;
			while (chave != "PVTTABLE")
				lendoPVTSim >> chave;
		}

	}
	lendoPVTSim.close();
	for (int i = 0; i < ndiv + 2; i++) {
		for (int j = 0; j < ndiv + 2; j++) {
			if (threeOrtwo == 1) {
				if (var == 1)
					flash.rhogF[i][j] = VarTemp[i][j];  //rhog
				else if (var == 2)
					flash.rholF[i][j] = VarTemp[i][j];  //rhol
				else if (var == 3)
					flash.DrhogDpF[i][j] = VarTemp[i][j]/**98066.14*/; //drhogdp
				else if (var == 4)
					flash.DrholDpF[i][j] = VarTemp[i][j]/**98066.14*/; //drholdp
				else if (var == 5)
					flash.DrhogDtF[i][j] = VarTemp[i][j];  //dhhogdt
				else if (var == 6)
					flash.DrholDtF[i][j] = VarTemp[i][j];  //drholdt
				else if (var == 7)
					flash.DrhowDtF[i][j] = VarTemp[i][j];  //drhowdt
				else if (var == 8)
					flash.RSF[i][j] = VarTemp[i][j];  //RS
				else if (var == 9)
					flash.cpgF[i][j] = VarTemp[i][j];  //Cpg
				else if (var == 10)
					flash.cplF[i][j] = VarTemp[i][j];  //Cpl
				else if (var == 11)
					flash.cpwF[i][j] = VarTemp[i][j];  //Cpw
				else if (var == 12)
					flash.HgF[i][j] = VarTemp[i][j];  //Hg
				else if (var == 13)
					flash.HlF[i][j] = VarTemp[i][j];  //Hl
				else if (var == 14)
					flash.sigOGF[i][j] = VarTemp[i][j];  //Sig g-l
				else if (var == 15)
					flash.sigWGF[i][j] = VarTemp[i][j];  //Sig g-w
				else if (var == 16)
					flash.viscO[i][j] = VarTemp[i][j]; //visc oleo //alteracao6
				else if (var == 17)
					flash.viscG[i][j] = VarTemp[i][j]; //visc gas //alteracao6

				if (var == 8 && i > 0 && j > 0) {
					double rhostd = 141.5 * 1000.
							/ (131.5 + flup.API);
					double rgstd = 1.225 * flup.Deng;
					flash.RSF[i][j] = (flup.RGO
							- (rhostd + rgstd * flup.RGO)
									* flash.RSF[i][j] / rgstd) * 35.31467
							/ 6.29;
					if(flash.RSF[i][j]<0)flash.RSF[i][j]=0.;
					//falcatrua-teste de consistencia de RS do PVTSIM
					//if(i>2 && flash.RSF[i][j]<flash.RSF[i-1][j])flash.RSF[i][j]=flash.RSF[i-1][j];
					VarTemp[i][j] = flash.RSF[i][j] * 6.29 / 35.31467;
				}
				if ((var == 16 || var == 17) && i > 0 && j > 0) {  //alteracao6
					if (var == 16)
						flash.viscO[i][j] *= 1000.;
					else
						flash.viscG[i][j] *= 1000.;
					VarTemp[i][j] *= 1000.;
				}
			} else {
				if (var == 1)
					flash.rhogF[i][j] = VarTemp[i][j];  //rhog
				else if (var == 2)
					flash.rholF[i][j] = VarTemp[i][j];  //rhol
				else if (var == 3)
					flash.DrhogDpF[i][j] = VarTemp[i][j]/**98066.5*/; //drhogdp
				else if (var == 4)
					flash.DrholDpF[i][j] = VarTemp[i][j]/**98066.5*/; //drholdp
				else if (var == 5)
					flash.DrhogDtF[i][j] = VarTemp[i][j];  //dhhogdt
				else if (var == 6)
					flash.DrholDtF[i][j] = VarTemp[i][j];  //drholdt
				else if (var == 8)
					flash.RSF[i][j] = VarTemp[i][j];  //RS
				else if (var == 9)
					flash.cpgF[i][j] = VarTemp[i][j];  //Cpg
				else if (var == 10)
					flash.cplF[i][j] = VarTemp[i][j];  //Cpl
				else if (var == 12)
					flash.HgF[i][j] = VarTemp[i][j];  //Hg
				else if (var == 13)
					flash.HlF[i][j] = VarTemp[i][j];  //Hl
				else if (var == 14)
					flash.sigOGF[i][j] = VarTemp[i][j];  //Sig g-l
				else if (var == 16)
					flash.viscO[i][j] = VarTemp[i][j]; //visc oleo //alteracao6
				else if (var == 17)
					flash.viscG[i][j] = VarTemp[i][j]; //visc gas //alteracao6

				if (var == 8 && i > 0 && j > 0) {
					double rhostd = 141.5 * 1000.
							/ (131.5 + flup.API);
					double rgstd = 1.225 * flup.Deng;
					flash.RSF[i][j] = (flup.RGO
							- (rhostd + rgstd * flup.RGO)
									* flash.RSF[i][j] / rgstd) * 35.31467
							/ 6.29;
					if(flash.RSF[i][j]<0)flash.RSF[i][j]=0.;
					//falcatrua-teste de consistencia de RS do PVTSIM
					//if(i>2 && flash.RSF[i][j]<flash.RSF[i-1][j])flash.RSF[i][j]=flash.RSF[i-1][j];
					VarTemp[i][j] = flash.RSF[i][j] * 6.29 / 35.31467;
				}
				if ((var == 16 || var == 17) && i > 0 && j > 0) {  //alteracao6
					if (var == 16)
						flash.viscO[i][j] *= 1000.;
					else
						flash.viscG[i][j] *= 1000.;
					VarTemp[i][j] *= 1000.;
				}
			}
		}

	}
	/*double del2e;
	 double del1e;
	 double del1d;
	 double del2d;
	 for (int i = 3; i < ndiv-1; i++) {
	 for (int j = 3; j < ndiv-1; j++){

	 if(var==1){
	 del2e=flash.rhogF[i-1][j]-flash.rhogF[i-2][j];
	 del1e=flash.rhogF[i][j]-flash.rhogF[i-1][j];
	 del1d=flash.rhogF[i+1][j]-flash.rhogF[i][j];
	 del2d=flash.rhogF[i+2][j]-flash.rhogF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.rhogF[i][j]=flash.rhogF[i-1][j];
	 }
	 else if(var==2){
	 //flash.rholF[i][j]= VarTemp[i][j];
	 del2e=flash.rholF[i-1][j]-flash.rholF[i-2][j];
	 del1e=flash.rholF[i][j]-flash.rholF[i-1][j];
	 del1d=flash.rholF[i+1][j]-flash.rholF[i][j];
	 del2d=flash.rholF[i+2][j]-flash.rholF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.rholF[i][j]=flash.rholF[i-1][j];
	 }
	 else if(var==3){
	 //flash.DrhogDpF[i][j]= VarTemp[i][j];
	 del2e=flash.DrhogDpF[i-1][j]-flash.DrhogDpF[i-2][j];
	 del1e=flash.DrhogDpF[i][j]-flash.DrhogDpF[i-1][j];
	 del1d=flash.DrhogDpF[i+1][j]-flash.DrhogDpF[i][j];
	 del2d=flash.DrhogDpF[i+2][j]-flash.DrhogDpF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrhogDpF[i][j]=flash.DrhogDpF[i-1][j];
	 }
	 else if(var==4){
	 //flash.DrholDpF[i][j]= VarTemp[i][j];
	 del2e=flash.DrholDpF[i-1][j]-flash.DrholDpF[i-2][j];
	 del1e=flash.DrholDpF[i][j]-flash.DrholDpF[i-1][j];
	 del1d=flash.DrholDpF[i+1][j]-flash.DrholDpF[i][j];
	 del2d=flash.DrholDpF[i+2][j]-flash.DrholDpF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrholDpF[i][j]=flash.DrholDpF[i-1][j];
	 }
	 else if(var==5){
	 //flash.DrhogDtF[i][j]= VarTemp[i][j];
	 del2e=flash.DrhogDtF[i-1][j]-flash.DrhogDtF[i-2][j];
	 del1e=flash.DrhogDtF[i][j]-flash.DrhogDtF[i-1][j];
	 del1d=flash.DrhogDtF[i+1][j]-flash.DrhogDtF[i][j];
	 del2d=flash.DrhogDtF[i+2][j]-flash.DrhogDtF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrhogDtF[i][j]=flash.DrhogDtF[i-1][j];
	 }
	 else if(var==6){
	 //flash.DrholDtF[i][j]= VarTemp[i][j];
	 del2e=flash.rhogF[i-1][j]-flash.rhogF[i-2][j];
	 del1e=flash.rhogF[i][j]-flash.rhogF[i-1][j];
	 del1d=flash.rhogF[i+1][j]-flash.rhogF[i][j];
	 del2d=flash.rhogF[i+2][j]-flash.rhogF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.rhogF[i][j]=flash.rhogF[i-1][j];
	 }
	 else if(var==7){
	 //flash.DrhowDtF[i][j]= VarTemp[i][j];
	 del2e=flash.DrhowDtF[i-1][j]-flash.DrhowDtF[i-2][j];
	 del1e=flash.DrhowDtF[i][j]-flash.DrhowDtF[i-1][j];
	 del1d=flash.DrhowDtF[i+1][j]-flash.DrhowDtF[i][j];
	 del2d=flash.DrhowDtF[i+2][j]-flash.DrhowDtF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrhowDtF[i][j]=flash.DrhowDtF[i-1][j];
	 }
	 else if(var==8){
	 //flash.RSF[i][j]= VarTemp[i][j];
	 del2e=flash.RSF[i-1][j]-flash.RSF[i-2][j];
	 del1e=flash.RSF[i][j]-flash.RSF[i-1][j];
	 del1d=flash.RSF[i+1][j]-flash.RSF[i][j];
	 del2d=flash.RSF[i+2][j]-flash.RSF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.RSF[i][j]=flash.RSF[i-1][j];
	 }


	 }
	 }*/

	string variavel;
	if (var == 1)
		variavel = "rhog";
	else if (var == 2)
		variavel = "rhol";
	else if (var == 3)
		variavel = "drhogdp";
	else if (var == 4)
		variavel = "drholdp";
	else if (var == 5)
		variavel = "dhhogdt";
	else if (var == 6)
		variavel = "drholdt";
	else if (var == 7)
		variavel = "drhowdt";
	else if (var == 8)
		variavel = "RS";
	else if (var == 9)
		variavel = "Cpg";
	else if (var == 10)
		variavel = "Cpl";
	else if (var == 11)
		variavel = "Cpw";
	else if (var == 12)
		variavel = "Hg";
	else if (var == 13)
		variavel = "Hl";
	else if (var == 14)
		variavel = "Sig-g-l";
	else if (var == 15)
		variavel = "Sig-g-w";
	else if (var == 16)
		variavel = "visc oleo";  //alteracao6
	else if (var == 17)
		variavel = "visc gas";  //alteracao6
	ostringstream saidaLatente;
	saidaLatente << pathPrefixoArqSaida << "perfiFlash-" << "-var-"
			<< variavel << ".dat";
	string tmp = saidaLatente.str();
	ofstream escreveMass(tmp.c_str(), ios_base::out);
	escreveMass << VarTemp;
	escreveMass.close();
	//delete [] tenta;
}


void dadosPoro::parse_fluido_producao(
		Value& fluidos_producao_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fluidosProducao");

	double api = 20;
	double rgo = 0;
	double deng = 1;
	double bsw = 0;
	double denag = 1;
	double templ = 10;
	double lvisl = 2;
	double temph = 20;
	double lvish = 1;
	int tipoemul = 0;
	double bswCorte = 1;
	double yco2 = 0;
	int corrC = 0;
	int corrSat = 0;  //alteracao4
	double aemul = 0.;  //alteracao4
	double bemul = 0.;  //alteracao4
	int vcorrOM = 0;
	int vcorrOV = 0;
	int vcorrOS = 0;
	int corrDeng=0;
	int fracMolarUsuario=0;
	double PHI100=0.765;
	int nvecBSW=0;

			tabVisc.parserie=0;
			tabVisc.temp=0;
			tabVisc.visc=0;
			if (flashCompleto == 0) {
				// loop para carga do vetor do JSON
					// de-para do elemento "fluidosProducao" do json para o struct
					api = fluidos_producao_json["api"].GetDouble();
					rgo = fluidos_producao_json["rgo"].GetDouble();
					deng =
							fluidos_producao_json["densidadeGas"].GetDouble();
					/*bsw = fluidos_producao_json["bsw"].GetDouble();
					if(bsw>1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"BSW > 1", chaveJson,
								"BSW>1");
					}*/
					denag = 1;
					if (fluidos_producao_json.HasMember("densidadeAgua"))
						denag =
								fluidos_producao_json["densidadeAgua"].GetDouble();
					if(denag<1.0)denag=1.0;

					tipoemul = 0;
					if (fluidos_producao_json.HasMember("tipoEmul")) {
						tipoemul =
								fluidos_producao_json["tipoEmul"].GetDouble();
						// caso modelo exponencial
						if (tipoemul == 4) {
							if (!fluidos_producao_json.HasMember("coefAModeloExp")
									|| !fluidos_producao_json.HasMember("coefBModeloExp")) {
								// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
										chaveJson, "tipoEmul=4");
							} else {
								aemul =
										fluidos_producao_json["coefAModeloExp"].GetDouble();
								bemul =
										fluidos_producao_json["coefBModeloExp"].GetDouble();
							}
						}
						else if(tipoemul == 5 && fluidos_producao_json.HasMember("PHI100"))
							PHI100=fluidos_producao_json["PHI100"].GetDouble();
						else if(tipoemul == 6 && nvecBSW==0){
							nvecBSW=1;
							if(!fluidos_producao_json.HasMember("BSWVec") ||
							   !fluidos_producao_json.HasMember("emulVec")){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' requeridas",
										chaveJson, "tipoEmul=6");
							}
							else{
								nvecEmul =fluidos_producao_json["BSWVec"].Size();
								if(nvecEmul!=fluidos_producao_json["emulVec"].Size()){
									logger.log(LOGGER_FALHA,
									LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
											"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
											chaveJson, "tipoEmul=6");
								}
								else{
									BSWVec = new double[nvecEmul];
									emulVec = new double[nvecEmul];
									for (int i = 0; i < nvecEmul; i++) {
										BSWVec[i] = fluidos_producao_json["BSWVec"][i].GetDouble();
										emulVec[i] = fluidos_producao_json["emulVec"][i].GetDouble();
									}
								}
							}
						}
					}
					bswCorte = 1.;
					if (fluidos_producao_json.HasMember("bswCorte"))
						bswCorte =
								fluidos_producao_json["bswCorte"].GetDouble();
					yco2 =
							fluidos_producao_json["fracCO2"].GetDouble();
					corrC =
							fluidos_producao_json["correlacaoCritica"].GetInt();

					if (fluidos_producao_json.HasMember("modeloRsPb")) {
						corrSat =
								fluidos_producao_json["modeloRsPb"].GetInt(); //alteracao4
						// caso modelo RsPb da Livia
						/*if (corrSat == 4) {
							// TODO: CORRIGIR ESSE TRECHO DE CÓDIGO NO FUTURO
							corrSat = -4;
							if (pvtsimarq.length() == 0) {
								// RN-042: Chave "pvtsimArq" requerida em caso de "latente" ou "modeloCp" ou "modeloJTL"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"arquivo pvtsim requerido", chaveJson,
										"modeloRsPb=4");
								// Caso o arquivo pvtsim inexista
							} else if (!ifstream(pvtsimarq)) {
								// criar variavel para o nome da propriedade json em processo de parse
								string chavePvtsimArq(chaveJson + "/pvtsimArq");
								// RN-043: Arquivo "pvtsimArq" inexistente
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Arquivo inexistente", chavePvtsimArq,
										pvtsimarq);
							}
						}*/
					}

					vcorrOM = 0;
					// caso modelo oleo morto preenchido
					if (fluidos_producao_json.HasMember("modeloOleoMorto")) {
						//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
						vcorrOM =
								fluidos_producao_json["modeloOleoMorto"].GetInt();
					}
					if (vcorrOM == 0) {
						if (!fluidos_producao_json.HasMember("temp1")
								|| !fluidos_producao_json.HasMember("visc1")
								|| !fluidos_producao_json.HasMember("temp2")
								|| !fluidos_producao_json.HasMember("visc2")) {
							// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
									chaveJson,
									"modeloOleoMorto = 0 ou ausente");
						} else {
							templ =
									fluidos_producao_json["temp1"].GetDouble();
							lvisl =
									fluidos_producao_json["visc1"].GetDouble();
							temph =
									fluidos_producao_json["temp2"].GetDouble();
							lvish =
									fluidos_producao_json["visc2"].GetDouble();
						}
					}
					else if (vcorrOM == 7) {
						if (!fluidos_producao_json.HasMember("tempOleoMorto")
								|| !fluidos_producao_json.HasMember("viscOleoMorto")) {
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'tempOleoMorto', 'viscOleoMorto' requeridas",
									chaveJson,
									"modeloOleoMorto = 7 ou ausente");
						} else {
							tabVisc.parserie=fluidos_producao_json["tempOleoMorto"].Size();
							tabVisc.temp=new double[tabVisc.parserie];
							tabVisc.visc=new double[tabVisc.parserie];
							for(int itab=0;itab<tabVisc.parserie;itab++){
								tabVisc.temp[itab]=fluidos_producao_json["tempOleoMorto"][itab].GetDouble();
								tabVisc.visc[itab]=fluidos_producao_json["viscOleoMorto"][itab].GetDouble();
							}
						}
					}

					// caso modelo oleo vivo preenchido
					if (fluidos_producao_json.HasMember("modeloOleoVivo"))
						//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
						vcorrOV =
								fluidos_producao_json["modeloOleoVivo"].GetInt();

					// caso modelo oleo subsaturado preenchido
					if (fluidos_producao_json.HasMember("modeloOleoSubSaturado"))
						//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
						vcorrOS =
								fluidos_producao_json["modeloOleoSubSaturado"].GetInt();

					if (fluidos_producao_json.HasMember("correcaoDenGas"))
						corrDeng =
								fluidos_producao_json["correcaoDenGas"].GetInt();

					flup = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul, PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, 0); //alteracao4
					flup.zdranP = zdranP;  //alteracao2
					flup.dzdpP = dzdpP;  //alteracao2
					flup.dzdtP = dzdtP;  //alteracao2
					flup.npontos = npontos;
					flup.viscBlackOil = 1;
					flup.modelaAgua = 1;
					flup.corrDeng=corrDeng;
					flup.nvecEmul=nvecEmul;
					flup.BSWVec=BSWVec;
					flup.emulVec=emulVec;
					flup.dStockTankVaporMassFraction=flup.FracMass(1.01, 20.);
					flup.parserie=tabVisc.parserie;
					flup.viscTab=tabVisc.visc;
					flup.tempTab=tabVisc.temp;
			} else if(flashCompleto==1){
				///////////////////////////////////////////////////////////

				if (fluidos_producao_json.HasMember("pvtsimArq"))
					pvtsimarq = pathArqExtEntrada
							+ string(fluidos_producao_json["pvtsimArq"].GetString());

				string dadosMRPre;
				dadosMRPre = pvtsimarq;
				ifstream lendoPVTSimPre(dadosMRPre.c_str(), ios_base::in);
				string chave;
				char* tenta;
				//tenta = new char[400];
				double testatok;
				char line[4000];
				lendoPVTSimPre.get(line, 4000);
				tenta = strtok(line, " ,()=");
				lendoPVTSimPre >> chave;
				while (chave != "BUBBLEPRESSURES") {
					lendoPVTSimPre >> chave;
				}

				lendoPVTSimPre.get(line, 4000);
				int ntab=0;
				tenta = strtok(line, " ,()=");
				while(tenta != nullptr &&  std::string(tenta)!="Pa"){
					ntab++;
					tenta = strtok(NULL, " ,)/");
				}
				lendoPVTSimPre.close();
				tabent.npont=ntab;
				//////////////////////////////////////////////////
					flash.arquivo = pvtsimarq;

					/*bsw = fluidos_producao_json["bsw"].GetDouble();
					if(bsw>1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"BSW > 1", chaveJson,
								"BSW>1");
					}*/
					denag = 1;
					if (fluidos_producao_json.HasMember("densidadeAgua"))
						denag =
								fluidos_producao_json["densidadeAgua"].GetDouble();
					if(denag<1.0)denag=1.0;

					int modelagua=1;
					if (fluidos_producao_json.HasMember("modeloAguaBlackOil"))  //alteracao6
						modelagua =
								fluidos_producao_json["modeloAguaBlackOil"].GetInt();

					flash.visc = 0;  // modelo viscosidade tabela flash
					if (fluidos_producao_json.HasMember("modeloViscBlackOil"))  //alteracao6
						flash.visc =
								fluidos_producao_json["modeloViscBlackOil"].GetInt(); //alteracao6
					if (flash.visc == 1) {  //modelo viscosidade black oil
						vcorrOM = 0;
						// caso modelo oleo morto preenchido
						if (fluidos_producao_json.HasMember("modeloOleoMorto")) {
							//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
							vcorrOM =
									fluidos_producao_json["modeloOleoMorto"].GetInt();
						}
						if (vcorrOM == 0) {
							if (!fluidos_producao_json.HasMember("temp1")
									|| !fluidos_producao_json.HasMember("visc1")
									|| !fluidos_producao_json.HasMember("temp2")
									|| !fluidos_producao_json.HasMember("visc2")) {
								// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
										chaveJson,
										"modeloOleoMorto = 0 ou ausente");
							} else {
								templ =
										fluidos_producao_json["temp1"].GetDouble();
								lvisl =
										fluidos_producao_json["visc1"].GetDouble();
								temph =
										fluidos_producao_json["temp2"].GetDouble();
								lvish =
										fluidos_producao_json["visc2"].GetDouble();
							}
						}

						if (fluidos_producao_json.HasMember("modeloOleoVivo"))
							//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
							vcorrOV =
									fluidos_producao_json["modeloOleoVivo"].GetInt();
						if (fluidos_producao_json.HasMember("modeloOleoSubSaturado"))
							//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
							vcorrOS =
									fluidos_producao_json["modeloOleoSubSaturado"].GetInt();
					} else {
						vcorrOM = 4;
						vcorrOV = 0;
						vcorrOS = 0;
					}

					tipoemul = 0;
					if (fluidos_producao_json.HasMember("tipoEmul"))
						tipoemul =
								fluidos_producao_json["tipoEmul"].GetInt();
					bswCorte = 1.;
					if (fluidos_producao_json.HasMember("bswCorte"))
						bswCorte =
								fluidos_producao_json["bswCorte"].GetDouble();

					if (tipoemul == 4) {
						if (!fluidos_producao_json.HasMember("coefAModeloExp")
								|| !fluidos_producao_json.HasMember("coefBModeloExp")) {
							// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
									chaveJson, "tipoEmul=4");
						} else {
							aemul =
									fluidos_producao_json["coefAModeloExp"].GetDouble();
							bemul =
									fluidos_producao_json["coefBModeloExp"].GetDouble();
						}
					}
					else if(tipoemul == 5 && fluidos_producao_json.HasMember("PHI100"))
						PHI100=fluidos_producao_json["PHI100"].GetDouble();
					else if(tipoemul == 6 && nvecBSW==0){
						nvecBSW=1;
						if(!fluidos_producao_json.HasMember("BSWVec") ||
						   !fluidos_producao_json.HasMember("emulVec")){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'BSWVec', 'emulVec' requeridas",
									chaveJson, "tipoEmul=6");
						}
						else{
							nvecEmul =fluidos_producao_json["BSWVec"].Size();
							if(nvecEmul!=fluidos_producao_json["emulVec"].Size()){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
										chaveJson, "tipoEmul=6");
							}
							else{
								BSWVec = new double[nvecEmul];
								emulVec = new double[nvecEmul];
								for (int i = 0; i < nvecEmul; i++) {
									BSWVec[i] = fluidos_producao_json["BSWVec"][i].GetDouble();
									emulVec[i] = fluidos_producao_json["emulVec"][i].GetDouble();
								}
							}
						}
					}

					flash.id =0;
					flash.rholF = new double*[tabent.npont + 1];
					flash.rhogF = new double*[tabent.npont + 1];
					flash.DrholDpF = new double*[tabent.npont + 1];
					flash.DrhogDpF = new double*[tabent.npont + 1];
					flash.DrholDtF = new double*[tabent.npont + 1];
					flash.DrhogDtF = new double*[tabent.npont + 1];
					flash.DrhowDtF = new double*[tabent.npont + 1];
					flash.RSF = new double*[tabent.npont + 1];
					flash.cplF = new double*[tabent.npont + 1];
					flash.cpgF = new double*[tabent.npont + 1];
					flash.cpwF = new double*[tabent.npont + 1];
					flash.HlF = new double*[tabent.npont + 1];
					flash.HgF = new double*[tabent.npont + 1];
					flash.sigOGF = new double*[tabent.npont + 1];
					flash.sigWGF = new double*[tabent.npont + 1];
					flash.viscO = new double*[tabent.npont + 1]; //alteracao6
					flash.viscG = new double*[tabent.npont + 1]; //alteracao6
					flash.PBF = new double[tabent.npont];
					flash.TBF = new double[tabent.npont];

					for (int k = 0; k < tabent.npont + 1; k++) {
						flash.rholF[k] = new double[tabent.npont + 1];
						flash.rhogF[k] = new double[tabent.npont + 1];
						flash.DrholDpF[k] =
								new double[tabent.npont + 1];
						flash.DrhogDpF[k] =
								new double[tabent.npont + 1];
						flash.DrholDtF[k] =
								new double[tabent.npont + 1];
						flash.DrhogDtF[k] =
								new double[tabent.npont + 1];
						flash.DrhowDtF[k] =
								new double[tabent.npont + 1];
						flash.RSF[k] = new double[tabent.npont + 1];
						flash.cplF[k] = new double[tabent.npont + 1];
						flash.cpgF[k] = new double[tabent.npont + 1];
						flash.cpwF[k] = new double[tabent.npont + 1];
						flash.HlF[k] = new double[tabent.npont + 1];
						flash.HgF[k] = new double[tabent.npont + 1];
						flash.sigOGF[k] = new double[tabent.npont + 1];
						flash.sigWGF[k] = new double[tabent.npont + 1];
						flash.viscO[k] = new double[tabent.npont + 1]; //alteracao6
						flash.viscG[k] = new double[tabent.npont + 1]; //alteracao6
					}

					for (int k = 0; k < tabent.npont + 1; k++) {
						for (int j = 0; j < tabent.npont + 1; j++) {
							flash.rholF[k][j] = 0;
							flash.rhogF[k][j] = 0;
							flash.DrholDpF[k][j] = 0;
							flash.DrhogDpF[k][j] = 0;
							flash.DrholDtF[k][j] = 0;
							flash.DrhogDtF[k][j] = 0;
							flash.DrhowDtF[k][j] = 0;
							flash.RSF[k][j] = 0;
							flash.cplF[k][j] = 0;
							flash.cpgF[k][j] = 0;
							flash.cpwF[k][j] = 0;
							flash.HlF[k][j] = 0;
							flash.HgF[k][j] = 0;
							flash.sigOGF[k][j] = 0;
							flash.sigWGF[k][j] = 0;
							flash.viscO[k][j] = 0;  //alteracao6
							flash.viscG[k][j] = 0;  //alteracao6
						}
					}
					for (int k = 0; k < tabent.npont; k++) {
						flash.PBF[k] = 0;
						flash.TBF[k] = 0;
					}

					string dadosMR;
					dadosMR= pvtsimarq;
					ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
					//string chave;
					//char* tenta;
					//tenta = new char[400];
					//double testatok;
					//char line[4000];
					lendoPVTSim.get(line, 4000);
					tenta = strtok(line, " ,()=");
					while (tenta != nullptr && strcmp(tenta, "PHASE") != 0) {
						tenta = strtok(NULL, " ,()=");
					}
					tenta = strtok(NULL, " ,()=");
					int threeOrtwo = 0;
					flash.TwoOrThree = 0;
					if (strcmp(tenta, "THREE") == 0) {
						threeOrtwo = 1;
						flash.TwoOrThree = 1;
					}

					lendoPVTSim >> chave;
					while (chave != "GOR")
						lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					rgo = stof(chave);

					/*if (threeOrtwo == 1) {
						while (chave != "WC")
							lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						bsw = stof(chave);
					} else
						bsw = 0.;*/

					while (chave != "STDGASDENSITY")
						lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					deng = stof(chave);
					deng /= 1.225;

					while (chave != "STDOILDENSITY")
						lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					api = stof(chave);
					api /= 1000.;
					api = 141.5 / api - 131.5;

					/*if (threeOrtwo == 1) {
						while (chave != "STDWATDENSITY")
							lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						denag = stof(chave);
						denag /= 1000.;
						if (denag == 0)
							denag = 1.;
					} else
						denag = 1.;*/

					//double rgoSpare = fluidos_producao_json[indAtivo].rgo();
					//if(rgoSpare>rgo) rgo=rgoSpare;

					flup = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul,PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, 0);
					flup.indiceFlash = 0;
					//flup.viscBlackOil = 1 - flash.visc;
					flup.viscBlackOil = flash.visc;
					flup.modelaAgua=modelagua;

					int ndiv = tabent.npont - 1;
					flup.npontos=tabent.npont;
					lendoPVTSim >> chave;
					int lacoleitura = ndiv;
					double valor;

					while (chave != "BUBBLEPRESSURES") {
						lendoPVTSim >> chave;
					}
					lendoPVTSim.get(line, 4000);
					tenta = strtok(line, " ,()=");
					flash.PBF[0] = atof(tenta) * 0.00014503773800722;
					for (int kontaPVT = 1; kontaPVT <= lacoleitura;
							kontaPVT++) {
						tenta = strtok(NULL, " ,");
						testatok = atof(tenta);
						flash.PBF[kontaPVT] = testatok * 0.00014503773800722;
					}
					while (chave != "BUBBLETEMPERATURES") {
						lendoPVTSim >> chave;
					}
					lendoPVTSim.get(line, 4000);
					tenta = strtok(line, " ,()=");
					flash.TBF[0] = atof(tenta);
					for (int kontaPVT = 1; kontaPVT <= lacoleitura;
							kontaPVT++) {
						tenta = strtok(NULL, " ,");
						testatok = atof(tenta);
						flash.TBF[kontaPVT] = testatok;
					}

					FullMtx<double> BolhaTemp(ndiv + 2, 2);
					for (int k = 0; k <= ndiv; k++) {
						BolhaTemp[k][0] = flash.PBF[k];
						BolhaTemp[k][1] = flash.TBF[k];
					}
					ostringstream saidaBolha;
					saidaBolha << pathPrefixoArqSaida << "perfilBolha.dat";
					string tmp = saidaBolha.str();
					ofstream escreveMass(tmp.c_str(), ios_base::out);
					escreveMass << BolhaTemp;
					escreveMass.close();
					lendoPVTSim.close();

					geraTabFlash(1);
					geraTabFlash(2);
					geraTabFlash(3);
					geraTabFlash(4);
					geraTabFlash(5);
					geraTabFlash(6);
					geraTabFlash(8);
					geraTabFlash(9);
					geraTabFlash(10);
					geraTabFlash(12);
					geraTabFlash(13);
					geraTabFlash(14);
					geraTabFlash(16);  //alteracao6
					geraTabFlash(17);  //alteracao6
					if (threeOrtwo == 1) {
						geraTabFlash(7);
						geraTabFlash(11);
						geraTabFlash(15);
					}
					flup.rhogF = flash.rhogF;

					int erroP=0;
					int erroT=0;
					vector<double> dP;
					vector<double> pchange;
					vector<int> indp;
					int ndp=0;
					vector<double> dT;
					vector<double> tchange;
					vector<int> indt;
					int ndt=0;
					double dp0=0.;
					double dp1;
					double dt0=0.;
					double dt1;

					int imin=1;
					while(flup.rhogF[imin][0]<=1.0332274497825)imin++;
					flup.indMinEquP=imin+1;
					int iant=imin+1;
					flup.pMinEqu=flup.rhogF[imin+1][0];
					flup.delpTab=flup.rhogF[imin+2][0]-flup.rhogF[imin+1][0];
					dp0=(flup.rhogF[ndiv+1][0]-flup.rhogF[imin+1][0])/(ndiv-imin);
					for(int j=imin+1;j<ndiv;j++){
						dp0=flup.rhogF[j+1][0]-flup.rhogF[j][0];
						dp1=flup.rhogF[j+2][0]-flup.rhogF[j+1][0];
						if(fabsl((dp0-dp1)/dp0)>1e-3){
							ndp++;
							pchange.push_back(flup.rhogF[j+1][0]);
							indp.push_back(j+1);
							dP.push_back((flup.rhogF[j+1][0]-flup.rhogF[iant][0])/(j+1-iant));
							iant=j+1;
							if(ndp>3)erroP=1;
						}
						else if(j==ndiv-1 && ndp>0){
							ndp++;
							pchange.push_back(flup.rhogF[ndiv+1][0]);
							indp.push_back(ndiv+1);
							dP.push_back((flup.rhogF[ndiv+1][0]-flup.rhogF[iant][0])/(ndiv+1-iant));
							if(ndp>3)erroP=1;

						}
					}
                    if(ndp==0){
                    	ndp++;
                    	pchange.push_back(flup.rhogF[ndiv+1][0]);
                    	indp.push_back(ndiv+1);
                    	dP.push_back((flup.rhogF[ndiv+1][0]-flup.rhogF[imin+1][0])/(ndiv-imin));
                    }
                    if(erroP==0){
                      flup.pchange=pchange;
                      flup.dpchange=dP;
                      flup.ipchange=indp;
                    }
					flup.delpTab=ndp;
					flup.erroP=erroP;

					imin=1;
					while(flup.rhogF[0][imin]<=16)imin++;
					flup.indMinEquT=imin+1;
					iant=imin+1;
					flup.tMinEqu=flup.rhogF[0][imin+1];
					flup.deltTab=flup.rhogF[0][imin+2]-flup.rhogF[0][imin+1];
					dt0=(flup.rhogF[0][ndiv+1]-flup.rhogF[0][imin+1])/(ndiv-imin);
					for(int j=imin+1;j<ndiv;j++){
						dt0=flup.rhogF[0][j+1]-flup.rhogF[0][j];
						dt1=flup.rhogF[0][j+2]-flup.rhogF[0][j+1];
						if(fabsl((dt0-dt1)/dt0)>1e-3){
							ndt++;
							tchange.push_back(flup.rhogF[0][j+1]);
							indt.push_back(j+1);
							dT.push_back((flup.rhogF[0][j+1]-flup.rhogF[0][iant])/(j+1-iant));
							iant=j+1;
							if(ndt>3)erroT=1;
						}
						else if(j==ndiv-1 && ndt>0){
							ndt++;
							tchange.push_back(flup.rhogF[0][ndiv+1]);
							indt.push_back(ndiv+1);
							dT.push_back((flup.rhogF[0][ndiv+1]-flup.rhogF[0][iant])/(ndiv+1-iant));
							if(ndt>3)erroT=1;

						}
					}
                    if(ndt==0){
                    	ndt++;
                    	tchange.push_back(flup.rhogF[0][ndiv+1]);
                    	indt.push_back(ndiv+1);
                    	dT.push_back((flup.rhogF[0][ndiv+1]-flup.rhogF[0][imin+1])/(ndiv-imin));
                    }
                    if(erroT==0){
                      flup.tchange=tchange;
                      flup.dtchange=dT;
                      flup.itchange=indt;
                    }
					flup.deltTab=ndt;
					flup.erroT=erroT;

					flup.rholF = flash.rholF;
					flup.DrhogDpF = flash.DrhogDpF;
					flup.DrholDpF = flash.DrholDpF;
					flup.DrhogDtF = flash.DrhogDtF;
					flup.DrholDtF = flash.DrholDtF;
					flup.DrhowDtF = flash.DrhowDtF;
                    flup.RSF = flash.RSF;
					flup.cpgF = flash.cpgF;
					flup.cplF = flash.cplF;
					flup.cpwF = flash.cpwF;
					flup.HgF = flash.HgF;
					flup.HlF = flash.HlF;
					flup.sigOGF = flash.sigOGF;
					flup.sigWGF = flash.sigWGF;
					flup.viscO = flash.viscO;
					flup.viscG = flash.viscG;
					flup.PBPVTSim = flash.PBF;
					flup.TBPVTSim = flash.TBF;

					/*for(int j=1;j>ndiv;j++){
						for(int k=2;k<ndiv+2;k++){
							if(flup.RSF[j][k]<0.){
								//flup.rholF[j][k]=1000*141.5/(131.5+api);
								flup.rholF[j][k]=flup.rholF[j][k-1];
								flup.DrholDtF[j][k]=0.;
								flup.DrholDpF[j][k]=0.;
							}
						}
					}*/

					flup.nvecEmul=nvecEmul;
					flup.BSWVec=BSWVec;
					flup.emulVec=emulVec;
					flup.parserie=tabVisc.parserie;
					flup.viscTab=tabVisc.visc;
					flup.tempTab=tabVisc.temp;

					rgo = fluidos_producao_json["rgo"].GetDouble();
					if(rgo>flup.RGO){
						flup.RGO=rgo;
						flup.IRGO=flup.RGO*35.31467/6.29;
					}

					if(flash.rhogF[0][1]<20 && flash.rhogF[1][0]<1.01)
						//flup.dStockTankVaporDensity=flup.FracMass(1.01, 20.);
						flup.dStockTankVaporMassFraction=flup.FracMass(1.01, 20.);
					else
						//flup.dStockTankVaporDensity=0.5;
						flup.dStockTankVaporMassFraction=0.5;
					//delete [] tenta;
			}
			else{

				/*api = fluidos_producao_json[indAtivo].api();
				rgo = fluidos_producao_json[indAtivo].rgo();
				deng =
						fluidos_producao_json[indAtivo].densidadeGas();
				bsw = fluidos_producao_json[indAtivo].bsw();
				if(bsw>1){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"BSW > 1", chaveJson,
							"BSW>1");
				}
				denag = 1;
				if (fluidos_producao_json[indAtivo].densidadeAgua().exists())
					denag =
							fluidos_producao_json[indAtivo].densidadeAgua();
				if(denag<1.0)denag=1.0;*/

				string dadosMRPre;
				dadosMRPre = pvtsimarq;
				int npseudo=0;
				int erro=0;
                GetMixtureComponentNumberFromExternalFile(dadosMRPre.c_str(),dadosMRPre.size(),0,
                		    &npseudo,&erro);
                if(erro!=0)logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Leitura CTM", chaveJson, "Erro na leitura do CTM");
                double fractemp[npseudo];

                	compDet.fracMol = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.fracMol[inpseudo]=0.;
                	compDet.TIndepPeneloux = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.TIndepPeneloux[inpseudo]=0.;
                	compDet.fatAcent = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.fatAcent[inpseudo]=0.;
                	compDet.masMol = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.masMol[inpseudo]=0.;
                	compDet.parac = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.parac[inpseudo]=0.;
                	compDet.presCrit = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.presCrit[inpseudo]=0.;
                	compDet.tempCrit = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.tempCrit[inpseudo]=0.;
                	compDet.CpIGCoefs = new double [4*npseudo];
                	for(int inpseudo=0;inpseudo<4*npseudo;inpseudo++)compDet.CpIGCoefs[inpseudo]=0.;
                	compDet.lij = new double [npseudo*npseudo];
                	for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet.lij[inpseudo]=0.;
                	compDet.kij = new double [npseudo*npseudo];
                	for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet.kij[inpseudo]=0.;

                	/*compDet.fracMol = new double [npseudo];
                	compDet.TIndepPeneloux = new double [npseudo];
                	compDet.fatAcent = new double [npseudo];
                	compDet.masMol = new double [npseudo];
                	compDet.parac = new double [npseudo];
                	compDet.presCrit = new double [npseudo];
                	compDet.tempCrit = new double [npseudo];
                	compDet.CpIGCoefs = new double [4*npseudo];
                	compDet.lij = new double [npseudo*npseudo];
                	compDet.kij = new double [npseudo*npseudo];*/

                	InputForViscosity modeloVisc;

                		ReadMixtureComponentsFromExternalFile(dadosMRPre.c_str(), dadosMRPre.size(), 0,
                				&npseudo, &fractemp[0], compDet.masMol,
								compDet.tempCrit, compDet.presCrit, compDet.fatAcent,
								compDet.parac,compDet.kij,compDet.lij,compDet.TIndepPeneloux,
								&compDet.liqModel, compDet.CpIGCoefs,&modeloVisc, &erro);
            		/*ReadMixtureComponentsFromExternalFile(dadosMRPre.c_str(), dadosMRPre.size(), 0,
            				&npseudo, &fractemp[0], compDet.masMol,
							compDet.tempCrit, compDet.presCrit, compDet.fatAcent,
							compDet.parac,compDet.kij,compDet.lij,compDet.TIndepPeneloux,
							&compDet.liqModel, compDet.CpIGCoefs,&modeloVisc, &erro);*/
              		int fracusu=0;
    				if (fluidos_producao_json.HasMember("fracMolarUsuario"))
    						fracusu =fluidos_producao_json["fracMolarUsuario"].GetDouble();
    				if(fracusu==1){
    					int nusu=fluidos_producao_json["fracMolar"].Size();
    					if(nusu!=npseudo)logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
    								"Dimensao da composicao diferente da dimensao da composicao dada no CTM",
									chaveJson, "Erro no preenchimento da Fracao Molar");
    					for(int j=0;j<npseudo;j++)
    							compDet.fracMol[j]=fluidos_producao_json["fracMolar"][j].GetDouble()/100.;
    					/*for(int j=0;j<npseudo;j++)
    					    	compDet.fracMol[j]=fluidos_producao_json[indAtivo].fracMolar()[j];*/

    			    }
    				else /*memcpy(compDet.fracMol,&fractemp[0],npseudo*sizeof(double));*/
    					memcpy(compDet.fracMol,&fractemp[0],npseudo*sizeof(double));
    				double totFrac=0.;
    				for(int j=0;j<npseudo;j++)totFrac+=compDet.fracMol[j];
    				if(totFrac>1e-15)for(int j=0;j<npseudo;j++)compDet.fracMol[j]/=totFrac;
    				/*bsw = fluidos_producao_json["bsw"].GetDouble();
    				if(bsw>1){
    							logger.log(LOGGER_FALHA,
    							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
    							"BSW > 1", chaveJson,
    							"BSW>1");
    					   	}*/
    				denag = 1;
    				if (fluidos_producao_json.HasMember("densidadeAgua"))
    							denag =	fluidos_producao_json["densidadeAgua"].GetDouble();
    				if(denag<1.0)denag=1.0;

					yco2 =
							fluidos_producao_json["fracCO2"].GetDouble();
					corrC =
							fluidos_producao_json["correlacaoCritica"].GetInt();


					int modelovisc = 1;  // modelo viscosidade tabela flash
					if (fluidos_producao_json.HasMember("modeloViscBlackOil"))  //alteracao6
						modelovisc =
								fluidos_producao_json["modeloViscBlackOil"].GetInt(); //alteracao6
					if (modelovisc == 1) {  //modelo viscosidade black oil
						vcorrOM = 0;
						// caso modelo oleo morto preenchido
						if (fluidos_producao_json.HasMember("modeloOleoMorto")) {
							//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
							vcorrOM =
									fluidos_producao_json["modeloOleoMorto"].GetInt();
						}
						if (vcorrOM == 0) {
							if (!fluidos_producao_json.HasMember("temp1")
									|| !fluidos_producao_json.HasMember("visc1")
									|| !fluidos_producao_json.HasMember("temp2")
									|| !fluidos_producao_json.HasMember("visc2")) {
								// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
										chaveJson,
										"modeloOleoMorto = 0 ou ausente");
							} else {
								templ =
										fluidos_producao_json["temp1"].GetDouble();
								lvisl =
										fluidos_producao_json["visc1"].GetDouble();
								temph =
										fluidos_producao_json["temp2"].GetDouble();
								lvish =
										fluidos_producao_json["visc2"].GetDouble();
							}
						}

						if (fluidos_producao_json.HasMember("modeloOleoVivo"))
							//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
							vcorrOV =
									fluidos_producao_json["modeloOleoVivo"].GetInt();
						if (fluidos_producao_json.HasMember("modeloOleoSubSaturado"))
							//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
							vcorrOS =
									fluidos_producao_json["modeloOleoSubSaturado"].GetInt();
					} else {
						vcorrOM = 4;
						vcorrOV = 0;
						vcorrOS = 0;
					}

					tipoemul = 0;
					if (fluidos_producao_json.HasMember("tipoEmul"))
						tipoemul =
								fluidos_producao_json["tipoEmul"].GetInt();
					bswCorte = 1.;
					if (fluidos_producao_json.HasMember("bswCorte"))
						bswCorte =
								fluidos_producao_json["bswCorte"].GetDouble();

					if (tipoemul == 4) {
						if (!fluidos_producao_json.HasMember("coefAModeloExp")
								|| !fluidos_producao_json.HasMember("coefBModeloExp")) {
							// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
									chaveJson, "tipoEmul=4");
						} else {
							aemul =
									fluidos_producao_json["coefAModeloExp"].GetDouble();
							bemul =
									fluidos_producao_json["coefBModeloExp"].GetDouble();
						}
					}
					else if(tipoemul == 5 && fluidos_producao_json.HasMember("PHI100"))
						PHI100=fluidos_producao_json["PHI100"].GetDouble();
					else if(tipoemul == 6 && nvecBSW==0){
						nvecBSW=1;
						if(!fluidos_producao_json.HasMember("BSWVec") ||
						   !fluidos_producao_json.HasMember("emulVec")){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'BSWVec', 'emulVec' requeridas",
									chaveJson, "tipoEmul=6");
						}
						else{
							nvecEmul =fluidos_producao_json["BSWVec"].Size();
							if(nvecEmul!=fluidos_producao_json["emulVec"].Size()){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
										chaveJson, "tipoEmul=6");
							}
							else{
								BSWVec = new double[nvecEmul];
								emulVec = new double[nvecEmul];
								for (int i = 0; i < nvecEmul; i++) {
									BSWVec[i] = fluidos_producao_json["BSWVec"][i].GetDouble();
									emulVec[i] = fluidos_producao_json["emulVec"][i].GetDouble();
								}
							}
						}
					}


					/*vcorrOM = 0;
					// caso modelo oleo morto preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoMorto().exists()) {
						//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
						vcorrOM =
								fluidos_producao_json[indAtivo].modeloOleoMorto();
					}
					if (vcorrOM == 0) {
						if (!fluidos_producao_json[indAtivo].temp1().exists()
								|| !fluidos_producao_json[indAtivo].visc1().exists()
								|| !fluidos_producao_json[indAtivo].temp2().exists()
								|| !fluidos_producao_json[indAtivo].visc2().exists()) {
							// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
									chaveJson,
									"modeloOleoMorto = 0 ou ausente");
						} else {
							templ =
									fluidos_producao_json[indAtivo].temp1();
							lvisl =
									fluidos_producao_json[indAtivo].visc1();
							temph =
									fluidos_producao_json[indAtivo].temp2();
							lvish =
									fluidos_producao_json[indAtivo].visc2();
						}
					}

					// caso modelo oleo vivo preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoVivo().exists())
						//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
						vcorrOV =
								fluidos_producao_json[indAtivo].modeloOleoVivo();

					// caso modelo oleo subsaturado preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoSubSaturado().exists())
						//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
						vcorrOS =
								fluidos_producao_json[indAtivo].modeloOleoSubSaturado();*/

					flup = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul, PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, 0,npseudo);
					flup.viscBlackOil = modelovisc;
					flup.modelaAgua=1;
					tabp=0;
					flup.tabelaDinamica=tabelaDinamica;

					flup.TIndepPeneloux=compDet.TIndepPeneloux;
					flup.fatAcent=compDet.fatAcent;
					flup.masMol=compDet.masMol;
					flup.parac=compDet.parac;
					flup.presCrit=compDet.presCrit;
					flup.tempCrit=compDet.tempCrit;
					flup.CpIGCoefs=compDet.CpIGCoefs;
					flup.lij=compDet.lij;
					flup.kij=compDet.kij;
					flup.liqModel=compDet.liqModel;
					memcpy(flup.fracMol,compDet.fracMol, npseudo*sizeof(double));
					flup.Pmol=0.;
					for(int j=0;j<flup.npseudo;j++)flup.Pmol+=flup.fracMol[j]*flup.masMol[j];
					flup.atualizaPropCompStandard();

					flup.parserie=tabVisc.parserie;
					flup.viscTab=tabVisc.visc;
					flup.tempTab=tabVisc.temp;
					//flup.atualizaPropComp(90,10);
					/*FullMtx<double> fatores(3,flup.npseudo);
					for(int ipseu=0;ipseu<flup.npseudo;ipseu++){
						fatores[0][ipseu]=flup.fatAcent[ipseu];
						fatores[1][ipseu]=flup.masMol[ipseu];
						fatores[2][ipseu]=flup.masMol[ipseu];
					}
					cout<<fatores;
					cout<<dadosMRPre<<" "<<flup.iIER<<"   "<<flup.dStockTankVaporDensity<<"  "<<flup.dStockTankVaporMassFraction<<"\n";*/
					//flup.atualizaPropComp(50,50);
					/*flup.TIndepPeneloux=compDet.TIndepPeneloux;
					flup.fatAcent=compDet.fatAcent;
					flup.masMol=compDet.masMol;
					flup.parac=compDet.parac;
					flup.presCrit=compDet.presCrit;
					flup.tempCrit=compDet.tempCrit;
					flup.CpIGCoefs=compDet.CpIGCoefs;
					flup.lij=compDet.lij;
					flup.kij=compDet.kij;
					flup.liqModel=compDet.liqModel;
					memcpy(flup.fracMol,compDet.fracMol, npseudo*sizeof(double));*/
					//flup.atualizaPropCompStandard();
					//flup.atualizaPropComp(50,50);
					//double teste=flup.DZDT(50, 50);
					//teste=flup.BOFunc(50, 50);

			}
}

void dadosPoro::parse_PW(Value& pW_json) {
	string chaveJson("#/pW");


	pW.nserie=1;
	//pW.rotulo=;
	pW.tempo=0;
	pW.val=0;
	pW.valSat=0;

	if (pW_json.HasMember("tempo"))
		pW.nserie =
				pW_json["tempo"].Size();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempo' inexistente",chaveJson, "pW");

	pW.tempo=new double [pW.nserie];
	pW.val=new double [pW.nserie];
	pW.valSat=new double [pW.nserie];
	for(int i=0; i<pW.nserie; i++){
		pW.tempo[i]=pW_json["tempo"][i].GetDouble();
		pW.val[i]=pW_json["valor"][i].GetDouble();
	}
}

void dadosPoro::indrazT(int& ind, double& raz) {
	for (int i = 0; i <= temp.parserie - 1; i++) {
		if (i < temp.parserie - 1) {
			if (tempo >= temp.tempo[i] && tempo < temp.tempo[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - temp.tempo[i])
								/ (temp.tempo[i + 1] - temp.tempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == temp.parserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void dadosPoro::indrazPW(int& ind, double& raz) {
	for (int i = 0; i <= pW.nserie - 1; i++) {
		if (i < pW.nserie - 1) {
			if (tempo >= pW.tempo[i] && tempo < pW.tempo[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - pW.tempo[i])
								/ (pW.tempo[i + 1] - pW.tempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == pW.nserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void dadosPoro::indraz(int& ind, double& raz, double* serieTempo, int nserie) {
	for (int i = 0; i <= nserie - 1; i++) {
		if (i < nserie - 1) {
			if (tempo >= serieTempo[i] && tempo < serieTempo[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - serieTempo[i])
								/ (serieTempo[i + 1] - serieTempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == nserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void dadosPoro::parse_fluido_complementar(Value& fluido_json) {
	// caso a propriedade "ativo" esteja habilitada
		// de-para do elementoPoroso "fluido_complementar" do json para o struct fluc
	int tipoflui=0;
	if (fluido_json.HasMember("tipoF"))
			tipoflui=fluido_json["tipoF"].GetInt();
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
	if (fluido_json.HasMember("salinidade"))
		sal= fluido_json["salinidade"].GetDouble();
	if(tipoflui!=1){
		masesp =fluido_json["massaEspecifica"].GetDouble();
			//compP = fluido_json["compP"].GetDouble();
		compT = fluido_json["compT"].GetDouble();
			//tensup = fluido_json["tensup"].GetDouble();
		calesp = fluido_json["calorEspecifico"].GetDouble();
		condut = fluido_json["condutividade"].GetDouble();
		templ = fluido_json["temp1"].GetDouble();
		lvisl = fluido_json["visc1"].GetDouble();
		temph = fluido_json["temp2"].GetDouble();
		lvish = fluido_json["visc2"].GetDouble();
	}

	fluc = ProFluCol(masesp, compP, compT, tensup, calesp,
				condut, sal, templ, lvisl, temph, lvish,0, tipoflui);
	fluc.npontos = 0;
}

void dadosPoro::parse_tempo(Value& tempo_json) {
	string chaveJson("#/tempo");
/*
 * 	int perm;
	int trans;
	double relax;
	int parserie;
	double* dtmax;
	double* tempo;
 */

	temp.perm=1;
	temp.trans=0;
	temp.relax=1.;
	temp.parserie=0;
	temp.dtmax=0;
	temp.tempo=0;
	temp.tmax=0.;
	temp.tempoImp=0;
	temp.parserieImp=0;
	centroX=-1000.;
	centroY=-1000.;
	zDatumRef=0.;
	flashCompleto =0;
	tabp=0;
	miniTabAtraso=10;
	miniTabDp=10.;
	miniTabDt=10.;
	satAconat=0.;
	ippoco=1.;

	tipoModelOleo=1;
	compOleo=1e-5;
	rhoRef=900;
	BSW=0.;

	if (tempo_json.HasMember("threadLocal"))
		nthrdLocal = tempo_json["threadLocal"].GetInt();

	if (tempo_json.HasMember("modeloOleo"))
		tipoModelOleo =
				tempo_json["modeloOleo"].GetInt();

	if (tempo_json.HasMember("acop"))
		acop = tempo_json["acop"].GetInt();

	if (tempo_json.HasMember("relax"))
		temp.relax = tempo_json["relax"].GetDouble();

	if(acop==1){
		//pInt = tempo_json["presInterna"].GetDouble();
		angAcop= tempo_json["anguloAcoplamento"].GetDouble();
		centroX= tempo_json["centroX"].GetDouble();
		centroY= tempo_json["centroY"].GetDouble();
	}

	if (tempo_json.HasMember("modeloFluidoTabelaFlash"))
		flashCompleto =
				tempo_json["modeloFluidoTabelaFlash"].GetInt();
	// caso simulacao com tabela flash - modeloFluidoTabelaFlash: true
	if (flashCompleto == 0) {
		int verifComp=0;
		if (tempo_json.HasMember("modeloFluidoComposicional"))
			verifComp=tempo_json["modeloFluidoComposicional"].GetInt();
		flashCompleto+=2*verifComp;
	}
	if(flashCompleto == 0){
		if (tempo_json.HasMember("tabP"))
			tabp = tempo_json["tabP"].GetInt();
		if (tempo_json.HasMember("correcaoDenGasLivreBlackOil"))
			corrDeng = tempo_json["correcaoDenGasLivreBlackOil"].GetInt();
	}
	if(flashCompleto==2){
	    if (tempo_json.HasMember("miniTabDinAtraso"))
	    	miniTabAtraso=tempo_json["miniTabDinAtraso"].GetInt();
	    if (tempo_json.HasMember("miniTabDinDp"))
	    	miniTabDp=tempo_json["miniTabDinDp"].GetInt();
	    if (tempo_json.HasMember("miniTabDinDt"))
	    	miniTabDt=tempo_json["miniTabDinDt"].GetInt();
	}
	if(flashCompleto==1 || flashCompleto==2){
		if (tempo_json.HasMember("pvtsimArq"))
			pvtsimarq = pathArqExtEntrada
					+ string(tempo_json["pvtsimArq"].GetString());
		else{
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'pvtsimArq' inexistente",
					chaveJson, "Condicoes de tempo");
		}
	}

	satAconat = tempo_json["aguaConata"].GetDouble();
	if(acop==1)ippoco = tempo_json["ipLocal"].GetDouble();
	tRes = tempo_json["temperaturaReservatorio"].GetDouble();
	espessuraRes = tempo_json["espessuraReservatorio"].GetDouble();

	zDatumRef = tempo_json["datumReferencia"].GetDouble();

	pRes = tempo_json["pressaoReferencia"].GetDouble();

	if (tempo_json.HasMember("tmax"))
		temp.tmax =
				tempo_json["tmax"].GetDouble();
	if (tempo_json.HasMember("perm"))
		temp.perm =
				tempo_json["perm"].GetBool();
	if (tempo_json.HasMember("trans"))
		temp.trans =
				tempo_json["trans"].GetBool();
	if (tempo_json.HasMember("relax"))
		temp.relax =
				tempo_json["relax"].GetDouble();

	if (tempo_json.HasMember("dtMax"))
		temp.parserie =
				tempo_json["dtMax"].Size();
	else if(temp.trans==1)logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'dtMax' inexistente",chaveJson, "condicao de tempo");
	if(tempo_json["dtMax"].Size()!=tempo_json["tempo"].Size()){
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Chaves 'dtMax', 'tempo' com dimensoes diferentes",
				chaveJson, "Condicoes de tempo");
	}
	temp.dtmax=new double [temp.parserie];
	temp.tempo=new double [temp.parserie];
	for(int i=0; i<temp.parserie; i++){
		temp.dtmax[i]=tempo_json["dtMax"][i].GetDouble();
		temp.tempo[i]=tempo_json["tempo"][i].GetDouble();
	}


	if (tempo_json.HasMember("tempoImp"))
		temp.parserieImp =
				tempo_json["tempoImp"].Size();
	else if(temp.trans==1)logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempoImp' inexistente para impressão do processo transiente",chaveJson, "condicao de tempo");
	temp.tempoImp=new double [temp.parserieImp];
	for(int i=0; i<temp.parserieImp; i++){
		temp.tempoImp[i]=tempo_json["tempoImp"][i].GetDouble();
	}


	if (tempo_json.HasMember("tempoSatAPoc"))
		nsatAPoc =
				tempo_json["tempoSatAPoc"].Size();
	else nsatAPoc=1;
	if(tempo_json.HasMember("tempoSatAPoc")){
		if(tempo_json["tempoSatAPoc"].Size()!=tempo_json["satAPoc"].Size()){
			logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'tempoSatAPoc', 'satAPoc' com dimensoes diferentes",
					chaveJson, "Condicoes de tempo");
		}
		satAPoc=new double [nsatAPoc];
		tempoSatAPoc=new double [nsatAPoc];
		for(int i=0; i<nsatAPoc; i++){
			satAPoc[i]=tempo_json["satAPoc"][i].GetDouble();
			tempoSatAPoc[i]=tempo_json["tempoSatAPoc"][i].GetDouble();
		}
	}
	else{
		satAPoc=new double [nsatAPoc];
		tempoSatAPoc=new double [nsatAPoc];
		for(int i=0; i<nsatAPoc; i++){
			satAPoc[i]=satAconat;
			tempoSatAPoc[i]=0.;
		}
	}

	/*if (tempo_json.HasMember("tempoSatLRes"))
		nsatLRes =
				tempo_json["tempoSatLRes"].Size();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempoSatLRes' inexistente",chaveJson, "condicao de tempo");
	if(tempo_json["tempoSatLRes"].Size()!=tempo_json["satLRes"].Size()){
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Chaves 'tempoSatLRes', 'satLRes' com dimensoes diferentes",
				chaveJson, "Condicoes de tempo");
	}
	satLRes=new double [nsatLRes];
	tempoSatLRes=new double [nsatLRes];
	for(int i=0; i<nsatLRes; i++){
		satLRes[i]=tempo_json["satLRes"][i].GetDouble();
		tempoSatLRes[i]=tempo_json["tempoSatLRes"][i].GetDouble();
	}*/
	nsatLRes =1.;
	satLRes=new double [nsatLRes];
	tempoSatLRes=new double [nsatLRes];
	for(int i=0; i<nsatLRes; i++){
		satLRes[i]=1.;
		tempoSatLRes[i]=0;
	}


	solverMat=0;
	if (tempo_json.HasMember("solverMat"))
		solverMat = tempo_json["solverMat"].GetInt();
	rankLU=-1;
	if (tempo_json.HasMember("rankLU"))
		rankLU = tempo_json["rankLU"].GetInt();
	colore=0;
	if(rankLU>=0){
		if (tempo_json.HasMember("corLU"))
			colore = tempo_json["corLU"].GetBool();
	}

}

void dadosPoro::parse_Prop(Value& prop_json){
	string chaveJson("#/prop");
	prop.parserie=prop_json.Size();
	prop.xmax=new double [prop.parserie];
	prop.xmin=new double [prop.parserie];
	prop.ymax=new double [prop.parserie];
	prop.ymin=new double [prop.parserie];
	prop.kX=new double [prop.parserie];
	prop.kY=new double [prop.parserie];
	prop.poro=new double [prop.parserie];
	prop.compRoc=new double [prop.parserie];
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
		if (prop_json[i].HasMember("permeabilidadeY")) prop.kY[i]=prop_json[i]["permeabilidadeY"].GetDouble()*(9.869233/(1e16));
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'permeabilidadeY', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("permeabilidadeX")) prop.kX[i]=prop_json[i]["permeabilidadeX"].GetDouble()*(9.869233/(1e16));
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'permeabilidadeX', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("porosidade")) prop.poro[i]=prop_json[i]["porosidade"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'porosidade', ", chaveJson, "faltante");

		if (prop_json[i].HasMember("compressibilidadeRocha")) prop.compRoc[i]=prop_json[i]["compressibilidadeRocha"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'compressibilidadeRocha', ", chaveJson, "faltante");

	}
}

void dadosPoro::parse_CI(Value& CI_json){
	string chaveJson("#/CI");
	CI.parserie=CI_json.Size();
	CI.xmax=new double [CI.parserie];
	CI.xmin=new double [CI.parserie];
	CI.ymax=new double [CI.parserie];
	CI.ymin=new double [CI.parserie];
	CI.Dxmax=new double [CI.parserie];
	CI.Dxmin=new double [CI.parserie];
	CI.Dymax=new double [CI.parserie];
	CI.Dymin=new double [CI.parserie];
	CI.val1=new double [CI.parserie];
	CI.val2=new double [CI.parserie];
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

		if (CI_json[i].HasMember("Dxmax")) CI.Dxmax[i]=CI_json[i]["Dxmax"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves D'xmax', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("Dxmin")) CI.Dxmin[i]=CI_json[i]["Dxmin"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'Dxmin',", chaveJson, "faltante");

		if (CI_json[i].HasMember("Dymax")) CI.Dymax[i]=CI_json[i]["Dymax"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'Dymax', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("Dymin")) CI.Dymin[i]=CI_json[i]["Dymin"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'Dymin', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("pressao")) CI.val1[i]=CI_json[i]["pressao"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'pres', ", chaveJson, "faltante");

		if (CI_json[i].HasMember("satAgua")) CI.val2[i]=CI_json[i]["satAgua"].GetDouble();
		else logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Chaves 'satAgua', ", chaveJson, "faltante");

	}
}

void dadosPoro::parse_CC(Value& CC_json) {
	string chaveJson("#/CC");
/*
 * 	int nDiri;
	int nRic;
	int nVN;
	detDiriPoroso* ccDir;
	detRicPoroso* ccRic;
	detVNPoroso* ccVN;
 */

	CC.nDiri=0;
	CC.ccDir=0;
	CC.nRic=0;
	CC.ccRic=0;
	CC.nVN=0;
	CC.ccVN=0;
	CC.rotuloAcop=-1;
	CC.satAcop=satAconat;

	if (CC_json.HasMember("Dirichlet")){
		CC.nDiri =
				CC_json["Dirichlet"].Size();
		CC.ccDir=new detDiriPoroso [CC.nDiri];
	}
	if (CC_json.HasMember("Richardson")){
		CC.nRic =
				CC_json["Richardson"].Size();
		CC.ccRic=new detRicPoroso [CC.nRic];
	}
	if (CC_json.HasMember("VonNewman")){
		CC.nVN =
				CC_json["VonNewman"].Size();
		CC.ccVN=new detVNPoroso [CC.nVN];
	}
	if(acop==1){
		CC.rotuloAcop=CC_json["acoplamento"].GetInt();
		if(CC_json.HasMember("satAcop"))CC.satAcop=CC_json["satAcop"].GetDouble();
		else CC.satAcop=satAconat;
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
		CC.ccDir[i].valSat=new double [CC.ccDir[i].nserie];
		for(int j=0; j<CC.ccDir[i].nserie; j++){
			CC.ccDir[i].tempo[j]=CC_json["Dirichlet"][i]["tempo"][j].GetDouble();
			CC.ccDir[i].val[j]=CC_json["Dirichlet"][i]["valor"][j].GetDouble();
			if(CC_json["Dirichlet"][i].HasMember("valorSat"))
				CC.ccDir[i].valSat[j]=CC_json["Dirichlet"][i]["valorSat"][j].GetDouble();
			else CC.ccDir[i].valSat[j]=satAconat;
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
		CC.ccVN[i].valSat=new double [CC.ccVN[i].nserie];
		for(int j=0; j<CC.ccVN[i].nserie; j++){
			CC.ccVN[i].tempo[j]=CC_json["VonNewman"][i]["tempo"][j].GetDouble();
			CC.ccVN[i].val[j]=CC_json["VonNewman"][i]["valor"][j].GetDouble();
			if(CC_json["VonNewman"][i].HasMember("valorSat"))
				CC.ccVN[i].valSat[j]=CC_json["VonNewman"][i]["valorSat"][j].GetDouble();
			else CC.ccVN[i].valSat[j]=satAconat;
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
		CC.ccRic[i].valSat=new double [CC.ccRic[i].nserie];
		for(int j=0; j<CC.ccRic[i].nserie; j++){
			CC.ccRic[i].tempo[j]=CC_json["Richardson"][i]["tempo"][j].GetDouble();
			CC.ccRic[i].valAmb[j]=CC_json["Richardson"][i]["valorAmb"][j].GetDouble();
			CC.ccRic[i].hAmb[j]=CC_json["Richardson"][i]["hAmb"][j].GetDouble();
			if(CC_json["Richardson"][i].HasMember("valorSat"))
				CC.ccRic[i].valSat[j]=CC_json["Richardson"][i]["valorSat"][j].GetDouble();
			else CC.ccRic[i].valSat[j]=satAconat;
		}
	}
}

void dadosPoro::parse_malha(Value& malha_json) {
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

	if (malha_json.HasMember("dxMalhaRegular"))
		dxH = malha_json["dxMalhaRegular"].GetDouble();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'dxMalhaRegular' inexistente",chaveJson, "malha");

	if (malha_json.HasMember("dyMalhaRegular"))
		dyH = malha_json["dyMalhaRegular"].GetDouble();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'dyMalhaRegular' inexistente",chaveJson, "malha");

}


