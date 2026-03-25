using namespace std;
#include "PropFlu.h"
//#include "MarlimComposicional.h"

//extern "C" void MarlimCalculateViscosityPedersen(double dPressure, double dTemperature, int iNComp, double* oTc, double* oPc, double* oMW,
//        double* oComposition, double* dEta, int* iIER);

ProFlu::ProFlu(varGlob1D* Vvg1dSP ,double vapi,double vrgo,double vdeng,double vbsw ,
                                    double vdenag,double vtempl,double vlvisl,double vtemph,
                                    double vlvish,int vtipoemul, double vaemul,double vbemul,
									double vPHI100, double vbswCorte, int vtab, double vyco2,
									int vcorrC,int vcorrSat,int vcorrOM,int vcorrOV, int vcorrOS,int vflash,
									int vid,int vnpseudo,double vpcf,double vtcf):
                                BPPF(17), GMF(17), APIEMW(10),EMW(10)
                            {//alteracao2//alteracao4
//construtor a partir de valores espec         ficos,
// vmascor vaz         o m         ssica da corrente bif         sica, kg/s
//vapi grau api do          leo da corrente bif         sica
// vrgo RGO da corrente bif         sica m3/m3
//vdeng densidade do g         s
// vbsw, BSW da corrente, m3/m3
//vdenag densidade da          gua
//vtempl -> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, temperatura C de um dos pontos do ASTM
//vlvisl -> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtempl,vlvisl)
//vtemph -> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, temperatura C de um dos pontos do ASTM
//vlvish -> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtemph,vlvish)

  mascor=1.;
  vg1dSP=Vvg1dSP;
  API=vapi;
  RGO=vrgo;
  Deng=vdeng;
  BSW=vbsw;
  Denag=vdenag;
  TempL=vtempl;
  LVisL=vlvisl;
  TempH=vtemph;
  LVisH=vlvish;
  PCF=vpcf;
  TCF=vtcf;
  tipoemul=vtipoemul;
  bswCorte=vbswCorte;
  
  modelaAgua=1;

  posic=0;

  A0Liv=6.54269213;A1Liv=2.60464618;A2Liv=1.21334544;A3Liv=0.16464125;A4Liv=1.19382967;
  B0Liv=0.00000087;B1Liv=4.77390016;B2Liv=1.77267703;B3Liv=-0.73072038;B4Liv=1.58093857;
  C0Liv=0.00322609;C1Liv=0.09281075;C2Liv=0.13633665;C3Liv=0.09634381;C4Liv=0.53238728;
  D0Liv=1.00544053;D1Liv=-0.00134177;D2Liv=0.51839397;


  FAC = 1 + 0.5912*API*(1.8*60 + 32)*0.0001*log10l(147/114.7);
  SG100vb = FAC*Deng;
  if(SG100vb<0.5538)SG100vb=0.5538;
  if(API > 30){
   Avb = 1.187;
   Bvb = 56.06;
   Cvb = 10.393;
  }
  else{
   Avb = 1.0937;
   Bvb = 27.64;
   Cvb = 11.172;
  }

  IRGO=RGO*35.31467/6.29;

  yco2=vyco2;
  corrC=vcorrC;
  if(corrC==0){
    if(Deng<=0.7){
      PC = 708.75 - 57.5*Deng;
      TC = 169 + 314.0*Deng;
    }
    else{
    PC = 708.75 - 57.5*0.7;
    TC = 169 + 314.0*0.7;
    }
  }
  else if(corrC==1){
    double sggHC=0.;
    if(yco2<1.)sggHC=(Deng-1.52*yco2)/(1.-yco2);
    double PCHC=48.86659-3.96448528*sggHC;
    double TCHC=93.889+174.44*sggHC;
    PC=(1.-yco2)*PCHC+73.8*yco2;
    TC=(1.-yco2)*TCHC+304.1*yco2;
    double ES=0.;
    if(yco2>0.0)ES=(120./1.8)*pow(yco2,0.9)-pow(yco2,1.6);
    PC*=(1.-ES/TC);
    TC-=ES;
    PC=psia(PC);
    TC=Faren(TC-273.15)+460.;
  }
  else if(corrC==2){
     double k=3.8216+17.438*Deng-3.2191*(Deng*Deng)-7.0485*yco2;
     double j=0.11582+0.70729*Deng-0.099397*(Deng*Deng)-0.46185*yco2;
     TC=k*k/(1.8*j);
     PC=0.124105621*TC/j;
   PC=psia(PC);
   TC=Faren(TC-273.15)+460.;
  }


  double delaux= BSW*1000.*Denag+(1-BSW)*(141.5*1000./(131.5+API));
  double degaux=1.29*Deng;

  term1=-degaux*RGO*(1-BSW)/(delaux+(1-BSW)*RGO*degaux);
  term2=-2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*degaux*RGO)*352.9797*Deng);
  term3=2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*RGO*degaux));
  term4=degaux*RGO*(1-BSW)/((delaux+(1-BSW)*RGO*degaux)*352.9797*Deng);
  term5=API*Cvb;

  tab=vtab;

  zdranP=0;
  dzdpP=0;
  dzdtP=0;

  cpg=0;
  cpl=0;
  drholdT=0;
  npontos=0;
  npontosB=0;

  nfluP=0;

  ModelCp=0;
  Modeljtl=0;
  CalcLat=0;
  trackRGO=0;
  trackDeng=0;
  corrDeng=0;
  ninjgas=0;
  lingas=0;

  //alteracao4
  BPPF[0]=0.17;BPPF[1]=0.30;BPPF[2]=0.43;BPPF[3]=0.58;BPPF[4]=0.75;BPPF[5]=0.94;BPPF[6]=1.19;BPPF[7]=1.47;BPPF[8]=1.74;
  BPPF[9]=2.10;BPPF[10]=2.70;BPPF[11]=3.29;BPPF[12]=3.80;BPPF[13]=4.30;BPPF[14]=4.90;BPPF[15]=5.70;BPPF[16]=6.70;

  GMF[0]=0.05;GMF[1]=0.10;GMF[2]=0.15;GMF[3]=0.20;GMF[4]=0.25;GMF[5]=0.30;GMF[6]=0.35;GMF[7]=0.40;GMF[8]=0.45;
  GMF[9]=0.50;GMF[10]=0.55;GMF[11]=0.60;GMF[12]=0.65;GMF[13]=0.70;GMF[14]=0.75;GMF[15]=0.80;GMF[16]=0.85;

  APIEMW[0]=5;APIEMW[1]=13;APIEMW[2]=19;APIEMW[3]=24;APIEMW[4]=28;APIEMW[5]=33;APIEMW[6]=38;
  APIEMW[7]=44;APIEMW[8]=52; APIEMW[9]=70;

  EMW[0]=600;EMW[1]=500;EMW[2]=450;EMW[3]=400;EMW[4]=350;EMW[5]=300;EMW[6]=250;EMW[7]=200;EMW[8]=150;
  EMW[9]=100;

  corrSat=vcorrSat;
  corrOM=vcorrOM;
  corrOV=vcorrOV;
  corrOS=vcorrOS;
  aemul=vaemul;
  bemul=vbemul;
  PHI100=vPHI100;
  PBPVTSim=0;
  TBPVTSim=0;
  TabRSLivia=0;
  tabRSPB=0;
  //alteracao4

  flashCompleto=vflash;
  tabelaDinamica=0;
  indiceFlash=0;
  rholF=0;
  rhogF=0;
  DrholDpF=0;
  DrhogDpF=0;
  DrholDtF=0;
  DrhogDtF=0;
  DrhowDtF=0;
  RSF=0;
  cplF=0;
  cpgF=0;
  cpwF=0;
  HlF=0;
  HgF=0;
  sigOGF=0;
  sigWGF=0;
  viscO=0;
  viscG=0;
  //itempAnt=new int;
  itempAnt=0;
  //ipresAnt=new int;
  ipresAnt=0;
  //PBF=0;
  //TBF=0;

  viscBlackOil=1;
  id = vid;

  rDgD=1.;
  rDgL=1.;
  PCis=PC;
  TCis=TC;

  pMinEqu=0.;
  delpTab=0;
  indMinEquP=0;

  tMinEqu=0.;
  deltTab=0;
  indMinEquT=0;
  erroP=0;
  erroT=0;

  JTLiquidoSimple=0.;

  bASTM1 = 0.;
  bASTM2=0.;
  if((flashCompleto==0 || viscBlackOil==1)&&corrOM==0){
	  double rhol=141.5/(131.5 + API);
	  bASTM1 = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)));
	  bASTM2=log10l(log10l((LVisL/1)/rhol + 0.7));
  }

  nvecEmul=0;
  BSWVec=0;
  emulVec=0;
  npseudo=0;
  if(flashCompleto==2){
    npseudo=vnpseudo;
    fracMol=new double[npseudo];
	oCalculatedLiqComposition=new double[npseudo];
	oCalculatedVapComposition=new double[npseudo];
	Pmol=0.;
  }
  else{
	  npseudo=0;
	  //fracMol=0;
	  //oCalculatedLiqComposition=0;
	  //oCalculatedVapComposition=0;
	  Pmol=0.;
  }
  dCalculatedBubbleT=0;
  dCalculatedDewT=0;
  dCalculatedBeta=0;
  dCalculatedBubbleP=0;
  iCalculatedThermodynamicCondition=0;
  iCalculatedStockTankThermodynamicCondition=0;
  dLiquidPhaseMW=0;
  dVaporPhaseMW=0;
  dVaporMassFraction=0;
  dStockTankVaporMassFraction=0;
  dStockTankLiquidDensity=0;
  dStockTankVaporDensity=0;

  masMol=0;
  tempCrit=0;
  presCrit=0;
  fatAcent=0;
  parac=0;
  TIndepPeneloux=0;
  kij=0;
  lij=0;
  CpIGCoefs=0;
  liqModel=0;
  iIER_BubbleT=0;
  iIER_DewT=0;
  iIER_Flash=0;
  iIER_BubbleP=0;
  iIER=0;

  modoBlackTemp=0;

  parserie=0;
  viscTab=0;
  tempTab=0;

  modoParafina=0;
  oPressurePoints=0;
  oCloudPointTemperatures=0;
  oStructurePressures=0;
  oStructureTemperatures=0;
  oStructureDensities=0;
  oStructureLiqMW=0;
  oStructureCPWax=0;
  oStructureThermCond=0;
  oStructureWaxConcs=0;
  oMolecularWeightsOfWaxComponents=0;
  oLiquidDensitiesOfWaxComponents=0;

  npseudoWax=0;
  iPresWax=0;
  iTempWax=0;
  npseudoWaxOut=0;
  oMolecularWeightsOfWaxComponentsOut=0;
  oInterpolatedWaxConcsTDerivOutput=0;
  oInterpolatedWaxConcs=0;
  iIERW=0;
  iWaxComponentCountOutput=0;
  oLiquidDensitiesOfWaxComponentsOutput=0;
  dCloudPointTOutput=0;
  dInterpolatedCPWaxOutput=0;
  dInterpolatedThermCondOutput=0;
  dInterpolatedDensOutput=0;
  dInterpolatedLiqMWOutput=0;
  coefViscWax=1.;

  int simulaTransienteTemp=0;
  if(vg1dSP!=0)simulaTransienteTemp=(*vg1dSP).simulaTransiente;
  if(flashCompleto==2 && npseudo>0 && simulaTransienteTemp==1){
	  	int dim=2;
	  	miniTabDin.rholF=new double*[dim];
	  	miniTabDin.rhogF=new double*[dim];
	  	miniTabDin.DrholDpF=new double*[dim];
	  	miniTabDin.DrhogDpF=new double*[dim];
	  	miniTabDin.DrholDtF=new double*[dim];
	  	miniTabDin.DrhogDtF=new double*[dim];
	  	miniTabDin.valBO=new double*[dim];
	  	miniTabDin.tit=new double*[dim];
	  	miniTabDin.rs=new double*[dim];
	  	miniTabDin.cplF=new double*[dim];
	  	miniTabDin.cpgF=new double*[dim];
	  	miniTabDin.valZ=new double*[dim];
	  	miniTabDin.HlF=new double*[dim];
	  	miniTabDin.HgF=new double*[dim];
	  	miniTabDin.valdZdT=new double*[dim];
	  	miniTabDin.valdZdP=new double*[dim];
	  	miniTabDin.PBF=new double[dim];
	  	miniTabDin.TBF=new double[dim];
		for(int k=0;k<dim;k++){
			miniTabDin.rholF[k]=new double[dim];
			miniTabDin.rhogF[k]=new double[dim];
			miniTabDin.DrholDpF[k]=new double[dim];
			miniTabDin.DrhogDpF[k]=new double[dim];
			miniTabDin.DrholDtF[k]=new double[dim];
			miniTabDin.DrhogDtF[k]=new double[dim];
			miniTabDin.valBO[k]=new double[dim];
			miniTabDin.tit[k]=new double[dim];
			miniTabDin.rs[k]=new double[dim];
			miniTabDin.cplF[k]=new double[dim];
			miniTabDin.cpgF[k]=new double[dim];
			miniTabDin.valZ[k]=new double[dim];
			miniTabDin.HlF[k]=new double[dim];
			miniTabDin.HgF[k]=new double[dim];
			miniTabDin.valdZdT[k]=new double[dim];
			miniTabDin.valdZdP[k]=new double[dim];
		}
		for (int k = 0; k < dim; k++) {
			for (int j = 0; j < dim; j++) {
				miniTabDin.rholF[k][j] = 0;
				miniTabDin.rhogF[k][j] = 0;
				miniTabDin.DrholDpF[k][j] = 0;
				miniTabDin.DrhogDpF[k][j] = 0;
				miniTabDin.DrholDtF[k][j] = 0;
				miniTabDin.DrhogDtF[k][j] = 0;
				miniTabDin.valBO[k][j] = 0;
				miniTabDin.tit[k][j] = 0;
				miniTabDin.rs[k][j] = 0;
				miniTabDin.cplF[k][j] = 0;
				miniTabDin.cpgF[k][j] = 0;
				miniTabDin.valZ[k][j] = 0;
				miniTabDin.HlF[k][j] = 0;
				miniTabDin.HgF[k][j] = 0;
				miniTabDin.valdZdT[k][j] = 0;
				miniTabDin.valdZdP[k][j] = 0;
			}
		}
		for (int k = 0; k < dim; k++) {
			miniTabDin.PBF[k] = 0;
			miniTabDin.TBF[k] = 0;
		}
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
  }
  else{
		miniTabDin.rholF = 0;
		miniTabDin.rhogF = 0;
		miniTabDin.DrholDpF = 0;
		miniTabDin.DrhogDpF = 0;
		miniTabDin.DrholDtF = 0;
		miniTabDin.DrhogDtF = 0;
		miniTabDin.valBO = 0;
		miniTabDin.tit = 0;
		miniTabDin.rs = 0;
		miniTabDin.cplF = 0;
		miniTabDin.cpgF = 0;
		miniTabDin.valZ = 0;
		miniTabDin.HlF = 0;
		miniTabDin.HgF = 0;
		miniTabDin.valdZdT = 0;
		miniTabDin.valdZdP = 0;
		miniTabDin.PBF = 0;
		miniTabDin.TBF = 0;
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
  }

}//alteracao2

ProFlu::ProFlu(varGlob1D* Vvg1dSP, const double* const fluido, int vtipoemul,
                                double vaemul, double vbemul,double vPHI100, double vbswCorte,
                                int vtab, double vyco2, int vcorrC,int vcorrSat,int vcorrOM,int vcorrOV,
								int vcorrOS,int vflash, int vid, int vnpseudo):
                                    BPPF(17), GMF(17), APIEMW(10),EMW(10){//alteracao2//alteracao4
//construtor em que as propriedades vem agrupadas em um vetor
//vmascor vaz         o m         ssica da corrente bif         sica, kg/s
//         ndice 0-> grau api do          leo da corrente bif         sica
//         ndice 1-> RGO da corrente bif         sica m3/m3
//         ndice 2-> densidade do g         s
//         ndice 3-> BSW da corrente, m3/m3
//         ndice 4-> densidade da          gua
//         ndice 5-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, temperatura C de um dos pontos do ASTM
//         ndice 6-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtempl,vlvisl)
//         ndice 7-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, temperatura C de um dos pontos do ASTM
//         ndice 8-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtemph,vlvish)

  mascor=1;
  vg1dSP=Vvg1dSP;
  API=fluido[0];
  RGO=fluido[1];
  Deng=fluido[2];
  BSW=fluido[3];
  Denag=fluido[4];
  TempL=fluido[5];
  LVisL=fluido[6];
  TempH=fluido[7];
  LVisH=fluido[8];
  PCF=fluido[9];
  TCF=fluido[10];
  tipoemul=vtipoemul;
  bswCorte=vbswCorte;

  modelaAgua=1;

  posic=0;

  A0Liv=6.54269213;A1Liv=2.60464618;A2Liv=1.21334544;A3Liv=0.16464125;A4Liv=1.19382967;
  B0Liv=0.00000087;B1Liv=4.77390016;B2Liv=1.77267703;B3Liv=-0.73072038;B4Liv=1.58093857;
  C0Liv=0.00322609;C1Liv=0.09281075;C2Liv=0.13633665;C3Liv=0.09634381;C4Liv=0.53238728;
  D0Liv=1.00544053;D1Liv=-0.00134177;D2Liv=0.51839397;



  FAC = 1 + 0.5912*API*(1.8*60 + 32)*0.0001*log10l(147/114.7);
  SG100vb = FAC*Deng;
  if(SG100vb<0.5538)SG100vb=0.5538;
  if(API > 30){
   Avb = 1.187;
   Bvb = 56.06;
   Cvb = 10.393;
  }
  else{
   Avb = 1.0937;
   Bvb = 27.64;
   Cvb = 11.172;
  }

  IRGO=RGO*35.31467/6.29;

  yco2=vyco2;
  corrC=vcorrC;
  if(corrC==0){
    if(Deng<=0.7){
      PC = 708.75 - 57.5*Deng;
      TC = 169 + 314.0*Deng;
    }
    else{
    PC = 708.75 - 57.5*0.7;
    TC = 169 + 314.0*0.7;
    }
  }
  else if(corrC==1){
    double sggHC=0.;
    if(yco2<1.)sggHC=(Deng-1.52*yco2)/(1.-yco2);
    double PCHC=48.86659-3.96448528*sggHC;
    double TCHC=93.889+174.44*sggHC;
    PC=(1.-yco2)*PCHC+73.8*yco2;
    TC=(1.-yco2)*TCHC+304.1*yco2;
    double ES=0.;
    if(yco2>0.0)ES=(120./1.8)*pow(yco2,0.9)-pow(yco2,1.6);
    PC*=(1.-ES/TC);
    TC-=ES;
    PC=psia(PC);
    TC=Faren(TC-273.15)+460.;
  }
  else if(corrC==2){
     double k=3.8216+17.438*Deng-3.2191*(Deng*Deng)-7.0485*yco2;
     double j=0.11582+0.70729*Deng-0.099397*(Deng*Deng)-0.46185*yco2;
     TC=k*k/(1.8*j);
     PC=0.124105621*TC/j;
   PC=psia(PC);
   TC=Faren(TC-273.15)+460.;
  }

  double delaux= BSW*1000.*Denag+(1-BSW)*(141.5*1000./(131.5+API));
  double degaux=1.29*Deng;

  term1=-degaux*RGO*(1-BSW)/(delaux+(1-BSW)*RGO*degaux);
  term2=-2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*degaux*RGO)*352.9797*Deng);
  term3=2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*RGO*degaux));
  term4=degaux*RGO*(1-BSW)/((delaux+(1-BSW)*RGO*degaux)*352.9797*Deng);
  term5=API*Cvb;

  tab=vtab;

  zdranP=0;
  dzdpP=0;
  dzdtP=0;

  cpg=0;
  cpl=0;
  drholdT=0;
  npontos=0;
  npontosB=0;

  nfluP=0;

  ModelCp=0;
  Modeljtl=0;
  CalcLat=0;
  trackRGO=0;
  trackDeng=0;
  corrDeng=0;
  ninjgas=0;
  lingas=0;

  //alteracao4
  BPPF[0]=0.17;BPPF[1]=0.30;BPPF[2]=0.43;BPPF[3]=0.58;BPPF[4]=0.75;BPPF[5]=0.94;BPPF[6]=1.19;BPPF[7]=1.47;BPPF[8]=1.74;
  BPPF[9]=2.10;BPPF[10]=2.70;BPPF[11]=3.29;BPPF[12]=3.80;BPPF[13]=4.30;BPPF[14]=4.90;BPPF[15]=5.70;BPPF[16]=6.70;

  GMF[0]=0.05;GMF[1]=0.10;GMF[2]=0.15;GMF[3]=0.20;GMF[4]=0.25;GMF[5]=0.30;GMF[6]=0.35;GMF[7]=0.40;GMF[8]=0.45;
  GMF[9]=0.50;GMF[10]=0.55;GMF[11]=0.60;GMF[12]=0.65;GMF[13]=0.70;GMF[14]=0.75;GMF[15]=0.80;GMF[16]=0.85;

  APIEMW[0]=5;APIEMW[1]=13;APIEMW[2]=19;APIEMW[3]=24;APIEMW[4]=28;APIEMW[5]=33;APIEMW[6]=38;
  APIEMW[7]=44;APIEMW[8]=52; APIEMW[9]=70;

  EMW[0]=600;EMW[1]=500;EMW[2]=450;EMW[3]=400;EMW[4]=350;EMW[5]=300;EMW[6]=250;EMW[7]=200;EMW[8]=150;
  EMW[9]=100;

  corrSat=vcorrSat;
  corrOM=vcorrOM;
  corrOV=vcorrOV;
  corrOS=vcorrOS;
  aemul=vaemul;
  bemul=vbemul;
  PHI100=vPHI100;
  if(corrSat==1)corrSat=4;
  PBPVTSim=0;
  TBPVTSim=0;
  TabRSLivia=0;
 tabRSPB=0;
  //alteracao4
 flashCompleto=vflash;
 tabelaDinamica=0;
 indiceFlash=0;
 rholF=0;
 rhogF=0;
 DrholDpF=0;
 DrhogDpF=0;
 DrholDtF=0;
 DrhogDtF=0;
 DrhowDtF=0;
 RSF=0;
 cplF=0;
 cpgF=0;
 cpwF=0;
 HlF=0;
 HgF=0;
 sigOGF=0;
 sigWGF=0;
 viscO=0;
 viscG=0;
 //itempAnt=new int;
 itempAnt=0;
 //ipresAnt=new int;
 ipresAnt=0;
 //PBF=0;
 //TBF=0;

 viscBlackOil=1;
 id = vid;

 rDgD=1.;
 rDgL=1.;
 PCis=PC;
 TCis=TC;

 pMinEqu=0.;
 delpTab=0;
 indMinEquP=0;

 tMinEqu=0.;
 deltTab=0;
 indMinEquT=0;
 erroP=0;
 erroT=0;

 modoParafina=0;
 oPressurePoints=0;
 oCloudPointTemperatures=0;
 oStructurePressures=0;
 oStructureTemperatures=0;
 oStructureDensities=0;
 oStructureLiqMW=0;
 oStructureCPWax=0;
 oStructureThermCond=0;
 oStructureWaxConcs=0;
 oMolecularWeightsOfWaxComponents=0;
 oLiquidDensitiesOfWaxComponents=0;

 npseudoWax=0;
 iPresWax=0;
 iTempWax=0;
 npseudoWaxOut=0;
 oMolecularWeightsOfWaxComponentsOut=0;
 oInterpolatedWaxConcsTDerivOutput=0;
 oInterpolatedWaxConcs=0;

 iIERW=0;
 iWaxComponentCountOutput=0;
 dCloudPointTOutput=0;
 dInterpolatedCPWaxOutput=0;
 dInterpolatedThermCondOutput=0;
 dInterpolatedDensOutput=0;
 dInterpolatedLiqMWOutput=0;
 oLiquidDensitiesOfWaxComponentsOutput=0;
 coefViscWax=1.;

 bASTM1 = 0.;
 bASTM2=0.;
 if((flashCompleto==0 || viscBlackOil==1)&&corrOM==0){
	  double rhol=141.5/(131.5 + API);
	  bASTM1 = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)));
	  bASTM2=log10l(log10l((LVisL/1)/rhol + 0.7));
 }

 nvecEmul=0;
 BSWVec=0;
 emulVec=0;
 npseudo=0;
 if(flashCompleto==2){
   npseudo=vnpseudo;
   fracMol=new double[npseudo];
	oCalculatedLiqComposition=new double[npseudo];
	oCalculatedVapComposition=new double[npseudo];
	Pmol=0.;
 }
 else{
	  npseudo=0;
	  //fracMol=0;
	  //oCalculatedLiqComposition=0;
	  //oCalculatedVapComposition=0;
	  Pmol=0.;
 }
 dCalculatedBubbleT=0;
 dCalculatedDewT=0;
 dCalculatedBeta=0;
 dCalculatedBubbleP=0;
 iCalculatedThermodynamicCondition=0;
 iCalculatedStockTankThermodynamicCondition=0;
 dLiquidPhaseMW=0;
 dVaporPhaseMW=0;
 dVaporMassFraction=0;

 masMol=0;
 tempCrit=0;
 presCrit=0;
 fatAcent=0;
 parac=0;
 TIndepPeneloux=0;
 kij=0;
 lij=0;
 CpIGCoefs=0;
 liqModel=0;
 iIER_BubbleT=0;
 iIER_DewT=0;
 iIER_Flash=0;
 iIER_BubbleP=0;
 iIER=0;

 dStockTankVaporMassFraction=0.;
 dStockTankLiquidDensity=0.;
 dStockTankVaporDensity=0.;

 modoBlackTemp=0;

 parserie=0;
 viscTab=0;
 tempTab=0;

 JTLiquidoSimple=0.;

 int simulaTransienteTemp=0;
 if(vg1dSP!=0)simulaTransienteTemp=(*vg1dSP).simulaTransiente;
 if(flashCompleto==2 && npseudo>0 && simulaTransienteTemp==1){
	  	int dim=2;
	  	miniTabDin.rholF=new double*[dim];
	  	miniTabDin.rhogF=new double*[dim];
	  	miniTabDin.DrholDpF=new double*[dim];
	  	miniTabDin.DrhogDpF=new double*[dim];
	  	miniTabDin.DrholDtF=new double*[dim];
	  	miniTabDin.DrhogDtF=new double*[dim];
	  	miniTabDin.valBO=new double*[dim];
	  	miniTabDin.tit=new double*[dim];
	  	miniTabDin.rs=new double*[dim];
	  	miniTabDin.cplF=new double*[dim];
	  	miniTabDin.cpgF=new double*[dim];
	  	miniTabDin.valZ=new double*[dim];
	  	miniTabDin.HlF=new double*[dim];
	  	miniTabDin.HgF=new double*[dim];
	  	miniTabDin.valdZdT=new double*[dim];
	  	miniTabDin.valdZdP=new double*[dim];
	  	miniTabDin.PBF=new double[dim];
	  	miniTabDin.TBF=new double[dim];
		for(int k=0;k<dim;k++){
			miniTabDin.rholF[k]=new double[dim];
			miniTabDin.rhogF[k]=new double[dim];
			miniTabDin.DrholDpF[k]=new double[dim];
			miniTabDin.DrhogDpF[k]=new double[dim];
			miniTabDin.DrholDtF[k]=new double[dim];
			miniTabDin.DrhogDtF[k]=new double[dim];
			miniTabDin.valBO[k]=new double[dim];
			miniTabDin.tit[k]=new double[dim];
			miniTabDin.rs[k]=new double[dim];
			miniTabDin.cplF[k]=new double[dim];
			miniTabDin.cpgF[k]=new double[dim];
			miniTabDin.valZ[k]=new double[dim];
			miniTabDin.HlF[k]=new double[dim];
			miniTabDin.HgF[k]=new double[dim];
			miniTabDin.valdZdT[k]=new double[dim];
			miniTabDin.valdZdP[k]=new double[dim];
		}
		for (int k = 0; k < dim; k++) {
			for (int j = 0; j < dim; j++) {
				miniTabDin.rholF[k][j] = 0;
				miniTabDin.rhogF[k][j] = 0;
				miniTabDin.DrholDpF[k][j] = 0;
				miniTabDin.DrhogDpF[k][j] = 0;
				miniTabDin.DrholDtF[k][j] = 0;
				miniTabDin.DrhogDtF[k][j] = 0;
				miniTabDin.valBO[k][j] = 0;
				miniTabDin.tit[k][j] = 0;
				miniTabDin.rs[k][j] = 0;
				miniTabDin.cplF[k][j] = 0;
				miniTabDin.cpgF[k][j] = 0;
				miniTabDin.valZ[k][j] = 0;
				miniTabDin.HlF[k][j] = 0;
				miniTabDin.HgF[k][j] = 0;
				miniTabDin.valdZdT[k][j] = 0;
				miniTabDin.valdZdP[k][j] = 0;
			}
		}
		for (int k = 0; k < dim; k++) {
			miniTabDin.PBF[k] = 0;
			miniTabDin.TBF[k] = 0;
		}
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
 }
 else{
		miniTabDin.rholF = 0;
		miniTabDin.rhogF = 0;
		miniTabDin.DrholDpF = 0;
		miniTabDin.DrhogDpF = 0;
		miniTabDin.DrholDtF = 0;
		miniTabDin.DrhogDtF = 0;
		miniTabDin.valBO = 0;
		miniTabDin.tit = 0;
		miniTabDin.rs = 0;
		miniTabDin.cplF = 0;
		miniTabDin.cpgF = 0;
		miniTabDin.valZ = 0;
		miniTabDin.HlF = 0;
		miniTabDin.HgF = 0;
		miniTabDin.valdZdT = 0;
		miniTabDin.valdZdP = 0;
		miniTabDin.PBF = 0;
		miniTabDin.TBF = 0;
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
 }

}//alteracao2

