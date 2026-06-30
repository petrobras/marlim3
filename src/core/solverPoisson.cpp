/*
 * solverPoisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */
#include "solverPoisson.h"

solverP::solverP(varGlob1D *Vvg1dSP, string nomeArquivoEntrada, double vCondGlob, double vCondLoc, double vhE, double vhInt,
                 double vTint, double vTamb, double vdiamI, double vdiamE, int vindcel) : dados(nomeArquivoEntrada, vCondGlob, vCondLoc, vhE, vhInt, vTint, vTamb, vdiamI, vdiamE),
                                                                                          termolivre(dados.nele), matglob(dados.nele, dados.noZero, dados.vecsra, dados.veclm, dados.vecfnz) {
    vg1dSP = Vvg1dSP;
    indCel = vindcel;
    malha = malha2d(vg1dSP, dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno, dados.temp.perm, dados.temp.trans);
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].CC = dados.CC;
        for (int j = 0; j < dados.prop.parserie; j++) {
            double xc = malha.mlh2d[i].cel2D.centroideElem[0];
            double yc = malha.mlh2d[i].cel2D.centroideElem[1];
            if ((xc <= dados.prop.xmax[j] && xc >= dados.prop.xmin[j]) && (yc <= dados.prop.ymax[j] && yc >= dados.prop.ymin[j])) {
                malha.mlh2d[i].cel2D.cond = dados.prop.cond[j];
                malha.mlh2d[i].cel2D.cp = dados.prop.cp[j];
                malha.mlh2d[i].cel2D.rho = dados.prop.rho[j];
                break;
            }
        }
        for (int j = 0; j < dados.CI.parserie; j++) {
            double xc = malha.mlh2d[i].cel2D.centroideElem[0];
            double yc = malha.mlh2d[i].cel2D.centroideElem[1];
            if ((xc <= dados.CI.xmax[j] && xc >= dados.CI.xmin[j]) && (yc <= dados.CI.ymax[j] && yc >= dados.CI.ymin[j])) {
                malha.mlh2d[i].cel2D.tempC = dados.CI.temp[j];
                malha.mlh2d[i].cel2D.tempC0 = dados.CI.temp[j];
                break;
            }
        }
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }

    int indsra = 0;
    for (int i = 0; i < malha.nele; i++) {
        int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
        matglob.fnz[i] = indsra;
        for (int j = 0; j < dimNoZero; j++) {
            int ind = malha.mlh2d[i].cel2D.indVizCres[j];
            matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            matglob.clm[indsra + j] = ind;
        }
        indsra += dimNoZero;
        termolivre[i] = malha.mlh2d[i].TL[0];
    }
    matglob.fnz[malha.nele] = dados.noZero;

    if (dados.rankLU >= 0) {
        matglob.rank = dados.rankLU;
        if (dados.rankLU > 0) {
            matglob.geraILUrank();
            matglob.sraILU = new double[matglob.noZeroIlu];
        }
        matglob.colorido = dados.colore;
        if (dados.colore)
            matglob.geraILUcor();
    }
    precn = 1;
    if (dados.rankLU >= 0)
        precn = 4;

    if (dados.acop == 1) {
        dados.hE = vhE;
        dados.hI = vhInt;
        dados.condGlob = vCondGlob;
        dados.condLoc = vCondLoc;
        dados.tAmb = vTamb;
        dados.tInt = vTint;
        dados.diamInt = vdiamI;
        dados.diamExt = vdiamE;
        dados.qDesacop = ((2 * M_PI - dados.angAcop) * (dados.tInt - dados.tAmb)) / (1. / dados.condGlob + 1. / (0.5 * dados.diamInt * dados.hI) +
                                                                                     1. / (0.5 * dados.diamExt * dados.hE));
        dados.qAcop = 0.;
        for (int i = 0; i < dados.CC.nRic; i++) {
            for (int j = 0; j < dados.CC.ccRic[i].nserie; j++) {
                dados.CC.ccRic[i].valAmb[j] = dados.tAmb;
                dados.CC.ccRic[i].hAmb[j] = dados.hE;
            }
        }
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    int kcc = 0;
                    while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                        kcc++;
                    if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                        malha.mlh2d[i].ccHR[j] = dados.condLoc;
                        dados.qAcop += malha.mlh2d[i].cel2D.sFaceMod[j] * (dados.tInt - malha.mlh2d[i].cel2D.tempC) / (1 / dados.condLoc + 1. / (dados.hI));
                    }
                }
            }
        }
        dados.qTotal = dados.qAcop + dados.qDesacop;
        dados.tParede = dados.tInt - dados.qTotal / (M_PI * dados.diamInt * dados.hI);
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    int kcc = 0;
                    while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                        kcc++;
                    if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                        malha.mlh2d[i].ccTambR[j] = dados.tParede;
                    }
                }
            }
        }
    }
    kontaTempo = 0;
}

