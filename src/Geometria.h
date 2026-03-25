#ifndef _GEOM
#define _GEOM

#define _USE_MATH_DEFINES // para MPI
#include <math.h>

using namespace std;

class DadosGeo{ //Vcr � um template para vetores
	public:
        double a; //Altura do canal (metros)
        double b;  // Largura do Canal (metros)
        double teta; //inclina��o da linha em radianos
        double area;
        double dia;
        double rug;
        double peri;
        int ncamadas;
        int* indmat;
        double* cond;//condutividade da tubula��o
        double* diamC;
        double* espessuR;
        double* cp;
        double* rhoC;
        double* visc;
        double* beta;
        int* tipomat;
        int revest;
        int indD0;
        DadosGeo(double va=0 ,double vb=0, double vang=0, double vrug=0.,int vrevest=0, int vn=0,
        		 double* vcond=0, double* vdiamC=0,double* vcp=0,
        		 double* vrhoc=0, double* vvisc=0,  double* vbeta=0, int* vtipomat=0, int* vindmat=0){
                 a=va;
                 b=vb;
                 dia=a;
                 teta=vang;
                 rug=vrug;
                 indD0=0;
                 double MPI=3.14159265359;

                 if(vn>0){
                   ncamadas=vn;
                   cond = new double [ncamadas];
                   diamC = new double [ncamadas];
                   espessuR = new double [ncamadas];
                   cp = new double [ncamadas];
                   rhoC = new double [ncamadas];
                   visc = new double [ncamadas];
                   beta = new double [ncamadas];
                   tipomat = new int [ncamadas];
                   indmat = new int [ncamadas];
                   for(int i=0;i<ncamadas;i++){
                     cond[i]=vcond[i];
                     diamC[i] =vdiamC[i];
                     if(i>0)espessuR[i]=(diamC[i]-diamC[i-1])/2.;
                     else espessuR[i]=(diamC[i]-a)/2.;
                     cp[i]=vcp[i];
                     rhoC[i]=vrhoc[i];
                     visc[i]=vvisc[i]/1000.;
                     beta[i]=vbeta[i];
                     tipomat[i]=vtipomat[i];
                     indmat[i]=vindmat[i];
                   }
                 }
                 else{
                	 ncamadas=0;
                	 cond =0;
                	 diamC = 0;
                	 espessuR=0;
                	 cp = 0;
                	 rhoC = 0;
                	 visc = 0;
                	 beta = 0;
                	 tipomat = 0;
                	 indmat=0;
                 }
                 revest=vrevest;
                 if(revest==0){
                	 area=MPI*a*a/4.;
                	 peri=MPI*a;
                 }
                 else{
                	 area=MPI*(a*a-b*b)/4.;
                	 peri=MPI*(a+b);
                	 dia=4.*area/peri;
                 }
        } //construtor default

        DadosGeo( const DadosGeo& OutraGeo){
        a=OutraGeo.a;
        b=OutraGeo.b;
        dia=OutraGeo.dia;
        teta=OutraGeo.teta;

        area=OutraGeo.area;
        rug=OutraGeo.rug;
        peri=OutraGeo.peri;
        revest=OutraGeo.revest;

        indD0=OutraGeo.indD0;

        if(OutraGeo.ncamadas>0){
              ncamadas=OutraGeo.ncamadas;
              cond = new double [ncamadas];
              diamC = new double [ncamadas];
              espessuR = new double [ncamadas];
              cp = new double [ncamadas];
              rhoC = new double [ncamadas];
              visc = new double [ncamadas];
              beta = new double [ncamadas];
              tipomat = new int [ncamadas];
              indmat = new int [ncamadas];
              for(int i=0;i<ncamadas;i++){
                    cond[i]=OutraGeo.cond[i];
                    diamC[i] =OutraGeo.diamC[i];
                    espessuR[i]=OutraGeo.espessuR[i];
                    cp[i]=OutraGeo.cp[i];
                    rhoC[i]=OutraGeo.rhoC[i];
                    visc[i]=OutraGeo.visc[i];
                    beta[i]=OutraGeo.beta[i];
                    tipomat[i]=OutraGeo.tipomat[i];
                    indmat[i]=OutraGeo.indmat[i];
              }
        }
        else{
             ncamadas=0;
             cond =0;
             diamC = 0;
             espessuR=0;
             cp = 0;
             rhoC = 0;
             visc = 0;
             beta=0;
             tipomat = 0;
             indmat = 0;
            }
        } //construtor c�pia

