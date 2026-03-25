/*
 * PropFluCol.cpp
 *
 *  Created on: 21/12/2015
 *      Author: bwz1
 */
#include <algorithm>
#include <complex>

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

using namespace std;
#include "PropFluColVF.h"


ProFluColVF::ProFluColVF(double vrholStd,double vcompresP,double vcompresT, double vtenssup,double vcp,double vcond, double sal,double vTempL, double vLVisL,
	                                      double vTempH,double vLVisH,int vdescarga, int vtipo,double vrhogstd, double vrhog,
											int vtab, int vnTemp, int vnNew,int vnaoN ){

  rholStd=vrholStd;
  rhogStd=vrhogstd;
  rhog=vrhog;
  compresP=vcompresP;
  compresT=vcompresT;
  tenssup=vtenssup;
  cp=vcp;
  cond=vcond;
  TempL=vTempL;
  LVisL=vLVisL;
  TempH=vTempH;
  LVisH=vLVisH;
  salin=sal;
  injPoc=0;
  RhoInj=0;
  ViscInj=0;
  CondInj=0;
  CpInj=0;
  DrhoDtInj=0;
  npontos=0;
  descarga=vdescarga;
  tipoF=vtipo;
  TR=0.;
  tabelaVisc=vtab;
  naoN=vnaoN;
  nTemp=vnTemp;
  nDeform=vnNew;
  viscNaoNew=0;
  if(tabelaVisc>0){
	  viscNaoNew=new double* [nTemp+1];
	  if(naoN==0)nDeform=1;
	  for(int i=0;i<nTemp+1;i++)viscNaoNew[i]=new double [nDeform+1];
  }
}

ProFluColVF::ProFluColVF(const ProFluColVF& fluido){ //construtor por c�pia
	  rholStd=fluido.rholStd;
	  rhogStd=fluido.rhogStd;
	  rhog=fluido.rhog;
	  compresP=fluido.compresP;
	  compresT=fluido.compresT;
	  tenssup=fluido.tenssup;
	  cp=fluido.cp;
	  cond=fluido.cond;
	  TempL=fluido.TempL;
	  LVisL=fluido.LVisL;
	  TempH=fluido.TempH;
	  LVisH=fluido.LVisH;
	  salin=fluido.salin;
	  injPoc=fluido.injPoc;
	  RhoInj=fluido.RhoInj;
	  ViscInj=fluido.ViscInj;
	  CondInj=fluido.CondInj;
	  CpInj=fluido.CpInj;
	  DrhoDtInj=fluido.DrhoDtInj;
	  npontos=fluido.npontos;
	  descarga=fluido.descarga;
	  tipoF=fluido.tipoF;
	  TR=fluido.TR;
	  tabelaVisc=fluido.tabelaVisc;
	  nTemp=fluido.nTemp;
	  nDeform=fluido.nDeform;
	  naoN=fluido.naoN;
	  viscNaoNew=0;
	  if(tabelaVisc>0){
		  viscNaoNew=new double* [nTemp+1];
		  if(naoN==0)nDeform=1;
		  for(int i=0;i<nTemp+1;i++){
			  viscNaoNew[i]=new double [nDeform+1];
			  for(int j=0;j<nDeform+1;j++){
				  viscNaoNew[i][j]=fluido.viscNaoNew[i][j];
			  }
		  }
	  }
}


ProFluColVF& ProFluColVF::operator =(const ProFluColVF& fluido){//iguala duas listas de primitivas
 if(this!=&fluido){//verifica se se est� igualando a mesma lista
	  rholStd=fluido.rholStd;
	  rhogStd=fluido.rhogStd;
	  rhog=fluido.rhog;
	  compresP=fluido.compresP;
	  compresT=fluido.compresT;
	  tenssup=fluido.tenssup;
	  cp=fluido.cp;
	  cond=fluido.cond;
	  TempL=fluido.TempL;
	  LVisL=fluido.LVisL;
	  TempH=fluido.TempH;
	  LVisH=fluido.LVisH;
	  salin=fluido.salin;
	  injPoc=fluido.injPoc;
	  RhoInj=fluido.RhoInj;
	  ViscInj=fluido.ViscInj;
	  CondInj=fluido.CondInj;
	  CpInj=fluido.CpInj;
	  DrhoDtInj=fluido.DrhoDtInj;
	  npontos=fluido.npontos;
	  descarga=fluido.descarga;
	  tipoF=fluido.tipoF;
	  TR=fluido.TR;
	  if (tabelaVisc>0) {
		  if(nTemp>0){
			  for(int i=0;i<nTemp+1;i++) delete[] viscNaoNew[i];
			  delete[] viscNaoNew;
		  }
	  }
	  tabelaVisc=fluido.tabelaVisc;
	  nTemp=fluido.nTemp;
	  nDeform=fluido.nDeform;
	  naoN=fluido.naoN;
	  viscNaoNew=0;
	  if(tabelaVisc>0 && nTemp>0 && nDeform>0){
		  viscNaoNew=new double* [nTemp+1];
		  if(naoN==0)nDeform=1;
		  for(int i=0;i<nTemp+1;i++){
			  viscNaoNew[i]=new double [nDeform+1];
			  for(int j=0;j<nDeform+1;j++){
				  viscNaoNew[i][j]=fluido.viscNaoNew[i][j];
			  }
		  }
	  }
 }
 return *this;
}

