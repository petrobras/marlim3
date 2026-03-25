#include "Acidentes2.h"

double dpVarA(const double vazmas,const double pres, const double temp, const double areaM, const double areaJ,const double alf,
		                   const double bet,
		                   const ProFlu fluido,const ProFluCol fluidocol){
  double RazA=areaM/areaJ;
  double cd=CdA(areaM,areaJ);
  double flux1=vazmas/areaM;
  //double x=fluido.FracMassHidra(pres,temp);
  double rhog=fluido.MasEspGas(pres, temp);
  double rhol=(1-bet)*fluido.MasEspLiq(pres, temp)+bet*fluidocol.MasEspFlu(pres, temp);
  double x=rhog*alf/(rhol*(1-alf)+rhog*alf);
  if(RazA>1) return -cd*(flux1*flux1/rhol)*RazA*(RazA-1)*(1+x*(rhol/rhog-1));
  else if(RazA<1) return -cd*(0.5*flux1*flux1/rhol)*(RazA-1)*(RazA-1)*(1+x*(rhol/rhog-1));
  else return 0.;
}

double dTVarA(const double vazmas,const double pres, const double temp, const double areaM, const double areaJ,const double alf,
		                   const double bet,
		                   const ProFlu fluido,const ProFluCol fluidocol){
   double dp=dpVarA(vazmas,pres,temp,areaM,areaJ,alf,bet,fluido,fluidocol)/98066.5;
   double k=fluido.ConstAdG(pres,temp);
   double Tad=pow((pres+dp)/pres,(k-1)/k)*(temp+273)-273-temp;
   //double x=fluido.FracMassHidra(pres,temp);
   double rhog=fluido.MasEspGas(pres, temp);
   double rhol=(1-bet)*fluido.MasEspLiq(pres, temp)+bet*fluidocol.MasEspFlu(pres, temp);
   double x=rhog*alf/(rhol*(1-alf)+rhog*alf);
   double cpg=fluido.CalorGas(pres,temp);
   double cpl=(1-bet)*fluido.CalorLiq(pres,temp)+bet*fluidocol.CalorLiq(pres,temp);
   return cpg*x*Tad/(cpg*x+cpl*(1-x));
}