ProFlu::ProFlu(varGlob1D* Vvg1dSP, const double* const fluido, const double* const astm, int vtipoemul
                               , double vaemul, double vbemul,double vPHI100,
                                double vbswCorte,int vtab, double vyco2,
								int vcorrC,int vcorrSat,int vcorrOM,int vcorrOV,
								int vcorrOS,int vflash, int vid, int vnpseudo):
                                    BPPF(17), GMF(17), APIEMW(10),EMW(10)
                  {//alteracao2//alteracao4
//construtor em que os termos de viscosidade ASTM vem separados das outras propriedades
//vmascor vaz         o m         ssica da corrente bif         sica, kg/s
//primi          ndice 0-> grau api do          leo da corrente bif         sica
//primi          ndice 1-> RGO da corrente bif         sica m3/m3
//primi          ndice 2-> densidade do g         s
//primi          ndice 3-> BSW da corrente, m3/m3
//primi          ndice 4-> densidade da          gua
//astm          ndice 0-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, temperatura C de um dos pontos do ASTM
//astm          ndice 1-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtempl,vlvisl)
//astm          ndice 2-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, temperatura C de um dos pontos do ASTM
//astm          ndice 3-> para o caso de c         lculo de viscosidade do l         quido com o aux         lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtemph,vlvish)
  mascor=1.;
  vg1dSP=Vvg1dSP;
  API=fluido[0];
  RGO=fluido[1];
  Deng=fluido[2];
  BSW=fluido[3];
  Denag=fluido[4];
  PCF=fluido[5];
  TCF=fluido[6];
  TempL=astm[0];
  LVisL=astm[1];
  TempH=astm[2];
  LVisH=astm[3];
  tipoemul=vtipoemul;
  bswCorte=vbswCorte;

  modelaAgua=1;

  posic=0;

  A0Liv=6.54269213;A1Liv=2.60464618;A2Liv=1.21334544;A3Liv=0.16464125;A4Liv=1.19382967;
  B0Liv=0.00000087;B1Liv=4.77390016;B2Liv=1.77267703;B3Liv=-0.73072038;B4Liv=1.58093857;
  C0Liv=0.00322609;C1Liv=0.09281075;C2Liv=0.13633665;C3Liv=0.09634381;C4Liv=0.53238728;
  D0Liv=1.00544053;D1Liv=-0.00134177;D2Liv=0.51839397;



  FAC = 1 + 0.5912*API*(1.8*60 + 32)*0.0001*log10l(147/114.7);
  SG100vb = FAC*Deng;
  if(SG100vb<0.5538)SG100vb=0.5538;
  if(API > 30){
   Avb = 1.187;
   Bvb = 56.06;
   Cvb = 10.393;
  }
  else{
   Avb = 1.0937;
   Bvb = 27.64;
   Cvb = 11.172;
  }

  IRGO=RGO*35.31467/6.29;
/*  if(Deng<=0.7){
    PC = 708.75 - 57.5*Deng;
    TC = 169 + 314.0*Deng;
  }
  else{
  PC = 708.75 - 57.5*0.7;
  TC = 169 + 314.0*0.7;
  }*/
  yco2=vyco2;
  corrC=vcorrC;
  if(corrC==0){
    if(Deng<=0.7){
      PC = 708.75 - 57.5*Deng;
      TC = 169 + 314.0*Deng;
    }
    else{
    PC = 708.75 - 57.5*0.7;
    TC = 169 + 314.0*0.7;
    }
  }
  else if(corrC==1){
    double sggHC=0.;
    if(yco2<1.)sggHC=(Deng-1.52*yco2)/(1.-yco2);
    double PCHC=48.86659-3.96448528*sggHC;
    double TCHC=93.889+174.44*sggHC;
    PC=(1.-yco2)*PCHC+73.8*yco2;
    TC=(1.-yco2)*TCHC+304.1*yco2;
    double ES=0.;
    if(yco2>0.0)ES=(120./1.8)*pow(yco2,0.9)-pow(yco2,1.6);
    PC*=(1.-ES/TC);
    TC-=ES;
    PC=psia(PC);
    TC=Faren(TC-273.15)+460.;
  }
  else if(corrC==2){
     double k=3.8216+17.438*Deng-3.2191*(Deng*Deng)-7.0485*yco2;
     double j=0.11582+0.70729*Deng-0.099397*(Deng*Deng)-0.46185*yco2;
     TC=k*k/(1.8*j);
     PC=0.124105621*TC/j;
   PC=psia(PC);
   TC=Faren(TC-273.15)+460.;
  }

  double delaux= BSW*1000.*Denag+(1-BSW)*(141.5*1000./(131.5+API));
  double degaux=1.29*Deng;

  term1=-degaux*RGO*(1-BSW)/(delaux+(1-BSW)*RGO*degaux);
  term2=-2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*degaux*RGO)*352.9797*Deng);
  term3=2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*RGO*degaux));
  term4=degaux*RGO*(1-BSW)/((delaux+(1-BSW)*RGO*degaux)*352.9797*Deng);
  term5=API*Cvb;

  tab=vtab;
  zdranP=0;
  dzdpP=0;
  dzdtP=0;
  cpg=0;
  cpl=0;
  drholdT=0;
  npontos=0;
  npontosB=0;
  nfluP=0;

  ModelCp=0;
  Modeljtl=0;
  CalcLat=0;
  trackRGO=0;
  trackDeng=0;
  corrDeng=0;
  ninjgas=0;
  lingas=0;

  //alteracao4
  BPPF[0]=0.17;BPPF[1]=0.30;BPPF[2]=0.43;BPPF[3]=0.58;BPPF[4]=0.75;BPPF[5]=0.94;BPPF[6]=1.19;BPPF[7]=1.47;BPPF[8]=1.74;
  BPPF[9]=2.10;BPPF[10]=2.70;BPPF[11]=3.29;BPPF[12]=3.80;BPPF[13]=4.30;BPPF[14]=4.90;BPPF[15]=5.70;BPPF[16]=6.70;

  GMF[0]=0.05;GMF[1]=0.10;GMF[2]=0.15;GMF[3]=0.20;GMF[4]=0.25;GMF[5]=0.30;GMF[6]=0.35;GMF[7]=0.40;GMF[8]=0.45;
  GMF[9]=0.50;GMF[10]=0.55;GMF[11]=0.60;GMF[12]=0.65;GMF[13]=0.70;GMF[14]=0.75;GMF[15]=0.80;GMF[16]=0.85;

  APIEMW[0]=5;APIEMW[1]=13;APIEMW[2]=19;APIEMW[3]=24;APIEMW[4]=28;APIEMW[5]=33;APIEMW[6]=38;
  APIEMW[7]=44;APIEMW[8]=52; APIEMW[9]=70;

  EMW[0]=600;EMW[1]=500;EMW[2]=450;EMW[3]=400;EMW[4]=350;EMW[5]=300;EMW[6]=250;EMW[7]=200;EMW[8]=150;
  EMW[9]=100;

  corrSat=vcorrSat;
  corrOM=vcorrOM;
  corrOV=vcorrOV;
  corrOS=vcorrOS;
  aemul=vaemul;
  bemul=vbemul;
  PHI100=vPHI100;
  PBPVTSim=0;
  TBPVTSim=0;
  TabRSLivia=0;
  tabRSPB=0;
  //if(corrSat==1)corrSat=4;
  //alteracao4
  flashCompleto=vflash;
  tabelaDinamica=0;
  indiceFlash=0;
  rholF=0;
  rhogF=0;
  DrholDpF=0;
  DrhogDpF=0;
  DrholDtF=0;
  DrhogDtF=0;
  DrhowDtF=0;
  RSF=0;
  cplF=0;
  cpgF=0;
  cpwF=0;
  HlF=0;
  HgF=0;
  sigOGF=0;
  sigWGF=0;
  viscO=0;
  viscG=0;
  //itempAnt=new int;
  itempAnt=0;
  //ipresAnt=new int;
  ipresAnt=0;
  //PBF=0;
  //TBF=0;

  viscBlackOil=1;
  id = vid;

  rDgD=1.;
  rDgL=1.;
  PCis=PC;
  TCis=TC;

  pMinEqu=0.;
  delpTab=0;
  indMinEquP=0;

  tMinEqu=0.;
  deltTab=0;
  indMinEquT=0;
  erroP=0;
  erroT=0;

  JTLiquidoSimple=0.;

  modoParafina=0;
  oPressurePoints=0;
  oCloudPointTemperatures=0;
  oStructurePressures=0;
  oStructureTemperatures=0;
  oStructureDensities=0;
  oStructureLiqMW=0;
  oStructureCPWax=0;
  oStructureThermCond=0;
  oStructureWaxConcs=0;
  oMolecularWeightsOfWaxComponents=0;
  oLiquidDensitiesOfWaxComponents=0;

  npseudoWax=0;
  iPresWax=0;
  iTempWax=0;
  npseudoWaxOut=0;
  oMolecularWeightsOfWaxComponentsOut=0;
  oInterpolatedWaxConcsTDerivOutput=0;
  oInterpolatedWaxConcs=0;

  iIERW=0;
  iWaxComponentCountOutput=0;
  dCloudPointTOutput=0;
  dInterpolatedCPWaxOutput=0;
  dInterpolatedThermCondOutput=0;
  dInterpolatedDensOutput=0;
  dInterpolatedLiqMWOutput=0;
  oLiquidDensitiesOfWaxComponentsOutput=0;
  coefViscWax=1.;

  bASTM1 = 0.;
  bASTM2=0.;
  if((flashCompleto==0 || viscBlackOil==1)&&corrOM==0){
	  double rhol=141.5/(131.5 + API);
	  bASTM1 = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)));
	  bASTM2=log10l(log10l((LVisL/1)/rhol + 0.7));
  }

  nvecEmul=0;
  BSWVec=0;
  emulVec=0;

  npseudo=0;
  if(flashCompleto==2){
    npseudo=vnpseudo;
    fracMol=new double[npseudo];
	oCalculatedLiqComposition=new double[npseudo];
	oCalculatedVapComposition=new double[npseudo];
	Pmol=0.;
  }
  else{
	  npseudo=0;
	  //fracMol=0;
	  //Pmol=0.;
  }
  dCalculatedBubbleT=0;
  dCalculatedDewT=0;
  dCalculatedBeta=0;
  dCalculatedBubbleP=0;
  iCalculatedThermodynamicCondition=0;
  iCalculatedStockTankThermodynamicCondition=0;
  dLiquidPhaseMW=0;
  dVaporPhaseMW=0;
  dVaporMassFraction=0;

  masMol=0;
  tempCrit=0;
  presCrit=0;
  fatAcent=0;
  parac=0;
  TIndepPeneloux=0;
  kij=0;
  lij=0;
  CpIGCoefs=0;
  liqModel=0;
  iIER_BubbleT=0;
  iIER_DewT=0;
  iIER_Flash=0;
  iIER_BubbleP=0;
  iIER=0;

  dStockTankVaporMassFraction=0.;
  dStockTankLiquidDensity=0.;
  dStockTankVaporDensity=0.;

  modoBlackTemp=0;

  parserie=0;
  viscTab=0;
  tempTab=0;



  int simulaTransienteTemp=0;
  if(vg1dSP!=0)simulaTransienteTemp=(*vg1dSP).simulaTransiente;
  if(flashCompleto==2 && npseudo>0 && simulaTransienteTemp==1){
	  	int dim=2;
	  	miniTabDin.rholF=new double*[dim];
	  	miniTabDin.rhogF=new double*[dim];
	  	miniTabDin.DrholDpF=new double*[dim];
	  	miniTabDin.DrhogDpF=new double*[dim];
	  	miniTabDin.DrholDtF=new double*[dim];
	  	miniTabDin.DrhogDtF=new double*[dim];
	  	miniTabDin.valBO=new double*[dim];
	  	miniTabDin.tit=new double*[dim];
	  	miniTabDin.rs=new double*[dim];
	  	miniTabDin.cplF=new double*[dim];
	  	miniTabDin.cpgF=new double*[dim];
	  	miniTabDin.valZ=new double*[dim];
	  	miniTabDin.HlF=new double*[dim];
	  	miniTabDin.HgF=new double*[dim];
	  	miniTabDin.valdZdT=new double*[dim];
	  	miniTabDin.valdZdP=new double*[dim];
	  	miniTabDin.PBF=new double[dim];
	  	miniTabDin.TBF=new double[dim];
		for(int k=0;k<dim;k++){
			miniTabDin.rholF[k]=new double[dim];
			miniTabDin.rhogF[k]=new double[dim];
			miniTabDin.DrholDpF[k]=new double[dim];
			miniTabDin.DrhogDpF[k]=new double[dim];
			miniTabDin.DrholDtF[k]=new double[dim];
			miniTabDin.DrhogDtF[k]=new double[dim];
			miniTabDin.valBO[k]=new double[dim];
			miniTabDin.tit[k]=new double[dim];
			miniTabDin.rs[k]=new double[dim];
			miniTabDin.cplF[k]=new double[dim];
			miniTabDin.cpgF[k]=new double[dim];
			miniTabDin.valZ[k]=new double[dim];
			miniTabDin.HlF[k]=new double[dim];
			miniTabDin.HgF[k]=new double[dim];
			miniTabDin.valdZdT[k]=new double[dim];
			miniTabDin.valdZdP[k]=new double[dim];
		}
		for (int k = 0; k < dim; k++) {
			for (int j = 0; j < dim; j++) {
				miniTabDin.rholF[k][j] = 0;
				miniTabDin.rhogF[k][j] = 0;
				miniTabDin.DrholDpF[k][j] = 0;
				miniTabDin.DrhogDpF[k][j] = 0;
				miniTabDin.DrholDtF[k][j] = 0;
				miniTabDin.DrhogDtF[k][j] = 0;
				miniTabDin.valBO[k][j] = 0;
				miniTabDin.tit[k][j] = 0;
				miniTabDin.rs[k][j] = 0;
				miniTabDin.cplF[k][j] = 0;
				miniTabDin.cpgF[k][j] = 0;
				miniTabDin.valZ[k][j] = 0;
				miniTabDin.HlF[k][j] = 0;
				miniTabDin.HgF[k][j] = 0;
				miniTabDin.valdZdT[k][j] = 0;
				miniTabDin.valdZdP[k][j] = 0;
			}
		}
		for (int k = 0; k < dim; k++) {
			miniTabDin.PBF[k] = 0;
			miniTabDin.TBF[k] = 0;
		}
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
  }
  else{
		miniTabDin.rholF = 0;
		miniTabDin.rhogF = 0;
		miniTabDin.DrholDpF = 0;
		miniTabDin.DrhogDpF = 0;
		miniTabDin.DrholDtF = 0;
		miniTabDin.DrhogDtF = 0;
		miniTabDin.valBO = 0;
		miniTabDin.tit = 0;
		miniTabDin.rs = 0;
		miniTabDin.cplF = 0;
		miniTabDin.cpgF = 0;
		miniTabDin.valZ = 0;
		miniTabDin.HlF = 0;
		miniTabDin.HgF = 0;
		miniTabDin.valdZdT = 0;
		miniTabDin.valdZdP = 0;
		miniTabDin.PBF = 0;
		miniTabDin.TBF = 0;
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
  }

}//alteracao2

ProFlu::ProFlu(const ProFlu& fluido):
                        BPPF(17), GMF(17), APIEMW(10),EMW(10){//alteracao2//alteracao4
//construtor por c         pia
  mascor=fluido.mascor;
  vg1dSP=fluido.vg1dSP;
  API=fluido.API;
  RGO=fluido.RGO;
  Deng=fluido.Deng;
  BSW=fluido.BSW;
  Denag=fluido.Denag;
  TempL=fluido.TempL;
  LVisL=fluido.LVisL;
  TempH=fluido.TempH;
  LVisH=fluido.LVisH;
  tipoemul=fluido.tipoemul;
  bswCorte=fluido.bswCorte;
  FAC=fluido.FAC;
  SG100vb=fluido.SG100vb;
  Avb=fluido.Avb;
  Bvb=fluido.Bvb;
  Cvb=fluido.Cvb;
  IRGO=fluido.IRGO;
  yco2=fluido.yco2;
  corrC=fluido.corrC;
  PC=fluido.PC;
  TC=fluido.TC;
  PCF=fluido.PCF;
  TCF=fluido.TCF;

  modelaAgua=fluido.modelaAgua;

  posic=0;

  A0Liv=6.54269213;A1Liv=2.60464618;A2Liv=1.21334544;A3Liv=0.16464125;A4Liv=1.19382967;
  B0Liv=0.00000087;B1Liv=4.77390016;B2Liv=1.77267703;B3Liv=-0.73072038;B4Liv=1.58093857;
  C0Liv=0.00322609;C1Liv=0.09281075;C2Liv=0.13633665;C3Liv=0.09634381;C4Liv=0.53238728;
  D0Liv=1.00544053;D1Liv=-0.00134177;D2Liv=0.51839397;



  term1=fluido.term1;
  term2=fluido.term2;
  term3=fluido.term3;
  term4=fluido.term4;
  term5=fluido.term5;

  tab=fluido.tab;
  zdranP=fluido.zdranP;
  dzdpP=fluido.dzdpP;
  dzdtP=fluido.dzdtP;

  cpg=fluido.cpg;
  cpl=fluido.cpl;
  drholdT=fluido.drholdT;

  npontos=fluido.npontos;
  npontosB=fluido.npontosB;

  nfluP=fluido.nfluP;

  ModelCp=fluido.ModelCp;
  Modeljtl=fluido.Modeljtl;
  CalcLat=fluido.CalcLat;
  trackRGO=fluido.trackRGO;
  trackDeng=fluido.trackDeng;
  corrDeng=fluido.corrDeng;
  ninjgas=fluido.ninjgas;
  lingas=fluido.lingas;

  //alteracao4
  BPPF[0]=0.17;BPPF[1]=0.30;BPPF[2]=0.43;BPPF[3]=0.58;BPPF[4]=0.75;BPPF[5]=0.94;BPPF[6]=1.19;BPPF[7]=1.47;BPPF[8]=1.74;
  BPPF[9]=2.10;BPPF[10]=2.70;BPPF[11]=3.29;BPPF[12]=3.80;BPPF[13]=4.30;BPPF[14]=4.90;BPPF[15]=5.70;BPPF[16]=6.70;

  GMF[0]=0.05;GMF[1]=0.10;GMF[2]=0.15;GMF[3]=0.20;GMF[4]=0.25;GMF[5]=0.30;GMF[6]=0.35;GMF[7]=0.40;GMF[8]=0.45;
  GMF[9]=0.50;GMF[10]=0.55;GMF[11]=0.60;GMF[12]=0.65;GMF[13]=0.70;GMF[14]=0.75;GMF[15]=0.80;GMF[16]=0.85;

  APIEMW[0]=5;APIEMW[1]=13;APIEMW[2]=19;APIEMW[3]=24;APIEMW[4]=28;APIEMW[5]=33;APIEMW[6]=38;
  APIEMW[7]=44;APIEMW[8]=52; APIEMW[9]=70;

  EMW[0]=600;EMW[1]=500;EMW[2]=450;EMW[3]=400;EMW[4]=350;EMW[5]=300;EMW[6]=250;EMW[7]=200;EMW[8]=150;
  EMW[9]=100;

  corrSat=fluido.corrSat;
  corrOM=fluido.corrOM;
  corrOV=fluido.corrOV;
  corrOS=fluido.corrOS;
  aemul=fluido.aemul;
  bemul=fluido.bemul;
  PHI100=fluido.PHI100;
  PBPVTSim=fluido.PBPVTSim;
  TBPVTSim=fluido.TBPVTSim;
  TabRSLivia=fluido.TabRSLivia;
  tabRSPB=fluido.tabRSPB;
 // if(corrSat==1)corrSat=4;
  //alteracao4

  flashCompleto=fluido.flashCompleto;
  tabelaDinamica=fluido.tabelaDinamica;
  indiceFlash=fluido.indiceFlash;
  rholF=fluido.rholF;
  rhogF=fluido.rhogF;
  DrholDpF=fluido.DrholDpF;
  DrhogDpF=fluido.DrhogDpF;
  DrholDtF=fluido.DrholDtF;
  DrhogDtF=fluido.DrhogDtF;
  DrhowDtF=fluido.DrhowDtF;
  RSF=fluido.RSF;
  cplF=fluido.cplF;
  cpgF=fluido.cpgF;
  cpwF=fluido.cpwF;
  HlF=fluido.HlF;
  HgF=fluido.HgF;
  sigOGF=fluido.sigOGF;
  sigWGF=fluido.sigWGF;
  viscO=fluido.viscO;
  viscG=fluido.viscG;
  //itempAnt=new int;
  itempAnt=0;
  //ipresAnt=new int;
  ipresAnt=0;
  //PBF=fluido.PBF;
  //TBF=fluido.TBF;

  viscBlackOil=fluido.viscBlackOil;
  id = fluido.id;

  rDgD=fluido.rDgD;
  rDgL=fluido.rDgL;
  PCis=fluido.PCis;
  TCis=fluido.TCis;

  JTLiquidoSimple=fluido.JTLiquidoSimple;

  pMinEqu=fluido.pMinEqu;
  delpTab=fluido.delpTab;
  indMinEquP=fluido.indMinEquP;

  tMinEqu=fluido.tMinEqu;
  deltTab=fluido.deltTab;
  indMinEquT=fluido.indMinEquT;
  erroP=fluido.erroP;
  erroT=fluido.erroT;
  pchange=fluido.pchange;
  dpchange=fluido.dpchange;
  ipchange=fluido.ipchange;
  tchange=fluido.tchange;
  dtchange=fluido.dtchange;
  itchange=fluido.itchange;

  modoParafina=fluido.modoParafina;
  oPressurePoints=fluido.oPressurePoints;
  oCloudPointTemperatures=fluido.oCloudPointTemperatures;
  oStructurePressures=fluido.oStructurePressures;
  oStructureTemperatures=fluido.oStructureTemperatures;
  oStructureDensities=fluido.oStructureDensities;
  oStructureLiqMW=fluido.oStructureLiqMW;
  oStructureCPWax=fluido.oStructureCPWax;
  oStructureThermCond=fluido.oStructureThermCond;
  oStructureWaxConcs=fluido.oStructureWaxConcs;
  oMolecularWeightsOfWaxComponents=fluido.oMolecularWeightsOfWaxComponents;
  oLiquidDensitiesOfWaxComponents=fluido.oLiquidDensitiesOfWaxComponents;

  npseudoWax=fluido.npseudoWax;
  iPresWax=fluido.iPresWax;
  iTempWax=fluido.iTempWax;
  npseudoWaxOut=fluido.npseudoWaxOut;
  coefViscWax=fluido.coefViscWax;

  iIERW=fluido.iIERW;
  iWaxComponentCountOutput=fluido.iWaxComponentCountOutput;
  dCloudPointTOutput=fluido.dCloudPointTOutput;
  dInterpolatedCPWaxOutput=fluido.dInterpolatedCPWaxOutput;
  dInterpolatedThermCondOutput=fluido.dInterpolatedThermCondOutput;
  dInterpolatedDensOutput=fluido.dInterpolatedDensOutput;
  dInterpolatedLiqMWOutput=fluido.dInterpolatedLiqMWOutput;

  if(npseudoWax>0){
	  oLiquidDensitiesOfWaxComponentsOutput=new double[npseudoWax];
	  oMolecularWeightsOfWaxComponentsOut=new double[npseudoWax];
	  oInterpolatedWaxConcsTDerivOutput=new double[npseudoWax];
	  oInterpolatedWaxConcs=new double[npseudoWax];
	  for(int j=0; j<npseudoWax;j++){
		  oLiquidDensitiesOfWaxComponentsOutput[j]=fluido.oLiquidDensitiesOfWaxComponentsOutput[j];
		  oMolecularWeightsOfWaxComponentsOut[j]=fluido.oMolecularWeightsOfWaxComponentsOut[j];
		  oInterpolatedWaxConcsTDerivOutput[j]=fluido.oInterpolatedWaxConcsTDerivOutput[j];
		  oInterpolatedWaxConcs[j]=fluido.oInterpolatedWaxConcs[j];
	  }
  }

  bASTM1 = 0.;
  bASTM2=0.;
  if((flashCompleto==0 || viscBlackOil==1)&&corrOM==0){
	  double rhol=141.5/(131.5 + API);
	  bASTM1 = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)));
	  bASTM2=log10l(log10l((LVisL/1)/rhol + 0.7));
  }

  nvecEmul=fluido.nvecEmul;
  BSWVec=fluido.BSWVec;
  emulVec=fluido.emulVec;
  npseudo=0;
  if(flashCompleto==2 || fluido.npseudo>0){
	//if(npseudo>0)delete fracMol;
	//if(npseudo>0)delete oCalculatedLiqComposition;
	//if(npseudo>0)delete oCalculatedVapComposition;
    npseudo=fluido.npseudo;
    fracMol=new double[npseudo];
	oCalculatedLiqComposition=new double[npseudo];
	oCalculatedVapComposition=new double[npseudo];
	/*memcpy(fracMol,fluido.fracMol, npseudo*sizeof(double));
	memcpy(oCalculatedLiqComposition,fluido.oCalculatedLiqComposition, npseudo*sizeof(double));
	memcpy(oCalculatedVapComposition,fluido.oCalculatedVapComposition, npseudo*sizeof(double));*/
	 for(int i=0;i<npseudo;i++){
		 fracMol[i]=fluido.fracMol[i];
		 oCalculatedLiqComposition[i]=fluido.oCalculatedLiqComposition[i];
		 oCalculatedVapComposition[i]=fluido.oCalculatedVapComposition[i];
	 }
  }
  else{
	  npseudo=0;
	  //fracMol=0;
	  //oCalculatedLiqComposition=0;
	  //oCalculatedVapComposition=0;
  }
  dCalculatedBubbleT=fluido.dCalculatedBubbleT;
  dCalculatedDewT=fluido.dCalculatedDewT;
  dCalculatedBeta=fluido.dCalculatedBeta;
  dCalculatedBubbleP=fluido.dCalculatedBubbleP;
  iCalculatedThermodynamicCondition=fluido.iCalculatedThermodynamicCondition;
  iCalculatedStockTankThermodynamicCondition=fluido.iCalculatedStockTankThermodynamicCondition;
  dLiquidPhaseMW=fluido.dLiquidPhaseMW;
  dVaporPhaseMW=fluido.dVaporPhaseMW;
  dVaporMassFraction=fluido.dVaporMassFraction;


  masMol=fluido.masMol;
  Pmol=0.;
  if(masMol!=0)
  for(int i=0;i<npseudo;i++)Pmol+=fracMol[i]*masMol[i];
  tempCrit=fluido.tempCrit;
  presCrit=fluido.presCrit;
  fatAcent=fluido.fatAcent;
  parac=fluido.parac;
  TIndepPeneloux=fluido.TIndepPeneloux;
  kij=fluido.kij;
  lij=fluido.lij;
  CpIGCoefs=fluido.CpIGCoefs;
  liqModel=fluido.liqModel;
  iIER_BubbleT=fluido.iIER_BubbleT;
  iIER_DewT=fluido.iIER_DewT;
  iIER_Flash=fluido.iIER_Flash;
  iIER_BubbleP=fluido.iIER_BubbleP;
  iIER=fluido.iIER;

  dStockTankVaporMassFraction=fluido.dStockTankVaporMassFraction;
  dStockTankLiquidDensity=fluido.dStockTankLiquidDensity;
  dStockTankVaporDensity=fluido.dStockTankVaporDensity;

  modoBlackTemp=0;

  parserie=fluido.parserie;
  viscTab=fluido.viscTab;
  tempTab=fluido.tempTab;

  int simulaTransienteTemp=0;
  if(vg1dSP!=0)simulaTransienteTemp=(*vg1dSP).simulaTransiente;
  if(flashCompleto==2 && npseudo>0 && simulaTransienteTemp==1){
	  	int dim=2;
	  	miniTabDin.rholF=new double*[dim];
	  	miniTabDin.rhogF=new double*[dim];
	  	miniTabDin.DrholDpF=new double*[dim];
	  	miniTabDin.DrhogDpF=new double*[dim];
	  	miniTabDin.DrholDtF=new double*[dim];
	  	miniTabDin.DrhogDtF=new double*[dim];
	  	miniTabDin.valBO=new double*[dim];
	  	miniTabDin.tit=new double*[dim];
	  	miniTabDin.rs=new double*[dim];
	  	miniTabDin.cplF=new double*[dim];
	  	miniTabDin.cpgF=new double*[dim];
	  	miniTabDin.valZ=new double*[dim];
	  	miniTabDin.HlF=new double*[dim];
	  	miniTabDin.HgF=new double*[dim];
	  	miniTabDin.valdZdT=new double*[dim];
	  	miniTabDin.valdZdP=new double*[dim];
	  	miniTabDin.PBF=new double[dim];
	  	miniTabDin.TBF=new double[dim];
		for(int k=0;k<dim;k++){
			miniTabDin.rholF[k]=new double[dim];
			miniTabDin.rhogF[k]=new double[dim];
			miniTabDin.DrholDpF[k]=new double[dim];
			miniTabDin.DrhogDpF[k]=new double[dim];
			miniTabDin.DrholDtF[k]=new double[dim];
			miniTabDin.DrhogDtF[k]=new double[dim];
			miniTabDin.valBO[k]=new double[dim];
			miniTabDin.tit[k]=new double[dim];
			miniTabDin.rs[k]=new double[dim];
			miniTabDin.cplF[k]=new double[dim];
			miniTabDin.cpgF[k]=new double[dim];
			miniTabDin.valZ[k]=new double[dim];
			miniTabDin.HlF[k]=new double[dim];
			miniTabDin.HgF[k]=new double[dim];
			miniTabDin.valdZdT[k]=new double[dim];
			miniTabDin.valdZdP[k]=new double[dim];
		}
		for (int k = 0; k < dim; k++) {
			for (int j = 0; j < dim; j++) {
				miniTabDin.rholF[k][j] = fluido.miniTabDin.rholF[k][j];
				miniTabDin.rhogF[k][j] = fluido.miniTabDin.rhogF[k][j];
				miniTabDin.DrholDpF[k][j] = fluido.miniTabDin.DrholDpF[k][j];
				miniTabDin.DrhogDpF[k][j] = fluido.miniTabDin.DrhogDpF[k][j];
				miniTabDin.DrholDtF[k][j] = fluido.miniTabDin.DrholDtF[k][j];
				miniTabDin.DrhogDtF[k][j] = fluido.miniTabDin.DrhogDtF[k][j];
				miniTabDin.valBO[k][j] = fluido.miniTabDin.valBO[k][j];
				miniTabDin.tit[k][j] = fluido.miniTabDin.tit[k][j];
				miniTabDin.rs[k][j] = fluido.miniTabDin.rs[k][j];
				miniTabDin.cplF[k][j] = fluido.miniTabDin.cplF[k][j];
				miniTabDin.cpgF[k][j] = fluido.miniTabDin.cpgF[k][j];
				miniTabDin.valZ[k][j] = fluido.miniTabDin.valZ[k][j];
				miniTabDin.HlF[k][j] = fluido.miniTabDin.HlF[k][j];
				miniTabDin.HgF[k][j] = fluido.miniTabDin.HgF[k][j];
				miniTabDin.valdZdT[k][j] = fluido.miniTabDin.valdZdT[k][j];
				miniTabDin.valdZdP[k][j] = fluido.miniTabDin.valdZdP[k][j];
			}
		}
		for (int k = 0; k < dim; k++) {
			miniTabDin.PBF[k] = fluido.miniTabDin.PBF[k];
			miniTabDin.TBF[k] = fluido.miniTabDin.TBF[k];
		}
		miniTabDin.pmax=fluido.miniTabDin.pmax;
		miniTabDin.pmin=fluido.miniTabDin.pmin;
		miniTabDin.tmax=fluido.miniTabDin.tmax;
		miniTabDin.tmin=fluido.miniTabDin.tmin;
  }
  else{
		miniTabDin.rholF = 0;
		miniTabDin.rhogF = 0;
		miniTabDin.DrholDpF = 0;
		miniTabDin.DrhogDpF = 0;
		miniTabDin.DrholDtF = 0;
		miniTabDin.DrhogDtF = 0;
		miniTabDin.valBO = 0;
		miniTabDin.tit = 0;
		miniTabDin.rs = 0;
		miniTabDin.cplF = 0;
		miniTabDin.cpgF = 0;
		miniTabDin.valZ = 0;
		miniTabDin.HlF = 0;
		miniTabDin.HgF = 0;
		miniTabDin.valdZdT = 0;
		miniTabDin.valdZdP = 0;
		miniTabDin.PBF = 0;
		miniTabDin.TBF = 0;
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
  }

}//alteracao2

