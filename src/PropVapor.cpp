/*
 * PropVapor.cpp
 *
 *  Created on: 14 de mai de 2018
 *      Author: eduardo
 */




#include "PropVapor.h"

using namespace rapidjson;


ProVap::ProVap(const ProVap& vVap) {
	impfileSat=vVap.impfileSat;
	impfileLiq=vVap.impfileLiq;
	impfileVap=vVap.impfileVap;
	impfileCondL=vVap.impfileCondL;
	impfileCondG=vVap.impfileCondG;
	impfileViscL=vVap.impfileViscL;
	impfileViscG=vVap.impfileViscG;

	TempSup=0;
	vvSup=0;
	uvSup=0;
	hvSup=0;
	svSup=0;
	npropSup=0;

	TempLiqC=0;
	vlComp=0;
	ulComp=0;
	hlComp=0;
	slComp=0;
	npropComp=0;

	TempCondL=0;
	CondL=0;
	nTCondL=0;
	////////////////////////
	TempCondG=0;
	CondG=0;
	nTCondG=0;
	////////////////////////
	TempViscL=0;
	ViscL=0;
	nTViscL=0;
	////////////////////////
	TempViscG=0;
	ViscG=0;
	nTViscG=0;

	LerArqVap();
}

ProVap::ProVap(){

	impfileSat="VaporSat.json";
	impfileLiq="AguaComp.json";
	impfileVap="VaporSupAq.json";
	impfileCondL="CondutividadeLiqComp.json";
	impfileCondG="CondutividadeGasSup.json";
	impfileViscL="ViscLiqComp.json";
	impfileViscG="ViscGasSup.json";

	TempSup=0;
	vvSup=0;
	uvSup=0;
	hvSup=0;
	svSup=0;
	npropSup=0;

	TempLiqC=0;
	vlComp=0;
	ulComp=0;
	hlComp=0;
	slComp=0;
	npropComp=0;

	TempCondL=0;
	CondL=0;
	nTCondL=0;
	////////////////////////
	TempCondG=0;
	CondG=0;
	nTCondG=0;
	////////////////////////
	TempViscL=0;
	ViscL=0;
	nTViscL=0;
	////////////////////////
	TempViscG=0;
	ViscG=0;
	nTViscG=0;

	LerArqVap();

}


ProVap& ProVap::operator =(const ProVap& vVap){
	if (this != &vVap) {
		impfileSat=vVap.impfileSat;
		impfileLiq=vVap.impfileLiq;
		impfileVap=vVap.impfileVap;
		impfileCondL=vVap.impfileCondL;
		impfileCondG=vVap.impfileCondG;
		impfileViscL=vVap.impfileViscL;
		impfileViscG=vVap.impfileViscG;

		delete [] TempSup;
		delete [] vvSup;
		delete [] uvSup;
		delete [] hvSup;
		delete [] svSup;
		delete [] npropSup;

		delete [] TempLiqC;
		delete [] vlComp;
		delete [] ulComp;
		delete [] hlComp;
		delete [] slComp;
		delete [] npropComp;

		delete[] TempCondL;
		delete[] CondL;
		delete[] nTCondL;
		////////////////////////
		delete[] TempCondG;
		delete[] CondG;
		delete[] nTCondG;
		////////////////////////
		delete[] TempViscL;
		delete[] ViscL;
		delete[] nTViscL;
		////////////////////////
		delete[] TempViscG;
		delete[] ViscG;
		delete[] nTViscG;

		LerArqVap();
	}

	return *this;
}

