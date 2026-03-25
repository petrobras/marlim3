#include "FerramentasNumericas.h"

void NumError(const string& t){
  // incluir falha
  logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
      t, "", "");
  // gravar arquivo de log
  logger.writeOutputLog();
  // abandonar a aplicacao
  exit(EXIT_SUCCESS);
}

template<class T> T gammln(const T xx){//Retorna o valor de ln(gama(xx)) para xx>0
 int j;
 T x,tmp,y,ser;
 static const double cof[14]={57.1562356658629235,-59.5979603554754912,14.1360979747417471,
 -0.491913816097620199,.339946499848118887e-4,.465236289270485756e-4,-.983744753048795646e-4,
 0.158088703224912494e-3,-.210264441724104883e-3,0.217439618115212643e-3,-.164318106536763890e-3,
 0.844182239838527433e-4,-.261908384015814087e-4,0.368991826595316234e-5};
 if(xx<=0) NumError("Argumento inadequado para o calculo da funcao gamma.");
 y=x=xx;
 tmp=x+5.24218750000000000;
 tmp=(x+0.5)*log(tmp)-tmp;
 ser=0.999999999999997092;
 for(j=0;j<14;j++)ser+=cof[j]/++y;
 return tmp+log(2.5066282746310005*ser/x);
}

template<class T>
        T PoliNewton(const Vcr<T>& vx, const Vcr<T>& vy, T x){
        //vx-> vetor com dados da variavel para montar o polinomio
        //vy-> vetor com dados da funcao (para cada valor da variavel) para montar o polinomio
        //x-> valor da variavel com a qual se obtera o valor interpolado pelo polinomio
        // a ordem do polinomio e dada pelo tamanho do vetor vy
          Vcr<T> b=vy;
          int n=vy.size-1;

          for(int j=1;j<=n;j++)
            for(int i=n;i>=j;i--)
              b[i]=(b[i]-b[i-1])/(vx[i]-vx[i-j]);

          T u=b[n];
          for(int i=n-1;i>=0;i--)u=b[i]+(x-vx[i])*u;
          return u;
}

template<class T>
        T InterLinear(const Vcr<T>& vx, const Vcr<T>& vy, T x){
        //vx-> lista de valores da variavel para fazer uma interpolacao linear
        //vy-> lista de valores da funcao para se fazer a interpolacao
        //x-> valor da variavel onde se quer fazer a interpolacao
          int i=0;
          int lim=vx.size();
          if(x==vx[0])return vy[0];
          while(x>vx[i]&&i<lim)i++;//busca o valor imediatamente maior que x em vx
          if(i>=lim||i==0)NumError("Valor de x fora de range na interpolacao linear");
          T var=vy[i-1]+(vy[i]-vy[i-1])*(x-vx[i-1])/(vx[i]-vx[i-1]);
          return var;
}

template<class T>
        T InterLinear(const FullMtx<T>& super, const Vcr<T>& vx, const Vcr<T>& vy, T x, T y){
        //super-> matriz com os valores com os quais se ir� interpolar
        //vx e vy->lista de pares de variaveis onde se localiza cada valor de super, vx[j] e vy[i] -> super[i][j]
        //x e y-> localizacao onde se quer fazer a interpolacao
          int j=0;
          int i=0;
          int limx=vx.size();
          int limy=vy.size();
          while(x>vx[j]&&j<limx)j++;//busca valor imediatamente superior a x em vx
          while(y>vy[i]&&i<limy)i++;//busca valor imediatamente superior a y em vy
          if(j>=limx||i>=limy||j==0||i==0)NumError("Valor de x ou de y fora de range na interpolacao linear para superficie");
          T vmedxy0=super[i-1][j-1]+(super[i-1][j]-super[i-1][j-1])*(x-vx[j-1])/(vx[j]-vx[j-1]);
          T vmedxy1=super[i][j-1]+(super[i][j]-super[i][j-1])*(x-vx[j-1])/(vx[j]-vx[j-1]);
          T vmed=vmedxy0+(vmedxy1-vmedxy0)*(y-vy[i-1])/(vy[i]-vy[i-1]);
          return vmed;
}

