#include "celula3.h"
#include "Leitura.h"

Cel::Cel(varGlob1D *Vvg1dSP, const DadosGeo vdutoL, const DadosGeo vduto,
         const DadosGeo vdutoR,
         const ProFlu vflui, const ProFluCol vfluicol,
         const double vtempL, const double vtemp,
         const double vtempR, const double vtempini,
         const double vpresL, const double vpres, const double vpresR,
         const double vML, const double vMC, const double vMR,
         const double vMliqiniL, const double vMliqini, const double vMliqiniR,
         const double valfL, const double valf, const double valfR,
         const double vbetL, const double vbet, const double vbetR,
         const double vdxL, const double vdx, const double vdxR,
         const double vdt, const int vposic,
         const TransCal vcalor,
         const acessorio vacsr,
         acessorio *vacsrL) : TL(2), local(2, 6) { // construtor default

    // Solver de Hidratos

    // modeloIII
    V_h_total = 0.0;
    V_h_dep = 0.0;
    V_h_disp = 0.0;

    e_dep = 0.0;
    D_h_eff = 0.0;
    A_eff = 0.0;
    phi_h_disp = 0.0;

    phi_h_eff = 0.0;
    mu_rel_slurry = 0.0;
    mu_base_liq = 0.0;
    mu_slurry = 0.0; // modeloIII

    // fluxos em massa (por área) – começam em zero
    j_H = 0.0; // não são usados no modelo I de Hidratos
    j_G = 0.0; // não são usados no modelo I de Hidratos
    j_W = 0.0; // não são usados no modelo I de Hidratos

    // volumes físicos
    V_h = 0.0; // sem hidrato no início
    V_w = 0.;
    V_g = 0.;

    FVH = 0.0;
    agua_consumida = 0.;
    gas_consumido = 0.;

    agua_consumida_massa = 0.;
    gas_consumido_massa = 0.;

    agua_consumida_massa_step = 0.;
    gas_consumido_massa_step = 0.;

    massa_hidrato = 0.0;

    vg1dSP = Vvg1dSP;
    dutoL = vdutoL;
    duto = vduto;
    dutoR = vdutoR;
    flui = vflui;
    flui.posic = vposic;
    fluiL = 0;
    fluiR = 0;
    fluicol = vfluicol;
    acsr = vacsr;
    acsrL = vacsrL;
    calor = vcalor;
    tempR = vtempR;
    temp = vtemp;
    tempL = vtempL;
    tempini = vtempini;
    presL = vpresL;
    presLini = vpresL;
    presauxL = vpresL;
    presaux = pres = presini = vpres;
    dpresaux = 0.;
    presauxR = vpresR;
    presR = vpresR;
    dxL = vdxL;
    dx = vdx;
    dxR = vdxR;
    razdxTM = 0.;
    razdxTM0 = 0.;
    dt = vdt;
    dt1 = dt;
    dt2 = dt;
    dTdt = 0.;
    dTdtIni = 0.;
    dTdtL = 0.;
    posic = vposic;
    ML = vML;
    MC = vMC;
    MR = vMR;
    MRini = MR;
    alfL = valfL;
    alfIter = alf = valf;
    alfR = valfR;
    betL = vbetL;
    bet = vbet;
    betR = vbetR;
    betLI = betL;
    betI = bet;
    betRI = betR;
    MliqiniL = vMliqiniL;
    Mliqini = vMliqini;
    MliqiniR = vMliqiniR;
    MliqiniR0 = vMliqiniR;
    MComp = 0.;
    presRini = presR;
    MCini = MC;
    MLini = ML;
    Mliqini0 = Mliqini;
    MliqiniL0 = MliqiniL;
    tempini = temp;
    tempLini = tempL;
    tempRini = tempR;
    presauxini = presaux;
    alfLini = alfL;
    alfini = alf;
    alfRini = alfR;
    betLini = betL;
    betini = bet;
    betRini = betR;
    FWini = FW = flui.BSW;
    massfonteCH = 0;
    c0 = 1.;
    ud = 0.;
    c0ini = c0;
    udini = ud;
    c0Spare = c0;
    udSpare = ud;
    transmassL = 0.;
    transmassR = 0.;
    FonteMudaFase = 0.;
    DTransDt1 = 0.;
    DTransDt0 = 0.;
    DTransDx = 0.;
    DTransDxR = 0.;
    DTransDxL = 0.;
    DTransDxLp = 0.;
    DTransDxRp = 0.;
    DtransDxLinear = 0;
    DTransDxLpini = 0.;
    DTransDxRpini = 0.;
    DTransDtp = 0.;
    DTransDtpL = 0.;
    DTransDtpini = 0.;
    DTransDtT = 0.;
    DTransDtTL = 0.;
    CoefDTR = 0.;
    CoefDTL = 0.;
    fontedissolv = 0.;
    ativaDeri = 0.;
    coefTransBet = 0.;
    TMModel = 2;
    TMModelL = 2;
    fontemassLL = 0.;
    fontemassGL = 0.;
    fontemassCL = 0.;
    fontemassLR = 0.;
    fontemassGR = 0.;
    fontemassCR = 0.;
    dpB = 0.;
    potB = 0;
    potBT = 0;
    potTermo = 0.;
    dpBL = 0.;
    potBL = 0;
    potBTL = 0;
    cinematico = 1.;
    arranjo = 1;
    arranjoR = 1;
    if (fabs(ML) < 1e-5) {
        term1L = 0.;
        term2L = 0.;
    } else {
        term1L = MliqiniL / ML;
        term2L = 0.;
    }
    if (fabs(MC) < 1e-5) {
        term1 = 0.;
        term2 = 0.;
    } else {
        term1 = Mliqini / MC;
        term2 = 0.;
    }
    if (fabs(MR) < 1e-5) {
        term1R = 0.;
        term2R = 0.;
    } else {
        term1R = MliqiniR / MR;
        term2R = 0.;
    }
    double localtinyTemp;
    if (vg1dSP != 0)
        localtinyTemp = (*vg1dSP).localtiny;
    else
        localtinyTemp = 1e-9;
    if (fabs(alf) >= localtinyTemp && fabs(alf) <= 1 - localtinyTemp)
        transic = 0;
    else if (fabs(alf) >= localtinyTemp)
        transic = -1;
    else
        transic = 1;
    transic0 = transic;
    double razdx;
    razdx = dx / (dx + dxL);
    double pmed = razdx * pres + (1 - razdx) * presL;
    double rl = flui.MasEspLiq(pmed, temp);
    double rc = fluicol.MasEspFlu(pmed, temp);
    double rg = flui.MasEspGas(pmed, temp);

    rpL = rpLi = flui.MasEspLiq(presL, tempL);
    rpC = rpCi = flui.MasEspLiq(pres, temp);
    rpR = rpRi = flui.MasEspLiq(presR, tempR);
    rcL = rcLi = fluicol.MasEspFlu(presL, tempL);
    rcC = rcCi = fluicol.MasEspFlu(pres, temp);
    rcR = rcRi = fluicol.MasEspFlu(presR, tempR);
    rgL = rgLi = flui.MasEspGas(presL, tempL);
    rgC = rgCi = flui.MasEspGas(pres, temp);
    rgR = rgRi = flui.MasEspGas(presR, tempR);
    drhogdt = flui.drhodt(presL, tempL);
    double rmisL = rpL * (1 - betL) + rcL * betL;
    double rmisR = rpR * (1 - betR) + rcR * betR;
    double rmisC = rl * (1 - bet) + rc * bet;

    mipC = flui.ViscOleo(pres, temp);
    micC = fluicol.VisFlu(pres, temp);
    migC = flui.ViscGas(pres, temp);
    mipR = flui.ViscOleo(pres, temp);
    micR = fluicol.VisFlu(pres, temp);
    migR = flui.ViscGas(pres, temp);

    QL = Mliqini / rmisC;
    QG = (MC - Mliqini) / rg;

    QLL = MliqiniL / rmisL;
    QLR = MliqiniR / rmisR;
    QLRini = MliqiniR / rmisR;

    if (vdutoR.area > 0.)
        VTemper = (QL + QG) / vdutoR.area;
    else
        VTemper = 0.;

    perdaEstratL = 0.;
    perdaEstratG = 0.;

    estadoPig = 0;
    estadoPigini = 0;
    dtPig = dt;
    alfPigE = 1. - alf;
    alfPigER = alfPigE;
    alfPigD = alfPigE;
    betPigE = bet;
    betPigD = bet;
    alfPigEini = 1. - alf;
    alfPigDini = alfPigE;
    betPigEini = bet;
    betPigDini = bet;
    velPig = 0.;
    velPigini = 0.;
    razPig = 0.;
    razPigini = 0.;
    RazAreaPig = 0.;
    DelPig = 0.;
    VazaPig = 0.;
    cdpig = 0.;
    indpig = -1;

    VolLeveST = ((alf * flui.MasEspGas(pres, temp) / (1.225 * flui.Deng) /*flui.MasEspGas(1., 15.6)*/) +
                 (1. - alf) * (1 - bet) * (1. - flui.BSW) * flui.RS(pres, temp) / flui.BOFunc(pres, temp));
    VolPesaST = (1. - alf) * (1 - bet) * (1. - flui.BSW) / flui.BOFunc(pres, temp);
    VolAguaST = (1. - alf) * (1 - bet) * flui.BSW;

    DmasschokeG = 0.;
    DmasschokeL = 0.;
    DmasschokeC = 0.;

    chutearranjo = -1.;

    presBuf = pres;
    presLiniBuf = presLini;
    presLBuf = presL;
    presRBuf = presR;
    presauxBuf = presaux;
    MCBuf = MC;
    MRBuf = MR;
    MliqiniRBuf = MliqiniR;
    MRiniBuf = MRini;
    MLBuf = ML;
    MliqiniBuf = Mliqini;
    MliqiniLBuf = MliqiniL;
    corrigeMassaPres = 0.;
    fluxcalmed = 0.;

    termoHidro = 0.;
    termoFric = 0.;
    QLLini = QLL;
    QLRini = QLR;
    QLini = QL;
    QGini = QG;
    presauxRini = presauxR;
    presauxLini = presauxL;
    fontemassLLini = fontemassLL;
    fontemassGLini = fontemassGL;
    fontemassCLini = fontemassCL;
    fontemassLRini = fontemassLR;
    fontemassGRini = fontemassGR;
    fontemassCRini = fontemassCR;
    transmassLini = transmassL;
    transmassRini = transmassR;
    FonteMudaFaseini = FonteMudaFase;
    DTransDt1ini = DTransDt1;
    DTransDt0ini = DTransDt0;
    DTransDxRini = DTransDxR;
    DTransDxLini = DTransDxL;
    coefTransBetini = coefTransBet;
    CoefDTRini = CoefDTR;
    CoefDTLini = CoefDTL;
    VTemperini = VTemper;

    termRug = pow(duto.rug / duto.dia / 3.7, 1.11);

    angEsq = 0.;
    angDir = 0.;

    mudaDT = 0;
    mudaDTL = 0;

    dpdt = 0.;
    dpdtIni = 0.;
    m2d = 0.;
    m2dL = 0.;
    d2pdt2 = 0.;

    alfPigERini = alfPigER;
    indpigini = indpig;
    betIini = betI;
    betRIini = betRI;
    betLIini = betLI;

    estabCol = 0;
    mudaArea = 0;
    nMol = 0.;
    nMolIni = 0.;
    dR = 0.;
    indGeom = 0;
    dPdLHidro = 1.;
    dPdLFric = 1.;
    dTdLCor = 1.;
    inddPdLHidro = -1;
    inddPdLFric = -1;
    inddTdLCor = -1;

    reiniciaAlf = 0;
    reiniciaBet = 0;
    reiniciaPig = 0;

    correrGlobHol = 1;
    miniTabAtraso = 0;
    modoParafina = 0;
    parafinado = 0;
    deltaPar = 0;

    fluxCal2D = 0.;
    C2 = 0.;
    C3 = 0.;
    diamInterIni = duto.a;
    porosoPar = 0.;
    MW_wax = 0.;
    rhoWaxLiq = 0.;
    Sum_dCwaxdT = 0.;

    correlacaoMR2 = -1;

    fonteCompartilhada = 0;
    multiFComp = 1.0;

    fluxcalAcopRedeP = 0.;
    resAcopRedeP = 0.;

    fonteCal = 0.;
    detParCel = detalhaParafina();
}

