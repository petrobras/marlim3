#include "FonteMas.h"

/*InjLiq& InjLiq::operator =(const InjLiq& vinj){
  QLiq=vinj.QLiq;
  this->FluidoPro=vinj.FluidoPro;
  return *this;
}*/

IPR::IPR(double vpres, double vtres, double vip,double vij, double vqm,ProFlu vfluido, int tipoip){
 Pres=vpres;
 Tres=vtres;
 ip=vip;
 qMax=vqm;
 ij=vij;
 deriP=0.;
 deriG=0.;
 deriC=0.;
 tipoIPR=tipoip;
 this->FluidoPro=vfluido;
 Psat=vfluido.PB(Pres, Tres)/(0.9678411*14.69595);
}

IPR::IPR(double vpres, double vtres, double vip,double vij, double vqm,int tipoip){
 Pres=vpres;
 Tres=vtres;
 ip=vip;
 qMax=vqm;
 ij=vij;
 Psat=0;
 deriP=0.;
 deriG=0.;
 deriC=0.;
 tipoIPR=tipoip;
 this->FluidoPro=ProFlu();
 Psat=0.;
}

IPR::IPR(const IPR& vipr){//construtor de c�pia
 Pres=vipr.Pres;
 Tres=vipr.Tres;
 ip=vipr.ip;
 qMax=vipr.qMax;
 ij=vipr.ij;
 deriP=vipr.deriP;
 deriG=vipr.deriG;
 deriC=vipr.deriC;
 tipoIPR=vipr.tipoIPR;
 this->FluidoPro=vipr.FluidoPro;
 Psat=vipr.Psat;
}

IPR& IPR::operator =(const IPR& vipr){
//if (this != &vipr){
	Pres=vipr.Pres;
	Tres=vipr.Tres;
	ip=vipr.ip;
	qMax=vipr.qMax;
	ij=vipr.ij;
	deriP=vipr.deriP;
	deriG=vipr.deriG;
	deriC=vipr.deriC;
	tipoIPR=vipr.tipoIPR;
	this->FluidoPro=vipr.FluidoPro;
	Psat=vipr.Psat;
//}
 return *this;
}