template<class T, T F(T,const T* const)>
        T NIntegrate(T lower,T upper,const T* const par,int n,int tipo){
        //lower-> limite inferior da integracao
        //upper-> limite superior da integracao
        //n->numero de intervalos em que sera feita a integracao
        //tipo-> indica qual o metodo se Euler (1) ou Simpson (2)
        //F(T)-> funcao a ser integrada
          if(tipo==1){//integracao por Euler
            T h=(upper-lower)/n;
            T sum=F(lower,par)*0.5;
            for(int i=1;i<n;i++)sum+=F(lower+i*h,par);
            sum+=F(upper,par)*0.5;
            return sum*h;
          }
          else if(tipo==2){//integracao por Simpson
           T h=(upper-lower)/n;
           T sum=F(lower,par)*0.5;
           for(int i=1;i<n;i++)sum+=F(lower+i*h,par);
           sum+=F(upper,par)*0.5;
           T summid=0.0;
           for(int i=1;i<=n;i++)summid+=F(lower+(i-0.5)*h,par);
           return (sum+2*summid)*h/3.0;
          }
          else if(tipo==3){//quadratura gaussiana por Legendre
           return quadleg<T,F>(lower,upper,par,n);
          }
          else{
           NumError("Identificador de tipo de integracao errado");
           return 0.0;
          }
}

template<class T> void gauleg(const T x1, const T x2, Vcr<T>& x, Vcr<T>& w){//subrotina auxiliar para quadratura gaussiana
//usando polinomio de Legendre, da os pontos x da quadartura e seus pesos, x1 e x2 sao os limites de integracao

 const double EPS=1.0e-14;
 T z1,z,xm,xl,pp,p3,p2,p1;
 int n=x.size();
 int m=(n+1)/2;
 xm=0.5*(x2+x1);
 xl=0.5*(x2-x1);
 for(int i=0;i<m;i++){
   z=cos(3.141592654*(i+0.75)/(n+0.5));
   do{
     p1=1.0;
     p2=0.0;
     for(int j=0;j<n;j++){
       p3=p2;
       p2=p1;
       p1=((2.0*j+1.0)*z*p2-j*p3)/(j+1);
     }
     pp=n*(z*p1-p2)/(z*z-1.0);
     z1=z;
     z=z1-p1/pp;
   }while(fabs(z-z1)>EPS);
   x[i]=xm-xl*z;
   x[n-1-i]=xm+xl*z;
   w[i]=2.0*xl/((1.0-z*z)*pp*pp);
   w[n-1-i]=w[i];
 }
}

template<class T> void gaulag(Vcr<T>& x, Vcr<T>& w, const T alf){//subrotina auxiliar para quadratura gaussiana
//usando polinomio de Laguerre, da os pontos x da quadartura e seus pesos

 const int MAXIT=10;
 const double EPS = 1.0e-14;
 int i, its,j;
 T ai,p1,p2,p3,pp,z,z1;
 int n=x.size();
 for(i=0;i<n;i++){
   if(i==0){
     z=(1.0+alf)*(3.0+0.92*alf)/(1.0+2.4*n+1.8*alf);
   }
   else if(i==1){
     z+=(15.0+6.25*alf)/(1.0+0.9*alf+2.5*n);
   }
   else{
     ai=i-1;
     z+=((1.0+2.55*ai)/(1.9*ai)+1.26*ai*alf/(1.0+3.5*ai))*(z-x[i-2])/(1.0+0.3*alf);
   }
   for(its=0;its<MAXIT;its++){
     p1=1.0;
     p2=0.0;
     for(j=0;j<n;j++){
       p3=p2;
       p2=p1;
       p1=((2*j+1+alf-z)*p2-(j+alf)*p3)/(j+1);
     }
     pp=(n*p1-(n+alf)*p2)/z;
     z1=z;
     z=z1-p1/pp;
     if(fabs(z-z1)<=EPS)break;
   }
   if(its>=MAXIT)NumError("Excedeu o numero de iteracoes em Laguerre (quadratura)");
   x[i]=z;
   w[i]=-exp(gammln(alf+n)-gammln(double(n)))/(pp*n*p2);
 }
}

