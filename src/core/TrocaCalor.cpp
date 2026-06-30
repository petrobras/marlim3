/*
 * TrocaCalor.cpp
 *
 *  Created on: 24/04/2014
 *      Author: bwz1
 */

#define _USE_MATH_DEFINES // para M_PI
#include "TrocaCalor.h"
#include "Matriz.h"
#include "Vetor.h"
#include <algorithm>
#include <fstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

TransCal::TransCal(varGlob1D *Vvg1dSP, const DadosGeo vgeom, const int vpermanente, const int *const vncamada,
                   const double *const vdrcamada, double **const vTcamada, const double vTint,
                   const double vTextern1, const double vTextern2, const double vVint, const double vVextern1,
                   const int vdirconvExt, const double vdt, const double vkint, const double vcpint,
                   const double vrhoint, const double vviscint, const double vkextern1,
                   const double vcpextern1, const double vrhoextern1, const double vviscextern1,
                   const int vformac, const int vambext, const double vtempprod, const double vcondform,
                   const double vcpform, const double vrhoform,
                   const double pres, const double vdx, const double vkextern2,
                   const double vcpextern2, const double vrhoextern2,
                   const double vviscextern2, const int vcoluna, const double vcolunaDia) : localvet(2),
                                                                                            localmat(2, 6) {
    vg1dSP = Vvg1dSP;
    geom = vgeom;
    permanente = vpermanente;
    lenth = geom.ncamadas;
    coluna = vcoluna;
    colunaDia = vcolunaDia;

    formacPoc = vformac;
    ambext = vambext;
    tempprod = vtempprod;
    condform = vcondform;
    cpform = vcpform;
    rhoform = vrhoform;

    ccon = 0.;
    ncon = 0.;
    mcon = 0.;
    npet = 0.;
    Vconf = 0.;
    fluxIni = 0.;
    fluxFim = 0.;
    resGlob = 0.;
    resFim = 0.000001;

    condiTparede = 0;
    novoHi = 0;

    airMW = 28.97;
    RGas = 8314.4621;
    pressao = pres;
    TCNitro = Faren(-146.9) + 460.;
    PCNitro = psia(3399.9 * 1000. / 98066.52);

    reyi = 0;
    reye = 0;
    grashi = 0;
    grashe = 0;
    nusi = 0;
    nuse = 0;
    hi = 0;
    he = 0;
    pri = 0;
    pre = 0;
    prG = 0;
    prL = 0;
    indCel = 0;

    dx = vdx;
    dtL = 0.;
    dimExt = -1;

    difus2D = 0;
    poisson2D = solverP(); // Burial

    if (lenth > 0) {
        ncamada = new int[lenth];
        for (int i = 0; i < lenth; i++)
            ncamada[i] = vncamada[i];
        drcamada = new double[lenth];
        for (int i = 0; i < lenth; i++)
            drcamada[i] = vdrcamada[i];
        Tcamada = new double *[lenth];
        Tini = new double *[lenth];
        for (int i = 0; i < lenth; i++) {
            Tcamada[i] = new double[ncamada[i] + 1];
            Tini[i] = new double[ncamada[i] + 1];
            for (int j = 0; j < ncamada[i] + 1; j++) {
                Tcamada[i][j] = vTcamada[i][j];
                Tini[i][j] = vTcamada[i][j];
            }
        }
        Qcamada = new double *[lenth];
        Qini = new double *[lenth];
        for (int i = 0; i < lenth; i++) {
            Qcamada[i] = new double[ncamada[i] + 1];
            Qini[i] = new double[ncamada[i] + 1];
            for (int j = 0; j < ncamada[i] + 1; j++) {
                Qcamada[i][j] = 0.;
                Qini[i][j] = 0.;
            }
        }

        nglobal = 1;
        for (int i = 0; i < geom.ncamadas; i++)
            nglobal += ncamada[i];

        if (formacPoc == 1)
            resFim = ResForm();

        Tint = vTint;
        Tint2 = Tint;
        Textern1 = vTextern1;
        Textern2 = vTextern2;
        Vint = vVint;
        Vextern1 = vVextern1;
        dirconvExt = vdirconvExt;
        dt = vdt;
        betext = 1 / (273.15 + Textern1);
        betint = 1.;

        kint = vkint;
        cpint = vcpint;
        rhoint = vrhoint;
        viscint = vviscint;
        kextern1 = vkextern1;
        cpextern1 = vcpextern1;
        rhoextern1 = vrhoextern1;
        viscextern1 = vviscextern1 / 1000.;
        kextern2 = vkextern2;
        cpextern2 = vcpextern2;
        rhoextern2 = vrhoextern2;
        viscextern2 = vviscextern2;
    } else {
        lenth = 0;
        ncamada = 0;
        drcamada = 0;
        Tcamada = 0;
        Qcamada = 0;
        Tini = 0;
        Qini = 0;
        Tint = 0;
        Tint2 = Tint;
        Textern1 = 0;
        Textern2 = 0;
        Vint = 0;
        Vextern1 = 0;
        dirconvExt = 0;
        dt = 0;
        betext = 0;

        nglobal = 0;

        kint = 0;
        cpint = 0;
        rhoint = 0;
        viscint = 0;
        kextern1 = 0;
        cpextern1 = 0;
        rhoextern1 = 0;
        viscextern1 = 0;
        kextern2 = 0;
        cpextern2 = 0;
        rhoextern2 = 0;
        viscextern2 = 0;
    }
}

TransCal::TransCal(const TransCal &vTransCal) : localvet(2),
                                                localmat(2, 6) {

    vg1dSP = vTransCal.vg1dSP;
    geom = vTransCal.geom;
    permanente = vTransCal.permanente;
    lenth = geom.ncamadas;
    coluna = vTransCal.coluna;
    colunaDia = vTransCal.colunaDia;

    formacPoc = vTransCal.formacPoc;
    ambext = vTransCal.ambext;
    resFim = vTransCal.resFim;
    tempprod = vTransCal.tempprod;
    condform = vTransCal.condform;
    cpform = vTransCal.cpform;
    rhoform = vTransCal.rhoform;

    condiTparede = vTransCal.condiTparede;
    novoHi = vTransCal.novoHi;
    indCel = vTransCal.indCel;

    ccon = 0.;
    ncon = 0.;
    mcon = 0.;
    npet = 0.;

    airMW = 28.97;
    RGas = 8314.4621;
    pressao = vTransCal.pressao;
    TCNitro = vTransCal.TCNitro;
    PCNitro = vTransCal.PCNitro;

    reyi = vTransCal.reyi;
    reye = vTransCal.reye;
    grashi = vTransCal.grashi;
    grashe = vTransCal.grashe;
    nusi = vTransCal.nusi;
    nuse = vTransCal.nuse;
    hi = vTransCal.hi;
    he = vTransCal.he;
    pri = vTransCal.pri;
    pre = vTransCal.pre;
    prG = vTransCal.prG;
    prL = vTransCal.prL;

    dx = vTransCal.dx;
    dtL = vTransCal.dtL;
    dimExt = vTransCal.dimExt;

    difus2D = vTransCal.difus2D;
    poisson2D = vTransCal.poisson2D; // Burial

    if (lenth > 0) {
        Vconf = vTransCal.Vconf;
        ncamada = new int[lenth];
        for (int i = 0; i < lenth; i++)
            ncamada[i] = vTransCal.ncamada[i];
        drcamada = new double[lenth];
        for (int i = 0; i < lenth; i++)
            drcamada[i] = vTransCal.drcamada[i];
        Tcamada = new double *[lenth];
        Tini = new double *[lenth];
        for (int i = 0; i < lenth; i++) {
            Tcamada[i] = new double[ncamada[i] + 1];
            Tini[i] = new double[ncamada[i] + 1];
            for (int j = 0; j < ncamada[i] + 1; j++) {
                Tcamada[i][j] = vTransCal.Tcamada[i][j];
                Tini[i][j] = vTransCal.Tini[i][j];
            }
        }
        Qcamada = new double *[lenth];
        Qini = new double *[lenth];
        for (int i = 0; i < lenth; i++) {
            Qcamada[i] = new double[ncamada[i] + 1];
            Qini[i] = new double[ncamada[i] + 1];
            for (int j = 0; j < ncamada[i] + 1; j++) {
                Qcamada[i][j] = vTransCal.Qcamada[i][j];
                Qini[i][j] = vTransCal.Qini[i][j];
            }
        }

        nglobal = 1;
        for (int i = 0; i < geom.ncamadas; i++)
            nglobal += ncamada[i];

        Tint = vTransCal.Tint;
        Tint2 = vTransCal.Tint2;
        Textern1 = vTransCal.Textern1;
        Textern2 = vTransCal.Textern2;
        Vint = vTransCal.Vint;
        Vextern1 = vTransCal.Vextern1;
        dirconvExt = vTransCal.dirconvExt;
        dt = vTransCal.dt;

        kint = vTransCal.kint;
        cpint = vTransCal.cpint;
        rhoint = vTransCal.rhoint;
        viscint = vTransCal.viscint;
        kextern1 = vTransCal.kextern1;
        cpextern1 = vTransCal.cpextern1;
        rhoextern1 = vTransCal.rhoextern1;
        viscextern1 = vTransCal.viscextern1;
        betext = vTransCal.betext;
        betint = vTransCal.betint;
        kextern2 = vTransCal.kextern2;
        cpextern2 = vTransCal.cpextern2;
        rhoextern2 = vTransCal.rhoextern2;
        viscextern2 = vTransCal.viscextern2;

        fluxIni = vTransCal.fluxIni;
        fluxFim = vTransCal.fluxFim;
        resGlob = vTransCal.resGlob;

    } else {
        lenth = 0;
        ncamada = 0;
        drcamada = 0;
        Tcamada = 0;
        Qcamada = 0;
        Tini = 0;
        Qini = 0;
        Tint = 0;
        Tint2 = 0;
        Textern1 = 0;
        Textern2 = 0;
        betint = 0;
        Vint = 0;
        Vextern1 = 0;
        Vconf = 0.;
        dirconvExt = 0;
        dt = 0;
        nglobal = 0;
        betext = 0;
        indCel = 0;

        kint = 0;
        cpint = 0;
        rhoint = 0;
        viscint = 0;
        kextern1 = 0;
        cpextern1 = 0;
        rhoextern1 = 0;
        viscextern1 = 0;
        kextern2 = 0;
        cpextern2 = 0;
        rhoextern2 = 0;
        viscextern2 = 0;

        fluxIni = 0.;
        fluxFim = 0.;
        resGlob = 0.;
    }
}

