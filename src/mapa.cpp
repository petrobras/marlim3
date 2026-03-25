/*
 * mapa.cpp
 *
 *  Created on: 19 de abr de 2016
 *      Author: bwz1
 */

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
using namespace std;
#include <stdlib.h>
#include "mapa.h"
/*#include "Vetor.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "Geometria.h"*/

arranjo::arranjo(double vdia, double vuls, double vugs,
                 double vrl, double vrg,   double vmil,
				 double vmig,double vhol,double vang, double vdina, int vmap,varGlob1D* Vvg1dSP){

	vg1dSP=Vvg1dSP;
	dia=vdia;
	uls=vuls;
	ugs=vugs;
	rl=vrl;
	rg=vrg;
	mil=vmil;
	mig=vmig;
	hol=vhol;
	ang=vang;
	arr=1;
	dina=vdina;

	double localtinyTemp;
	if(vg1dSP!=0)localtinyTemp=(*vg1dSP).localtiny;
	else localtinyTemp=1e-9;

	if(hol>localtinyTemp) reyL=dia*rl*uls/mil;
	else reyL=0.;
	if((1.-hol)>localtinyTemp) reyG=dia*rg*ugs/mig;
	else reyG=0.;
	reyM=dia*((1-hol)*rg+hol*rl)*(ugs+uls)/((1-hol)*mig+hol*mil);

	fl=fricl();
	fg=fricg();

	dpg=0.;
	dpl=0.;
	hidro=0.;

	map=vmap;

}

arranjo::arranjo(const arranjo& varr){

	dia=varr.dia;
	uls=varr.uls;
	ugs=varr.ugs;
	rl=varr.rl;
	rg=varr.rg;
	mil=varr.mil;
	mig=varr.mig;
	hol=varr.hol;
	ang=varr.ang;
	arr=varr.arr;
	reyL=varr.reyL;
    reyG=varr.reyG;
    reyM=varr.reyM;
	fl=varr.fl;
	fg=varr.fg;
	dina=varr.dina;
	dpg=varr.dpg;
	dpl=varr.dpl;
	hidro=varr.hidro;
	map=varr.map;
	vg1dSP=varr.vg1dSP;
}

arranjo& arranjo::operator =(const arranjo& varr){

	dia=varr.dia;
	uls=varr.uls;
	ugs=varr.ugs;
	rl=varr.rl;
	rg=varr.rg;
	mil=varr.mil;
	mig=varr.mig;
	hol=varr.hol;
	ang=varr.ang;
	arr=varr.arr;
	reyL=varr.reyL;
    reyG=varr.reyG;
    reyM=varr.reyM;
	fl=varr.fl;
	fg=varr.fg;
	dina=varr.dina;
	dpg=varr.dpg;
	dpl=varr.dpl;
	hidro=varr.hidro;
	map=varr.map;
	vg1dSP=varr.vg1dSP;

	return *this;

}


double arranjo::fricl(){

  if(reyL<1e-20)reyL=1e-20;
  double c;
  double n;
  if(reyL>2400){
    c=0.046;
    n=-0.2;
  }
  else{
    c=16.;
    n=-1.;
  }
  return c*pow(reyL,n);
}

double arranjo::fricg(){

  if(reyG<1e-20) reyG=1e-20;
  double c;
  double n;
  if(reyG>2400){
    c=0.046;
    n=-0.2;
  }
  else{
    c=16.;
    n=-1.;
  }
  return c*pow(reyG,n);
}

double arranjo::fricm(){

  if(reyM<1e-20)reyM=1e-20;
  double c;
  double n;
  if(reyM>2400){
    c=0.046;
    n=-0.2;
  }
  else{
    c=16.;
    n=-1.;
  }
  return c*pow(reyM,n);
}

double arranjo::zbrent(double x1,double x2,
                            double tol, double epsn, int maxit){
//Acha zero de fun��o ver NR
 double EPS=epsn;
 //double a=x1;
 //double b=x2;
 //double c=x2;
 double a=0.98;
 double b=0.05;
 double c=b;
 double fa=funcarranjo(a);
 double fb=funcarranjo(b);
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
   fb=funcarranjo(b);
 }
 //cout<<"Falsa corda atingiu n�mero m�ximo de itera��es"<<"\n";
 b=-1.;
 return b;
}