template<class T> void gauher(Vcr<T>& x, Vcr<T>& w){//subrotina auxiliar para quadratura gaussiana
//Polin�mio de Hermite
 const double EPS=1.0e-14, PIM4=0.7511255444649425;
 const int MAXIT=10;
 int i, its,j,m;
 T p1,p2,p3,pp,z,z1;
 int n=x.size();
 m=(n+1)/2;
 for(i=0;i<m;i++){
   if(i==0){
     z=sqrt(double(2*n+1))-1.85575*pow(double(2*n+1),-0.16667);
   }
   else if(i==1){
     z-=1.14*pow(double(n),0.426)/z;
   }
   else if(i==2){
     z=1.86*z-0.86*x[0];
   }
   else if(i==3){
     z=1.91*z-0.91*x[1];
   }
   else{
     z=2.0*z-x[i-2];
   }
   for(its=0;its<MAXIT;its++){
     p1=PIM4;
     p2=0.0;
     for(j=0;j<n;j++){
       p3=p2;
       p2=p1;
       p1=z*sqrt(2.0/(j+1))*p2-sqrt(double(j)/(j+1))*p3;
     }
     pp=sqrt(double(2*n))*p2;
     z1=z;
     z=z1-p1/pp;
     if(fabs(z-z1)<=EPS)break;
   }
   if(its>=MAXIT)NumError("Excedeu o numero de iteracoes em Hermite (quadratura)");
   x[i]=z;
   x[n-1-i]=-z;
   w[i]=2.0/(pp*pp);
   w[n-1-i]=w[i];
 }
}


template<class T> void gaujac(const T x1, const T x2, Vcr<T>& x, Vcr<T>& w, const T alf, const T bet){//subrotina auxiliar para quadratura gaussiana
//polinomio de Jacobi
 T L=0.5*(x2-x1);
 T coef=1/L;
 T trans=-(1+coef*x1);
 const int MAXIT=10;
 const double EPS=1.0e-14;
 int i,its,j;
 double alfbet,an,bn,r1,r2,r3;
 double a,b,c,p1,p2,p3,pp,temp,z,z1;
 int n=x.size();
 for(i=0;i<n;i++){
   if(i==0){
     an=alf/n;
     bn=bet/n;
     r1=(1.0+alf)*(2.78/(4.0+n*n)+0.768*an/n);
     r2=1.0+1.48*an+0.96*bn+0.452*an*an+0.83*an*bn;
     z=1.0-r1/r2;
   }
   else if(i==1){
     r1=(4.1+alf)/((1.0+alf)*(1.0+0.156*alf));
     r2=1.0+0.06*(n-8.0)*(1.0+0.12*alf)/n;
     r3=1.0+0.012*bet*(1.0+0.25*fabs(alf))/n;
     z-=(1.0-z)*r1*r2*r3;
   }
   else if(i==2){
     r1=(1.67+0.28*alf)/(1.0+0.37*alf);
     r2=1.0+0.22*(n-8.0)/n;
     r3=1.0+8.0*bet/((6.28+bet)*n*n);
     z-=(x[0]-z)*r1*r2*r3;
   }
   else if(i==n-2){
     r1=(1.0+0.235*bet)/(0.766+0.119*bet);
     r2=1.0/(1.0+0.639*(n-4.0)/(1.0+0.71*(n-4.0)));
     r3=1.0/(1.0+20.0*alf/((7.5+alf)*n*n));
     z+=(z-x[n-4])*r1*r2*r3;
   }
   else if(i==n-1){
     r1=(1.0+0.37*bet)/(1.67+0.28*bet);
     r2=1.0/(1.0+0.22*(n-8.0)/n);
     r3=1.0/(1.0+8.0*alf/((6.28+alf)*n*n));
     z+=(z-x[n-3])*r1*r2*r3;
   }
   else{
     z=3.0*x[i-1]-3.0*x[i-2]+x[i-3];
   }
   alfbet=alf+bet;
   for(its=1;its<=MAXIT;its++){
     temp=2.0+alfbet;
     p1=(alf-bet+temp*z)/2.0;
     p2=1.0;
     for(j=2;j<=n;j++){
       p3=p2;
       p2=p1;
       temp=2*j+alfbet;
       a=2*j*(j+alfbet)*(temp-2.0);
       b=(temp-1.0)*(alf*alf-bet*bet+temp*(temp-2.0)*z);
       c=2.0*(j-1.0+alf)*(j-1.0+bet)*temp;
       p1=(b*p2-c*p3)/a;
     }
     pp=(n*(alf-bet-temp*z)*p1+2.0*(n+alf)*(n+bet)*p2)/(temp*(1.0-z*z));
     z1=z;
     z=z1-p1/pp;
     if(fabs(z-z1)<=EPS) break;
   }
   if(its>MAXIT)NumError("Excedeu o numero de iteracoes em Jacobi (quadratura)");
   x[i]=z;
   w[i]=exp(gammln(alf+n)+gammln(bet+n)-gammln(n+1.0)-gammln(n+alfbet+1.0))*temp*pow(2.0,alfbet)/(pp*p2);
 }
 x=(x-trans)/coef;
 w=w/coef;

}


