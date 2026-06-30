/*
 * dados1Poisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */

#include "dados3DPoisson.h"

dadosP3D::dadosP3D(string nomeArquivoEntrada){
	iniciaVariaveis();
	lerPoisson(nomeArquivoEntrada);
}

dadosP3D::dadosP3D(){
	iniciaVariaveis();
}

dadosP3D::dadosP3D(const dadosP3D& vPoisson){
	iniciaVariaveis();
	lerPoisson(vPoisson.entrada);
}

dadosP3D& dadosP3D::operator =(const dadosP3D& vPoisson) {
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
		if(nele>0)delete[] vecsra;
		if(nele>0)delete[] veclm;
		if(nele>0)delete[] vecfnz;
	    if(elementoO!=0)delete [] elementoO;

	    if(acop==1 && CC.nAcop>0){
	    	if(tParede!=0)delete[] tParede;
	    	if(tInt!=0)delete[] tInt;
	    	if(hE!=0)delete[] hE;
	    	if(hI!=0)delete[] hI;
	    	if(qAcop!=0)delete[] qAcop;
	    	if(qTotal!=0)delete[] qTotal;
	    	if(tInt0!=0)delete[] tInt0;
	    	if(diamRef!=0)delete[] diamRef;
	    }

		iniciaVariaveis();
		if(vPoisson.nele>0)	lerPoisson2(vPoisson.entrada,vPoisson);
	}

	return *this;
}

void dadosP3D::iniciaVariaveis(){

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
	tParede=0;
	tInt=0;

	hE=0;
	hI=0;
	qAcop=0;
	qTotal=0;
	tInt0=tInt;

	elearq="";
	noarq="";
	polyarq="";
	entrada="";

	elementoO=0;

	dimExt=-1;
	diamRef=0;

	tParede=0;
	tInt=0;
	hE=0;
	hI=0;
	qAcop=0;
	qTotal=0;
	tInt0=0;
	diamRef=0;

	rank=0;
	colorido=0;
	solverMat=0;

}

