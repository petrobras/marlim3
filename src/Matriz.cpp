#include "Matriz.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


//funcao de erro:

template<class T> void error(const T& t){//aviso de erro em matriz
  // gravar arquivo de log
  logger.writeOutputLog();
  // abandonar a aplicacao
  exit(EXIT_SUCCESS);
}

template<class T> ostream& operator<<(ostream& s, const FullMtx<T>& mat){//saida de valores de Matriz
	int nc=mat.col();
	int nl=mat.lin();
	for ( int i = 0; i<nl; i++){
          for(int j=0;j<nc;j++){
            s.width(20);
	    s.precision(19);
            s<<mat.mx[i][j]<<" ; ";
          }
	  s<<"\n";
	}
        s<<"\n";
	return s;
}


/*//Fun��es para Matriz Abstrata:
template<class T> int AbsMtx<T>::CG(Vcr<T>& x, const Vcr<T>& b, T& eps, int& iter, int pcn){//gradiente conjugado
//Gradiente conjugado  com pr�-condicionador para um sistema do tipo Ax=b
//retorna 0 se teve sucesso e 1 se ocorreu algum problema
//A: Matriz Hermitiana positiova definida
//x: na entrada � o chute inicial, ap�s passar pela fun��o torna-se a solu��o aproximada
//b: vetor livre do lado direito
//eps: na entrada -> crit�rio para fim de itera��o, na sa�da res�duo da solu��o aproximada
//iter: na entrada-> m�paximo n�mero de itera��es; na sa�da-> n�mero de itera��as feitas durante a execu��o da fun��o
//pcn-> �ndice para escolah do pr�-condicionador
  if(this->nrows!=b.size())error("tamanho de matriz e vetor incompat�veis, erro ocorrido no gradiente conjugado");
  const int maxiter=iter;
  Vcr<T> r=b-(*this)*x;//erro inicial
  Vcr<T> z=preconding(r,pcn); //solve precondit system
  Vcr<T> p=z;//p:dire��o do gradiente conjugado
  T zr=dot(z,r);//produto interno
  const double stp=eps*b.twonorm();//crit�rio para parada da itera��o

  if(!r.maxnorm()){//se o chute inicial � a solu��o
    eps=0.0;
    iter=0;
    return 0;
  }

  for(iter =0; iter<maxiter;iter++){
    Vcr<T> mp=(*this)*p;
    T pap=dot(mp,p);
    T alpha=zr/pap;
    x+=alpha*p;
    r-=alpha*mp;
    if(r.twonorm()<=stp)break;//para la�o se convergiu
    z=preconding(r,pcn);//pr�-condicionamento
    T zrold=zr;
    zr=dot(z,r);
    T beta=zr/zrold;
    p=z+beta*p;
  }

  eps=r.twonorm();
  if(iter==maxiter) return 1;
  else return 0;
}*/


/*template<class T> int AbsMtx<T>::GMRES(Vcr<T>& x, const Vcr<T>& b, T& eps, int& iter, int pcn, int m){ //GMRES
//GMRES com opera��o de "restart", utiliza pr�-condicionador para um sistema do tipo Ax=b
//retorna 0 se teve sucesso e 1 se ocorreu algum problema
//A: Matriz quaquer
//x: na entrada � o chute inicial, ap�s passar pela fun��o torna-se a solu��o aproximada
//b: vetor livre do lado direito
//eps: na entrada -> crit�rio para fim de itera��o, na sa�da res�duo da solu��o aproximada
//iter: na entrada-> m�paximo n�mero de itera��es; na sa�da-> n�mero de itera��as feitas durante a execu��o da fun��o
//pcn-> �ndice para escolah do pr�-condicionador
//m -> n�mero de itera��es ap�s as quais o GMRES � reiniciado
  const int maxiter=iter;
  const T stp=(preconding(b,pcn)).twonorm()*eps;
  Vcr<T> r=preconding(b-(*this)*x,pcn);
  T beta=r.twonorm();
  bool conv=false;
  if(m>this->nrows) error("n�mero de itera��es para restart � maior do que o n�mero de linhas");
  if(fabs(beta)<=stp){//finaliza o calculo se o chute inicial for a solu��o
    eps=fabs(beta);
    iter=0;
    return 0;
  }

  //obten��o de uma base ortonormal para o espa�o de Krilov
  Vcr<T>** v=new Vcr<T>* [m+1];
  for(int i=0;i<=m;i++) v[i]=new Vcr<T>(this->nrows);//i-�sima base

  //Matriz de Hessenberg->h[m+1][m]
  //s� a parte diferente de zero de h � armazenada
  T** h=new T* [m];
  for(int i=0;i<m;i++) h[i]=new T [i+2];

  iter=1;
  while(iter<=maxiter){//itera��es para gmres(m->par�metro de "restart")
    *v[0]=r/beta;
    Vcr<T> g(m+1);
    g[0]=beta;

    Vcr<T> cs(m),sn(m);//rota��o de Givens
    int k;
    for(k=0;k<m && iter<=maxiter;k++,iter++){
       //cout<<iter;
       //cout<<"\n";
      //ortogonaliza��o:
      Vcr<T> w=preconding((*this)*(*v[k]),pcn);
      T nmw=w.twonorm();
      for(int i=0;i<=k;i++){
        h[k][i]=dot(w,*v[i]);
        w-=h[k][i]*(*v[i]);
      }
      h[k][k+1]=w.twonorm();
      //verifica o ortogonaliza��o: se h[k][k+1] � pequeno, faz-se uma reortogonaliza��o
      if(nmw+1.0e-4*h[k][k+1]==nmw){
        for(int i=0;i<=k;i++){
          T hri = dot(w,*v[i]);
          h[k][i]+=hri;
          w-=hri*(*v[i]);
        }
        h[k][k+1]=w.twonorm();
      }
      if(h[k][k+1]==0) error("divis�o por zero no GMRES");
      *v[k+1]=w/h[k][k+1];

      //aplicando a rota��o de Givens para a coluna k de h
      for(int i=0;i<k;i++){
        T tv[2]={cs[i],sn[i]};
        T tmp=dot(&h[k][i],tv,2);
        h[k][i+1]= -sn[i]*h[k][i]+cs[i]*h[k][i+1];
        h[k][i]=tmp;
      }

      //gerando os termos para a rora��o de Givens, cs e sn
      if(h[k][k+1]==0){
        cs[k]=1;
        sn[k]=0;
      }else{
        T tpm=sqrt(dot(&h[k][k],&h[k][k],2));
        cs[k]=h[k][k]/tpm;
        sn[k]=h[k][k+1]/tpm;
      }

      //aplicando a rota��o de Givens para a coluna k de h e g
      T tv[2]={cs[k],sn[k]};
      h[k][k]=dot(&h[k][k],tv,2);
      T tmp=dot(&g[k],tv,2);
      g[k+1]=-sn[k]*g[k]+cs[k]*g[k+1];
      g[k]=tmp;
      if(fabs(g[k+1])<=stp){//para quando o res�duo for pequeno
        k++;
        break;
      }
    }

    //resolve o sistema triangular superior
    for(int i=k-1;i>=0;i--){
      for(int j=i+1;j<k;j++)g[i]-=h[j][i]*g[j];
      g[i]/=h[i][i];
    }

    //recalcula x
    for(int i=0;i<k;i++)x+=(*v[i])*g[i];

    //calcula o res�duo e checa a converg�ncia
    r=preconding(b-(*this)*x,pcn);
    beta=r.twonorm();
    if(fabs(beta)<=stp){
      conv=true;
      break;
    }
  }

  eps=(b-(*this)*x).twonorm();//res�duo final

  for(int i=0;i<=m;i++)delete v[i];
  delete[] v;
  for(int i=0;i<m;i++)delete[] h[i];
  delete[] h;

  if(conv) return 0;
  else return 1;
}*/

/*template<class T> int AbsMtx<T>::GMRES(Vcr<T>& x, const Vcr<T>& b, double& eps, int& iter, int pcn, int m){ //GMRES
//GMRES com opera��o de "restart", utiliza pr�-condicionador para um sistema do tipo Ax=b
//retorna 0 se teve sucesso e 1 se ocorreu algum problema
//A: Matriz quaquer
//x: na entrada � o chute inicial, ap�s passar pela fun��o torna-se a solu��o aproximada
//b: vetor livre do lado direito
//eps: na entrada -> crit�rio para fim de itera��o, na sa�da res�duo da solu��o aproximada
//iter: na entrada-> m�paximo n�mero de itera��es; na sa�da-> n�mero de itera��as feitas durante a execu��o da fun��o
//pcn-> �ndice para escolah do pr�-condicionador
//m -> n�mero de itera��es ap�s as quais o GMRES � reiniciado
  const int maxiter=iter;
  const double stp=(preconding(b,pcn)).twonorm()*eps;
  Vcr<T> r=preconding(b-(*this)*x,pcn);
  T beta=r.twonorm();
  bool conv=false;
  if(m>this->nrows) error("n�mero de itera��es para restart � maior do que o n�mero de linhas");
  if(fabs(beta)<=stp){//finaliza o calculo se o chute inicial for a solu��o
    eps=fabs(beta);
    iter=0;
    return 0;
  }

  //obten��o de uma base ortonormal para o espa�o de Krilov
  T** v=new T* [m+1];
  for(int i=0;i<=m;i++){
     cout<<i;
     cout<<"\n";
     v[i]=new T [this->nrows];//i-�sima base
  }
  T var;

  //Matriz de Hessenberg->h[m+1][m]
  //s� a parte diferente de zero de h � armazenada
  T** h=new T* [m];
  for(int i=0;i<m;i++) h[i]=new T [i+2];

  iter=1;
  while(iter<=maxiter){//itera��es para gmres(m->par�metro de "restart")
    //v[0]=r/beta;
    for(int i=0;i<this->nrows;i++)v[0][i]=r[i]/beta;
    Vcr<T> g(m+1);
    g[0]=beta;

    Vcr<T> cs(m),sn(m);//rota��o de Givens
    int k;
    for(k=0;k<m && iter<=maxiter;k++,iter++){
      //ortogonaliza��o:
      //Vcr<T> w=preconding((*this)*(v[k]),pcn);
      Vcr<T> w=preconding((*this)*Vcr<T>(this->nrows,v[k]),pcn);
      T nmw=w.twonorm();
      for(int i=0;i<=k;i++){
        //h[k][i]=dot(w,v[i]);
        //w-=h[k][i]*(v[i]);
        var=0;
        for(int i2=0;i2<this->nrows;i2++)var+= w[i2]*v[i][i2];
        h[k][i]=var;
        for(int i2=0;i2<this->nrows;i2++)w[i2]-=var*v[i][i2];

      }
      h[k][k+1]=w.twonorm();
      //verifica o ortogonaliza��o: se h[k][k+1] � pequeno, faz-se uma reortogonaliza��o
      if(nmw+1.0e-4*h[k][k+1]==nmw){
        for(int i=0;i<=k;i++){
          //T hri = dot(w,v[i]);
          T hri=0.;
          for(int i2=0;i2<this->nrows;i2++)hri+= w[i2]*v[i][i2];
          h[k][i]+=hri;
          //w-=hri*(v[i]);
          for(int i2=0;i2<this->nrows;i2++)w[i2]-=hri*v[i][i2];
        }
        h[k][k+1]=w.twonorm();
      }
      if(h[k][k+1]==0) error("divis�o por zero no GMRES");
      //v[k+1]=w/h[k][k+1];
      var=h[k][k+1];
      for(int i2=0;i2<this->nrows;i2++)v[k+1][i2]=w[i2]/var;

      //aplicando a rota��o de Givens para a coluna k de h
      for(int i=0;i<k;i++){
        T tv[2]={cs[i],sn[i]};
        T tmp=dot(&h[k][i],tv,2);
        h[k][i+1]= -sn[i]*h[k][i]+cs[i]*h[k][i+1];
        h[k][i]=tmp;
      }

      //gerando os termos para a rora��o de Givens, cs e sn
      if(h[k][k+1]==0){
        cs[k]=1;
        sn[k]=0;
      }else{
        T tpm=sqrt(dot(&h[k][k],&h[k][k],2));
        cs[k]=h[k][k]/tpm;
        sn[k]=h[k][k+1]/tpm;
      }

      //aplicando a rota��o de Givens para a coluna k de h e g
      T tv[2]={cs[k],sn[k]};
      h[k][k]=dot(&h[k][k],tv,2);
      T tmp=dot(&g[k],tv,2);
      g[k+1]=-sn[k]*g[k]+cs[k]*g[k+1];
      g[k]=tmp;
      if(fabs(g[k+1])<=stp){//para quando o res�duo for pequeno
        k++;
        break;
      }
    }

    //resolve o sistema triangular superior
    for(int i=k-1;i>=0;i--){
      for(int j=i+1;j<k;j++)g[i]-=h[j][i]*g[j];
      g[i]/=h[i][i];
    }

    //recalcula x
    //for(int i=0;i<k;i++)x+=(v[i])*g[i];
    for(int i=0;i<k;i++){
       for(int i2=0;i2<this->nrows;i2++){
          var=v[i][i2]*g[i];
          x[i2]+=var;
       }
    }

    //calcula o res�duo e checa a converg�ncia
    r=preconding(b-(*this)*x,pcn);
    beta=r.twonorm();
    if(fabs(beta)<=stp){
      conv=true;
      break;
    }
  }

  eps=(b-(*this)*x).twonorm();//res�duo final

  for(int i=0;i<=m;i++)delete v[i];
  delete[] v;
  for(int i=0;i<m;i++)delete[] h[i];
  delete[] h;

  if(conv) return 0;
  else return 1;
} */