template<class T, T F(T,const T* const)> T quadleg(const T x1, const T x2, const T* const par,int n)
{//quadratura gaussiana usando Legendre, x1-> limite inferior, x2-> limite superior
//n->numero de pontos da quadratura
 Vcr<T> x(n);
 Vcr<T> w(n);
 gauleg<T>(x1,x2,x,w);
 T s=0;
 for(int i=0;i<n;i++)s+=w[i]*F(x[i],par);
 return s;
}

template<class T, T F(T,const T* const)> T quadjac(const T x1, const T x2, const T* const par,const int n, const T alf, const T bet){ //quadratura gaussiana usando Jacobi, x1-> limite inferior, x2-> limite superior
//n->numero de pontos da quadratura, alf e bet expoentes da funcao ponderadora-> (x^alfa)*(e^-x),
//integrando do tipo  (x^alfa)*(e^-x)*f(x);
 Vcr<T> x(n);
 Vcr<T> w(n);
 gaujac<T>(x1,x2,x,w,alf,bet);
 T s=0;
 for(int i=0;i<n;i++)s+=w[i]*F(x[i],par);
 return s;
}

template<class T, T F(T,const T* const)> T quadlag(const T* const par,const int n, const T alf){ //quadratura gaussiana usando Laguerre
//n->numero de pontos da quadratura, integracao vai de 0 a infinito, alf e o expoente da funcao ponderadora->(x^alfa)*(e^-x)
//integrando do tipo (x^alfa)*(e^-x)*f(x);
 Vcr<T> x(n);
 Vcr<T> w(n);
 gaulag<T>(x,w,alf);
 T s=0;
 for(int i=0;i<n;i++)s+=w[i]*F(x[i],par);
 return s;
}

template<class T, T F(T,const T* const)> T quadher(const T* const par,const int n){ //quadratura gaussiana usando Hermite
//n->numero de pontos da quadratura, integracao vai de -infinito a infinito
//integrando do tipo ((e^-x)^2)*f(x);
 Vcr<T> x(n);
 Vcr<T> w(n);
 gauher<T>(x,w);
 T s=0;
 for(int i=0;i<n;i++)s+=w[i]*F(x[i],par);
 return s;
}