void ProVap::LerArqVap() {

	char readBuffer[65536];

	FILE *fp1 = fopen(impfileSat.c_str(), "r");

	FileReadStream is(fp1, readBuffer, sizeof(readBuffer));
	Document elementoRaiz;
	elementoRaiz.ParseStream(is);
	fclose(fp1);

	assert(elementoRaiz.HasMember("Propriedades"));
	nSat=elementoRaiz["Propriedades"].Size();

	for(int i=0;i<nSat;i++){
		double temp=elementoRaiz["Propriedades"][i][0].GetDouble()*1.0197;
		preSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][1].GetDouble();
		tempSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][2].GetDouble();
		temp/=1000.;
		vlSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][3].GetDouble();
		vvSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][4].GetDouble()*1000.;
		ulSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][5].GetDouble()*1000.;
		uvSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][6].GetDouble()*1000.;
		hlSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][7].GetDouble()*1000.;
		hlvSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][8].GetDouble()*1000.;
		hvSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][9].GetDouble()*1000.;
		slSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][10].GetDouble()*1000.;
		svSat.push_back(temp);
		temp=elementoRaiz["Propriedades"][i][11].GetDouble()/1000.;
		TensSupSat.push_back(temp);
	}

	char readBuffer2[65536];
	FILE *fp2 = fopen(impfileVap.c_str(), "r");

	FileReadStream is2(fp2, readBuffer2, sizeof(readBuffer2));
	Document elementoRaiz2;
	elementoRaiz2.ParseStream(is2);
	fclose(fp2);

	assert(elementoRaiz2.HasMember("Propriedades"));
	nPSupAq=elementoRaiz2["Propriedades"].Size();
	TempSup=new vector<double>[nPSupAq];
	vvSup=new vector<double>[nPSupAq];
	uvSup=new vector<double>[nPSupAq];
	hvSup=new vector<double>[nPSupAq];
	svSup=new vector<double>[nPSupAq];
	npropSup= new int[nPSupAq];
	for(int i=0;i<nPSupAq;i++){
		npropSup[i]=elementoRaiz2["Propriedades"][i].Size();
		PresSupAq.push_back(elementoRaiz2["Propriedades"][i][0][0].GetDouble()*1.0197);
		for(int j=0; j<npropSup[i];j++){
			TempSup[i].push_back(elementoRaiz2["Propriedades"][i][j][1].GetDouble());
			vvSup[i].push_back(elementoRaiz2["Propriedades"][i][j][2].GetDouble());
			uvSup[i].push_back(elementoRaiz2["Propriedades"][i][j][3].GetDouble()*1000.);
			hvSup[i].push_back(elementoRaiz2["Propriedades"][i][j][4].GetDouble()*1000.);
			svSup[i].push_back(elementoRaiz2["Propriedades"][i][j][5].GetDouble()*1000.);
		}
		int nmax=npropSup[i]-1;
		MaxTSupAq.push_back(TempSup[i][nmax]);
		MinTSupAq.push_back(TempSup[i][0]);
		MaxUSupAq.push_back(uvSup[i][nmax]);
		MinUSupAq.push_back(uvSup[i][0]);
		MaxHSupAq.push_back(hvSup[i][nmax]);
		MinHSupAq.push_back(hvSup[i][0]);
	}


	char readBuffer3[65536];
	FILE *fp3 = fopen(impfileLiq.c_str(), "r");

	FileReadStream is3(fp3, readBuffer3, sizeof(readBuffer3));
	Document elementoRaiz3;
	elementoRaiz3.ParseStream(is3);
	fclose(fp3);

	assert(elementoRaiz3.HasMember("Propriedades"));
	nPCompL=elementoRaiz3["Propriedades"].Size();
	TempLiqC=new vector<double>[nPCompL];
	vlComp=new vector<double>[nPCompL];
	ulComp=new vector<double>[nPCompL];
	hlComp=new vector<double>[nPCompL];
	slComp=new vector<double>[nPCompL];
	npropComp= new int[nPCompL];
	for(int i=0;i<nPCompL;i++){
		npropComp[i]=elementoRaiz3["Propriedades"][i].Size();
		PresLiqC.push_back(elementoRaiz3["Propriedades"][i][0][0].GetDouble()*1.0197);
		for(int j=0; j<npropComp[i];j++){
			TempLiqC[i].push_back(elementoRaiz3["Propriedades"][i][j][1].GetDouble());
			vlComp[i].push_back(elementoRaiz3["Propriedades"][i][j][2].GetDouble()/1000.);
			ulComp[i].push_back(elementoRaiz3["Propriedades"][i][j][3].GetDouble()*1000.);
			hlComp[i].push_back(elementoRaiz3["Propriedades"][i][j][4].GetDouble()*1000.);
			slComp[i].push_back(elementoRaiz3["Propriedades"][i][j][5].GetDouble()*1000.);
		}
		int nmax=npropComp[i]-1;
		MaxTComp.push_back(TempLiqC[i][nmax]);
		MinTComp.push_back(TempLiqC[i][0]);
		MaxUComp.push_back(ulComp[i][nmax]);
		MinUComp.push_back(ulComp[i][0]);
		MaxHComp.push_back(hlComp[i][nmax]);
		MinHComp.push_back(hlComp[i][0]);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	char readBuffer4[65536];
	FILE *fp4 = fopen(impfileCondL.c_str(), "r");

	FileReadStream is4(fp4, readBuffer4, sizeof(readBuffer4));
	Document elementoRaiz4;
	elementoRaiz4.ParseStream(is4);
	fclose(fp4);

	assert(elementoRaiz4.HasMember("Propriedades"));
	nPCondL=elementoRaiz4["Propriedades"].Size();
	TempCondL=new vector<double>[nPCondL];
	CondL=new vector<double>[nPCondL];
	nTCondL= new int[nPCondL];
	for(int i=0;i<nPCondL;i++){
		nTCondL[i]=elementoRaiz4["Propriedades"][i].Size();
		PresCondL.push_back(elementoRaiz4["Propriedades"][i][0][0].GetDouble()*1.0197);
		for(int j=0; j<nTCondL[i];j++){
			TempCondL[i].push_back(elementoRaiz4["Propriedades"][i][j][1].GetDouble());
			CondL[i].push_back(elementoRaiz4["Propriedades"][i][j][2].GetDouble()/1000.);
		}
	}

	char readBuffer5[65536];
	FILE *fp5 = fopen(impfileCondG.c_str(), "r");

	FileReadStream is5(fp5, readBuffer5, sizeof(readBuffer5));
	Document elementoRaiz5;
	elementoRaiz5.ParseStream(is5);
	fclose(fp5);

	assert(elementoRaiz5.HasMember("Propriedades"));
	nPCondG=elementoRaiz5["Propriedades"].Size();
	TempCondG=new vector<double>[nPCondG];
	CondG=new vector<double>[nPCondG];
	nTCondG= new int[nPCondG];
	for(int i=0;i<nPCondG;i++){
		nTCondG[i]=elementoRaiz5["Propriedades"][i].Size();
		PresCondG.push_back(elementoRaiz5["Propriedades"][i][0][0].GetDouble()*1.0197);
		for(int j=0; j<nTCondL[i];j++){
			TempCondG[i].push_back(elementoRaiz5["Propriedades"][i][j][1].GetDouble());
			CondG[i].push_back(elementoRaiz5["Propriedades"][i][j][2].GetDouble()/1000.);
		}
	}

	char readBuffer6[65536];
	FILE *fp6 = fopen(impfileViscL.c_str(), "r");

	FileReadStream is6(fp6, readBuffer6, sizeof(readBuffer6));
	Document elementoRaiz6;
	elementoRaiz6.ParseStream(is6);
	fclose(fp6);

	assert(elementoRaiz6.HasMember("Propriedades"));
	nPViscL=elementoRaiz6["Propriedades"].Size();
	TempViscL=new vector<double>[nPViscL];
	ViscL=new vector<double>[nPViscL];
	nTViscL= new int[nPViscL];
	for(int i=0;i<nPViscL;i++){
		nTViscL[i]=elementoRaiz6["Propriedades"][i].Size();
		PresViscL.push_back(elementoRaiz6["Propriedades"][i][0][0].GetDouble()*1.0197);
		for(int j=0; j<nTViscL[i];j++){
			TempViscL[i].push_back(elementoRaiz6["Propriedades"][i][j][1].GetDouble());
			ViscL[i].push_back(elementoRaiz6["Propriedades"][i][j][2].GetDouble()/1000.);
		}
	}

	char readBuffer7[65536];
	FILE *fp7 = fopen(impfileViscG.c_str(), "r");

	FileReadStream is7(fp7, readBuffer7, sizeof(readBuffer7));
	Document elementoRaiz7;
	elementoRaiz7.ParseStream(is7);
	fclose(fp7);

	assert(elementoRaiz7.HasMember("Propriedades"));
	nPViscG=elementoRaiz7["Propriedades"].Size();
	TempViscG=new vector<double>[nPViscG];
	ViscG=new vector<double>[nPViscG];
	nTViscG= new int[nPViscG];
	for(int i=0;i<nPCondG;i++){
		nTViscG[i]=elementoRaiz7["Propriedades"][i].Size();
		PresViscG.push_back(elementoRaiz7["Propriedades"][i][0][0].GetDouble()*1.0197);
		for(int j=0; j<nTViscL[i];j++){
			TempViscG[i].push_back(elementoRaiz7["Propriedades"][i][j][1].GetDouble());
			ViscG[i].push_back(elementoRaiz7["Propriedades"][i][j][2].GetDouble()/1000.);
		}
	}



}

void ProVap::verificaFase(double pres, double var,  termprop tipo){

	if(tipo==0){
		if(pres>(220.9*1.0197) && var<374.1 ){
			BuscaBinPres(pres,PresLiqC);
			fase=0;
		}
		else if(pres>(220.9*1.0197) && var>=374.1 ){
			BuscaBinPres(pres,PresSupAq);
			fase=1;
		}
		else{
			BuscaBinPres(pres,preSat);
			double temp=tempSat[imPres]+iPres*(tempSat[imPres+1]-tempSat[imPres]);
			if(var<temp){
				BuscaBinPres(pres,PresLiqC);
				fase=0;
			}
			else if(var>temp){
				BuscaBinPres(pres,PresSupAq);
				fase=1;
			}
			else fase=2;
		}
	}
	if(tipo==1){
		if(pres>(220.9*1.0197)){
			BuscaBinPres(pres,PresSupAq);
			double uv=uvSup[imPres][0]+iPres*(uvSup[imPres+1][0]-uvSup[imPres][0]);
			if(var<uv){
				BuscaBinPres(pres,PresLiqC);
				fase=0;
			}
			else fase=1;
		}
		else{
			BuscaBinPres(pres,preSat);
			double ul=ulSat[imPres]+iPres*(ulSat[imPres+1]-ulSat[imPres]);
			double ug=uvSat[imPres]+iPres*(uvSat[imPres+1]-uvSat[imPres]);
			if(var<ul){
				BuscaBinPres(pres,PresLiqC);
				fase=0;
			}
			else if(var>ug){
				BuscaBinPres(pres,PresSupAq);
				fase=1;
			}
			else fase=2;
		}
	}
	if(tipo==2){
		if(pres>(220.9*1.0197)){
			BuscaBinPres(pres,PresSupAq);
			double hv=hvSup[imPres][0]+iPres*(hvSup[imPres+1][0]-hvSup[imPres][0]);
			if(var<hv){
				BuscaBinPres(pres,PresLiqC);
				fase=0;
			}
			else fase=1;
		}
		else{
			BuscaBinPres(pres,preSat);
			double hl=hlSat[imPres]+iPres*(hlSat[imPres+1]-hlSat[imPres]);
			double hg=hvSat[imPres]+iPres*(hvSat[imPres+1]-hvSat[imPres]);
			if(var<hl){
				BuscaBinPres(pres,PresLiqC);
				fase=0;
			}
			else if(var>hg){
				BuscaBinPres(pres,PresSupAq);
				fase=1;
			}
			else fase=2;
		}
	}

}

