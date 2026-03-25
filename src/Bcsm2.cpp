#include "Bcsm2.h"

BomCentSub::BomCentSub(int nC,const double* const Vvaz, const double* const Vhead,
                                const double* const Vpower,const double* const Vefic, double Vfreq, int Vnestag, double VeficM,
								double vfreqMinima, int vcorrecHI){//construtor

	if(nC>0){
        nC--;
 	    vaz = new double [lenth=nC+1];
        head = new double [nC+1];
        power = new double [nC+1];
        efic = new double [nC+1];
        d2head = new double [nC];
        d2power= new double [nC];
        d2efic = new double [nC];
        freq=Vfreq;
        freqnova=freq;
        nestag=Vnestag;
        eficM=VeficM;
        freqMinima=vfreqMinima;
        correcHI=vcorrecHI;

	for (int i=0; i<lenth; i++){
         vaz[i]= *(Vvaz+i);//copiando de uma �rea apontada por um ponteiro
         head[i]= *(Vhead+i);
         power[i]= *(Vpower+i);
         efic[i]= *(Vefic+i);
        }

        double p;
        double sig;

        double* uh;
        double* up;
        double* ue;
 	    uh = new double [nC-1];
        up = new double [nC-1];
        ue = new double [nC-1];


        d2head[0]=d2power[0]=d2efic[0]=0;
        uh[0]=up[0]=ue[0]=0;

        for(int i=1;i<nC-1;i++){

           sig=(vaz[i]-vaz[i-1])/(vaz[i+1]-vaz[i-1]);

           p=sig*d2head[i-1]+2.0;
           d2head[i]=(sig-1.0)/p;
           uh[i]=(head[i+1]-head[i])/(vaz[i+1]-vaz[i])-(head[i]-head[i-1])/(vaz[i]-vaz[i-1]);
           uh[i]=(6.0*uh[i]/(vaz[i+1]-vaz[i-1])-sig*uh[i-1])/p;

           p=sig*d2power[i-1]+2.0;
           d2power[i]=(sig-1.0)/p;
           up[i]=(power[i+1]-power[i])/(vaz[i+1]-vaz[i])-(power[i]-power[i-1])/(vaz[i]-vaz[i-1]);
           up[i]=(6.0*up[i]/(vaz[i+1]-vaz[i-1])-sig*up[i-1])/p;

           p=sig*d2efic[i-1]+2.0;
           d2efic[i]=(sig-1.0)/p;
           ue[i]=(efic[i+1]-efic[i])/(vaz[i+1]-vaz[i])-(efic[i]-efic[i-1])/(vaz[i]-vaz[i-1]);
           ue[i]=(6.0*ue[i]/(vaz[i+1]-vaz[i-1])-sig*ue[i-1])/p;
        }

        d2head[nC-1]= d2power[nC-1]=d2efic[nC-1]=0;
        for(int k=nC-2;k>=0;k--){
         d2head[k]=d2head[k]*d2head[k+1]+uh[k];
         d2power[k]=d2power[k]*d2power[k+1]+up[k];
         d2efic[k]=d2efic[k]*d2efic[k+1]+ue[k];
        }

        double maxef=efic[0];
        int k=1;
        while(maxef<efic[k]&&k<lenth-1){maxef=efic[k];k++;}
        BEP=FalsaCorda(1,vaz[k-2],vaz[k]);
        if(BEP<0)BEP=vaz[k-1];
        //BEP=FalsaCorda(1,vaz[0],vaz[lenth-1]);
        Qzero=FalsaCorda(0,vaz[lenth-2],vaz[lenth-1]);

        Hvis=0.;
        Pvis=0.;
        Evis=0.;

        delete[] uh;
        delete[] up;
        delete[] ue;
       }
       else{
        lenth=0;
	    vaz=0;
        head=0;
        power=0;
        efic=0;
        d2head=0;
        d2power=0;
        d2efic=0;
        BEP=0;
        Qzero=0;
        freq=0;
        Hvis=0.;
        Pvis=0.;
        Evis=0.;
        freqMinima=0.;
        eficM=0.;
        correcHI=0;
       }
}
BomCentSub::BomCentSub(const BomCentSub& bombaantiga){
//construtor de c�pia

        int n=bombaantiga.lenth;
        n--;
	    vaz = new double [lenth=n+1];
        head = new double [n+1];
        power = new double [n+1];
        efic = new double [n+1];
        d2head = new double [n];
        d2power= new double [n];
        d2efic = new double [n];

	for (int i=0; i<lenth; i++){
         vaz[i]= bombaantiga.vaz[i];
         head[i]= bombaantiga.head[i];
         power[i]= bombaantiga.power[i];
         efic[i]= bombaantiga.efic[i];
        }
	for (int i=0; i<lenth-1; i++){
         d2head[i]= bombaantiga.d2head[i];
         d2power[i]= bombaantiga.d2power[i];
         d2efic[i]= bombaantiga.d2efic[i];
        }
        BEP=bombaantiga.BEP;
        Qzero=bombaantiga.Qzero;
        freq=bombaantiga.freq;
        freqnova=bombaantiga.freqnova;
        nestag=bombaantiga.nestag;
        freqMinima=bombaantiga.freqMinima;
        eficM=bombaantiga.eficM;
        Hvis=bombaantiga.Hvis;
        Pvis=bombaantiga.Pvis;
        Evis=bombaantiga.Evis;
        correcHI=bombaantiga.correcHI;
}

