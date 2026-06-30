#include "estrat.h"

estratificado::estratificado(double vdia, double Ql, double Qg,
                             double vrl, double vrg, double vmil,
                             double vmig, double hol,
                             double vang, double eps) {

    ang = vang;
    rl = vrl;
    rg = vrg;
    mil = vmil;
    mig = vmig;

    dia = vdia;

    double area = M_PI * dia * dia / 4.;
    hfilm = fhfilm(hol * dia, hol, dia);
    holliq = hol;

    double var = 2. * hfilm / dia - 1.;
    alNd = al(var, dia) / (dia * dia);
    agNd = ag(var, dia) / (dia * dia);
    swlNd = sl(var, dia) / dia;
    swgNd = sg(var, dia) / dia;
    siNd = si(var, dia) / dia;
    uls = Ql / area;
    ugs = Qg / area;
    fg = fricg(dia, Qg, eps);
    fl = fricl(dia, Ql, eps);
    fi = fricg(dia, Qg - Ql, eps);
    reyL = reyl(var, dia, Ql);
    reyG = reyg(var, dia, Qg);

    double vell = uls / (4. * alNd / M_PI);
    double velg = ugs / (4. * agNd / M_PI);

    twl = tall(vell);
    twg = talg(velg);
    ti = tali(velg, vell);

    velref = 0.25 * M_PI * (rl * uls / (alNd * alNd) + rg * ugs / (agNd * agNd)) / (rl / alNd + rg / agNd);

    dina = 0.;
    cine = 0.;

    arr = 1;
    coefC0 = 0.;
    valUd = 0.;
    fatorperdaGas = 0.;
    fatorperdaLiq = 0.;
}

estratificado::estratificado(const estratificado &vest) {

    ang = vest.ang;
    rl = vest.rl;
    rg = vest.rg;
    mil = vest.mil;
    mig = vest.mig;

    dia = vest.dia;
    hfilm = vest.hfilm;
    holliq = vest.holliq;

    alNd = vest.alNd;
    agNd = vest.agNd;
    swlNd = vest.swlNd;
    swgNd = vest.swgNd;
    siNd = vest.siNd;
    uls = vest.uls;
    ugs = vest.ugs;
    fg = vest.fg;
    fl = vest.fl;
    reyL = vest.reyL;
    reyG = vest.reyG;

    twl = vest.twl;
    twg = vest.twg;
    ti = vest.ti;

    velref = vest.velref;

    dina = vest.dina;
    cine = vest.cine;

    arr = vest.arr;
    coefC0 = vest.coefC0;
    valUd = vest.valUd;

    fatorperdaGas = vest.fatorperdaGas;
    fatorperdaLiq = vest.fatorperdaLiq;

    dFdAl = vest.dFdAl;
    dFdUg = vest.dFdUg;
    dFdUl = vest.dFdUl;
    fi = vest.fi;
    multTrans = vest.multTrans;
}

estratificado &estratificado::operator=(const estratificado &vest) {
    ang = vest.ang;
    rl = vest.rl;
    rg = vest.rg;
    mil = vest.mil;
    mig = vest.mig;

    dia = vest.dia;
    hfilm = vest.hfilm;
    holliq = vest.holliq;

    alNd = vest.alNd;
    agNd = vest.agNd;
    swlNd = vest.swlNd;
    swgNd = vest.swgNd;
    siNd = vest.siNd;
    uls = vest.uls;
    ugs = vest.ugs;
    fg = vest.fg;
    fl = vest.fl;
    reyL = vest.reyL;
    reyG = vest.reyG;

    twl = vest.twl;
    twg = vest.twg;
    ti = vest.ti;

    velref = vest.velref;

    dina = vest.dina;
    cine = vest.cine;

    arr = vest.arr;
    coefC0 = vest.coefC0;
    valUd = vest.valUd;

    fatorperdaGas = vest.fatorperdaGas;
    fatorperdaLiq = vest.fatorperdaLiq;

    dFdAl = vest.dFdAl;
    dFdUg = vest.dFdUg;
    dFdUl = vest.dFdUl;
    fi = vest.fi;
    multTrans = vest.multTrans;

    return *this;
}