void ProVap::BuscaBinPres(double pres,vector<double> vec){

		   int e, d;
		   int n=vec.size();

			if(pres<vec[0]){
				cout<<"pressão fora do range";
				exit(-1);
			}
			if(pres>vec[n-1]){
				cout<<"pressão fora do range";
				exit(-1);
			}

		   e = 0;
		   d = n-1;
		   while (e <= d) {
		      imPres = (e + d)/2;
		      if (vec[imPres] <= pres && vec[imPres+1]>pres){
		    	  iPres=(pres-vec[imPres])/(vec[imPres+1]-vec[imPres]);
		    	  break;
		      }
		      if (vec[imPres] < pres) e = imPres + 1;
		      else d = imPres - 1;
		   }
		   //exit(-1);
}

void ProVap::BuscaBinVar(double var,vector<double> vec, int& im, double& raz){

		   int e, d;
		   int n=vec.size();
		   int errorInt=0;

			if(var<vec[0]){
				//cout<<"variável fora do range";
				//exit(-1);
				im=0;
				raz=0;
				errorInt=1;
			}
			if(var>vec[n-1]){
				//cout<<"variável fora do range";
				//exit(-1);
				im=n-2;
				raz=1;
				errorInt=1;
			}

		   if(errorInt==0){
			 e = 0;
		     d = n-1;
		     while (e <= d) {
		       im = (e + d)/2;
		       if (vec[im] <= var && vec[im+1]>var){
		    	  raz=(var-vec[im])/(vec[im+1]-vec[im]);
		    	  break;
		       }
		       if (vec[im] < var) e = im + 1;
		       else d = im - 1;
		     }
		     //exit(-1);
		   }
}