Cel::Cel(const Cel &vcel) : TL(2), local(2, 6) { // construtor por copia

    // Solver de Hidratos
    j_H = vcel.j_H;
    j_G = vcel.j_G;
    j_W = vcel.j_W;
    V_h = vcel.V_h;
    V_w = vcel.V_w;
    V_g = vcel.V_g;
    FVH = vcel.FVH;

    massa_hidrato = vcel.massa_hidrato;
    agua_consumida = vcel.agua_consumida;
    gas_consumido = vcel.gas_consumido;

    agua_consumida_massa = vcel.agua_consumida_massa;
    gas_consumido_massa = vcel.gas_consumido_massa;

    agua_consumida_massa_step = vcel.agua_consumida_massa_step;
    gas_consumido_massa_step = vcel.gas_consumido_massa_step;

    // modeloIII
    V_h_total = vcel.V_h_total;
    V_h_dep = vcel.V_h_dep;
    V_h_disp = vcel.V_h_disp;

    e_dep = vcel.e_dep;
    D_h_eff = vcel.D_h_eff;
    A_eff = vcel.A_eff;
    phi_h_disp = vcel.phi_h_disp;

    phi_h_disp = 0.0;
    phi_h_eff = vcel.phi_h_eff;
    mu_rel_slurry = vcel.mu_rel_slurry;
    mu_base_liq = vcel.mu_base_liq;
    mu_slurry = vcel.mu_slurry;

    vg1dSP = vcel.vg1dSP;
    dutoL = vcel.dutoL;
    duto = vcel.duto;
    dutoR = vcel.dutoR;
    flui = vcel.flui;
    flui.posic = vcel.posic;
    fluiL = vcel.fluiL;
    fluiR = vcel.fluiR;
    fluicol = vcel.fluicol;
    acsr = vcel.acsr;
    acsrL = vcel.acsrL;
    calor = vcel.calor;
    tempR = vcel.tempR;
    temp = vcel.temp;
    tempL = vcel.tempL;
    tempini = vcel.tempini;
    presL = vcel.presL;
    presLini = vcel.presLini;
    pres = vcel.pres;
    presini = vcel.presini;
    presauxL = vcel.presauxL;
    presaux = vcel.presaux;
    dpresaux = vcel.dpresaux;
    presauxR = vcel.presauxR;
    presR = vcel.presR;
    dxL = vcel.dxL;
    dx = vcel.dx;
    dxR = vcel.dxR;
    razdxTM = vcel.razdxTM;
    razdxTM0 = vcel.razdxTM0;
    dt = vcel.dt;
    dt1 = vcel.dt1;
    dt2 = vcel.dt2;
    dTdt = vcel.dTdt;
    dTdtIni = vcel.dTdtIni;
    dTdtL = vcel.dTdtL;
    posic = vcel.posic;
    ML = vcel.ML;
    MC = vcel.MC;
    MR = vcel.MR;
    MRini = vcel.MRini;
    presRini = vcel.presRini;
    MCini = vcel.MCini;
    MLini = vcel.MLini;
    Mliqini0 = vcel.Mliqini0;
    MliqiniL0 = vcel.MliqiniL0;
    MComp = vcel.MComp;
    tempini = vcel.tempini;
    tempLini = vcel.tempLini;
    tempRini = vcel.tempRini;
    presauxini = vcel.presauxini;
    alfL = vcel.alfL;
    alf = vcel.alf;
    alfIter = vcel.alfIter;
    alfR = vcel.alfR;
    betL = vcel.betL;
    bet = vcel.bet;
    betR = vcel.betR;
    betLI = vcel.betLI;
    betI = vcel.betI;
    betRI = vcel.betRI;
    MliqiniL = vcel.MliqiniL;
    Mliqini = vcel.Mliqini;
    MliqiniR = vcel.MliqiniR;
    MliqiniR0 = vcel.MliqiniR0;
    alfLini = vcel.alfLini;
    alfini = vcel.alfini;
    alfRini = vcel.alfRini;
    betLini = vcel.betLini;
    betini = vcel.betini;
    betRini = vcel.betRini;
    FW = vcel.FW;
    FWini = vcel.FWini;
    TL = vcel.TL;
    local = vcel.local;
    massfonteCH = vcel.massfonteCH;
    c0 = vcel.c0;
    ud = vcel.ud;
    c0ini = vcel.c0ini;
    udini = vcel.udini;
    c0Spare = vcel.c0Spare;
    udSpare = vcel.udSpare;
    transmassL = vcel.transmassL;
    transmassR = vcel.transmassR;
    FonteMudaFase = vcel.FonteMudaFase;
    DTransDt1 = vcel.DTransDt1;
    DTransDt0 = vcel.DTransDt0;
    DTransDx = vcel.DTransDx;
    DTransDxR = vcel.DTransDxR;
    DTransDxL = vcel.DTransDxL;
    DTransDxLp = vcel.DTransDxLp;
    DTransDxRp = vcel.DTransDxRp;
    DtransDxLinear = vcel.DtransDxLinear;
    DTransDxLpini = vcel.DTransDxLpini;
    DTransDxRpini = vcel.DTransDxRpini;
    DTransDtp = vcel.DTransDtp;
    DTransDtpL = vcel.DTransDtpL;
    DTransDtpini = vcel.DTransDtpini;
    DTransDtT = vcel.DTransDtT;
    DTransDtTL = vcel.DTransDtTL;
    CoefDTR = vcel.CoefDTR;
    CoefDTL = vcel.CoefDTL;
    fontedissolv = vcel.fontedissolv;
    ativaDeri = vcel.ativaDeri;
    coefTransBet = vcel.coefTransBet;
    TMModel = vcel.TMModel;
    TMModelL = vcel.TMModelL;
    fontemassLL = vcel.fontemassLL;
    fontemassGL = vcel.fontemassGL;
    fontemassCL = vcel.fontemassCL;
    fontemassLR = vcel.fontemassLR;
    fontemassGR = vcel.fontemassGR;
    fontemassCR = vcel.fontemassCR;
    dpB = vcel.dpB;
    potB = vcel.potB;
    potBT = vcel.potBT;
    potTermo = vcel.potTermo;
    dpBL = vcel.dpBL;
    potBL = vcel.potBL;
    potBTL = vcel.potBTL;
    term1L = vcel.term1L;
    term2L = vcel.term2L;
    term1 = vcel.term1;
    term2 = vcel.term2;
    term1R = vcel.term1R;
    term2R = vcel.term2R;
    transic = vcel.transic;
    transic0 = vcel.transic0;
    QL = vcel.QL;
    QG = vcel.QG;
    QLL = vcel.QLL;
    QLR = vcel.QLR;
    QLRini = vcel.QLRini;
    cinematico = vcel.cinematico;
    arranjo = vcel.arranjo;
    arranjoR = vcel.arranjoR;
    VTemper = vcel.VTemper;

    rpL = vcel.rpL;
    rpC = vcel.rpC;
    rpR = vcel.rpR;
    rcL = vcel.rcL;
    rcC = vcel.rcC;
    rcR = vcel.rcR;
    rgL = vcel.rgL;
    rgC = vcel.rgC;
    rgR = vcel.rgR;
    drhogdt = vcel.drhogdt;

    rgLi = vcel.rgLi;
    rgCi = vcel.rgCi;
    rgRi = vcel.rgRi;
    rpLi = vcel.rpLi;
    rpCi = vcel.rpCi;
    rpRi = vcel.rpRi;
    rcLi = vcel.rcLi;
    rcCi = vcel.rcCi;
    rcRi = vcel.rcRi;

    mipC = vcel.mipC;
    micC = vcel.micC;
    migC = vcel.migC;
    mipR = vcel.mipR;
    micR = vcel.micR;
    migR = vcel.migR;

    perdaEstratL = vcel.perdaEstratL;
    perdaEstratG = vcel.perdaEstratG;

    estadoPig = vcel.estadoPig;
    estadoPigini = vcel.estadoPigini;
    dtPig = vcel.dtPig;
    alfPigE = vcel.alfPigE;
    alfPigER = vcel.alfPigER;
    alfPigD = vcel.alfPigD;
    betPigE = vcel.betPigE;
    betPigD = vcel.betPigD;
    alfPigEini = vcel.alfPigEini;
    alfPigDini = vcel.alfPigDini;
    betPigEini = vcel.betPigEini;
    betPigDini = vcel.betPigDini;
    velPig = vcel.velPig;
    velPigini = vcel.velPigini;
    razPig = vcel.razPig;
    razPigini = vcel.razPigini;
    RazAreaPig = vcel.RazAreaPig;
    DelPig = vcel.DelPig;
    VazaPig = vcel.VazaPig;
    cdpig = vcel.cdpig;
    indpig = vcel.indpig;

    VolLeveST = vcel.VolLeveST;
    VolPesaST = vcel.VolPesaST;
    VolAguaST = vcel.VolAguaST;

    DmasschokeG = vcel.DmasschokeG;
    DmasschokeL = vcel.DmasschokeL;
    DmasschokeC = vcel.DmasschokeC;

    chutearranjo = vcel.chutearranjo;
    presBuf = vcel.presBuf;
    presLiniBuf = vcel.presLiniBuf;
    presLBuf = vcel.presLBuf;
    presRBuf = vcel.presRBuf;
    presauxBuf = vcel.presauxBuf;
    MCBuf = vcel.MCBuf;
    MRBuf = vcel.MRBuf;
    MliqiniRBuf = vcel.MliqiniRBuf;
    MRiniBuf = vcel.MRiniBuf;
    MLBuf = vcel.MLBuf;
    MliqiniBuf = vcel.MliqiniBuf;
    MliqiniLBuf = vcel.MliqiniLBuf;
    corrigeMassaPres = 0.;
    fluxcalmed = vcel.fluxcalmed;

    termoHidro = vcel.termoHidro;
    termoFric = vcel.termoFric;
    QLLini = vcel.QLLini;
    QLRini = vcel.QLRini;
    QLini = vcel.QLini;
    QGini = vcel.QGini;
    presauxRini = vcel.presauxRini;
    presauxLini = vcel.presauxLini;
    fontemassLLini = vcel.fontemassLLini;
    fontemassGLini = vcel.fontemassGLini;
    fontemassCLini = vcel.fontemassCLini;
    fontemassLRini = vcel.fontemassLRini;
    fontemassGRini = vcel.fontemassGRini;
    fontemassCRini = vcel.fontemassCRini;
    transmassLini = vcel.transmassLini;
    transmassRini = vcel.transmassRini;
    FonteMudaFaseini = vcel.FonteMudaFaseini;
    DTransDt1ini = vcel.DTransDt1ini;
    DTransDt0ini = vcel.DTransDt0ini;
    DTransDxRini = vcel.DTransDxRini;
    DTransDxLini = vcel.DTransDxLini;
    coefTransBetini = vcel.coefTransBetini;
    CoefDTRini = vcel.CoefDTRini;
    CoefDTLini = vcel.CoefDTLini;
    VTemperini = vcel.VTemperini;

    termRug = vcel.termRug;

    angEsq = vcel.angEsq;
    angDir = vcel.angDir;

    mudaDT = 0;
    mudaDTL = 0;
    dpdt = vcel.dpdt;
    dpdtIni = vcel.dpdtIni;
    m2d = vcel.m2d;
    m2dL = vcel.m2dL;
    d2pdt2 = vcel.d2pdt2;

    alfPigERini = vcel.alfPigERini;
    indpigini = vcel.indpigini;
    betIini = vcel.betIini;
    betRIini = vcel.betRIini;
    betLIini = vcel.betLIini;

    estabCol = vcel.estabCol;

    mudaArea = vcel.mudaArea;

    nMol = vcel.nMol;
    nMolIni = vcel.nMolIni;

    dR = vcel.dR;

    indGeom = vcel.indGeom;

    dPdLHidro = vcel.dPdLHidro;
    dPdLFric = vcel.dPdLFric;
    dTdLCor = vcel.dTdLCor;

    inddPdLHidro = vcel.inddPdLHidro;
    inddPdLFric = vcel.inddPdLFric;
    inddTdLCor = vcel.inddTdLCor;

    reiniciaAlf = vcel.reiniciaAlf;
    reiniciaBet = vcel.reiniciaBet;
    reiniciaPig = vcel.reiniciaPig;

    correrGlobHol = 1;

    miniTabAtraso = vcel.miniTabAtraso;

    modoParafina = vcel.modoParafina;
    parafinado = vcel.parafinado;
    deltaPar = vcel.deltaPar;

    fluxCal2D = vcel.fluxCal2D;

    C2 = vcel.C2;
    C3 = vcel.C3;
    diamInterIni = vcel.diamInterIni;
    porosoPar = vcel.porosoPar;
    MW_wax = vcel.MW_wax;
    rhoWaxLiq = vcel.rhoWaxLiq;
    Sum_dCwaxdT = vcel.Sum_dCwaxdT;

    correlacaoMR2 = vcel.correlacaoMR2;

    fonteCompartilhada = vcel.fonteCompartilhada;
    multiFComp = vcel.multiFComp;

    fluxcalAcopRedeP = vcel.fluxcalAcopRedeP;
    resAcopRedeP = vcel.resAcopRedeP;

    fonteCal = vcel.fonteCal;
}