void dadosP3D::lerPoisson(string nomeArquivoEntrada){


	entrada=nomeArquivoEntrada;
	FILE *fp = fopen(nomeArquivoEntrada.c_str(), "r");
	  char readBuffer[125536];
	  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	  Document elementoPoisson3DRaiz;
	  elementoPoisson3DRaiz.ParseStream(is);
	  fclose(fp);

	  assert(elementoPoisson3DRaiz.HasMember("malha"));

	  if (elementoPoisson3DRaiz.HasMember("malha")){
				parse_malha(elementoPoisson3DRaiz["malha"]);
	  } else {
				exit(-1);
	  }
	  parse_configIni(elementoPoisson3DRaiz["configuracaoInicial"]);
	  parse_CC(elementoPoisson3DRaiz["CC"]);
	  parse_Prop(elementoPoisson3DRaiz["prop"]);
	  if(acop==1){
		  tParede=new double[CC.nAcop];
		  hE=new double[CC.nAcop];
		  hI=new double[CC.nAcop];
		  qAcop=new double[CC.nAcop];
		  qTotal=new double[CC.nAcop];
		  parse_corte(elementoPoisson3DRaiz["secaoTransversal"]);
	  }

	  string val;
	  //nno;//vari�vel inteira indicando o n�mero de n�s da malha

	  vector<cordaNo> cordas;
	  vector<faceNo> faces;
	  vector<eleNo> elemento;


	  int nvert=4;
	  vector<string> strCC;
	  string eleUNV;
	  eleUNV=elearq;
	  ifstream lendoUNV(eleUNV.c_str(), ios_base::in);

	  string chave;
	  vector<cartesiano> vecXY;
	  char line[4000];
	  lendoUNV.get(line, 4000);
	  lendoUNV >> chave;
	  while (chave != "2411") {
		  lendoUNV >> chave;
	  }

	  double chaveN[3]={0,0,0};
	  cartesiano tempC;
	  for(int i=0;i<3;i++)lendoUNV>>chaveN[i];
	  tempC.ind=chaveN[0];
	  tempC.tipo=0;
	  tempC.vertice=0;
	  tempC.condCont="interno";
	  while(chaveN[0]!=-1 && chaveN[1]!=-1 && chaveN[2]!=2412){
		  for(int i=1;i<3;i++){
			  chaveN[i-1]=chaveN[i];
		  }
		  lendoUNV>>chaveN[2];
		  for(int i=0;i<3;i++)lendoUNV>>tempC.x[i];
		  vecXY.push_back(tempC);
		  for(int i=0;i<3;i++)lendoUNV>>chaveN[i];
		  tempC.ind=chaveN[0];
		  tempC.tipo=0;
		  tempC.vertice=0;
		  tempC.condCont="interno";
	  }

	  cordaNo tempCorda;
	  int chaveFace[8]={0,0,0,0,0,0,0,0};
	  int chaveInt;
	  int match;
	  while(chaveFace[4]!=3){
		  lendoUNV>>tempCorda.ind;
		  for(int i=0;i<8;i++)lendoUNV>>chaveFace[i];
		  if(chaveFace[4]!=3){
			  for(int i=0;i<2;i++)lendoUNV>>tempCorda.no[i];
			  match=0;
			  int nVceXY=vecXY.size();
			  for(int kno=0; kno<nVceXY; kno++){
				  for(int kloc=0;kloc<2;kloc++){
					  if(vecXY[kno].ind==tempCorda.no[kloc]){
						  match++;
					  }
				  }
				  if(match==2) break;
			  }
			  cordas.push_back(tempCorda);
		  }
	  }

	  int ncord=cordas.size();

	  faceNo tempFace;

	  tempFace.ind=tempCorda.ind;
	  vector<int> seqNoF;
	  for(int i=0;i<3;i++){
		  tempFace.no[i]=chaveFace[i+5];
		  seqNoF.push_back(tempFace.no[i]);
	  }
	  sort(seqNoF.begin(),seqNoF.end());
	  tempFace.nomeFace=to_string(seqNoF[0]);
	  for(int j=1;j<3;j++){
	  	  tempFace.nomeFace+="-";
	  	  tempFace.nomeFace+=to_string(seqNoF[j]);
	  }
	  match=0;
	  for(int kco=0;kco<ncord;kco++){
		  int matchLoc=0;
		  for(int kloc=0;kloc<2;kloc++){
			  for(int kloc2=0;kloc2<3;kloc2++){
				  if(cordas[kco].no[kloc]==tempFace.no[kloc2]){
					  matchLoc++;
				  }
			  }
			  if(matchLoc==2){
				  match++;
			  }
		  }
		  if(match==3) break;
	  }
	  tempFace.tipo=0;
	  tempFace.condCont="interno";
	  faces.push_back(tempFace);
	  int chaveEle[5]={0,0,0,0,0};
	  while(chaveEle[4]!=4){
		  lendoUNV>>tempFace.ind;
		  for(int i=0;i<5;i++)lendoUNV>>chaveEle[i];
		  if(chaveEle[4]!=4){
			  vector<int> seqNo;
			  for(int i=0;i<3;i++){
				  lendoUNV>>tempFace.no[i];
				  seqNo.push_back(tempFace.no[i]);
			  }
			  sort(seqNo.begin(),seqNo.end());
			  tempFace.nomeFace=to_string(seqNo[0]);
			  for(int j=1;j<3;j++){
			  	  tempFace.nomeFace+="-";
			  	  tempFace.nomeFace+=to_string(seqNo[j]);
			  }
			  match=0;
			  for(int kco=0;kco<ncord;kco++){
				  int matchLoc=0;
				  for(int kloc=0;kloc<2;kloc++){
					  for(int kloc2=0;kloc2<3;kloc2++){
						  if(cordas[kco].no[kloc]==tempFace.no[kloc2]){
							  matchLoc++;
						  }
					  }
					  if(matchLoc==2){
						  match++;
					  }
				  }
				  if(match==3) break;
			  }
			  tempFace.tipo=0;
			  tempFace.condCont="interno";
			  faces.push_back(tempFace);
		  }
	  }

	  unordered_map<string, int> mapaFaces;
	  for (int i = 0; i < faces.size(); ++i) {
	  	mapaFaces[faces[i].nomeFace] = i;
	  }

	  eleNo tempEle;
	  int nface=faces.size();
	  tempEle.ind=tempFace.ind;
	  for(int i=0;i<4;i++)lendoUNV>>tempEle.no[i];
	  for(int i=0;i<4;i++){
		  int konta=i;
		  vector<int> seqNo;
		  for(int j=0;j<3;j++){
			  if(konta>3)konta=0;
			  tempEle.indFaceNo[i][j]=tempEle.no[konta];
			  seqNo.push_back(tempEle.indFaceNo[i][j]);
			  konta++;
		  }
		  sort(seqNo.begin(),seqNo.end());
		  tempEle.nomeFace[i]=to_string(seqNo[0]);
		  for(int j=1;j<3;j++){
			  tempEle.nomeFace[i]+="-";
			  tempEle.nomeFace[i]+=to_string(seqNo[j]);
		  }
	  }

	  for (int kloc2 = 0; kloc2 < 4; ++kloc2) {
	  	auto it = mapaFaces.find(tempEle.nomeFace[kloc2]);
	  	if (it != mapaFaces.end()) {
	  		tempEle.indFace[kloc2] = it->second;
	  	}
	  }

	  elemento.push_back(tempEle);
	  lendoUNV>>tempEle.ind;
	  for(int i=0;i<2;i++) lendoUNV>>chaveEle[i];
	  while(tempEle.ind!=-1 && chaveEle[0]!=-1 && chaveEle[1]!=2467){
		  for(int kVert=0;kVert<nvert;kVert++){
			  tempEle.indFace[kVert]=-1;
			  for(int kT=0; kT<3; kT++){
				  tempEle.indFaceNo[kVert][kT]=-1;
			  }
		  }
		  for(int i=2;i<5;i++)lendoUNV>>chaveEle[i];
		  for(int i=0;i<4;i++)lendoUNV>>tempEle.no[i];
		  for(int i=0;i<4;i++){
			  int konta=i;
			  vector<int> seqNo;
			  for(int j=0;j<3;j++){
				  if(konta>3)konta=0;
				  tempEle.indFaceNo[i][j]=tempEle.no[konta];
				  seqNo.push_back(tempEle.indFaceNo[i][j]);
				  konta++;
			  }
			  sort(seqNo.begin(),seqNo.end());
			  tempEle.nomeFace[i]=to_string(seqNo[0]);
			  for(int j=1;j<3;j++){
				  tempEle.nomeFace[i]+="-";
				  tempEle.nomeFace[i]+=to_string(seqNo[j]);
			  }
		  }

		  for (int kloc2 = 0; kloc2 < 4; ++kloc2) {
		  	auto it = mapaFaces.find(tempEle.nomeFace[kloc2]);
		  	if (it != mapaFaces.end()) {
		  		tempEle.indFace[kloc2] = it->second;
		  	}
		  }

		  elemento.push_back(tempEle);
		  lendoUNV>>tempEle.ind;
		  for(int i=0;i<2;i++) lendoUNV>>chaveEle[i];
	  }
	  nele=elemento.size();

	  string condicaoContorno;
	  lendoUNV>>chaveInt;
	  int inCC=1;
	  while(chaveInt!=-1){
		  for(int i=0;i<7;i++){
			  lendoUNV>>chaveInt;
		  }
		  lendoUNV>>condicaoContorno;
		  strCC.push_back(condicaoContorno);
		  int indCont;
		  int konta=0;
		  int evita=0;
		  for(int ncc=0; ncc<CC.nMuda; ncc++){
			  if(condicaoContorno==CC.mudaRegiao[ncc].rotulo){
				  evita=1;
			  }
		  }
		  while(konta<chaveInt){
			  lendoUNV>>indCont;
			  lendoUNV>>indCont;
			  if(indCont>=faces[0].ind && indCont<=faces[nface-1].ind){
				  for(int i=0;i<nface;i++){
					  int queNo[3]={-1,-1,-1};
					  if(faces[i].ind==indCont){
						  if(evita==0)faces[i].tipo=inCC;
						  if(evita==0)faces[i].condCont=condicaoContorno;
						  queNo[0]=faces[i].no[0]-1;
						  queNo[1]=faces[i].no[1]-1;
						  queNo[2]=faces[i].no[2]-1;
						  if(evita==0){
							  if(vecXY[queNo[0]].tipo==0 && vecXY[queNo[1]].tipo==0 && vecXY[queNo[2]].tipo==0){
								  vecXY[queNo[0]].vertice=0;
								  vecXY[queNo[1]].vertice=0;
								  vecXY[queNo[2]].vertice=0;
								  vecXY[queNo[0]].tipo=inCC;
								  vecXY[queNo[0]].condCont=condicaoContorno;
								  vecXY[queNo[1]].tipo=inCC;
								  vecXY[queNo[1]].condCont=condicaoContorno;
								  vecXY[queNo[2]].tipo=inCC;
								  vecXY[queNo[2]].condCont=condicaoContorno;
							  }
							  else{
								  for(int kont=0;kont<3;kont++){
									  if(vecXY[queNo[kont]].tipo>0){
										  if(vecXY[queNo[kont]].tipo==inCC){
											  vecXY[queNo[kont]].vertice=0;
										  }
										  else{
											  vecXY[queNo[kont]].vertice=1;
										  }
									  }
									  else{
										  vecXY[queNo[kont]].vertice=0;
										  vecXY[queNo[kont]].tipo=inCC;
										  vecXY[queNo[kont]].condCont=condicaoContorno;
									  }
								  }
							  }
						  }
					  }
					  if(queNo[0]>=0 && queNo[1]>=0 && queNo[2]>=0)break;
				  }
			  }
			  else{
				  for(int i=0;i<nele;i++){
					  if(elemento[i].ind==indCont){
						  elemento[i].regiao=condicaoContorno;
					  }
				  }
			  }
			  for(int i=0;i<2;i++)lendoUNV>>indCont;
			  konta++;
		  }
		  lendoUNV>>chaveInt;
		  inCC++;
	  }
	  nno=vecXY.size();
	  xcoor = new double* [nno];
	  for(int i=0;i<nno;i++)xcoor[i]=new double [nvert-1];
	  atributo = new double [nno];
	  tipo = new int [nno];
	  for(int j=0;j<nno;j++){
		  xcoor[j][0]=vecXY[j].x[0];
		  xcoor[j][1]=vecXY[j].x[1];
		  xcoor[j][2]=vecXY[j].x[2];
		  atributo[j]=vecXY[j].vertice;
		  tipo[j]=vecXY[j].tipo;
	  }
	  noEle = new int* [nele];
	  for(int i=0;i<nele;i++){
		  noEle[i]=new int [nvert];
	  }

	  for(int j=0;j<nele;j++){
		  for(int k=0;k<nvert;k++){
			  noEle[j][k]=elemento[j].no[k];
			  noEle[j][k]--;
		  }
	  }
	  lendoUNV.close();


	  elementoO=new eleOBJ [nele];
	  for(int konta=0; konta<nele;konta++){
		  elementoO[konta].ind=elemento[konta].ind-nface-ncord-1;
		  elementoO[konta].regiao=elemento[konta].regiao;
		  for(int kontaN=0;kontaN<4;kontaN++){
			  elementoO[konta].noGlobal[kontaN]=elemento[konta].no[kontaN]-1;
			  elementoO[konta].nomeFace[kontaN]=elemento[konta].nomeFace[kontaN];
			  if(elemento[konta].indFace[kontaN]>=0 && faces[elemento[konta].indFace[kontaN]].condCont!="interno"){
				  elementoO[konta].indFaceGlobal[kontaN]=elemento[konta].indFace[kontaN];
			  }
			  else{
				  elementoO[konta].indFaceGlobal[kontaN]=-1;
			  }
			  elementoO[konta].objNo[kontaN]=vecXY[elementoO[konta].noGlobal[kontaN]];
			  if(elemento[konta].indFace[kontaN]>=0 && faces[elemento[konta].indFace[kontaN]].condCont!="interno"){
				  elementoO[konta].objFace[kontaN]=faces[elementoO[konta].indFaceGlobal[kontaN]];
			  }
			  for(int kontaN2=0;kontaN2<3;kontaN2++)elementoO[konta].indFaceNo[kontaN][kontaN2]=elemento[konta].indFaceNo[kontaN][kontaN2]-1;
		   }
	  }

	  unordered_map<string, vector<int>> mapaFaceParaElementos;
	  for (int i = 0; i < nele; ++i) {
	  	for (int f = 0; f < 4; ++f) {
	  		if (elementoO[i].indFaceGlobal[f] < 0) { // apenas faces internas
	  			mapaFaceParaElementos[elementoO[i].nomeFace[f]].push_back(i);
	  		}
	  	}
	  }


	  for (int i = 0; i < nele; ++i) {
	  	for (int f = 0; f < 4; ++f) {
	  		if (elementoO[i].indViz[f] < 0) {
	  			if (elementoO[i].indFaceGlobal[f] >= 0) {
	  				// face de contorno
	  				elementoO[i].faceCCfaceReal[f] = f;
	  				continue;
	  			}
	  			const auto& nome = elementoO[i].nomeFace[f];
	  			auto termoMapa = mapaFaceParaElementos.find(nome);
	  			if (termoMapa != mapaFaceParaElementos.end()) {
	  				const auto& candidatos = termoMapa->second;
	  				for (int viz : candidatos) {
	  					if (viz != i) {
	  						// procurar qual face local em viz tem o mesmo nome
	  						for (int f2 = 0; f2 < 4; ++f2) {
	  							if (elementoO[viz].nomeFace[f2] == nome) {
	  								elementoO[i].indViz[f] = viz;
	  								elementoO[viz].indViz[f2] = i;
	  								break;
	  							}
	  						}
	  					}
	  				}
	  			}
	  		}
	  	}
	  }



  noZero=0;
  for(int elem=0;elem<nele;elem++){
	  noZero++;
	  for(int kontaF=0;kontaF<4;kontaF++){
		  if(elementoO[elem].indViz[kontaF]>=0)noZero++;
	  }
  }



	ostringstream saidaElemento;
	saidaElemento << pathPrefixoArqSaida << "elementoUNV-Detalhes.dat";
	string tmpEle = saidaElemento.str();
	ofstream escreveEle(tmpEle.c_str(), ios_base::out);

	int indMaxRASpec=0;
	for(int i=0;i<nele;i++){
		double aresta[3];
		double arestaMax=0;
		double hmin=1e6;
		for(int j=0;j<4;j++){

			int vert[3];
			int kvT=0;
			for(int kv=1;kv<4;kv++){
				if(j+kv>3)kvT=4;
				vert[kv-1]=j+kv-kvT;
			}

			double dcoor[3];
			for(int kv=0;kv<3;kv++){
				aresta[kv]=0;
				for(int k=0;k<3;k++){
					dcoor[k]=elementoO[i].objNo[j].x[k]-elementoO[i].objNo[vert[kv]].x[k];
					aresta[kv]+=dcoor[k]*dcoor[k];
				}
				aresta[kv]=sqrt(aresta[kv]);
				if(arestaMax<aresta[kv])arestaMax=aresta[kv];
			}
		}
		for(int j=0;j<4;j++){

			double V1[3];
			double V2[3];
			double centro[3];
			double prodV[3];
			double corda[3];
			int vert[3];
			int kvT=0;
			for(int kv=1;kv<4;kv++){
				if(j+kv>3)kvT=4;
				vert[kv-1]=j+kv-kvT;
			}
			for(int k=0;k<3;k++){
				V1[k]=elementoO[i].objNo[vert[1]].x[k]-elementoO[i].objNo[vert[0]].x[k];
				V2[k]=elementoO[i].objNo[vert[2]].x[k]-elementoO[i].objNo[vert[0]].x[k];
				centro[k]=(elementoO[i].objNo[vert[2]].x[k]+elementoO[i].objNo[vert[1]].x[k]+elementoO[i].objNo[vert[0]].x[k])/3.;
				corda[k]=elementoO[i].objNo[j].x[k]-elementoO[i].objNo[vert[1]].x[k];
			}
			prodV[0]=V1[1]*V2[2]-V1[2]*V2[1];
			prodV[1]=V1[2]*V2[0]-V1[0]*V2[2];
			prodV[2]=V1[0]*V2[1]-V1[1]*V2[0];
			double modPV=0;
			for(int k=0;k<3;k++)modPV+=prodV[k]*prodV[k];
			modPV=sqrt(modPV);
			for(int k=0;k<3;k++)prodV[k]/=modPV;
			double alt=0;
			for(int k=0;k<3;k++)alt+=corda[k]*prodV[k];
			alt=fabs(alt);
			if(alt<hmin)hmin=alt;
		}
		elementoO[i].RAspec=arestaMax/hmin;
		if(i>0){
			if(elementoO[i].RAspec>elementoO[indMaxRASpec].RAspec)indMaxRASpec=i;
		}
	}

	for(int i=0;i<nele;i++){
		int kontaCC=0;
		for(int j=0;j<4;j++){
			if(elementoO[i].indViz[j]<0)kontaCC++;
		}
		escreveEle << i<<" ; ";
		escreveEle << elementoO[i].noGlobal[0]<<" ; ";
		escreveEle << elementoO[i].noGlobal[1]<<" ; ";
		escreveEle << elementoO[i].noGlobal[2]<<" ; ";
		escreveEle << elementoO[i].noGlobal[3]<<" ; ";
		escreveEle << elementoO[i].regiao<<" ; ";
		escreveEle << elementoO[i].indViz[0]<<" ; ";
		escreveEle << elementoO[i].indViz[1]<<" ; ";
		escreveEle << elementoO[i].indViz[2]<<" ; ";
		escreveEle << elementoO[i].indViz[3]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[0]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[1]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[2]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[3]<<" ; ";
		escreveEle << elementoO[i].objFace[0].condCont<<" ; ";
		escreveEle << elementoO[i].objFace[1].condCont<<" ; ";
		escreveEle << elementoO[i].objFace[2].condCont<<" ; ";
		escreveEle << elementoO[i].objFace[3].condCont<<" ; ";
		escreveEle << elementoO[i].RAspec<<" ; ";
		escreveEle << "numero de fronteiras = "<<kontaCC;
		escreveEle << "\n";


	}
	escreveEle.close();

	ostringstream saidaElemento2;
	saidaElemento2 << pathPrefixoArqSaida << "elementoUNV.dat";
	string tmpEle2 = saidaElemento2.str();
	ofstream escreveEle2(tmpEle2.c_str(), ios_base::out);

	for(int i=0;i<nele;i++){
		escreveEle2 << i<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[0]<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[1]<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[2]<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[3];
		escreveEle2 << "\n";


	}
	escreveEle2.close();

	ostringstream saidaNo;
	saidaNo << pathPrefixoArqSaida << "noUNV.dat";
	string tmpNo = saidaNo.str();
	ofstream escreveNo(tmpNo.c_str(), ios_base::out);

	for(int i=0;i<nno;i++){
		escreveNo << i<<" ; ";
		escreveNo << vecXY[i].x[0]<<" ; ";
		escreveNo << vecXY[i].x[1]<<" ; ";
		escreveNo << vecXY[i].x[2];
		escreveNo << "\n";
	}
	escreveNo.close();


  vecsra=new double [noZero];
  veclm=new int [noZero];
  vecfnz=new int [nele+1];
  vecfnz[nele]=noZero;

}