//Fun��es para Matriz Cheia:
//membros:



template<class T> FullMtx<T>::FullMtx(int n, int m, const T** const dbp){//construtor a partir de uma �rea j� armazenada, n->n de linhas, m-> n de colunas
	this->nrows=n;
	ncols=m;
	mx=new T* [this->nrows];
	for(int i=0; i<this->nrows; i++){
		mx[i] = new T [ncols];
		for(int j = 0; j< ncols; j++) mx[i][j]=dbp[i][j];
	}
}


template<class T> FullMtx<T>::FullMtx(int n, int m, T a){//construtor com valores da matriz igual a um escalar t que tem como default zero
	this->nrows=n;
	ncols=m;
	mx=new T* [this->nrows];
	for(int i=0; i<this->nrows; i++){
		mx[i] = new T [ncols];
		for(int j = 0; j< ncols; j++) mx[i][j]=a;
	}
}


template<class T> FullMtx<T>::FullMtx(const FullMtx& mat){//construtor de c�pia
	this->nrows=mat.nrows;
	ncols=mat.ncols;
	mx=new T* [this->nrows];
	for(int i=0; i<this->nrows; i++){
		mx[i] = new T [ncols];
		for(int j = 0; j< ncols; j++) mx[i][j]=mat.mx[i][j];
	}
}

template<class T> Vcr<T> FullMtx<T>::preconding(const Vcr<T>& r, int precn) {//const{
  if(precn==0){//sem pr�-condicionamento
    return r;
  }else if(precn==1){//pr�-condicionamento diagonal
     Vcr<T> z(this->nrows);
     for(int i=0; i< this->nrows;i++)z[i]=r[i]/mx[i][i];
     return z;
  }else if(precn==2){//pr�-condicionador sim�trico SOR
     const T omega =1.2;//par�metro do SOR
     Vcr<T> z(this->nrows);
     for(int i=0;i<this->nrows;i++){
       T sum=0;
       for(int j=0;j<i;j++)sum += mx[i][j]*z[j];
       z[i]=(r[i]-omega*sum)/mx[i][i];
     }
     for(int i=this->nrows-1;i>=0;i--){
       T sum=0;
       for(int j=i+1;j<this->nrows;j++)
         sum+=mx[i][j]*z[j];
       z[i]-=omega*sum/mx[i][i];
     }
     return z;
  }else{
     error("n�mero inv�lido para sele��o de precondicionador > 2 ");
     return Vcr<T>();
  }
}


template<class T> FullMtx<T>& FullMtx<T>::operator =(const FullMtx& mat){//sobrecarga de operador =
	if (this != &mat){//verifica se n�o se est� fazendo uma atribui��o de um objeto com ele mesmo
		if(this->nrows != mat.nrows || ncols != mat.ncols){
			//error("erro no tamanho da matriz coluna!=linha, ocorrido na opera��o =" );

                   for(int i=0; i<this->nrows;i++) delete[] mx[i];
                   delete [] mx;

                   this->nrows=mat.nrows;
	           ncols=mat.ncols;
                   mx=new T* [this->nrows];
	           for(int i=0; i<this->nrows; i++){
		      mx[i] = new T [ncols];
		      for(int j = 0; j< ncols; j++) mx[i][j]=mat.mx[i][j];
                 }
                }
                else{
                  for(int i =0;i<this->nrows; i++)
                     for(int j =0;j<ncols;j++)mx[i][j]=mat.mx[i][j];
                }
	}
	else error("erro, tentando atribuir uma matriz a ela pr�pria, ocorrido na opera��o =" );
	return *this;
}


template<class T> FullMtx<T>& FullMtx<T>::operator +=(const FullMtx& mat){//sobrecarga de operador +=
		if(this->nrows != mat.nrows || ncols != mat.ncols)
			error("erro no tamanho da matriz coluna!=linha, ocorrido na opera��o =" );
		for(int i=0;i<this->nrows;i++)
			for(int j=0;j<ncols;j++)mx[i][j]+=mat.mx[i][j];
		return *this;
}

template<class T> FullMtx<T>& FullMtx<T>::operator -=(const FullMtx& mat){//sobrecarga de operador -=
		if(this->nrows != mat.nrows || ncols != mat.ncols)
			error("erro no tamanho da matriz coluna!=linha, ocorrido na opera��o =" );
		for(int i=0;i<this->nrows;i++)
			for(int j=0;j<ncols;j++)mx[i][j]-=mat.mx[i][j];
		return *this;
}

template<class T> FullMtx<T> FullMtx<T>::operator -() const{//sobrecarga de operador un�rio -
	FullMtx<T> zero(this->nrows,ncols);
	return zero-*this;
}

template<class T> FullMtx<T> FullMtx<T>::operator +(const FullMtx& mat) const{//sobrecarga de operador +
	FullMtx<T> sum = *this;
	sum += mat;
	return sum;
}

template<class T> FullMtx<T> FullMtx<T>::operator -(const FullMtx& mat) const{//sobrecarga de operador -
	FullMtx<T> sum = *this;
	sum -= mat;
	return sum;
}

template<class T> FullMtx<T> FullMtx<T>::operator *(T a) const{//multiplica��o de matriz por um escalar
        FullMtx<T> prod(this->nrows,ncols);
	for (int i=0;i<this->nrows;i++)
          for(int j=0;j<ncols;j++)prod.mx[i][j]=a*mx[i][j];
	return prod;
}

template<class T> Vcr<T> FullMtx<T>::operator *(const Vcr<T>& v) const{//multiplica��o de matriz por vetor
	if(ncols != v.size()) 
		error("erro no tamanho da matriz coluna!=tamanho do vetor, ocorrido na opera��o Mat*Vet" );
	Vcr<T> tm(this->nrows);
	for (int i=0;i<this->nrows;i++)
		for(int j=0;j<ncols;j++)tm[i]+=mx[i][j]*v[j];
	return tm;
}

template<class T> FullMtx<T> FullMtx<T>::operator *(const FullMtx& mat) const{//multiplica��o de matriz por matriz
	if(ncols != mat.nrows)
		error("erro no n�mero de colunas em mat1!= n�mero de linhas em mat2, ocorrido na opera��o mat1*mat2" );
	FullMtx<T> nova(this->nrows,mat.ncols,0.);
        //FullMtx<T> tmpx=*this;
	for (int i=0;i<this->nrows;i++)
		for(int j=0;j<mat.ncols;j++)
                    for(int k=0;k<ncols;k++)nova.mx[i][j]+=mx[i][k]*mat.mx[k][j];
	return nova;
}

template<class T> void FullMtx<T>::GaussElim(Vcr<T>& bb) const{ //elimina��o gaussiana sem pivoteamento
	if(this->nrows != bb.size() || this->nrows != ncols) error("tamanho de matriz ou de vetor difere");
	FullMtx<T> tmpx=*this;

	for(int k=0; k<this->nrows-1;k++){
		if(tmpx.mx[k][k]==0)
			error("pivot � zero em FullMtx::GaussElim()");
		for(int i=k+1;i<this->nrows;i++){
			if(tmpx.mx[i][k]!=0){
				T mult=tmpx.mx[i][k]/tmpx.mx[k][k];
				tmpx.mx[i][k]=mult;
				for (int j=k+1;j<ncols;j++) tmpx.mx[i][j]-=mult*tmpx.mx[k][j];
			}
		}
	}

	for(int i=1;i<this->nrows;i++)
		for(int j=0;j<i;j++) bb[i]-=tmpx.mx[i][j]*bb[j];

	for(int i=this->nrows-1;i>=0;i--){
			for(int j=i+1;j<this->nrows;j++)bb[i]-=tmpx.mx[i][j]*bb[j];
			bb[i]/=tmpx.mx[i][i];
		}
}


/*template<class T> void FullMtx<T>::Inverte1(FullMtx<T>& bb) const{ //elimina��o gaussiana sem pivoteamento
	if(this->nrows != bb.nrows || this->nrows != ncols) error("tamanho de matriz difere");
	FullMtx<T> tmpx=*this;

        for(int i=0;i<this->nrows;i++){
          for(int j=0;j<ncols;j++){
            if(i==j)bb[i][j]=1.0;
            else bb[i][j]=0;
          }
        }

	for(int k=0; k<this->nrows-1;k++){
		if(tmpx[k][k]==0)
			error("pivot � zero em FullMtx::Inverte1()");
		for(int i=k+1;i<this->nrows;i++){
			if(tmpx[i][k]!=0){
				T mult=tmpx[i][k]/tmpx[k][k];
				//tmpx[i][k]=mult;
				for (int j=k;j<ncols;j++) tmpx[i][j]-=mult*tmpx[k][j];
                                for(int j=0;j<ncols;j++) bb[i][j]-=mult*bb[k][j];
			}
		}
	}

       //	for(int i=1;i<this->nrows;i++)
		//for(int j=0;j<i;j++) bb[i]-=tmpx[i][j]*bb[j];

        //for(int j=0;j<ncols;j++)bb[this->nrows-1][j]/=tmpx[this->nrows-1][ncols-1];
        //tmpx[this->nrows][ncols]=1.;
	for(int i=this->nrows-1;i>0;i--){
          for(int j=0;j<ncols;j++)bb[i][j]/=tmpx[i][i];
          for(int k=i-1;k>=0;k--){
            T mult=tmpx[k][i];
            for(int j=ncols-1;j>=0;j--) bb[k][j]-=mult*bb[i][j];
          }
          //for(int j=ncols-1;j<=0;j--) bb[i-2][j]/=tmpx[i-2][i-2];
        }
        for(int j=0;j<ncols;j++)bb[0][j]/=tmpx[0][0];
}   */

template<class T> void FullMtx<T>::Inverte1(FullMtx<T>& bb) const{ //elimina��o gaussiana sem pivoteamento
	if(this->nrows != bb.nrows || this->nrows != ncols) error("tamanho de matriz difere");
	FullMtx<T> tmpx=*this;

        Vcr<T> auxvec(ncols);
        for(int j=0;j<ncols;j++){
          for(int i=0;i<ncols;i++){
            if(i==j)auxvec[i]=1.0;
            else auxvec[i]=0.;
          }
          tmpx.GaussElimCP(auxvec);
          for(int k=0;k<ncols;k++)bb.mx[k][j]=auxvec[k];
        }
        
}

template<class T> T FullMtx<T>::InterpolaUnif(T lin, T col) const{

	int iL;
    int iC;
    double razL;
    double razC;
    if(lin>=mx[1][0] && lin<=mx[this->nrows-1][0]){
        double lmin=mx[1][0];
        double dell=mx[2][0]-mx[1][0];
        iL=floor((lin-lmin)/dell)+1+1;
        razL=(mx[iL][0]-lin)/(mx[iL][0]-mx[iL-1][0]);
    }
    else if(lin<mx[1][0]){
    	iL=1;
    	razL=0;
    }
    else{
    	iL=this->nrows-1;
    	razL=1;
    }

    if(col>=mx[0][1] && col<=mx[0][ncols-1]){
        double cmin=mx[0][1];
        double delc=mx[0][2]-mx[0][1];
        iC=floor((col-cmin)/delc)+1+1;
        razC=(mx[0][iC]-col)/(mx[0][iC]-mx[0][iC-1]);
    }
    else if(col<mx[0][1]){
    	iC=1;
    	razC=0;
    }
    else{
    	iC=ncols-1;
    	razC=1;
    }
    double lC1=(1-razL)*(mx[iL-1][iC-1])+razL*(mx[iL][iC-1]);
    double lC2=(1-razL)*(mx[iL-1][iC])+razL*(mx[iL][iC]);
    return (1-razC)*lC1+razC*lC2;

  }


template<class T> void FullMtx<T>::GaussElimPP(Vcr<T>& bb) const{ //elimina��o gaussiana com pivoteamento parcial, bb � o vetor livre, que tamb�m ser� o vetor solu��o
      if(this->nrows != bb.size()||this->nrows!=ncols) error("h� discord�ncia entre o tamanho do vetor e da matriz");
      FullMtx<T> tmpx = *this;

      Vcr<int> pvt(this->nrows); //vetor para fazer o pivoteamento parcial
      for(int k = 0; k<this->nrows;k++) pvt[k]=k; //registra a ordem inicial das linhas da matriz

      for(int k=0;k<this->nrows-1;k++){//loop principal
     //a decomposi��o � sempre feita com a linha a partir de k (i>=k) que tenha o maior termo matriz[i][k]
     // a linha deve ser permutada sempre que n�o tiver o maior termo. Efetivamente, apenas o termo de pvt � permutado
        int pc=k;
        double aet=fabs(tmpx.mx[pvt[k]][k]);
        for(int i=k+1;i<this->nrows;i++){
            if(fabs(tmpx.mx[pvt[i]][k])>aet){
               aet=fabs(tmpx.mx[pvt[i]][k]);
               pc=i;
            }
        }
        if(!aet)error("termo de diagonal � zero, erro ocorrido durante pivoteamento parcial");
        if(pc!=k){
        	swap(pvt[k],pvt[pc]);
        }
        int pvtk=pvt[k];
        T pivot=tmpx.mx[pvtk][k];

        for(int i=k+1;i<this->nrows;i++){
            int pvti=pvt[i];
            if(tmpx.mx[pvti][k]!=0){
                T mult=tmpx.mx[pvti][k]/pivot;
                tmpx.mx[pvti][k]=mult;
                for(int j=k+1;j<ncols;j++) tmpx.mx[pvti][j]-=mult*tmpx.mx[pvtk][j];
                }
            }
        }

        //forward substitution para a parte L da matriz L*y=P*b
        for(int i=1;i<this->nrows;i++)
           for(int j=0;j<i;j++) bb[pvt[i]]-=tmpx.mx[pvt[i]][j]*bb[pvt[j]];

        //back substitution para Ux=y
        Vcr<T> xx(this->nrows);//armazena a solu��o na ordem original
        for(int i=this->nrows-1;i>=0;i--){
           for(int j=i+1;j<ncols;j++) bb[pvt[i]]-=tmpx.mx[pvt[i]][j]*xx[j];
           xx[i]=bb[pvt[i]]/tmpx.mx[pvt[i]][i];
           }

        bb=xx;
     }

