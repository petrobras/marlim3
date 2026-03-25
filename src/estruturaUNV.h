/*
 * structUNV.h
 *
 *  Created on: 9 de set. de 2024
 *      Author: Eduardo
 */

#ifndef ESTRUTURAUNV_H_
#define ESTRUTURAUNV_H_

#include <string>
using namespace std;
#include <string.h>

struct cartesiano{
	int ind;
	double x[3];
	int tipo;
	int vertice;
	string condCont;
	cartesiano(){
		ind=-1;
		for(int i=0; i<3; i++)x[i]=0.;
		tipo=-1;
		vertice=-1;
		condCont="lixo";
	}
	cartesiano(const cartesiano& vtemp){
		ind=vtemp.ind;
		for(int i=0; i<3; i++)x[i]=vtemp.x[i];
		tipo=vtemp.tipo;
		vertice=vtemp.vertice;
		condCont=vtemp.condCont;
	}
	cartesiano& operator=(cartesiano& vtemp){
		if (this != &vtemp) {
			ind=vtemp.ind;
			for(int i=0; i<3; i++)x[i]=vtemp.x[i];
			tipo=vtemp.tipo;
			vertice=vtemp.vertice;
			condCont=vtemp.condCont;
		}
		return *this;
	}
};
struct cordaNo{
	int ind;
	int no[2];
	//cartesiano objNo[2];
	cordaNo(){
		ind=-1;
		for(int i=0; i<2; i++)no[i]=0;
	}
	cordaNo(const cordaNo& vtemp){
		ind=vtemp.ind;
		for(int i=0; i<2; i++)no[i]=vtemp.no[i];
	}
	cordaNo& operator=(cordaNo& vtemp){
		if (this != &vtemp) {
			ind=vtemp.ind;
			for(int i=0; i<2; i++)no[i]=vtemp.no[i];
		}
		return *this;
	}
};
struct faceNo{
	int ind;
	int no[3];
	//cordaNo objCorda[3];
	//int indCorda[3];
	int tipo;
	string condCont;
	string nomeFace;
	faceNo(){
		ind=-1;
		tipo=0;
		condCont="interno";
		nomeFace="vazio";
		for(int i=0; i<3; i++){
			no[i]=0;
			//indCorda[i]=-1;
		}
	}
	faceNo(const faceNo& vtemp){
		ind=vtemp.ind;
		tipo=vtemp.tipo;
		condCont=vtemp.condCont;
		nomeFace=vtemp.nomeFace;
		for(int i=0; i<3; i++){
			no[i]=vtemp.no[i];
			//indCorda[i]=vtemp.indCorda[i];
		}
	}
	faceNo& operator=(const faceNo& vtemp){
		if (this != &vtemp) {
			ind=vtemp.ind;
			tipo=vtemp.tipo;
			condCont=vtemp.condCont;
			nomeFace=vtemp.nomeFace;
			for(int i=0; i<3; i++){
				no[i]=vtemp.no[i];
				//indCorda[i]=vtemp.indCorda[i];
			}
		}
		return *this;
	}

	faceNo& operator=(faceNo&& vtemp) noexcept {
			if (this != &vtemp) {
				ind=vtemp.ind;
				tipo=vtemp.tipo;
				//condCont=vtemp.condCont;
				//nomeFace=vtemp.nomeFace;
				condCont = std::move(vtemp.condCont);
				nomeFace = std::move(vtemp.nomeFace);
				for(int i=0; i<3; i++){
					no[i]=vtemp.no[i];
					//indCorda[i]=vtemp.indCorda[i];
				}
			}
			return *this;
		}
};
struct eleNo{
	int ind;
	int no[4];
	//faceNo objFace[4];
	int indFace[4];
	string nomeFace[4];
	int indFaceNo[4][3];
	string regiao;
	eleNo(){
		ind=-1;
		regiao="unica";
		for(int i=0; i<4; i++){
			no[i]=0;
			indFace[i]=-1;
			nomeFace[i]="vazio";
			for(int j=0;j<3;j++)indFaceNo[i][j]=-1;
		}
	}
	eleNo(const eleNo& vtemp){
		ind=vtemp.ind;
		regiao=vtemp.regiao;
		for(int i=0; i<4; i++){
			no[i]=vtemp.no[i];
			indFace[i]=vtemp.indFace[i];
			nomeFace[i]=vtemp.nomeFace[i];
			for(int j=0;j<3;j++)indFaceNo[i][j]=vtemp.indFaceNo[i][j];
		}
	}
	eleNo& operator=(const eleNo& vtemp){
		if (this != &vtemp) {
			ind=vtemp.ind;
			regiao=vtemp.regiao;
			for(int i=0; i<4; i++){
				no[i]=vtemp.no[i];
				indFace[i]=vtemp.indFace[i];
				nomeFace[i]=vtemp.nomeFace[i];
				for(int j=0;j<3;j++)indFaceNo[i][j]=vtemp.indFaceNo[i][j];
			}
		}
		return *this;
	}

	eleNo& operator=(eleNo&& vtemp) noexcept {
			if (this != &vtemp) {
				ind = vtemp.ind;
				regiao = std::move(vtemp.regiao);
				for (int i = 0; i < 4; i++) {
					no[i] = vtemp.no[i];
					indFace[i] = vtemp.indFace[i];
					nomeFace[i] = std::move(vtemp.nomeFace[i]);
					for (int j = 0; j < 3; j++) {
						indFaceNo[i][j] = vtemp.indFaceNo[i][j];
					}
				}
			}
			return *this;
		}
};


struct cordaOBJ{
	int ind;
	int no[2];
	cartesiano* objNo;

