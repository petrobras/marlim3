#include "estrat.h"

estratificado::estratificado(double vdia, double Ql, double Qg,
                             double vrl, double vrg, double vmil,
                             double vmig, double hol,
                             double vang, double eps){


   ang=vang;
   rl=vrl;
   rg=vrg;
   mil=vmil;
   mig=vmig;

   dia=vdia;

   double area=M_PI*dia*dia/4.;
   hfilm=fhfilm(hol*dia,hol,dia);
   holliq=hol;

   double var=2.*hfilm/dia-1.;
   alNd=al(var,dia)/(dia*dia);
   agNd=ag(var,dia)/(dia*dia);
   swlNd=sl(var,dia)/dia;
   swgNd=sg(var,dia)/dia;
   siNd=si(var,dia)/dia;
   uls=Ql/area;
   ugs=Qg/area;
   fg=fricg(dia,Qg,eps);
   fl=fricl(dia,Ql,eps);
   fi=fricg(dia,Qg-Ql,eps);
   reyL=reyl(var,dia,Ql);
   reyG=reyg(var,dia,Qg);

   double vell=uls/(4.*alNd/M_PI);
   double velg=ugs/(4.*agNd/M_PI);

   twl=tall(vell);
   twg=talg(velg);
   ti=tali(velg,vell);

   velref=0.25*M_PI*(rl*uls/(alNd*alNd)+rg*ugs/(agNd*agNd))/(rl/alNd+rg/agNd);

   dina =0.;
   cine=0.;

   arr=1;
   coefC0=0.;
   valUd=0.;
   fatorperdaGas=0.;
   fatorperdaLiq=0.;

}

estratificado::estratificado(const estratificado& vest){

   ang=vest.ang;
   rl=vest.rl;
   rg=vest.rg;
   mil=vest.mil;
   mig=vest.mig;

   dia=vest.dia;
   hfilm=vest.hfilm;
   holliq=vest.holliq;

   alNd=vest.alNd;
   agNd=vest.agNd;
   swlNd=vest.swlNd;
   swgNd=vest.swgNd;
   siNd=vest.siNd;
   uls=vest.uls;
   ugs=vest.ugs;
   fg=vest.fg;
   fl=vest.fl;
   reyL=vest.reyL;
   reyG=vest.reyG;

   twl=vest.twl;
   twg=vest.twg;
   ti=vest.ti;

   velref=vest.velref;

   dina=vest.dina;
   cine=vest.cine;

   arr=vest.arr;
   coefC0=vest.coefC0;
   valUd=vest.valUd;

   fatorperdaGas=vest.fatorperdaGas;
   fatorperdaLiq=vest.fatorperdaLiq;

   dFdAl=vest.dFdAl;
   dFdUg=vest.dFdUg;
   dFdUl=vest.dFdUl;
   fi=vest.fi;
   multTrans=vest.multTrans;

}

estratificado& estratificado::operator =(const estratificado& vest){
   ang=vest.ang;
   rl=vest.rl;
   rg=vest.rg;
   mil=vest.mil;
   mig=vest.mig;

   dia=vest.dia;
   hfilm=vest.hfilm;
   holliq=vest.holliq;

   alNd=vest.alNd;
   agNd=vest.agNd;
   swlNd=vest.swlNd;
   swgNd=vest.swgNd;
   siNd=vest.siNd;
   uls=vest.uls;
   ugs=vest.ugs;
   fg=vest.fg;
   fl=vest.fl;
   reyL=vest.reyL;
   reyG=vest.reyG;

   twl=vest.twl;
   twg=vest.twg;
   ti=vest.ti;

   velref=vest.velref;

   dina=vest.dina;
   cine=vest.cine;

   arr=vest.arr;
   coefC0=vest.coefC0;
   valUd=vest.valUd;

   fatorperdaGas=vest.fatorperdaGas;
   fatorperdaLiq=vest.fatorperdaLiq;

   dFdAl=vest.dFdAl;
   dFdUg=vest.dFdUg;
   dFdUl=vest.dFdUl;
   fi=vest.fi;
   multTrans=vest.multTrans;

   return *this;

}

double estratificado::fricl(double dia,
                                 double Ql, double eps){
  double var=2.*hfilm/dia-1.;
  double re=fabsl(reyl(var, dia,Ql));
/*  if(vrey<1e-5)vrey=1e-5;
  double c;
  double n;
  if(vrey>1e-6){
    if(vrey>2400){
      c=0.046;
      n=-0.2;
    }
    else{
      c=16.;
      n=-1.;
    }
    return c*powl(vrey,n);
  }
  else return 0.;*/
	double val;
	//if (fabsl(re) < 1e-5)re= 1e-5;
	if (fabsl(re) > 1e-5) {
		if (re > 2400) {
			val = 6.9 / fabsl(re) + powl(eps / 3.7, 1.11);
			val = -1.8 * (logl(val) / logl(10.));
			val = powl(1 / val,2.);
		} else
			val = 4*16. / fabsl(re);
	} else
		val = 0.005;
	return val/4.;
  //return 0.005;
}

