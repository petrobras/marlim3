#include "celulaGas.h"

CelG::CelG(const DadosGeo vdutoL,const DadosGeo vduto,
		const DadosGeo vdutoR, const ProFlu vflui,
		const ChokeGas vchkcell,
		const double vtempL,const double vtemp,const double vtempR,
		const double vpresL, const double vpres,const double vpresR,
		const double vVGasL,const double vVGasR, const double vVGasRR,
		const double vu1L, const double vu1R, const double vu1LL, const double vdx0,
		const double vdx1,const double vdxL, const double vdt, const int vposic,const int vfecham
		,const int vtipoCC,const TransCal vcalor) :
		TL(3), local(3,9) { //construtor default


	//Solver de Hidratos

	// --- Solver de Hidratos: inicialização ---
	// fluxos em massa (por área) – começam em zero
	j_H = 0.0L;
	j_G = 0.0L;
	j_W = 0.0L;

	// volumes físicos
	V_h = 0.0L;   // sem hidrato no início
	V_w = 0.0L;   // será atribuído em solverHidrato() usando BSW

	FVHG = 0.0L;
	agua_consumida=0.;
	massa_hidrato=0.0L; //chris - Hidratos

	dutoL= vdutoL;
	duto = vduto;
	dutoR = vdutoR;
	flui = vflui;
	calor=vcalor;
	tempL = vtempL;
	temp = vtemp;
	tempR = vtempR;
	presL=vpresL;
	presini=pres = vpres;
	presR = vpresR;
	dx0 = vdx0;
	dx1 = vdx1;
	dxL=vdxL;
	dt = vdt;
	dTdt=0.;
	posic = vposic;
	VGasL = vVGasL;
	VGasR = vVGasR;
	VGasRBuf = vVGasR;
	VGasRR = vVGasRR;
	rg=flui.MasEspGas(pres, temp);
	rgR=rg;
	u1L = rg* duto.area;
	u1R = vu1R;
	u1LL=vu1LL;
	chkcell=vchkcell;
	labelchk=0;
	massfonteCH=0;
	rpchk=0.6;
	fluxcal=0.;
	fechamon=vfecham;
	tipoCC=vtipoCC;

	fonteM2=0.;
	salinidade=0.;//alteracao2
	//posicLiq=-1000.;
	celInter=0;//alteracao2
	razInter=1.;//alteracao2
	razInterIni=1.;//alteracao2
	//dtInter=0.;
	//velInter=0.;
	//deriLM2=0.;
	//deriCM2=0.;
	//deriRM2=0.;

    tempLini=tempL;
    tempini=temp;
    tempRini=tempR;
    presLini=presL;
    presini=pres;
    presRini=presR;
    VGasLini=VGasL;
    VGasRini=VGasR;
    VGasRRini=VGasRR;
    u1LLini=u1LL;
    u1Lini=u1L;
    u1Rini=u1R;
    massfonteCHini=massfonteCH;
    fonteM2ini=fonteM2;
    fechamonini=fechamon;
   // razInterIni0=razInterIni;
    celInterini=celInter;//alteracao2
    posicini=posic;
    rpchkini=rpchk;
    vgl=0;
    tEstag=0;
    pEstag=0;
    tGarg=0;
    pGarg=0;
    qGarg=0;
    areaGarg=0;

    indGeom=0;

    dPdLHidro=1.;
    dPdLFric=1.;
    dTdLCor=1.;
    inddPdLHidro=-1;
    inddPdLFric=-1;
    inddTdLCor=-1;

    termoHidro=0.;
    termoFric=0.;

}