ProFlu& ProFlu::operator =(const ProFlu& fluido){//alteracao2
  //iguala duas listas de primitivas
 if(this!=&fluido){//verifica se se est          igualando a mesma lista
   mascor=fluido.mascor;
   vg1dSP=fluido.vg1dSP;
   API=fluido.API;
   RGO=fluido.RGO;
   Deng=fluido.Deng;
   BSW=fluido.BSW;
   tipoemul=fluido.tipoemul;
   bswCorte=fluido.bswCorte;
   Denag=fluido.Denag;
   TempL=fluido.TempL;
   LVisL=fluido.LVisL;
   TempH=fluido.TempH;
   LVisH=fluido.LVisH;
   FAC=fluido.FAC;
   SG100vb=fluido.SG100vb;
   Avb=fluido.Avb;
   Bvb=fluido.Bvb;
   Cvb=fluido.Cvb;
   IRGO=fluido.IRGO;
   yco2=fluido.yco2;
   corrC=fluido.corrC;
   PC=fluido.PC;
   TC=fluido.TC;
   PCF=fluido.PCF;
   TCF=fluido.TCF;
  term1=fluido.term1;
  term2=fluido.term2;
  term3=fluido.term3;
  term4=fluido.term4;
  term5=fluido.term5;

  modelaAgua=fluido.modelaAgua;

  A0Liv=6.54269213;A1Liv=2.60464618;A2Liv=1.21334544;A3Liv=0.16464125;A4Liv=1.19382967;
  B0Liv=0.00000087;B1Liv=4.77390016;B2Liv=1.77267703;B3Liv=-0.73072038;B4Liv=1.58093857;
  C0Liv=0.00322609;C1Liv=0.09281075;C2Liv=0.13633665;C3Liv=0.09634381;C4Liv=0.53238728;



  tab=fluido.tab;
  zdranP=fluido.zdranP;
  dzdpP=fluido.dzdpP;
  dzdtP=fluido.dzdtP;

  cpg=fluido.cpg;
  cpl=fluido.cpl;
  drholdT=fluido.drholdT;

  npontos=fluido.npontos;
  npontosB=fluido.npontosB;

  nfluP=fluido.nfluP;

  ModelCp=fluido.ModelCp;
  Modeljtl=fluido.Modeljtl;
  CalcLat=fluido.CalcLat;
  trackRGO=fluido.trackRGO;
  trackDeng=fluido.trackDeng;
  corrDeng=fluido.corrDeng;
  ninjgas=fluido.ninjgas;
  lingas=fluido.lingas;

  //alteracao4
  BPPF[0]=0.17;BPPF[1]=0.30;BPPF[2]=0.43;BPPF[3]=0.58;BPPF[4]=0.75;BPPF[5]=0.94;BPPF[6]=1.19;BPPF[7]=1.47;BPPF[8]=1.74;
  BPPF[9]=2.10;BPPF[10]=2.70;BPPF[11]=3.29;BPPF[12]=3.80;BPPF[13]=4.30;BPPF[14]=4.90;BPPF[15]=5.70;BPPF[16]=6.70;

  GMF[0]=0.05;GMF[1]=0.10;GMF[2]=0.15;GMF[3]=0.20;GMF[4]=0.25;GMF[5]=0.30;GMF[6]=0.35;GMF[7]=0.40;GMF[8]=0.45;
  GMF[9]=0.50;GMF[10]=0.55;GMF[11]=0.60;GMF[12]=0.65;GMF[13]=0.70;GMF[14]=0.75;GMF[15]=0.80;GMF[16]=0.85;

  APIEMW[0]=5;APIEMW[1]=13;APIEMW[2]=19;APIEMW[3]=24;APIEMW[4]=28;APIEMW[5]=33;APIEMW[6]=38;
  APIEMW[7]=44;APIEMW[8]=52; APIEMW[9]=70;

  EMW[0]=600;EMW[1]=500;EMW[2]=450;EMW[3]=400;EMW[4]=350;EMW[5]=300;EMW[6]=250;EMW[7]=200;EMW[8]=150;
  EMW[9]=100;

  corrSat=fluido.corrSat;
  corrOM=fluido.corrOM;
  corrOV=fluido.corrOV;
  corrOS=fluido.corrOS;
  aemul=fluido.aemul;
  bemul=fluido.bemul;
  PHI100=fluido.PHI100;
  PBPVTSim=fluido.PBPVTSim;
  TBPVTSim=fluido.TBPVTSim;
  TabRSLivia=fluido.TabRSLivia;
 tabRSPB=fluido.tabRSPB;
 // if(corrSat==1)corrSat=4;

 flashCompleto=fluido.flashCompleto;
 tabelaDinamica=fluido.tabelaDinamica;
 indiceFlash=fluido.indiceFlash;
 rholF=fluido.rholF;
 rhogF=fluido.rhogF;modelaAgua=fluido.modelaAgua;
 DrholDpF=fluido.DrholDpF;
 DrhogDpF=fluido.DrhogDpF;
 DrholDtF=fluido.DrholDtF;
 DrhogDtF=fluido.DrhogDtF;
 DrhowDtF=fluido.DrhowDtF;
 RSF=fluido.RSF;
 cplF=fluido.cplF;
 cpgF=fluido.cpgF;
 cpwF=fluido.cpwF;
 HlF=fluido.HlF;
 HgF=fluido.HgF;
 sigOGF=fluido.sigOGF;
 sigWGF=fluido.sigWGF;
 viscO=fluido.viscO;
 viscG=fluido.viscG;
 //itempAnt=new int;
 itempAnt=0;
 //ipresAnt=new int;
 ipresAnt=0;
 //PBF=fluido.PBF;
 //TBF=fluido.TBF;
 viscBlackOil=fluido.viscBlackOil;
 id = fluido.id;

 rDgD=fluido.rDgD;
 rDgL=fluido.rDgL;
 PCis=fluido.PCis;
 TCis=fluido.TCis;

 pMinEqu=fluido.pMinEqu;
 delpTab=fluido.delpTab;
 indMinEquP=fluido.indMinEquP;

 tMinEqu=fluido.tMinEqu;
 deltTab=fluido.deltTab;
 indMinEquT=fluido.indMinEquT;
 erroP=fluido.erroP;
 erroT=fluido.erroT;

 pchange=fluido.pchange;
 dpchange=fluido.dpchange;
 ipchange=fluido.ipchange;
 tchange=fluido.tchange;
 dtchange=fluido.dtchange;
 itchange=fluido.itchange;

 JTLiquidoSimple=fluido.JTLiquidoSimple;

 modoParafina=fluido.modoParafina;
 oPressurePoints=fluido.oPressurePoints;
 oCloudPointTemperatures=fluido.oCloudPointTemperatures;
 oStructurePressures=fluido.oStructurePressures;
 oStructureTemperatures=fluido.oStructureTemperatures;
 oStructureDensities=fluido.oStructureDensities;
 oStructureLiqMW=fluido.oStructureLiqMW;
 oStructureCPWax=fluido.oStructureCPWax;
 oStructureThermCond=fluido.oStructureThermCond;
 oStructureWaxConcs=fluido.oStructureWaxConcs;
 oMolecularWeightsOfWaxComponents=fluido.oMolecularWeightsOfWaxComponents;
 oLiquidDensitiesOfWaxComponents=fluido.oLiquidDensitiesOfWaxComponents;
 npseudoWax=fluido.npseudoWax;
 if(modoParafina==1 && npseudoWax>0){
	delete[] oMolecularWeightsOfWaxComponentsOut;
	delete[] oInterpolatedWaxConcsTDerivOutput;
	delete[] oInterpolatedWaxConcs;
	delete[] oLiquidDensitiesOfWaxComponentsOutput;


	iPresWax=fluido.iPresWax;
	iTempWax=fluido.iTempWax;
	npseudoWaxOut=fluido.npseudoWaxOut;
	iIERW=fluido.iIERW;
	iWaxComponentCountOutput=fluido.iWaxComponentCountOutput;
	dCloudPointTOutput=fluido.dCloudPointTOutput;
	dInterpolatedCPWaxOutput=fluido.dInterpolatedCPWaxOutput;
	dInterpolatedThermCondOutput=fluido.dInterpolatedThermCondOutput;
	dInterpolatedDensOutput=fluido.dInterpolatedDensOutput;
	dInterpolatedLiqMWOutput=fluido.dInterpolatedLiqMWOutput;
	coefViscWax=fluido.coefViscWax;

	oLiquidDensitiesOfWaxComponentsOutput=new double[npseudoWax];
	oMolecularWeightsOfWaxComponentsOut=new double[npseudoWax];
	oInterpolatedWaxConcsTDerivOutput=new double[npseudoWax];
	oInterpolatedWaxConcs=new double[npseudoWax];
	for(int j=0; j<npseudoWax;j++){
		oLiquidDensitiesOfWaxComponentsOutput[j]=fluido.oLiquidDensitiesOfWaxComponentsOutput[j];
		oMolecularWeightsOfWaxComponentsOut[j]=fluido.oMolecularWeightsOfWaxComponentsOut[j];
		oInterpolatedWaxConcs[j]=fluido.oInterpolatedWaxConcs[j];
		oInterpolatedWaxConcsTDerivOutput[j]=fluido.oInterpolatedWaxConcsTDerivOutput[j];
	}
 }

 bASTM1 = 0.;
 bASTM2=0.;
 if((flashCompleto==0 || viscBlackOil==1)&&corrOM==0){
	  double rhol=141.5/(131.5 + API);
	  bASTM1 = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)));
	  bASTM2=log10l(log10l((LVisL/1)/rhol + 0.7));
 }

 nvecEmul=fluido.nvecEmul;
 BSWVec=fluido.BSWVec;
 emulVec=fluido.emulVec;

 parserie=fluido.parserie;
 viscTab=fluido.viscTab;
 tempTab=fluido.tempTab;

 int simulaTransienteTemp=0;
 if(vg1dSP!=0)simulaTransienteTemp=(*vg1dSP).simulaTransiente;
 if(flashCompleto==2 && npseudo>0 && simulaTransienteTemp==1){
		for(int j=0; j<2;j++){
			//if(miniTabDin.rhogF!=0)
				delete[] miniTabDin.rhogF[j];
			//if(miniTabDin.rholF!=0)
				delete[] miniTabDin.rholF[j];
			//if(miniTabDin.DrhogDpF!=0)
				delete[] miniTabDin.DrhogDpF[j];
			//if(miniTabDin.DrhogDtF!=0)
				delete[] miniTabDin.DrhogDtF[j];
			//if(miniTabDin.DrholDpF!=0)
				delete[] miniTabDin.DrholDpF[j];
			//if(miniTabDin.DrholDtF!=0)
				delete[] miniTabDin.DrholDtF[j];
			//if(miniTabDin.valBO!=0)
				delete[] miniTabDin.valBO[j];
			//if(miniTabDin.HgF!=0)
				delete[] miniTabDin.HgF[j];
			//if(miniTabDin.HlF!=0)
				delete[] miniTabDin.HlF[j];
			//if(miniTabDin.cpgF!=0)
				delete[] miniTabDin.cpgF[j];
			//if(miniTabDin.cplF!=0)
				delete[] miniTabDin.cplF[j];
			//if(miniTabDin.valZ!=0)
				delete[] miniTabDin.valZ[j];
			//if(miniTabDin.valdZdT!=0)
				delete[] miniTabDin.valdZdT[j];
			//if(miniTabDin.valdZdP!=0)
				delete[] miniTabDin.valdZdP[j];
			//if(miniTabDin.tit!=0)
				delete[] miniTabDin.tit[j];
			//if(miniTabDin.rs!=0)
				delete[] miniTabDin.rs[j];
		}
		//if(miniTabDin.rhogF!=0)
			delete[] miniTabDin.rhogF;
		//if(miniTabDin.rholF!=0)
			delete[] miniTabDin.rholF;
		//if(miniTabDin.DrhogDpF!=0)
			delete[] miniTabDin.DrhogDpF;
		//if(miniTabDin.DrhogDtF!=0)
			delete[] miniTabDin.DrhogDtF;
		//if(miniTabDin.DrholDpF!=0)
			delete[] miniTabDin.DrholDpF;
		//if(miniTabDin.DrholDtF!=0)
			delete[] miniTabDin.DrholDtF;
		//if(miniTabDin.valBO!=0)
			delete[] miniTabDin.valBO;
		//if(miniTabDin.HgF!=0)
			delete[] miniTabDin.HgF;
		//if(miniTabDin.HlF!=0)
			delete[] miniTabDin.HlF;
		//if(miniTabDin.cpgF!=0)
			delete[] miniTabDin.cpgF;
		//if(miniTabDin.cplF!=0)
			delete[] miniTabDin.cplF;
		//if(miniTabDin.valZ!=0)
			delete[] miniTabDin.valZ;
		//if(miniTabDin.valdZdT!=0)
			delete[] miniTabDin.valdZdT;
		//if(miniTabDin.valdZdP!=0)
			delete[] miniTabDin.valdZdP;
		//if(miniTabDin.tit!=0)
			delete[] miniTabDin.tit;
		//if(miniTabDin.rs!=0)
			delete[] miniTabDin.rs;
		//if(miniTabDin.TBF!=0)
			delete[] miniTabDin.TBF;
		//if(miniTabDin.PBF!=0)
			delete[] miniTabDin.PBF;
 }

 if(flashCompleto==2 || fluido.npseudo>0){
	 if(npseudo>0)delete fracMol;
	 if(npseudo>0)delete oCalculatedLiqComposition;
	 if(npseudo>0)delete oCalculatedVapComposition;
	 npseudo=fluido.npseudo;
	 fracMol=new double[npseudo];
	 oCalculatedLiqComposition=new double[npseudo];
	 oCalculatedVapComposition=new double[npseudo];
	 /*memcpy(fracMol,fluido.fracMol, npseudo*sizeof(double));
	 memcpy(oCalculatedLiqComposition,fluido.oCalculatedLiqComposition, npseudo*sizeof(double));
	 memcpy(oCalculatedVapComposition,fluido.oCalculatedVapComposition, npseudo*sizeof(double));*/
	 for(int i=0;i<npseudo;i++){
		 fracMol[i]=fluido.fracMol[i];
		 oCalculatedLiqComposition[i]=fluido.oCalculatedLiqComposition[i];
		 oCalculatedVapComposition[i]=fluido.oCalculatedVapComposition[i];
	 }
 }
 else{
	  npseudo=0;
	  //fracMol=0;
	  //oCalculatedLiqComposition=0;
	  //oCalculatedVapComposition=0;
 }
 dCalculatedBubbleT=fluido.dCalculatedBubbleT;
 dCalculatedDewT=fluido.dCalculatedDewT;
 dCalculatedBeta=fluido.dCalculatedBeta;
 dCalculatedBubbleP=fluido.dCalculatedBubbleP;
 iCalculatedThermodynamicCondition=fluido.iCalculatedThermodynamicCondition;
 iCalculatedStockTankThermodynamicCondition=fluido.iCalculatedStockTankThermodynamicCondition;
 dLiquidPhaseMW=fluido.dLiquidPhaseMW;
 dVaporPhaseMW=fluido.dVaporPhaseMW;
 dVaporMassFraction=fluido.dVaporMassFraction;

 masMol=fluido.masMol;
 Pmol=0.;
 if(masMol!=0)
 for(int i=0;i<npseudo;i++)Pmol+=fracMol[i]*masMol[i];
 tempCrit=fluido.tempCrit;
 presCrit=fluido.presCrit;
 fatAcent=fluido.fatAcent;
 parac=fluido.parac;
 TIndepPeneloux=fluido.TIndepPeneloux;
 kij=fluido.kij;
 lij=fluido.lij;
 CpIGCoefs=fluido.CpIGCoefs;
 liqModel=fluido.liqModel;
 iIER_BubbleT=fluido.iIER_BubbleT;
 iIER_DewT=fluido.iIER_DewT;
 iIER_Flash=fluido.iIER_Flash;
 iIER_BubbleP=fluido.iIER_BubbleP;
 iIER=fluido.iIER;

 dStockTankVaporMassFraction=fluido.dStockTankVaporMassFraction;
 dStockTankLiquidDensity=fluido.dStockTankLiquidDensity;
 dStockTankVaporDensity=fluido.dStockTankVaporDensity;
  //alteracao4

 if(flashCompleto==2 /*&& npseudo>0*/ && simulaTransienteTemp==1){
	  	int dim=2;
	  	miniTabDin.rholF=new double*[dim];
	  	miniTabDin.rhogF=new double*[dim];
	  	miniTabDin.DrholDpF=new double*[dim];
	  	miniTabDin.DrhogDpF=new double*[dim];
	  	miniTabDin.DrholDtF=new double*[dim];
	  	miniTabDin.DrhogDtF=new double*[dim];
	  	miniTabDin.valBO=new double*[dim];
	  	miniTabDin.tit=new double*[dim];
	  	miniTabDin.rs=new double*[dim];
	  	miniTabDin.cplF=new double*[dim];
	  	miniTabDin.cpgF=new double*[dim];
	  	miniTabDin.valZ=new double*[dim];
	  	miniTabDin.HlF=new double*[dim];
	  	miniTabDin.HgF=new double*[dim];
	  	miniTabDin.valdZdT=new double*[dim];
	  	miniTabDin.valdZdP=new double*[dim];
	  	miniTabDin.PBF=new double[dim];
	  	miniTabDin.TBF=new double[dim];
		for(int k=0;k<dim;k++){
			miniTabDin.rholF[k]=new double[dim];
			miniTabDin.rhogF[k]=new double[dim];
			miniTabDin.DrholDpF[k]=new double[dim];
			miniTabDin.DrhogDpF[k]=new double[dim];
			miniTabDin.DrholDtF[k]=new double[dim];
			miniTabDin.DrhogDtF[k]=new double[dim];
			miniTabDin.valBO[k]=new double[dim];
			miniTabDin.tit[k]=new double[dim];
			miniTabDin.rs[k]=new double[dim];
			miniTabDin.cplF[k]=new double[dim];
			miniTabDin.cpgF[k]=new double[dim];
			miniTabDin.valZ[k]=new double[dim];
			miniTabDin.HlF[k]=new double[dim];
			miniTabDin.HgF[k]=new double[dim];
			miniTabDin.valdZdT[k]=new double[dim];
			miniTabDin.valdZdP[k]=new double[dim];
		}
		for (int k = 0; k < dim; k++) {
			for (int j = 0; j < dim; j++) {
				miniTabDin.rholF[k][j] = fluido.miniTabDin.rholF[k][j];
				miniTabDin.rhogF[k][j] = fluido.miniTabDin.rhogF[k][j];
				miniTabDin.DrholDpF[k][j] = fluido.miniTabDin.DrholDpF[k][j];
				miniTabDin.DrhogDpF[k][j] = fluido.miniTabDin.DrhogDpF[k][j];
				miniTabDin.DrholDtF[k][j] = fluido.miniTabDin.DrholDtF[k][j];
				miniTabDin.DrhogDtF[k][j] = fluido.miniTabDin.DrhogDtF[k][j];
				miniTabDin.valBO[k][j] = fluido.miniTabDin.valBO[k][j];
				miniTabDin.tit[k][j] = fluido.miniTabDin.tit[k][j];
				miniTabDin.rs[k][j] = fluido.miniTabDin.rs[k][j];
				miniTabDin.cplF[k][j] = fluido.miniTabDin.cplF[k][j];
				miniTabDin.cpgF[k][j] = fluido.miniTabDin.cpgF[k][j];
				miniTabDin.valZ[k][j] = fluido.miniTabDin.valZ[k][j];
				miniTabDin.HlF[k][j] = fluido.miniTabDin.HlF[k][j];
				miniTabDin.HgF[k][j] = fluido.miniTabDin.HgF[k][j];
				miniTabDin.valdZdT[k][j] = fluido.miniTabDin.valdZdT[k][j];
				miniTabDin.valdZdP[k][j] = fluido.miniTabDin.valdZdP[k][j];
			}
		}
		for (int k = 0; k < dim; k++) {
			miniTabDin.PBF[k] = fluido.miniTabDin.PBF[k];
			miniTabDin.TBF[k] = fluido.miniTabDin.TBF[k];
		}
		miniTabDin.pmax=fluido.miniTabDin.pmax;
		miniTabDin.pmin=fluido.miniTabDin.pmin;
		miniTabDin.tmax=fluido.miniTabDin.tmax;
		miniTabDin.tmin=fluido.miniTabDin.tmin;
 }
 else{
		miniTabDin.rholF = 0;
		miniTabDin.rhogF = 0;
		miniTabDin.DrholDpF = 0;
		miniTabDin.DrhogDpF = 0;
		miniTabDin.DrholDtF = 0;
		miniTabDin.DrhogDtF = 0;
		miniTabDin.valBO = 0;
		miniTabDin.tit = 0;
		miniTabDin.rs = 0;
		miniTabDin.cplF = 0;
		miniTabDin.cpgF = 0;
		miniTabDin.valZ = 0;
		miniTabDin.HlF = 0;
		miniTabDin.HgF = 0;
		miniTabDin.valdZdT = 0;
		miniTabDin.valdZdP = 0;
		miniTabDin.PBF = 0;
		miniTabDin.TBF = 0;
		miniTabDin.pmax=0.;
		miniTabDin.pmin=0.;
		miniTabDin.tmax=0.;
		miniTabDin.tmin=0.;
 }
 }
 return *this;
}//alteracao2

double ProFlu::operator[](int i) const{//sobrecarga do operador []
 if(i==0)return mascor;
 else if(i==1)return API;
 else if(i==2)return RGO;
 else if(i==3)return Deng;
 else if(i==4)return BSW;
 else if(i==5)return Denag;
 else if(i==6)return TempL;
 else if(i==7)return LVisL;
 else if(i==8)return TempH;
 else if(i==9)return LVisH;
 else return -100.;

 }

ProFlu ProFlu::operator+(const ProFlu& fluido) const{
 //A funcao apresentada a seguir obtem para uma determinada soma de correntes  a RGO (m3/m3) da mistura,
 //o API da mistura, a densidade do gas da mistura e para o metodo ASTM, temperatura mais baixa ,
 //temperatura mais alta, viscosidade (cp) e temperatura mais baixa e viscosidade (cp) e temperatura mais alta.

 //soma de duas correntes de propriedades
 if(fabsl(mascor)<1e-9)return fluido;
 else if (fabsl(fluido.mascor)<1e-9)return *this;
 else{
      //Procura correntes onde so ha injecaoo de gas,estas sao rotuladas por uma RGO negativa
      double multo1=1;
      double multo2=1;
      if(RGO < 0) multo1=0;
      if(fluido.RGO<0)multo2=0;

      //massa especifica do oleo morto de cada corrente : kg/m3
      double masesp[] = {1000*141.5/(131.5 + API),1000*141.5/(131.5 + fluido.API)};
      //Vazao volumetrica de oleo nas condicoes standard : m3/s
      double qocor[] = {multo1*mascor/(RGO*Deng*1.29 + masesp[0]+Denag*1000*BSW/(1-BSW)),
                multo2*fluido.mascor/(fluido.RGO*fluido.Deng*1.29 + masesp[1]+
                fluido.Denag*1000*fluido.BSW/(1-fluido.BSW))
                };
      //Vazao volumetrica de agua nas condicoes standard : m3/s
      double qacor[] = {multo1*qocor[0]*BSW/(1-BSW),
                multo2*qocor[1]*fluido.BSW/(1-fluido.BSW)};
      //Vazao volumetrica de gas nas condicoes standard : m3/s
      double qgcor[] = {multo1*RGO*qocor[0]+(1-multo1)*mascor/(1.29*Deng),
                multo2*fluido.RGO*qocor[1]+(1-multo2)*fluido.mascor/(1.29*fluido.Deng)};
      //Vazao massica de oleo morto, kg/s
      double mocor[] = {qocor[0]*masesp[0],qocor[1]*masesp[1]};
      //Vazao massica de agua kg/s
      double macor[] = {qacor[0]*1000*Denag,qacor[1]*1000*fluido.Denag};
      //Vazao massica de gas kg/s
      double mgcor[] = {1.29*Deng*qgcor[0],1.29*fluido.Deng*qgcor[1]};

      //calculo do RGO da mistura
      double RGOmis;
      double somao;
      double somag;
      if(multo1>0.1||multo2>0.1){
        somag = qgcor[0]+qgcor[1];
        somao = (multo1)*qocor[0]+(multo2)*qocor[1];
        RGOmis=somag/somao;
      }
      else RGOmis=-1;//quando nao ha corrente de oleo

      double fracoleo[]={0,0};//fracao volumetrica de cada corrente na corrente total
      if(RGOmis>0){
       fracoleo[0]=qocor[0]/somao;
       fracoleo[1]=qocor[1]/somao;
      }

      double DENGmis=((mgcor[0]+mgcor[1])/(qgcor[0]+qgcor[1]))/1.29;//densidade do gas da mistura
      double APImis=-1;
      double Denagmis=-1;
      double BSWmis=-1;
      if(RGOmis>0){
       APImis=1000*141.5/((mocor[0]+mocor[1])/somao)-131.5;//API da mistura
       if(fluido.BSW>0.||BSW>0.)Denagmis=((macor[0]+macor[1])/((qacor[0]+qacor[1])*1000));//densidade da agua da mistura
       else Denagmis=1.;
       BSWmis=(qacor[0]+qacor[1])/(qacor[0]+qacor[1]+somao);//BSW da mistura
      }

     //Calculo das viscosidades de cada fluido na corrente para as novas
     //temperaturas do metodo ASTM : viscosidade a 20 e viscosidade a 40

      double vis2040A[]={0,0};
      double vis2040B[]={0,0};
      if(multo1>0){
       vis2040A[0]=VisOM(20);
       vis2040A[1]=VisOM(40);
      }
      if(multo2>0){
       vis2040B[0]=fluido.VisOM(20);
       vis2040B[1]=fluido.VisOM(40);
      }
      double TempLmis=20;
      double TempHmis=40;
      double LVisLmis=vis2040A[0]*fracoleo[0]+vis2040B[0]*fracoleo[1];//viscosidade da mistura obtida a partir de ponderacao volumetrica
      double LVisHmis=vis2040A[1]*fracoleo[0]+vis2040B[1]*fracoleo[1];

      //ProFlu somafluidos(mascor+fluido.mascor,APImis,RGOmis,DENGmis,BSWmis,Denagmis,TempLmis,LVisLmis,TempHmis,LVisHmis);
      //return somafluidos;
      return 0;
     }

}

double ProFlu::interpolaVarProd(double pres, double temp, double** Var/*,int renovaInd*/) const{
    int ipres;
    int itemp;
    int ipmarcador;
    int itmarcador;
    double varprop;
    int ndiv=npontos-1;
    if(pres<Var[1][0] || pres>=Var[ndiv+1][0] || temp<Var[0][1] || temp>=Var[0][ndiv+1]){
    	varprop=1.225*Deng;
    	if(pres>=Var[ndiv+1][0]|| temp<Var[0][1] || temp>=Var[0][ndiv+1]){
            // incluir falha
            string complementoFalha = string("Limite alto na tabela ") + to_string((*vg1dSP).contaExit);
            logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Pressão ou temperatura excedeu o limite máximo da tabela de Flash", "N/A",
                complementoFalha);
            // gravar arquivo de log
            logger.writeOutputLog();
            // encerrar a aplicacao
            exit(EXIT_SUCCESS);
    		int para;
    		para++;
    	}
    }

    else{
      int e, m, d;
      if(pres>=Var[indMinEquP][0] && erroP==0){
          //double pmin=Var[indMinEquP][0];
          double delp=delpTab;
          int busca=0;
          //int busca=(*ipresAnt);
          while(pres>pchange[busca])busca++;
          double pmax=Var[ipchange[busca]][0];
          //ipres=floor((pres-pmin)/delp)+indMinEquP;
          ipres=ipchange[busca]-ceil((pmax-pres)/dpchange[busca]);
          if(pres<Var[ipres][0])ipres--;
      }
      else{
        e = 1;
    	//if((*ipresAnt)>0)e=(*ipresAnt);
        if(erroP==1)d = ndiv+1;
        else d = indMinEquP;
        ipres=d;
        while (e <= d) {
	      m = (e + d)/2;
	      ipmarcador=m;
 	      if(m==1){
 	    	  ipres=m;
 	    	  break;
 	      }
 	      else if (m==ndiv+1 && Var[m][0] == pres){
 	    	ipres=m-1;
 	    	break;
 	      }
	      if (Var[m][0] > pres && Var[m-1][0]<=pres){
	    	  ipres=m-1;
	    	  break;
	      }
	      if (Var[m][0] < pres) e = m + 1;
	      else d = m - 1;
	    }
      }
      if(temp>=Var[0][indMinEquT] && erroT==0){
          //double tmin=Var[0][indMinEquT];
          double delt=deltTab;
          int busca=0;
          //int busca=(*itempAnt);
          while(temp>tchange[busca])busca++;
          double tmax=Var[0][itchange[busca]];
          //itemp=floor((temp-tmin)/delt)+indMinEquT;
          itemp=itchange[busca]-ceil((tmax-temp)/dtchange[busca]);
          if(temp<Var[0][itemp])itemp--;
      }
      else{
        e = 1;
        //if((*itempAnt)>0)e=*itempAnt;
        if(erroT==1)d = ndiv+1;
        else d = indMinEquT;
        itemp=d;
        while (e <= d) {
 	      m = (e + d)/2;
 	      itmarcador=m;
 	      if(m==1){
 	    	  itemp=m;
 	    	  break;
 	      }
 	      else if (m==ndiv+1 && Var[0][m] == temp){
 	    	itemp=m-1;
 	    	break;
 	      }
 	      if (Var[0][m] > temp && Var[0][m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (Var[0][m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	    }
      }
      double razpres=(Var[ipres][0]-pres)/(Var[ipres][0]-Var[ipres+1][0]);
      double raztemp=(Var[0][itemp]-temp)/(Var[0][itemp]-Var[0][itemp+1]);
      double varp1=(1-razpres)*(Var[ipres][itemp])+razpres*(Var[ipres+1][itemp]);
      double varp2=(1-razpres)*(Var[ipres][itemp+1])+razpres*(Var[ipres+1][itemp+1]);
      varprop=(1-raztemp)*varp1+raztemp*varp2;
     }
   /* if(renovaInd>0){
    	(*itempAnt)=itemp;
    	if((*itempAnt)>1)(*itempAnt)-=2;
    	(*ipresAnt)=ipres;
    	if((*ipresAnt)>1)(*ipresAnt)-=2;
    }*/
    return varprop;
}


/*double ProFlu::interpolaVarProd(double pres, double temp, double** Var) const{
    int ipres;
    int itemp;
    int ipmarcador;
    int itmarcador;
    double varprop;
    int ndiv=npontos-1;
    double razpres;
    double raztemp;
    int e, m, d;

    if(pres<Var[1][0] || pres>=Var[ndiv+1][0] || temp<Var[0][1] || temp>=Var[0][ndiv+1]) varprop=0.;

    if(pres>=Var[1][0] || pres<Var[ndiv+1][0]){
      e = 1;
      d = ndiv+1;
      while (e <= d) {
	      m = (e + d)/2;
	      ipmarcador=m;
 	      if(m==1){
 	    	  ipres=m;
 	    	  break;
 	      }
 	      else if (m==ndiv+1 && Var[m][0] == pres){
 	    	ipres=m-1;
 	    	break;
 	      }
	      if (Var[m][0] > pres && Var[m-1][0]<=pres){
	    	  ipres=m-1;
	    	  break;
	      }
	      if (Var[m][0] < pres) e = m + 1;
	      else d = m - 1;
	  }
      razpres=(Var[ipres][0]-pres)/(Var[ipres][0]-Var[ipres+1][0]);
    }
    else if(pres<Var[1][0]){
    	ipres=1;
    	razpres=0.;
    }
    else{
    	ipres=ndiv;
    	razpres=1.;
    }

    if(temp>=Var[0][1] || temp<Var[0][ndiv+1]){
      e = 1;
      d = ndiv+1;
      while (e <= d) {
 	      m = (e + d)/2;
 	      itmarcador=m;
 	      if(m==1){
 	    	  itemp=m;
 	    	  break;
 	      }
 	      else if (m==ndiv+1 && Var[0][m] == temp){
 	    	itemp=m-1;
 	    	break;
 	      }
 	      if (Var[0][m] > temp && Var[0][m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (Var[0][m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	  }
      raztemp=(Var[0][itemp]-temp)/(Var[0][itemp]-Var[0][itemp+1]);
    }
    else if(temp<Var[0][1]){
    	itemp=1;
    	raztemp=0.;
    }
    else{
    	itemp=ndiv;
    	raztemp=1.;
    }
      double varp1=(1-razpres)*(Var[ipres][itemp])+razpres*(Var[ipres+1][itemp]);
      double varp2=(1-razpres)*(Var[ipres][itemp+1])+razpres*(Var[ipres+1][itemp+1]);
      varprop=(1-raztemp)*varp1+raztemp*varp2;

    return varprop;
}*/

void ProFlu::RenovaFluido() {
	  FAC = 1 + 0.5912*API*(1.8*60 + 32)*0.0001*log10l(147/114.7);
	  SG100vb = FAC*Deng;
	  if(API > 30){
	   Avb = 1.187;
	   Bvb = 56.06;
	   Cvb = 10.393;
	  }
	  else{
	   Avb = 1.0937;
	   Bvb = 27.64;
	   Cvb = 11.172;
	  }

	  IRGO=RGO*35.31467/6.29;

	  if(corrC==0){
	    if(Deng<=0.7){
	      PC = 708.75 - 57.5*Deng;
	      TC = 169 + 314.0*Deng;
	    }
	    else{
		  PC = 708.75 - 57.5*0.7;
		  TC = 169 + 314.0*0.7;
	    }
	  }
	  else if(corrC==1){
		  double sggHC=0.;
		  if(yco2<1.)sggHC=(Deng-1.52*yco2)/(1.-yco2);
		  double PCHC=48.86659-3.96448528*sggHC;
		  double TCHC=93.889+174.44*sggHC;
		  PC=(1.-yco2)*PCHC+73.8*yco2;
		  TC=(1.-yco2)*TCHC+304.1*yco2;
		  double ES=0.;
		  if(yco2>0.0)ES=(120./1.8)*pow(yco2,0.9)-pow(yco2,1.6);
		  PC*=(1.-ES/TC);
		  TC-=ES;
		  PC=psia(PC);
		  TC=Faren(TC-273.15)+460.;
	  }
	  else if(corrC==2){
	     double k=3.8216+17.438*Deng-3.2191*(Deng*Deng)-7.0485*yco2;
	     double j=0.11582+0.70729*Deng-0.099397*(Deng*Deng)-0.46185*yco2;
	     TC=k*k/(1.8*j);
	     PC=0.124105621*TC/j;
		 PC=psia(PC);
		 TC=Faren(TC-273.15)+460.;
	  }

	  double delaux= BSW*1000.*Denag+(1-BSW)*(141.5*1000./(131.5+API));
	  double degaux=1.29*Deng;

	  term1=-degaux*RGO*(1-BSW)/(delaux+(1-BSW)*RGO*degaux);
	  term2=-2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*degaux*RGO)*352.9797*Deng);
	  term3=2.5334*SG100vb*degaux*(1-BSW)/(Bvb*(delaux+(1-BSW)*RGO*degaux));
	  term4=degaux*RGO*(1-BSW)/((delaux+(1-BSW)*RGO*degaux)*352.9797*Deng);
	  term5=API*Cvb;

	  if((flashCompleto==0 || viscBlackOil==1)&&corrOM==0){
	 	  double rhol=141.5/(131.5 + API);
	 	  bASTM1 = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)));
	 	  bASTM2=log10l(log10l((LVisL/1)/rhol + 0.7));
	  }

	  if(flashCompleto!=2)dStockTankVaporMassFraction=FracMass(1.0, 20.);
}

