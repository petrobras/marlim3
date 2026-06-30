/*
 * solverP3Doisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */
#include "solver3DPoisson.h"

solverP3D::solverP3D(string nomeArquivoEntrada, varGlob1D *DGP3DV, int nacopV, vector<DadosGeo> &dutoAux,
                     double hi, double he, double ti) : dados(nomeArquivoEntrada), termolivre(dados.nele), matglob(dados.nele, dados.noZero, dados.vecsra, dados.veclm, dados.vecfnz) {
    nacop = nacopV;
    itera = 0;
    penalizaDt = 0;
    DGP3D = DGP3DV;
    (*DGP3D).tempo = 0.;

    rank = dados.rank;
    precn = 4;
    if (rank < 0)
        precn = 1;
    matglob.colorido = dados.colorido;
    matglob.rank = rank;

    malha = malha3d(DGP3D, dados.elementoO, dados.xcoor, dados.noEle, dados.tipo,
                    dados.atributo, 4, dados.nele, dados.nno);
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh3d[i].CC = &dados.CC;
        for (int j = 0; j < dados.prop.parserie; j++) {
            if (malha.mlh3d[i].eleUNV.regiao == dados.prop.regiao[j]) {
                malha.mlh3d[i].cel3D.cond = dados.prop.cond[j];
                malha.mlh3d[i].cel3D.cp = dados.prop.cp[j];
                malha.mlh3d[i].cel3D.rho = dados.prop.rho[j];
                break;
            }
        }
        malha.mlh3d[i].cel3D.tempC = ti;
        malha.mlh3d[i].cel3D.tempC0 = ti;
        malha.mlh3d[i].cel3D.tempCI = ti;
        for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
            if (malha.mlh3d[i].kvizinho[j] < 0) {
                malha.mlh3d[i].atualizaCC(j);
            }
        }
    }

    int indsra = 0;
    for (int i = 0; i < malha.nele; i++) {
        int dimNoZero = malha.mlh3d[i].cel3D.indVizCres.size();
        matglob.fnz[i] = indsra;
        for (int j = 0; j < dimNoZero; j++) {
            int ind = malha.mlh3d[i].cel3D.indVizCres[j];
            matglob.sra[indsra + j] = malha.mlh3d[i].local.mx[0][j];
            matglob.clm[indsra + j] = ind;
        }
        indsra += dimNoZero;
        termolivre[i] = malha.mlh3d[i].TL[0];
    }
    matglob.fnz[malha.nele] = dados.noZero;

    if (rank > 0) {
        matglob.geraILUrank();
        matglob.sraILU = new double[matglob.noZeroIlu];
    }
    if (matglob.colorido && rank >= 0)
        matglob.geraILUcor();

    if (dados.acop == 1) {
        for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
            dados.hE[iacop] = he;
            dados.hI[iacop] = hi;
            dados.tInt[iacop] = ti;
            dados.qAcop[iacop] = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                    if (malha.mlh3d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < dados.CC.nAcop && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                            kcc++;
                        if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                            malha.mlh3d[i].ccHR[j] = dados.hI[iacop];
                            dados.qAcop[iacop] += malha.mlh3d[i].cel3D.sFaceMod[j] *
                                                  (dados.tInt[iacop] - malha.mlh3d[i].cel3D.tempC) / (1. / (dados.hI[iacop]));
                        }
                    }
                }
            }
            dados.qTotal[iacop] = dados.qAcop[iacop];
            dados.tParede[iacop] = dados.tInt[iacop];
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                    if (malha.mlh3d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                            kcc++;
                        if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                            malha.mlh3d[i].ccTambR[j] = dados.tParede[iacop];
                        }
                    }
                }
            }
        }
    }

    ki = 0.;
    cpi = 0.;
    rhoi = 0.;
    visci = 0.;
    kontaTempo = 0;
}

solverP3D::solverP3D() : dados(), termolivre(1), matglob(1, 1) {
    nacop = 0;
    DGP3D = 0;
    ki = 0.;
    cpi = 0.;
    rhoi = 0.;
    visci = 0.;
    itera = 0;
    penalizaDt = 0;
    kontaTempo = 0;

    rank = dados.rank;
    precn = 1;
}

