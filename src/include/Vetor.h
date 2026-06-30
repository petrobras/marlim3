#ifndef _VETOR
#define _VETOR

#include <iostream>
#include <omp.h>

// The Vcr class defines a vector and provides several useful vector operations,
// including addition, subtraction, multiplication, norm calculation, inner products,
// and value output.

// Reference used in the development of this class:
// C++ and Object-Oriented Numeric Computing for Scientists and Engineers, Daoqi Yang

using namespace std;
extern int nthrdMatriz;

template <class T>
class Vcr {    // Vcr is a template class for vectors
    int lenth; // Vector length
    T *vr;     // Underlying vector storage of type T
  public:
    Vcr(int, const T *const); // Constructor
    Vcr(int = 0, T = 0);      // Default constructor
    Vcr(const Vcr &);         // Copy constructor
    ~Vcr() {
        if (vr)
            delete[] vr;
    } // Destructor
    int size() const { return lenth; }                   // Returns the vector length
    void resize(int);                                    // Resizes the vector
    Vcr &operator=(const Vcr &);                         // Overloads the assignment operator for vectors
    T &operator[](int i) const { return vr[i]; }         // Overloads the subscript operator
    Vcr &operator+=(const Vcr &);                        // Overloads the vector addition-assignment operator
    Vcr &operator+=(T);                                  // Overloads the scalar addition-assignment operator
    Vcr &operator-=(const Vcr &);                        // Overloads the vector subtraction-assignment operator
    Vcr &operator-=(T);                                  // Overloads the scalar subtraction-assignment operator
    Vcr operator-(T) const;                              // Subtracts a scalar from the vector
    Vcr operator-(const Vcr &) const;                    // Subtracts one vector from another
    Vcr operator-() { return Vcr<T>(lenth) - (*this); }; // Reverses the sign of all vector elements
    Vcr operator+(T) const;                              // Adds a scalar to the vector: V + a
    Vcr operator+(const Vcr &) const;                    // Adds two vectors
    Vcr operator/(T) const;                              // Divides the vector by a scalar: v[] / a
    Vcr operator*(T) const;                              // Multiplies the vector by a scalar: v[] * a
    Vcr operator*(const Vcr &) const;                    // Multiplies one vector by another
    ostream &saida(ostream &, const Vcr &) const;        // Outputs two vectors in separate columns
    T maxnorm() const;                                   // Computes the maximum norm using the largest vector element
    T onenorm() const;                                   // Computes the 1-norm
    T twonorm() const;                                   // Computes the 2-norm
};

// Additional overloaded operators that could not be declared as friend functions
double dot(const Vcr<double> &, const Vcr<double> &); // Computes the inner product
double dot(double *, double *, int);                  // Inner-product overload for arrays
ostream &operator<<(ostream &s, const Vcr<double> &); // Outputs vector values
Vcr<double> operator*(double, const Vcr<double> &);   // Multiplies a scalar by a vector: a * v[]

#endif