void ProFlu::PcTcIS(){
	  double dgis=rDgL*Deng;
	  if(corrC==0){
	    if(dgis<=0.7){
	      PCis = 708.75 - 57.5*dgis;
	      TCis = 169 + 314.0*dgis;
	    }
	    else{
		  PCis = 708.75 - 57.5*0.7;
		  TCis = 169 + 314.0*0.7;
	    }
	  }
	  else if(corrC==1){
		  double sggHC=0.;
		  if(yco2<1.)sggHC=(dgis-1.52*yco2)/(1.-yco2);
		  double PCHC=48.86659-3.96448528*sggHC;
		  double TCHC=93.889+174.44*sggHC;
		  PCis=(1.-yco2)*PCHC+73.8*yco2;
		  TCis=(1.-yco2)*TCHC+304.1*yco2;
		  double ES=0.;
		  if(yco2>0.0)ES=(120./1.8)*pow(yco2,0.9)-pow(yco2,1.6);
		  PCis*=(1.-ES/TCis);
		  TCis-=ES;
		  PCis=psia(PCis);
		  TCis=Faren(TCis-273.15)+460.;
	  }
	  else if(corrC==2){
	     double k=3.8216+17.438*dgis-3.2191*(dgis*dgis)-7.0485*yco2;
	     double j=0.11582+0.70729*dgis-0.099397*(dgis*dgis)-0.46185*yco2;
	     TCis=k*k/(1.8*j);
	     PCis=0.124105621*TCis/j;
		 PCis=psia(PCis);
		 TCis=Faren(TCis-273.15)+460.;
	  }
}

double ProFlu::VisAgua(double temper) const{
	  double vis= 2.414E-05*pow(10.0,247.8/(temper + 133.15));
	  return vis*1000.;
}

double ProFlu::VisOM(double temper) const{
//Viscosidade de oleo morto pelo metodo ASTM
//Viscosidade de oleo morto em funcao da temperatura em Celcius, viscosidade em cP

 if(flashCompleto==0 || flashCompleto==3 || viscBlackOil==1){
   if(corrOM==0){
	 //ASTM
     double rhol=1*(141.5/(131.5 + API));

     //double bASTM = (log10l(log10l((LVisL/1)/rhol + 0.7)) - log10l(log10l((LVisH/1)/rhol + 0.7)))/
    		 //log10l((TempL + 273)/(TempH + 273));
     double bASTM = bASTM1/log10l((TempL + 273)/(TempH + 273));

     //double visASTM=rhol*(pow(10,pow(10,(bASTM*log10l((temper + 273)/
    		 //(TempL + 273))+log10l(log10l((LVisL/1)/rhol + 0.7)))))-0.7);
     double visASTM=rhol*(pow(10,pow(10,(bASTM*log10l((temper + 273)/
         		 (TempL + 273))+bASTM2)))-0.7);
     return visASTM;
   }
   else{
  // Beggs_Robinson
    double x;
    double y;
    double z;
    double t=Faren(temper);
    if(corrOM==1){
	  double sgo=141.5/(131.5+API);
	  if(t<50)t=50;
      z = 3.0324 - 0.02023*API;
      //y = pow(10.0,z);
      y=exp(13.108-6.591/sgo);
      x = y*pow(t,-1.163);
      double visc=pow(10.0,x) - 1.0;
      return visc;
    }
   //Beggs_Robinson_Modificado
    else if(corrOM==2){
     if(t<0)t=0;
     z  = 1.8653 - 0.025086*API;
     y  = pow(10.0,z);
     x  = y*pow(t,-0.5644);
     double visc=pow(10.0,x) - 1.0;
     return visc;
    }
    else if(corrOM==3){

     //Glaso
     x  = 10.313*log10(t) - 36.447;
     y  = log10(API);
     double visc=3.141E+10*pow(t,-3.444)*pow(y,x);
     return visc;
    }
    else if(corrOM==4){

     //Kartoatmodjo_Schmidt
	    x = 5.7526*log10(t) - 26.9718;
	    y = log10(API);
	    double visc=16.0E+08*pow(t,-2.8177)*pow(y,x);
	    return visc;
    }
    else if(corrOM==5){

     //Petrosky_Farshad
	    x = 4.59388*log10(t) - 22.82792;
	    y = log10(API);
	    double visc=2.3511E+07*pow(t,-2.10255)*pow(y,x);
	    return visc;
    }
    else if(corrOM==6){

     //Beal
	    z = pow(10.0,0.43 + 8.33/API);
	    y = pow(360.0/(t + 199.67),z);
	    x = 0.32 + 1.8E+07*pow(API,-4.53);
	    double visc=x*y;
	    return visc;
    }
    else if(corrOM==7){

     //tabela
    	double viso;
		if(temper<=tempTab[0]) viso=viscTab[0];
		else if(temper>=tempTab[parserie-1]) viso=viscTab[parserie-1];
		else{
			int indMin=0;
			int confere=-1;
			while(confere<0){
				if(temper>tempTab[indMin] && temper<=tempTab[indMin+1])confere=1;
				else indMin++;
			}
			double raz=(temper-tempTab[indMin])/(tempTab[indMin+1]-tempTab[indMin]);
			viso=(1.-raz)*viscTab[indMin]+raz*tempTab[indMin+1];
		}
		return viso;
    }
   }
 }
 else{
	 return interpolaVarProd(1.033210485, temper, viscO);
 }


}

double ProFlu::interpolaLas1( double var)const{//alteracao4
  //GMF,BPPF
  int comp=GMF.size();
  int para=0;
  int pos=0;
  while(para==0){
    if(pos==comp-1)
      para=1;
    else if(var>=GMF[pos] && var<GMF[pos+1])
      para=1;
    else pos++;
  }
  if(pos==comp-1)return BPPF[comp-1];
  else{
    double raz=(var-GMF[pos])/(GMF[pos+1]-GMF[pos]);
    return raz*BPPF[pos+1]+(1-raz)*BPPF[pos];
  }
}
double ProFlu::interpolaLas2(double var)const{//alteracao4
  //BPPF, GMF
  int comp=BPPF.size();
  int para=0;
  int pos=0;
  while(para==0){
    if(pos==comp-1)
      para=1;
    else if(var>=BPPF[pos] && var<BPPF[pos+1])
      para=1;
    else pos++;
  }
  if(pos==comp-1)return GMF[comp-1];
  else{
    double raz=(var-BPPF[pos])/(BPPF[pos+1]-BPPF[pos]);
    return raz*GMF[pos+1]+(1-raz)*GMF[pos];
  }
}
double ProFlu::interpolaLas3( double var)const{//alteracao4
  //APIEMW, EMW
  int comp=APIEMW.size();
  int para=0;
  int pos=0;
  while(para==0){
    if(pos==comp-1)
      para=1;
    else if(var>=APIEMW[pos] && var<APIEMW[pos+1])
      para=1;
    else pos++;
  }
  if(pos==comp-1)return EMW[comp-1];
  else{
    double raz=(var-APIEMW[pos])/(APIEMW[pos+1]-APIEMW[pos]);
    return raz*EMW[pos+1]+(1-raz)*EMW[pos];
  }
}

/*double ProFlu::interpolaPB( double temp)const{//alteracao4

    int itemp;
    int itmarcador;
    double var;
    int ndiv=npontos-1;
    //if(temp<TBPVTSim[0] || temp>=TBPVTSim[npontos-1]) var=0.;
   // else
   // pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb)
    if((temp-TBPVTSim[0])<1e-10)return PBPVTSim[0];
    else if((temp-TBPVTSim[ndiv])>1e-10)return PBPVTSim[ndiv];
    else{

      if(temp>=TBPVTSim[indMinEquT] && erroT==0){


            //double tmin=Var[0][indMinEquT];
            double delt=deltTab;
            int busca=0;
            while(temp>tchange[busca])busca++;
            double tmax=TBPVTSim[itchange[busca]-1];
            //itemp=floor((temp-tmin)/delt)+indMinEquT;
            itemp=itchange[busca]-ceil((tmax-temp)/dtchange[busca])-1;
            if(temp<TBPVTSim[itemp])itemp--;
      }
      else{

        int e, m, d;
        e = 0;
        if(erroT==1)
        	d = ndiv;
        else d = indMinEquT-1;
        itemp=d;
        while (e <= d) {
 	      m = (e + d)/2;
 	      if(m==0){
 	    	  itemp=m;
 	    	  break;
 	      }
 	      else if (m==ndiv && TBPVTSim[m] == temp){
 	    	itemp=m-1;
 	    	break;
 	      }
 	      itmarcador=m;
 	      if (TBPVTSim[m] > temp && TBPVTSim[m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (TBPVTSim[m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	    }
      }
      double raztemp=(temp-TBPVTSim[itemp])/(TBPVTSim[itemp+1]-TBPVTSim[itemp]);
      var=(1-raztemp)*PBPVTSim[itemp]+raztemp*PBPVTSim[itemp+1];

      //var=(1-raztemp)*pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/(Faren(TBPVTSim[itemp])+460))),1/Avb)+
    		  //raztemp*pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/(Faren(TBPVTSim[itemp+1])+460))),1/Avb);
     }
    return var;


}*/


double ProFlu::interpolaPB( double temp)const{//alteracao4

    int itemp;
    int itmarcador;
    double var;
    int ndiv=npontosB-1;
    //if(temp<TBPVTSim[0] || temp>=TBPVTSim[npontos-1]) var=0.;
   // else
   // pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb)
    if((temp-TBPVTSim[0])<1e-10)return PBPVTSim[0];
    else if((temp-TBPVTSim[ndiv])>1e-10)return PBPVTSim[ndiv];
    else{
        int e, m, d;
        e = 0;
        d = ndiv;
        itemp=d;
        while (e <= d) {
 	      m = (e + d)/2;
 	      if(m==0){
 	    	  itemp=m;
 	    	  break;
 	      }
 	      else if (m==ndiv && TBPVTSim[m] == temp){
 	    	itemp=m-1;
 	    	break;
 	      }
 	      itmarcador=m;
 	      if (TBPVTSim[m] > temp && TBPVTSim[m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (TBPVTSim[m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	    }
        double raztemp=(temp-TBPVTSim[itemp])/(TBPVTSim[itemp+1]-TBPVTSim[itemp]);
        var=(1-raztemp)*PBPVTSim[itemp]+raztemp*PBPVTSim[itemp+1];

    }
    return var;


}

double ProFlu::interpolaRS( double pres, double temp)const{//alteracao4
  int comp=npontos;

  double tmax=TabRSLivia[0][npontos];
  double tmin=TabRSLivia[0][1];
  double delt=TabRSLivia[0][2]-TabRSLivia[0][1];
  int itemp=floor((temp-tmin)/delt)+2;

  double pmax=TabRSLivia[npontos][0];
  double pmin=TabRSLivia[1][0];
  double delp=TabRSLivia[npontos][0]-TabRSLivia[npontos-1][0];
  int ipres=floor((pmax-pres)/delp)+1;
  ipres=npontos-ipres;

  double razpres=(pres-TabRSLivia[ipres][0])/(TabRSLivia[ipres+1][0]-TabRSLivia[ipres][0]);
  double raztemp=(temp-TabRSLivia[0][itemp])/(TabRSLivia[0][itemp+1]-TabRSLivia[0][itemp]);

  double medT1= razpres*TabRSLivia[ipres+1][itemp]+(1-razpres)*TabRSLivia[ipres][itemp];
  double medT2= razpres*TabRSLivia[ipres+1][itemp+1]+(1-razpres)*TabRSLivia[ipres][itemp+1];

  return raztemp*medT2+(1-raztemp)*medT1;

}

double ProFlu::RS(double pres, double temp, double varPb) const{//alteracao4
//Calcula Rs para uma  press         o em kgf/cm2 e temperatura em Celcius
// Rs em ft3 por barris


 double ipres=psia(pres);
 double itemp=Faren(temp)+460;
 double rstemp;
 double pbtemp;
 if(flashCompleto==0 || flashCompleto==3){
    if(corrSat==0 || corrSat==-4){
	   if(tabRSPB==1 && pres<TabRSLivia[npontos][0] && pres >TabRSLivia[1][0] &&
			   temp<TabRSLivia[0][npontos] && temp>TabRSLivia[0][1]){
	    rstemp=interpolaRS(pres,temp);

	   }
	   else{
         rstemp = (SG100vb*pow(ipres,Avb)/Bvb)*(pow(10,Cvb*API/itemp));
         if(varPb<0)
              pbtemp = pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb);
         else pbtemp=varPb;
         //double pbtemp = 1.1206*pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb); //correção apenas para test
       if(rstemp < 0) rstemp=0;
       else if(ipres >= pbtemp)rstemp=IRGO;
	   }
       return rstemp;
    }
 //////////////////////////////////////////////////////////////////////////////////////////

    else if(corrSat==1){
	   if(tabRSPB==1 && pres<TabRSLivia[npontos][0] && pres >TabRSLivia[1][0] &&
			   temp<TabRSLivia[0][npontos] && temp>TabRSLivia[0][1]){
	     rstemp=interpolaRS(pres,temp);

	   }
	   else{
         double GMF1;
         double EMWGT;
         double SGO=141.5/(131.5+API);

         if(API<51.1) EMWGT=interpolaLas3( API);
         else EMWGT = 6084.0/(API - 5.9);

         GMF1  = 1.0/(1.0 + 132755*SGO/IRGO/EMWGT);
         double BPPF1 = interpolaLas1(GMF1);
         double pbtemp  = BPPF1*itemp/Deng;

         BPPF1 = ipres*Deng/itemp;
         if(ipres>=pbtemp)rstemp=IRGO;
         else{
           if (BPPF1 > 0.17){
             if (BPPF1 <= 6.7)GMF1=interpolaLas2(BPPF1);
             else{
                GMF1=1.0-0.99308*exp(-0.28075*BPPF1);
                if(GMF1==1.0)GMF1=0.999;
             }
           }
           else{
             GMF1=(BPPF1-0.06263)/2.1474;
             if(GMF1<0.0)GMF1=0.;
           }

           rstemp=(132755*SGO/EMWGT)*(GMF1/(1.0 - GMF1));
           if(rstemp<0.)rstemp=0.;
           //else if(ipres >= pbtemp)rstemp=IRGO;
         }
	   }

       return rstemp;
    }

 ////////////////////////////////////////////////////////////////////////////
    else if(corrSat==2){
	 //pbtemp = interpolaPB( temp);;
	   if(tabRSPB==1 && pres<TabRSLivia[npontos][0] && pres >TabRSLivia[1][0] &&
			   temp<TabRSLivia[0][npontos] && temp>TabRSLivia[0][1]){
	    rstemp=interpolaRS(pres,temp);

	   }
	   else{
           double A = 0.0125*API - 0.00091*Faren(temp);
           double B = pow(10.0,A);
           if(varPb<0)
                pbtemp = 18.2*((1./B)*pow((IRGO/Deng),0.83)-1.4);
           else pbtemp=varPb;
           if(ipres>pbtemp) rstemp=IRGO;
           else rstemp =Deng*pow((1.4+ipres/18.2)*B,1.0/0.83);
           if(rstemp<0.)rstemp=0.;
	   }
       return rstemp;
    }

 /////////////////////////////////////////////////////////////////////////////

    else if(corrSat==3){
	   if(tabRSPB==1 && pres<TabRSLivia[npontos][0] && pres >TabRSLivia[1][0] &&
			   temp<TabRSLivia[0][npontos] && temp>TabRSLivia[0][1]){
	    rstemp=interpolaRS(pres,temp);

	   }
	   else{
 // Calcula a razão de solubilidade do gás natural no óleo
 // Calcula a pressão de bolha (despreza o gás em solução na água)
         double A;
         double B;
         if(varPb<0){
        	 A = pow((IRGO/Deng),0.816)*pow(Faren(temp),0.172)*pow(API,-0.989);
        	 B = log10l(A);
        	 pbtemp = pow(10.0,((5.17967 - pow(0.60436*B - 1.7447,2.))/1.20872));
         }
         else pbtemp=varPb;
    	 // Limita PB. Pela eq. (6a) o limite de PB é aproximadamente 19284 psia, Alcino 20110118
    	 if (pbtemp > 19284.0) pbtemp = 19284.0;


         if(ipres>pbtemp)rstemp=IRGO;
         else{
           A = 5.17967 - 1.20872*log10l(ipres);
           if (A < 0) A = 0.0;
           A =sqrtl(A);
           B  = (1.7447 - A)/0.60436;
           rstemp= Deng*pow((pow(API,0.989)*pow(10.0,B)*pow(Faren(temp),-0.172)),1.22549);
           if(rstemp<0.)rstemp=0.;
         }
	   }
       return rstemp;
    }
    else if(corrSat==4){

       if(tabRSPB==1 && pres<TabRSLivia[npontos][0] && pres >TabRSLivia[1][0] &&
			   temp<TabRSLivia[0][npontos] && temp>TabRSLivia[0][1]){
         //rstemp=interpolaRS(pres,temp);
         rstemp=interpolaVarProd(pres,temp, TabRSLivia);
         return rstemp*IRGO;
       }
       else{

         double PbLivia;// = interpolaPB( temp);

         if(varPb<0)
        	 pbtemp = PB(pres,temp);
         else pbtemp=varPb;

         double P_psi = ipres - 14.69;
         PbLivia=pbtemp-14.69;

         double Pr = P_psi/PbLivia; //eq. 2b do paper

         vector<double> coefs = {2.82677257121833e-06,0.0998265347002496,2.43122948321117,-0.0308131130446396,0.359655761028360,
                                     8.90264742708233e-05,0.515954791023796,-0.346605890074461,0.421825090518311,1.15333863637077,
                                     0.0148148245973275,-0.431815389633499,-0.185931200429990,0.232081366777634,0.390656570641476,
                                     0.0280606339605950,5.23718115829871,1.04333875401628};

         double f_Pb = 1 + coefs[15] * pow(yco2, coefs[16]) * pow(P_psi, coefs[17]);  // Eq. 2g do paper
         double Pb_corr_psi = PbLivia * f_Pb;

         double yN2 = 0.; //fração molar de N2 no gás de superfície [-] CORRIGIR FUTURAMENTE. Importar na chamada da função PropFlu.
         double yH2S = 0.; //fração molar de H2S no gás de superfície [-] CORRIGIR FUTURAMENTE. Importar na chamada da função PropFlu.

         double dg_HC = (Deng - 0.967 * yN2 - 1.52 * yco2 - 34.082 / 28.96 * yH2S) / (1 - yN2 - yco2 - yH2S); //eq. 1 do paper

         double TFa=Faren(temp);

         double a1 = coefs[0] * pow(dg_HC, coefs[1]) * pow(API, coefs[2]) * pow(TFa, coefs[3]) * pow(Pb_corr_psi, coefs[4]); // Eq. 2d do paper
         double a2 = coefs[5] * pow(dg_HC, coefs[6]) * pow(API, coefs[7]) * pow(TFa, coefs[8]) * pow(Pb_corr_psi, coefs[9]); // Eq. 2e do paper
         double a3 = coefs[10] * pow(dg_HC, coefs[11]) * pow(API, coefs[12]) * pow(TFa, coefs[13]) * pow(Pb_corr_psi, coefs[14]); // Eq. 2f do paper

         double Rsr = (a1 * pow(Pr,  a2) + (1 - a1) * pow(Pr, a3)); //eq. 2a do paper
         rstemp = Rsr * IRGO; //[Sft3/bbl] //eq. 2c do paper //variavel ja inicializada

         //pbtemp = Pb_psia;  //variavel ja inicializada

         if(rstemp<0.)rstemp=0.;
         else if(ipres>pbtemp)rstemp=IRGO;
         else if (rstemp>IRGO)rstemp=IRGO;
         else rstemp=IRGO*Rsr;


         /*double a1,a2,a3;
         double TFa=Faren(temp);
         double prB;
         double pcorB;

         double A0,A1,A2,A3,A4;
         double B0,B1,B2,B3,B4;
         double C0,C1,C2,C3,C4;
         double D0,D1,D2;

         A0=6.54269213;
         A1=2.60464618;
         A2=1.21334544;
         A3=0.16464125;
         A4=1.19382967;
         B0=8.7;
         B1=4.77390016;
         B2=1.77267703;
         B3=-0.73072038;
         B4=1.58093857;
         C0=3.22609;
         C1=0.09281075;
         C2=0.13633665;
         C3=0.09634381;
         C4=0.53238728;
         D0=1.00544053;
         D1=-0.00134177;
         D2=0.51839397;

         a1=A0*pow(Deng,A1)*pow(API,A2)*pow(TFa,A3)*pow(PbLivia,A4)/1e8;
         a2=B0*pow(Deng,B1)*pow(API,B2)*pow(TFa,B3)*pow(PbLivia,B4)/1e7;
         a3=C0*pow(Deng,C1)*pow(API,C2)*pow(TFa,C3)*pow(PbLivia,C4)/1e3;

         double multCor=(D0+D1*yco2*pow(TFa,D2));


         pbtemp=PbLivia;

         double pcor=ipres*multCor;
         double pr=pcor/pbtemp;

         double Rsr=a1*pow(pr,a2)+(1.-a1)*pow(pr,a3);

         if(rstemp<0.)rstemp=0.;
         else if(ipres>pbtemp)rstemp=IRGO;
         else rstemp=IRGO*Rsr;*/
         return rstemp;
          /* rstemp = (SG100vb*pow(ipres,Avb)/Bvb)*(pow(10,Cvb*API/itemp));
           pbtemp = interpolaPB( temp);
           //pbtemp = pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb);
         if(rstemp < 0) rstemp=0;
         else if(ipres >= pbtemp)rstemp=IRGO;
         return rstemp;*/
       }
    }
 }
 else if(flashCompleto==1){
	 rstemp=interpolaVarProd(pres, temp, RSF);
     if(rstemp < 0) rstemp=0;
	 return rstemp;
 }
 else{
    double rstemptemp;
     if((*vg1dSP).modoTransiente==0){
    	 if(tabelaDinamica==0){
    		 Marlim_CalculateSolutionGasOilRatio(dVaporMassFraction,dStockTankVaporMassFraction,
    				 dStockTankLiquidDensity,dStockTankVaporDensity, &rstemptemp);
    		 if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)rstemptemp=0.;
    		 //if(dVaporMassFraction>1.-1e-15)rstemptemp=0.;
    		 if(IRGO<rstemptemp)rstemptemp=IRGO;
    		 if(isnan(rstemptemp))rstemptemp=0.;
    	 }
    	 else{
    		 if(temp<tabDin.tmin)temp=tabDin.tmin;
    		 if(temp>tabDin.tmax)temp=tabDin.tmax;
    		 if(pres<tabDin.pmin) pres=tabDin.pmin;
    		 if(pres>tabDin.pmax)pres=tabDin.pmax;
    		 if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
    			 int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
    			 int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
    			 double raztemp;
    			 double razpres;
    			 double valT0;
    			 double valT1;
    			 posicT++;
    			 posicP++;
    			 raztemp=(temp-tabDin.rs[0][posicT])/(tabDin.delT);
    			 razpres=(pres-tabDin.rs[posicP][0])/(tabDin.delP);
    			 valT0=raztemp*tabDin.rs[posicP][posicT+1]+(1.-raztemp)*tabDin.rs[posicP][posicT];
    			 valT1=raztemp*tabDin.rs[posicP+1][posicT+1]+(1.-raztemp)*tabDin.rs[posicP+1][posicT];
    			 rstemptemp=razpres*valT1+(1.-razpres)*valT0;

    		 }
    		 else{
    			 Marlim_CalculateSolutionGasOilRatio(dVaporMassFraction,dStockTankVaporMassFraction,
    					 dStockTankLiquidDensity,dStockTankVaporDensity, &rstemptemp);
    			 if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)rstemptemp=0.;
    			 //if(dVaporMassFraction>1.-1e-15)rstemptemp=0.;
    			 if(IRGO<rstemptemp)rstemptemp=IRGO;
    			 if(isnan(rstemptemp))rstemptemp=0.;
    		 }
    	 }
     }
     else{
		 double raztemp;
		 double razpres;
		 double valT0;
		 double valT1;
		 if(temp<miniTabDin.tmin){
			 temp=miniTabDin.tmin;
		 }
		 if(temp>miniTabDin.tmax){
			 temp=miniTabDin.tmax;
		 }
		 if(pres<miniTabDin.pmin){
			 pres=miniTabDin.pmin;
		 }
		 if(pres>miniTabDin.pmax){
			 pres=miniTabDin.pmax;
		 }
		 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
		 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
		 valT0=raztemp*miniTabDin.rs[0][1]+(1.-raztemp)*miniTabDin.rs[0][0];
		 valT1=raztemp*miniTabDin.rs[1][1]+(1.-raztemp)*miniTabDin.rs[1][0];
		 rstemptemp=razpres*valT1+(1.-razpres)*valT0;
/*		 if(isnan(rstemptemp)){
			 cout<<"Estou aqui 1 "<<rstemptemp<<endl;
			 cout<<"tmin "<<miniTabDin.tmin<<endl;
			 cout<<"tmax "<<miniTabDin.tmax<<endl;
			 cout<<"temp "<<temp<<endl;
			 cout<<"pmin "<<miniTabDin.pmin<<endl;
			 cout<<"pmax "<<miniTabDin.pmax<<endl;
			 cout<<"pres "<<pres<<endl;
			 cout<<"rs 0-0 "<<miniTabDin.rs[0][0]<<endl;
			 cout<<"rs 0-1 "<<miniTabDin.rs[0][1]<<endl;
			 cout<<"rs 1-0 "<<miniTabDin.rs[1][0]<<endl;
			 cout<<"rs 1-1 "<<miniTabDin.rs[1][1]<<endl;
		 }*/
     }
	 return rstemptemp;
 }


}//alteracao4

double ProFlu::PB(double pres, double temp) const{//alteracao4
//Calcula Pb para uma dada  press         o em kgf/cm2 e temperatura em Celcius
//Pb em psi
 double itemp;
 double pbtemp=0;
 if(flashCompleto==0 || flashCompleto==3){
   if(corrSat==0|| corrSat==-4){
	  if(tabRSPB==1){
	    pbtemp=interpolaPB(temp);

	 }
	 else{
     itemp=Faren(temp)+460;
     pbtemp = pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb);
     //double pbtemp = 1.1206*pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb); //correção apenas para teste
	 }
   }
 ////////////////////////////////////////////////////////////////////////////////////////////////////////

   else if(corrSat==1){
	 if(tabRSPB==1){
		 pbtemp=interpolaPB(temp);

	 }
	 else{
	   itemp=Faren(temp)+460;
       double SGO=141.5/(131.5+API);
       double EMWGT;
       if(API<51.1) EMWGT=interpolaLas3(API);
       else EMWGT = 6084.0/(API - 5.9);
       double GMF1  = 1.0/(1.0 + 132755*SGO/IRGO/EMWGT);
       double BPPF1 = interpolaLas1(GMF1);
       pbtemp  = BPPF1*itemp/Deng;
	 }
   }
 ///////////////////////////////////////////////////////////////////////////////////////////////
   else if(corrSat==2){
	 if(tabRSPB==1){
		 pbtemp=interpolaPB(temp);

	 }
	 else{
        double A = 0.0125*API - 0.00091*Faren(temp);
        double B = pow(10.0,A);
        pbtemp = 18.2*((1./B)*pow((IRGO/Deng),0.83)-1.4);
	 }
   }
 /////////////////////////////////////////////////////////////////////////////////////////////////
   else if(corrSat==3){
	  if(tabRSPB==1){
	    pbtemp=interpolaPB(temp);

      }
      else{
  // Calcula a pressão de bolha (despreza o gás em solução na água)
      double A  = pow((IRGO/Deng),0.816)*pow(Faren(temp),0.172)*pow(API,-0.989);
      double B  = log10l(A);
      pbtemp = pow(10.0,((5.17967 - pow(0.60436*B - 1.7447,2.))/1.20872));
    }
   }
   else if(corrSat==4){
        //pbtemp = interpolaPB( temp); caso antigo
	   if(yco2<0.1){
			 if(tabRSPB==1){
				 pbtemp=interpolaPB(temp);

			 }
			 else{
		        double A = 0.0125*API - 0.00091*Faren(temp);
		        double B = pow(10.0,A);
		        pbtemp = 18.2*((1./B)*pow((IRGO/Deng),0.83)-1.4);
			 }
	   }
	   else{
		   //Deng = densidade do gás de superfície [-]
		   //API = grau API do óleo [oAPI]
		   //yco2 = fração molar de CO2 no gás de superfície [-]
		   //IRGO = razão gas-oleo do fluido de reservatório [Sm3/Sm3]
		   //ipres = pressão em que se está calculando a propriedade [psia]
		   //temp = temperatura em que se está calculando a propriedade [oC]

		   double yN2 = 0. ;//fração molar de N2 no gás de superfície [-] CORRIGIR FUTURAMENTE. Importar na chamada da função PropFlu.
		   double yH2S = 0. ;//fração molar de H2S no gás de superfície [-] CORRIGIR FUTURAMENTE. Importar na chamada da função PropFlu.

		   double TFa=Faren(temp); //temperatura [oF]
		   //double GOR = IRGO  * pow((6.29 / 35.315), -1) ;//conversao de Sm3/Sm3 para scf/STB
		   double GOR = IRGO;
		   double dg_HC = (Deng - 0.967 * yN2 - 1.52 * yco2 - 34.082 / 28.96 * yH2S) / (1 - yN2 - yco2 - yH2S); //eq. 1 do paper

		   double b[4];
		   b[0]=-0.129702305170694;b[1]=0.532007403334036;b[2]=-0.333164287519332;b[3]=0.623534976620241; // b coefficients

		   double pcorrCO2 = 1 + b[0] * pow(yco2, b[1]) * pow(GOR, b[2]) * pow(TFa, b[3]); //eq. 3c do paper
		   double pcorrN2 = 1 + ((-2.65e-4 * API + 5.5e-3) * TFa + (0.0931 * API - 0.8295)) * yN2 +
		                         ((1.954e-11 * pow(API, 4.699)) * TFa + (0.027 * API - 2.366)) * pow(yN2, 2); //eq. 3d do paper
		   double pcorrH2S = 1 - (0.9035 + 0.0015 * API) * yH2S + 0.019 * (45 - API) * pow(yH2S, 2); //eq. 3e do paper

		   double pb_star = pow((GOR / dg_HC), 0.816) * pow(TFa, 0.172) / pow(API, 0.989); //eq. 3b do paper

		   double aux = 1.7669 + 1.7447 * log10(pb_star) - 0.30218 * pow(log10(pb_star), 2);
		   double Pb_psia = pow(10, aux) * pcorrN2 * pcorrCO2 * pcorrH2S; //eq. 3a do paper
		   pbtemp=Pb_psia;

	   }

   }
 }
 else if(flashCompleto==1){
	 pbtemp = interpolaPB(temp);
 }
 else{
	 if((*vg1dSP).modoTransiente==0){
		 if(tabelaDinamica==0){
			 //pbtemp=psia(dCalculatedBubbleP/98066.52);

			 int iIERT;
			 double pbolha;
			 /*Marlim_CalculateBubblePressure(temp, npseudo,
					 fracMol, tempCrit, presCrit, fatAcent, kij, lij,
					 TIndepPeneloux, liqModel, liqModel,
					pres, &pbolha,&iIERT);
			 pbtemp=psia(pbolha);*/
			 itemp=Faren(temp)+460;
			 pbtemp=pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb);
		 }
		 else{
			 if(temp<tabDin.tmin)temp=tabDin.tmin;
			 if(temp>tabDin.tmax)temp=tabDin.tmax;
			 if(pres<tabDin.pmin) pres=tabDin.pmin;
			 if(pres>tabDin.pmax)pres=tabDin.pmax;
			 if(temp>=tabDin.tmin && temp<=tabDin.tmax){
				 int posic=floor(temp-tabDin.tmin)/(tabDin.delT);
				 if(posic<tabDin.npontosT){
					 //posic++;
					 double raztemp=(temp-tabDin.TBF[posic])/(tabDin.delT);
					 pbtemp=raztemp*tabDin.PBF[posic+1]+(1.-raztemp)*tabDin.PBF[posic];
				 }
				 else pbtemp=tabDin.PBF[posic+1];
			 }
			 else{
				 pbtemp=psia(dCalculatedBubbleP/98066.52);
			 }
		 }
	 }
     else{
		 double raztemp;
		 if(temp<miniTabDin.tmin){
			 temp=miniTabDin.tmin;
		 }
		 if(temp>miniTabDin.tmax){
			 temp=miniTabDin.tmax;
		 }
		 if(pres<miniTabDin.pmin){
			 pres=miniTabDin.pmin;
		 }
		 if(pres>miniTabDin.pmax){
			 pres=miniTabDin.pmax;
		 }
		 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
		 pbtemp=raztemp*miniTabDin.PBF[1]+(1.-raztemp)*miniTabDin.PBF[0];
     }
 }

 return pbtemp;
}