double CdA(const double areaM, const double areaJ){
 return 0.84;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


double VelSomH(const double pres, const double temp,const double alf, const double bet,
		                    const ProFlu fluido, const ProFluCol fluidocol){
 //double x=fluido.FracMassHidra(pres,temp);
 double k=fluido.ConstAdG(pres,temp);
 double rhog=fluido.MasEspGas(pres, temp);
 double rhol=(1-bet)*fluido.MasEspLiq(pres, temp)+bet*fluidocol.MasEspFlu(pres, temp);
 //double alf=rhol*x/(rhog*(1-x)+rhol*x);
 double rhomis=alf*rhog+(1-alf)*rhol;
 double Rgas=fluido.retornaR();
 double vsh2=(rhomis/rhog)*alf/(k*Rgas*(temp+273.0))+(rhomis/rhol)*(1.-alf)/pow(1400.,2.);
 return 1/sqrt(vsh2);

}

double VelSomG(const double pres, const double temp,const ProFlu fluido){
 //double x=fluido.FracMassHidra(pres,temp);
 double k=fluido.ConstAdG(pres,temp);
 double rhog=fluido.MasEspGas(pres, temp);
 double Rgas=fluido.retornaR();
 double vsh2=1./(k*Rgas*(temp+273.1));
 return 1/sqrt(vsh2);

}

double FunRPresCritH(const double eps, const double* const par){
//par->(p0:(0),T0:(1),rhol:(2),rhog0:(3),Cpg:(4),k:(5),x:(6))
 double T0=par[1];
 double p0=par[0];
 double x=par[6];
 double k=par[5];
 double rhog=par[3];
 double rhol=par[2];
 double Cpg=par[4];
 double alf=rhol*x/(rhog*(1-x)+rhol*x);
 double rhomis=alf*rhog*pow(eps,1/k)+(1-alf)*rhol;
 double val=x*Cpg*(T0+273.)*(pow(eps,(k-1)/k)-1.)+
       (1-x)*p0*98066.5*(eps-1)/rhol+
       0.5*k*eps*par[0]*98066.5/(alf*rhomis);
 return val;
}


double RPresCritH(const double pres, const double temp, const double alf, const double bet
		                      ,const ProFlu fluido, const ProFluCol fluidocol){

 //double x=fluido.FracMassHidra(pres,temp);
 double k=fluido.ConstAdG(pres,temp);
 double rhog=fluido.MasEspGas(pres, temp);
 double rhol=(1-bet)*fluido.MasEspLiq(pres, temp)+bet*fluidocol.MasEspFlu(pres, temp);
 double Cpg=fluido.CalorGas(pres,temp);
 double Cpl=(1-bet)*fluido.CalorLiq(pres,temp)+bet*fluidocol.CalorLiq(pres,temp);
 double x=rhog*alf/(rhol*(1-alf)+rhog*alf);
 //double alf=rhol*x/(rhog*(1-x)+rhol*x);
 double rhomis=alf*rhog+(1-alf)*rhol;
 double cpmis=(alf*rhog*Cpg+(1-alf)*rhol*Cpl)/(rhog*alf+(1-alf)*rhol);
 double cmis=VelSomH(pres,temp,alf,bet,fluido,fluidocol);
 double t1=temp+273.1;
 double t2=(cpmis*t1-0.5*cmis*cmis)/cpmis;
 double aux1=x*rhol/((1-x)*rhog*pow(t2/t1,1/(k-1))+x*rhol);
 double rhomis2=(1-aux1)*rhol+aux1*rhog*pow(t2/t1,1/(k-1));
 double val=(pres*98066.5+cmis*cmis*(rhomis2-rhomis))/98066.5;
 val/=pres;
 if(val<0.)val=0.;

 return val;
}

double massica(double presEstag, double presGarg, const double temp, double alf, double bet, double x,const ProFlu fluido,
		                    const ProFluCol fluidocol,const double areaE,double areaG){

 //double x=fluido.FracMassHidra(pres,temp);
 double sinal=1.;
 if(presEstag<presGarg){
  double buffer=presEstag;
  presEstag=presGarg;
  presEstag=buffer;
  alf=1;
  x=1;
  sinal=-1.;
 // areaG*=0.1;
 }
 double k=fluido.ConstAdG(presEstag,temp);
 double rhogE=fluido.MasEspGas(presEstag, temp);
 double rholE=(1-bet)*fluido.MasEspLiq(presEstag, temp)+bet*fluidocol.MasEspFlu(presEstag, temp);
 double Cpg=fluido.CalorGas(presEstag,temp);
 double Cpl=(1-bet)*fluido.CalorLiq(presEstag,temp)+bet*fluidocol.CalorLiq(presEstag,temp);
// double x=rhogE*alf/(rholE*(1-alf)+rhogE*alf);
 double cpmis=(alf*rhogE*Cpg+(1-alf)*rholE*Cpl)/(rhogE*alf+(1-alf)*rholE);
 double razpres=presEstag/presGarg;
 double t1=(temp+273.1)*pow(razpres,(1-k)/k);
 t1-=273.1;
 if(t1<-50.)t1=-50.;
 double rholG=(1-bet)*fluido.MasEspLiq(presGarg, temp)+bet*fluidocol.MasEspFlu(presGarg, temp);
 double rhogG=fluido.MasEspGas(presGarg, t1);
 double alfG=rholG*x/(rholG*x+rhogG*(1-x));
 double rhomisE=(1-alf)*rholE+alf*rhogE;
 double rhomisG=(1-alfG)*rholG+alfG*rhogG;
// double vs=VelSomH(presGarg, t1, alf, fluido);
// double maxmas=areaG*rhogG*vs;
 double val;
// double num=2*((Cpl*(1-x)+Cpg*x)*(temp-t1)+(1-x)*(1/rhogE)*fabsl(presEstag-presGarg)*98066.5);
 //double den=(1/(areaG*areaG*rhomisG)-1/(areaE*areaE*rhomisE));
 double den=0.5*(1./(pow(areaG,2.)*rhomisG*rhomisG)-1./(pow(areaE,2.)*rhomisE*rhomisE));
 double num=x*Cpg*(temp+273.1)*(1-pow(razpres,(1-k)/k))+(1-x)*presGarg*98066.5*(razpres-1)/rholE;
 val=sqrtl(fabs(num/den));
 //if(val>maxmas)val=maxmas;
 return sinal*val;
}

double MasMax(double presEstag, double presGarg, const double temp, double alf, double bet,
		                   double x,const ProFlu fluido,const ProFluCol fluidocol, const double areaG){

 double sinal=1.;
 if(presEstag<presGarg){
	double buffer=presEstag;
	presEstag=presGarg;
	presEstag=buffer;
	x=1.;
	alf=1.;
	sinal=-1.;
 }
 double k=fluido.ConstAdG(presEstag,temp);
 double rhogE=fluido.MasEspGas(presEstag, temp);
 double rhol=(1-bet)*fluido.MasEspLiq(presEstag, temp)+bet*fluidocol.MasEspFlu(presEstag, temp);
 //double x=rhogE*alf/(rhol*(1-alf)+rhogE*alf);
 double razpres=presEstag/presGarg;
 double t1=(temp+273.1)*pow(razpres,(1-k)/k);
 t1-=273.1;
 if(t1<-50.)t1=-50.;
 double rhogG=fluido.MasEspGas(presGarg, t1);
 double alfG=rhol*x/(rhol*x+rhogG*(1-x));

 rhol=(1-bet)*fluido.MasEspLiq(presGarg, temp)+bet*fluidocol.MasEspFlu(presGarg, temp);
 double rhomis=alfG*rhogG+(1-alfG)*rhol;
 double Rgas=fluido.retornaR();
 double vsh2=(rhomis/rhogG)*alfG/(k*Rgas*(t1+273.0))+(rhomis/rhol)*(1.-alfG)/pow(1400.,2.);
 double vs=1/sqrt(vsh2);
 double maxmas=areaG*rhomis*vs;

 return sinal*maxmas;
}



double RPresH(const double pres, const double temp,const double alf,const double bet,
                           const double Qg, const double Ql,const double areaTub, const double areaGarg,
						   const ProFlu fluido, const ProFluCol fluidocol){
	double presGarg=pres;
    double rl1=(1-bet)*fluido.MasEspLiq(presGarg, temp)+bet*fluidocol.MasEspFlu(presGarg, temp);
    double rg1=fluido.MasEspGas(presGarg, temp);
    double masentrada=(rg1*Qg+rl1*Ql);
    double rmist=rl1*(1-alf)+rg1*alf;
    double sensSup=1.;
    if(masentrada<0)sensSup=-1.;
    double delp=0.5*(1/rmist)*(1/pow(areaGarg,2.)-1/pow(areaTub,2.))*masentrada*masentrada;
    double presEstag=presGarg+delp/98066.5;

   /* double k;
    double rhogE;
    double rhol;
    double Cpg;
    double Cpl;
    double x;
    //double cpmis;
    double razpres;
    double t1;
    double alfG;
    double rhomisE;
    double rhomisG;
    double val;

    double raz0;
    double dif=1.;
    if(presEstag/presGarg>1.01){
     while(dif>1e-3){
      raz0=presEstag/presGarg;
      k=fluido.ConstAdG(presEstag,temp);
      rhogE=fluido.MasEspGas(presEstag, temp);
      rhol=(1-bet)*fluido.MasEspLiq(presEstag, temp)+bet*fluidocol.MasEspFlu(presEstag, temp);
      Cpg=fluido.CalorGas(presEstag,temp);
      Cpl=(1-bet)*fluido.CalorLiq(presEstag,temp)+bet*fluidocol.CalorLiq(presEstag,temp);
      x=rhogE*alf*Qg/(rhol*(1-alf)*Ql+rhogE*alf*Qg);
      //cpmis=(alf*rhogE*Cpg+(1-alf)*rhol*Cpl)/(rhogE*alf+(1-alf)*rhol);
      razpres=presEstag/presGarg;
      //t1=((1-x)*Cpl+x*Cpg*pow(razpres,(1-k)/k))*(temp+273.1)/cpmis;
      t1=(temp+273.1)*pow(razpres,(1-k)/k);
      t1-=273.1;
      if(t1<-50.) t1=-50.;
      rl1=(1-bet)*fluido.MasEspLiq(presGarg, temp)+bet*fluidocol.MasEspFlu(presGarg, temp);
      rg1=fluido.MasEspGas(presGarg, t1);
      alfG=rhol*x/(rl1*x+rg1*(1-x));
      rhomisE=(1-alf)*rhol+alf*rhogE;
      rhomisG=(1-alfG)*rl1+alfG*rg1;
      //num=2*((Cpl*(1-x)+Cpg*x)*(temp-t1)+fabsl((presEstag/rhol)-(presGarg/rl1))*98066.5);
     // den=(1/(areaGarg*areaGarg*rhomisG)-1/(areaTub*areaTub*rhomisE));

      val=0.5*masentrada*masentrada*(1./(pow(areaGarg,2.)*rhomisG*rhomisG)-1./(pow(areaTub,2.)*rhomisE*rhomisE));
     // val-=Cpg*x*(temp-t1);
      //val+=(1-x)*presGarg*98066.5/rl1;
      //val*=(rhol/(98066.5)*(1-x));
     // presEstag=val;
      val=x*Cpg*(temp+273.1)*(1-pow(razpres,(1-k)/k))-val;
      val=val+(1-x)*presGarg*98066.5*(razpres-1)/rhol;
      double der=(1-x)*presGarg*98066.5/rhol-x*Cpg*(temp+273.1)*((1-k)/k)*pow(razpres,1/k);
      razpres=razpres-val/der;
      dif=fabsl(raz0-razpres);
      presEstag=razpres*presGarg;
      if(presEstag<presGarg){
    	int para=0;
    	para++;
      }
     }
    }*/

      return presEstag/presGarg;
}

double RPresHVap(const double pres, const double temp,const double alf,
                           const double Qg, const double Ql,const double areaTub, const double areaGarg){

	ProVap fluido;
	double presGarg=pres;
    double rl1=fluido.MasEspLiq(presGarg, temp);
    double rg1=fluido.MasEspGas(presGarg, temp);
    double masentrada=(rg1*Qg+rl1*Ql);
    double rmist=rl1*(1-alf)+rg1*alf;
    double sensSup=1.;
    if(masentrada<0)sensSup=-1.;
    double delp=0.5*(1/rmist)*(1/pow(areaGarg,2.)-1/pow(areaTub,2.))*masentrada*masentrada;
    double presEstag=presGarg+delp/98066.5;

      return presEstag/presGarg;
}

double DPBocConH(const double pres, const double temp,const double alf,const double bet,
                              const double Qg, const double Ql,const double areaTubo,
                              const double areaGarg, const ProFlu fluido, const ProFluCol fluidocol){
// if(areaGarg/areaTubo>0.5){
  //   double rhog=fluido.MasEspGas(pres, temp);
    // double rhol=(1-bet)*fluido.MasEspLiq(pres, temp)+bet*fluidocol.MasEspFlu(pres, temp);
     //double vazmas=rhol*Ql+rhog*Qg;
	 //return dpVarA(vazmas,pres,temp,areaTubo,areaGarg,alf, bet,fluido,fluidocol)+
       // dpVarA(vazmas,pres,temp,areaGarg,areaTubo,alf, bet,fluido,fluidocol);
 //}
 //else{


  double RP=RPresH(pres,temp,alf,bet,Qg,Ql,areaTubo,areaGarg,fluido,fluidocol);
  double eps;

  eps=RP;
  return pres*98066.5*(eps-1);
 //}
}

double DPBocConHVap(const double pres, const double temp,const double alf,
                              const double Qg, const double Ql,const double areaTubo, const double areaGarg){
// if(areaGarg/areaTubo>0.5){
  //   double rhog=fluido.MasEspGas(pres, temp);
    // double rhol=(1-bet)*fluido.MasEspLiq(pres, temp)+bet*fluidocol.MasEspFlu(pres, temp);
     //double vazmas=rhol*Ql+rhog*Qg;
	 //return dpVarA(vazmas,pres,temp,areaTubo,areaGarg,alf, bet,fluido,fluidocol)+
       // dpVarA(vazmas,pres,temp,areaGarg,areaTubo,alf, bet,fluido,fluidocol);
 //}
 //else{


  double RP=RPresHVap(pres,temp,alf,Qg,Ql,areaTubo,areaGarg);
  double eps;

  eps=RP;
  return pres*98066.5*(eps-1);
 //}
}

double DTBocConH(const double pres, const double temp,const double alf,const double bet,const double delp,
		const ProFlu fluido, const ProFluCol fluidocol){
 //double x=fluido.FracMassHidra(pres,temp);
 double rhog=fluido.MasEspGas(pres, temp);
 double rhol=(1-bet)*fluido.MasEspLiq(pres, temp)+bet*fluidocol.MasEspFlu(pres, temp);
 double x=rhog*alf/(rhol*(1-alf)+rhog*alf);
 double k=fluido.ConstAdG(pres,temp);
 double eps=(pres+delp)/pres;
 double Cpg=fluido.CalorGas(pres,temp);
 double Cpl=fluido.CalorLiq(pres,temp);
 double dtg=(temp+273)*pow(eps,(k-1)/k)-(temp+273);
 return Cpg*x*dtg/(Cpg*x+Cpl*(1-x));
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	choke::choke(const double tub,const double garg, const double tcd){
	AreaGarg=garg;
	AreaTub=tub;
	razpres=1.;
	cdchk=tcd;
}
	choke::choke(const choke& antigo){
	AreaGarg=antigo.AreaGarg;
	AreaTub=antigo.AreaTub;
	razpres=antigo.razpres;
	cdchk=antigo.cdchk;
}
choke& choke::operator=(const choke& antigo){
   if (this != &antigo) {
	AreaGarg=antigo.AreaGarg;
    AreaTub=antigo.AreaTub;
    razpres=antigo.razpres;
    cdchk=antigo.cdchk;
   }
   return *this;
}

double choke::dpVap(const double pres, const double temp,const double alf,const double Qg,const double Ql)const
{
    return DPBocConHVap(pres,temp,alf,Qg,Ql,AreaTub,AreaGarg);
}

double choke::razpresSachdF(double y, double presE, double tempE,double alf, double bet,double  x, const ProFlu fluido,const ProFluCol fluidocol){

	 double rhog=fluido.MasEspGas(presE, tempE);
	 double rhol=(1.-bet)*fluido.MasEspLiq(presE, tempE)+bet*fluidocol.MasEspFlu(presE, tempE);
	 double Vg=1./rhog;
	 double Vl=1./rhol;

	 double k=fluido.ConstAdG(presE,tempE);
	 double Cpg=fluido.CalorGas(presE,tempE);
	 double Cpl=(1.-bet)*fluido.CalorLiq(presE,tempE)+bet*fluidocol.CalorLiq(presE,tempE);
	 double Cvg=Cpg/k;

	 double n=1.+(x*(Cpg-Cvg))/(x*Cvg+(1-x)*Cpl);

	 double presG=y*presE;
	 double presvol=presE*pow(Vg,n);
	 double Vg2=pow(y, -1./n);
	 //double Vg2=Vg*pow(y,-1./k);

	 double razK=k/(k-1.);

	 double num=razK+(1-x)*Vl*(1-y)/(x*Vg);
	 double den=razK+0.5*n+n*(1-x)*Vl/(x*Vg2)+0.5*n*pow((1-x)*Vl/(x*Vg2),2.);

	 return y-pow(num/den,razK);
}

double choke::razpresSachdZ(double a,double b, double presE, double tempE,
		                         double alf, double bet,double  x,
		                         const ProFlu fluido,const ProFluCol fluidocol
		                         ,double delta ,double epsn,int maxit){
        //a e b-> chutes iniciais para a falsa corda
        //epsn -> limite em que se admite F(double) aproximadamente zero
        //delta-> limite a partir do qual admite-se que j� n�o existe varia��o no c�lculo da raiz
        //maxit-> m�ximo n�mero de itera��es permitido no c�lculo
        //F(double)-> fun��o que se quer obter a raiz
          double u=razpresSachdF(a, presE,tempE,alf, bet,x, fluido,fluidocol);
          double e=b-a;
          double c;

          double v=razpresSachdF(b, presE,tempE,alf, bet,x, fluido,fluidocol);
          int inter=0;
          if(u!=0 && v!=0){//neste primeiro bloco, busca-se assegurar que sempre se estra� trabalhando com valores de fun��o negativa nos chutes iniciais para se garantir maior converg�ncia na falsa corda
           if(u/v>0){
            if(u>0){
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u<proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy>0&&inter<400){
              prox=prox-del/10;
              proy=razpresSachdF(prox, presE,tempE,alf, bet,x, fluido,fluidocol);
              inter++;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
            else{
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u>proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy<0&&inter<400){
              prox=prox-del/10;
              proy=razpresSachdF(prox, presE,tempE,alf, bet,x, fluido,fluidocol);
              inter++;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
           }
          }

         int controla=0;
          for(int k=1;k<=maxit;k++){//este bloco trata da falsa corda propriamente dita
            e*=0.5;
            c=a+e;
            double w=razpresSachdF(c, presE,tempE,alf, bet,x, fluido,fluidocol);
            if(fabs(e)<delta||fabs(w)<epsn) return c;
            ((u>0&&w<0)||(u<0&&w>0))?(b=c):(a=c,u=w);
            controla++;
          }
          cout<<"Falsa corda em Choke atingiu n�mero m�ximo de itera��es"<<"\n" ;
          //if(controla>=maxit)contaExit++;
          //if(contaExit>10) exit(-1);
          return c;
}

double choke::vazmassSachd(double y, double presE, double tempE,double alf, double bet,
		double  x, const ProFlu fluido,const ProFluCol fluidocol, double xmin){
	double G2;
	if(y<1.){
		if(x>xmin){
	       double rhog=fluido.MasEspGas(presE, tempE);
	       double rhol=(1.-bet)*fluido.MasEspLiq(presE, tempE)+bet*fluidocol.MasEspFlu(presE, tempE);
	       double Vg=1./rhog;
	       double Vl=1./rhol;

	       double k=fluido.ConstAdG(presE,tempE);
	       //k=0.5*(k+fluido.ConstAdG(presE*y,tempE*pow(y,(k-1)/k)));
	       double Cpg=fluido.CalorGas(presE,tempE);
	       double Cpl=(1.-bet)*fluido.CalorLiq(presE,tempE)+bet*fluidocol.CalorLiq(presE,tempE);
	       double Cvg=Cpg/k;
	       double n=1.+(x*(Cpg-Cvg))/(x*Cvg+(1-x)*Cpl);
	       //double Vg2=Vg*pow(y,-1./k);
	       double Vg2=Vg*pow(y,-1./n);

	       double rm2=x*Vg2+(1-x)*Vl;
	       rm2=1./rm2;
	       //double rm2=x*(1./Vg2)+(1-x)*rhol;

	       G2= 2*presE*98066.5*rm2*rm2*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
	       //G2= presE*98600*rm2*rm2*pow(y,2.*(1-k*k)/k)*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
	       G2=sqrtl(G2);
		}
		else{
		       double rhog=fluido.MasEspGas(presE, tempE);
		       double rhol=(1.-bet)*fluido.MasEspLiq(presE, tempE)+bet*fluidocol.MasEspFlu(presE, tempE);
		       double Vg=1./rhog;
		       double Vl=1./rhol;
		       double xi=xmin;

		       double k=fluido.ConstAdG(presE,tempE);
		       //k=0.5*(k+fluido.ConstAdG(presE*y,tempE*pow(y,(k-1)/k)));
		       double Cpg=fluido.CalorGas(presE,tempE);
		       double Cpl=(1.-bet)*fluido.CalorLiq(presE,tempE)+bet*fluidocol.CalorLiq(presE,tempE);
		       double Cvg=Cpg/k;
		       double n=1.+(xi*(Cpg-Cvg))/(xi*Cvg+(1-xi)*Cpl);
		       //double Vg2=Vg*pow(y,-1./k);
		       double Vg2=Vg*pow(y,-1./n);

		       double rm2=xi*Vg2+(1-xi)*Vl;
		       rm2=1./rm2;
		       //double rm2=x*(1./Vg2)+(1-x)*rhol;

		       G2= 2*presE*98066.5*rm2*rm2*((1-xi)*(1-y)/rhol+xi*(k/(k-1.))*(Vg-y*Vg2));
		       double GL= 2*presE*98066.5*rhol*rhol*((1-y)/rhol);
		       G2=(x/xi)*G2+((xmin-x)/xi)*GL;

		       //G2= presE*98600*rm2*rm2*pow(y,2.*(1-k*k)/k)*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
		       G2=sqrtl(G2);
		}
	 }
	 else G2=0.;
	 return cdchk*G2*AreaGarg;
}

double choke::vazmaxSachd(double presE, double tempE,double alf, double bet,double  x,
	 	                                    const ProFlu fluido,const ProFluCol fluidocol, double xmin){
	double G2;
	if(x>xmin){
    	 double y;
    	 if(x>1-1e-15){
			double kad=fluido.ConstAdG(presE,tempE);
			y=pow((kad+1)/2.,-kad/(kad-1));
    	 }
    	 else y = razpresSachdZ(0.8,0.2,presE,tempE,alf,bet,x,fluido,fluidocol);
    	 razpres=y;
    	 double rhog=fluido.MasEspGas(presE, tempE);
    	 double rhol=(1.-bet)*fluido.MasEspLiq(presE, tempE)+bet*fluidocol.MasEspFlu(presE, tempE);
    	 double Vg=1./rhog;
    	 double Vl=1./rhol;

    	 double k=fluido.ConstAdG(presE,tempE);
    	 //k=0.5*(k+fluido.ConstAdG(presE*y,tempE*pow(y,(k-1)/k)));
    	 double Cpg=fluido.CalorGas(presE,tempE);
    	 double Cpl=(1.-bet)*fluido.CalorLiq(presE,tempE)+bet*fluidocol.CalorLiq(presE,tempE);
    	 double Cvg=Cpg/k;
    	 double n=1.+(x*(Cpg-Cvg))/(x*Cvg+(1-x)*Cpl);
    	 //double Vg2=Vg*pow(y,-1./k);
    	 double Vg2=Vg*pow(y,-1./n);

    	 double rm2=x*Vg2+(1-x)*Vl;
    	 rm2=1./rm2;
    	 //double rm2=x*(1./Vg2)+(1-x)*rhol;

    	 G2= 2*presE*98066.5*rm2*rm2*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
    	 //double G2= presE*98600*rm2*rm2*pow(y,2.*(1-k*k)/k)*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
		 G2=sqrtl(G2);
     }
     else{
       	 double y;
       	 double xi=xmin;
         y = razpresSachdZ(0.8,0.2,presE,tempE,alf,bet,xi,fluido,fluidocol);
         razpres=1.;
         double rhog=fluido.MasEspGas(presE, tempE);
         double rhol=(1.-bet)*fluido.MasEspLiq(presE, tempE)+bet*fluidocol.MasEspFlu(presE, tempE);
         double Vg=1./rhog;
         double Vl=1./rhol;

         double k=fluido.ConstAdG(presE,tempE);
         //k=0.5*(k+fluido.ConstAdG(presE*y,tempE*pow(y,(k-1)/k)));
         double Cpg=fluido.CalorGas(presE,tempE);
         double Cpl=(1.-bet)*fluido.CalorLiq(presE,tempE)+bet*fluidocol.CalorLiq(presE,tempE);
         double Cvg=Cpg/k;
         double n=1.+(xi*(Cpg-Cvg))/(xi*Cvg+(1-xi)*Cpl);
         //double Vg2=Vg*pow(y,-1./k);
         double Vg2=Vg*pow(y,-1./n);

         double rm2=xi*Vg2+(1-xi)*Vl;
         rm2=1./rm2;
         //double rm2=x*(1./Vg2)+(1-x)*rhol;

         G2= 2*presE*98066.5*rm2*rm2*((1-xi)*(1-y)/rhol+xi*(k/(k-1.))*(Vg-y*Vg2));
         //double G2= presE*98600*rm2*rm2*pow(y,2.*(1-k*k)/k)*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
	     double GL= 2*presE*98066.5*rhol*rhol*((1-y)/rhol);
	     G2=(x/xi)*G2+((xmin-x)/xi)*GL;
         G2=sqrtl(G2);
     }
	 return cdchk*G2*AreaGarg;
}


double choke::razpresSachdFVap(double y, double presE, double tempE,double alf,double  x){

	 ProVap fluido;

	 double rhog=fluido.MasEspGas(presE, tempE,TEMP);
	 double rhol=fluido.MasEspLiq(presE, tempE,TEMP);
	 double Vg=1./rhog;
	 double Vl=1./rhol;

	 double k=fluido.KAgFunc(presE,tempE);
	 double Cpg=fluido.CPgFunc(presE,tempE);
	 double Cpl=fluido.CPlFunc(presE,tempE);
	 double Cvg=Cpg/k;

	 double n=1.+(x*(Cpg-Cvg))/(x*Cvg+(1-x)*Cpl);

	 double presG=y*presE;
	 double presvol=presE*pow(Vg,n);
	 double Vg2=pow(y, -1./n);
	 //double Vg2=Vg*pow(y,-1./k);

	 double razK=k/(k-1.);

	 double num=razK+(1-x)*Vl*(1-y)/(x*Vg);
	 double den=razK+0.5*n+n*(1-x)*Vl/(x*Vg2)+0.5*n*pow((1-x)*Vl/(x*Vg2),2.);

	 return y-pow(num/den,razK);
}

double choke::razpresSachdZVap(double a,double b, double presE, double tempE,double alf,double  x
		                                    ,double delta ,double epsn,int maxit){
        //a e b-> chutes iniciais para a falsa corda
        //epsn -> limite em que se admite F(double) aproximadamente zero
        //delta-> limite a partir do qual admite-se que j� n�o existe varia��o no c�lculo da raiz
        //maxit-> m�ximo n�mero de itera��es permitido no c�lculo
        //F(double)-> fun��o que se quer obter a raiz

          double u=razpresSachdFVap(a, presE,tempE,alf, x);
          double e=b-a;
          double c;

          double v=razpresSachdFVap(b, presE,tempE,alf,x);
          int inter=0;
          if(u!=0 && v!=0){//neste primeiro bloco, busca-se assegurar que sempre se estra� trabalhando com valores de fun��o negativa nos chutes iniciais para se garantir maior converg�ncia na falsa corda
           if(u/v>0){
            if(u>0){
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u<proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy>0&&inter<400){
              prox=prox-del/10;
              proy=razpresSachdFVap(prox, presE,tempE,alf,x);
              inter++;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
            else{
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u>proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy<0&&inter<400){
              prox=prox-del/10;
              proy=razpresSachdFVap(prox, presE,tempE,alf,x);
              inter++;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
           }
          }

         int controla=0;
          for(int k=1;k<=maxit;k++){//este bloco trata da falsa corda propriamente dita
            e*=0.5;
            c=a+e;
            double w=razpresSachdFVap(c, presE,tempE,alf, x);
            if(fabs(e)<delta||fabs(w)<epsn) return c;
            ((u>0&&w<0)||(u<0&&w>0))?(b=c):(a=c,u=w);
            controla++;
          }
          cout<<"Falsa corda em Choke atingiu n�mero m�ximo de itera��es"<<"\n" ;
          //if(controla>=maxit)contaExit++;
          //if(contaExit>10) exit(-1);
          return c;
}

double choke::vazmassSachdVap(double y, double presE, double tempE,double alf,double  x){
	double G2;
	ProVap fluido;
	if(y<1.){
	 double rhog=fluido.MasEspGas(presE, tempE,TEMP);
	 double rhol=fluido.MasEspLiq(presE, tempE,TEMP);
	 double Vg=1./rhog;
	 double Vl=1./rhol;

	 double k=fluido.KAlFunc(presE,tempE);
	 //k=0.5*(k+fluido.ConstAdG(presE*y,tempE*pow(y,(k-1)/k)));
	 double Cpg=fluido.CPgFunc(presE,tempE);
	 double Cpl=fluido.CPgFunc(presE,tempE);
	 double Cvg=Cpg/k;
	 double n=1.+(x*(Cpg-Cvg))/(x*Cvg+(1-x)*Cpl);
	 //double Vg2=Vg*pow(y,-1./k);
	 double Vg2=Vg*pow(y,-1./n);

	 double rm2=x*Vg2+(1-x)*Vl;
	 rm2=1./rm2;
	 //double rm2=x*(1./Vg2)+(1-x)*rhol;

	 G2= 2*presE*98600*rm2*rm2*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
	 //G2= presE*98600*rm2*rm2*pow(y,2.*(1-k*k)/k)*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
	 G2=sqrtl(G2);
	 }
	 else G2=0.;
	 return cdchk*G2*AreaGarg;
}

double choke::vazmaxSachdVap(double presE, double tempE,double alf,double  x){

	 ProVap fluido;
	 double y = razpresSachdZVap(0.8,0.2,presE,tempE,alf,x);
     razpres=y;
	 double rhog=fluido.MasEspGas(presE, tempE,TEMP);
	 double rhol=fluido.MasEspLiq(presE, tempE,TEMP);
	 double Vg=1./rhog;
	 double Vl=1./rhol;

	 double k=fluido.KAgFunc(presE,tempE);
	 //k=0.5*(k+fluido.ConstAdG(presE*y,tempE*pow(y,(k-1)/k)));
	 double Cpg=fluido.CPgFunc(presE,tempE);
	 double Cpl=fluido.CPlFunc(presE,tempE);
	 double Cvg=Cpg/k;
	 double n=1.+(x*(Cpg-Cvg))/(x*Cvg+(1-x)*Cpl);
	 //double Vg2=Vg*pow(y,-1./k);
	 double Vg2=Vg*pow(y,-1./n);

	 double rm2=x*Vg2+(1-x)*Vl;
	 rm2=1./rm2;
	 //double rm2=x*(1./Vg2)+(1-x)*rhol;

	 double G2= 2*presE*98600*rm2*rm2*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
	 //double G2= presE*98600*rm2*rm2*pow(y,2.*(1-k*k)/k)*((1-x)*(1-y)/rhol+x*(k/(k-1.))*(Vg-y*Vg2));
	 G2=sqrtl(G2);
	 return cdchk*G2*AreaGarg;
}

//template class choke;