double estratificado::fricl(double dia,
                            double Ql, double eps) {
    double var = 2. * hfilm / dia - 1.;
    double re = fabs(reyl(var, dia, Ql));
    double val;
    if (fabs(re) > 1e-5) {
        if (re > 2400) {
            val = 6.9 / fabs(re) + pow(eps / 3.7, 1.11);
            val = -1.8 * (log(val) / log(10.));
            val = pow(1 / val, 2.);
        } else
            val = 4 * 16. / fabs(re);
    } else
        val = 0.005;
    return val / 4.;
}

double estratificado::fricg(double dia,
                            double Qg, double eps) {
    double var = 2. * hfilm / dia - 1.;
    double re = fabs(reyg(var, dia, Qg));
    double val;
    if (fabs(re) > 1e-5) {
        if (re > 2400) {
            val = 6.9 / fabs(re) + pow(eps / 3.7, 1.11);
            val = -1.8 * (log(val) / log(10.));
            val = pow(1 / val, 2.);
        } else
            val = 4 * 16. / fabs(re);
    } else
        val = 0.005;
    return val / 4.;
}

double estratificado::fonteQMGas(double dia) {
    return -twg * swgNd / (agNd * dia) - ti * siNd / (agNd * dia) - 9.82 * rg * sin(ang);
}

double estratificado::fonteQMliquido(double dia) {
    return -twl * swlNd / (alNd * dia) + ti * siNd / (alNd * dia) - 9.82 * rl * sin(ang);
}

void estratificado::mapa(double dia, double alf) {

    double signUl;
    if ((uls) > 0.)
        signUl = 1.;
    else
        signUl = -1.;
    double signUg;
    if ((ugs) > 0.)
        signUg = 1.;
    else
        signUg = -1.;
    double signUd;
    if ((ugs / alf - uls / (1 - alf)) > 0)
        signUd = 1.;
    else
        signUd = -1.;

    double coefpi2 = pow(0.25 * M_PI, 2.);
    double coefpi3 = pow(0.25 * M_PI, 3.);

    dFdUl = signUl * (fl * rl * swlNd * (uls)*coefpi2 / (dia * pow(alNd, 3.))) +
            signUd * (fg * rg * siNd * coefpi3 / (dia * alNd * agNd)) * (-(uls) / (alNd * alNd) + (ugs) / (alNd * agNd));
    dFdUg = -signUg * (fg * rg * swgNd * (ugs)*coefpi2 / (dia * pow(agNd, 3.))) -
            signUd * (fg * rg * siNd * coefpi3 / (dia * alNd * agNd)) * (-(uls) / (alNd * agNd) + (ugs) / (agNd * agNd));

    double termo = 2. * (hfilm / dia) - 1.;
    double dSldAl = 2. / (1. - termo * termo);
    double dSgdAl = -dSldAl;
    double dSidAl = -2. * termo / (1. - termo * termo);

    double termo1 = signUl * (fl * rl * uls * uls * coefpi2 / (2. * dia)) *
                    (dSldAl / pow(alNd, 3.) - 3. * swlNd / pow(alNd, 4.));

    double termo2 = -signUg * (fg * rg * ugs * ugs * coefpi2 / (2. * dia)) *
                    (dSgdAl / pow(agNd, 3.) + 3. * swgNd / pow(agNd, 4.));

    double termo3 = -signUd * (fg * rg * coefpi3 / (2. * dia));

    double termo3a =
        ugs * ugs * (dSidAl / (pow(agNd, 3.) * alNd) - siNd / (pow(alNd, 2.) * pow(agNd, 3.)) + 3. * siNd / (pow(agNd, 4.) * alNd));

    double termo3b = -2. * uls * ugs * (dSidAl / (pow(alNd, 2.) * pow(agNd, 2.)) - 2. * siNd / (pow(alNd, 3.) * pow(agNd, 2.)) + 2. * siNd / (pow(alNd, 2.) * pow(agNd, 3.)));

    double termo3c =
        uls * uls * (dSidAl / (pow(alNd, 3.) * agNd) - 3. * siNd / (pow(alNd, 4.) * agNd) + siNd / (pow(alNd, 3.) * pow(agNd, 2.)));

    dFdAl = termo1 + termo2 + termo3 * (termo3a + termo3b + termo3c);

    if (fabs(dFdUl - dFdUg) > 1e-20)
        cine = -0.25 * M_PI * dFdAl / (dFdUl - dFdUg);
    else
        cine = 0.;
    dina = (9.82 * dia * cos(ang) * (rl - rg) / siNd -
            coefpi2 * pow(ugs / agNd - uls / alNd, 2.) / (alNd / rl + agNd / rg)) /
           (rl / alNd + rg / agNd);
    arr = 1;
    multTrans = 0.;
    double fator = 1;
    if (dina > fator * pow(cine - velref, 2.))
        arr = -1;

    if (alNd > 1e-4)
        fatorperdaLiq = fl * swlNd * dia / (2. * rl * pow(alNd * dia * dia, 2.));
    else
        fatorperdaLiq = 0.;
    if (agNd > 1e-4)
        fatorperdaGas = fg * swgNd * dia / (2. * rg * pow(agNd * dia * dia, 2.));
    else
        fatorperdaGas = 0.;
}
double estratificado::mapa2(double chute) {
    double area = 0.25 * M_PI * dia * dia;
    double x1 = 0.98;
    double x2 = 0.01;
    if (chute > 0.) {
        x1 = chute;
        x2 = 0.9 * chute;
        if (x1 >= 1.)
            x1 = 0.98;
    } else
        x1 = 0.9;
    double hol = zbrentUls(x1, x2, uls * area, ugs * area, 0.00001 / dia);
    if (hol > 0) {
        double ulsT = qlequil(hol, uls * area, ugs * area, 0.00001 / dia);
        arr = 1;
        if (fabs(uls) < fabs(ulsT))
            arr = -1;
    } else
        arr = 1;
    return hol;
}