BomCentSub& BomCentSub::operator =(const BomCentSub& bombaantiga){
	if(this!=&bombaantiga){
		int n=bombaantiga.lenth;
        n--;
        if(lenth>0){
        	delete[] vaz;
        	delete[] head;
        	delete[] power;
        	delete[] efic;
        	delete[] d2head;
        	delete[] d2power;
        	delete[] d2efic;
        }
        if(n>-1){
        	vaz = new double [lenth=n+1];
        	head = new double [n+1];
        	power = new double [n+1];
        	efic = new double [n+1];
        	d2head = new double [n];
        	d2power= new double [n];
        	d2efic = new double [n];
        	freq=bombaantiga.freq;
        	freqnova=bombaantiga.freqnova;
        	nestag=bombaantiga.nestag;
        	eficM=bombaantiga.eficM;
        	freqMinima=bombaantiga.freqMinima;
        	correcHI=bombaantiga.correcHI;

        	for (int i=0; i<lenth; i++){
        		vaz[i]= bombaantiga.vaz[i];
        		head[i]= bombaantiga.head[i];
        		power[i]= bombaantiga.power[i];
        		efic[i]= bombaantiga.efic[i];
        	}
        	for (int i=0; i<lenth-1; i++){
        		d2head[i]= bombaantiga.d2head[i];
        		d2power[i]= bombaantiga.d2power[i];
        		d2efic[i]= bombaantiga.d2efic[i];
        	}
        	BEP=bombaantiga.BEP;
        	Qzero=bombaantiga.Qzero;
        	Hvis=bombaantiga.Hvis;
        	Pvis=bombaantiga.Pvis;
        	Evis=bombaantiga.Evis;
        }
        else{
                lenth=0;
        	vaz=0;
                head=0;
                power=0;
                efic=0;
                d2head=0;
                d2power=0;
                d2efic=0;
                BEP=0;
                Qzero=0;
                freq=0;
                Hvis=0.;
                Pvis=0.;
                Evis=0.;
                freqMinima=0.;
                eficM=0.;
                correcHI=1;
               }
	}
    return *this;
}