solverP3D::solverP3D(const solverP3D &vPoisson) : dados(vPoisson.dados.entrada), termolivre(dados.nele), matglob(dados.nele, dados.noZero, dados.vecsra, dados.veclm, dados.vecfnz) {
    nacop = vPoisson.nacop;
    itera = 0;
    penalizaDt = 0;
    DGP3D = vPoisson.DGP3D;
    (*DGP3D).tempo = 0.;

    rank = dados.rank;
    precn = 4;
    if (rank < 0)
        precn = 1;
    matglob.colorido = dados.colorido;
    matglob.rank = rank;

    malha = malha3d(DGP3D, dados.elementoO, dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 4, dados.nele, dados.nno);

    for (int i = 0; i < malha.nele; i++) {
        malha.mlh3d[i].CC = &dados.CC;
        for (int j = 0; j < dados.prop.parserie; j++) {
            if (malha.mlh3d[i].eleUNV.regiao == dados.prop.regiao[j]) {
                malha.mlh3d[i].cel3D.cond = dados.prop.cond[j];
                malha.mlh3d[i].cel3D.cp = dados.prop.cp[j];
                malha.mlh3d[i].cel3D.rho = dados.prop.rho[j];
                break;
            }
        }
        malha.mlh3d[i].cel3D.tempC = vPoisson.malha.mlh3d[i].cel3D.tempC;
        malha.mlh3d[i].cel3D.tempC0 = vPoisson.malha.mlh3d[i].cel3D.tempC0;
        malha.mlh3d[i].cel3D.tempCI = vPoisson.malha.mlh3d[i].cel3D.tempCI;
        for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
            if (malha.mlh3d[i].kvizinho[j] < 0) {
                malha.mlh3d[i].atualizaCC(j);
            }
        }
    }

    int indsra = 0;
    for (int i = 0; i < malha.nele; i++) {
        int dimNoZero = malha.mlh3d[i].cel3D.indVizCres.size();
        matglob.fnz[i] = indsra;
        for (int j = 0; j < dimNoZero; j++) {
            int ind = malha.mlh3d[i].cel3D.indVizCres[j];
            matglob.sra[indsra + j] = malha.mlh3d[i].local.mx[0][j];
            matglob.clm[indsra + j] = ind;
        }
        indsra += dimNoZero;
        termolivre[i] = malha.mlh3d[i].TL[0];
    }
    matglob.fnz[malha.nele] = dados.noZero;

    matglob.noZeroIlu = vPoisson.matglob.noZeroIlu;
    if (matglob.noZeroIlu > 0) {
        matglob.veclmILU = new int[matglob.noZeroIlu];
        matglob.veclevILU = new int[matglob.noZeroIlu];
        matglob.vecRelILU = new int[matglob.noZeroIlu];
        for (int i = 0; i < matglob.noZeroIlu; i++) {
            matglob.veclmILU[i] = vPoisson.matglob.veclmILU[i];
            matglob.veclevILU[i] = vPoisson.matglob.veclevILU[i];
            matglob.vecRelILU[i] = vPoisson.matglob.vecRelILU[i];
        }
        matglob.vecfnzILU = new int[malha.nele + 1];
        for (int i = 0; i < malha.nele + 1; i++)
            matglob.vecfnzILU[i] = vPoisson.matglob.vecfnzILU[i];
        matglob.sraILU = new double[matglob.noZeroIlu];
    } else {
        matglob.veclevILU = 0;
        matglob.vecRelILU = 0;
        matglob.veclmILU = 0;
        matglob.vecfnzILU = 0;
        matglob.sraILU = 0;
    }

    matglob.color_forward = vPoisson.matglob.color_forward;
    matglob.color_backward = vPoisson.matglob.color_backward;

    if (dados.acop == 1) {
        for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
            dados.hE[iacop] = vPoisson.dados.hE[iacop];
            dados.hI[iacop] = vPoisson.dados.hI[iacop];
            dados.tInt[iacop] = vPoisson.dados.tInt[iacop];
            dados.qAcop[iacop] = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                    if (malha.mlh3d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                            kcc++;
                        if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                            malha.mlh3d[i].ccHR[j] = dados.hI[iacop];
                            dados.qAcop[iacop] += malha.mlh3d[i].cel3D.sFaceMod[j] *
                                                  (dados.tInt[iacop] - malha.mlh3d[i].cel3D.tempC) / (1. / (dados.hI[iacop]));
                        }
                    }
                }
            }
            dados.qTotal[iacop] = dados.qAcop[iacop];
            dados.tParede[iacop] = dados.tInt[iacop];
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                    if (malha.mlh3d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                            kcc++;
                        if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                            malha.mlh3d[i].ccTambR[j] = dados.tParede[iacop];
                        }
                    }
                }
            }
        }
    }
    ki = vPoisson.ki;
    cpi = vPoisson.cpi;
    rhoi = vPoisson.rhoi;
    visci = vPoisson.visci;
    kontaTempo = vPoisson.kontaTempo;
}