Cel &Cel::operator=(const Cel &vcel) {
    if (this != &vcel) {

        // Solver de Hidratos
        j_H = vcel.j_H;
        j_G = vcel.j_G;
        j_W = vcel.j_W;
        V_h = vcel.V_h;
        V_w = vcel.V_w;
        V_g = vcel.V_g;
        FVH = vcel.FVH;

        massa_hidrato = vcel.massa_hidrato;
        agua_consumida = vcel.agua_consumida;
        gas_consumido = vcel.gas_consumido;

        agua_consumida_massa = vcel.agua_consumida_massa;
        gas_consumido_massa = vcel.gas_consumido_massa;

        agua_consumida_massa_step = vcel.agua_consumida_massa_step;
        gas_consumido_massa_step = vcel.gas_consumido_massa_step;

        // modeloIII
        V_h_total = vcel.V_h_total;
        V_h_dep = vcel.V_h_dep;
        V_h_disp = vcel.V_h_disp;

        e_dep = vcel.e_dep;
        D_h_eff = vcel.D_h_eff;
        A_eff = vcel.A_eff;
        phi_h_disp = vcel.phi_h_disp;

        phi_h_disp = 0.0;
        phi_h_eff = vcel.phi_h_eff;
        mu_rel_slurry = vcel.mu_rel_slurry;
        mu_base_liq = vcel.mu_base_liq;
        mu_slurry = vcel.mu_slurry;

        vg1dSP = vcel.vg1dSP;
        dutoL = vcel.dutoL;
        duto = vcel.duto;
        dutoR = vcel.dutoR;
        flui = vcel.flui;
        flui.posic = vcel.posic;
        fluiL = vcel.fluiL;
        fluiR = vcel.fluiR;
        fluicol = vcel.fluicol;
        acsr = vcel.acsr;
        acsrL = vcel.acsrL;
        calor = vcel.calor;
        tempR = vcel.tempR;
        temp = vcel.temp;
        tempL = vcel.tempL;
        tempini = vcel.tempini;
        presL = vcel.presL;
        presLini = vcel.presLini;
        pres = vcel.pres;
        presini = vcel.presini;
        presauxL = vcel.presauxL;
        presaux = vcel.presaux;
        dpresaux = vcel.dpresaux;
        presauxR = vcel.presauxR;
        presR = vcel.presR;
        dxL = vcel.dxL;
        dx = vcel.dx;
        dxR = vcel.dxR;
        razdxTM = vcel.razdxTM;
        razdxTM0 = vcel.razdxTM0;
        dt = vcel.dt;
        dt1 = vcel.dt1;
        dt2 = vcel.dt2;
        dTdt = vcel.dTdt;
        dTdtIni = vcel.dTdtIni;
        dTdtL = vcel.dTdtL;
        posic = vcel.posic;
        ML = vcel.ML;
        MC = vcel.MC;
        MR = vcel.MR;
        MRini = vcel.MRini;
        alfL = vcel.alfL;
        alf = vcel.alf;
        alfIter = vcel.alfIter;
        alfR = vcel.alfR;
        betL = vcel.betL;
        bet = vcel.bet;
        betR = vcel.betR;
        betLI = vcel.betLI;
        betI = vcel.betI;
        betRI = vcel.betRI;
        FW = vcel.FW;
        FWini = vcel.FWini;
        MliqiniL = vcel.MliqiniL;
        Mliqini = vcel.Mliqini;
        MliqiniR = vcel.MliqiniR;
        MliqiniR0 = vcel.MliqiniR0;
        presRini = vcel.presRini;
        MCini = vcel.MCini;
        MLini = vcel.MLini;
        Mliqini0 = vcel.Mliqini0;
        MliqiniL0 = vcel.MliqiniL0;
        MComp = vcel.MComp;
        tempini = vcel.tempini;
        tempLini = vcel.tempLini;
        tempRini = vcel.tempRini;
        presauxini = vcel.presauxini;
        alfLini = vcel.alfLini;
        alfini = vcel.alfini;
        alfRini = vcel.alfRini;
        betLini = vcel.betLini;
        betini = vcel.betini;
        betRini = vcel.betRini;
        TL = vcel.TL;
        local = vcel.local;
        massfonteCH = vcel.massfonteCH;
        c0 = vcel.c0;
        ud = vcel.ud;
        c0ini = vcel.c0ini;
        udini = vcel.udini;
        c0Spare = vcel.c0Spare;
        udSpare = vcel.udSpare;
        transmassL = vcel.transmassL;
        transmassR = vcel.transmassR;
        FonteMudaFase = vcel.FonteMudaFase;
        DTransDt1 = vcel.DTransDt1;
        DTransDt0 = vcel.DTransDt0;
        DTransDx = vcel.DTransDx;
        DTransDxR = vcel.DTransDxR;
        DTransDxL = vcel.DTransDxL;
        DTransDxLp = vcel.DTransDxLp;
        DTransDxRp = vcel.DTransDxRp;
        DtransDxLinear = vcel.DtransDxLinear;
        DTransDxLpini = vcel.DTransDxLpini;
        DTransDxRpini = vcel.DTransDxRpini;
        DTransDtp = vcel.DTransDtp;
        DTransDtpL = vcel.DTransDtpL;
        DTransDtpini = vcel.DTransDtpini;
        DTransDtT = vcel.DTransDtT;
        DTransDtTL = vcel.DTransDtTL;
        fontedissolv = vcel.fontedissolv;
        ativaDeri = vcel.ativaDeri;
        CoefDTR = vcel.CoefDTR;
        CoefDTL = vcel.CoefDTL;
        coefTransBet = vcel.coefTransBet;
        TMModel = vcel.TMModel;
        TMModelL = vcel.TMModelL;
        fontemassLL = vcel.fontemassLL;
        fontemassGL = vcel.fontemassGL;
        fontemassCL = vcel.fontemassCL;
        fontemassLR = vcel.fontemassLR;
        fontemassGR = vcel.fontemassGR;
        fontemassCR = vcel.fontemassCR;
        dpB = vcel.dpB;
        potB = vcel.potB;
        potBT = vcel.potBT;
        potTermo = vcel.potTermo;
        dpBL = vcel.dpBL;
        potBL = vcel.potBL;
        potBTL = vcel.potBTL;
        term1L = vcel.term1L;
        term2L = vcel.term2L;
        term1 = vcel.term1;
        term2 = vcel.term2;
        term1R = vcel.term1R;
        term2R = vcel.term2R;
        transic = vcel.transic;
        transic0 = vcel.transic0;
        QL = vcel.QL;
        QG = vcel.QG;
        QLL = vcel.QLL;
        QLR = vcel.QLR;
        QLRini = vcel.QLRini;
        cinematico = vcel.cinematico;
        arranjo = vcel.arranjo;
        arranjoR = vcel.arranjoR;
        VTemper = vcel.VTemper;

        rpL = vcel.rpL;
        rpC = vcel.rpC;
        rpR = vcel.rpR;
        rcL = vcel.rcL;
        rcC = vcel.rcC;
        rcR = vcel.rcR;
        rgL = vcel.rgL;
        rgC = vcel.rgC;
        rgR = vcel.rgR;
        drhogdt = vcel.drhogdt;

        rgLi = vcel.rgLi;
        rgCi = vcel.rgCi;
        rgRi = vcel.rgRi;
        rpLi = vcel.rpLi;
        rpCi = vcel.rpCi;
        rpRi = vcel.rpRi;
        rcLi = vcel.rcLi;
        rcCi = vcel.rcCi;
        rcRi = vcel.rcRi;

        mipC = vcel.mipC;
        micC = vcel.micC;
        migC = vcel.migC;
        mipR = vcel.mipR;
        micR = vcel.micR;
        migR = vcel.migR;

        perdaEstratL = vcel.perdaEstratL;
        perdaEstratG = vcel.perdaEstratG;

        estadoPig = vcel.estadoPig;
        estadoPigini = vcel.estadoPigini;
        dtPig = vcel.dtPig;
        alfPigE = vcel.alfPigE;
        alfPigER = vcel.alfPigER;
        alfPigD = vcel.alfPigD;
        betPigE = vcel.betPigE;
        betPigD = vcel.betPigD;
        alfPigEini = vcel.alfPigEini;
        alfPigDini = vcel.alfPigDini;
        betPigEini = vcel.betPigEini;
        betPigDini = vcel.betPigDini;
        velPig = vcel.velPig;
        velPigini = vcel.velPigini;
        razPig = vcel.razPig;
        razPigini = vcel.razPigini;
        RazAreaPig = vcel.RazAreaPig;
        DelPig = vcel.DelPig;
        VazaPig = vcel.VazaPig;
        cdpig = vcel.cdpig;
        indpig = vcel.indpig;

        VolLeveST = vcel.VolLeveST;
        VolPesaST = vcel.VolPesaST;
        VolAguaST = vcel.VolAguaST;

        DmasschokeG = vcel.DmasschokeG;
        DmasschokeL = vcel.DmasschokeL;
        DmasschokeC = vcel.DmasschokeC;

        chutearranjo = vcel.chutearranjo;

        presBuf = vcel.presBuf;
        presLiniBuf = vcel.presLiniBuf;
        presLBuf = vcel.presLBuf;
        presRBuf = vcel.presRBuf;
        presauxBuf = vcel.presauxBuf;
        MCBuf = vcel.MCBuf;
        MRBuf = vcel.MRBuf;
        MliqiniRBuf = vcel.MliqiniRBuf;
        MRiniBuf = vcel.MRiniBuf;
        MLBuf = vcel.MLBuf;
        MliqiniBuf = vcel.MliqiniBuf;
        MliqiniLBuf = vcel.MliqiniLBuf;
        corrigeMassaPres = 0.;
        fluxcalmed = vcel.fluxcalmed;

        termoHidro = vcel.termoHidro;
        termoFric = vcel.termoFric;
        QLLini = vcel.QLLini;
        QLRini = vcel.QLRini;
        QLini = vcel.QLini;
        QGini = vcel.QGini;
        presauxRini = vcel.presauxRini;
        presauxLini = vcel.presauxLini;
        fontemassLLini = vcel.fontemassLLini;
        fontemassGLini = vcel.fontemassGLini;
        fontemassCLini = vcel.fontemassCLini;
        fontemassLRini = vcel.fontemassLRini;
        fontemassGRini = vcel.fontemassGRini;
        fontemassCRini = vcel.fontemassCRini;
        transmassLini = vcel.transmassLini;
        transmassRini = vcel.transmassRini;
        FonteMudaFaseini = vcel.FonteMudaFaseini;
        DTransDt1ini = vcel.DTransDt1ini;
        DTransDt0ini = vcel.DTransDt0ini;
        DTransDxRini = vcel.DTransDxRini;
        DTransDxLini = vcel.DTransDxLini;
        coefTransBetini = vcel.coefTransBetini;
        CoefDTRini = vcel.CoefDTRini;
        CoefDTLini = vcel.CoefDTLini;
        VTemperini = vcel.VTemperini;

        termRug = vcel.termRug;

        angEsq = vcel.angEsq;
        angDir = vcel.angDir;

        mudaDT = vcel.mudaDT;
        mudaDTL = vcel.mudaDTL;
        dpdt = vcel.dpdt;
        dpdtIni = vcel.dpdtIni;
        m2d = vcel.m2d;
        m2dL = vcel.m2dL;
        d2pdt2 = vcel.d2pdt2;

        alfPigERini = vcel.alfPigERini;
        indpigini = vcel.indpigini;
        betIini = vcel.betIini;
        betRIini = vcel.betRIini;
        betLIini = vcel.betLIini;

        estabCol = vcel.estabCol;

        mudaArea = vcel.mudaArea;

        nMol = vcel.nMol;
        nMolIni = vcel.nMolIni;

        dR = vcel.dR;

        indGeom = vcel.indGeom;

        dPdLHidro = vcel.dPdLHidro;
        dPdLFric = vcel.dPdLFric;
        dTdLCor = vcel.dTdLCor;

        inddPdLHidro = vcel.inddPdLHidro;
        inddPdLFric = vcel.inddPdLFric;
        inddTdLCor = vcel.inddTdLCor;

        reiniciaAlf = vcel.reiniciaAlf;
        reiniciaBet = vcel.reiniciaBet;
        reiniciaPig = vcel.reiniciaPig;

        correrGlobHol = vcel.correrGlobHol;

        miniTabAtraso = vcel.miniTabAtraso;

        modoParafina = vcel.modoParafina;
        parafinado = vcel.parafinado;
        deltaPar = vcel.deltaPar;

        fluxCal2D = vcel.fluxCal2D;

        C2 = vcel.C2;
        C3 = vcel.C3;
        diamInterIni = vcel.diamInterIni;
        porosoPar = vcel.porosoPar;
        MW_wax = vcel.MW_wax;
        rhoWaxLiq = vcel.rhoWaxLiq;
        Sum_dCwaxdT = vcel.Sum_dCwaxdT;

        correlacaoMR2 = vcel.correlacaoMR2;

        fonteCompartilhada = vcel.fonteCompartilhada;
        multiFComp = vcel.multiFComp;

        fluxcalAcopRedeP = vcel.fluxcalAcopRedeP;
        resAcopRedeP = vcel.resAcopRedeP;

        fonteCal = vcel.fonteCal;
    }
    return *this;
}

double Cel::Rey(double dia, double vel,
                double rho, double vis) {
    return dia * fabs(vel) * rho / (vis * 1e-3);
}
double Cel::fric(double re, double eps, int tipo) {

    double val;
    if (fabs(re) > 1e-5) {
        if (re > 2400) {
            if (tipo == -1)
                val = 6.9 / fabs(re) + pow(eps / 3.7, 1.11);
            else
                val = 6.9 / fabs(re) + termRug;
            val = -1.8 * (log(val) / 2.30259);
            val *= val;
            val = 1 / val;
            for (int konta = 0; konta < 2; konta++) {
                val = -2. * log(2.51 / fabs(re * sqrt(val)) + eps / 3.7) / 2.30259;
                val *= val;
                val = 1. / val;
            }
            val /= 4.;
        } else
            val = 16. / fabs(re);
    } else
        val = 0.;
    return val;
}

void Cel::avancalf(int &reinicia, int ncel) {

    double convec = 0.;
    double term1Mass = 0.;
    dt1 = dt;
    if (alf <= (*vg1dSP).CritCond && transmassR < 0.) {
        transmassR = 0.;
        FonteMudaFase = 0.;
    }
    double MliqCR = QLR * betRI * rcRi;
    double MliqC = QL * betI * rcCi;
    double termoPtrans = 0.;
    double termoDrholP = 0.;
    double termoTtrans = 0.;
    double termoDrholT = 0.;
    double jmix = (fabs(QL) + fabs(QG)) / duto.area;

    if (TMModel == 0) {
        termoPtrans = -(DTransDtp / rpC) * m2d * (dpdt);
        if (flui.flashCompleto != 2 || miniTabAtraso > 0)
            termoDrholP = (duto.area * (1. - alf) * (1 - bet) / rpC) * ((rpC - flui.MasEspLiq(presini * 0.999, temp)) / (0.001 * presini)) *
                          m2d * (dpdt)*ativaDeri;
        else {
            ProFlu flud = flui;
            flud.atualizaPropComp(presini * 0.999, temp, flud.dCalculatedBeta, flud.oCalculatedLiqComposition,
                                  flud.oCalculatedVapComposition, 0);
            termoDrholP = (duto.area * (1. - alf) * (1 - bet) / rpC) * ((rpC - flud.MasEspLiq(presini * 0.999, temp)) / (0.001 * presini)) *
                          m2d * (dpdt)*ativaDeri;
        }

        double dTtemp = dTdt;
        if (posic == ncel)
            dTtemp = dTdtL;
        termoTtrans = -(DTransDtT / rpC) * (dTtemp)*mudaDT;
        termoDrholT = (duto.area * (1. - alf) * (1 - bet) / rpC) * ((rpC - flui.MasEspLiq(presini, temp * 0.999)) / (0.001 * temp)) *
                      (dTtemp)*ativaDeri * mudaDT;
    }
    if (posic < ncel) {
        convec = (1 / rpC) * MliqiniR / dx - (1 / rpC) * Mliqini / dx + (1 / rcC - 1 / rpC) * MliqCR / dx - (1 / rcC - 1 / rpC) * MliqC / dx;
        double term1TransBet = -((MliqCR / rcC) / dx - (MliqC / rcC) / dx) / duto.area + fontemassCR / (dx * rcC * duto.area);
        term1Mass = (convec - fontemassLR / (dx * rpC) - fontemassCR / (dx * rcC) + (transmassR) / rpC +
                     coefTransBet * term1TransBet / rpC + termoPtrans + termoDrholP + termoTtrans + termoDrholT) /
                    duto.area;
    } else {
        convec = -(1 / rpC) * Mliqini / dx - (1 / rcC - 1 / rpC) * MliqC / dx;
        term1Mass = (convec - fontemassLR / (dx * rpC) - fontemassCR / (dx * rcC) + termoPtrans + termoDrholP +
                     termoTtrans + termoDrholT) /
                    duto.area;
        CoefDTR = 0.;
        CoefDTL = 0.;
        coefTransBet = 0.;
    }
    if (term1Mass > 0 && alfini > 1 - (*vg1dSP).localtiny)
        term1Mass = 0.;
    if (term1Mass < 0 && alfini < (*vg1dSP).localtiny)
        term1Mass = 0.;

    alf = alfini +
          (term1Mass / (1 + CoefDTR / (rpC * duto.area))) * dt;

    if (((alf <= (*vg1dSP).localtiny) && (alf >= -(*vg1dSP).localtiny)) ||
        ((alf <= 1e-3) && (alf >= -1e-3) && (fabs(QG) / duto.area) <= 1e-2)) {
        alf = fabs(0.);
    } else if (alf < -(*vg1dSP).localtiny) {
        double dtaux;
        dtaux = -alfini / (term1Mass / (1 + CoefDTR / (rpC * duto.area)));
        if (reinicia == 0 && jmix > (*vg1dSP).CritDTMin) {
            dt1 = dtaux;
            reiniciaAlf = -1;
            alf = fabs(0.);
        } else
            alfIter = alf = fabs(0.);
    } else if (((alf >= (1. - (*vg1dSP).localtiny) && alf <= (1. + (*vg1dSP).localtiny))) ||
               ((alf >= (1. - 1e-3) && alf <= (1. + 1e-3)) && (fabs(QL) / duto.area) <= 1e-2)) {
        alf = 1.;
    } else if (alf > (1. + (*vg1dSP).localtiny)) {
        double dtaux;
        dtaux = (1. - alfini) / (term1Mass / (1 + CoefDTR / (rpC * duto.area)));
        if (reinicia == 0 && jmix > (*vg1dSP).CritDTMin) {
            dt1 = dtaux;
            reiniciaAlf = -1;
            alf = 1.;
        } else
            alf = 1.;
    }

    alfIter = alf;
}

void Cel::avancbet(int &reinicia, int ncel) {

    double convec = 0.;
    double term1Mass = 0.;

    double fator1 = rcC * duto.area;
    double MliqCR = QLR * betRI * rcRi;
    double MliqC = QL * betI * rcCi;
    double jmix = fabs(QL) / duto.area;

    dt2 = dt;

    if (posic < ncel) {
        if (alfini <= 1. - (*vg1dSP).localtiny) {
            convec = MliqCR / dx - MliqC / dx;
            term1Mass = (-convec + fator1 * betini * alf / dt - fator1 * betini * alfini / dt + fontemassCR / dx) / (fator1 - fator1 * alfini);
        } else
            term1Mass = 0.;
    } else {
        double dxncel = 1. * dx;
        if (alfini <= 1. - (*vg1dSP).localtiny) {
            convec = (-MliqC / dx) / dxncel;
            term1Mass = (-convec + fator1 * betini * (alf - alfini) / dt + fontemassCR / dx) / (fator1 * (1 - alfini));
        } else
            term1Mass = 0.;
    }

    if (term1Mass > 0 && betini > 1 - (*vg1dSP).localtiny)
        term1Mass = 0.;
    if (term1Mass < 0 && betini < (*vg1dSP).localtiny)
        term1Mass = 0.;
    bet = betini + term1Mass * dt;

    if (alfini >= 1 - (*vg1dSP).localtiny)
        bet = 0.;

    if (((bet <= (*vg1dSP).localtiny) && (bet >= -(*vg1dSP).localtiny))) {
        bet = fabs(0.);
    } else if (bet < -(*vg1dSP).localtiny) {
        double dtaux;

        dtaux = -betini / term1Mass;
        if (reinicia == 0 && jmix > (*vg1dSP).CritDTMin) {
            dt2 = dtaux;
            reiniciaBet = -1;
            bet = fabs(0.);
        } else
            bet = fabs(0.);
    } else if ((bet >= (1. - (*vg1dSP).localtiny) && bet <= (1. + (*vg1dSP).localtiny))) {
        bet = 1.;
    } else if (bet > (1. + (*vg1dSP).localtiny)) {
        double dtaux;
        dtaux = (1. - betini) / term1Mass;
        if (reinicia == 0 && jmix > (*vg1dSP).CritDTMin) {
            dt2 = dtaux;
            reiniciaBet = -1;
            bet = 1.;
        } else
            bet = 1.;
    }
}

