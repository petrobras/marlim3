/*
 * versao.cpp
 *
 *  Created on: 6 de jan de 2020
 *      Author: cx3d
 */

/**
 * A versão Windows depende da biblioteca pthreads
 * que pode ser baixada em
 * ftp://sourceware.org/pub/pthreads-win32
 * para compilar:
 * make CROSS=x86_64-w64-mingw32- clean GC-static
 * ln -s libpthreadGC2.a libpthread.a
 */
#include "versao.h"
#include <iostream>

using namespace std;

void printInfo() {
	cerr << "Marlim transiente id " << VERSAO << endl;
	cerr << "Data/hora do build: " << DATA_BUILD;
	cerr << " feito por " << USUARIO << endl;
}