solverP::solverP() : dados(), termolivre(1), matglob(1, 1) {
    vg1dSP = 0;
    indCel = 0;
    kontaTempo = 0;
    malha = malha2d();
    precn = 1;
}

solverP::solverP(const solverP &vPoisson) : dados(vPoisson.dados.entrada, vPoisson.dados.condGlob, vPoisson.dados.condLoc, vPoisson.dados.hE, vPoisson.dados.hI,
                                                  vPoisson.dados.tInt, vPoisson.dados.tAmb, vPoisson.dados.diamInt, vPoisson.dados.diamExt),
                                            termolivre(dados.nele), matglob(dados.nele, dados.noZero, dados.vecsra, dados.veclm, dados.vecfnz) {
    vg1dSP = vPoisson.vg1dSP;
    malha = malha2d(vg1dSP, dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno, dados.temp.perm, dados.temp.trans);
    indCel = vPoisson.indCel;
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].CC = dados.CC;
        for (int j = 0; j < dados.prop.parserie; j++) {
            double xc = malha.mlh2d[i].cel2D.centroideElem[0];
            double yc = malha.mlh2d[i].cel2D.centroideElem[1];
            if ((xc <= dados.prop.xmax[j] && xc >= dados.prop.xmin[j]) && (yc <= dados.prop.ymax[j] && yc >= dados.prop.ymin[j])) {
                malha.mlh2d[i].cel2D.cond = dados.prop.cond[j];
                malha.mlh2d[i].cel2D.cp = dados.prop.cp[j];
                malha.mlh2d[i].cel2D.rho = dados.prop.rho[j];
                break;
            }
        }
        for (int j = 0; j < dados.CI.parserie; j++) {
            double xc = malha.mlh2d[i].cel2D.centroideElem[0];
            double yc = malha.mlh2d[i].cel2D.centroideElem[1];
            if ((xc <= dados.CI.xmax[j] && xc >= dados.CI.xmin[j]) && (yc <= dados.CI.ymax[j] && yc >= dados.CI.ymin[j])) {
                malha.mlh2d[i].cel2D.tempC = dados.CI.temp[j];
                malha.mlh2d[i].cel2D.tempC0 = dados.CI.temp[j];
                break;
            }
        }
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }

    int indsra = 0;
    for (int i = 0; i < malha.nele; i++) {
        int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
        matglob.fnz[i] = indsra;
        for (int j = 0; j < dimNoZero; j++) {
            int ind = malha.mlh2d[i].cel2D.indVizCres[j];
            matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            matglob.clm[indsra + j] = ind;
        }
        indsra += dimNoZero;
        termolivre[i] = malha.mlh2d[i].TL[0];
    }
    matglob.fnz[malha.nele] = dados.noZero;

    if (dados.rankLU >= 0) {
        matglob.rank = dados.rankLU;
        if (dados.rankLU > 0) {
            matglob.geraILUrank();
            matglob.sraILU = new double[matglob.noZeroIlu];
        }
        matglob.colorido = dados.colore;
        if (dados.colore)
            matglob.geraILUcor();
    }
    precn = 1;
    if (dados.rankLU >= 0)
        precn = 4;

    if (dados.acop == 1) {
        dados.qDesacop = ((2 * M_PI - dados.angAcop) * (dados.tInt - dados.tAmb)) / (1. / dados.condGlob + 1. / (0.5 * dados.diamInt * dados.hI) +
                                                                                     1. / (0.5 * dados.diamExt * dados.hE));
        dados.qAcop = 0.;
        for (int i = 0; i < dados.CC.nRic; i++) {
            for (int j = 0; j < dados.CC.ccRic[i].nserie; j++) {
                dados.CC.ccRic[i].valAmb[j] = dados.tAmb;
                dados.CC.ccRic[i].hAmb[j] = dados.hE;
            }
        }
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    int kcc = 0;
                    while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                        kcc++;
                    if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                        malha.mlh2d[i].ccHR[j] = dados.condLoc;
                        dados.qAcop += malha.mlh2d[i].cel2D.sFaceMod[j] * (dados.tInt - malha.mlh2d[i].cel2D.tempC) / (1 / dados.condLoc + 1. / (dados.hI));
                    }
                }
            }
        }
        dados.qTotal = dados.qAcop + dados.qDesacop;
        dados.tParede = dados.tInt - dados.qTotal / (M_PI * dados.diamInt * dados.hI);
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    int kcc = 0;
                    while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                        kcc++;
                    if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                        malha.mlh2d[i].ccTambR[j] = dados.tParede;
                    }
                }
            }
        }
    }
    kontaTempo = vPoisson.kontaTempo;
}