double BomCentSub::Fhead(double xvaz) const{
 int klo=0;
 int khi;
 int mult=1.;
 if(freqnova<freqMinima)mult=0.;
 if(xvaz<0 || xvaz>Qzero) return 0.;
 else{
   while(xvaz>vaz[klo]&&klo<lenth-1)klo++;
   if(xvaz<=vaz[0])return mult*nestag*(head[0]+(head[1]-head[0])*(xvaz-vaz[0])/(vaz[1]-vaz[0]))*pow(freqnova/freq,2.);
   else if(klo>lenth-2){
      double h=vaz[lenth-1]-vaz[lenth-2];
      double a=(vaz[lenth-1]-xvaz)/h;
      double b=(xvaz-vaz[lenth-2])/h;
      double deri= (head[lenth-1]-head[lenth-2])/(vaz[lenth-1]-vaz[lenth-2])-
             (2*a*a-1)*(vaz[lenth-1]-vaz[lenth-2])*d2head[lenth-2]/6+
             (3*b*b-1)*(vaz[lenth-1]-vaz[lenth-2])*d2head[lenth-2]/6;
      return mult*nestag*(head[lenth-1]+deri*(xvaz-vaz[lenth-1]))*pow(freqnova/freq,2.);
   }
   else{

        klo--;
        khi=klo+1;
        double h=vaz[khi]-vaz[klo];
        double a=(vaz[khi]-xvaz)/h;
        double b=(xvaz-vaz[klo])/h;
        double y=a*head[klo]+b*head[khi]+((a*a*a-a)*d2head[klo]+(b*b*b-b)*d2head[khi])*(h*h)/6.;
        return mult*nestag*y*pow(freqnova/freq,2.);
   }
  }
}

double BomCentSub::Fpower(double xvaz) const{
 int klo=0;
 int khi;
 int mult=1.;
 if(freqnova<freqMinima)mult=0.;
 if(xvaz<0 || xvaz>Qzero) return 0.;
 else{
   while(xvaz > vaz[klo] && klo < lenth-1) klo++;
   if(xvaz<=vaz[0])return mult*nestag*(power[0]+(power[1]-power[0])*(xvaz-vaz[0])/(vaz[1]-vaz[0]))*pow(freqnova/freq,3.);
   else if(klo>lenth-2){
      double h=vaz[lenth-1]-vaz[lenth-2];
      double a=(vaz[lenth-1]-xvaz)/h;
      double b=(xvaz-vaz[lenth-2])/h;
      double deri= (power[lenth-1]-power[lenth-2])/(vaz[lenth-1]-vaz[lenth-2])-
             (2*a*a-1)*(vaz[lenth-1]-vaz[lenth-2])*d2power[lenth-2]/6+
             (3*b*b-1)*(vaz[lenth-1]-vaz[lenth-2])*d2power[lenth-2]/6;
      return mult*nestag*(power[lenth-1]+deri*(xvaz-vaz[lenth-1]))*pow(freqnova/freq,3.);
   }
   else{
     klo--;
     khi=klo+1;
     double h=vaz[khi]-vaz[klo];
     double a=(vaz[khi]-xvaz)/h;
     double b=(xvaz-vaz[klo])/h;
     double y=a*power[klo]+b*power[khi]+((a*a*a-a)*d2power[klo]+(b*b*b-b)*d2power[khi])*(h*h)/6.;
     return mult*nestag*y*pow(freqnova/freq,3.);
   }
 }
}