void dadosP3D::lerPoisson2(string nomeArquivoEntrada,const dadosP3D& aux){


	entrada=nomeArquivoEntrada;
	FILE *fp = fopen(nomeArquivoEntrada.c_str(), "r");
	  char readBuffer[125536];
	  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	  Document elementoPoisson3DRaiz;
	  elementoPoisson3DRaiz.ParseStream(is);
	  fclose(fp);

	  assert(elementoPoisson3DRaiz.HasMember("malha"));

	  if (elementoPoisson3DRaiz.HasMember("malha")){
				parse_malha(elementoPoisson3DRaiz["malha"]);
	  } else {
				exit(-1);
	  }
	  parse_configIni(elementoPoisson3DRaiz["configuracaoInicial"]);
	  parse_CC(elementoPoisson3DRaiz["CC"]);
	  parse_Prop(elementoPoisson3DRaiz["prop"]);
	  if(acop==1){
		  tParede=new double[CC.nAcop];
		  hE=new double[CC.nAcop];
		  hI=new double[CC.nAcop];
		  qAcop=new double[CC.nAcop];
		  qTotal=new double[CC.nAcop];
		  parse_corte(elementoPoisson3DRaiz["secaoTransversal"]);
	  }

	  string val;

	  int nvert=4;

	  nele=aux.nele;


	  nno=aux.nno;
	  xcoor = new double* [nno];
	  for(int i=0;i<nno;i++)xcoor[i]=new double [nvert-1];
	  atributo = new double [nno];
	  tipo = new int [nno];
	  for(int j=0;j<nno;j++){
	  		  xcoor[j][0]=aux.xcoor[j][0];
	  		  xcoor[j][1]=aux.xcoor[j][1];
	  		  xcoor[j][2]=aux.xcoor[j][2];
	  		  atributo[j]=aux.atributo[j];
	  		  tipo[j]=aux.tipo[j];
	  }
	  nele=aux.nele;
	  noEle = new int* [nele];
	  for(int i=0;i<nele;i++){
		  noEle[i]=new int [nvert];
	  }
	  for(int j=0;j<nele;j++){
		  for(int k=0;k<nvert;k++){
			  noEle[j][k]=aux.noEle[j][k];
		  }
	  }

	  elementoO=new eleOBJ [nele];
	  for(int konta=0; konta<nele;konta++)elementoO[konta]=aux.elementoO[konta];

  noZero=aux.noZero;

	ostringstream saidaElemento;
	saidaElemento << pathPrefixoArqSaida << "elementoUNV-Detalhes.dat";
	string tmpEle = saidaElemento.str();
	ofstream escreveEle(tmpEle.c_str(), ios_base::out);


	for(int i=0;i<nele;i++){
		int kontaCC=0;
		for(int j=0;j<4;j++){
			if(elementoO[i].indViz[j]<0)kontaCC++;
		}
		escreveEle << i<<" ; ";
		escreveEle << elementoO[i].noGlobal[0]<<" ; ";
		escreveEle << elementoO[i].noGlobal[1]<<" ; ";
		escreveEle << elementoO[i].noGlobal[2]<<" ; ";
		escreveEle << elementoO[i].noGlobal[3]<<" ; ";
		escreveEle << elementoO[i].regiao<<" ; ";
		escreveEle << elementoO[i].indViz[0]<<" ; ";
		escreveEle << elementoO[i].indViz[1]<<" ; ";
		escreveEle << elementoO[i].indViz[2]<<" ; ";
		escreveEle << elementoO[i].indViz[3]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[0]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[1]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[2]<<" ; ";
		escreveEle << elementoO[i].indFaceGlobal[3]<<" ; ";
		escreveEle << elementoO[i].objFace[0].condCont<<" ; ";
		escreveEle << elementoO[i].objFace[1].condCont<<" ; ";
		escreveEle << elementoO[i].objFace[2].condCont<<" ; ";
		escreveEle << elementoO[i].objFace[3].condCont<<" ; ";
		escreveEle << elementoO[i].RAspec<<" ; ";
		escreveEle << "numero de fronteiras = "<<kontaCC;
		escreveEle << "\n";


	}
	escreveEle.close();

	ostringstream saidaElemento2;
	saidaElemento2 << pathPrefixoArqSaida << "elementoUNV.dat";
	string tmpEle2 = saidaElemento2.str();
	ofstream escreveEle2(tmpEle2.c_str(), ios_base::out);

	for(int i=0;i<nele;i++){
		escreveEle2 << i<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[0]<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[1]<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[2]<<" ; ";
		escreveEle2 << elementoO[i].noGlobal[3];
		escreveEle2 << "\n";


	}
	escreveEle2.close();

	ostringstream saidaNo;
	saidaNo << pathPrefixoArqSaida << "noUNV.dat";
	string tmpNo = saidaNo.str();
	ofstream escreveNo(tmpNo.c_str(), ios_base::out);

	for(int i=0;i<nno;i++){
		escreveNo << i<<" ; ";
		escreveNo << xcoor[i][0]<<" ; ";
		escreveNo << xcoor[i][1]<<" ; ";
		escreveNo << xcoor[i][2];
		escreveNo << "\n";
	}
	escreveNo.close();


  vecsra=new double [noZero];
  veclm=new int [noZero];
  vecfnz=new int [nele+1];
  vecfnz[nele]=noZero;

}



