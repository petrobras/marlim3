#ifndef _MATRIZ
#define _MATRIZ

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <complex>
#include <vector>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <utility> // para std::pair
using namespace std;
#include "Vetor.h"
#include "Log.h"
#include <omp.h>

// criar objeto para log da aplicacao
extern Logger logger;

extern int nthrdMatriz;
//extern int ntrd;

//Classe de matriz:

// Usamos um hash para armazenar os níveis de forma eficiente
using LevelMap = std::unordered_map<long long, int>; // Mapeia (i, j) -> nível


template<class T> class AbsMtx{ //classe abstrata para matriz, algumas fun��es virtuais ser�o colocadas nesta classe,
	//esta servir� de interface para outras classes de matriz, cheia, esparsa e banda.
	protected://as vari�veis declaradas aqui n�o ser�o privadas para classes derivadas
		int nrows; //n�mero de linhas
		virtual Vcr<T> preconding(const Vcr<T>& r, int i=0) =0;//const=0;
		//Resolve Pz=r, P-> matriz precondicionadora (acelera a converg�ncia do GMRES e do GC)
		//r -> vetor res�duo de um sistema do tipo Ax-vl=0, z � retornado como solu��o
		// do sitema, um Vcr
		//i � um �ndice que indica que tipo de precondicionamento ser� utilizado, 
		//i=0-> sem precondicionamento, i=1 -> precondicionador diagonal,
		//i=2-> precondicionador SSOR
	public:
		virtual Vcr<T> operator*(const Vcr<T> &) const=0;//sobrecarga de operador * para multiplica��o MatrizXVetor
		int CG(Vcr<T>&, const Vcr<T>&, T&, int&, int);//gradiente conjugado
		int GMRES(Vcr<T>&, const Vcr<T>&, T&, int&, int, int); //GMRES
};



template<class T> void error(const T& t);//aviso de erro em matriz



template<class T> class FullMtx: public AbsMtx<T>{//classe para matriz cheia, herda a interface de AbsMtx
	private:
		int ncols; //n�mero de colunas na matriz, n�mero de linhas j� est� foi herdada de AbsMtx
		//T** mx; //ponteiro da �rea armazenada para o preenchimento da matriz
		Vcr<T> preconding(const Vcr<T>& r, int i=0) ;//const;//o precondicionador herdado para o caso de matriz cheia
	public:
		T** mx; //ponteiro da �rea armazenada para o preenchimento da matriz
		FullMtx(int, int, const T** const); //construtor a partir de uma �rea j� armazenada, n->n de linhas, m-> n de colunas
		FullMtx(int n=15, int m=15, T t=0);//construtor com valores da matriz igual a um escalar t que tem como default zero
		FullMtx(const FullMtx&);//construtor de c�pia
		~FullMtx(){//destrutor
			for(int i=0; i<this->nrows;i++) delete[] mx[i];
			delete [] mx;
		}
		
                int col() const {return ncols;}
		int lin() const {return this->nrows;}
                T cheia(int i, int j) const {return mx[i][j];}//trata a matriz com os �ndices normais de matriz cheia

		FullMtx& operator =(const FullMtx&);//sobrecarga de operador =
		FullMtx& operator +=(const FullMtx&);//sobrecarga de operador +=
		FullMtx& operator -=(const FullMtx&);//sobrecarga de operador -=
		FullMtx  operator -() const; //sobrecarga de operador un�rio - -> m2=-m1
		FullMtx  operator +(const FullMtx&) const;//sobrecarga de operador + -> m2+m1
		FullMtx  operator -(const FullMtx&) const;//sobrecarga de operador - -> m2-m1
		T* operator [](int i) const {return mx[i];} //operador de �ndice
                FullMtx operator *(T) const;//multiplica��o de matriz por vetor mat*a
		Vcr<T> operator *(const Vcr<T>&) const;//multiplica��o de matriz por vetor
		FullMtx operator *(const FullMtx&) const;//multiplica��o de matriz por matriz
                void GaussElim(Vcr<T>&) const; //elimina��o gaussiana sem pivoteamento
		void GaussElimPP(Vcr<T>&) const; //elimina��o gaussiana com pivoteamento parcial
		void GaussElimCP(Vcr<T>&) const; //elimina��o gaussiana com pivoteamento completo
		void LU(Vcr<T>&) const; //fatoracao LU
        void Inverte1(FullMtx&) const; //invers�o de matriz sem pivoteamento
        T InterpolaUnif(T lin, T col) const;//interpolação com espaçamentos uniformes
};