template<class T> void FullMtx<T>::GaussElimCP(Vcr<T>& bb) const{ //elimina��o gaussiana com pivoteamento completo,
      //bb � o vetor livre, que tamb�m ser� o vetor solu��o
     //esta subrotina � similar � de pivoteamento parcial, a diferen�a est� no fato de se buscar o termo de maior
     //coeficiente para a diagonal n�o s� pivoteando as linhas, mas tamb�m pivoteando as colunas
     if(this->nrows!=bb.size()||this->nrows!=ncols) error("h� discord�ncia entre o tamanho do vetor e da matriz");
     FullMtx<T> tmpx=*this;

     Vcr<int> px(this->nrows); //pivoteamento das linhas
     Vcr<int> qy(this->nrows); //pivoteamento das colunas
     for(int k=0;k<this->nrows;k++)px[k]=qy[k]=k;

     for(int k=0;k<this->nrows-1;k++){
        int pct=k, qdt=k;
        double aet=0;
        for(int i=k;i<this->nrows;i++){
           for(int j=k;j<this->nrows;j++){
              double tmp=fabs(tmpx.mx[px[i]][qy[j]]);
              if(tmp>aet){aet=tmp;pct=i;qdt=j;}
              }
           }
        if(!aet)error("problemas na elimina��o de Gauss por pivoteamento completo, termo diagonal igual a zero");
        swap(px[k],px[pct]);
        swap(qy[k],qy[qdt]);

        for(int i=k+1;i<this->nrows;i++){
           if(tmpx.mx[px[i]][qy[k]]!=0){
             T mult=tmpx.mx[px[i]][qy[k]]/tmpx.mx[px[k]][qy[k]];
             tmpx.mx[px[i]][qy[k]]=mult;
             for(int j=k+1;j<this->nrows;j++)
                tmpx.mx[px[i]][qy[j]]-=mult*tmpx.mx[px[k]][qy[j]];
           }
        }
     }

     for(int i=1;i<this->nrows;i++)
        for(int j=0;j<i;j++)
         bb[px[i]]-=tmpx.mx[px[i]][qy[j]]*bb[px[j]];

     Vcr<T> xx(this->nrows);
     for(int i=this->nrows-1;i>=0;i--){
        for(int j=i+1;j<this->nrows;j++) bb[px[i]]-=tmpx.mx[px[i]][qy[j]]*xx[qy[j]];
        xx[qy[i]]=bb[px[i]]/tmpx.mx[px[i]][qy[i]];
     }

     bb=xx;
   }

template<class T> void FullMtx<T>::LU(Vcr<T>& bb) const{
    if(this->nrows!=bb.size()||this->nrows!=ncols) error("h� discord�ncia entre o tamanho do vetor e da matriz");
    FullMtx<T> tmpx=*this;

	  for (int k = 0; k < this->nrows; k++) {
	  	  		for (int j = k; j < this->nrows; j++) {
	  	  				double sum = tmpx[k][j];
	  	  				for (int m = 0; m < k; m++) {
	  		  				sum -= tmpx[k][m] * tmpx[m][j];
	  	  				}
	  	  			tmpx[k][j]=sum;
	  	  		}
	  	  		for (int j = k+1; j < this->nrows; j++) {
	  	  				double sum = tmpx[j][k];
	  	  				for (int m = 0; m < k; m++) {
	  		  				sum-= tmpx[j][m] * tmpx[m][k];
	  	  				}
	  	  			tmpx[j][k]=sum/tmpx[k][k];
	  	  		}
	  	  	}

	  for (int i = 0; i < this->nrows; ++i) {
	  	  	        double sum = bb[i];
	  	  	        for (int j = 0; j < this->nrows; ++j) {
	  	  	            if (j < i) {
	  	  	                sum -= tmpx[i][j] * bb[j];
	  	  	            }
	  	  	        }
	  	  	        bb[i] = sum;
	  	 }

	  for (int i = this->nrows - 1; i >= 0; --i) {
	  	  	        double sum = bb[i];
	  	  	        for (int j = 0; j < this->nrows; ++j) {
	  	  	            if (j > i) {
	  	  	                sum -= tmpx[i][j] * bb[j];
	  	  	            }
	  	  	        }
	  	  	        bb[i] = sum / tmpx[i][i];
	  	  	    }
}



//Fun��es para Matriz Banda:
//membros e fun��es relacionadas:

  template<class T> BandMtx<T>::BandMtx(int n, int p, int r,const T** const t){//construtor a partir de �rea armazenada apontada por t
    this->nrows=n; //n�mero de equa��es a ser resolvidas
    bwlef=p;//range de elementos n�o-zero � esquerda
    bwrit=r;//range de elementos n�o-zero � direita

    bdmx= new T* [this->nrows];
    for(int i=0;i<this->nrows;i++){
      bdmx[i]=new T[bwlef+bwrit+1];//armazena o termo da diagonal principal, os termos n�o-zeros � esquerda e � direita da diagonal
      bdmx[i]+=bwlef;//offset, agora os termos na diagonal principal t�m �ndice i=0
    }

    for(int i=0;i<this->nrows;i++)
      for(int j=-bwlef;j<=bwrit;j++)bdmx[i][j]=t[i][j];//aten��o, observe que  t � armazenado da mesma maneira que bdmx
  }

  template<class T> BandMtx<T>::BandMtx(int n, int p,int r, T t){//construtor onde todos os termos t�m o valor t
    this->nrows=n; //n�mero de equa��es a ser resolvidas
    bwlef=p;//range de elementos n�o-zero � esquerda
    bwrit=r;//range de elementos n�o-zero � direita

    bdmx= new T* [this->nrows];
    for(int i=0;i<this->nrows;i++){
      bdmx[i]=new T[bwlef+bwrit+1];//armazena o termo da diagonal principal, os termos n�o-zeros � esquerda e � direita da diagonal
      bdmx[i]+=bwlef;//offset, agora os termos na diagonal principal t�m �ndice i=0
    }

    for(int i=0;i<this->nrows;i++)
      for(int j=-bwlef;j<=bwrit;j++)bdmx[i][j]=t;
  }


 template<class T> BandMtx<T>::BandMtx(int n,int p,int r, const FullMtx<T>& fm){//construtor a partir de uma matriz cheia
    this->nrows=n; //n�mero de equa��es a ser resolvidas
    bwlef=p;//range de elementos n�o-zero � esquerda
    bwrit=r;//range de elementos n�o-zero � direita

    bdmx= new T* [this->nrows];
    for(int i=0;i<this->nrows;i++){
      bdmx[i]=new T[bwlef+bwrit+1];//armazena o termo da diagonal principal, os termos n�o-zeros � esquerda e � direita da diagonal
      bdmx[i]+=bwlef;//offset, agora os termos na diagonal principal t�m �ndice i=0
    }

    for(int i=0;i<this->nrows;i++)
      for(int j=-bwlef;j<=bwrit;j++)bdmx[i][j]=0;//armazenamento inicial com zeros. A pr�ncipio isto seria desnecess�rio j� que a seguir haver� uma armazenagem com uma matriz cheia

    for(int i=0;i<this->nrows;i++){
      int ip=max(i-bwlef,0);
      int ir=min(i+bwrit,this->nrows-1);
      for(int j=ip;j<=ir;j++)bdmx[i][j-i]=fm.mx[i][j];//observe que j-i estar� sempre entre -bwlef e bwrit
    }
  }

  template<class T> BandMtx<T>::BandMtx(const BandMtx& bd){//construtor c�pia
    this->nrows=bd.nrows; //n�mero de equa��es a ser resolvidas
    bwlef=bd.bwlef; //range de elementos n�o-zero � esquerda
    bwrit=bd.bwrit; //range de elementos n�o-zero � direita

    bdmx=new T* [this->nrows];
    for(int i=0;i<this->nrows;i++){
      bdmx[i]=new T[bwlef+bwrit+1];//armazena o termo da diagonal principal, os termos n�o-zeros � esquerda e � direita da diagonal
      bdmx[i]+=bwlef;//offset, agora os termos na diagonal principal t�m �ndice i=0
    }

    for(int i=0;i<this->nrows;i++)
      for(int j=-bwlef;j<=bwrit;j++)bdmx[i][j]=bd[i][j];
 }  


template<class T> Vcr<T> BandMtx<T>::preconding(const Vcr<T>& r, int precn) {//const{
  if(precn==0){//sem pr�-condicionamento
    return r;
  }else if(precn==1){//pr�-condicionamento diagonal
     Vcr<T> z(this->nrows);
	 //#pragma omp parallel for num_threads(ntrd)
     for(int i=0; i< this->nrows;i++)z[i]=r[i]/bdmx[i][0];
     return z;
  }else if(precn==2){//pr�-condicionador sim�trico SOR
     const T omega =1.2;//par�metro do SOR
     Vcr<T> z(this->nrows);
     for(int i=0;i<this->nrows;i++){
       T sum=0;
       int ip=max(i-bwlef,0);
       for(int j=ip;j<i;j++)
         sum+=bdmx[i][j-i]*z[j];
       z[i]=(r[i]-omega*sum)/bdmx[i][0];
     }
     for(int i=this->nrows-1;i>=0;i--){
       T sum=0;
       int ir=min(i+bwrit,this->nrows-1);
       for(int j=i+1;j<=ir;j++)
         sum+=bdmx[i][j-i]*z[j];
       z[i]-=omega*sum/bdmx[i][0];
     }
     return z;
  }else{
     error("n�mero inv�lido para sele��o de precondicionador > 2");
     return Vcr<T>();
  }
}


 template<class T> T BandMtx<T>::cheia(int i, int j) const{//trata a matriz com os �ndices normais de matriz cheia
    int entran=j-i;
    T val;
    if(entran<-bwlef||entran>bwrit)val=0;
    else val=bdmx[i][entran];
    return val;
 }


 template<class T> BandMtx<T>& BandMtx<T>::operator=(const BandMtx& bd){//sobrecarga de operador =
   if(this->nrows>0){
     if(this->nrows!=bd.nrows||bwlef!=bd.bwlef||bwrit!=bd.bwrit){
       //error("problemas no assignment de uma matriz banda, tamanho ou range da banda n�o confere");
         for(int i=0;i<this->nrows;i++)delete[](bdmx[i]-=bwlef);//no construtor no momento do aramzenamento � feito um "offset", os termos diagonais tem �ndice zero, por isto, � necess�rio o -=bwleft
         delete[] bdmx;
         this->nrows=bd.nrows;
         bwlef=bd.bwlef;
         bwrit=bd.bwrit;
         bdmx= new T* [this->nrows];
         for(int i=0;i<this->nrows;i++){
          bdmx[i]=new T[bwlef+bwrit+1];//armazena o termo da diagonal principal, os termos n�o-zeros � esquerda e � direita da diagonal
          bdmx[i]+=bwlef;//offset, agora os termos na diagonal principal t�m �ndice i=0
        }
     }
   }
   else{
     this->nrows=bd.nrows;
     bwlef=bd.bwlef;
     bwrit=bd.bwrit;
     bdmx= new T* [this->nrows];
     for(int i=0;i<this->nrows;i++){
      bdmx[i]=new T[bwlef+bwrit+1];//armazena o termo da diagonal principal, os termos n�o-zeros � esquerda e � direita da diagonal
      bdmx[i]+=bwlef;//offset, agora os termos na diagonal principal t�m �ndice i=0
    }

   }

   for(int j=0;j<this->nrows;j++)
     for(int k = -bwlef; k<=bwrit; k++)bdmx[j][k]=bd[j][k];
     return *this;
 }

 template<class T> BandMtx<T> BandMtx<T>::operator *(T a) const{//multiplica��o de matriz por escalar mat*a
    BandMtx<T> prod=*this;
    for(int i=0;i<this->nrows;i++)
     for(int j=-bwlef;j<=bwrit;j++)prod[i][j]*=a;
    return prod;
  }


 template<class T> Vcr<T> BandMtx<T>::operator*(const Vcr<T>& vec) const{//sobrecarga de multiplica��o, matriz por vetor
   if(this->nrows!=vec.size()) error("tamanho de matriz e vetor s�o diferentes");
   //observe que esta mensagem de erro seria mais correta se fosse comparado ncols com size,
   //mas isto n�o � poss�vel para uma matriz banda, pontanto, esta checagem � feita partindo do pressuposto que se est� trabalhando
   //com uma matriz quadrada

   Vcr<T> tm(this->nrows);
   for(int i=0;i<this->nrows;i++){
     int ip=max(i-bwlef,0);
     int ir=min(i+bwrit,this->nrows-1);
     for(int j= ip; j<= ir; j++) tm[i]+=bdmx[i][j-i]*vec[j];
   }

   return tm;
 }


 template<class T> BandMtx<T>& BandMtx<T>::operator +=(const BandMtx& mat){//sobrecarga de operador +=
		if(this->nrows != mat.nrows || bwlef != mat.bwlef || bwrit != mat.bwrit)
			error("erro no tamanho da matriz coluna, ocorrido na opera��o +=" );
		for(int i=0;i<this->nrows;i++)
			for(int j=-bwlef;j<=bwrit;j++)this->bdmx[i][j]+=mat[i][j];
		return *this;
}