solverP3D &solverP3D::operator=(const solverP3D &vPoisson) {
    if (this != &vPoisson && vPoisson.dados.nele > 0) {
        nacop = vPoisson.nacop;
        itera = 0;
        penalizaDt = 0;

        DGP3D = vPoisson.DGP3D;
        dados = vPoisson.dados;
        termolivre = vPoisson.termolivre;
        matglob = vPoisson.matglob;

        rank = dados.rank;
        precn = 4;
        if (rank < 0)
            precn = 1;
        matglob.colorido = dados.colorido;
        matglob.rank = rank;

        (*DGP3D).tempo = 0.;
        if (malha.mlh3d.size() > 0)
            malha.mlh3d.clear();
        malha = malha3d(DGP3D,
                        dados.elementoO, dados.xcoor, dados.noEle, dados.tipo,
                        dados.atributo, 4, dados.nele, dados.nno);

        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].CC = &dados.CC;
            for (int j = 0; j < dados.prop.parserie; j++) {
                if (malha.mlh3d[i].eleUNV.regiao == dados.prop.regiao[j]) {
                    malha.mlh3d[i].cel3D.cond = dados.prop.cond[j];
                    malha.mlh3d[i].cel3D.cp = dados.prop.cp[j];
                    malha.mlh3d[i].cel3D.rho = dados.prop.rho[j];
                    break;
                }
            }
            malha.mlh3d[i].cel3D.tempC = vPoisson.malha.mlh3d[i].cel3D.tempC;
            malha.mlh3d[i].cel3D.tempC0 = vPoisson.malha.mlh3d[i].cel3D.tempC0;
            malha.mlh3d[i].cel3D.tempCI = vPoisson.malha.mlh3d[i].cel3D.tempCI;
            for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                if (malha.mlh3d[i].kvizinho[j] < 0) {
                    malha.mlh3d[i].atualizaCC(j);
                }
            }
        }

        int indsra = 0;
        for (int i = 0; i < malha.nele; i++) {

            int dimNoZero = malha.mlh3d[i].cel3D.indVizCres.size();
            matglob.fnz[i] = indsra;
            for (int j = 0; j < dimNoZero; j++) {
                int ind = malha.mlh3d[i].cel3D.indVizCres[j];
                matglob.sra[indsra + j] = malha.mlh3d[i].local.mx[0][j];
                matglob.clm[indsra + j] = ind;
            }
            indsra += dimNoZero;
            termolivre[i] = malha.mlh3d[i].TL[0];
        }
        matglob.fnz[malha.nele] = dados.noZero;

        if (matglob.noZeroIlu > 0) {
            for (int i = 0; i < matglob.noZeroIlu; i++) {
                matglob.veclmILU[i] = vPoisson.matglob.veclmILU[i];
                matglob.veclevILU[i] = vPoisson.matglob.veclevILU[i];
                matglob.vecRelILU[i] = vPoisson.matglob.vecRelILU[i];
            }
            for (int i = 0; i < malha.nele + 1; i++)
                matglob.vecfnzILU[i] = vPoisson.matglob.vecfnzILU[i];
        } else {
            matglob.veclevILU = 0;
            matglob.vecRelILU = 0;
            matglob.veclmILU = 0;
            matglob.vecfnzILU = 0;
            matglob.sraILU = 0;
        }

        matglob.color_forward = vPoisson.matglob.color_forward;
        matglob.color_backward = vPoisson.matglob.color_backward;

        if (dados.acop == 1) {
            for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
                dados.hE[iacop] = vPoisson.dados.hE[iacop];
                dados.hI[iacop] = vPoisson.dados.hI[iacop];
                dados.tInt[iacop] = vPoisson.dados.tInt[iacop];
                dados.qAcop[iacop] = 0.;
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccHR[j] = dados.hI[iacop];
                                dados.qAcop[iacop] += malha.mlh3d[i].cel3D.sFaceMod[j] *
                                                      (dados.tInt[iacop] - malha.mlh3d[i].cel3D.tempC) /
                                                      (1. / (dados.hI[iacop]));
                            }
                        }
                    }
                }
                dados.qTotal[iacop] = dados.qAcop[iacop];
                dados.tParede[iacop] = dados.tInt[iacop];
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccTambR[j] = dados.tParede[iacop];
                            }
                        }
                    }
                }
            }
        }
        ki = vPoisson.ki;
        cpi = vPoisson.cpi;
        rhoi = vPoisson.rhoi;
        visci = vPoisson.visci;
        kontaTempo = vPoisson.kontaTempo;
    }
    return *this;
}