template<class S> ostream& operator<<(ostream& s, const FullMtx<S>&);//saida de valores de Matriz

//matriz banda
template<class T> class BandMtx: public AbsMtx<T>{//classe para matriz banda, herda a interface de AbsMtx
        private:
          int bwlef;//range de termos n�o-zeros � esquerda da diagonal principal
          int bwrit;//range de termos n�o-zeros � direita da diagonal principal
          T** bdmx;//ponteiro para �rea de armazenamento da matriz banda, s� os termos n�o-zero s�o armazenados
          Vcr<T> preconding(const Vcr<T>&, int=0) ;//const;//o precondicionador herdado para o caso de matriz banda
        public:
          BandMtx(int, int, int, const T** const);//construtor
          //n->n�mero de linhas=n�mero de colunas, p->largura da banda � esquerda da diag. principal
          //r->largura da banda � direita da diag. principal, t-> ponteiro da �rea onde se armazena os termos n�o zero
          BandMtx(int n=0, int p=0, int r=0, T t=0);//construtor
          BandMtx(int n, int p, int r, const FullMtx<T>& m);//construtor
          BandMtx(const BandMtx &);//cosntrutor c�pia
          ~BandMtx(){
                for(int i=0;i<this->nrows;i++)delete[](bdmx[i]-=bwlef);//no construtor no momento do aramzenamento � feito um "offset", os termos diagonais tem �ndice zero, por isto, � necess�rio o -=bwleft
                delete[] bdmx;
          }

          int lin()const {return this->nrows;}//retorna o n�mero de linhas
          int esq()const {return bwlef;}//retorna a largura da banda � esquerda da diagonal principal
          int dir()const {return bwrit;}//retorna a largura da banda � direita da diagonal principal
          T cheia(int, int) const;//trata a matriz com os �ndices normais de matriz cheia

          BandMtx& operator=(const BandMtx&);
          T* operator[](int i)const{return bdmx[i];}
          BandMtx operator *(T) const;//multiplica��o de matriz por escalar mat*a
          Vcr<T> operator *(const Vcr<T>&) const; //multiplica��o de matriz por vetor
          BandMtx& operator +=(const BandMtx&);//sobrecarga de operador +=
          BandMtx& operator -=(const BandMtx&);//sobrecarga de operador -=
	      BandMtx  operator -() const; //sobrecarga de operador un�rio - -> m2=-m1
          BandMtx operator +(const BandMtx&) const;//sobrecarga de operador +
          BandMtx operator -(const BandMtx&) const;//sobrecarga de operador -

          void GaussElim(Vcr<T>& )const;//elimina��o gaussiana sem pivoteamento
          void GaussElimPP(Vcr<T>& ) const;//elimina��o gaussiana com pivoteamento parcial

};

template<class S> ostream& operator<<(ostream& s, const BandMtx<S>&);//saida de valores de Matriz Banda