template<class T> BandMtx<T>& BandMtx<T>::operator -=(const BandMtx& mat){//sobrecarga de operador -=
		if(this->nrows != mat.nrows || bwlef != mat.bwlef || bwrit != mat.bwrit)
			error("erro no tamanho da matriz coluna, ocorrido na opera��o +=" );
		for(int i=0;i<this->nrows;i++)
			for(int j=-bwlef;j<=bwrit;j++)this->bdmx[i][j]-=mat[i][j];
		return *this;
}

template<class T> BandMtx<T> BandMtx<T>::operator -() const{//sobrecarga de operador un�rio -
	BandMtx<T> zero(this->nrows,bwlef,bwrit);
	return zero-*this;
}

template<class T> BandMtx<T> BandMtx<T>::operator -(const BandMtx& mat) const{//sobrecarga de operador -
	BandMtx<T> sum=*this;
        sum-=mat;
	return sum;
}

template<class T> BandMtx<T> BandMtx<T>::operator +(const BandMtx& mat) const{//sobrecarga de operador +
	BandMtx<T> sum=*this;
        sum+=mat;
	return sum;
}


 template<class T> ostream& operator<<(ostream& s, const BandMtx<T>& mat){//saida de valores de Matriz banda
   int esq=mat.esq();
   int dir=mat.dir();
   int lin=mat.lin();
   for ( int i = 0; i<lin; i++){
     int ip=max(i-esq,0);
     int ir=min(i+dir,lin-1);
     for(int j=0;j<lin;j++) {
       s.width(6);
       s.precision(5);
       if(j<ip||j>ir)s<<0<<"  ";
       else s<<mat[i][j-i]<<"  ";
     }
     s<<"\n";
   }
   s<<"\n";
   return s;
}

template<class T> void BandMtx<T>::GaussElim(Vcr<T>& bb)const{//LU para matriz banda sem pivoteamento
  if(this->nrows!=bb.size())error("tamanho de matriz e vetor n�o confere, erro ocorrido na rotina LU sem pivoteamento para matriz banda");
  BandMtx<T> tmx=*this;

  for(int k=0;k<this->nrows;k++){
    if(tmx[k][0]==0)error("termo na diagonal principal = 0, ocorrido no LU sem pivoteamento da matriz banda");
    int kbf=min(this->nrows-1-k,bwlef);
    int kbr=min(this->nrows-1-k,bwrit);
    for(int i=1;i<=kbf;i++){
      int kpi=k+i;
      if(tmx[kpi][-i]!=0){
        T dmul=tmx[kpi][-i]/tmx[k][0];
        tmx[kpi][-i]=dmul;
        for(int j=1;j<=kbr;j++)
          tmx[kpi][j-i]-=dmul*tmx[k][j];
      }
    }
  }

  //forward substitution
  for(int k=1;k<this->nrows;k++){
    int kf=min(k,bwlef);
    for(int j=1;j<=kf;j++)
      bb[k]-=tmx[k][-j]*bb[k-j];
  }

  //backward substitution
  for(int k=this->nrows-1;k>=0;k--){
    int kbr=min(this->nrows-1-k,bwrit);
    for(int j=1;j<=kbr;j++)
      bb[k]-=tmx[k][j]*bb[k+j];
    bb[k]/=tmx[k][0];
  }
 }

template<class T> void BandMtx<T>::GaussElimPP(Vcr<T>& bb)const{//LU para matriz banda com pivoteamento parcial
  if(this->nrows!=bb.size())error("tamanho de matriz e vetor n�o confere, erro ocorrido na rotina LU com pivoteamento parcial para matriz banda");

  BandMtx<T> tx(this->nrows,bwlef,min(this->nrows-1,bwlef+bwrit));
#pragma omp parallel for num_threads(nthrdMatriz)
  for(int i=0;i<this->nrows;i++)
    for(int j=-bwlef;j<=bwrit;j++)tx[i][j]=bdmx[i][j];//a princ�pio, isto poderia ser feito automaticamente pelo assignment tx=*this

  Vcr<int> pvt(this->nrows);

  const int nrowsmone=tx.nrows-1;
  for(int k=0;k<nrowsmone;k++){
    int kbrow=min(nrowsmone-k, tx.bwlef);
    int kbcol=min(nrowsmone-k,tx.bwrit);

    int pc=k;
    double aet=fabs(tx[k][0]);
    for(int i=1;i<=kbrow;i++){
      if(fabs(tx[k+i][-i])>aet){
        aet=fabs(tx[k+i][-i]);
        pc=k+i;
      }
    }
    if(!aet)error("Termo na diagonal principal = zero. Problema ocorrido no LU com pivoteamento parcial para matriz banda");
    pvt[k]=pc;

    if(pc!=k){
      for(int j=0;j<=kbcol;j++) swap(tx[pc][k+j-pc],tx[k][j]);
    }

//#pragma omp parallel for num_threads(nthrdMatriz)
    for(int i=1;i<=kbrow;i++){
      int kpi=k+i;
      if(tx[kpi][-i]!=0){
        T dmul=tx[kpi][-i]/tx[k][0];
        tx[kpi][-i]=dmul;
        for(int j=1;j<=kbcol;j++)
          tx[kpi][j-i]-=dmul*tx[k][j];
      }
    }
  }

  //forward substitution
  //for(int k=0;k<this->nrowsmone;k++){
    //int kbrow=min(this->nrowsmone-k,tx.bwlef);
  for(int k=0;k<nrowsmone;k++){
    int kbrow=min(nrowsmone-k,tx.bwlef);
    int pvtk=pvt[k];
    T sb=bb[pvtk];
    if(k!=pvtk)swap(bb[k],bb[pvtk]);
    for(int j=1;j<=kbrow;j++) bb[k+j]-=tx[k+j][-j]*sb;
  }

  //backward substitution
  //for(int k=this->nrowsmone;k>=0;k--){
    //int kb=min(this->nrowsmone-k,tx.bwrit);
  for(int k=nrowsmone;k>=0;k--){
    int kb=min(nrowsmone-k,tx.bwrit);
    for(int j=1;j<=kb;j++)bb[k]-=tx[k][j]*bb[k+j];
    bb[k]/=tx[k][0];
  }
 }


//Fun��es para Matriz Esparsa:
//membros e fun��es relacionadas:

 template<class T> void SparseMtx<T>::geraNivelFillinILU(std::vector<int>& level,std::vector<int>& linhaLev,
		 std::vector<int>& colunaLev) {
     const int INF = 1e9;
     LevelMap levelMap;
     int n = this->nrows;

     // Inicializa termos da matriz original com nível 0
     for (int i = 0; i < n; ++i) {
         for (int k = fnz[i]; k < fnz[i + 1]; ++k) {
             int j = clm[k];
             levelMap[key(i, j)] = 0;
         }
     }

     // Iterativamente propaga os níveis até não haver mais mudanças
     bool houveMudanca = true;
     while (houveMudanca) {
         houveMudanca = false;

         // Atualiza a estrutura L[i] a partir de levelMap
         std::vector<std::vector<int>> L(n);
         for (const auto& [chave, nivel] : levelMap) {
             int i = chave / n;
             int j = chave % n;
             L[i].push_back(j);
         }
         for (int i = 0; i < n; ++i) {
             std::sort(L[i].begin(), L[i].end());
             L[i].erase(std::unique(L[i].begin(), L[i].end()), L[i].end()); // remove duplicatas
         }

         // Aplica a regra de propagação
         for (int i = 0; i < n; ++i) {
             for (int k : L[i]) {
                 if (k >= i) continue;
                 for (int j : L[k]) {
                     if (j <= k) continue;

                     auto chave_ij = key(i, j);
                     auto chave_ik = key(i, k);
                     auto chave_kj = key(k, j);

                     int levij = levelMap.count(chave_ij) ? levelMap[chave_ij] : INF;
                     int levik = levelMap.count(chave_ik) ? levelMap[chave_ik] : INF;
                     int levkj = levelMap.count(chave_kj) ? levelMap[chave_kj] : INF;

                     int novoLev = levik + levkj + 1;
                     if (novoLev < levij && novoLev <= rank) {
                         levelMap[chave_ij] = novoLev;
                         houveMudanca = true;
                     }
                 }
             }
         }
     }

     // Converte o resultado final para vetores
     for (const auto& entry : levelMap) {
         int i = entry.first / n;
         int j = entry.first % n;
         linhaLev.push_back(i);
         colunaLev.push_back(j);
         level.push_back(entry.second);
     }
 }

 template<class T> void SparseMtx<T>::montaILUfillinCSR(
 	    const std::vector<int>& level,  const std::vector<int>& linhaLev,const std::vector<int>& colunaLev
 	) {
 	    noZeroIlu = level.size();
 	    veclmILU = new int[noZeroIlu];
 	    veclevILU = new int[noZeroIlu];
 	    vecRelILU = new int[noZeroIlu];
 	    vecfnzILU = new int[this->nrows + 1];

 	    int n=this->nrows;

 	    // Passo 1: Agrupar termos por linha usando um unordered_map
 	       std::unordered_map<int, std::vector<std::pair<int, int>>> termosPorLinha;

 	       for (int idx = 0; idx < noZeroIlu; ++idx) {
 	           int i = linhaLev[idx];
 	           int j = colunaLev[idx];
 	           int lev = level[idx];
 	           termosPorLinha[i].emplace_back(j, lev);  // Agrupa por linha (CSR)
 	       }


 	       // Passo 2: Ordenar termos por coluna e preencher os vetores de saída
 	       int offset = 0;
 	       vecfnzILU[0] = 0;

 	       for (int i = 0; i < n; ++i) {
 	           if (termosPorLinha.count(i)) {
 	               auto& termos = termosPorLinha[i];
 	               std::sort(termos.begin(), termos.end());  // Ordena por coluna

 	               for (const auto& [j, lev] : termos) {
 	                   veclmILU[offset] = j;
 	                   veclevILU[offset] = lev;
 	                   offset++;
 	               }
 	           }
 	           vecfnzILU[i + 1] = offset;
 	       }

 	       for (int i = 0; i < n; ++i) {
 	    	   int iniLinSp=fnz[i];
 	    	   int fimLinSp=fnz[i+1];

 	    	   int iniLinIlu=vecfnzILU[i];
 	    	   int fimLinIlu=vecfnzILU[i+1];

 	    	   for(int iILU=iniLinIlu; iILU<fimLinIlu; iILU++){
 	    		   vecRelILU[iILU]=-1;
 	    		   for(int iSpar=iniLinSp; iSpar<fimLinSp; iSpar++){
 	    		   	   if(veclmILU[iILU]==clm[iSpar]){
 	    		   		   vecRelILU[iILU]=iSpar;
 	    		   		   break;
 	    		   	   }
 	    		   }
 	    	   }

 	       }
 	}


 template<class T> std::vector<std::vector<int>> SparseMtx<T>::montaForwardDependencias(int N, int* VvecfnzILU, int* VveclmILU) {
     std::vector<std::vector<int>> dependencies(N);
     for (int i = 0; i < N; ++i) {
         for (int j = VvecfnzILU[i]; j < VvecfnzILU[i + 1]; ++j) {
             int col = VveclmILU[j];
             if (col < i) {
                 dependencies[i].push_back(col);
             }
         }
         std::sort(dependencies[i].begin(), dependencies[i].end());
     }
     return dependencies;
 }

 // Calcula cor (nível) da linha i com base nas dependências já resolvidas
template<class T> int SparseMtx<T>::cor_dependenciaForward(int i, const std::vector<std::vector<int>>& dependencias, const std::vector<int>& color) {
     int nivel = 0;
     for (int j : dependencias[i]) {
         nivel = std::max(nivel, color[j] + 1);
     }
     return nivel;
 }

template<class T>  std::vector<int> SparseMtx<T>::coloreForwardILU() {
 	std::vector<std::vector<int>> dependencias;
 	if(rank==0){
 		dependencias=montaForwardDependencias(this->nrows, fnz, clm);
 	}
 	else{
 		dependencias=montaForwardDependencias(this->nrows, vecfnzILU, veclmILU);
 	}
 	int N = this->nrows;
     std::vector<int> color(N, -1);
     for (int i = 0; i < N; ++i) {
         color[i] = cor_dependenciaForward(i, dependencias, color);
     }
     std::vector<int> colorMenor=color;
     std::sort(colorMenor.begin(), colorMenor.end());
     auto last = std::unique(colorMenor.begin(), colorMenor.end());
     colorMenor.erase(last, colorMenor.end());
     int tamanho=colorMenor.size();

     std::unordered_map<int, int> freq;

      for (int val : color) {
          freq[val]++;
      }

      //for (const auto& [valor, contagem] : freq) {
        //  std::cout << "O valor " << valor << " aparece " << contagem << " vezes.\n";
      //}

     int corLocal;
     for (int i = 0; i < tamanho; ++i) {
     	corLocal=colorMenor[i];
     }
     return color;
 }



 // Gera dependências superiores (col > i)