void estratificado::mapaTD(double chute) {
    if (fabs(uls) > 0.01 && fabs(ugs) > 0.01) {
        double area = 0.25 * M_PI * dia * dia;
        double ql = uls * area;
        double qg = ugs * area;
        double hol1 = 0.98;
        double hol2 = 0.02;
        double hol;
        if (chute < 1e-15)
            hol = zbrent(hol1, hol2, fabs(ql), fabs(qg), 0.0001);
        else
            hol = zbrent(hol1, hol2, fabs(ql), fabs(qg), 0.0001, 0.01, 0.01);
        holliq = hol;
        if (hol > 0) {
            double hvar = fhfilm(hol * dia, hol, dia);
            double C2 = 1 - hvar / dia;
            double var = 2. * hvar / dia - 1.;
            double C1 = (rl - rg) * 9.82 * cos(ang) * ag(var, dia) / (rg * si(var, dia));

            double maxug = C2 * sqrt(C1) * (1 - hol);

            arr = -1;
            double ul = uls / hol;
            if (fabs(ugs) > maxug)
                arr = 1;
            else if (ul * ang < 0 && ul * ul >= 9.82 * dia * C2 * cos(ang) / fl)
                arr = 1;
        } else
            arr = 1;
    } else
        arr = 1;
}

double estratificado::equil(double dia) {

    double coefpi2 = pow(0.25 * M_PI, 2.);
    double coefpi3 = pow(0.25 * M_PI, 3.);
    double signUd;
    double ugsv = fabs(ugs);
    double ulsv = fabs(uls);
    if ((ugsv - ulsv) > 0.)
        signUd = 1.;
    else
        signUd = -1.;

    return fl * rl * coefpi2 * swlNd * ulsv * ulsv / (2. * dia * pow(alNd, 3.)) -
           fg * rg * coefpi2 * swgNd * ugsv * ugsv / (2. * dia * pow(agNd, 3.)) -
           signUd * (fg * rg * coefpi3 * siNd / (2. * dia)) * (ugsv * ugsv / (alNd * pow(agNd, 3.)) - 2. * ulsv * ugsv / (pow(alNd, 2.) * pow(agNd, 2.)) + ulsv * ulsv / (pow(alNd, 3.) * agNd)) +
           9.82 * (rl - rg) * sin(ang);
}