        DadosGeo& operator=(const DadosGeo& vgeo){
        	if(ncamadas>0){
        	     if(this!=&vgeo){//verifica se se est� igualando o mesmo vetor
        	     if(ncamadas!=vgeo.ncamadas){
        	    	 //cout<<"dimens�es de vetores diferem, imposs�vel igualar na geometria";//mesmo tamanho de vetores?
        	        	if(cond!=0 && cond!=nullptr) delete[] cond;
        	        	if(diamC!=0 && diamC!=nullptr) delete[] diamC;
        	        	if(espessuR!=0 && espessuR!=nullptr) delete[] espessuR;
        	        	if(cp!=0 && cp!=nullptr) delete[] cp;
        	        	if(rhoC!=0 && rhoC!=nullptr) delete[] rhoC;
        	        	if(visc!=0 && visc!=nullptr) delete[] visc;
        	        	if(beta!=0 && beta!=nullptr) delete[] beta;
        	        	if(tipomat!=0 && tipomat!=nullptr) delete[] tipomat;
        	        	if(indmat!=0 && indmat!=nullptr) delete[] indmat;

        	        	ncamadas=vgeo.ncamadas;
        	            cond = new double [ncamadas];
        	            diamC = new double [ncamadas];
        	            espessuR = new double [ncamadas];
        	            cp = new double [ncamadas];
        	            rhoC = new double [ncamadas];
        	            visc = new double [ncamadas];
        	            beta = new double [ncamadas];
        	            tipomat = new int [ncamadas];
        	            indmat = new int [ncamadas];
        	     }
        	     a=vgeo.a;
        	     b=vgeo.b;
        	     dia=vgeo.dia;
        	     teta=vgeo.teta;

        	     area=vgeo.area;
        	     rug=vgeo.rug;
        	     peri=vgeo.peri;
        	     revest=vgeo.revest;

        	     indD0=vgeo.indD0;

        	     for(int i=0;i<ncamadas;i++){
        	         cond[i]=vgeo.cond[i];
        	         diamC[i] =vgeo.diamC[i];
        	         espessuR[i]=vgeo.espessuR[i];
        	         cp[i]=vgeo.cp[i];
        	         rhoC[i]=vgeo.rhoC[i];
        	         visc[i]=vgeo.visc[i];
        	         beta[i]=vgeo.beta[i];
        	         tipomat[i]=vgeo.tipomat[i];
        	         indmat[i]=vgeo.indmat[i];
        	      }
        	   }
        	}
        	else {
        		 a=vgeo.a;
        		 b=vgeo.b;
        		 dia=vgeo.dia;
        		 teta=vgeo.teta;

        		 area=vgeo.area;
        		 rug=vgeo.rug;
        		 peri=vgeo.peri;
        		 revest=vgeo.revest;
        		 indD0=vgeo.indD0;

        		 if(vgeo.ncamadas>0){
        		     ncamadas=vgeo.ncamadas;
        		     cond = new double [ncamadas];
        		     diamC = new double [ncamadas];
        		     espessuR = new double [ncamadas];
        		     cp = new double [ncamadas];
        		     rhoC = new double [ncamadas];
        		     visc = new double [ncamadas];
        		     beta = new double [ncamadas];
        		     tipomat = new int [ncamadas];
        		     indmat = new int [ncamadas];
        		     for(int i=0;i<ncamadas;i++){
        		           cond[i]=vgeo.cond[i];
        		           diamC[i] =vgeo.diamC[i];
        		           espessuR[i]=vgeo.espessuR[i];
        		           cp[i]=vgeo.cp[i];
        		           rhoC[i]=vgeo.rhoC[i];
        		           visc[i]=vgeo.visc[i];
        		           beta[i]=vgeo.beta[i];
        		           tipomat[i]=vgeo.tipomat[i];
        		           indmat[i]=vgeo.indmat[i];
        		     }
        	     }
             }

        	return *this;
        }

        ~DadosGeo(){
        	/*if(cond!=0) delete[] cond;
        	if(diamC!=0) delete[] diamC;
        	if(espessuR!=0) delete[] espessuR;
        	if(cp!=0) delete[] cp;
        	if(rhoC!=0) delete[] rhoC;
        	if(visc!=0) delete[] visc;
        	if(beta!=0) delete[] beta;
        	if(tipomat!=0) delete[] tipomat;
        	if(indmat!=0) delete[] indmat;*/

        	if(cond!=0 && cond!=nullptr) delete[] cond;
        	if(diamC!=0 && diamC!=nullptr) delete[] diamC;
        	if(espessuR!=0 && espessuR!=nullptr) delete[] espessuR;
        	if(cp!=0 && cp!=nullptr) delete[] cp;
        	if(rhoC!=0 && rhoC!=nullptr) delete[] rhoC;
        	if(visc!=0 && visc!=nullptr) delete[] visc;
        	if(beta!=0 && beta!=nullptr) delete[] beta;
        	if(tipomat!=0 && tipomat!=nullptr) delete[] tipomat;
        	if(indmat!=0 && indmat!=nullptr) delete[] indmat;
        } //destrutor