void dadosP3D::parse_corte(Value& corte_json) {
			// de-para do elementoPoisson3D do json para o struct
	string chaveJson("#/secao Transversal");
	if(corte_json.Size()==CC.nAcop){
		for(int i=0;i<CC.nAcop;i++){

			diamRef=new double[CC.nAcop];
			if (corte_json[i].HasMember("diamRef"))diamRef[i] = corte_json[i]["diamRef"].GetDouble();
			else logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Chave diamRef",
							chaveJson, "Chave inexistente");
			tInt=new double[CC.nAcop];
			tInt0=new double[CC.nAcop];

			if (corte_json[i].HasMember("DimensaoReferenciaExterna"))
				dimExt =corte_json[i]["DimensaoReferenciaExterna"].GetDouble();
		}
	}
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave secao Transversal",
					chaveJson, "dimensao distinta do numeros de condicoes de acoplamento");
}



void dadosP3D::parse_configIni(Value& configuracaoInicial_json) {
	string chaveJson("#/tempo");

	temp.relax=1.;
	temp.parserie=0;
	temp.dtmax=0;
	temp.tempo=0;
	temp.tempoImp=0;
	temp.parserieImp=0;

	acop=1;
	angAcop=6.2831853;



	solverMat=0;
	if (configuracaoInicial_json.HasMember("solverMat"))
		solverMat = configuracaoInicial_json["solverMat"].GetInt();
	rank=0;
	if (configuracaoInicial_json.HasMember("rankLU"))
		rank = configuracaoInicial_json["rankLU"].GetInt();

	colorido=0;
	if (configuracaoInicial_json.HasMember("multiColor"))
		colorido = configuracaoInicial_json["multiColor"].GetInt();

	if (configuracaoInicial_json.HasMember("relax"))
		temp.relax =
				configuracaoInicial_json["relax"].GetDouble();

	if (configuracaoInicial_json.HasMember("dtMax"))
		temp.parserie =
				configuracaoInicial_json["dtMax"].Size();
	if(configuracaoInicial_json["dtMax"].Size()!=configuracaoInicial_json["tempo"].Size()){
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Chaves 'dtMax', 'tempo' com dimensoes diferentes",
				chaveJson, "Condicoes de tempo");
	}
	temp.dtmax=new double [temp.parserie];
	temp.tempo=new double [temp.parserie];
	for(int i=0; i<temp.parserie; i++){
		temp.dtmax[i]=configuracaoInicial_json["dtMax"][i].GetDouble();
		temp.tempo[i]=configuracaoInicial_json["tempo"][i].GetDouble();
	}


	if (configuracaoInicial_json.HasMember("tempoImp"))
		temp.parserieImp =
				configuracaoInicial_json["tempoImp"].Size();
	temp.tempoImp=new double [temp.parserieImp];
	for(int i=0; i<temp.parserieImp; i++){
		temp.tempoImp[i]=configuracaoInicial_json["tempoImp"][i].GetDouble();
	}
}