double IPR::VMas(const double& pfundo, const double& tfundo){
//retorna a vaz�o m�ssica para uma dada press�o e temperatura de fundo. kg/s
//Determina o valor da vaz�o m�ssica produzida por um reservat�rio em kg/s
//para uma determinada press�o de fundo (kgf/cm2), o ip � dado em m3/d
//kgf/cm2, a IPR � obtida por Voguel na condi��o de saturado e linear no
//subsaturado, a press�o de reservat�rio em kgf/
//cm2, a temperatura de reservat�rio em C, observe que na IPR � adicionada
//a possibilidade que o po�o beba, neste caso, poder� ocorrer escoamento
//descendente

 double var=0.;
 double multiplicador=0.;
 if(tipoIPR==1){
	 Psat=this->FluidoPro.PB(Pres, Tres)/(0.9678411*14.69595);
	 //Psat=0.;
	 double Psat2=Psat;
	 if(Psat>Pres) Psat2=Pres;
	 double qsat = ip*(Pres-Psat2);
	 if(qsat<0){
		 qsat=0;
		 Psat2=Pres;
	 }
	 double qmax = ip/(1.8/Psat2) + qsat;

	 double Rhogs;
	 double Rhols;
	 double bsw=0*this->FluidoPro.BSW;

	 if(pfundo <= Pres){
		 if(pfundo > Psat2){
			 var = ip*(Pres - pfundo);
			 deriG=deriP=-ip;
			 deriC=0.;
		 }
		 else{ var = (1-bsw)*((qmax - qsat)*(1 - 0.2*(pfundo/Psat2)-
				 0.8*(pfundo/Psat2)*(pfundo/Psat2))+qsat)+bsw*ip*(Pres-pfundo);
         	 deriG=deriP=-(1-bsw)*(qmax - qsat)*(0.2/Psat2+1.6*pfundo/(Psat2*Psat2))-
        		      bsw*ip;
         	 /*var = ((qmax - qsat)*(1 - 0.2*(pfundo/Psat2)-
	                0.8*(pfundo/Psat2)*(pfundo/Psat2))+qsat);
	           deriG=deriP=-(qmax - qsat)*(0.2/Psat2+1.6*pfundo/(Psat2*Psat2));*/
         	 deriC=0.;
		 }
		 Rhogs = this->FluidoPro.Deng*1.225;//this->FluidoPro.MasEspGas(1, 15);
		 Rhols = ((1000*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+
				 1000.*this->FluidoPro[5]*this->FluidoPro[4]);
		 multiplicador =(Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400;
	 }
	 else{
		 var = ij*(Pres - pfundo);
		 deriC=deriG=deriP=-ij;
		 Rhols = (1000*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+
				 1000.*this->FluidoPro[5]*this->FluidoPro[4];
		 // multiplicador =Rhols/86400;
		 //multiplicador =(Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400;
		 multiplicador =1./86400;
	 }
 }
 else if(tipoIPR==0){
	 double Rhogs;
	 double Rhols;
	 if(pfundo <= Pres){
		 var = ip*(Pres - pfundo);
		 deriG=deriP=-ip;
		 deriC=0.;
		 Rhogs = this->FluidoPro.Deng*1.225;//this->FluidoPro.MasEspGas(1, 15);
		 Rhols = ((1000*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+
				 1000.*this->FluidoPro[5]*this->FluidoPro[4]);
		 multiplicador =(Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400;
	 }
	 else{
		 var = ij*(Pres - pfundo);
		 deriC=deriG=deriP=-ij;
		 Rhols = (1000*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+
				 1000.*this->FluidoPro[5]*this->FluidoPro[4];
		 // multiplicador =Rhols/86400;
		 //multiplicador =(Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400;
		 multiplicador =1./86400;
	 }
 }
 else if(tipoIPR==2){
	 double Rhogs;
	 double Rhols;
	 var = qMax*(1 - 0.2*(pfundo/Pres)-0.8*(pfundo/Pres)*(pfundo/Pres));
     deriG=deriP=-qMax*(0.2/Pres+1.6*pfundo/(Pres*Pres));
     deriC=0.;
	 Rhogs = this->FluidoPro.Deng*1.225;//this->FluidoPro.MasEspGas(1, 15);
	 Rhols = ((1000*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+
				 1000.*this->FluidoPro[5]*this->FluidoPro[4]);
	 multiplicador =(Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400;
 }

 deriG=deriP=multiplicador*deriP;
 deriC*=multiplicador;
 return multiplicador*var;
}

double IPR::MasL(const double& pfundo, const double& tfundo){
//retorna a vaz�o m�ssica de l�quido para uma dada press�o e temperatura de fundo. kg/s
//Determina o valor da vaz�o m�ssica produzida por um reservat�rio em kg/s
//para uma determinada press�o de fundo (kgf/cm2), o ip � dado em m3/d
//kgf/cm2, a IPR � obtida por Voguel na condi��o de saturado e linear no
//subsaturado, a press�o de reservat�rio em kgf/
//cm2, a temperatura de reservat�rio em C, observe que na IPR � adicionada
//a possibilidade que o po�o beba, neste caso, poder� ocorrer escoamento
//descendente

 /*double var;
 double Psat2=Psat;
 if(Psat>Pres)
 Psat2=Pres;
 double qsat = ip*(Pres-Psat2);
 if(qsat<0){
   qsat=0;
   Psat2=Pres;
 }
 double qmax = ip/(1.8/Psat2) + qsat;
 if(pfundo <= Pres){
   if(pfundo > Psat2) var = ip*(Pres - pfundo);
   else var = (qmax - qsat)*(1 - 0.2*(pfundo/Psat2)-0.8*(pfundo/Psat2)*(pfundo/Psat2))+qsat;
 }
 else var = ij*(Pres - pfundo);
  double Rhols = (1000*141/(131 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+1000.*this->FluidoPro[5]*this->FluidoPro[4];
 double multiplicador =(Rhols )/86400;
 return multiplicador*var;*/
	////////////////////ATENCAO////////////////////////////////////////////////////
	////////Parece que esta errado/////////////////////////////////////////////////
	///////////Se esta errando no calculo das derivadas da vazao massica das fases em relacao a pressao
	///////////o correto deveria ser calcular a vazao massica total primeiro para depois calcular a derivada:

	double vazmass= VMas(pfundo,tfundo);
    deriP*=(1.-this->FluidoPro.FracMassHidra(pfundo,tfundo));
    deriC=0;
	return (1.-this->FluidoPro.FracMassHidra(pfundo,tfundo))*vazmass;

    /*deriP*=(1.-this->FluidoPro.FracMassHidra(pfundo,tfundo));
    deriC=0;
	return (1.-this->FluidoPro.FracMassHidra(pfundo,tfundo))*VMas(pfundo,tfundo);*/
}

double IPR::MasG(const double& pfundo, const double& tfundo){
//retorna a vaz�o m�ssica de l�quido para uma dada press�o e temperatura de fundo. kg/s
//Determina o valor da vaz�o m�ssica produzida por um reservat�rio em kg/s
//para uma determinada press�o de fundo (kgf/cm2), o ip � dado em m3/d
//kgf/cm2, a IPR � obtida por Voguel na condi��o de saturado e linear no
//subsaturado, a press�o de reservat�rio em kgf/
//cm2, a temperatura de reservat�rio em C, observe que na IPR � adicionada
//a possibilidade que o po�o beba, neste caso, poder� ocorrer escoamento
//descendente

 /*double var;
 double Psat2=Psat;
 double qsat = ip*(Pres-Psat2);
 if(qsat<0){
   qsat=0;
   Psat2=Pres;
 }
 double qmax = ip/(1.8/Psat2) + qsat;
 if(pfundo <= Pres){
   if(pfundo > Psat2) var = ip*(Pres - pfundo);
   else var = (qmax - qsat)*(1 - 0.2*(pfundo/Psat2)-0.8*(pfundo/Psat2)*(pfundo/Psat2))+qsat;
 }
 else var =ij*(Pres - pfundo);
 double Rhogs = this->FluidoPro.MasEspGas(1, 15);
 double multiplicador =(this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400;
 return multiplicador*var;*/
	////////////////////ATENCAO////////////////////////////////////////////////////
	////////Parece que esta errado/////////////////////////////////////////////////
	///////////Se esta errando no calculo das derivadas da vazao massica das fases em relacao a pressao
	///////////o correto deveria ser calcular a vazao massica total primeiro para depois calcular a derivada:

	double vazmass= VMas(pfundo,tfundo);
	deriG*=this->FluidoPro.FracMassHidra(pfundo,tfundo);
	return this->FluidoPro.FracMassHidra(pfundo,tfundo)*vazmass;

	/*deriG*=this->FluidoPro.FracMassHidra(pfundo,tfundo);
	return this->FluidoPro.FracMassHidra(pfundo,tfundo)*VMas(pfundo,tfundo);*/
}

/*double* IPR::contorno(const double& pfundo, const double& tfundo){//calcula os coeficientes referentes � condi��o de contorno caso esta venha a ser uma IPR
  double* var;
  Psat=this->FluidoPro.PB(Pres, Tres)/(0.9678411*14.69595);
  //Psat=0.;
  double Psat2=Psat;
  var = new double [2];
  double qsat = ip*(Pres-Psat2);
  if(qsat<0){
   qsat=0;
   Psat2=Pres;
  }
  double qmax = ip/(1.8/Psat2) + qsat;
  double Rhogs = this->FluidoPro.Deng*1.225;//this->FluidoPro.MasEspGas(1, 15);
  double Rhols = (1000*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+1000.*this->FluidoPro[5]*this->FluidoPro[4];
  double multiplicador =(Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400;
  if(pfundo <= Pres){
    if(pfundo > Psat2){
     var[0]=multiplicador*ip;//coeficiente multiplicando a press�o na matriz de coeficientes
     var[1]=multiplicador*ip*Pres;//termo livre devido � condi��o de contorno
    }
    else{
     var[0]=multiplicador*(qmax - qsat)*(0.2+1.6*pfundo/Psat2)/Psat2;//coeficiente multiplicando a press�o na matriz de coeficientes
     var[1]=multiplicador*(qmax+0.8*(qmax - qsat)*(pfundo/Psat2)*(pfundo/Psat2));//termo livre devido � condi��o de contorno
    }
 }
 else{
   var[0]=multiplicador*ij;//coeficiente multiplicando a press�o na matriz de coeficientes
   var[1]=multiplicador*ij*Pres;//termo livre devido � condi��o de contorno
 }
 return var;
}*/

InjGas::InjGas(double qgij,double vtemp,ProFlu vfluido,ProFluCol vfluicol, double vqcomp){
 QGas=qgij;
 razCompGas=vqcomp;
 temp=vtemp;
 tipoflu=0;
 seco=1;
 this->FluidoPro=vfluido;
 this->fluidocol=vfluicol;
 this->FluidoPro.MudaVazMas(VMas(1, 15));
}

InjGas::InjGas(double qgij,double vtemp, double vqcomp){
 QGas=qgij;
 razCompGas=vqcomp;
 temp=vtemp;
 tipoflu=0;
 seco=1;
 this->FluidoPro=ProFlu();
 this->fluidocol=ProFluCol();
}

InjGas::InjGas(const InjGas& vinj){//construtor de c�pia
 QGas=vinj.QGas;
 razCompGas=vinj.razCompGas;
 temp=vinj.temp;
 tipoflu=0;
 seco=vinj.seco;
 this->FluidoPro=vinj.FluidoPro;
 this->fluidocol=vinj.fluidocol;
}

InjGas& InjGas::operator =(const InjGas& vinj){
  QGas=vinj.QGas;
  razCompGas=vinj.razCompGas;
  temp=vinj.temp;
  tipoflu=0;
  seco=vinj.seco;
  this->FluidoPro=vinj.FluidoPro;
  this->fluidocol=vinj.fluidocol;
  return *this;
}

double InjGas::VMas(const double& pres, const double& temp){
//Para o caso de uma fonte de gas Lift, vaz�o injetada em Sm3/d, vaz�o m�ssica em kg/s

 //double multiplicador = this->FluidoPro.MasEspGas(1, 15)/86400;
 double multiplicador = this->FluidoPro.Deng*1.225/86400;
 return multiplicador*QGas;
}


InjLiq::InjLiq(double qlij,double vtemp, double vbet,ProFlu vfluido, ProFluCol vfluidoC){
 QLiq=qlij;
 temp=vtemp;
 bet=vbet;
 this->FluidoPro=vfluido;
 fluidocol=vfluidoC;
}

InjLiq::InjLiq(double qlij, double vtemp, double vbet){
 QLiq=qlij;
 temp=vtemp;
 bet=vbet;
 this->FluidoPro=ProFlu();
 fluidocol=ProFluCol();
}

InjLiq::InjLiq(const InjLiq& vinj){//construtor de c�pia
 QLiq=vinj.QLiq;
 temp=vinj.temp;
 bet=vinj.bet;
 this->FluidoPro=vinj.FluidoPro;
}

InjLiq& InjLiq::operator =(const InjLiq& vinj){
  QLiq=vinj.QLiq;
  temp=vinj.temp;
  bet=vinj.bet;
  this->FluidoPro=vinj.FluidoPro;
  fluidocol=vinj.fluidocol;
  return *this;
}

double InjLiq::VMas(const double& pres, const double& temp){
//Para o caso de uma fonte de l�quido mais g�s, vaz�o injetada em Sm3/d, vaz�o m�ssica em kg/s
	 //if(i==0)return mascor;
	 //else if(i==1)return API;
	 //else if(i==2)return RGO;
	 //else if(i==3)return Deng;
	 //else if(i==4)return BSW;
	 //else if(i==5)return Denag;
 //double Rhogs = this->FluidoPro.MasEspGas(1, 15);
 double Rhols = (1000.*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+1000.*this->FluidoPro[5]*this->FluidoPro[4];
 //double multiplicador = (Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400.;
 //return (multiplicador*(1.-bet)+bet*fluidocol.MasEspFlu(1.,15))*QLiq/86400.;
	//return (this->FluidoPro.MasEspLiq(1.,15)*(1.-bet)+bet*fluidocol.MasEspFlu(1.,15))*QLiq/86400.;
 return (Rhols*(1.-bet)+bet*fluidocol.MasEspFlu(1.,15))*QLiq/86400.;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//alteracao7

InjMult::InjMult(double vmp, double vmc, double vmg,double vtemp,
		ProFlu vfluido, ProFluCol vfluidoC){
	MassP=vmp;
	MassC=vmc;
	MassG=vmg;
	temp=vtemp;
	condTermo=1;
	this->FluidoPro=vfluido;
	fluidocol=vfluidoC;
}

InjMult::InjMult(double vmp, double vmc, double vmg,double vtemp){
	 MassP=vmp;
	 MassC=vmc;
	 MassG=vmg;
	 temp=vtemp;
	 condTermo=1;
	 this->FluidoPro=ProFlu();
	 fluidocol=ProFluCol();
}

InjMult::InjMult(const InjMult& vinj){//construtor de c�pia
	 MassP=vinj.MassP;
	 MassC=vinj.MassC;
	 MassG=vinj.MassG;
	 temp=vinj.temp;
	 condTermo=vinj.condTermo;
	 this->FluidoPro=vinj.FluidoPro;
}

InjMult& InjMult::operator =(const InjMult& vinj){
	 MassP=vinj.MassP;
	 MassC=vinj.MassC;
	 MassG=vinj.MassG;
	 temp=vinj.temp;
	 condTermo=vinj.condTermo;
	 this->FluidoPro=vinj.FluidoPro;
	 fluidocol=vinj.fluidocol;
	 return *this;
}

double InjMult::VMas(const double& pres, const double& temp){
//Para o caso de uma fonte de l�quido mais g�s, vaz�o injetada em Sm3/d, vaz�o m�ssica em kg/s

 //double Rhogs = this->FluidoPro.MasEspGas(1, 15);
 //double Rhols = (1000.*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+1000.*this->FluidoPro[5]*this->FluidoPro[4];
 //double multiplicador = (Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400.;
 //return (multiplicador*(1.-bet)+bet*fluidocol.MasEspFlu(1.,15))*QLiq/86400.;
	return MassP+MassC+MassG;
}
////////////////////////////////////////////////////////////////////////////////////////////


//template class IPR;
//template class InjGas;
//template class InjLiq;
//template class InjMult;


