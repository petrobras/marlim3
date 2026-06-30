#ifndef _MATRIZ
#define _MATRIZ
#define _USE_MATH_DEFINES // Required for M_PI

#include "Log.h"
#include "Vetor.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <math.h>
#include <mutex>
#include <omp.h>
#include <unordered_map>
#include <utility> // Required for std::pair
#include <vector>

using namespace std;

// Application logger object
extern Logger logger;

extern int nthrdMatriz;

// Matrix classes

// Hash map used to store fill levels efficiently
using LevelMap = std::unordered_map<long long, int>; // Maps (i, j) to its fill level

template <class T>
class AbsMtx {                                                 // Abstract matrix class containing the common virtual interface.
                                                               // It serves as the base interface for full, sparse, and band matrix implementations.
  protected:                                                   // Members declared here are accessible to derived classes
    int nrows;                                                 // Number of rows
    virtual Vcr<T> preconding(const Vcr<T> &r, int i = 0) = 0; // const=0;
                                                               // Solves Pz = r, where P is the preconditioner matrix used to accelerate GMRES and CG convergence
                                                               // r is the residual vector of a system such as Ax - vl = 0; z is returned as the solution
                                                               // of the system as a Vcr object
                                                               // i selects the preconditioning strategy:
                                                               // i = 0: no preconditioning; i = 1: diagonal preconditioner;
                                                               // i = 2: SSOR preconditioner
  public:
    virtual Vcr<T> operator*(const Vcr<T> &) const = 0;        // Overloaded operator for matrix-vector multiplication
    int CG(Vcr<T> &, const Vcr<T> &, T &, int &, int);         // Conjugate gradient method
    int GMRES(Vcr<T> &, const Vcr<T> &, T &, int &, int, int); // GMRES method
};

template <class T>
void error(const T &t); // Reports a matrix-related error

template <class T>
class FullMtx : public AbsMtx<T> { // Full matrix implementation derived from AbsMtx
  private:
    int ncols; // Number of matrix columns; the row count is inherited from AbsMtx
    // T** mx; // Pointer to the storage area used by the matrix
    Vcr<T> preconding(const Vcr<T> &r, int i = 0); // const; // Preconditioner implementation for full matrices
  public:
    T **mx;                                   // Pointer to the storage area used by the matrix
    FullMtx(int, int, const T **const);       // Constructs a matrix from existing storage; n is the row count and m is the column count
    FullMtx(int n = 15, int m = 15, T t = 0); // Constructs a matrix initialized with scalar value t, which defaults to zero
    FullMtx(const FullMtx &);                 // Copy constructor
    ~FullMtx() {                              // Destructor
        for (int i = 0; i < this->nrows; i++)
            delete[] mx[i];
        delete[] mx;
    }

    int col() const { return ncols; }
    int lin() const { return this->nrows; }
    T cheia(int i, int j) const { return mx[i][j]; } // Accesses the matrix using standard full-matrix indices

    FullMtx &operator=(const FullMtx &);         // Overloaded assignment operator
    FullMtx &operator+=(const FullMtx &);        // Overloaded += operator
    FullMtx &operator-=(const FullMtx &);        // Overloaded -= operator
    FullMtx operator-() const;                   // Overloaded unary minus operator: m2 = -m1
    FullMtx operator+(const FullMtx &) const;    // Overloaded addition operator: m2 + m1
    FullMtx operator-(const FullMtx &) const;    // Overloaded subtraction operator: m2 - m1
    T *operator[](int i) const { return mx[i]; } // Indexing operator
    FullMtx operator*(T) const;                  // Matrix-scalar multiplication: mat * a
    Vcr<T> operator*(const Vcr<T> &) const;      // Matrix-vector multiplication
    FullMtx operator*(const FullMtx &) const;    // Matrix-matrix multiplication
    void GaussElim(Vcr<T> &) const;              // Gaussian elimination without pivoting
    void GaussElimPP(Vcr<T> &) const;            // Gaussian elimination with partial pivoting
    void GaussElimCP(Vcr<T> &) const;            // Gaussian elimination with complete pivoting
    void LU(Vcr<T> &) const;                     // LU factorization
    void Inverte1(FullMtx &) const;              // Matrix inversion without pivoting
    T InterpolaUnif(T lin, T col) const;         // Interpolation on a uniformly spaced grid
};