double estratificado::equil2(double ql, double qt, double eps) {

    double coefpi2 = pow(0.25 * M_PI, 2.);
    double area = 0.25 * M_PI * dia * dia;
    double ulsC = ql / area;
    double ugsC = (qt - ql) / area;
    double fgC = fricg(dia, ugsC * area, eps);
    double flC = fricl(dia, ulsC * area, eps);

    return -flC * rl * coefpi2 * swlNd * ulsC * fabs(ulsC) / (2. * dia * pow(alNd, 3.)) +
           fgC * rg * coefpi2 * swgNd * ugsC * fabs(ugsC) / (2. * dia * pow(agNd, 3.)) +
           (fgC * rg * coefpi2 * siNd / (2. * dia)) * (1 / agNd + 1 / alNd) *
               (ugsC / agNd - ulsC / alNd) * fabs(ugsC / agNd - ulsC / alNd) -
           9.82 * (rl - rg) * sin(ang);
}

double estratificado::equil3(double hol, double Ql, double Qg, double eps) {
    double area = 0.25 * M_PI * dia * dia;

    double hvar = fhfilm(hol * dia, hol, dia);

    double var = 2. * hvar / dia - 1.;
    double alNdv = al(var, dia) / (dia * dia);
    double agNdv = ag(var, dia) / (dia * dia);
    double swlNdv = sl(var, dia) / dia;
    double swgNdv = sg(var, dia) / dia;
    double siNdv = si(var, dia) / dia;

    double coefpi2 = pow(0.25 * M_PI, 2.);
    double ulsC = Ql / area;
    double ugsC = (Qg) / area;
    double fgC = fricg(dia, ugsC * area, eps);
    double flC = fricl(dia, ulsC * area, eps);
    double signd = 1.;
    if ((Qg / (1. - hol) - Ql / hol) < 0.)
        signd = -1.;

    return -flC * rl * coefpi2 * swlNdv * ulsC * fabs(ulsC) / (2. * dia * pow(alNdv, 3.)) +
           fgC * rg * coefpi2 * swgNdv * ugsC * fabs(ugsC) / (2. * dia * pow(agNdv, 3.)) +
           signd * (fgC * rg * coefpi2 * siNdv / (2. * dia)) * (ugsC * ugsC / (alNdv * pow(agNdv, 3)) - 2. * ulsC * ugsC / (pow(alNdv, 2.) * pow(agNdv, 2)) + ulsC * ulsC / (agNdv * pow(alNdv, 3))) - 9.82 * (rl - rg) * sin(ang);
}

double estratificado::qlequil(double hol, double Ql, double Qg, double eps) {
    double area = 0.25 * M_PI * dia * dia;

    double hvar = fhfilm(hol * dia, hol, dia);

    double var = 2. * hvar / dia - 1.;
    double alNdv = al(var, dia) / (dia * dia);
    double agNdv = ag(var, dia) / (dia * dia);
    double swlNdv = sl(var, dia) / dia;
    double swgNdv = sg(var, dia) / dia;
    double siNdv = si(var, dia) / dia;

    double coefpi2 = pow(0.25 * M_PI, 2.);
    double ulsC = Ql / area;
    double ugsC = (Qg) / area;
    double fgC = fg + 0 * fricg(dia, ugsC * area, eps);
    double flC = fl + 0 * fricl(dia, ulsC * area, eps);

    double signl = 1.;
    if (Ql < 0)
        signl = -1.;
    double signg = 1.;
    if (Qg < 0.)
        signg = -1.;
    double alf = 1. - hol;
    double refdrift = (Qg / alf - Ql / (1 - alf));
    double signd = 1.;
    if (refdrift < 0.)
        signd = -1.;
    int loop = 1;
    double saida = 0.;
    while (loop == 1) {
        double term1 = signl * flC * rl * coefpi2 * swlNdv / (2. * dia * pow(alNdv, 3.)) -
                       signd * (fgC * rg * coefpi2 * siNdv / (2. * dia)) / (agNdv * pow(alNdv, 3));
        double term2 = signd * (fgC * rg * coefpi2 * siNdv / (2. * dia)) * (2. * ugsC / (pow(agNdv, 2) * pow(alNdv, 2)));
        double term3 = -signd * (fgC * rg * coefpi2 * siNdv / (2. * dia)) * (1 / (alNdv * agNdv)) * (ugsC / agNdv) * (ugsC / agNdv) -
                       signg * fgC * rg * coefpi2 * swgNdv * ugsC * (ugsC) / (2. * dia * pow(agNdv, 3.)) - 9.82 * (rl - rg) * sin(ang);

        double delta = fabs(term2 * term2 - 4. * term1 * term3);
        double q1 = (-term2 + sqrt(delta)) / (2. * term1);
        double q2 = (-term2 - sqrt(delta)) / (2. * term1);
        if (Ql > 0) {
            if (q1 < 0)
                q1 = 1000000.;
            if (q2 < 0)
                q2 = 1000000.;
            if (q1 < q2)
                saida = q1;
            else
                saida = q2;
        }
        if (Ql < 0) {
            if (q1 > 0)
                q1 = -1000000.;
            if (q2 > 0)
                q2 = -1000000.;
            if (q1 > q2)
                saida = q1;
            else
                saida = q2;
        }
        double refdrift2 = (Qg / alf - saida / (1 - alf));
        if (refdrift2 * refdrift < 0) {
            refdrift = refdrift2;
            if (refdrift < 0.)
                signd = -1.;
            else
                signd = 1.;
        } else
            loop = 0;
    }
    return saida;
}