double ProVap::interp(double pres, double var,termprop tipo , int sai){

	double tit;
	verificaFase(pres, var, tipo);
    if(fase==0){
    	tit=0.;
	    if(tipo==0){
		  int im1;
		  double raz1;
		  BuscaBinVar(var,TempLiqC[imPres],im1, raz1);
		  int im2;
		  double raz2;
		  BuscaBinVar(var,TempLiqC[imPres+1],im2, raz2);
		  if(sai==1){
			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==2){
			  double val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
			  double val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==3){
			  double val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
			  double val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==4){
			  double val1=slComp[imPres][im1]+raz1*(slComp[imPres][im1+1]-slComp[imPres][im1]);
			  double val2=slComp[imPres+1][im2]+raz2*(slComp[imPres+1][im2+1]-slComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else return tit;

	  }
	  else if(tipo==1){
		  int im1;
		  double raz1;
		  BuscaBinVar(var,ulComp[imPres],im1, raz1);
		  int im2;
		  double raz2;
		  BuscaBinVar(var,ulComp[imPres+1],im2, raz2);
		  if(sai==0){
			  double val1=TempLiqC[imPres][im1]+raz1*(TempLiqC[imPres][im1+1]-TempLiqC[imPres][im1]);
			  double val2=TempLiqC[imPres+1][im2]+raz2*(TempLiqC[imPres+1][im2+1]-TempLiqC[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==1){
			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==3){
			  double val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
			  double val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==4){
			  double val1=slComp[imPres][im1]+raz1*(slComp[imPres][im1+1]-slComp[imPres][im1]);
			  double val2=slComp[imPres+1][im2]+raz2*(slComp[imPres+1][im2+1]-slComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else return tit;

	  }
	  else if(tipo==2){
		  int im1;
		  double raz1;
		  BuscaBinVar(var,hlComp[imPres],im1, raz1);
		  int im2;
		  double raz2;
		  BuscaBinVar(var,hlComp[imPres+1],im2, raz2);
		  if(sai==0){
			  double val1=TempLiqC[imPres][im1]+raz1*(TempLiqC[imPres][im1+1]-TempLiqC[imPres][im1]);
			  double val2=TempLiqC[imPres+1][im2]+raz2*(TempLiqC[imPres+1][im2+1]-TempLiqC[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==1){
			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==2){
			  double val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
			  double val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else if(sai==4){
			  double val1=slComp[imPres][im1]+raz1*(slComp[imPres][im1+1]-slComp[imPres][im1]);
			  double val2=slComp[imPres+1][im2]+raz2*(slComp[imPres+1][im2+1]-slComp[imPres+1][im2]);
			  return	val1+iPres*(val1-val2);
		  }
		  else return tit;

	  }
    }
    else  if(fase==1){
      tit=1.;
  	  if(tipo==0){
  		  int im1;
  		  double raz1;
  		  BuscaBinVar(var,TempSup[imPres],im1, raz1);
  		  int im2;
  		  double raz2;
  		  BuscaBinVar(var,TempSup[imPres+1],im2, raz2);
  		  if(sai==1){
  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==2){
  			  double val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
  			  double val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==3){
  			  double val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
  			  double val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==4){
  			  double val1=svSup[imPres][im1]+raz1*(svSup[imPres][im1+1]-svSup[imPres][im1]);
  			  double val2=svSup[imPres+1][im2]+raz2*(svSup[imPres+1][im2+1]-svSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		else return tit;

  	  }
  	  else if(tipo==1){
  		  int im1;
  		  double raz1;
  		  BuscaBinVar(var,uvSup[imPres],im1, raz1);
  		  int im2;
  		  double raz2;
  		  BuscaBinVar(var,uvSup[imPres+1],im2, raz2);
  		  if(sai==0){
  			  double val1=TempSup[imPres][im1]+raz1*(TempSup[imPres][im1+1]-TempSup[imPres][im1]);
  			  double val2=TempSup[imPres+1][im2]+raz2*(TempSup[imPres+1][im2+1]-TempSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==1){
  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==3){
  			  double val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
  			  double val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==4){
  			  double val1=svSup[imPres][im1]+raz1*(svSup[imPres][im1+1]-svSup[imPres][im1]);
  			  double val2=svSup[imPres+1][im2]+raz2*(svSup[imPres+1][im2+1]-svSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		else return tit;

  	  }
  	  else if(tipo==2){
  		  int im1;
  		  double raz1;
  		  BuscaBinVar(var,hvSup[imPres],im1, raz1);
  		  int im2;
  		  double raz2;
  		  BuscaBinVar(var,hvSup[imPres+1],im2, raz2);
  		  if(sai==0){
  			  double val1=TempSup[imPres][im1]+raz1*(TempSup[imPres][im1+1]-TempSup[imPres][im1]);
  			  double val2=TempSup[imPres+1][im2]+raz2*(TempSup[imPres+1][im2+1]-TempSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==1){
  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==2){
  			  double val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
  			  double val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		  else if(sai==4){
  			  double val1=svSup[imPres][im1]+raz1*(svSup[imPres][im1+1]-svSup[imPres][im1]);
  			  double val2=svSup[imPres+1][im2]+raz2*(svSup[imPres+1][im2+1]-svSup[imPres+1][im2]);
  			  return	val1+iPres*(val1-val2);
  		  }
  		else return tit;

  	  }
    }
    else  if(fase==2){
       if(tipo==0){
    	   tit=0.5;
    	   if(sai==1) return (1.-tit)*(vlSat[imPres]+iPres*(vlSat[imPres+1]-vlSat[imPres]))+
    			              tit*(vvSat[imPres]+iPres*(vvSat[imPres+1]-vvSat[imPres]));
    	   if(sai==2) return (1.-tit)*ulSat[imPres]+iPres*(ulSat[imPres+1]-ulSat[imPres])+
    			              tit*uvSat[imPres]+iPres*(uvSat[imPres+1]-uvSat[imPres]);
    	   if(sai==3) return (1.-tit)*hlSat[imPres]+iPres*(hlSat[imPres+1]-hlSat[imPres])+
    			               tit*hvSat[imPres]+iPres*(hvSat[imPres+1]-hvSat[imPres]);
    	   if(sai==4) return (1.-tit)*slSat[imPres]+iPres*(slSat[imPres+1]-slSat[imPres])+
    			                tit*svSat[imPres]+iPres*(svSat[imPres+1]-svSat[imPres]);
    	   else return tit;
       }
       if(tipo==1){
           double varL=ulSat[imPres]+iPres*(ulSat[imPres+1]-ulSat[imPres]);
           double varH=uvSat[imPres]+iPres*(uvSat[imPres+1]-uvSat[imPres]);
           tit=(var-varL)/(varH-varL);
    	   if(sai==0) return tempSat[imPres]+iPres*(tempSat[imPres+1]-tempSat[imPres]);
    	   if(sai==1) return (1.-tit)*(vlSat[imPres]+iPres*(vlSat[imPres+1]-vlSat[imPres]))+
    			              tit*(vvSat[imPres]+iPres*(vvSat[imPres+1]-vvSat[imPres]));
    	   if(sai==3) return (1.-tit)*(hlSat[imPres]+iPres*(hlSat[imPres+1]-hlSat[imPres]))+
		              tit*(hvSat[imPres]+iPres*(hvSat[imPres+1]-hvSat[imPres]));
    	   if(sai==4) return (1.-tit)*(slSat[imPres]+iPres*(slSat[imPres+1]-slSat[imPres]))+
		              tit*(svSat[imPres]+iPres*(svSat[imPres+1]-svSat[imPres]));
    	   else return tit;
       }
       if(tipo==2){
           double varL=hlSat[imPres]+iPres*(hlSat[imPres+1]-hlSat[imPres]);
           double varH=hvSat[imPres]+iPres*(hvSat[imPres+1]-hvSat[imPres]);
           tit=(var-varL)/(varH-varL);
    	   if(sai==0) return tempSat[imPres]+iPres*(tempSat[imPres+1]-tempSat[imPres]);
    	   if(sai==1) return (1.-tit)*(vlSat[imPres]+iPres*(vlSat[imPres+1]-vlSat[imPres]))+
		              tit*(vvSat[imPres]+iPres*(vvSat[imPres+1]-vvSat[imPres]));
    	   if(sai==2) return (1.-tit)*(ulSat[imPres]+iPres*(ulSat[imPres+1]-ulSat[imPres]))+
		              tit*(uvSat[imPres]+iPres*(uvSat[imPres+1]-uvSat[imPres]));
    	   if(sai==4) return (1.-tit)*(slSat[imPres]+iPres*(slSat[imPres+1]-slSat[imPres]))+
		              tit*(svSat[imPres]+iPres*(svSat[imPres+1]-svSat[imPres]));
    	   else return tit;
       }
    }
}

double ProVap::MasEspLiq(double pres, double var, termprop tipo){

	double volesp;
	verificaFase(pres, var, tipo);
	if(fase==1)return 1000.;
	else if (fase==0){

		if(tipo==0){
			  int im1;
			  double raz1;
			  BuscaBinVar(var,TempLiqC[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,TempLiqC[imPres+1],im2, raz2);

			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
			  volesp=val1+iPres*(val1-val2);

			  return 1./volesp;
		}
		else if(tipo==1){
			  int im1;
			  double raz1;
			  BuscaBinVar(var,ulComp[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,ulComp[imPres+1],im2, raz2);

			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
	  		  volesp=val1+iPres*(val1-val2);

	  		  return 1./volesp;
		}
		else{
			  int im1;
			  double raz1;
			  BuscaBinVar(var,hlComp[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,hlComp[imPres+1],im2, raz2);

			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
			  volesp=val1+iPres*(val1-val2);

			  return 1./volesp;
		}
	}
	else{
		    volesp=vlSat[imPres]+iPres*(vlSat[imPres+1]-vlSat[imPres]);
		    return 1./volesp;
	}

}

double ProVap::MasEspGas(double pres, double var, termprop tipo){

	double volesp;
	verificaFase(pres, var, tipo);
	if(fase==0)return 1.;
	else if (fase==1){

		if(tipo==0){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,TempSup[imPres+1],im2, raz2);

  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);

			  return 1./volesp;
		}
		else if(tipo==1){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,uvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,uvSup[imPres+1],im2, raz2);

 			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);

	  		  return 1./volesp;
		}
		else{
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,hvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,hvSup[imPres+1],im2, raz2);

 			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);

			  return 1./volesp;
		}
	}
	else{
		    volesp=vvSat[imPres]+iPres*(vvSat[imPres+1]-vvSat[imPres]);
		    return 1./volesp;
	}

}

double ProVap::UiLiq(double pres, double var, termprop tipo){

	verificaFase(pres, var, tipo);
	if(fase==1)return 0.;
	else if (fase==0){

		if(tipo==0){
			  int im1;
			  double raz1;
			  BuscaBinVar(var,TempLiqC[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,TempLiqC[imPres+1],im2, raz2);

			  double val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
			  double val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
			  return val1+iPres*(val1-val2);

		}
		else if(tipo==1) return var;
		else{
			  int im1;
			  double raz1;
			  BuscaBinVar(var,hlComp[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,hlComp[imPres+1],im2, raz2);

			  double val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
			  double val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
			  return val1+iPres*(val1-val2);

		}
	}
	else{
		    return ulSat[imPres]+iPres*(ulSat[imPres+1]-ulSat[imPres]);
	}

}

double ProVap::UiGas(double pres, double var, termprop tipo){

	verificaFase(pres, var, tipo);
	if(fase==0)return 0.;
	else if (fase==1){

		if(tipo==0){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,TempSup[imPres+1],im2, raz2);

  			  double val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
  			  double val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
		else if(tipo==1){
	  		  return var;
		}
		else{
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,hvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,hvSup[imPres+1],im2, raz2);

 			  double val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
  			  double val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
	}
	else{
		    return uvSat[imPres]+iPres*(uvSat[imPres+1]-uvSat[imPres]);
	}

}

double ProVap::HeLiq(double pres, double var, termprop tipo){

	verificaFase(pres, var, tipo);
	if(fase==1)return 0.;
	else if (fase==0){

		if(tipo==0){
			  int im1;
			  double raz1;
			  BuscaBinVar(var,TempLiqC[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,TempLiqC[imPres+1],im2, raz2);

			  double val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
			  double val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
			  return val1+iPres*(val1-val2);

		}
		else if(tipo==1){
			  int im1;
			  double raz1;
			  BuscaBinVar(var,ulComp[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,ulComp[imPres+1],im2, raz2);

			  double val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
			  double val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
	  		  return val1+iPres*(val1-val2);

		}
		else{
			  return var;

		}
	}
	else{
		    return hlSat[imPres]+iPres*(hlSat[imPres+1]-hlSat[imPres]);
	}

}

double ProVap::HeGas(double pres, double var, termprop tipo){

	verificaFase(pres, var, tipo);
	if(fase==0)return 0.;
	else if (fase==1){

		if(tipo==0){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,TempSup[imPres+1],im2, raz2);

  			  double val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
  			  double val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
		else if(tipo==1){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,uvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,uvSup[imPres+1],im2, raz2);

 			  double val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
  			  double val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
		else{

			  return var;
		}
	}
	else{
		   return hvSat[imPres]+iPres*(hvSat[imPres+1]-hvSat[imPres]);
	}

}

double ProVap::SLiq(double pres, double var, termprop tipo){

	verificaFase(pres, var, tipo);
	if(fase==1)return 0.;
	else if (fase==0){

		if(tipo==0){
			  int im1;
			  double raz1;
			  BuscaBinVar(var,TempLiqC[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,TempLiqC[imPres+1],im2, raz2);

			  double val1=slComp[imPres][im1]+raz1*(slComp[imPres][im1+1]-slComp[imPres][im1]);
			  double val2=slComp[imPres+1][im2]+raz2*(slComp[imPres+1][im2+1]-slComp[imPres+1][im2]);
			  return val1+iPres*(val1-val2);

		}
		else if(tipo==1){
			  int im1;
			  double raz1;
			  BuscaBinVar(var,ulComp[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,ulComp[imPres+1],im2, raz2);

			  double val1=slComp[imPres][im1]+raz1*(slComp[imPres][im1+1]-slComp[imPres][im1]);
			  double val2=slComp[imPres+1][im2]+raz2*(slComp[imPres+1][im2+1]-slComp[imPres+1][im2]);
	  		  return val1+iPres*(val1-val2);

		}
		else{
			  int im1;
			  double raz1;
			  BuscaBinVar(var,hlComp[imPres],im1, raz1);
			  int im2;
			  double raz2;
			  BuscaBinVar(var,hlComp[imPres+1],im2, raz2);

			  double val1=slComp[imPres][im1]+raz1*(slComp[imPres][im1+1]-slComp[imPres][im1]);
			  double val2=slComp[imPres+1][im2]+raz2*(slComp[imPres+1][im2+1]-slComp[imPres+1][im2]);
			  return val1+iPres*(val1-val2);

		}
	}
	else{
		    return slSat[imPres]+iPres*(slSat[imPres+1]-slSat[imPres]);
	}

}

double ProVap::SGas(double pres, double var, termprop tipo){

	verificaFase(pres, var, tipo);
	if(fase==0)return 0.;
	else if (fase==1){

		if(tipo==0){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,TempSup[imPres+1],im2, raz2);

  			  double val1=svSup[imPres][im1]+raz1*(svSup[imPres][im1+1]-svSup[imPres][im1]);
  			  double val2=svSup[imPres+1][im2]+raz2*(svSup[imPres+1][im2+1]-svSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
		else if(tipo==1){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,uvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,uvSup[imPres+1],im2, raz2);

 			  double val1=svSup[imPres][im1]+raz1*(svSup[imPres][im1+1]-svSup[imPres][im1]);
  			  double val2=svSup[imPres+1][im2]+raz2*(svSup[imPres+1][im2+1]-svSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
		else{

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,hvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,hvSup[imPres+1],im2, raz2);

			  double val1=svSup[imPres][im1]+raz1*(svSup[imPres][im1+1]-svSup[imPres][im1]);
			  double val2=svSup[imPres+1][im2]+raz2*(svSup[imPres+1][im2+1]-svSup[imPres+1][im2]);
			  return val1+iPres*(val1-val2);
		}
	}
	else{
		   return svSat[imPres]+iPres*(svSat[imPres+1]-svSat[imPres]);
	}

}

double ProVap::TempFun(double pres, double var, termprop tipo){

	verificaFase(pres, var, tipo);
	if (fase==0){
       if(tipo==0){
			return var;
		}
		else if(tipo==1){
			int im1;
			double raz1;
			BuscaBinVar(var,ulComp[imPres],im1, raz1);
			int im2;
			double raz2;
			BuscaBinVar(var,ulComp[imPres+1],im2, raz2);

			double val1=TempLiqC[imPres][im1]+raz1*(TempLiqC[imPres][im1+1]-TempLiqC[imPres][im1]);
			double val2=TempLiqC[imPres+1][im2]+raz2*(TempLiqC[imPres+1][im2+1]-TempLiqC[imPres+1][im2]);
		  	return val1+iPres*(val1-val2);

		}
		else{
			int im1;
			double raz1;
			BuscaBinVar(var,hlComp[imPres],im1, raz1);
			int im2;
			double raz2;
			BuscaBinVar(var,hlComp[imPres+1],im2, raz2);

			double val1=TempLiqC[imPres][im1]+raz1*(TempLiqC[imPres][im1+1]-TempLiqC[imPres][im1]);
			double val2=TempLiqC[imPres+1][im2]+raz2*(TempLiqC[imPres+1][im2+1]-TempLiqC[imPres+1][im2]);
			return val1+iPres*(val1-val2);

		}
	}
	else if(fase==1){
		if(tipo==0){
             return var;
		}
		else if(tipo==1){
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,uvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,uvSup[imPres+1],im2, raz2);

 			  double val1=TempSup[imPres][im1]+raz1*(TempSup[imPres][im1+1]-TempSup[imPres][im1]);
  			  double val2=TempSup[imPres+1][im2]+raz2*(TempSup[imPres+1][im2+1]-TempSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
		else{
	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(var,hvSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(var,hvSup[imPres+1],im2, raz2);

 			  double val1=TempSup[imPres][im1]+raz1*(TempSup[imPres][im1+1]-TempSup[imPres][im1]);
  			  double val2=TempSup[imPres+1][im2]+raz2*(TempSup[imPres+1][im2+1]-TempSup[imPres+1][im2]);
  			  return val1+iPres*(val1-val2);

		}
	}
	else return tempSat[imPres]+iPres*(tempSat[imPres+1]-tempSat[imPres]);

}

double ProVap::Titulo(double pres, double var, termprop tipo){
	if(fase==0)return 0.;
	else if(fase==1)return 1.;
	else{
		if(tipo==0) return 0.5;
		else if(tipo==1){
	           double varL=ulSat[imPres]+iPres*(ulSat[imPres+1]-ulSat[imPres]);
	           double varH=uvSat[imPres]+iPres*(uvSat[imPres+1]-uvSat[imPres]);
	           return (var-varL)/(varH-varL);
		}
		else if(tipo==2){
	           double varL=hlSat[imPres]+iPres*(hlSat[imPres+1]-hlSat[imPres]);
	           double varH=hvSat[imPres]+iPres*(hvSat[imPres+1]-hvSat[imPres]);
	           return (var-varL)/(varH-varL);
		}
	}
}

double ProVap::ZFunc(double pres, double temp){

	double volesp;
	double Runi=(8.20574587e-5)*1.033211;
	double MMol=18.015/1000.;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 1.;
	else if (fase==1){

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              return pres*volesp*MMol/(Runi*(temp+273.15));

	}
	else{
		    volesp=vvSat[imPres]+iPres*(vvSat[imPres+1]-vvSat[imPres]);
		    return pres*volesp*MMol/(Runi*(temp+273.15));

	}

}

double ProVap::DZDTFunc(double pres, double temp){

	double volesp;
	double Runi=(8.20574587e-5)*1.033211;
	double MMol=18.015/1000.;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=pres*volesp*MMol/(Runi*(temp+273.15));

              if(temp<MaxTSupAq[imPres]/1.01 && temp<MaxTSupAq[imPres+1]/1.01){

    	  		  BuscaBinVar(temp*1.01,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempSup[imPres+1],im2, raz2);

      			  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
      			  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=pres*volesp*MMol/(Runi*(temp*1.01+273.15));

                  return(z2-z1)/(temp*0.01);
              }
              else{

        	  	  BuscaBinVar(temp*0.99,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempSup[imPres+1],im2, raz2);

          		  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
          		  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=pres*volesp*MMol/(Runi*(temp*0.99+273.15));

                  return(z1-z2)/(temp*0.01);
              }

	}


}

double ProVap::DZDPFunc(double pres, double temp){

	double volesp;
	double Runi=(8.20574587e-5)*1.033211;
	double MMol=18.015/1000.;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=pres*volesp*MMol/(Runi*(temp+273.15));

              if(pres<PresSupAq[nPSupAq]/1.01){
            	  verificaFase(pres*1.01, temp, TEMP);

    	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

      			  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
      			  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=pres*1.01*volesp*MMol/(Runi*(temp+273.15));

                  return(z2-z1)/(pres*0.01);
              }
              else{
            	  verificaFase(pres*0.99, temp, TEMP);

        	  	  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

          		  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
          		  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=pres*0.99*volesp*MMol/(Runi*(temp+273.15));

                  return(z1-z2)/(pres*0.01);
              }

	}


}

double ProVap::DRhogDTFunc(double pres, double temp){

	double volesp;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=1./volesp;

              if(temp<MaxTSupAq[imPres]/1.01 && temp<MaxTSupAq[imPres+1]/1.01){

    	  		  BuscaBinVar(temp*1.01,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempSup[imPres+1],im2, raz2);

      			  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
      			  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z2-z1)/(temp*0.01);
              }
              else{

        	  	  BuscaBinVar(temp*0.99,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempSup[imPres+1],im2, raz2);

          		  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
          		  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z1-z2)/(temp*0.01);
              }

	}


}

double ProVap::DRhogDPFunc(double pres, double temp){

	double volesp;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=1./volesp;

              if(pres<PresSupAq[nPSupAq]/1.01){
            	  verificaFase(pres*1.01, temp, TEMP);

    	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

      			  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
      			  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z2-z1)/(pres*0.01);
              }
              else{
            	  verificaFase(pres*0.99, temp, TEMP);

        	  	  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

          		  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
          		  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z1-z2)/(pres*0.01);
              }

	}


}

double ProVap::DRholDTFunc(double pres, double temp){

	double volesp;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==1)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

  			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
  			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=1./volesp;

              if(temp<MaxTComp[imPres]/1.01 && temp<MaxTComp[imPres+1]/1.01){

    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres+1],im2, raz2);

      			  val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
      			  val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z2-z1)/(temp*0.01);
              }
              else{

        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres+1],im2, raz2);

          		  val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
          		  val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z1-z2)/(temp*0.01);
              }

	}


}

double ProVap::DRholDPFunc(double pres, double temp){

	double volesp;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==1)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

  			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
  			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=1./volesp;

              if(pres<PresLiqC[nPCompL]/1.01){
            	  verificaFase(pres*1.01, temp, TEMP);

    	  		  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
    	  		  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

      			  val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
      			  val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z2-z1)/(pres*0.01);
              }
              else{
            	  verificaFase(pres*0.99, temp, TEMP);

        	  	  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
        	  	  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

          		  val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
          		  val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return(z1-z2)/(pres*0.01);
              }

	}


}

double ProVap::CVgFunc(double pres, double temp){

	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
  			  double val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
  			  z1=val1+iPres*(val1-val2);

              if(temp<(MaxTSupAq[imPres]-0.001) && temp<(MaxTSupAq[imPres+1]-0.001)){

            	  double delp=-DRhogDTFunc(pres, temp)*(0.001)/DRhogDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp+0.001, TEMP);

    	  		  BuscaBinVar(temp+0.001,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp+0.001,TempSup[imPres+1],im2, raz2);

      			  val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
      			  val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
      			  z2=val1+iPres*(val1-val2);
                  return(z2-z1)/(0.001);
              }
              else{

            	  double delp=-DRhogDTFunc(pres, temp)*(-0.001)/DRhogDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp-0.001, TEMP);

        	  	  BuscaBinVar(temp-0.001,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp-0.001,TempSup[imPres+1],im2, raz2);

          		  val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
          		  val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
          		  z2=val1+iPres*(val1-val2);
                  return(z1-z2)/(0.001);
              }

	}


}