double estratificado::fricg(double dia,
                                 double Qg, double eps){
  double var=2.*hfilm/dia-1.;
  double re=fabsl(reyg(var, dia,Qg));
/*  if(vrey<1e-5)vrey=1e-5;
  double c;
  double n;
  if(vrey>1e-6){
    if(vrey>2400){
      c=0.046;
      n=-0.2;
    }
    else{
      c=16.;
      n=-1.;
    }
    return c*powl(vrey,n);
  }
  else return 0.;*/
	double val;
	//if (fabsl(re) < 1e-5)re= 1e-5;
	if (fabsl(re) > 1e-5) {
		if (re > 2400) {
			val = 6.9 / fabsl(re) + powl(eps / 3.7, 1.11);
			val = -1.8 * (logl(val) / logl(10.));
			val = powl(1 / val,2.);
		} else
			val = 4*16. / fabsl(re);
	} else
		val = 0.005;
	return val/4.;
 // return 0.005;
}

double estratificado::fonteQMGas(double dia){
   return -twg*swgNd/(agNd*dia)-ti*siNd/(agNd*dia)-9.82*rg*sinl(ang);
}

double estratificado::fonteQMliquido(double dia){
    return -twl*swlNd/(alNd*dia)+ti*siNd/(alNd*dia)-9.82*rl*sinl(ang);
}

void estratificado::mapa(double dia,double alf){

 /*fatorperdaLiq=0.;
 fatorperdaGas=0.;
 arr=0;
 multTrans=0.;*/
 //if(alNd>1e-4 && agNd>1e-4){
	double signUl;
    if((uls)>0.)signUl=1.;
    else signUl=-1.;
    double signUg;
    if((ugs)>0.)signUg=1.;
    else signUg=-1.;
    double signUd;
    if((ugs/alf-uls/(1-alf))>0)signUd=1.;
    else signUd=-1.;

    double coefpi2=powl(0.25*M_PI,2.);
    double coefpi3=powl(0.25*M_PI,3.);

    dFdUl=signUl*(fl*rl*swlNd*(uls)*coefpi2/(dia*powl(alNd,3.)))+
    signUd*(fg*rg*siNd*coefpi3/(dia*alNd*agNd))*(-(uls)/(alNd*alNd)+(ugs)/(alNd*agNd));
    dFdUg=-signUg*(fg*rg*swgNd*(ugs)*coefpi2/(dia*powl(agNd,3.)))-
    signUd*(fg*rg*siNd*coefpi3/(dia*alNd*agNd))*(-(uls)/(alNd*agNd)+(ugs)/(agNd*agNd));

    /*if(reyL<2400){
      double dfwldUl=-signUl*(16/M_PI)*mil*swlNd/(rl*dia);
      dFdUl+=signUl*rl*coefpi2*swlNd/(2.*dia*powl(alNd,3.))*dfwldUl;
    }
    if(reyG<2400){
      double dfwgdUg;
      if(fabsl(ugs)>1e-20)dfwgdUg=-signUg*(16/M_PI)*mig*swgNd/(rg*dia*ugs*ugs);
      else dfwgdUg=0.;
      dFdUg+=(-rg*coefpi2*swgNd*ugs*fabsl(ugs)/(2.*dia*powl(agNd,3.))-
               signUd*(rg*coefpi3*siNd/(2.*dia))*(ugs*ugs/(alNd*powl(agNd,3.))-
         2.*uls*ugs/(powl(alNd,2.)*powl(agNd,2.))+uls*uls/(powl(alNd,3.)*agNd)))*
         dfwgdUg;
    }*/


    double termo=2.*(hfilm/dia)-1.;
    double dSldAl=2./(1.-termo*termo);
    double dSgdAl=-dSldAl;
    double dSidAl=-2.*termo/(1.-termo*termo);

    double termo1=signUl*(fl*rl*uls*uls*coefpi2/(2.*dia))*
                    (dSldAl/powl(alNd,3.)-3.*swlNd/powl(alNd,4.));

    double termo2=-signUg*(fg*rg*ugs*ugs*coefpi2/(2.*dia))*
                     (dSgdAl/powl(agNd,3.)+3.*swgNd/powl(agNd,4.));

    double termo3=-signUd*(fg*rg*coefpi3/(2.*dia));

    double termo3a=
    ugs*ugs*(dSidAl/(powl(agNd,3.)*alNd)-siNd/(powl(alNd,2.)*powl(agNd,3.))+
    3.*siNd/(powl(agNd,4.)*alNd));

    double termo3b=-2.*uls*ugs*(dSidAl/(powl(alNd,2.)*powl(agNd,2.))-
    2.*siNd/(powl(alNd,3.)*powl(agNd,2.))+2.*siNd/(powl(alNd,2.)*powl(agNd,3.)));

    double termo3c=
    uls*uls*(dSidAl/(powl(alNd,3.)*agNd)-3.*siNd/(powl(alNd,4.)*agNd)+
    siNd/(powl(alNd,3.)*powl(agNd,2.)));

    dFdAl=termo1+termo2+termo3*(termo3a+termo3b+termo3c);

    if(fabsl(dFdUl-dFdUg)>1e-20)cine=-0.25*M_PI*dFdAl/(dFdUl-dFdUg);
    else cine=0.;
    dina=(9.82*dia*cosl(ang)*(rl-rg)/siNd-
       coefpi2*powl(ugs/agNd-uls/alNd,2.)/(alNd/rl+agNd/rg))/
       (rl/alNd+rg/agNd);
    arr=1;
    multTrans=0.;
    double fator=1;
    if(dina>fator*powl(cine-velref,2.)) arr=-1;

   // if(arr!=1){
	   // fatorperdaLiq=0.;
	    //fatorperdaGas=0.;
    //}
    //else{
	    if(alNd>1e-4)fatorperdaLiq=fl*swlNd*dia/(2.*rl*powl(alNd*dia*dia,2.));
	    else fatorperdaLiq=0.;
	    if(agNd>1e-4)fatorperdaGas=fg*swgNd*dia/(2.*rg*powl(agNd*dia*dia,2.));
	    else fatorperdaGas=0.;
    //}

 //double razao=dina/powl(cine-velref,2.);
 //if(razao>fator){
	 //multTrans=(1.-0.)*(razao-fator)/(1.-fator);
	 //if(multTrans>1.)multTrans=1.;
	 //else if(multTrans<0.)multTrans=0.;

// }
// }

}
double estratificado::mapa2(double chute){
	double area=0.25*M_PI*dia*dia;
	double x1=0.98;
	double x2=0.01;
	if(chute>0.){
		x1=chute;
		x2=0.9*chute;
		if(x1>=1.)x1=0.98;
	}
	else x1=0.9;
	double hol=zbrentUls(x1,x2,uls*area,ugs*area,0.00001/dia);
	if(hol>0){
	double ulsT=qlequil(hol,uls*area,ugs*area,0.00001/dia);
	  arr=1;
	  if(fabsl(uls)<fabsl(ulsT))arr=-1;
	}
	else arr=1;
	return hol;
}