TransCal &TransCal::operator=(const TransCal &vTransCal) {

    localvet = vTransCal.localvet;
    localmat = vTransCal.localmat;
    vg1dSP = vTransCal.vg1dSP;
    if (lenth > 0) {
        if (this != &vTransCal) {
            geom = vTransCal.geom;
            permanente = vTransCal.permanente;
            coluna = vTransCal.coluna;
            colunaDia = vTransCal.colunaDia;
            formacPoc = vTransCal.formacPoc;
            ambext = vTransCal.ambext;
            resFim = vTransCal.resFim;
            tempprod = vTransCal.tempprod;
            condform = vTransCal.condform;
            cpform = vTransCal.cpform;
            rhoform = vTransCal.rhoform;

            reyi = vTransCal.reyi;
            reye = vTransCal.reye;
            grashi = vTransCal.grashi;
            grashe = vTransCal.grashe;
            nusi = vTransCal.nusi;
            nuse = vTransCal.nuse;
            hi = vTransCal.hi;
            he = vTransCal.he;
            pri = vTransCal.pri;
            pre = vTransCal.pre;
            prG = vTransCal.prG;
            prL = vTransCal.prL;

            indCel = vTransCal.indCel;

            condiTparede = vTransCal.condiTparede;
            difus2D = vTransCal.difus2D;
            poisson2D = vTransCal.poisson2D; // Burial

            dx = vTransCal.dx;
            dtL = vTransCal.dtL;
            dimExt = vTransCal.dimExt;

            if (lenth != vTransCal.lenth)
                cout << "dimensoes de vetores diferem em número de camadas, imposs�vel igualar na TransCal";
            for (int i = 0; i < lenth; i++)
                drcamada[i] = vTransCal.drcamada[i];
            for (int i = 0; i < lenth; i++) {
                if (ncamada[i] > 0) {
                    if (ncamada[i] != vTransCal.ncamada[i])
                        cout << "dimensoes de vetores diferem em número de discretizações, impossivel igualar na TransCal";
                    for (int j = 0; j < ncamada[i] + 1; j++)
                        Tcamada[i][j] = vTransCal.Tcamada[i][j];
                    for (int j = 0; j < ncamada[i] + 1; j++)
                        Qcamada[i][j] = vTransCal.Qcamada[i][j];
                }
            }
            for (int i = 0; i < lenth; i++) {
                if (ncamada[i] > 0) {
                    if (ncamada[i] != vTransCal.ncamada[i])
                        cout << "dimensoes de vetores diferem em número de discretizações, impossivel igualar na TransCal";
                    for (int j = 0; j < ncamada[i] + 1; j++)
                        Tini[i][j] = vTransCal.Tini[i][j];
                    for (int j = 0; j < ncamada[i] + 1; j++)
                        Qini[i][j] = vTransCal.Qini[i][j];
                }
            }

            nglobal = 1;
            for (int i = 0; i < geom.ncamadas; i++)
                nglobal += ncamada[i];

            Tint = vTransCal.Tint;
            Tint2 = vTransCal.Tint2;
            Textern1 = vTransCal.Textern1;
            Textern2 = vTransCal.Textern2;
            Vint = vTransCal.Vint;
            Vextern1 = vTransCal.Vextern1;
            Vconf = vTransCal.Vconf;
            dirconvExt = vTransCal.dirconvExt;
            dt = vTransCal.dt;
            betext = vTransCal.betext;
            betint = vTransCal.betint;

            pressao = vTransCal.pressao;
            TCNitro = vTransCal.TCNitro;
            PCNitro = vTransCal.PCNitro;
        }
    } else if (lenth == 0) {
        if (this != &vTransCal) {
            geom = vTransCal.geom;
            permanente = vTransCal.permanente;
            lenth = geom.ncamadas;
            coluna = vTransCal.coluna;
            colunaDia = vTransCal.colunaDia;
            formacPoc = vTransCal.formacPoc;
            ambext = vTransCal.ambext;
            resFim = vTransCal.resFim;
            tempprod = vTransCal.tempprod;
            condform = vTransCal.condform;
            cpform = vTransCal.cpform;
            rhoform = vTransCal.rhoform;
            pressao = vTransCal.pressao;
            TCNitro = vTransCal.TCNitro;
            PCNitro = vTransCal.PCNitro;

            reyi = vTransCal.reyi;
            reye = vTransCal.reye;
            grashi = vTransCal.grashi;
            grashe = vTransCal.grashe;
            nusi = vTransCal.nusi;
            nuse = vTransCal.nuse;
            hi = vTransCal.hi;
            he = vTransCal.he;
            pri = vTransCal.pri;
            pre = vTransCal.pre;
            prG = vTransCal.prG;
            prL = vTransCal.prL;

            condiTparede = vTransCal.condiTparede;
            novoHi = vTransCal.novoHi;
            indCel = vTransCal.indCel;

            dx = vTransCal.dx;
            dtL = vTransCal.dtL;
            dimExt = vTransCal.dimExt;

            if (lenth > 0) {
                ncamada = new int[lenth];
                for (int i = 0; i < lenth; i++)
                    ncamada[i] = vTransCal.ncamada[i];
                drcamada = new double[lenth];
                for (int i = 0; i < lenth; i++)
                    drcamada[i] = vTransCal.drcamada[i];
                Tcamada = new double *[lenth];
                Tini = new double *[lenth];
                for (int i = 0; i < lenth; i++) {
                    Tcamada[i] = new double[ncamada[i] + 1];
                    Tini[i] = new double[ncamada[i] + 1];
                    for (int j = 0; j < ncamada[i] + 1; j++) {
                        Tcamada[i][j] = vTransCal.Tcamada[i][j];
                        Tini[i][j] = vTransCal.Tini[i][j];
                    }
                }
                Qcamada = new double *[lenth];
                Qini = new double *[lenth];
                for (int i = 0; i < lenth; i++) {
                    Qcamada[i] = new double[ncamada[i] + 1];
                    Qini[i] = new double[ncamada[i] + 1];
                    for (int j = 0; j < ncamada[i] + 1; j++) {
                        Qcamada[i][j] = vTransCal.Qcamada[i][j];
                        Qini[i][j] = vTransCal.Qini[i][j];
                    }
                }

                nglobal = 1;
                for (int i = 0; i < geom.ncamadas; i++)
                    nglobal += ncamada[i];

                Tint = vTransCal.Tint;
                Tint2 = vTransCal.Tint2;
                Textern1 = vTransCal.Textern1;
                Textern2 = vTransCal.Textern2;
                Vint = vTransCal.Vint;
                Vextern1 = vTransCal.Vextern1;
                Vconf = vTransCal.Vconf;
                dirconvExt = vTransCal.dirconvExt;
                dt = vTransCal.dt;
                betext = vTransCal.betext;
                betint = vTransCal.betint;

                kint = vTransCal.kint;
                cpint = vTransCal.cpint;
                rhoint = vTransCal.rhoint;
                viscint = vTransCal.viscint;
                kextern1 = vTransCal.kextern1;
                cpextern1 = vTransCal.cpextern1;
                rhoextern1 = vTransCal.rhoextern1;
                viscextern1 = vTransCal.viscextern1;
                kextern2 = vTransCal.kextern2;
                cpextern2 = vTransCal.cpextern2;
                rhoextern2 = vTransCal.rhoextern2;
                viscextern2 = vTransCal.viscextern2;

                fluxIni = vTransCal.fluxIni;
                fluxFim = vTransCal.fluxFim;
                resGlob = vTransCal.resGlob;
            } else {
                lenth = 0;
                ncamada = 0;
                drcamada = 0;
                Tcamada = 0;
                Tini = 0;
                Qcamada = 0;
                Qini = 0;
                Tint = 0;
                Tint2 = 0;
                Textern1 = 0;
                Textern2 = 0;
                Vint = 0;
                Vextern1 = 0;
                Vconf = 0.;
                dirconvExt = 0;
                dt = 0;
                coluna = 0;
                colunaDia = 0.;
                betext = 0.;
                betint = 0;
                indCel = 0;

                nglobal = 0;

                kint = 0;
                cpint = 0;
                rhoint = 0;
                viscint = 0;
                kextern1 = 0;
                cpextern1 = 0;
                rhoextern1 = 0;
                viscextern1 = 0;
                kextern2 = 0;
                cpextern2 = 0;
                rhoextern2 = 0;
                viscextern2 = 0;

                fluxIni = 0.;
                fluxFim = 0.;
                resGlob = 0.;
            }
        }
    }

    ccon = 0.;
    ncon = 0.;
    mcon = 0.;
    npet = 0.;

    return *this;
}

void TransCal::atualiza(DadosGeo vgeom, int discre) {
    TransCal vTransCal = *this;
    if (ncamada)
        delete[] ncamada;
    if (drcamada)
        delete[] drcamada;
    if (lenth > 0) {
        for (int i = 0; i < lenth; i++) {
            delete[] Tcamada[i];
            delete[] Tini[i];
        }
        delete[] Tcamada;
        delete[] Tini;
        for (int i = 0; i < lenth; i++) {
            delete[] Qcamada[i];
            delete[] Qini[i];
        }
        delete[] Qcamada;
        delete[] Qini;
    }

    geom = vgeom;
    lenth = geom.ncamadas;

    ncamada = new int[lenth];
    for (int i = 0; i < lenth - 1; i++)
        ncamada[i + 1] = vTransCal.ncamada[i];
    ncamada[0] = 1;
    drcamada = new double[lenth];
    for (int i = 0; i < lenth - 1; i++)
        drcamada[i + 1] = vTransCal.drcamada[i];
    drcamada[0] = vgeom.espessuR[0];
    Tcamada = new double *[lenth];
    Tini = new double *[lenth];
    for (int i = 0; i < lenth; i++) {
        Tcamada[i] = new double[ncamada[i] + 1];
        Tini[i] = new double[ncamada[i] + 1];
    }
    for (int j = 0; j < ncamada[0] + 1; j++) {
        if (j == 0) {
            Tcamada[0][j] = vTransCal.Tint;
            Tini[0][j] = vTransCal.Tint;
        } else {
            Tcamada[0][j] = vTransCal.Tcamada[0][0];
            Tini[0][j] = vTransCal.Tini[0][0];
        }
    }
    for (int i = 1; i < lenth; i++) {
        for (int j = 0; j < ncamada[i] + 1; j++) {
            Tcamada[i][j] = vTransCal.Tcamada[i - 1][j];
            Tini[i][j] = vTransCal.Tini[i - 1][j];
        }
    }
    Qcamada = new double *[lenth];
    Qini = new double *[lenth];
    for (int i = 0; i < lenth; i++) {
        Qcamada[i] = new double[ncamada[i] + 1];
        Qini[i] = new double[ncamada[i] + 1];
    }
    for (int j = 0; j < ncamada[0] + 1; j++) {
        Qcamada[0][j] = vTransCal.Qcamada[0][0];
        Qini[0][j] = vTransCal.Qini[0][0];
    }
    for (int i = 1; i < lenth; i++) {
        for (int j = 0; j < ncamada[i] + 1; j++) {
            Qcamada[i][j] = vTransCal.Qcamada[i - 1][j];
            Qini[i][j] = vTransCal.Qini[i - 1][j];
        }
    }

    nglobal = 1;
    for (int i = 0; i < geom.ncamadas; i++)
        nglobal += ncamada[i];
}