double ProFlu::dRSdP(double pres, double temp) const{
 return RS(pres,temp)*Avb*pow(14.223344,2.*(1-Avb))*pow(pres,1-2.*Avb);
}

double ProFlu::ViscOleo(double pres, double temp,int semEmul) const{
//Calcula a viscosidade do          leo vivo para uma  press         o em kgf/cm2 e temperatura em Celcius
//A viscosidade           retornada em cP
 double viso=0;
 if(flashCompleto!=2 || (flashCompleto==2 && dStockTankVaporMassFraction<1-1e-15)){
	 if((flashCompleto==0 || flashCompleto==3 || viscBlackOil==1) && dStockTankVaporMassFraction<1-1e-15){
		 double PBolha = PB(pres, temp);
		 if(flashCompleto==2 && PBolha<0){
			 double itemp=Faren(temp)+460;
			 PBolha = pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb);
		 }
		 if(flashCompleto==2 && PBolha<1e-15)PBolha=1.;
		 double vismorto = VisOM(temp);
		 double ipres=psia(pres);

		 if((RGO > 0) && (ipres < PBolha || (flashCompleto==2 && iCalculatedThermodynamicCondition!=0 && iCalculatedThermodynamicCondition!=4))){
			 if(corrOV==0){
	    //Beggs_Robinson
				 double RazSub = RS(pres, temp,PBolha);
				 if(RazSub<0.)RazSub=0.;
				 double xvisc = 10.715/pow(RazSub + 100,0.515);
				 double yvisc = 5.439/pow(RazSub + 150,0.338);
				 viso = xvisc*pow(vismorto,yvisc);
			 }
			 if(corrOV==1){
				 //Kartoatmodjo_Schmidt
				 double RazSub = RS(pres, temp,PBolha);
				 if(RazSub<0.)RazSub=0.;
				 double xvisc = pow(10.0,-0.00081*RazSub);
				 double yvisc = (0.2001 + 0.8428*pow(10.0,-0.000845*RazSub))*pow(vismorto,0.43 + 0.5165*xvisc);
				 viso = -0.06821 + 0.9824*yvisc + 0.0004034*yvisc*yvisc;
			 }
			 if(corrOV==2){
				 //Petrosky_Farshad
				 double RazSub = RS(pres, temp,PBolha);
				 if(RazSub<0.)RazSub=0.;
				 double xvisc = 0.1651 + 0.6165*pow(10.0,-6.0866E-04*RazSub);
				 double yvisc = 0.5131 + 0.5109*pow(10.0,-1.1831E-03*RazSub);
				 viso= xvisc*pow(vismorto,yvisc);
			 }
		 }
		 else if((ipres>PBolha || (flashCompleto==2 && (iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==4))) && RGO>0.){

			 double visSat=0.;
			 double deltaVisSat=0.;
			 double pBolhaKGF=PBolha/(0.9678411*14.69595);
			 ////Calculo da viscosidade na pressão de bolha:
			 if(corrOV==0){
	    //Beggs_Robinson
				 double RazSub = RS(pBolhaKGF, temp,PBolha);
				 if(RazSub<0.)RazSub=0.;
				 double xvisc = 10.715/pow(RazSub + 100,0.515);
				 double yvisc = 5.439/pow(RazSub + 150,0.338);
				 visSat = xvisc*pow(vismorto,yvisc);
			 }
			 if(corrOV==1){
				 //Kartoatmodjo_Schmidt
				 double RazSub = RS(pBolhaKGF, temp,PBolha);
				 if(RazSub<0.)RazSub=0.;
				 double xvisc = pow(10.0,-0.00081*RazSub);
				 double yvisc = (0.2001 + 0.8428*pow(10.0,-0.000845*RazSub))*pow(vismorto,0.43 + 0.5165*xvisc);
				 visSat = -0.06821 + 0.9824*yvisc + 0.0004034*yvisc*yvisc;
			 }
			 if(corrOV==2){
				 //Petrosky_Farshad
				 double RazSub = RS(pBolhaKGF, temp,PBolha);
				 if(RazSub<0.)RazSub=0.;
				 double xvisc = 0.1651 + 0.6165*pow(10.0,-6.0866E-04*RazSub);
				 double yvisc = 0.5131 + 0.5109*pow(10.0,-1.1831E-03*RazSub);
				 visSat= xvisc*pow(vismorto,yvisc);
			 }
			 //////// Calculo da correção entre a correlação de viscosidade acima da pressão de bolha e a viscosidade na pressão de bolha
			 /*if(corrOS!=1){
				 deltaVisSat = vismorto-visSat;
			 }
			 else if(corrOS==1){
				 deltaVisSat = 1.00081*vismorto-visSat;
			 }*/

			 if(corrOS==0){
				 //Vazquez_Beggs
				 double xvisc = (-3.9*pow(10,-5))*ipres - 5;
				 double yvisc = 2.6*pow(ipres,1.187)*pow(10,xvisc);
				 //viso = vismorto*pow(ipres/PBolha,yvisc)-deltaVisSat;
				 viso = visSat*pow(ipres/PBolha,yvisc);
			 }
			 else if(corrOS==1){
				 //Kartoatmodjo_Schmidt
				 double xvisc = -0.006517*pow(vismorto,1.8148);
				 double yvisc =  0.038*pow(vismorto,1.59);
				 //viso = 1.00081*vismorto + 0.001127*(ipres - PBolha)*(xvisc + yvisc)-deltaVisSat;
				 viso = visSat + 0.001127*(ipres - PBolha)*(xvisc + yvisc);
			 }
			 else if(corrOS==2){
				 //Petrosky_Farshad
				 double xvisc = log10(vismorto);
				 double yvisc =  -1.0146 + 1.3322*xvisc - 0.4876*xvisc*xvisc - 1.15036*xvisc*xvisc*xvisc;
				 //viso = vismorto + 0.0013449*(ipres - PBolha)*pow(10.0,yvisc)-deltaVisSat;
				 viso = visSat + 0.0013449*(ipres - PBolha)*pow(10.0,yvisc);
			 }
			 else if(corrOS==3){
				 //Beal
				 double xvisc = 0.024*pow(vismorto,1.6);
				 double yvisc =  0.038*pow(vismorto,0.56);
				 //viso = vismorto + 0.001*(ipres - PBolha)*(xvisc + yvisc)-deltaVisSat;
				 viso = visSat + 0.001*(ipres - PBolha)*(xvisc + yvisc);
			 }
			 else if(corrOS==4){
				 //Khan
				 //viso =vismorto *exp(9.6E-5*(ipres - PBolha))-deltaVisSat;
				 viso =visSat *exp(9.6E-5*(ipres - PBolha));
			 }

		 }
		 else viso = vismorto;
		 viso*=coefViscWax;
		 if(semEmul==0){
			 if(tipoemul==0 || (BSW>=bswCorte && tipoemul!=7) ){
				 double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
				 viso=(1-xliq)*viso+VisAgua(temp)*xliq;
			 }
			 else if(tipoemul==7 && BSW>=bswCorte )viso=VisAgua(temp);
			 else viso*=emul(pres,temp);
			 return viso;
		 }
		 else return viso;
	 }
	 else if(dStockTankVaporMassFraction<1-1e-15){
		 //double viso;
		 viso=interpolaVarProd(pres, temp, viscO);
		 viso*=coefViscWax;
		 if(semEmul==0){
			 if(tipoemul==0 || (BSW>=bswCorte && tipoemul!=7) ){
				 double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
				 viso=(1-xliq)*viso+VisAgua(temp)*xliq;
			 }
			 else if(tipoemul==7 && BSW>=bswCorte )viso=VisAgua(temp);
			 else viso*=emul(pres,temp);
			 return viso;
		 }
		 else return viso;
	 }
	 else{
		 viso=ViscGas(pres,temp);
		 viso*=coefViscWax;
		 if(semEmul==0){
			 if(tipoemul==0 || (BSW>=bswCorte && tipoemul!=7) ){
				 double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
				 viso=(1-xliq)*viso+VisAgua(temp)*xliq;
			 }
			 else if(tipoemul==7 && BSW>=bswCorte )viso=VisAgua(temp);
			 else viso*=emul(pres,temp);
			 return viso;
		 }
		 else{
			 return viso;
		 }
	 }
 }
 else{
	 /*viso=ViscGas(pres,temp);
	 if(tipoemul==0 || (BSW>=bswCorte && tipoemul!=7) ){
		 double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
		 viso=(1-xliq)*viso+VisAgua(temp)*xliq;
	 }
	 else if(tipoemul==7 && BSW>=bswCorte )viso=VisAgua(temp);
	 else viso*=emul(pres,temp);
	 return viso;*/
	 if(tabelaDinamica==0){
		 double visoP=1.;
		 double presC=pres*98066.5;
		 double tempC=temp+273.15;
		 int iIER;
		/* MarlimCalculateViscosityPedersen(presC, tempC, npseudo,tempCrit, presCrit, masMol,
				 oCalculatedLiqComposition, &visoP, &iIER);
		 if(isnan(visoP))*/visoP=ViscGas(pres,temp);
		 visoP*=coefViscWax;
		 ///alteracao para dStockTankVaporMassFraction=0 comentando a parte de emulsão
		 if(semEmul==0){
			 if(tipoemul==0 || (BSW>=bswCorte && tipoemul!=7) ){
				 double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
				 visoP=(1-xliq)*visoP+VisAgua(temp)*xliq;
			 }
			 else if(tipoemul==7 && BSW>=bswCorte )visoP=VisAgua(temp);
			 else visoP*=emul(pres,temp);
			 return visoP;
			 //return 20.;
		 }
		 else return visoP;
	 }
	 else{
			if(temp<tabDin.tmin)temp=tabDin.tmin;
			if(temp>tabDin.tmax)temp=tabDin.tmax;
			if(pres<tabDin.pmin) pres=tabDin.pmin;
			if(pres>tabDin.pmax)pres=tabDin.pmax;
		 if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
		 		 int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
		 		 int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
		 		 double raztemp;
		 		 double razpres;
		 		 double valT0;
		 		 double valT1;
		 		 posicT++;
		 		 posicP++;
		 		 raztemp=(temp-tabDin.viscO[0][posicT])/(tabDin.delT);
		 		 razpres=(pres-tabDin.viscO[posicP][0])/(tabDin.delP);
		 		 /*if(!isnan(tabDin.viscO[posicP][posicT+1])&&!isnan(tabDin.viscO[posicP][posicT])&&
		 				!isnan(tabDin.viscO[posicP+1][posicT+1])&&!isnan(tabDin.viscO[posicP+1][posicT])){
		 			 valT0=raztemp*tabDin.viscO[posicP][posicT+1]+(1.-raztemp)*tabDin.viscO[posicP][posicT];
		 			 valT1=raztemp*tabDin.viscO[posicP+1][posicT+1]+(1.-raztemp)*tabDin.viscO[posicP+1][posicT];
		 			 viso=razpres*valT1+(1.-razpres)*valT0;
		 		 }
		 		 else*/ viso=ViscGas(pres,temp);
		 		 viso*=coefViscWax;
		 		 return viso;

		 }
		 else{
			 double visoP=1.;
			 double presC=pres*98066.5;
			 double tempC=temp+273.15;
			 int iIER;
			 /*MarlimCalculateViscosityPedersen(presC, tempC, npseudo,tempCrit, presCrit, masMol,
					 oCalculatedLiqComposition, &visoP, &iIER);
			 if(isnan(visoP))*/visoP=ViscGas(pres,temp);
			 visoP*=coefViscWax;
			 if(semEmul==0){
				 if(tipoemul==0 || (BSW>=bswCorte && tipoemul!=7) ){
					 double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
					 visoP=(1-xliq)*visoP+VisAgua(temp)*xliq;
				 }
				 else if(tipoemul==7 && BSW>=bswCorte )visoP=VisAgua(temp);
				 else visoP*=emul(pres,temp);
				 return visoP;
			 }
			 else return visoP;
		 }
	 }

 }
}

double ProFlu::ViscGas(double pres,double temp) const{
   if(flashCompleto==0 || flashCompleto==2 || flashCompleto==3){
     double TF=Faren(temp);
     double TR=TF+459.67;
     double Ppsi=psia(pres);
     double wg=rDgL*Deng*29;
     double AK=(9.4+0.02*wg)*pow(TR,1.5)/(209.+19.*wg+TR);
     double x=3.5+(986/TR)+0.01*wg;
     double y=2.4-0.2*x;
     double rhog=MasEspGas(pres, temp)/1000.;
     double visc=AK*exp(x*pow(rhog,y))/10000.;
     return visc;
   }
   else{
	   double visc=interpolaVarProd(pres, temp, viscG);
	   return visc;
   }
}

double ProFlu::emul(double pres,double temp) const{
   double multplic;
   double rhoO=MasEspoleo(pres, temp);
   double rhoL=MasEspLiq(pres, temp);
   double frac=FracAguaInSitu(pres, temp);
   multplic=rhoL/rhoO;
  if(tipoemul==1) multplic*=exp((2.6672*frac+2.4740)*frac);
  else if(tipoemul==2)multplic*=exp((5.1920*frac+1.6691)*frac);
  else if(tipoemul==3)multplic*=exp((5.2456*frac+2.1102)*frac);
  else if(tipoemul==4)multplic*=exp((aemul*frac+bemul)*frac);//alteracao4
  else if(tipoemul==5){
	  double raz=frac/PHI100;
	  multplic=pow(1.+raz/(1.1884-raz),2.5);
  }
  else if(tipoemul==6){
	    if((frac-BSWVec[0])<1e-10) multplic=emulVec[0];
	    else if((frac-BSWVec[nvecEmul-1])>1e-10) multplic=emulVec[nvecEmul-1];
	    else{
	          int e, m, d,ifrac;
	          e = 0;
	          d = nvecEmul-1;
	          ifrac=d;
	          while (e <= d) {
	        	  m = (e + d)/2;
	        	  if(m==0){
	        		  ifrac=m;
	        		  break;
	        	  }
	        	  else if (m==nvecEmul-1 && BSWVec[m] == frac){
	        		  ifrac=m-1;
	        		  break;
	        	  }
	        	  if (BSWVec[m] > frac && BSWVec[m-1]<=frac){
	        		  ifrac=m-1;
	        		  break;
	        	  }
	        	  if (BSWVec[m-1] < frac) e = m + 1;
	        	  else d = m - 1;
	          }
	          double raztemp=(frac-BSWVec[ifrac])/(BSWVec[ifrac+1]-BSWVec[ifrac]);
	          multplic=(1-raztemp)*emulVec[ifrac]+raztemp*emulVec[ifrac+1];
	    }
  }
  else if(tipoemul==7){
	  multplic=1.;
  }
  return multplic;
}

double ProFlu::ZGopal(double PR, double TR) const{
double ZGop;
 if(PR<(TR - 0.73)||(PR<(4.7*TR-6.74)&&PR<(5.6875-0.525*TR)))ZGop=1;
 else if(PR<=5.4){
  int i = 1;
  if(PR > 1.2){
   if(PR > 1.4 || TR < 1.08 || TR > 1.19){
    if(PR <= 2.8) i = 2;
    else i = 3;
   }
  }
  int k=4;
  if(TR <= 2) k = 3;
  if(TR <= 1.4) k = 2;
  if(TR <= 1.2) k = 1;
  int j = 16*i + 4*k - 19;
  ZGop=PR*(CoefGopal[j-1]*TR+CoefGopal[j])+CoefGopal[j+1]*TR+CoefGopal[j+2];
 }
 else ZGop=PR*pow(0.711+3.66*TR,-1.4667)-1.637/(0.319*TR+0.522)+2.071;
 return ZGop;
}

double ProFlu::FNdran(double PR, double TR, double denr) const{
 double Adran = 0.064225133;
 double Bdran = 0.53530771*TR - 0.61232032;
 double Cdran = 0.31506237*TR - 1.04670990 - 0.57832728/(TR*TR);
 double Ddran = TR;
 double Edran = 0.68157001/(TR*TR);
 double Fdran = 0.68446549;
 double Gdran = 0.27*PR;
 double X1 = Fdran*(denr*denr);
 double X2 = exp(-X1);
 double X3 = 1 + X1;
 double X4 = denr*denr*denr;
 double X5 = X4*X4;
 return Adran*X5+Bdran*X4+Cdran*denr*denr+Ddran*denr+Edran*X4*X3*X2-Gdran;
}


double ProFlu::FalsaCorda(double a,double b,double PR,double TR,double delta ,double epsn,int maxit)const{
        //a e b-> chutes iniciais para a falsa corda
        //epsn -> limite em que se admite F(double) aproximadamente zero
        //delta-> limite a partir do qual admite-se que ja nao existe variacao no calculo da raiz
        //maxit-> maximo numero de iteracoes permitido no calculo
        //F(double)-> funcao que se quer obter a raiz
          double u=FNdran(PR,TR,a);
          double e=b-a;
          double c=0.;

          double v=FNdran(PR,TR,b);
          int inter=0;
          if(u!=0 && v!=0){//neste primeiro bloco, busca-se assegurar que sempre se esta trabalhando com valores de funcao negativa nos chutes iniciais para se garantir maior convergencia na falsa corda
           if(u/v>0){
            if(u>0){
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u<proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy>0&&inter<400){
              prox=prox-del/10; proy=FNdran(PR,TR,prox);
              inter++;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
            else{
             double proy=v; double prox=b; double distx=a; double del=a-prox;
             if(u>proy){
              proy=u; prox=a; distx=b; del=b-prox;
             }
             while(proy<0&&inter<400){
              prox=prox-del/10; proy=FNdran(PR,TR,prox);
              inter++;
             }
             a=prox; b=distx; u=proy; e=b-a;
            }
           }
          }
          
         int controla=0;
          for(int k=1;k<=maxit;k++){//este bloco trata da falsa corda propriamente dita
            e*=0.5;
            c=a+e;
            double w=FNdran(PR,TR,c);
            if(fabs(e)<delta||fabs(w)<epsn) return c;
            ((u>0&&w<0)||(u<0&&w>0))?(b=c):(a=c,u=w);
            controla++;
          }
          if(controla>=maxit)(*vg1dSP).contaExit++;
          if((*vg1dSP).contaExit>400) {
            // incluir falha
            string complementoFalha = string("Maximo falhas atingido: ") + to_string((*vg1dSP).contaExit);
            logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "Falsa corda em PropFlu atingiu numero maximo de iteracoes", "N/A",
                complementoFalha);
            // gravar arquivo de log
            logger.writeOutputLog();
            // encerrar a aplicacao
            exit(EXIT_SUCCESS);
          }
          return c;
}

double ProFlu::PreRog(double a,double b,double rog,double temp,double delta ,double epsn,int maxit)const{
    double valz=Zdran(a,temp);
	double valp=rog/(((rDgL*Deng*28.9625)*98066.5)/(8.0465*1000*valz*(temp + 273)));
	for(int i=0;i<5;i++){
			valz=Zdran(valp,temp);
			valp=rog/(((rDgL*Deng*28.9625)*98066.5)/(8.0465*1000*valz*(temp + 273)));
	}
	return valp;
}

double ProFlu::ZdranOriginal(double pres, double temp, int cordg, double a, double b, int interno) const{
//Funcao para calcular o fator de compressibilidade,  pressao em kgf/cm2 e temperatura em Celsius
 double PCtemp;
 double TCtemp;
 double rDgDtemp;
 double rDgLtemp;
 if(cordg==-1){
		PCtemp=PCis;
		TCtemp=TCis;
		rDgDtemp=rDgD;
		rDgLtemp=rDgL;
 }
 else{
		PCtemp=PC;
		TCtemp=TC;
		rDgDtemp=1.;
		rDgLtemp=1.;
 }
 if(temp<-200.)temp=-200;
 double ipres=psia(pres);
 double itemp=Faren(temp);
 double PR = ipres/PCtemp;
 double TR = (itemp + 460)/TCtemp;
 double varZ;
 double Gdran = 0.27*PR;
 if(tab==0 || interno==0){
	 if(rDgLtemp*Deng<=2)varZ = FalsaCorda(0.27*PR/(ZGopal(PR,TR)*(itemp + 460)/TCtemp),0.27*PR/(0.1*(itemp + 460)/TCtemp),PR,TR)*TR;
	 else varZ = FalsaCorda(0.27*PR/(40*(itemp + 460)/TCtemp),0.27*PR/(0.01*(itemp + 460)/TCtemp),PR,TR)*TR;
 }
 else{
	 if(rDgLtemp*Deng<=2) varZ = FalsaCorda(Gdran/(b*TR),Gdran/(a*TR),PR,TR)*TR;
	 else varZ = FalsaCorda(0.27*PR/(40*(itemp + 460)/TCtemp),0.27*PR/(0.01*(itemp + 460)/TCtemp),PR,TR)*TR;
 }
 varZ=Gdran/varZ;
 if(varZ<0)varZ=1.;
 else if(varZ<=0.05)varZ=0.05;
 return varZ;
}

double ProFlu::Zdran(double pres, double temp, int cordg, double masespG)const{//alteracao2
  double zt=0.;
  if(flashCompleto==0 || flashCompleto==3 || (flashCompleto==2 && tab==1)){
	  if(masespG<0){
		  double PCtemp;
		  double TCtemp;
		  double rDgDtemp;
		  double rDgLtemp;
		  if(cordg==-1){
			  PCtemp=PCis;
			  TCtemp=TCis;
			  rDgDtemp=rDgD;
			  rDgLtemp=rDgL;
		  }
		  else{
			  PCtemp=PC;
			  TCtemp=TC;
			  rDgDtemp=1.;
			  rDgLtemp=1.;
		  }
		  int interno=1;
		  double presR=(0.9678411*14.69595*pres)/PCtemp;
		  double tempR=(1.8*temp+32+460)/TCtemp;
		  if(tab>0){
			  if(presR>zdranP[1][0]-1e-5)interno=0;//alteracao2
			  if(presR<zdranP[npontos+1][0]+1e-5)interno=0;//alteracao2
			  if(tempR>zdranP[0][1]-1e-5)interno=0;//alteracao2
			  if(tempR<zdranP[0][npontos+1]+1e-5)interno=0;//alteracao2
			  if(interno==1){
				  int ipres;
				  int itemp;
				  int ipmarcador;
				  int itmarcador;
				  if(tab==1){
					  if(presR>zdranP[1][0] || presR<=zdranP[npontos+1][0] || tempR>zdranP[0][1] ||
							  tempR<=zdranP[0][npontos+1]) zt=0.;
					  else{
						  double pmin=zdranP[npontos+1][0];
						  double pmax=zdranP[1][0];
						  double delp=zdranP[1][0]-zdranP[2][0];
						  ipres=floor((pmax-presR)/delp)+1;
						  double tmin=zdranP[0][npontos+1];
						  double tmax=zdranP[0][1];
						  double delt=zdranP[0][1]-zdranP[0][2];
						  itemp=floor((tmax-tempR)/delt)+1;

						  double razpres=(zdranP[ipres][0]-presR)/(zdranP[ipres][0]-zdranP[ipres+1][0]);
						  double raztemp=(zdranP[0][itemp]-tempR)/(zdranP[0][itemp]-zdranP[0][itemp+1]);
						  double latp1=(1-razpres)*(zdranP[ipres][itemp])+razpres*(zdranP[ipres+1][itemp]);
						  double latp2=(1-razpres)*(zdranP[ipres][itemp+1])+razpres*(zdranP[ipres+1][itemp+1]);
						  zt=(1-raztemp)*latp1+raztemp*latp2;
					  }
				  }
			  }
			  else zt=ZdranOriginal(pres,temp,cordg,1,10,interno);
		  }
		  else zt=ZdranOriginal(pres,temp,cordg,1,10,interno);
	  }
	  else{
		  zt=((rDgL*Deng*28.9625)*pres*98066.5)/(8.0465*1000*masespG*(temp + 273.15));
	  }
  }
  else if(flashCompleto==1){
	  double masesp;
	  if(masespG<0)masesp=interpolaVarProd(pres, temp, rhogF);
	  else masesp=masespG;
	  zt=((Deng*28.9625)*pres*98066.5/masesp)/(8.0465*1000*(temp + 273.15));
  }
  else{
	  if((*vg1dSP).modoTransiente==0){
		  if(tabelaDinamica==0){
			  double masesp;
			  double ztC;
			  int iIERT;
			  Marlim_CalculatePhaseDensity(pres, temp, 1,
					  oCalculatedVapComposition, npseudo,  tempCrit, presCrit,
					  fatAcent, TIndepPeneloux, kij, lij,
					  liqModel,dVaporPhaseMW, &masesp,
					  &ztC, &iIERT);
			  if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)ztC=1.;
			  //if(dVaporMassFraction<1e-15)ztC=1.;
			  zt=ztC;
		  }
		  else{
			  if(temp<tabDin.tmin)temp=tabDin.tmin;
			  if(temp>tabDin.tmax)temp=tabDin.tmax;
			  if(pres<tabDin.pmin) pres=tabDin.pmin;
			  if(pres>tabDin.pmax)pres=tabDin.pmax;
			  if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
				  int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
				  int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
				  double raztemp;
				  double razpres;
				  double valT0;
				  double valT1;
				  posicT++;
				  posicP++;
				  double titP0T0=tabDin.tit[posicP][posicT];
				  double titP0T1=tabDin.tit[posicP][posicT+1];
				  double titP1T0=tabDin.tit[posicP+1][posicT];
				  double titP1T1=tabDin.tit[posicP+1][posicT+1];
				  double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
				  double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
				  if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
					  double masesp;
					  double ztC;
					  int iIERT;
					  Marlim_CalculatePhaseDensity(pres, temp, 1,
							  oCalculatedVapComposition, npseudo,  tempCrit, presCrit,
							  fatAcent, TIndepPeneloux, kij, lij,
							  liqModel,dVaporPhaseMW, &masesp,
							  &ztC, &iIERT);
					  if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)ztC=1.;
					  //if(dVaporMassFraction<1e-15)ztC=1.;
					  zt=ztC;
				  }
				  else{
					  raztemp=(temp-tabDin.valZ[0][posicT])/(tabDin.delT);
					  razpres=(pres-tabDin.valZ[posicP][0])/(tabDin.delP);
					  valT0=raztemp*tabDin.valZ[posicP][posicT+1]+(1.-raztemp)*tabDin.valZ[posicP][posicT];
					  valT1=raztemp*tabDin.valZ[posicP+1][posicT+1]+(1.-raztemp)*tabDin.valZ[posicP+1][posicT];
					  zt=razpres*valT1+(1.-razpres)*valT0;
				  }

			  }
			  else{
				  double masesp;
				  double ztC;
				  int iIERT;
				  Marlim_CalculatePhaseDensity(pres, temp, 1,
						  oCalculatedVapComposition, npseudo,  tempCrit, presCrit,
						  fatAcent, TIndepPeneloux, kij, lij,
						  liqModel,dVaporPhaseMW, &masesp,
						  &ztC, &iIERT);
				  if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)ztC=1.;
				  //if(dVaporMassFraction<1e-15)ztC=1.;
				  zt=ztC;
			  }
		  }
	  }
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.valZ[0][1]+(1.-raztemp)*miniTabDin.valZ[0][0];
			 valT1=raztemp*miniTabDin.valZ[1][1]+(1.-raztemp)*miniTabDin.valZ[1][0];
			 zt=razpres*valT1+(1.-razpres)*valT0;
	     }
  }
  return zt;
}//alteracao2

double ProFlu::DZDTOriginal(double pres, double temp, int cordg) const{
//Funcao para calcular a derivada de z com pressao em kgf/cm2 e temperatura em Celsius
double PCtemp;
double TCtemp;
double rDgDtemp;
double rDgLtemp;
if(cordg==-1){
	PCtemp=PCis;
	TCtemp=TCis;
	rDgDtemp=rDgD;
	rDgLtemp=rDgL;
}
else{
	PCtemp=PC;
	TCtemp=TC;
	rDgDtemp=1.;
	rDgLtemp=1.;
}
if(rDgLtemp*Deng<=100){
 double ipres=psia(pres);
 double itemp=Faren(temp);
 double zvar = ZdranOriginal(pres, temp,cordg);
 double TR = (itemp + 460)/TCtemp;
 double PR = ipres/PCtemp;
 double TCKel = TCtemp/1.8;
 double Adran = 0.064225133;
 double Bdran = 0.53530771*TR - 0.61232032;
 double Cdran = 0.31506237*TR - 1.04670990 - 0.57832728/(TR*TR);
 double Ddran = TR;
 double Edran = 0.68157001/(TR*TR);
 double Fdran = 0.68446549;
 double denrvar = 0.27*PR/(zvar*TR);
 double X1 = Fdran*denrvar*denrvar;
 double X2 = exp(-X1);
 double X3 = 1 + X1;
 double X4 = denrvar*denrvar*denrvar;
 double X5 = X4*X4;
 double dfdy =6*Adran*X5/denrvar + 3*Bdran*denrvar*denrvar + 2*Cdran*denrvar + Ddran +
             Edran*X4*(3 + X1*(3 - 2*X1))*X2/denrvar;
 if(zvar<=0.06)return 1e-10;
 else
 return (((0.5353 - 2*Edran*(1/TR)*X3*X2)*denrvar*denrvar + (0.3151 +
                  1.1566*(1/(TR*TR))/TR)*denrvar + 1)/dfdy - 1/TR)*zvar/TCKel;
}
else{
	double dt;
	if(fabsl(temp)>1e-5)dt=temp*0.01;
	else dt=0.01;
	return (Zdran(pres, temp+dt,cordg)-Zdran(pres, temp,cordg))/(dt);
}
}
double ProFlu::DZDT(double pres, double temp,double masespG)const{//alteracao2
  double zt;
  if(flashCompleto==0 || flashCompleto==3 || (flashCompleto==2 && tab==1)){
    int interno=1;
    double presR=(0.9678411*14.69595*pres)/PCis;
    double tempR=(1.8*temp+32+460)/TCis;
    if(tab>0){
      if(presR>dzdtP[1][0]-1e-5)interno=0;//alteracao2
      if(presR<dzdtP[npontos+1][0]+1e-5)interno=0;//alteracao2
      if(tempR>dzdtP[0][1]-1e-5)interno=0;//alteracao2
      if(tempR<dzdtP[0][npontos+1]+1e-5)interno=0;//alteracao2
      if(interno==1){
	    int ipres;
        int itemp;
        int ipmarcador;
        int itmarcador;
        if(tab==1){
          if(presR>dzdtP[1][0] || presR<=dzdtP[npontos+1][0] || tempR>dzdtP[0][1] ||
             tempR<=dzdtP[0][npontos+1]) zt=0.;
          else{
              double pmin=zdranP[npontos+1][0];
              double pmax=zdranP[1][0];
              double delp=zdranP[1][0]-zdranP[2][0];
              ipres=floor((pmax-presR)/delp)+1;
              double tmin=zdranP[0][npontos+1];
              double tmax=zdranP[0][1];
              double delt=zdranP[0][1]-zdranP[0][2];
              itemp=floor((tmax-tempR)/delt)+1;
	          double razpres=(dzdtP[ipres][0]-presR)/(dzdtP[ipres][0]-dzdtP[ipres+1][0]);
	          double raztemp=(dzdtP[0][itemp]-tempR)/(dzdtP[0][itemp]-dzdtP[0][itemp+1]);
	          double latp1=(1-razpres)*(dzdtP[ipres][itemp])+razpres*(dzdtP[ipres+1][itemp]);
	          double latp2=(1-razpres)*(dzdtP[ipres][itemp+1])+razpres*(dzdtP[ipres+1][itemp+1]);
	          zt=((1-raztemp)*latp1+raztemp*latp2)*(1.8/TCis);
          }
        }
      }
      else zt=DZDTOriginal(pres,temp);
    }
    else zt=DZDTOriginal(pres,temp);
  }
  else if(flashCompleto==1){
	  double masesp;
	  if(masespG<0)masesp=interpolaVarProd(pres, temp, rhogF);
	  else masesp=masespG;
	  double drhodt=interpolaVarProd(pres, temp, DrhogDtF);
	  double constante=((Deng*28.9625)*98066.5)/(8.0465*1000);
	  zt=((Deng*28.9625)*pres*98066.5/masesp)/(8.0465*1000*(temp + 273.15));
	  double tK=temp + 273.15;
	  zt=-(drhodt+constante*pres/(zt*tK*tK))*(zt*zt*tK/(constante*pres));
  }
  else{
	  if((*vg1dSP).modoTransiente==0){
		  if(tabelaDinamica==0){
			  int iIER;
			  double tempZ;
			  double dZdT;
			  double drhogdT;
			  double dZdp;
			  double drhogdp;

			  Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
					  liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
					  presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
					  &tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
			  if(dVaporMassFraction<1e-15)dZdT=0.;

			  zt=dZdT;
		  }
		  else{
			  if(temp<tabDin.tmin)temp=tabDin.tmin;
			  if(temp>tabDin.tmax)temp=tabDin.tmax;
			  if(pres<tabDin.pmin) pres=tabDin.pmin;
			  if(pres>tabDin.pmax)pres=tabDin.pmax;
			  if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
				  int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
				  int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
				  double raztemp;
				  double razpres;
				  double valT0;
				  double valT1;
				  posicT++;
				  posicP++;
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						  int iIER;
						  double tempZ;
						  double dZdT;
						  double drhogdT;
						  double dZdp;
						  double drhogdp;

						  Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
								  liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
								  presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
								  &tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
						  if(dVaporMassFraction<1e-15)dZdT=0.;

						  zt=dZdT;
					}
					else{
						raztemp=(temp-tabDin.valdZdT[0][posicT])/(tabDin.delT);
						razpres=(pres-tabDin.valdZdT[posicP][0])/(tabDin.delP);
						valT0=raztemp*tabDin.valdZdT[posicP][posicT+1]+(1.-raztemp)*tabDin.valdZdT[posicP][posicT];
						valT1=raztemp*tabDin.valdZdT[posicP+1][posicT+1]+(1.-raztemp)*tabDin.valdZdT[posicP+1][posicT];
						zt=razpres*valT1+(1.-razpres)*valT0;
					}

			  }
			  else{
				  int iIER;
				  double tempZ;
				  double dZdT;
				  double drhogdT;
				  double dZdp;
				  double drhogdp;

				  Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
						  liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
						  presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
						  &tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
				  if(dVaporMassFraction<1e-15)dZdT=0.;

				  zt=dZdT;
			  }
		  }
	  }
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.valdZdT[0][1]+(1.-raztemp)*miniTabDin.valdZdT[0][0];
			 valT1=raztemp*miniTabDin.valdZdT[1][1]+(1.-raztemp)*miniTabDin.valdZdT[1][0];
			 zt=razpres*valT1+(1.-razpres)*valT0;
	     }
  }
  return zt;
}//alteracao2