void dadosP3D::parse_Prop(Value& prop_json){
	string chaveJson("#/prop");
	prop.parserie=prop_json.Size();
	prop.regiao=new string [prop.parserie];
	prop.cond=new double [prop.parserie];
	prop.rho=new double [prop.parserie];
	prop.cp=new double [prop.parserie];
	for(int i=0;i<prop.parserie;i++){
		if (prop_json[i].HasMember("regiao")) prop.regiao[i]=prop_json[i]["regiao"].GetString();
		else{
			if(prop.parserie>1)
				logger.log(LOGGER_FALHA,LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,"Chaves 'regiao', ", chaveJson, "faltante");
			else
				prop.regiao[i]="unica";
		}
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



void dadosP3D::parse_CC(Value& CC_json) {
	string chaveJson("#/CC");

	CC.nDiri=0;
	CC.ccDir=0;
	CC.nRic=0;
	CC.ccRic=0;
	CC.nVN=0;
	CC.ccVN=0;
	CC.nMuda=0;
	CC.nAcop=0;

	if (CC_json.HasMember("Dirichlet")){
		CC.nDiri =
				CC_json["Dirichlet"].Size();
		CC.ccDir=new detDiriPoisson3D [CC.nDiri];
	}
	if (CC_json.HasMember("Richardson")){
		CC.nRic =
				CC_json["Richardson"].Size();
		CC.ccRic=new detRicPoisson3D [CC.nRic];
	}
	if (CC_json.HasMember("VonNewman")){
		CC.nVN =
				CC_json["VonNewman"].Size();
		CC.ccVN=new detVNPoisson3D [CC.nVN];
	}
	if (CC_json.HasMember("interface")){
		CC.nMuda =
				CC_json["interface"].Size();
		CC.mudaRegiao=new detMudaRegiao3D [CC.nMuda];
	}
	if(acop==1){
		CC.nAcop =CC_json["acoplamento"].Size();
		CC.rotuloAcop=new string [CC.nAcop];
	}

	for(int i=0; i<CC.nAcop; i++){
		CC.rotuloAcop[i]=CC_json["acoplamento"][i].GetString();
	}

	for(int i=0; i<CC.nDiri; i++){
		CC.ccDir[i].rotulo=CC_json["Dirichlet"][i]["rotulo"].GetString();
		CC.ccDir[i].nserie=1;
		CC.ccDir[i].tempo=new double [CC.ccDir[i].nserie];
		CC.ccDir[i].val=new double [CC.ccDir[i].nserie];
		for(int j=0; j<CC.ccDir[i].nserie; j++){
			CC.ccDir[i].tempo[j]=CC_json["Dirichlet"][i]["tempo"][j].GetDouble();
			CC.ccDir[i].val[j]=CC_json["Dirichlet"][i]["valor"][j].GetDouble();
		}
	}

	for(int i=0; i<CC.nVN; i++){
		CC.ccVN[i].rotulo=CC_json["VonNewman"][i]["rotulo"].GetString();
		CC.ccVN[i].nserie=1;
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
		CC.ccRic[i].rotulo=CC_json["Richardson"][i]["rotulo"].GetString();
		CC.ccRic[i].nserie=1;
		CC.ccRic[i].tempo=new double [CC.ccRic[i].nserie];
		CC.ccRic[i].valAmb=new double [CC.ccRic[i].nserie];
		CC.ccRic[i].hAmb=new double [CC.ccRic[i].nserie];
		for(int j=0; j<CC.ccRic[i].nserie; j++){
			CC.ccRic[i].tempo[j]=CC_json["Richardson"][i]["tempo"][j].GetDouble();
			CC.ccRic[i].valAmb[j]=CC_json["Richardson"][i]["valorAmb"][j].GetDouble();
			CC.ccRic[i].hAmb[j]=CC_json["Richardson"][i]["hAmb"][j].GetDouble();
		}
	}

	for(int i=0; i<CC.nMuda; i++){
		CC.mudaRegiao[i].rotulo=CC_json["interface"][i]["rotulo"].GetString();
	}
}

void dadosP3D::parse_malha(Value& malha_json) {
	string chaveJson("#/malha");
	if (malha_json.HasMember("UNV"))
		elearq = pathArqExtEntrada + string(malha_json["UNV"].GetString());
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'arquivo de UNV' inexistente",chaveJson, "malha");
}