template <class S>
ostream &operator<<(ostream &s, const FullMtx<S> &); // Outputs matrix values

// Band matrix
template <class T>
class BandMtx : public AbsMtx<T> { // Band matrix implementation derived from AbsMtx
  private:
    int bwlef;                                  // Number of nonzero diagonals to the left of the main diagonal
    int bwrit;                                  // Number of nonzero diagonals to the right of the main diagonal
    T **bdmx;                                   // Pointer to band-matrix storage; only nonzero terms are stored
    Vcr<T> preconding(const Vcr<T> &, int = 0); // const; // Preconditioner implementation for band matrices
  public:
    BandMtx(int, int, int, const T **const); // Constructor
    // n is the number of rows and columns; p is the bandwidth to the left of the main diagonal
    // r is the bandwidth to the right of the main diagonal; t points to the nonzero-term storage
    BandMtx(int n = 0, int p = 0, int r = 0, T t = 0); // Constructor
    BandMtx(int n, int p, int r, const FullMtx<T> &m); // Constructor
    BandMtx(const BandMtx &);                          // Copy constructor
    ~BandMtx() {
        for (int i = 0; i < this->nrows; i++)
            delete[] (bdmx[i] -= bwlef); // The constructor applies a storage offset so diagonal terms have index zero; therefore, the offset must be removed before deletion
        delete[] bdmx;
    }

    int lin() const { return this->nrows; } // Returns the number of rows
    int esq() const { return bwlef; }       // Returns the bandwidth to the left of the main diagonal
    int dir() const { return bwrit; }       // Returns the bandwidth to the right of the main diagonal
    T cheia(int, int) const;                // Accesses the matrix using standard full-matrix indices

    BandMtx &operator=(const BandMtx &);
    T *operator[](int i) const { return bdmx[i]; }
    BandMtx operator*(T) const;               // Matrix-scalar multiplication: mat * a
    Vcr<T> operator*(const Vcr<T> &) const;   // Matrix-vector multiplication
    BandMtx &operator+=(const BandMtx &);     // Overloaded += operator
    BandMtx &operator-=(const BandMtx &);     // Overloaded -= operator
    BandMtx operator-() const;                // Overloaded unary minus operator: m2 = -m1
    BandMtx operator+(const BandMtx &) const; // Overloaded addition operator
    BandMtx operator-(const BandMtx &) const; // Overloaded subtraction operator

    void GaussElim(Vcr<T> &) const;   // Gaussian elimination without pivoting
    void GaussElimPP(Vcr<T> &) const; // Gaussian elimination with partial pivoting
};

template <class S>
ostream &operator<<(ostream &s, const BandMtx<S> &); // Outputs matrix values Banda

// Sparse matrix
template <class T>
class SparseMtx : public AbsMtx<T> { // Sparse matrix implementation derived from AbsMtx
  private:
    Vcr<T> preconding(const Vcr<T> &, int i = 0); // const; // Preconditioner for GMRES and conjugate gradient
  public:
    int lenth; // Number of nonzero terms stored in the primary storage array
    T *sra;    // Points to the storage area containing nonzero terms
    int *clm;  //*clm stores an array with the same size as *sra, containing the matrix column of each nonzero term
    int *fnz;  //*fnz stores the index in *sra of the first nonzero entry in each row i; its size is lenth + 1

    int rank;
    int colorido;
    int noZeroIlu;
    int *veclevILU;
    int *vecRelILU;
    int *veclmILU;
    int *vecfnzILU;
    T *sraILU;

    std::vector<int> color_forward;
    std::vector<int> color_backward;

