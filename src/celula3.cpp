#include "celula3.h"
#include "Leitura.h"

Cel::Cel(varGlob1D* Vvg1dSP,const DadosGeo vdutoL,const DadosGeo vduto,
		   const DadosGeo vdutoR,
           const ProFlu vflui,const ProFluCol vfluicol,
           const double vtempL,const double vtemp,
           const double vtempR,const double vtempini,
           const double vpresL,const double vpres,const double vpresR,
           const double vML,const double vMC,const double vMR,
           const double vMliqiniL,const double vMliqini,const double vMliqiniR,
           const double valfL,const double valf,const double valfR,
           const double vbetL,const double vbet,const double vbetR,
           const double vdxL,const double vdx,const double vdxR,
           const double vdt, const int vposic,
           const TransCal vcalor,
           const acessorio vacsr,
           acessorio* vacsrL) :
		TL(2), local(2,6) { //construtor default

	//Solver de Hidratos

	// --- Solver de Hidratos: inicialização ---
	// fluxos em massa (por área) – começam em zero
	j_H = 0.0L;
	j_G = 0.0L;
	j_W = 0.0L;

	// volumes físicos
	V_h = 0.0L;   // sem hidrato no início
	V_w=0.;
	//V_w = 0.0L;   // será atribuído em solverHidrato() usando BSW

	FVH = 0.0L;
	agua_consumida=0.;
	massa_hidrato=0.0L; //chris - Hidratos
	
	vg1dSP=Vvg1dSP;
	dutoL = vdutoL;
	duto = vduto;
	dutoR = vdutoR;
	flui = vflui;
	flui.posic=vposic;
	fluiL=0;
	fluiR=0;
	fluicol=vfluicol;
	acsr=vacsr;
	acsrL=vacsrL;
	calor=vcalor;
	tempR = vtempR;
	temp = vtemp;
	tempL = vtempL;
	tempini = vtempini;
	presL=vpresL;
	presLini=vpresL;
	presauxL=vpresL;
	presaux=pres=presini=vpres;
	dpresaux=0.;
	presauxR=vpresR;
	presR = vpresR;
	dxL = vdxL;
	dx = vdx;
	dxR = vdxR;
    razdxTM=0.;
    razdxTM0=0.;
	dt = vdt;
	dt1=dt;
	dt2=dt;
	dTdt=0.;
	dTdtIni=0.;
	dTdtL=0.;
	posic = vposic;
	ML = vML;
	MC = vMC;
	MR = vMR;
	MRini = MR;
	alfL=valfL;
	alfIter=alf=valf;
	alfR=valfR;
	betL=vbetL;
	bet=vbet;
	betR=vbetR;
	betLI=betL;
	betI=bet;
	betRI=betR;
	MliqiniL = vMliqiniL;
	Mliqini = vMliqini;
	MliqiniR = vMliqiniR;
	MliqiniR0 = vMliqiniR;// alteracao7
	MComp=0.;
	//////////////////////////////////////////////////////////////////
	presRini=presR;
	MCini=MC;
	MLini=ML;
	Mliqini0=Mliqini;
	MliqiniL0=MliqiniL;
	tempini=temp;
	tempLini=tempL;
	tempRini=tempR;
	presauxini=presaux;
	/////////////////////////////////////////////////////////////////
	alfLini=alfL;
	alfini=alf;
	alfRini=alfR;
	betLini=betL;
	betini=bet;
	betRini=betR;
	FWini=FW=flui.BSW;
	massfonteCH=0;
	c0=1.;
	ud=0.;
	c0ini=c0;
	udini=ud;
	c0Spare=c0;
	udSpare=ud;
    transmassL=0.;
    transmassR=0.;
    FonteMudaFase=0.;
    DTransDt1=0.;
    DTransDt0=0.;
    DTransDx=0.;
    DTransDxR=0.;
    DTransDxL=0.;
    DTransDxLp=0.;
    DTransDxRp=0.;
    DtransDxLinear=0;
    DTransDxLpini=0.;
    DTransDxRpini=0.;
    DTransDtp=0.;
    DTransDtpL=0.;
    DTransDtpini=0.;
    DTransDtT=0.;
    DTransDtTL=0.;
    CoefDTR=0.;
    CoefDTL=0.;
    fontedissolv=0.;
    ativaDeri=0.;
    coefTransBet=0.;
    TMModel=2;
    TMModelL=2;
    fontemassLL=0.;
    fontemassGL=0.;
    fontemassCL=0.;
    fontemassLR=0.;
    fontemassGR=0.;
    fontemassCR=0.;
    dpB=0.;
    potB=0;
    potBT=0;
    dpBL=0.;
    potBL=0;
    potBTL=0;
    cinematico=1.;
    arranjo=1;
    arranjoR=1;
	if(fabsl(ML)<1e-5){
		term1L=0.;
		term2L=0.;
	}
	else{
		term1L=MliqiniL/ML;
		term2L=0.;
	}
	if(fabsl(MC)<1e-5){
		term1=0.;
		term2=0.;
	}
	else{
		term1=Mliqini/MC;
		term2=0.;
	}
	if(fabsl(MR)<1e-5){
		term1R=0.;
		term2R=0.;
	}
	else{
		term1R=MliqiniR/MR;
		term2R=0.;
	}
	double localtinyTemp;
	if(vg1dSP!=0)localtinyTemp=(*vg1dSP).localtiny;
	else localtinyTemp=1e-9;
	if(fabsl(alf)>=localtinyTemp && fabsl(alf)<=1-localtinyTemp)transic=0;
	else if(fabsl(alf)>=localtinyTemp)transic=-1;
	else transic=1;
	transic0=transic;
	double razdx;
	razdx=dx/(dx+dxL);
	double pmed=razdx*pres+(1-razdx)*presL;
	double rl=flui.MasEspLiq(pmed,temp);
	double rc=fluicol.MasEspFlu(pmed,temp);
	double rg=flui.MasEspGas(pmed,temp);

	rpL=rpLi=flui.MasEspLiq(presL,tempL);
	rpC=rpCi=flui.MasEspLiq(pres,temp);
	rpR=rpRi=flui.MasEspLiq(presR,tempR);
	rcL=rcLi=fluicol.MasEspFlu(presL,tempL);
	rcC=rcCi=fluicol.MasEspFlu(pres,temp);
	rcR=rcRi=fluicol.MasEspFlu(presR,tempR);
	rgL=rgLi=flui.MasEspGas(presL,tempL);
	rgC=rgCi=flui.MasEspGas(pres,temp);
	rgR=rgRi=flui.MasEspGas(presR,tempR);
	drhogdt=flui.drhodt(presL,tempL);
	double rmisL=rpL*(1-betL)+rcL*betL;
	double rmisR=rpR*(1-betR)+rcR*betR;
	double rmisC=rl*(1-bet)+rc*bet;

    mipC=flui.ViscOleo(pres, temp);
    micC=fluicol.VisFlu(pres, temp);
    migC=flui.ViscGas(pres, temp);
    mipR=flui.ViscOleo(pres, temp);
    micR=fluicol.VisFlu(pres, temp);
    migR=flui.ViscGas(pres, temp);

	QL=Mliqini/rmisC;
	QG=(MC-Mliqini)/rg;

	QLL=MliqiniL/rmisL;
	QLR=MliqiniR/rmisR;
	QLRini=MliqiniR/rmisR;// alteracao7

	if(vdutoR.area>0.)VTemper=(QL+QG)/vdutoR.area;
	else VTemper=0.;

	perdaEstratL=0.;
	perdaEstratG=0.;

	estadoPig=0;
	estadoPigini=0;
	dtPig=dt;
	alfPigE=1.-alf;
	alfPigER=alfPigE;
	alfPigD=alfPigE;
	betPigE=bet;
	betPigD=bet;
	alfPigEini=1.-alf;
	alfPigDini=alfPigE;
	betPigEini=bet;
	betPigDini=bet;
	velPig=0.;
	velPigini=0.;
	razPig=0.;
	razPigini=0.;
	RazAreaPig=0.;//alteracao2
    DelPig=0.;//alteracao2
    VazaPig=0.;//alteracao2
    cdpig=0.;//alteracao2
    indpig=-1;//alteracao2

    double v1=flui.MasEspGas(pres, temp);
    //double v2=flui.MasEspGas(1., 15.6);
    double v2=1.225*flui.Deng;
    double v3=flui.RS(pres,temp);
    double v4=flui.BOFunc(pres,temp);

	VolLeveST=((alf*flui.MasEspGas(pres, temp)/(1.225*flui.Deng)/*flui.MasEspGas(1., 15.6)*/)+
			(1.-alf)*(1-bet)*(1.-flui.BSW)*flui.RS(pres,temp)/flui.BOFunc(pres,temp));
    VolPesaST=(1.-alf)*(1-bet)*(1.-flui.BSW)/flui.BOFunc(pres,temp);
    VolAguaST=(1.-alf)*(1-bet)*flui.BSW;

    DmasschokeG=0.;
    DmasschokeL=0.;
    DmasschokeC=0.;

    chutearranjo=-1.;

    ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
    presBuf=pres;
    presLiniBuf=presLini;
    presLBuf=presL;
    presRBuf=presR;
    presauxBuf=presaux;
    MCBuf=MC;
    MRBuf=MR;
    MliqiniRBuf=MliqiniR;
    MRiniBuf=MRini;
    MLBuf=ML;
    MliqiniBuf=Mliqini;
    MliqiniLBuf=MliqiniL;
    //////////////////////////// 05-07-2018 ///////////////////////////////////////////////

    corrigeMassaPres=0.;

    fluxcalmed=0.;

    ////novas saidas
    termoHidro=0.;
    termoFric=0.;
    /////////////////////////////////////////////////////////////////////////////////////
	QLLini=QLL;
	QLRini=QLR;
	QLini=QL;
	QGini=QG;
	presauxRini=presauxR;
	presauxLini=presauxL;
	fontemassLLini=fontemassLL;
	fontemassGLini=fontemassGL;
	fontemassCLini=fontemassCL;
	fontemassLRini=fontemassLR;
	fontemassGRini=fontemassGR;
	fontemassCRini=fontemassCR;
	transmassLini=transmassL;
	transmassRini=transmassR;
	FonteMudaFaseini=FonteMudaFase;
	DTransDt1ini=DTransDt1;
	DTransDt0ini=DTransDt0;
	DTransDxRini=DTransDxR;
	DTransDxLini=DTransDxL;
	coefTransBetini=coefTransBet;
	CoefDTRini=CoefDTR;
	CoefDTLini=CoefDTL;
	VTemperini=VTemper;

	termRug=pow(duto.rug/duto.dia / 3.7, 1.11);

	angEsq=0.;
	angDir=0.;

	mudaDT=0;
	mudaDTL=0;

    dpdt=0.;
    dpdtIni=0.;
    m2d=0.;
    m2dL=0.;
    d2pdt2=0.;

    alfPigERini=alfPigER;
    indpigini=indpig;
    betIini=betI;
    betRIini=betRI;
    betLIini=betLI;

    estabCol=0;

    mudaArea=0;

    nMol=0.;
    nMolIni=0.;

    dR=0.;

    indGeom=0;

    dPdLHidro=1.;
    dPdLFric=1.;
    dTdLCor=1.;
    inddPdLHidro=-1;
    inddPdLFric=-1;
    inddTdLCor=-1;

    reiniciaAlf=0;
    reiniciaBet=0;
    reiniciaPig=0;

    correrGlobHol=1;

    miniTabAtraso=0;

    modoParafina=0;
    parafinado=0;
    deltaPar=0;

    fluxCal2D=0.;
    C2=0.;
    C3=0.;
    diamInterIni=duto.a;
    porosoPar=0.;
    MW_wax=0.;
    rhoWaxLiq=0.;
    Sum_dCwaxdT=0.;

	correlacaoMR2=-1;

	fonteCompartilhada=0;
	multiFComp=1.0;

	fluxcalAcopRedeP=0.;
	resAcopRedeP=0.;
}

Cel::Cel(const Cel& vcel) :
			TL(2), local(2, 6) { //construtor por copia
			
				//Solver de Hidratos
			j_H=vcel.j_H;
			j_G=vcel.j_G;
			j_W=vcel.j_W;
			V_h=vcel.V_h;
			V_w=vcel.V_w;
			FVH=vcel.FVH;
			massa_hidrato=vcel.massa_hidrato; //chris - Hidratos
			agua_consumida=vcel.agua_consumida; //chris - Hidratos
	
			vg1dSP=vcel.vg1dSP;
			dutoL = vcel.dutoL;
			duto = vcel.duto;
			dutoR = vcel.dutoR;
			flui = vcel.flui;
			flui.posic=vcel.posic;
			fluiL = vcel.fluiL;
			fluiR = vcel.fluiR;
			fluicol = vcel.fluicol;
			acsr=vcel.acsr;
			acsrL=vcel.acsrL;
			calor=vcel.calor;
			tempR = vcel.tempR;
			temp = vcel.temp;
			tempL = vcel.tempL;
			tempini = vcel.tempini;
			presL= vcel.presL;
			presLini=vcel.presLini;
			pres= vcel.pres;
			presini= vcel.presini;
			presauxL= vcel.presauxL;
			presaux= vcel.presaux;
			dpresaux=vcel.dpresaux;
			presauxR= vcel.presauxR;
			presR = vcel.presR;
			dxL = vcel.dxL;
			dx = vcel.dx;
			dxR = vcel.dxR;
		    razdxTM=vcel.razdxTM;
		    razdxTM0=vcel.razdxTM0;
			dt = vcel.dt;
			dt1=vcel.dt1;
			dt2 = vcel.dt2;
			dTdt=vcel.dTdt;
			dTdtIni=vcel.dTdtIni;
			dTdtL=vcel.dTdtL;
			posic = vcel.posic;
			ML = vcel.ML;
			MC = vcel.MC;
			MR = vcel.MR;
			MRini=vcel.MRini;
			//////////////////////////////////////////////////////////////////
			presRini=vcel.presRini;
			MCini=vcel.MCini;
			MLini=vcel.MLini;
			Mliqini0=vcel.Mliqini0;
			MliqiniL0=vcel.MliqiniL0;
			MComp=vcel.MComp;
			tempini=vcel.tempini;
			tempLini=vcel.tempLini;
			tempRini=vcel.tempRini;
			presauxini=vcel.presauxini;
			/////////////////////////////////////////////////////////////////
			alfL= vcel.alfL;
			alf= vcel.alf;
			alfIter= vcel.alfIter;
			alfR= vcel.alfR;
			betL= vcel.betL;
			bet= vcel.bet;
			betR= vcel.betR;
			betLI= vcel.betLI;
			betI= vcel.betI;
			betRI= vcel.betRI;
			MliqiniL = vcel.MliqiniL;
			Mliqini = vcel.Mliqini;
			MliqiniR = vcel.MliqiniR;
			MliqiniR0 = vcel.MliqiniR0;// alteracao7
			alfLini= vcel.alfLini;
			alfini= vcel.alfini;
			alfRini= vcel.alfRini;
			betLini= vcel.betLini;
			betini= vcel.betini;
			betRini= vcel.betRini;
			FW=vcel.FW;
			FWini=vcel.FWini;
			TL=vcel.TL;
			local=vcel.local;
			massfonteCH=vcel.massfonteCH;
			c0=vcel.c0;
			ud=vcel.ud;
			c0ini=vcel.c0ini;
			udini=vcel.udini;
			c0Spare=vcel.c0Spare;
			udSpare=vcel.udSpare;
		    transmassL=vcel.transmassL;
		    transmassR=vcel.transmassR;
		    FonteMudaFase=vcel.FonteMudaFase;
		    DTransDt1=vcel.DTransDt1;
		    DTransDt0=vcel.DTransDt0;
		    DTransDx=vcel.DTransDx;
		    DTransDxR=vcel.DTransDxR;
		    DTransDxL=vcel.DTransDxL;
		    DTransDxLp=vcel.DTransDxLp;
		    DTransDxRp=vcel.DTransDxRp;
		    DtransDxLinear=vcel.DtransDxLinear;
		    DTransDxLpini=vcel.DTransDxLpini;
		    DTransDxRpini=vcel.DTransDxRpini;
		    DTransDtp=vcel.DTransDtp;
		    DTransDtpL=vcel.DTransDtpL;
		    DTransDtpini=vcel.DTransDtpini;
		    DTransDtT=vcel.DTransDtT;
		    DTransDtTL=vcel.DTransDtTL;
		    CoefDTR=vcel.CoefDTR;
		    CoefDTL=vcel.CoefDTL;
		    fontedissolv=vcel.fontedissolv;
		    ativaDeri=vcel.ativaDeri;
		    coefTransBet=vcel.coefTransBet;
		    TMModel=vcel.TMModel;
		    TMModelL=vcel.TMModelL;
		    fontemassLL=vcel.fontemassLL;
		    fontemassGL=vcel.fontemassGL;
		    fontemassCL=vcel.fontemassCL;
		    fontemassLR=vcel.fontemassLR;
		    fontemassGR=vcel.fontemassGR;
		    fontemassCR=vcel.fontemassCR;
		    dpB=vcel.dpB;
		    potB=vcel.potB;
		    potBT=vcel.potBT;
		    dpBL=vcel.dpBL;
		    potBL=vcel.potBL;
		    potBTL=vcel.potBTL;
			term1L=vcel.term1L;
			term2L=vcel.term2L;
			term1=vcel.term1;
			term2=vcel.term2;
			term1R=vcel.term1R;
			term2R=vcel.term2R;
			transic=vcel.transic;
			transic0=vcel.transic0;
			QL=vcel.QL;
			QG=vcel.QG;
			QLL=vcel.QLL;
			QLR=vcel.QLR;
			QLRini=vcel.QLRini;// alteracao7
			cinematico=vcel.cinematico;
			arranjo=vcel.arranjo;
			arranjoR=vcel.arranjoR;
			VTemper=vcel.VTemper;

			rpL=vcel.rpL;
			rpC=vcel.rpC;
			rpR=vcel.rpR;
			rcL=vcel.rcL;
			rcC=vcel.rcC;
			rcR=vcel.rcR;
			rgL=vcel.rgL;
			rgC=vcel.rgC;
			rgR=vcel.rgR;
			drhogdt=vcel.drhogdt;

			rgLi=vcel.rgLi;
			rgCi=vcel.rgCi;
			rgRi=vcel.rgRi;
			rpLi=vcel.rpLi;
			rpCi=vcel.rpCi;
			rpRi=vcel.rpRi;
			rcLi=vcel.rcLi;
			rcCi=vcel.rcCi;
			rcRi=vcel.rcRi;

		    mipC=vcel.mipC;
		    micC=vcel.micC;
		    migC=vcel.migC;
		    mipR=vcel.mipR;
		    micR=vcel.micR;
		    migR=vcel.migR;

			perdaEstratL=vcel.perdaEstratL;
			perdaEstratG=vcel.perdaEstratG;

			estadoPig=vcel.estadoPig;
			estadoPigini=vcel.estadoPigini;
			dtPig = vcel.dtPig;
			alfPigE=vcel.alfPigE;
			alfPigER=vcel.alfPigER;
			alfPigD=vcel.alfPigD;
			betPigE=vcel.betPigE;
			betPigD=vcel.betPigD;
			alfPigEini=vcel.alfPigEini;
			alfPigDini=vcel.alfPigDini;
			betPigEini=vcel.betPigEini;
			betPigDini=vcel.betPigDini;
			velPig=vcel.velPig;
			velPigini=vcel.velPigini;
			razPig=vcel.razPig;
			razPigini=vcel.razPigini;
			RazAreaPig=vcel.RazAreaPig;//alteracao2
		    DelPig=vcel.DelPig;//alteracao2
		    VazaPig=vcel.VazaPig;//alteracao2
		    cdpig=vcel.cdpig;//alteracao2
		    indpig=vcel.indpig;//alteracao2

			VolLeveST=vcel.VolLeveST;
		    VolPesaST=vcel.VolPesaST;
		    VolAguaST=vcel.VolAguaST;

		    DmasschokeG=vcel.DmasschokeG;
		    DmasschokeL=vcel.DmasschokeL;
		    DmasschokeC=vcel.DmasschokeC;

		    chutearranjo=vcel.chutearranjo;

		    ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
		    presBuf=vcel.presBuf;
		    presLiniBuf=vcel.presLiniBuf;
		    presLBuf=vcel.presLBuf;
		    presRBuf=vcel.presRBuf;
		    presauxBuf=vcel.presauxBuf;
		    MCBuf=vcel.MCBuf;
		    MRBuf=vcel.MRBuf;
		    MliqiniRBuf=vcel.MliqiniRBuf;
		    MRiniBuf=vcel.MRiniBuf;
		    MLBuf=vcel.MLBuf;
		    MliqiniBuf=vcel.MliqiniBuf;
		    MliqiniLBuf=vcel.MliqiniLBuf;
		    //////////////////////////// 05-07-2018 ///////////////////////////////////////////////

		    corrigeMassaPres=0.;

		    fluxcalmed=vcel.fluxcalmed;

		    ////novas saidas
		    termoHidro=vcel.termoHidro;
		    termoFric=vcel.termoFric;
		    /////////////////////////////////////////////////////////////////////////////////////
			QLLini=vcel.QLLini;
			QLRini=vcel.QLRini;
			QLini=vcel.QLini;
			QGini=vcel.QGini;
			presauxRini=vcel.presauxRini;
			presauxLini=vcel.presauxLini;
			fontemassLLini=vcel.fontemassLLini;
			fontemassGLini=vcel.fontemassGLini;
			fontemassCLini=vcel.fontemassCLini;
			fontemassLRini=vcel.fontemassLRini;
			fontemassGRini=vcel.fontemassGRini;
			fontemassCRini=vcel.fontemassCRini;
			transmassLini=vcel.transmassLini;
			transmassRini=vcel.transmassRini;
			FonteMudaFaseini=vcel.FonteMudaFaseini;
			DTransDt1ini=vcel.DTransDt1ini;
			DTransDt0ini=vcel.DTransDt0ini;
			DTransDxRini=vcel.DTransDxRini;
			DTransDxLini=vcel.DTransDxLini;
			coefTransBetini=vcel.coefTransBetini;
			CoefDTRini=vcel.CoefDTRini;
			CoefDTLini=vcel.CoefDTLini;
			VTemperini=vcel.VTemperini;

			termRug=vcel.termRug;

			angEsq=vcel.angEsq;
			angDir=vcel.angDir;

			mudaDT=0;
			mudaDTL=0;
			dpdt=vcel.dpdt;
			dpdtIni=vcel.dpdtIni;
		    m2d=vcel.m2d;
		    m2dL=vcel.m2dL;
		    d2pdt2=vcel.d2pdt2;

		    alfPigERini=vcel.alfPigERini;
		    indpigini=vcel.indpigini;
		    betIini=vcel.betIini;
		    betRIini=vcel.betRIini;
		    betLIini=vcel.betLIini;

		    estabCol=vcel.estabCol;

		    mudaArea=vcel.mudaArea;

		    nMol=vcel.nMol;
		    nMolIni=vcel.nMolIni;

		    dR=vcel.dR;

		    indGeom=vcel.indGeom;

		    dPdLHidro=vcel.dPdLHidro;
		    dPdLFric=vcel.dPdLFric;
		    dTdLCor=vcel.dTdLCor;

		    inddPdLHidro=vcel.inddPdLHidro;
		    inddPdLFric=vcel.inddPdLFric;
		    inddTdLCor=vcel.inddTdLCor;

		    reiniciaAlf=vcel.reiniciaAlf;
		    reiniciaBet=vcel.reiniciaBet;
		    reiniciaPig=vcel.reiniciaPig;

		    correrGlobHol=1;

		    miniTabAtraso=vcel.miniTabAtraso;

		    modoParafina=vcel.modoParafina;
		    parafinado=vcel.parafinado;
		    deltaPar=vcel.deltaPar;

		    fluxCal2D=vcel.fluxCal2D;

		    C2=vcel.C2;
		    C3=vcel.C3;
		    diamInterIni=vcel.diamInterIni;
		    porosoPar=vcel.porosoPar;
		    MW_wax=vcel.MW_wax;
		    rhoWaxLiq=vcel.rhoWaxLiq;
		    Sum_dCwaxdT=vcel.Sum_dCwaxdT;

			correlacaoMR2=vcel.correlacaoMR2;

			fonteCompartilhada=vcel.fonteCompartilhada;
			multiFComp=vcel.multiFComp;

			fluxcalAcopRedeP=vcel.fluxcalAcopRedeP;
			resAcopRedeP=vcel.resAcopRedeP;

}

