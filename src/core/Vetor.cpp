#define _USE_MATH_DEFINES // para M_PI
#include "Vetor.h"
#include <algorithm>
#include <fstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

// membros de VCR:
template <class T>
Vcr<T>::Vcr(int n, const T *const abd) { // construtor
    vr = new T[lenth = n];
    for (int i = 0; i < lenth; i++)
        vr[i] = *(abd + i); // copiando de uma area apontada por um ponteiro
}

template <class T>
Vcr<T>::Vcr(int n, T d) { // construtor a partir de um valor
    vr = new T[lenth = n];
    for (int i = 0; i < lenth; i++)
        vr[i] = d; // copiando um valor para o vetor
}

template <class T>
Vcr<T>::Vcr(const Vcr &v) { // construtor de c�pia
    vr = new T[lenth = v.lenth];
    for (int i = 0; i < lenth; i++)
        vr[i] = v[i]; // copiando os valores de um vetor para outro vetor
}

template <class T>
void Vcr<T>::resize(int n) { // redimensionamento de vetor
    Vcr<T> v = *this;
    if (n < lenth)
        cout << "tamanho original do vetor maior do que tamanho redimensionado";
    delete[] vr;
    vr = new T[lenth = n];
    if (v.lenth > 0) {
        for (int i = 0; i < v.lenth; i++)
            vr[i] = v[i];
    } // copiando os valores de um vetor para outro vetor
    for (int i = v.lenth; i < lenth; i++)
        vr[i] = 0; // espa�o redimensionado todo inicializado com zeros
}

template <class T>
Vcr<T> &Vcr<T>::operator=(const Vcr &v) { // iguala dois vetores
    if (lenth > 0) {
        if (this != &v) { // verifica se se est� igualando o mesmo vetor
            if (lenth != v.lenth) {
                if (vr)
                    delete[] vr;
                vr = new T[lenth = v.lenth];
            }
#pragma omp parallel for num_threads(nthrdMatriz)
            for (int i = 0; i < lenth; i++)
                vr[i] = v[i]; // igualando vetores, lembre-se da sobrecarga do operador [] em v[i]
        }
    } else {
        vr = new T[lenth = v.lenth];
#pragma omp parallel for num_threads(nthrdMatriz)
        for (int i = 0; i < lenth; i++)
            vr[i] = v[i]; // copiando os valores de um vetor para outro vetor
    }
    return *this;
}

template <class T>
ostream &Vcr<T>::saida(ostream &s, const Vcr &v) const { // sa�da
    if (lenth != v.lenth)
        cout << "Dimensao de vetores diferentes para impressao em paralelo";
    for (int i = 0; i < lenth; i++) {
        s.width(30);
        s.precision(15);
        s << vr[i] << "      " << v[i] << "\n";
    }
    return s;
}

template <class T>
T Vcr<T>::maxnorm() const {
    T nm = fabs(vr[0]);
    for (int i = 1; i < lenth; i++)
        nm = max<T>(nm, fabs(vr[i])); // busca o maior elemento de vr em m�dulo
    return nm;
}

template <class T>
T Vcr<T>::onenorm() const {
    T nm = fabs(vr[0]);
    for (int i = 1; i < lenth; i++)
        nm += fabs(vr[i]); // soma os m�dulos dos elementos de vr
    return nm;
}

template <class T>
T Vcr<T>::twonorm() const {

    T nm = 0.0;
#pragma omp parallel for reduction(+ : nm) num_threads(nthrdMatriz)
    for (int i = 0; i < lenth; i++)
        nm += vr[i] * vr[i]; // soma os quadrados dos elementos de vr
    return sqrt(nm);         // retorna a raiz do somat�rio
}

template <class T>
Vcr<T> &Vcr<T>::operator+=(const Vcr &v) { // soma o pr�prio vetor com outro vetor e iguala com o primeiro
    if (lenth != v.lenth)
        cout << "dimens�es de vetores diferem, imposs�vel igualar e somar"; // mesmo tamanho de vetores?
#pragma omp parallel for num_threads(nthrdMatriz)
    for (int i = 0; i < lenth; i++)
        vr[i] += v[i]; // igualando vetores, lembre-se da sobrecarga do operador [] em v[i]
    return *this;
}

template <class T>
Vcr<T> &Vcr<T>::operator+=(T a) { // soma o pr�prio vetor com um escalar
#pragma omp parallel for num_threads(nthrdMatriz)
    for (int i = 0; i < lenth; i++)
        vr[i] += a; // igualando vetores, lembre-se da sobrecarga do operador [] em v[i]
    return *this;
}