CelG::CelG(const CelG& vcel) :
		TL(3), local(3, 9) { //construtor por c�pia


	//Solver de Hidratos
	j_H=vcel.j_H;
	j_G=vcel.j_G;
	j_W=vcel.j_W;
	V_h=vcel.V_h;
	V_w=vcel.V_w;
	FVHG=vcel.FVHG;
	agua_consumida=vcel.agua_consumida;
	massa_hidrato=vcel.massa_hidrato; //chris - Hidratos


	dutoL = vcel.dutoL;
	duto = vcel.duto;
	dutoR = vcel.dutoR;
	flui = vcel.flui;
	calor=vcel.calor;
	tempL = vcel.tempL;
	temp = vcel.temp;
	tempR = vcel.tempR;
	presL=vcel.presL;
	pres = vcel.pres;
	presini = vcel.presini;
	presR = vcel.presR;
	dx0 = vcel.dx0;
	dx1 = vcel.dx1;
	dxL=vcel.dxL;
	dt = vcel.dt;
	dTdt=vcel.dTdt;
	posic = vcel.posic;
	VGasL = vcel.VGasL;
	VGasR = vcel.VGasR;
	VGasRBuf = vcel.VGasRBuf;
	VGasRR = vcel.VGasRR;
	rg=vcel.rg;
	rgR=vcel.rgR;
	u1L = vcel.u1L;
	u1R = vcel.u1R;
	u1LL=vcel.u1LL;
	TL=vcel.TL;
	local=vcel.local;
	chkcell=vcel.chkcell;
	labelchk=vcel.labelchk;
	massfonteCH=vcel.massfonteCH;
	rpchk=vcel.rpchk;
	fluxcal=vcel.fluxcal;
	fechamon=vcel.fechamon;
	tipoCC=vcel.tipoCC;

	fonteM2=vcel.fonteM2;
	salinidade=vcel.salinidade;//alteracao2
	//posicLiq=vcel.posicLiq;
	celInter=vcel.celInter;//alteracao2
	razInter=vcel.razInter;//alteracao2
	razInterIni=vcel.razInterIni;//alteracao2
	//dtInter=vcel.dtInter;
	//velInter=vcel.velInter;
	//deriLM2=vcel.deriLM2;
	//deriCM2=vcel.deriCM2;
	//deriRM2=vcel.deriRM2;
    tempLini=vcel.tempLini;
    tempini=vcel.tempini;
    tempRini=vcel.tempRini;
    presLini=vcel.presLini;
    presini=vcel.presini;
    presRini=vcel.presRini;
    VGasLini=vcel.VGasLini;
    VGasRini=vcel.VGasRini;
    VGasRRini=vcel.VGasRRini;
    u1LLini=vcel.u1LLini;
    u1Lini=vcel.u1Lini;
    u1Rini=vcel.u1Rini;
    massfonteCHini=vcel.massfonteCHini;
    fonteM2ini=vcel.fonteM2ini;
    fechamonini=vcel.fechamonini;
    //razInterIni0=vcel.razInterIni0;
    celInterini=vcel.celInterini;//alteracao2
    posicini=vcel.posicini;
    rpchkini=vcel.rpchkini;
    vgl=vcel.vgl;
    tEstag=vcel.tEstag;
    pEstag=vcel.pEstag;
    tGarg=vcel.tGarg;
    pGarg=vcel.pGarg;
    qGarg=vcel.qGarg;
    areaGarg=vcel.areaGarg;

    indGeom=vcel.indGeom;

    dPdLHidro=vcel.dPdLHidro;
    dPdLFric=vcel.dPdLFric;
    dTdLCor=vcel.dTdLCor;
    inddPdLHidro=vcel.inddPdLHidro;
    inddPdLFric=vcel.inddPdLFric;
    inddTdLCor=vcel.inddTdLCor;

    termoHidro=vcel.termoHidro;
    termoFric=vcel.termoFric;

}

CelG& CelG::operator =(const CelG& vcel) {
	if (this != &vcel) {

		//Solver de Hidratos
		j_H=vcel.j_H;
		j_G=vcel.j_G;
		j_W=vcel.j_W;
		V_h=vcel.V_h;
		V_w=vcel.V_w;
		FVHG=vcel.FVHG;
		agua_consumida=vcel.agua_consumida;
		massa_hidrato=vcel.massa_hidrato; //chris - Hidratos

		dutoL = vcel.dutoL;
		duto = vcel.duto;
		dutoR = vcel.dutoR;
		flui = vcel.flui;
		calor=vcel.calor;
		tempL = vcel.tempL;
		temp = vcel.temp;
		tempR = vcel.tempR;
		presL=vcel.presL;
		pres = vcel.pres;
		presini = vcel.presini;
		presR = vcel.presR;
		dx0 = vcel.dx0;
		dx1 = vcel.dx1;
		dxL=vcel.dxL;
		dt = vcel.dt;
		dTdt=vcel.dTdt;
		posic = vcel.posic;
		VGasL = vcel.VGasL;
		VGasR = vcel.VGasR;
		VGasRBuf = vcel.VGasRBuf;
		VGasRR = vcel.VGasRR;
		rg=vcel.rg;
		rgR=vcel.rgR;
		u1L = vcel.u1L;
		u1R = vcel.u1R;
		u1LL=vcel.u1LL;
		chkcell=vcel.chkcell;
		labelchk=vcel.labelchk;
		massfonteCH=vcel.massfonteCH;
		rpchk=vcel.rpchk;
		fluxcal=vcel.fluxcal;
		fechamon=vcel.fechamon;
		tipoCC=vcel.tipoCC;

		fonteM2=vcel.fonteM2;
		salinidade=vcel.salinidade;//alteracao2
		//posicLiq=vcel.posicLiq;
		celInter=vcel.celInter;//alteracao2
		razInter=vcel.razInter;//alteracao2
		razInterIni=vcel.razInterIni;//alteracao2
		//dtInter=vcel.dtInter;
		//velInter=vcel.velInter;
		//deriLM2=vcel.deriLM2;
		//deriCM2=vcel.deriCM2;
		//deriRM2=vcel.deriRM2;
		   tempLini=vcel.tempLini;
		    tempini=vcel.tempini;
		    tempRini=vcel.tempRini;
		    presLini=vcel.presLini;
		    presini=vcel.presini;
		    presRini=vcel.presRini;
		    VGasLini=vcel.VGasLini;
		    VGasRini=vcel.VGasRini;
		    VGasRRini=vcel.VGasRRini;
		    u1LLini=vcel.u1LLini;
		    u1Lini=vcel.u1Lini;
		    u1Rini=vcel.u1Rini;
		    massfonteCHini=vcel.massfonteCHini;
		    fonteM2ini=vcel.fonteM2ini;
		    fechamonini=vcel.fechamonini;
		    //razInterIni0=vcel.razInterIni0;
		    celInterini=vcel.celInterini;//alteracao2
		    posicini=vcel.posicini;
		    rpchkini=vcel.rpchkini;
		    vgl=vcel.vgl;
		    tEstag=vcel.tEstag;
		    pEstag=vcel.pEstag;
		    tGarg=vcel.tGarg;
		    pGarg=vcel.pGarg;
		    qGarg=vcel.qGarg;
		    areaGarg=vcel.areaGarg;

		    indGeom=vcel.indGeom;

		    dPdLHidro=vcel.dPdLHidro;
		    dPdLFric=vcel.dPdLFric;
		    dTdLCor=vcel.dTdLCor;
		    inddPdLHidro=vcel.inddPdLHidro;
		    inddPdLFric=vcel.inddPdLFric;
		    inddTdLCor=vcel.inddTdLCor;

		    termoHidro=vcel.termoHidro;
		    termoFric=vcel.termoFric;
	}
	return *this;
}