void TransCal::atualiza2(DadosGeo vgeom) {
    drcamada[0] = vgeom.espessuR[0];
    geom.atualizaCamada3(vgeom.espessuR[0], vgeom.cp[0], vgeom.cond[0], vgeom.rhoC[0]);
}

double TransCal::ZGopal(double PR, double TR) const {
    double ZGop;
    if (PR < (TR - 0.73) || (PR < (4.7 * TR - 6.74) && PR < (5.6875 - 0.525 * TR)))
        ZGop = 1;
    else if (PR <= 5.4) {
        int i = 1;
        if (PR > 1.2) {
            if (PR > 1.4 || TR < 1.08 || TR > 1.19) {
                if (PR <= 2.8)
                    i = 2;
                else
                    i = 3;
            }
        }
        int k = 4;
        if (TR <= 2)
            k = 3;
        if (TR <= 1.4)
            k = 2;
        if (TR <= 1.2)
            k = 1;
        int j = 16 * i + 4 * k - 19;
        ZGop = PR * (CoefGopalHT[j - 1] * TR + CoefGopalHT[j]) + CoefGopalHT[j + 1] * TR + CoefGopalHT[j + 2];
    } else
        ZGop = PR * pow(0.711 + 3.66 * TR, -1.4667) - 1.637 / (0.319 * TR + 0.522) + 2.071;
    return ZGop;
}

double TransCal::MasEspGas(double temp) const {
    // Gas density in kg/m^3, pressure in kgf/cm^2, and temperature in degrees Celsius.
    double Deng = 0.9669;
    double ipres = psia(pressao);
    double itemp = Faren(temp);
    double PR = ipres / PCNitro;
    double TR = (itemp + 460) / TCNitro;
    return ((Deng * 28.9625) * pressao * 98066.5) / (8.0465 * 1000 * ZGopal(PR, TR) * (temp + 273));
}

double TransCal::CalorGas(double temp) const {
    double Deng = 0.9669;
    return 1. * ((1.88 + 1.712 * Deng) * temp + 2651. - 897.2 * Deng);
}

double TransCal::CondGas(double temp) const {
    double ppas = 98066.5 * pressao;
    double fatT = (temp + 273.15) / 191.1;
    double XK1 = 3.04314 / 100. + (1.3242 / 10000. + 1.27534 * temp / 10000000.) * temp;
    double RXK = 0.99783 + (1.973 / 100000000. + 7.8868 * pow(10., -16.) * ppas) * ppas;
    if (fatT < 3.0)
        return XK1 * (1. + (1. - RXK) * (fatT - 3.) / 1.354);
    else
        return XK1;
}

double TransCal::ViscGas(double temp) const {
    double Deng = 0.9669;
    double TF = Faren(temp);
    double TR = TF + 459.67;
    double wg = Deng * 29;
    double AK = (9.4 + 0.02 * wg) * pow(TR, 1.5) / (209. + 19. * wg + TR);
    double x = 3.5 + (986 / TR) + 0.01 * wg;
    double y = 2.4 - 0.2 * x;
    double rhog = MasEspGas(temp) / 1000.;
    double visc = AK * exp(x * pow(rhog, y)) / 10000.;
    return visc;
}

double TransCal::beta(double temp, int tipo) const {
    double bet;
    double dtemp;
    if (fabs(temp) > 1e-15)
        dtemp = 0.01 * temp;
    else
        dtemp = 0.1;
    if (tipo == 1) {
        double rho0 = MasEspLiq(temp);
        double rhodel = MasEspLiq(temp + dtemp);
        bet = -(1 / rho0) * (rhodel - rho0) / (dtemp);
    } else if (tipo == 2) {
        double rho0 = MasEspGas(temp);
        double rhodel = MasEspGas(temp + dtemp);
        bet = -(1 / rho0) * (rhodel - rho0) / (dtemp);
    } else {
        double rho0 = MasEspAr(temp);
        double rhodel = MasEspAr(temp + dtemp);
        bet = -(1 / rho0) * (rhodel - rho0) / (dtemp);
    }
    return bet;
}

double TransCal::Grash(double dteta, double beta, double cinVisc, double del) {
    return 9.82 * beta * pow(del, 3.) * dteta / (cinVisc * cinVisc);
}

double TransCal::Pr(double cinVisc, double difus) {
    return cinVisc / difus;
}

double TransCal::Ra(double gra, double pran) {
    return gra * pran;
}

double TransCal::NussConf(double Ra, double heigth, double delD) {
    return ccon * pow(Ra, ncon) * pow(heigth / delD, mcon);
}

double TransCal::NussConf2(double Ra, double heigth, double delD, double teta) {
    if (fabs(ccon - 1.) > 1.e-5) {
        double nu90 = 0.;
        double nu60 = 0.;
        double nufim;
        double nu1 = 0.0605 * pow(Ra, 1. / 3);

        double val1 = pow(Ra, 0.293);
        double val2 = pow(6310 / Ra, 1.36);
        double val3 = pow(0.104 * val1 / (1 + val2), 3.);
        double nu2 = pow(1 + val3, 1 / 3);

        double a = heigth / delD;
        double nu3 = 0.242 * pow(Ra / a, 0.272);

        double max = nu1;
        if (nu2 > max)
            max = nu2;
        if (nu3 > max)
            max = nu3;
        nufim = nu90 = max;
        if ((M_PI / 2. - fabs(teta)) > 1e-3 && (fabs(teta) - M_PI / 3.) >= 0.) {
            double g = 0.5 / pow(1 + pow(Ra / 3160, 20.6), 0.1);
            nu1 = pow(1. + pow(0.093 * pow(Ra, 0.314) / (1. + g), 7.), 1. / 7.);
            nu2 = (0.104 + 0.175 / a) * pow(Ra, 0.283);
            max = nu1;
            if (nu2 > max)
                max = nu2;
            nu60 = max;
            nufim = ((M_PI / 2. - fabs(teta)) * nu60 + (fabs(teta) - M_PI / 3.) * nu90) / (M_PI / 6.);
        }
        if ((fabs(teta) - M_PI / 3.) < 0.) {
            double val1 = 1 - 1708. / (Ra * cos(teta));
            if (val1 < 0.)
                val1 = 0.;
            double val2 = 1 - 1708. * pow(sin(1.8 * fabs(teta)), 1.6) / (Ra * cos(teta));
            double val3 = pow(Ra * cos(teta) / 5830., 1. / 3.) - 1.;
            if (val3 < 0.)
                val3 = 0.;
            nufim = 1 + 1.44 * val1 * val2 + val3;
        }
        return nufim;
    } else
        return ccon * pow(Ra, ncon) * pow(heigth / delD, mcon);
}

double TransCal::NussConf3(double pr, double Ra, double heigth, double delD) {
    double nu1 = 0.;

    double gr = Ra / pr;

    if (gr < 2000.)
        nu1 = 1.;
    else if (Ra > 2 * 1e4 && Ra < 2. * 1e5)
        nu1 = 0.18 * pow(gr, 0.25) * pow(heigth / delD, -1. / 9.);
    else if (Ra > 2 * 1e5 && Ra < 1.1 * 1e7)
        nu1 = 0.065 * pow(gr, 0.33) * pow(heigth / delD, -1. / 9.);
    return nu1;
}
double TransCal::RaInt(double dteta, double beta, double cinVisc, double difus) {
    return 9.82 * beta * pow(0.2854 * geom.dia, 3.) * dteta / (cinVisc * difus);
}

double TransCal::RaExt(double dteta, double beta, double cinVisc, double difus) {
    if (dimExt < 0)
        return 9.82 * beta * pow(geom.diamC[geom.ncamadas - 1] * fabs(cos(geom.teta)), 3.) * dteta / (cinVisc * difus);
    else
        return 9.82 * beta * pow(dimExt * fabs(cos(geom.teta)), 3.) * dteta / (cinVisc * difus);
}

double TransCal::ReyIn(double vel, double visc, double rho, double dia) {
    return dia * rho * fabs(vel) / visc;
}

double TransCal::fric(double vrey, double eps) {

    if (vrey < 0.0000001)
        vrey = 0.0000001;
    double val;
    if (vrey > 2400) {
        val = 6.9 / fabs(vrey) + pow(eps / 3.7, 1.11);
        val = -1.8 * (log(val) / log(10.));
        val = pow(1 / val, 2.);
    } else {
        val = 16. / fabs(vrey);
    }
    return 4. * val;
}

double TransCal::nussPet(double vrey, double pr, double eps, double viscb, double viscw) {
    if (vrey > 2400.) {
        if ((viscw / viscb) < 40.) {
            double x;
            double f;
            f = fric(vrey, eps);
            x = 1.07 + 12.7 * (pow(pr, 2. / 3.) - 1.) * sqrt(f / 8.);
            return (vrey * pr / x) * (f / 8.) * pow(viscb / viscw, npet);
        } else {
            double alocal = 0.88 - 0.24 / (4. + pr);
            double blocal = 0.33 + 0.5 * exp(-0.6 * pr);
            return 5. + 0.016 * pow(vrey, alocal) * pow(pr, blocal);
        }
    } else
        return 3.6;
}

double TransCal::nussChuBer(double vrey, double pr) {
    double val;
    if (vrey < 20000. || vrey > 400000.)
        val = 0.3 + 0.62 * sqrt(vrey) * pow(pr, 1. / 3.) * pow(1. + pow(vrey / 282000, 5. / 8.), 4. / 5.) / pow(1. + pow(0.4 / pr, 2. / 3.), 1. / 4.);
    else
        val = 0.3 + 0.62 * sqrt(vrey) * pow(pr, 1. / 3.) * (1. + pow(vrey / 282000, 1. / 2.)) / pow(1. + pow(0.4 / pr, 2. / 3.), 1. / 4.);
    return val;
}

double TransCal::nussNatExt(double ra, double pr) {
    double val;
    if (ra < 1e-3)
        ra = 1e-3;
    val = 0.6 + 0.387 * pow(ra, 0.166667) / pow(1 + pow(0.559 / pr, 0.5625), 0.2963);
    return val;
}

void TransCal::defineConf(double pr, double ra) {
    if ((pr >= 0.5 && pr <= 2) && (ra >= 2 * 1e3 && ra <= 2 * 1e5)) {
        ccon = 0.197;
        ncon = 1. / 4.;
        mcon = -1. / 9.;
    } else if (ra >= 2 * 1e5) {
        ccon = 0.073;
        ncon = 1. / 3.;
        mcon = -1. / 9.;
    } else {
        ccon = 1.;
        ncon = 0.;
        mcon = 0.;
    }
}

