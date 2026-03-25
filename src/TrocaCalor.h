/*
 * TrocaCalor.h
 *
 *  Created on: 24/04/2014
 *      Author: bwz1
 */

#ifndef TROCACALOR_H_
#define TROCACALOR_H_

#include "Vetor.h"
//#include "Vetor.cpp"
#include "Matriz.h"
//#include "Matriz.cpp"
#include "Geometria.h"

#include "dados1Poisson.h"
#include "solverPoisson.h"
#include "variaveisGlobais1D.h"

//extern double lixo5;

constexpr double CoefGopalHT[] = { 1.6643,-2.2114,-0.3647, 1.4385, 0.5220,-0.8511,
                              -0.0364, 1.0490, 0.1391,-0.2988, 0.0007, 0.9969,
                              0.0295,-0.0825, 0.0009, 0.9967,-1.3570, 1.4942,
                              4.6315,-4.7009, 0.1717,-0.3232, 0.5869, 0.1229,
                              0.0984,-0.2053, 0.0621, 0.8580, 0.0211,-0.0527,
                              0.0127, 0.9549,-0.3278, 0.4752, 1.8223,-1.9036,
                              -0.2521, 0.3871, 1.6087,-1.6635,-0.0284, 0.0625,
                              0.4714,-0.0011, 0.0041, 0.0039, 0.0607, 0.7927};

class TransCal{
	public:
	DadosGeo geom;
	double airMW;// alteracao0
	double RGas;// alteracao0
	double pressao;//alteracao2
	double TCNitro;//alteracao2
	double PCNitro;//alteracao2
	int lenth;
	int permanente;
	int* ncamada;
	int nglobal;
	double* drcamada;
	double** Tcamada;
	double** Qcamada;
	double** Tini;
	double** Qini;
	double Tint;
	double Tint2;
	double Textern1;
	double Textern2;
	double Vint;
	double Vextern1;
	double Vconf;
	int dirconvExt;
	int coluna;
	double colunaDia;
	double dt;
	double kint;
	double cpint;
	double rhoint;
	double viscint;
	double kextern1;
    double cpextern1;
	double rhoextern1;
	double viscextern1;
	double kextern2;
	double cpextern2;
    double rhoextern2;
	double viscextern2;
	double betext;
	double betint;
	double fluxIni;
	double fluxFim;
	double resGlob;
	Vcr<double> localvet;
	FullMtx<double> localmat;
	double dimExt;

	double ccon;
	double ncon;
	double mcon;
	double npet;

	int formacPoc;
	int ambext;// alteracao0
	double tempprod;
	double condform;
	double cpform;
	double rhoform;
	double resFim;

	double reyi;
	double reye;
	double grashi;
	double grashe;
	double nusi;
	double nuse;
	double hi;
	double he;
	double pri;
	double pre;
	double prG;
	double prL;

	double dtL;
	double dx;

	int condiTparede;
	double novoHi;

	int difus2D;
	//enterramento
	solverP poisson2D;
	int indCel;

	varGlob1D* vg1dSP;


	TransCal(varGlob1D* Vvg1dSP=0,const DadosGeo vgeom=DadosGeo(),const int vpermanente=-1, const int* const vncamada=0,
			const double* const vdrcamada=0,double** const vTcamada=0,const double vTint=0.,
			const double vTextern1=0., const double vTextern2=0.,const double vVint=0.,
			const double vVextern1=0.,
			const int vdirconvExt=0, const double vdt=0,const double vkint=0,
			const double vcpint=0,
			const double vrhoint=0,const double vviscint=0,const double vkextern1=0,
			const double vcpextern1=0,
			const double vrhoextern1=0,const double vviscextern1=0,const int vformac=0,
			const int vambext=0,
			const double vtempprod=-100,
			const double vcondform=0, const double vcpform=0, const double vrhoform=0
			,const double pres=0, const double vdx=1,const double vkextern2=0,
			const double vcpextern2=0,
			const double vrhoextern2=0,const double vviscextern2=0,const int vcoluna=0,
			const double vcolunaDia=0);//alteracao2