template<class T> std::vector<std::vector<int>> SparseMtx<T>::montaBackwardDependencias(int N, int* VvecfnzILU, int* VveclmILU) {
     std::vector<std::vector<int>> dependencias(N);
     for (int i = 0; i < N; ++i) {
         for (int j = VvecfnzILU[i]; j < VvecfnzILU[i + 1]; ++j) {
             int col = VveclmILU[j];
             if (col > i) {
                 dependencias[i].push_back(col);
             }
         }
         std::sort(dependencias[i].begin(), dependencias[i].end());
     }
     return dependencias;
 }

 // Calcula cor (nível) da linha i com base nas dependências já resolvidas
template<class T> int SparseMtx<T>::cor_dependenciaBackward(int i, const std::vector<std::vector<int>>& dependencias, const std::vector<int>& color) {
     int nivel = 0;
     for (int j : dependencias[i]) {
         nivel = std::max(nivel, color[j] + 1);
     }
     return nivel;
 }

template<class T> std::vector<int> SparseMtx<T>::coloreBackwardILU() {

 	std::vector<std::vector<int>> dependencias;
 	if(rank==0){
 		dependencias=montaBackwardDependencias(this->nrows, fnz, clm);
 	}
 	else{
 		dependencias=montaBackwardDependencias(this->nrows, vecfnzILU, veclmILU);
 	}

 	int N = this->nrows;
     std::vector<int> color(N, -1);
     for (int i = N - 1; i >= 0; --i) {
         color[i] = cor_dependenciaBackward(i, dependencias, color);
     }

     std::vector<int> colorMenor=color;
     std::sort(colorMenor.begin(), colorMenor.end());
     auto last = std::unique(colorMenor.begin(), colorMenor.end());
     colorMenor.erase(last, colorMenor.end());
     int tamanho=colorMenor.size();

     std::unordered_map<int, int> freq;

     for (int val : color) {
         freq[val]++;
     }

     //for (const auto& [valor, contagem] : freq) {
       //  std::cout << "O valor " << valor << " aparece " << contagem << " vezes.\n";
     //}


     int corLocal;
     for (int i = 0; i < tamanho; ++i) {
     	corLocal=colorMenor[i];
     }

     return color;
 }


template<class T> void SparseMtx<T>::geraILUrank(){
    	std::vector<int> level;
    	std::vector<int> colunaLev;
    	std::vector<int> linhaLev;
    	geraNivelFillinILU(level, linhaLev, colunaLev);
    	montaILUfillinCSR(level, linhaLev, colunaLev);
    	sraILU=new double[noZeroIlu];

}

template<class T> void SparseMtx<T>::geraILUcor(){
    	// Colorização para L (forward)
    	color_forward = coloreForwardILU();

    	// Colorização para U (backward)
    	color_backward = coloreBackwardILU();

}




template<class T> SparseMtx<T>::SparseMtx(int n, int m, T* et, int* cn, int* da, int vrank, int vcolorido, int vnoZeroIlu,
		int* VveclevILU, int* VvecRelILU, int* VveclmILU, int* VvecfnzILU,std::vector<int> vcolor_forward,
 		std::vector<int> vcolor_backward){//construtor
  this->nrows = n;
  lenth = m;
  sra = new T[lenth];
  clm = new int[lenth];
  fnz = new int[this->nrows+1];

  for(int i=0;i<lenth;i++){
    sra[i]=et[i];
    clm[i]=cn[i];
  }
  for(int i=0;i<=this->nrows;i++) fnz[i]=da[i];

  rank=vrank;
  colorido=vcolorido;
  noZeroIlu=vnoZeroIlu;
  if(noZeroIlu>0){
	    veclmILU = new int[noZeroIlu];
	    veclevILU = new int[noZeroIlu];
	    vecRelILU = new int[noZeroIlu];
	    for(int i=0;i<noZeroIlu;i++){
		    veclmILU[i] = VveclmILU[i];
		    veclevILU[i] = VveclevILU[i];
		    vecRelILU[i] = VvecRelILU[i];
	    }
	    vecfnzILU = new int[this->nrows + 1];
	    for(int i=0;i<this->nrows + 1;i++)vecfnzILU[i] = VvecfnzILU[i];
  }
  else{
	  veclevILU=0;
	  vecRelILU=0;
	  veclmILU=0;
	  vecfnzILU=0;
  }
  color_forward=vcolor_forward;
  color_backward=vcolor_backward;


  //if(rank>0)geraILUrank();
  //if(colorido>0)geraILUcor();
  if(noZeroIlu>0)sraILU = new T[noZeroIlu];
  else sraILU=0;


}

template<class T> SparseMtx<T>::SparseMtx(int n, int m){//construtor
  this->nrows = n;
  lenth = m;
  sra = new T[lenth];
  clm = new int[lenth];
  fnz = new int[this->nrows+1];

  for(int i=0;i<lenth;i++){
    sra[i]=0;
    clm[i]=0;
  }
  for(int i=0;i<=this->nrows;i++) fnz[i]=0;

  rank=0;
  colorido=0;
  noZeroIlu=0;
  veclevILU=0;
  vecRelILU=0;
  veclmILU=0;
  vecfnzILU=0;

  color_forward=std::vector<int>(1,0);
  color_backward=std::vector<int>(1,0);

  //if(rank>0)geraILUrank();
  //if(colorido>0)geraILUcor();
  if(noZeroIlu>0)sraILU = new T[noZeroIlu];
  else sraILU=0;

}

template<class T> SparseMtx<T>::SparseMtx(const SparseMtx& mat){//construtor de c�pia
  this->nrows=mat.nrows;
  lenth=mat.lenth;
  sra = new T[lenth];
  clm = new int[lenth];
  fnz = new int[this->nrows+1];

  for(int i=0;i<lenth;i++){
    sra[i]=mat[i];//utiliza o operador de subescrito
    clm[i]=mat.clm[i];
  }
  for(int i=0;i<=this->nrows;i++) fnz[i]=mat.fnz[i];

  rank=mat.rank;
  colorido=mat.colorido;
  noZeroIlu=mat.noZeroIlu;
  if(noZeroIlu>0){
	    veclmILU = new int[noZeroIlu];
	    veclevILU = new int[noZeroIlu];
	    vecRelILU = new int[noZeroIlu];
	    for(int i=0;i<noZeroIlu;i++){
		    veclmILU[i] = mat.veclmILU[i];
		    veclevILU[i] = mat.veclevILU[i];
		    vecRelILU[i] = mat.vecRelILU[i];
	    }
	    vecfnzILU = new int[this->nrows + 1];
	    for(int i=0;i<this->nrows + 1;i++)vecfnzILU[i] = mat.vecfnzILU[i];
  }
  else{
	  veclevILU=0;
	  vecRelILU=0;
	  veclmILU=0;
	  vecfnzILU=0;
  }

  color_forward=mat.color_forward;
  color_backward=mat.color_backward;

  //if(rank>0)geraILUrank();
  //if(colorido>0)geraILUcor();
  if(noZeroIlu>0)sraILU = new T[noZeroIlu];
  else sraILU=0;

}

template<class T> void SparseMtx<T>::qualPos(int& match, int& inf, int& sup,int linha, int coluna){
	inf=fnz[linha];
	sup=fnz[linha+1];
	match=-1;
	for(int kcol=inf; kcol<sup; kcol++){
		if(clm[kcol]==coluna)match=kcol;
		if(match>=0)break;
	}
}
template<class T> void SparseMtx<T>::qualPosILU(int& match, int& inf, int& sup,int linha, int coluna){
	inf=vecfnzILU[linha];
	sup=vecfnzILU[linha+1];
	match=-1;
	for(int kcol=inf; kcol<sup; kcol++){
		if(veclmILU[kcol]==coluna)match=kcol;
		if(match>=0)break;
	}
}