void estratificado::mapaTD(double chute){
	if(fabsl(uls)>0.01 && fabsl(ugs)>0.01){
	  double area=0.25*M_PI*dia*dia;
	  double ql=uls*area;
	  double qg=ugs*area;
	  double hol1=0.98;
	  double hol2=0.02;
	  /*if(chute>=-1e-15){
		  hol1=1.1*holliq;
		  hol2=0.9*holliq;
	  }*/
	  double hol;
	  if(chute<1e-15)hol=zbrent(hol1,hol2,fabsl(ql),fabsl(qg),0.0001);
	  else hol=zbrent(hol1,hol2,fabsl(ql),fabsl(qg),0.0001,0.01,0.01);
	  holliq=hol;
	  if(hol>0){
	    double hvar=fhfilm(hol*dia,hol,dia);
	    double C2=1-hvar/dia;
	    double var=2.*hvar/dia-1.;
	    double C1=(rl-rg)*9.82*cosl(ang)*ag(var,dia)/(rg*si(var,dia));

	    double maxug=C2*sqrtl(C1)*(1-hol);

	    arr=-1;
	    double ul=uls/hol;
	    if(fabsl(ugs)>maxug) arr=1;
	    else if(ul*ang<0 && ul*ul>=9.82*dia*C2*cosl(ang)/fl)arr=1;
	  }
	  else arr=1;
	}
	else arr=1;
}

double estratificado::equil(double dia){

  double coefpi2=powl(0.25*M_PI,2.);
  double coefpi3=powl(0.25*M_PI,3.);
  double signUd;
  double ugsv=fabsl(ugs);
  double ulsv=fabsl(uls);
  if((ugsv-ulsv)>0.)signUd=1.;
  else signUd=-1.;

  return fl*rl*coefpi2*swlNd*ulsv*ulsv/(2.*dia*powl(alNd,3.))-
  fg*rg*coefpi2*swgNd*ugsv*ugsv/(2.*dia*powl(agNd,3.))-
  signUd*(fg*rg*coefpi3*siNd/(2.*dia))*(ugsv*ugsv/(alNd*powl(agNd,3.))-
  2.*ulsv*ugsv/(powl(alNd,2.)*powl(agNd,2.))+ulsv*ulsv/(powl(alNd,3.)*agNd))+
  9.82*(rl-rg)*sinl(ang);
  
}