void TransCal::definePet(double tb, double tw) {
    if (tb > tw)
        npet = 0.25;
    else
        npet = 0.11;
}

double TransCal::ResForm() {

    double difusForm = condform / (rhoform * cpform);
    double tadim = 4. * tempprod * 86400 * difusForm / (geom.diamC[geom.ncamadas - 1] * geom.diamC[geom.ncamadas - 1]);
    double temperadim;
    if (tadim <= 1.)
        temperadim = 1.1281 * sqrt(tadim) * (1. - 0.3 * sqrt(tadim));
    else if (tadim <= 4.5)
        temperadim = 0.432 + 0.4792 * tadim - 0.127 * tadim * tadim + 0.0201 * tadim * tadim * tadim - 0.0013 * pow(tadim, 4.);
    else
        temperadim = (0.4063 + 0.5 * log(tadim)) * (1. + 0.6 / tadim);

    return temperadim / (2. * M_PI * condform);
}

double TransCal::MasEspLiq(double temp) const {

    double tfarAmb = Faren(20.);
    double ppsiAmb = psia(1.);
    double tfar = Faren(temp);
    double bwAmb = 1.0 + 1.2e-04 * (tfarAmb - 60.0) + 1.0e-06 * pow(tfarAmb - 60.0, 2.) - 3.33e-06 * ppsiAmb;
    double bw = 1.0 + 1.2e-04 * (tfar - 60.0) + 1.0e-06 * pow(tfar - 60.0, 2.);

    double rholw = (1000. / bw) * bwAmb;
    double rhosal = 2160.;
    double x = 0. / 1000.;
    double rhomist = (1 - x) / rholw + x / rhosal;
    return 1. / rhomist;
}

double TransCal::VisLiq(double temp) const {

    double vis = 2.414E-05 * pow(10.0, 247.8 / (temp + 133.15));
    return vis * 1000.;
}

double TransCal::CondLiq(double temp) const {

    return 5.64981e-01 + (1.75148e-03 - 6.21225e-06 * temp) * temp;
}

double TransCal::CalorLiq(double temp) const {
    double ValCp;
    if (temp < 136.85)
        ValCp = 4185.5 * (1.00536 + (-4.31108e-04 + (7.01150e-06 - 2.42139e-08 * temp) * temp) * temp);
    else
        ValCp = 4185.5 * (0.189300 + (1.30609e-02 + (-6.89620e-05 + 1.29156e-07 * temp) * temp) * temp);
    return ValCp;
}

double TransCal::MasEspAr(double temp) const {

    return 1101325.0 * airMW / (RGas * (temp + 273.15));
}

double TransCal::VisAr(double temp) const {

    return 1.79903E-05 + 1.00046E-07 * temp - 1.36976E-09 * temp * temp + 6.96E-12 * temp * temp * temp;
}

double TransCal::CondAr(double temp) const {

    return 0.0241028 + 7.586E-05 * temp;
}

double TransCal::CalorAr(double temp) const {

    return 1000.0 * (1.00525 + 1.09582E-05 * temp + 4.60017E-07 * temp * temp);
}

double TransCal::hExt(double resanul, double diaRef) {
    double Nu3;
    double h3;

    double reyE;
    double rug = geom.rug / geom.dia;

    int nflui = 2;
    for (int i = 0; i < geom.ncamadas; i++)
        if (geom.tipomat[i] != 0)
            nflui++;
    Vcr<double> prT(nflui);
    if (formacPoc == 0)
        prT[nflui - 1] = Pr(viscextern1 / rhoextern1, kextern1 / (rhoextern1 * cpextern1));
    else
        prT[nflui - 1] = 0;

    if (ambext == 1 && formacPoc == 0) {
        double tmed = 0.5 * (Textern1 + Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]]);
        viscextern1 = VisLiq(tmed) * 1e-3;
        rhoextern1 = MasEspLiq(tmed);
        kextern1 = CondLiq(tmed);
        cpextern1 = CalorLiq(tmed);
        betext = beta(tmed, 1);
    } else if (ambext == 2 && formacPoc == 0) {
        double tmed = 0.5 * (Textern1 + Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]]);
        viscextern1 = VisAr(tmed) * 1e-3;
        rhoextern1 = MasEspAr(tmed);
        kextern1 = CondAr(tmed);
        cpextern1 = CalorAr(tmed);
        betext = beta(tmed, 3);
    }

    if (formacPoc == 0) {
        double dE;

        if (dimExt < 0 && diaRef <= 0)
            dE = geom.diamC[geom.ncamadas - 1];
        else if (diaRef <= 0)
            dE = dimExt;
        else
            dE = diaRef;
        reyE = ReyIn(Vextern1, rhoextern1 * viscextern1 / rhoextern1, rhoextern1, dE);
        if (dirconvExt == 0)
            Nu3 = nussChuBer(reyE, prT[nflui - 1]);
        else
            Nu3 = nussPet(reyE, prT[nflui - 1], rug, viscextern1, 1. / 1e3);
        h3 = Nu3 * kextern1 / dE;

        nuse = Nu3;
        reye = reyE;
        he = h3;
        pre = prT[nflui - 1];
    } else {
        if (diaRef <= 0)
            h3 = 1 / (M_PI * geom.diamC[geom.ncamadas - 1] * resFim);
        else
            h3 = 1 / (M_PI * diaRef * resFim);
    }
    return h3;
}

double TransCal::hInt(double resanul, double diaRef) {
    double Nu1;
    double h1;

    double reyI;
    double rug = geom.rug / geom.dia;
    if (diaRef > 0)
        rug = geom.rug / diaRef;

    int nflui = 2;
    for (int i = 0; i < geom.ncamadas; i++)
        if (geom.tipomat[i] != 0)
            nflui++;
    Vcr<double> prT(nflui);

    prT[0] = Pr(viscint / rhoint, kint / (rhoint * cpint));
    definePet(Tint, Textern1);
    if (diaRef < 0)
        reyi = reyI = ReyIn(Vint, viscint, rhoint, geom.dia);
    else
        reyi = reyI = ReyIn(Vint, viscint, rhoint, diaRef);
    pri = prT[0];

    if (reyI > 2400)
        Nu1 = nussPet(reyI, prT[0], rug, viscint, 1. / 1e3);
    else {
        double difus = kint / (cpint * rhoint);
        double viscinem = viscint / rhoint;
        double pr = viscinem / difus;
        double rai = RaInt(fabs(Tint - Tini[0][0]), betint, viscinem, difus);

        grashi = rai / pr;

        if (rai < 1000. || (reyI > 1e-15 && (rai * difus / viscinem) / (reyI * reyI) < 1.))
            Nu1 = 3.6;
        else {
            Nu1 = 0.22 * pow(pr * rai / (0.2 * pr), 0.28) * pow(3.5, -0.25);
            if (diaRef < 0)
                Nu1 *= (geom.a / (0.2854 * geom.a));
            else
                Nu1 *= (diaRef / (0.2854 * diaRef));
        }
    }

    nusi = Nu1;
    if (diaRef < 0)
        h1 = Nu1 * kint / geom.a;
    else
        h1 = Nu1 * kint / diaRef;
    hi = h1;

    return h1;
}