double estratificado::criterio(double hol, double qlv, double qg, double eps) {
    double area = 0.25 * M_PI * dia * dia;
    double ql = qlequil(hol, qlv, qg, eps) * area;
    double signUl;
    if ((ql) > 0.)
        signUl = 1.;
    else
        signUl = -1.;
    double signUg;
    if ((qg) > 0.)
        signUg = 1.;
    else
        signUg = -1.;
    double signUd;
    double alf = 1. - hol;
    if ((qg / alf - ql / (1 - alf)) > 0)
        signUd = 1.;
    else
        signUd = -1.;

    double coefpi2 = pow(0.25 * M_PI, 2.);
    double coefpi3 = pow(0.25 * M_PI, 3.);

    double hvar = fhfilm(hol * dia, hol, dia);

    double var = 2. * hvar / dia - 1.;
    double alNdv = al(var, dia) / (dia * dia);
    double agNdv = ag(var, dia) / (dia * dia);
    double swlNdv = sl(var, dia) / dia;
    double swgNdv = sg(var, dia) / dia;
    double siNdv = si(var, dia) / dia;

    double ulsC = ql / area;
    double ugsC = (qg) / area;
    double fgC = fg; //+0*fricg(dia,ugsC*area,eps);
    double flC = fl; //+0*fricl(dia,ulsC*area,eps);

    dFdUl = signUl * (flC * rl * swlNdv * (ulsC)*coefpi2 / (dia * pow(alNdv, 3.))) +
            signUd * (fgC * rg * siNdv * coefpi3 / (dia * alNdv * agNdv)) * (-(ulsC) / (alNdv * alNdv) + (ugsC) / (alNdv * agNdv));
    dFdUg = -signUg * (fgC * rg * swgNdv * (ugsC)*coefpi2 / (dia * pow(agNdv, 3.))) -
            signUd * (fgC * rg * siNdv * coefpi3 / (dia * alNdv * agNdv)) * (-(ulsC) / (alNdv * agNdv) + (ugsC) / (agNdv * agNdv));

    double termo = 2. * (hvar / dia) - 1.;
    double dSldAl = 2. / (1. - termo * termo);
    double dSgdAl = -dSldAl;
    double dSidAl = -2. * termo / (1. - termo * termo);

    double termo1 = signUl * (flC * rl * ulsC * ulsC * coefpi2 / (2. * dia)) *
                    (dSldAl / pow(alNdv, 3.) - 3. * swlNdv / pow(alNdv, 4.));

    double termo2 = -signUg * (fgC * rg * ugsC * ugsC * coefpi2 / (2. * dia)) *
                    (dSgdAl / pow(agNdv, 3.) + 3. * swgNdv / pow(agNdv, 4.));

    double termo3 = -signUd * (fgC * rg * coefpi3 / (2. * dia));

    double termo3a =
        ugsC * ugsC * (dSidAl / (pow(agNdv, 3.) * alNdv) - siNdv / (pow(alNdv, 2.) * pow(agNdv, 3.)) + 3. * siNdv / (pow(agNdv, 4.) * alNdv));

    double termo3b = -2. * ulsC * ugsC * (dSidAl / (pow(alNdv, 2.) * pow(agNdv, 2.)) - 2. * siNdv / (pow(alNdv, 3.) * pow(agNdv, 2.)) + 2. * siNdv / (pow(alNdv, 2.) * pow(agNdv, 3.)));

    double termo3c =
        ulsC * ulsC * (dSidAl / (pow(alNdv, 3.) * agNdv) - 3. * siNdv / (pow(alNdv, 4.) * agNdv) + siNdv / (pow(alNdv, 3.) * pow(agNdv, 2.)));

    dFdAl = termo1 + termo2 + termo3 * (termo3a + termo3b + termo3c);

    if (fabs(dFdUl - dFdUg) > 1e-20)
        cine = -0.25 * M_PI * dFdAl / (dFdUl - dFdUg);
    else
        cine = 0.;
    dina = (9.82 * dia * cos(ang) * (rl - rg) / siNdv -
            coefpi2 * pow(ugsC / agNdv - ulsC / alNdv, 2.) / (alNdv / rl + agNdv / rg)) /
           (rl / alNdv + rg / agNdv);

    velref = 0.25 * M_PI * (rl * ulsC / (alNdv * alNdv) + rg * ugsC / (agNdv * agNdv)) / (rl / alNdv + rg / agNdv);

    return (dina - pow(cine - velref, 2.));
}