double ProVap::CVgFuncMod(double pres, double temp){

	  double cp=CPgFunc(pres,temp);
      double cv=CVgFunc(pres,temp);
	  double rhog=MasEspGas(pres,temp,TEMP);
	  double dzdt=DZDTFunc(pres,temp);
	  double dzdp=DZDPFunc(pres,temp);
	  double zg=ZFunc(pres,temp);
	  double dvdt=(1/rhog)*(1/(temp+273)+dzdt/zg);
	  double dvdp=(1/rhog)*(-1/(pres*98066.5)+dzdp/zg);

	  //double RG=(8.0465*1000)/(Deng*28.9625);
	  double RG=cp-cv;
	  double rel1=zg+(temp+273)*dzdt;
	  double rel2=zg-dzdp*(pres*98066.5);
	  return cv-RG*rel1*(0*1-rel1/rel2);


}

double ProVap::CVlFunc(double pres, double temp){

	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==1)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

  			  double val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
  			  double val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
  			  z1=val1+iPres*(val1-val2);

              if(temp<(MaxTComp[imPres]-0.001) && temp<(MaxTComp[imPres+1]-1.001)){



            	  double delp=-DRholDTFunc(pres, temp)*(0.001)/DRholDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp+0.001, TEMP);

    	  		  BuscaBinVar(temp+0.001,TempLiqC[imPres],im1, raz1);
    	  		  BuscaBinVar(temp+0.001,TempLiqC[imPres+1],im2, raz2);

      			  val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
      			  val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
      			  z2=val1+iPres*(val1-val2);

                  return(z2-z1)/(0.001);
              }
              else{

            	  double delp=-DRhogDTFunc(pres, temp)*(-0.001)/DRhogDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp-0.001, TEMP);

        	  	  BuscaBinVar(temp-0.001,TempLiqC[imPres],im1, raz1);
        	  	  BuscaBinVar(temp-0.001,TempLiqC[imPres+1],im2, raz2);

          		  val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
          		  val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
          		  z2=val1+iPres*(val1-val2);

                  return(z1-z2)/(0.001);
              }

	}


}