double estratificado::equil2(double ql,double qt, double eps){

  double coefpi2=powl(0.25*M_PI,2.);
  double area=0.25*M_PI*dia*dia;
  double ulsC=ql/area;
  double ugsC=(qt-ql)/area;
  double fgC=fricg(dia,ugsC*area,eps);
  double flC=fricl(dia,ulsC*area,eps);

  return
  -flC*rl*coefpi2*swlNd*ulsC*fabsl(ulsC)/(2.*dia*powl(alNd,3.))+
  fgC*rg*coefpi2*swgNd*ugsC*fabsl(ugsC)/(2.*dia*powl(agNd,3.))+
  (fgC*rg*coefpi2*siNd/(2.*dia))*(1/agNd+1/alNd)*
  (ugsC/agNd-ulsC/alNd)*fabsl(ugsC/agNd-ulsC/alNd)-
  9.82*(rl-rg)*sinl(ang);
}

double estratificado::equil3(double hol,double Ql,double Qg, double eps){
	  double area=0.25*M_PI*dia*dia;

	   double hvar=fhfilm(hol*dia,hol,dia);

	   double var=2.*hvar/dia-1.;
	   double alNdv=al(var,dia)/(dia*dia);
	   double agNdv=ag(var,dia)/(dia*dia);
	   double swlNdv=sl(var,dia)/dia;
	   double swgNdv=sg(var,dia)/dia;
	   double siNdv=si(var,dia)/dia;


  double coefpi2=powl(0.25*M_PI,2.);
  double ulsC=Ql/area;
  double ugsC=(Qg)/area;
  double fgC=fricg(dia,ugsC*area,eps);
  double flC=fricl(dia,ulsC*area,eps);
  double signd=1.;
  if((Qg/(1.-hol)-Ql/hol)<0.)signd=-1.;

  return
  -flC*rl*coefpi2*swlNdv*ulsC*fabsl(ulsC)/(2.*dia*powl(alNdv,3.))+
  fgC*rg*coefpi2*swgNdv*ugsC*fabsl(ugsC)/(2.*dia*powl(agNdv,3.))+
  signd*(fgC*rg*coefpi2*siNdv/(2.*dia))*(ugsC*ugsC/(alNdv*powl(agNdv,3))-2.*ulsC*ugsC/(powl(alNdv,2.)*powl(agNdv,2))+
		  ulsC*ulsC/(agNdv*powl(alNdv,3)))-9.82*(rl-rg)*sinl(ang);
}

double estratificado::qlequil(double hol,double Ql,double Qg,double eps){
	  double area=0.25*M_PI*dia*dia;

	   double hvar=fhfilm(hol*dia,hol,dia);

	   double var=2.*hvar/dia-1.;
	   double alNdv=al(var,dia)/(dia*dia);
	   double agNdv=ag(var,dia)/(dia*dia);
	   double swlNdv=sl(var,dia)/dia;
	   double swgNdv=sg(var,dia)/dia;
	   double siNdv=si(var,dia)/dia;


       double coefpi2=powl(0.25*M_PI,2.);
       double ulsC=Ql/area;
       double ugsC=(Qg)/area;
       double fgC=fg+0*fricg(dia,ugsC*area,eps);
       double flC=fl+0*fricl(dia,ulsC*area,eps);

       double signl=1.;
       if(Ql<0) signl=-1.;
       double signg=1.;
       if(Qg<0.)signg=-1.;
       double alf=1.-hol;
       double refdrift=(Qg/alf-Ql/(1-alf));
       double signd=1.;
       if(refdrift<0.)signd=-1.;
       int loop=1;
       double saida=0.;
       while(loop==1){
       double term1=signl*flC*rl*coefpi2*swlNdv/(2.*dia*powl(alNdv,3.))-
    		               signd*(fgC*rg*coefpi2*siNdv/(2.*dia))/(agNdv*powl(alNdv,3));
       double term2=signd*(fgC*rg*coefpi2*siNdv/(2.*dia))*(2.*ugsC/(powl(agNdv,2)*powl(alNdv,2)));
       double term3=-signd*(fgC*rg*coefpi2*siNdv/(2.*dia))*(1/(alNdv*agNdv))*(ugsC/agNdv)*(ugsC/agNdv)-
    		   signg*fgC*rg*coefpi2*swgNdv*ugsC*(ugsC)/(2.*dia*powl(agNdv,3.))-9.82*(rl-rg)*sinl(ang);

       double delta=fabsl(term2*term2-4.*term1*term3);
       double q1=(-term2+sqrtl(delta))/(2.*term1);
       double q2=(-term2-sqrtl(delta))/(2.*term1);
       if(Ql>0){
    	   if(q1<0)q1=1000000.;
    	   if(q2<0)q2=1000000.;
    	   if(q1<q2)saida=q1;
    	   else saida=q2;
       }
       if(Ql<0){
    	   if(q1>0)q1=-1000000.;
    	   if(q2>0)q2=-1000000.;
    	   if(q1>q2)saida=q1;
    	   else saida=q2;
       }
       double refdrift2=(Qg/alf-saida/(1-alf));
        if(refdrift2*refdrift<0){
        	refdrift=refdrift2;
            if(refdrift<0.)signd=-1.;
            else signd=1.;
        }
        else loop=0;
       }
       return saida;
}