template <class T>
Vcr<T> &Vcr<T>::operator-=(const Vcr &v) { // subtrai o pr�prio vetor com outro vetor e iguala com o primeiro
    if (lenth != v.lenth)
        cout << "dimens�es de vetores diferem, imposs�vel igualar e somar"; // mesmo tamanho de vetores?
#pragma omp parallel for num_threads(nthrdMatriz)
    for (int i = 0; i < lenth; i++)
        vr[i] -= v[i]; // igualando vetores, lembre-se da sobrecarga do operador [] em v[i]
    return *this;
}

template <class T>
Vcr<T> &Vcr<T>::operator-=(T a) { // subtrai o pr�prio vetor com um escalar
#pragma omp parallel for num_threads(nthrdMatriz)
    for (int i = 0; i < lenth; i++)
        vr[i] -= a; // igualando vetores, lembre-se da sobrecarga do operador [] em v[i]
    return *this;
}

template <class T>
Vcr<T> Vcr<T>::operator-(const Vcr<T> &v) const { // subtra��o de vetores
    if (lenth != v.lenth)
        cout << "tamanho de vetores n�o confere\n";
    Vcr<T> sum = *this; // c�pia de vetores
    sum -= v;
    return sum;
}

template <class T>
Vcr<T> Vcr<T>::operator+(T a) const { // soma de vetor com escalar
    Vcr<T> sum = *this;               // c�pia de vetores
    sum += a;
    return sum;
}

template <class T>
Vcr<T> Vcr<T>::operator-(T a) const { // subtra��o de vetor com escalar
    Vcr<T> sum = *this;               // c�pia de vetores
    sum -= a;
    return sum;
}

template <class T>
Vcr<T> Vcr<T>::operator+(const Vcr<T> &v) const { // soma de vetores
    if (lenth != v.lenth)
        cout << "tamanho de vetores n�o confere\n";
    Vcr<T> sum = *this; // c�pia de vetores
    sum += v;
    return sum;
}

template <class T>
Vcr<T> Vcr<T>::operator/(T a) const { // divisao de um vetor por um escalar v[]/a
    Vcr<T> tm(lenth);
#pragma omp parallel for num_threads(nthrdMatriz)
    for (int i = 0; i < lenth; i++)
        tm[i] = (vr[i]) / a;
    return tm;
}

template <class T>
Vcr<T> Vcr<T>::operator*(T a) const { // multiplicacao de um vetor por um escalar v[]/a
    Vcr<T> tm(lenth);
#pragma omp parallel for num_threads(nthrdMatriz)
    for (int i = 0; i < lenth; i++)
        tm[i] = a * (vr[i]);
    return tm;
}

template <class T>
Vcr<T> Vcr<T>::operator*(const Vcr<T> &v) const { // multiplicacao de um vetor por outro
    if (lenth != v.lenth)
        cout << "tamanho de vetores n�o confere\n";
    int n = lenth;
    Vcr<T> tm(n);
#pragma omp parallel for num_threads(nthrdMatriz)
    for (int i = 0; i < n; i++)
        tm[i] = vr[i] * v[i]; // observe que na multiplicacao, ao contr�rio do "dot", o resultado ainda � um vetor e n�o um escalar
    return tm;
}

// friends de VCR:

double dot(const Vcr<double> &v1, const Vcr<double> &v2) { // calcula produto interno
    if (v1.size() != v2.size())
        cout << "tamanho de vetores n�o confere\n";
    int tam = v1.size();
    double tm = 0;
#pragma omp parallel for reduction(+ : tm) num_threads(nthrdMatriz)
    for (int i = 0; i < tam; i++)
        tm += v1[i] * v2[i];
    return tm;
}

double dot(double *a, double *b, int n) { // sobrecarga do produto interno para array
    // a e b sao os dois arrays para o produto interno, n o tamanho do array
    double init = 0;
    for (int i = 0; i < n; i++)
        init += *a++ * *b++;
    return init;
}

ostream &operator<<(ostream &s, Vcr<double> &v1) { // saida de valores de vetor
    int n = v1.size();
    for (int i = 0; i < n; i++) {
        s << v1[i] << "  ";
        if (i % 1 == 0 || i == n - 1)
            s << "\n"; // usa a fatoracao para contar a impressao de 10 em 10, cada linha tem no m�ximo 10 elementos
    }
    return s;
}

Vcr<double> operator*(double a, const Vcr<double> &v) { // Calculo da multiplicacao de um escalar por um vetor a*v[]
    return v * a;
}

template class Vcr<double>;
template class Vcr<int>;