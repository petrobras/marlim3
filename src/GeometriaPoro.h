#ifndef _GEOMPoro
#define _GEOMPoro

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

using namespace std;

class DadosGeoPoro{ //Vcr � um template para vetores
	public:
        double a; //Altura do canal (metros)
        double b;  // Largura do Canal (metros)
        double area;
        double dia;
        double peri;
        int ncamadas;
        double* kX;//condutividade da tubula��o
        double* diamC;
        double* espessuR;
        double* kY;
        double* poro;
        double* compRoc;
        DadosGeoPoro(double va=0 ,double vb=0,int vn=0, double* vkX=0, double* vdiamC=0,double* vkY=0,
        		 double* vporo=0, double* vcompRoc=0){
                 a=va;
                 b=vb;
                 dia=a;

                 if(vn>0){
                   ncamadas=vn;
                   kY = new double [ncamadas];
                   diamC = new double [ncamadas];
                   espessuR = new double [ncamadas];
                   kX = new double [ncamadas];
                   poro = new double [ncamadas];
                   compRoc = new double [ncamadas];
                   for(int i=0;i<ncamadas;i++){
                     kX[i]=vkX[i];
                     diamC[i] =vdiamC[i];
                     if(i>0)espessuR[i]=(diamC[i]-diamC[i-1])/2.;
                     else espessuR[i]=(diamC[i]-a)/2.;
                     kY[i]=vkY[i];
                     poro[i]=vporo[i];
                     compRoc[i]=vcompRoc[i]/1000.;
                   }
                 }
                 else{
                	 ncamadas=0;
                	 kY =0;
                	 diamC = 0;
                	 espessuR=0;
                	 kX = 0;
                	 poro = 0;
                	 compRoc = 0;
                 }
                 double varpi=3.14159265359;
            	 area= varpi*a*a/4.;
            	 peri=varpi*a;
        } //construtor default

        DadosGeoPoro( const DadosGeoPoro& OutraGeo){
        a=OutraGeo.a;
        b=OutraGeo.b;
        dia=OutraGeo.dia;

        area=OutraGeo.area;
        peri=OutraGeo.peri;

        if(OutraGeo.ncamadas>0){
              ncamadas=OutraGeo.ncamadas;
              kX = new double [ncamadas];
              diamC = new double [ncamadas];
              espessuR = new double [ncamadas];
              kY = new double [ncamadas];
              poro = new double [ncamadas];
              compRoc = new double [ncamadas];
              for(int i=0;i<ncamadas;i++){
                    kX[i]=OutraGeo.kX[i];
                    diamC[i] =OutraGeo.diamC[i];
                    espessuR[i]=OutraGeo.espessuR[i];
                    kY[i]=OutraGeo.kY[i];
                    poro[i]=OutraGeo.poro[i];
                    compRoc[i]=OutraGeo.compRoc[i];
              }
        }
        else{
             ncamadas=0;
             kY =0;
             diamC = 0;
             espessuR=0;
             kX = 0;
             poro = 0;
             compRoc = 0;
            }
        } //construtor c�pia

        DadosGeoPoro& operator=(const DadosGeoPoro& vgeo){
        	if(ncamadas>0){
        	     if(this!=&vgeo){//verifica se se est� igualando o mesmo vetor
        	     if(ncamadas!=vgeo.ncamadas) cout<<"dimens�es de vetores diferem, imposs�vel igualar na geometria";//mesmo tamanho de vetores?
        	     a=vgeo.a;
        	     b=vgeo.b;
        	     dia=vgeo.dia;

        	     area=vgeo.area;
        	     peri=vgeo.peri;

        	     for(int i=0;i<ncamadas;i++){
        	         kX[i]=vgeo.kX[i];
        	         diamC[i] =vgeo.diamC[i];
        	         espessuR[i]=vgeo.espessuR[i];
        	         poro[i]=vgeo.poro[i];
        	         compRoc[i]=vgeo.compRoc[i];
        	         kY[i]=vgeo.kY[i];
        	      }
        	   }
        	}
        	else {
        		 a=vgeo.a;
        		 b=vgeo.b;
        		 dia=vgeo.dia;

        		 area=vgeo.area;
        		 peri=vgeo.peri;

        		 if(vgeo.ncamadas>0){
        		     ncamadas=vgeo.ncamadas;
        		     kX = new double [ncamadas];
        		     diamC = new double [ncamadas];
        		     espessuR = new double [ncamadas];
        		     kY = new double [ncamadas];
        		     poro = new double [ncamadas];
        		     compRoc = new double [ncamadas];
        		     for(int i=0;i<ncamadas;i++){
        		           kX[i]=vgeo.kX[i];
        		           diamC[i] =vgeo.diamC[i];
        		           espessuR[i]=vgeo.espessuR[i];
        		           kY[i]=vgeo.kY[i];
        		           poro[i]=vgeo.poro[i];
        		           compRoc[i]=vgeo.compRoc[i];
        		     }
        	     }
             }

        	return *this;
        }

        ~DadosGeoPoro(){
        	if(kY!=0) delete[] kY;
        	if(diamC!=0) delete[] diamC;
        	if(espessuR!=0) delete[] espessuR;
        	if(kX!=0) delete[] kX;
        	if(poro!=0) delete[] poro;
        	if(compRoc!=0) delete[] compRoc;
        } //destrutor

        void renovaD(){
        	for(int i=0;i<ncamadas;i++){
        		if(i>0)diamC[i]=diamC[i-1]+2.*espessuR[i];
        		else diamC[i]=a+2.*espessuR[i];
        	}
        }
};


#endif