//matriz esparsa
template<class T> class SparseMtx: public AbsMtx<T>{//classe para matriz esparsa, herda a interface de AbsMtx
   private:
//o armazenamento � feito a partir de tr�s vetores, a matriz esparsa � comprimida no formato linha
    // int lenth; //n�mero de termos diferentes de zero a serem armazenados np primeiro vetor de aramzenamento
    // T* sra; //aponta para a �rea onde ser� armazenado os termos n�o zero
    // int* clm; //*clm armazena um outro vetor com a mesma dimens�o de *sra, nele se guardar� em que coluna da matriz o termo n�o zero se encontra
    // int* fnz;//*fnz armazena o �ndice em *sra da primeira entrada n�o-zero para uma determinada linha i. O seu tamanho � lenth+1
     Vcr<T> preconding(const Vcr<T>&, int i =0);//const;//precondicionador par GMRES e Gradiente conjugado
   public:
      int lenth; //n�mero de termos diferentes de zero a serem armazenados np primeiro vetor de aramzenamento
      T* sra; //aponta para a �rea onde ser� armazenado os termos n�o zero
      int* clm; //*clm armazena um outro vetor com a mesma dimens�o de *sra, nele se guardar� em que coluna da matriz o termo n�o zero se encontra
      int* fnz;//*fnz armazena o �ndice em *sra da primeira entrada n�o-zero para uma determinada linha i. O seu tamanho � lenth+1

	  int rank;
	  int colorido;
	  int noZeroIlu;
	  int* veclevILU;
	  int* vecRelILU;
	  int* veclmILU;
	  int* vecfnzILU;
	  T* sraILU;

	  std::vector<int> color_forward;
	  std::vector<int> color_backward;

	  SparseMtx(int n, int m, T* t, int* c, int* f, int vrank=0, int vcolorido=0, int vnoZeroIlu=0, int* VveclevILU=0, int* VvecRelILU=0,
	      		   int* VveclmILU=0, int* vecfnzILU=0,std::vector<int> vcolor_forward=std::vector<int>(1,0),
	  			   std::vector<int> vcolor_backward=std::vector<int>(1,0));//construtor
     //n: n�mero de linhas (e de colunas) da matriz
     //m: n�mero de termos n�o zero na matriz (tamanho de *sra)
     //t: ponteiro para as entradas n�o-zero, ser�o armazenadas em sra
     //c: ponteiro para o armnazenamento dos �ndices da coluna para cada entrada n�o-zero
     //f: ponteiro para o armazenamento dos �ndice em sra do primeiro termo n�o-zero em cada linha
     SparseMtx(int, int);//construtor com todas as entradas = zero
     SparseMtx(const SparseMtx&); //construtor c�pia
     ~SparseMtx(){
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
       }//destrutor

     int comp() const {return lenth;}//retorna o n�mero de n�o zeros da matriz
     int lin() const {return this->nrows;}//retorna o n�mero de linhas da matriz
     T cheia(int, int) const;//trata a matriz com os �ndices normais de matriz cheia
     int& getfnz(int i) const {return fnz[i];}//primeira entrada n�0-zero para uma certa linha
     int& getclm(int i) const {return clm[i];}//retorna em que coluna um certo termo n�o-zero (sra[i]) se encontra
     void qualPos(int& match, int& inf, int& sup,int linha, int coluna) ;
     void qualPosILU(int& match, int& inf, int& sup,int linha, int coluna) ;


	 inline long long key(int i, int j) {
		    return static_cast<long long>(i) * this->nrows + j;
	 }

	 void geraNivelFillinILU( std::vector<int>& level/*níveis dos termos*/,
			std::vector<int>& linhaLev/*linha de cada termo*/,
		    std::vector<int>& colunaLev/*coluna de cada termo*/);

	 void montaILUfillinCSR(const std::vector<int>& level/*níveis de cada termo (fill-in)*/,
		    const std::vector<int>& linhaLev/*linha correspondente a cada termo*/,
		    const std::vector<int>& colunaLev/*coluna correspondente a cada termo*/);

	 std::vector<std::vector<int>> montaForwardDependencias(int N, int* VvecfnzILU, int* VveclmILU);
	 int cor_dependenciaForward(int i, const std::vector<std::vector<int>>& dependencies, const std::vector<int>& color);
	 std::vector<int> coloreForwardILU();

	 std::vector<std::vector<int>> montaBackwardDependencias(int N, int* VvecfnzILU, int* VveclmILU);
	 int cor_dependenciaBackward(int i, const std::vector<std::vector<int>>& dependencies, const std::vector<int>& color);
	 std::vector<int> coloreBackwardILU();

	 void geraILUrank();
	 void geraILUcor();

     SparseMtx& operator=(const SparseMtx&);//sobrecarga =
     SparseMtx operator *(T) const;//multiplica��o de matriz por escalar mat*a
     Vcr<T> operator*(const Vcr<T>&) const;//multiplica��o entre matriz e vetor
     T& operator[](int i) const {return sra[i];}//subescrito
     SparseMtx& operator +=(const SparseMtx&);//sobrecarga de operador +=
     SparseMtx& operator -=(const SparseMtx&);//sobrecarga de operador -=
     SparseMtx  operator -() const; //sobrecarga de operador un�rio - -> m2=-m1
     SparseMtx operator +(const SparseMtx&) const;//sobrecarga de operador +
     SparseMtx operator -(const SparseMtx&) const;//sobrecarga de operador -
     int CG(Vcr<T>&, const Vcr<T>&, T&, int&, int);//gradiente conjugado
     int BiCGStab(Vcr<T>&, const Vcr<T>&, T&, int&, int);//BiCGStab
     int GMRES(Vcr<T>&, const Vcr<T>&, T&, int&, int, int); //GMRES
     int FGMRES(Vcr<T>&, const Vcr<T>&, T&, int&, int, int); //Flexible GMRES

};

template<class S> ostream& operator<<(ostream& s, const SparseMtx<S>&);//saida de valores de Matriz Banda
#endif