template<class T, T F(T,const T* const)> void MelhoraChute(T& a,T& b, const T* const par, int dinter){//Melhora chute para calculo de zero
//Esta funcao deve ser utilizada como auxilio para rotinas de calculo de zero de funcao que
//precisem de chutes em que a raiz esteja englobada nestes chutes
          T u=F(a,par);
          T v=F(b,par);
          if(u!=0 && v!=0){//neste primeiro bloco, busca-se assegurar que sempre se estrai trabalhando com valores de funcao negativa nos chutes iniciais para se garantir maior convergencia na falsa corda
           if(u/v>0){
            if(u>0){
             T proy=v; T prox=b; T distx=a; T del=a-prox;
             if(u<proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy>0){
              prox=prox-del/dinter; proy=F(prox,par);
             }
             a=prox; b=distx; u=proy;
            }
            else{
             T proy=v; T prox=b; T distx=a; T del=a-prox;
             if(u>proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy<0){
              prox=prox-del/dinter;
              double proy2=F(prox,par);
              if(-proy2>-proy){
                 prox=a;
                 a=b;
                 b=(b+prox)/2.;
                 prox=a;
              }
              proy=F(prox,par);

             }
             a=prox; b=distx; u=proy;
            }
           }
          }
}

template<class T, T F(T, const T* const)>
        T FalsaCorda(T a,T b, const T* const par,T delta ,T epsn,int maxit){//zero de funcao por falsa corda
        //a e b-> chutes iniciais para a falsa corda
        //epsn -> limite em que se admite F(T) aproximadamente zero
        //delta-> limite a partir do qual admite-se que ja nao existe variacao no calculo da raiz
        //maxit-> maximo numero de iteracoes permitido no calculo
        //F(T)-> funcao que se quer obter a raiz
          MelhoraChute<T,F>(a,b,par);
          T u=F(a,par);
          T e=b-a;
          T c;

          for(int k=1;k<=maxit;k++){//este bloco trata da falsa corda propriamente dita
            e*=0.5;
            c=a+e;
            T w=F(c,par);
            if(fabs(e)<delta||fabs(w)<epsn) return c;
            ((u>0&&w<0)||(u<0&&w>0))?(b=c):(a=c,u=w);
          }
          cout<<"Falsa corda atingiu numero maximo de iteracoes"<<"\n" ;
          return c;
}


template<class T, T F(T, const T* const)> T Bissecao(T a,T b, const T* const par,T delta ,T epsn,int maxit){
//Acha zero de funcao por bissecao
//a e b-> chutes iniciais para a falsa corda
//epsn -> limite em que se admite F(T) aproximadamente zero
//delta-> limite a partir do qual admite-se que ja nao existe variacao no calculo da raiz
//maxit-> maximo numero de iteracoes permitido no calculo
//F(T)-> funcao que se quer obter a raiz
 MelhoraChute<double,F>(a,b,par);
 T u=F(a,par);
 T e=b-a;
 T c;
 for(int k=1;k<=maxit;k++){
  e*=0.5; //metade do intervalo original->b-a
  c=a+e;//ponto central
  T w=F(c,par);
  if(fabs(e)<delta||fabs(w)<epsn) return c;//convergiu
  ((u>0&&w<0)||(u<0&&w>0))?(b=c):(a=c,u=w);
 }
 return c; //retornando por excesso de iteracoes
}


template<class T> T SIGN(T a,T b){return (b>=0?1.0:-1.0)*fabsl(a);}


template<class T, T F(T, const T* const)> T zriddr(T x1,T x2, const T* const par,T xacc, int maxit){
 MelhoraChute<double,F>(x1,x2,par);
 T fl=F(x1,par);
 T fh=F(x2,par);
 if((fl>0.0&&fh<0.0)||(fl<0.0&&fh>0.0)){
   T xl=x1;
   T xh=x2;
   T ans=-1000000.;
   for(int j=0;j<maxit;j++){
     T xm=0.5*(x1+xh);
     T fm=F(xm,par);
     T s=sqrtl(fm*fm-fl*fh);
     if(s==0.0) return ans;
     T xnew=xm+(xm-xl)*((fl>=fh?1.0:-1.0)*fm/s);
     if(fabs(xnew-ans)<=xacc)return ans;
     ans=xnew;
     T fnew=F(ans,par);
     if(fnew==0.0)return ans;
     if(SIGN(fm,fnew)!=fm){
       xl=xm;
       fl=fm;
       xh=ans;
       fh=fnew;
     }
     else if(SIGN(fl,fnew)!=fl){
       xh=ans;
       fh=fnew;
     }
     else if(SIGN(fh,fnew)!=fh){
       xl=ans;
       fl=fnew;
     }
     else NumError("Chute inadequado para o calculo de raiz no metodo Ridders");
     if(fabs(xh-xl)<=xacc)return ans;
   }
     NumError("Metodo de Ridders atingiu o maximo numero de iteracoes");
 }
 else{
   if(fl==0.)return x1;
   if(fh==0.)return x2;
   NumError("Chute inadequado para o calculo de raiz no metodo Ridders");
 }
}