double estratificado::criterio(double hol,double qlv,double qg,double eps){
	double area=0.25*M_PI*dia*dia;
	double ql=qlequil(hol,qlv,qg,eps)*area;
	double signUl;
    if((ql)>0.)signUl=1.;
    else signUl=-1.;
    double signUg;
    if((qg)>0.)signUg=1.;
    else signUg=-1.;
    double signUd;
    double alf=1.-hol;
    if((qg/alf-ql/(1-alf))>0)signUd=1.;
    else signUd=-1.;

    double coefpi2=powl(0.25*M_PI,2.);
    double coefpi3=powl(0.25*M_PI,3.);

	double hvar=fhfilm(hol*dia,hol,dia);

	double var=2.*hvar/dia-1.;
	double alNdv=al(var,dia)/(dia*dia);
	double agNdv=ag(var,dia)/(dia*dia);
	double swlNdv=sl(var,dia)/dia;
	double swgNdv=sg(var,dia)/dia;
	double siNdv=si(var,dia)/dia;

    double ulsC=ql/area;
    double ugsC=(qg)/area;
    double fgC=fg;//+0*fricg(dia,ugsC*area,eps);
    double flC=fl;//+0*fricl(dia,ulsC*area,eps);

    dFdUl=signUl*(flC*rl*swlNdv*(ulsC)*coefpi2/(dia*powl(alNdv,3.)))+
    signUd*(fgC*rg*siNdv*coefpi3/(dia*alNdv*agNdv))*(-(ulsC)/(alNdv*alNdv)+(ugsC)/(alNdv*agNdv));
    dFdUg=-signUg*(fgC*rg*swgNdv*(ugsC)*coefpi2/(dia*powl(agNdv,3.)))-
    signUd*(fgC*rg*siNdv*coefpi3/(dia*alNdv*agNdv))*(-(ulsC)/(alNdv*agNdv)+(ugsC)/(agNdv*agNdv));

    double termo=2.*(hvar/dia)-1.;
    double dSldAl=2./(1.-termo*termo);
    double dSgdAl=-dSldAl;
    double dSidAl=-2.*termo/(1.-termo*termo);

    double termo1=signUl*(flC*rl*ulsC*ulsC*coefpi2/(2.*dia))*
                    (dSldAl/powl(alNdv,3.)-3.*swlNdv/powl(alNdv,4.));

    double termo2=-signUg*(fgC*rg*ugsC*ugsC*coefpi2/(2.*dia))*
                     (dSgdAl/powl(agNdv,3.)+3.*swgNdv/powl(agNdv,4.));

    double termo3=-signUd*(fgC*rg*coefpi3/(2.*dia));

    double termo3a=
    ugsC*ugsC*(dSidAl/(powl(agNdv,3.)*alNdv)-siNdv/(powl(alNdv,2.)*powl(agNdv,3.))+
    3.*siNdv/(powl(agNdv,4.)*alNdv));

    double termo3b=-2.*ulsC*ugsC*(dSidAl/(powl(alNdv,2.)*powl(agNdv,2.))-
    2.*siNdv/(powl(alNdv,3.)*powl(agNdv,2.))+2.*siNdv/(powl(alNdv,2.)*powl(agNdv,3.)));

    double termo3c=
    ulsC*ulsC*(dSidAl/(powl(alNdv,3.)*agNdv)-3.*siNdv/(powl(alNdv,4.)*agNdv)+
    siNdv/(powl(alNdv,3.)*powl(agNdv,2.)));

    dFdAl=termo1+termo2+termo3*(termo3a+termo3b+termo3c);

    if(fabsl(dFdUl-dFdUg)>1e-20)cine=-0.25*M_PI*dFdAl/(dFdUl-dFdUg);
    else cine=0.;
    dina=(9.82*dia*cosl(ang)*(rl-rg)/siNdv-
       coefpi2*powl(ugsC/agNdv-ulsC/alNdv,2.)/(alNdv/rl+agNdv/rg))/
       (rl/alNdv+rg/agNdv);

    velref=0.25*M_PI*(rl*ulsC/(alNdv*alNdv)+rg*ugsC/(agNdv*agNdv))/(rl/alNdv+rg/agNdv);

    return (dina-powl(cine-velref,2.));
}