void solverP3D::indrazT(int &ind, double &raz) {

    for (int i = 0; i <= dados.temp.parserie - 1; i++) {
        if (i < dados.temp.parserie - 1) {
            if ((*DGP3D).tempo >= dados.temp.tempo[i] && (*DGP3D).tempo < dados.temp.tempo[i + 1]) {
                ind = i;
                raz = 1 - ((*DGP3D).tempo - dados.temp.tempo[i]) / (dados.temp.tempo[i + 1] - dados.temp.tempo[i]);
                break;
            }
        } else if (i == dados.temp.parserie - 1) {
            ind = i;
            raz = 1;
        }
    }
}

int solverP3D::permanentePoisson() {
    double norma = 1e5;
    int itera = 0;
    int indsra;
    int diverge = 0;
    double norma0 = norma;
    while ((norma > 1e-5 || itera < 5) && diverge == 0) {
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh3d[i].cel3D.dim; j++) {
                malha.mlh3d[i].cel3D.gradGreenTI[j] = malha.mlh3d[i].cel3D.gradGreenT[j];
            }
        }
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].calcGradGreen();
        }
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].GeraLocal(dados.temp.relax);
        }
        for (int i = 0; i < malha.nele; i++)
            malha.mlh3d[i].cel3D.tempC0 = malha.mlh3d[i].cel3D.tempCI = malha.mlh3d[i].cel3D.tempC;
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh3d[i].cel3D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh3d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh3d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh3d[i].cel3D.tempC;

        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].cel3D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh3d[i].cel3D.tempC - malha.mlh3d[i].cel3D.tempC0);
        }
        norma /= malha.nele;
        if (norma >= norma0)
            diverge = 1;
        else
            norma0 = norma;

        if (dados.acop == 1) {
            for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
                dados.qAcop[iacop] = 0.;
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccHR[j] = dados.hI[iacop];
                                dados.qAcop[iacop] += malha.mlh3d[i].cel3D.sFaceMod[j] *
                                                      (dados.tInt[iacop] - malha.mlh3d[i].tempF[j]) / (1. / (dados.hI[iacop]));
                            }
                        }
                    }
                }
                dados.qTotal[iacop] = dados.qAcop[iacop];
                dados.tParede[iacop] = dados.tInt[iacop];
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccTambR[j] = dados.tParede[iacop];
                            }
                        }
                    }
                }
            }
        }
        itera++;
    }
    malha.imprime(0.);
    return diverge;
}

void solverP3D::inicializaPermanentePoisson() {

    for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
        dados.tInt0[iacop] = dados.tInt[iacop];
    }
}

void solverP3D::inicializaTransientePoisson() {
    kontaTempo = 0;
    for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
        dados.tInt0[iacop] = dados.tInt[iacop];
    }
}

double solverP3D::defineDeltPoisson() {
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

void solverP3D::transientePoisson(double vdelt) {
    double delt = vdelt;
    double norma;
    int indsra;
    (*DGP3D).tempo = (*DGP3D).lixo5;
    norma = 1e5;
    for (int i = 0; i < malha.nele; i++)
        malha.mlh3d[i].dt = delt;
    for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
            if (malha.mlh3d[i].kvizinho[j] < 0) {
                malha.mlh3d[i].atualizaCC(j);
            }
        }
    }
    itera = 0;
    while (norma > 1.e-20 || itera < 5) {
#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh3d[i].cel3D.dim; j++) {
                malha.mlh3d[i].cel3D.gradGreenTI[j] = malha.mlh3d[i].cel3D.gradGreenT[j];
            }
        }
#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].calcGradGreen();
        }
#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].GeraLocal(dados.temp.relax);
        }
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh3d[i].cel3D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh3d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh3d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++) {
            if (i == 16220) {
            }
            malha.vecSolv[i] = malha.mlh3d[i].cel3D.tempC;
        }
        norma = 0.;