double BomCentSub::Fefic(double xvaz) const{
 int klo=0;
 int khi;
 int mult=1.;
 if(freqnova<freqMinima)mult=1.;
 if(xvaz<0 || xvaz>Qzero) return 0.;
 else{
   while(xvaz>vaz[klo]&&klo<lenth-1)klo++;
   if(xvaz<=vaz[0])return mult*efic[0]+(efic[1]-efic[0])*(xvaz-vaz[0])/(vaz[1]-vaz[0]);
   else if(klo>lenth-2){
      double h=vaz[lenth-1]-vaz[lenth-2];
      double a=(vaz[lenth-1]-xvaz)/h;
      double b=(xvaz-vaz[lenth-2])/h;
      double deri= (efic[lenth-1]-efic[lenth-2])/(vaz[lenth-1]-vaz[lenth-2])-
           (2*a*a-1)*(vaz[lenth-1]-vaz[lenth-2])*d2efic[lenth-2]/6+
           (3*b*b-1)*(vaz[lenth-1]-vaz[lenth-2])*d2efic[lenth-2]/6;
      return mult*efic[lenth-1]+deri*(xvaz-vaz[lenth-1]);
   }
   else{
     klo--;
     khi=klo+1;
     double h=vaz[khi]-vaz[klo];
     double a=(vaz[khi]-xvaz)/h;
     double b=(xvaz-vaz[klo])/h;
     double y=a*efic[klo]+b*efic[khi]+((a*a*a-a)*d2efic[klo]+(b*b*b-b)*d2efic[khi])*(h*h)/6.;
     return mult*y;
   }
 }
}

double BomCentSub::Defic(double xvaz) const{
 int klo=0;
 int khi;
 while(xvaz>vaz[klo]&&klo<lenth-1)klo++;
 if(xvaz<=vaz[0])return (efic[1]-efic[0])/(vaz[1]-vaz[0]);
 else if(klo>lenth-2){
    double h=vaz[lenth-1]-vaz[lenth-2];
    double a=(vaz[lenth-1]-xvaz)/h;
    double b=(xvaz-vaz[lenth-2])/h;
    return (efic[lenth-1]-efic[lenth-2])/(vaz[lenth-1]-vaz[lenth-2])-
           (2*a*a-1)*(vaz[lenth-1]-vaz[lenth-2])*d2efic[lenth-2]/6+
           (3*b*b-1)*(vaz[lenth-1]-vaz[lenth-2])*d2efic[lenth-2]/6;
 }
 else{
    klo--;
    khi=klo+1;
    double h=vaz[khi]-vaz[klo];
    double a=(vaz[khi]-xvaz)/h;
    double b=(xvaz-vaz[klo])/h;
    return (efic[khi]-efic[klo])/(vaz[khi]-vaz[klo])-
           (2*a*a-1)*(vaz[khi]-vaz[klo])*d2efic[klo]/6+
           (3*b*b-1)*(vaz[khi]-vaz[klo])*d2efic[klo]/6;
 }
}