Cel& Cel::operator =(const Cel& vcel) {
	if (this != &vcel) {
	
		//Solver de Hidratos
		j_H=vcel.j_H;
		j_G=vcel.j_G;
		j_W=vcel.j_W;
		V_h=vcel.V_h;
		V_w=vcel.V_w;
		FVH=vcel.FVH;
		massa_hidrato=vcel.massa_hidrato; //chris - Hidratos
		agua_consumida=vcel.agua_consumida;
		
		vg1dSP=vcel.vg1dSP;
		dutoL = vcel.dutoL;
		duto = vcel.duto;
		dutoR = vcel.dutoR;
		flui = vcel.flui;
		flui.posic=vcel.posic;
		fluiL = vcel.fluiL;
		fluiR = vcel.fluiR;
		fluicol = vcel.fluicol;
		acsr=vcel.acsr;
		acsrL=vcel.acsrL;
		calor=vcel.calor;
		tempR = vcel.tempR;
		temp = vcel.temp;
		tempL = vcel.tempL;
		tempini = vcel.tempini;
		presL= vcel.presL;
		presLini=vcel.presLini;
		pres= vcel.pres;
		presini= vcel.presini;
		presauxL= vcel.presauxL;
		presaux= vcel.presaux;
		dpresaux=vcel.dpresaux;
		presauxR= vcel.presauxR;
		presR = vcel.presR;
		dxL = vcel.dxL;
		dx = vcel.dx;
		dxR = vcel.dxR;
	    razdxTM=vcel.razdxTM;
	    razdxTM0=vcel.razdxTM0;
		dt = vcel.dt;
		dt1=vcel.dt1;
		dt2 = vcel.dt2;
		dTdt=vcel.dTdt;
		dTdtIni=vcel.dTdtIni;
		dTdtL=vcel.dTdtL;
		posic = vcel.posic;
		ML = vcel.ML;
		MC = vcel.MC;
		MR = vcel.MR;
		MRini=vcel.MRini;
		alfL= vcel.alfL;
		alf= vcel.alf;
		alfIter= vcel.alfIter;
		alfR= vcel.alfR;
		betL= vcel.betL;
		bet= vcel.bet;
		betR= vcel.betR;
		betLI= vcel.betLI;
		betI= vcel.betI;
		betRI= vcel.betRI;
		FW=vcel.FW;
		FWini=vcel.FWini;
		MliqiniL = vcel.MliqiniL;
		Mliqini = vcel.Mliqini;
		MliqiniR = vcel.MliqiniR;
		MliqiniR0 = vcel.MliqiniR0;// alteracao7
		//////////////////////////////////////////////////////////////////
		presRini=vcel.presRini;
		MCini=vcel.MCini;
		MLini=vcel.MLini;
		Mliqini0=vcel.Mliqini0;
		MliqiniL0=vcel.MliqiniL0;
		MComp=vcel.MComp;
		tempini=vcel.tempini;
		tempLini=vcel.tempLini;
		tempRini=vcel.tempRini;
		presauxini=vcel.presauxini;
		/////////////////////////////////////////////////////////////////
		alfLini= vcel.alfLini;
		alfini= vcel.alfini;
		alfRini= vcel.alfRini;
		betLini= vcel.betLini;
		betini= vcel.betini;
		betRini= vcel.betRini;
		TL=vcel.TL;
		local=vcel.local;
		massfonteCH=vcel.massfonteCH;
		c0=vcel.c0;
		ud=vcel.ud;
		c0ini=vcel.c0ini;
		udini=vcel.udini;
		c0Spare=vcel.c0Spare;
		udSpare=vcel.udSpare;
	    transmassL=vcel.transmassL;
	    transmassR=vcel.transmassR;
	    FonteMudaFase=vcel.FonteMudaFase;
	    DTransDt1=vcel.DTransDt1;
	    DTransDt0=vcel.DTransDt0;
	    DTransDx=vcel.DTransDx;
	    DTransDxR=vcel.DTransDxR;
	    DTransDxL=vcel.DTransDxL;
	    DTransDxLp=vcel.DTransDxLp;
	    DTransDxRp=vcel.DTransDxRp;
	    DtransDxLinear=vcel.DtransDxLinear;
	    DTransDxLpini=vcel.DTransDxLpini;
	    DTransDxRpini=vcel.DTransDxRpini;
	    DTransDtp=vcel.DTransDtp;
	    DTransDtpL=vcel.DTransDtpL;
	    DTransDtpini=vcel.DTransDtpini;
	    DTransDtT=vcel.DTransDtT;
	    DTransDtTL=vcel.DTransDtTL;
	    fontedissolv=vcel.fontedissolv;
	    ativaDeri=vcel.ativaDeri;
	    CoefDTR=vcel.CoefDTR;
	    CoefDTL=vcel.CoefDTL;
	      ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
        /*double presBuf;
        double presLiniBuf;
        double presLBuf;
        double presRBuf;
        double presauxBuf;
        double MCBuf;
        double MRBuf;
        double MliqiniRBuf;
        double MRiniBuf;
        double MLBuf;
        double MliqiniBuf;
        double MliqiniLBuf;*/
        //////////////////////////// 05-07-2018 ///////////////////////////////////////////////
	    coefTransBet=vcel.coefTransBet;
	    TMModel=vcel.TMModel;
	    TMModelL=vcel.TMModelL;
	    fontemassLL=vcel.fontemassLL;
	    fontemassGL=vcel.fontemassGL;
	    fontemassCL=vcel.fontemassCL;
	    fontemassLR=vcel.fontemassLR;
	    fontemassGR=vcel.fontemassGR;
	    fontemassCR=vcel.fontemassCR;
	    dpB=vcel.dpB;
	    potB=vcel.potB;
	    potBT=vcel.potBT;
	    dpBL=vcel.dpBL;
	    potBL=vcel.potBL;
	    potBTL=vcel.potBTL;
		term1L=vcel.term1L;
		term2L=vcel.term2L;
		term1=vcel.term1;
		term2=vcel.term2;
		term1R=vcel.term1R;
		term2R=vcel.term2R;
		transic=vcel.transic;
		transic0=vcel.transic0;
		QL=vcel.QL;
		QG=vcel.QG;
		QLL=vcel.QLL;
		QLR=vcel.QLR;
		QLRini=vcel.QLRini;// alteracao7
		cinematico=vcel.cinematico;
		arranjo=vcel.arranjo;
		arranjoR=vcel.arranjoR;
		VTemper=vcel.VTemper;

		rpL=vcel.rpL;
		rpC=vcel.rpC;
		rpR=vcel.rpR;
		rcL=vcel.rcL;
		rcC=vcel.rcC;
		rcR=vcel.rcR;
		rgL=vcel.rgL;
		rgC=vcel.rgC;
		rgR=vcel.rgR;
		drhogdt=vcel.drhogdt;

		rgLi=vcel.rgLi;
		rgCi=vcel.rgCi;
		rgRi=vcel.rgRi;
		rpLi=vcel.rpLi;
		rpCi=vcel.rpCi;
		rpRi=vcel.rpRi;
		rcLi=vcel.rcLi;
		rcCi=vcel.rcCi;
		rcRi=vcel.rcRi;

	    mipC=vcel.mipC;
	    micC=vcel.micC;
	    migC=vcel.migC;
	    mipR=vcel.mipR;
	    micR=vcel.micR;
	    migR=vcel.migR;


		perdaEstratL=vcel.perdaEstratL;
		perdaEstratG=vcel.perdaEstratG;

		estadoPig=vcel.estadoPig;
		estadoPigini=vcel.estadoPigini;
		dtPig = vcel.dtPig;
		alfPigE=vcel.alfPigE;
		alfPigER=vcel.alfPigER;
		alfPigD=vcel.alfPigD;
		betPigE=vcel.betPigE;
		betPigD=vcel.betPigD;
		alfPigEini=vcel.alfPigEini;
		alfPigDini=vcel.alfPigDini;
		betPigEini=vcel.betPigEini;
		betPigDini=vcel.betPigDini;
		velPig=vcel.velPig;
		velPigini=vcel.velPigini;
		razPig=vcel.razPig;
		razPigini=vcel.razPigini;
		RazAreaPig=vcel.RazAreaPig;//alteracao2
	    DelPig=vcel.DelPig;//alteracao2
	    VazaPig=vcel.VazaPig;//alteracao2
	    cdpig=vcel.cdpig;//alteracao2
	    indpig=vcel.indpig;//alteracao2

		VolLeveST=vcel.VolLeveST;
	    VolPesaST=vcel.VolPesaST;
	    VolAguaST=vcel.VolAguaST;

	    DmasschokeG=vcel.DmasschokeG;
	    DmasschokeL=vcel.DmasschokeL;
	    DmasschokeC=vcel.DmasschokeC;

	    chutearranjo=vcel.chutearranjo;

	    ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
	    presBuf=vcel.presBuf;
	    presLiniBuf=vcel.presLiniBuf;
	    presLBuf=vcel.presLBuf;
	    presRBuf=vcel.presRBuf;
	    presauxBuf=vcel.presauxBuf;
	    MCBuf=vcel.MCBuf;
	    MRBuf=vcel.MRBuf;
	    MliqiniRBuf=vcel.MliqiniRBuf;
	    MRiniBuf=vcel.MRiniBuf;
	    MLBuf=vcel.MLBuf;
	    MliqiniBuf=vcel.MliqiniBuf;
	    MliqiniLBuf=vcel.MliqiniLBuf;
	    //////////////////////////// 05-07-2018 ///////////////////////////////////////////////

	    corrigeMassaPres=0.;

	    fluxcalmed=vcel.fluxcalmed;

	    ////novas saidas
	    termoHidro=vcel.termoHidro;
	    termoFric=vcel.termoFric;
	    /////////////////////////////////////////////////////////////////////////////////////
		QLLini=vcel.QLLini;
		QLRini=vcel.QLRini;
		QLini=vcel.QLini;
		QGini=vcel.QGini;
		presauxRini=vcel.presauxRini;
		presauxLini=vcel.presauxLini;
		fontemassLLini=vcel.fontemassLLini;
		fontemassGLini=vcel.fontemassGLini;
		fontemassCLini=vcel.fontemassCLini;
		fontemassLRini=vcel.fontemassLRini;
		fontemassGRini=vcel.fontemassGRini;
		fontemassCRini=vcel.fontemassCRini;
		transmassLini=vcel.transmassLini;
		transmassRini=vcel.transmassRini;
		FonteMudaFaseini=vcel.FonteMudaFaseini;
		DTransDt1ini=vcel.DTransDt1ini;
		DTransDt0ini=vcel.DTransDt0ini;
		DTransDxRini=vcel.DTransDxRini;
		DTransDxLini=vcel.DTransDxLini;
		coefTransBetini=vcel.coefTransBetini;
		CoefDTRini=vcel.CoefDTRini;
		CoefDTLini=vcel.CoefDTLini;
		VTemperini=vcel.VTemperini;

		termRug=vcel.termRug;

		angEsq=vcel.angEsq;
		angDir=vcel.angDir;

		mudaDT=vcel.mudaDT;
		mudaDTL=vcel.mudaDTL;
		dpdt=vcel.dpdt;
		dpdtIni=vcel.dpdtIni;
	    m2d=vcel.m2d;
	    m2dL=vcel.m2dL;
	    d2pdt2=vcel.d2pdt2;

	    alfPigERini=vcel.alfPigERini;
	    indpigini=vcel.indpigini;
	    betIini=vcel.betIini;
	    betRIini=vcel.betRIini;
	    betLIini=vcel.betLIini;

	    estabCol=vcel.estabCol;

	    mudaArea=vcel.mudaArea;

	    nMol=vcel.nMol;
	    nMolIni=vcel.nMolIni;

	    dR=vcel.dR;

	    indGeom=vcel.indGeom;

	    dPdLHidro=vcel.dPdLHidro;
	    dPdLFric=vcel.dPdLFric;
	    dTdLCor=vcel.dTdLCor;

	    inddPdLHidro=vcel.inddPdLHidro;
	    inddPdLFric=vcel.inddPdLFric;
	    inddTdLCor=vcel.inddTdLCor;

	    reiniciaAlf=vcel.reiniciaAlf;
	    reiniciaBet=vcel.reiniciaBet;
	    reiniciaPig=vcel.reiniciaPig;

	    correrGlobHol=vcel.correrGlobHol;

	    miniTabAtraso=vcel.miniTabAtraso;

	    modoParafina=vcel.modoParafina;
	    parafinado=vcel.parafinado;
	    deltaPar=vcel.deltaPar;

	    fluxCal2D=vcel.fluxCal2D;

	    C2=vcel.C2;
	    C3=vcel.C3;
	    diamInterIni=vcel.diamInterIni;
	    porosoPar=vcel.porosoPar;
	    MW_wax=vcel.MW_wax;
	    rhoWaxLiq=vcel.rhoWaxLiq;
	    Sum_dCwaxdT=vcel.Sum_dCwaxdT;

		correlacaoMR2=vcel.correlacaoMR2;

		fonteCompartilhada=vcel.fonteCompartilhada;
		multiFComp=vcel.multiFComp;

		fluxcalAcopRedeP=vcel.fluxcalAcopRedeP;
		resAcopRedeP=vcel.resAcopRedeP;


	}
	return *this;
}

double Cel::Rey(double dia, double vel,
		double rho, double vis) {
	return dia * fabsl(vel) * rho / (vis * 1e-3);
}
double Cel::fric(double re, double eps, int tipo) {

/*
 double val;
 if (fabsl(re) > 1e-5) {
		if (re > 2400) {
			val = 6.9 / fabsl(re) + pow(eps / 3.7, 1.11);
			val = -1.8 * (logl(val) / logl(10.));
			val = pow(1 / val,2.);
		} else
			val = 4*16. / fabsl(re);
	} else
		val = 0.;
	return val/4.;*/
	double val;
	   if (fabsl(re) > 1e-5) {
			if (re > 2400) {
				if(tipo==-1)
				val = 6.9 / fabsl(re) + pow(eps / 3.7, 1.11);
				else val = 6.9 / fabsl(re) + termRug;
				//val = -1.8 * (logl(val) / logl(10.));
				val = -1.8 * (logl(val) / 2.30259);
				val*=val;
				val=1/val;
				//val = pow(1 / val,2.);
				for(int konta=0;konta<2;konta++){
				  //val=pow(-2.*logl( 2.51 / fabsl(re*sqrtl(val)) + eps/ 3.7)/logl(10.),2.);
				  val=-2.*logl( 2.51 / fabsl(re*sqrtl(val)) + eps/ 3.7)/2.30259;
				  val*=val;
				  val=1./val;
				}
				val/=4.;
			} else
				val = 16. / fabsl(re);
		} else
			val = 0.;

	   /*double Hl=1.-alf;
	   double Cl=QL/(QL+QG);
	   double y;
	   if(Hl>1e-5 && QL>1e-5)y=Cl/(Hl*Hl);
	   else y=1.;
	   double S;
	   if(y>=1. && y<=1.2)S=logl(2.2*y-1.2);
	   else S=logl(y)/(-0.0523+3.182*logl(y)-0.8725*pow(logl(y),2.)+0.01853*pow(logl(y),4.));
	   val*=exp(S);*/



	   return val;
}