template<class T> Vcr<T> SparseMtx<T>::preconding(const Vcr<T>& r, int precn) {//const{
  if(precn==0){//sem pr�-condicionamento
    return r;
  }else if(precn==1){//pr�-condicionamento diagonal
     Vcr<T> diag(this->nrows); //termos diagonais
#pragma omp parallel for num_threads(nthrdMatriz)
     for(int i=0; i< this->nrows;i++){
       for(int j=fnz[i];j<fnz[i+1];j++)diag[i]+=sra[j]*sra[j];//c�lculo de um valor m�dio para a diagonal
       diag[i]=sqrt(diag[i]);
     }
     Vcr<T> z(this->nrows);
#pragma omp parallel for num_threads(nthrdMatriz)
     for(int i=0;i<this->nrows;i++)z[i]=r[i]/diag[i];
     return z;
  }else if(precn==2){//pr�-condicionador sim�trico SOR
     const T omega =.3;//par�metro do SOR
     Vcr<T> diag(this->nrows); //termos diagonais
#pragma omp parallel for num_threads(nthrdMatriz)
     for(int i=0;i<this->nrows;i++){//c�lculo do termo diagonal m�dio
       for(int j=fnz[i];j<fnz[i+1];j++){
         if(clm[j]==i)diag[i]+=sra[j]*sra[j];
         else diag[i]+=omega*omega*sra[j]*sra[j];
       }
       diag[i]=sqrt(diag[i]);
     }
     Vcr<T> z(this->nrows);
     for(int i=0;i<this->nrows;i++){
       T sum=0;
       for(int j=fnz[i];j<fnz[i+1];j++){
         if(clm[j]<i)sum+=sra[j]*z[clm[j]];
       }
       z[i]=(r[i]-omega*sum)/diag[i];
     }
     for(int i=this->nrows-1;i>=0;i--){
       T sum=0;
       for(int j=fnz[i];j<fnz[i+1];j++){
         if(clm[j]>i)sum+=sra[j]*z[clm[j]];
       }
       z[i]-=omega*sum/diag[i];
     }
     return z;
  }else if(precn==3){
	  int N = this->nrows;
	  	Vcr<T> sra_copy(lenth);
#pragma omp parallel for num_threads(nthrdMatriz)
	  	for (int i = 0; i < lenth; i++) sra_copy[i]=sra[i];
	      Vcr<T> z(N);
	  	z=r;

	  	// Etapa de fatoração ILU(0) em sra_copy
//#pragma omp parallel for num_threads(ntrhd)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  	    for (int i = 0; i < N; ++i) {
	  	        int row_start = fnz[i];
	  	        int row_end = fnz[i + 1];

	  	        // Loop nos elementos da linha i
	  	        for (int j = row_start; j < row_end; ++j) {
	  	            int col = clm[j];

	  	            if (col < i) {
	  	                // Calcula o termo L[i][col] e armazena em sra_copy
	  	                double sum = sra_copy[j];
	  	                for (int k = row_start; k < j; ++k) {
	  	                    int l_col = clm[k];
	  	                    int match_l, inf, sup;
	  	                    qualPos(match_l, inf, sup, l_col, col);
	  	                    if (match_l != -1) {
	  	                        sum -= sra_copy[k] * sra_copy[match_l];
	  	                    }
	  	                }

	  	                // Divide pelo termo diagonal U[col][col]
	  	                int diag_index;
	  	                qualPos(diag_index, row_start, row_end, col, col);
	  	              //(int& match, int& inf, int& sup,int linha, int coluna)
	  	                if (diag_index != -1) {
	  	                    sra_copy[j] = sum / sra_copy[diag_index];
	  	                } else {
	  	                    throw std::runtime_error("Elemento diagonal não encontrado na posição esperada.");
	  	                }
	  	            } else if (col == i) {
	  	                // Calcula o termo U[i][i] e armazena em sra_copy
	  	                double sum = sra_copy[j];
	  	                for (int k = row_start; k < j; ++k) {
	  	                    int u_col = clm[k];
	  	                    int match_u;
	  	                    qualPos(match_u, row_start, row_end, u_col, i);
	  	                    if (match_u != -1) {
	  	                        sum -= sra_copy[k] * sra_copy[match_u];
	  	                    }
	  	                }
	  	                sra_copy[j] = sum;
	  	            }
	  	        }
	  	    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  	/*for (int k = 0; k < N; k++) {
	  		for (int j = k; j < N; j++) {
	  			int infu;
	  			int supu;
	  			int match;
	  			qualPos(match, infu, supu, k, j);
	  			if(match>=0){
	  				double sum = sra[match];
	  				for (int m = 0; m < k; m++) {
		  				int infu2;
		  				int supu2;
		  				int match2;
		  				int match3;
		  				qualPos(match2, infu2, supu2, m, j);
		  				qualPos(match3, infu, supu, k, m);
		  				if(match3>=0 && match2>=0)
		  				sum -= sra_copy[match3] * sra_copy[match2];
	  				}
	  				sra_copy[match]=sum;
	  			}
	  		}
	  		for (int j = k+1; j < N; j++) {
	  			int infl;
	  			int supl;
	  			int match;
	  			qualPos(match, infl, supl, j, k);
	  			if(match>=0){
	  				double sum = sra[match];
	  				for (int m = 0; m < k; m++) {
		  				int infl2;
		  				int supl2;
		  				int match2;
		  				int match3;
		  				qualPos(match2, infl2, supl2, m, k);
		  				qualPos(match3, infl, supl, j, m);
		  				if(match3>=0 && match2>=0)
		  				sum-= sra_copy[match3] * sra_copy[match2];
	  				}
	  				int infldiag;
	  				int supldiag;
	  				int matchdiag;
	  				qualPos(matchdiag, infldiag, supldiag, k, k);
	  				sra_copy[match]=sum/sra_copy[matchdiag];
	  			}
	  		}
	  	}*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  	    // Forward substitution: L * z = r

	  	    for (int i = 0; i < N; ++i) {
	  	        double sum = z[i];
	  	        for (int j = fnz[i]; j < fnz[i + 1]; ++j) {
	  	            int col = clm[j];
	  	            if (col < i) {
	  	                sum -= sra_copy[j] * z[col];
	  	            }
	  	        }
	  	        int diag_index;
	  	        int lim1=fnz[i];
	  	        int lim2=fnz[i+1];
	  	        qualPos(diag_index, lim1, lim2, i, i);
	  	        z[i] = sum / sra_copy[diag_index];
	  	    }
		    /*Vcr<T> zI(N);
		  	zI=z;
for(int itera =0; itera<3;itera++){
		  	#pragma omp parallel for num_threads(nthrdMatriz)
		  	for (int i = 0; i < N; i++) {
	  	        double sum = r[i];
	  	        for (int j = fnz[i]; j < fnz[i + 1]; j++) {
	  	            int col = clm[j];
	  	            if (col < i) {
	  	                sum -= sra_copy[j] * zI[col];
	  	            }
	  	        }
	  	        int diag_index;
	  	        int lim1=fnz[i];
	  	        int lim2=fnz[i+1];
	  	        qualPos(diag_index, lim1, lim2, i, i);
	  	        z[i] = sum / sra_copy[diag_index];
	  	        //z[i] = sum ;
		  	}
		  	zI=z;
}*/

	  	    // Backward substitution: U * x = z
	  	    for (int i = N - 1; i >= 0; --i) {
	  	        double sum = z[i];
	  	        for (int j = fnz[i]; j < fnz[i + 1]; ++j) {
	  	            int col = clm[j];
	  	            if (col > i) {
	  	                sum -= sra_copy[j] * z[col];
	  	            }
	  	        }
	  	        int diag_index;
	  	        int lim1=fnz[i];
	  	        int lim2=fnz[i+1];
	  	        qualPos(diag_index, lim1, lim2, i, i);
	  	        z[i] = sum / sra_copy[diag_index];
	  	    }
/*Vcr<T> y(N);
	y=z;
for(int itera =0; itera<3;itera++){
#pragma omp parallel for num_threads(nthrdMatriz)
	    for (int i = N - 1; i >= 0; --i) {
	        double sum = y[i];
	        for (int j = fnz[i]; j < fnz[i + 1]; ++j) {
	            int col = clm[j];
	            if (col > i) {
	                sum -= sra_copy[j] * zI[col];
	            }
	        }
	        int diag_index;
	        int lim1=fnz[i];
	        int lim2=fnz[i+1];
	        qualPos(diag_index, lim1, lim2, i, i);
	        z[i] = sum / sra_copy[diag_index];
	    }
	    zI=z;
}*/


	  	    return z;  // Retorna o vetor resultante do pré-condicionamento
}else if(precn==4){

	if(rank==0){
		int N = this->nrows;
		Vcr<T> sra_copy(lenth);
		#pragma omp parallel for num_threads(nthrdMatriz)
		for (int i = 0; i < lenth; i++) sra_copy[i]=sra[i];
	    Vcr<T> z(N);
	  	z=r;

	  	// Etapa de fatoração ILU(0) em sra_copy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  	for (int i = 1; i < N; i++) {
	  	     int row_start = fnz[i];
	  	     int row_end = fnz[i + 1];

	  	     // Loop nos elementos da linha i
	  	     for (int k = row_start; k < row_end; k++) {
	  	         int col = clm[k];
	  	         if (col < i) {
	  	          int diag, inf, sup;
	  	          qualPos(diag, inf, sup, col, col);
	  	          sra_copy[k]=sra_copy[k]/sra_copy[diag];
	  	          for (int j = k+1; j < row_end; j++) {
	  	        	   int colj = clm[j];
		  	           int pos,posj, inf, sup;
		  	           qualPos(pos, inf, sup, i, col);
		  	           qualPos(posj, inf, sup, col, colj);
		  	           if(pos>=0 && posj>=0)
		  	        	   sra_copy[j]-=sra_copy[pos]*sra_copy[posj];
	  	          }
	  	        }
	  	    }
	  	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  	if(colorido==0){
  	    // Forward substitution: L * z = r

	  		for (int i = 0; i < N; ++i) {
	  			double sum = z[i];
	  			for (int j = fnz[i]; j < fnz[i + 1]; ++j) {
	  				int col = clm[j];
	  				if (col < i) {
	  					sum -= sra_copy[j] * z[col];
	  				}
	  			}
	  			int diag_index;
	  			int lim1=fnz[i];
	  			int lim2=fnz[i+1];
	  			qualPos(diag_index, lim1, lim2, i, i);
	  			//z[i] = sum / sra_copy[diag_index];
	  			z[i] = sum ;
	  		}
	  	// Backward substitution: U * x = z
	  		for (int i = N - 1; i >= 0; --i) {
	  			double sum = z[i];
	  			for (int j = fnz[i]; j < fnz[i + 1]; ++j) {
	  				int col = clm[j];
	  				if (col > i) {
	  					sum -= sra_copy[j] * z[col];
	  				}
	  			}
	  			int diag_index;
	  			int lim1=fnz[i];
	  			int lim2=fnz[i+1];
	  			qualPos(diag_index, lim1, lim2, i, i);
	  			z[i] = sum / sra_copy[diag_index];
	  		}
	  	}
	  	else{

	  		int max_color_forward = *std::max_element(color_forward.begin(), color_forward.end());

	  		int max_color_backward = *std::max_element(color_backward.begin(), color_backward.end());

	  		// Forward substitution (L * z = r)
	  		for (int c = 0; c <= max_color_forward; ++c) {
				#pragma omp parallel for num_threads(nthrdMatriz)
	  			for (int i = 0; i < N; ++i) {
	  				if (color_forward[i] == c) {
	  					double sum = z[i];
	  					for (int j = fnz[i]; j < fnz[i + 1]; ++j) {
	  						int col = clm[j];
	  						if (col < i) {
	  							sum -= sra_copy[j] * z[col];
	  						}
	  					}
	  					int diag_index;
	  					int lim1 = fnz[i];
	  					int lim2 = fnz[i + 1];
	  					qualPos(diag_index, lim1, lim2, i, i);
	  					z[i] = sum;  // Divisão pelo pivô é feita na backward
	  				}
	  			}
	  		}

	  		// Backward substitution: U * x = z
	  		for (int c = 0; c <= max_color_backward; ++c) {
	  	    	#pragma omp parallel for num_threads(nthrdMatriz)
	  			for (int i = 0; i < N; ++i) {
	  				if (color_backward[i] == c) {
	  					double sum = z[i];
	  					for (int j = fnz[i]; j < fnz[i + 1]; ++j) {
	  						int col = clm[j];
	  						if (col > i) {  // Parte superior!
	  							sum -= sra_copy[j] * z[col];
	  						}
	  					}
	  					int diag_index;
	  					int lim1 = fnz[i], lim2 = fnz[i + 1];
	  					qualPos(diag_index, lim1, lim2, i, i);
	  					z[i] = sum / sra_copy[diag_index];
	  				}
	  			}
	  		}
	  	}


	  	return z;  // Retorna o vetor resultante do pré-condicionamento
	}
	else{
		int N = this->nrows;
		Vcr<T> sra_copy(lenth);
		#pragma omp parallel for num_threads(nthrdMatriz)
		for (int i = 0; i < noZeroIlu; i++){
			if(vecRelILU[i]>=0)sraILU[i]=sra[vecRelILU[i]];
			else sraILU[i]=0;
		}
	    Vcr<T> z(N);
	  	z=r;

	  	// Etapa de fatoração ILU(P) em sra_copy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  	for (int i = 1; i < N; i++) {
	  	     int row_start = vecfnzILU[i];
	  	     int row_end = vecfnzILU[i + 1];

	  	     // Loop nos elementos da linha i
	  	     for (int k = row_start; k < row_end; k++) {
	  	         int col = veclmILU[k];
	  	         if (col < i) {
	  	          int diag, inf, sup;
	  	          qualPosILU(diag, inf, sup, col, col);
	  	          sraILU[k]=sraILU[k]/sraILU[diag];
	  	          for (int j = k+1; j < row_end; j++) {
	  	        	   int colj = veclmILU[j];
		  	           int pos,posj, inf, sup;
		  	           qualPosILU(pos, inf, sup, i, col);
		  	           qualPosILU(posj, inf, sup, col, colj);
		  	           if(pos>=0 && posj>=0)
		  	        	 sraILU[j]-=sraILU[pos]*sraILU[posj];
	  	          }
	  	        }
	  	    }
	  	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  	if(colorido==0){
  	    // Forward substitution: L * z = r

	  		for (int i = 0; i < N; ++i) {
	  			double sum = z[i];
	  			for (int j = vecfnzILU[i]; j < vecfnzILU[i + 1]; ++j) {
	  				int col = veclmILU[j];
	  				if (col < i) {
	  					sum -= sraILU[j] * z[col];
	  				}
	  			}
	  			int diag_index;
	  			int lim1=vecfnzILU[i];
	  			int lim2=vecfnzILU[i+1];
	  			qualPosILU(diag_index, lim1, lim2, i, i);
	  			//z[i] = sum / sra_copy[diag_index];
	  			z[i] = sum ;
	  		}
	  	// Backward substitution: U * x = z
	  		for (int i = N - 1; i >= 0; --i) {
	  			double sum = z[i];
	  			for (int j = vecfnzILU[i]; j < vecfnzILU[i + 1]; ++j) {
	  				int col = veclmILU[j];
	  				if (col > i) {
	  					sum -=sraILU[j] * z[col];
	  				}
	  			}
	  			int diag_index;
	  			int lim1=vecfnzILU[i];
	  			int lim2=vecfnzILU[i+1];
	  			qualPosILU(diag_index, lim1, lim2, i, i);
	  			z[i] = sum / sraILU[diag_index];
	  		}
	  	}
	  	else{

	  		int max_color_forward = *std::max_element(color_forward.begin(), color_forward.end());

	  		int max_color_backward = *std::max_element(color_backward.begin(), color_backward.end());

	  		// Forward substitution (L * z = r)
	  		for (int c = 0; c <= max_color_forward; ++c) {
				#pragma omp parallel for num_threads(nthrdMatriz)
	  			for (int i = 0; i < N; ++i) {
	  				if (color_forward[i] == c) {
	  					double sum = z[i];
	  					for (int j = vecfnzILU[i]; j < vecfnzILU[i + 1]; ++j) {
	  						int col = veclmILU[j];
	  						if (col < i) {
	  							sum -= sraILU[j] * z[col];
	  						}
	  					}
	  					int diag_index;
	  					int lim1 = vecfnzILU[i];
	  					int lim2 = vecfnzILU[i + 1];
	  					qualPosILU(diag_index, lim1, lim2, i, i);
	  					z[i] = sum;  // Divisão pelo pivô é feita na backward
	  				}
	  			}
	  		}

	  		// Backward substitution: U * x = z
	  		for (int c = 0; c <= max_color_backward; ++c) {
	  	    	#pragma omp parallel for num_threads(nthrdMatriz)
	  			for (int i = 0; i < N; ++i) {
	  				if (color_backward[i] == c) {
	  					double sum = z[i];
	  					for (int j = vecfnzILU[i]; j < vecfnzILU[i + 1]; ++j) {
	  						int col = veclmILU[j];
	  						if (col > i) {  // Parte superior!
	  							sum -= sraILU[j] * z[col];
	  						}
	  					}
	  					int diag_index;
	  					int lim1 = vecfnzILU[i], lim2 = vecfnzILU[i + 1];
	  					qualPosILU(diag_index, lim1, lim2, i, i);
	  					z[i] = sum / sraILU[diag_index];
	  				}
	  			}
	  		}
	  	}


	  	return z;  // Retorna o vetor resultante do pré-condicionamento
	}
}else{
     error("n�mero inv�lido para sele��o de precondicionador > 2");
     return Vcr<T>();
  }
}


template<class T> T SparseMtx<T>::cheia(int i, int j) const{
  T val = 0;
  for(int k = fnz[i];k<fnz[i+1];k++){if(clm[k]==j)val=sra[k];}
  return val;
}