double ProVap::CPgFunc(double pres, double temp){

	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
  			  double val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
  			  z1=val1+iPres*(val1-val2);

              if(temp<MaxTSupAq[imPres]/1.01 && temp<MaxTSupAq[imPres+1]/1.01){

    	  		  BuscaBinVar(temp*1.01,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempSup[imPres+1],im2, raz2);

      			  val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
      			  val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
      			  z2=val1+iPres*(val1-val2);
                  return(z2-z1)/(temp*0.01);
              }
              else{

        	  	  BuscaBinVar(temp*0.99,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempSup[imPres+1],im2, raz2);

          		  val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
          		  val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
          		  z2=val1+iPres*(val1-val2);
                  return(z1-z2)/(temp*0.01);
              }

	}


}

double ProVap::CPgFuncMod(double pres, double temp){

	  double cp=CPgFunc(pres,temp);
      double cv=CVgFunc(pres,temp);
	  double rhog=MasEspGas(pres,temp,TEMP);
	  double dzdt=DZDTFunc(pres,temp);
	  double dzdp=DZDPFunc(pres,temp);
	  double zg=ZFunc(pres,temp);
	  double drhodp=rhog*(1/(pres*98066.5)-dzdp/zg);

	 // double RG=(8.0465*1000)/(Deng*28.9625);
	  double RG=cp-cv;
	  double rel1=zg+(temp+273)*dzdt;
	  double rel2=zg-dzdp*(pres*98066.5);
	  return (1/(rhog*zg))*rel2*(1-rel1/rel2)-(pres*98066.5/(rhog*rhog))*drhodp;


}