void Cel::avancalf(int& reinicia, int ncel){

   double convec=0.;
   double term1Mass=0.;
   //mudaDT=0;

/*   double rlm=(1-betini)*rpC+betini*rcC;

   double fator1=1.-(rcC-rpC)*betini/rlm;
   fator1=1./fator1;
   double fator2=(rcC-rpC)/rcC;
   double bet0;
   if(Mliqini>0)bet0=betLini;
   else bet0=betini;
   double bet1;
   if(MliqiniR>0)bet1=betini;
   else bet1=betRini;

   double razdx0=dx/(dx+dxL);
   double razdx1=dxR/(dx+dxR);
   double tmed0=razdx0*temp+(1-razdx0)*tempL;
   double tmed1=razdx1*tempR+(1-razdx1)*temp;
   double rc0=fluicol.MasEspFlu(presaux,tmed0);
   double rc1=fluicol.MasEspFlu(presauxR,tmed1);

   if(posic<ncel){
	   convec=fator1*MliqiniR/dx-fator1*Mliqini/dx-fator2*fator1*QLR*rc1*bet1/dx+fator2*fator1*QL*rc0*bet0/dx;
	   double term1TransBet=-((QLR*betRI*rcRi/rcC)/dx-(QL*betI*rcCi/rcC)/dx)/duto.area+fontemassCR/(dx*rcC*duto.area)-bet*alf/dt;
	   term1Mass=(convec-fator1*fontemassLR/dx-fator1*fontemassCR/dx+fator1*fator2*fontemassCR/dx+
			      fator1*transmassR+fator1*coefTransBet*term1TransBet)/(rlm*duto.area);
   }
   else{
	   double dxncel=1.*dx;
	   convec=fator1*(-Mliqini)/dxncel-fator2*fator1*(-QL*rc0*bet0)/dxncel;
	   term1Mass=(convec-fator1*(fontemassLR/dx+(1.-fator2)*fontemassCR/dx))/(rlm*duto.area);
	   CoefDTR=0.;
	   CoefDTL=0.;
   }
   if(term1Mass>0 && alfini>1-(*vg1dSP).localtiny)term1Mass=0.;
   if(term1Mass<0 && alfini<(*vg1dSP).localtiny)term1Mass=0.;
   alf=(1-CoefDTL*fator1/(rlm*duto.area))*alfini/ (1+CoefDTR*fator1/(rlm*duto.area)-coefTransBet*bet*fator1/(rlm*duto.area))+
		  ((CoefDTR*fator1+CoefDTL*fator1)/(rlm*duto.area))/ (1+CoefDTR*fator1/(rlm*duto.area)-coefTransBet*bet*fator1/(rlm*duto.area))+
		  (term1Mass/ (1+CoefDTR*fator1/(rlm*duto.area)-coefTransBet*bet*fator1/(rlm*duto.area)))*dt;*/


    double rlm=rpC;
    double fator1=1.;
    dt1=dt;
    if(alf<=(*vg1dSP).CritCond&&transmassR<0.){
    	transmassR=0.;
    	FonteMudaFase=0.;
    }
    //if(alf<=1e-2&&transmassR<0. && (*acsrL).tipo==4){
    	//transmassR=0.;
    	//FonteMudaFase=0.;
    //}
	double MliqCR=QLR*betRI*rcRi;
	double MliqC=QL*betI*rcCi;
    double termoPtrans=0.;
    double termoDrholP=0.;
    double termoTtrans=0.;
    double termoDrholT=0.;
    double jmix=(fabsl(QL)+fabsl(QG))/duto.area;
    if(posic==44){
    	int para;
    	para=0;
    }

    if(TMModel==0 /*&& alfini>0.1 && alfini<0.9*/){
    	termoPtrans=-(DTransDtp/rpC)*m2d*(dpdt);
    	if(flui.flashCompleto!=2 || miniTabAtraso>0)
    	termoDrholP=(duto.area*(1.-alf)*(1-bet)/rpC)*((rpC-flui.MasEspLiq(presini*0.999, temp))/(0.001*presini))*
 			   m2d*(dpdt)*ativaDeri;
    	else{
    	    ProFlu flud=flui;
    	    flud.atualizaPropComp(presini*0.999, temp,flud.dCalculatedBeta,flud.oCalculatedLiqComposition,
    	    	  					  flud.oCalculatedVapComposition,0);
    	    termoDrholP=(duto.area*(1.-alf)*(1-bet)/rpC)*((rpC-flud.MasEspLiq(presini*0.999, temp))/(0.001*presini))*
    	     			   m2d*(dpdt)*ativaDeri;
    	}

    	double dTtemp=dTdt;
    	if(posic==ncel)dTtemp=dTdtL;
    	termoTtrans=-(DTransDtT/rpC)*(dTtemp)*mudaDT;
    	termoDrholT=(duto.area*(1.-alf)*(1-bet)/rpC)*((rpC-flui.MasEspLiq(presini, temp*0.999))/(0.001*temp))*
 			   (dTtemp)*ativaDeri*mudaDT;
    }
    if(posic<ncel){
//	   convec=(1/rpC)*MliqiniR/dx-(1/rpC)*Mliqini/dx+(1/rcC-1/rpC)*QLR*betRI*rcRi/dx-(1/rcC-1/rpC)*QL*betI*rcCi/dx;
    	convec=(1/rpC)*MliqiniR/dx-(1/rpC)*Mliqini/dx+(1/rcC-1/rpC)*MliqCR/dx-(1/rcC-1/rpC)*MliqC/dx;
//	   double term1TransBet=-((QLR*betRI*rcRi/rcC)/dx-(QL*betI*rcCi/rcC)/dx)/duto.area+fontemassCR/(dx*rcC*duto.area)-
//			                       bet*alf/dt;
    	/*double term1TransBet=-((MliqCR/rcC)/dx-(MliqC/rcC)/dx)/duto.area+fontemassCR/(dx*rcC*duto.area)-
    				                       bet*alf/dt;*/
       double term1TransBet=-((MliqCR/rcC)/dx-(MliqC/rcC)/dx)/duto.area+fontemassCR/(dx*rcC*duto.area);
	   term1Mass=(convec-fontemassLR/(dx*rpC)-fontemassCR/(dx*rcC)+(transmassR)/rpC+
			   coefTransBet*term1TransBet/rpC+termoPtrans+termoDrholP+termoTtrans+termoDrholT)/duto.area;
   }
   else{
	   //convec=-QL*rpCi*(1.-betI)/(dx*rpC)-QL*rcCi*betI/(dx*rcC);
	   convec=-(1/rpC)*Mliqini/dx-(1/rcC-1/rpC)*MliqC/dx;
	   term1Mass=(convec-fontemassLR/(dx*rpC)-fontemassCR/(dx*rcC)+termoPtrans+termoDrholP+
			                                                       termoTtrans+termoDrholT)/duto.area;
	   CoefDTR=0.;
	   CoefDTL=0.;
	   coefTransBet=0.;
   }
   if(term1Mass>0 && alfini>1-(*vg1dSP).localtiny)term1Mass=0.;
   if(term1Mass<0 && alfini<(*vg1dSP).localtiny)term1Mass=0.;
   /*alf=(1-CoefDTL/(rpC*duto.area))*alfini/ (1+CoefDTR/(rpC*duto.area)-coefTransBet*bet/(rpC*duto.area))+
		  ((CoefDTR+CoefDTL)/(rpC*duto.area))/ (1+CoefDTR/(rpC*duto.area)-coefTransBet*bet/(rpC*duto.area))+
		  (term1Mass/ (1+CoefDTR/(rpC*duto.area)-coefTransBet*bet/(rpC*duto.area)))*dt;*/

   alf=alfini+
   		  (term1Mass/ (1+CoefDTR/(rpC*duto.area)))*dt;

   double resto=0.;
   if(((alf<=(*vg1dSP).localtiny)&&(alf>=-(*vg1dSP).localtiny))){
	   alf=fabsl(0.);
   }
   else if(alf<-(*vg1dSP).localtiny){
      double dtaux;
      /*dtaux=-((1-CoefDTL*fator1/(rlm*duto.area))*alfini/ (1+CoefDTR*fator1/(rlm*duto.area)-
    		  coefTransBet*bet*fator1/(rlm*duto.area))+
    		((CoefDTR*fator1+CoefDTL*fator1)/(rlm*duto.area))/ (1+CoefDTR*fator1/(rlm*duto.area)-
    				coefTransBet*bet*fator1/(rlm*duto.area)))/(term1Mass/ (1+CoefDTR*fator1/(rlm*duto.area)-
    						coefTransBet*bet*fator1/(rlm*duto.area)));*/
      dtaux=-alfini/(term1Mass/(1+CoefDTR/(rpC*duto.area)));
      //if(dtaux<dt*(*vg1dSP).CritDTMin)dtaux=dt*(*vg1dSP).CritDTMin;
      if(reinicia==0 && jmix>(*vg1dSP).CritDTMin){
        dt1=dtaux;
        //if(correrGlobHol==1)reinicia=-1;
        //else
        	reiniciaAlf=-1;
        alf=fabsl(0.);
        //mudaDT=1;
      }
      else alfIter=alf=fabsl(0.);
   }
   else if((alf>=(1.-(*vg1dSP).localtiny)&&alf<=(1.+(*vg1dSP).localtiny))){
	   alf=1.;
   }
   else if(alf>(1.+(*vg1dSP).localtiny)){
      double dtaux;
      //dtaux=(1.-alfini)/term1Mass;
     /* dtaux=(1.-((1-CoefDTL*fator1/(rlm*duto.area))*alfini/ (1+CoefDTR*fator1/(rlm*duto.area)-
    		  coefTransBet*bet*fator1/(rlm*duto.area))+
    		((CoefDTR*fator1+CoefDTL*fator1)/(rlm*duto.area))/ (1+CoefDTR*fator1/(rlm*duto.area)-
    				coefTransBet*bet*fator1/(rlm*duto.area))))/
    		(term1Mass/ (1+CoefDTR*fator1/(rlm*duto.area)-coefTransBet*bet*fator1/(rlm*duto.area)));*/
      dtaux=(1.-alfini)/(term1Mass/(1+CoefDTR/(rpC*duto.area)));
      //if(dtaux<dt*(*vg1dSP).CritDTMin)dtaux=dt*(*vg1dSP).CritDTMin;
      if(reinicia==0 && jmix>(*vg1dSP).CritDTMin){
        dt1=dtaux;
        //if(correrGlobHol==0)reinicia=-1;
        //else
        	reiniciaAlf=-1;
        alf=1.;
        //mudaDT=1;
      }
      else alf=1.;
   }

   alfIter=alf;

}