double CelG::Rey(double dia, double vel,
		double rho, double vis) {
	return dia * fabsl(vel) * rho / (vis * 1e-3);
}
double CelG::fric(double re, double eps) {
/*	double val;
	if (fabsl(re) > 1e-5) {
		if (re > 2400) {
			val = 6.9 / fabsl(re) + pow(eps / 3.7, 1.11);
			val = -1.8 * (logl(val) / logl(10.));
			val = pow(1 / val,2.)/4.;
		} else
			val = 16. / fabsl(re);
	} else
		val = 0.;
	return val;*/
	double val;
	   if (fabsl(re) > 1e-5) {
			if (re > 2400) {
				val = 6.9 / fabsl(re) + pow(eps / 3.7, 1.11);
				val = -1.8 * (logl(val) / logl(10.));
				val = pow(1 / val,2.);
				for(int konta=0;konta<2;konta++){
				  val=pow(-2.*logl( 2.51 / fabsl(re*sqrtl(val)) + eps/ 3.7)/logl(10.),2.);
				  val=1./val;
				}
				val/=4.;
			} else
				val = 16. / fabsl(re);
		} else
			val = 0.;
	   return val;
}

double  CelG::MasEspFlu(double pres, double temper) const{// alteracao2

	  double tfarAmb=Faren(20.);
	  double ppsiAmb=psia(1.);
	  double tfar=Faren(temper);
	  double ppsi=psia(pres);
	  double bwAmb = 1.0 + 1.2e-04*(tfarAmb - 60.0) + 1.0e-06*pow(tfarAmb - 60.0,2.) - 3.33e-06*ppsiAmb;
	  double bw = 1.0 + 1.2e-04*(tfar - 60.0) + 1.0e-06*pow(tfar - 60.0,2.) - 3.33e-06*ppsi;

	  double rholw=(1000./bw)*bwAmb;
	  double rhosal=2160.;
	  double x=salinidade/1000.;
	  double rhomist=(1-x)/rholw+x/rhosal;
	  return 1./rhomist;

}// alteracao2
double CelG::VisFlu(double pres, double temper) const{//alteracao2

	  double vis= 2.414E-05*pow(10.0,247.8/(temper + 133.15));
	  return vis*1000.;

}//alteracao2

double CelG::CondLiq(double pres, double temper) const{//alteracao2

		return 5.64981e-01 + (1.75148e-03 - 6.21225e-06*temper)*temper;
}//alteracao2

double CelG::CalorLiq(double pres, double temper) const{//alteracao2

		double ValCp;
		if (temper< 136.85)
		  ValCp= 4185.5*(1.00536 + (-4.31108e-04 + (7.01150e-06 - 2.42139e-08*temper)*temper)*temper);
		 else
		  ValCp= 4185.5*(0.189300 + (1.30609e-02 + (-6.89620e-05 + 1.29156e-07*temper)*temper)*temper);
		return ValCp;
}//alteracao2

double CelG::DrhoDtFlu(double pres, double temper) const{//alteracao2

	 return 0.;

}//alteracao2