double estratificado::zbrentUls(double x1,double x2,double qlv,double qg,double eps,
		          double tol,
                         double epsn,int maxit){
	double EPS=epsn;
	 //double a=x1;
	 //double b=x2;
	 //double c=x2;
	 //double a=0.99;
	 //double b=0.02;
	 //double c=b;
	 //double fa=criterio(a,qlv,qg,eps);
	 //double fb=criterio(b,qlv,qg,eps);
	 double d,e,fc,p,q,r,s,tol1,xm;
	 double x=x1;
	 double dx1;
	 double fa=1;
	 double fb;
	 double dfdx;
	 int conta=0;
	 while(fabsl(fa)>0.00001&&x<0.99&&x>0.01 && conta<20){
	   dx1=x*0.0001;
	   fa=criterio(x,qlv,qg,eps);
	   fb=criterio(x+dx1,qlv,qg,eps);
	   dfdx=(fb-fa)/dx1;
	   x-=(fa/dfdx);
	   if(x>0.99) x=0.98;
	   if(x<0.01)x=0.012;
	   conta++;
	 }
	 if(x>=0.99||x<=0.01||conta>19) return -1;
	 else return x;

	/* if((fa>0.0&&fb>0.0)||(fa<0.0&&fb<0.0)){
		 double dhol=0.05;
		 while((fa>0.0&&fb>0.0)||(fa<0.0&&fb<0.0)&&a>b){
			 a-=dhol;
			 //if(a<=b){
				 //cout<<"Chute inadequado para o c�lculo de raiz no m�todo Ridders"<<"\n";
				// b=4.*alNd/M_PI;
			 //}
			 if(a>b)fa=criterio(a,qlv,qg,eps);
		 }
	 }
	 if(a>b){
	 fc=fb;
	 for(int iter=0;iter<maxit;iter++){
	   if((fb>0.0&&fc>0.0)||(fb<0.0&&fc<0.0)){
	     c=a;
	     fc=fa;
	     e=d=b-a;
	   }
	   if(fabs(fc)<fabs(fb)){
	     a=b;
	     b=c;
	     c=a;
	     fa=fb;
	     fb=fc;
	     fc=fa;
	   }
	   tol1=2.0*EPS*fabs(b)+0.5*tol;
	   xm=0.5*(c-b);
	   if(fabs(xm)<=tol1||fb==0.0)return b;
	   if(fabs(e)>=tol1&&fabs(fa)>fabs(fb)){
	     s=fb/fa;
	     if(a==c){
	       p=2.0*xm*s;
	       q=1.0-s;
	     }
	     else{
	       q=fa/fc;
	       r=fb/fc;
	       p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
	       q=(q-1.0)*(r-1.0)*(s-1.0);
	     }
	     if(p>0.0)q=-q;
	     p=fabs(p);
	     double min1=3.0*xm*q-fabs(tol1*q);
	     double min2=fabs(e*q);
	     if(2.0*p<(min1<min2?min1:min2)){
	       e=d;
	       d=p/q;
	     }
	     else{
	       d=xm;
	       e=d;
	     }
	   }
	   else{
	     d=xm;
	     e=d;
	   }
	   a=b;
	   fa=fb;
	   if(fabs(d)>tol1)b+=d;
	   else b+=SIGN(tol1,xm);
	   fb=criterio(b,qlv,qg,eps);
	 }
	 cout<<"Falsa corda atingiu n�mero m�ximo de itera��es"<<"\n";
	 return b;
	 }
	 else return -1.;*/
}

double estratificado::findalf(double hol1, double hol2, double ql, double qg
		                    , double eps){
return zbrent(hol1,hol2,ql,qg,eps);
}

double  estratificado::calcC0(double x1,
                                   const double qt,
                                   double alf,double eps,
                                  double tol,
                                  double epsn,
                                  int maxit){
 double signC=1.;
 if(x1<0.)signC=-1.;
 double rtn=signC*0.5*qt;
 double area=0.25*M_PI*dia*dia;
 double f;
 double df;
 double dx;
 for(int j=0;j<maxit;j++){
   f=equil2(rtn,qt,eps);
   df=(f-equil2(rtn*0.9999,qt,eps))/(0.0001*rtn);
   dx=f/df;
   rtn-=dx;
   if(fabsl(dx)<epsn||fabsl(f)<tol) return (qt-rtn)/(alf*qt);

   }
   throw("M�ximo n�mero de intera��es em Newton");
}