void Cel::avancbet(int& reinicia, int ncel){

   double convec=0.;
   double term1Mass=0.;

   double fator1=rcC*duto.area;
   double bet0;
   double MliqCR=QLR*betRI*rcRi;
   double MliqC=QL*betI*rcCi;
   if((MC-Mliqini)*0+1*Mliqini>0)bet0=betLini;//duvidabeta
   else bet0=betini;
   double bet1;
   if((MR-MliqiniR)*0+1*MliqiniR>0)bet1=betini;//duvidabeta
   else bet1=betRini;

   double razdx0=dx/(dx+dxL);
   double razdx1=dxR/(dx+dxR);
   double tmed0=razdx0*temp+(1-razdx0)*tempL;
   double tmed1=razdx1*tempR+(1-razdx1)*temp;
   double rc0=fluicol.MasEspFlu(presaux,tmed0);
   double rc1=fluicol.MasEspFlu(presauxR,tmed1);

   double jmix=fabsl(QL)/duto.area;

   dt2=dt;

   //double rc0=0.5*(rcC+rcL);
   //double rc1=0.5*(rcC+rcR);

   if(posic<ncel){
	   if(alfini<=1.-(*vg1dSP).localtiny){
	     //convec=QLR*rcRi*betRI/dx-QL*rcCi*betI/dx;
		   convec=MliqCR/dx-MliqC/dx;
	     term1Mass=(-convec+fator1*betini*alf/dt-fator1*betini*alfini/dt+fontemassCR/dx)/(fator1-fator1*alfini);
	   }
	   else term1Mass=0.;
   }
   else{
	   double dxncel=1.*dx;
	   if(alfini<=1.-(*vg1dSP).localtiny){
	     //convec=(-QL*rcCi*betI/dx)/dxncel;
		   convec=(-MliqC/dx)/dxncel;
	     term1Mass=(-convec+fator1*betini*(alf-alfini)/dt+fontemassCR/dx)/(fator1*(1-alfini));
	   }
	   else term1Mass=0.;
   }

   if(term1Mass>0 && betini>1-(*vg1dSP).localtiny)term1Mass=0.;
   if(term1Mass<0 && betini<(*vg1dSP).localtiny)term1Mass=0.;
   //if(betini>(1-(*vg1dSP).localtiny) && betR>(1-(*vg1dSP).localtiny) && betL>(1-(*vg1dSP).localtiny))bet=1.;
   //else if(betini<((*vg1dSP).localtiny) && betR<((*vg1dSP).localtiny) && betL<((*vg1dSP).localtiny))bet=0.;
   //else
	   bet=betini+term1Mass*dt;

	  // if(alfini>1.-(*vg1dSP).localtiny){
		  // if(QL>=0)bet=betI;
		  // else if(QLR<=0)bet=betRI;
	  // }

	   if(alfini>=1-(*vg1dSP).localtiny)bet=0.;

   double resto=0.;
   if(((bet<=(*vg1dSP).localtiny)&&(bet>=-(*vg1dSP).localtiny))){
	   bet=fabsl(0.);
   }
   else if(bet<-(*vg1dSP).localtiny){
      double dtaux;

      dtaux=-betini/term1Mass;
      //if(dtaux<dt*(*vg1dSP).CritDTMin)dtaux=dt*(*vg1dSP).CritDTMin;
      if(reinicia==0 && jmix>(*vg1dSP).CritDTMin/*||alfini>0.*/){
        dt2=dtaux;
        //if(correrGlobHol==1)reinicia=-1;
        //else
        	reiniciaBet=-1;
        bet=fabsl(0.);
      }
      else bet=fabsl(0.);
   }
   else if((bet>=(1.-(*vg1dSP).localtiny)&&bet<=(1.+(*vg1dSP).localtiny))){
	   bet=1.;
   }
   else if(bet>(1.+(*vg1dSP).localtiny)){
      double dtaux;
      dtaux=(1.-betini)/term1Mass;
      //if(dtaux<dt*(*vg1dSP).CritDTMin)dtaux=dt*(*vg1dSP).CritDTMin;
      if(reinicia==0 && jmix>(*vg1dSP).CritDTMin/*||alfini<1.*/){
        dt2=dtaux;
        //if(correrGlobHol==0)reinicia=-1;
        //else
        	reiniciaBet=-1;
        bet=1.;
      }
      else bet=1.;
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
  double rhol = (1. - betmed) * flui.MasEspLiq(presfim, temp)
        + betmed * fluicol.MasEspFlu(presfim, temp);
  double ugsmed = (MC - Mliqini) / (area * rhog);
  double ulsmed = Mliqini / (area * rhol);

  double j = ugsmed + ulsmed;

  double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
  double viscmix = alfmed * flui.ViscGas(presfim, temp)
        + (1 - alfmed)
            * ((1. - betmed) * flui.ViscOleo(presfim, temp)
                + betmed * fluicol.VisFlu(presfim, temp));

  double re1;
  if (duto.revest == 0) re1 = Rey(duto.a, j, rhomix, viscmix);
  else {
      double dhid = 4 * area / si;
      re1 = Rey(dhid, j, rhomix, viscmix);
  }
  double f1 = fric(re1, duto.rug / dia);
  double gradfric = 0.5 * f1 * rhomix * (fabsl(j) * j) * si * dxmed / area;
  double gradhidro = 9.82 * sinl(duto.teta) * rhomix * dxmed;
  return (0*gradfric + gradhidro) / 98066.5;
}


void Cel::GeraLocal(double presfim, int masChkSup, int ncel, double razareativa,
         double presE, double tempE, double titE, double betE, int ciclo,
		 int modelo, int noextremo, int corrigeContSep, double areaChoke, int vexpi) {

 	 double deriPres=1.*mudaDT;
 	 if(modelo==0)deriPres=0.;
 	 double deriMas=vexpi;
 	 //if(modelo==0)deriMas=0.;
	 if (posic != 0 && posic != ncel) {

 		   if(posic>=95 && posic<=98){
 		   	int para;
 		   	para=0;
 		   }
 	    double AL=dutoL.area;
 	    double AC=AL;
 	    double AR=AL;
 	    double siL=duto.peri;
 		double siC=siL;
 		double siR=siL;
 		//double rhogL= (*fluiL).MasEspGas(presL,tempL);
 		double rhogL= rgL;
 		double rholL;
 		double rhogC=1.;
 		double rholC=1000.;
 		double rhogR;
 		double rholR=1000.;
 		double compres;
 		double dzdp;
 		double dpdrho=0.;
 		double drhoLdp=0.;
 		double multL=1.;
 		double multR=1.;
 		double corDRhol=1.-0*mudaDTL;
 		double corDRholC=1.-0*mudaDT;

 	    if(posic==44){
 	    	int para;
 	    	para=0;
 	    }

 	  if(ciclo==0){

 	    AC =duto.area;
 	    AR =dutoR.area;
         siL=dutoL.peri;
 	    siC=duto.peri;
 	    siR=dutoR.peri;
 	    rholL = rpL*(1-betL)+rcL*betL;
 	    //rhogC = flui.MasEspGas(pres,temp);
 	    rhogC = rgC;
 	    rholC = rpC*(1-bet)+rcC*bet;
 	    //rhogR = (*fluiR).MasEspGas(presR,tempR);
 	    rhogR = rgR;
 	    rholR = rpR*(1-betR)+rcR*betR;

 	    //double drhodt;
 	    multL=((1/rpL)*(1-betLI)*rpLi+(1/rcL)*betLI*rcLi)/((1-betLI)*rpLi+betLI*rcLi);
 	    multR=((1/rpL)*(1-betI)*rpCi+(1/rcL)*betI*rcCi)/((1-betI)*rpCi+betI*rcCi);
 	   //???????????????????????????????Não seria rcCi em vez de rcRi???????????????????
 	  }

 	  int master=1;
 	     if(acsrL!=0){
 			if(((*acsrL).tipo==5&&(*acsrL).chk.AreaGarg<(1e-3+razareativa)*AL)
 				||((*acsrL).tipo==8&&fabsl((*acsrL).bvol.freq)>1 ))master=0;
 	     }
 	     if(master==1){
 		    if(ciclo==0){
     	     compres = (*fluiL).Zdran(presL, tempL);
     	     dzdp = (*fluiL).DZDP(presL, tempL);
     	     dpdrho = rhogL* (1 / (presL * 98066.5) - (1 / compres) * dzdp);
     	     dpdrho = 1. / dpdrho;
     	     double rholdp;
     	     if((*fluiL).flashCompleto!=2 || miniTabAtraso>0)
     	     rholdp=(*fluiL).MasEspLiq(presL*0.999, tempL);
     	     else{
     	    	ProFlu flud=(*fluiL);
     	    	flud.atualizaPropComp(presL*0.999, tempL,flud.dCalculatedBeta,flud.oCalculatedLiqComposition,
     	    	  					  flud.oCalculatedVapComposition,0);
     	    	rholdp=flud.MasEspLiq(presL*0.999, tempL);
     	     }
     	     drhoLdp=(rpL-rholdp)/(0.001*presL);
     	  //drhodt=(*fluiL).drhodt(presL, tempL);

     	  if(alfLini<=(*vg1dSP).CritCond&&transmassL<0.)transmassL=0.;
     	  //if(alfLini<=1e-2&&transmassL<0. && (*acsrL).tipo==4)transmassL=0.;

     	  local[0][0] =-dxL*(1/rhogL-1/rpL)*DTransDxLp;
     	  local[0][1] = -((1./rhogL)*(1.-term1L)+multL*term1L)-dxL*(1/rhogL-1/rpL)*term1L*DTransDxL;
     	  //local[0][0] = -1./rhogL+term1L/rhogL-term1L/rholL;
 	      local[0][2] = dxL*AL*(((alfL/rhogL)/dpdrho)*98066.5+corDRhol*(deriPres*(1.-alfL)*(1-betL)*drhoLdp/rpL+
 	    		 deriPres*(1/AL)*(1/rhogL-1/rpL)*DTransDtpL)) / dt-
 	    		        dxL*(1/rhogL-1/rpL)*(DTransDxLp+DTransDxRp);
 	      local[0][3] = (1./rhogL)*(1-term1)+multR*term1-dxL*(1/rhogL-1/rpL)*term1*DTransDxR;
 	       //local[0][2] = 1./rhogL-term1/rhogL+term1/rholL;
 	      local[0][4] = -dxL*(1/rhogL-1/rpL)*DTransDxRp;
 	      local[0][5] = 0.;

 	      if(posic==1 && presE>0)fontemassGL+=corrigeMassaPres*dxL;

 	      if(TMModelL==1 || TMModelL==2 || TMModelL==3){
 	        TL[0] = dxL*AL*(((alfL/rhogL)/dpdrho)*presL*98066.5+corDRhol*(deriPres*(1.-alfL)*(1-betL)*drhoLdp*presL/rpL+
 	        		deriPres*(1/AL)*(1/rhogL-1/rpL)*DTransDtpL*presL)) / dt+
 	        		(1/rhogL-multR)*term2-
 	    		(1/rhogL-multL)*term2L+
 	    		(fontemassGL/rhogL+(fontemassLL/rpL+fontemassCL/rcL))+dxL*(1/rhogL-1/rpL)*transmassL;
 	       }
 	       else if(TMModelL==0){
 	         TL[0] = dxL*AL*(((alfL/rhogL)/dpdrho)*presL*98066.5+
 	        		 corDRhol*(deriPres*(1.-alfL)*(1-betL)*drhoLdp*presL/rpL+
 	        				deriPres*(1/AL)*(1/rhogL-1/rpL)*DTransDtpL*presL)) / dt+
 	        		 (1/rhogL-multR)*term2-
 	    	    		(1/rhogL-multL)*term2L+
 	    	    		(fontemassGL/rhogL+(fontemassLL/rpL+fontemassCL/rcL))+
 						dxL*(1/rhogL-1/rpL)*(DTransDt1*(1.-alfL)*(1.-betL)/dt+
 						DTransDt0*(1.-alfLini)*(1.-betLini)/dt+
 						term2*DTransDxR+term2L*DTransDxL+DtransDxLinear);
 	         }

 	       if(acsrL!=0){
 	         if((*acsrL).tipo==3){
 	    	   local[0][2]-=((*acsrL).ipr.deriG/rhogL+((*acsrL).ipr.deriP+(*acsrL).ipr.deriC)/rholL);
 	    	   TL[0] -=((*acsrL).ipr.deriG/rhogL+((*acsrL).ipr.deriP+(*acsrL).ipr.deriC)/rholL)*presL;
 	         }
 	         if((*acsrL).tipo==15){
 	    	   local[0][2]-=((*acsrL).radialPoro.DfluxIniG/rhogL+((*acsrL).radialPoro.DfluxIni+
 	    			  (*acsrL).radialPoro.DfluxIniA)/rholL);
 	    	   TL[0] -=((*acsrL).radialPoro.DfluxIniG/rhogL+((*acsrL).radialPoro.DfluxIni+
  	    			  (*acsrL).radialPoro.DfluxIniA)/rholL)*presL;
 	         }
 	         if((*acsrL).tipo==16){
 	    	   local[0][2]-=((*acsrL).poroso2D.dados.transfer.DfluxIniG/rhogL+((*acsrL).poroso2D.dados.transfer.DfluxIni+
 	    			  (*acsrL).poroso2D.dados.transfer.DfluxIniA)/rholL);
 	    	   TL[0] -=((*acsrL).poroso2D.dados.transfer.DfluxIniG/rhogL+((*acsrL).poroso2D.dados.transfer.DfluxIni+
  	    			  (*acsrL).poroso2D.dados.transfer.DfluxIniA)/rholL)*presL;
 	         }
 	       }
 		  }
 		  //else{
 	         double drhodt=(*fluiL).drhodt(presL, tempL);
 	         //double drhodt=(rgL-(*fluiL).MasEspGas(presL, tempL-0.01))/0.01;
 	         double drhoLdt=(rpL-(*fluiL).MasEspLiq(presL, tempL-0.01))/0.01;
 		     //double drhodt=drhogdt;
 	         TL[0] -=dxL*AL*((alfL/rhogL)*drhodt+(1-alfL)*(1-betL)*drhoLdt/rpL+
 	        		 (1/AL)*(1/rhogL-1/rpL)*DTransDtTL)*dTdtL*mudaDT;
 		  //}

       }
      else{
    	local[0][0] =0.;
        local[0][1] = 0.;
  	    local[0][2] = 0.;
  	    local[0][3] = 1.;
  	    local[0][4] = 0.;
  	    local[0][5] = 0.;

  	    TL[0] = 0.;
      }



      if((acsr.tipo!=5||acsr.chk.AreaGarg>=(1e-3+razareativa)*AC)&&
    	 (acsr.tipo!=8||fabsl(acsr.bvol.freq)<=1.)&&ciclo==0){

		double dxmed = 0.5 * (dx + dxR);
		//double Amed =(dx*AC+dxR*AR)/(dx + dxR);
		double Amed;
		if(AC<AR)Amed=AC;
		else Amed=AR;
		//double Amed =AC;
		//if((MR-MliqiniR)<0.)Amed =AR;


		double ugL=vexpi*((MC-Mliqini)/(rhogC))/AC;
		if(alf>(*vg1dSP).localtiny)ugL/=alf;
		else ugL=0.;
		double ugR=vexpi*((MR-MliqiniR)/(rhogR))/AR;
		if(alfR>(*vg1dSP).localtiny)ugR/=alfR;
		else ugR=0.;
		double ulL=vexpi*((Mliqini)/(rholC))/AC;
		if((1.-alf)>(*vg1dSP).localtiny)ulL/=(1.-alf);
		else ulL=0.;
		double ulR=vexpi*((MliqiniR)/(rholR))/AR;
		if((1.-alfR)>(*vg1dSP).localtiny)ulR/=(1.-alfR);
		else ulR=0.;
		local[1][1] = 0.;
		local[1][2] = -ugL*(1-term1)/dx-ulL*term1/dx;
		local[1][3] = -Amed*98066.5/dxmed;
		local[1][4] = (1-(1.-corDRholC*deriMas)*term1R) / dt+ugR*(1-term1R)/dx+ulR*term1R/dx;
		local[1][5] = Amed*98066.5/dxmed;

		TL[1] = (MR-(1.-corDRholC*deriMas)*MliqiniR) / dt+ (1.-corDRholC*deriMas)*term2R / dt+(ugR*term2R-ugL*term2)/dx-
				(ulR*term2R-ulL*term2)/dx;

		double j1;
		double j2;

		j1=(MliqiniR/rholC+(MR-MliqiniR)/rhogC)/AC;
		j2=(MliqiniR/rholR+(MR-MliqiniR)/rhogR)/AR;
		//j1=((term1R*MR+term2R)/rholC+(MR-(term1R*MR+term2R))/rhogC)/AC;
		//j2=((term1R*MR+term2R)/rholR+(MR-(term1R*MR+term2R))/rhogR)/AR;

		double coef1C=((1/rhogC)+term1R*(rhogC-rholC)/(rhogC*rholC))/AC;
		double coef2C=(term2R*(rhogC-rholC)/(rhogC*rholC))/AC;
		double coef1R=((1/rhogR)+term1R*(rhogR-rholR)/(rhogR*rholR))/AR;
		double coef2R=(term2R*(rhogR-rholR)/(rhogR*rholR))/AR;

		double rhomix1;
		double rhomix2;

		rhomix1=alf*rhogC+(1-alf)*rholC;
		rhomix2=alfR*rhogR+(1-alfR)*rholR;
		//rhomix1=alf*rhogC+(1-alf)*rholC;
		//rhomix2=alfR*rhogR+(1-alfR)*rholR;

		double viscmix1;
		double viscmix2;


		double CorrViscG=1.;
		double CorrViscL=1.;

		//if(arranjoR==3 && alf<1-1e-3)CorrViscL=1.+2.5*alf;
		//else if(arranjoR==2 && alf<1-1e-3){
			//CorrViscG=0.;
			//CorrViscL=1./(1.-alf);
		//}

		/*viscmix1=alf*flui.ViscGas(pres,temp)*CorrViscG+
				(1-alf)*((1-bet)*flui.ViscOleo(pres,temp)+bet*fluicol.VisFlu(pres,temp))*CorrViscL;*/
		viscmix1=alf*migC*CorrViscG+
						(1-alf)*((1-bet)*mipC+bet*micC)*CorrViscL;

		CorrViscG=1.;
		CorrViscL=1.;

		//if(arranjoR==3 && alfR<1-1e-3)CorrViscL=1.+2.5*alfR;
		//else if(arranjoR==2 && alfR<1-1e-3){
			//CorrViscG=0.;
			//CorrViscL=1./(1.-alfR);
		//}
		/*viscmix2=alfR*(*fluiR).ViscGas(presR,tempR)*CorrViscG+
				(1-alfR)*((1-betR)*(*fluiR).ViscOleo(presR,tempR)+betR*fluicol.VisFlu(presR,tempR))*CorrViscL;*/
		viscmix2=alfR*migR*CorrViscG+
						(1-alfR)*((1-betR)*mipR+betR*micR)*CorrViscL;
		//viscmix1=alf*flui.ViscGas(pres,temp)+
				//(1-alf)*((1-bet)*flui.ViscOleo(pres,temp)+bet*fluicol.VisFlu(temp));
		//viscmix2=alfR*flui.ViscGas(presR,tempR)+
				//(1-alfR)*((1-betR)*flui.ViscOleo(presR,tempR)+betR*fluicol.VisFlu(tempR));


		double re1;
		double re2;

		if(dutoL.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
		else{
			double dhid=4* duto.area/duto.peri;
			re1 = Rey(dhid, j1, rhomix1,viscmix1);
		}
		if(dutoR.revest==0)re2 = Rey(dutoR.a, j2, rhomix2,viscmix2);
		else{
			double dhid=4* dutoR.area/dutoR.peri;
			re2 = Rey(dhid, j2, rhomix2,viscmix2);
		}
		double f1 = fric(re1, duto.rug / duto.a);
		double f2 = fric(re2, dutoR.rug / dutoR.a,-1);
		if(duto.teta<0.1){
			f1*=1.;
			f2*=1.;
		}
		double razdx = dx / (dxR + dx);
		double coefTensC=dPdLFric*0.5 * f1*rhomix1*( fabsl(j1)) * siC * razdx;
		double dpfric=coefTensC*coef2C;
		double coefTensR=dPdLFric*0.5 * f2*rhomix2*( fabsl(j2)) * siR * (1.-razdx);
		dpfric+=coefTensR*coef2R;
		double dphidro=dPdLHidro*9.82 * sinl(duto.teta) *AC*rhomix1* razdx;
		termoHidro=dphidro/AC;
		termoHidro+=9.82 * sinl(dutoR.teta) *rhomix2* (1.-razdx);
		termoFric=coefTensC*j1/AC+coefTensR*j2/AR;
		dphidro+=9.82 * sinl(dutoR.teta) *AR*rhomix2* (1.-razdx);
		double estrat1=perdaEstratL*term1R*fabsl(MliqiniR)+perdaEstratG*(1-term1R)*fabsl(MR-MliqiniR);
		double estrat2=(perdaEstratL*fabsl(MliqiniR)-perdaEstratG*fabsl(MR-MliqiniR))*term2R;

		estrat1=0.;
		estrat2=0.;

		dpB=0.;
		potB=0.;
		potBT=0.;
		double coefDpB=0.;
		double perdArea=0.;
		if((duto.area!=dutoR.area && re1>2400 && re2>2400) && (mudaArea==1 && (fabsl(j1)>=0.1 && fabsl(j2)>=0.1))){
			double areaMenor;
			double areaMaior;
			double bernou;
			double perdLoc;
			bernou=0.5*(MR/(duto.area*duto.area*rhomix1))*(1.-pow(duto.area/dutoR.area,2.));
			if(duto.area>dutoR.area){
				areaMenor=dutoR.area;
				areaMaior=duto.area;;
				if(MR>0.){
					perdLoc=0.42*0.5*(MR/(duto.area*duto.area*rhomix1))*(1.-pow(duto.area/dutoR.area,2.));
				}
				else{
					double expanse=(1.-pow(areaMenor/areaMaior,2.));
					perdLoc=0.5*(MR/(areaMenor*areaMenor*rhomix1))*expanse*expanse;
				}
			}
			else{
				areaMenor=duto.area;
				areaMaior=dutoR.area;
				if(MR<0.){
					perdLoc=0.42*0.5*(MR/(duto.area*duto.area*rhomix1))*(1.-pow(duto.area/dutoR.area,2.));
				}
				else{
					double expanse=(1.-pow(areaMenor/areaMaior,2.));
					perdLoc=-0.5*(MR/(areaMenor*areaMenor*rhomix1))*expanse*expanse;
				}
			}
			perdArea=-(bernou+perdLoc)*Amed/dxmed;

		}
		if(acsr.tipo==4&&acsr.bcs.freqnova>1. && j1>=0.){
		     double vazmix=j1*AC;
		     double alf0=alf;
		     vazmix*=(86400/0.1589876);
		     //BomCentSub bombavisc=
		     acsr.bcs.NovaVis(viscmix1,rhomix1,vazmix);
		     //dpB=0.3048*bombavisc.Fhead(vazmix)*rhomix1*9.82;
		     //potB=(1.-bombavisc.Fefic(vazmix)/100.+100.*(1.-bombavisc.eficM/100.)/bombavisc.eficM)*
		    	  //bombavisc.Fpower(vazmix)*745.7;
		     dpB=0.3048*acsr.bcs.Hvis*rhomix1*9.82;
		     potB=acsr.bcs.Pvis*745.7;
		     if(acsr.bcs.Evis>0.)potB*=(100./acsr.bcs.Evis);
		     else potB=0.;
		     if(acsr.bcs.eficM>0.)
		     potBT=(1.+100.*(1.-acsr.bcs.eficM/100.)/acsr.bcs.eficM)*potB;
		     else potBT=0.;
		     double Cdpb=AC*(86400/0.1589876);
		     double djdm=(term1R/rholC+(1-term1R)/rhogC)/AC;
		     //double dpdvaz=0.3048*bombavisc.Fhead(vazmix*1.001)*rhomix1*9.82;
		     acsr.bcs.NovaVis(viscmix1,rhomix1,vazmix*1.001);
		     double dpdvaz=(0.3048*acsr.bcs.Hvis*rhomix1*9.82-dpB)/(0.001*vazmix);
		     coefDpB=Cdpb*djdm*dpdvaz;
		}
		else if(acsr.tipo==7){
			dpB=acsr.delp*98066.5;
			double qgMon=(MR-MliqiniR)/rhogC;
			double qlmon=MliqiniR/rholC;
			double npoli=1.;
			if(acsr.tipoCompGas==0){
				  npoli=flui.ConstAdG(pres, temp);
				  if((npoli-1.05)<1e-2)npoli=1.05;
			}
			else if(acsr.tipoCompGas==1)npoli=acsr.fatPoli;
			double Wcomp;
			double Wbomb;
			Wbomb=(100./acsr.eficLiq)*dpB*qlmon;
			if(acsr.tipoCompGas!=2)
				 Wcomp=-(presauxR*98066.5*qgMon/(1.-npoli))*
						  (pow(1+acsr.delp/presauxR,(npoli-1.)/npoli)-1.);
			else Wcomp=presauxR*98066.5*qgMon*logl(1+acsr.delp/presauxR);
			Wcomp*=(100./acsr.eficGas);
			potB =Wcomp+Wbomb;
			potBT =Wcomp+Wbomb;
		}

		double delpChoke=0.;

		double sinalPig=1.;
		if(velPig<0.)sinalPig=1.;

		local[1][4] +=(coefTensC*coef1C+coefTensR*coef1R+estrat1);
		//local[1][4] -=coefDpB*((dx*AC+dxR*AR)/(dx + dxR))/dx;
		local[1][4] -=coefDpB*Amed/dxmed;
		local[1][4]+=perdArea;
		//TL[1]-=(dpfric+dphidro-(dpB+delpChoke-
				//coefDpB*MR-(sinalPig*DelPig*velPig*98066.5))*((dx*AC+dxR*AR)/(dx + dxR))/dx+estrat2);
		TL[1]-=(dpfric+dphidro-(dpB+delpChoke-
						coefDpB*MR-(sinalPig*DelPig*velPig*98066.5))*Amed/dxmed+estrat2);


		//cinematico=0.*(fabsl(MR-MRini) / dt)/fabsl((Amed*98066.5/dxmed)*(presR-pres));
		cinematico=0;
      }
      else{
    	  if(ciclo==0){
    		//deriPres*=m2d;
    	    compres = flui.Zdran(pres, temp);
    	    dzdp = flui.DZDP(pres, temp);
    	    dpdrho = rhogC* (1 / (pres * 98066.5) - (1 / compres) * dzdp);
            dpdrho = 1. / dpdrho;
            multL=((1/rpC)*(1-betI)*rpCi+(1/rcC)*betI*rcCi)/((1-betI)*rpCi+betI*rcCi);
            double drhoLdpC=(rpC-flui.MasEspLiq(pres*0.999, temp))/(0.001*pres);
            if(alf<1e-3 && drhoLdpC<0.1)drhoLdpC=0.1;
            local[1][1] = 0.;
   	        local[1][2] = -((1./rhogC)*(1.-term1)+multL*term1);
   	        local[1][3] =  dx*AC*(((alf/rhogC)/dpdrho)*98066.5+corDRholC*(deriPres*(1.-alf)*(1-bet)*drhoLdpC/rpC+
   	        		deriPres*(1/AC)*(1/rhogC-1/rpC)*DTransDtp)) / dt;
   	        local[1][4] = 0.;
   	        local[1][5] = 0.;

   	        TL[1] = dx*AC*(((alf/rhogC)/dpdrho)*pres*98066.5+corDRholC*(deriPres*(1.-alf)*(1-bet)*drhoLdp*pres/rpC+
   	        		deriPres*(1/AC)*(1/rhogC-1/rpC)*DTransDtp*pres)) / dt-(1/rhogC-multL)*term2+
   	    		(fontemassGR/rhogC+(fontemassLR/rpC+fontemassCR/rcC))+dx*(1/rhogC-1/rpC)*transmassR;
    	  }
	    double drhodt=flui.drhodt(pres, temp);
	    double drhoLdt=(rpC-flui.MasEspLiq(pres, temp-0.01))/0.01;
	    TL[1] -=dx*AC*((alf/rhogC)*drhodt+(1-alf)*(1-bet)*drhoLdt/rpC+
        		 (1/AC)*(1/rhogC-1/rpC)*DTransDtT)*dTdt*mudaDT;

   	    if(acsr.tipo==8 && ciclo==0){
   	    	double drhodp=1/dpdrho;
   	    	double drhodp1;
   	    	compres = flui.Zdran(presR, tempR);
   	    	dzdp = flui.DZDP(presR, tempR);
   	    	drhodp1 = rhogR* (1 / (pres * 98066.5) - (1 / compres) * dzdp);
   	    	double correcBVol=-(-(1/rhogR)*drhodp*pres*98066.5+rhogC*drhodp1*presR*98066.5/(rhogR*rhogR));
   	    	local[1][3] +=(acsr.bvol.MultGas/rhogC+(acsr.bvol.MLiqP/rpC+acsr.bvol.MLiqC/rcC))*(1/rhogR)*drhodp*98066.5;
   	    	local[1][5] -= (acsr.bvol.MultGas/rhogC+(acsr.bvol.MLiqP/rpC+acsr.bvol.MLiqC/rcC))*rhogC*drhodp1*98066.5/(rhogR*rhogR);
   	    	TL[1] += (acsr.bvol.MultGas/rhogC+(acsr.bvol.MLiqP/rpC+acsr.bvol.MLiqC/rcC))*correcBVol;
   	    }


      }

   	}
	else if (posic == ncel) {
 		double corDRhol=1.-0*mudaDTL;
 		double corDRholC=1.-0*mudaDT;
       if(masChkSup==0){
    	   double AL=dutoL.area;
    	   double rhogL= (*fluiL).MasEspGas(presL,tempL);
    	   if(ciclo==0){
		      double rholL = rpL*(1-betL)+rcL*betL;
	          double compres = (*fluiL).Zdran(presL, tempL);
		      double dzdp = (*fluiL).DZDP(presL, tempL);
		      double dpdrho = rhogL* (1 / (presL * 98066.5) - (1 / compres) * dzdp);
		      dpdrho = 1. / dpdrho;
		      double drhoLdp;
		      if((*fluiL).flashCompleto!=2 || miniTabAtraso>0)
		    	  drhoLdp=(rpL-(*fluiL).MasEspLiq(presL*0.999, tempL))/(0.001*presL);
		      else{
		    	    ProFlu flud=(*fluiL);
		    	    flud.atualizaPropComp(presL*0.999, tempL,flud.dCalculatedBeta,flud.oCalculatedLiqComposition,
		    	    	  					  flud.oCalculatedVapComposition,0);
		    	    drhoLdp=(rpL-flud.MasEspLiq(presL*0.999, tempL))/(0.001*presL);
		      }
		      double multL=((1/rpL)*(1-betLI)*rpLi+(1/rcL)*betLI*rcLi)/((1-betLI)*rpLi+betLI*rcLi);
		      double multR=((1/rpL)*(1-betI)*rpCi+(1/rcL)*betI*rcCi)/((1-betI)*rpCi+betI*rcCi);
		      //???????????????????????????????Não seria rcCi em vez de rcRi???????????????????

		      double coluna=1.;
		      if(duto.teta<0.8*M_PI/2.)coluna=1.;

		      local[0][1] = -((1./rhogL)*(1.-term1L)+multL*term1L);
	          local[0][2] = coluna*dxL*AL*(((alfL/rhogL)/dpdrho)*98066.5+corDRhol*(deriPres*(1.-alfL)*(1-betL)*drhoLdp/rpL+
	        		  deriPres*(1/AL)*(1/rhogL-1/rpL)*DTransDtpL)) / dt;
	          local[0][3] = (1./rhogL)*(1-term1)+multR*term1;
	          local[0][4] = 0.;
	          local[0][5] = 0.;

	          TL[0] = coluna*dxL*AL*(((alfL/rhogL)/dpdrho)*presL*98066.5+corDRhol*(deriPres*(1.-alfL)*(1-betL)*drhoLdp*presL/rpL+
	        		  deriPres*(1/AL)*(1/rhogL-1/rpL)*DTransDtpL*presL)) / dt+
	        		  (1/rhogL-multR)*term2-(1/rhogL-multL)*term2L+
	        		(fontemassGL/rhogL+(fontemassLL/rpL+fontemassCL/rcL)/rholL)+dxL*(1/rhogL-1/rpL)*transmassL;

    	   }
		   double drhodt=(*fluiL).drhodt(presL, tempL);
	       double drhoLdt=(rpL-(*fluiL).MasEspLiq(presL, tempL-0.01))/0.01;

	       //int mult=1.;
	       //if(chaverede==1 && noextremo==0 ) mult=0.;
	       TL[0] -=dxL*AL*((alfL/rhogL)*drhodt+(1-alfL)*(1-betL)*drhoLdt/rpL+
	    		   (1/AL)*(1/rhogL-1/rpL)*DTransDtTL)*dTdtL*mudaDT;
		   //TL[0] -=dxL*AL*(alfL/rhogL)*drhodt*dTdtL;

          if(ciclo==0){
		      local[1][1] = 0.;
		      local[1][2] = 0.;
		      local[1][3] = 1.;
		      local[1][4] = 0.;
		      local[1][5] = 0.;
		      ////////////////////////////////////////////////////////////////////////////
		      if(((*vg1dSP).chaveredeT==0 || noextremo==1) && corrigeContSep==1){
		    	double dxmed = 0.5 * dx;
				double j1;

		 	    double AC =duto.area;
		 	    double siC=duto.peri;
		 	    double rhogC = rgC;
		 	    double rholC = rpC*(1-bet)+rcC*bet;

				j1=(MliqiniR/rholC+(MR-MliqiniR)/rhogC)/AC;

				double coef1C=((1/rhogC)+term1*(rhogC-rholC)/(rhogC*rholC))/AC;
				double coef2C=(term2*(rhogC-rholC)/(rhogC*rholC))/AC;

				double rhomix1;

				rhomix1=alf*rhogC+(1-alf)*rholC;

				double viscmix1;

				double CorrViscG=1.;
				double CorrViscL=1.;

				/*viscmix1=alf*flui.ViscGas(pres,temp)*CorrViscG+
						(1-alf)*((1-bet)*flui.ViscOleo(pres,temp)+bet*fluicol.VisFlu(pres,temp))*CorrViscL;*/
				viscmix1=alf*migC*CorrViscG+
										(1-alf)*((1-bet)*mipC+bet*micC)*CorrViscL;

				CorrViscG=1.;
				CorrViscL=1.;


				double re1;

				if(duto.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
				else{
					double dhid=4* duto.area/duto.peri;
					re1 = Rey(dhid, j1, rhomix1,viscmix1);
				}
				double f1 = fric(re1, duto.rug / duto.a);
				if(duto.teta<0.1){
					f1*=1.;
				}

				double coefTensC=0.5 * f1*rhomix1*( fabsl(j1)) * siC/AC;
				double dpfric=coefTensC*coef2C;
				double dphidro=9.82 * sinl(duto.teta)*rhomix1;
				termoHidro=dphidro;
				termoFric=coefTensC*j1;

				local[1][2] -=(coefTensC*coef1C)*dxmed/ 98066.5;
				TL[1]=presfim+(dpfric+dphidro)*dxmed/ 98066.5;
		      }
		      else TL[1] = presfim;


		      ///////////////////////////////////////////////////////////////////////////

		      //double corrigePJ=0.;
		      //if(chaveredeT==0 || noextremo==1)corrigePJ=delpJus(presfim);

		     // TL[1] = presfim+corrigePJ;

          }

		  ////////////////////////////////////////////////////////////////////////////////////////////////////


	    /* double alffim=alfL;
	     double betfim=betL;
	     if(MC-Mliqini<0){
	    	 alffim=alf;
	    	 betfim=bet;
	     }
	     double pmed=0.5*(presL+presfim);
	     double  rhogC = flui.MasEspGas(pmed,temp);
		 double  rhogR = flui.MasEspGas(pmed,temp);
		 double  rholC = flui.MasEspLiq(pmed,temp)*(1-betfim)+fluicol.MasEspFlu(pmed,temp)*betfim;
		 double AC=duto.area;
		 double siC=duto.peri;


		 double j1=(Mliqini/rholC+(MC-Mliqini)/rhogC)/AC;
		 double coef1C=((1/rhogC)+term1*(rhogC-rholC)/(rhogC*rholC))/AC;
		 double coef2C=(term2*(rhogC-rholC)/(rhogC*rholC))/AC;
		 double rhomix1=alffim*rhogC+(1-alffim)*rholC;
		 double viscmix1=alffim*flui.ViscGas(pmed,temp)+
				(1-alffim)*((1-betfim)*flui.ViscOleo(pmed,temp)+betfim*fluicol.VisFlu(pmed,temp));
		 double re1;
		  if(duto.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
		  else{
			  double dhid=4* duto.area/duto.peri;
			  re1 = Rey(dhid, j1, rhomix1,viscmix1);
		  }
		  double f1 = fric(re1, duto.rug / duto.a);
		  double coefTensC=0.5 * f1*rhomix1*( fabsl(j1)) * siC;
		  double dpfric=coefTensC*coef2C;
		  double dphidro=9.82 * sinl(duto.teta) *AC*rhomix1;

		  local[0][0] = 0.;
		  local[0][1] = -AC*98066.5/(dx);
		  local[0][2] =coefTensC*coef1C+(1-term1) / dt;
		  local[0][3] = 0.;
		  local[0][4] = 0;

		  TL[0] = (MC-Mliqini) / dt+ term2 / dt;

		  TL[0] +=(-AC*98066.5*presfim/(dx)-(dpfric+dphidro));*/

        }
       else{
    	   double AL=dutoL.area;
    	   //double rhogL= (*fluiL).MasEspGas(presL,tempL);
    	   double rhogL= rgL;
    	   double rholL;
    	   double compres;
    	   double dzdp;
    	   double dpdrho;
    	   double coluna=1.;
    	   double multL;
    	   double multR;
    	   if(ciclo==0){

  		     rholL = rpL*(1-betL)+rcL*betL;
  	         compres = (*fluiL).Zdran(presL, tempL);
  		     dzdp = (*fluiL).DZDP(presL, tempL);
  		     dpdrho = rhogL* (1 / (presL * 98066.5) - (1 / compres) * dzdp);
  		     dpdrho = 1. / dpdrho;
		     double drhoLdp;
		     if((*fluiL).flashCompleto!=2 || miniTabAtraso>0)
		    	 drhoLdp=(rpL-(*fluiL).MasEspLiq(presL*0.999, tempL))/(0.001*presL);
		     else{
		    	 ProFlu flud=(*fluiL);
		    	 flud.atualizaPropComp(presL*0.999, tempL,flud.dCalculatedBeta,flud.oCalculatedLiqComposition,
		    	    	  					  flud.oCalculatedVapComposition,0);
		    	 drhoLdp=(rpL-flud.MasEspLiq(presL*0.999, tempL))/(0.001*presL);
		     }
		     multL=((1/rpL)*(1-betLI)*rpLi+(1/rcL)*betLI*rcLi)/((1-betLI)*rpLi+betLI*rcLi);
		     multR=((1/rpL)*(1-betI)*rpCi+(1/rcL)*betI*rcRi)/((1-betI)*rpCi+betI*rcCi);

  		     if(duto.teta<0.8*M_PI/2.)coluna=1.;

  		     local[0][0]=0.;
  		     local[0][1] = -((1./rhogL)*(1.-term1L)+multL*term1L);
  	         local[0][2] = coluna*dxL*AL*(((alfL/rhogL)/dpdrho)*98066.5+corDRhol*(deriPres*(1.-alfL)*(1-betL)*drhoLdp/rpL+
  	        		deriPres*(1/AL)*(1/rhogL-1/rpL)*DTransDtpL)) / dt;
  	         local[0][3] = (1./rhogL)*(1-term1)+multR*term1;
  	         local[0][4] = 0.;
  	         local[0][5] = 0.;

  	         TL[0] = coluna*dxL*AL*(((alfL/rhogL)/dpdrho)*presL*98066.5+corDRhol*(deriPres*(1.-alfL)*(1-betL)*drhoLdp*presL/rpL+
  	        		deriPres*(1/AL)*(1/rhogL-1/rpL)*DTransDtpL*presL)) / dt+(1/rhogL-multR)*term2-(1/rhogL-multL)*term2L+
	        		(fontemassGL/rhogL+(fontemassLL/rpL+fontemassCL/rcL))+dxL*(1/rhogL-1/rpL)*transmassL;
    	   }
		   double drhodt=(*fluiL).drhodt(presL, tempL);
	       double drhoLdt=(rpL-(*fluiL).MasEspLiq(presL, tempL-0.01))/0.01;

	       TL[0] -=dxL*AL*((alfL/rhogL)*drhodt+(1-alfL)*(1-betL)*drhoLdt/rpL+
	    		   (1/AL)*(1/rhogL-1/rpL)*DTransDtTL)*dTdtL*mudaDT;
		   //TL[0] -=dxL*AL*(alfL/rhogL)*drhodt*dTdtL;

		   double dxncel=1.*dx;
		   double AC =dutoR.area;
		   //double rhog = flui.MasEspGas(pres,temp);
		   double rhog = rgC;
		   double rhol;
		   if(ciclo==0){
			   //deriPres*=m2d;
  	         AC =dutoR.area;
  		     rhog = flui.MasEspGas(pres,temp);
  		     rhol = rpC*(1-bet)+rcC*bet;
  	         compres = flui.Zdran(pres, temp);
  		     dzdp = flui.DZDP(pres, temp);
  		     dpdrho = rhog* (1 / (pres * 98066.5) - (1 / compres) * dzdp);
  		     dpdrho = 1. / dpdrho;
  		     double drhoLdp;
		     if(flui.flashCompleto!=2 || miniTabAtraso>0)
		    	 drhoLdp=(rpC-flui.MasEspLiq(pres*0.999, temp))/(0.001*pres);
		     else{
		    	 ProFlu flud=flui;
		    	 flud.atualizaPropComp(pres*0.999, temp,flud.dCalculatedBeta,flud.oCalculatedLiqComposition,
		    	    	  					  flud.oCalculatedVapComposition,0);
		    	 drhoLdp=(rpC-flud.MasEspLiq(pres*0.999, temp))/(0.001*pres);
		     }
  		     coluna=1.;
  		     if(duto.teta<0.8*M_PI/2.)coluna=1.;

  		     multL=((1/rpL)*(1-betI)*rpCi+(1/rcL)*betI*rcRi)/((1-betI)*rpCi+betI*rcCi);

  		     if(alf<1e-3 && drhoLdp<0.1)drhoLdp=0.1;

  		     local[1][0] = 0.;
  		     local[1][1] = 0.;
  		     local[1][2] = -((1./rhog)*(1.-term1)+multL*term1);
  		     local[1][3] = coluna*dxncel*AC*(((alf/rhog)/dpdrho)*98066.5+corDRholC*(deriPres*(1.-alf)*(1-bet)*drhoLdp/rpC+
  		    		deriPres*(1/AC)*(1/rhog-1/rpC)*DTransDtp)) / dt-
  				(DmasschokeG/rhog+DmasschokeL/rpC+DmasschokeC/rcC);
  		     local[1][4] = 0.;
  		     local[1][5] = 0.;

  		     TL[1] = coluna*dxncel*AC*(((alf/rhog)/dpdrho)*pres*98066.5+corDRholC*(deriPres*(1.-alf)*(1-bet)*drhoLdp*pres/rpC+
  		    		deriPres*(1/AC)*(1/rhog-1/rpC)*DTransDtp*pres)) / dt
  				  -(1/rhog-multL)*term2+
	        	  ((fontemassGR-DmasschokeG*pres)/rhog+(fontemassLR-DmasschokeL*pres)/rpC+
	        			  (fontemassCR-DmasschokeC*pres)/rcC);
		   }
 		   drhodt=flui.drhodt(presini, tempini);
		   if(flui.flashCompleto!=2 || miniTabAtraso>0)
			   drhoLdt=(rpC-flui.MasEspLiq(pres, temp-0.01))/0.01;
		   else{
		    	 ProFlu flud=flui;
		    	 flud.atualizaPropComp(pres, temp-0.01,flud.dCalculatedBeta,flud.oCalculatedLiqComposition,
		    	    	  					  flud.oCalculatedVapComposition,0);
		    	 drhoLdt=(rpC-flud.MasEspLiq(pres, temp-0.01))/0.01;
		   }

	       //TL[0] -=dxL*AL*((alfL/rhogL)*drhodt+(1-alfL)*(1-betL)*drhoLdt/rpL)*dTdtL;
	       //int mult=1.;
	       //if(areaChoke>1e-15 && chaverede==1 && noextremo==0 ) mult=0.;
	       TL[1] -=dxncel*AC*((alf/rhog)*drhodt+(1-alf)*(1-bet)*drhoLdt/rpC+
	    		   (1/AC)*(1/rhog-1/rpC)*DTransDtT)*dTdtL*mudaDT;
 		   //TL[1] -=dxncel*AC*(alf/rhog)*drhodt*dTdtL;



       }

	}
	else if (posic == 0 && ciclo==0) {

		double corDRholC=1.-0*mudaDT;

		if(presE<0){//alteracao4

		  local[0][1] = 0.;
		  local[0][2] = 0.;
		  local[0][3] =1.;
		  local[0][4] = 0.;
		  local[0][5] = 0;

		  TL[0] =0.;
		}
		else {
			  double AC =duto.area;
			  double AR =dutoR.area;
			  double Amed =0.5*(AC+AR);
			  double siC=duto.peri;
			  double rhogC;
			  double rhogR;
			  double rholC;
			  if(flui.flashCompleto!=2 || miniTabAtraso>0){
				  rhogC = flui.MasEspGas(0.5*(presE+pres),tempE);
				  rhogR = flui.MasEspGas(pres,temp);
				  rholC = flui.MasEspLiq(0.5*(presE+pres),tempE)*(1-betE)+
						  fluicol.MasEspFlu(0.5*(presE+pres),tempE)*betE;
			  }
			  else{
			    	 ProFlu flud=flui;
			    	 flud.atualizaPropComp(0.5*(presE+pres),tempE,flud.dCalculatedBeta,flud.oCalculatedLiqComposition,
			    	    	  					  flud.oCalculatedVapComposition,0);
			    	 rhogC = flud.MasEspGas(0.5*(presE+pres),tempE);
			    	 rholC = flud.MasEspLiq(0.5*(presE+pres),tempE)*(1-betE)+
			    			 fluicol.MasEspFlu(0.5*(presE+pres),tempE)*betE;
			  }

			  double j1=(Mliqini/rholC+(MC-Mliqini)/rhogC)/AC;
			  double coef1C=((1/rhogC)+term1*(rhogC-rholC)/(rhogC*rholC))/AC;
			  double coef2C=(term2*(rhogC-rholC)/(rhogC*rholC))/AC;
			  double rhomix1=titE*rhogC+(1-titE)*rholC;
			  double viscmix1=titE*flui.ViscGas(0.5*(presE+pres),tempE)+
					(1-titE)*((1-betE)*flui.ViscOleo(0.5*(presE+pres),tempE)+
							betE*fluicol.VisFlu(0.5*(presE+pres),tempE));
			  double re1;
			  if(duto.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
			  else{
				  double dhid=4* duto.area/duto.peri;
				  re1 = Rey(dhid, j1, rhomix1,viscmix1);
			  }
			  double f1 = fric(re1, duto.rug / duto.a);
			  double coefTensC=dPdLFric*0.5 * f1*rhomix1*( fabsl(j1)) * siC;
			  double dpfric=coefTensC*coef2C;
			  double dphidro=dPdLHidro*9.82 * sinl(duto.teta) *AC*rhomix1;
			  termoHidro=dphidro/AC;
              termoFric=coefTensC*j1/AC;

			  double ugl=(MC-Mliqini)/rhogC;
			  double ugr=(MR-MliqiniR)/rhogR;
			  double dxm=dx;
			  double coefacelera1L=(-(1-term1)*ugl+0.25*(ugl+ugr)*(1-term1))/dxm;
			  double coefacelar2L=(term2*ugl-0.25*(ugl+ugr)*term2)/dxm;
			  double coefacelera1R=(0.25*(ugl+ugr)*(1-term1R))/dxm;
			  double coefacelar2R=(-0.25*(ugl+ugr)*term2R)/dxm;

			  local[0][1] = 0.;
			  local[0][2] = 0.;
			  local[0][3] =coefTensC*coef1C+(1.-(1.-corDRholC*deriMas))*term1 / dt;
			  local[0][4] = Amed*98066.5/(dx);
			  local[0][5] = 0;

			  TL[0] = (MC-(1.-corDRholC*deriMas)*Mliqini) / dt+ (1.-corDRholC*deriMas)*term2 / dt;

			  TL[0] +=(Amed*98066.5*presE/(dx)-(dpfric+dphidro));
		}

		  double AC =duto.area;
		  double AR =dutoR.area;
		  double siC=duto.peri;
		  double siR=dutoR.peri;
		  double rhogC = flui.MasEspGas(pres,temp);
		  double rholC = rpC*(1-bet)+rcC*bet;
		  double rhogR = (*fluiR).MasEspGas(presR,tempR);
		  double rholR = rpR*(1-betR)+rcR*betR;

		  double dxmed = 0.5 * (dx + dxR);
		  double Amed =0.5*(AC+AR);

		  local[1][1] = 0.;
		  local[1][2] = 0.;
		  local[1][3] = -Amed*98066.5/dxmed;
		  local[1][4] = (1-(1.-corDRholC*deriMas)*term1R) / dt;
		  local[1][5] = Amed*98066.5/dxmed;

		  TL[1] = (MR-(1.-corDRholC*deriMas)*MliqiniR) / dt+(1.-corDRholC*deriMas)*term2R/dt;

		  double j1;
		  double j2;

		  j1=(MliqiniR/rholC+(MR-MliqiniR)/rhogC)/AC;
		  j2=(MliqiniR/rholR+(MR-MliqiniR)/rhogR)/AR;

		  double coef1C=((1/rhogC)+term1R*(rhogC-rholC)/(rhogC*rholC))/AC;
		  double coef2C=(term2R*(rhogC-rholC)/(rhogC*rholC))/AC;
		  double coef1R=((1/rhogR)+term1R*(rhogR-rholR)/(rhogR*rholR))/AR;
		  double coef2R=(term2R*(rhogR-rholR)/(rhogR*rholR))/AR;

		  double rhomix1;
		  double rhomix2;

		  rhomix1=alf*rhogC+(1-alf)*rholC;
		  rhomix2=alfR*rhogR+(1-alfR)*rholR;

		  double viscmix1;
		  double viscmix2;


		 /* viscmix1=alf*flui.ViscGas(pres,temp)+
				(1-alf)*((1-bet)*flui.ViscOleo(pres,temp)+bet*fluicol.VisFlu(pres,temp));
		  viscmix2=alfR*(*fluiR).ViscGas(presR,tempR)+
				(1-alfR)*((1-betR)*(*fluiR).ViscOleo(presR,tempR)+betR*fluicol.VisFlu(presR,tempR));*/
		  viscmix1=alf*migC+
		 				(1-alf)*((1-bet)*mipC+bet*micC);
		  viscmix2=alfR*migR+
		 				(1-alfR)*((1-betR)*mipR+betR*micR);
		  //viscmix1=alf*flui.ViscGas(pres,temp)+(1-alf)*flui.ViscOleo(pres,temp);
		  //viscmix2=alfR*flui.ViscGas(presR,tempR)+(1-alfR)*flui.ViscOleo(presR,tempR);

		  double re1;
		  double re2;

		  if(duto.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
		  else{
			  double dhid=4* duto.area/duto.peri;
			  re1 = Rey(dhid, j1, rhomix1,viscmix1);
		  }
		  if(dutoR.revest==0)re2 = Rey(dutoR.a, j2, rhomix2,viscmix2);
		  else{
			  double dhid=4* duto.area/duto.peri;
			  re2 = Rey(dhid, j2, rhomix2,viscmix2);
		  }
		  double f1 = fric(re1, duto.rug / duto.a);
		  double f2 = fric(re2, dutoR.rug / dutoR.a,-1);
		  double razdx = dx / (dxR + dx);
		  double coefTensC=dPdLFric*0.5 * f1*rhomix1*( fabsl(j1)) * siC * razdx;
		  double dpfric=coefTensC*coef2C;
		  double coefTensR=dPdLFric*0.5 * f2*rhomix2*( fabsl(j2)) * siR * (1.-razdx);
		  dpfric+=coefTensR*coef2R;
		  double dphidro=dPdLHidro*9.82 * sinl(duto.teta) *AC*rhomix1* razdx;
		  termoHidro=dphidro/AC;
		  termoHidro+=9.82 * sinl(dutoR.teta) *rhomix2* (1.-razdx);
		  termoFric=coefTensC*j1+coefTensR*j2;
		  dphidro+=9.82 * sinl(dutoR.teta) *AR*rhomix2* (1.-razdx);

		  local[1][4] +=(coefTensC*coef1C+coefTensR*coef1R);
		  TL[1]-=(dpfric+dphidro);

		  termoHidro=dphidro;
		  termoFric=dpfric;
		/*}
		else{
			  double AC =duto.area;
			  double AR =dutoR.area;
			  double siC=duto.peri;
			  double siR=dutoR.peri;
			  double rhogC = flui.MasEspGas(pres,temp);
			  double rholC = rpC*(1-bet)+rcC*bet;
			  double rhogR = (*fluiR).MasEspGas(presR,tempR);
			  double rholR = rpR*(1-betR)+rcR*betR;

			  double dxmed = 0.5 * (dx + dxR);
			  double Amed =0.5*(AC+AR);

			  local[1][0] = 0.;
			  local[1][1] = 0.;
			  local[1][2] = -Amed*98066.5/dxmed;
			  //local[1][3] = 1 / dt;
			  local[1][3] = (1-term1R) / dt;
			  local[1][4] = Amed*98066.5/dxmed;

			  //TL[1] = MR / dt;
			  TL[1] = (MR-MliqiniR) / dt+ term2R / dt;

				double j1;
				double j2;

				j1=(MliqiniR/rholC+(MR-MliqiniR)/rhogC)/AC;
				j2=(MliqiniR/rholR+(MR-MliqiniR)/rhogR)/AR;
				//j1=((term1R*MR+term2R)/rholC+(MR-(term1R*MR+term2R))/rhogC)/AC;
				//j2=((term1R*MR+term2R)/rholR+(MR-(term1R*MR+term2R))/rhogR)/AR;

				double coef1C=((1/rhogC)+term1R*(rhogC-rholC)/(rhogC*rholC))/AC;
				double coef2C=(term2R*(rhogC-rholC)/(rhogC*rholC))/AC;
				double coef1R=((1/rhogR)+term1R*(rhogR-rholR)/(rhogR*rholR))/AR;
				double coef2R=(term2R*(rhogR-rholR)/(rhogR*rholR))/AR;

				double rhomix1;
				double rhomix2;

				rhomix1=alf*rhogC+(1-alf)*rholC;
				rhomix2=alfR*rhogR+(1-alfR)*rholR;
				//rhomix1=alf*rhogC+(1-alf)*rholC;
				//rhomix2=alfR*rhogR+(1-alfR)*rholR;

				double viscmix1;
				double viscmix2;


				double CorrViscG=1.;
				double CorrViscL=1.;

				//if(arranjoR==3 && alf<1-1e-3)CorrViscL=1.+2.5*alf;
				//else if(arranjoR==2 && alf<1-1e-3){
					//CorrViscG=0.;
					//CorrViscL=1./(1.-alf);
				//}

				viscmix1=alf*flui.ViscGas(pres,temp)*CorrViscG+
						(1-alf)*((1-bet)*flui.ViscOleo(pres,temp)+bet*fluicol.VisFlu(pres,temp))*CorrViscL;

				CorrViscG=1.;
				CorrViscL=1.;

				//if(arranjoR==3 && alfR<1-1e-3)CorrViscL=1.+2.5*alfR;
				//else if(arranjoR==2 && alfR<1-1e-3){
					//CorrViscG=0.;
					//CorrViscL=1./(1.-alfR);
				//}
				viscmix2=alfR*(*fluiR).ViscGas(presR,tempR)*CorrViscG+
						(1-alfR)*((1-betR)*(*fluiR).ViscOleo(presR,tempR)+betR*fluicol.VisFlu(presR,tempR))*CorrViscL;
				//viscmix1=alf*flui.ViscGas(pres,temp)+
						//(1-alf)*((1-bet)*flui.ViscOleo(pres,temp)+bet*fluicol.VisFlu(temp));
				//viscmix2=alfR*flui.ViscGas(presR,tempR)+
						//(1-alfR)*((1-betR)*flui.ViscOleo(presR,tempR)+betR*fluicol.VisFlu(tempR));


				double re1;
				double re2;

				if(dutoL.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
				else{
					double dhid=4* duto.area/duto.peri;
					re1 = Rey(dhid, j1, rhomix1,viscmix1);
				}
				if(dutoR.revest==0)re2 = Rey(dutoR.a, j2, rhomix2,viscmix2);
				else{
					double dhid=4* duto.area/duto.peri;
					re2 = Rey(dhid, j2, rhomix2,viscmix2);
				}
				double f1 = fric(re1, duto.rug / duto.a);
				double f2 = fric(re2, dutoR.rug / dutoR.a);
				if(duto.teta<0.1){
					f1*=1.;
					f2*=1.;
				}
				double razdx = dx / (dxR + dx);
				double coefTensC=0.5 * f1*rhomix1*( fabsl(j1)) * siC * razdx;
				double dpfric=coefTensC*coef2C;
				double coefTensR=0.5 * f2*rhomix2*( fabsl(j2)) * siR * (1.-razdx);
				dpfric+=coefTensR*coef2R;
				double dphidro=9.82 * sinl(duto.teta) *AC*rhomix1* razdx;
				dphidro+=9.82 * sinl(dutoR.teta) *AR*rhomix2* (1.-razdx);



				double sinalPig=1.;
				if(velPig<0.)sinalPig=1.;

				local[1][3] +=(coefTensC*coef1C+coefTensR*coef1R);
				TL[1]-=(dpfric+dphidro-(dpB-(sinalPig*DelPig*velPig*98066.5))*
						((dx*AC+dxR*AR)/(dx + dxR))/dx);

			  ////////////////////////////////////////////////////////////////////////////////////////////////////

			  rhogC = flui.MasEspGas(presE,tempE);
			  rhogR = flui.MasEspGas(pres,temp);
			  rholC = flui.MasEspLiq(presE,tempE)*(1-betE)+fluicol.MasEspFlu(presE,tempE)*betE;

			  j1=(Mliqini/rholC+(MC-Mliqini)/rhogC)/AC;
			  coef1C=((1/rhogC)+term1*(rhogC-rholC)/(rhogC*rholC))/AC;
			  coef2C=(term2*(rhogC-rholC)/(rhogC*rholC))/AC;
			  rhomix1=titE*rhogC+(1-titE)*rholC;
			  viscmix1=titE*flui.ViscGas(presE,tempE)+
					(1-titE)*((1-betE)*flui.ViscOleo(presE,tempE)+betE*fluicol.VisFlu(presE,tempE));
			  if(duto.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
			  else{
				  double dhid=4* duto.area/duto.peri;
				  re1 = Rey(dhid, j1, rhomix1,viscmix1);
			  }
			  f1 = fric(re1, duto.rug / duto.a);
			  coefTensC=0.5 * f1*rhomix1*( fabsl(j1)) * siC;
			  dpfric=coefTensC*coef2C;
			  dphidro=9.82 * sinl(duto.teta) *AC*rhomix1;

			  double ugl=(MC-Mliqini)/rhogC;
			  double ugr=(MR-MliqiniR)/rhogR;
			  double dxm=dx;
			  double coefacelera1L=(-(1-term1)*ugl+0.25*(ugl+ugr)*(1-term1))/dxm;
			  double coefacelar2L=(term2*ugl-0.25*(ugl+ugr)*term2)/dxm;
			  double coefacelera1R=(0.25*(ugl+ugr)*(1-term1R))/dxm;
			  double coefacelar2R=(-0.25*(ugl+ugr)*term2R)/dxm;

			  local[0][0] = 0.;
			  local[0][1] = 0.;
			  local[0][2] =coefTensC*coef1C+(1-term1) / dt;
			  local[0][3] = Amed*98066.5/(dx);
			  local[0][4] = 0;

			  TL[0] = (MC-Mliqini) / dt+ term2 / dt;

			  TL[0] +=(Amed*98066.5*presE/(dx)-(dpfric+dphidro));
			  //corrigeMassaPres=-
					 // (0.25*(MR-MliqiniR+MC-Mliqini)*(ugr+ugl) / dxm-
					  // (MC-Mliqini)*(ugl) / dxm);


		}*/

	}

}

 void Cel::avancPig(int& reinicia){
  //double AC =duto.area;
  //double rhogR = flui.MasEspGas(presR,tempR);
  //double jL=(QL+QG)/AC;
  //double jR=(QLR+((MR-MliqiniR)/rhogR))/AC;
  //velPig=jL*razPig+jR*(1.-razPig);
  razPig=(razPigini*dx+velPig*dt)/dx;

  if(((razPig<=(*vg1dSP).localtiny)&&(razPig>=-(*vg1dSP).localtiny))) razPig=0;
  else if(razPig<-(*vg1dSP).localtiny){
     double dtaux;
     dtaux=-razPigini*dx/velPig;
     if(dtaux>(*vg1dSP).localtiny){
       dtPig=dtaux;
       //if(correrGlobHol==1)reinicia=-1;
       //else
    	   reiniciaPig=-1;
       razPig=fabsl(0.);
     }
     else razPig=fabsl(0.);
  }
  else if((razPig>=(1.-(*vg1dSP).localtiny)&&razPig<=(1.+(*vg1dSP).localtiny))){
	   razPig=1.;
  }
  else if(razPig>(1.+(*vg1dSP).localtiny)){
     double dtaux;
     dtaux=(1.-razPigini)*dx/velPig;
     if(dtaux>(*vg1dSP).localtiny){
       dtPig=dtaux;
       //if(correrGlobHol==0)reinicia=-1;
       //else
    	   reiniciaPig=-1;
       razPig=1.;
     }
     else razPig=1.;
  }

}


 void Cel::avancalfPig(){
   double term1Mass=0.;
   if(alf<=(*vg1dSP).localtiny){
	   alfPigD=0.;
	   alfPigE=0.;
   }
   else if(alf>=(1.-(*vg1dSP).localtiny)){
	   alfPigD=1.;
	   alfPigE=1.;
   }
   else{
	   if(velPig>=0.){

		 double termoPtrans=0.;
		 double termoDrholP=0.;
		 double termoTtrans=0.;
		 double termoDrholT=0.;
		 if(TMModel==0){
		    termoPtrans=-(DTransDtp/rpC)*m2d*(dpdt);
		    termoDrholP=(duto.area*(1.-alf)*(1-bet)/rpC)*((rpC-flui.MasEspLiq(presini*0.999, temp))/(0.001*presini))*
		 			   m2d*(dpdt)*ativaDeri;

		    double dTtemp=dTdt;
		    termoTtrans=-(DTransDtT/rpC)*(dTtemp);
		    termoDrholT=(duto.area*(1.-alf)*(1-bet)/rpC)*((rpC-flui.MasEspLiq(presini, temp*0.999))/(0.001*temp))*
		 			   (dTtemp)*ativaDeri;
		}

        if(RazAreaPig>(*vg1dSP).localtiny){//alteracao2
    	 double areagarg=RazAreaPig*duto.area;
    	 double rlm=(1.-alfPigEini)*((1-betPigEini)*rpC+betPigEini*rcC)+
    			          alfPigEini*flui.MasEspGas(pres,temp);
     	 double massica=areagarg*sqrtl(2.*rlm*(DelPig*velPig)*98066.52);
   		 massica=cdpig*massica;
   		 VazaPig=massica/rlm;
        }//alteracao2
        else VazaPig=0.;//alteracao2
        double fontepigL=VazaPig*(1.-alfPigEini)*(1.-betPigEini)*rpC;//alteracao2
        double fontepigC=VazaPig*(1.-alfPigEini)*(betPigEini)*rcC;//alteracao2


	    if(razPig<=(1.-(*vg1dSP).localtiny)){
	     double rlm=(1-betPigDini)*rpC+betPigDini*rcC;

	     double fator1=1.-(rcC-rpC)*betPigDini/rlm;
	     fator1=1./fator1;
	     double fator2=(rcC-rpC)/rcC;

	     double bet0=betPigDini;
	     double bet1;
	     if(MliqiniR>0)bet1=betPigDini;
	     else bet1=betRini;

	     double razdx1=dxR/(dx+dxR);
	     double tmed1=razdx1*tempR+(1-razdx1)*temp;
	     double rc0=rcC;
	     double rc1=fluicol.MasEspFlu(presauxR,tmed1);

	     double convec=fator1*(MliqiniR-rlm*(1-alfPigDini)*velPig*duto.area)/((1.-razPig)*dx)-
			     fator2*fator1*(QLR*rc1*bet1-velPig*(1-alfPigDini)*duto.area*rc0*bet0)/((1.-razPig)*dx)
				 -fator1*(fontepigL/((1.-razPig)*dx)+(1.-fator2)*fontepigC/((1.-razPig)*dx));//alteracao2
	     //double convec=0.;

	     if(razPig<0.5)term1Mass=(convec-fator1*(fontemassLR/((1.-razPig)*dx)+
	    		            (1.-fator2)*fontemassCR/((1.-razPig)*dx)-transmassR-
				    		 rpC*(termoPtrans+termoDrholP+termoTtrans+termoDrholT)))/(rlm*duto.area);
	     else term1Mass=(convec+fator1*(transmassR+
	    		 rpC*(termoPtrans+termoDrholP+termoTtrans+termoDrholT)))/(rlm*duto.area);
	     //term1Mass=(convec-fator1*(fontemassLR/(dx)+(1.-fator2)*fontemassCR/(dx)-transmassR))/(rlm*duto.area);
	     alfPigD=alfPigDini+term1Mass*dt;
	     if(alfPigD<0.)alfPigD=0.;
	     if(alfPigD>1.)alfPigD=1.;
	     alfPigE=(-alfPigD*(1.-razPig)+alf)/razPig;
	     if(alfPigE<0.)alfPigE=0.;
	     if(alfPigE>1.)alfPigE=1.;
	   }
	   else{
		   alfPigD=alfPigDini;
		   alfPigE=alf;
	   }
   }
   else{
		 double termoPtrans=0.;
		 double termoDrholP=0.;
		 double termoTtrans=0.;
		 double termoDrholT=0.;
		 if(TMModel==0){
		    termoPtrans=-(DTransDtp/rpC)*m2d*(dpdt);
		    termoDrholP=(duto.area*(1.-alf)*(1-bet)/rpC)*((rpC-flui.MasEspLiq(presini*0.999, temp))/(0.001*presini))*
		 			   m2d*(dpdt)*ativaDeri;

		    double dTtemp=dTdt;
		    termoTtrans=-(DTransDtT/rpC)*(dTtemp);
		    termoDrholT=(duto.area*(1.-alf)*(1-bet)/rpC)*((rpC-flui.MasEspLiq(presini, temp*0.999))/(0.001*temp))*
		 			   (dTtemp)*ativaDeri;
		}
       if(RazAreaPig>(*vg1dSP).localtiny){//alteracao2
   	     double areagarg=RazAreaPig*duto.area;
   	     double rlm=(alfPigDini)*((1-betPigDini)*rpC+betPigDini*rcC)+
		                  alfPigDini*flui.MasEspGas(pres,temp);
    	 double massica=areagarg*sqrtl(2.*rlm*(DelPig*fabsl(velPig))*98066.52);
  		 massica=cdpig*massica;
  		 VazaPig=-massica/rlm;
       }//alteracao2
       else VazaPig=0.;//alteracao2
       double fontepigL=-VazaPig*(1.-alfPigDini)*(1.-betPigDini)*rpC;//alteracao2
       double fontepigC=-VazaPig*(1.-alfPigDini)*(betPigDini)*rcC;//alteracao2

	   if(razPig>=(*vg1dSP).localtiny){

	     double rlm=(1-betPigEini)*rpC+betPigEini*rcC;

	     double fator1=1.-(rcC-rpC)*betPigEini/rlm;
	     fator1=1./fator1;
	     double fator2=(rcC-rpC)/rcC;

	     double bet0;
	     if(Mliqini>0)bet0=betLini;
	     else bet0=betPigEini;
	     double bet1=betPigEini;

	     double razdx0=dx/(dx+dxL);
	     double tmed0=razdx0*temp+(1-razdx0)*tempL;
	     double rc0=fluicol.MasEspFlu(presaux,tmed0);
	     double rc1=rcC;
	     double convec=fator1*(rlm*(1.-alfPigEini)*velPig*duto.area-Mliqini)/(razPig*dx)-
			              fator2*fator1*(velPig*(1.-alfPigEini)*duto.area*rc1*bet1-QL*rc0*bet0)/(razPig*dx)
	                     -fator1*(fontepigL/(razPig*dx)+(1.-fator2)*fontepigC/(razPig*dx));//alteracao2
	     //double convec=0.;

	     if(razPig>=0.5)term1Mass=(convec-fator1*(fontemassLR/(razPig*dx)+(1.-fator2)*fontemassCR/(razPig*dx)-
	    		                   transmassR-
						    	rpC*(termoPtrans+termoDrholP+termoTtrans+termoDrholT)))/(rlm*duto.area);
	     else term1Mass=(convec+fator1*(transmassR+
	    		 rpC*(termoPtrans+termoDrholP+termoTtrans+termoDrholT)))/(rlm*duto.area);
	     //term1Mass=(convec-fator1*(fontemassLR/(dx)+(1.-fator2)*fontemassCR/(dx)-transmassR))/(rlm*duto.area);
	     alfPigE=alfPigEini+term1Mass*dt;
	     if(alfPigE<0.)alfPigE=0.;
	     if(alfPigE>1.)alfPigE=1.;
	     alfPigD=(-alfPigE*razPig+alf)/(1-razPig);
	     if(alfPigD<0.)alfPigD=0.;
	     if(alfPigD>1.)alfPigD=1.;
     }
	 else{
		   alfPigD=alf;
		   alfPigE=alfPigEini;
	 }
   }
   }
}

 void Cel::avancbetPig(){

   double term1Mass=0.;

   double fator1=rcC*duto.area;
   if(bet<=(*vg1dSP).localtiny){
	   betPigD=0.;
	   betPigE=0.;
   }
   else if(bet>=(1.-(*vg1dSP).localtiny)){
	   betPigD=1.;
	   betPigE=1.;
   }
   else{
   if(velPig>=0.){
	   double fontepigC=VazaPig*(1.-alfPigEini)*(betPigEini)*rcC;//alteracao2
	   if(razPig<=(1.-(*vg1dSP).localtiny)){
	     double bet0=betPigDini;
	     double bet1;
	     if(MliqiniR>0)bet1=betPigDini;
	     else bet1=betRini;

	     double razdx1=dxR/(dx+dxR);
	     double tmed1=razdx1*tempR+(1-razdx1)*temp;
	     double rc0=rcC;
	     double rc1=fluicol.MasEspFlu(presauxR,tmed1);


	     double convec=(QLR*rc1*bet1-velPig*duto.area*(1-alfPigDini)*rc0*bet0)/((1.-razPig)*dx)+fontepigC/((1.-razPig)*dx);
	     //double convec=0.;
	     if(razPig<0.5)term1Mass=(-convec+fator1*betPigDini*(alfPigD-alfPigDini)/dt+fontemassCR/((1.-razPig)*dx))/(fator1*(1-alfPigDini));
	     else term1Mass=(-convec+fator1*betPigDini*(alfPigD-alfPigDini)/dt)/(fator1*(1-alfPigDini));
	     //term1Mass=(-convec+fator1*betPigDini*(alfPigD-alfPigDini)/dt+fontemassCR/(dx))/(fator1*(1-alfPigDini));
	     betPigD=betPigDini+term1Mass*dt;
	     if(betPigD<0.)betPigD=0.;
	     if(betPigD>1.)betPigD=1.;
	     betPigE=(-betPigD*(1.-razPig)+bet)/razPig;
	     if(betPigE<0.)betPigE=0.;
	     if(betPigE>1.)betPigE=1.;
	   }
	   else{
		   betPigD=betPigDini;
		   betPigE=bet;
	   }
   }
   else{
	   double fontepigC=-VazaPig*(1.-alfPigDini)*(betPigDini)*rcC;//alteracao2
	   if(razPig>=(*vg1dSP).localtiny){
	     double bet0;
	     if(Mliqini>0)bet0=betLini;
	     else bet0=betPigEini;
	     double bet1=betPigEini;

	     double razdx0=dx/(dx+dxL);
	     double tmed0=razdx0*temp+(1-razdx0)*tempL;
	     double rc0=fluicol.MasEspFlu(presaux,tmed0);
	     double rc1=rcC;

	     double convec=(velPig*duto.area*(1-alfPigEini)*rc1*bet1-QL*rc0*bet0)/(razPig*dx)+fontepigC/(razPig*dx);
	     //double convec=0.;
	     if(razPig>=0.5)term1Mass=(-convec+fator1*betPigEini*(alfPigE-alfPigEini)/dt+fontemassCR/(razPig*dx))/(fator1*(1-alfPigEini));
	     else term1Mass=(-convec+fator1*betPigEini*(alfPigE-alfPigEini)/dt)/(fator1*(1-alfPigEini));
	     //term1Mass=(-convec+fator1*betPigEini*(alfPigE-alfPigEini)/dt+fontemassCR/(dx))/(fator1*(1-alfPigEini));
	     betPigE=betPigEini+term1Mass*dt;
	     if(betPigE<0.)betPigE=0.;
	     if(betPigE>1.)betPigE=1.;
	     betPigD=(-betPigE*razPig+bet)/(1-razPig);
	     if(betPigD<0.)betPigD=0.;
	     if(betPigD>1.)betPigD=1.;
	   }
	   else{
			betPigD=bet;
			betPigE=betPigEini;
	   }
   }
   }

}

 void Cel::FeiticoDoTempo(){
	pres=presini;
	presL=presLini;
	MliqiniR=MliqiniR0;
	MR=MRini;
	temp=tempini;
	//////////////////////////////////////////////////////////////////
	presR=presRini;
	presaux=presauxini;
	MC=MCini;
	MR=MRini;
	ML=MLini;
	Mliqini=Mliqini0;
	MliqiniL=MliqiniL0;
	tempL=tempLini;
	tempR=tempRini;
	//////////////////////////////////////////////////////////////////////
	QLL=QLLini;
	QLR=QLRini;
	QL=QLini;
	QG=QGini;
	presauxR=presauxRini;
	presauxL=presauxLini;
	/*fontemassLL=fontemassLLini;
	fontemassGL=fontemassGLini;
	fontemassCL=fontemassCLini;
	fontemassLR=fontemassLRini;
	fontemassGR=fontemassGRini;
	fontemassCR=fontemassCRini;
	transmassL=transmassLini;
	transmassR=transmassRini;
	FonteMudaFase=FonteMudaFaseini;
	DTransDt1=DTransDt1ini;
	DTransDt0=DTransDt0ini;
	DTransDxR=DTransDxRini;
	DTransDxL=DTransDxLini;
    DTransDxRp=DTransDxRpini;
	DTransDxLp=DTransDxLpini;
	coefTransBet=coefTransBetini;
	CoefDTR=CoefDTRini;
	CoefDTL=CoefDTLini;*/
	VTemper=VTemperini;

	calor.FeiticoDoTempo();

}

 void Cel::FeiticoDoTempo2(){

	    alfPigER=alfPigERini;
	    indpig=indpigini;
	    betI=betIini;
	    betRI=betRIini;
	    betLI=betLIini;
	    alfL=alfLini;
	    alf=alfini;
	    alfR=alfRini;
	    betL=betLini;
	    bet=betini;
	    betR=betRini;
		estadoPig=estadoPigini;
		alfPigE=alfPigEini;
		alfPigD=alfPigDini;
		betPigE=betPigEini;
		betPigD=betPigDini;
		velPig=velPigini;
		razPig=razPigini;

		transic=transic0;
		c0=c0ini;
		ud=udini;

		/*pres=presini;
		presL=presLini;
		MliqiniR=MliqiniR0;
		MR=MRini;
 	//////////////////////////////////////////////////////////////////
		presR=presRini;
		presaux=presauxini;
		MC=MCini;
		MR=MRini;
		ML=MLini;
		Mliqini=Mliqini0;
		MliqiniL=MliqiniL0;
 	//////////////////////////////////////////////////////////////////////
		QLL=QLLini;
		QLR=QLRini;
		QL=QLini;
		QG=QGini;
		presauxR=presauxRini;
		presauxL=presauxLini;*/


 }

 void Cel::FeiticoDoTempo3(){
 	pres=presini;
 	presL=presLini;
 	MliqiniR=MliqiniR0;
 	MR=MRini;
 	//////////////////////////////////////////////////////////////////
 	presR=presRini;
 	presaux=presauxini;
 	MC=MCini;
 	MR=MRini;
 	ML=MLini;
 	Mliqini=Mliqini0;
 	MliqiniL=MliqiniL0;
 	//////////////////////////////////////////////////////////////////////
 	QLL=QLLini;
 	QLR=QLRini;
 	QL=QLini;
 	QG=QGini;
 	presauxR=presauxRini;
 	presauxL=presauxLini;


 }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 double Cel::SIGN(double a, double b) {
   return (b >= 0 ? 1.0 : -1.0) * fabsl(a);
 }


 double  Cel::zriddrRhop(double x1,double x2, double rp){
 	double xacc=1e-10;
 	int maxit=100;
 	double fmin;
 	double xmin;
 	double fl=rp-flui.MasEspLiq(x1,temp);
 	double fh=rp-flui.MasEspLiq(x2,temp);
 	if(fabsl(fh)<fabsl(fl)){
 		fmin=fh;
 		xmin=x2;
 	}
 	else{
 		fmin=fl;
 		xmin=x1;
 	}
 	if((fl>0.0 && fh<0.0) || (fl<0.0 && fh>0.0)){
 		double xl=x1;
 		double xh=x2;
 		double ans=-1.e20;
 		for(int j=0;j<maxit;j++){
 			double xm=0.5*(xl+xh);
 			double fm=rp-flui.MasEspLiq(xm,temp);
 			if(fabsl(fm)<fabsl(fmin)){
 				fmin=fm;
 				xmin=xm;
 			}
 			double s=sqrtl(fm*fm-fl*fh);
 			if(s==0.0){
 				fmin=rp-flui.MasEspLiq(xmin,temp);
 				return xmin;
 			}
 			double xnew=xm+(xm-xl)*((fl>=fh ? 1.0 : -1.0)*fm/s);
 			if(fabsl(xnew-ans)<=xacc ){
 				fmin=rp-flui.MasEspLiq(xmin,temp);
 				return xmin;
 			}
 			ans=xnew;
 			double fnew=rp-flui.MasEspLiq(ans,temp);
 			if(fabsl(fnew)<fabsl(fmin)){
 				fmin=fnew;
 				xmin=ans;
 			}
 			if(fabsl(fnew)<=xacc ){
 				fmin=rp-flui.MasEspLiq(xmin,temp);
 				return xmin;
 			}
 			if(SIGN(fm, fnew)!=fm){
 				xl=xm;
 				fl=fm;
 				xh=ans;
 				fh=fnew;
 			}
 			else if(SIGN(fl,fnew)!=fl){
 				xh=ans;
 				fh=fnew;
 			}
 			else if(SIGN(fh,fnew)!=fh){
 				xl=ans;
 				fl=fnew;
 			}
 			else return -100000.;
 			if(fabsl(xh-xl)<=xacc ){
 				fmin=rp-flui.MasEspLiq(xmin,temp);
 				return xmin;
 			}

 		}
 		 return 100000.;
 	}
 	else {
 		if(fabsl(fl)<=xacc) return x1;
 		if(fabsl(fh)<=xacc) return x2;
 		return -10000.;
 	}
 }

double Cel::somVel(){
	double val;
	double alfI;
	double betI;
	if(QG>0)alfI=alfL;
	else alfI=alf;
	if(QL>0)betI=bet;
	else betI=bet;

	double tempMed;
	double raz;
	if(posic==0)tempMed=temp;
	else{
		raz=dxL/(dx+dxL);
		tempMed=(1.-raz)*tempL+raz*temp;
	}
	if(alfI<1e-15){
		val=1500.;
	}
	else{
		 double kad=flui.ConstAdG(presaux, tempMed,rgCi);
		 double rlmix=rpCi*(1.-betI)+rcCi*betI;
		 double cg2=kad/flui.drhodp(presaux, tempMed);
		 double vl;
		 if(alfI<1-1e-15)vl=QL/(duto.area*(1.-alfI));
		 else vl=0;
		 double vg;
		 if(alfI>1e-15)vg=QG/(duto.area*alfI);
		 else vg=0.;
		 double val1=1.-c0*alfI+c0*alfI*alfI*(vg-vl)*(vg-vl)*(c0*alfI-1.)/(cg2)+c0*alfI*rgCi/rlmix;
		 val1*=rgCi*cg2;
		 val1=sqrtl(val1);
		 val1/=sqrtl(alfI*rlmix);
		 val1/=(1.-c0*alfI+c0*alfI*(rgCi/rlmix));
		 val=val1;
		 if(val>1500.)val=1500.;
	}
	return val;
 }

double Cel::termAdSomVel(){
	double val;
	double alfI;
	double betI;
	if(QG>0)alfI=alfL;
	else alfI=alf;
	if(QL>0)betI=bet;
	else betI=bet;

	double tempMed;
	double raz;
	if(posic==0)tempMed=temp;
	else{
		raz=dxL/(dx+dxL);
		tempMed=(1.-raz)*tempL+raz*temp;
	}
	if(alfI<1e-15){
		 if(alfI<1-1e-15)val=QL/(duto.area*(1.-alfI));
		 else val=0;
	}
	else{
		 double rlmix=rpCi*(1.-betI)+rcCi*betI;
		 double vl;
		 if(alfI<1-1e-15)vl=QL/(duto.area*(1.-alfI));
		 else vl=0;
		 double vg;
		 if(alfI>1e-15)vg=QG/(duto.area*alfI);
		 else vg=0.;
		 double val1=(alfI*c0)*(rgCi/rlmix);
		 val=(val1*(vg)+vl*(1.-c0*alfI))/(val1+(1.-c0*alfI));
	}
	return val;
 }

/*void Cel::WaxDeposition(int BooleanPi, double C2_star, double C3_star, int BooleanFi,
		                double ConstFi, double MultipDwax, double rug, double MultipVisc,
						int alteraViscFlu, int ncel){

	if(posic==186){
		int para;
		para=0;
	}

	flui.atualizaPropParafina(pres, temp); // A fração molar dos componentes parafínicos na fase líquida vecZwaxLiq[i] vem da tabela termodinâmica do PVTsim

	double fracOil = (1-alf)*(1.-bet)*(1-FW);
	if(calor.Tcamada[0][0]<flui.dCloudPointTOutput && fracOil>1.0E-2 && calor.fluxIni<0.){

		int comp=flui.npseudoWax;
		double MW_wax = 0.0;
		double rhoWaxLiq = 0.0;
		double SumZwaxLiq = 0.0;
		double SumCwaxLiq = 0.0;
		double Vwax;
		double vecCwaxLiq[comp];
		for (int i = 0; i < comp; i++){
			//MW_wax = MW_wax + vecZwaxLiq[i]*flui.oMolecularWeightsOfWaxComponentsOut[i];
			MW_wax = MW_wax + flui.oInterpolatedWaxConcs[i]*flui.oMolecularWeightsOfWaxComponentsOut[i];
			vecCwaxLiq[i] = flui.oInterpolatedWaxConcs[i]*flui.oMolecularWeightsOfWaxComponentsOut[i];
			rhoWaxLiq = rhoWaxLiq + vecCwaxLiq[i]*flui.oLiquidDensitiesOfWaxComponents[i];
			SumZwaxLiq = SumZwaxLiq + flui.oInterpolatedWaxConcs[i];
			SumCwaxLiq = SumCwaxLiq + vecCwaxLiq[i];
		}
		MW_wax = MW_wax/SumZwaxLiq;
		rhoWaxLiq = rhoWaxLiq/SumCwaxLiq;
		Vwax = 1.0E+3*MW_wax/rhoWaxLiq;

		double Dparaffin;
		double AssocParam = 1.0;
		double Sum_dCwaxdT = 0.0;
		double muOil=flui.ViscOleo(pres, temp,1);
		double muOilf=flui.ViscOleo(pres, calor.Tcamada[0][0],1);
		muOil = MultipVisc*muOil;
		muOilf = MultipVisc*muOilf;
		double Tint=calor.Tcamada[0][0] + 273.15;
		double MwOil=flui.dInterpolatedLiqMWOutput;
		Dparaffin = MultipDwax*7.4E-12*Tint*pow(AssocParam*MwOil,0.5)/((muOilf*1e-3)*pow(Vwax,0.6));
		for (int i = 0; i < comp; i++){
			Sum_dCwaxdT = Sum_dCwaxdT + flui.oInterpolatedWaxConcsTDerivOutput[i];
		}

		double Re_f, Re_delta;
		double Fi;
		double velOilUp=0.;
		double velSupLiqUp;
		double alfDn;
		double betaDn;
		double areaDn;
		double velOilDn=0.;
		double velSupLiqDn;
		double alfUp;
		double betaUp;
		double areaUp;

		if(QL>0. && posic>0){
			areaDn=dutoL.area;
			betaDn=betL;
		}
		else{
			areaDn=duto.area;
			betaDn=bet;
		}
		if(QG>0. && posic>0){
			alfDn=alfL;
		}
		else{
			alfDn=alf;
		}
		if(QLR<0.&& posic<ncel){
			areaUp=dutoR.area;
			betaUp=betR;
		}
		else{
			areaUp=duto.area;
			betaUp=bet;
		}
		if(MR<0.&& posic<ncel){
			alfUp=alfR;
		}
		else{
			alfUp=alf;
		}

		velSupLiqUp=QLR/areaUp;
		velSupLiqDn=QL/areaDn;
		if(fabsl(1.-alfUp)>1e-15)velOilUp=velSupLiqUp/(1.-alfUp);
		if(fabsl(1.-alfDn)>1e-15)velOilDn=velSupLiqUp/(1.-alfDn);

		double velOil = (velOilUp + velOilDn)/2.0;
		if(posic==0)velOil = velOilUp;
		double velSupLiq = (velSupLiqUp + velSupLiqDn)/2.0;
		if(posic==0)velSupLiq = velSupLiqUp;

		double rhoOil=flui.MasEspoleo(pres, temp);
		double alphaLiq=(1-bet)*(1-alf)*(1-FW);
		double Dwax_old=duto.dia;

		Re_f = (rhoOil*velOil*Dwax_old)/(1.0E-3*muOilf);
		Fi = ConstFi*BooleanFi + (1 - BooleanFi)*(1.0 - pow(Re_f,0.15)/8.0);
		porosoPar=Fi;
		int flowPattern=arranjo;
		double delta_old;
		if(parafinado==0)delta_old=0.;
		else delta_old=duto.espessuR[0];
		double rhoLiq=(1. - bet) * rpC+ bet * rcC;
		double viscLiq=(1. - bet) * mipC+ bet * micC;
		double rhoMix=alf*rgC+(1-alf)*rhoLiq;

		if (flowPattern == 0 || flowPattern == -1){
			Re_delta = rhoLiq*velOil*delta_old/(1.0E-3*viscLiq);} // rhoLiq está em kg/m^3? A dimensão deve ser kg/m^3.
		else if (flowPattern == 1 || flowPattern == 2){
			Re_delta = rhoMix*velSupLiq*delta_old/(alphaLiq*(1.0E-3*viscLiq));}
		else if (flowPattern == -2){
			Re_delta = pow(rhoMix*rhoLiq,0.5)*velSupLiq*delta_old/(alphaLiq*(1.0E-3*viscLiq));
		}

		double pi1, pi2;
		double C1;//, C2, C3;
		C1 = 15.0;
		C2 = BooleanPi*C2_star + (1-BooleanPi)*0.055;
		C3 = BooleanPi*C3_star + (1-BooleanPi)*1.4;
		pi1 = C1;
		pi2 = C2*pow(Re_delta,C3);

		// Obtain the deposit thickness, free flow diameter, specific mass of the solid phase and deposit, specific heat and conductivity of the deposit

		double rhoWaxSolid = 1.1*rhoWaxLiq;
		double ddeltadt;
		double heatFluxInt=-calor.fluxIni;
		double kMix;
		double condliq = (1. - bet) * flui.CondLiq(pres, temp) + bet * fluicol.CondLiq(pres, temp);
		double kOil=flui.CondOleo(pres, temp);
		kMix = condliq * (1 - alf) + flui.CondGas(pres, temp) * alf;

		//????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
		//????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
		// Neste ponto surge uma dúvida, considere que ainda existe uma certa fração de líquido na célula, isto,
		// pelo teste de entrada no cálculo do depósito vai nos levar até este ponto, se esta fração de óleo for muito pequena,
		// não existe o risco de se calcular uma massa depositada maior do que a quantiodade de óleo disponível na célula?
		// não se deve fazer um teste para esta condição? Considero que isto não é tão improvávcel em processos de parada de produção segregação
		// dos fluidos. NO FUTURO, AVALIAR CASOS DE PARADA DE PRODUÇÃO, SEGREGAÇÃO, ESCOAMENTO REVERSO, REDUÇÃO DA FRAÇÃO DE PARAFINA, ETC.

		//atencao!!!!!!!!! Coloquei em módulo, mas apenas para teste, algo está estranho espessura negativa!!!!!!!!!
		ddeltadt = (pi1/(1.0+pi2))*((rhoLiq*Fi)/(rhoWaxSolid*(1.0-Fi)))*(heatFluxInt/kMix)*Dparaffin*fabsl(Sum_dCwaxdT);
		double delta =ddeltadt*dt;
		deltaPar=delta;

		double rhoDep = Fi*rhoOil + (1.0-Fi)*rhoWaxSolid;
		double cpDep = flui.dInterpolatedCPWaxOutput;

		double kDep = ((2*flui.dInterpolatedThermCondOutput + kOil +
						   (flui.dInterpolatedThermCondOutput - kOil)*Fi)/
						   (2*flui.dInterpolatedThermCondOutput + kOil - 2*(flui.dInterpolatedThermCondOutput - kOil)*Fi))*kOil;
		if(isnan(delta)){
			int para;
			para=0;
		}
		if(parafinado==0 && delta>0.){
			duto.atualizaCamada(delta, rug, cpDep, kDep, rhoDep);
			calor.atualiza(duto, 1);
			parafinado=1;
		}
		else if(delta>0){
			duto.atualizaCamada2(delta, cpDep, kDep, rhoDep);
			calor.atualiza2(duto);
		}
	}
}*/

/*celula[i].WaxDeposition(arq.detalParafina.C2C3, arq.detalParafina.valC2, arq.detalParafina.valC3,
		arq.detalParafina.poroRey, arq.detalParafina.valRey, arq.detalParafina.multDifus,
		arq.detalParafina.rug, arq.detalParafina.multVis, arq.detalParafina.alteraViscFlu, ncel);
void Cel::WaxDeposition(int BooleanPi, double C2_star, double C3_star, int BooleanFi,
		                double ConstFi, double MultipDwax, double rug, double MultipVisc,
						int alteraViscFlu, int ncel)

	arq.detalParafina.C2C3 =  BooleanPi
	arq.detalParafina.valC2 = C2_star
	arq.detalParafina.valC3 = C3_star
	arq.detalParafina.poroRey = BooleanFi
	arq.detalParafina.valRey = ConstFi
	arq.detalParafina.multDifus = MultipDwax
	arq.detalParafina.rug = rug
	arq.detalParafina.multVis = MultipVisc
	arq.detalParafina.alteraViscFlu = alteraViscFlu




*/


void Cel::WaxDeposition(dadosParafina& detalParafina, int ncel){

	if(posic==186){
		int para;
		para=0;
	}


	double BooleanPi=detalParafina.C2C3;
	double C2_star=detalParafina.valC2;
	double C3_star=detalParafina.valC3;
	double BooleanFi=detalParafina.poroRey;
	double ConstFi=detalParafina.valRey;
	double MultipDwax=detalParafina.multDifus;
	double rug=detalParafina.rug;
	double MultipVisc=detalParafina.multVis;
	double alteraViscFlu=detalParafina.alteraViscFlu;
	double DVisc=detalParafina.DViscWax;
	double EVisc=detalParafina.EViscWax;
	double FVisc=detalParafina.FViscWax;
	double D_multip = detalParafina.DmultipWax; // Samuel - 10/10/25
	double E_multip = detalParafina.EmultipWax; // Samuel - 10/10/25
	double F_multip = detalParafina.FmultipWax; // Samuel - 10/10/25

	//flui.atualizaPropParafina(pres, temp); // A fração molar dos componentes parafínicos na fase líquida vecZwaxLiq[i] vem da tabela termodinâmica do PVTsim
	flui.atualizaPropParafina(pres, calor.Tcamada[0][0]);
	double fracOil = (1-alf)*(1.-bet)*(1-FW);
	if(calor.Tcamada[0][0]<flui.dCloudPointTOutput && fracOil>1.0E-2 && calor.fluxIni<0.){

		int comp=flui.npseudoWax;
		MW_wax = 0.0;
		rhoWaxLiq = 0.0;
		double SumZwaxLiq = 0.0;
		double SumCwaxLiq = 0.0;
		double Vwax;
		double vecCwaxLiq[comp];
		for (int i = 0; i < comp; i++){
			//MW_wax = MW_wax + vecZwaxLiq[i]*flui.oMolecularWeightsOfWaxComponentsOut[i];
			MW_wax = MW_wax + flui.oInterpolatedWaxConcs[i]*flui.oMolecularWeightsOfWaxComponentsOut[i];
			vecCwaxLiq[i] = flui.oInterpolatedWaxConcs[i]*flui.oMolecularWeightsOfWaxComponentsOut[i];
			rhoWaxLiq = rhoWaxLiq + vecCwaxLiq[i]*flui.oLiquidDensitiesOfWaxComponents[i];
			SumZwaxLiq = SumZwaxLiq + flui.oInterpolatedWaxConcs[i];
			SumCwaxLiq = SumCwaxLiq + vecCwaxLiq[i];
		}
		MW_wax = MW_wax/SumZwaxLiq;
		rhoWaxLiq = rhoWaxLiq/SumCwaxLiq;
		Vwax = 1.0E+3*MW_wax/rhoWaxLiq;

		double Re_f, Re_delta;
		double Fi;
		double velOilUp=0.;
		double velSupLiqUp;
		double alfDn;
		double betaDn;
		double areaDn;
		double velOilDn=0.;
		double velSupLiqDn;
		double alfUp;
		double betaUp;
		double areaUp;

		if(QL>0. && posic>0){
			areaDn=dutoL.area;
			betaDn=betL;
		}
		else{
			areaDn=duto.area;
			betaDn=bet;
		}
		if(QG>0. && posic>0){
			alfDn=alfL;
		}
		else{
			alfDn=alf;
		}
		if(QLR<0.&& posic<ncel){
			areaUp=dutoR.area;
			betaUp=betR;
		}
		else{
			areaUp=duto.area;
			betaUp=bet;
		}
		if(MR<0.&& posic<ncel){
			alfUp=alfR;
		}
		else{
			alfUp=alf;
		}

		velSupLiqUp=QLR/areaUp;
		velSupLiqDn=QL/areaDn;
		if(fabsl(1.-alfUp)>1e-15)velOilUp=velSupLiqUp/(1.-alfUp);
		if(fabsl(1.-alfDn)>1e-15)velOilDn=velSupLiqUp/(1.-alfDn);

		double velOil = (velOilUp + velOilDn)/2.0;
		if(posic==0)velOil = velOilUp;
		double velSupLiq = (velSupLiqUp + velSupLiqDn)/2.0;
		if(posic==0)velSupLiq = velSupLiqUp;

		double rhoOil=flui.MasEspoleo(pres, temp);
		double alphaLiq=(1-bet)*(1-alf)*(1-FW);
		double Dwax_old=duto.dia;

		int flowPattern=arranjo;
		double delta_old;
		if(parafinado==0)delta_old=0.;
		else delta_old=duto.espessuR[0];
		double rhoLiq=(1. - bet) * rpC+ bet * rcC;
		double viscLiq=(1. - bet) * mipC+ bet * micC;
		double rhoMix=alf*rgC+(1-alf)*rhoLiq;

		double Dparaffin;
		double AssocParam = 1.0;
		Sum_dCwaxdT = 0.0;
		double muOil=flui.ViscOleo(pres, temp,1);
		double muOilf=flui.ViscOleo(pres, calor.Tcamada[0][0],1);
		Re_f = (rhoOil*velOil*Dwax_old)/(1.0E-3*muOilf);
		Fi = ConstFi*BooleanFi + (1 - BooleanFi)*(1.0 - pow(Re_f,0.15)/8.0);
		porosoPar=Fi;
		flui.coefViscWax=1.;
		if(alteraViscFlu==1){
			double re1;
			double velSupGasUp=((MR-MliqiniR)/rgRi)/areaUp;
			double velSupGasDn=QG/areaDn;
			double velSupGas = (velSupGasUp + velSupGasDn)/2.0;
			if(posic==0)velSupGas = velSupGasUp;
			double j1=velSupGas+velSupLiq;
			double viscmix1=alf*migC+(1-alf)*viscLiq;
			if(duto.revest==0)re1 = Rey(duto.a, j1,  rhoMix,viscmix1);
			else{
				double dhid=4* duto.area/duto.peri;
				re1 = Rey(dhid, j1, rhoMix,viscmix1);
			}
			double f1 = fric(re1, duto.rug / duto.a);
			double tau_int=f1*rhoMix*j1*j1/2.;
			double dudr_int;
			dudr_int = tau_int/(1.0E-3*muOilf);
			/*double coefCalsep;
			flui.coefViscWax=coefCalsep = exp(DVisc*Fi) + EVisc*Fi/pow(dudr_int,0.5) + FVisc*pow(Fi,4.0)/dudr_int;
			if (dudr_int >= 10.0 && coefCalsep > 1.0){ // Obs: a correção ocorre somente na viscosidade do óleo e não é utilizada na viscosidade da fase líquida (viscLiq)
				muOil = coefCalsep*muOil; // Viscosity in Pa s, and shear rate in s^-1
				muOilf = coefCalsep*muOilf;
			}*/
			double coefPedRon;
			// Os multiplicadores D_multip, E_multip e F_multip são passados como 1 se o usuário do Marlim desejar usar os valores default de DVisc, EVisc e FVisc // Samuel - 10/10/25
			DVisc = D_multip*DVisc; // Samuel - 10/10/25
			EVisc = E_multip*EVisc; // Samuel - 10/10/25
			FVisc = F_multip*FVisc; // Samuel - 10/10/25
			flui.coefViscWax = coefPedRon = exp(DVisc*Fi) + EVisc*Fi/pow(dudr_int,0.5) + FVisc*pow(Fi,4.0)/dudr_int;
			if (dudr_int >= 10.0 && coefPedRon > 1.0){ // Obs: a correção ocorre somente na viscosidade do óleo e não é utilizada na viscosidade da fase líquida (viscLiq)
				muOil = coefPedRon*muOil; // Viscosity in Pa s, and shear rate in s^-1
				muOilf = coefPedRon*muOilf;
			}

		}
		muOil = MultipVisc*muOil;
		muOilf = MultipVisc*muOilf;


		double Tint=calor.Tcamada[0][0] + 273.15;
		double MwOil=flui.dInterpolatedLiqMWOutput;
		Dparaffin = MultipDwax*7.4E-12*Tint*pow(AssocParam*MwOil,0.5)/((muOilf*1e-3)*pow(Vwax,0.6));//imprimir nova saida
		for (int i = 0; i < comp; i++){
			Sum_dCwaxdT = Sum_dCwaxdT + flui.oInterpolatedWaxConcsTDerivOutput[i];
		}

		if (flowPattern == 0 || flowPattern == -1){
			Re_delta = rhoLiq*velOil*delta_old/(1.0E-3*viscLiq);} // rhoLiq está em kg/m^3? A dimensão deve ser kg/m^3.
		else if (flowPattern == 1 || flowPattern == 2){
			Re_delta = rhoMix*velSupLiq*delta_old/(alphaLiq*(1.0E-3*viscLiq));}
		else if (flowPattern == -2){
			Re_delta = pow(rhoMix*rhoLiq,0.5)*velSupLiq*delta_old/(alphaLiq*(1.0E-3*viscLiq));
		}

		double pi1, pi2;
		double C1;//, C2, C3;
		C1 = 15.0;
		C2 = BooleanPi*C2_star + (1-BooleanPi)*0.055;
		C3 = BooleanPi*C3_star + (1-BooleanPi)*1.4;
		pi1 = C1;
		pi2 = C2*pow(Re_delta,C3);

		// Obtain the deposit thickness, free flow diameter, specific mass of the solid phase and deposit, specific heat and conductivity of the deposit

		double rhoWaxSolid = 1.1*rhoWaxLiq;
		double ddeltadt;
		double heatFluxInt=-calor.fluxIni;
		double kMix;
		double condliq = (1. - bet) * flui.CondLiq(pres, temp) + bet * fluicol.CondLiq(pres, temp);
		double kOil=flui.CondOleo(pres, temp);
		kMix = condliq * (1 - alf) + flui.CondGas(pres, temp) * alf;

		//????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
		//????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
		// Neste ponto surge uma dúvida, considere que ainda existe uma certa fração de líquido na célula, isto,
		// pelo teste de entrada no cálculo do depósito vai nos levar até este ponto, se esta fração de óleo for muito pequena,
		// não existe o risco de se calcular uma massa depositada maior do que a quantiodade de óleo disponível na célula?
		// não se deve fazer um teste para esta condição? Considero que isto não é tão improvávcel em processos de parada de produção segregação
		// dos fluidos. NO FUTURO, AVALIAR CASOS DE PARADA DE PRODUÇÃO, SEGREGAÇÃO, ESCOAMENTO REVERSO, REDUÇÃO DA FRAÇÃO DE PARAFINA, ETC.

		//atencao!!!!!!!!! Coloquei em módulo, mas apenas para teste, algo está estranho espessura negativa!!!!!!!!!
		double dCwaxdTemp=Sum_dCwaxdT;
		if(Sum_dCwaxdT<0) dCwaxdTemp=0.;
		ddeltadt = (pi1/(1.0+pi2))*((rhoLiq*Fi)/(rhoWaxSolid*(1.0-Fi)))*(heatFluxInt/kMix)*Dparaffin*dCwaxdTemp;
		double novaVar=(pi1/(1.0+pi2))*((rhoLiq*Fi))*(heatFluxInt/kMix)*Dparaffin*dCwaxdTemp;//imprimir nova saida
		double delta =ddeltadt*dt;
		deltaPar=delta;

		double rhoDep = Fi*rhoOil + (1.0-Fi)*rhoWaxSolid;
		double cpDep = flui.dInterpolatedCPWaxOutput;

		double kDep = ((2*flui.dInterpolatedThermCondOutput + kOil +
						   (flui.dInterpolatedThermCondOutput - kOil)*Fi)/
						   (2*flui.dInterpolatedThermCondOutput + kOil - 2*(flui.dInterpolatedThermCondOutput - kOil)*Fi))*kOil;
		if(isnan(delta)){
			int para;
			para=0;
		}
		if(parafinado==0 && delta>0.){
			duto.atualizaCamada(delta, rug, cpDep, kDep, rhoDep);
			calor.atualiza(duto, 1);
			parafinado=1;
		}
		else if(delta>0){
			duto.atualizaCamada2(delta, cpDep, kDep, rhoDep);
			calor.atualiza2(duto);
		}
	}
}

FullMtx<double> Cel::Jacobiana(){
	 FullMtx<double> jac(3,3);

		double alfI;
		double betI;
		double vl;
		double vg;
		double rlmix;
		double cg2;
		if(QG>0)alfI=alfL;
		else alfI=alf;
		if(QL>0)betI=bet;
		else betI=bet;

		double tempMed;
		double raz;
		if(posic==0)tempMed=temp;
		else{
			raz=dxL/(dx+dxL);
			tempMed=(1.-raz)*tempL+raz*temp;
		}
		double kad=flui.ConstAdG(presaux, tempMed,rgCi);
		rlmix=rpCi*(1.-betI)+rcCi*betI;
		cg2=kad/flui.drhodp(presaux, tempMed);
		if(alfI<1-1e-15)vl=QL/(duto.area*(1.-alfI));
		else vl=0;
		if(alfI>1e-15)vg=QG/(duto.area*alfI);
		else vg=0.;
		double den=(1.-c0*alfI*(1.-rgCi/rlmix));
		double razDen=rgCi/rlmix;
		jac[0][0]=vg*c0*alfI*(rgCi/rlmix)/den;
		jac[0][1]=-vg*(1.-c0*alfI)/den;
		jac[0][2]=(1.-c0*alfI)/den;
		jac[1][0]=-vg*c0*alfI*(rgCi/rlmix)/den;
		jac[1][1]=vg*(1.-c0*alfI)/den;
		jac[1][2]=c0*alfI*(rgCi/rlmix)/den;
		if(alfI>1e-15){
			jac[2][0]=(rlmix*(cg2*razDen-vl*vl*alfI)+c0*alfI*(cg2*rgCi*(razDen-1.)+alfI*(-2.*vg*vg*rgCi+2.*vg*vl*rgCi+vl*vl*rlmix*(1-razDen))))/(alfI*rlmix*den);
			jac[2][1]=((cg2+vg*alfI*(vg-2.*vl))-c0*alfI*(cg2*(1.-razDen)+vg*alfI*(-2.*vl+vg*(razDen+1.))))/(alfI*den);
		}
		jac[2][2]=2.*(vl+c0*alfI*(vg*razDen-vl))/den;

	 return jac;
}

Vcr<double> Cel::autoVec(int ind,FullMtx<double> jac){
	 Vcr<double> autvec(3);

	 double autval;
	 if(ind==2){
			double alfI;
			double vg;
			if(QG>0)alfI=alfL;
			else alfI=alf;
			if(alfI>1e-15)vg=QG/(duto.area*alfI);
			else vg=0.;
			autval=vg;
	 }
	 else{
		 double velsom=somVel();
		 double term=termAdSomVel();
		 if(ind==0)autval=term+velsom;
		 else autval=term-velsom;
	 }

	 autvec[0]=1.;
	 Vcr<double> TL(2);
	 TL[0]=autval-jac[0][0];
	 TL[1]=-jac[1][0];

	 FullMtx<double> matResto(2,2);
	 matResto[0][0]=jac[0][1];
	 matResto[0][1]=jac[0][2];
	 matResto[1][0]=jac[1][1]-autval;
	 matResto[1][1]=jac[1][2];
	 matResto.GaussElimCP(TL);
	 autvec[1]=TL[0];
	 autvec[2]=TL[1];
	 return autvec;
}

Vcr<double> Cel::flutua(){
	 Vcr<double> estados(3);
	 FullMtx<double> jac(3,3);
	 FullMtx<double> matAutoVec(3,3);
	 jac=Jacobiana();

	 double alfI;
	 double betI;
	 if(QG>0)alfI=alfL;
	 else alfI=alf;
	 if(QL>0)betI=bet;
	 else betI=bet;

	 double tempMed;
	 double raz;
	 if(posic==0)tempMed=temp;
	 else{
		raz=dxL/(dx+dxL);
		tempMed=(1.-raz)*tempL+raz*temp;
	 }

	 double vl;
	 if(alfI<1-1e-15)vl=QL/(duto.area*(1.-alfI));
	 else vl=0;
	 double vg;
	 if(alfI>1e-15)vg=QG/(duto.area*alfI);
	 else vg=0.;
	 double rlmix=rpCi*(1.-betI)+rcCi*betI;

	 estados[0]=(1.-alfI)*rlmix;
	 estados[1]=alfI*rgCi;
	 estados[2]=(1.-alfI)*rlmix*vl+alfI*rgCi*vg;

	 for(int i=0; i<3;i++){
		 Vcr<double> autvec(3);
		 autvec=autoVec(i,jac);
		 for(int j=0; j<3; j++){
			 matAutoVec[j][i]=autvec[j];
		 }
	 }

	 matAutoVec.GaussElimCP(estados);

	 return estados;
}

//template class Cel;