	cordaOBJ(){
		ind=-1;
		for(int i=0; i<2; i++){
			no[i]=0.;
		}
		//objNo=0;
		objNo=new cartesiano [2];
	}
	cordaOBJ(const cordaOBJ& vtemp){
		ind=vtemp.ind;
		objNo=new cartesiano [2];
		for(int i=0; i<2; i++){
			no[i]=vtemp.no[i];
			objNo[i]=vtemp.objNo[i];
		}
	}
	cordaOBJ& operator=(cordaOBJ& vtemp){
		if (this != &vtemp) {
		   	if(objNo!=0){
		    	delete [] objNo;
		   	}
			ind=vtemp.ind;
		   	objNo=new cartesiano [2];
			for(int i=0; i<2; i++){
				no[i]=vtemp.no[i];
				objNo[i]=vtemp.objNo[i];
			}
		}
		return *this;
	}
	~cordaOBJ(){
	   	if(objNo!=0){
	    	delete [] objNo;
	   	}
	 }
};
struct faceOBJ{
	int ind;
	int tipo;
	string condCont;
	int no[3];
	//int indCorda[3];
	//cordaOBJ* objCorda;
	cartesiano* objNo;
	faceOBJ(){
		ind=-1;
		tipo=0;
		condCont="interno";
		for(int i=0; i<3; i++){
			no[i]=0.;
			//indCorda[i]=0.;
		}
		//objCorda=0;
		//objNo=0;
		objNo=new cartesiano [3];
	}
	faceOBJ(const faceOBJ& vtemp){
		ind=vtemp.ind;
		tipo=vtemp.tipo;
		condCont=vtemp.condCont;
		//objCorda=new cordaOBJ [3];
		objNo=new cartesiano [3];
		for(int i=0; i<3; i++){
			no[i]=vtemp.no[i];
			//indCorda[i]=vtemp.indCorda[i];
			//objCorda[i]=vtemp.objCorda[i];
			objNo[i]=vtemp.objNo[i];
		}
	}
	faceOBJ& operator=(const faceOBJ& vtemp){
		if (this != &vtemp) {
		   	//if(objCorda!=0)	delete [] objCorda;
		   	if(objNo!=0) delete [] objNo;
			ind=vtemp.ind;
			tipo=vtemp.tipo;
			condCont=vtemp.condCont;
			//objCorda=new cordaOBJ [3];
			objNo=new cartesiano [3];
			for(int i=0; i<3; i++){
				no[i]=vtemp.no[i];
				//indCorda[i]=vtemp.indCorda[i];
				//objCorda[i]=vtemp.objCorda[i];
				objNo[i]=vtemp.objNo[i];
			}
		}
		return *this;
	}
	~faceOBJ(){
	   	//if(objCorda!=0)	delete [] objCorda;
	   	if(objNo!=0) delete [] objNo;
	 }
};
struct eleOBJ{
	int ind;
	int noGlobal[4];
	int indFaceGlobal[4];
	int indViz[4];
	int indFaceNo[4][3];
	int faceCCfaceReal[4];
	double RAspec;
	string nomeFace[4];
	faceNo* objFace;
	cartesiano* objNo;
	string regiao;
	eleOBJ(){
		ind=-1;
		regiao="unica";
		RAspec=1;
		for(int i=0; i<4; i++){
			noGlobal[i]=0.;
			indFaceGlobal[i]=-1;
			indViz[i]=-1;
			faceCCfaceReal[i]=-1;
			nomeFace[i]="vazio";
			for(int j=0;j<3;j++)indFaceNo[i][j]=-1;
		}
		//objNo=0;
		//objFace=0;
		objNo=new cartesiano [4];
		objFace=new faceNo [4];
	}
	eleOBJ(const eleOBJ& vtemp){
		ind=vtemp.ind;
		objNo=new cartesiano [4];
		objFace=new faceNo [4];
		regiao=vtemp.regiao;
		RAspec=vtemp.RAspec;
		for(int i=0; i<4; i++){
			noGlobal[i]=vtemp.noGlobal[i];
			indFaceGlobal[i]=vtemp.indFaceGlobal[i];
			objNo[i]=vtemp.objNo[i];
			objFace[i]=vtemp.objFace[i];
			indViz[i]=vtemp.indViz[i];
			faceCCfaceReal[i]=vtemp.faceCCfaceReal[i];
			nomeFace[i]=vtemp.nomeFace[i];
			for(int j=0;j<3;j++)indFaceNo[i][j]=vtemp.indFaceNo[i][j];
		}
	}
	eleOBJ& operator=(const eleOBJ& vtemp){
		if (this != &vtemp) {
		   	if(objNo!=0) delete [] objNo;
		   	if(objFace!=0) delete [] objFace;
			ind=vtemp.ind;
			objNo=new cartesiano [4];
			objFace=new faceNo [4];
			regiao=vtemp.regiao;
			RAspec=vtemp.RAspec;
			for(int i=0; i<4; i++){
				noGlobal[i]=vtemp.noGlobal[i];
				indFaceGlobal[i]=vtemp.indFaceGlobal[i];
				objNo[i]=vtemp.objNo[i];
				objFace[i]=vtemp.objFace[i];
				indViz[i]=vtemp.indViz[i];
				faceCCfaceReal[i]=vtemp.faceCCfaceReal[i];
				nomeFace[i]=vtemp.nomeFace[i];
				for(int j=0;j<3;j++)indFaceNo[i][j]=vtemp.indFaceNo[i][j];
			}
		}
		return *this;
	}
	~eleOBJ(){
	   	if(objNo!=0) delete [] objNo;
	   	if(objFace!=0) delete [] objFace;
	 }
};



#endif /* ESTRUTURAUNV_H_ */