    SparseMtx(int n, int m, T *t, int *c, int *f, int vrank = 0, int vcolorido = 0, int vnoZeroIlu = 0, int *VveclevILU = 0, int *VvecRelILU = 0,
              int *VveclmILU = 0, int *vecfnzILU = 0, std::vector<int> vcolor_forward = std::vector<int>(1, 0),
              std::vector<int> vcolor_backward = std::vector<int>(1, 0)); // Constructor
    // n: number of matrix rows and columns
    // m: number of nonzero matrix terms, equal to the size of *sra
    // t: pointer to the nonzero entries that will be stored in sra
    // c: pointer to the column-index storage for each nonzero entry
    // f: pointer to the sra index of the first nonzero term in each row
    SparseMtx(int, int);          // Constructor com todas as entradas = zero
    SparseMtx(const SparseMtx &); // Constructor c�pia
    ~SparseMtx() {
        if (lenth > 0) {
            delete[] sra;
            delete[] fnz;
            delete[] clm;
        }
        if (noZeroIlu > 0) {
            delete[] sraILU;
        }
        if (rank > 0 && noZeroIlu > 0) {
            delete[] veclevILU;
            delete[] vecRelILU;
            delete[] veclmILU;
            delete[] vecfnzILU;
        }
    } // Destructor

    int comp() const { return lenth; }          // Returns the number of nonzero matrix entries
    int lin() const { return this->nrows; }     // Returns the number of rows da matriz
    T cheia(int, int) const;                    // Accesses the matrix using standard full-matrix indices
    int &getfnz(int i) const { return fnz[i]; } // Returns the first nonzero entry for a given row
    int &getclm(int i) const { return clm[i]; } // Returns the column containing a given nonzero term, sra[i]
    void qualPos(int &match, int &inf, int &sup, int linha, int coluna);
    void qualPosILU(int &match, int &inf, int &sup, int linha, int coluna);

    inline long long key(int i, int j) {
        return static_cast<long long>(i) * this->nrows + j;
    }

    void geraNivelFillinILU(std::vector<int> &level /* Term fill levels */,
                            std::vector<int> &linhaLev /* Row of each term */,
                            std::vector<int> &colunaLev /* Column of each term */);

    void montaILUfillinCSR(const std::vector<int> &level /* Fill level of each term */,
                           const std::vector<int> &linhaLev /* Row corresponding to each term */,
                           const std::vector<int> &colunaLev /* Column corresponding to each term */);

    std::vector<std::vector<int>> montaForwardDependencias(int N, int *VvecfnzILU, int *VveclmILU);
    int cor_dependenciaForward(int i, const std::vector<std::vector<int>> &dependencies, const std::vector<int> &color);
    std::vector<int> coloreForwardILU();

    std::vector<std::vector<int>> montaBackwardDependencias(int N, int *VvecfnzILU, int *VveclmILU);
    int cor_dependenciaBackward(int i, const std::vector<std::vector<int>> &dependencies, const std::vector<int> &color);
    std::vector<int> coloreBackwardILU();

    void geraILUrank();
    void geraILUcor();

    SparseMtx &operator=(const SparseMtx &);                    // Overloaded assignment operator
    SparseMtx operator*(T) const;                               // Matrix-scalar multiplication: mat * a
    Vcr<T> operator*(const Vcr<T> &) const;                     // Matrix-vector multiplication
    T &operator[](int i) const { return sra[i]; }               // Subscript operator
    SparseMtx &operator+=(const SparseMtx &);                   // Overloaded += operator
    SparseMtx &operator-=(const SparseMtx &);                   // Overloaded -= operator
    SparseMtx operator-() const;                                // Overloaded unary minus operator: m2 = -m1
    SparseMtx operator+(const SparseMtx &) const;               // Overloaded addition operator
    SparseMtx operator-(const SparseMtx &) const;               // Overloaded subtraction operator
    int CG(Vcr<T> &, const Vcr<T> &, T &, int &, int);          // Conjugate gradient method
    int BiCGStab(Vcr<T> &, const Vcr<T> &, T &, int &, int);    // BiCGStab method
    int GMRES(Vcr<T> &, const Vcr<T> &, T &, int &, int, int);  // GMRES method
    int FGMRES(Vcr<T> &, const Vcr<T> &, T &, int &, int, int); // Flexible GMRES method
};

template <class S>
ostream &operator<<(ostream &s, const SparseMtx<S> &); // Outputs matrix values Banda

#endif