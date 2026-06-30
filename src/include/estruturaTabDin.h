/*
 * estruturaTabDin.h
 *
 *  Created on: 6 de abr. de 2024
 *      Author: Eduardo
 */

#ifndef ESTRUTURATABDIN_H_
#define ESTRUTURATABDIN_H_

struct tabelaDinamica{
    int id;
    int TwoOrThree;//indicador se a tabela e bifpasica ou trifasica
    double **rholF;
    double **rhogF;
    double **DrholDpF;
    double **DrhogDpF;
    double **DrholDtF;
    double **DrhogDtF;
    double **tit;
    double **rs;
    double **cplF;
    double **cpgF;
    double **HlF;
    double **HgF;
    double **valBO;
    double **valZ;
    double **valdZdT;
    double **valdZdP;
    double **viscO;
    double **viscG;
    double *PBF;
    double *TBF;
    double pmax;
    double pmin;
    double tmax;
    double tmin;
    double delP;
    double delT;
    int npontosT;
    int npontosP;
    int celIni;
    int celFim;
    tabelaDinamica(){
        id=0;
        TwoOrThree=-1;//indicador se a tabela e bifpasica ou trifasica
        rholF=0;
        rhogF=0;
        DrholDpF=0;
        DrhogDpF=0;
        DrholDtF=0;
        DrhogDtF=0;
        tit=0;
        rs=0;
        cplF=0;
        cpgF=0;
        HlF=0;
        HgF=0;
        valBO=0;
        valZ=0;
        valdZdT=0;
        valdZdP=0;
        viscO=0;
        viscG=0;
        PBF=0;
        TBF=0;
        pmax=0.;
        pmin=0.;
        tmax=0.;
        tmin=0.;
        delP=0.;
        delT=0.;
        npontosT=0;
        npontosP=0;
        celIni=0;
        celFim=0;
    }
    ~tabelaDinamica(){
		for(int j=0; j<npontosP+1;j++){
			if(rhogF!=0 && npontosP!=0)delete[] rhogF[j];
			if(rholF!=0 && npontosP!=0)delete[] rholF[j];
			if(DrhogDpF!=0 && npontosP!=0)delete[] DrhogDpF[j];
			if(DrhogDtF!=0 && npontosP!=0)delete[] DrhogDtF[j];
			if(DrholDpF!=0 && npontosP!=0)delete[] DrholDpF[j];
			if(DrholDtF!=0)delete[] DrholDtF[j];
			if(valBO!=0 && npontosP!=0)delete[] valBO[j];
			if(HgF!=0 && npontosP!=0)delete[] HgF[j];
			if(HlF!=0 && npontosP!=0)delete[] HlF[j];
			if(cpgF!=0 && npontosP!=0)delete[] cpgF[j];
			if(cplF!=0 && npontosP!=0)delete[] cplF[j];
			if(valZ!=0 && npontosP!=0)delete[] valZ[j];
			if(valdZdT!=0 && npontosP!=0)delete[] valdZdT[j];
			if(valdZdP!=0 && npontosP!=0)delete[] valdZdP[j];
			if(tit!=0 && npontosP!=0)delete[] tit[j];
			if(rs!=0 && npontosP!=0)delete[] rs[j];
			if(viscG!=0 && npontosP!=0)delete[] viscG[j];
			if(viscO!=0 && npontosP!=0)delete[] viscO[j];
		}
		if(rhogF!=0 && npontosP!=0)delete [] rhogF;
		if(rholF!=0 && npontosP!=0)delete[] rholF;
		if(DrhogDpF!=0 && npontosP!=0)delete[] DrhogDpF;
		if(DrhogDtF!=0 && npontosP!=0)delete[] DrhogDtF;
		if(DrholDpF!=0 && npontosP!=0)delete[] DrholDpF;
		if(DrholDtF!=0 && npontosP!=0)delete[] DrholDtF;
		if(valBO!=0 && npontosP!=0)delete[] valBO;
		if(HgF!=0 && npontosP!=0)delete[] HgF;
		if(HlF!=0 && npontosP!=0)delete[] HlF;
		if(cpgF!=0 && npontosP!=0)delete[] cpgF;
		if(cplF!=0 && npontosP!=0)delete[] cplF;
		if(valZ!=0 && npontosP!=0)delete[] valZ;
		if(valdZdT!=0 && npontosP!=0)delete[] valdZdT;
		if(valdZdP!=0 && npontosP!=0)delete[] valdZdP;
		if(tit!=0 && npontosP!=0)delete[] tit;
		if(rs!=0 && npontosP!=0)delete[] rs;
		if(viscG!=0 && npontosP!=0)delete[] viscG;
		if(viscO!=0 && npontosP!=0)delete[] viscO;
		if(TBF!=0 && npontosP!=0)delete [] TBF;
		if(PBF!=0 && npontosP!=0)delete [] PBF;
		npontosP=0;
    }
};



#endif /* ESTRUTURATABDIN_H_ */