template<class T> SparseMtx<T>& SparseMtx<T>::operator=(const SparseMtx& ssm){//sobrecarga do operado =
  //if(this->nrows!=ssm.nrows||lenth!=ssm.lenth)error("problema no tamanho da matriz, ocorrido na opera��o =");
  if(this->nrows!=ssm.nrows||lenth!=ssm.lenth){
		  //error("problema no tamanho da matriz, ocorrido na opera��o =");
		  if(lenth>0){
			  delete[] sra; delete[] fnz; delete[] clm;
		  }
	      if(noZeroIlu>0){
	      	 delete[] sraILU;
	      }
		  if(rank>0 && noZeroIlu>0){
			 delete[] veclevILU;
			 delete[] vecRelILU;
			 delete[] veclmILU;
			 delete[] vecfnzILU;
		  }
		  this->nrows = ssm.nrows;
		  lenth = ssm.lenth;
		  sra = new T[lenth];
		  clm = new int[lenth];
		  fnz = new int[this->nrows+1];
  }
  for(int i=0;i<lenth;i++){
    sra[i]=ssm[i];
    clm[i]=ssm.clm[i];
  }
  for(int i=0;i<=this->nrows;i++)fnz[i]=ssm.fnz[i];

  rank=ssm.rank;
  colorido=ssm.colorido;
  noZeroIlu=ssm.noZeroIlu;
  if(noZeroIlu>0){
	    veclmILU = new int[noZeroIlu];
	    veclevILU = new int[noZeroIlu];
	    vecRelILU = new int[noZeroIlu];
	    for(int i=0;i<noZeroIlu;i++){
		    veclmILU[i] = ssm.veclmILU[i];
		    veclevILU[i] = ssm.veclevILU[i];
		    vecRelILU[i] = ssm.vecRelILU[i];
	    }
	    vecfnzILU = new int[this->nrows + 1];
	    for(int i=0;i<this->nrows + 1;i++)vecfnzILU[i] = ssm.vecfnzILU[i];
  }
  else{
	  veclevILU=0;
	  vecRelILU=0;
	  veclmILU=0;
	  vecfnzILU=0;
  }

  color_forward=ssm.color_forward;
  color_backward=ssm.color_backward;

  //if(rank>0)geraILUrank();
  //if(colorido>0)geraILUcor();
  if(noZeroIlu>0)sraILU = new T[noZeroIlu];
  else sraILU=0;


  return *this;
}

template<class T> SparseMtx<T> SparseMtx<T>::operator *(T a) const{//multiplica��o de matriz por escalar mat*a
  SparseMtx<T> tm(this->nrows, lenth);
  for(int i=0;i<lenth;i++)
    tm[i]=sra[i]*a;
  return tm;
}

template<class T> Vcr<T> SparseMtx<T>::operator*(const Vcr<T>& vec)const{//multiplica��o entre matriz e vetor
  if(this->nrows != vec.size())error("tamanho de matriz e de vetor n�o conferem, problema ocorrido na opera��o de multiplica��o matrizXvetor");
  Vcr<T> tm(this->nrows);
#pragma omp parallel for num_threads(nthrdMatriz)
  for(int i=0;i<this->nrows;i++)
    for(int j=fnz[i];j<fnz[i+1];j++)
      tm[i]+=sra[j]*vec[clm[j]];
  return tm;
}

template<class T> SparseMtx<T>& SparseMtx<T>::operator +=(const SparseMtx& ssm){//sobrecarga de operador +=
  for(int i=0;i<lenth;i++)
    sra[i]+=ssm[i];
  return *this;
}

template<class T> SparseMtx<T>& SparseMtx<T>::operator -=(const SparseMtx& ssm){//sobrecarga de operador -=
  for(int i=0;i<lenth;i++)
    sra[i]-=ssm[i];
  return *this;
}

template<class T> SparseMtx<T> SparseMtx<T>::operator -()const{//sobrecarga de operador un�rio - -> m2=-m1
  SparseMtx<T> zero(this->nrows, lenth);
  return zero-*this;
} 

template<class T> SparseMtx<T> SparseMtx<T>::operator +(const SparseMtx& ssm) const{//sobrecarga de operador +
  if(this->nrows!=ssm.nrows)error("matrizes t�m tamanhos diferentes, erro ocorrido na soma de matrizes esparsas");

  int templ=lenth+ssm.lenth; //tamanho de vetor tempor�rio para armazenar as entradas para a matriz de soma
  T* tempm;//�rea de armazenamento tempor�rio para os valores n�o-zero da matriz
  int* tempcol;//�rea de armazenamento tempor�rio para o �ndice da coluna de cada entrada n�o zero da matriz
  int* tempini;//�rea de armazenamento tempor�rio para o �ndice da primeira entrada n�o zero em uma linha
  tempm = new T[templ];
  tempcol = new int[templ];
  tempini = new int[this->nrows+1];

  int ltemp=0;//contador do n�mero de entradas n�o-zero
  int l1=0;//contador para rastrear em que posi��o de sra se encontra a primeira matriz na soma
  int l2=0;//contador para rastrear em que posi��o de sra se encontra a segunda matriz na soma
  for(int i=0;i<this->nrows;i++){//la�o linha a linha
   int col1=clm[fnz[i]];//primeira coluna com termo n�o-zero na primeira matriz para a linha i
   int col2=ssm.clm[ssm.fnz[i]];//primeira coluna com termo n�o-zero na segunda matriz para a linha i
   int fim1=fnz[i+1]-1;//�ltima coluna com termo n�o-zero na primeira matriz para a linha i
   int fim2=ssm.fnz[i+1]-1;//�ltima coluna com termo n�o-zero na segunda matriz para a linha i
   tempini[i]=ltemp;//armazenando o �ndice do primeiro termo sra da nova matriz para a linha i
   while(l1<=fim1||l2<=fim2){//o la�o s� termina quando todos os termos n�o-zeros da duas matrizes na linha que ser�o somados s�o rastreados
     if(col1<col2||(l1<=fim1&&l2>fim2)){//a primeira matriz tem um termo n�o zero antes de algum termo n�o zero da segunda
       tempm[ltemp]=sra[l1]; tempcol[ltemp]=col1;//a matriz soma receber� os valores da primeira matriz
       l1++; ltemp++; col1=clm[l1];//s� os �ndices da primeira matriz s�o acrescidos de 1
     }
     else if(col2<col1||(l2<=fim2&&l1>fim1)){//a segunda matriz tem um termo n�o zero antes de algum termo n�o zero da segunda
       tempm[ltemp]=ssm.sra[l2]; tempcol[ltemp]=col2;//a matriz soma receber� os valores da segunda matriz
       l2++; ltemp++; col2=ssm.clm[l2];//s� os �ndices da segundaa matriz s�o acrescidos de 1
     }
     else{//os termo n�o zero das duas matrizes coincidem na mesma coluna
       tempm[ltemp]=sra[l1]+ssm.sra[l2]; tempcol[ltemp]=col1;//a matriz soma receber� a soma dos valores da primeira  e segunda matriz
       l1++; l2++; ltemp++; col1=clm[l1]; col2=ssm.clm[l2];//os �ndices da primeira e da segunda matriz ser�o acrescidos de 1
     }
   }
  }
  tempini[this->nrows]=ltemp;

  SparseMtx<T> sum(this->nrows,ltemp,tempm,tempcol,tempini);//constr�i nova matriz esparsa a partir dos vetores obtidos no la�o anterior
  delete[] tempm; delete[] tempcol; delete[] tempini;
  return sum;
}

template<class T> SparseMtx<T> SparseMtx<T>::operator -(const SparseMtx& ssm) const{//sobrecarga de operador +
  if(this->nrows!=ssm.nrows)error("matrizes t�m tamanhos diferentes, erro ocorrido na soma de matrizes esparsas");

  int templ=lenth+ssm.lenth; //tamanho de vetor tempor�rio para armazenar as entradas para a matriz de soma
  T* tempm;//�rea de armazenamento tempor�rio para os valores n�o-zero da matriz
  int* tempcol;//�rea de armazenamento tempor�rio para o �ndice da coluna de cada entrada n�o zero da matriz
  int* tempini;//�rea de armazenamento tempor�rio para o �ndice da primeira entrada n�o zero em uma linha
  tempm = new T[templ];
  tempcol = new int[templ];
  tempini = new int[this->nrows+1];

  int ltemp=0;//contador do n�mero de entradas n�o-zero
  int l1=0;//contador para rastrear em que posi��o de sra se encontra a primeira matriz na soma
  int l2=0;//contador para rastrear em que posi��o de sra se encontra a segunda matriz na soma
  for(int i=0;i<this->nrows;i++){//la�o linha a linha
   int col1=clm[fnz[i]];//primeira coluna com termo n�o-zero na primeira matriz para a linha i
   int col2=ssm.clm[ssm.fnz[i]];//primeira coluna com termo n�o-zero na segunda matriz para a linha i
   int fim1=fnz[i+1]-1;//�ltima coluna com termo n�o-zero na primeira matriz para a linha i
   int fim2=ssm.fnz[i+1]-1;//�ltima coluna com termo n�o-zero na segunda matriz para a linha i
   tempini[i]=ltemp;//armazenando o �ndice do primeiro termo sra da nova matriz para a linha i
   while(l1<=fim1||l2<=fim2){//o la�o s� termina quando todos os termos n�o-zeros da duas matrizes na linha que ser�o somados s�o rastreados
     if(col1<col2||(l1<=fim1&&l2>fim2)){//a primeira matriz tem um termo n�o zero antes de algum termo n�o zero da segunda
       tempm[ltemp]=sra[l1]; tempcol[ltemp]=col1;//a matriz soma receber� os valores da primeira matriz
       l1++; ltemp++; col1=clm[l1];//s� os �ndices da primeira matriz s�o acrescidos de 1
     }
     else if(col2<col1||(l2<=fim2&&l1>fim1)){//a segunda matriz tem um termo n�o zero antes de algum termo n�o zero da segunda
       tempm[ltemp]=-ssm.sra[l2]; tempcol[ltemp]=col2;//a matriz soma receber� os valores da segunda matriz
       l2++; ltemp++; col2=ssm.clm[l2];//s� os �ndices da segundaa matriz s�o acrescidos de 1
     }
     else{//os termo n�o zero das duas matrizes coincidem na mesma coluna
       tempm[ltemp]=sra[l1]-ssm.sra[l2]; tempcol[ltemp]=col1;//a matriz soma receber� a soma dos valores da primeira  e segunda matriz
       l1++; l2++; ltemp++; col1=clm[l1]; col2=ssm.clm[l2];//os �ndices da primeira e da segunda matriz ser�o acrescidos de 1
     }
   }
  }
  tempini[this->nrows]=ltemp;

  SparseMtx<T> subtr(this->nrows,ltemp,tempm,tempcol,tempini);//constr�i nova matriz esparsa a partir dos vetores obtidos no la�o anterior
  delete[] tempm; delete[] tempcol; delete[] tempini;
  return subtr;
} 


//Fun��es para Matriz Abstrata:
template<class T> int  SparseMtx<T>::CG(Vcr<T>& x, const Vcr<T>& b, T& eps, int& iter, int pcn){//gradiente conjugado
//Gradiente conjugado  com pr�-condicionador para um sistema do tipo Ax=b
//retorna 0 se teve sucesso e 1 se ocorreu algum problema
//A: Matriz Hermitiana positiova definida
//x: na entrada � o chute inicial, ap�s passar pela fun��o torna-se a solu��o aproximada
//b: vetor livre do lado direito
//eps: na entrada -> crit�rio para fim de itera��o, na sa�da res�duo da solu��o aproximada
//iter: na entrada-> m�paximo n�mero de itera��es; na sa�da-> n�mero de itera��as feitas durante a execu��o da fun��o
//pcn-> �ndice para escolah do pr�-condicionador
  if(this->nrows!=b.size())error("tamanho de matriz e vetor incompat�veis, erro ocorrido no gradiente conjugado");
  const int maxiter=iter;
  Vcr<T> r=b-(*this)*x;//erro inicial
  Vcr<T> z=preconding(r,pcn); //solve precondit system
  Vcr<T> p=z;//p:dire��o do gradiente conjugado
  T zr=dot(z,r);//produto interno
  const double stp=eps*b.twonorm();//crit�rio para parada da itera��o

  if(!r.maxnorm()){//se o chute inicial � a solu��o
    eps=0.0;
    iter=0;
    return 0;
  }

  for(iter =0; iter<maxiter;iter++){
    Vcr<T> mp=(*this)*p;
    T pap=dot(mp,p);
    if (fabs(pap) < 1e-15) {
        break;  // Evita divisão por zero
    }
    T alpha=zr/pap;
    x+=alpha*p;
    r-=alpha*mp;
    if(r.twonorm()<=stp)break;//para la�o se convergiu
    z=preconding(r,pcn);//pr�-condicionamento
    T zrold=zr;
    zr=dot(z,r);
    T beta=zr/zrold;
    p=z+beta*p;
  }

  eps=r.twonorm();
  if(iter==maxiter) return 1;
  else return 0;
}