        void renovaD(){
        	for(int i=0;i<ncamadas;i++){
        		if(i>0)diamC[i]=diamC[i-1]+2.*espessuR[i];
        		else diamC[i]=a+2.*espessuR[i];
        	}
        }

        void atualizaCamada(double espessura, double rugosidade, double cpW, double kW, double rhoW){
        	DadosGeo vgeo=*this;

        	/*if(cond!=0) delete[] cond;
        	if(diamC!=0) delete[] diamC;
        	if(espessuR!=0) delete[] espessuR;
        	if(cp!=0) delete[] cp;
        	if(rhoC!=0) delete[] rhoC;
        	if(visc!=0) delete[] visc;
        	if(beta!=0) delete[] beta;
        	if(tipomat!=0) delete[] tipomat;
        	if(indmat!=0) delete[] indmat;*/
        	if(cond!=0 && cond!=nullptr) delete[] cond;
        	if(diamC!=0 && diamC!=nullptr) delete[] diamC;
        	if(espessuR!=0 && espessuR!=nullptr) delete[] espessuR;
        	if(cp!=0 && cp!=nullptr) delete[] cp;
        	if(rhoC!=0 && rhoC!=nullptr) delete[] rhoC;
        	if(visc!=0 && visc!=nullptr) delete[] visc;
        	if(beta!=0 && beta!=nullptr) delete[] beta;
        	if(tipomat!=0 && tipomat!=nullptr) delete[] tipomat;
        	if(indmat!=0 && indmat!=nullptr) delete[] indmat;

        	rug=rugosidade;

        	ncamadas++;
        	indD0++;
            cond = new double [ncamadas];
            diamC = new double [ncamadas];
            espessuR = new double [ncamadas];
            cp = new double [ncamadas];
            rhoC = new double [ncamadas];
            visc = new double [ncamadas];
            beta = new double [ncamadas];
            tipomat = new int [ncamadas];
            indmat = new int [ncamadas];

            for(int i=0;i<ncamadas-1;i++){
 		           cond[i+1]=vgeo.cond[i];
 		           diamC[i+1] =vgeo.diamC[i];
 		           espessuR[i+1]=vgeo.espessuR[i];
 		           cp[i+1]=vgeo.cp[i];
 		           rhoC[i+1]=vgeo.rhoC[i];
 		           visc[i+1]=vgeo.visc[i];
 		           beta[i+1]=vgeo.beta[i];
 		           tipomat[i+1]=vgeo.tipomat[i];
 		           indmat[i+1]=vgeo.indmat[i];
            }
            double MPI=3.14159265359;
	        cond[0]=kW;
	        diamC[0]=a;
	        espessuR[0]=espessura;
	        cp[0]=cpW;
	        rhoC[0]=rhoW;
	        visc[0]=0;
	        beta[0]=0;
	        tipomat[0]=0;
	        indmat[0]=-1;
	        dia=a-2.*espessura;
	        a=dia;
            if(revest==0){
           	 area=MPI*a*a/4.;
           	 peri=MPI*a;
            }
            else{
           	 area=MPI*(a*a-b*b)/4.;
           	 peri=MPI*(a+b);
           	 dia=4.*area/peri;
            }

        }
        void atualizaCamada2(double espessura, double cpW, double kW, double rhoW){
        	espessuR[0]+=espessura;
        	dia=a-2*espessura;
            a=dia;
            double MPI=3.14159265359;
            if(revest==0){
           	 area=MPI*a*a/4.;
           	 peri=MPI*a;
            }
            else{
           	 area=MPI*(a*a-b*b)/4.;
           	 peri=MPI*(a+b);
           	 dia=4.*area/peri;
            }
	        cond[0]=kW;
	        cp[0]=cpW;
	        rhoC[0]=rhoW;

        }

        void atualizaCamada3(double espessura, double cpW, double kW, double rhoW){
        	double delta=espessura-espessuR[0];
        	espessuR[0]=espessura;
        	dia=a-2*delta;
            a=dia;
            double MPI=3.14159265359;
            if(revest==0){
           	 area=MPI*a*a/4.;
           	 peri=MPI*a;
            }
            else{
           	 area=MPI*(a*a-b*b)/4.;
           	 peri=MPI*(a+b);
           	 dia=4.*area/peri;
            }
	        cond[0]=kW;
	        cp[0]=cpW;
	        rhoC[0]=rhoW;

        }
};



#endif