/*void CelG::GeraLocal(int ncelGas,double presiniG,double tempiniG) {


	  if (posic != 0 && posic != ncelGas) {

		    double varpresL=0.;
		    double varpresR=0.;
		  if(posic<(*celInter)){//alteracao2
		    local[0][0] = 0.;
		    local[0][1] = -1 / dx0;
		    local[0][2] = 0.;
		    local[0][3] = 1 / dt;
		    local[0][4] = 1 / dx0;
		    local[0][5] = 0.;
		    local[0][6] = 0.;
		    local[0][7] = 0.;
		    local[0][8] = 0.;

		    TL[0] = u1L / dt-massfonteCH / dx0+fonteM2/dx0;
		  }//alteracao2
		  else{//alteracao2
			    local[0][0] = 0.;
			    local[0][1] = 0.;
			    local[0][2] = 0.;
			    local[0][3] = 1.;
			    local[0][4] =0.;
			    local[0][5] = 0.;
			    local[0][6] = 0.;
			    local[0][7] = 0.;
			    local[0][8] = 0.;

			    TL[0] = u1L;

		  }//alteracao2
		  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  if(posic<((*celInter)-1)){//alteracao2
		     double dxmed = 0.5 * (dx0 + dx1);
		     double pmed = 0.5 * (pres + presR);
		     double tmed = 0.5 * (temp + tempR);
		     double Amed = 0.5 * (duto.area + dutoR.area);
		     double rhomed = 0.5 * (u1L / duto.area + u1R / dutoR.area);
		     double compres = flui.Zdran(pmed, tmed);//alteracao2
		     double compres1 = flui.Zdran(pres, temp);//alteracao2
		     double compres2 = flui.Zdran(presR, tempR);//alteracao2
		     double RUni=(8.0465*1000)/(flui.Deng*28.9625);//alteracao2
		     //double dzdp = flui.DZDP(pmed, tmed);//alteracao2
		     //double dzdt = flui.DZDT(pmed, tmed);//alteracao2
		     //double dpdrho = rhomed* (1 / (pmed * 98066.5) -  dzdp/ (compres) );//alteracao2
		     //dpdrho = 1. / dpdrho;//alteracao2
		     //double drhodT=-rhomed*(1./(tmed+273.1)+(1/compres)*dzdt);//alteracao2
		     double VGasmedL =0.5 * (VGasR + VGasL);
		     double VGasmedR = 0.5 * (VGasR + VGasRR);
		     double vel1 = VGasmedL / u1L;
		     double vel2 = VGasmedR / u1R;

		     double rho1 = u1L / duto.area;
		     double rho2 = u1R / dutoR.area;
		     double re1;
		     double re2;

		     if(duto.revest==0)re1 = Rey(duto.a, vel1, u1L / duto.area,
							         flui.ViscGas(pres, temp));
		     else{
			      double dhid=4* duto.area/duto.peri;
			      re1 = Rey(dhid, vel1, u1L / duto.area,
					    flui.ViscGas(pres, temp));
		      }
		      if(dutoR.revest==0)re2 = Rey(dutoR.a, vel2, u1R / dutoR.area,
							              flui.ViscGas(presR, tempR));
		      else{
			        double dhid=4* dutoR.area/dutoR.peri;
			        re2 = Rey(dhid, vel2, u1R / dutoR.area,
					       flui.ViscGas(presR, tempR));
		      }
		      double f1 = fric(re1, duto.rug / duto.a);
		      double f2 = fric(re2, dutoR.rug / dutoR.a);
		      double razdx = dx0 / (dx1 + dx0);
		      double multvelL=0.5 * f1*( fabsl(vel1)/duto.area) * duto.peri * razdx;
		      double multvelR=0.5 * f2*( fabsl(vel2)/dutoR.area) * dutoR.peri * (1.-razdx);
		      varpresL=1*( 9.82 * sinl(duto.teta) * dx0) /( (dx1 + dx0));
		      varpresR=1*( 9.82 * sinl(dutoR.teta) * dx1) / ((dx1 + dx0));

		      local[1][0] = 0.;
		      local[1][1] = -vel1 / (2. * dxmed)+0.5*multvelL;
		      local[1][2] = 0.;
		      //local[1][3] = -(Amed * dpdrho * (1 / duto.area) * (1 / dxmed))+varpresL;//alteracao2
		      local[1][3] = -(Amed * (compres1*RUni*(temp+273)) * (1 / duto.area) * (1 / dxmed))+varpresL;//alteracao2
		      local[1][4] =1 / dt +(vel2 - vel1) / (2. * dxmed)+0.5*(multvelL+multvelR);
		      //local[1][5] =-0*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		      local[1][5] =0.;//alteracao2
		      //local[1][6] = (Amed * dpdrho * (1 / dutoR.area) * (1 / dxmed))+varpresR;
		      local[1][6] = (Amed * (compres2*RUni*(tempR+273)) * (1 / dutoR.area) * (1 / dxmed))+varpresR;//alteracao2
		      local[1][7] = vel2 / (2. * dxmed)+0.5*multvelR;
		      //local[1][8] = 0*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		      local[1][8] = 0.;

		      TL[1] = VGasR / dt;
          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

              double areamenor=duto.area;
              if(areamenor>dutoR.area)areamenor=dutoR.area;
		      //if(labelchk==1 && chkcell.areagarg<1e-5*areamenor){
              if(labelchk==1 && chkcell.areagarg<=0.0001*areamenor){
			     local[1][0] = 0.;
			     local[1][1] = 0.;
			     local[1][2] = 0.;
			     local[1][3] = 0.;
			     local[1][4] = 1.;
			     local[1][5] = 0.;
			     local[1][6] = 0.;
			     local[1][7] = 0.;
			     local[1][8] = 0.;


			     TL[1] = 0.;
			     rpchk=0.6;
			     //tet=1;
		     }
		  }//alteracao2
		  else{//alteracao2
		     local[1][0] = 0.;
		     local[1][1] = 0.;
		     local[1][2] = 0.;
		     local[1][3] = 0.;
		     local[1][4] = 1.;
		     local[1][5] = 0.;
		     local[1][6] = 0.;
		     local[1][7] = 0.;
		     local[1][8] = 0.;


		     TL[1] = VGasR;
	     }//alteracao2


		  local[2][0]=0.;
		  local[2][1]=0.;
		  local[2][2]=0.;
		  local[2][3]=0.;
		  local[2][4]=0.;
		  local[2][5]=1.;
		  local[2][6]=0.;
		  local[2][7]=0.;
		  local[2][8]=0.;
		  TL[2]=temp;

	  }
	  else if (posic == 0) {
		    if(tipoCC==0){
			  local[0][0] = 0.;
			  local[0][1] = 0.;
			  local[0][2] = 0.;
			  local[0][3] = 1.;
			  local[0][4] = 0.;
			  local[0][5] = 0.;
			  local[0][6] = 0.;
			  local[0][7] = 0.;
			  local[0][8] = 0.;

			  TL[0] = flui.MasEspGas(presiniG, tempiniG) * dutoR.area;
		    }
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		    else if(tipoCC==1){
				  local[0][0] = 0.;
				  local[0][1] = 0;
				  local[0][2] = 0.;
				  local[0][3] = 1/dt;
				  local[0][4] = 1/dx0;
				  local[0][5] = 0.;
				  local[0][6] = 0.;
				  local[0][7] = 0.;
				  local[0][8] = 0.;

				  TL[0] = u1L / dt+massfonteCH / dx0;
		    }
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  double dxmed = 0.5 * dx1;
		  double pmed = pres;
		  //double tetmed=dutoR.teta;
		  double Amed = dutoR.area;
		  double rhomed = u1L / dutoR.area;//0.5 * (u1L / dutoR.area + u1R / dutoR.area);
		  double compres = flui.Zdran(pmed, temp);
		  double compres1 = flui.Zdran(pres, temp);//alteracao2
		  double compres2 = flui.Zdran(presR, tempR);//alteracao2
		  double RUni=(8.0465*1000)/(flui.Deng*28.9625);//alteracao2
		  //double dzdp = flui.DZDP(pmed, temp);//alteracao2
		  //double dzdt = flui.DZDT(pmed, temp);//alteracao2
		  //double dpdrho = rhomed* (1 / (pmed * 98066.5) - (1 / compres) * dzdp);//alteracao2
		  //dpdrho = 1. / dpdrho;//alteracao2
		  //double drhodT=-rhomed*(1./(temp+273.1)+(1/compres)*dzdt);//alteracao2
		  double VGasmedL = VGasR;
		  double VGasmedR = 0.5 * (VGasR + VGasRR);
		  double vel1 = VGasmedL / u1L;
		  double vel2 = VGasmedR / u1R;

		  double rho1 = u1L / dutoR.area;

		  double re1;
		  if(dutoR.revest==0)re1 = Rey(dutoR.a, vel1, u1L / dutoR.area,
				  flui.ViscGas(pres, temp));
				  else{
					  double dhid=4* dutoR.area/dutoR.peri;
					  re1 =Rey(dhid, vel1, u1L / dutoR.area,
							flui.ViscGas(pres, temp));
				  }

		  double f1 = fric(re1, dutoR.rug / dutoR.a);
		  double multvelL=0.5 * f1*( fabsl(vel1)/dutoR.area) * dutoR.peri;

		  local[1][0] = 0.;
		  local[1][1] = 0.;
		  local[1][2] = 0.;
		  //local[1][3] = -(Amed * dpdrho * (1 / dutoR.area) * (1 / dxmed))+1*9.82 * sinl(duto.teta);//alteracao2
		  local[1][3] = -(Amed * (compres1*RUni*(temp+273)) * (1 / duto.area) * (1 / dxmed));//alteracao2
		  local[1][4] = 1 / dt + (vel2 - 2. * vel1) / (2. * dxmed)+multvelL;
		  //local[1][5] =-0*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		  local[1][5] =0.;//alteracao2
		  //local[1][6] = (Amed * dpdrho * (1 / dutoR.area) * (1 / dxmed));//alteracao2
		  local[1][6] = (Amed * (compres2*RUni*(tempR+273)) * (1 / dutoR.area) * (1 / dxmed));//alteracao2
		  local[1][7] = vel2 / (2. * dxmed);
		  //local[1][8] = 0*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		  local[1][8] = 0.;//alteracao2

		  TL[1] = VGasR / dt;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  local[2][0]=0.;
		  local[2][1]=0.;
		  local[2][2]=0.;
		  local[2][3]=0.;
		  local[2][4]=0.;
		  local[2][5]=1.;
		  local[2][6]=0.;
		  local[2][7]=0.;
		  local[2][8]=0.;
		  TL[2]=tempiniG;



	  } else if (posic == ncelGas) {
		  if(posic<(*celInter)){//alteracao2
		     local[0][0] = 0.;
		     local[0][1] = -1 / dx0;
		     local[0][2] = 0.;
		     local[0][3] = 1 / dt;
		     local[0][4] = 1 / dx0;
		     local[0][5] = 0.;
		     local[0][6] = 0.;
		     local[0][7] = 0.;
		     local[0][8] = 0.;

		   //TL[0] = u1L / dt - 1. * massfonte / dx0;
		     TL[0] = u1L / dt - 1. * massfonteCH / dx0;
		  }//alteracao2
		  else{//alteracao2
			    local[0][0] = 0.;
			    local[0][1] = 0.;
			    local[0][2] = 0.;
			    local[0][3] = 1.;
			    local[0][4] =0.;
			    local[0][5] = 0.;
			    local[0][6] = 0.;
			    local[0][7] = 0.;
			    local[0][8] = 0.;

			    TL[0] = u1L;
		  }//alteracao2
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  local[1][0] = 0.;
		  local[1][1] = 0.;
		  local[1][2] = 0.;
		  local[1][3] = 0.;
		  local[1][4] = 1.;
		  local[1][5] = 0.;
		  local[1][6] = 0.;
		  local[1][7] = 0.;
		  local[1][8] = 0.;

		  TL[1] = 0.;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		  local[2][0]=0.;
		  local[2][1]=0.;
		  local[2][2]=0.;
		  local[2][3]=0.;
		  local[2][4]=0.;
		  local[2][5]=1.;
		  local[2][6]=0.;
		  local[2][7]=0.;
		  local[2][8]=0.;
		  TL[2]=temp;


	  }

}*/