#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].cel3D.tempCI = malha.mlh3d[i].cel3D.tempC;
            malha.mlh3d[i].cel3D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh3d[i].cel3D.tempC - malha.mlh3d[i].cel3D.tempCI);
            if (fabs(malha.mlh3d[i].cel3D.tempC - malha.mlh3d[i].cel3D.tempCI) > 1) {
            }
        }
        norma /= malha.nele;

        if (dados.acop == 1) {
            for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
                dados.qAcop[iacop] = 0.;
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccHR[j] = dados.hI[iacop];
                                dados.qAcop[iacop] += malha.mlh3d[i].cel3D.sFaceMod[j] *
                                                      (dados.tInt[iacop] - malha.mlh3d[i].tempF[j]) /
                                                      (1. / (dados.hI[iacop]));
                            }
                        }
                    }
                }
                dados.qTotal[iacop] = dados.qAcop[iacop];
                dados.tParede[iacop] = dados.tInt[iacop];
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccTambR[j] = dados.tParede[iacop];
                            }
                        }
                    }
                }
            }
        }
        itera++;
    }
    if (penalizaDt > 0)
        penalizaDt--;

    cout << "tempo= " << (*DGP3D).tempo << " norma= " << norma << " iteracoes= " << itera;
    cout << endl;

    if (((*DGP3D).tempo > 1e-15 && (*DGP3D).tempo <= dados.temp.tempoImp[kontaTempo] && (*DGP3D).tempo + delt >= dados.temp.tempoImp[kontaTempo])) {
        malha.imprime((*DGP3D).tempo);
        kontaTempo++;
        if (kontaTempo >= dados.temp.parserieImp)
            kontaTempo--;
    }
}

void solverP3D::renova() {
    for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
        dados.tInt0[iacop] = dados.tInt[iacop];
    }
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh3d[i].cel3D.tempC0 = malha.mlh3d[i].cel3D.tempC;
    }
}

void solverP3D::transientePoissonDummy(double vdelt, int konta) {
    double delt = vdelt;
    double norma;
    int itera;
    int indsra;
    norma = 1e5;
    for (int i = 0; i < malha.nele; i++)
        malha.mlh3d[i].dt = delt;
    for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
            if (malha.mlh3d[i].kvizinho[j] < 0) {
                malha.mlh3d[i].atualizaCC(j);
            }
        }
    }
    itera = 0;
    while (norma > 1.e-20 || itera < 5) {
#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh3d[i].cel3D.dim; j++) {
                malha.mlh3d[i].cel3D.gradGreenTI[j] = malha.mlh3d[i].cel3D.gradGreenT[j];
            }
        }
#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh3d[i].calcGradGreen();
#pragma omp parallel for num_threads((*DGP3D).ntrd3DP)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh3d[i].GeraLocal(1.);
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh3d[i].cel3D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh3d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh3d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh3d[i].cel3D.tempC;
        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh3d[i].cel3D.tempCI = malha.mlh3d[i].cel3D.tempC;
            malha.mlh3d[i].cel3D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh3d[i].cel3D.tempC - malha.mlh3d[i].cel3D.tempCI);
        }
        norma /= malha.nele;

        if (dados.acop == 1) {
            for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
                dados.qAcop[iacop] = 0.;
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccHR[j] = dados.hI[iacop];
                                dados.qAcop[iacop] += malha.mlh3d[i].cel3D.sFaceMod[j] * (dados.tInt[iacop] - malha.mlh3d[i].tempF[j]) /
                                                      (1. / (dados.hI[iacop]));
                            }
                        }
                    }
                }
                dados.qTotal[iacop] = dados.qAcop[iacop];
                dados.tParede[iacop] = dados.tInt[iacop];
                for (int i = 0; i < malha.nele; i++) {
                    for (int j = 0; j < malha.mlh3d[i].cel3D.nvert; j++) {
                        if (malha.mlh3d[i].kvizinho[j] < 0) {
                            int kcc = 0;
                            while (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] != dados.CC.rotuloAcop[iacop])
                                kcc++;
                            if (kcc < 1 && malha.mlh3d[i].cel3D.ccFace[j] == dados.CC.rotuloAcop[iacop]) {
                                malha.mlh3d[i].ccTambR[j] = dados.tParede[iacop];
                            }
                        }
                    }
                }
            }
        }
        itera++;
    }
    for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
        dados.tInt0[iacop] = dados.tInt[iacop];
    }
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh3d[i].cel3D.tempC0 = malha.mlh3d[i].cel3D.tempC;
    }

    cout << "kontatempo= " << konta << " norma= " << norma << " iteracoes= " << itera << " deltempo= " << delt;
    cout << endl;
}

void solverP3D::FeiticoDoTempo() {
    for (int iacop = 0; iacop < dados.CC.nAcop; iacop++) {
        dados.tInt0[iacop] = dados.tInt[iacop];
    }
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh3d[i].cel3D.tempC = malha.mlh3d[i].cel3D.tempC0;
    }
}