double ProVap::CPlFunc(double pres, double temp){

	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==1)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

  			  double val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
  			  double val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
  			  z1=val1+iPres*(val1-val2);

              if(temp<MaxTComp[imPres]/1.01 && temp<MaxTComp[imPres+1]/1.01){

    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres+1],im2, raz2);

      			  val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
      			  val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
      			  z2=val1+iPres*(val1-val2);

                  return(z2-z1)/(temp*0.01);
              }
              else{


        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres+1],im2, raz2);

          		  val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
          		  val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
          		  z2=val1+iPres*(val1-val2);

                  return(z1-z2)/(temp*0.01);
              }

	}


}

double ProVap::KAgFunc(double pres, double temp){

	double z1;
	double z2;
	double z1h;
	double z2h;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
  			  double val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
  			  z1=val1+iPres*(val1-val2);

  			  val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
  			  val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
  			  z1h=val1+iPres*(val1-val2);

              if(temp<MaxTSupAq[imPres]/1.01 && temp<MaxTSupAq[imPres+1]/1.01){


    	  		  BuscaBinVar(temp*1.01,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempSup[imPres+1],im2, raz2);

      			  val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
      			  val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
      			  z2h=val1+iPres*(val1-val2);


            	  double delp=-DRhogDTFunc(pres, temp)*(0.01*temp)/DRhogDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp*1.01, TEMP);


    	  		  BuscaBinVar(temp*1.01,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempSup[imPres+1],im2, raz2);

      			  val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
      			  val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
      			  z2=val1+iPres*(val1-val2);

                  return (z2h-z1h)/(z2-z1);
              }
              else{


        	  	  BuscaBinVar(temp*0.99,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempSup[imPres+1],im2, raz2);

          		  val1=hvSup[imPres][im1]+raz1*(hvSup[imPres][im1+1]-hvSup[imPres][im1]);
          		  val2=hvSup[imPres+1][im2]+raz2*(hvSup[imPres+1][im2+1]-hvSup[imPres+1][im2]);
          		  z2h=val1+iPres*(val1-val2);

            	  double delp=-DRhogDTFunc(pres, temp)*(-0.01*temp)/DRhogDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp*0.99, TEMP);

        	  	  BuscaBinVar(temp*0.99,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempSup[imPres+1],im2, raz2);

          		  val1=uvSup[imPres][im1]+raz1*(uvSup[imPres][im1+1]-uvSup[imPres][im1]);
          		  val2=uvSup[imPres+1][im2]+raz2*(uvSup[imPres+1][im2+1]-uvSup[imPres+1][im2]);
          		  z2=val1+iPres*(val1-val2);

                  return (z1h-z2h)/(z1-z2);
              }

	}


}

double ProVap::KAlFunc(double pres, double temp){

	double z1;
	double z2;
	double z1h;
	double z2h;
	verificaFase(pres, temp, TEMP);
	if(fase==1)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

  			  double val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
  			  double val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
  			  z1=val1+iPres*(val1-val2);

 			  val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
  			  val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
  			  z1h=val1+iPres*(val1-val2);

              if(temp<MaxTComp[imPres]/1.01 && temp<MaxTComp[imPres+1]/1.01){


    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres+1],im2, raz2);

      			  val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
      			  val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
      			  z2h=val1+iPres*(val1-val2);

            	  double delp=-DRholDTFunc(pres, temp)*(0.01*temp)/DRholDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp*1.01, TEMP);


    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres+1],im2, raz2);

      			  val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
      			  val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
      			  z2=val1+iPres*(val1-val2);

                  return (z2h-z1h)/(z2-z1);
              }
              else{

        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres+1],im2, raz2);

          		  val1=hlComp[imPres][im1]+raz1*(hlComp[imPres][im1+1]-hlComp[imPres][im1]);
          		  val2=hlComp[imPres+1][im2]+raz2*(hlComp[imPres+1][im2+1]-hlComp[imPres+1][im2]);
          		  z2h=val1+iPres*(val1-val2);

            	  double delp=-DRhogDTFunc(pres, temp)*(-0.01*temp)/DRhogDPFunc(pres, temp);
            	  verificaFase(pres+delp, temp*0.99, TEMP);

        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres+1],im2, raz2);

          		  val1=ulComp[imPres][im1]+raz1*(ulComp[imPres][im1+1]-ulComp[imPres][im1]);
          		  val2=ulComp[imPres+1][im2]+raz2*(ulComp[imPres+1][im2+1]-ulComp[imPres+1][im2]);
          		  z2=val1+iPres*(val1-val2);

                  return (z1h-z2h)/(z1-z2);
              }

	}


}

