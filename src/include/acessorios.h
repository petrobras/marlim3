#ifndef _ACESSORIO
#define _ACESSORIO
#define _USE_MATH_DEFINES // para M_PI

#include "Acidentes2.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "FonteMas.h"
#include "FonteMasVap.h"
#include "FonteMassCHK.h"
#include "PorosoRad-Simples.h"
#include "PropFlu.h"
#include "PropVapor.h"
#include "multiBCS.h"
#include "solverPoroso.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>

using namespace std;

class acessorio {
  public:
    // tipo -> Accessory type.
    // For an accessory vector, this value is used to identify the accessory type
    // and access its corresponding data.
    // 0 -> No accessory, 1 -> Gas source, 2 -> Liquid source, 3 -> IPR,
    // 4 -> ESP, 5 -> Choke, 6 -> Diameter change, 7 -> Pressure drop,
    // 8 -> Positive displacement pump, 9 -> Choke source, 10 -> Multiple source,
    // 11 -> Vapor IPR, 12 -> Vapor mass flow source, 14 -> Vapor positive displacement pump,
    // 15 -> Radial porous medium, 16 -> 2D porous medium, 17 -> Multiple ESPs.
    int tipo;

    BomCentSub bcs;
    multiBomCentSub multibcs;
    BomVol bvol;
    MudaArea mda;
    choke chk;
    IPR ipr;
    IPRVap iprvap;
    InjGas injg;
    InjLiq injl;
    InjMult injm;
    InjMultVap injmvap;
    fontemaschk fontechk;
    PorosRadSimp radialPoro;
    solverPoro poroso2D;
    ProFlu fluido;
    double delp;
    int tipoCompGas;
    double fatPoli;
    double eficLiq;
    double eficGas;
    acessorio(const int vtipo = 0);                // construtor
    acessorio(const acessorio &antigo);            // construtor de copia
    acessorio &operator=(const acessorio &antigo); // sobrecarga do operador =
};

inline void carrega(const BomCentSub &acsr, acessorio &grupo) {
    grupo.bcs = acsr;
    grupo.tipo = 3;
}
inline void carrega(const multiBomCentSub &acsr, acessorio &grupo) {
    grupo.multibcs = acsr;
    grupo.tipo = 17;
}
inline void carrega(const MudaArea &acsr, acessorio &grupo) {
    grupo.mda = acsr;
    grupo.tipo = 5;
}
inline void carrega(const choke &acsr, acessorio &grupo) {
    grupo.chk = acsr;
    grupo.tipo = 4;
}
inline void carrega(const IPR &acsr, acessorio &grupo) {
    grupo.ipr = acsr;
    grupo.fluido = acsr.FluidoPro;
    grupo.tipo = 3;
}
inline void carrega(const InjGas &acsr, acessorio &grupo) {
    grupo.injg = acsr;
    grupo.fluido = acsr.FluidoPro;
    grupo.tipo = 1;
}
inline void carrega(const InjLiq &acsr, acessorio &grupo) {
    grupo.injl = acsr;
    grupo.fluido = acsr.FluidoPro;
    grupo.tipo = 2;
}
inline void carrega(const InjMult &acsr, acessorio &grupo) {
    grupo.injm = acsr;
    grupo.fluido = acsr.FluidoPro;
    grupo.tipo = 10;
}
inline void carrega(const IPRVap &acsr, acessorio &grupo) {
    grupo.iprvap = acsr;
    grupo.tipo = 11;
}
inline void carrega(const InjMultVap &acsr, acessorio &grupo) {
    grupo.injmvap = acsr;
    grupo.tipo = 12;
}

#endif