template<class T, T F(T, const T* const)> T zbrent(T x1,T x2, const T* const par,T tol, T epsn, int maxit){
//Acha zero de funcao ver NR
 MelhoraChute<double,F>(x1,x2, par);
 T EPS=epsn;
 T a=x1;
 T b=x2;
 T c=x2;
 T fa=F(a,par);
 T fb=F(b,par);
 T d,e,fc,p,q,r,s,tol1,xm;

 if((fa>0.0&&fb>0.0)||(fa<0.0&&fb<0.0))NumError("Chute inadequado para o calculo de raiz no metodo Ridders");
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
     T min1=3.0*xm*q-fabs(tol1*q);
     T min2=fabs(e*q);
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
   fb=F(b,par);
 }
 NumError("Metodo Van Winjngaarden-Dekker-Brent para calcular zero de funcao atingiu maximo de iteracoes");
 return 0.0;
}

template<class T, T F(T)> T zbrent2(T x1,T x2,T tol, T epsn, int maxit){
//Acha zero de funcao ver NR
 T EPS=epsn;
 T a=x1;
 T b=x2;
 T c=x2;
 T fa=F(a);
 T fb=F(b);
 T d,e,fc,p,q,r,s,tol1,xm;

 if((fa>0.0&&fb>0.0)||(fa<0.0&&fb<0.0))NumError("Chute inadequado para o calculo de raiz no metodo Ridders");
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
     T min1=3.0*xm*q-fabs(tol1*q);
     T min2=fabs(e*q);
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
   fb=F(b);
 }
 NumError("Metodo Van Winjngaarden-Dekker-Brent para calcular zero de funcao atingiu maximo de iteracoes");
 return 0.0;
}


template<class T> spline<T>::spline(int n, const T* const abs, const T* const ord){//construtor
        n--;
 	vx = new T [lenth=n+1];
        vy = new T [n+1];
        d2vy = new T [n];

	for (int i=0; i<lenth; i++){
         vx[i]= *(abs+i);//copiando de uma area apontada por um ponteiro
         vy[i]= *(ord+i);
         }

        T p;
        T sig;

        T* u;
  	u = new T [n-1];

        d2vy[0]=0;
        u[0]=0;

        for(int i=1;i<n-1;i++){

           sig=(vx[i]-vx[i-1])/(vx[i+1]-vx[i-1]);
                      p=sig*d2vy[i-1]+2.0;
           d2vy[i]=(sig-1.0)/p;
           u[i]=(vy[i+1]-vy[i])/(vx[i+1]-vx[i])-(vy[i]-vy[i-1])/(vx[i]-vx[i-1]);
           u[i]=(6.0*u[i]/(vx[i+1]-vx[i-1])-sig*u[i-1])/p;
        }

        d2vy[n-1]= 0;
        for(int k=n-2;k>=0;k--) d2vy[k]=d2vy[k]*d2vy[k+1]+u[k];
        delete[] u;
}

template<class T> spline<T>::spline(const spline& bombaantiga){
//construtor de copia

        T n=bombaantiga.lenth;
        n--;
	vx = new T [lenth=n+1];
        vy = new T [n+1];
        d2vy = new T [n];

	for (int i=0; i<lenth; i++){
         vx[i]= bombaantiga.vx[i];
         vy[i]= bombaantiga.vy[i];
       }
	for (int i=0; i<lenth-1; i++){
         d2vy[i]= bombaantiga.d2vy[i];
        }
}