double ProVap::JTgFunc(double pres, double temp){

	double volesp;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempSup[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempSup[imPres+1],im2, raz2);

  			  double val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
  			  double val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=1./volesp;

              if(temp<MaxTSupAq[imPres]/1.01 && temp<MaxTSupAq[imPres+1]/1.01){

    	  		  BuscaBinVar(temp*1.01,TempSup[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempSup[imPres+1],im2, raz2);

      			  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
      			  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return -(1./z1)*(1.+((temp+273.15)/z1)*(z2-z1)/(temp*0.01));
              }
              else{

        	  	  BuscaBinVar(temp*0.99,TempSup[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempSup[imPres+1],im2, raz2);

          		  val1=vvSup[imPres][im1]+raz1*(vvSup[imPres][im1+1]-vvSup[imPres][im1]);
          		  val2=vvSup[imPres+1][im2]+raz2*(vvSup[imPres+1][im2+1]-vvSup[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return -(1./z1)*(1.+((temp+273.15)/z1)*(z1-z2)/(temp*0.01));
              }

	}


}

double ProVap::JTlFunc(double pres, double temp){

	double volesp;
	double z1;
	double z2;
	verificaFase(pres, temp, TEMP);
	if(fase==1)return 0.;
	else {

	  		  int im1;
	  		  double raz1;
	  		  BuscaBinVar(temp,TempLiqC[imPres],im1, raz1);
	  		  int im2;
	  		  double raz2;
	  		  BuscaBinVar(temp,TempLiqC[imPres+1],im2, raz2);

  			  double val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
  			  double val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
  			  volesp=val1+iPres*(val1-val2);
              z1=1./volesp;

              if(temp<MaxTComp[imPres]/1.01 && temp<MaxTComp[imPres+1]/1.01){

    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres],im1, raz1);
    	  		  BuscaBinVar(temp*1.01,TempLiqC[imPres+1],im2, raz2);

      			  val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
      			  val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
      			  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return -(1./z1)*(1.+((temp+273.15)/z1)*(z2-z1)/(temp*0.01));
              }
              else{

        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres],im1, raz1);
        	  	  BuscaBinVar(temp*0.99,TempLiqC[imPres+1],im2, raz2);

          		  val1=vlComp[imPres][im1]+raz1*(vlComp[imPres][im1+1]-vlComp[imPres][im1]);
          		  val2=vlComp[imPres+1][im2]+raz2*(vlComp[imPres+1][im2+1]-vlComp[imPres+1][im2]);
          		  volesp=val1+iPres*(val1-val2);
                  z2=1./volesp;

                  return -(1./z1)*(1.+((temp+273.15)/z1)*(z1-z2)/(temp*0.01));
              }

	}


}

double ProVap::TensSupFunc(double pres, double temp){

	double var;
	verificaFase(pres, temp, TEMP);
	if(fase==0)return 0.;
	else if (fase==1) return 0.;
	else{
		    var=TensSupSat[imPres]+iPres*(TensSupSat[imPres+1]-TensSupSat[imPres]);
		    return var;

	}

}

double ProVap::CondLiq(double pres, double temp){

	double var;
	verificaFase(pres, temp, TEMP);
    if (fase==1) return 0.;
	else{
		BuscaBinPres(pres,PresCondL);

		int im1;
		double raz1;
		BuscaBinVar(temp,TempCondL[imPres],im1, raz1);
		int im2;
		double raz2;
		BuscaBinVar(temp,TempCondL[imPres+1],im2, raz2);

		double val1=CondL[imPres][im1]+raz1*(CondL[imPres][im1+1]-CondL[imPres][im1]);
		double val2=CondL[imPres+1][im2]+raz2*(CondL[imPres+1][im2+1]-CondL[imPres+1][im2]);
		var=val1+iPres*(val1-val2);

		return var;

	}

}

double ProVap::CondGas(double pres, double temp){

	double var;
	verificaFase(pres, temp, TEMP);
    if (fase==0) return 0.;
	else{
		BuscaBinPres(pres,PresCondG);

		int im1;
		double raz1;
		BuscaBinVar(temp,TempCondG[imPres],im1, raz1);
		int im2;
		double raz2;
		BuscaBinVar(temp,TempCondG[imPres+1],im2, raz2);

		double val1=CondG[imPres][im1]+raz1*(CondG[imPres][im1+1]-CondG[imPres][im1]);
		double val2=CondG[imPres+1][im2]+raz2*(CondG[imPres+1][im2+1]-CondG[imPres+1][im2]);
		var=val1+iPres*(val1-val2);

		return var;

	}

}

double ProVap::ViscLiq(double pres, double temp){

	double var;
	verificaFase(pres, temp, TEMP);
    if (fase==1) return 0.;
	else{
		BuscaBinPres(pres,PresViscL);

		int im1;
		double raz1;
		BuscaBinVar(temp,TempViscL[imPres],im1, raz1);
		int im2;
		double raz2;
		BuscaBinVar(temp,TempViscL[imPres+1],im2, raz2);

		double val1=ViscL[imPres][im1]+raz1*(ViscL[imPres][im1+1]-ViscL[imPres][im1]);
		double val2=ViscL[imPres+1][im2]+raz2*(ViscL[imPres+1][im2+1]-ViscL[imPres+1][im2]);
		var=val1+iPres*(val1-val2);

		return var;

	}

}

double ProVap::ViscGas(double pres, double temp){

	double var;
	verificaFase(pres, temp, TEMP);
    if (fase==0) return 0.;
	else{
		BuscaBinPres(pres,PresViscG);

		int im1;
		double raz1;
		BuscaBinVar(temp,TempViscG[imPres],im1, raz1);
		int im2;
		double raz2;
		BuscaBinVar(temp,TempViscG[imPres+1],im2, raz2);

		double val1=ViscG[imPres][im1]+raz1*(ViscG[imPres][im1+1]-ViscG[imPres][im1]);
		double val2=ViscG[imPres+1][im2]+raz2*(ViscG[imPres+1][im2+1]-ViscG[imPres+1][im2]);
		var=val1+iPres*(val1-val2);

		return var;

	}

}

double ProVap::energmix(double pres, double temp, double alf){

	if(alf<=1e-15){
        double uinternl=UiLiq(pres,temp, TEMP);
        double rhol=MasEspLiq(pres,temp, TEMP);
        return rhol*uinternl;
	}
	else if (alf>=1-1e-15){
	    double uinterng=UiGas(pres,temp, TEMP);
	    double rhog=MasEspGas(pres,temp, TEMP);
	    return rhog*uinterng;
	}
	else{
        double uinternl=UiLiq(pres,temp, TEMP);
        double rhol=MasEspLiq(pres,temp, TEMP);
	    double uinterng=UiGas(pres,temp, TEMP);
	    double rhog=MasEspGas(pres,temp, TEMP);
	    return alf*rhog*uinterng+(1-alf)*rhol*uinternl;
	}

}

double ProVap::entalpmix(double pres, double temp,
		                                           double uls, double ugs){

	if(fabsl(ugs)<=1e-15){
        double hl=HeLiq(pres,temp, TEMP);
        double rhol=MasEspLiq(pres,temp, TEMP);
        return rhol*hl*uls;
	}
	else if (fabsl(uls)<=1-1e-15){
	    double hg=HeGas(pres,temp, TEMP);
	    double rhog=MasEspGas(pres,temp, TEMP);
	    return rhog*hg*ugs;
	}
	else{
        double hl=HeLiq(pres,temp, TEMP);
        double rhol=MasEspLiq(pres,temp, TEMP);
	    double hg=HeGas(pres,temp, TEMP);
	    double rhog=MasEspGas(pres,temp, TEMP);
	    return rhog*hg*ugs+rhol*hl*uls;
	}

}

//template class ProVap;