double TransCal::condParede(double resanul) {
    double lconv = 1000.;
    double graA;
    double raA;
    double Nu2;
    double h2;
    double condParedeGlob;

    int nflui = 2;
    for (int i = 0; i < geom.ncamadas; i++)
        if (geom.tipomat[i] != 0)
            nflui++;
    Vcr<double> prT(nflui);
    double rug = geom.rug / geom.dia;

    prT[0] = 0.;
    prT[nflui - 1] = 0;
    if (nflui > 2) {
        int j = 1;
        for (int i = 0; i < geom.ncamadas; i++) {
            if (geom.tipomat[i] != 0) {
                if (geom.tipomat[i] == 2) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = VisLiq(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspLiq(tmed);
                    geom.cond[i] = CondLiq(tmed);
                    geom.cp[i] = CalorLiq(tmed);
                    geom.beta[i] = beta(tmed, 1);
                } else if (geom.tipomat[i] == 3) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = ViscGas(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspGas(tmed);
                    geom.cond[i] = CondGas(tmed);
                    geom.cp[i] = CalorGas(tmed);
                    geom.beta[i] = beta(tmed, 2);
                }
                prT[j] = Pr(geom.visc[i] / geom.rhoC[i], geom.cond[i] / (geom.rhoC[i] * geom.cp[i]));
                j++;
            }
        }
    }

    int limiter = 1;
    Vcr<double> tec(geom.ncamadas + 2);
    tec[0] = 0.;
    tec[geom.ncamadas + 1] = 0.;

    for (int i = 0; i < limiter; i++) {
        int kontaflu = 0;
        for (int j = 0; j < geom.ncamadas; j++) {
            if (geom.tipomat[j] != 0 && fabs(Vconf) < 1e-3) {
                kontaflu++;
                graA = Grash(Tcamada[j][0] - Tcamada[j][ncamada[j]], geom.beta[j],
                             geom.visc[j] / geom.rhoC[j], 0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                raA = Ra(graA, prT[kontaflu]);
                defineConf(prT[kontaflu], raA);
                Nu2 = NussConf2(raA, lconv, 0.5 * (geom.diamC[j] - geom.diamC[j - 1]), geom.teta);
                h2 = Nu2 * geom.cond[j] / (0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                tec[j + 1] = 0.5 * (geom.diamC[j] - geom.diamC[j - 1]) * h2 / log(geom.diamC[j] / geom.diamC[j - 1]);
            } else if (geom.tipomat[j] != 0 && fabs(Vconf) > 1e-3) {

                double a = geom.diamC[j];
                double b = geom.diamC[j - 1];
                double area = M_PI * (a * a - b * b) / 4.;
                double peri = M_PI * (a + b);
                double dia = 4. * area / peri;
                double reyConf = ReyIn(Vconf, geom.visc[j], geom.rhoC[j], dia);
                Nu2 = nussPet(reyConf, prT[kontaflu], rug, geom.visc[j], 1. / 1e3);
                double h2a = Nu2 * geom.cond[j] / a;
                double h2b = Nu2 * geom.cond[j] / b;
                h2 = 1. / ((b / a) * (1 / h2a) + (1 / h2b));
                tec[j + 1] = 0.5 * b * h2;
            } else {
                if (j > 0)
                    tec[j + 1] = geom.cond[j] / log(geom.diamC[j] / geom.diamC[j - 1]);
                else
                    tec[j + 1] = geom.cond[j] / log(geom.diamC[j] / geom.a);
            }
        }

        condParedeGlob = 0.;
        for (int k = 0; k < geom.ncamadas; k++)
            condParedeGlob += 1 / tec[k + 1];
    }

    return 1 / condParedeGlob;
}

double TransCal::condParedeLocal(double resanul) {
    double lconv = 1000.;
    double graA;
    double raA;
    double Nu2;
    double h2;
    double condParedeLoc;

    int nflui = 2;
    for (int i = 0; i < geom.ncamadas; i++)
        if (geom.tipomat[i] != 0)
            nflui++;
    Vcr<double> prT(nflui);
    double rug = geom.rug / geom.dia;

    prT[0] = 0.;
    prT[nflui - 1] = 0;
    if (nflui > 2) {
        int j = 1;
        for (int i = 0; i < geom.ncamadas; i++) {
            if (geom.tipomat[i] != 0) {
                if (geom.tipomat[i] == 2) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = VisLiq(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspLiq(tmed);
                    geom.cond[i] = CondLiq(tmed);
                    geom.cp[i] = CalorLiq(tmed);
                    geom.beta[i] = beta(tmed, 1);
                } else if (geom.tipomat[i] == 3) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = ViscGas(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspGas(tmed);
                    geom.cond[i] = CondGas(tmed);
                    geom.cp[i] = CalorGas(tmed);
                    geom.beta[i] = beta(tmed, 2);
                }
                prT[j] = Pr(geom.visc[i] / geom.rhoC[i], geom.cond[i] / (geom.rhoC[i] * geom.cp[i]));
                j++;
            }
        }
    }

    int limiter = 1;
    Vcr<double> tec(geom.ncamadas + 2);
    tec[0] = 0.;
    tec[geom.ncamadas + 1] = 0.;

    for (int i = 0; i < limiter; i++) {
        int kontaflu = 0;
        for (int j = 0; j < geom.ncamadas; j++) {
            if (geom.tipomat[j] != 0 && fabs(Vconf) < 1e-3) {
                kontaflu++;
                graA = Grash(Tcamada[j][0] - Tcamada[j][ncamada[j]], geom.beta[j],
                             geom.visc[j] / geom.rhoC[j], 0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                raA = Ra(graA, prT[kontaflu]);
                defineConf(prT[kontaflu], raA);
                Nu2 = NussConf2(raA, lconv, 0.5 * (geom.diamC[j] - geom.diamC[j - 1]), geom.teta);
                h2 = Nu2 * geom.cond[j] / (0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                tec[j + 1] = 0.5 * (geom.diamC[j] - geom.diamC[j - 1]) * h2 / log(geom.diamC[j] / geom.diamC[j - 1]);
            } else if (geom.tipomat[j] != 0 && fabs(Vconf) > 1e-3) {

                double a = geom.diamC[j];
                double b = geom.diamC[j - 1];
                double area = M_PI * (a * a - b * b) / 4.;
                double peri = M_PI * (a + b);
                double dia = 4. * area / peri;
                double reyConf = ReyIn(Vconf, geom.visc[j], geom.rhoC[j], dia);
                Nu2 = nussPet(reyConf, prT[kontaflu], rug, geom.visc[j], 1. / 1e3);
                double h2a = Nu2 * geom.cond[j] / a;
                double h2b = Nu2 * geom.cond[j] / b;
                h2 = 1. / ((b / a) * (1 / h2a) + (1 / h2b));
                tec[j + 1] = 0.5 * b * h2;
            } else {
                double dMed;
                if (j > 0) {
                    dMed = 0.5 * (geom.diamC[j] + geom.diamC[j - 1]) / geom.diamC[geom.ncamadas - 1];
                    tec[j + 1] = 2. * geom.cond[j] * dMed / (geom.diamC[j] - geom.diamC[j - 1]);
                } else {
                    dMed = 0.5 * (geom.diamC[j] + geom.a) / geom.diamC[geom.ncamadas - 1];
                    tec[j + 1] = 2. * geom.cond[j] * dMed / (geom.diamC[j] - geom.a);
                }
            }
        }

        condParedeLoc = 0.;
        for (int k = 0; k < geom.ncamadas; k++)
            condParedeLoc += 1 / tec[k + 1];
    }

    return 1 / condParedeLoc;
}

double TransCal::transperm(double resanul) {

    if (difus2D == 0) {
        double lconv = 1000.;
        double graA;
        double raA;
        double Nu1;
        double Nu2;
        double Nu3 = 0.;
        double h1;
        double h2;
        double h3;
        double cal;

        double reyE = 0.;
        double prE = 0.;
        double reyI;
        double rug = geom.rug / geom.dia;

        int nflui = 2;
        for (int i = 0; i < geom.ncamadas; i++)
            if (geom.tipomat[i] != 0)
                nflui++;
        Vcr<double> prT(nflui);

        if (ambext == 1 && formacPoc == 0) {
            double tmed = 0.5 * (Textern1 + Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]]);
            viscextern1 = VisLiq(tmed) * 1e-3;
            rhoextern1 = MasEspLiq(tmed);
            kextern1 = CondLiq(tmed);
            cpextern1 = CalorLiq(tmed);
            betext = beta(tmed, 1);
        } else if (ambext == 2 && formacPoc == 0) {
            double tmed = 0.5 * (Textern1 + Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]]);
            viscextern1 = VisAr(tmed) * 1e-3;
            rhoextern1 = MasEspAr(tmed);
            kextern1 = CondAr(tmed);
            cpextern1 = CalorAr(tmed);
            betext = beta(tmed, 3);
        }

        if (condiTparede == 0) {
            prT[0] = Pr(viscint / rhoint, kint / (rhoint * cpint));
            if (formacPoc == 0)
                prT[nflui - 1] = Pr(viscextern1 / rhoextern1, kextern1 / (rhoextern1 * cpextern1));
            else
                prT[nflui - 1] = 0;
            if (nflui > 2) {
                int j = 1;
                for (int i = 0; i < geom.ncamadas; i++) {
                    if (geom.tipomat[i] != 0) {
                        if (geom.tipomat[i] == 2) {
                            double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                            geom.visc[i] = VisLiq(tmed) * 1e-3;
                            geom.rhoC[i] = MasEspLiq(tmed);
                            geom.cond[i] = CondLiq(tmed);
                            geom.cp[i] = CalorLiq(tmed);
                            geom.beta[i] = beta(tmed, 1);
                        } else if (geom.tipomat[i] == 3) {
                            double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                            geom.visc[i] = ViscGas(tmed) * 1e-3;
                            geom.rhoC[i] = MasEspGas(tmed);
                            geom.cond[i] = CondGas(tmed);
                            geom.cp[i] = CalorGas(tmed);
                            geom.beta[i] = beta(tmed, 2);
                        }
                        prT[j] = Pr(geom.visc[i] / geom.rhoC[i], geom.cond[i] / (geom.rhoC[i] * geom.cp[i]));
                        j++;
                    }
                }
            }

            definePet(Tint, Textern1);
            reyi = reyI = ReyIn(Vint, viscint, rhoint, geom.dia);
            pri = prT[0];

            if (reyI > 2400)
                Nu1 = nussPet(reyI, prT[0], rug, viscint, 1. / 1e3);
            else {
                double difus = kint / (cpint * rhoint);
                double viscinem = viscint / rhoint;
                double pr = viscinem / difus;
                double rai = RaInt(fabs(Tint - Tini[0][0]), betint, viscinem, difus);

                grashi = rai / pr;

                if (rai < 1000. || (reyI > 1e-15 && (rai * difus / viscinem) / (reyI * reyI) < 1.))
                    Nu1 = 3.6;
                else {
                    Nu1 = 0.22 * pow(pr * rai / (0.2 * pr), 0.28) * pow(3.5, -0.25);
                    Nu1 *= (geom.a / (0.2854 * geom.a));
                }
            }

            nusi = Nu1;
            h1 = Nu1 * kint / geom.a;
            hi = h1;
        } else
            hi = h1 = 10000000. * kint / geom.a;
        if (formacPoc == 0) {
            reyE = ReyIn(Vextern1, rhoextern1 * viscextern1 / rhoextern1, rhoextern1, geom.diamC[geom.ncamadas - 1]);
            prE = prT[nflui - 1];
            if (dirconvExt == 0)
                Nu3 = nussChuBer(reyE, prT[nflui - 1]);
            else
                Nu3 = nussPet(reyE, prT[nflui - 1], rug, viscextern1, 1. / 1e3);
            h3 = Nu3 * kextern1 / geom.diamC[geom.ncamadas - 1];

            nuse = Nu3;
            reye = reyE;
            he = h3;
            pre = prT[nflui - 1];
        } else {
            h3 = 1 / (M_PI * geom.diamC[geom.ncamadas - 1] * resFim);
        }

        int limiter = 1;
        if (dirconvExt == 0 && formacPoc == 0 && reyE <= 5000)
            limiter = 5;
        if (nflui > 2)
            limiter = 5;
        Vcr<double> tec(geom.ncamadas + 2);
        tec[0] = M_PI * geom.a * h1;
        if (dirconvExt == 1 || formacPoc == 1 || reyE > 5000)
            tec[geom.ncamadas + 1] = M_PI * geom.diamC[geom.ncamadas - 1] * h3;

        for (int i = 0; i < limiter; i++) {
            int kontaflu = 0;
            if (dirconvExt == 0 && formacPoc == 0 && reyE <= 5000)
                tec[geom.ncamadas + 1] = M_PI * geom.diamC[geom.ncamadas - 1] * h3;
            for (int j = 0; j < geom.ncamadas; j++) {
                if (geom.tipomat[j] != 0 && fabs(Vconf) < 1e-3) {
                    kontaflu++;
                    graA = Grash(Tcamada[j][0] - Tcamada[j][ncamada[j]], geom.beta[j],
                                 geom.visc[j] / geom.rhoC[j], 0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                    raA = Ra(graA, prT[kontaflu]);
                    defineConf(prT[kontaflu], raA);
                    Nu2 = NussConf2(raA, lconv, 0.5 * (geom.diamC[j] - geom.diamC[j - 1]), geom.teta);
                    h2 = Nu2 * geom.cond[j] / (0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                    tec[j + 1] = M_PI * (geom.diamC[j] - geom.diamC[j - 1]) * h2 / log(geom.diamC[j] / geom.diamC[j - 1]);
                } else if (geom.tipomat[j] != 0 && fabs(Vconf) > 1e-3) {

                    double a = geom.diamC[j];
                    double b = geom.diamC[j - 1];
                    double area = M_PI * (a * a - b * b) / 4.;
                    double peri = M_PI * (a + b);
                    double dia = 4. * area / peri;
                    double reyConf = ReyIn(Vconf, geom.visc[j], geom.rhoC[j], dia);
                    Nu2 = nussPet(reyConf, prT[kontaflu], rug, geom.visc[j], 1. / 1e3);
                    double h2a = Nu2 * geom.cond[j] / a;
                    double h2b = Nu2 * geom.cond[j] / b;
                    h2 = 1. / ((b / a) * (1 / h2a) + (1 / h2b));
                    tec[j + 1] = M_PI * b * h2;
                } else {
                    if (j > 0)
                        tec[j + 1] = 2 * M_PI * geom.cond[j] / log(geom.diamC[j] / geom.diamC[j - 1]);
                    else
                        tec[j + 1] = 2 * M_PI * geom.cond[j] / log(geom.diamC[j] / geom.a);
                }
            }

            resGlob = 1 / tec[0] + 1 / tec[geom.ncamadas + 1];
            for (int k = 0; k < geom.ncamadas; k++)
                resGlob += 1 / tec[k + 1];
            cal = (Textern1 - Tint) / (resGlob + resanul);
            Tini[0][0] = Tcamada[0][0];
            Tcamada[0][0] = Tint + cal / tec[0];
            for (int k = 1; k < geom.ncamadas; k++) {
                Tini[k][0] = Tcamada[k][0];
                Tcamada[k][0] = Tcamada[k - 1][0] + cal / tec[k];
                Tini[k - 1][ncamada[k - 1]] = Tcamada[k - 1][ncamada[k - 1]];
                Tcamada[k - 1][ncamada[k - 1]] = Tcamada[k][0];
            }
            Tini[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]] = Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]];
            Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]] = Tcamada[geom.ncamadas - 1][0] + cal / tec[geom.ncamadas];
            if (dirconvExt == 0 && formacPoc == 0 && reyE <= 5000) {
                double dteta = fabs(Textern1 - Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]]);
                double rae = RaExt(dteta, betext, viscextern1 / rhoextern1, kextern1 / (rhoextern1 * cpextern1));
                double grase = rae / prE;
                double nusN;
                if (reye < 1e-15 || grase / (reye * reye) > 0.9) {
                    nusN = nussNatExt(rae, prE);
                } else
                    nusN = 0.;
                h3 = (Nu3 + nusN) * kextern1 / geom.diamC[geom.ncamadas - 1];
                nuse = Nu3 + nusN;
            }
        }
        for (int i = 0; i < geom.ncamadas; i++) {
            if (ncamada[i] > 1) {
                for (int j = 1; j < ncamada[i]; j++) {
                    Tini[i][j] = Tcamada[i][j];
                    Tcamada[i][j] = Tcamada[i][0] + (j / ncamada[i]) * (Tcamada[i][ncamada[i]] - Tcamada[i][0]);
                }
            }
        }
        fluxIni = cal;
        fluxFim = cal;
        for (int i = 0; i < lenth; i++) {
            for (int j = 0; j < ncamada[i] + 1; j++) {
                Qcamada[i][j] = cal;
                Qini[i][j] = Qcamada[i][j];
            }
        }

        return cal;
    } else { // Burial
        double cal;
        cal = transperm2D();
        return cal;
    }
}