double ProFlu::DZDPOriginal(double pres, double temp, int cordg) const{
//Funcao para calcular a derivada de z com a  pressao em kgf/cm2 e temperatura em Celsius
double PCtemp;
double TCtemp;
double rDgDtemp;
double rDgLtemp;
if(cordg==-1){
			PCtemp=PCis;
			TCtemp=TCis;
			rDgDtemp=rDgD;
			rDgLtemp=rDgL;
}
else{
			PCtemp=PC;
			TCtemp=TC;
			rDgDtemp=1.;
			rDgLtemp=1.;
}
if(rDgLtemp*Deng<=100){
 double ipres=psia(pres);
 double itemp=Faren(temp);
 double zvar = ZdranOriginal(pres, temp,cordg);
 double TR = (itemp + 460)/TCtemp;
 double PR = ipres/PCtemp;
 double PCPA = 6894.8*PCtemp;
 double Adran = 0.064225133;
 double Bdran = 0.53530771*TR - 0.61232032;
 double Cdran = 0.31506237*TR - 1.04670990 - 0.57832728/(TR*TR);
 double Ddran = TR;
 double Edran = 0.68157001/(TR*TR);
 double Fdran = 0.68446549;
 double denrvar = 0.27*PR/(zvar*TR);
 double X1 = Fdran*(denrvar*denrvar);
 double X2 = exp(-X1);
 double X4 = denrvar*(denrvar*denrvar);
 double X5 = (X4*X4);
 double dfdy = 6*Adran*X5/denrvar + 3*Bdran*(denrvar*denrvar)+2*Cdran*denrvar+Ddran+
      Edran*X4*(3+X1*(3-2*X1))*X2/denrvar;
 if(zvar<=0.06)return 1e-10;
 else
 return (1 - zvar*TR/dfdy)*zvar/PR/PCPA;
}
else{
	double dp;
	if(fabsl(pres)>1e-5)dp=pres*0.01;
	else dp=0.01;
	return (Zdran(pres+dp, temp,cordg)-Zdran(pres, temp,cordg))/(98066.5*dp);
}
}
double ProFlu::DZDP(double pres, double temp,double masespG)const{//alteracao2
  double zt;
  if(flashCompleto==0 || flashCompleto==3 || (flashCompleto==2 && tab==1)){
    int interno=1;
    double presR=(0.9678411*14.69595*pres)/PCis;
    double tempR=(1.8*temp+32+460)/TCis;
    if(tab>0){
      if(presR>dzdpP[1][0]-1e-5)interno=0;//alteracao2
      if(presR<dzdpP[npontos+1][0]+1e-5)interno=0;//alteracao2
      if(tempR>dzdpP[0][1]-1e-5)interno=0;//alteracao2
      if(tempR<dzdpP[0][npontos+1]+1e-5)interno=0;//alteracao2
      if(interno==1){
	    int ipres;
        int itemp;
        int ipmarcador;
        int itmarcador;
        if(tab==1){
          if(presR>dzdpP[1][0] || presR<=dzdpP[npontos+1][0] || tempR>dzdpP[0][1] ||
             tempR<=dzdpP[0][npontos+1]) zt=0.;
          else{
              double pmin=zdranP[npontos+1][0];
              double pmax=zdranP[1][0];
              double delp=zdranP[1][0]-zdranP[2][0];
              ipres=floor((pmax-presR)/delp)+1;
              double tmin=zdranP[0][npontos+1];
              double tmax=zdranP[0][1];
              double delt=zdranP[0][1]-zdranP[0][2];
              itemp=floor((tmax-tempR)/delt)+1;
	          double razpres=(dzdpP[ipres][0]-presR)/(dzdpP[ipres][0]-dzdpP[ipres+1][0]);
	          double raztemp=(dzdpP[0][itemp]-tempR)/(dzdpP[0][itemp]-dzdpP[0][itemp+1]);
	          double latp1=(1-razpres)*(dzdpP[ipres][itemp])+razpres*(dzdpP[ipres+1][itemp]);
	          double latp2=(1-razpres)*(dzdpP[ipres][itemp+1])+razpres*(dzdpP[ipres+1][itemp+1]);
	          zt=((1-raztemp)*latp1+raztemp*latp2)/(PCis*6894.8);
          }
        }
      }
      else zt=DZDPOriginal(pres,temp);
    }
    else zt=DZDPOriginal(pres,temp);
  }
  else if(flashCompleto==1){
	  double masesp;
	  if(masespG<0)masesp=interpolaVarProd(pres, temp, rhogF);
	  else masesp=masespG;
	  double drhodp=interpolaVarProd(pres, temp, DrhogDpF);
	  double constante=((Deng*28.9625))/(8.0465*1000);
	  zt=((Deng*28.9625)*pres*98066.5/masesp)/(8.0465*1000*(temp + 273.15));
	  double tK=temp + 273.15;
	  zt=-(drhodp-constante/(zt*tK))*(zt*zt*tK/(constante*pres*98066.5));
  }
  else{
	  if((*vg1dSP).modoTransiente==0){
		  if(tabelaDinamica==0){
			  int iIER;
			  double tempZ;
			  double dZdT;
			  double drhogdT;
			  double dZdp;
			  double drhogdp;

			  Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
					  liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
					  presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
					  &tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
			  if(dVaporMassFraction<1e-15)dZdp=0.;
			  zt=dZdp/98066.5;
		  }
		  else{
			  if(temp<tabDin.tmin)temp=tabDin.tmin;
			  if(temp>tabDin.tmax)temp=tabDin.tmax;
			  if(pres<tabDin.pmin) pres=tabDin.pmin;
			  if(pres>tabDin.pmax)pres=tabDin.pmax;
			  if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
				  int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
				  int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
				  double raztemp;
				  double razpres;
				  double valT0;
				 double valT1;
				 posicT++;
				 posicP++;
				 double titP0T0=tabDin.tit[posicP][posicT];
				 double titP0T1=tabDin.tit[posicP][posicT+1];
				 double titP1T0=tabDin.tit[posicP+1][posicT];
				 double titP1T1=tabDin.tit[posicP+1][posicT+1];
				 double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
				 double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
				 if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
					  int iIER;
					  double tempZ;
					  double dZdT;
					  double drhogdT;
					  double dZdp;
					  double drhogdp;

					  Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
							  liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
							  presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
							  &tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
					  if(dVaporMassFraction<1e-15)dZdp=0.;
					  zt=dZdp/98066.5;
				 }
				 else{
					 raztemp=(temp-tabDin.valdZdP[0][posicT])/(tabDin.delT);
					 razpres=(pres-tabDin.valdZdP[posicP][0])/(tabDin.delP);
					 valT0=raztemp*tabDin.valdZdP[posicP][posicT+1]+(1.-raztemp)*tabDin.valdZdP[posicP][posicT];
					 valT1=raztemp*tabDin.valdZdP[posicP+1][posicT+1]+(1.-raztemp)*tabDin.valdZdP[posicP+1][posicT];
					 zt=razpres*valT1+(1.-razpres)*valT0;
				 }

			  }
			  else{
				  int iIER;
				  double tempZ;
				  double dZdT;
				  double drhogdT;
				  double dZdp;
				  double drhogdp;

				  Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
						  liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
						  presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
						  &tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
				  if(dVaporMassFraction<1e-15)dZdp=0.;
				  zt=dZdp/98066.5;
			  }
		  }
	  }
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.valdZdP[0][1]+(1.-raztemp)*miniTabDin.valdZdP[0][0];
			 valT1=raztemp*miniTabDin.valdZdP[1][1]+(1.-raztemp)*miniTabDin.valdZdP[1][0];
			 zt=razpres*valT1+(1.-razpres)*valT0;
	     }
  }
  return zt;
}//alteracao2

void ProFlu::tabela(double pmax, double tmax,
		                                 double pmin, double tmin) const{//alteracao2
    double pteste=pmax;
    double tteste=tmax;
    double dpteste=(pteste-pmin)/npontos;
    double dtteste=(tteste-tmin)/npontos;
    FullMtx<double> zdran(npontos+2,npontos+2);
    if(tab==1){
      for(int i=1; i<=npontos+1;i++){
    	  double ptesteN=PC*pteste/(0.9678411*14.69595);
    	  //double ttesteN=(TC*tteste-460-32)/1.8;
    	  zdranP[i][0]=pteste;
    	  for(int j=1;j<=npontos+1;j++){
    		  double ttesteN=(TC*tteste-460-32)/1.8;
    		  zdranP[0][j]=tteste;
    		  if(j==1){
    			  if(i==1)zdranP[i][j]=ZdranOriginal(ptesteN,ttesteN,1);
    			  else zdranP[i][j]=ZdranOriginal(ptesteN,ttesteN,1,
    					  0.8*(zdranP[i-1][j]), 2*(zdranP[i-1][j]));
    		  }
    		  else{
    			  double a=(zdranP[i][j-1]);
    			  double b=a;
    			  if(i>1)b=zdranP[i-1][j];
    			  if(a>b){
    				  double temp=a;
    				  a=b;
    				  b=temp;
    			  }
    			  zdranP[i][j]=ZdranOriginal(ptesteN,ttesteN,1, 0.5*a, 2*b);
    		  }
    		  tteste-=dtteste;
    	  }
    	  pteste-=dpteste;
    	  tteste=tmax;
      }
  	  for(int i=0; i<npontos+2; i++){
  		for(int j = 0; j< npontos+2; j++) zdran[i][j]=zdranP[i][j];
  	  }
      ostringstream saidaZDran;
	  saidaZDran << pathPrefixoArqSaida << "perfilZDranP.dat";
	  string tmp = saidaZDran.str();
	  ofstream escreveZ(tmp.c_str(), ios_base::out);
	  escreveZ<<zdran;
	  escreveZ.close();
	  arqRelatorioPerfis << tmp.c_str() << endl;
	  arqRelatorioPerfis.flush();
      }
}//alteracao2
void ProFlu::tabelaDZDP(double pmax, double tmax,
		                                 double pmin, double tmin) const{//alteracao2
    double pteste=pmax;
    double tteste=tmax;
    double dpteste=(pteste-pmin)/npontos;
    double dtteste=(tteste-tmin)/npontos;
    FullMtx<double> saida(npontos+2,npontos+2,0.);
    if(tab==1){
      for(int i=1; i<=npontos+1;i++){
    	double ptesteN=PC*pteste/(0.9678411*14.69595);
    	//double ttesteN=(TC*tteste-460-32)/1.8;
    	dzdpP[i][0]=pteste;
    	for(int j=1;j<=npontos+1;j++){
    		double ttesteN=(TC*tteste-460-32)/1.8;
    		dzdpP[0][j]=tteste;
    		dzdpP[i][j]=DZDPOriginal(ptesteN,ttesteN,1)*(PC*6894.8);
    		tteste-=dtteste;
    	}
    	pteste-=dpteste;
    	tteste=tmax;
      }
  	  for(int i=0; i<npontos+2; i++){
  	     for(int j = 0; j< npontos+2; j++) saida[i][j]=dzdpP[i][j];
  	  }
      ostringstream saidaDZDP;
	  saidaDZDP << pathPrefixoArqSaida << "perfilDZDPProd.dat";
	  string tmp = saidaDZDP.str();
	  ofstream escreveDZDP(tmp.c_str(), ios_base::out);
	  escreveDZDP<<saida;
	  escreveDZDP.close();
  	arqRelatorioPerfis << tmp.c_str() << endl;
  	arqRelatorioPerfis.flush();
    }
}//alteracao2
void ProFlu::tabelaDZDT(double pmax, double tmax,
		                                 double pmin, double tmin) const{//alteracao2
    double pteste=pmax;
    double tteste=tmax;
    double dpteste=(pteste-pmin)/npontos;
    double dtteste=(tteste-tmin)/npontos;
    FullMtx<double> saida(npontos+2,npontos+2,0.);
    if(tab==1){
      for(int i=1; i<=npontos+1;i++){
      	double ptesteN=PC*pteste/(0.9678411*14.69595);
      	//double ttesteN=(TC*tteste-460-32)/1.8;
    	dzdtP[i][0]=pteste;
    	for(int j=1;j<=npontos+1;j++){
    		double ttesteN=(TC*tteste-460-32)/1.8;
    		dzdtP[0][j]=tteste;
    		dzdtP[i][j]=DZDTOriginal(ptesteN,ttesteN,1)*(TC/1.8);
    		tteste-=dtteste;
    	}
    	pteste-=dpteste;
    	tteste=tmax;
      }
  	  for(int i=0; i<npontos+2; i++){
  	     for(int j = 0; j< npontos+2; j++) saida[i][j]=dzdtP[i][j];
  	  }
      ostringstream saidaDZDT;
	  saidaDZDT << pathPrefixoArqSaida << "perfilDZDTProd.dat";
	  string tmp = saidaDZDT.str();
	  ofstream escreveDZDT(tmp.c_str(), ios_base::out);
	  escreveDZDT<<saida;
	  escreveDZDT.close();
	  arqRelatorioPerfis << tmp.c_str() << endl;
	  arqRelatorioPerfis.flush();
    }
}//alteracao2

double ProFlu::BOFunc(double pres, double temp, double varRS)const{
//Fator volume de formacao, pressao em kgf/cm2 e temperatura em Celsius
double BO;
if(flashCompleto==0 || flashCompleto==3){
	if(IRGO>0){
	double kgf100= kgf(100);
    double ipres=psia(pres);
    double itemp= Faren(temp);
    /*double rDgLtemp=rDgL;
    double PCistemp=PCis;
    double TCisTemp=TCis;
    rDgL=1.;
    PCis=PC;
    TCis=TC;*/
    double SG100 = (kgf100*293*Deng)/(Zdran(kgf100, temp,1)*(temp + 273));
    /*rDgL=rDgLtemp;
    PCis=PCistemp;
    TCis=TCisTemp;*/
    double Dvazbeg = (itemp - 60)*API/SG100;
    double Avazbeg;
    double Bvazbeg;
// double BO;
    if(API > 30){
     Avazbeg = 0.11;
     Bvazbeg = 0.1337;
    }
    else{
     Avazbeg = 0.1751;
     Bvazbeg = -1.8106;
    }

    double PBolhavar = PB(pres, temp);
    double RSvar;
    if(varRS<0)RSvar= RS(pres, temp);
    else RSvar=varRS;

    if(ipres >= PBolhavar){
       double BOB=1+0.000467*IRGO + Avazbeg*Dvazbeg*0.0001 +
          Bvazbeg*IRGO*Dvazbeg*pow(10,-8);

       double CO=(-1433+5*IRGO+17.2*itemp-1180*SG100+12.61*API)/(ipres*pow(10,5));

       BO=BOB*exp(-(CO*(PBolhavar - ipres)));
    }
    else BO=1+0.000467*RSvar+Avazbeg*Dvazbeg*0.0001+Bvazbeg*RSvar*Dvazbeg*pow(10,-8);
    //if(BO < 1) BO=1;
	}
	else{
		double rhostd=1000.*141.5/(131.5+API);
		double rho1=rhostd-(624/rhostd)*(temp-15.56);
		double phi=(1.9818*1e-5)*exp((2.4169*1e-4)*temp+(8.6835*1e5+(4.1868*1e3)*temp)/(rho1*rho1));
		double rhois=rho1/(1.-phi*(pres-1.03322745)/1.01971621);
		BO=rhostd/rhois;
	}
 }
 else if(flashCompleto==1){
	 double rhol=interpolaVarProd(pres, temp, rholF);
	 double rs;
	 if(varRS<0)rs=interpolaVarProd(pres, temp, RSF);
	 else rs=varRS;
	 double rhostd=141.5*1000./(131.5+API);
	 BO=(rhostd+Deng*1.225*rs*6.29/35.31467)/rhol;
 }
 else{
	 if((*vg1dSP).modoTransiente==0){
		 if(tabelaDinamica==0){
			 double tempBO=1.;
			 int iIER;
			 Marlim_CalculateOilFormationVolumeFactor(pres, temp,
					 npseudo,  tempCrit, presCrit, fatAcent,  kij,
					 lij,TIndepPeneloux,liqModel,
					 oCalculatedLiqComposition, dLiquidPhaseMW, dVaporMassFraction,
					 dStockTankLiquidDensity, dStockTankVaporMassFraction,
					 &tempBO, &iIER);
			 if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)tempBO=1.;
			 //if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)tempBO=1.;
			 if(isnan(tempBO) || tempBO<1e-15)tempBO=1.;
			 if(tempBO>100 || tempBO<-100)tempBO=1.;
			 BO=tempBO;
		 }
		 else{
			 if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
		 		 	 int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
		 		 	 int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
		 		 	 double raztemp;
		 		 	 double razpres;
		 		 	 double valT0;
		 		 	 double valT1;
		 		 	 posicT++;
		 		 	 posicP++;
					 double titP0T0=tabDin.tit[posicP][posicT];
					 double titP0T1=tabDin.tit[posicP][posicT+1];
					 double titP1T0=tabDin.tit[posicP+1][posicT];
					 double titP1T1=tabDin.tit[posicP+1][posicT+1];
					 double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					 double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					 if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						 double tempBO=1.;
						 int iIER;
						 Marlim_CalculateOilFormationVolumeFactor(pres, temp,
								 npseudo,  tempCrit, presCrit, fatAcent,  kij,
								 lij,TIndepPeneloux,liqModel,
								 oCalculatedLiqComposition, dLiquidPhaseMW, dVaporMassFraction,
								 dStockTankLiquidDensity, dStockTankVaporMassFraction,
								 &tempBO, &iIER);
						 if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)tempBO=1.;
						 //if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)tempBO=1.;
						 if(isnan(tempBO) || tempBO<1e-15)tempBO=1.;
						 if(tempBO>100 || tempBO<-100)tempBO=1.;
						 BO=tempBO;
					 }
					 else{
						 raztemp=(temp-tabDin.valBO[0][posicT])/(tabDin.delT);
						 razpres=(pres-tabDin.valBO[posicP][0])/(tabDin.delP);
						 valT0=raztemp*tabDin.valBO[posicP][posicT+1]+(1.-raztemp)*tabDin.valBO[posicP][posicT];
						 valT1=raztemp*tabDin.valBO[posicP+1][posicT+1]+(1.-raztemp)*tabDin.valBO[posicP+1][posicT];
						 BO=razpres*valT1+(1.-razpres)*valT0;
					 }

			 }
			 else{
				 double tempBO=1.;
				 int iIER;
				 Marlim_CalculateOilFormationVolumeFactor(pres, temp,
						 npseudo,  tempCrit, presCrit, fatAcent,  kij,
						 lij,TIndepPeneloux,liqModel,
						 oCalculatedLiqComposition, dLiquidPhaseMW, dVaporMassFraction,
						 dStockTankLiquidDensity, dStockTankVaporMassFraction,
						 &tempBO, &iIER);
				 if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)tempBO=1.;
				 //if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)tempBO=1.;
				 if(isnan(tempBO) || tempBO<1e-15)tempBO=1.;
				 if(tempBO>100 || tempBO<-100)tempBO=1.;
				 BO=tempBO;
			 }
		 }
	 }
     else{
		 double raztemp;
		 double razpres;
		 double valT0;
		 double valT1;
		 if(temp<miniTabDin.tmin){
			 temp=miniTabDin.tmin;
		 }
		 if(temp>miniTabDin.tmax){
			 temp=miniTabDin.tmax;
		 }
		 if(pres<miniTabDin.pmin){
			 pres=miniTabDin.pmin;
		 }
		 if(pres>miniTabDin.pmax){
			 pres=miniTabDin.pmax;
		 }
		 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
		 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
		 valT0=raztemp*miniTabDin.valBO[0][1]+(1.-raztemp)*miniTabDin.valBO[0][0];
		 valT1=raztemp*miniTabDin.valBO[1][1]+(1.-raztemp)*miniTabDin.valBO[1][0];
		 BO=razpres*valT1+(1.-razpres)*valT0;
     }
 }

 return BO;
}

double ProFlu::dBOSatdP(double pres, double temp)const{
 double kgf100= kgf(100);
 double itemp= Faren(temp);
 /*double rDgLtemp=rDgL;
 double PCistemp=PCis;
 double TCisTemp=TCis;
 rDgL=1.;
 PCis=PC;
 TCis=TC;*/
 double SG100 = (kgf100*293*Deng)/(Zdran(kgf100, temp,1)*(temp + 273));
 /*rDgL=rDgLtemp;
 PCis=PCistemp;
 TCis=TCisTemp;*/
 double Dvazbeg = (itemp - 60)*API/SG100;
 double Bvazbeg;
 if(API > 30)  Bvazbeg = 0.1337;
 else  Bvazbeg = -1.8106;
 return (0.000467+Bvazbeg*Dvazbeg*pow(10.,-8.))*dRSdP(pres, temp);
}

double ProFlu::interpolaCpl(double pres, double temp) const{
    int ipres=0;
    int itemp=0;
    int ipmarcador;
    int itmarcador;
    double vcpl;
    int ndiv=npontos-1;
    if(pres<cpl[1][0] || pres>=cpl[ndiv+1][0] || temp<cpl[0][1] || temp>=cpl[0][ndiv+1]) vcpl=CalorLiqOriginal(pres, temp);

    else{

      int e, m, d;
      e = 0;
      d = ndiv+1;
      while (e <= d) {
	      m = (e + d)/2;
	      ipmarcador=m;
	      if (cpl[m][0] > pres && cpl[m-1][0]<=pres){
	    	  ipres=m-1;
	    	  break;
	      }
	      if (cpl[m][0] < pres) e = m + 1;
	      else d = m - 1;
	  }
      e = 0;
      d = ndiv+1;
      while (e <= d) {
 	      m = (e + d)/2;
 	      itmarcador=m;
 	      if (cpl[0][m] > temp && cpl[0][m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (cpl[0][m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	  }
      double razpres=(cpl[ipres][0]-pres)/(cpl[ipres][0]-cpl[ipres+1][0]);
      double raztemp=(cpl[0][itemp]-temp)/(cpl[0][itemp]-cpl[0][itemp+1]);
      double latp1=(1-razpres)*(cpl[ipres][itemp])+razpres*(cpl[ipres+1][itemp]);
      double latp2=(1-razpres)*(cpl[ipres][itemp+1])+razpres*(cpl[ipres+1][itemp+1]);
      vcpl=(1-raztemp)*latp1+raztemp*latp2;
      double CPWI;
      double tempK=temp+273.16;
      double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
      if(tempK<410.0) CPWI=4185.5*(2.13974-9.68137*tempK/1000.+2.68536*tempK*tempK/100000.-
      2.42139*pow(10.,-8.)*tempK*tempK*tempK);
      else CPWI=4185.5*(-11.1558+7.96443*tempK/100.-1.74799*tempK*tempK/10000.+
      1.29156*pow(10.,-7.)*tempK*tempK*tempK);
      vcpl=1.*(xliq*CPWI+(1-xliq)*vcpl);
     }
    return vcpl;
}

double ProFlu::CalorLiqOriginal(double pres, double temp) const{
 double CPWI;
 double Bcp=0.06103;
 double tempfar=Faren(temp);
 double tempK=temp+273.16;
 double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
 double CPOI=4187.0*((2.6948*pow(10,-6)*API+3.88402/10000.)*tempfar+(0.0027665*API+0.366079)-Bcp);
 if(tempK<410.0) CPWI=4185.5*(2.13974-9.68137*tempK/1000.+2.68536*tempK*tempK/100000.-
 2.42139*pow(10.,-8.)*tempK*tempK*tempK);
 else CPWI=4185.5*(-11.1558+7.96443*tempK/100.-1.74799*tempK*tempK/10000.+
 1.29156*pow(10.,-7.)*tempK*tempK*tempK);
 return 1.*(xliq*CPWI+(1-xliq)*CPOI);
 //return 3000.;
}

double ProFlu::CalorLiq(double pres, double temp) const{
	double vcp;
	if(flashCompleto!=2){
		if(flashCompleto==0 || flashCompleto==3){
			if(ModelCp>0)vcp=interpolaCpl(pres, temp);
			else vcp=CalorLiqOriginal(pres, temp);
		}
		else{
			vcp=interpolaVarProd(pres, temp, cplF);
			double CPWI;
			//double Bcp=0.06103;
			double tempfar=Faren(temp);
			double tempK=temp+273.16;
			double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
			if(tempK<410.0) CPWI=4185.5*(2.13974-9.68137*tempK/1000.+2.68536*tempK*tempK/100000.-
					2.42139*pow(10.,-8.)*tempK*tempK*tempK);
			else CPWI=4185.5*(-11.1558+7.96443*tempK/100.-1.74799*tempK*tempK/10000.+
					1.29156*pow(10.,-7.)*tempK*tempK*tempK);
			vcp= 1.*(xliq*CPWI+(1-xliq)*vcp);
		}
	}
	else{
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double entalp;
				double cp;
				Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 0,oCalculatedLiqComposition,
						npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
						kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dLiquidPhaseMW,
						&entalp, &cp, &iIER);
				if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)cp=2000.;
				//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)cp=2000.;
				vcp=cp;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					raztemp=(temp-tabDin.cplF[0][posicT])/(tabDin.delT);
					razpres=(pres-tabDin.cplF[posicP][0])/(tabDin.delP);
					valT0=raztemp*tabDin.cplF[posicP][posicT+1]+(1.-raztemp)*tabDin.cplF[posicP][posicT];
					valT1=raztemp*tabDin.cplF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.cplF[posicP+1][posicT];
					vcp=razpres*valT1+(1.-razpres)*valT0;

				}
				else{
					int iIER;
					double entalp;
					double cp;
					Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 0,oCalculatedLiqComposition,
						 npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
						 kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dLiquidPhaseMW,
						&entalp, &cp, &iIER);
					if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)cp=2000.;
					//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)cp=2000.;
					vcp=cp;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.cplF[0][1]+(1.-raztemp)*miniTabDin.cplF[0][0];
			 valT1=raztemp*miniTabDin.cplF[1][1]+(1.-raztemp)*miniTabDin.cplF[1][0];
			 vcp=razpres*valT1+(1.-razpres)*valT0;
	     }
		double CPWI;
		//double Bcp=0.06103;
		double tempfar=Faren(temp);
		double tempK=temp+273.16;
		double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
		if(tempK<410.0) CPWI=4185.5*(2.13974-9.68137*tempK/1000.+2.68536*tempK*tempK/100000.-
				2.42139*pow(10.,-8.)*tempK*tempK*tempK);
		else CPWI=4185.5*(-11.1558+7.96443*tempK/100.-1.74799*tempK*tempK/10000.+
				1.29156*pow(10.,-7.)*tempK*tempK*tempK);
		vcp= 1.*(xliq*CPWI+(1-xliq)*vcp);
	}
	return vcp;
}