void CelG::GeraLocal(int ncelGas,double presiniG,double tempiniG
		                                  ,double abertura) {


	double drhodp;
	double drhodt=flui.drhodt(pres,temp);
	//double rhog = flui.MasEspGas(pres,temp);
	double rhog = rg;
	double compres=flui.Zdran(pres, temp,rg);
	double dzdp = flui.DZDP(pres, temp);
	drhodp = rhog* (1 / (pres * 98066.5) - (1 / compres) * dzdp);
	/*double drhodpR;
	double rhogR = flui.MasEspGas(presR,tempR);
	double compresR=flui.Zdran(presR, tempR);
	double dzdpR = flui.DZDP(presR, tempR);
	drhodpR = rhogR* (1 / (presR * 98066.5) - (1 / compresR) * dzdpR);*/
	  if (posic != 0 && posic != ncelGas) {

		    double varpresL=0.;
		    double varpresR=0.;
		  if(posic<(*celInter)){//alteracao2

		    local[0][0] = 0.;
		    local[0][1] = -1 / dx0;
		    local[0][2] = 0.;
		    local[0][3] = drhodp *duto.area*98066.5/ dt;
		    local[0][4] = 1 / dx0;
		    local[0][5] = 0.;
		    local[0][6] = 0.;
		    local[0][7] = 0.;
		    local[0][8] = 0.;

		    TL[0] = drhodp *duto.area*pres*98066.5 / dt-massfonteCH / dx0+fonteM2/dx0-drhodt*duto.area*dTdt;

		  }//alteracao2
		  else{//alteracao2
			    local[0][0] = 0.;
			    local[0][1] = 0.;
			    local[0][2] = 0.;
			    local[0][3] = 1.;
			    local[0][4] =0.;
			    local[0][5] = 0.;
			    local[0][6] = 0.;
			    local[0][7] = 0.;
			    local[0][8] = 0.;

			    TL[0] = pres;

		  }//alteracao2
		  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  if(posic<((*celInter)-1)){//alteracao2
		     double dxmed = 0.5 * (dx0 + dx1);
		     //double pmed = 0.5 * (pres + presR);
		     //double tmed = 0.5 * (temp + tempR);
		     double pmed = pres;
		     double tmed = temp;
		     if(VGasR<0.){
		    	 pmed = presR;
		    	 tmed = tempR;
		     }
		     double Amed = 0.5 * (duto.area + dutoR.area);
		     //double rhomed = 0.5 * (u1L / duto.area + u1R / dutoR.area);
		     //double compres = flui.Zdran(pmed, tmed);//alteracao2
		     //double compres1 = flui.Zdran(pres, temp,rg);//alteracao2
		     //double compres2 = flui.Zdran(presR, tempR,rgR);//alteracao2
		     double RUni=(8.0465*1000)/(flui.Deng*28.9625);//alteracao2

		     double VGasmedL =0.5 * (VGasR + VGasL);
		     double VGasmedR = 0.5 * (VGasR + VGasRR);
		     double vel1 = VGasmedL / u1L;
		     double vel2 = VGasmedR / u1R;
		     double u1med=u1L;
		     if(VGasR<0.)u1med=u1R;
		     double vel0=VGasR/u1med;

		     double rho1 = u1L / duto.area;
		     double rho2 = u1R / dutoR.area;
		     double re1;
		     double re2;

		     if(duto.revest==0)re1 = Rey(duto.a, vel1, u1L / duto.area,
							         flui.ViscGas(pres, temp));
		     else{
			      double dhid=4* duto.area/duto.peri;
			      re1 = Rey(dhid, vel1, u1L / duto.area,
					    flui.ViscGas(pres, temp));
		      }
		      if(dutoR.revest==0)re2 = Rey(dutoR.a, vel2, u1R / dutoR.area,
							              flui.ViscGas(presR, tempR));
		      else{
			        double dhid=4* dutoR.area/dutoR.peri;
			        re2 = Rey(dhid, vel2, u1R / dutoR.area,
					       flui.ViscGas(presR, tempR));
		      }
		      double f1 = fric(re1, duto.rug / duto.a);
		      double f2 = fric(re2, dutoR.rug / dutoR.a);
		      double razdx = dx0 / (dx1 + dx0);
		      //double multvelL=dPdLFric*0.5 * f1*( fabsl(vel1)/duto.area) * duto.peri * razdx;
		      double multvelL=dPdLFric*0.5 * f1*( fabsl(vel1)/duto.area)* duto.peri * razdx;
		      //double multvelR=dPdLFric*0.5 * f2*( fabsl(vel2)/dutoR.area) * dutoR.peri * (1.-razdx);
		      double multvelR=dPdLFric*0.5 * f2*( fabsl(vel2)/dutoR.area) * dutoR.peri * (1.-razdx);
		      varpresL=dPdLHidro*( 9.82 * sinl(duto.teta) * dx0*u1L) /( (dx1 + dx0));
		      varpresR=dPdLHidro*( 9.82 * sinl(dutoR.teta) * dx1*u1R) / ((dx1 + dx0));

		      termoFric=0.5 * f1*( fabsl(vel1)*vel1/duto.area) * duto.peri * razdx*(u1L/duto.area)+
		    		  0.5 * f2*( fabsl(vel2)*vel2/dutoR.area) * dutoR.peri * (1.-razdx)*(u1R/dutoR.area);
		      termoHidro=( 9.82 * sinl(duto.teta) * dx0*(u1L/duto.area)) /( (dx1 + dx0))+
		    		  ( 9.82 * sinl(dutoR.teta) * dx1*(u1R/dutoR.area)) / ((dx1 + dx0));

		      local[1][0] = 0.;
		      local[1][1] = -vel1 / (2. * dxmed)+0.5*multvelL;
		      local[1][2] = 0.;

		      //local[1][3] = -(Amed * (compres1*RUni*(temp+273)) * (1 / duto.area) * (1 / dxmed))+varpresL;//alteracao2
		      local[1][3] = -Amed * 98066.5* (1 / dxmed);//alteracao2

		      local[1][4] =1 / dt +(vel2 - vel1) / (2. * dxmed)+0.5*(multvelL+multvelR);
		      //local[1][5] =-1*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		      //local[1][5] =-1*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		      //local[1][6] = (Amed * dpdrho * (1 / dutoR.area) * (1 / dxmed))+varpresR;
		      local[1][6] = Amed * 98066.5* (1 / dxmed);//alteracao2
		      //local[1][6] = (Amed * (dzdp*RUni*(tmed+273)) * (1 / dutoR.area) * (1 / dxmed))+varpresR;//alteracao2
		      local[1][7] = vel2 / (2. * dxmed)+0.5*multvelR;
		      //local[1][8] = 0*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		      local[1][8] = 0.;


		      TL[1] = VGasR / dt-(varpresL+varpresR);
          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

              double areamenor=duto.area;
              if(areamenor>dutoR.area)areamenor=dutoR.area;
		      //if(labelchk==1 && chkcell.areagarg<1e-5*areamenor){
              if(labelchk==1 && chkcell.areagarg<=0.0001*areamenor){
			     local[1][0] = 0.;
			     local[1][1] = 0.;
			     local[1][2] = 0.;
			     local[1][3] = 0.;
			     local[1][4] = 1.;
			     local[1][5] = 0.;
			     local[1][6] = 0.;
			     local[1][7] = 0.;
			     local[1][8] = 0.;


			     TL[1] = 0.;
			     rpchk=0.6;
			     //tet=1;
		     }
		  }//alteracao2
		  else{//alteracao2
		     local[1][0] = 0.;
		     local[1][1] = 0.;
		     local[1][2] = 0.;
		     local[1][3] = 0.;
		     local[1][4] = 1.;
		     local[1][5] = 0.;
		     local[1][6] = 0.;
		     local[1][7] = 0.;
		     local[1][8] = 0.;


		     TL[1] = VGasR;
	     }//alteracao2


		  local[2][0]=0.;
		  local[2][1]=0.;
		  local[2][2]=0.;
		  local[2][3]=0.;
		  local[2][4]=0.;
		  local[2][5]=1.;
		  local[2][6]=0.;
		  local[2][7]=0.;
		  local[2][8]=0.;
		  TL[2]=temp;

	  }
	  else if (posic == 0) {
		    if(tipoCC==0 && abertura>=0.2){
			  local[0][0] = 0.;
			  local[0][1] = 0.;
			  local[0][2] = 0.;
			  local[0][3] = 1.;
			  local[0][4] = 0.;
			  local[0][5] = 0.;
			  local[0][6] = 0.;
			  local[0][7] = 0.;
			  local[0][8] = 0.;

			  //TL[0] = flui.MasEspGas(presiniG, tempiniG) * dutoR.area;
			  TL[0] = presiniG;
		    }
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		    else if(tipoCC==1 || abertura<0.2){

				  local[0][0] = 0.;
				  local[0][1] = 0;
				  local[0][2] = 0.;
				  local[0][3] = drhodp*duto.area*98066.5/dt;
				  local[0][4] = 1/dx0;
				  local[0][5] = 0.;
				  local[0][6] = 0.;
				  local[0][7] = 0.;
				  local[0][8] = 0.;

				  TL[0] = drhodp*duto.area*pres*98066.5 / dt+massfonteCH / dx0-drhodt*duto.area*dTdt;
		    }
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  double dxmed = 0.5 * dx1;
		  double pmed = pres;
		  //double tetmed=dutoR.teta;
		  double Amed = dutoR.area;
		  //double rhomed = u1L / dutoR.area;//0.5 * (u1L / dutoR.area + u1R / dutoR.area);
		  //double compres = flui.Zdran(pmed, temp);
		  //double compres1 = flui.Zdran(pres, temp,rg);//alteracao2
		  //double compres2 = flui.Zdran(presR, tempR,rgR);//alteracao2
		  double RUni=(8.0465*1000)/(flui.Deng*28.9625);//alteracao2
		  //double dzdp = flui.DZDP(pmed, temp);//alteracao2
		  //double dzdt = flui.DZDT(pmed, temp);//alteracao2
		  //double dpdrho = rhomed* (1 / (pmed * 98066.5) - (1 / compres) * dzdp);//alteracao2
		  //dpdrho = 1. / dpdrho;//alteracao2
		  //double drhodT=-rhomed*(1./(temp+273.1)+(1/compres)*dzdt);//alteracao2
		  double VGasmedL = VGasR;
		  double VGasmedR = 0.5 * (VGasR + VGasRR);
		  double vel1 = VGasmedL / u1L;
		  double vel2 = VGasmedR / u1R;

		  double rho1 = u1L / dutoR.area;

		  double re1;
		  if(dutoR.revest==0)re1 = Rey(dutoR.a, vel1, u1L / dutoR.area,
				  flui.ViscGas(pres, temp));
				  else{
					  double dhid=4* dutoR.area/dutoR.peri;
					  re1 =Rey(dhid, vel1, u1L / dutoR.area,
							flui.ViscGas(pres, temp));
				  }

		  double f1 = fric(re1, dutoR.rug / dutoR.a);
		  double multvelL=dPdLFric*0.5 * f1*( fabsl(vel1)/dutoR.area) * dutoR.peri;

		  local[1][0] = 0.;
		  local[1][1] = 0.;
		  local[1][2] = 0.;
		  //local[1][3] = -(Amed * dpdrho * (1 / dutoR.area) * (1 / dxmed))+1*9.82 * sinl(duto.teta);//alteracao2
		  local[1][3] = -(Amed* 98066.5* (1 / dxmed));//alteracao2
		  local[1][4] = 1 / dt + (vel2 - 2. * vel1) / (2. * dxmed)+multvelL;
		  //local[1][5] =-0*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		  local[1][5] =0.;//alteracao2
		  //local[1][6] = (Amed * dpdrho * (1 / dutoR.area) * (1 / dxmed));//alteracao2
		  local[1][6] = (Amed * 98066.5 * (1 / dxmed));//alteracao2
		  local[1][7] = vel2 / (2. * dxmed);
		  //local[1][8] = 0*Amed*dpdrho*(-drhodT)*(1 / dxmed);//alteracao2
		  local[1][8] = 0.;//alteracao2

		  TL[1] = VGasR / dt;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  local[2][0]=0.;
		  local[2][1]=0.;
		  local[2][2]=0.;
		  local[2][3]=0.;
		  local[2][4]=0.;
		  local[2][5]=1.;
		  local[2][6]=0.;
		  local[2][7]=0.;
		  local[2][8]=0.;
		  TL[2]=tempiniG;



	  } else if (posic == ncelGas) {
		  if(posic<(*celInter)){//alteracao2

		     local[0][0] = 0.;
		     local[0][1] = -1 / dx0;
		     local[0][2] = 0.;
		     local[0][3] = drhodp*duto.area*98066.5 / dt;
		     local[0][4] = 1 / dx0;
		     local[0][5] = 0.;
		     local[0][6] = 0.;
		     local[0][7] = 0.;
		     local[0][8] = 0.;

		   //TL[0] = u1L / dt - 1. * massfonte / dx0;
		     TL[0] = drhodp*duto.area*pres*98066.5 / dt - 1. * massfonteCH / dx0-drhodt*duto.area*dTdt;
		  }//alteracao2
		  else{//alteracao2
			    local[0][0] = 0.;
			    local[0][1] = 0.;
			    local[0][2] = 0.;
			    local[0][3] = 1.;
			    local[0][4] =0.;
			    local[0][5] = 0.;
			    local[0][6] = 0.;
			    local[0][7] = 0.;
			    local[0][8] = 0.;

			    TL[0] = pres;
		  }//alteracao2
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  local[1][0] = 0.;
		  local[1][1] = 0.;
		  local[1][2] = 0.;
		  local[1][3] = 0.;
		  local[1][4] = 1.;
		  local[1][5] = 0.;
		  local[1][6] = 0.;
		  local[1][7] = 0.;
		  local[1][8] = 0.;

		  TL[1] = 0.;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		  local[2][0]=0.;
		  local[2][1]=0.;
		  local[2][2]=0.;
		  local[2][3]=0.;
		  local[2][4]=0.;
		  local[2][5]=1.;
		  local[2][6]=0.;
		  local[2][7]=0.;
		  local[2][8]=0.;
		  TL[2]=temp;


	  }

}


 void CelG::FeiticoDoTempo(){
    tempL=tempLini;
    temp=tempini;
    tempR=tempRini;
    presL=presLini;
    pres=presini;
    presR=presRini;
    VGasL=VGasLini;
    VGasR=VGasRini;
    VGasRR=VGasRRini;
    u1LL=u1LLini;
    u1L=u1Lini;
    u1R=u1Rini;
    //massfonteCH=massfonteCHini;
    //fonteM2=fonteM2ini;
    fechamon=fechamonini;
    razInter=razInterIni;
    //celInter=celInterini;//alteracao2
    posic=posicini;
    rpchk=rpchkini;

    calor.FeiticoDoTempo();
}

 void CelG::DeVoltaParaoFuturo(){
    tempLini=tempL;
    tempini=temp;
    tempRini=tempR;
    presLini=presL;
    presini=pres;
    presRini=presR;
    VGasLini=VGasL;
    VGasRini=VGasR;
    VGasRRini=VGasRR;
    u1LLini=u1LL;
    u1Lini=u1L;
    u1Rini=u1R;
    //massfonteCHini=massfonteCH;
    //fonteM2ini=fonteM2;
    fechamonini=fechamon;
    razInterIni=razInter;
    //celInterini=celInter;//alteracao2
    posicini=posic;
    rpchkini=rpchk;
}

/* void CelG::avancInter(int& reinicia){

  razInter=(razInterIni*dx0+velInter*dt)/dx0;

  if(((razInter<=localtiny)&&(razInter>=-localtiny))) razInter=0;
  else if(razInter<-localtiny){
     double dtaux;
     dtaux=-razInterIni*dx0/velInter;
     if(dtaux>localtiny){
       dtInter=dtaux;
       reinicia=-1;
       razInter=fabsl(0.);
     }
     else razInter=fabsl(0.);
  }
  else if((razInter>=(1.-localtiny)&&razInter<=(1.+localtiny))){
	   razInter=1.;
  }
  else if(razInter>(1.+localtiny)){
     double dtaux;
     dtaux=(1.-razInterIni)*dx0/velInter;
     if(dtaux>localtiny){
       dtInter=dtaux;
       reinicia=-1;
       razInter=1.;
     }
     else razInter=1.;
  }

}*/

//template class CelG;