void TransCal::transcel(int icam, int idisc) {
    double lconv = 12.19;
    const int lastLayer = geom.ncamadas - 1;
    const int layerDiscCount = ncamada[icam];
    const int lastDisc = ncamada[lastLayer];

    double rho = 0.;
    double cp = 0.;
    double k1 = 0.;

    double tec1 = 0.;
    double h0;
    double h1 = 0.;

    double r0 = 0.;
    double r1 = 0.;
    double r2 = 0.;

    for (int i = 0; i < 2; i++) {
        localvet[i] = 0.;
        for (int j = 0; j < 6; j++)
            localmat[i][j] = 0.;
    }

    if (ambext == 1) {
        double tmed = Tcamada[lastLayer][lastDisc];
        viscextern1 = VisLiq(tmed) * 1e-3;
        rhoextern1 = MasEspLiq(tmed);
        kextern1 = CondLiq(tmed);
        cpextern1 = CalorLiq(tmed);
        betext = beta(tmed, 1);
    } else if (ambext == 2 && formacPoc == 0) {
        double tmed = 0.5 * (Textern1 + Tcamada[lastLayer][lastDisc]);
        viscextern1 = VisAr(tmed) * 1e-3;
        rhoextern1 = MasEspAr(tmed);
        kextern1 = CondAr(tmed);
        cpextern1 = CalorAr(tmed);
        betext = beta(tmed, 3);
    }

    if (icam > 0 || (icam == 0 && idisc > 0)) {

        if (icam > 0) {
            r0 = geom.diamC[icam - 1] / 2. + (idisc - 1) * drcamada[icam];
            r1 = geom.diamC[icam - 1] / 2. + idisc * drcamada[icam];
            if (idisc < layerDiscCount)
                r2 = geom.diamC[icam - 1] / 2. + (idisc + 1) * drcamada[icam];
            else if (icam < lastLayer)
                r2 = geom.diamC[icam] / 2. + drcamada[icam + 1];
            else
                r2 = r1;
        } else {
            r0 = geom.a / 2. + (idisc - 1) * drcamada[0];
            r1 = geom.a / 2. + idisc * drcamada[0];
            if (idisc < ncamada[0])
                r2 = geom.a / 2. + (idisc + 1) * drcamada[0];
            else
                r2 = geom.diamC[0] / 2. + drcamada[1];
        }
        rho = geom.rhoC[icam];
        cp = geom.cp[icam];
        if (idisc < layerDiscCount)
            k1 = geom.cond[icam];
        else if (icam < lastLayer)
            k1 = geom.cond[icam + 1];
        else
            k1 = geom.cond[icam];

        double graA;
        double raA;
        double prT;
        double Nu2;
        if (icam < lastLayer - 1 && idisc == layerDiscCount && geom.tipomat[icam + 1] != 0) {
            const int nextCam = icam + 1;
            if (geom.tipomat[nextCam] == 2) {
                double tmed = 0.5 * (Tcamada[nextCam][0] + Tcamada[nextCam][1]);
                geom.visc[nextCam] = VisLiq(tmed) * 1e-3;
                geom.rhoC[nextCam] = MasEspLiq(tmed);
                geom.cond[nextCam] = CondLiq(tmed);
                geom.cp[nextCam] = CalorLiq(tmed);
                geom.beta[nextCam] = beta(tmed, 1);
            } else if (geom.tipomat[nextCam] == 3) {
                double tmed = 0.5 * (Tcamada[nextCam][0] + Tcamada[nextCam][1]);
                geom.visc[nextCam] = ViscGas(tmed) * 1e-3;
                geom.rhoC[nextCam] = MasEspGas(tmed);
                geom.cond[nextCam] = CondGas(tmed);
                geom.cp[nextCam] = CalorGas(tmed);
                geom.beta[nextCam] = beta(tmed, 1);
            }
            if (fabs(Vconf) < 1e-3) {
                const double viscRatio = geom.visc[nextCam] / geom.rhoC[nextCam];
                const double condRatio = geom.cond[nextCam] / (geom.rhoC[nextCam] * geom.cp[nextCam]);
                const double diamGap = geom.diamC[nextCam] - geom.diamC[icam];
                const double halfGap = 0.5 * diamGap;
                prT = Pr(viscRatio, condRatio);
                graA = Grash(Tcamada[nextCam][0] - Tcamada[nextCam][1], geom.beta[nextCam],
                             viscRatio, halfGap);
                raA = Ra(graA, prT);
                defineConf(prT, raA);
                Nu2 = NussConf2(raA, lconv, halfGap, geom.teta);
                h0 = Nu2 * geom.cond[nextCam] / halfGap;
                tec1 = M_PI * diamGap * h0 / log(geom.diamC[nextCam] / geom.diamC[icam]);
                k1 = 0.;
            } else {
                const double viscRatio = geom.visc[nextCam] / geom.rhoC[nextCam];
                const double condRatio = geom.cond[nextCam] / (geom.rhoC[nextCam] * geom.cp[nextCam]);
                double a = geom.diamC[nextCam];
                double b = geom.diamC[icam];
                double area = M_PI * (a * a - b * b) / 4.;
                double peri = M_PI * (a + b);
                double dia = 4. * area / peri;
                prT = Pr(viscRatio, condRatio);
                double reyConf = ReyIn(Vconf, geom.visc[nextCam], geom.rhoC[nextCam], dia);
                Nu2 = nussPet(reyConf, prT, geom.rug, geom.visc[nextCam], 1. / 1e3);
                double h2a = Nu2 * geom.cond[nextCam] / a;
                double h2b = Nu2 * geom.cond[nextCam] / b;
                h0 = 1. / ((b / a) * (1 / h2a) + (1 / h2b));
                tec1 = M_PI * b * h0;
                k1 = 0.;
            }
        }
    }
    if (icam == 0 && idisc == 0 && condiTparede == 0) {
        r0 = geom.a / 2.;
        r1 = geom.a / 2. + drcamada[0];
        definePet(Tint, Tcamada[0][1]);
        double reyI = ReyIn(Vint, viscint, rhoint, geom.dia);
        double prT = Pr(viscint / rhoint, kint / (rhoint * cpint));
        double rug = geom.rug / geom.dia;
        reyi = reyI;
        pri = prT;
        double Nu1;
        if (reyI > 2400)
            Nu1 = nussPet(reyI, prT, rug, viscint, 1. / 1e3);
        else {
            if (geom.revest == 0) {
                double difus = kint / (cpint * rhoint);
                double viscinem = viscint / rhoint;
                double pr = viscinem / difus;
                double raI = RaInt(fabs(Tint - Tini[0][0]), betint, viscinem, difus);
                grashi = raI / pri;

                if (raI < 1000. || (reyI > 1e-15 && (raI * difus / viscinem) / (reyI * reyI) < 1.))
                    Nu1 = 3.6;
                else {
                    Nu1 = 0.22 * pow(pr * raI / (0.2 * pr), 0.28) * pow(3.5, -0.25);
                }
            } else {
                double graI = Grash(fabs(Tint2 - Tcamada[0][0]), betint,
                                    viscint / rhoint, 0.5 * (geom.a - geom.b));
                double raI = Ra(graI, prT);

                pri = prT;
                grashi = graI;

                if (raI < 1000. || (reyI > 1e-15 && (graI) / (reyI * reyI) < 1.))
                    Nu1 = 3.6;
                else {
                    defineConf(prT, raI);
                    Nu1 = NussConf2(raI, lconv, 0.5 * (geom.a + geom.b), geom.teta);
                    Nu1 = 1 * Nu1 / (0.5 * log(geom.a / geom.b));
                }
            }
        }
        nusi = Nu1;
        h1 = Nu1 * kint / geom.a;
        hi = h1;
    } else if (icam == 0 && idisc == 0 && condiTparede == 1) {
        if (novoHi == 0)
            hi = h1 = 50000. * geom.cond[0] * (geom.a / 2.) / drcamada[0];
        else
            hi = h1 = novoHi;
    }

    if (icam == lastLayer && idisc == layerDiscCount) {
        if (formacPoc == 0) {
            definePet(Tcamada[lastLayer][lastDisc - 1], Textern1);
            double prT = Pr(viscextern1 / rhoextern1, kextern1 / (rhoextern1 * cpextern1));
            double reyE;
            if (coluna == 0)
                reyE = ReyIn(Vextern1, rhoextern1 * viscextern1 / rhoextern1, rhoextern1, geom.diamC[lastLayer]);
            else
                reyE = ReyIn(Vextern1, rhoextern1 * viscextern1 / rhoextern1, rhoextern1, colunaDia);
            double rug;
            if (coluna == 0)
                rug = geom.rug / geom.diamC[lastLayer];
            else
                rug = geom.rug / colunaDia;
            double Nu1;
            reye = reyE;
            pre = prT;
            if (dirconvExt == 0) {
                Nu1 = nussChuBer(reyE, prT);
                if (reyE <= 5000) {
                    double dteta = fabs(Textern1 - Tcamada[lastLayer][lastDisc]);
                    double rae = RaExt(dteta, betext, viscextern1 / rhoextern1, kextern1 / (rhoextern1 * cpextern1));
                    double grase = rae / prT;
                    double nusN;
                    if (reye < 1e-15 || grase / (reye * reye) > 0.9) {
                        nusN = nussNatExt(rae, prT);
                    } else
                        nusN = 0.;
                    Nu1 += nusN;
                }
            } else {
                if (reyE > 2400)
                    Nu1 = nussPet(reyE, prT, rug, viscextern1, 1. / 1e3);
                else if (coluna == 0)
                    Nu1 = 3.6;
                else {
                    double graE = Grash(fabs(Tcamada[lastLayer][lastDisc - 1] - Textern2),
                                        betext, viscextern1 / rhoextern1, 0.5 * (geom.b - geom.diamC[lastLayer]));
                    grashe = graE;
                    double raE = Ra(graE, prT);
                    if (raE < 1000. || (reyE > 1e-15 && graE / (reyE * reyE) < 1.))
                        Nu1 = 3.6;
                    else {
                        defineConf(prT, raE);
                        Nu1 = NussConf2(raE, lconv, 0.5 * (geom.diamC[lastLayer] + geom.b), geom.teta);
                        Nu1 = Nu1 / (0.5 * log(geom.b / geom.diamC[lastLayer]));
                    }
                }
            }
            h1 = Nu1 * kextern1 / geom.diamC[lastLayer];

            nuse = Nu1;
            he = h1;
        } else
            h1 = 1 / (M_PI * geom.diamC[lastLayer] * resFim);
    }

    if (icam > 0 || (icam == 0 && idisc > 0)) {
        localmat[0][1] = (1 / (0.5 * (r1 + r0))) / (r1 - r0);
        localmat[0][2] = (rho * cp) / dt;
        localmat[0][3] = -(1 / (0.5 * (r1 + r0))) / (r1 - r0);
        localvet[0] = Tcamada[icam][idisc] * (rho * cp) / dt;
        if (icam < lastLayer || (icam == lastLayer && idisc < layerDiscCount)) {
            double ciL;
            ciL = (tec1 / (2. * M_PI) + 0.5 * k1 * (r1 + r2) / (r2 - r1));
            localmat[1][0] = 0;
            localmat[1][2] = ciL;
            localmat[1][3] = 1.;
            localmat[1][4] = -ciL;
        } else {
            localmat[1][2] = h1 * r1;
            localmat[1][3] = 1.;
            localvet[1] = h1 * r1 * Textern1;
        }
    } else {
        localmat[0][2] = -(h1 * (geom.a / 2.) + geom.cond[0] * (geom.a / 2.) / drcamada[0]);
        localmat[0][4] = geom.cond[0] * (geom.a / 2.) / drcamada[0];
        localmat[1][2] = -h1 * geom.a / 2.;
        localmat[1][3] = 1.;
        localvet[0] = -h1 * (geom.a / 2.) * Tint;
        localvet[1] = -h1 * (geom.a / 2.) * Tint;
    }
}