double ProFlu::interpolaCpg(double pres, double temp) const{
    int ipres=0;
    int itemp=0;
    int ipmarcador;
    int itmarcador;
    double vcpg;
    int ndiv=npontos-1;
    if(pres<cpg[1][0] || pres>=cpg[ndiv+1][0] || temp<cpg[0][1] || temp>=cpg[0][ndiv+1]) vcpg=CalorGasOriginal(pres, temp);

    else{

      int e, m, d;
      e = 0;
      d = ndiv+1;
      while (e <= d) {
	      m = (e + d)/2;
	      ipmarcador=m;
	      if (cpg[m][0] > pres && cpg[m-1][0]<=pres){
	    	  ipres=m-1;
	    	  break;
	      }
	      if (cpg[m][0] < pres) e = m + 1;
	      else d = m - 1;
	  }
      e = 0;
      d = ndiv+1;
      while (e <= d) {
 	      m = (e + d)/2;
 	      itmarcador=m;
 	      if (cpg[0][m] > temp && cpg[0][m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (cpg[0][m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	  }
      double razpres=(cpg[ipres][0]-pres)/(cpg[ipres][0]-cpg[ipres+1][0]);
      double raztemp=(cpg[0][itemp]-temp)/(cpg[0][itemp]-cpg[0][itemp+1]);
      double latp1=(1-razpres)*(cpg[ipres][itemp])+razpres*(cpg[ipres+1][itemp]);
      double latp2=(1-razpres)*(cpg[ipres][itemp+1])+razpres*(cpg[ipres+1][itemp+1]);
      vcpg=(1-raztemp)*latp1+raztemp*latp2;
     }
    return vcpg;
}

double ProFlu::interpolaDrholDT(double pres, double temp) const{
    int ipres;
    int itemp;
    int ipmarcador;
    int itmarcador;
    double vdrholdt;
    int ndiv=npontos-1;
    if(pres<drholdT[1][0] || pres>=drholdT[ndiv+1][0] || temp<drholdT[0][1] ||
    		temp>=drholdT[0][ndiv+1]) vdrholdt=DrholDTOriginal(pres, temp);

    else{

      int e, m, d;
      e = 0;
      d = ndiv+1;
      while (e <= d) {
	      m = (e + d)/2;
	      ipmarcador=m;
	      if (drholdT[m][0] > pres && drholdT[m-1][0]<=pres){
	    	  ipres=m-1;
	    	  break;
	      }
	      if (drholdT[m][0] < pres) e = m + 1;
	      else d = m - 1;
	  }
      e = 0;
      d = ndiv+1;
      while (e <= d) {
 	      m = (e + d)/2;
 	      itmarcador=m;
 	      if (drholdT[0][m] > temp && drholdT[0][m-1]<=temp){
 	    	  itemp=m-1;
 	    	  break;
 	      }
 	      if (drholdT[0][m-1] < temp) e = m + 1;
 	      else d = m - 1;
 	  }
      double razpres=(drholdT[ipres][0]-pres)/(drholdT[ipres][0]-drholdT[ipres+1][0]);
      double raztemp=(drholdT[0][itemp]-temp)/(drholdT[0][itemp]-drholdT[0][itemp+1]);
      double latp1=(1-razpres)*(drholdT[ipres][itemp])+razpres*(drholdT[ipres+1][itemp]);
      double latp2=(1-razpres)*(drholdT[ipres][itemp+1])+razpres*(drholdT[ipres+1][itemp+1]);
      vdrholdt=(1-raztemp)*latp1+raztemp*latp2;
     }
    return vdrholdt;
}

double ProFlu::CalorGasOriginal(double pres, double temp) const{
  //return 1.*((1.88+1.712*Deng)*temp+2651.-897.2*Deng);
	double RG=(8.0465*1000)/(rDgL*Deng*28.9625);
	double tempK=temp+273.16;
	if (tempK < 240.0) tempK = 240.0;
    if (tempK > 480.0) tempK = 480.0;
    double CP0mM = 0.047801*tempK + 21.714;
    double CP0mE = 0.12532*tempK + 15.341;
    double CP0mP = 0.19347*tempK + 16.371;
    double CP0mB = 0.24799*tempK + 25.170;

    double CP0mCO2 = -3.7498E-05*(tempK*tempK) + 6.6944E-02*tempK + 20.538;

    double yM;
    if(yco2 < 1.0) yM = 1.7278095 + 0.26875568*yco2 - 1.3142579*rDgL*Deng;
    else yM = 0.0;
    if (yM > 1.0) yM = 1.0;
    if (yM < 0.0) yM = 0.0;

    double yE = 4.0*(1.0 - yco2 - yM)/7.0;
    if(yE < 0.0) yE = 0.0;
    double yP = 0.5*yE;
    double yB = 1.0 - yM - yE - yP - yco2;
    if(yB < 0.0) yB = 0.0;

    double CP0 = 1000.0*(yM*CP0mM + yE*CP0mE + yP*CP0mP + yB*CP0mB + yco2*CP0mCO2)/(rDgL*Deng*28.9625);
    double CV0 = CP0 - RG;

    double zg=Zdran(pres,temp);
    double dzdt=DZDT(pres,temp);
    double dzdp=DZDP(pres,temp);
    double TF=Faren(temp);
    double TRan=TF+459.67;
    double Ppsi=psia(pres);
    double PR = Ppsi/PCis;
    double TR = TRan/TCis;
    double RhoR = 0.27*PR/zg/TR;

    double CV = CV0 - 5.104577*RG*(1.0 - (0.6275544 - 0.03688454/TR + 0.2023674/(TR*TR))*RhoR + 0.03612444*TR*(RhoR*RhoR) -
           0.0015258965*TR*pow(RhoR,5) - exp(-0.7210*(RhoR*RhoR))*(1.0 + 0.3605*(RhoR*RhoR)))/TR/(TR*TR);

    return  CV+RG*pow(zg+TR*dzdt,2)/(zg-PR*dzdp);



}

double ProFlu::CalorGas(double pres, double temp) const{
	double vcp;
	if(flashCompleto!=2 || (flashCompleto==2 && tab==1)){
		if(flashCompleto==0 || flashCompleto==3){
			if(ModelCp>0)vcp=interpolaCpg(pres, temp);
			else vcp=CalorGasOriginal(pres, temp);
		}
		else{
			vcp=interpolaVarProd(pres, temp, cpgF);
		}
	}
	else{
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double entalp;
				double cp;
				Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 1,oCalculatedVapComposition,
						npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
						kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dVaporPhaseMW,
						&entalp, &cp, &iIER);
				if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)cp=4000.;
				//if(dVaporMassFraction<1e-15)cp=4000.;
				vcp=cp;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					raztemp=(temp-tabDin.cpgF[0][posicT])/(tabDin.delT);
					razpres=(pres-tabDin.cpgF[posicP][0])/(tabDin.delP);
					valT0=raztemp*tabDin.cpgF[posicP][posicT+1]+(1.-raztemp)*tabDin.cpgF[posicP][posicT];
					valT1=raztemp*tabDin.cpgF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.cpgF[posicP+1][posicT];
					vcp=razpres*valT1+(1.-razpres)*valT0;

				}
				else{
					int iIER;
					double entalp;
					double cp;
					Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 1,oCalculatedVapComposition,
						 npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
						 kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dVaporPhaseMW,
						&entalp, &cp, &iIER);
					if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)cp=4000.;
					//if(dVaporMassFraction<1e-15)cp=4000.;
					vcp=cp;
			  }
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.cpgF[0][1]+(1.-raztemp)*miniTabDin.cpgF[0][0];
			 valT1=raztemp*miniTabDin.cpgF[1][1]+(1.-raztemp)*miniTabDin.cpgF[1][0];
			 vcp=razpres*valT1+(1.-razpres)*valT0;
	     }
	}
	return vcp;
}

double ProFlu::CalorGasVolMod(double pres, double temp,double rhogini) const{
  double cpg=CalorGas(pres,temp);
  double rhog=MasEspGas(pres,temp);
  double dzdt=DZDT(pres,temp,rhog);
  double dzdp=DZDP(pres,temp,rhog);
  double zg=Zdran(pres,temp,rhog);
  double dvdt=(1/rhog)*(1/(temp+273)+dzdt/zg);
  double dvdp=(1/rhog)*(-1/(pres*98066.5)+dzdp/zg);

  double RG=(8.0465*1000)/(rDgL*Deng*28.9625);
  double cv=cpg+(temp+273)*dvdt*dvdt/dvdp;
  double rel1=zg+(temp+273)*dzdt;
  double rel2=zg-dzdp*(pres*98066.5);
  return cv-RG*rel1*(0*1-rel1/rel2);
}
double ProFlu::CalorGasVolMod2(double pres, double temp,double rhogini) const{
  double cpg=CalorGas(pres,temp);
  double rhog;
  if(rhogini<0)rhog=MasEspGas(pres,temp);
  else rhog=rhogini;
  double dzdt=DZDT(pres,temp,rhog);
  double dzdp=DZDP(pres,temp,rhog);
  double zg=Zdran(pres,temp,rhog);
  double dvdt=(1/rhog)*(1/(temp+273)+dzdt/zg);
  double dvdp=(1/rhog)*(-1/(pres*98066.5)+dzdp/zg);

  double RG=(8.0465*1000)/(rDgL*Deng*28.9625);
  double cv=cpg+(temp+273)*dvdt*dvdt/dvdp;
  double rel1=zg+(temp+273)*dzdt;
  double rel2=zg-dzdp*(pres*98066.5);
  return cv+RG*rel1*(rel1/rel2);
}
double ProFlu::CalorGasPresMod(double pres, double temp,double rhogini) const{

	double rhog;
	if(rhogini<0)rhog=MasEspGas(pres,temp);
	else rhog=rhogini;
	double dzdt=DZDT(pres,temp,rhog);
	double dzdp=DZDP(pres,temp,rhog);
	double zg=Zdran(pres,temp,rhog);
	double drhodp=rhog*(1/(pres*98066.5)-dzdp/zg);

	double RG=(8.0465*1000)/(rDgL*Deng*28.9625);
	double rel1=zg+(temp+273)*dzdt;
	double rel2=zg-dzdp*(pres*98066.5);
	return (1/(rhog*zg))*rel2*(1-rel1/rel2)-(pres*98066.5/(rhog*rhog))*drhodp;
}

double ProFlu::CondLiq(double pres,double temp)const{
	   double tempfar=Faren(temp);
       double tempK=temp+273.16;
       double SGO=141.5/(API+131.5);
       double XKWDI;
       double XKOI=116.8*(1.-3.*(tempfar-32.)/10000.)/1000.;
       double xliq= MasAgua(pres,temp)/MasLiq(pres, temp);
       if(tempK<273.16)XKWDI=418.4*(273.778+3.9*tempK)/1000000.;
       else if(tempK<413.16)XKWDI=418.4*(-1390.53+15.1937*tempK-0.0190398*tempK*tempK)/1000000.;
       else XKWDI=418.4*(-339.838+9.86669*tempK-0.0123045*tempK*tempK)/1000000.;
       return XKOI*pow(XKWDI/XKOI,xliq);
}

double ProFlu::CondOleo(double pres,double temp)const{
	   double tempfar=Faren(temp);
       double tempK=temp+273.16;
       double SGO=141.5/(API+131.5);
       double XKWDI;
       double XKOI=116.8*(1.-3.*(tempfar-32.)/10000.)/1000.;
       return XKOI;
}

double ProFlu::ConstAdG(double pres, double temp,double rhogini) const{
  double cpg=CalorGas(pres,temp);
  double rhog;
  if(rhogini<0)rhog=MasEspGas(pres,temp);
  else rhog=rhogini;
  double dzdt=DZDT(pres,temp,rhog);
  double dzdp=DZDP(pres,temp,rhog);
  double zg=Zdran(pres,temp,rhog);
  double dvdt=(1/rhog)*(1/(temp+273)+dzdt/zg);
  double dvdp=(1/rhog)*(-1/(pres*98066.5)+dzdp/zg);
  double aux=cpg/(cpg+(temp+273)*dvdt*dvdt/dvdp);
  if(aux<0.)aux=0.1;//atencao: observar esta acochambracao
  return 0*cpg/(cpg-pres*98066.5/((temp+273)*rhog*zg))+aux;
}

double ProFlu::CondGas(double pres,double temp)const{
 double ppas=98066.5*pres;
 double fatT=(temp+273.15)/191.1;
 double XK1=3.04314/100.+(1.3242/10000.+1.27534*temp/10000000.)*temp;
 double RXK=0.99783+(1.973/100000000.+7.8868*pow(10.,-16.)*ppas)*ppas;
 if(fatT<3.0)return XK1*(1.+(1.-RXK)*(fatT-3.)/1.354);
 else return XK1;
}

double ProFlu::DrholDTOriginal(double pres,double temp) const{
	double roIS=MasEspoleo(pres,temp);
	return -624/roIS;
}
double ProFlu::DrholDT(double pres, double temp) const{
	double vdrholdt;
	if(flashCompleto!=2){
		if(flashCompleto==0||flashCompleto==3){
			if(Modeljtl!=2){
				if(Modeljtl==1)vdrholdt=interpolaDrholDT(pres, temp);
				else vdrholdt=DrholDTOriginal(pres, temp);
			}
			else vdrholdt=0;
		}
		else{
			vdrholdt=interpolaVarProd(pres, temp, DrholDtF);
		}
	}
	else{
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double tempZ;
				double dZdT;
				double drholdT;
				double dZdp;
				double drholdp;

				Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
						liqModel, 0,  npseudo,oCalculatedLiqComposition, tempCrit,
						presCrit, fatAcent, kij, lij, TIndepPeneloux, dLiquidPhaseMW,
						&tempZ, &dZdT, &drholdT, &dZdp, &drholdp, &iIER);
				if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)drholdT=1.;
				//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)drholdT=1.;
				vdrholdt=drholdT;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						int iIER;
						double tempZ;
						double dZdT;
						double drholdT;
						double dZdp;
						double drholdp;

						Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
								liqModel, 0,  npseudo,oCalculatedLiqComposition, tempCrit,
								presCrit, fatAcent, kij, lij, TIndepPeneloux, dLiquidPhaseMW,
								&tempZ, &dZdT, &drholdT, &dZdp, &drholdp, &iIER);
						if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)drholdT=1.;
						//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)drholdT=1.;
						vdrholdt=drholdT;
					}
					else{
						raztemp=(temp-tabDin.DrholDtF[0][posicT])/(tabDin.delT);
						razpres=(pres-tabDin.DrholDtF[posicP][0])/(tabDin.delP);
						valT0=raztemp*tabDin.DrholDtF[posicP][posicT+1]+(1.-raztemp)*tabDin.DrholDtF[posicP][posicT];
						valT1=raztemp*tabDin.DrholDtF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.DrholDtF[posicP+1][posicT];
						vdrholdt=razpres*valT1+(1.-razpres)*valT0;
					}

				}
				else{
					int iIER;
					double tempZ;
					double dZdT;
					double drholdT;
					double dZdp;
					double drholdp;

					Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
							 liqModel, 0,  npseudo,oCalculatedLiqComposition, tempCrit,
							 presCrit, fatAcent, kij, lij, TIndepPeneloux, dLiquidPhaseMW,
							 &tempZ, &dZdT, &drholdT, &dZdp, &drholdp, &iIER);
					if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)drholdT=1.;
					//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)drholdT=1.;
					vdrholdt=drholdT;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.DrholDtF[0][1]+(1.-raztemp)*miniTabDin.DrholDtF[0][0];
			 valT1=raztemp*miniTabDin.DrholDtF[1][1]+(1.-raztemp)*miniTabDin.DrholDtF[1][0];
			 vdrholdt=razpres*valT1+(1.-razpres)*valT0;
	     }
	}
	return vdrholdt;
}

double ProFlu::JTL(double pres,double temp) const{
 //return -1/MasEspLiq(pres,temp);
/*	double rholst=1000*131.5/(141.5+API);
	double bo=BOFunc(pres, temp);
	double mult=6.29/35.31467;
	double rs=RS(pres,temp)*mult;
	//double rs=RS(pres,temp);
	double alfgd=(bo-1.)/bo;
	double dtemp=0.001*temp;
	double rhogd;
	double DrhogdDt;
	double jtl;
	if(bo>1.){
	 rhogd=1.225*Deng*rs/(bo-1);
	 double titgd=rhogd*alfgd/(rhogd*alfgd+rholst*(1.-alfgd));
	 DrhogdDt=1.225*Deng*(rs/(bo-1)-RS(pres,temp-dtemp)*mult/(BOFunc(pres, temp-dtemp)-1.))/dtemp;
	 //jtl=(1.-alfgd)*(1./rholst-1*(temp+273.15)*(624/(rholst*rholst*rholst)))+
	 			    //alfgd*(1./rhogd+(temp+273.15)*DrhogdDt/(rhogd*rhogd));
	 jtl=(1.-titgd)*(1./rholst-(temp+273.15)*(624/(rholst*rholst*rholst)))+
	 	 			    titgd*(1./rhogd+(temp+273.15)*DrhogdDt/(rhogd*rhogd));
	}
	else{
		 rhogd=0.;
		 DrhogdDt=0.;
		 jtl=(1./rholst-1*(temp+273.15)*(624/(rholst*rholst*rholst)));
	}

	double fw=BSW/(bo+BSW-BSW*bo);
	return -(1-fw)*jtl-fw*(1/(1000.*Denag)-(temp+273.15)*(3*1e-4)/(1000.*Denag));*/
	double bo=BOFunc(pres, temp);
	double roIS=MasEspoleo(pres,temp);
	double fw=BSW/(bo+BSW-BSW*bo);
	double jtl;
	jtl=-(1./roIS+(1.- JTLiquidoSimple)*(temp+273.15)*(DrholDT(pres, temp)/(roIS*roIS)));
	double jtw;
	double corrjtw=1;
	if(Modeljtl==2)corrjtw=0;
	if(flashCompleto!=1 || modelaAgua==1)jtw=(-1/(1000.*Denag)+corrjtw*(temp+273.15)*(3*1e-4)/(1000.*Denag));
	else{
		double drhowdt=interpolaVarProd(pres, temp, DrhowDtF);
		jtw=-1/(1000.*Denag)+(temp+273.15)*drhowdt/(1000.*Denag*1000*Denag);
	}

	return (1-fw)*jtl+fw*jtw;

	/*double kgf100= kgf(100);
	double itemp= Faren(temp);
	double ipres=psia(pres);
	double SG100 = (kgf100*293*Deng)/(Zdran(kgf100, temp)*(temp + 273));
	double Djt = (itemp - 60)*API/SG100;
	double Ajt;
	double Bjt;
	double Cjt;
	double temporario=API*Bvb*Cvb*IRGO*logl(10.)/(Avb*SG100*itemp*itemp);
	double dpbdt=pow(10.,-API*Cvb/itemp);
	dpbdt=dpbdt*temporario*pow(dpbdt*Bvb*IRGO/SG100,(1-Avb)/Avb);

	if(API>30. || IRGO>750.){
		Ajt=4.67e-4;
		Bjt=1.1e-5;
		Cjt=1.337e-9;
	}
	else{
		Ajt=4.677e-4;
		Bjt=1.751e-5;
		Cjt=-1.811e-8;
	}

	double PBolhavar = PB(pres, temp);
	double RSvar= RS(pres, temp);
	double alfap;
	double BO;
	 if(ipres >= PBolhavar){
	    double BOB=1+Ajt*IRGO + Djt*(Bjt+Cjt*IRGO);
	    double CO=(-1433+5*IRGO+17.2*itemp-1180*SG100+12.61*API)/(ipres*pow(10,5));
	    double DCODT=(17.2e-5)/ipres;
	    BO=BOB*exp(-(CO*(PBolhavar - ipres)));
	    double DBOBDT=(API/SG100)*(Bjt+Cjt*RSvar);
	    alfap=DBOBDT/BOB+(PBolhavar-ipres)*DCODT+CO*dpbdt;

	 }
	 else{
		 BO=1.+Ajt*RSvar+Djt*(Bjt+Cjt*RSvar);
		 double DBODT=(API/SG100)*(Bjt+Cjt*RSvar);
		 alfap=DBODT/BO;
	 }
	 alfap=alfap/1.8;
	 jtl=-(1-(temp+273.14)*alfap)/roIS;
	 return (1-fw)*jtl-fw*(1/(1000.*Denag)-(temp+273.15)*(3*1e-4)/(1000.*Denag));*/
}

/*double ProFlu::JTL(double pres,double temp) const{
 //return -1/MasEspLiq(pres,temp);
	double rholst=1000*131.5/(141.5+API);
	double bo=BOFunc(pres, temp);
	double mult=6.29/35.31467;
	double rs=RS(pres,temp)*mult;
	//double rs=RS(pres,temp);
	double alfgd=(bo-1.)/bo;
	double dtemp=0.001*temp;
	double rhogd;
	double DrhogdDt;
	double jtl;
	if(bo>1.){
	 rhogd=1.225*Deng*rs/(bo-1);
	 double titgd=rhogd*alfgd/(rhogd*alfgd+rholst*(1.-alfgd));
	 DrhogdDt=1.225*Deng*(rs/(bo-1)-RS(pres,temp-dtemp)*mult/(BOFunc(pres, temp-dtemp)-1.))/dtemp;
	 //jtl=1./((1.-alfgd)*rholst+alfgd*rhogd)+(temp+273.15)*((1.-alfgd)*(-624/(rholst*rholst*rholst))+alfgd*DrhogdDt/(rhogd*rhogd));
	 jtl=1./((1.-alfgd)*rholst+alfgd*rhogd)+(temp+273.15)*((1.-titgd)*(-624/(rholst*rholst*rholst))+titgd*DrhogdDt/(rhogd*rhogd));
	}
	else{
		 rhogd=0.;
		 DrhogdDt=0.;
		 jtl=(1./rholst-1*(temp+273.15)*(624/(rholst*rholst*rholst)));
	}

	double fw=BSW/(bo+BSW-BSW*bo);
	return -(1-fw)*jtl-fw*(1/(1000.*Denag)-(temp+273.15)*(3*1e-4)/(1000.*Denag));
}*/

double ProFlu::JTG(double pres,double temp, double rhog) const{
 //return ((temp+273.16)*((8.0465*1000./(Deng*28.9625*98066.5))/pres)*(Zdran(pres, temp)+(temp+273.16)*DZDT(pres, temp))-1/MasEspGas(pres, temp));
	if(rhog<0)return (temp+273.16)*DZDT(pres, temp)/(Zdran(pres, temp)*MasEspGas(pres, temp));
	else return (temp+273.16)*DZDT(pres, temp,rhog)/(Zdran(pres, temp,rhog)*rhog);
}


double ProFlu::EntalpLiq(double pres,double temp) const{
   //return quadleg(TCF,temp,2,2,PCF,10)+quadleg(PCF,pres,1,2,temp,10);
	if(flashCompleto!=2){
		if(flashCompleto==0 || flashCompleto==3)
			return quadleg(0,temp,2,2,2,10)+quadleg(2,pres,1,2,temp,10);
		else
			return interpolaVarProd(pres, temp, HlF);
	}
	else{
		double entalp;
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double cp;
				Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 0,oCalculatedLiqComposition,
						npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
						kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dLiquidPhaseMW,
						&entalp, &cp, &iIER);
				if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)entalp=1000.;
				//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)entalp=1000.;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						int iIER;
						double cp;
						Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 0,oCalculatedLiqComposition,
								npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
								kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dLiquidPhaseMW,
								&entalp, &cp, &iIER);
						if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)entalp=1000.;
					}
					else{
						raztemp=(temp-tabDin.HlF[0][posicT])/(tabDin.delT);
						razpres=(pres-tabDin.HlF[posicP][0])/(tabDin.delP);
						valT0=raztemp*tabDin.HlF[posicP][posicT+1]+(1.-raztemp)*tabDin.HlF[posicP][posicT];
						valT1=raztemp*tabDin.HlF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.HlF[posicP+1][posicT];
						entalp=razpres*valT1+(1.-razpres)*valT0;
					}

				}
				else{
					int iIER;
					double cp;
					Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 0,oCalculatedLiqComposition,
							npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
							kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dLiquidPhaseMW,
							&entalp, &cp, &iIER);
					if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)entalp=1000.;
					//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)entalp=1000.;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.HlF[0][1]+(1.-raztemp)*miniTabDin.HlF[0][0];
			 valT1=raztemp*miniTabDin.HlF[1][1]+(1.-raztemp)*miniTabDin.HlF[1][0];
			 entalp=razpres*valT1+(1.-razpres)*valT0;
	     }
		return entalp;
	}
}

double ProFlu::EntalpLiqIsoT(double pres,double temp) const{
   double pb=kgf(PB(pres, temp));
   return quadleg(pb,pres,1,2,temp,10);
}

double ProFlu::EntalpLiqIsoB(double pres,double temp) const{
   return quadleg(TCF,temp,2,2,pres,10);
}

double ProFlu::EntalpGas(double pres,double temp) const{
   //return quadleg(TCF,temp,2,1,PCF,10)+quadleg(PCF,pres,1,1,temp,10);
	if(flashCompleto!=2){
		if(flashCompleto==0 || flashCompleto==3)
			return quadleg(0,temp,2,1,2,10)+quadleg(2,pres,1,1,temp,10);
		else
			return interpolaVarProd(pres, temp, HgF);
	}
	else{
		double entalp;
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double cp;
				Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 1,oCalculatedVapComposition,
						npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
						kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dVaporPhaseMW,
						&entalp, &cp, &iIER);
				if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)entalp=1000.;
			//if(dVaporMassFraction<1e-15)entalp=1000.;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						int iIER;
						double cp;
						Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 1,oCalculatedVapComposition,
								npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
								kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dVaporPhaseMW,
								&entalp, &cp, &iIER);
						if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)entalp=1000.;
					}
					else{
						raztemp=(temp-tabDin.HgF[0][posicT])/(tabDin.delT);
						razpres=(pres-tabDin.HgF[posicP][0])/(tabDin.delP);
						valT0=raztemp*tabDin.HgF[posicP][posicT+1]+(1.-raztemp)*tabDin.HgF[posicP][posicT];
						valT1=raztemp*tabDin.HgF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.HgF[posicP+1][posicT];
						entalp=razpres*valT1+(1.-razpres)*valT0;
					}

				}
				else{
					int iIER;
					double cp;
					Marlim_CalculatePhaseCpAndEnthalpy(pres, temp, 1,oCalculatedVapComposition,
							npseudo,tempCrit,presCrit, fatAcent,TIndepPeneloux,
							kij,  lij, liqModel, 0, CpIGCoefs, CpIGCoefs, dVaporPhaseMW,
							&entalp, &cp, &iIER);
					if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)entalp=1000.;
					//if(dVaporMassFraction<1e-15)entalp=1000.;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.HgF[0][1]+(1.-raztemp)*miniTabDin.HgF[0][0];
			 valT1=raztemp*miniTabDin.HgF[1][1]+(1.-raztemp)*miniTabDin.HgF[1][0];
			 entalp=razpres*valT1+(1.-razpres)*valT0;
	     }
		return entalp;
	}
}

double ProFlu::EntalpGasIsoT(double pres,double temp) const{
   double pb=kgf(PB(pres, temp));
   return quadleg(pb,pres,1,1,temp,10);
}

double ProFlu::EntalpGasIsoB(double pres,double temp) const{
   return quadleg(TCF,temp,2,1,pres,10);
}

//void ProFlu::gauleg(const double x1, const double x2, Vcr<double>& x, Vcr<double>& w)const{//subrotina auxiliar para quadratura gaussiana
//usando polinomio de Legendre, da os pontos x da quadartura e seus pesos, x1 e x2 sao os limites de integracao
void ProFlu::gauleg(const double x1, const double x2, double* const x, double* const w,const int size)const{//subrotina auxiliar para quadratura gaussiana
//usando polinomio de Legendre, da os pontos x da quadartura e seus pesos, x1 e x2 sao os limites de integracao

 const double EPS=1.0e-14;
 double z1,z,xm,xl,pp,p3,p2,p1;
// int n=x.size();
 int n=size;
 int m=(n+1)/2;
 xm=0.5*(x2+x1);
 xl=0.5*(x2-x1);
 for(int i=0;i<m;i++){
   z=cos(3.141592654*(i+0.75)/(n+0.5));
   do{
     p1=1.0;
     p2=0.0;
     for(int j=0;j<n;j++){
       p3=p2;
       p2=p1;
       p1=((2.0*j+1.0)*z*p2-j*p3)/(j+1);
     }
     pp=n*(z*p1-p2)/(z*z-1.0);
     z1=z;
     z=z1-p1/pp;
   }while(fabs(z-z1)>EPS);
   x[i]=xm-xl*z;
   x[n-1-i]=xm+xl*z;
   w[i]=2.0*xl/((1.0-z*z)*pp*pp);
   w[n-1-i]=w[i];
 }
}

double ProFlu::quadleg(const double x1, const double x2, int tipo,int fase,const double par,int n)const{//quadratura gaussiana usando Legendre, x1-> limite inferior, x2-> limite superior
//n->numero de pontos da quadratura. tipo->integra na pressao (1); integra na temperatura (2). fase->gas->1; liquido->2, parametro-> pressao se tipo=2 e temperatura se tipo=1
 //Vcr<double> x(n);
 //Vcr<double> w(n);

 double* x;
 x = new double [n];
 double* w;
 w = new double [n];

 gauleg(x1,x2,x,w,n);
 double s=0;
 if(tipo==1){
  if(fase==1){
   double Rgas=8.0465*1000./(rDgL*Deng*28.9625*98066.5);
   for(int i=0;i<n;i++)s+=-w[i]*((par+273.16)*(Rgas/x[i])*(Zdran(x[i], par)+(par+273.16)*DZDT(x[i], par))-1/MasEspGas(x[i], par))*98066.5;
  }
  else for(int i=0;i<n;i++)s+=(w[i]/MasEspLiq(x[i], par))*98066.5;
 }
 else{
  if(fase==1)
   for(int i=0;i<n;i++)s+=w[i]*CalorGas(par, x[i]);
  else for(int i=0;i<n;i++)s+=w[i]*CalorLiq(par, x[i]);
 }
 if(x)delete[] x;
 if(w)delete[] w;
 return s;
}

double ProFlu::MasEspoleo(double pres, double temp,double varRS) const{
//Massa especifica do oleo kg/m^3, pressao em kgf/cm2 e temperatura em Celcius
	if(flashCompleto!=2){
		if(flashCompleto==0 || flashCompleto==3){
			double rsVar;
			if(varRS<0 && flashCompleto!=2)rsVar=RS(pres, temp);
			else rsVar=varRS;
			return (1000*141.5/(131.5 + API) +
					rDgD*Deng*1.225*rsVar*6.29/35.31467)/BOFunc(pres, temp,rsVar);
		}
		else
			return interpolaVarProd(pres, temp, rholF);
	}
	else{
		double rhol;
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double presC=pres;
				double tempC=temp;
				double zfac;

				Marlim_CalculatePhaseDensity(presC,  tempC, 0, oCalculatedLiqComposition, npseudo,
						tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dLiquidPhaseMW, &rhol,
						&zfac,&iIER);
				if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)rhol=900.;
				//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)rhol=900.;
				if(rhol<1e-15) rhol=900;
				return rhol;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					raztemp=(temp-tabDin.rholF[0][posicT])/(tabDin.delT);
					razpres=(pres-tabDin.rholF[posicP][0])/(tabDin.delP);

					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						rhol=MasEspOleoComp(pres, temp);
					}
					else{
						valT0=raztemp*tabDin.rholF[posicP][posicT+1]+(1.-raztemp)*tabDin.rholF[posicP][posicT];
						valT1=raztemp*tabDin.rholF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.rholF[posicP+1][posicT];
						rhol=razpres*valT1+(1.-razpres)*valT0;
						if(rhol<1e-15) rhol=900;
					}

				}
				else{
					int iIER;
					//double rhol;
					double presC=pres;
					double tempC=temp;
					double zfac;

					Marlim_CalculatePhaseDensity(presC,  tempC, 0, oCalculatedLiqComposition, npseudo,
							tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dLiquidPhaseMW, &rhol,
							&zfac,&iIER);
					if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4)rhol=900.;
					//if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15)rhol=900.;
					if(rhol<1e-15) rhol=900;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.rholF[0][1]+(1.-raztemp)*miniTabDin.rholF[0][0];
			 valT1=raztemp*miniTabDin.rholF[1][1]+(1.-raztemp)*miniTabDin.rholF[1][0];
			 rhol=razpres*valT1+(1.-razpres)*valT0;
	     }

		if(rhol<1e-15) rhol=900;
		return rhol;
	}
}

double ProFlu::MasEspOleoComp(double pres, double temp) const{

		double rhol;
		double bo;
		int iIER;
		double presC=pres;
		double tempC=temp;
		double zfac;

		Marlim_CalculatePhaseDensity(presC,  tempC, 0, oCalculatedLiqComposition, npseudo,
						tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dLiquidPhaseMW, &rhol,
						&zfac,&iIER);
		if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4){
					bo=1.;
					rhol=900.;
		}
		else bo=BOFunc(pres, temp);//bo=dStockTankLiquidDensity/rhol;
		if(rhol<1e-15) rhol=900;

		if(rhol<1e-15) rhol=900;
		return rhol;
}

double ProFlu::drhodt(double pres, double temp) const{
	if(flashCompleto!=2){
		double z=Zdran(pres,temp);
		double masesp=((rDgL*Deng*28.9625)*pres*98066.5)/(8.0465*1000*z*(temp + 273));
		double dzdt=DZDT(pres,temp);
		return -masesp*(1./(temp+272.15)+dzdt/z);
	}
	else{
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double tempZ;
				double dZdT;
				double drhogdT;
				double dZdp;
				double drhogdp;

				Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
						liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
						presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
						&tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
				if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)drhogdT=1.;
				return drhogdT;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					double drhogdT;
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						int iIER;
						double tempZ;
						double dZdT;
						double drhogdT;
						double dZdp;
						double drhogdp;

						Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
								liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
								presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
								&tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
						if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)drhogdT=1.;
					}
					else{
						raztemp=(temp-tabDin.DrhogDtF[0][posicT])/(tabDin.delT);
						razpres=(pres-tabDin.DrhogDtF[posicP][0])/(tabDin.delP);
						valT0=raztemp*tabDin.DrhogDtF[posicP][posicT+1]+(1.-raztemp)*tabDin.DrhogDtF[posicP][posicT];
						valT1=raztemp*tabDin.DrhogDtF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.DrhogDtF[posicP+1][posicT];
						drhogdT=razpres*valT1+(1.-razpres)*valT0;
					}
					return drhogdT;

				}
				else{
					int iIER;
					double tempZ;
					double dZdT;
					double drhogdT;
					double dZdp;
					double drhogdp;

					Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
							liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
							presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
							&tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
					if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)drhogdT=1.;
					return drhogdT;
				}
			}
		}
	     else{
	    	 double drhogdT;
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.DrhogDtF[0][1]+(1.-raztemp)*miniTabDin.DrhogDtF[0][0];
			 valT1=raztemp*miniTabDin.DrhogDtF[1][1]+(1.-raztemp)*miniTabDin.DrhogDtF[1][0];
			 drhogdT=razpres*valT1+(1.-razpres)*valT0;
			 return drhogdT;
	     }
	}
}