	TransCal(const TransCal&); //construtor por c�pia
	TransCal& operator=(const TransCal&);
	 ~TransCal(){
	        	if(ncamada) delete[] ncamada;
	        	if(drcamada) delete[] drcamada;
	        	if(lenth>0){
	        	  for(int i=0; i<lenth;i++){
	        		  delete[] Tcamada[i];
	        		  delete[] Tini[i];
	        	  }
	        	  delete [] Tcamada;
	        	  delete [] Tini;
	        	  for(int i=0; i<lenth;i++){
	        		  delete[] Qcamada[i];
	        		  delete[] Qini[i];
	        	  }
	        	  delete [] Qcamada;
	        	  delete [] Qini;
	        	}
	        	/*if(lenth>0){
	        	  for(int i=0; i<lenth;i++) delete[] Tcamadaini[i];
	        	  delete [] Tcamadaini;
	        	  for(int i=0; i<lenth;i++) delete[] Qcamadaini[i];
	        	  delete [] Qcamadaini;
	        	}*/

	        } //destrutor


  void atualiza(DadosGeo vgeom=DadosGeo(), int discre=1);
  void atualiza2(DadosGeo vgeom=DadosGeo());
  double Grash(double dteta,double beta, double cinVisc, double del);
  double Pr(double cinVisc,double difus);
  double Ra(double gra, double pran);
  double RaInt(double dteta,double beta, double cinVisc,double difus);
  double RaExt(double dteta,double beta, double cinVisc,double difus);
  double NussConf(double Ra, double heigth, double delD);
  double NussConf2(double Ra, double heigth, double delD, double teta);
  double NussConf3(double pr,double Ra, double heigth, double delD);
  double ReyIn(double vel,double visc, double rho, double dia);
  double fric(double vrey, double eps);
  double nussPet(double vrey,double pr,double eps, double viscb, double viscw);
  double nussChuBer(double vrey, double pr);
  double nussNatExt(double ra, double pr);
  void defineConf(double pr, double ra);
  void definePet(double tb,double tw);
  double ResForm();
  double MasEspLiq(double temp) const; // alteracao0
  double VisLiq(double temp) const; // alteracao0
  double CalorLiq(double temp) const; // alteracao0
  double CondLiq(double temp) const; // alteracao0
  double MasEspAr(double temp) const; // alteracao0
  double VisAr(double temp) const; // alteracao0
  double CalorAr(double temp) const; // alteracao0
  double CondAr(double temp) const; // alteracao0

  double ZGopal(double PR, double TR) const;//alteracao2
  double MasEspGas(double temp) const;//alteracao2
  double CalorGas(double temp) const;//alteracao2
  double CondGas(double temp)const;//alteracao2
  double ViscGas(double temp) const;//alteracao2

  double beta(double temp, int tipo) const;

  double hExt(double resanul=0, double diaRef=-1);
  double hInt(double resanul=0, double diaRef=-1);
  double condParede(double resanul=0);
  double condParedeLocal(double resanul=0);
  double transperm(double resanul=0);
  void transcel(int icam, int idisc);
  double transtrans();
  double transperm2D();//enterramento
  double transtrans2D();//enterramento
  void FeiticoDoTempo();
  FullMtx<double> perfil();
  double psia(const double p)const { return (p*0.9678411)*14.69595;} //p->kgf, transforma kgf para psia // alteracao0
  double Faren(const double t)const { return 1.8*t + 32;} //t em Celcius, transforma Celcius para Farenheit // alteracao0
  double cel(const double t)const {return (t - 32)/1.8;} //t em Farenheit, transforma Farenheit para Celcius //alteracao2
  double kgf(const double p)const { return p/(14.69595*0.9678411);}//p em psia, transforma psia para kgf/cm2 //alteracao2

};


#endif /* TROCACALOR_H_ */