double TransCal::transtrans() {
    if (difus2D == 0) {
        const int totalLayers = geom.ncamadas;
        const double twoPi = 2. * M_PI;

        double tintTemp = Tint;
        double texternTemp = Textern1;

        int nglob = 0;
        for (int i = 0; i < totalLayers; i++) {
            nglob += ncamada[i];
        }
        nglob = 2 * (nglob + 1);
        Vcr<double> vetliv(nglob);
        BandMtx<double> matglob(nglob, 3, 2);
        transcel(0, 0);
        vetliv[0] = localvet[0];
        vetliv[1] = localvet[1];
        matglob[0][0] = localmat[0][2];
        matglob[0][2] = localmat[0][4];
        matglob[1][-1] = localmat[1][2];
        matglob[1][0] = localmat[1][3];
        int konta = 2;
        for (int i = 0; i < totalLayers; i++) {
            for (int j = 1; j <= ncamada[i]; j++) {
                transcel(i, j);
                vetliv[konta] = localvet[0];
                vetliv[konta + 1] = localvet[1];
                matglob[konta][-1] = localmat[0][1];
                matglob[konta][0] = localmat[0][2];
                matglob[konta][1] = localmat[0][3];
                matglob[konta + 1][-3] = localmat[1][0];
                matglob[konta + 1][-1] = localmat[1][2];
                matglob[konta + 1][0] = localmat[1][3];
                if (konta + 1 <= nglob - 1)
                    matglob[konta + 1][1] = localmat[1][4];
                konta += 2;
            }
        }
        matglob.GaussElimPP(vetliv);

        Tini[0][0] = Tcamada[0][0];
        Tcamada[0][0] = vetliv[0];
        Qini[0][0] = Qcamada[0][0];
        Qcamada[0][0] = twoPi * vetliv[1];
        konta = 2;
        for (int i = 0; i < totalLayers; i++) {
            for (int j = 1; j <= ncamada[i]; j++) {
                Tini[i][j] = Tcamada[i][j];
                Tcamada[i][j] = vetliv[konta];
                Qini[i][j] = Qcamada[i][j];
                Qcamada[i][j] = twoPi * vetliv[konta + 1];
                konta += 2;
            }
            if (i < totalLayers - 1) {
                Tini[i + 1][0] = Tcamada[i + 1][0];
                Tcamada[i + 1][0] = Tcamada[i][ncamada[i]];
                Qini[i + 1][0] = Qcamada[i + 1][0];
                Qcamada[i + 1][0] = Qcamada[i][ncamada[i]];
            }
        }
        fluxIni = twoPi * vetliv[1];
        konta -= 2;
        fluxFim = twoPi * vetliv[konta + 1];

        Tint = tintTemp;
        Textern1 = texternTemp;

        return fluxIni;
    } else { // Burial
        double cal;
        cal = transtrans2D();
        return cal;
    }
}

double TransCal::transperm2D() {

    double lconv = 1000.;
    double graA;
    double raA;
    double h2;
    double Nu2;
    double rug = geom.rug / geom.dia;
    dimExt = poisson2D.dados.diamExt;

    int nflui = 2;
    for (int i = 0; i < geom.ncamadas; i++)
        if (geom.tipomat[i] != 0)
            nflui++;
    Vcr<double> prT(nflui);
    prT[0] = Pr(viscint / rhoint, kint / (rhoint * cpint));
    if (formacPoc == 0)
        prT[nflui - 1] = Pr(viscextern1 / rhoextern1, kextern1 / (rhoextern1 * cpextern1));
    else
        prT[nflui - 1] = 0;
    if (nflui > 2) {
        int j = 1;
        for (int i = 0; i < geom.ncamadas; i++) {
            if (geom.tipomat[i] != 0) {
                if (geom.tipomat[i] == 2) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = VisLiq(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspLiq(tmed);
                    geom.cond[i] = CondLiq(tmed);
                    geom.cp[i] = CalorLiq(tmed);
                    geom.beta[i] = beta(tmed, 1);
                } else if (geom.tipomat[i] == 3) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = ViscGas(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspGas(tmed);
                    geom.cond[i] = CondGas(tmed);
                    geom.cp[i] = CalorGas(tmed);
                    geom.beta[i] = beta(tmed, 2);
                }
                prT[j] = Pr(geom.visc[i] / geom.rhoC[i], geom.cond[i] / (geom.rhoC[i] * geom.cp[i]));
                j++;
            }
        }
    }

    poisson2D.dados.tInt = Tint;
    poisson2D.dados.tAmb = Textern1;
    poisson2D.dados.condGlob = condParede();
    poisson2D.dados.condLoc = condParedeLocal();
    poisson2D.dados.hE = he = hExt();
    poisson2D.dados.hI = hi = hInt();
    double deltFic = 1.;
    double qparede0 = 1e15;
    double erroParede = 1000.;
    for (int i = 0; i < poisson2D.dados.nele; i++) {
        poisson2D.malha.mlh2d[i].cel2D.tempC0 = poisson2D.malha.mlh2d[i].cel2D.tempCI =
            poisson2D.malha.mlh2d[i].cel2D.tempC = Textern1;
    }
    while (erroParede > 1e-1) {
        poisson2D.inicializaTransientePoisson();
        poisson2D.transientePoissonDummy(deltFic);
        deltFic *= 2.0;
        if (deltFic > 100.)
            deltFic = 100.;
        erroParede = fabs(poisson2D.dados.qTotal - qparede0);
        qparede0 = poisson2D.dados.qTotal;
    }
    poisson2D.inicializaPermanentePoisson();
    poisson2D.permanentePoisson();
    double cal = -poisson2D.dados.qTotal;
    int limiter = 1;
    Vcr<double> tec(geom.ncamadas + 2);
    tec[0] = M_PI * geom.a * hi;
    if (dirconvExt == 1 || formacPoc == 1)
        tec[geom.ncamadas + 1] = M_PI * geom.diamC[geom.ncamadas - 1] * he;

    for (int i = 0; i < limiter; i++) {
        int kontaflu = 0;
        if (dirconvExt == 0 && formacPoc == 0)
            tec[geom.ncamadas + 1] = M_PI * geom.diamC[geom.ncamadas - 1] * he;
        for (int j = 0; j < geom.ncamadas; j++) {
            if (geom.tipomat[j] != 0 && fabs(Vconf) < 1e-3) {
                kontaflu++;
                graA = Grash(Tcamada[j][0] - Tcamada[j][ncamada[j]], geom.beta[j],
                             geom.visc[j] / geom.rhoC[j], 0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                raA = Ra(graA, prT[kontaflu]);
                defineConf(prT[kontaflu], raA);
                Nu2 = NussConf2(raA, lconv, 0.5 * (geom.diamC[j] - geom.diamC[j - 1]), geom.teta);
                h2 = Nu2 * geom.cond[j] / (0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                tec[j + 1] = M_PI * (geom.diamC[j] - geom.diamC[j - 1]) * h2 / log(geom.diamC[j] / geom.diamC[j - 1]);
            } else if (geom.tipomat[j] != 0 && fabs(Vconf) > 1e-3) {

                double a = geom.diamC[j];
                double b = geom.diamC[j - 1];
                double area = M_PI * (a * a - b * b) / 4.;
                double peri = M_PI * (a + b);
                double dia = 4. * area / peri;
                double reyConf = ReyIn(Vconf, geom.visc[j], geom.rhoC[j], dia);
                Nu2 = nussPet(reyConf, prT[kontaflu], rug, geom.visc[j], 1. / 1e3);
                double h2a = Nu2 * geom.cond[j] / a;
                double h2b = Nu2 * geom.cond[j] / b;
                h2 = 1. / ((b / a) * (1 / h2a) + (1 / h2b));
                tec[j + 1] = M_PI * b * h2;
            } else {
                if (j > 0)
                    tec[j + 1] = 2 * M_PI * geom.cond[j] / log(geom.diamC[j] / geom.diamC[j - 1]);
                else
                    tec[j + 1] = 2 * M_PI * geom.cond[j] / log(geom.diamC[j] / geom.a);
            }
        }

        resGlob = 1 / tec[0] + 1 / tec[geom.ncamadas + 1];
        for (int k = 0; k < geom.ncamadas; k++)
            resGlob += 1 / tec[k + 1];
        Tini[0][0] = Tcamada[0][0];
        Tcamada[0][0] = Tint + cal / tec[0];
        for (int k = 1; k < geom.ncamadas; k++) {
            Tini[k][0] = Tcamada[k][0];
            Tcamada[k][0] = Tcamada[k - 1][0] + cal / tec[k];
            Tini[k - 1][ncamada[k - 1]] = Tcamada[k - 1][ncamada[k - 1]];
            Tcamada[k - 1][ncamada[k - 1]] = Tcamada[k][0];
        }
        Tini[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]] = Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]];
        Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]] = Tcamada[geom.ncamadas - 1][0] + cal / tec[geom.ncamadas];
    }
    for (int i = 0; i < geom.ncamadas; i++) {
        if (ncamada[i] > 1) {
            for (int j = 1; j < ncamada[i]; j++) {
                Tini[i][j] = Tcamada[i][j];
                Tcamada[i][j] = Tcamada[i][0] + (j / ncamada[i]) * (Tcamada[i][ncamada[i]] - Tcamada[i][0]);
            }
        }
    }
    fluxIni = cal;
    fluxFim = cal;
    return cal;
}