double estratificado::FalsaCorda(double a,double b,double qt,
          double alf,double eps,double delta ,double epsn,int maxit){
//zero de fun��o por falsa corda
        //a e b-> chutes iniciais para a falsa corda
        //epsn -> limite em que se admite F(T) aproximadamente zero
        //delta-> limite a partir do qual admite-se que j� n�o existe varia��o no c�lculo da raiz
        //maxit-> m�ximo n�mero de itera��es permitido no c�lculo
        //F(T)-> fun��o que se quer obter a raiz

          double u=equil2(a,qt,eps);
          double e=b-a;
          double c=0.;

          for(int k=1;k<=maxit;k++){//este bloco trata da falsa corda propriamente dita
            e*=0.5;
            c=a+e;
            double w=equil2(c,qt,eps);
            if(fabs(e)<delta&&fabs(w)<epsn) return c;
            ((u>0&&w<0)||(u<0&&w>0))?(b=c):(a=c,u=w);
          }
          cout<<"Falsa corda atingiu n�mero m�ximo de itera��es"<<"\n"<<equil2(c,qt,eps) ;
          //return -10001.;
          return c;
}


double estratificado::zbrent(double x1,double x2,double ql,double qg,double eps,
                                  double tol, double epsn, int maxit){
//Acha zero de fun��o ver NR
 double EPS=epsn;
 //double a=x1;
 //double b=x2;
 //double c=x2;
 double a=0.98;
 double b=0.05;
 double c=b;
 double fa=equil3(a,ql,qg,eps);
 double fb=equil3(b,ql,qg,eps);
 double e=0.;
 double d,fc,p,q,r,s,tol1,xm;

 if((fa>0.0&&fb>0.0)||(fa<0.0&&fb<0.0)){
	 double dhol=0.01;
	 while(((fa>0.0&&fb>0.0)||(fa<0.0&&fb<0.0)) ){
		 a-=dhol;
		 if(a<=b){
			// cout<<"Chute inadequado para o c�lculo de raiz no m�todo Ridders"<<"\n";
			// b=4.*alNd/M_PI;
			 b=-1.;
			 return b;
		 }
	 }
 }
 fc=fb;
 for(int iter=0;iter<maxit;iter++){
   if((fb>0.0&&fc>0.0)||(fb<0.0&&fc<0.0)){
     c=a;
     fc=fa;
     e=d=b-a;
   }
   if(fabs(fc)<fabs(fb)){
     a=b;
     b=c;
     c=a;
     fa=fb;
     fb=fc;
     fc=fa;
   }
   tol1=2.0*EPS*fabs(b)+0.5*tol;
   xm=0.5*(c-b);
   if(fabs(xm)<=tol1||fb==0.0)return b;
   if(fabs(e)>=tol1&&fabs(fa)>fabs(fb)){
     s=fb/fa;
     if(a==c){
       p=2.0*xm*s;
       q=1.0-s;
     }
     else{
       q=fa/fc;
       r=fb/fc;
       p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
       q=(q-1.0)*(r-1.0)*(s-1.0);
     }
     if(p>0.0)q=-q;
     p=fabs(p);
     double min1=3.0*xm*q-fabs(tol1*q);
     double min2=fabs(e*q);
     if(2.0*p<(min1<min2?min1:min2)){
       e=d;
       d=p/q;
     }
     else{
       d=xm;
       e=d;
     }
   }
   else{
     d=xm;
     e=d;
   }
   a=b;
   fa=fb;
   if(fabs(d)>tol1)b+=d;
   else b+=SIGN(tol1,xm);
   fb=equil3(b,ql,qg,eps);
 }
 //cout<<"Falsa corda atingiu n�mero m�ximo de itera��es"<<"\n";
 b=-1.;
 return b;
}

double  estratificado::calcC0FC(const double x1,const double qt,
                                   double alf,double eps,double coaux){
 double area=0.25*M_PI*dia*dia;
 double signC=1.;
 if(x1<0.)signC=-1.;
 double rtna;
 double rtnb;
 if(signC<0.){
   rtna=10*area;
   rtnb=-10*area;
 }
 else{
   rtna=-10*area;
   rtnb=10*area;
 }

 //rtna=-5*fabsl(qt);
 //rtnb=5*fabsl(qt);


 rtna=FalsaCorda(rtna,rtnb,qt,alf,eps);
 //rtna=zbrent(rtna,rtnb,qt,mig,mil,eps);

 double f;
 f=equil2(rtna,qt,eps);
 double ulszero=rtna/area;

 return ((1-coaux*alf)*qt-rtna)/(alf*area);

}