double estratificado::zbrentUls(double x1, double x2, double qlv, double qg, double eps,
                                double tol,
                                double epsn, int maxit) {
    double x = x1;
    double dx1;
    double fa = 1;
    double fb;
    double dfdx;
    int conta = 0;
    while (fabs(fa) > 0.00001 && x < 0.99 && x > 0.01 && conta < 20) {
        dx1 = x * 0.0001;
        fa = criterio(x, qlv, qg, eps);
        fb = criterio(x + dx1, qlv, qg, eps);
        dfdx = (fb - fa) / dx1;
        x -= (fa / dfdx);
        if (x > 0.99)
            x = 0.98;
        if (x < 0.01)
            x = 0.012;
        conta++;
    }
    if (x >= 0.99 || x <= 0.01 || conta > 19)
        return -1;
    else
        return x;
}

double estratificado::findalf(double hol1, double hol2, double ql, double qg, double eps) {
    return zbrent(hol1, hol2, ql, qg, eps);
}

double estratificado::calcC0(double x1,
                             const double qt,
                             double alf, double eps,
                             double tol,
                             double epsn,
                             int maxit) {
    double signC = 1.;
    if (x1 < 0.)
        signC = -1.;
    double rtn = signC * 0.5 * qt;
    double f;
    double df;
    double dx;
    for (int j = 0; j < maxit; j++) {
        f = equil2(rtn, qt, eps);
        df = (f - equil2(rtn * 0.9999, qt, eps)) / (0.0001 * rtn);
        dx = f / df;
        rtn -= dx;
        if (fabs(dx) < epsn || fabs(f) < tol)
            return (qt - rtn) / (alf * qt);
    }
    throw("M�ximo n�mero de intera��es em Newton");
}

double estratificado::FalsaCorda(double a, double b, double qt,
                                 double alf, double eps, double delta, double epsn, int maxit) {
    // zero de fun��o por falsa corda
    // a e b-> chutes iniciais para a falsa corda
    // epsn -> limite em que se admite F(T) aproximadamente zero
    // delta-> limite a partir do qual admite-se que j� n�o existe varia��o no c�lculo da raiz
    // maxit-> m�ximo n�mero de itera��es permitido no c�lculo
    // F(T)-> fun��o que se quer obter a raiz

    double u = equil2(a, qt, eps);
    double e = b - a;
    double c = 0.;

    for (int k = 1; k <= maxit; k++) { // este bloco trata da falsa corda propriamente dita
        e *= 0.5;
        c = a + e;
        double w = equil2(c, qt, eps);
        if (fabs(e) < delta && fabs(w) < epsn)
            return c;
        ((u > 0 && w < 0) || (u < 0 && w > 0)) ? (b = c) : (a = c, u = w);
    }
    cout << "Falsa corda atingiu n�mero m�ximo de itera��es" << "\n"
         << equil2(c, qt, eps);
    return c;
}