double TransCal::transtrans2D() {

    double lconv = 1000.;
    double graA;
    double raA;
    double h2;
    double Nu2;
    double rug = geom.rug / geom.dia;
    dimExt = poisson2D.dados.diamExt;

    if ((*vg1dSP).lixo5 < 1e-15) {
        if (poisson2D.dados.temp.perm == 1 && poisson2D.dados.temp.trans == 1) {
            for (int i = 0; i < poisson2D.malha.nele; i++) {
                poisson2D.malha.mlh2d[i].perm = 0;
                poisson2D.malha.mlh2d[i].trans = 1;
            }

            poisson2D.kontaTempo = 0;
            poisson2D.dados.tInt0 = poisson2D.dados.tInt;
        }
    }

    int nflui = 2;
    for (int i = 0; i < geom.ncamadas; i++)
        if (geom.tipomat[i] != 0)
            nflui++;
    Vcr<double> prT(nflui);
    prT[0] = Pr(viscint / rhoint, kint / (rhoint * cpint));
    if (formacPoc == 0)
        prT[nflui - 1] = Pr(viscextern1 / rhoextern1, kextern1 / (rhoextern1 * cpextern1));
    else
        prT[nflui - 1] = 0;
    if (nflui > 2) {
        int j = 1;
        for (int i = 0; i < geom.ncamadas; i++) {
            if (geom.tipomat[i] != 0) {
                if (geom.tipomat[i] == 2) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = VisLiq(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspLiq(tmed);
                    geom.cond[i] = CondLiq(tmed);
                    geom.cp[i] = CalorLiq(tmed);
                    geom.beta[i] = beta(tmed, 1);

                } else if (geom.tipomat[i] == 3) {
                    double tmed = 0.5 * (Tcamada[i][0] + Tcamada[i][1]);
                    geom.visc[i] = ViscGas(tmed) * 1e-3;
                    geom.rhoC[i] = MasEspGas(tmed);
                    geom.cond[i] = CondGas(tmed);
                    geom.cp[i] = CalorGas(tmed);
                    geom.beta[i] = beta(tmed, 2);
                }
                prT[j] = Pr(geom.visc[i] / geom.rhoC[i], geom.cond[i] / (geom.rhoC[i] * geom.cp[i]));
                j++;
            }
        }
    }

    poisson2D.dados.tInt = Tint;
    poisson2D.dados.tAmb = Textern1;
    poisson2D.dados.condGlob = condParede();
    poisson2D.dados.condLoc = condParedeLocal();
    poisson2D.dados.hE = he = hExt();
    poisson2D.dados.hI = hi = hInt();
    for (int i = 0; i < poisson2D.dados.CC.nRic; i++) {
        for (int j = 0; j < poisson2D.dados.CC.ccRic[i].nserie; j++) {
            poisson2D.dados.CC.ccRic[i].valAmb[j] = poisson2D.dados.tAmb;
            poisson2D.dados.CC.ccRic[i].hAmb[j] = poisson2D.dados.hE;
        }
    }
    poisson2D.transientePoisson(dt);
    double cal = -poisson2D.dados.qTotal;

    int limiter = 1;
    Vcr<double> tec(geom.ncamadas + 2);
    tec[0] = M_PI * geom.a * hi;
    if (dirconvExt == 1 || formacPoc == 1)
        tec[geom.ncamadas + 1] = M_PI * geom.diamC[geom.ncamadas - 1] * he;

    for (int i = 0; i < limiter; i++) {
        int kontaflu = 0;
        if (dirconvExt == 0 && formacPoc == 0)
            tec[geom.ncamadas + 1] = M_PI * geom.diamC[geom.ncamadas - 1] * he;
        for (int j = 0; j < geom.ncamadas; j++) {
            if (geom.tipomat[j] != 0 && fabs(Vconf) < 1e-3) {
                kontaflu++;
                graA = Grash(Tcamada[j][0] - Tcamada[j][ncamada[j]], geom.beta[j],
                             geom.visc[j] / geom.rhoC[j], 0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                raA = Ra(graA, prT[kontaflu]);
                defineConf(prT[kontaflu], raA);
                Nu2 = NussConf2(raA, lconv, 0.5 * (geom.diamC[j] - geom.diamC[j - 1]), geom.teta);
                h2 = Nu2 * geom.cond[j] / (0.5 * (geom.diamC[j] - geom.diamC[j - 1]));
                tec[j + 1] = M_PI * (geom.diamC[j] - geom.diamC[j - 1]) * h2 / log(geom.diamC[j] / geom.diamC[j - 1]);
            } else if (geom.tipomat[j] != 0 && fabs(Vconf) > 1e-3) {

                double a = geom.diamC[j];
                double b = geom.diamC[j - 1];
                double area = M_PI * (a * a - b * b) / 4.;
                double peri = M_PI * (a + b);
                double dia = 4. * area / peri;
                double reyConf = ReyIn(Vconf, geom.visc[j], geom.rhoC[j], dia);
                Nu2 = nussPet(reyConf, prT[kontaflu], rug, geom.visc[j], 1. / 1e3);
                double h2a = Nu2 * geom.cond[j] / a;
                double h2b = Nu2 * geom.cond[j] / b;
                h2 = 1. / ((b / a) * (1 / h2a) + (1 / h2b));
                tec[j + 1] = M_PI * b * h2;
            } else {
                if (j > 0)
                    tec[j + 1] = 2 * M_PI * geom.cond[j] / log(geom.diamC[j] / geom.diamC[j - 1]);
                else
                    tec[j + 1] = 2 * M_PI * geom.cond[j] / log(geom.diamC[j] / geom.a);
            }
        }

        resGlob = 1 / tec[0] + 1 / tec[geom.ncamadas + 1];
        for (int k = 0; k < geom.ncamadas; k++)
            resGlob += 1 / tec[k + 1];
        Tini[0][0] = Tcamada[0][0];
        Tcamada[0][0] = Tint + cal / tec[0];
        for (int k = 1; k < geom.ncamadas; k++) {
            Tini[k][0] = Tcamada[k][0];
            Tcamada[k][0] = Tcamada[k - 1][0] + cal / tec[k];
            Tini[k - 1][ncamada[k - 1]] = Tcamada[k - 1][ncamada[k - 1]];
            Tcamada[k - 1][ncamada[k - 1]] = Tcamada[k][0];
        }
        Tini[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]] = Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]];
        Tcamada[geom.ncamadas - 1][ncamada[geom.ncamadas - 1]] = Tcamada[geom.ncamadas - 1][0] + cal / tec[geom.ncamadas];
    }
    for (int i = 0; i < geom.ncamadas; i++) {
        if (ncamada[i] > 1) {
            for (int j = 1; j < ncamada[i]; j++) {
                Tini[i][j] = Tcamada[i][j];
                Tcamada[i][j] = Tcamada[i][0] + (j / ncamada[i]) * (Tcamada[i][ncamada[i]] - Tcamada[i][0]);
            }
        }
    }
    fluxIni = cal;
    fluxFim = cal;
    return cal;
}

void TransCal::FeiticoDoTempo() {
    Tcamada[0][0] = Tini[0][0];
    Qcamada[0][0] = Qini[0][0];
    int konta = 2;
    for (int i = 0; i < geom.ncamadas; i++) {
        for (int j = 1; j <= ncamada[i]; j++) {
            Tcamada[i][j] = Tini[i][j];
            Qcamada[i][j] = Qini[i][j];
            konta += 2;
        }
        if (i < geom.ncamadas - 1) {
            Tcamada[i + 1][0] = Tini[i + 1][0];
            Qcamada[i + 1][0] = Qini[i + 1][0];
        }
    }
    if (difus2D == 1)
        poisson2D.FeiticoDoTempo(); // Burial
}

FullMtx<double> TransCal::perfil() {
    FullMtx<double> matrizsaida(nglobal, 4);
    int konta = 1;
    matrizsaida[0][0] = geom.a / 2.;
    matrizsaida[0][1] = matrizsaida[0][0] * 100. / 2.54;
    matrizsaida[0][2] = Tcamada[0][0];
    matrizsaida[0][3] = Qcamada[0][0];
    for (int i = 0; i < geom.ncamadas; i++) {
        for (int j = 1; j <= ncamada[i]; j++) {
            if (i > 0)
                matrizsaida[konta][0] = geom.diamC[i - 1] / 2. + (j)*drcamada[i];
            else
                matrizsaida[konta][0] = geom.a / 2. + (j)*drcamada[i];
            matrizsaida[konta][1] = matrizsaida[konta][0] * 100. / 2.54;
            matrizsaida[konta][2] = Tcamada[i][j];
            matrizsaida[konta][3] = Qcamada[i][j];
            konta += 1;
        }
    }
    return matrizsaida;
}