template<class T> int  SparseMtx<T>::BiCGStab(Vcr<T>& x, const Vcr<T>& b, T& eps, int& iter, int pcn){
//BiCGStab  com pr�-condicionador para um sistema do tipo Ax=b
//retorna 0 se teve sucesso e 1 se ocorreu algum problema
//A: Matriz Hermitiana positiova definida
//x: na entrada � o chute inicial, ap�s passar pela fun��o torna-se a solu��o aproximada
//b: vetor livre do lado direito
//eps: na entrada -> crit�rio para fim de itera��o, na sa�da res�duo da solu��o aproximada
//iter: na entrada-> m�paximo n�mero de itera��es; na sa�da-> n�mero de itera��as feitas durante a execu��o da fun��o
//pcn-> �ndice para escolah do pr�-condicionador
  if(this->nrows!=b.size())error("tamanho de matriz e vetor incompat�veis, erro ocorrido no gradiente conjugado");
  const int maxiter=iter;
  Vcr<T> r=b-(*this)*x;//erro inicial
  Vcr<T> rstar=r;//residuo star
  T rho0=1.;
  T alpha=1.;
  T w=1;
  Vcr<T> v=Vcr<T>(this->nrows,0.);
  Vcr<T> p=Vcr<T>(this->nrows,0.);;
  Vcr<T> y=Vcr<T>(this->nrows,0.);;
  Vcr<T> s=Vcr<T>(this->nrows,0.);;
  Vcr<T> z=Vcr<T>(this->nrows,0.);;
  Vcr<T> t=Vcr<T>(this->nrows,0.);;
  Vcr<T> tpcn=Vcr<T>(this->nrows,0.);;
  Vcr<T> spcn=Vcr<T>(this->nrows,0.);;

  const double stp=eps*b.twonorm();//crit�rio para parada da itera��o

  if(!r.maxnorm()){//se o chute inicial e a solucao
    eps=0.0;
    iter=0;
    return 0;
  }

  for(iter =0; iter<maxiter;iter++){

	T rho1=dot(rstar,r);
	T beta=(rho1/rho0)*(alpha/w);
	rho0=rho1;
	p=r+beta*(p-w*v);
	y=preconding(p,pcn);//pre-condicionamento
	v=(*this)*y;
	alpha=rho1/dot(rstar,v);
	s=r-alpha*v;
	z=preconding(s,pcn);//pre-condicionamento
	t=(*this)*z;
	tpcn=preconding(t,pcn);//pre-condicionamento
	T tat=dot(tpcn,tpcn);
    if (fabs(tat) < 1e-15) {
        break;  // Evita divisão por zero
    }
	w=dot(tpcn,z)/tat;
	x+=alpha*y+w*z;
	r=s-w*t;//novo residuo

    if(r.twonorm()<=stp)break;//para la�o se convergiu
  }

  eps=r.twonorm();
  if(iter==maxiter) return 1;
  else return 0;
}


template<class T> int SparseMtx<T>::GMRES(Vcr<T>& x, const Vcr<T>& b, T& eps, int& iter, int pcn, int m){ //GMRES
//GMRES com opera��o de "restart", utiliza pr�-condicionador para um sistema do tipo Ax=b
//retorna 0 se teve sucesso e 1 se ocorreu algum problema
//A: Matriz quaquer
//x: na entrada � o chute inicial, ap�s passar pela fun��o torna-se a solu��o aproximada
//b: vetor livre do lado direito
//eps: na entrada -> crit�rio para fim de itera��o, na sa�da res�duo da solu��o aproximada
//iter: na entrada-> m�paximo n�mero de itera��es; na sa�da-> n�mero de itera��as feitas durante a execu��o da fun��o
//pcn-> �ndice para escolah do pr�-condicionador
//m -> n�mero de itera��es ap�s as quais o GMRES � reiniciado
  const int maxiter=iter;
  const T stp=(preconding(b,pcn)).twonorm()*eps;
  Vcr<T> r=preconding(b-(*this)*x,pcn);
  T beta=r.twonorm();
  bool conv=false;
  if(m>this->nrows) error("n�mero de itera��es para restart � maior do que o n�mero de linhas");
  if(fabs(beta)<=stp){//finaliza o calculo se o chute inicial for a solu��o
    eps=fabs(beta);
    iter=0;
    return 0;
  }

  //obten��o de uma base ortonormal para o espa�o de Krilov
  Vcr<T>** v=new Vcr<T>* [m+1];
  for(int i=0;i<=m;i++) v[i]=new Vcr<T>(this->nrows);//i-�sima base

  //Matriz de Hessenberg->h[m+1][m]
  //s� a parte diferente de zero de h � armazenada
  T** h=new T* [m];
  for(int i=0;i<m;i++) h[i]=new T [i+2];

  iter=1;
  while(iter<=maxiter){//itera��es para gmres(m->par�metro de "restart")
    *v[0]=r/beta;
    Vcr<T> g(m+1);
    g[0]=beta;

    Vcr<T> cs(m),sn(m);//rota��o de Givens
    int k;
    for(k=0;k<m && iter<=maxiter;k++,iter++){
       //cout<<iter;
       //cout<<"\n";
      //ortogonaliza��o:
      Vcr<T> w=preconding((*this)*(*v[k]),pcn);
      T nmw=w.twonorm();
      for(int i=0;i<=k;i++){
        h[k][i]=dot(w,*v[i]);
        w-=h[k][i]*(*v[i]);
      }
      h[k][k+1]=w.twonorm();
      //verifica o ortogonaliza��o: se h[k][k+1] � pequeno, faz-se uma reortogonaliza��o
      if(nmw+1.0e-4*h[k][k+1]==nmw){
        for(int i=0;i<=k;i++){
          T hri = dot(w,*v[i]);
          h[k][i]+=hri;
          w-=hri*(*v[i]);
        }
        h[k][k+1]=w.twonorm();
      }
      if(h[k][k+1]==0) error("divis�o por zero no GMRES");
      *v[k+1]=w/h[k][k+1];

      //aplicando a rota��o de Givens para a coluna k de h
      for(int i=0;i<k;i++){
        T tv[2]={cs[i],sn[i]};
        T tmp=dot(&h[k][i],tv,2);
        h[k][i+1]= -sn[i]*h[k][i]+cs[i]*h[k][i+1];
        h[k][i]=tmp;
      }

      //gerando os termos para a rora��o de Givens, cs e sn
      if(h[k][k+1]==0){
        cs[k]=1;
        sn[k]=0;
      }else{
        T tpm=sqrt(dot(&h[k][k],&h[k][k],2));
        cs[k]=h[k][k]/tpm;
        sn[k]=h[k][k+1]/tpm;
      }

      //aplicando a rota��o de Givens para a coluna k de h e g
      T tv[2]={cs[k],sn[k]};
      h[k][k]=dot(&h[k][k],tv,2);
      T tmp=dot(&g[k],tv,2);
      g[k+1]=-sn[k]*g[k]+cs[k]*g[k+1];
      g[k]=tmp;
      if(fabs(g[k+1])<=stp){//para quando o res�duo for pequeno
        k++;
        break;
      }
    }

    //resolve o sistema triangular superior
//#pragma omp parallel for num_threads(nthrdMatriz)
    for(int i=k-1;i>=0;i--){
      for(int j=i+1;j<k;j++)g[i]-=h[j][i]*g[j];
      g[i]/=h[i][i];
    }

    //recalcula x
    for(int i=0;i<k;i++)x+=(*v[i])*g[i];

    //calcula o res�duo e checa a converg�ncia
    r=preconding(b-(*this)*x,pcn);
    beta=r.twonorm();
    if(fabs(beta)<=stp){
      conv=true;
      break;
    }
  }

  eps=(b-(*this)*x).twonorm();//res�duo final

  for(int i=0;i<=m;i++)delete v[i];
  delete[] v;
  for(int i=0;i<m;i++)delete[] h[i];
  delete[] h;

  if(conv) return 0;
  else return 1;
}

template<class T> int SparseMtx<T>::FGMRES(Vcr<T>& x, const Vcr<T>& b, T& eps, int& iter, int pcn, int m){ //GMRES
//FGMRES com opera��o de "restart", utiliza pr�-condicionador para um sistema do tipo Ax=b
//retorna 0 se teve sucesso e 1 se ocorreu algum problema
//A: Matriz quaquer
//x: na entrada � o chute inicial, ap�s passar pela fun��o torna-se a solu��o aproximada
//b: vetor livre do lado direito
//eps: na entrada -> crit�rio para fim de itera��o, na sa�da res�duo da solu��o aproximada
//iter: na entrada-> m�paximo n�mero de itera��es; na sa�da-> n�mero de itera��as feitas durante a execu��o da fun��o
//pcn-> �ndice para escolah do pr�-condicionador
//m -> n�mero de itera��es ap�s as quais o GMRES � reiniciado
  const int maxiter=iter;
  const T stp=(preconding(b,pcn)).twonorm()*eps;
  //Vcr<T> r=preconding(b-(*this)*x,pcn);
  Vcr<T> r=b-(*this)*x;
  T beta=r.twonorm();
  bool conv=false;
  if(m>this->nrows) error("n�mero de itera��es para restart � maior do que o n�mero de linhas");
  if(fabs(beta)<=stp){//finaliza o calculo se o chute inicial for a solu��o
    eps=fabs(beta);
    iter=0;
    return 0;
  }

  T beta0 = beta;
  T razReducao =0.;
  int pcnEfetivo=pcn;

  //obten��o de uma base ortonormal para o espa�o de Krilov
  Vcr<T>** v=new Vcr<T>* [m+1];
  Vcr<T>** z=new Vcr<T>* [m+1];
  for(int i=0;i<=m;i++) v[i]=new Vcr<T>(this->nrows);//i-�sima base
  for(int i=0;i<=m;i++) z[i]=new Vcr<T>(this->nrows);//i-�sima base

  //Matriz de Hessenberg->h[m+1][m]
  //s� a parte diferente de zero de h � armazenada
  T** h=new T* [m];
  for(int i=0;i<m;i++) h[i]=new T [i+2];

  iter=1;
  while(iter<=maxiter){//itera��es para gmres(m->par�metro de "restart")
    *v[0]=r/beta;
    Vcr<T> g(m+1);
    g[0]=beta;

    Vcr<T> cs(m),sn(m);//rota��o de Givens
    int k;
    for(k=0;k<m && iter<=maxiter;k++,iter++){
       //cout<<iter;
       //cout<<"\n";
      //ortogonaliza��o:
      //if(iter<10)*z[k]=preconding((*v[k]),1);
      //else *z[k]=preconding((*v[k]),pcn);

      if (razReducao > 0.9) { // Estagnação: redução < 10%
    	  pcnEfetivo = pcn; // Usa pré-condicionador robusto
      } else {
    	  pcnEfetivo = 1; // Continua com diagonal
      }
      *z[k] = preconding(*v[k], pcnEfetivo);

      Vcr<T> w=(*this)*(*z[k]);
      T nmw=w.twonorm();
      for(int i=0;i<=k;i++){
        h[k][i]=dot(w,*v[i]);
        w-=h[k][i]*(*v[i]);
      }
      h[k][k+1]=w.twonorm();
      //verifica o ortogonaliza��o: se h[k][k+1] � pequeno, faz-se uma reortogonaliza��o
      if(nmw+1.0e-4*h[k][k+1]==nmw){
        for(int i=0;i<=k;i++){
          T hri = dot(w,*v[i]);
          h[k][i]+=hri;
          w-=hri*(*v[i]);
        }
        h[k][k+1]=w.twonorm();
      }
      if(h[k][k+1]==0) error("divis�o por zero no GMRES");
      *v[k+1]=w/h[k][k+1];

      //aplicando a rota��o de Givens para a coluna k de h
      for(int i=0;i<k;i++){
        T tv[2]={cs[i],sn[i]};
        T tmp=dot(&h[k][i],tv,2);
        h[k][i+1]= -sn[i]*h[k][i]+cs[i]*h[k][i+1];
        h[k][i]=tmp;
      }

      //gerando os termos para a rora��o de Givens, cs e sn
      if(h[k][k+1]==0){
        cs[k]=1;
        sn[k]=0;
      }else{
        T tpm=sqrt(dot(&h[k][k],&h[k][k],2));
        cs[k]=h[k][k]/tpm;
        sn[k]=h[k][k+1]/tpm;
      }

      //aplicando a rota��o de Givens para a coluna k de h e g
      T tv[2]={cs[k],sn[k]};
      h[k][k]=dot(&h[k][k],tv,2);
      T tmp=dot(&g[k],tv,2);
      g[k+1]=-sn[k]*g[k]+cs[k]*g[k+1];
      g[k]=tmp;
      if(fabs(g[k+1])<=stp){//para quando o res�duo for pequeno
        k++;
        break;
      }
    }

    //resolve o sistema triangular superior
//#pragma omp parallel for num_threads(nthrdMatriz)
    for(int i=k-1;i>=0;i--){
      for(int j=i+1;j<k;j++)g[i]-=h[j][i]*g[j];
      g[i]/=h[i][i];
    }

    //recalcula x
    for(int i=0;i<k;i++)x+=(*z[i])*g[i];

    //calcula o res�duo e checa a converg�ncia
    r=b-(*this)*x;
    beta=r.twonorm();
    // Dentro do loop, após calcular beta = r.twonorm()
    razReducao = fabsl(beta / beta0);
    beta0 = beta;
    if(fabs(beta)<=stp){
      conv=true;
      break;
    }
  }

  eps=(b-(*this)*x).twonorm();//res�duo final

  for(int i=0;i<=m;i++)delete v[i];
  delete[] v;
  for(int i=0;i<=m;i++)delete z[i];
  delete[] z;
  for(int i=0;i<m;i++)delete[] h[i];
  delete[] h;

  if(conv) return 0;
  else return 1;
}

template<class S> ostream& operator<<(ostream& s, const SparseMtx<S>& mat){//saida de valores de Matriz Banda
  int nl=mat.lin();
  for ( int i = 0; i<nl; i++){
    for(int j=0;j<nl;j++){
      s.width(9);
      s.precision(7);
      s<<mat.cheia(i,j)<<"  ";
    }
    s<<"\n";
  }
  s<<"\n";
  return s;
}

template class FullMtx<double>;
template class BandMtx<double>;
template class SparseMtx<double>;

template ostream& operator<<(ostream&, const FullMtx<double>&);