template<class T> spline<T>& spline<T>::operator =(const spline& antigo){
        T n=antigo.lenth;
        n--;
        vx = new T [lenth=n+1];
        vy = new T [n+1];
        d2vy = new T [n];

	for (int i=0; i<lenth; i++){
         vx[i]= antigo.vx[i];
         vy[i]= antigo.vy[i];
       }
	for (int i=0; i<lenth-1; i++){
         d2vy[i]= antigo.d2vy[i];
        }
}

template<class T> T spline<T>::Fy(T xvx) const{
 int klo=0;
 int khi;
 while(xvx>vx[klo]&&klo<lenth-1)klo++;
 if(xvx<=vx[0])return vx[0]+(vy[1]-vy[0])*(xvx-vx[0])/(vx[1]-vx[0]);
 else if(klo>lenth-2){
    T h=vx[lenth-1]-vx[lenth-2];
    T a=(vx[lenth-1]-xvx)/h;
    T b=(xvx-vx[lenth-2])/h;
    T deri= (vy[lenth-1]-vy[lenth-2])/(vx[lenth-1]-vx[lenth-2])-
           (2*a*a-1)*(vx[lenth-1]-vx[lenth-2])*d2vy[lenth-2]/6+
           (3*b*b-1)*(vx[lenth-1]-vx[lenth-2])*d2vy[lenth-2]/6;
    return vy[lenth-1]+deri*(xvx-vx[lenth-1]);
 }
 else{
      klo--;
      khi=klo+1;
      T h=vx[khi]-vx[klo];
      T a=(vx[khi]-xvx)/h;
      T b=(xvx-vx[klo])/h;
      T y=a*vy[klo]+b*vy[khi]+((a*a*a-a)*d2vy[klo]+(b*b*b-b)*d2vy[khi])*(h*h)/6.;
      return y;
 }
}

template<class T> T spline<T>::Dy(T ex) const{
 int klo=0;
 int khi;
 while(ex>vx[klo]&&klo<lenth-1)klo++;
 if(ex<vx[0])return (vy[1]-vy[0])/(vx[1]-vx[0]);
 else if(klo>lenth-2){
    T h=vx[lenth-1]-vx[lenth-2];
    T a=(vx[lenth-1]-ex)/h;
    T b=(ex-vx[lenth-2])/h;
    return (vy[lenth-1]-vy[lenth-2])/(vx[lenth-1]-vx[lenth-2])-
           (2*a*a-1)*(vx[lenth-1]-vx[lenth-2])*d2vy[lenth-2]/6+
           (3*b*b-1)*(vx[lenth-1]-vx[lenth-2])*d2vy[lenth-2]/6;
 }
 else{
    klo--;
    khi=klo+1;
    T h=vx[khi]-vx[klo];
    T a=(vx[khi]-ex)/h;
    T b=(ex-vx[klo])/h;
    return (vy[khi]-vy[klo])/(vx[khi]-vx[klo])-
           (2*a*a-1)*(vx[khi]-vx[klo])*d2vy[klo]/6+
           (3*b*b-1)*(vx[khi]-vx[klo])*d2vy[klo]/6;
 }
}

template<class T> ostream& spline<T>::curva(ostream& s,  T lim1, T lim2,int npontos) const{
   T dx=(lim2-lim1)/(npontos-1);
   for ( int i = 0; i<npontos; i++){
        s.width(6);
        s.precision(5);
        s<<"X =  "<<lim1+i*dx<<" Y =  "<<Fy(lim1+i*dx)<<"\n";
       }
        s<<"\n";
  return s;
}

template<class T, T F(T,const T* const)> T derivada(T x,const T* const par, T dx){ //calcula a derivada de uma funcao
//a partir de um spline, dx e o valor do diferencial de x utilizado no calculo da derivada
        T n=7;
        T* vx;
        T* vy;
	vx = new T [n];
        vy = new T [n];
        x=x-3*dx;
        for(int i=0;i<n;i++){
          vx[i]=x;
          vy[i]=F(x,par);
          x+=dx;
        }
        spline<T> curvaprox(7,vx,vy);
        return curvaprox.Dy(vx[3]);
}

//template void NumError<string>(const string&);