double ProFluColVF::operator[](int i) const{//sobrecarga do operador []
 if(i==0)return rholStd;
 else if(i==1)return compresP;
 else if(i==2)return compresT;
 else if(i==3)return tenssup;
 else if(i==4)return cp;
 else if(i==5)return cond;
 else if(i==6)return TempL;
 else if(i==7)return LVisL;
 else if(i==8)return TempH;
 else if(i==9)return LVisH;
 else return -100.;

 }

double ProFluColVF::MasEspFlu(double pres, double temper) const{
  if((injPoc<=1 || injPoc>3) && descarga==0)return rholStd+0.*(1.-compresT*(temper-20.))*expl((pres-1.033211)*98066.52*compresP);//alteracao2
  else if(injPoc==2 || descarga==1 || tipoF==1){
	  double tfarAmb=Faren(20.);
	  double ppsiAmb=psia(1.);
	  double tfar=Faren(temper);
	  double ppsi=psia(pres);
	  double bwAmb = 1.0 + 1.2e-04*(tfarAmb - 60.0) + 1.0e-06*pow(tfarAmb - 60.0,2.) - 3.33e-06*ppsiAmb;
	  double bw = 1.0 + 1.2e-04*(tfar - 60.0) + 1.0e-06*pow(tfar - 60.0,2.) - 3.33e-06*ppsi;

	  double rholw=(1000./bw)*bwAmb;
	  double rhosal=2160.;
	  double x=salin/1000.;
	  double rhomist=(1-x)/rholw+x/rhosal;
	  return 1./rhomist;
  }
  else{
	  if(pres<1.02 && temper<20.1) return rholStd;
	  else return interpolaVarInj(pres, temper, RhoInj);
  }
}

double ProFluColVF::VisFlu(double pres, double temper, double deform) const{
//Viscosidade de �leo morto pelo m�todo ASTM
//Viscosidade de �leo morto em fun��o da temperatura em Celcius, viscosidade em cP


	if(tabelaVisc==0){
		if((injPoc<=1 || injPoc>3) && descarga==0){
			double rhol=rholStd/1000.;
			double bASTM = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)))/log10l((TempL + 273)/(TempH + 273));

			double visASTM=rhol*(pow(10,pow(10,(bASTM*log10l((temper + 273)/(TempL + 273))+log10l(log10l((LVisL/1)/rhol + 0.7)))))-0.7);

			return visASTM;
		}
		else if(injPoc==2 || descarga==1 || tipoF==1){
			double vis= 2.414E-05*pow(10.0,247.8/(temper + 133.15));
			return vis*1000.;
		}
		else return interpolaVarInj(pres, temper, ViscInj)*1000.;
	}
	else{
		int posTe=0;
		int posTd=0;
		int posDe=0;
		int posDd=0;
		if(temper<viscNaoNew[1][0]){
			posTe=1;
			posTd=1;
		}
		else if(temper>viscNaoNew[nTemp][0]){
			posTe=nTemp;
			posTd=nTemp;
		}
		else{
			for(int itemp=1; itemp<nTemp;itemp++){
				if(temper>=viscNaoNew[itemp][0] && temper<=viscNaoNew[itemp+1][0]){
					posTe=itemp;
					posTd=itemp+1;
				}
			}
			if(nDeform>1){
				if(deform<viscNaoNew[0][1]){
					posDe=1;
					posDd=1;
				}
				else if(deform>viscNaoNew[0][nDeform]){
					posDe=nDeform;
					posDd=nDeform;
				}
				else{
					for(int ideform=1; ideform<nDeform;ideform++){
						if(deform>=viscNaoNew[0][ideform] && deform<=viscNaoNew[0][ideform+1]){
							posDe=ideform;
							posDd=ideform+1;
						}
					}
				}
			}
			else{
				posDe=1;
				posDd=1;
			}
		}
		double razT;
		double razD;
		if(posTe==posTd)razT=1.;
		else razT=(temper-viscNaoNew[posTe][0])/(viscNaoNew[posTd][0]-viscNaoNew[posTe][0]);
		if(posDe==posDd)razD=1.;
		else razD=(deform-viscNaoNew[0][posDe])/(viscNaoNew[0][posDd]-viscNaoNew[0][posDe]);
		double viscE=(1.-razT)*viscNaoNew[posTe][posDe]+razT*viscNaoNew[posTd][posDe];
		double viscD=(1.-razT)*viscNaoNew[posTe][posDd]+razT*viscNaoNew[posTd][posDd];
		return (1.-razD)*viscE+razD*viscD;
	}
}