/*double estratificado::CalcUdTrans(double alfc, double alf0,
                        double alf1,double Ql0, double Ql1,
                        double Qg0,double Qg1,
                        double c0aux,double dt,double dx){

 double area=0.25*M_PI*dia*dia;
 double fa=rg*(1-alfc)/(rl*alfc);
 double qt=Qg1+Ql1;
 double j=qt/area;
 double ugs0=Qg0/area;
 double ugs1=Qg1/area;
 double uls0=Ql0/area;
 double uls1=Ql1/area;
 double ug0=ugs0;
 if(alf0>0.00000001) ug0=ugs0/alf0;
 double ug1=ugs1;
 if(alf1>0.00000001) ug1=ugs1/alf1;
 double ul0=uls0;
 if(1-alf0>0.0000001) ul0=uls0/(1-alf0);
 double ul1=uls1;
 if(1-alf1>0.0000001) ul1=uls1/(1-alf1);

 double du1= dt*(ug1*ugs1-ug0*ugs0)/dx;
 double du2=dt*(-1./(1.+fa))*(ug1*ugs1+ul1*uls1-ug0*ugs0-ul0*uls0)/dx;
 double du3=-dt*(9.82*area/(siNd*dia))*(alfc-(1./(1+fa)))*cosl(ang)*(alf1-alf0)/dx;

 double fonte1=dt*(fa/(1+fa))*(1./area)*(-(twg*siNd*dia)/rg-twg*swgNd*dia/rg-
                                          9.82*area*alfc*sinl(ang));
 double fonte2=dt*(1./(1+fa))*(1./area)*((twg*siNd*dia)/rl-twl*swlNd*dia/rl-
                                          9.82*area*(1-alfc)*sinl(ang));

 double ugsN=ugs1-du1-du2-du3+fonte1+fonte2;
 double ulsN=j-ugsN;

 //-1./(1+fa)*(ug1*ugs1+ul1*uls1+ug0*ugs0+ul0*uls0)/dx

 return ((1-c0aux*alfc)*j-uls)/(alfc);

}*/

double estratificado::CalcUdTrans(double alfc, double alf0,
                        double alf1,double Ql0, double Ql1,
                        double Qg0,double Qg1,
                        double pL,double pR,
                        double c0aux,double dt,double dx, int verifica){

if(alf1>0.0000001){
	double area=0.25*M_PI*dia*dia;
 double qt=Qg1+Ql1;
 double j=qt/area;
 double uls0=Ql0/area;
 double uls1=Ql1/area;
  double ul0=uls0;
 if((1-alf0)>0.0000001) ul0=uls0/(1-alf0);
 double ul1=uls1;
 if((1-alf1)>0.0000001) ul1=uls1/(1-alf1);
 double ug1=Qg1/area;
 if(alf1>0.0000001) ug1=ug1/(alf1);

 double du1=-1*dt*(ul1*(ul1-ul0))/dx;
 double du2=-dt*(1/rl)*(pR-pL)*98600/dx;
 double du3=1*dt*9.82*cosl(ang)*(area/(siNd*dia))*(alf1-alf0)/dx;
 double fonte1=dt*(((1*twg*siNd*dia)/rl-twl*swlNd*dia/rl)/(dia*dia*alNd)-
         9.82*sinl(ang));
 double du2g=-dt*(1/rg)*(pR-pL)*98600/dx;
 double fonte2=dt*(((-twg*siNd*dia)/rg-twg*swgNd*dia/rg)/(dia*dia*agNd)-
          9.82*sinl(ang));
 double ugsN=(ug1+du2g+fonte2)*(alf1);

 double ulsN=(ul1+du1+1*du2+du3+fonte1)*(1.-alf1);
// j=ugsN+ulsN;
 double raz=fabsl(twg*siNd*dia/rl)/(fabsl(twg*siNd*dia/rl)+fabsl(9.82*sinl(ang)*dia*dia*alNd));
 if(raz>1)raz=1.;
 double vud=((1-c0aux*alf1)*j-ulsN)/(alf1);
 //double sinal=1.;
 //if(ang<0.)sinal=-1.;
 //if(fabsl(vud)>0.05)vud=sinal*0.05;
// vud=0.2*raz+vud*(1-raz);

 return vud;
}
 else return -1e10;

}

/*double estratificado::CalcUdTrans(double alfc, double alf0,
                        double alf1,double Ql0, double Ql1,
                        double Qg0,double Qg1,
                        double pL,double pR,
                        double c0aux,double dt,double dx, double rg, double rl ){

if(alf1>0.0000001){
	double area=0.25*M_PI*dia*dia;
 double qt=Qg1+Ql1;
 double j=qt/area;
 double uls0=Ql0/area;
 double uls1=Ql1/area;
  double ul0=uls0;
 if((1-alf0)>0.0000001) ul0=uls0/(1-alf0);
 double ul1=uls1;
 if((1-alf1)>0.0000001) ul1=uls1/(1-alf1);
 double ug1=Qg1/area;
 if(alf1>0.0000001) ug1=ug1/(alf1);


 double du1=-dt*area*(pR-pL)*98600/dx;
 double fonte1=dt*((-twg*siNd-twl*swlNd)-9.82*sinl(ang)*area*(rg*alfc+rl*(1-alfc)));

 double ugsN=(Qg1*rg+du1+fonte1)/(area*rg);

 double ulsN=j-ugsN;
 double vud=((1-c0aux*alf1)*j-ulsN)/(alf1);

 return vud;
}
 else return -1e10;

}*/