double arranjo::funcarranjo(double alfl){

	double alfl3=alfl*alfl*alfl;
	double den1=pow(1-alfl,1.5)*alfl;
	return den1*alfl3*hidro-(1+75.*alfl)*alfl3*dpg+den1*dpl;
}

int arranjo::verificaArr(){
//Barnea:
  //teste bubbly	alfa<0.25
	if(map==1){
		double fator=sqrt((rl-rg)*dina/(rl*rl*9.81));
		double crit1=dia-19.*fator;//positivo -> bolha
		double Cl=0.8;
		double gam=1.3;
		double U0=1.53*sqrt(9.81*fator);
		double sinbet=sinl(ang);
		double cosbet=cosl(ang);
		double crit2=0.75*0.70711*U0*U0*Cl*gam*gam*sinbet*sinbet/(9.81*dia)-cosbet;//positivo->bolha
		double crit3=uls-ugs/3.-1.53*0.75*sqrt(9.81*fator)*sinbet;
		if(fabsl(ugs+uls)>1e-10)crit3*=((uls+ugs))/fabsl((uls+ugs));//alfa=0.25 e positivo->bolha
		//bolha dispersa
		double fm=fricm();
		double j=fabsl(uls)+fabsl(ugs);
		double dcd=2*sqrt(0.4*dina/(9.81*(rl-rg)));
		double dcb=0.375*(rl/(rl-rg))*fm*j*j/(9.81*cosl(ang));
		double dc=dcd;
		if(dcb<dc)dc=dcb;
		double crit4;
		if(j>0)crit4=dc-(0.725+4.15*sqrtl(fabsl(ugs)/j))*pow(dina/rl,0.6)*pow(2*fm*j*j*j/dia,-0.4);//se positivo, bolha dispersa
		else crit4=-1.;
		//double crit5=uls-0.92308*ugs;
		double crit5=hol-0.48;
		if(crit1>=0 && crit2>=0 && crit3>=0)return 1;
		else if(crit4>0 && crit5>0) return 1;
		else{
			fl=fricl();
			fg=fricg();
			dpg=(4/dia)*fg*rg*ugs*fabsl(ugs)/2.;
			dpl=(4/dia)*fl*rl*uls*fabsl(uls)/2.;
			hidro=(rl-rg)*9.81*sinl(ang);
			//if(fabsl(ugs+uls)>1e-10)hidro*=(ugs+uls)/fabsl(ugs+uls);
			//double alfl=zbrent(0.0001,1);
			double alfl=hol;
			double crit6=hidro*alfl*alfl*alfl*(1.-1.5*alfl)-(2.-1.5*alfl)*dpl;
			double crit7=alfl-0.24;
			if(crit6>=0 || crit7>=0) return 2;
			else return -2;
		}
	}
	else{
	// simplificado
		double usgAnuH;
		double LocMat=1.6;
		double fl=fricl();
		double fg=fricg();
		usgAnuH=fabsl(uls)*sqrtl(fl*rl)/sqrtl(LocMat*LocMat*fg*rg);

		double uslBolH;
		uslBolH=9.7*sqrtl((rl-rg)*9.82/(2.*fl*rl/dia))/pow(dina/(9.82*dia*dia*(rl-rg)),-0.25);

		double uslBolV0;
		double uslBolDV0;
		uslBolV0=3.*fabsl(ugs)-1.15*pow(9.82*(rl-rg)*dina/(rl*rl),0.25);
		double term1=pow(dia,0.429)*pow(dina/rl,0.089)/pow(mil/rl,0.072);
		double term2=pow(9.82*(rl-rg)/rl,0.446);
		uslBolDV0=-fabsl(ugs)+4.*term1*term2;
		double uslBolV=uslBolV0;
		if(uslBolDV0<uslBolV)uslBolV=uslBolDV0;


		double usgAnuV;
		usgAnuV=3.1*pow(dina*9.82*(rl-rg),0.25)/sqrtl(rg);

		double usgAnuGen=cosl(ang)*usgAnuH+sinl(ang)*usgAnuV;
		double uslBolGen=cosl(ang)*uslBolH+sinl(ang)*uslBolV;

		if(hol>1-0.01)return 1;
		else if(fabsl(ugs)>usgAnuGen && hol<0.2) return -2;
		else if(fabsl(uls)>uslBolGen && hol>0.48) return 1;
		else return 2;
	}



}
