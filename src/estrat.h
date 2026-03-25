#ifndef _ESTRAT
#define _ESTRAT

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
#include <iostream>
#include <stdlib.h>

using namespace std;

class estratificado{
public:
   double dia;
   double hfilm;
   double holliq;
   double twg;
   double twl;
   double ti;
   double fg;
   double fl;
   double fi;
   double reyL;
   double reyG;
   double alNd;
   double agNd;
   double swlNd;
   double swgNd;
   double siNd;
   double uls;
   double ugs;
   double ang;
   double rl;
   double rg;
   double mil;
   double mig;
   double velref;
   double dFdUl;
   double dFdUg;
   double dFdAl;
   double cine;
   double dina;
   double coefC0;
   double valUd;
   int arr;
   double multTrans;
   double fatorperdaLiq;
   double fatorperdaGas;

   estratificado(double vdia=1*2.54/100., double Ql=0., double Qg=0.,
                 double vrl=10000., double vrg=1.,
                 double vmil=1./1000.,double vmig=1./10000,
                 double hol=0.5,double vang=0., double eps=0.); //construtor
   estratificado(const estratificado&); //construtor por c�pia
   ~estratificado(){} //destrutor

   estratificado& operator=(const estratificado&);
   double al(const double var,const double dia){
             return 0.25*dia*dia*(M_PI-acosl(var)+var*sqrtl(1-var*var));
        }
   double ag(const double var,const double dia){
             return 0.25*dia*dia*(acosl(var)-var*sqrtl(1-var*var));
        }
   double sl(const double var, const double dia){
             double peri;
             peri=dia*(M_PI-acosl(var));
             if(peri<4.*1e-20)peri=4.*1e-20;
             return peri;
        }
   double sg(const double var, const double dia){
             double peri;
             peri=dia*acosl(var);
             if(peri<4.*1e-20)peri=4.*1e-20;
             return peri;
        }
   double si(const double var, const double dia){
             double peri;
             peri=dia*sqrtl(1-var*var);
             if(peri<4.*1e-20)peri=4.*1e-20;
             return peri;
        }

   double fraiz(const double hfilm,const double hol,
                     const double dia){
                                double var=2.*hfilm/dia-1.;
                                return 4.*al(var,dia)/(M_PI*dia*dia)-hol;
        }
   double fhfilm(double x1,
                                  const double hol,
                                  const double dia,
                                  double tol=0.01,
                                  double epsn=0.01,
                                  int maxit=400){
             if(hol<0.9999){
               double rtn=x1;
               for(int j=0;j<maxit;j++){
                 double f=fraiz(rtn, hol,dia);
                 double df=4.*si(rtn,dia)/(M_PI*dia*dia);
                 double dx=f/df;
                 rtn-=dx;
                 if(fabsl(dx)<epsn||fabsl(f)<tol||rtn<dia/50) return rtn;

               }
               throw("M�ximo n�mero de intera��es em Newton");
              }
              else{
                return hol*dia;
              }

        }

        double dhl(double var, double dia){
          return 4.*alNd*dia*dia/(swlNd*dia);
        }
        double dhg(double var, double dia){
          //return 4.*agNd*dia*dia/(sgNd*dia+siNd*dia);
          return 4.*agNd*dia*dia/(swgNd*dia);
        }
        double reyl(double var, double dia,
                         double Ql){
          if(alNd<1e-20)alNd=1e-20;
          return dhl(var,dia)*rl*(fabsl(Ql)/(alNd*dia*dia))/mil;
        }
        double reyg(double var, double dia,
                         double Qg){
          if(agNd<1e-20){
           agNd=1e-20;
           //int para;
           //para++;
          }
          return dhg(var,dia)*rg*(fabsl(Qg)/(agNd*dia*dia))/mig;

        }
////////////////////////////////////////////////////////////////////////////////
        double fricl(double dia,
                          double Ql, double eps);
        double fricg(double dia,
                          double Qg, double eps);
        double tall(double vell){
             return 0.5*fl*rl*vell*fabsl(vell);
        }

        double talg(double velg){
             return 0.5*fg*rg*velg*fabsl(velg);
        }

        double tali(double velg,double vell){
             return 0.5*fg*rg*(velg-vell)*fabsl(velg-vell);
        }
        
        double fonteQMGas(double dia);
        double fonteQMliquido(double dia);
        void mapa(double dia,double alf);
        double mapa2(double chute);
        void mapaTD(double chute=-1.);
        double equil(double dia);
        double equil2(double ql,double qt,double eps);
        double equil3(double hol,double ql,double qg,double eps);
        double qlequil(double hol,double ql,double qg,double eps);
        double criterio(double hol,double ql,double qg,double eps);
        double zbrentUls(double x1,double x2,double ql,double qg,double eps,double tol=0.00000000001,
                                 double epsn=0.00000000001,int maxit=4000);
        double findalf(double hol1, double hol2, double ql, double qg, double eps);
        double  calcC0(double x1,const double qt,
                                   double alf,double eps,double tol=1.0,
                                  double epsn=0.000000001,int maxit=800);
        double FalsaCorda(double a,double b,
                               const double qt,
                               double alf,double eps,
                               double delta=0.00001 ,
                               double epsn=0.00001,int maxit=2000);
        double SIGN(double a,double b){return (b>=0?1.0:-1.0)*fabs(a);}
        //verifica se um valor com a magnitude de a e sinal de b � diferente de a, ver NR
        double zbrent(double x1,double x2,double ql,double qg,
               double eps,double tol=0.001,
               double epsn=0.001,int maxit=100);


        double calcC0FC(const double x1,const double qt,
                                   double alf,double eps,double coaux);

        double CalcUdTrans(double alfc, double alf0,
                        double alf1,double Ql0,double Ql1,
                        double Qg0,double Qg1,
                        double pL,double pR,
                        double c0aux,double dt,double dx,int verifica);
        /*double CalcUdTrans(double alfc, double alf0,
                                double alf1,double Ql0,double Ql1,
                                double Qg0,double Qg1,
                                double pL,double pR,
                                double c0aux,double dt,double dx,double rg, double rl);*/

};




#endif