double BomCentSub::FalsaCorda(int qfunc,double a,double b,double delta ,double epsn,int maxit)const{
        //a e b-> chutes iniciais para a falsa corda
        //epsn -> limite em que se admite F(double) aproximadamente zero
        //delta-> limite a partir do qual admite-se que j� n�o existe varia��o no c�lculo da raiz
        //maxit-> m�ximo n�mero de itera��es permitido no c�lculo
        //F(double)-> fun��o que se quer obter a raiz
          double u=(qfunc==1)?Defic(a):Fhead(a)/nestag;
          double e=b-a;
          double c;

          double v=(qfunc==1)?Defic(b):Fhead(b)/nestag;
          if(u!=0 && v!=0){//neste primeiro bloco, busca-se assegurar que sempre se estra� trabalhando com valores de fun��o negativa nos chutes iniciais para se garantir maior converg�ncia na falsa corda
           if(u/v>0){
            if(u>0){
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u<proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             int konta=0;
             while(proy>0){
              prox=prox-del/10; proy=(qfunc==1)?Defic(prox):Fhead(prox)/nestag;
              if(fabs(proy)<1e-10)return prox;
              konta++;
              if(konta>=maxit) return -1.;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
            else{
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u>proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             int konta=0;
             while(proy<0){
              prox=prox-del/10; proy=(qfunc==1)?Defic(prox):Fhead(prox)/nestag;
              if(fabs(proy)<1e-10)return prox;
              konta++;
              if(konta>=maxit) return -1.;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
           }
          }


          for(int k=1;k<=maxit;k++){//este bloco trata da falsa corda propriamente dita
              e*=0.5;
              c=a+e;
              double w=(qfunc==1)?Defic(c):Fhead(c)/nestag;
              if(fabs(e)<delta||fabs(w)<epsn) return c;
              ((u>0&&w<0)||(u<0&&w>0))?(b=c):(a=c,u=w);
          }
          cout<<"Falsa corda em Bomba atingiu n�mero m�ximo de itera��es"<<"\n" ;
          c=-1;
          return c;
}

void BomCentSub::NovaVis(double vis, double MasEsp, double Qvis){


	   /* double visC=vis*1000/MasEsp;
        double Y=-112.1374+6.6504*logl(Fhead(BEP)/nestag)+12.8429*logl(BEP);
        double Qast=exp((39.52766+26.5605*logl(visC)-Y)/51.6565);
        double Cq=1-4.0327*pow(10,-3)*Qast-1.724*pow(10,-4)*Qast*Qast;
        double Coeff=1-3.3075*pow(10,-2)*Qast+2.8875*pow(10,-4)*Qast*Qast;
        double Ch1=1-3.68*pow(10,-3)*Qast-4.36*pow(10,-5)*Qast*Qast;
        double Ch2=1-4.4723*pow(10,-3)*Qast-4.18*pow(10,-5)*Qast*Qast;
        double Ch3=1-7.0076*pow(10,-3)*Qast-1.41*pow(10,-5)*Qast*Qast;
        double Ch4=1-9.01*pow(10,-3)*Qast-1.31*pow(10,-5)*Qast*Qast;

        double q0=0;
        double q1=Cq*(BEP);
        double q0p6=0.6*q1;
        double q0p8=0.8*q1;
        double q1p2=1.2*q1;
        double qmax=(Qzero/0.16)*Cq;

        double h0=Fhead(0)/nestag;
        double h1=Ch1*Fhead(q0p6)/nestag;
        double h2=Ch2*Fhead(q0p8)/nestag;
        double h3=Ch3*Fhead(q1)/nestag;
        double h4=Ch4*Fhead(q1p2)/nestag;
        double hmax=0;

        double e0=Fefic(0);
        double e1=Coeff*Fefic(q0p6);
        double e2=Coeff*Fefic(q0p8);
        double e3=Coeff*Fefic(q1);
        double e4=Coeff*Fefic(q1p2);
        double emax=0;

        double multip=(9.81/(1/100.))*0.001341022*0.3048*0.159/86400.;
        double p0=Fpower(0)/nestag;
        double p1=(h1*q0p6*MasEsp/e1)*multip;
        double p2=(h2*q0p8*MasEsp/e2)*multip;
        double p3=(h3*q1*MasEsp/e3)*multip;
        double p4=(h4*q1p2*MasEsp/e4)*multip;
        double pmax=p4+(p4-p3)*(qmax-q1p2)/(q1p2-q1);

        double vq[]={q0,q0p6,q0p8,q1,q1p2,qmax};
        double vh[]={h0,h1,h2,h3,h4,hmax};
        double ve[]={e0,e1,e2,e3,e4,emax};
        double vp[]={p0,p1,p2,p3,p4,pmax};
        BomCentSub bombaviscosa(6,vq,vh,vp,ve,freq,nestag,eficM);
        bombaviscosa.freqnova=freqnova;
        bombaviscosa.BEP=BEP;

        //BomCentSub bombaviscosa(*this);

        return bombaviscosa;*/

	 if(freqnova>=freqMinima){
	    double Qbep=BEP;
	    double visC=vis*1000/MasEsp;
	    double Hbep=Fhead(BEP)/nestag;
	    double Nrpm=freqnova*60;
	    double Bhi=26.6*pow(visC,0.5)*pow(Hbep,0.0625)/(pow(Qbep*(42./144.),0.375)*pow(Nrpm, 0.25));
	    double HbepV;
	    double Qw;
	    double Cq;
	    double CbepH;
	    double Ch;
	    double Ce;

	    if(correcHI==1){
	    	if(Bhi<=1.){
	    		Qw=Qvis;
	    		Hvis=Fhead(Qw);
	    		Evis=Fefic(Qw);
	    		Pvis=Fpower(Qw);
	    	}
	    	else{
	    		double expoente=-0.165*pow(log10(Bhi),3.15);
	    		Cq=pow(2.71,expoente);
	    		Qw=Qvis/Cq;
	    		CbepH=Cq;
	    		HbepV=CbepH*Hbep;
	    		Ch=1.-((1-CbepH)*pow(Qw/BEP,0.75));
	    		Hvis=Ch*Fhead(Qw);
	    		expoente=-0.0547*pow(Bhi,0.69);
	    		Ce=pow(Bhi,expoente);
	    		Evis=Ce*Fefic(Qw);
	    		if(Evis>1e-5){
	    			Pvis=(42./144.)*Qvis*Hvis*(MasEsp/1000.)/(3960*(Evis)/100);
	    		}
	    		else Pvis=0.;
	    	}
	    }
	    else{
    		Qw=Qvis;
    		Hvis=Fhead(Qw);
    		Evis=Fefic(Qw);
    		Pvis=Fpower(Qw);
	    }
	 }
	 else{
	    	Hvis=0.;
	    	Evis=100.;
	    	Pvis=0.;
	 }



}

ostream& operator<<(ostream& s, const BomCentSub& vb){
	int n=vb.size();
        double vhpe;
	for ( int i = 0; i<n; i++){
        vhpe=vb[i];
        s.width(6);
        s.precision(5);
        s<<"Vaz�o BPD = "<<vhpe<<"  "<<"HEAD ft = "<<vb.Fhead(vhpe)/vb.nestag<<"  "<<"Pot�ncia hp="<<vb.Fpower(vhpe)/vb.nestag<<"  "<<"efici�ncia ="<<vb.Fefic(vhpe)<<"\n";
        }
        double QE=vb.MelhorVaz();
        double QM=vb.MaxVaz();
        s<<"M�xima Efici�ncia:"<<"\n";
        s.width(6);
        s.precision(5);
        s<<"Vaz�o BPD = "<<QE<<"  "<<"HEAD ft = "<<vb.Fhead(QE)/vb.nestag<<"  "<<"Pot�ncia hp="<<vb.Fpower(QE)/vb.nestag<<"  "<<"efici�ncia ="<<vb.Fefic(QE)<<"\n";
        s<<"M�xima Vaz�o:"<<"\n";
        s.width(6);
        s.precision(5);
        s<<"Vaz�o BPD = "<<QM<<"  "<<"HEAD ft = "<<vb.Fhead(QM)/vb.nestag<<"  "<<"Pot�ncia hp="<<vb.Fpower(QM)/vb.nestag<<"  "<<"efici�ncia ="<<vb.Fefic(QM)<<"\n";
        s<<"\n";
	for ( int i = 0; i<n-1; i++){
        s.width(6);
        s.precision(5);
        s<<"seg. der. head = "<< vb.segundaH(i)<<"  "<<"seg. der. efic. = "<<vb.segundaE(i)<<"  "<<"seg. der. pot.="<<vb.segundaP(i)<<"\n";
        }
       	return s;
}

ostream& BomCentSub::curva(ostream& s, double Vfreq, double estag, int npontos) const{
   double dvaz=Qzero/(npontos-1);
   double Vvaz=0;
   for ( int i = 0; i<npontos; i++){
        s.width(6);
        s.precision(5);
        s<<"Vaz�o BPD =  "<<Vvaz<<" HEAD ft =  "<<Fhead(Vvaz)/nestag<<" Pot�ncia hp= "<<Fpower(Vvaz)/nestag<<" efici�ncia = "<<Fefic(Vvaz)<<"\n";
        Vvaz=Vvaz+dvaz;
        }
        s<<"\n";
  return s;
}
 
//template class BomCentSub;