double ProFlu::drhodp(double pres, double temp) const{
	if(flashCompleto!=2){
		double z=Zdran(pres,temp);
		double masesp=((rDgL*Deng*28.9625)*pres*98066.5)/(8.0465*1000*z*(temp + 272.15));
		double dzdp=DZDP(pres,temp);
		return masesp*(1./(pres*98066.52)-dzdp/z);
	}
	else{
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double tempZ;
				double dZdT;
				double drhogdT;
				double dZdp;
				double drhogdp;

				Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
						liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
						presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
						&tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
				if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)drhogdp=1.;
				return drhogdp;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					double drhogdp;
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						int iIER;
						double tempZ;
						double dZdT;
						double drhogdT;
						double dZdp;
						double drhogdp;

						Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
								liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
								presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
								&tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
						if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)drhogdp=1.;
					}
					else{
						raztemp=(temp-tabDin.DrhogDpF[0][posicT])/(tabDin.delT);
						razpres=(pres-tabDin.DrhogDpF[posicP][0])/(tabDin.delP);
						valT0=raztemp*tabDin.DrhogDpF[posicP][posicT+1]+(1.-raztemp)*tabDin.DrhogDpF[posicP][posicT];
						valT1=raztemp*tabDin.DrhogDpF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.DrhogDpF[posicP+1][posicT];
						drhogdp=razpres*valT1+(1.-razpres)*valT0;
					}
					return drhogdp;

				}
				else{
					int iIER;
					double tempZ;
					double dZdT;
					double drhogdT;
					double dZdp;
					double drhogdp;

					Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(pres, temp,
							liqModel, 1,  npseudo,oCalculatedVapComposition, tempCrit,
							presCrit, fatAcent, kij, lij, TIndepPeneloux, dVaporPhaseMW,
							&tempZ, &dZdT, &drhogdT, &dZdp, &drhogdp, &iIER);
					if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)drhogdp=1.;
					return drhogdp;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.DrhogDpF[0][1]+(1.-raztemp)*miniTabDin.DrhogDpF[0][0];
			 valT1=raztemp*miniTabDin.DrhogDpF[1][1]+(1.-raztemp)*miniTabDin.DrhogDpF[1][0];
			 return razpres*valT1+(1.-razpres)*valT0;
	     }
	}
}

void ProFlu::razDegD(double pres, double temp){
	if(flashCompleto==0  || flashCompleto==3|| (flashCompleto==2 && tab==1)){
		double rs=RS(pres, temp)* (6.29 / 35.31467);
		double SGDG = (API + 12.5)/50 - 0.0000035715*API*rs;
		if (SGDG < Deng) SGDG = Deng;
	/////////////////////////////////////////////////////////////////////////
		rDgD=SGDG/Deng;
		double SGFG;
		if(fabsl(RGO - rs)>1e-5)SGFG= Deng*(RGO-rDgD*rs)/(RGO - rs);
		else SGFG=Deng;
		if (SGFG < 0.5538){
			SGFG = 0.5538; // metano
			if(fabsl(rs)>1e-5)rDgD=-(SGFG*(RGO - rs)/Deng-RGO)/rs;
			else rDgD=1.;
		}
		if(SGFG>Deng){
			SGFG=Deng;
			if(fabsl(rs)>1e-5)rDgD=-(SGFG*(RGO - rs)/Deng-RGO)/rs;
			else rDgD=1.;
		}
	}
	else rDgD=1.;
	/////////////////////////////////////////////////////////////////////
}
void ProFlu::rzDegL(double pres , double temp){
	if(flashCompleto==0 || flashCompleto==3 || (flashCompleto==2 && tab==1)){
		double rs=RS(pres, temp)* (6.29 / 35.31467);
		double SGFG;
		if(fabsl(RGO - rs)>1e-5)SGFG= Deng*(RGO-rDgD*rs)/(RGO - rs);
		else SGFG=Deng;
		if (SGFG < 0.5538) SGFG = 0.5538; // metano
		if(SGFG>Deng)SGFG=Deng;
		rDgL=SGFG/Deng;
		//if(rDgL<=1+1e-15 &&rDgL>=1-1e-15)rDgD=1.;
	}
	else rDgL=1.;
}

double ProFlu::MasEspGas(double pres, double temp) const{
//Massa especifica do gas kg/m^3, pressao em kgf/cm2 e temperatura em Celcius
	if(flashCompleto==0 || flashCompleto==3 || (flashCompleto==2 && tab==1))
      return ((rDgL*Deng*28.9625)*pres*98066.5)/(8.0465*1000*Zdran(pres, temp)*(temp + 273.15));
	else if(flashCompleto==1)
		return interpolaVarProd(pres, temp, rhogF);
	else{
		double rhog;
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				/*int iIER;
				double presC=pres;
				double tempC=temp;
				double zfac;

				Marlim_CalculatePhaseDensity(presC,  tempC, 1, oCalculatedVapComposition, npseudo,
						tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dVaporPhaseMW, &rhog,
						&zfac,&iIER);
				if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)rhog=Deng*1.225;*/
				rhog=MasEspGasComp(pres, temp);
				//if(dVaporMassFraction<1e-15 || rhog<1e-15)rhog=Deng*1.225;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					raztemp=(temp-tabDin.rhogF[0][posicT])/(tabDin.delT);
					razpres=(pres-tabDin.rhogF[posicP][0])/(tabDin.delP);
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						rhog=MasEspGasComp(pres, temp);
					}
					else{
						valT0=raztemp*tabDin.rhogF[posicP][posicT+1]+(1.-raztemp)*tabDin.rhogF[posicP][posicT];
						valT1=raztemp*tabDin.rhogF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.rhogF[posicP+1][posicT];
						rhog=razpres*valT1+(1.-razpres)*valT0;
					}
					/*double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					if(titP0T0<1e-15 && titP0T1>(1-1e-15))valT0=tabDin.rhogF[posicP][posicT+1];
					else if(titP0T1<1e-15 && titP0T0>(1-1e-15))valT0=tabDin.rhogF[posicP][posicT];
					else valT0=raztemp*tabDin.rhogF[posicP][posicT+1]+(1.-raztemp)*tabDin.rhogF[posicP][posicT];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					if(titP1T0<1e-15 && titP1T1>(1-1e-15))valT1=tabDin.rhogF[posicP+1][posicT+1];
					else if(titP1T1<1e-15 && titP1T0>(1-1e-15))valT1=tabDin.rhogF[posicP][posicT];
					else valT1=raztemp*tabDin.rhogF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.rhogF[posicP+1][posicT];*/
				}
				else{
					/*int iIER;
					double presC=pres;
					double tempC=temp;
					double zfac;

					Marlim_CalculatePhaseDensity(presC,  tempC, 1, oCalculatedVapComposition, npseudo,
							tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dVaporPhaseMW, &rhog,
							&zfac,&iIER);
					if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)rhog=Deng*1.225;*/
					rhog=MasEspGasComp(pres, temp);
					//if(dVaporMassFraction<1e-15 || rhog<1e-15)rhog=Deng*1.225;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.rhogF[0][1]+(1.-raztemp)*miniTabDin.rhogF[0][0];
			 valT1=raztemp*miniTabDin.rhogF[1][1]+(1.-raztemp)*miniTabDin.rhogF[1][0];
			 rhog=razpres*valT1+(1.-razpres)*valT0;
	     }
		return rhog;
	}
}

double ProFlu::MasEspGasComp(double pres, double temp) const{
//Massa especifica do gas kg/m^3, pressao em kgf/cm2 e temperatura em Celcius
	int iIER;
	double presC=pres;
	double tempC=temp;
	double zfac;
	double rhog;

	Marlim_CalculatePhaseDensity(presC,  tempC, 1, oCalculatedVapComposition, npseudo,
			tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dVaporPhaseMW, &rhog,
			&zfac,&iIER);
	if(iCalculatedThermodynamicCondition==0 || iCalculatedThermodynamicCondition==1)rhog=Deng*1.225;
		return rhog;
}

double ProFlu::MasEspAgua(double pres, double temp) const{
	  double tfarAmb=Faren(20.);
	  double ppsiAmb=psia(1.);
	  double tfar=Faren(temp);
	  double ppsi=psia(pres);
	  double bwAmb = 1.0 + 1.2e-04*(tfarAmb - 60.0) + 1.0e-06*pow(tfarAmb - 60.0,2.) - 3.33e-06*ppsiAmb;
	  double bw = 1.0 + 1.2e-04*(tfar - 60.0) + 1.0e-06*pow(tfar - 60.0,2.) - 3.33e-06*ppsi;

	  double rholw=(1000./bw)*bwAmb;
	  double din=2.16;
	  double vi=1.-(Denag-din)/(1-din);
	  double x=vi*din/Denag;

	  double rhomist=(1-x)/rholw+x/(din*1000.);
	  return 1./rhomist;
}
double ProFlu::DMasEspAgua(double pres, double temp) const{
	  double tfarAmb=Faren(20.);
	  double ppsiAmb=psia(1.);
	  double tfar=Faren(temp);
	  double ppsi=psia(pres);
	  double bwAmb = 1.0 + 1.2e-04*(tfarAmb - 60.0) + 1.0e-06*pow(tfarAmb - 60.0,2.) - 3.33e-06*ppsiAmb;
	  double bw = 1.0 + 1.2e-04*(tfar - 60.0) + 1.0e-06*pow(tfar - 60.0,2.) - 3.33e-06*ppsi;

	  double rholw=(1000./bw)*bwAmb;
	  double din=2.16;
	  double vi=1.-(Denag-din)/(1-din);
	  double x=vi*din/Denag;

	  double rhomist=(1-x)/rholw+x/(din*1000.);
	  double t1= -1./(rhomist*rhomist);
	  double t2=((1-x)/(rholw*rholw))*(1000./(bw*bw))*bwAmb;
	  double t3=- 3.33e-06*0.9678411*14.69595;
	  return t1*t2*t3;
	  //return Denag*1000.;
}
double ProFlu::BAFunc(double pres, double temp) const{

	double rhoa=MasEspAgua(pres, temp);
    return (1000.*Denag/rhoa);
}

double ProFlu::MasEspLiq(double pres, double temp,double varRS) const{
	if(flashCompleto!=2){
		double rs;
		if(varRS<0)rs=RS(pres, temp);
		else rs=varRS;
		double masoleo=((1-BSW)*(1000*141.5/(131.5 + API) +
    		rDgD*Deng*1.225*rs*6.29/35.31467));
		double bo=BOFunc(pres, temp, rs);
		double masagua=BSW*1000.*Denag;
		double rhoa;
		double ba=1.;
		if(BSW>0.){
			rhoa=MasEspAgua(pres, temp);
			ba=(1000.*Denag/rhoa);
		}
		return (masoleo+masagua)/((1-BSW)*bo+BSW*ba);
	}
	else{
		double rhol;
		double bo;
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				/*int iIER;
				double presC=pres;
				double tempC=temp;
				double zfac;

				Marlim_CalculatePhaseDensity(presC,  tempC, 0, oCalculatedLiqComposition, npseudo,
						tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dLiquidPhaseMW, &rhol,
						&zfac,&iIER);
				if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4){
					bo=1.;
					rhol=900.;
				}
				else bo=BOFunc(pres, temp);//bo=dStockTankLiquidDensity/rhol;
				if(rhol<1e-15) rhol=900;*/
				rhol=MasEspLiqComp(pres, temp);
				bo=-1.;
			}
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					double titP0T0=tabDin.tit[posicP][posicT];
					double titP0T1=tabDin.tit[posicP][posicT+1];
					double titP1T0=tabDin.tit[posicP+1][posicT];
					double titP1T1=tabDin.tit[posicP+1][posicT+1];
					double titTotal=titP0T0+titP0T1+titP1T0+titP1T1;
					double multTit=titP0T0*titP0T1*titP1T0*titP1T1;
					if(fabsl(multTit)<1e-15 && (fabsl(titTotal)>1e-15/*||fabsl(titTotal-2.)<1e-15||fabsl(titTotal-1.)<1e-15*/)){
						rhol=MasEspLiqComp(pres, temp);
						bo=-1.;
					}
					else{
						raztemp=(temp-tabDin.rholF[0][posicT])/(tabDin.delT);
						razpres=(pres-tabDin.rholF[posicP][0])/(tabDin.delP);
						valT0=raztemp*tabDin.rholF[posicP][posicT+1]+(1.-raztemp)*tabDin.rholF[posicP][posicT];
						valT1=raztemp*tabDin.rholF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.rholF[posicP+1][posicT];
						rhol=razpres*valT1+(1.-razpres)*valT0;
						bo=BOFunc(pres, temp);
						if(rhol<1e-15) rhol=900;
					}
					/*raztemp=(temp-tabDin.rholF[0][posicT])/(tabDin.delT);
					razpres=(pres-tabDin.rholF[posicP][0])/(tabDin.delP);
					valT0=raztemp*tabDin.rholF[posicP][posicT+1]+(1.-raztemp)*tabDin.rholF[posicP][posicT];
					valT1=raztemp*tabDin.rholF[posicP+1][posicT+1]+(1.-raztemp)*tabDin.rholF[posicP+1][posicT];
					rhol=razpres*valT1+(1.-razpres)*valT0;
					bo=BOFunc(pres, temp);
					if(rhol<1e-15) rhol=900;*/
				}
				else{
					/*int iIER;
					//double rhol;
					double presC=pres;
					double tempC=temp;
					double zfac;

					Marlim_CalculatePhaseDensity(presC,  tempC, 0, oCalculatedLiqComposition, npseudo,
							tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dLiquidPhaseMW, &rhol,
							&zfac,&iIER);
					if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4){
						bo=1.;
						rhol=900.;
					}
					else bo=BOFunc(pres, temp);//bo=dStockTankLiquidDensity/rhol;
					if(rhol<1e-15) rhol=900;*/
					rhol=MasEspLiqComp(pres, temp);
					bo=-1.;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.rholF[0][1]+(1.-raztemp)*miniTabDin.rholF[0][0];
			 valT1=raztemp*miniTabDin.rholF[1][1]+(1.-raztemp)*miniTabDin.rholF[1][0];
			 rhol=razpres*valT1+(1.-razpres)*valT0;
			 bo=BOFunc(pres, temp);
	     }
		  //if(dVaporMassFraction>1.-1e-15 || dLiquidPhaseMW<1e-15){
			//  bo=1.;
			  //rhol=1000.;
		  //}
		  //if(rhol<1e-15)rhol=1000.;
		if(bo>0){
		  if(rhol<1e-15) rhol=900;
		  double rhoa=MasEspAgua(pres, temp);
		  if(isnan(rhoa)|| rhoa<1e-15)rhoa=1000.;
		  double ba;
		  if(dVaporMassFraction>1.-1e-15)ba=1.;
		  else ba=(1000.*Denag/rhoa);
		  double Fa=BSW*ba / (bo + ba*BSW - BSW * bo);
		  if(isnan(BSW))Fa=0.;
		  return rhol*(1.-Fa)+Fa*rhoa;
		}
		else return rhol;
	}
}

double ProFlu::MasEspLiqComp(double pres, double temp) const{

		double rhol;
		double bo;
		int iIER;
		double presC=pres;
		double tempC=temp;
		double zfac;

		Marlim_CalculatePhaseDensity(presC,  tempC, 0, oCalculatedLiqComposition, npseudo,
						tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dLiquidPhaseMW, &rhol,
						&zfac,&iIER);
		if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4){
					bo=1.;
					rhol=900.;
		}
		else bo=BOFunc(pres, temp);//bo=dStockTankLiquidDensity/rhol;
		if(rhol<1e-15) rhol=900;

		if(rhol<1e-15) rhol=900;
		double rhoa=MasEspAgua(pres, temp);
		if(isnan(rhoa)|| rhoa<1e-15)rhoa=1000.;
		double ba;
		if(dVaporMassFraction>1.-1e-15)ba=1.;
		else ba=(1000.*Denag/rhoa);
		double Fa=BSW*ba / (bo + ba*BSW - BSW * bo);
		if(isnan(BSW))Fa=0.;
		return rhol*(1.-Fa)+Fa*rhoa;
}

double ProFlu::MasOleo(double pres, double temp,double varRS) const{
// Massa de oleo((kg)) para 1 m^3\de oleo morto, pressao em kgf/cm2 e temperatura em Celcius
 double rs;
 if(varRS<0)rs=RS(pres, temp);
 else rs=varRS;
 double var;
 //if(rs>1e-15)
	 var=(1000*141.5/(131.5 + API)+rDgD*Deng*1.225*rs*6.29/35.31467);
 //else var=0.;
 return var;
}

double ProFlu::MasOleoHidra(double pres, double temp,double varRS) const{
// Massa de oleo((kg)) para 1 m^3\de mistura (oleo morto+agua), pressao em kgf/cm2 e temperatura em Celcius
	 double rs;
	 if(varRS<0)rs=RS(pres, temp);
	 else rs=varRS;
	 double var;
	 //if(rs>1e-15)
		 var=(1-BSW)*(1000*141.5/(131.5 + API)+rDgD*Deng*1.225*rs*6.29/35.31467);
	 //else var=0.;
	 return var;
}

double ProFlu::MasLiq(double pres, double temp,double varRS) const{
// Massa de Liquido((kg)) para 1 m^3\de mistura (oleo morto+agua), pressao em kgf/cm2 e temperatura em Celcius
	 double rs;
	 if(varRS<0)rs=RS(pres, temp);
	 else rs=varRS;
	 double var;
	 //if(rs>1e-15)
		 var=(1-BSW)*(1000*141.5/(131.5 + API)+rDgD*Deng*1.225*rs*6.29/35.31467)+BSW*1000*Denag;
	 //else var=BSW*1000*Denag;
	 return var;
}

double ProFlu::MasAgua(double pres, double temp) const{
// Massa de agua((kg)) para 1 m^3\de mistura (oleo morto+agua), pressao em kgf/cm2 e temperatura em Celcius
 return BSW*1000*Denag;
}

double ProFlu::MasGasLivre(double pres, double temp,double varRS) const{
//Massa de gas livre(kg) para 1 m^3 de oleo morto, pressao em kgf/cm2 e temperatura em Celcius
	 double rs;
	 if(varRS<0)rs=RS(pres, temp);
	 else rs=varRS;
     return 1.225*Deng*(IRGO - rDgD*rs)*6.29/35.31467;
}
double ProFlu::FracAguaInSitu(double pres, double temp,double varRS) const{
	if(flashCompleto!=2){
		double rs=RS(pres, temp);
		double MA=MasAgua(pres, temp);
		double MO=MasOleoHidra(pres, temp,rs);
		double rhoA=1000*Denag;
		double rhoO=MasEspoleo(pres, temp,rs);
		double VO=MO/rhoO;
		double VA=MA/rhoA;
		return VA/(VO+VA);
	}
	else{
		double bo;
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0){
				int iIER;
				double rhol;
				double presC=pres;
				double tempC=temp;
				double zfac;

				Marlim_CalculatePhaseDensity(presC,  tempC, 0, oCalculatedLiqComposition, npseudo,
						tempCrit, presCrit,fatAcent,TIndepPeneloux, kij, lij,liqModel, dLiquidPhaseMW, &rhol,
						&zfac,&iIER);
				//if(dVaporMassFraction>1.-1e-15){
				if(iCalculatedThermodynamicCondition==3 || iCalculatedThermodynamicCondition==4){
					bo=1.;
					rhol=900.;
				}
				else bo=BOFunc(pres, temp);//dStockTankLiquidDensity/rhol;
			}
			else{
				bo=BOFunc(pres, temp);
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.valBO[0][1]+(1.-raztemp)*miniTabDin.valBO[0][0];
			 valT1=raztemp*miniTabDin.valBO[1][1]+(1.-raztemp)*miniTabDin.valBO[1][0];
			 bo=razpres*valT1+(1.-razpres)*valT0;
	     }
		double rhoa=MasEspAgua(pres, temp);
		double ba=(1000.*Denag/rhoa);
		return BSW*ba / (bo + ba*BSW - BSW * bo);
	}
}

double ProFlu::MasGasLivreHidra(double pres, double temp,double varRS) const{
//Massa de gas livre(kg) para 1 m^3 de mistura (oleo morto+agua), pressao em kgf/cm2 e temperatura em Celcius
	 double rs;
	 if(varRS<0)rs=RS(pres, temp);
	 else rs=varRS;
	 return (1-BSW)*1.225*Deng*(IRGO - rDgD*rs)*6.29/35.31467;
}

double ProFlu::FracMass(double pres, double temp) const{
//Titulo de gas livre (oleo +gas), pressao em kgf/cm2 e temperatura em Celcius
	if(flashCompleto!=2 /*|| tabelaDinamica==0*/){
		double rs=RS(pres, temp);
		return MasGasLivre(pres, temp,rs)/(MasGasLivre(pres, temp,rs)+MasOleo(pres, temp,rs));
	}
	else{
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0)return dVaporMassFraction;
			else{
				double valx;
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					raztemp=(temp-tabDin.tit[0][posicT])/(tabDin.delT);
					razpres=(pres-tabDin.tit[posicP][0])/(tabDin.delP);
					valT0=raztemp*tabDin.tit[posicP][posicT+1]+(1.-raztemp)*tabDin.tit[posicP][posicT];
					valT1=raztemp*tabDin.tit[posicP+1][posicT+1]+(1.-raztemp)*tabDin.tit[posicP+1][posicT];
					valx=razpres*valT1+(1.-razpres)*valT0;

				}
				else{
					valx=dVaporMassFraction;
				}
				return valx;
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.tit[0][1]+(1.-raztemp)*miniTabDin.tit[0][0];
			 valT1=raztemp*miniTabDin.tit[1][1]+(1.-raztemp)*miniTabDin.tit[1][0];
			 return razpres*valT1+(1.-razpres)*valT0;
	     }
	}
}

double ProFlu::FracMassHidra(double pres, double temp) const{
//Titulo de gas livre (oleo +gas+agua), pressao em kgf/cm2 e temperatura em Celcius
	if(flashCompleto!=2 /*|| tabelaDinamica==0*/){
		double rs=RS(pres, temp);
		return MasGasLivreHidra(pres, temp,rs)/(MasGasLivreHidra(pres, temp,rs)+
                                  MasOleoHidra(pres, temp,rs)+MasAgua(pres, temp));
	}
	else{
		double boF = BOFunc(pres, temp);
		double baF = BAFunc(pres, temp);
		double fwF = BSW*baF/ (boF + baF*BSW -BSW * boF);
		double rhoOF=MasEspoleo(pres, temp);
		double rhoWF=MasEspAgua(pres, temp);
		double titF=(1-fwF)*rhoOF/((1-fwF)*rhoOF+fwF*rhoWF);
		double titG;
		if((*vg1dSP).modoTransiente==0){
			if(tabelaDinamica==0)titG=dVaporMassFraction;
			else{
				if(temp<tabDin.tmin)temp=tabDin.tmin;
				if(temp>tabDin.tmax)temp=tabDin.tmax;
				if(pres<tabDin.pmin) pres=tabDin.pmin;
				if(pres>tabDin.pmax)pres=tabDin.pmax;
				if(temp>=tabDin.tmin && temp<=tabDin.tmax && pres>=tabDin.pmin && pres<=tabDin.pmax){
					int posicT=floor(temp-tabDin.tmin)/(tabDin.delT);
					int posicP=floor(pres-tabDin.pmin)/(tabDin.delP);
					double raztemp;
					double razpres;
					double valT0;
					double valT1;
					posicT++;
					posicP++;
					raztemp=(temp-tabDin.tit[0][posicT])/(tabDin.delT);
					razpres=(pres-tabDin.tit[posicP][0])/(tabDin.delP);
					valT0=raztemp*tabDin.tit[posicP][posicT+1]+(1.-raztemp)*tabDin.tit[posicP][posicT];
					valT1=raztemp*tabDin.tit[posicP+1][posicT+1]+(1.-raztemp)*tabDin.tit[posicP+1][posicT];
					titG=razpres*valT1+(1.-razpres)*valT0;

				}
				else{
					titG=dVaporMassFraction;
				}
			}
		}
	     else{
			 double raztemp;
			 double razpres;
			 double valT0;
			 double valT1;
			 if(temp<miniTabDin.tmin){
				 temp=miniTabDin.tmin;
			 }
			 if(temp>miniTabDin.tmax){
				 temp=miniTabDin.tmax;
			 }
			 if(pres<miniTabDin.pmin){
				 pres=miniTabDin.pmin;
			 }
			 if(pres>miniTabDin.pmax){
				 pres=miniTabDin.pmax;
			 }
			 raztemp=(temp-miniTabDin.tmin)/(miniTabDin.tmax-miniTabDin.tmin);
			 razpres=(pres-miniTabDin.pmin)/(miniTabDin.pmax-miniTabDin.pmin);
			 valT0=raztemp*miniTabDin.tit[0][1]+(1.-raztemp)*miniTabDin.tit[0][0];
			 valT1=raztemp*miniTabDin.tit[1][1]+(1.-raztemp)*miniTabDin.tit[1][0];
			 titG=razpres*valT1+(1.-razpres)*valT0;
	     }
		return titG*titF/(1.+titG*(titF-1.));
	}
}

double ProFlu::interpola(Vcr<double> abs, Vcr<double> coord, double var){
	int comp=abs.size();
	int para=0;
	int pos=0;
	while(para==0){
		if(pos==comp-1)
			para=1;
		else if(var>=abs[pos] && var<abs[pos+1])
			para=1;
		else pos++;
	}
	if(pos==comp-1)return coord[comp-1];
	else{
		double raz=(var-abs[pos])/(abs[pos+1]-abs[pos]);
		return raz*coord[pos+1]+(1-raz)*coord[pos];
	}
}
double ProFlu::TensSuper(double pres,double temp, double bet, double tensupBet){

	double TF=Faren(temp);
	double Ppsi=psia(pres);

	Vcr<double> PRI(8,0.);
	PRI[0]=0.;PRI[1]=200.;PRI[2]=400.;PRI[3]=600.;PRI[4]=1000.;PRI[5]=1400.;PRI[6]=2200.;PRI[7]=2800.;
	Vcr<double> PERC(8,0.);
	PERC[0]=100.;PERC[1]=86.;PERC[2]=73.;PERC[3]=63.;PERC[4]=48.;PERC[5]=37.;PERC[6]=20.;PERC[7]=12.;
	Vcr<double> STVA(10,0.);
	STVA[0]=0.;STVA[1]=1000.;STVA[2]=2000.;STVA[3]=3000.;STVA[4]=4000.;STVA[5]=5000.;STVA[6]=6000.;STVA[7]=7000.;STVA[8]=8000.;STVA[9]=9000.;
	Vcr<double> STV74(10,0.);
	STV74[0]=75;STV74[1]=63.;STV74[2]=59.;STV74[3]=57.;STV74[4]=54.;STV74[5]=52.;STV74[6]=52.;STV74[7]=51.;STV74[8]=50.;STV74[9]=49.;
	Vcr<double> STV280(10,0.);
	STV280[0]=53;STV280[1]=46.;STV280[2]=40.;STV280[3]=33.;STV280[4]=26.;STV280[5]=21.;STV280[6]=21.;STV280[7]=22.;STV280[8]=23.;STV280[9]=24.;

	double SURO=0.;
	double SUR1;
	double SUR2;
	double SUR68;
	double SUR100;
	double FW=BSW/(BOFunc(pres,temp)*(1-BSW)+BSW);
	double FO=1.-FW;
	if(FO!=0.){
		SUR68=39.-0.2571*API;
		SUR100=37.5-0.2571*API;
		SUR1=SUR68-(TF-68.)*(SUR68-SUR100)/32.;
		if(TF<=68)SUR1=SUR68;
		if(TF>=100.)SUR1=SUR100;
		SUR2=interpola(PRI, PERC,Ppsi);
        SURO=SUR1*SUR2/100.;
	}
	double SURW=0.;
	double STW74;
	double STW280;
	if(FW!=0.){
		STW74=interpola(STVA,STV74,Ppsi);
		STW280=interpola(STVA,STV280,Ppsi);
		if(TF<74)SURW=STW74;
		else if(TF>280)SURW=STW280;
		else SURW=STW74-(STW74-STW280)*(TF-74)/(280-74);
	}
	double SURL=SURO*FO+SURW*FW;
	SURL/=1000.;
	SURL=(1-bet)*SURL+bet*tensupBet;

	return SURL;
	//return 0.07;
}

double ProFlu::TensSuperOleo(double pres,double temp){

	double TF=Faren(temp);
	double Ppsi=psia(pres);

	Vcr<double> PRI(8,0.);
	PRI[0]=0.;PRI[1]=200.;PRI[2]=400.;PRI[3]=600.;PRI[4]=1000.;PRI[5]=1400.;PRI[6]=2200.;PRI[7]=2800.;
	Vcr<double> PERC(8,0.);
	PERC[0]=100.;PERC[1]=86.;PERC[2]=73.;PERC[3]=63.;PERC[4]=48.;PERC[5]=37.;PERC[6]=20.;PERC[7]=12.;
	Vcr<double> STVA(10,0.);
	STVA[0]=0.;STVA[1]=1000.;STVA[2]=2000.;STVA[3]=3000.;STVA[4]=4000.;STVA[5]=5000.;STVA[6]=6000.;STVA[7]=7000.;STVA[8]=8000.;STVA[9]=9000.;
	Vcr<double> STV74(10,0.);
	STV74[0]=75;STV74[1]=63.;STV74[2]=59.;STV74[3]=57.;STV74[4]=54.;STV74[5]=52.;STV74[6]=52.;STV74[7]=51.;STV74[8]=50.;STV74[9]=49.;
	Vcr<double> STV280(10,0.);
	STV280[0]=53;STV280[1]=46.;STV280[2]=40.;STV280[3]=33.;STV280[4]=26.;STV280[5]=21.;STV280[6]=21.;STV280[7]=22.;STV280[8]=23.;STV280[9]=24.;

	double SURO=0.;
	double SUR1;
	double SUR2;
	double SUR68;
	double SUR100;
	double FW=BSW/(BOFunc(pres,temp)*(1-BSW)+BSW);
	double FO=1.-FW;
	if(FO!=0.){
		SUR68=39.-0.2571*API;
		SUR100=37.5-0.2571*API;
		SUR1=SUR68-(TF-68.)*(SUR68-SUR100)/32.;
		if(TF<=68)SUR1=SUR68;
		if(TF>=100.)SUR1=SUR100;
		SUR2=interpola(PRI, PERC,Ppsi);
        SURO=SUR1*SUR2/100.;
	}

	return SURO/1000.;
	//return 0.07;
}

double ProFlu::TensSuperAgua(double pres,double temp){

	double TF=Faren(temp);
	double Ppsi=psia(pres);

	Vcr<double> STVA(10,0.);
	STVA[0]=0.;STVA[1]=1000.;STVA[2]=2000.;STVA[3]=3000.;STVA[4]=4000.;STVA[5]=5000.;STVA[6]=6000.;STVA[7]=7000.;STVA[8]=8000.;STVA[9]=9000.;
	Vcr<double> STV74(10,0.);
	STV74[0]=75;STV74[1]=63.;STV74[2]=59.;STV74[3]=57.;STV74[4]=54.;STV74[5]=52.;STV74[6]=52.;STV74[7]=51.;STV74[8]=50.;STV74[9]=49.;
	Vcr<double> STV280(10,0.);
	STV280[0]=53;STV280[1]=46.;STV280[2]=40.;STV280[3]=33.;STV280[4]=26.;STV280[5]=21.;STV280[6]=21.;STV280[7]=22.;STV280[8]=23.;STV280[9]=24.;
	double FW=BSW/(BOFunc(pres,temp)*(1-BSW)+BSW);
	double SURW=0.;
	double STW74;
	double STW280;
	if(FW!=0.){
		STW74=interpola(STVA,STV74,Ppsi);
		STW280=interpola(STVA,STV280,Ppsi);
		if(TF<74)SURW=STW74;
		else if(TF>280)SURW=STW280;
		else SURW=STW74-(STW74-STW280)*(TF-74)/(280-74);
	}

	return SURW/1000.;
}

ostream& operator<<(ostream& s, const ProFlu& flui){

  s.width(6);
  s.precision(5);
  s<<"      API         = "<<flui[1]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"      RGO         = "<<flui[2]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"Densidade do Gas  = "<<flui[3]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"      BSW         = "<<flui[4]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"Densidade da agua = "<<flui[5]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"Temper -1 do ASTM = "<<flui[6]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"Viscos -1 do ASTM = "<<flui[7]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"Temper -2 do ASTM = "<<flui[8]<<"  "<<"\n";
  s.width(6);
  s.precision(5);
  s<<"Viscos -2 do ASTM = "<<flui[9]<<"  "<<"\n";
  s<<"\n";

  return s;
}

//template class ProFlu;