solverP &solverP::operator=(const solverP &vPoisson) {
    if (this != &vPoisson) {
        vg1dSP = vPoisson.vg1dSP;
        indCel = vPoisson.indCel;
        dados = vPoisson.dados;
        termolivre = vPoisson.termolivre;
        matglob = vPoisson.matglob;

        malha = malha2d(vg1dSP, dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno, dados.temp.perm, dados.temp.trans);
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].CC = dados.CC;
            for (int j = 0; j < dados.prop.parserie; j++) {
                double xc = malha.mlh2d[i].cel2D.centroideElem[0];
                double yc = malha.mlh2d[i].cel2D.centroideElem[1];
                if ((xc <= dados.prop.xmax[j] && xc >= dados.prop.xmin[j]) && (yc <= dados.prop.ymax[j] && yc >= dados.prop.ymin[j])) {
                    malha.mlh2d[i].cel2D.cond = dados.prop.cond[j];
                    malha.mlh2d[i].cel2D.cp = dados.prop.cp[j];
                    malha.mlh2d[i].cel2D.rho = dados.prop.rho[j];
                    break;
                }
            }
            for (int j = 0; j < dados.CI.parserie; j++) {
                double xc = malha.mlh2d[i].cel2D.centroideElem[0];
                double yc = malha.mlh2d[i].cel2D.centroideElem[1];
                if ((xc <= dados.CI.xmax[j] && xc >= dados.CI.xmin[j]) && (yc <= dados.CI.ymax[j] && yc >= dados.CI.ymin[j])) {
                    malha.mlh2d[i].cel2D.tempC = dados.CI.temp[j];
                    malha.mlh2d[i].cel2D.tempC0 = dados.CI.temp[j];
                    break;
                }
            }
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    malha.mlh2d[i].atualizaCC(j);
                }
            }
        }

        int indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            matglob.fnz[i] = indsra;
            for (int j = 0; j < dimNoZero; j++) {
                int ind = malha.mlh2d[i].cel2D.indVizCres[j];
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
                matglob.clm[indsra + j] = ind;
            }
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        if (malha.nele > 0)
            matglob.fnz[malha.nele] = dados.noZero;

        if (dados.rankLU >= 0) {
            matglob.rank = dados.rankLU;
            if (dados.rankLU > 0) {
                matglob.geraILUrank();
                matglob.sraILU = new double[matglob.noZeroIlu];
            }
            matglob.colorido = dados.colore;
            if (dados.colore)
                matglob.geraILUcor();
        }
        precn = 1;
        if (dados.rankLU >= 0)
            precn = 4;

        if (dados.acop == 1) {
            dados.qDesacop = ((2 * M_PI - dados.angAcop) * (dados.tInt - dados.tAmb)) / (1. / dados.condGlob + 1. / (0.5 * dados.diamInt * dados.hI) +
                                                                                         1. / (0.5 * dados.diamExt * dados.hE));
            dados.qAcop = 0.;
            for (int i = 0; i < dados.CC.nRic; i++) {
                for (int j = 0; j < dados.CC.ccRic[i].nserie; j++) {
                    dados.CC.ccRic[i].valAmb[j] = dados.tAmb;
                    dados.CC.ccRic[i].hAmb[j] = dados.hE;
                }
            }
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            malha.mlh2d[i].ccHR[j] = dados.condLoc;
                            dados.qAcop += malha.mlh2d[i].cel2D.sFaceMod[j] * (dados.tInt - malha.mlh2d[i].cel2D.tempC) / (1 / dados.condLoc + 1. / (dados.hI));
                        }
                    }
                }
            }
            dados.qTotal = dados.qAcop + dados.qDesacop;
            dados.tParede = dados.tInt - dados.qTotal / (M_PI * dados.diamInt * dados.hI);
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            malha.mlh2d[i].ccTambR[j] = dados.tParede;
                        }
                    }
                }
            }
        }
        kontaTempo = vPoisson.kontaTempo;
    }

    return *this;
}