double Cel::delpJus(double presfim) {

    double dxmed = 0.5 * dx;
    double dia = duto.a;
    double area = 0.25 * M_PI * dia * dia;
    double si = duto.peri;
    double alfmed = alf;
    double betmed = bet;
    double rhog = flui.MasEspGas(presfim, temp);
    double rhol = (1. - betmed) * flui.MasEspLiq(presfim, temp) + betmed * fluicol.MasEspFlu(presfim, temp);
    double ugsmed = (MC - Mliqini) / (area * rhog);
    double ulsmed = Mliqini / (area * rhol);

    double j = ugsmed + ulsmed;

    double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    double viscmix = alfmed * flui.ViscGas(presfim, temp) + (1 - alfmed) * ((1. - betmed) * flui.ViscOleo(presfim, temp) + betmed * fluicol.VisFlu(presfim, temp));

    double re1;
    if (duto.revest == 0)
        re1 = Rey(duto.a, j, rhomix, viscmix);
    else {
        double dhid = 4 * area / si;
        re1 = Rey(dhid, j, rhomix, viscmix);
    }
    double f1 = fric(re1, duto.rug / dia);
    double gradfric = 0.5 * f1 * rhomix * (fabs(j) * j) * si * dxmed / area;
    double gradhidro = 9.82 * sin(duto.teta) * rhomix * dxmed;
    return (0 * gradfric + gradhidro) / 98066.5;
}