double estratificado::zbrent(double x1, double x2, double ql, double qg, double eps,
                             double tol, double epsn, int maxit) {
    // Acha zero de fun��o ver NR
    double EPS = epsn;
    double a = 0.98;
    double b = 0.05;
    double c = b;
    double fa = equil3(a, ql, qg, eps);
    double fb = equil3(b, ql, qg, eps);
    double e = 0.;
    double d, fc, p, q, r, s, tol1, xm;

    if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0)) {
        double dhol = 0.01;
        while (((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0))) {
            a -= dhol;
            if (a <= b) {
                b = -1.;
                return b;
            }
        }
    }
    fc = fb;
    for (int iter = 0; iter < maxit; iter++) {
        if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
            c = a;
            fc = fa;
            e = d = b - a;
        }
        if (fabs(fc) < fabs(fb)) {
            a = b;
            b = c;
            c = a;
            fa = fb;
            fb = fc;
            fc = fa;
        }
        tol1 = 2.0 * EPS * fabs(b) + 0.5 * tol;
        xm = 0.5 * (c - b);
        if (fabs(xm) <= tol1 || fb == 0.0)
            return b;
        if (fabs(e) >= tol1 && fabs(fa) > fabs(fb)) {
            s = fb / fa;
            if (a == c) {
                p = 2.0 * xm * s;
                q = 1.0 - s;
            } else {
                q = fa / fc;
                r = fb / fc;
                p = s * (2.0 * xm * q * (q - r) - (b - a) * (r - 1.0));
                q = (q - 1.0) * (r - 1.0) * (s - 1.0);
            }
            if (p > 0.0)
                q = -q;
            p = fabs(p);
            double min1 = 3.0 * xm * q - fabs(tol1 * q);
            double min2 = fabs(e * q);
            if (2.0 * p < (min1 < min2 ? min1 : min2)) {
                e = d;
                d = p / q;
            } else {
                d = xm;
                e = d;
            }
        } else {
            d = xm;
            e = d;
        }
        a = b;
        fa = fb;
        if (fabs(d) > tol1)
            b += d;
        else
            b += SIGN(tol1, xm);
        fb = equil3(b, ql, qg, eps);
    }
    b = -1.;
    return b;
}

double estratificado::calcC0FC(const double x1, const double qt,
                               double alf, double eps, double coaux) {
    double area = 0.25 * M_PI * dia * dia;
    double signC = 1.;
    if (x1 < 0.)
        signC = -1.;
    double rtna;
    double rtnb;
    if (signC < 0.) {
        rtna = 10 * area;
        rtnb = -10 * area;
    } else {
        rtna = -10 * area;
        rtnb = 10 * area;
    }

    rtna = FalsaCorda(rtna, rtnb, qt, alf, eps);

    return ((1 - coaux * alf) * qt - rtna) / (alf * area);
}

double estratificado::CalcUdTrans(double alfc, double alf0,
                                  double alf1, double Ql0, double Ql1,
                                  double Qg0, double Qg1,
                                  double pL, double pR,
                                  double c0aux, double dt, double dx, int verifica) {

    if (alf1 > 0.0000001) {
        double area = 0.25 * M_PI * dia * dia;
        double qt = Qg1 + Ql1;
        double j = qt / area;
        double uls0 = Ql0 / area;
        double uls1 = Ql1 / area;
        double ul0 = uls0;
        if ((1 - alf0) > 0.0000001)
            ul0 = uls0 / (1 - alf0);
        double ul1 = uls1;
        if ((1 - alf1) > 0.0000001)
            ul1 = uls1 / (1 - alf1);
        double ug1 = Qg1 / area;
        if (alf1 > 0.0000001)
            ug1 = ug1 / (alf1);

        double du1 = -1 * dt * (ul1 * (ul1 - ul0)) / dx;
        double du2 = -dt * (1 / rl) * (pR - pL) * 98600 / dx;
        double du3 = 1 * dt * 9.82 * cos(ang) * (area / (siNd * dia)) * (alf1 - alf0) / dx;
        double fonte1 = dt * (((1 * twg * siNd * dia) / rl - twl * swlNd * dia / rl) / (dia * dia * alNd) -
                              9.82 * sin(ang));

        double ulsN = (ul1 + du1 + 1 * du2 + du3 + fonte1) * (1. - alf1);
        double raz = fabs(twg * siNd * dia / rl) / (fabs(twg * siNd * dia / rl) + fabs(9.82 * sin(ang) * dia * dia * alNd));
        if (raz > 1)
            raz = 1.;
        double vud = ((1 - c0aux * alf1) * j - ulsN) / (alf1);
        return vud;
    } else
        return -1e10;
}