void solverP::indrazT(int &ind, double &raz) {

    for (int i = 0; i <= dados.temp.parserie - 1; i++) {
        if (i < dados.temp.parserie - 1) {
            if ((*vg1dSP).lixo5 >= dados.temp.tempo[i] && (*vg1dSP).lixo5 < dados.temp.tempo[i + 1]) {
                ind = i;
                raz = 1 - ((*vg1dSP).lixo5 - dados.temp.tempo[i]) / (dados.temp.tempo[i + 1] - dados.temp.tempo[i]);
                break;
            }
        } else if (i == dados.temp.parserie - 1) {
            ind = i;
            raz = 1;
        }
    }
}

void solverP::permanentePoisson() {
    double norma = 1e5;
    int itera = 0;
    int indsra;
    int diverge = 0;
    double norma0 = norma;
    while (norma > 1e-5 && diverge == 0) {
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.dim; j++) {
                malha.mlh2d[i].cel2D.gradGreenTI[j] = malha.mlh2d[i].cel2D.gradGreenT[j];
            }
        }
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].calcGradGreen();
        }
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].GeraLocal();
        }
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].cel2D.tempC0 = malha.mlh2d[i].cel2D.tempCI = malha.mlh2d[i].cel2D.tempC;
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh2d[i].cel2D.tempC;

        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].cel2D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh2d[i].cel2D.tempC - malha.mlh2d[i].cel2D.tempC0);
        }
        norma /= malha.nele;
        if (norma >= norma0)
            diverge = 1;
        else
            norma0 = norma;

        if (dados.acop == 1) {
            dados.qDesacop = ((2 * M_PI - dados.angAcop) * (dados.tInt - dados.tAmb)) / (1. / dados.condGlob + 1. / (0.5 * dados.diamInt * dados.hI) +
                                                                                         1. / (0.5 * dados.diamExt * dados.hE));
            dados.qAcop = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            dados.qAcop += malha.mlh2d[i].cel2D.sFaceMod[j] * (dados.tInt - malha.mlh2d[i].tempF[j]) / (1 / dados.condLoc + 1. / (dados.hI));
                        }
                    }
                }
            }
            dados.qTotal = dados.qAcop + dados.qDesacop;
            dados.tParede = dados.tInt - dados.qTotal / (M_PI * dados.diamInt * dados.hI);
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            malha.mlh2d[i].ccTambR[j] = dados.tParede;
                        }
                    }
                }
            }
        }
        itera++;
    }
}

void solverP::imprimePermanente(int indTramo) {
    malha.imprime(0., indCel, indTramo);
}

void solverP::inicializaPermanentePoisson() {
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].perm = 1;
        malha.mlh2d[i].trans = 0;
    }
    dados.tInt0 = dados.tInt;
}

void solverP::inicializaTransientePoisson() {
    if (dados.temp.perm == 1 && dados.temp.trans == 1) {
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].perm = 0;
            malha.mlh2d[i].trans = 1;
        }
    }

    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].perm = 0;
        malha.mlh2d[i].trans = 1;
    }
    kontaTempo = 0;
    dados.tInt0 = dados.tInt;
}

void solverP::transientePoissonDummy(double vdelt) {
    double delt = vdelt;
    double norma;
    int itera;
    int indsra;

    norma = 1e5;

    for (int i = 0; i < malha.nele; i++)
        malha.mlh2d[i].dt = delt;
    for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }
    itera = 0;
    while (norma > 1.e-5) {
#pragma omp parallel for num_threads(dados.nthrdLocal)
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.dim; j++) {
                malha.mlh2d[i].cel2D.gradGreenTI[j] = malha.mlh2d[i].cel2D.gradGreenT[j];
            }
        }