void Cel::GeraLocal(double presfim, int masChkSup, int ncel, double razareativa,
                    double presE, double tempE, double titE, double betE, int ciclo,
                    int modelo, int noextremo, int corrigeContSep, double areaChoke, int vexpi) {

    double deriPres = 1. * mudaDT;
    if (modelo == 0)
        deriPres = 0.;
    double deriMas = vexpi;
    if (posic != 0 && posic != ncel) {

        double AL = dutoL.area;
        double AC = AL;
        double AR = AL;
        double siL = duto.peri;
        double siC = siL;
        double siR = siL;
        double rhogL = rgL;
        double rholL = 0.0;
        double rhogC = 1.;
        double rholC = 1000.;
        double rhogR;
        double rholR = 1000.;
        double compres;
        double dzdp;
        double dpdrho = 0.;
        double drhoLdp = 0.;
        double multL = 1.;
        double multR = 1.;
        double corDRhol = 1. - 0 * mudaDTL;
        double corDRholC = 1. - 0 * mudaDT;

        if (ciclo == 0) {

            AC = duto.area;
            AR = dutoR.area;
            siL = dutoL.peri;
            siC = duto.peri;
            siR = dutoR.peri;
            rholL = rpL * (1 - betL) + rcL * betL;
            rhogC = rgC;
            rholC = rpC * (1 - bet) + rcC * bet;
            rhogR = rgR;
            rholR = rpR * (1 - betR) + rcR * betR;

            multL = ((1 / rpL) * (1 - betLI) * rpLi + (1 / rcL) * betLI * rcLi) / ((1 - betLI) * rpLi + betLI * rcLi);
            multR = ((1 / rpL) * (1 - betI) * rpCi + (1 / rcL) * betI * rcCi) / ((1 - betI) * rpCi + betI * rcCi);
            // QUESTION: Shouldn't this be rcCi instead of rcRi?
        }

        int master = 1;
        if (acsrL != 0) {
            if (((*acsrL).tipo == 5 && (*acsrL).chk.AreaGarg < (1e-3 + razareativa) * AL) || ((*acsrL).tipo == 8 && fabs((*acsrL).bvol.freq) > 1))
                master = 0;
        }
        if (master == 1) {
            if (ciclo == 0) {
                compres = (*fluiL).Zdran(presL, tempL);
                dzdp = (*fluiL).DZDP(presL, tempL);
                dpdrho = rhogL * (1 / (presL * 98066.5) - (1 / compres) * dzdp);
                dpdrho = 1. / dpdrho;
                double rholdp;
                if ((*fluiL).flashCompleto != 2 || miniTabAtraso > 0)
                    rholdp = (*fluiL).MasEspLiq(presL * 0.999, tempL);
                else {
                    ProFlu flud = (*fluiL);
                    flud.atualizaPropComp(presL * 0.999, tempL, flud.dCalculatedBeta, flud.oCalculatedLiqComposition,
                                          flud.oCalculatedVapComposition, 0);
                    rholdp = flud.MasEspLiq(presL * 0.999, tempL);
                }
                drhoLdp = (rpL - rholdp) / (0.001 * presL);

                if (alfLini <= (*vg1dSP).CritCond && transmassL < 0.)
                    transmassL = 0.;

                local[0][0] = -dxL * (1 / rhogL - 1 / rpL) * DTransDxLp;
                local[0][1] = -((1. / rhogL) * (1. - term1L) + multL * term1L) - dxL * (1 / rhogL - 1 / rpL) * term1L * DTransDxL;
                local[0][2] = dxL * AL * (((alfL / rhogL) / dpdrho) * 98066.5 + corDRhol * (deriPres * (1. - alfL) * (1 - betL) * drhoLdp / rpL + deriPres * (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtpL)) / dt -
                              dxL * (1 / rhogL - 1 / rpL) * (DTransDxLp + DTransDxRp);
                local[0][3] = (1. / rhogL) * (1 - term1) + multR * term1 - dxL * (1 / rhogL - 1 / rpL) * term1 * DTransDxR;
                local[0][4] = -dxL * (1 / rhogL - 1 / rpL) * DTransDxRp;
                local[0][5] = 0.;

                if (posic == 1 && presE > 0)
                    fontemassGL += corrigeMassaPres * dxL;

                if (TMModelL == 1 || TMModelL == 2 || TMModelL == 3) {
                    TL[0] = dxL * AL * (((alfL / rhogL) / dpdrho) * presL * 98066.5 + corDRhol * (deriPres * (1. - alfL) * (1 - betL) * drhoLdp * presL / rpL + deriPres * (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtpL * presL)) / dt +
                            (1 / rhogL - multR) * term2 -
                            (1 / rhogL - multL) * term2L +
                            (fontemassGL / rhogL + (fontemassLL / rpL + fontemassCL / rcL)) + dxL * (1 / rhogL - 1 / rpL) * transmassL;
                } else if (TMModelL == 0) {
                    TL[0] = dxL * AL * (((alfL / rhogL) / dpdrho) * presL * 98066.5 + corDRhol * (deriPres * (1. - alfL) * (1 - betL) * drhoLdp * presL / rpL + deriPres * (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtpL * presL)) / dt +
                            (1 / rhogL - multR) * term2 -
                            (1 / rhogL - multL) * term2L +
                            (fontemassGL / rhogL + (fontemassLL / rpL + fontemassCL / rcL)) +
                            dxL * (1 / rhogL - 1 / rpL) * (DTransDt1 * (1. - alfL) * (1. - betL) / dt + DTransDt0 * (1. - alfLini) * (1. - betLini) / dt + term2 * DTransDxR + term2L * DTransDxL + DtransDxLinear);
                }

                if (acsrL != 0) {
                    if ((*acsrL).tipo == 3) {
                        local[0][2] -= ((*acsrL).ipr.deriG / rhogL + ((*acsrL).ipr.deriP + (*acsrL).ipr.deriC) / rholL);
                        TL[0] -= ((*acsrL).ipr.deriG / rhogL + ((*acsrL).ipr.deriP + (*acsrL).ipr.deriC) / rholL) * presL;
                    }
                    if ((*acsrL).tipo == 15) {
                        local[0][2] -= ((*acsrL).radialPoro.DfluxIniG / rhogL + ((*acsrL).radialPoro.DfluxIni +
                                                                                 (*acsrL).radialPoro.DfluxIniA) /
                                                                                    rholL);
                        TL[0] -= ((*acsrL).radialPoro.DfluxIniG / rhogL + ((*acsrL).radialPoro.DfluxIni +
                                                                           (*acsrL).radialPoro.DfluxIniA) /
                                                                              rholL) *
                                 presL;
                    }
                    if ((*acsrL).tipo == 16) {
                        local[0][2] -= ((*acsrL).poroso2D.dados.transfer.DfluxIniG / rhogL + ((*acsrL).poroso2D.dados.transfer.DfluxIni +
                                                                                              (*acsrL).poroso2D.dados.transfer.DfluxIniA) /
                                                                                                 rholL);
                        TL[0] -= ((*acsrL).poroso2D.dados.transfer.DfluxIniG / rhogL + ((*acsrL).poroso2D.dados.transfer.DfluxIni +
                                                                                        (*acsrL).poroso2D.dados.transfer.DfluxIniA) /
                                                                                           rholL) *
                                 presL;
                    }
                }
            }
            double drhodt = (*fluiL).drhodt(presL, tempL);
            double drhoLdt = (rpL - (*fluiL).MasEspLiq(presL, tempL - 0.01)) / 0.01;
            TL[0] -= dxL * AL * ((alfL / rhogL) * drhodt + (1 - alfL) * (1 - betL) * drhoLdt / rpL + (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtTL) * dTdtL * mudaDT;

        } else {
            local[0][0] = 0.;
            local[0][1] = 0.;
            local[0][2] = 0.;
            local[0][3] = 1.;
            local[0][4] = 0.;
            local[0][5] = 0.;

            TL[0] = 0.;
        }

        if ((acsr.tipo != 5 || acsr.chk.AreaGarg >= (1e-3 + razareativa) * AC) &&
            (acsr.tipo != 8 || fabs(acsr.bvol.freq) <= 1.) && ciclo == 0) {

            double dxmed = 0.5 * (dx + dxR);
            double Amed;
            if (AC < AR)
                Amed = AC;
            else
                Amed = AR;

            double ugL = vexpi * ((MC - Mliqini) / (rhogC)) / AC;
            if (alf > (*vg1dSP).localtiny)
                ugL /= alf;
            else
                ugL = 0.;
            double ugR = vexpi * ((MR - MliqiniR) / (rhogR)) / AR;
            if (alfR > (*vg1dSP).localtiny)
                ugR /= alfR;
            else
                ugR = 0.;
            double ulL = vexpi * ((Mliqini) / (rholC)) / AC;
            if ((1. - alf) > (*vg1dSP).localtiny)
                ulL /= (1. - alf);
            else
                ulL = 0.;
            double ulR = vexpi * ((MliqiniR) / (rholR)) / AR;
            if ((1. - alfR) > (*vg1dSP).localtiny)
                ulR /= (1. - alfR);
            else
                ulR = 0.;
            local[1][1] = 0.;
            local[1][2] = -ugL * (1 - term1) / dx - ulL * term1 / dx;
            local[1][3] = -Amed * 98066.5 / dxmed;
            local[1][4] = (1 - (1. - corDRholC * deriMas) * term1R) / dt + ugR * (1 - term1R) / dx + ulR * term1R / dx;
            local[1][5] = Amed * 98066.5 / dxmed;

            TL[1] = (MR - (1. - corDRholC * deriMas) * MliqiniR) / dt + (1. - corDRholC * deriMas) * term2R / dt + (ugR * term2R - ugL * term2) / dx -
                    (ulR * term2R - ulL * term2) / dx;

            double j1;
            double j2;

            j1 = (MliqiniR / rholC + (MR - MliqiniR) / rhogC) / AC;
            j2 = (MliqiniR / rholR + (MR - MliqiniR) / rhogR) / AR;

            double coef1C = ((1 / rhogC) + term1R * (rhogC - rholC) / (rhogC * rholC)) / AC;
            double coef2C = (term2R * (rhogC - rholC) / (rhogC * rholC)) / AC;
            double coef1R = ((1 / rhogR) + term1R * (rhogR - rholR) / (rhogR * rholR)) / AR;
            double coef2R = (term2R * (rhogR - rholR) / (rhogR * rholR)) / AR;

            double rhomix1;
            double rhomix2;

            rhomix1 = alf * rhogC + (1 - alf) * rholC;
            rhomix2 = alfR * rhogR + (1 - alfR) * rholR;

            double viscmix1;
            double viscmix2;

            double CorrViscG = 1.;
            double CorrViscL = 1.;

            viscmix1 = alf * migC * CorrViscG + (1 - alf) * ((1 - bet) * mipC + bet * micC) * CorrViscL;

            CorrViscG = 1.;
            CorrViscL = 1.;
            viscmix2 = alfR * migR * CorrViscG +
                       (1 - alfR) * ((1 - betR) * mipR + betR * micR) * CorrViscL;

            double re1;
            double re2;

            if (dutoL.revest == 0)
                re1 = Rey(duto.a, j1, rhomix1, viscmix1);
            else {
                double dhid = 4 * duto.area / duto.peri;
                re1 = Rey(dhid, j1, rhomix1, viscmix1);
            }
            if (dutoR.revest == 0)
                re2 = Rey(dutoR.a, j2, rhomix2, viscmix2);
            else {
                double dhid = 4 * dutoR.area / dutoR.peri;
                re2 = Rey(dhid, j2, rhomix2, viscmix2);
            }
            double f1 = fric(re1, duto.rug / duto.a);
            double f2 = fric(re2, dutoR.rug / dutoR.a, -1);
            if (duto.teta < 0.1) {
                f1 *= 1.;
                f2 *= 1.;
            }
            double razdx = dx / (dxR + dx);
            double coefTensC = dPdLFric * 0.5 * f1 * rhomix1 * (fabs(j1)) * siC * razdx;
            double dpfric = coefTensC * coef2C;
            double coefTensR = dPdLFric * 0.5 * f2 * rhomix2 * (fabs(j2)) * siR * (1. - razdx);
            dpfric += coefTensR * coef2R;
            double dphidro = dPdLHidro * 9.82 * sin(duto.teta) * AC * rhomix1 * razdx;
            termoHidro = dphidro / AC;
            termoHidro += 9.82 * sin(dutoR.teta) * rhomix2 * (1. - razdx);
            termoFric = coefTensC * j1 / AC + coefTensR * j2 / AR;
            dphidro += 9.82 * sin(dutoR.teta) * AR * rhomix2 * (1. - razdx);
            double estrat1 = perdaEstratL * term1R * fabs(MliqiniR) + perdaEstratG * (1 - term1R) * fabs(MR - MliqiniR);
            double estrat2 = (perdaEstratL * fabs(MliqiniR) - perdaEstratG * fabs(MR - MliqiniR)) * term2R;

            estrat1 = 0.;
            estrat2 = 0.;

            dpB = 0.;
            potB = 0.;
            potBT = 0.;
            double coefDpB = 0.;
            double perdArea = 0.;
            if ((duto.area != dutoR.area && re1 > 2400 && re2 > 2400) && (mudaArea == 1 && (fabs(j1) >= 0.1 && fabs(j2) >= 0.1))) {
                double areaMenor;
                double areaMaior;
                double bernou;
                double perdLoc;
                bernou = 0.5 * (MR / (duto.area * duto.area * rhomix1)) * (1. - pow(duto.area / dutoR.area, 2.));
                if (duto.area > dutoR.area) {
                    areaMenor = dutoR.area;
                    areaMaior = duto.area;
                    ;
                    if (MR > 0.) {
                        perdLoc = 0.42 * 0.5 * (MR / (duto.area * duto.area * rhomix1)) * (1. - pow(duto.area / dutoR.area, 2.));
                    } else {
                        double expanse = (1. - pow(areaMenor / areaMaior, 2.));
                        perdLoc = 0.5 * (MR / (areaMenor * areaMenor * rhomix1)) * expanse * expanse;
                    }
                } else {
                    areaMenor = duto.area;
                    areaMaior = dutoR.area;
                    if (MR < 0.) {
                        perdLoc = 0.42 * 0.5 * (MR / (duto.area * duto.area * rhomix1)) * (1. - pow(duto.area / dutoR.area, 2.));
                    } else {
                        double expanse = (1. - pow(areaMenor / areaMaior, 2.));
                        perdLoc = -0.5 * (MR / (areaMenor * areaMenor * rhomix1)) * expanse * expanse;
                    }
                }
                perdArea = -(bernou + perdLoc) * Amed / dxmed;
            }
            if (acsr.tipo == 4 && acsr.bcs.freqnova > 1. && j1 >= 0.) {
                double vazmix = j1 * AC;
                vazmix *= (86400 / 0.1589876);
                acsr.bcs.NovaVis(viscmix1, rhomix1, vazmix);
                dpB = 0.3048 * acsr.bcs.Hvis * rhomix1 * 9.82;
                potB = acsr.bcs.Pvis * 745.7;
                potTermo = (1. - acsr.bcs.Evis / 100.) * potB;
                if (acsr.bcs.eficM > 0.)
                    potBT = (1. + 100. * (1. - acsr.bcs.eficM / 100.) / acsr.bcs.eficM) * potB;
                else
                    potBT = 0.;
                potTermo += potBT * (1. - acsr.bcs.eficM / 100.) * acsr.bcs.fracTermMotorEfic;
                double Cdpb = AC * (86400 / 0.1589876);
                double djdm = (term1R / rholC + (1 - term1R) / rhogC) / AC;
                acsr.bcs.NovaVis(viscmix1, rhomix1, vazmix * 1.001);
                double dpdvaz = (0.3048 * acsr.bcs.Hvis * rhomix1 * 9.82 - dpB) / (0.001 * vazmix);
                coefDpB = Cdpb * djdm * dpdvaz;
            } else if (acsr.tipo == 7) {
                dpB = acsr.delp * 98066.5;
                double qgMon = (MR - MliqiniR) / rhogC;
                double qlmon = MliqiniR / rholC;
                double npoli = 1.;
                if (acsr.tipoCompGas == 0) {
                    npoli = flui.ConstAdG(pres, temp);
                    if ((npoli - 1.05) < 1e-2)
                        npoli = 1.05;
                } else if (acsr.tipoCompGas == 1)
                    npoli = acsr.fatPoli;
                double Wcomp;
                double Wbomb;
                Wbomb = (100. / acsr.eficLiq) * dpB * qlmon;
                if (acsr.tipoCompGas != 2)
                    Wcomp = -(presauxR * 98066.5 * qgMon / (1. - npoli)) *
                            (pow(1 + acsr.delp / presauxR, (npoli - 1.) / npoli) - 1.);
                else
                    Wcomp = presauxR * 98066.5 * qgMon * log(1 + acsr.delp / presauxR);
                Wcomp *= (100. / acsr.eficGas);
                potB = Wcomp + Wbomb;
                potTermo = ((1. - acsr.eficLiq / 100.) * Wbomb + (1. - acsr.eficGas / 100.) * Wcomp);
                potBT = Wcomp + Wbomb;
            } else if (acsr.tipo == 17 && acsr.multibcs.freqnova > 1. && j1 >= 0.) {
                double alf0 = alf;
                double bet0 = bet;
                acsr.multibcs.flui = flui;
                acsr.multibcs.fluicol = fluicol;
                acsr.multibcs.marchaMultiBcs((MR - MliqiniR) / rhogC, MliqiniR / rholC, presauxR, temp, alf0, bet0);
                dpB = acsr.multibcs.dpB * 98066.52;
                potB = acsr.multibcs.potB;
                potBT = acsr.multibcs.potBT;
                potTermo = acsr.multibcs.potTermo;
                potTermo += potBT * (1. - acsr.multibcs.eficM / 100.) * acsr.multibcs.fracTermMotorEfic;

                double Cdpb = AC * (86400 / 0.1589876);
                double djdm = (term1R / rholC + (1 - term1R) / rhogC) / AC;
                double vazmix = (MR - MliqiniR) / rhogC + MliqiniR / rholC;
                vazmix *= (86400 / 0.1589876);
                acsr.multibcs.marchaMultiBcs((MR - MliqiniR) * 1.001 / rhogC, MliqiniR * 1.001 / rholC, presauxR, temp, alf0, bet0);
                double dpdvaz = (acsr.multibcs.dpB * 98066.52 - dpB) / (0.001 * vazmix);
                coefDpB = Cdpb * djdm * dpdvaz;
            }
            double delpChoke = 0.;
            double sinalPig = 1.;
            if (velPig < 0.)
                sinalPig = 1.;

            local[1][4] += (coefTensC * coef1C + coefTensR * coef1R + estrat1);
            local[1][4] -= coefDpB * Amed / dxmed;
            local[1][4] += perdArea;
            TL[1] -= (dpfric + dphidro - (dpB + delpChoke - coefDpB * MR - (sinalPig * DelPig * velPig * 98066.5)) * Amed / dxmed + estrat2);
            cinematico = 0;
        } else {
            if (ciclo == 0) {
                compres = flui.Zdran(pres, temp);
                dzdp = flui.DZDP(pres, temp);
                dpdrho = rhogC * (1 / (pres * 98066.5) - (1 / compres) * dzdp);
                dpdrho = 1. / dpdrho;
                multL = ((1 / rpC) * (1 - betI) * rpCi + (1 / rcC) * betI * rcCi) / ((1 - betI) * rpCi + betI * rcCi);
                double drhoLdpC = (rpC - flui.MasEspLiq(pres * 0.999, temp)) / (0.001 * pres);
                if (alf < 1e-3 && drhoLdpC < 0.1)
                    drhoLdpC = 0.1;
                local[1][1] = 0.;
                local[1][2] = -((1. / rhogC) * (1. - term1) + multL * term1);
                local[1][3] = dx * AC * (((alf / rhogC) / dpdrho) * 98066.5 + corDRholC * (deriPres * (1. - alf) * (1 - bet) * drhoLdpC / rpC + deriPres * (1 / AC) * (1 / rhogC - 1 / rpC) * DTransDtp)) / dt;
                local[1][4] = 0.;
                local[1][5] = 0.;

                TL[1] = dx * AC * (((alf / rhogC) / dpdrho) * pres * 98066.5 + corDRholC * (deriPres * (1. - alf) * (1 - bet) * drhoLdp * pres / rpC + deriPres * (1 / AC) * (1 / rhogC - 1 / rpC) * DTransDtp * pres)) / dt - (1 / rhogC - multL) * term2 +
                        (fontemassGR / rhogC + (fontemassLR / rpC + fontemassCR / rcC)) + dx * (1 / rhogC - 1 / rpC) * transmassR;
            }
            double drhodt = flui.drhodt(pres, temp);
            double drhoLdt = (rpC - flui.MasEspLiq(pres, temp - 0.01)) / 0.01;
            TL[1] -= dx * AC * ((alf / rhogC) * drhodt + (1 - alf) * (1 - bet) * drhoLdt / rpC + (1 / AC) * (1 / rhogC - 1 / rpC) * DTransDtT) * dTdt * mudaDT;

            if (acsr.tipo == 8 && ciclo == 0) {
                double drhodp = 1 / dpdrho;
                double drhodp1;
                compres = flui.Zdran(presR, tempR);
                dzdp = flui.DZDP(presR, tempR);
                drhodp1 = rhogR * (1 / (pres * 98066.5) - (1 / compres) * dzdp);
                double correcBVol = -(-(1 / rhogR) * drhodp * pres * 98066.5 + rhogC * drhodp1 * presR * 98066.5 / (rhogR * rhogR));
                local[1][3] += (acsr.bvol.MultGas / rhogC + (acsr.bvol.MLiqP / rpC + acsr.bvol.MLiqC / rcC)) * (1 / rhogR) * drhodp * 98066.5;
                local[1][5] -= (acsr.bvol.MultGas / rhogC + (acsr.bvol.MLiqP / rpC + acsr.bvol.MLiqC / rcC)) * rhogC * drhodp1 * 98066.5 / (rhogR * rhogR);
                TL[1] += (acsr.bvol.MultGas / rhogC + (acsr.bvol.MLiqP / rpC + acsr.bvol.MLiqC / rcC)) * correcBVol;
            }
        }
    } else if (posic == ncel) {
        double corDRhol = 1. - 0 * mudaDTL;
        double corDRholC = 1. - 0 * mudaDT;
        if (masChkSup == 0) {
            double AL = dutoL.area;
            double rhogL = (*fluiL).MasEspGas(presL, tempL);
            if (ciclo == 0) {
                double rholL = rpL * (1 - betL) + rcL * betL;
                double compres = (*fluiL).Zdran(presL, tempL);
                double dzdp = (*fluiL).DZDP(presL, tempL);
                double dpdrho = rhogL * (1 / (presL * 98066.5) - (1 / compres) * dzdp);
                dpdrho = 1. / dpdrho;
                double drhoLdp;
                if ((*fluiL).flashCompleto != 2 || miniTabAtraso > 0)
                    drhoLdp = (rpL - (*fluiL).MasEspLiq(presL * 0.999, tempL)) / (0.001 * presL);
                else {
                    ProFlu flud = (*fluiL);
                    flud.atualizaPropComp(presL * 0.999, tempL, flud.dCalculatedBeta, flud.oCalculatedLiqComposition,
                                          flud.oCalculatedVapComposition, 0);
                    drhoLdp = (rpL - flud.MasEspLiq(presL * 0.999, tempL)) / (0.001 * presL);
                }
                double multL = ((1 / rpL) * (1 - betLI) * rpLi + (1 / rcL) * betLI * rcLi) / ((1 - betLI) * rpLi + betLI * rcLi);
                double multR = ((1 / rpL) * (1 - betI) * rpCi + (1 / rcL) * betI * rcCi) / ((1 - betI) * rpCi + betI * rcCi);
                // QUESTION: Shouldn't this be rcCi instead of rcRi?

                double coluna = 1.;
                if (duto.teta < 0.8 * M_PI / 2.)
                    coluna = 1.;

                local[0][1] = -((1. / rhogL) * (1. - term1L) + multL * term1L);
                local[0][2] = coluna * dxL * AL * (((alfL / rhogL) / dpdrho) * 98066.5 + corDRhol * (deriPres * (1. - alfL) * (1 - betL) * drhoLdp / rpL + deriPres * (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtpL)) / dt;
                local[0][3] = (1. / rhogL) * (1 - term1) + multR * term1;
                local[0][4] = 0.;
                local[0][5] = 0.;

                TL[0] = coluna * dxL * AL * (((alfL / rhogL) / dpdrho) * presL * 98066.5 + corDRhol * (deriPres * (1. - alfL) * (1 - betL) * drhoLdp * presL / rpL + deriPres * (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtpL * presL)) / dt +
                        (1 / rhogL - multR) * term2 - (1 / rhogL - multL) * term2L +
                        (fontemassGL / rhogL + (fontemassLL / rpL + fontemassCL / rcL) / rholL) + dxL * (1 / rhogL - 1 / rpL) * transmassL;
            }
            double drhodt = (*fluiL).drhodt(presL, tempL);
            double drhoLdt = (rpL - (*fluiL).MasEspLiq(presL, tempL - 0.01)) / 0.01;

            TL[0] -= dxL * AL * ((alfL / rhogL) * drhodt + (1 - alfL) * (1 - betL) * drhoLdt / rpL + (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtTL) * dTdtL * mudaDT;

            if (ciclo == 0) {
                local[1][1] = 0.;
                local[1][2] = 0.;
                local[1][3] = 1.;
                local[1][4] = 0.;
                local[1][5] = 0.;
                if (((*vg1dSP).chaveredeT == 0 || noextremo == 1) && corrigeContSep == 1) {
                    double dxmed = 0.5 * dx;
                    double j1;

                    double AC = duto.area;
                    double siC = duto.peri;
                    double rhogC = rgC;
                    double rholC = rpC * (1 - bet) + rcC * bet;

                    j1 = (MliqiniR / rholC + (MR - MliqiniR) / rhogC) / AC;

                    double coef1C = ((1 / rhogC) + term1 * (rhogC - rholC) / (rhogC * rholC)) / AC;
                    double coef2C = (term2 * (rhogC - rholC) / (rhogC * rholC)) / AC;
                    double rhomix1;

                    rhomix1 = alf * rhogC + (1 - alf) * rholC;

                    double viscmix1;
                    double CorrViscG = 1.;
                    double CorrViscL = 1.;

                    viscmix1 = alf * migC * CorrViscG + (1 - alf) * ((1 - bet) * mipC + bet * micC) * CorrViscL;
                    CorrViscG = 1.;
                    CorrViscL = 1.;

                    double re1;

                    if (duto.revest == 0)
                        re1 = Rey(duto.a, j1, rhomix1, viscmix1);
                    else {
                        double dhid = 4 * duto.area / duto.peri;
                        re1 = Rey(dhid, j1, rhomix1, viscmix1);
                    }
                    double f1 = fric(re1, duto.rug / duto.a);
                    if (duto.teta < 0.1) {
                        f1 *= 1.;
                    }

                    double coefTensC = 0.5 * f1 * rhomix1 * (fabs(j1)) * siC / AC;
                    double dpfric = coefTensC * coef2C;
                    double dphidro = 9.82 * sin(duto.teta) * rhomix1;
                    termoHidro = dphidro;
                    termoFric = coefTensC * j1;

                    local[1][2] -= (coefTensC * coef1C) * dxmed / 98066.5;
                    TL[1] = presfim + (dpfric + dphidro) * dxmed / 98066.5;
                } else
                    TL[1] = presfim;
            }
        } else {
            double AL = dutoL.area;
            double rhogL = rgL;
            double compres;
            double dzdp;
            double dpdrho;
            double coluna = 1.;
            double multL;
            double multR;
            if (ciclo == 0) {

                compres = (*fluiL).Zdran(presL, tempL);
                dzdp = (*fluiL).DZDP(presL, tempL);
                dpdrho = rhogL * (1 / (presL * 98066.5) - (1 / compres) * dzdp);
                dpdrho = 1. / dpdrho;
                double drhoLdp;
                if ((*fluiL).flashCompleto != 2 || miniTabAtraso > 0)
                    drhoLdp = (rpL - (*fluiL).MasEspLiq(presL * 0.999, tempL)) / (0.001 * presL);
                else {
                    ProFlu flud = (*fluiL);
                    flud.atualizaPropComp(presL * 0.999, tempL, flud.dCalculatedBeta, flud.oCalculatedLiqComposition,
                                          flud.oCalculatedVapComposition, 0);
                    drhoLdp = (rpL - flud.MasEspLiq(presL * 0.999, tempL)) / (0.001 * presL);
                }
                multL = ((1 / rpL) * (1 - betLI) * rpLi + (1 / rcL) * betLI * rcLi) / ((1 - betLI) * rpLi + betLI * rcLi);
                multR = ((1 / rpL) * (1 - betI) * rpCi + (1 / rcL) * betI * rcRi) / ((1 - betI) * rpCi + betI * rcCi);

                if (duto.teta < 0.8 * M_PI / 2.)
                    coluna = 1.;

                local[0][0] = 0.;
                local[0][1] = -((1. / rhogL) * (1. - term1L) + multL * term1L);
                local[0][2] = coluna * dxL * AL * (((alfL / rhogL) / dpdrho) * 98066.5 + corDRhol * (deriPres * (1. - alfL) * (1 - betL) * drhoLdp / rpL + deriPres * (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtpL)) / dt;
                local[0][3] = (1. / rhogL) * (1 - term1) + multR * term1;
                local[0][4] = 0.;
                local[0][5] = 0.;

                TL[0] = coluna * dxL * AL * (((alfL / rhogL) / dpdrho) * presL * 98066.5 + corDRhol * (deriPres * (1. - alfL) * (1 - betL) * drhoLdp * presL / rpL + deriPres * (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtpL * presL)) / dt + (1 / rhogL - multR) * term2 - (1 / rhogL - multL) * term2L +
                        (fontemassGL / rhogL + (fontemassLL / rpL + fontemassCL / rcL)) + dxL * (1 / rhogL - 1 / rpL) * transmassL;
            }
            double drhodt = (*fluiL).drhodt(presL, tempL);
            double drhoLdt = (rpL - (*fluiL).MasEspLiq(presL, tempL - 0.01)) / 0.01;

            TL[0] -= dxL * AL * ((alfL / rhogL) * drhodt + (1 - alfL) * (1 - betL) * drhoLdt / rpL + (1 / AL) * (1 / rhogL - 1 / rpL) * DTransDtTL) * dTdtL * mudaDT;

            double dxncel = 1. * dx;
            double AC = dutoR.area;
            double rhog = rgC;
            if (ciclo == 0) {
                AC = dutoR.area;
                rhog = flui.MasEspGas(pres, temp);
                compres = flui.Zdran(pres, temp);
                dzdp = flui.DZDP(pres, temp);
                dpdrho = rhog * (1 / (pres * 98066.5) - (1 / compres) * dzdp);
                dpdrho = 1. / dpdrho;
                double drhoLdp;
                if (flui.flashCompleto != 2 || miniTabAtraso > 0)
                    drhoLdp = (rpC - flui.MasEspLiq(pres * 0.999, temp)) / (0.001 * pres);
                else {
                    ProFlu flud = flui;
                    flud.atualizaPropComp(pres * 0.999, temp, flud.dCalculatedBeta, flud.oCalculatedLiqComposition,
                                          flud.oCalculatedVapComposition, 0);
                    drhoLdp = (rpC - flud.MasEspLiq(pres * 0.999, temp)) / (0.001 * pres);
                }
                coluna = 1.;
                if (duto.teta < 0.8 * M_PI / 2.)
                    coluna = 1.;

                multL = ((1 / rpL) * (1 - betI) * rpCi + (1 / rcL) * betI * rcRi) / ((1 - betI) * rpCi + betI * rcCi);

                if (alf < 1e-3 && drhoLdp < 0.1)
                    drhoLdp = 0.1;

                local[1][0] = 0.;
                local[1][1] = 0.;
                local[1][2] = -((1. / rhog) * (1. - term1) + multL * term1);
                local[1][3] = coluna * dxncel * AC * (((alf / rhog) / dpdrho) * 98066.5 + corDRholC * (deriPres * (1. - alf) * (1 - bet) * drhoLdp / rpC + deriPres * (1 / AC) * (1 / rhog - 1 / rpC) * DTransDtp)) / dt -
                              (DmasschokeG / rhog + DmasschokeL / rpC + DmasschokeC / rcC);
                local[1][4] = 0.;
                local[1][5] = 0.;

                TL[1] = coluna * dxncel * AC * (((alf / rhog) / dpdrho) * pres * 98066.5 + corDRholC * (deriPres * (1. - alf) * (1 - bet) * drhoLdp * pres / rpC + deriPres * (1 / AC) * (1 / rhog - 1 / rpC) * DTransDtp * pres)) / dt - (1 / rhog - multL) * term2 +
                        ((fontemassGR - DmasschokeG * pres) / rhog + (fontemassLR - DmasschokeL * pres) / rpC +
                         (fontemassCR - DmasschokeC * pres) / rcC);
            }
            drhodt = flui.drhodt(presini, tempini);
            if (flui.flashCompleto != 2 || miniTabAtraso > 0)
                drhoLdt = (rpC - flui.MasEspLiq(pres, temp - 0.01)) / 0.01;
            else {
                ProFlu flud = flui;
                flud.atualizaPropComp(pres, temp - 0.01, flud.dCalculatedBeta, flud.oCalculatedLiqComposition,
                                      flud.oCalculatedVapComposition, 0);
                drhoLdt = (rpC - flud.MasEspLiq(pres, temp - 0.01)) / 0.01;
            }

            TL[1] -= dxncel * AC * ((alf / rhog) * drhodt + (1 - alf) * (1 - bet) * drhoLdt / rpC + (1 / AC) * (1 / rhog - 1 / rpC) * DTransDtT) * dTdtL * mudaDT;
        }
    } else if (posic == 0 && ciclo == 0) {

        double corDRholC = 1. - 0 * mudaDT;
        if (presE < 0) {
            local[0][1] = 0.;
            local[0][2] = 0.;
            local[0][3] = 1.;
            local[0][4] = 0.;
            local[0][5] = 0;

            TL[0] = 0.;
        } else {
            double AC = duto.area;
            double AR = dutoR.area;
            double Amed = 0.5 * (AC + AR);
            double siC = duto.peri;
            double rhogC;
            double rholC;
            if (flui.flashCompleto != 2 || miniTabAtraso > 0) {
                rhogC = flui.MasEspGas(0.5 * (presE + pres), tempE);
                rholC = flui.MasEspLiq(0.5 * (presE + pres), tempE) * (1 - betE) +
                        fluicol.MasEspFlu(0.5 * (presE + pres), tempE) * betE;
            } else {
                ProFlu flud = flui;
                flud.atualizaPropComp(0.5 * (presE + pres), tempE, flud.dCalculatedBeta, flud.oCalculatedLiqComposition,
                                      flud.oCalculatedVapComposition, 0);
                rhogC = flud.MasEspGas(0.5 * (presE + pres), tempE);
                rholC = flud.MasEspLiq(0.5 * (presE + pres), tempE) * (1 - betE) +
                        fluicol.MasEspFlu(0.5 * (presE + pres), tempE) * betE;
            }

            double j1 = (Mliqini / rholC + (MC - Mliqini) / rhogC) / AC;
            double coef1C = ((1 / rhogC) + term1 * (rhogC - rholC) / (rhogC * rholC)) / AC;
            double coef2C = (term2 * (rhogC - rholC) / (rhogC * rholC)) / AC;
            double rhomix1 = titE * rhogC + (1 - titE) * rholC;
            double viscmix1 = titE * flui.ViscGas(0.5 * (presE + pres), tempE) +
                              (1 - titE) * ((1 - betE) * flui.ViscOleo(0.5 * (presE + pres), tempE) +
                                            betE * fluicol.VisFlu(0.5 * (presE + pres), tempE));
            double re1;
            if (duto.revest == 0)
                re1 = Rey(duto.a, j1, rhomix1, viscmix1);
            else {
                double dhid = 4 * duto.area / duto.peri;
                re1 = Rey(dhid, j1, rhomix1, viscmix1);
            }
            double f1 = fric(re1, duto.rug / duto.a);
            double coefTensC = dPdLFric * 0.5 * f1 * rhomix1 * (fabs(j1)) * siC;
            double dpfric = coefTensC * coef2C;
            double dphidro = dPdLHidro * 9.82 * sin(duto.teta) * AC * rhomix1;
            termoHidro = dphidro / AC;
            termoFric = coefTensC * j1 / AC;

            local[0][1] = 0.;
            local[0][2] = 0.;
            local[0][3] = coefTensC * coef1C + (1. - (1. - corDRholC * deriMas)) * term1 / dt;
            local[0][4] = Amed * 98066.5 / (dx);
            local[0][5] = 0;

            TL[0] = (MC - (1. - corDRholC * deriMas) * Mliqini) / dt + (1. - corDRholC * deriMas) * term2 / dt;
            TL[0] += (Amed * 98066.5 * presE / (dx) - (dpfric + dphidro));
        }

        double AC = duto.area;
        double AR = dutoR.area;
        double siC = duto.peri;
        double siR = dutoR.peri;
        double rhogC = flui.MasEspGas(pres, temp);
        double rholC = rpC * (1 - bet) + rcC * bet;
        double rhogR = (*fluiR).MasEspGas(presR, tempR);
        double rholR = rpR * (1 - betR) + rcR * betR;

        double dxmed = 0.5 * (dx + dxR);
        double Amed = 0.5 * (AC + AR);

        local[1][1] = 0.;
        local[1][2] = 0.;
        local[1][3] = -Amed * 98066.5 / dxmed;
        local[1][4] = (1 - (1. - corDRholC * deriMas) * term1R) / dt;
        local[1][5] = Amed * 98066.5 / dxmed;

        TL[1] = (MR - (1. - corDRholC * deriMas) * MliqiniR) / dt + (1. - corDRholC * deriMas) * term2R / dt;

        double j1;
        double j2;

        j1 = (MliqiniR / rholC + (MR - MliqiniR) / rhogC) / AC;
        j2 = (MliqiniR / rholR + (MR - MliqiniR) / rhogR) / AR;

        double coef1C = ((1 / rhogC) + term1R * (rhogC - rholC) / (rhogC * rholC)) / AC;
        double coef2C = (term2R * (rhogC - rholC) / (rhogC * rholC)) / AC;
        double coef1R = ((1 / rhogR) + term1R * (rhogR - rholR) / (rhogR * rholR)) / AR;
        double coef2R = (term2R * (rhogR - rholR) / (rhogR * rholR)) / AR;

        double rhomix1;
        double rhomix2;

        rhomix1 = alf * rhogC + (1 - alf) * rholC;
        rhomix2 = alfR * rhogR + (1 - alfR) * rholR;

        double viscmix1;
        double viscmix2;

        viscmix1 = alf * migC + (1 - alf) * ((1 - bet) * mipC + bet * micC);
        viscmix2 = alfR * migR + (1 - alfR) * ((1 - betR) * mipR + betR * micR);

        double re1;
        double re2;

        if (duto.revest == 0)
            re1 = Rey(duto.a, j1, rhomix1, viscmix1);
        else {
            double dhid = 4 * duto.area / duto.peri;
            re1 = Rey(dhid, j1, rhomix1, viscmix1);
        }
        if (dutoR.revest == 0)
            re2 = Rey(dutoR.a, j2, rhomix2, viscmix2);
        else {
            double dhid = 4 * duto.area / duto.peri;
            re2 = Rey(dhid, j2, rhomix2, viscmix2);
        }
        double f1 = fric(re1, duto.rug / duto.a);
        double f2 = fric(re2, dutoR.rug / dutoR.a, -1);
        double razdx = dx / (dxR + dx);
        double coefTensC = dPdLFric * 0.5 * f1 * rhomix1 * (fabs(j1)) * siC * razdx;
        double dpfric = coefTensC * coef2C;
        double coefTensR = dPdLFric * 0.5 * f2 * rhomix2 * (fabs(j2)) * siR * (1. - razdx);
        dpfric += coefTensR * coef2R;
        double dphidro = dPdLHidro * 9.82 * sin(duto.teta) * AC * rhomix1 * razdx;
        termoHidro = dphidro / AC;
        termoHidro += 9.82 * sin(dutoR.teta) * rhomix2 * (1. - razdx);
        termoFric = coefTensC * j1 + coefTensR * j2;
        dphidro += 9.82 * sin(dutoR.teta) * AR * rhomix2 * (1. - razdx);

        local[1][4] += (coefTensC * coef1C + coefTensR * coef1R);
        TL[1] -= (dpfric + dphidro);

        termoHidro = dphidro;
        termoFric = dpfric;
    }
}

void Cel::avancPig(int &reinicia) {
    razPig = (razPigini * dx + velPig * dt) / dx;

    if (((razPig <= (*vg1dSP).localtiny) && (razPig >= -(*vg1dSP).localtiny)))
        razPig = 0;
    else if (razPig < -(*vg1dSP).localtiny) {
        double dtaux;
        dtaux = -razPigini * dx / velPig;
        if (dtaux > (*vg1dSP).localtiny) {
            dtPig = dtaux;
            reiniciaPig = -1;
            razPig = fabs(0.);
        } else
            razPig = fabs(0.);
    } else if ((razPig >= (1. - (*vg1dSP).localtiny) && razPig <= (1. + (*vg1dSP).localtiny))) {
        razPig = 1.;
    } else if (razPig > (1. + (*vg1dSP).localtiny)) {
        double dtaux;
        dtaux = (1. - razPigini) * dx / velPig;
        if (dtaux > (*vg1dSP).localtiny) {
            dtPig = dtaux;
            reiniciaPig = -1;
            razPig = 1.;
        } else
            razPig = 1.;
    }
}

void Cel::avancalfPig() {
    double term1Mass = 0.;
    if (alf <= (*vg1dSP).localtiny) {
        alfPigD = 0.;
        alfPigE = 0.;
    } else if (alf >= (1. - (*vg1dSP).localtiny)) {
        alfPigD = 1.;
        alfPigE = 1.;
    } else {
        if (velPig >= 0.) {

            double termoPtrans = 0.;
            double termoDrholP = 0.;
            double termoTtrans = 0.;
            double termoDrholT = 0.;
            if (TMModel == 0) {
                termoPtrans = -(DTransDtp / rpC) * m2d * (dpdt);
                termoDrholP = (duto.area * (1. - alf) * (1 - bet) / rpC) * ((rpC - flui.MasEspLiq(presini * 0.999, temp)) / (0.001 * presini)) *
                              m2d * (dpdt)*ativaDeri;

                double dTtemp = dTdt;
                termoTtrans = -(DTransDtT / rpC) * (dTtemp);
                termoDrholT = (duto.area * (1. - alf) * (1 - bet) / rpC) * ((rpC - flui.MasEspLiq(presini, temp * 0.999)) / (0.001 * temp)) *
                              (dTtemp)*ativaDeri;
            }

            if (RazAreaPig > (*vg1dSP).localtiny) {
                double areagarg = RazAreaPig * duto.area;
                double rlm = (1. - alfPigEini) * ((1 - betPigEini) * rpC + betPigEini * rcC) +
                             alfPigEini * flui.MasEspGas(pres, temp);
                double massica = areagarg * sqrt(2. * rlm * (DelPig * velPig) * 98066.52);
                massica = cdpig * massica;
                VazaPig = massica / rlm;
            } else
                VazaPig = 0.;
            double fontepigL = VazaPig * (1. - alfPigEini) * (1. - betPigEini) * rpC;
            double fontepigC = VazaPig * (1. - alfPigEini) * (betPigEini)*rcC;

            if (razPig <= (1. - (*vg1dSP).localtiny)) {
                double rlm = (1 - betPigDini) * rpC + betPigDini * rcC;

                double fator1 = 1. - (rcC - rpC) * betPigDini / rlm;
                fator1 = 1. / fator1;
                double fator2 = (rcC - rpC) / rcC;

                double bet0 = betPigDini;
                double bet1;
                if (MliqiniR > 0)
                    bet1 = betPigDini;
                else
                    bet1 = betRini;

                double razdx1 = dxR / (dx + dxR);
                double tmed1 = razdx1 * tempR + (1 - razdx1) * temp;
                double rc0 = rcC;
                double rc1 = fluicol.MasEspFlu(presauxR, tmed1);

                double convec = fator1 * (MliqiniR - rlm * (1 - alfPigDini) * velPig * duto.area) / ((1. - razPig) * dx) -
                                fator2 * fator1 * (QLR * rc1 * bet1 - velPig * (1 - alfPigDini) * duto.area * rc0 * bet0) / ((1. - razPig) * dx) - fator1 * (fontepigL / ((1. - razPig) * dx) + (1. - fator2) * fontepigC / ((1. - razPig) * dx));

                if (razPig < 0.5)
                    term1Mass = (convec - fator1 * (fontemassLR / ((1. - razPig) * dx) +
                                                    (1. - fator2) * fontemassCR / ((1. - razPig) * dx) - transmassR -
                                                    rpC * (termoPtrans + termoDrholP + termoTtrans + termoDrholT))) /
                                (rlm * duto.area);
                else
                    term1Mass = (convec + fator1 * (transmassR +
                                                    rpC * (termoPtrans + termoDrholP + termoTtrans + termoDrholT))) /
                                (rlm * duto.area);
                alfPigD = alfPigDini + term1Mass * dt;
                if (alfPigD < 0.)
                    alfPigD = 0.;
                if (alfPigD > 1.)
                    alfPigD = 1.;
                alfPigE = (-alfPigD * (1. - razPig) + alf) / razPig;
                if (alfPigE < 0.)
                    alfPigE = 0.;
                if (alfPigE > 1.)
                    alfPigE = 1.;
            } else {
                alfPigD = alfPigDini;
                alfPigE = alf;
            }
        } else {
            double termoPtrans = 0.;
            double termoDrholP = 0.;
            double termoTtrans = 0.;
            double termoDrholT = 0.;
            if (TMModel == 0) {
                termoPtrans = -(DTransDtp / rpC) * m2d * (dpdt);
                termoDrholP = (duto.area * (1. - alf) * (1 - bet) / rpC) * ((rpC - flui.MasEspLiq(presini * 0.999, temp)) / (0.001 * presini)) *
                              m2d * (dpdt)*ativaDeri;

                double dTtemp = dTdt;
                termoTtrans = -(DTransDtT / rpC) * (dTtemp);
                termoDrholT = (duto.area * (1. - alf) * (1 - bet) / rpC) * ((rpC - flui.MasEspLiq(presini, temp * 0.999)) / (0.001 * temp)) *
                              (dTtemp)*ativaDeri;
            }
            if (RazAreaPig > (*vg1dSP).localtiny) {
                double areagarg = RazAreaPig * duto.area;
                double rlm = (alfPigDini) * ((1 - betPigDini) * rpC + betPigDini * rcC) +
                             alfPigDini * flui.MasEspGas(pres, temp);
                double massica = areagarg * sqrt(2. * rlm * (DelPig * fabs(velPig)) * 98066.52);
                massica = cdpig * massica;
                VazaPig = -massica / rlm;
            } else
                VazaPig = 0.;
            double fontepigL = -VazaPig * (1. - alfPigDini) * (1. - betPigDini) * rpC;
            double fontepigC = -VazaPig * (1. - alfPigDini) * (betPigDini)*rcC;

            if (razPig >= (*vg1dSP).localtiny) {

                double rlm = (1 - betPigEini) * rpC + betPigEini * rcC;

                double fator1 = 1. - (rcC - rpC) * betPigEini / rlm;
                fator1 = 1. / fator1;
                double fator2 = (rcC - rpC) / rcC;

                double bet0;
                if (Mliqini > 0)
                    bet0 = betLini;
                else
                    bet0 = betPigEini;
                double bet1 = betPigEini;

                double razdx0 = dx / (dx + dxL);
                double tmed0 = razdx0 * temp + (1 - razdx0) * tempL;
                double rc0 = fluicol.MasEspFlu(presaux, tmed0);
                double rc1 = rcC;
                double convec = fator1 * (rlm * (1. - alfPigEini) * velPig * duto.area - Mliqini) / (razPig * dx) -
                                fator2 * fator1 * (velPig * (1. - alfPigEini) * duto.area * rc1 * bet1 - QL * rc0 * bet0) / (razPig * dx) - fator1 * (fontepigL / (razPig * dx) + (1. - fator2) * fontepigC / (razPig * dx));

                if (razPig >= 0.5)
                    term1Mass = (convec - fator1 * (fontemassLR / (razPig * dx) + (1. - fator2) * fontemassCR / (razPig * dx) -
                                                    transmassR -
                                                    rpC * (termoPtrans + termoDrholP + termoTtrans + termoDrholT))) /
                                (rlm * duto.area);
                else
                    term1Mass = (convec + fator1 * (transmassR +
                                                    rpC * (termoPtrans + termoDrholP + termoTtrans + termoDrholT))) /
                                (rlm * duto.area);
                alfPigE = alfPigEini + term1Mass * dt;
                if (alfPigE < 0.)
                    alfPigE = 0.;
                if (alfPigE > 1.)
                    alfPigE = 1.;
                alfPigD = (-alfPigE * razPig + alf) / (1 - razPig);
                if (alfPigD < 0.)
                    alfPigD = 0.;
                if (alfPigD > 1.)
                    alfPigD = 1.;
            } else {
                alfPigD = alf;
                alfPigE = alfPigEini;
            }
        }
    }
}

void Cel::avancbetPig() {

    double term1Mass = 0.;

    double fator1 = rcC * duto.area;
    if (bet <= (*vg1dSP).localtiny) {
        betPigD = 0.;
        betPigE = 0.;
    } else if (bet >= (1. - (*vg1dSP).localtiny)) {
        betPigD = 1.;
        betPigE = 1.;
    } else {
        if (velPig >= 0.) {
            double fontepigC = VazaPig * (1. - alfPigEini) * (betPigEini)*rcC;
            if (razPig <= (1. - (*vg1dSP).localtiny)) {
                double bet0 = betPigDini;
                double bet1;
                if (MliqiniR > 0)
                    bet1 = betPigDini;
                else
                    bet1 = betRini;

                double razdx1 = dxR / (dx + dxR);
                double tmed1 = razdx1 * tempR + (1 - razdx1) * temp;
                double rc0 = rcC;
                double rc1 = fluicol.MasEspFlu(presauxR, tmed1);

                double convec = (QLR * rc1 * bet1 - velPig * duto.area * (1 - alfPigDini) * rc0 * bet0) / ((1. - razPig) * dx) + fontepigC / ((1. - razPig) * dx);
                if (razPig < 0.5)
                    term1Mass = (-convec + fator1 * betPigDini * (alfPigD - alfPigDini) / dt + fontemassCR / ((1. - razPig) * dx)) / (fator1 * (1 - alfPigDini));
                else
                    term1Mass = (-convec + fator1 * betPigDini * (alfPigD - alfPigDini) / dt) / (fator1 * (1 - alfPigDini));
                betPigD = betPigDini + term1Mass * dt;
                if (betPigD < 0.)
                    betPigD = 0.;
                if (betPigD > 1.)
                    betPigD = 1.;
                betPigE = (-betPigD * (1. - razPig) + bet) / razPig;
                if (betPigE < 0.)
                    betPigE = 0.;
                if (betPigE > 1.)
                    betPigE = 1.;
            } else {
                betPigD = betPigDini;
                betPigE = bet;
            }
        } else {
            double fontepigC = -VazaPig * (1. - alfPigDini) * (betPigDini)*rcC;
            if (razPig >= (*vg1dSP).localtiny) {
                double bet0;
                if (Mliqini > 0)
                    bet0 = betLini;
                else
                    bet0 = betPigEini;
                double bet1 = betPigEini;

                double razdx0 = dx / (dx + dxL);
                double tmed0 = razdx0 * temp + (1 - razdx0) * tempL;
                double rc0 = fluicol.MasEspFlu(presaux, tmed0);
                double rc1 = rcC;

                double convec = (velPig * duto.area * (1 - alfPigEini) * rc1 * bet1 - QL * rc0 * bet0) / (razPig * dx) + fontepigC / (razPig * dx);
                if (razPig >= 0.5)
                    term1Mass = (-convec + fator1 * betPigEini * (alfPigE - alfPigEini) / dt + fontemassCR / (razPig * dx)) / (fator1 * (1 - alfPigEini));
                else
                    term1Mass = (-convec + fator1 * betPigEini * (alfPigE - alfPigEini) / dt) / (fator1 * (1 - alfPigEini));
                betPigE = betPigEini + term1Mass * dt;
                if (betPigE < 0.)
                    betPigE = 0.;
                if (betPigE > 1.)
                    betPigE = 1.;
                betPigD = (-betPigE * razPig + bet) / (1 - razPig);
                if (betPigD < 0.)
                    betPigD = 0.;
                if (betPigD > 1.)
                    betPigD = 1.;
            } else {
                betPigD = bet;
                betPigE = betPigEini;
            }
        }
    }
}

void Cel::FeiticoDoTempo() {
    pres = presini;
    presL = presLini;
    MliqiniR = MliqiniR0;
    MR = MRini;
    temp = tempini;
    presR = presRini;
    presaux = presauxini;
    MC = MCini;
    MR = MRini;
    ML = MLini;
    Mliqini = Mliqini0;
    MliqiniL = MliqiniL0;
    tempL = tempLini;
    tempR = tempRini;
    QLL = QLLini;
    QLR = QLRini;
    QL = QLini;
    QG = QGini;
    presauxR = presauxRini;
    presauxL = presauxLini;
    VTemper = VTemperini;

    calor.FeiticoDoTempo();
}

void Cel::FeiticoDoTempo2() {

    alfPigER = alfPigERini;
    indpig = indpigini;
    betI = betIini;
    betRI = betRIini;
    betLI = betLIini;
    alfL = alfLini;
    alf = alfini;
    alfR = alfRini;
    betL = betLini;
    bet = betini;
    betR = betRini;
    estadoPig = estadoPigini;
    alfPigE = alfPigEini;
    alfPigD = alfPigDini;
    betPigE = betPigEini;
    betPigD = betPigDini;
    velPig = velPigini;
    razPig = razPigini;

    transic = transic0;
    c0 = c0ini;
    ud = udini;
}

void Cel::FeiticoDoTempo3() {
    pres = presini;
    presL = presLini;
    MliqiniR = MliqiniR0;
    MR = MRini;
    presR = presRini;
    presaux = presauxini;
    MC = MCini;
    MR = MRini;
    ML = MLini;
    Mliqini = Mliqini0;
    MliqiniL = MliqiniL0;
    QLL = QLLini;
    QLR = QLRini;
    QL = QLini;
    QG = QGini;
    presauxR = presauxRini;
    presauxL = presauxLini;
}
double Cel::SIGN(double a, double b) {
    return (b >= 0 ? 1.0 : -1.0) * fabs(a);
}

double Cel::zriddrRhop(double x1, double x2, double rp) {
    double xacc = 1e-10;
    int maxit = 100;
    double fmin;
    double xmin;
    double fl = rp - flui.MasEspLiq(x1, temp);
    double fh = rp - flui.MasEspLiq(x2, temp);
    if (fabs(fh) < fabs(fl)) {
        fmin = fh;
        xmin = x2;
    } else {
        fmin = fl;
        xmin = x1;
    }
    if ((fl > 0.0 && fh < 0.0) || (fl < 0.0 && fh > 0.0)) {
        double xl = x1;
        double xh = x2;
        double ans = -1.e20;
        for (int j = 0; j < maxit; j++) {
            double xm = 0.5 * (xl + xh);
            double fm = rp - flui.MasEspLiq(xm, temp);
            if (fabs(fm) < fabs(fmin)) {
                fmin = fm;
                xmin = xm;
            }
            double s = sqrt(fm * fm - fl * fh);
            if (s == 0.0) {
                fmin = rp - flui.MasEspLiq(xmin, temp);
                return xmin;
            }
            double xnew = xm + (xm - xl) * ((fl >= fh ? 1.0 : -1.0) * fm / s);
            if (fabs(xnew - ans) <= xacc) {
                fmin = rp - flui.MasEspLiq(xmin, temp);
                return xmin;
            }
            ans = xnew;
            double fnew = rp - flui.MasEspLiq(ans, temp);
            if (fabs(fnew) < fabs(fmin)) {
                fmin = fnew;
                xmin = ans;
            }
            if (fabs(fnew) <= xacc) {
                fmin = rp - flui.MasEspLiq(xmin, temp);
                return xmin;
            }
            if (SIGN(fm, fnew) != fm) {
                xl = xm;
                fl = fm;
                xh = ans;
                fh = fnew;
            } else if (SIGN(fl, fnew) != fl) {
                xh = ans;
                fh = fnew;
            } else if (SIGN(fh, fnew) != fh) {
                xl = ans;
                fl = fnew;
            } else
                return -100000.;
            if (fabs(xh - xl) <= xacc) {
                fmin = rp - flui.MasEspLiq(xmin, temp);
                return xmin;
            }
        }
        return 100000.;
    } else {
        if (fabs(fl) <= xacc)
            return x1;
        if (fabs(fh) <= xacc)
            return x2;
        return -10000.;
    }
}

double Cel::somVel() {
    double val;
    double alfI;
    double betI;
    if (QG > 0)
        alfI = alfL;
    else
        alfI = alf;
    if (QL > 0)
        betI = bet;
    else
        betI = bet;

    double tempMed;
    double raz;
    if (posic == 0)
        tempMed = temp;
    else {
        raz = dxL / (dx + dxL);
        tempMed = (1. - raz) * tempL + raz * temp;
    }
    if (alfI < 1e-15) {
        val = 1500.;
    } else {
        double kad = flui.ConstAdG(presaux, tempMed, rgCi);
        double rlmix = rpCi * (1. - betI) + rcCi * betI;
        double cg2 = kad / flui.drhodp(presaux, tempMed);
        double vl;
        if (alfI < 1 - 1e-15)
            vl = QL / (duto.area * (1. - alfI));
        else
            vl = 0;
        double vg;
        if (alfI > 1e-15)
            vg = QG / (duto.area * alfI);
        else
            vg = 0.;
        double val1 = 1. - c0 * alfI + c0 * alfI * alfI * (vg - vl) * (vg - vl) * (c0 * alfI - 1.) / (cg2) + c0 * alfI * rgCi / rlmix;
        val1 *= rgCi * cg2;
        val1 = sqrt(val1);
        val1 /= sqrt(alfI * rlmix);
        val1 /= (1. - c0 * alfI + c0 * alfI * (rgCi / rlmix));
        val = val1;
        if (val > 1500.)
            val = 1500.;
    }
    return val;
}

double Cel::termAdSomVel() {
    double val;
    double alfI;
    double betI;
    if (QG > 0)
        alfI = alfL;
    else
        alfI = alf;
    if (QL > 0)
        betI = bet;
    else
        betI = bet;

    if (alfI < 1e-15) {
        if (alfI < 1 - 1e-15)
            val = QL / (duto.area * (1. - alfI));
        else
            val = 0;
    } else {
        double rlmix = rpCi * (1. - betI) + rcCi * betI;
        double vl;
        if (alfI < 1 - 1e-15)
            vl = QL / (duto.area * (1. - alfI));
        else
            vl = 0;
        double vg;
        if (alfI > 1e-15)
            vg = QG / (duto.area * alfI);
        else
            vg = 0.;
        double val1 = (alfI * c0) * (rgCi / rlmix);
        val = (val1 * (vg) + vl * (1. - c0 * alfI)) / (val1 + (1. - c0 * alfI));
    }
    return val;
}

void Cel::WaxDeposition(dadosParafina &detalParafina, int ncel) {

    double BooleanPi = detalParafina.C2C3;
    double C2_star = detalParafina.valC2;
    double C3_star = detalParafina.valC3;
    double BooleanFi = detalParafina.poroRey;
    double ConstFi = detalParafina.valRey;
    double MultipDwax = detalParafina.multDifus;
    double rug = detalParafina.rug;
    double MultipVisc = detalParafina.multVis;
    double alteraViscFlu = detalParafina.alteraViscFlu;
    double DVisc = detalParafina.DViscWax;
    double EVisc = detalParafina.EViscWax;
    double FVisc = detalParafina.FViscWax;
    double D_multip = detalParafina.DmultipWax;
    double E_multip = detalParafina.EmultipWax;
    double F_multip = detalParafina.FmultipWax;
    detParCel.tempInterDeposito = calor.Tcamada[0][0];

    // A fração molar dos componentes parafínicos na fase líquida vecZwaxLiq[i] vem da tabela termodinâmica do PVTsim
    flui.atualizaPropParafina(pres, calor.Tcamada[0][0]);
    double fracOil = (1 - alf) * (1. - bet) * (1 - FW);
    if (calor.Tcamada[0][0] < flui.dCloudPointTOutput && fracOil > 1.0E-2 && calor.fluxIni < 0.) {
        int comp = flui.npseudoWax;
        MW_wax = 0.0;
        rhoWaxLiq = 0.0;
        double SumZwaxLiq = 0.0;
        double SumCwaxLiq = 0.0;
        double sumDen = 0.0;
        double Vwax;
        double vecCwaxLiq[comp];
        for (int i = 0; i < comp; i++) {
            MW_wax = MW_wax + flui.oInterpolatedWaxConcs[i] * flui.oMolecularWeightsOfWaxComponentsOut[i];
            SumZwaxLiq = SumZwaxLiq + flui.oInterpolatedWaxConcs[i];
            vecCwaxLiq[i] = flui.oInterpolatedWaxConcs[i] * flui.oMolecularWeightsOfWaxComponentsOut[i];
            SumCwaxLiq = SumCwaxLiq + vecCwaxLiq[i];
            sumDen = sumDen + vecCwaxLiq[i] / flui.oLiquidDensitiesOfWaxComponents[i];
        }
        MW_wax = MW_wax / SumZwaxLiq;
        rhoWaxLiq = SumCwaxLiq / sumDen;
        Vwax = 1.0E+3 * MW_wax / rhoWaxLiq;

        double Re_f = 0., Re_delta = 0.;
        double Fi;
        double velOilUp = 0.;
        double velSupLiqUp;
        double alfDn;
        double areaDn;
        double velOilDn = 0.;
        double velSupLiqDn;
        double alfUp;
        double areaUp;

        if (QL > 0. && posic > 0) {
            areaDn = dutoL.area;
        } else {
            areaDn = duto.area;
        }
        if (QG > 0. && posic > 0) {
            alfDn = alfL;
        } else {
            alfDn = alf;
        }
        if (QLR < 0. && posic < ncel) {
            areaUp = dutoR.area;
        } else {
            areaUp = duto.area;
        }
        if (MR < 0. && posic < ncel) {
            alfUp = alfR;
        } else {
            alfUp = alf;
        }

        velSupLiqUp = QLR / areaUp;
        velSupLiqDn = QL / areaDn;
        if (fabs(1. - alfUp) > 1e-15)
            velOilUp = velSupLiqUp / (1. - alfUp);
        if (fabs(1. - alfDn) > 1e-15)
            velOilDn = velSupLiqUp / (1. - alfDn);

        double velOil = (velOilUp + velOilDn) / 2.0;
        if (posic == 0)
            velOil = velOilUp;
        double velSupLiq = (velSupLiqUp + velSupLiqDn) / 2.0;
        if (posic == 0)
            velSupLiq = velSupLiqUp;

        double rhoOil = flui.MasEspoleo(pres, temp);
        double alphaLiq = (1 - bet) * (1 - alf) * (1 - FW);
        double Dwax_old = duto.dia;

        int flowPattern = arranjo;
        double delta_old;
        if (parafinado == 0)
            delta_old = 0.;
        else
            delta_old = duto.espessuR[0];
        double rhoLiq = (1. - bet) * rpC + bet * rcC;
        double viscLiq = (1. - bet) * mipC + bet * micC;
        double rhoMix = alf * rgC + (1 - alf) * rhoLiq;

        double Dparaffin;
        double AssocParam = 1.0;
        Sum_dCwaxdT = 0.0;
        double muOil = flui.ViscOleo(pres, temp, 1);
        double muOilf = flui.ViscOleo(pres, calor.Tcamada[0][0], 1);
        Re_f = (rhoOil * velOil * Dwax_old) / (1.0E-3 * muOilf);
        Fi = ConstFi * BooleanFi + (1 - BooleanFi) * (1.0 - pow(Re_f, 0.15) / 8.0);
        porosoPar = Fi;
        flui.coefViscWax = 1.;
        if (alteraViscFlu == 1) {
            double re1;
            double velSupGasUp = ((MR - MliqiniR) / rgRi) / areaUp;
            double velSupGasDn = QG / areaDn;
            double velSupGas = (velSupGasUp + velSupGasDn) / 2.0;
            if (posic == 0)
                velSupGas = velSupGasUp;
            double j1 = velSupGas + velSupLiq;
            double viscmix1 = alf * migC + (1 - alf) * viscLiq;
            if (duto.revest == 0)
                re1 = Rey(duto.a, j1, rhoMix, viscmix1);
            else {
                double dhid = 4 * duto.area / duto.peri;
                re1 = Rey(dhid, j1, rhoMix, viscmix1);
            }
            double f1 = fric(re1, duto.rug / duto.a);
            double tau_int = f1 * rhoMix * j1 * j1 / 2.;
            double dudr_int;
            dudr_int = tau_int / (1.0E-3 * muOilf);
            double coefPedRon;
            // Os multiplicadores D_multip, E_multip e F_multip são passados como 1 se o usuário do Marlim desejar usar os valores default de DVisc, EVisc e FVisc // Samuel - 10/10/25
            DVisc = D_multip * DVisc;
            EVisc = E_multip * EVisc;
            FVisc = F_multip * FVisc;
            flui.coefViscWax = coefPedRon = exp(DVisc * Fi) + EVisc * Fi / pow(dudr_int, 0.5) + FVisc * pow(Fi, 4.0) / dudr_int;
            if (dudr_int >= 10.0 && coefPedRon > 1.0) { // Obs: a correção ocorre somente na viscosidade do óleo e não é utilizada na viscosidade da fase líquida (viscLiq)
                muOil = coefPedRon * muOil;             // Viscosity in Pa s, and shear rate in s^-1
                muOilf = coefPedRon * muOilf;
            }
        }
        muOil = MultipVisc * muOil;
        muOilf = MultipVisc * muOilf;

        double Tint = calor.Tcamada[0][0] + 273.15;
        double MwOil = flui.dInterpolatedLiqMWOutput;
        detParCel.difusividadeParafina = Dparaffin = MultipDwax * 7.4E-12 * Tint * pow(AssocParam * MwOil, 0.5) / (muOilf * pow(Vwax, 0.6)); // imprimir nova saida
        for (int i = 0; i < comp; i++) {
            Sum_dCwaxdT = Sum_dCwaxdT + flui.oInterpolatedMassWaxConcsTDerivOutput[i];
        }

        if (flowPattern == 0 || flowPattern == -1) {
            Re_delta = rhoLiq * velOil * delta_old / (1.0E-3 * viscLiq);
        } // rhoLiq está em kg/m^3? A dimensão deve ser kg/m^3.
        else if (flowPattern == 1 || flowPattern == 2) {
            Re_delta = rhoMix * velSupLiq * delta_old / (alphaLiq * (1.0E-3 * viscLiq));
        } else if (flowPattern == -2) {
            Re_delta = pow(rhoMix * rhoLiq, 0.5) * velSupLiq * delta_old / (alphaLiq * (1.0E-3 * viscLiq));
        }

        double pi1, pi2;
        double C1; //, C2, C3;
        C1 = 15.0;
        C2 = BooleanPi * C2_star + (1 - BooleanPi) * 0.055;
        C3 = BooleanPi * C3_star + (1 - BooleanPi) * 1.4;
        pi1 = C1;
        pi2 = C2 * pow(Re_delta, C3);

        // Obtain the deposit thickness, free flow diameter, specific mass of the solid phase and deposit, specific heat and conductivity of the deposit

        double rhoWaxSolid = 1.1 * rhoWaxLiq;
        double ddeltadt;
        double heatFluxInt = -calor.fluxIni;
        double kMix;
        double condliq = (1. - bet) * flui.CondLiq(pres, temp) + bet * fluicol.CondLiq(pres, temp);
        double kOil = flui.CondOleo(pres, temp);
        kMix = condliq * (1 - alf) + flui.CondGas(pres, temp) * alf;

        // QUESTION: If a small liquid fraction is still present in the cell, the deposition
        // entry condition may allow the calculation to reach this point. If the oil fraction
        // is very small, could the calculated deposited mass exceed the amount of oil
        // available in the cell?
        //
        // TODO: Consider adding a validation to prevent the deposited mass from exceeding
        // the available oil mass.
        //
        // TODO: Evaluate this behavior under production shutdown, fluid segregation,
        // reverse flow, paraffin fraction reduction, and other similar scenarios.

        // FIXME: Temporarily using the absolute value for testing purposes.
        // A negative thickness indicates that something may be incorrect in the calculation.
        double dCwaxdTemp = Sum_dCwaxdT;
        if (Sum_dCwaxdT < 0)
            dCwaxdTemp = 0.;
        detParCel.gradienteConcentracao = (heatFluxInt / kMix) * dCwaxdTemp;
        detParCel.fluxMassParafina2 = (pi1)*rhoLiq * Fi * Dparaffin * detParCel.gradienteConcentracao;
        detParCel.fluxMassParafina1 = detParCel.fluxMassParafina2 / (1.0 + pi2);
        ddeltadt = (pi1 / (1.0 + pi2)) * ((rhoLiq * Fi) / (rhoWaxSolid * (1.0 - Fi))) * Dparaffin * detParCel.gradienteConcentracao;
        double delta = ddeltadt * dt;
        deltaPar = delta;

        double rhoDep = Fi * rhoOil + (1.0 - Fi) * rhoWaxSolid;
        double cpDep = flui.dInterpolatedCPWaxOutput;

        detParCel.kDep = ((2 * flui.dInterpolatedThermCondOutput + kOil + (flui.dInterpolatedThermCondOutput - kOil) * (1.0 - Fi)) /
                          (2 * flui.dInterpolatedThermCondOutput + kOil - 2 * (flui.dInterpolatedThermCondOutput - kOil) * (1.0 - Fi))) * kOil;

        if (parafinado == 0 && delta > 0.) {
            duto.atualizaCamada(delta, rug, cpDep, detParCel.kDep, rhoDep);
            calor.atualiza(duto, 1);
            parafinado = 1;
        } else if (delta > 0) {
            duto.atualizaCamada2(delta, cpDep, detParCel.kDep, rhoDep);
            calor.atualiza2(duto);
        }
    }
}

FullMtx<double> Cel::Jacobiana() {
    FullMtx<double> jac(3, 3);

    double alfI;
    double betI;
    double vl;
    double vg;
    double rlmix;
    double cg2;
    if (QG > 0)
        alfI = alfL;
    else
        alfI = alf;
    if (QL > 0)
        betI = bet;
    else
        betI = bet;

    double tempMed;
    double raz;
    if (posic == 0)
        tempMed = temp;
    else {
        raz = dxL / (dx + dxL);
        tempMed = (1. - raz) * tempL + raz * temp;
    }
    double kad = flui.ConstAdG(presaux, tempMed, rgCi);
    rlmix = rpCi * (1. - betI) + rcCi * betI;
    cg2 = kad / flui.drhodp(presaux, tempMed);
    if (alfI < 1 - 1e-15)
        vl = QL / (duto.area * (1. - alfI));
    else
        vl = 0;
    if (alfI > 1e-15)
        vg = QG / (duto.area * alfI);
    else
        vg = 0.;
    double den = (1. - c0 * alfI * (1. - rgCi / rlmix));
    double razDen = rgCi / rlmix;
    jac[0][0] = vg * c0 * alfI * (rgCi / rlmix) / den;
    jac[0][1] = -vg * (1. - c0 * alfI) / den;
    jac[0][2] = (1. - c0 * alfI) / den;
    jac[1][0] = -vg * c0 * alfI * (rgCi / rlmix) / den;
    jac[1][1] = vg * (1. - c0 * alfI) / den;
    jac[1][2] = c0 * alfI * (rgCi / rlmix) / den;
    if (alfI > 1e-15) {
        jac[2][0] = (rlmix * (cg2 * razDen - vl * vl * alfI) + c0 * alfI * (cg2 * rgCi * (razDen - 1.) + alfI * (-2. * vg * vg * rgCi + 2. * vg * vl * rgCi + vl * vl * rlmix * (1 - razDen)))) / (alfI * rlmix * den);
        jac[2][1] = ((cg2 + vg * alfI * (vg - 2. * vl)) - c0 * alfI * (cg2 * (1. - razDen) + vg * alfI * (-2. * vl + vg * (razDen + 1.)))) / (alfI * den);
    }
    jac[2][2] = 2. * (vl + c0 * alfI * (vg * razDen - vl)) / den;

    return jac;
}

Vcr<double> Cel::autoVec(int ind, FullMtx<double> jac) {
    Vcr<double> autvec(3);

    double autval;
    if (ind == 2) {
        double alfI;
        double vg;
        if (QG > 0)
            alfI = alfL;
        else
            alfI = alf;
        if (alfI > 1e-15)
            vg = QG / (duto.area * alfI);
        else
            vg = 0.;
        autval = vg;
    } else {
        double velsom = somVel();
        double term = termAdSomVel();
        if (ind == 0)
            autval = term + velsom;
        else
            autval = term - velsom;
    }

    autvec[0] = 1.;
    Vcr<double> TL(2);
    TL[0] = autval - jac[0][0];
    TL[1] = -jac[1][0];

    FullMtx<double> matResto(2, 2);
    matResto[0][0] = jac[0][1];
    matResto[0][1] = jac[0][2];
    matResto[1][0] = jac[1][1] - autval;
    matResto[1][1] = jac[1][2];
    matResto.GaussElimCP(TL);
    autvec[1] = TL[0];
    autvec[2] = TL[1];
    return autvec;
}

Vcr<double> Cel::flutua() {
    Vcr<double> estados(3);
    FullMtx<double> jac(3, 3);
    FullMtx<double> matAutoVec(3, 3);
    jac = Jacobiana();

    double alfI;
    double betI;
    if (QG > 0)
        alfI = alfL;
    else
        alfI = alf;
    if (QL > 0)
        betI = bet;
    else
        betI = bet;

    double vl;
    if (alfI < 1 - 1e-15)
        vl = QL / (duto.area * (1. - alfI));
    else
        vl = 0;
    double vg;
    if (alfI > 1e-15)
        vg = QG / (duto.area * alfI);
    else
        vg = 0.;
    double rlmix = rpCi * (1. - betI) + rcCi * betI;

    estados[0] = (1. - alfI) * rlmix;
    estados[1] = alfI * rgCi;
    estados[2] = (1. - alfI) * rlmix * vl + alfI * rgCi * vg;

    for (int i = 0; i < 3; i++) {
        Vcr<double> autvec(3);
        autvec = autoVec(i, jac);
        for (int j = 0; j < 3; j++) {
            matAutoVec[j][i] = autvec[j];
        }
    }

    matAutoVec.GaussElimCP(estados);
    return estados;
}