double ProFluColVF::VisGas(double pres,double temp) const{
     double TF=Faren(temp);
     double TR=TF+459.67;
     double Ppsi=psia(pres);
     double wg=(rhogStd/1.29)*29;
     double AK=(9.4+0.02*wg)*pow(TR,1.5)/(209.+19.*wg+TR);
     double x=3.5+(986/TR)+0.01*wg;
     double y=2.4-0.2*x;
     double visc=AK*expl(x*pow(rhog,y))/10000.;
     return visc;
}

double ProFluColVF::CondLiq(double pres,double temp) const{
	if((injPoc<=1 || injPoc>3) && descarga==0) return cond;
	else if(injPoc==2 || descarga==1 || tipoF==1){
		return 5.64981e-01 + (1.75148e-03 - 6.21225e-06*temp)*temp;
	}
	else return interpolaVarInj(pres, temp, CondInj);
}

double ProFluColVF::CalorLiq(double pres,double temp) const{
	if((injPoc<=1 || injPoc>3) && descarga==0) return cp;
	else if(injPoc==2 || descarga==1 || tipoF==1){
		double ValCp;
		if (temp< 136.85)
		  ValCp= 4185.5*(1.00536 + (-4.31108e-04 + (7.01150e-06 - 2.42139e-08*temp)*temp)*temp);
		 else
		  ValCp= 4185.5*(0.189300 + (1.30609e-02 + (-6.89620e-05 + 1.29156e-07*temp)*temp)*temp);
		return ValCp;
	}
	else return interpolaVarInj(pres, temp, CpInj);
}

double ProFluColVF::DrhoDtFlu(double pres,double temp) const{
	if((injPoc<=1 || injPoc>3) && descarga==0) return 0.;
	else if(injPoc==2 || descarga==1 || tipoF==1) return 0.;
	else return interpolaVarInj(pres, temp, DrhoDtInj);
}

double ProFluColVF::interpolaVarInj(double pres, double temp, double** VarInj) const{
    int ipres=0;
    int itemp=0;
    int ipmarcador;
    int itmarcador;
    double varinj=0.;
    int ndiv=npontos-1;
    if(pres<VarInj[1][0])pres=VarInj[1][0];
    if(pres>=VarInj[ndiv+1][0])pres=VarInj[ndiv+1][0];
    if(temp<VarInj[0][1])temp=VarInj[0][1];
    if(temp>=VarInj[0][ndiv+1])temp=VarInj[0][ndiv+1];

   // if(pres<VarInj[1][0] || pres>=VarInj[ndiv+1][0] || temp<VarInj[0][1] || temp>=VarInj[0][ndiv+1]) varinj=0.;

    else{

      int e, m, d;
      e = 1;
      d = ndiv+1;
      while (e <= d) {
	      m = (e + d)/2;
	      ipmarcador=m;
 	      if(m==1){
 	    	  ipres=m;
 	    	  break;
 	      }
 	      else if (m==ndiv+1 && VarInj[m][0] == pres){
 	    	ipres=m-1;
 	    	break;
 	      }
	      if (VarInj[m][0] > pres && VarInj[m-1][0]<=pres){
	    	  ipres=m-1;
	    	  break;
	      }
	      if (VarInj[m][0] < pres) e = m + 1;
	      else d = m - 1;
	  }
      e = 1;
      d = ndiv+1;
      while (e <= d) {
 	      m = (e + d)/2;
 	      itmarcador=m;
 	      if(m==1){
 	    	  itemp=m;
 	    	  break;
 	      }
 	      else if (m==ndiv+1 && VarInj[0][m] == temp){
 	    	itemp=m-1;
 	    	break;
 	      }
 	      if (VarInj[0][m] > temp && VarInj[0][m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (VarInj[0][m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	  }
      double razpres=(VarInj[ipres][0]-pres)/(VarInj[ipres][0]-VarInj[ipres+1][0]);
      double raztemp=(VarInj[0][itemp]-temp)/(VarInj[0][itemp]-VarInj[0][itemp+1]);
      double varp1=(1-razpres)*(VarInj[ipres][itemp])+razpres*(VarInj[ipres+1][itemp]);
      double varp2=(1-razpres)*(VarInj[ipres][itemp+1])+razpres*(VarInj[ipres+1][itemp+1]);
      varinj=(1-raztemp)*varp1+raztemp*varp2;
     }
    return varinj;
}

double ProFluColVF::calcDR(double reL){
	double dR0;
	double fatB;
	if(reL>=2.82*1.e4){
		dR0=sqrtl((2.5/100)*reL-704.156)/100.;
		fatB=-1.59e-6*(reL-2.82e4);
	}
	else{
		dR0=0.;
		fatB=0.;
	}
	return dR0*expl(fatB*TR/60);
}

//template class ProFluColVF;