#pragma omp parallel for num_threads(dados.nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].calcGradGreen();
#pragma omp parallel for num_threads(dados.nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].GeraLocal();
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh2d[i].cel2D.tempC;
        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].cel2D.tempCI = malha.mlh2d[i].cel2D.tempC;
            malha.mlh2d[i].cel2D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh2d[i].cel2D.tempC - malha.mlh2d[i].cel2D.tempCI);
        }
        norma /= malha.nele;

        if (dados.acop == 1) {
            dados.qDesacop = ((2 * M_PI - dados.angAcop) * (dados.tInt - dados.tAmb)) / (1. / dados.condGlob + 1. / (0.5 * dados.diamInt * dados.hI) +
                                                                                         1. / (0.5 * dados.diamExt * dados.hE));
            dados.qAcop = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            dados.qAcop += malha.mlh2d[i].cel2D.sFaceMod[j] * (dados.tInt - malha.mlh2d[i].tempF[j]) /
                                           (1 / dados.condLoc + 1. / (dados.hI));
                        }
                    }
                }
            }
            dados.qTotal = dados.qAcop + dados.qDesacop;
            dados.tParede = dados.tInt - dados.qTotal / (M_PI * dados.diamInt * dados.hI);
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            malha.mlh2d[i].ccTambR[j] = dados.tParede;
                        }
                    }
                }
            }
        }
        itera++;
    }
    if (dados.acop == 1)
        dados.tInt0 = dados.tInt;
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].cel2D.tempC0 = malha.mlh2d[i].cel2D.tempC;
    }
}

double solverP::defineDeltPoisson() {
    int ind = 0;
    double raz;
    double delt;

    indrazT(ind, raz);
    double inf = dados.temp.dtmax[ind];
    double sup;
    if (ind < dados.temp.parserie - 1) {
        sup = dados.temp.dtmax[ind + 1];
        delt = (inf * raz + (1 - raz) * sup);
    } else
        delt = dados.temp.dtmax[ind];
    return delt;
}

void solverP::transientePoisson(double vdelt) {
    double delt = vdelt;
    double norma;
    int itera;
    int indsra;
    norma = 1e5;
    for (int i = 0; i < malha.nele; i++)
        malha.mlh2d[i].dt = delt;
    for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }
    itera = 0;
    while (norma > 1.e-5) {
#pragma omp parallel for num_threads(dados.nthrdLocal)
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.dim; j++) {
                malha.mlh2d[i].cel2D.gradGreenTI[j] = malha.mlh2d[i].cel2D.gradGreenT[j];
            }
        }
#pragma omp parallel for num_threads(dados.nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].calcGradGreen();
#pragma omp parallel for num_threads(dados.nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].GeraLocal();
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh2d[i].cel2D.tempC;
        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].cel2D.tempCI = malha.mlh2d[i].cel2D.tempC;
            malha.mlh2d[i].cel2D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh2d[i].cel2D.tempC - malha.mlh2d[i].cel2D.tempCI);
        }
        norma /= malha.nele;

        if (dados.acop == 1) {
            dados.qDesacop = ((2 * M_PI - dados.angAcop) * (dados.tInt - dados.tAmb)) / (1. / dados.condGlob + 1. / (0.5 * dados.diamInt * dados.hI) +
                                                                                         1. / (0.5 * dados.diamExt * dados.hE));
            dados.qAcop = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            dados.qAcop += malha.mlh2d[i].cel2D.sFaceMod[j] * (dados.tInt - malha.mlh2d[i].tempF[j]) /
                                           (1 / dados.condLoc + 1. / (dados.hI));
                        }
                    }
                }
            }
            dados.qTotal = dados.qAcop + dados.qDesacop;
            dados.tParede = dados.tInt - dados.qTotal / (M_PI * dados.diamInt * dados.hI);
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados.CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados.CC.rotuloAcop) {
                            malha.mlh2d[i].ccTambR[j] = dados.tParede;
                        }
                    }
                }
            }
        }
        itera++;
    }
}

void solverP::finalizaPassoTransiente(double vdelt, int indTramo) {
    double delt = vdelt;
    if (dados.acop == 1)
        dados.tInt0 = dados.tInt;
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].cel2D.tempC0 = malha.mlh2d[i].cel2D.tempC;
    }

    if (((*vg1dSP).lixo5 <= dados.temp.tempoImp[kontaTempo] && (*vg1dSP).lixo5 + delt >= dados.temp.tempoImp[kontaTempo])) {
        malha.imprime((*vg1dSP).lixo5, indCel, indTramo);
        kontaTempo++;
        if (kontaTempo >= dados.temp.parserieImp)
            kontaTempo--;
    }
}

void solverP::FeiticoDoTempo() {
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].cel2D.tempC = malha.mlh2d[i].cel2D.tempC0;
    }
}