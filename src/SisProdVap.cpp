/*
 * SisProdVap.cpp
 *
 *  Created on: 22 de mai de 2019
 *      Author: eduardo
 */

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include "SisProdVap.h"

SProdVap::SProdVap(string nomeArquivoEntrada, string nomeArquivoLog,varGlob1D* Vvg1dSP) :
    arq(nomeArquivoEntrada,Vvg1dSP), flut(arq.ncelp, arq.nvarprofp + 2 + 1 + 1), matglobP(2 * arq.ncelp, 2, 2), termolivreP(
        2 * arq.ncelp) {

  vg1dSP=Vvg1dSP;
  receb = 0;
  fechaM1 = 0;
  abreM1 = 0;
  celula = 0;

  ncelperftransp = 0;
  TrendLengthP = 0;
  MatTrendP = 0;
  resettrend = 0;
  ntrend = 0;
  TrendLengthTransP = 0;
  MatTrendTransP = 0;
  resettrendtrans = 0;
  ntrendtrans = 0;
  noextremo=1;//alteracao7
  noinicial=1;//alteracao7

  fontemassPRBuf=0.;//alteracao7
  fontemassCRBuf=0.;//alteracao7
  fontemassGRBuf=0.;//alteracao7

  presE=-1;//alteracao4
  tempE=-1;//alteracao4
  titE=-1;//alteracao4

  tempMedContDesc=10.;//alteracao5
  maxVecContDesc=1000;//alteracao5
  velmedDesc=0;//alteracao5
  tempmedDEsc=0;//alteracao5
  //extrem=1;

  montasistema();
}

SProdVap::SProdVap() :
    arq(), flut(1, 1 + 2 + 1 + 1), matglobP(2 * 1, 2, 2), termolivreP(2 * 1) {
	tfinal=0;
	dtini=0;
	contaLog=0;

	menorDx=0.;
    iterperm=0.;
    kSP=0.;
    KontaImprime=0.;
    indevento=0.;
    modoPerm=0.;
    ktMedMov=0.;
    pTotal=0.;
    jTotal=0.;

	alfTotal=0.;
	kontaTempoProf=0;
	dt=0.;
	nabreM1=0;
	nfechaM1=0;

  indTramo=-1;
	ncel=0;
	reinicia=0;
	presfim=0;

	pGSup=0;
	tGSup=0;
	temperatura=0;

	masSup=0;
	tempSup=0;

	mult=0;
	presMedMov=0;
	jMedMov=0;
	alfMedMov=0;
	tMedMov=0;

	aberto=0;
	tempoaberto=0;
	EstadoMaster1=0;
	contaMaster1=0;
	masChkSup=0;
	TransMassModel=0;
	indpigP=0;
  npig=0;
  receb=0;

  ncelperftransp=0;
  TrendLengthP=0;
  MatTrendP=0;
  resettrend=0;
  ntrend=0;
  TrendLengthTransP=0;
  MatTrendTransP=0;
  resettrendtrans=0;
  ntrendtrans=0;

    kontaTempoProf=0;
    kontaTempoTransProf=0;

    noextremo=1;//alteracao7
    noinicial=1;//alteracao7

	fontemassPRBuf=0.;//alteracao7
	fontemassCRBuf=0.;//alteracao7
	fontemassGRBuf=0.;//alteracao7

    presE=-1;//alteracao4
    tempE=-1;//alteracao4
    titE=-1;//alteracao4

    tempMedContDesc=10.;//alteracao5
    maxVecContDesc=1000;//alteracao5
    velmedDesc=0;//alteracao5
    tempmedDEsc=0;//alteracao5

    vg1dSP=0;

}

SProdVap& SProdVap::operator =(const SProdVap& sp) {

  if (nabreM1 > 0) delete[] abreM1;
  if (nfechaM1 > 0) delete[] fechaM1;

  if (arq.nperfistransp > 0) delete[] ncelperftransp;
  if (arq.ntendp > 0) {
    for (int i = 0; i < arq.ntendp; i++) {
      for (int j = 0; j < TrendLengthP[i]; j++)
        if (MatTrendP[i][j]) delete[] MatTrendP[i][j];
      if (MatTrendP[i]) delete[] MatTrendP[i];
    }
    if (MatTrendP) delete[] MatTrendP;
    if (TrendLengthP) delete[] TrendLengthP;
    if (resettrend) delete[] resettrend;
    if (ntrend) delete[] ntrend;
  }


  if (arq.ntendtransp > 0) {
    for (int i = 0; i < arq.ntendtransp; i++) {
      for (int j = 0; j < TrendLengthTransP[i]; j++)
        if (MatTrendTransP[i][j]) delete[] MatTrendTransP[i][j];
      if (MatTrendTransP[i]) delete[] MatTrendTransP[i];
    }
    if (MatTrendTransP) delete[] MatTrendTransP;
    if (TrendLengthTransP) delete[] TrendLengthTransP;
    if (resettrendtrans) delete[] resettrendtrans;
    if (ntrendtrans) delete[] ntrendtrans;
  }
  if (ncel > 0) delete[] celula;
  if (npig > 0) delete[] receb;

  vg1dSP=sp.vg1dSP;
  arq = sp.arq;
  flut = sp.flut;
  flutG = sp.flutG;
  matglobP = sp.matglobP;
  termolivreP = sp.termolivreP;
  matglobG = sp.matglobG;
  termolivreG = sp.termolivreG;
  receb = 0;
  fechaM1 = 0;
  abreM1 = 0;
  celula = 0;

  ncelperftransp = 0;
  TrendLengthP = 0;
  MatTrendP = 0;
  resettrend = 0;
  ntrend = 0;
  TrendLengthTransP = 0;
  MatTrendTransP = 0;
  resettrendtrans = 0;
  ntrendtrans = 0;

  noextremo=sp.noextremo;//alteracao7
  noinicial=sp.noinicial;//alteracao7

  fontemassPRBuf=0.;//alteracao7
  fontemassCRBuf=0.;//alteracao7
  fontemassGRBuf=0.;//alteracao7

  presE=-1;//alteracao4
  tempE=-1;//alteracao4
  titE=-1;//alteracao4

  tempMedContDesc=10.;//alteracao5
  maxVecContDesc=1000;//alteracao5
  velmedDesc=0;//alteracao5
  tempmedDEsc=0;//alteracao5



 // extrem=sp.extrem;

  montasistema();
}

void SProdVap::montasistema() {
  //cout<<"Arquivo MRT: "<<nomeArquivoEntrada<<"\n";
  //arq(nomeArquivoEntrada);
  //arq=Ler(nomeArquivoEntrada);

  try {
  indTramo = -1;
  //noextremo=0;//alteracao7
  //noinicial=0;//alteraca07
  KontaImprime = 0;
  tempoaberto = 0.;
  modoPerm = 0;
  iterperm = 0;
  masChkSup = 0;
  dt = arq.dtmax;
  dtini = arq.dtmax;
  tfinal = arq.tfinal;
  TransMassModel = 1;
  indpigP = 0;
  reinicia = 0;
  masChkSup = 0;
  (*vg1dSP).localtiny = arq.mono;
  (*vg1dSP).CritCond = arq.critcond;
  titRev=-1;

  arq.geraduto();
  ncel = arq.ncelp - 1;
  temperatura = arq.celp[0].textern;
  celula = new CelVap [ncel + 1];
  arq.geracelp(celula);
  if (arq.nipr > 0) arq.geraipr(celula);
  if (arq.ninjliq > 0) arq.gerafliqFonte(celula);
  if(celula[0].acsr.tipo!=0 && arq.ConContEntrada==1 ){
      // RN-302: Condicao de contorno de pressao no início da tubulacao e foi colocada uma fonte de massa neste inicio
      // incluir aviso
      logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
          "Foi escolhida uma condicao de contorno de pressao no início da tubulacao e foi colocada uma fonte de massa neste inicio, isto pode causar inconsistencias",
          "", "");
  }
//  if (arq.perm == 1 && celula[0].acsr.tipo == 0) {
  //  InjLiq injliqMRT(0, 0, 0, celula[0].flui, celula[0].fluicol);
  //  celula[0].acsr.tipo = 2;
  //  celula[0].acsr.injl = injliqMRT;
//  }
  if((arq.perm==1||(*vg1dSP).chaverede>0) && celula[0].acsr.tipo==0){//alteracao4
  	if(arq.ConContEntrada==0 || arq.CCPres.tit[0]<(1-(*vg1dSP).localtiny)){
  	  InjMultVap injliqMRT(0, 0);
  	  celula[0].acsr.tipo = 12;
  	  celula[0].acsr.injmvap = injliqMRT;
  	}
  	else{
  		  InjMultVap injgasMRT(0, 0);
    	  celula[0].acsr.tipo = 1;
    	  celula[0].acsr.injmvap = injgasMRT;
  	}
  }
  if (arq.nbcs > 0) arq.gerafBCS(celula);
  if (arq.nbvol > 0) arq.gerafBVOL(celula);
  if (arq.ndpreq > 0) arq.geraDPReq(celula);
  if (arq.master1.posic > 0.) arq.geraMaster1(celula);
  else {
    int verifica = 0;
    double Lverifica = 0.5 * celula[verifica].dx;
    while (celula[verifica].acsr.tipo != 0) {
      verifica++;
      Lverifica += 0.5 * (celula[verifica].dx + celula[verifica - 1].dx);
    }
    arq.master1.posic = verifica;
    arq.master1.comp = Lverifica;
    arq.geraMaster1(celula);
  }
  arq.gerapresfim(presfim, pGSup);
  chokeSup = choke(1., 1.);
  arq.gerachokesup(chokeSup);
  npig = arq.npig;
  if (npig > 0) {
    receb = new int[npig];
    for (int i = 0; i < npig; i++)
      receb[i] = arq.pig[i].receb;
  }
  if (celula[0].acsr.tipo != 1 && celula[0].acsr.tipo != 2 && celula[0].acsr.tipo != 3) {
    if (arq.perm == 0 && arq.ConContEntrada==0) {
      // RN-300: Nao existe nenhuma fonte na primeira celula do sistema de producao
      // incluir aviso
      logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
          "Nao existe nenhuma fonte na primeira celula do sistema de producao, esta nao e a condicao mais adequada para a simulacao transiente, problemas podem ocorrer nesta situacao, embora nao seja impeditivo para o modo",
          "", "");
    } else {
      // RN-301: Nao existe nenhuma fonte na primeira celula do sistema de producao, no modo permanente
      // incluir falha
      NumError("Nao existe nenhuma fonte na primeira celula do sistema de producao, no modo permanente esta e uma condicao necessaria");
    }
  }

  arq.listaevento(noinicial,noextremo);
  for (int i = 0; i < arq.evento.size(); i++)
    cout << i << "  " << arq.evento[i] << "\n";
  arq.geraevento(noinicial,noextremo);

  (*vg1dSP).lixo5 = 0.;
  (*vg1dSP).contador = 0;
    arq.atualiza(noinicial,noextremo, chokeSup, celula, pGSup, temperatura,
    		presE,tempE,titE,(*vg1dSP).lixo5);
    //arq.atualiza(noextremo,chokeSup,celula,celulaG,pGSup,temperatura,
  		     //presiniG,tempiniG,presE,tempE,titE,betaE,(*vg1dSP).lixo5);//alteracao7
    if (chokeSup.AreaGarg >= 0.6 * celula[ncel - 1].duto.area) aberto = 1;
    else aberto = 0;

  tempSup = celula[ncel].temp;
  masSup = celula[ncel - 1].MC;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////

  nabreM1 = arq.eventoabre;
  nfechaM1 = arq.eventofecha;
  abreM1 = new double[nabreM1];
  fechaM1 = new double[nfechaM1];
  for (int i = 0; i < nabreM1; i++)
    abreM1[i] = arq.Tevento[i];
  for (int i = 0; i < nfechaM1; i++)
    fechaM1[i] = arq.Teventof[i];

  //int* ncelperftransp;
  if (arq.nperfistransp > 0) {
    ncelperftransp = new int[arq.nperfistransp];
    for (int i = 0; i < arq.nperfistransp; i++) {
      int posiccel = arq.proftransp.posic[i];
      ncelperftransp[i] = celula[posiccel].calor.nglobal;
    }
  }

  //int* TrendLengthP;
  if (arq.ntendp > 0) {
    TrendLengthP = new int[arq.ntendp];
    for (int i = 0; i < arq.ntendp; i++)
      TrendLengthP[i] = 1 + round(arq.tfinal / arq.trendp[i].dt);
  }
  //double*** MatTrendP;
  //double* resettrend;
  //int* ntrend;
  if (arq.ntendp > 0) {
    resettrend = new double[arq.ntendp];
    ntrend = new int[arq.ntendp];
    MatTrendP = new double**[arq.ntendp];
    for (int i = 0; i < arq.ntendp; i++) {
      MatTrendP[i] = new double*[TrendLengthP[i]];
      for (int j = 0; j < TrendLengthP[i]; j++) {
        MatTrendP[i][j] = new double[arq.nvartrendp[i] + 1];
        for (int k = 0; k <= arq.nvartrendp[i]; k++)
          MatTrendP[i][j][k] = -10000.;
      }
      resettrend[i] = 0;
      ntrend[i] = 0;
    }

  }

  // int* TrendLengthTransP;
  if (arq.ntendtransp > 0) {
    TrendLengthTransP = new int[arq.ntendtransp];
    for (int i = 0; i < arq.ntendtransp; i++)
      TrendLengthTransP[i] = 1 + round(arq.tfinal / arq.trendtransp[i].dt);
  }
  //double*** MatTrendTransP;
  //double* resettrendtrans;
  //int* ntrendtrans;
  if (arq.ntendtransp > 0) {
    resettrendtrans = new double[arq.ntendtransp];
    ntrendtrans = new int[arq.ntendtransp];
    MatTrendTransP = new double**[arq.ntendtransp];
    for (int i = 0; i < arq.ntendtransp; i++) {
      MatTrendTransP[i] = new double*[TrendLengthTransP[i]];
      for (int j = 0; j < TrendLengthTransP[i]; j++)
        MatTrendTransP[i][j] = new double[2];
      for (int j = 0; j < TrendLengthTransP[i]; j++)
        for (int k = 0; k < 2; k++)
          MatTrendTransP[i][j][k] = -10000.;

      resettrendtrans[i] = 0;
      ntrendtrans[i] = 0;
    }

  }

  //double velaux, dtaux;
  //double lixo1,lixo2,lixo3,lixo4;

  kontaTempoProf = 0;
  if (arq.nperfisp > 0) {
    arq.imprimeProfile(celula, flut, (*vg1dSP).lixo5, indTramo);
    if (arq.profp.tempo[0] <= 0 + (*vg1dSP).localtiny) kontaTempoProf++;
  }

  kontaTempoTransProf = 0;
  if (arq.nperfistransp > 0) {
    arq.imprimeProfileTrans(celula, ncelperftransp, (*vg1dSP).lixo5, indTramo);
    if (arq.proftransp.tempo[0] <= 0 + (*vg1dSP).localtiny) kontaTempoTransProf++;
  }

   //ostringstream saidaLog;
   saidaLog << pathPrefixoArqSaida << "LogEvento" << ".dat";
   tmpLog = saidaLog.str();
   contaLog = 0;
   int nevent = arq.logevento.size();
   while (fabsl(arq.logevento[contaLog].instante - (*vg1dSP).lixo5) < dt && contaLog < nevent) {
     time_t now = time(0);
     tm *ltm = localtime(&now);    ///////////Retirado de https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
     ofstream escreveIni(tmpLog.c_str(), ios_base::app);
     escreveIni << "Evento Externo = ";
     escreveIni << arq.logevento[contaLog].instante << " ; ";
     escreveIni << arq.logevento[contaLog].duracao << " ; ";
     escreveIni << arq.logevento[contaLog].descricao << " ; ";
     escreveIni << "datahora = ";
     escreveIni << ltm->tm_mday << "/";
     escreveIni << 1 + ltm->tm_mon << "/";
     escreveIni << 1900 + ltm->tm_year << " ";
     escreveIni << 0 + ltm->tm_hour << ":";
     escreveIni << 0 + ltm->tm_min << ":";
     escreveIni << 0 + ltm->tm_sec;
     escreveIni << "\n";
     contaLog++;

     escreveIni.close();
   }

//  double masCL,masCG;
//  double gasinj;

  menorDx = 1e10;
  for (int i = 0; i <= ncel; i++)
    if (celula[i].dx < menorDx) menorDx = celula[i].dx;
  kSP = 0;
  indevento = 1;
  mult = 0.8;
  presMedMov = 0.;
  jMedMov = 0.;
  tMedMov = 60.;
  ktMedMov = 0.;
  pTotal = 0.;
  jTotal = 0.;
  alfTotal = 0.;
  } catch (exception& excInt) {
    cout << "EXCECAO INESPERADA: " << excInt.what() << endl;
    // incluir falha
    logger.log(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "", "", excInt.what());
    // gravar arquivo de log
    logger.writeOutputLog();
    // encerrar a aplicacao
    exit (EXIT_SUCCESS);
  }
}

void SProdVap::calctemp(int i, double tempantiga) {

  double dx = celula[i].dx;
  double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
  double dia = celula[i].duto.a;
  double area = 0.25 * M_PI * dia * dia;
  double alfmed = celula[i].alf;
  double ugsmed;
  //if(i<ncel)ugsmed=celula[i].QG/area;
  //else ugsmed=celula[i].QG/area;
  if (celula[i].alf > (*vg1dSP).localtiny) ugsmed = celula[i].QG / area;
  else {
    //if(i<ncel)ugsmed=celula[i+1].QG/area;
    //else ugsmed=celula[i].QG/area;
    ugsmed = 0.;
  }
  double ulsmed;
  //if(i<ncel)ulsmed=celula[i].QL/area;
  //else ulsmed=celula[i].QL/area;
  if (celula[i].alf < 1. - (*vg1dSP).localtiny) ulsmed = celula[i].QL / area;
  else {
    //if(i<ncel)ulsmed=celula[i+1].QL/area;
    //else ulsmed=celula[i].QL/area;
    ulsmed = 0.;
  }
  /*if(celula[i].acsr.tipo==8 && celula[i].acsr.bvol.freq>1.){
   ugsmed=celula[i].QG/area;
   ulsmed=celula[i].QL/area;
   }
   if(celula[i].acsr.tipo==5 && celula[i].acsr.chk.AreaGarg<=(1e-3+0.1)*celula[i].duto.area){
   ugsmed=celula[i].QG/area;
   ulsmed=celula[i].QL/area;
   }*/
  double rp = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp,TEMP);
  double rhol = rp;
  double rhog = celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp,TEMP);
  double cpl = celula[i].flui.CPlFunc(celula[i].pres, celula[i].temp);
  double cvl = celula[i].flui.CVlFunc(celula[i].pres, celula[i].temp);
  double cpg = celula[i].flui.CPgFunc(celula[i].pres, celula[i].temp);
  double cvg = celula[i].flui.CVgFuncMod(celula[i].pres, celula[i].temp);
  //double razcp=celula[i].flui.ConstAdG(pres[i],celula[i].tr);
  double jtl = celula[i].flui.JTlFunc(celula[i].pres, celula[i].temp);
  double jtg = celula[i].flui.JTgFunc(celula[i].pres, celula[i].temp);
  double hidro = (rhol * ulsmed + rhog * ugsmed) * area * 9.82 * sinl(celula[i].duto.teta);


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  celula[i].calor.Tint = celula[i].temp;
  celula[i].calor.Vint = ugsmed + ulsmed;
  celula[i].calor.dt = celula[i].dt;
  double condliq =  celula[i].flui.CondLiq(celula[i].pres, celula[i].temp);
  celula[i].calor.kint = condliq * (1 - alfmed) +
		  celula[i].flui.CondGas(celula[i].pres, celula[i].temp) * alfmed;
  celula[i].calor.cpint = cpl * (1 - alfmed) + cpg * alfmed;
  celula[i].calor.rhoint = rhol * (1 - alfmed) + rhog * alfmed;
  double viscliq = celula[i].flui.ViscLiq(celula[i].pres, celula[i].temp);
  celula[i].calor.viscint = viscliq * (1 - alfmed) * 1.e-3
      + celula[i].flui.ViscGas(celula[i].pres, celula[i].temp) * alfmed * 1.e-3;
  double fluxcal = celula[i].calor.transtrans();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  double coefTempo = (rhol * (1 - alfmed) * cvl + rhog * alfmed * cvg) * area;
  double coefPresTempo;
  coefPresTempo = -celula[i].flui.CPgFuncMod(celula[i].pres, celula[i].temp) * (rhog * alfmed * area);
  double coefdxT = (rhol * ulsmed * cpl + rhog * ugsmed * cpg) * area;
  double coefdxP = (rhol * ulsmed * jtl + rhog * ugsmed * jtg) * area;
  double dpdx;
  if (i < ncel) dpdx = 2. * (celula[i + 1].presaux - celula[i].pres) * 98066.5 / celula[i].dx;
  else dpdx = 2. * (celula[i].pres - celula[i].presaux) * 98066.5 / celula[i].dx;
  if (celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 +arq.master1.razareaativ) * celula[i].duto.area) dpdx = 2.
      * (celula[i].pres - celula[i].presaux) * 98066.5 / celula[i].dx;
  //celula[i].VTemper=coefdxT*area/coefTempo;//corrigir a multiplicação pela área
  celula[i].VTemper = coefdxT / coefTempo;
  //double dtdx=(celula[i].tr-celula[i-1].tr)/dxmed;
  double dtdx = (celula[i].temp - tempantiga) / dxmed;

  if (i < ncel) if (celula[i].VTemper < 0) dtdx = (celula[i + 1].temp - celula[i].temp)
      / (0.5 * (celula[i + 1].dx + celula[i].dx));
  if (celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i].duto.area
      && celula[i].VTemper <= 0) dtdx = 0.;
  if (celula[i - 1].acsr.tipo == 5 && celula[i - 1].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i - 1].duto.area
      && celula[i].VTemper >= 0) dtdx = 0.;
  if ((i == 1 && celula[i].VTemper <= 0) || (i == ncel && celula[i].VTemper <= 0)) dtdx = 0.;
  if (celula[i].acsr.tipo == 4 && celula[i].acsr.bcs.freqnova > 1.) {
    //dtdx=0.;
    dpdx = 0.;
  }
  if (celula[i].acsr.tipo == 8 && celula[i].acsr.bvol.freq > 1.) {
    dpdx = (celula[i].pres - celula[i - 1].pres) * 98066.5 / dxmed;
    //dtdx=0.;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  double cinetico=0;
  double ugmed0=0;
  double ulmed0=0;
  double ugmed=0;
  double ulmed;
  if (celula[i].acsr.tipo == 0 && celula[i - 1].acsr.tipo == 0 && i>1) {

	double dxCin=celula[i-1].dx;
    double diaaux = celula[i - 1].duto.a;
    double areaaux = 0.25 * M_PI * diaaux * diaaux;

    double alf;
    if(celula[i].QL>0)alf=celula[i-1].alf;
    else alf=celula[i].alf;
    double alf0;
    if(celula[i-1].QL>0)alf0=celula[i-2].alf;
    else alf0=celula[i-1].alf;


    if (alf0 > 1e-3){
    	ugmed0 = celula[i-1].QG / (areaaux);
    	ugmed0/=alf0;
    }


    if (alf0 < 1. - 1e-3){
    	ulmed0 = celula[i-1].QL / (areaaux);
    	ulmed0/=(1.-alf0);
    }

    if (alf > 1e-3){
    	ugmed = ugsmed;
    	ugmed/=alf;
    }


    if (alf< 1. - 1e-3){
    	ulmed = ulsmed ;
    	ulmed/=(1.-alf);
    }

    cinetico = (celula[i].MC-celula[i-1].Mliqini)*ugmed*(ugmed-ugmed0)/dxCin+celula[i-1].Mliqini*ulmed*(ulsmed-ulmed0)/dxCin;

  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

  double fontemassG = 0.;
  double fontemassL = 0.;
  double tfonte = celula[i].temp;
  double cpgF;
  double razcpF;
  double cplF;
  if(celula[i].acsr.tipo==11){
	 tfonte = celula[i].acsr.iprvap.Tres;
	 cpgF = cpg;
	 razcpF = cpg/cvg;
	 cplF = cpl;

  }
  else if(celula[i].acsr.tipo==12){
  	tfonte=celula[i].acsr.injmvap.temp;
    cpgF=cpg;// alteracao7
    razcpF=cpg/cvg;
    cplF=cpl;
  }
  else if (celula[i].acsrL != 0) {
    if ((*celula[i].acsrL).tipo == 5 && (celula[i].fontemassLR )>0.) {    //alteracao2
      if ((*celula[i].acsrL).chk.AreaGarg < arq.master1.razareaativ * celula[i].dutoL.area
          && (*celula[i].acsrL).chk.AreaGarg > 1e-5 * celula[i].dutoL.area) {//alteracao8
        double tE = celula[i - 1].temp;
        double alfE = celula[i - 1].alf;

        double rholp = celula[i - 1].flui.MasEspLiq(celula[i - 1].pres, celula[i - 1].temp,TEMP);
        double rholmix = rholp;

        double alfJ = celula[i].alf;
        double rholpJ = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp,TEMP);
        double rholmixJ =rholpJ;

        double hidroM = sinl(celula[i - 1].duto.teta) * (0.5 * celula[i - 1].dx)
            * (rholmix * (1 - alfE) +
            		alfE * celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP))
            * 9.82 / 98600.;
        double hidroJ = sinl(celula[i].duto.teta) * (0.5 * celula[i].dx)
            * (rholmixJ * (1 - alfJ) + alfJ * celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp,TEMP)) *
			 9.82 / 98600.;

        double tit = alfE * celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP)
            / (celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP) * alfE +
            		rholmix * (1. - alfE));
        double jtlM = celula[i - 1].flui.JTlFunc(celula[i - 1].pres - hidroM, celula[i - 1].temp);
        double jtgM = celula[i - 1].flui.JTgFunc(celula[i - 1].pres - hidroM, celula[i - 1].temp);
        tfonte = tE + ((1. - tit) * jtlM + tit * jtgM) * (celula[i].pres + hidroJ - celula[i - 1].pres - hidroM);

        cpgF = celula[i - 1].flui.CPgFunc(celula[i - 1].pres, celula[i - 1].temp);
        cplF = celula[i - 1].flui.CPlFunc(celula[i - 1].pres, celula[i - 1].temp);
        razcpF = celula[i - 1].flui.KAgFunc(celula[i - 1].pres, celula[i - 1].temp);

      }    //alteracao2
      else {//alteracao8
        cpgF = celula[i - 1].flui.CPgFunc(celula[i - 1].pres, celula[i - 1].temp);
        cplF = celula[i - 1].flui.CPlFunc(celula[i - 1].pres, celula[i - 1].temp);
        tfonte = celula[i].temp;
        razcpF = celula[i - 1].flui.KAgFunc(celula[i - 1].pres, celula[i - 1].temp);
      }
    }  else if ((*celula[i].acsrL).tipo == 8) {
        double alfM = celula[i - 1].alf;
        cpgF = celula[i - 1].flui.CPgFunc(celula[i - 1].pres, celula[i - 1].temp);
        cplF = celula[i - 1].flui.CPlFunc(celula[i - 1].pres, celula[i - 1].temp);
        double n = (*celula[i].acsrL).bvol.npoli;
        double ypres = (celula[i].pres) / (celula[i - 1].pres);
        tfonte = celula[i - 1].temp * pow(ypres, (n - 1) / n);
      }else {
      cpgF = 0.;
      razcpF = 1.;
      cplF = 0.;
    }
  } else {
    cpgF = 0.;
    razcpF = 1.;
    cplF = 0.;
  }

  fontemassL = 0;
  if (celula[i].fontemassLR > 0.) fontemassL = celula[i].fontemassLR / dx;
  fontemassL *= (cplF) * (tfonte - celula[i].temp);

  fontemassG = celula[i].fontemassGR / dx;
  if (fontemassG > 0.) fontemassG *= (cpgF / razcpF) * (tfonte - celula[i].temp);
  else fontemassG = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  double latente;
  latente = (celula[i].flui.HeGas(celula[i].pres, celula[i].temp,TEMP)-
		  celula[i].flui.HeLiq(celula[i].pres, celula[i].temp,TEMP)) * celula[i].FonteMudaFase;

  double alfinter;
  double alfinterL;
  if (ugsmed > 0) {
    alfinter = celula[i].alf;
    alfinterL = celula[i].alfL;
  } else {
    alfinter = celula[i].alfR;
    alfinterL = celula[i].alf;
  }
  double delvel;
  if (alfinter > (*vg1dSP).localtiny && alfinter < (1. - (*vg1dSP).localtiny)) delvel = ugsmed / alfinter - ulsmed / (1. - alfinter);
  else if (alfinter > (*vg1dSP).localtiny) delvel = ugsmed;
  else delvel = ulsmed;
  double verifica = area * celula[i].pres * 98066.5* delvel * (alfinter - alfinterL) / celula[i].dx;

  if (celula[i].temp < 0 || celula[i].temp > 100) {
    int para;
    para = 0.;
  }

  celula[i].temp = ((coefTempo / celula[i].dt) * celula[i].temp
      - (coefPresTempo * (celula[i].pres - celula[i].presini) * 98066.5 / celula[i].dt) - coefdxT * dtdx + coefdxP * dpdx
      - cinetico - (hidro - 0. * verifica) + celula[i - 1].potB / dxmed + fontemassL + fontemassG + fluxcal -
	  latente) /(coefTempo / celula[i].dt);
  if (celula[i].temp < -50.) celula[i].temp = -50.;
  if (celula[i].temp > 200.) celula[i].temp = 200.;

}

 double SProdVap::calcHmix(int i,double tempantiga){

	  double dx = celula[i].dx;
	  double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
	  double dia = celula[i].duto.a;
	  double area = 0.25 * M_PI * dia * dia;
	  double alfmed = celula[i].alf;
	  double alfmed0 = celula[i].alfini;
	  double pmed=celula[i].pres;
	  double pmed0=celula[i].presini;
	  double tmed=celula[i].temp;
	  double ugsL;
	  double ulsL;
	  double ugsR;
	  double ulsR;

      ugsL = celula[i].QG / area;
      ulsL = celula[i].QL / area;
      ugsR = celula[i+1].QG / area;
      ulsR = celula[i+1].QL / area;


      double ugsmed=0.5*(ugsL+ugsR);
      double ulsmed=0.5*(ulsL+ulsR);

      double tmedL = tempantiga;
      if (celula[i].VTemper < 0.) tmedL = celula[i].temp;
      double tmedR = celula[i].temp;
      if (celula[i + 1].VTemper < 0.) tmedR = celula[i].tempR;

      double rhog=celula[i].flui.MasEspGas(pmed0,tmed,TEMP);
      double rhop=celula[i].flui.MasEspLiq(pmed0,tmed,TEMP);

      celula[i].calor.Tint = celula[i].temp;
      celula[i].calor.Vint = ugsmed + ulsmed;
      celula[i].calor.dt = celula[i].dt;
      double cpl = celula[i].flui.CPlFunc(celula[i].pres, celula[i].temp);
      double cpg = celula[i].flui.CPgFunc(celula[i].pres, celula[i].temp);
      double condliq =  celula[i].flui.CondLiq(celula[i].pres, celula[i].temp);
      celula[i].calor.kint = condliq * (1 - alfmed) +
     		  celula[i].flui.CondGas(celula[i].pres, celula[i].temp) * alfmed;
      celula[i].calor.cpint = cpl * (1 - alfmed) + cpg * alfmed;
      celula[i].calor.rhoint = rhop * (1 - alfmed) + rhog * alfmed;
      double viscliq = celula[i].flui.ViscLiq(celula[i].pres, celula[i].temp);
      celula[i].calor.viscint = viscliq * (1 - alfmed) * 1.e-3
           + celula[i].flui.ViscGas(celula[i].pres, celula[i].temp) * alfmed * 1.e-3;
      double fluxcal = celula[i].calor.transtrans()/area;



      double presL= celula[i].presaux;
      double presR= celula[i+1].presaux;
      if((celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 +arq.master1.razareaativ) * celula[i].duto.area) ||
    	  (celula[i].acsr.tipo == 4 && celula[i].acsr.bcs.freq>0) ||
		  (celula[i].acsr.tipo == 8 && celula[i].acsr.bvol.freq>0.) ||
		  (celula[i].acsr.tipo == 7 && fabsl(celula[i].acsr.delp)>0.) ){
    	  presR=celula[i].pres+(celula[i].pres-celula[i].presaux)*0.5;
      }



      double rhog1=celula[i].flui.MasEspGas(pmed,tmed,TEMP);
      double rhop1=celula[i].flui.MasEspLiq(pmed,tmed,TEMP);
      //double hg=celula[i].flui.HeGas(pmed0,tmed,TEMP);
      //double hp=celula[i].flui.HeLiq(pmed0,tmed,TEMP);
      double ug=celula[i].flui.UiGas(pmed0,tmed,TEMP);
      double up=celula[i].flui.UiLiq(pmed0,tmed,TEMP);

      double rhogL=celula[i].flui.MasEspGas(presL,tmedL,TEMP);
      double rhopL=celula[i].flui.MasEspLiq(presL,tmedL,TEMP);
      double hgL=celula[i].flui.HeGas(presL,tmedL,TEMP);
      double hpL=celula[i].flui.HeLiq(presL,tmedL,TEMP);

      double rhogR=celula[i].flui.MasEspGas(presR,tmedR,TEMP);
      double rhopR=celula[i].flui.MasEspLiq(presR,tmedR,TEMP);
      double hgR=celula[i].flui.HeGas(presR,tmedR,TEMP);
      double hpR=celula[i].flui.HeLiq(presR,tmedR,TEMP);

      //double energintmixT0=rhog*alfmed0*(hg-pmed0*98066.5/rhog)+rhop*(1.-alfmed0)*(hp-pmed0*98066.5/rhop);
      double energintmixT0=rhog*alfmed0*(ug)+rhop*(1.-alfmed0)*(up);

      double fluxHL=rhogL*ugsL*hgL+rhopL*ulsL*hpL;
      double fluxHR=rhogR*ugsR*hgR+rhopR*ulsR*hpR;
      double delFlux=(fluxHR-fluxHL)/dx;
      double hidro=(rhog1*ugsmed+ulsmed*rhop1)*sinl(celula[i].duto.teta)*9.82;


      double fontemassG = 0.;
      double fontemassL = 0.;
      double fontemassC = 0.;
      double tfonte = celula[i].temp;
      double hgF;
      double hlF;
      double hcF;

      if (celula[i].acsr.tipo == 12) {
        tfonte = celula[i].acsr.injmvap.temp;
        hgF = celula[i].flui.HeGas(celula[i].pres, tfonte,TEMP);
        hlF = celula[i].flui.HeLiq(celula[i].pres, tfonte,TEMP);
      } else if (celula[i].acsr.tipo == 11) {
        tfonte = celula[i].acsr.iprvap.Tres;
        hgF = celula[i].flui.HeGas(celula[i].pres, tfonte,TEMP);
        hlF = celula[i].flui.HeLiq(celula[i].pres, tfonte,TEMP);
        hcF=0;
      } else if (celula[i].acsrL != 0) {
        if ((*celula[i].acsrL).tipo == 5) {    //alteracao2
          if ((*celula[i].acsrL).chk.AreaGarg < arq.master1.razareaativ * celula[i].dutoL.area
              && (*celula[i].acsrL).chk.AreaGarg > 1e-5 * celula[i].dutoL.area) {
            double tE = celula[i - 1].temp;
            double alfE = celula[i - 1].alf;

            double rholp = celula[i - 1].flui.MasEspLiq(celula[i - 1].pres, celula[i - 1].temp,TEMP);
            double rholmix = rholp;

            double alfJ = celula[i].alf;
            double rholpJ = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp,TEMP);
            double rholmixJ = rholpJ;

            double hidroM = sinl(celula[i - 1].duto.teta) * (0.5 * celula[i - 1].dx)
                * (rholmix * (1 - alfE) + alfE * celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP))
                * 9.82 / 98600.;
            double hidroJ = sinl(celula[i].duto.teta) * (0.5 * celula[i].dx)
                * (rholmixJ * (1 - alfJ) + alfJ * celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp,TEMP)) * 9.82 / 98600.;

            double tit = alfE * celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP)
                / (celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP) * alfE + rholmix * (1. - alfE));

            double jtlM = celula[i - 1].flui.JTlFunc(celula[i - 1].pres - hidroM, celula[i - 1].temp);
            double jtgM = celula[i - 1].flui.JTgFunc(celula[i - 1].pres - hidroM, celula[i - 1].temp);
            tfonte = tE + ((1. - tit) * jtlM + tit * jtgM) * (celula[i].pres + hidroJ - celula[i - 1].pres - hidroM);

            hgF = celula[i - 1].flui.HeGas(celula[i - 1].pres, tfonte,TEMP);
            hlF = celula[i - 1].flui.HeLiq(celula[i - 1].pres, tfonte,TEMP);
          }    //alteracao2
          else {
        	tfonte = celula[i].temp;
            hgF = celula[i - 1].flui.HeGas(celula[i - 1].pres, celula[i - 1].temp,TEMP);
            hlF = celula[i - 1].flui.HeLiq(celula[i - 1].pres, celula[i - 1].temp,TEMP);
          }
        } else if ((*celula[i].acsrL).tipo == 8) {
          double alfM = celula[i - 1].alf;

          double n = (*celula[i].acsrL).bvol.npoli;
          double ypres = (celula[i].pres) / (celula[i - 1].pres);
          tfonte = celula[i - 1].temp * pow(ypres, (n - 1) / n);

          hgF = celula[i - 1].flui.HeGas(celula[i - 1].pres, tfonte,TEMP);
          hlF =  celula[i - 1].flui.HeLiq(celula[i - 1].pres, tfonte,TEMP);

        } else {
          hgF = 0.;
          hlF = 0.;
        }
      } else {
        hgF = 0.;
        hlF = 0.;
      }

      fontemassL = 0;
      fontemassG = 0;
      if (celula[i].fontemassLR > 0.) fontemassL = hlF*celula[i].fontemassLR / dx;
      if (fontemassG > 0.)fontemassG = hgF*celula[i].fontemassGR / dx;


    celula[i].uinter=(energintmixT0-(delFlux+hidro-fluxcal-(fontemassG+fontemassL)/area)*celula[i].dt);
	return energintmixT0-(delFlux+hidro-fluxcal-(fontemassG+fontemassL)/area)*celula[i].dt;
}

double SProdVap::SIGN(double a, double b) {
  return (b >= 0 ? 1.0 : -1.0) * fabsl(a);
}

double SProdVap::calcTempEntalp(int i,double tempantiga){
	double pres=celula[i].pres;
	double alf=celula[i].alf;
	double t0=celula[i].temp;
	double t1=t0;
	double energint=calcHmix(i,tempantiga);
	double energchute=celula[i].flui.energmix(pres,t0,alf);
	double difer=energint-energchute;
	double tneg;
	double tpos;
	if(alf<(*vg1dSP).localtiny || (1.-alf)<(*vg1dSP).localtiny){
		//celula[i].temp=celula[i].flui.TempFun(pres, energint, UINT);
  	  if(difer>0.){
		tpos=t0;
		while(difer>0.){
			t1=0.9*t1;
			energchute=celula[i].flui.energmix(pres,t1,alf);
			difer=energint-energchute;
		}
		tneg=t1;
	  }
	  else{
		tneg=t0;
		while(difer<0){
			t1=1.1*t1;
			energchute=celula[i].flui.energmix(pres,t1,alf);
			difer=energint-energchute;
		}
		tpos=t1;
	  }

	  double xacc=1e-5;
	  int maxit=100;
	  double x1=tneg;
	  double x2=tpos;
	  double fl=energint-celula[i].flui.energmix(pres,x1,alf);
	  double fh=energint-celula[i].flui.energmix(pres,x2,alf);
	  if(fl>=0.){
		x1=x1-1*(x2-x1);
		fl=energint-celula[i].flui.energmix(pres,x1,alf);
	  }
	  else if(fh<=0.){
		x2=x2-1*(x1-x2);
		fh=energint-celula[i].flui.energmix(pres,x2,alf);
	  }
	  if((fl>0.0 && fh<0.0) || (fl<0.0 && fh>0.0)){
		double xl=x1;
		double xh=x2;
		double ans=-1.e20;
		for(int j=0;j<maxit;j++){
			double xm=0.5*(xl+xh);
			double fm=energint-celula[i].flui.energmix(pres,xm,alf);
			double s=sqrtl(fm*fm-fl*fh);
			if(s==0.0) celula[i].temp=ans;
			double xnew=xm+(xm-xl)*((fl>=fh ? 1.0 : -1.0)*fm/s);
			if(fabsl(xnew-ans)<=xacc ){
				celula[i].temp=ans;
			}
			ans=xnew;
			double fnew=energint-celula[i].flui.energmix(pres,ans,alf);
			if(fabsl(fnew)<=xacc ) celula[i].temp=ans;
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
			else celula[i].temp=-100000.;
			if(fabsl(xh-xl)<=xacc ) celula[i].temp=ans;

		}
		celula[i].temp=100000.;
	  }
	  else {
		if(fabsl(fl)<=xacc) return x1;
		if(fabsl(fh)<=xacc) return x2;
		celula[i].temp= -10000.;
	  }

      if (celula[i].temp < -50.) celula[i].temp = -50.;
      if (celula[i].temp > 200.) celula[i].temp = 200.;
	}
	else{
		celula[i].temp=celula[i].flui.TempFun(pres,t0);
	}

}

void SProdVap::calcTransMassTermo(int i) {

	  double dx = celula[i].dx;
	  double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
	  double dia = celula[i].duto.a;
	  double area = 0.25 * M_PI * dia * dia;
	  double alfmed = celula[i].alf;
	  double ugsmed;

	  if (celula[i].alf > (*vg1dSP).localtiny) ugsmed = celula[i].QG / area;
	  else {
	    ugsmed = 0.;
	  }
	  double ulsmed;
	  if (celula[i].alf < 1. - (*vg1dSP).localtiny) ulsmed = celula[i].QL / area;
	  else {
	    ulsmed = 0.;
	  }
	  double rp = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp,TEMP);
	  double rhol = rp;
	  double rhog = celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp,TEMP);
	  double cpl = celula[i].flui.CPlFunc(celula[i].pres, celula[i].temp);
	  double cvl = cpl;
	  double cpg = celula[i].flui.CPgFunc(celula[i].pres, celula[i].temp);
	  double cvg = celula[i].flui.CVgFunc(celula[i].pres, celula[i].temp);
	  double jtl = celula[i].flui.JTlFunc(celula[i].pres, celula[i].temp);
	  double jtg = celula[i].flui.JTgFunc(celula[i].pres, celula[i].temp);
	  double hidro = (rhol * ulsmed + rhog * ugsmed) * area * 9.82 * sinl(celula[i].duto.teta);

	  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  celula[i].calor.Tint = celula[i].temp;
	  celula[i].calor.Vint = ugsmed + ulsmed;
	  celula[i].calor.dt = celula[i].dt;
	  double condliq = celula[i].flui.CondLiq(celula[i].pres, celula[i].temp);
	  celula[i].calor.kint = condliq * (1 - alfmed) +
			                 celula[i].flui.CondGas(celula[i].pres, celula[i].temp) * alfmed;
	  celula[i].calor.cpint = cpl * (1 - alfmed) + cpg * alfmed;
	  celula[i].calor.rhoint = rhol * (1 - alfmed) + rhog * alfmed;
	  double viscliq = celula[i].flui.ViscLiq(celula[i].pres, celula[i].temp);
	  celula[i].calor.viscint = viscliq * (1 - alfmed) * 1.e-3
	      + celula[i].flui.ViscGas(celula[i].pres, celula[i].temp) * alfmed * 1.e-3;
	  double fluxcal = celula[i].calor.transtrans();

	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  double coefTempo = (rhol * (1 - alfmed) * cvl + rhog * alfmed * cvg) * area;
	  double coefPresTempo;
	  coefPresTempo = -celula[i].flui.CPgFuncMod(celula[i].pres, celula[i].temp) * (rhog * alfmed * area);
	  double coefdxT = (rhol * ulsmed * cpl + rhog * ugsmed * cpg) * area;
	  double coefdxP = (rhol * ulsmed * jtl + rhog * ugsmed * jtg) * area;
	  double dpdx;
	  if (i < ncel) dpdx = 2. * (celula[i + 1].presaux - celula[i].pres) * 98066.5 / celula[i].dx;
	  else dpdx = 2. * (celula[i].pres - celula[i].presaux) * 98066.5 / celula[i].dx;
	  if (celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 +arq.master1.razareaativ) * celula[i].duto.area) dpdx = 2.
	      * (celula[i].pres - celula[i].presaux) * 98066.5 / celula[i].dx;
	  celula[i].VTemper = coefdxT / coefTempo;
	  double dtdx = (celula[i].temp - celula[i-1].temp) / dxmed;
	  if (i < ncel) if (celula[i].VTemper < 0) dtdx = (celula[i + 1].temp - celula[i].temp)
	      / (0.5 * (celula[i + 1].dx + celula[i].dx));
	  if (celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i].duto.area
	      && celula[i].VTemper <= 0) dtdx = 0.;
	  if (celula[i - 1].acsr.tipo == 5 && celula[i - 1].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i - 1].duto.area
	      && celula[i].VTemper >= 0) dtdx = 0.;
	  if ((i == 1 && celula[i].VTemper <= 0) || (i == ncel && celula[i].VTemper <= 0)) dtdx = 0.;
	  if (celula[i].acsr.tipo == 4 && celula[i].acsr.bcs.freqnova > 1.) {
	    dpdx = 0.;
	  }
	  if (celula[i].acsr.tipo == 8 && celula[i].acsr.bvol.freq > 1.) {
	    dpdx = (celula[i].pres - celula[i - 1].pres) * 98066.5 / dxmed;
	  }

	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  double cinetico=0;
	  double ugmed0=0;
	  double ulmed0=0;
	  double ugmed=0;
	  double ulmed;
	  if (celula[i].acsr.tipo == 0 && celula[i - 1].acsr.tipo == 0 && i>1) {

		double dxCin=celula[i-1].dx;
	    double diaaux = celula[i - 1].duto.a;
	    double areaaux = 0.25 * M_PI * diaaux * diaaux;

	    double alf;
	    if(celula[i].QL>0)alf=celula[i-1].alf;
	    else alf=celula[i].alf;
	    double alf0;
	    if(celula[i-1].QL>0)alf0=celula[i-2].alf;
	    else alf0=celula[i-1].alf;


	    if (alf0 > 1e-3){
	    	ugmed0 = celula[i-1].QG / (areaaux);
	    	ugmed0/=alf0;
	    }


	    if (alf0 < 1. - 1e-3){
	    	ulmed0 = celula[i-1].QL / (areaaux);
	    	ulmed0/=(1.-alf0);
	    }

	    if (alf > 1e-3){
	    	ugmed = ugsmed;
	    	ugmed/=alf;
	    }


	    if (alf< 1. - 1e-3){
	    	ulmed = ulsmed ;
	    	ulmed/=(1.-alf);
	    }

	    cinetico = (celula[i].MC-celula[i-1].Mliqini)*ugmed*(ugmed-ugmed0)/dxCin+celula[i-1].Mliqini*ulmed*(ulsmed-ulmed0)/dxCin;

	  }


	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  double fontemassG = 0.;
	    double fontemassL = 0.;
	    double tfonte = celula[i].temp;
	    double cpgF;
	    double razcpF;
	    double cplF;
	    if(celula[i].acsr.tipo==11){
	  	 tfonte = celula[i].acsr.iprvap.Tres;
	  	 cpgF = cpg;
	  	 razcpF = cpg/cvg;
	  	 cplF = cpl;

	    }
	    else if(celula[i].acsr.tipo==12){
	    	tfonte=celula[i].acsr.injmvap.temp;
	      cpgF=cpg;// alteracao7
	      razcpF=cpg/cvg;
	      cplF=cpl;
	    }
	    else if (celula[i].acsrL != 0) {
	      if ((*celula[i].acsrL).tipo == 5 && (celula[i].fontemassLR )>0.) {    //alteracao2
	        if ((*celula[i].acsrL).chk.AreaGarg < arq.master1.razareaativ * celula[i].dutoL.area
	            && (*celula[i].acsrL).chk.AreaGarg > 1e-5 * celula[i].dutoL.area) {//alteracao8
	          double tE = celula[i - 1].temp;
	          double alfE = celula[i - 1].alf;

	          double rholp = celula[i - 1].flui.MasEspLiq(celula[i - 1].pres, celula[i - 1].temp,TEMP);
	          double rholmix = rholp;

	          double alfJ = celula[i].alf;
	          double rholpJ = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp,TEMP);
	          double rholmixJ =rholpJ;

	          double hidroM = sinl(celula[i - 1].duto.teta) * (0.5 * celula[i - 1].dx)
	              * (rholmix * (1 - alfE) +
	              		alfE * celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP))
	              * 9.82 / 98600.;
	          double hidroJ = sinl(celula[i].duto.teta) * (0.5 * celula[i].dx)
	              * (rholmixJ * (1 - alfJ) + alfJ * celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp,TEMP)) *
	  			 9.82 / 98600.;

	          double tit = alfE * celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP)
	              / (celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp,TEMP) * alfE +
	              		rholmix * (1. - alfE));
	          double jtlM = celula[i - 1].flui.JTlFunc(celula[i - 1].pres - hidroM, celula[i - 1].temp);
	          double jtgM = celula[i - 1].flui.JTgFunc(celula[i - 1].pres - hidroM, celula[i - 1].temp);
	          tfonte = tE + ((1. - tit) * jtlM + tit * jtgM) * (celula[i].pres + hidroJ - celula[i - 1].pres - hidroM);

	          cpgF = celula[i - 1].flui.CPgFunc(celula[i - 1].pres, celula[i - 1].temp);
	          cplF = celula[i - 1].flui.CPlFunc(celula[i - 1].pres, celula[i - 1].temp);
	          razcpF = celula[i - 1].flui.KAgFunc(celula[i - 1].pres, celula[i - 1].temp);

	        }    //alteracao2
	        else {//alteracao8
	          cpgF = celula[i - 1].flui.CPgFunc(celula[i - 1].pres, celula[i - 1].temp);
	          cplF = celula[i - 1].flui.CPlFunc(celula[i - 1].pres, celula[i - 1].temp);
	          tfonte = celula[i].temp;
	          razcpF = celula[i - 1].flui.KAgFunc(celula[i - 1].pres, celula[i - 1].temp);
	        }
	      }else if ((*celula[i].acsrL).tipo == 8) {
	          double alfM = celula[i - 1].alf;
	          cpgF = celula[i - 1].flui.CPgFunc(celula[i - 1].pres, celula[i - 1].temp);
	          cplF = celula[i - 1].flui.CPlFunc(celula[i - 1].pres, celula[i - 1].temp);
	          double n = (*celula[i].acsrL).bvol.npoli;
	          double ypres = (celula[i].pres) / (celula[i - 1].pres);
	          tfonte = celula[i - 1].temp * pow(ypres, (n - 1) / n);
	        }else {
	        cpgF = 0.;
	        razcpF = 1.;
	        cplF = 0.;
	      }
	    } else {
	      cpgF = 0.;
	      razcpF = 1.;
	      cplF = 0.;
	    }

	    fontemassL = 0;
	    if (celula[i].fontemassLR > 0.) fontemassL = celula[i].fontemassLR / dx;
	    fontemassL *= (cplF) * (tfonte - celula[i].temp);

	    fontemassG = celula[i].fontemassGR / dx;
	    if (fontemassG > 0.) fontemassG *= (cpgF / razcpF) * (tfonte - celula[i].temp);
	    else fontemassG = 0;

	    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

	    double latente;
	    latente = (celula[i].flui.HeGas(celula[i].pres, celula[i].temp,TEMP)-
	  		  celula[i].flui.HeLiq(celula[i].pres, celula[i].temp,TEMP)) * celula[i].FonteMudaFase;

	    double alfinter;
	    double alfinterL;
	    if (ugsmed > 0) {
	      alfinter = celula[i].alf;
	      alfinterL = celula[i].alfL;
	    } else {
	      alfinter = celula[i].alfR;
	      alfinterL = celula[i].alf;
	    }
	    double delvel;
	    if (alfinter > (*vg1dSP).localtiny && alfinter < (1. - (*vg1dSP).localtiny)) delvel = ugsmed / alfinter - ulsmed / (1. - alfinter);
	    else if (alfinter > (*vg1dSP).localtiny) delvel = ugsmed;
	    else delvel = ulsmed;
	    double verifica = area * celula[i].pres * 98066.5* delvel * (alfinter - alfinterL) / celula[i].dx;

	    if (celula[i].temp < 0 || celula[i].temp > 100) {
	      int para;
	      para = 0.;
	    }

	  celula[i].FonteMudaFase = (-(coefTempo / celula[i].dt) *( celula[i].temp-celula[i].tempini)
	      - (coefPresTempo * (celula[i].pres - celula[i].presini) * 98066.5 / celula[i].dt) - coefdxT * dtdx + coefdxP * dpdx
	      - cinetico - (hidro - 0. * verifica) + celula[i - 1].potB / dxmed + fontemassL + fontemassG + fluxcal);// / (coefTempo / celula[i].dt);

	  celula[i].FonteMudaFase /=latente;

	}

void SProdVap::FonteValv(int ind) {
  if (celula[ind].acsr.chk.AreaGarg < (1e-3 + arq.master1.razareaativ) * celula[ind].duto.area
      && celula[ind].acsr.chk.AreaGarg > 1e-3 * celula[ind].duto.area /*&&
       celula[ind].pres>celula[ind+1].pres*/) {
    double tE = celula[ind].temp;
    double alfE = celula[ind].alf;
    double sense=1.;

    double razao = celula[ind].acsr.chk.AreaGarg / celula[ind].duto.area;

    double maxSup = 0.;

    double rholp = celula[ind].flui.MasEspLiq(celula[ind].pres, celula[ind].temp,TEMP);
    double rholmix = rholp;

    double alfJ = celula[ind + 1].alf;
    double rholpJ = celula[ind + 1].flui.MasEspLiq(celula[ind + 1].pres, celula[ind + 1].temp,TEMP);
    double rholmixJ = rholpJ;

    double hidroM = sinl(celula[ind].duto.teta) * (0.5 * celula[ind].dx)
        * (rholmix * (1 - alfE) + alfE * celula[ind].flui.MasEspGas(celula[ind].pres, celula[ind].temp,TEMP)) * 9.82
        / 98600.;
    double hidroJ = sinl(celula[ind + 1].duto.teta) * (0.5 * celula[ind + 1].dx)
        * (rholmixJ * (1 - alfJ) +
        		alfJ * celula[ind + 1].flui.MasEspGas(celula[ind + 1].pres, celula[ind + 1].temp,TEMP))
        * 9.82 / 98600.;

    double tit = alfE * celula[ind].flui.MasEspGas(celula[ind].pres, celula[ind].temp,TEMP)
        / (celula[ind].flui.MasEspGas(celula[ind].pres, celula[ind].temp,TEMP) * alfE + rholmix * (1. - alfE));
    double masChk;
    //masChk=celula[ind].acsr.chk.vazmass(celula[ind].pres-hidroM, celula[ind+1].pres+hidroJ, tE, alfE,betE,tit, celula[ind].flui,celula[ind].fluicol);
    //maxSup=celula[ind].acsr.chk.maxmass(celula[ind].pres-hidroM, celula[ind+1].pres+hidroJ, tE, alfE,betE,tit, celula[ind].flui,celula[ind].fluicol);

    double ypres = (celula[ind + 1].pres + hidroJ) / (celula[ind].pres - hidroM);
    int check=1;//alteracao8
    if(ypres<1. || check==1){//alteracao8
        if((*vg1dSP).lixo5>7059){
        	int para;
        	para=1;
        }
      masChk = celula[ind].acsr.chk.vazmassSachdVap(ypres, celula[ind].pres - hidroM, tE, alfE, tit);
      maxSup = celula[ind].acsr.chk.vazmaxSachdVap(celula[ind].pres - hidroM, tE, alfE, tit);
    }
    else{//alteracao8
        tE = celula[ind+1].temp;
        alfE = celula[ind+1].alf;
        ypres=1./ypres;
        sense=-1;
        if((*vg1dSP).lixo5>7059){
        	int para;
        	para=1;
        }
        masChk = celula[ind].acsr.chk.vazmassSachdVap(ypres, celula[ind + 1].pres + hidroJ, tE, alfE, tit);
        maxSup = celula[ind].acsr.chk.vazmaxSachdVap(celula[ind + 1].pres + hidroJ, tE, alfE, tit);
    }

    //if(fabsl(masChk)<fabsl(maxSup))maxSup=masChk;
    if (fabsl(ypres) > fabsl(celula[ind].acsr.chk.razpres)) maxSup = masChk;

    if (celula[ind].acsr.chk.AreaGarg < (1e-3) * celula[ind].duto.area || (celula[ind].pres < celula[ind + 1].pres && check==1)) maxSup =
        0.;//alteracao8

    double masliq;
    double masgas;
    masliq = sense*maxSup * (1. - tit);
    masgas = sense*maxSup * tit;

    celula[ind].fontemassLR += (-1 * masliq * rholp / rholmix);
    celula[ind].fontemassGR += (-1 * masgas);

    celula[ind + 1].fontemassLR += (masliq * rholp / rholmix);
    celula[ind + 1].fontemassGR += (masgas);
  }
}

void SProdVap::renovaFonte(int ind) {

  double pr = celula[ind].pres;
  double tr = celula[ind].temp;
  if (ind == 0) {
    celula[ind].fontemassLR = 0.;
    celula[ind].fontemassGR = 0.;
  } else {
    if (celula[ind - 1].acsr.tipo != 5 && celula[ind - 1].acsr.tipo != 8) {
      celula[ind].fontemassLR = 0.;
      celula[ind].fontemassGR = 0.;
    }
  }
  if (celula[ind].acsr.tipo == 11) {
//    int teste = 0;
    if (pr < celula[ind].acsr.ipr.Pres) {
      celula[ind].fontemassLR += celula[ind].acsr.ipr.MasL(pr, tr);
      celula[ind].fontemassGR += celula[ind].acsr.ipr.MasG(pr, tr);
    } else {
      celula[ind].fontemassLR += (1. - celula[ind].alf) * celula[ind].acsr.iprvap.VMas(pr, tr)
          * celula[ind].flui.MasEspLiq(pr, tr,TEMP);
      celula[ind].fontemassGR += 1 * celula[ind].alf * celula[ind].acsr.iprvap.VMas(pr, tr)
          * celula[ind].flui.MasEspGas(pr, tr,TEMP);
      celula[ind].acsr.iprvap.deriP *= (1. - celula[ind].alf)* celula[ind].flui.MasEspLiq(pr, tr,TEMP);
      celula[ind].acsr.iprvap.deriG *= 1 * celula[ind].alf * celula[ind].flui.MasEspGas(pr, tr,TEMP);

    }
  }
  if (celula[ind].acsr.tipo == 5) {
    celula[ind + 1].fontemassLR = 0.;
    celula[ind + 1].fontemassGR = 0.;
    FonteValv(ind);
  }
  if (celula[ind].acsr.tipo == 8) {
    celula[ind + 1].fontemassLR = 0.;
    celula[ind + 1].fontemassGR = 0.;
    if (fabsl(celula[ind].acsr.bvol.freq) > 1 ) {
      double alfM = celula[ind].alf;
      double presM = celula[ind].pres;
      double tempM = celula[ind].temp;
      double presM1 = celula[ind].presR;
      double tempM1 = celula[ind].tempR;
      celula[ind].acsr.bvol.vazmass(presM, tempM, presM1, tempM1,0, alfM,1);
      celula[ind].fontemassLR -= celula[ind].acsr.bvol.MLiqP;
      celula[ind].fontemassGR -= celula[ind].acsr.bvol.MGas;
      celula[ind + 1].fontemassLR += celula[ind].acsr.bvol.MLiqP;
      celula[ind + 1].fontemassGR += celula[ind].acsr.bvol.MGas;
    }

  }
  if(celula[ind].acsr.tipo==12){//alteracao7
	    double tit=celula[ind].flui.Titulo(celula[ind].pres,celula[ind].temp,TEMP);
		celula[ind].fontemassLR=(1.-tit)*celula[ind].acsr.injmvap.Mass;
		celula[ind].fontemassGR=tit*celula[ind].acsr.injmvap.Mass;
  }
}

void SProdVap::renovaalbetini() {


	celula[0].alfini = celula[0].alf;
	celula[0].alfPigDini = celula[0].alfPigD;
    celula[0].alfPigEini = celula[0].alfPigE;
    celula[1].alfLini = celula[0].alfini;
	if(arq.ConContEntrada==0){
        celula[0].alfLini = celula[0].alfini;
   }
   else{
	    celula[0].alfLini = titE;
   }

  indpigP = 0;

  for (int i = 0; i <= ncel; i++) {
    celula[i].alfini = celula[i].alf;
    if (i > 0) celula[i - 1].alfRini = celula[i].alfini;
    if (i < ncel) celula[i + 1].alfLini = celula[i].alfini;


    celula[i].alfPigDini = celula[i].alfPigD;
    celula[i].alfPigEini = celula[i].alfPigE;
    if(i>0)celula[i-1].alfPigER =celula[i].alfPigE;
    if(i==ncel)celula[i].alfPigER =celula[i].alf;
    celula[i].razPigini = celula[i].razPig;
    celula[i].DelPig = 0.;		   //alteracao2
    celula[i].RazAreaPig = 0.;		   //alteracao2
    celula[i].cdpig = 1.;		   //alteracao2
    if (celula[i].estadoPig == 1) {
      indpigP++;
      int ipig = celula[i].indpig;		   //alteracao2
      celula[i].DelPig = arq.pig[ipig].delpres;		   //alteracao2
      celula[i].RazAreaPig = arq.pig[ipig].razarea;		   //alteracao2
      celula[i].cdpig = arq.pig[ipig].cdPig;		   //alteracao2
      double AC = celula[i].duto.area;
      //double rhogR = celula[i].flui.MasEspGas(celula[i].presR,celula[i].tempR);
      double jL = (celula[i].QL + celula[i].QG) / AC;
      //double jR=(celula[i].QLR+((celula[i].MR-celula[i].MliqiniR)/rhogR))/AC;
      double jR;
      if (i < ncel) jR = (celula[i + 1].QL + celula[i + 1].QG) / AC;
      else jR = (celula[i].QL + celula[i].QG) / AC;
      celula[i].velPig = jL * celula[i].razPig + jR * (1. - celula[i].razPig) - celula[i].VazaPig / AC;
      celula[i].velPigini = celula[i].velPig;
      for (int j = 0; j < npig; j++) {
        if (receb[j] == i) {
          //if(i==ncel-1){
          celula[i].velPig = 0.;
          celula[i].estadoPig = 0;
          celula[i].razPig = 0.;
          celula[i].razPigini = 0.;
          celula[i].alfPigDini = celula[i].alf;
          celula[i].alfPigEini = celula[i].alf;
          celula[i].alfPigD = celula[i].alf;
          celula[i].alfPigE = celula[i].alf;
          if(i>0)celula[i-1].alfPigER =celula[i].alfPigE;
          if(i==ncel)celula[i].alfPigER =celula[i].alf;

          indpigP--;

        }
      }
    }

  }
  celula[ncel].alfRini = celula[ncel].alfini;
}

void SProdVap::renovaMasEsp() {
  double p = celula[0].pres;
  double t = celula[0].temp;
  celula[0].rpC = celula[0].flui.MasEspLiq(p, t,TEMP);
  celula[0].rpL = celula[0].rpC;

  celula[0].rpCi = celula[0].rpC;
  celula[0].rpLi = celula[0].rpCi;

  for (int i = 1; i <= ncel; i++) {
    p = celula[i].pres;
    t = celula[i].temp;
    celula[i].rpC = celula[i].flui.MasEspLiq(p, t,TEMP);

    celula[i - 1].rpR = celula[i].rpC;
    celula[i].rpL = celula[i - 1].rpC;

    double tmed = celula[i - 1].temp;
    if (celula[i].VTemper < 0.) tmed = celula[i].temp;
    ProVap flu;
    celula[i].rpCi = flu.MasEspLiq(celula[i].presaux, tmed,TEMP);

    celula[i - 1].rpRi = celula[i].rpCi;
    celula[i].rpLi = celula[i - 1].rpCi;

  }
  celula[ncel].rpR = celula[ncel].rpC;

}

void SProdVap::CalcC0Ud(int ind, double& c0, double& ud) {

  c0 = 1.;
  ud = 0.;
  if (celula[ind - 1].velPig > 0 && celula[ind - 1].estadoPig == 1) {
    c0 = 1.;
    ud = 0.;
    celula[ind].arranjo = 1.;
    celula[ind - 1].arranjoR = 1.;
    celula[ind - 1].perdaEstratL = 0.;
    celula[ind - 1].perdaEstratG = 0.;
  } else if (celula[ind].velPig < 0 && celula[ind].estadoPig == 1) {
    c0 = 1.;
    ud = 0.;
    celula[ind].arranjo = 1.;
    celula[ind - 1].arranjoR = 1.;
    celula[ind - 1].perdaEstratL = 0.;
    celula[ind - 1].perdaEstratG = 0.;
  } else if ((celula[ind].acsr.tipo != 4 || celula[ind].acsr.bcs.freqnova <= 1.)
  /*&&(celula[ind].acsr.tipo!=8 || celula[ind].acsr.bvol.freq<=1.)*/) {
    double hns;
    double razdx = celula[ind].dx / (celula[ind].dx + celula[ind].dxL);
    double razdx0;
    if (ind > 0) razdx0 = celula[ind - 1].dx / (celula[ind - 1].dx + celula[ind - 1].dxL);
    else razdx0 = razdx;

    if (ind > 0) hns = 1. - celula[ind - 1].alfPigD;
    else hns = 1. - celula[ind].alf;
    if (celula[ind].QG < 0) hns = 1. - celula[ind].alfPigE;
    if (fabsl(celula[ind].QG) < (*vg1dSP).localtiny * 1e-5) {
      if (fabsl(celula[ind].alfPigE) < (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) < (*vg1dSP).localtiny
          && fabsl(celula[ind - 1].alfPigD) > (*vg1dSP).localtiny && fabsl(1. - celula[ind - 1].alfPigD) > (*vg1dSP).localtiny) hns = 1.
          - celula[ind - 1].alfPigD;
      else if (fabsl(celula[ind - 1].alfPigD) < (*vg1dSP).localtiny && fabsl(1. - celula[ind - 1].alfPigD) < (*vg1dSP).localtiny
          && fabsl(celula[ind].alfPigE) > (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) > (*vg1dSP).localtiny) hns = 1.
          - celula[ind].alfPigE;
      else hns = 0.5 * (1. - celula[ind].alfPigE + 1. - celula[ind - 1].alfPigD);
    }
    if(hns<(*vg1dSP).localtiny || hns> 1.-(*vg1dSP).localtiny)  hns = 0.5 * (1. - celula[ind].alfPigE + 1. - celula[ind - 1].alfPigD);

    double hol0 = hns;
    double alf0 = 1 - hol0;
    double razdx1 = celula[ind].dxR / (celula[ind].dxR + celula[ind].dx);
    double alf1;
    alf1 = celula[ind].alf;

    double alfneg;
    if (ind > 1) alfneg = celula[ind - 2].alf;
    else if (ind > 0) alfneg = celula[ind - 1].alf;
    else alfneg = celula[ind].alf;

    double pmed;
    double pmed0;

    pmed = celula[ind].presaux;
    if (ind > 0) pmed0 = celula[ind - 1].presaux;
    if(ind==ncel)pmed = celula[ind].pres;
    else pmed0 = celula[ind].presaux;
    double tmed = razdx * celula[ind].temp + (1 - razdx) * celula[ind].tempL;
    tmed = celula[ind].tempL;
    if (celula[ind].VTemper < 0.) tmed = celula[ind].temp;
    double tmed0;
    if (ind > 0) tmed0 = razdx0 * celula[ind - 1].temp + (1 - razdx0) * celula[ind - 1].tempL;
    else tmed0 = tmed;
    //tmed = celula[ind].temp;
    if(ind<ncel)tmed = celula[ind].temp;
    else tmed = tGSup;

    double rlm;
    double viscl1;
    double tensup1;
    //if (celula[ind].QL < 0.) { testeBeta
    if (celula[ind].QG < 0.) {
      rlm = celula[ind].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[ind].flui.TensSupFunc(pmed, tmed);
    } else {
      rlm = celula[ind - 1].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind - 1].flui.ViscLiq(pmed, tmed);
      tensup1 =celula[ind - 1].flui.TensSupFunc(pmed, tmed);
    }

    double rgm;
    double viscg1;
    if (celula[ind].QG < 0.) {
      rgm = celula[ind].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind].flui.ViscGas(pmed, tmed);
    } else {
      rgm = celula[ind - 1].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind - 1].flui.ViscGas(pmed, tmed);
    }

    //if(cel[ind+1].estadoPig==1)tensup1=0.07;

    double ug1 = (celula[ind].MC - celula[ind].Mliqini) / rgm;
    double ul1 = celula[ind].Mliqini / rlm;
    double dia1 = celula[ind].duto.a;
    if (ind > 0 && ug1 >= 0) dia1 = celula[ind - 1].duto.a;
    double A1 = M_PI * dia1 * dia1 / 4.;

    double rmed = hns * rlm + (1 - hns) * rgm;
    double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
    double nrey = dia1 * rmed * (fabsl(ug1) / A1 + fabsl(ul1) / A1) / visc;

    int xarr1 = 1;
    double dtot = celula[ind].dxL + celula[ind].dx + 0 * celula[ind].dxR;
    double razL = celula[ind].dxL;
    double raz = celula[ind].dx;
    double razR = 0 * celula[ind].dxR;
    double ang = (razL * celula[ind].dutoL.teta + raz * celula[ind].duto.teta + razR * celula[ind].dutoR.teta)
        / dtot;
    if (ind >= 2) {
      if (celula[ind - 2].acsr.tipo == 5 && celula[ind - 2].acsr.chk.AreaGarg <= (1e-3)) {
        if (celula[ind].QG >= 0) ang = celula[ind].duto.teta;
        else ang = celula[ind].dutoR.teta;
      } else {
        if (celula[ind].QG >= 0) ang = celula[ind].dutoL.teta;
        else ang = celula[ind].duto.teta;
      }
    }
    double sinal = 1.;
    if (ang < 0.) sinal = -1.;
    double xc0 = 2.;
    double xud = sinal * 0.0246 * cosl(ang)
        + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
    if (nrey > 1e-30) {
      if (fabsl(0 * ang + 1 * celula[ind].duto.teta) < 45. * M_PI / 180. && hol0 < 0.99 && hol0 > 0.01
          && ind < ncel - 1) {
        double ug0;
        double ul0;

        /*         ug1=(cel[ind].MC-cel[ind].Mliqini)/cel[ind].flui.MasEspGas(pmed,tmed);
         ul1=cel[ind].Mliqini/
         ((1-betI)*cel[ind].flui.MasEspLiq(pmed,tmed)+betI*cel[ind].fluicol.MasEspFlu(pmed,tmed));*/
        ug1 = (celula[ind].MC - celula[ind].Mliqini) / rgm;
        ul1 = celula[ind].Mliqini / rlm;

        ug0 = (celula[ind - 1].MC - celula[ind - 1].Mliqini) / celula[ind].flui.MasEspGas(pmed0, tmed0);
        ul0 = celula[ind - 1].Mliqini
            / (celula[ind].flui.MasEspLiq(pmed0, tmed0));

        estratificado testamapa(dia1, ul1, ug1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, celula[ind].duto.rug / dia1);

        /*if((*vg1dSP).lixo5>7141.){
         int para;
         para=0;
         }*/
        testamapa.mapaTD();
        //testamapa.mapa(dia1,1.-hol0);
        //double chute=testamapa.mapa2(celula[ind].chutearranjo);
        //celula[ind].chutearranjo=chute;
        xarr1 = testamapa.arr;
        //xarr1=1;
        if (xarr1 == -1) {
          if (celula[ind].arranjo != 0) {
            if (((celula[ind].arranjo != xarr1) || celula[ind].transic > 0)) {
              if ((celula[ind].arranjo != xarr1) && celula[ind].transic > 0) celula[ind].transic = 0;
              celula[ind].transic++;
              if (celula[ind].transic > 19) celula[ind].transic = 0;
            }
          } else celula[ind].transic = 0;
          celula[ind].arranjo = xarr1 = testamapa.arr;
          celula[ind - 1].arranjoR = testamapa.arr;
          celula[ind - 1].perdaEstratL = testamapa.fatorperdaLiq;
          celula[ind - 1].perdaEstratG = testamapa.fatorperdaGas;
          sinal = 1.;
          if (ang < 0.) sinal = -1.;
          double xc0C = 2. / (1 + pow(nrey / 1000., 2.))
              + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));
          double udC = sinal * 0.0246 * cosl(ang)
              + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
          double mult0, mult1;
          mult0 = 1.;
          if (ul0 < 0.) mult0 = 0.;
          mult1 = 0.;
          if (ul1 < 0.) mult1 = 1.;
          double alf0E = celula[ind - 1].alf;
          xc0 = 1.;

          c0 = xc0;

          c0 = xc0 = 1.;
          ud = xud;
          if ((fabsl(ug1) + fabsl(ul1)) / A1 > 0.5) {
            c0 = 1.04;
            ud = 0.466 * sinal;
          } else if ((fabsl(ug1) + fabsl(ul1)) / A1 < 0.1) {
            c0 = xc0C;
            ud = udC;
          } else {
            double raz = (0.5 - (fabsl(ug1) + fabsl(ul1)) / A1) / 0.4;
            c0 = ((1. - raz) * 1.04 + raz * xc0C);
            ud = ((1. - raz) * 0.466 * sinal + raz * udC);
          }

          if (EstadoMaster1 == 0) {
            if (contaMaster1 > 19) {
              c0 = xc0C;
              ud = udC;
            } else {
              c0 = (xc0C * contaMaster1 + celula[ind].c0 * (20. - contaMaster1)) / 20.;
              ud = (udC * contaMaster1 + celula[ind].ud * (20. - contaMaster1)) / 20.;
            }
          }
          if (celula[ind].transic > 0 && EstadoMaster1 == 1) {
            c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
            ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
          }
        }
      }
      if (xarr1 == 1) {

        arranjo testamapa2(dia1, ul1 / A1, ug1 / A1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, tensup1);
        xarr1 = testamapa2.verificaArr();

        double sinal = 1.;
        if (ang < 0.) sinal = -1.;
        c0 = 2. / (1 + pow(nrey / 1000., 2.))
            + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));

        ud = sinal * 0.0246 * cosl(ang) + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
        // if(alf0>0.9999 && xarr1!=-2){
        //c0=1.-(*vg1dSP).localtiny;
        //ud=0.;
        //xarr1=4;
        // }
        if (xarr1 == -2) {
          c0 = 1. + (1. - alf0) / (alf0 + 4. * sqrtl(rgm / rlm));
          ud = sinal * (1. - alf0)
              / (alf0 + 4. * sqrtl(rgm / rlm) * sqrtl(9.82 * dia1 * (rlm - rgm) * (1. - alf0)) / (0.015 * rgm));
        }

        if (celula[ind].arranjo != 0) {
          if ((xarr1 != celula[ind].arranjo || celula[ind].transic > 0)) {
            if (xarr1 != celula[ind].arranjo && celula[ind].transic > 0) celula[ind].transic = 0;
            celula[ind].transic++;
            if (celula[ind].transic > 19) celula[ind].transic = 0;
          }
        } else celula[ind].transic = 0;
        if (celula[ind].transic > 0) {
          c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
          ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
        }
        double udM = sinal * 0.0246 * cosl(celula[ind].dutoL.teta)
            + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(celula[ind].dutoL.teta);
        celula[ind].arranjo = xarr1;
        celula[ind - 1].arranjoR = xarr1;
      }
      celula[ind].c0Spare = c0;
      celula[ind].udSpare = ud;
      if (ind > 0) {
        if ((celula[ind].arranjo != celula[ind - 1].arranjo && celula[ind - 1].arranjo != 0) /*&& (cel[ind].arranjo != 4 && cel[ind-1].arranjo!=4)*/) {
          c0 = (celula[ind].dx * c0 + celula[ind - 1].dx * celula[ind - 1].c0Spare)
              / (celula[ind].dx + celula[ind - 1].dx);
          ud = (celula[ind].dx * ud + celula[ind - 1].dx * celula[ind - 1].udSpare)
              / (celula[ind].dx + celula[ind - 1].dx);
        } else if (/*(cel[ind].arranjo != 4 && cel[ind-1].arranjo!=4) &&*/ind > 2) {
          if (celula[ind - 2].acsr.tipo == 5 && celula[ind - 2].acsr.chk.AreaGarg <= (1e-3)) {
            if (celula[ind].QG >= 0) ang = celula[ind].duto.teta;
            else ang = celula[ind].dutoR.teta;
          } else {
            if (celula[ind].QG >= 0) ang = celula[ind].dutoL.teta;
            else ang = celula[ind].duto.teta;
          }
          double angL;
          if (celula[ind - 3].acsr.tipo == 5 && celula[ind - 3].acsr.chk.AreaGarg <= (1e-3)) {
            if (celula[ind - 1].QG >= 0) angL = celula[ind - 1].duto.teta;
            else angL = celula[ind - 1].dutoR.teta;
          } else {
            if (celula[ind - 1].QG >= 0) angL = celula[ind - 1].dutoL.teta;
            else angL = celula[ind - 1].duto.teta;
          }
          if (ang != angL) {
            c0 = (celula[ind].dx * c0 + celula[ind - 1].dx * celula[ind - 1].c0Spare)
                / (celula[ind].dx + celula[ind - 1].dx);
            ud = (celula[ind].dx * ud + celula[ind - 1].dx * celula[ind - 1].udSpare)
                / (celula[ind].dx + celula[ind - 1].dx);
          }
        }
      }
    }
  }
  if(arq.escorregaTran==0){
     double ulsmed=celula[ind].QL/celula[ind].duto.area;
     double correcaoUd=1-(ulsmed-0.15)/0.35;
     double correcaoCo=c0-(c0-1)*(ulsmed-0.15)/0.35;
     if(correcaoUd>1.) correcaoUd=1.;
     if(correcaoUd<0.) correcaoUd=0.;
     if(correcaoCo>c0) correcaoCo=c0;
     if(correcaoCo<1) correcaoCo=1;
     c0=correcaoCo;
     ud=ud*correcaoUd;
  }
}

void SProdVap::CalcC0UdBuf(int ind, double& c0, double& ud) {

  c0 = 1.;
  ud = 0.;
  if (celula[ind - 1].velPig > 0 && celula[ind - 1].estadoPig == 1) {
    c0 = 1.;
    ud = 0.;
    celula[ind].arranjo = 1.;
    celula[ind - 1].arranjoR = 1.;
    celula[ind - 1].perdaEstratL = 0.;
    celula[ind - 1].perdaEstratG = 0.;
  } else if (celula[ind].velPig < 0 && celula[ind].estadoPig == 1) {
    c0 = 1.;
    ud = 0.;
    celula[ind].arranjo = 1.;
    celula[ind - 1].arranjoR = 1.;
    celula[ind - 1].perdaEstratL = 0.;
    celula[ind - 1].perdaEstratG = 0.;
  } else if ((celula[ind].acsr.tipo != 4 || celula[ind].acsr.bcs.freqnova <= 1.)
  /*&&(celula[ind].acsr.tipo!=8 || celula[ind].acsr.bvol.freq<=1.)*/) {
    double hns;
    double razdx = celula[ind].dx / (celula[ind].dx + celula[ind].dxL);
    double razdx0;
    if (ind > 0) razdx0 = celula[ind - 1].dx / (celula[ind - 1].dx + celula[ind - 1].dxL);
    else razdx0 = razdx;

    if (ind > 0) hns = 1. - celula[ind - 1].alfPigD;
    else hns = 1. - celula[ind].alf;
    if ((celula[ind].MCBuf- celula[ind].MliqiniBuf)< 0) hns = 1. - celula[ind].alfPigE;
    if (fabsl((celula[ind].MCBuf- celula[ind].MliqiniBuf)) < (*vg1dSP).localtiny * 1e-5) {
      if (fabsl(celula[ind].alfPigE) < (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) < (*vg1dSP).localtiny
          && fabsl(celula[ind - 1].alfPigD) > (*vg1dSP).localtiny && fabsl(1. - celula[ind - 1].alfPigD) > (*vg1dSP).localtiny) hns = 1.
          - celula[ind - 1].alfPigD;
      else if (fabsl(celula[ind - 1].alfPigD) < (*vg1dSP).localtiny && fabsl(1. - celula[ind - 1].alfPigD) < (*vg1dSP).localtiny
          && fabsl(celula[ind].alfPigE) > (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) > (*vg1dSP).localtiny) hns = 1.
          - celula[ind].alfPigE;
      else hns = 0.5 * (1. - celula[ind].alfPigE + 1. - celula[ind - 1].alfPigD);
    }
    if(hns<(*vg1dSP).localtiny || hns> 1.-(*vg1dSP).localtiny)  hns = 0.5 * (1. - celula[ind].alfPigE + 1. - celula[ind - 1].alfPigD);

    double hol0 = hns;
    double alf0 = 1 - hol0;
    double razdx1 = celula[ind].dxR / (celula[ind].dxR + celula[ind].dx);
    double alf1;
    alf1 = celula[ind].alf;

    double alfneg;
    if (ind > 1) alfneg = celula[ind - 2].alf;
    else if (ind > 0) alfneg = celula[ind - 1].alf;
    else alfneg = celula[ind].alf;


    double pmed;
    double pmed0;

    pmed = razdx * celula[ind].presBuf + (1 - razdx) * celula[ind].presLBuf;
    if(ind==ncel)pmed = celula[ind].presBuf;
    pmed0 = celula[ind].presauxL;
    double tmed = razdx * celula[ind].temp + (1 - razdx) * celula[ind].tempL;
    tmed = celula[ind].tempL;
    if (celula[ind].VTemper < 0.) {
        if(ind<ncel)tmed = celula[ind].temp;
        else tmed = tGSup;
    	//tmed = celula[ind].temp;

    }
    double tmed0;
    if (ind > 0) tmed0 = razdx0 * celula[ind - 1].temp + (1 - razdx0) * celula[ind - 1].tempL;
    else tmed0 = tmed;

    double rlm;
    double viscl1;
    double tensup1;
    //if ((celula[ind].MliqiniBuf) < 0.) { testeBeta
    if ((celula[ind].MC-celula[ind].MliqiniBuf) < 0.) {
      rlm = celula[ind].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[ind].flui.TensSupFunc(pmed, tmed);
    } else {
      rlm = celula[ind - 1].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind - 1].flui.ViscLiq(pmed, tmed);
      tensup1 =celula[ind - 1].flui.TensSupFunc(pmed, tmed);
    }

    double rgm;
    double viscg1;
    if ((celula[ind].MCBuf- celula[ind].MliqiniBuf) < 0.) {
      rgm = celula[ind].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind].flui.ViscGas(pmed, tmed);
    } else {
      rgm = celula[ind - 1].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind - 1].flui.ViscGas(pmed, tmed);
    }

    //if(cel[ind+1].estadoPig==1)tensup1=0.07;

    double ug1 = (celula[ind].MCBuf- celula[ind].MliqiniBuf)/ rgm;
    double ul1 = (celula[ind].MliqiniBuf) / rlm;
    double dia1 = celula[ind].duto.a;
    if (ind > 0 && ug1 >= 0) dia1 = celula[ind - 1].duto.a;
    double A1 = M_PI * dia1 * dia1 / 4.;

    double rmed = hns * rlm + (1 - hns) * rgm;
    double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
    double nrey = dia1 * rmed * (fabsl(ug1) / A1 + fabsl(ul1) / A1) / visc;

    int xarr1 = 1;
    double dtot = celula[ind].dxL + celula[ind].dx + 0 * celula[ind].dxR;
    double razL = celula[ind].dxL;
    double raz = celula[ind].dx;
    double razR = 0 * celula[ind].dxR;
    double ang = (razL * celula[ind].dutoL.teta + raz * celula[ind].duto.teta + razR * celula[ind].dutoR.teta)
        / dtot;
    if (ind >= 2) {
      if (celula[ind - 2].acsr.tipo == 5 && celula[ind - 2].acsr.chk.AreaGarg <= (1e-3)) {
        if ((celula[ind].MCBuf- celula[ind].MliqiniBuf)>= 0) ang = celula[ind].duto.teta;
        else ang = celula[ind].dutoR.teta;
      } else {
        if ((celula[ind].MCBuf- celula[ind].MliqiniBuf)>= 0) ang = celula[ind].dutoL.teta;
        else ang = celula[ind].duto.teta;
      }
    }
    double sinal = 1.;
    if (ang < 0.) sinal = -1.;
    double xc0 = 2.;
    double xud = sinal * 0.0246 * cosl(ang)
        + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
    if (nrey > 1e-30) {
      if (fabsl(0 * ang + 1 * celula[ind].duto.teta) < 45. * M_PI / 180. && hol0 < 0.99 && hol0 > 0.01
          && ind < ncel - 1) {
        double ug0;
        double ul0;

        /*         ug1=(cel[ind].MC-cel[ind].Mliqini)/cel[ind].flui.MasEspGas(pmed,tmed);
         ul1=cel[ind].Mliqini/
         ((1-betI)*cel[ind].flui.MasEspLiq(pmed,tmed)+betI*cel[ind].fluicol.MasEspFlu(pmed,tmed));*/
        ug1 = (celula[ind].MCBuf- celula[ind].MliqiniBuf)/ rgm;
        ul1 =(celula[ind].MliqiniBuf)/ rlm;

        ug0 = (celula[ind-1].MCBuf- celula[ind-1].MliqiniBuf) / celula[ind].flui.MasEspGas(pmed0, tmed0);
        ul0 = (celula[ind-1].MliqiniBuf)
            / (celula[ind].flui.MasEspLiq(pmed0, tmed0));

        estratificado testamapa(dia1, ul1, ug1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, celula[ind].duto.rug / dia1);

        /*if((*vg1dSP).lixo5>7141.){
         int para;
         para=0;
         }*/
        testamapa.mapaTD();
        //testamapa.mapa(dia1,1.-hol0);
        //double chute=testamapa.mapa2(celula[ind].chutearranjo);
        //celula[ind].chutearranjo=chute;
        xarr1 = testamapa.arr;
        //xarr1=1;
        if (xarr1 == -1) {
          celula[ind].arranjo = xarr1 = testamapa.arr;
          celula[ind - 1].arranjoR = testamapa.arr;
          celula[ind - 1].perdaEstratL = testamapa.fatorperdaLiq;
          celula[ind - 1].perdaEstratG = testamapa.fatorperdaGas;
          sinal = 1.;
          if (ang < 0.) sinal = -1.;
          double xc0C = 2. / (1 + pow(nrey / 1000., 2.))
              + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));
          double udC = sinal * 0.0246 * cosl(ang)
              + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
          double mult0, mult1;
          mult0 = 1.;
          if (ul0 < 0.) mult0 = 0.;
          mult1 = 0.;
          if (ul1 < 0.) mult1 = 1.;
          double alf0E = celula[ind - 1].alf;
          xc0 = 1.;

          c0 = xc0;

          c0 = xc0 = 1.;
          ud = xud;
          if ((fabsl(ug1) + fabsl(ul1)) / A1 > 0.5) {
            c0 = 1.04;
            ud = 0.466 * sinal;
          } else if ((fabsl(ug1) + fabsl(ul1)) / A1 < 0.1) {
            c0 = xc0C;
            ud = udC;
          } else {
            double raz = (0.5 - (fabsl(ug1) + fabsl(ul1)) / A1) / 0.4;
            c0 = ((1. - raz) * 1.04 + raz * xc0C);
            ud = ((1. - raz) * 0.466 * sinal + raz * udC);
          }

          if (EstadoMaster1 == 0) {
            if (contaMaster1 > 19) {
              c0 = xc0C;
              ud = udC;
            } else {
              c0 = (xc0C * contaMaster1 + celula[ind].c0 * (20. - contaMaster1)) / 20.;
              ud = (udC * contaMaster1 + celula[ind].ud * (20. - contaMaster1)) / 20.;
            }
          }
          if (celula[ind].transic > 0 && EstadoMaster1 == 1) {
            c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
            ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
          }
        }
      }
      if (xarr1 == 1) {

        arranjo testamapa2(dia1, ul1 / A1, ug1 / A1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, tensup1);
        xarr1 = testamapa2.verificaArr();

        double sinal = 1.;
        if (ang < 0.) sinal = -1.;
        c0 = 2. / (1 + pow(nrey / 1000., 2.))
            + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));

        ud = sinal * 0.0246 * cosl(ang) + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
        // if(alf0>0.9999 && xarr1!=-2){
        //c0=1.-(*vg1dSP).localtiny;
        //ud=0.;
        //xarr1=4;
        // }
        if (xarr1 == -2) {
          c0 = 1. + (1. - alf0) / (alf0 + 4. * sqrtl(rgm / rlm));
          ud = sinal * (1. - alf0)
              / (alf0 + 4. * sqrtl(rgm / rlm) * sqrtl(9.82 * dia1 * (rlm - rgm) * (1. - alf0)) / (0.015 * rgm));
        }

        if (celula[ind].transic > 0) {
          c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
          ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
        }
        double udM = sinal * 0.0246 * cosl(celula[ind].dutoL.teta)
            + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(celula[ind].dutoL.teta);
        celula[ind].arranjo = xarr1;
        celula[ind - 1].arranjoR = xarr1;
      }
      celula[ind].c0Spare = c0;
      celula[ind].udSpare = ud;
      if (ind > 0) {
        if ((celula[ind].arranjo != celula[ind - 1].arranjo && celula[ind - 1].arranjo != 0) /*&& (cel[ind].arranjo != 4 && cel[ind-1].arranjo!=4)*/) {
          c0 = (celula[ind].dx * c0 + celula[ind - 1].dx * celula[ind - 1].c0Spare)
              / (celula[ind].dx + celula[ind - 1].dx);
          ud = (celula[ind].dx * ud + celula[ind - 1].dx * celula[ind - 1].udSpare)
              / (celula[ind].dx + celula[ind - 1].dx);
        } else if (/*(cel[ind].arranjo != 4 && cel[ind-1].arranjo!=4) &&*/ind > 2) {
          if (celula[ind - 2].acsr.tipo == 5 && celula[ind - 2].acsr.chk.AreaGarg <= (1e-3)) {
            if ((celula[ind].MCBuf- celula[ind].MliqiniBuf) >= 0) ang = celula[ind].duto.teta;
            else ang = celula[ind].dutoR.teta;
          } else {
            if ((celula[ind].MCBuf- celula[ind].MliqiniBuf) >= 0) ang = celula[ind].dutoL.teta;
            else ang = celula[ind].duto.teta;
          }
          double angL;
          if (celula[ind - 3].acsr.tipo == 5 && celula[ind - 3].acsr.chk.AreaGarg <= (1e-3)) {
            if ((celula[ind-1].MCBuf- celula[ind-1].MliqiniBuf)>= 0) angL = celula[ind - 1].duto.teta;
            else angL = celula[ind - 1].dutoR.teta;
          } else {
            if ((celula[ind-1].MCBuf- celula[ind-1].MliqiniBuf) >= 0) angL = celula[ind - 1].dutoL.teta;
            else angL = celula[ind - 1].duto.teta;
          }
          if (ang != angL) {
            c0 = (celula[ind].dx * c0 + celula[ind - 1].dx * celula[ind - 1].c0Spare)
                / (celula[ind].dx + celula[ind - 1].dx);
            ud = (celula[ind].dx * ud + celula[ind - 1].dx * celula[ind - 1].udSpare)
                / (celula[ind].dx + celula[ind - 1].dx);
          }
        }
      }
    }
  }
  if(arq.escorregaTran==0){
     double ulsmed=celula[ind].QL/celula[ind].duto.area;
     double correcaoUd=1-(ulsmed-0.15)/0.35;
     double correcaoCo=c0-(c0-1)*(ulsmed-0.15)/0.35;
     if(correcaoUd>1.) correcaoUd=1.;
     if(correcaoUd<0.) correcaoUd=0.;
     if(correcaoCo>c0) correcaoCo=c0;
     if(correcaoCo<1) correcaoCo=1;
     c0=correcaoCo;
     ud=ud*correcaoUd;
  }
}

void SProdVap::CalcC0UdIni(int ind, double& c0, double& ud) {

  c0 = 1.;
  ud = 0.;
 if (celula[ind].velPig < 0 && celula[ind].estadoPig == 1) {
    c0 = 1.;
    ud = 0.;
    celula[ind].arranjo = 1.;

  } else if ((celula[ind].acsr.tipo != 4 || celula[ind].acsr.bcs.freqnova <= 1.)
  /*&&(celula[ind].acsr.tipo!=8 || celula[ind].acsr.bvol.freq<=1.)*/) {
    double hns;

    hns =1- alfE;

    if (celula[ind].QG < 0) hns = 1. - celula[ind].alfPigE;
    if (fabsl(celula[ind].QG) < (*vg1dSP).localtiny * 1e-5) {
      if (fabsl(celula[ind].alfPigE) < (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) < (*vg1dSP).localtiny
          && fabsl(alfE) > (*vg1dSP).localtiny && fabsl(1. - alfE) > (*vg1dSP).localtiny) hns = 1-alfE;
      else if (fabsl(alfE) < (*vg1dSP).localtiny && fabsl(1. - celula[ind - 1].alfPigD) < (*vg1dSP).localtiny
          && fabsl(celula[ind].alfPigE) > (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) > (*vg1dSP).localtiny) hns = 1.
          - celula[ind].alfPigE;
      else hns = 0.5 * (1. - celula[ind].alfPigE + 1. - alfE);
    }
    if(hns<(*vg1dSP).localtiny || hns> 1.-(*vg1dSP).localtiny)  hns = 0.5 * (1. - celula[ind].alfPigE + 1. - alfE);

    double hol0 = hns;
    double alf0 = 1 - hol0;
    double razdx1 = celula[ind].dxR / (celula[ind].dxR + celula[ind].dx);
    double alf1;
    alf1 = celula[ind].alf;

    double alfneg;
    if (ind > 1) alfneg = alfE;
    else if (ind > 0) alfneg = alfE;
    else alfneg = celula[ind].alf;

    double pmed;
    double pmed0;

    pmed = presE;
    if (ind > 0) pmed0 = presE;
    if(ind==ncel)pmed = celula[ind].pres;
    else pmed0 = presE;
    double tmed = tempE;

    double tmed0;
    tmed0 = tmed;


    double rlm;
    double viscl1;
    double tensup1;
    //if (celula[ind].QL < 0.) { testeBeta
    if (celula[ind].QG < 0.) {
      rlm = celula[ind].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[ind].flui.TensSupFunc(pmed, tmed);
    } else {
      rlm = celula[ind ].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind ].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[ind ].flui.TensSupFunc(pmed, tmed);
    }

    double rgm;
    double viscg1;
    if (celula[ind].QG < 0.) {
      rgm = celula[ind].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind].flui.ViscGas(pmed, tmed);
    } else {
      rgm = celula[ind ].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind ].flui.ViscGas(pmed, tmed);
    }

    //if(cel[ind+1].estadoPig==1)tensup1=0.07;

    double ug1 = (celula[ind].MC - celula[ind].Mliqini) / rgm;
    double ul1 = celula[ind].Mliqini / rlm;
    double dia1 = celula[ind].duto.a;
    if (ind > 0 && ug1 >= 0) dia1 = celula[ind - 1].duto.a;
    double A1 = M_PI * dia1 * dia1 / 4.;

    double rmed = hns * rlm + (1 - hns) * rgm;
    double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
    double nrey = dia1 * rmed * (fabsl(ug1) / A1 + fabsl(ul1) / A1) / visc;

    int xarr1 = 1;
    double dtot = celula[ind].dxL + celula[ind].dx + 0 * celula[ind].dxR;
    double razL = celula[ind].dxL;
    double raz = celula[ind].dx;
    double razR = 0 * celula[ind].dxR;
    double ang = (razL * celula[ind].dutoL.teta + raz * celula[ind].duto.teta + razR * celula[ind].dutoR.teta)
        / dtot;
    double sinal = 1.;
    if (ang < 0.) sinal = -1.;
    double xc0 = 2.;
    double xud = sinal * 0.0246 * cosl(ang)
        + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
    if (nrey > 1e-30) {
      if (fabsl(0 * ang + 1 * celula[ind].duto.teta) < 45. * M_PI / 180. && hol0 < 0.99 && hol0 > 0.01
          && ind < ncel - 1) {
        double ug0;
        double ul0;

        /*         ug1=(cel[ind].MC-cel[ind].Mliqini)/cel[ind].flui.MasEspGas(pmed,tmed);
         ul1=cel[ind].Mliqini/
         ((1-betI)*cel[ind].flui.MasEspLiq(pmed,tmed)+betI*cel[ind].fluicol.MasEspFlu(pmed,tmed));*/
        ug1 = (celula[ind].MC - celula[ind].Mliqini) / rgm;
        ul1 = celula[ind].Mliqini / rlm;

        ug0 = (celula[ind].MC - celula[ind ].Mliqini) / celula[ind ].flui.MasEspGas(pmed0, tmed0);
        ul0 = celula[ind ].Mliqini
            / (celula[ind ].flui.MasEspLiq(pmed0, tmed0));

        estratificado testamapa(dia1, ul1, ug1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, celula[ind].duto.rug / dia1);

        /*if((*vg1dSP).lixo5>7141.){
         int para;
         para=0;
         }*/
        testamapa.mapaTD();
        //testamapa.mapa(dia1,1.-hol0);
        //double chute=testamapa.mapa2(celula[ind].chutearranjo);
        //celula[ind].chutearranjo=chute;
        xarr1 = testamapa.arr;
        //xarr1=1;
        if (xarr1 == -1) {
          if (celula[ind].arranjo != 0) {
            if (((celula[ind].arranjo != xarr1) || celula[ind].transic > 0)) {
              if ((celula[ind].arranjo != xarr1) && celula[ind].transic > 0) celula[ind].transic = 0;
              celula[ind].transic++;
              if (celula[ind].transic > 19) celula[ind].transic = 0;
            }
          } else celula[ind].transic = 0;
          celula[ind].arranjo = xarr1 = testamapa.arr;
          sinal = 1.;
          if (ang < 0.) sinal = -1.;
          double xc0C = 2. / (1 + pow(nrey / 1000., 2.))
              + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));
          double udC = sinal * 0.0246 * cosl(ang)
              + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
          double mult0, mult1;
          mult0 = 1.;
          if (ul0 < 0.) mult0 = 0.;
          mult1 = 0.;
          if (ul1 < 0.) mult1 = 1.;
          double alf0E = alfE;
          xc0 = 1.;

          c0 = xc0;

          c0 = xc0 = 1.;
          ud = xud;
          if ((fabsl(ug1) + fabsl(ul1)) / A1 > 0.5) {
            c0 = 1.04;
            ud = 0.466 * sinal;
          } else if ((fabsl(ug1) + fabsl(ul1)) / A1 < 0.1) {
            c0 = xc0C;
            ud = udC;
          } else {
            double raz = (0.5 - (fabsl(ug1) + fabsl(ul1)) / A1) / 0.4;
            c0 = ((1. - raz) * 1.04 + raz * xc0C);
            ud = ((1. - raz) * 0.466 * sinal + raz * udC);
          }

          if (EstadoMaster1 == 0) {
            if (contaMaster1 > 19) {
              c0 = xc0C;
              ud = udC;
            } else {
              c0 = (xc0C * contaMaster1 + celula[ind].c0 * (20. - contaMaster1)) / 20.;
              ud = (udC * contaMaster1 + celula[ind].ud * (20. - contaMaster1)) / 20.;
            }
          }
          if (celula[ind].transic > 0 && EstadoMaster1 == 1) {
            c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
            ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
          }
        }
      }
      if (xarr1 == 1) {

        arranjo testamapa2(dia1, ul1 / A1, ug1 / A1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, tensup1);
        xarr1 = testamapa2.verificaArr();

        double sinal = 1.;
        if (ang < 0.) sinal = -1.;
        c0 = 2. / (1 + pow(nrey / 1000., 2.))
            + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));

        ud = sinal * 0.0246 * cosl(ang) + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
        // if(alf0>0.9999 && xarr1!=-2){
        //c0=1.-(*vg1dSP).localtiny;
        //ud=0.;
        //xarr1=4;
        // }
        if (xarr1 == -2) {
          c0 = 1. + (1. - alf0) / (alf0 + 4. * sqrtl(rgm / rlm));
          ud = sinal * (1. - alf0)
              / (alf0 + 4. * sqrtl(rgm / rlm) * sqrtl(9.82 * dia1 * (rlm - rgm) * (1. - alf0)) / (0.015 * rgm));
        }

        if (celula[ind].arranjo != 0) {
          if ((xarr1 != celula[ind].arranjo || celula[ind].transic > 0)) {
            if (xarr1 != celula[ind].arranjo && celula[ind].transic > 0) celula[ind].transic = 0;
            celula[ind].transic++;
            if (celula[ind].transic > 19) celula[ind].transic = 0;
          }
        } else celula[ind].transic = 0;
        if (celula[ind].transic > 0) {
          c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
          ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
        }
        double udM = sinal * 0.0246 * cosl(celula[ind].dutoL.teta)
            + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(celula[ind].dutoL.teta);
        celula[ind].arranjo = xarr1;

      }
      celula[ind].c0Spare = c0;
      celula[ind].udSpare = ud;

    }
  }
  if(arq.escorregaTran==0){
     double ulsmed=celula[ind].QL/celula[ind].duto.area;
     double correcaoUd=1-(ulsmed-0.15)/0.35;
     double correcaoCo=c0-(c0-1)*(ulsmed-0.15)/0.35;
     if(correcaoUd>1.) correcaoUd=1.;
     if(correcaoUd<0.) correcaoUd=0.;
     if(correcaoCo>c0) correcaoCo=c0;
     if(correcaoCo<1) correcaoCo=1;
     c0=correcaoCo;
     ud=ud*correcaoUd;
  }
}

void SProdVap::CalcC0UdIniBuf(int ind, double& c0, double& ud) {

  c0 = 1.;
  ud = 0.;
 if (celula[ind].velPig < 0 && celula[ind].estadoPig == 1) {
    c0 = 1.;
    ud = 0.;
    celula[ind].arranjo = 1.;

  } else if ((celula[ind].acsr.tipo != 4 || celula[ind].acsr.bcs.freqnova <= 1.)
  /*&&(celula[ind].acsr.tipo!=8 || celula[ind].acsr.bvol.freq<=1.)*/) {
    double hns;

    hns =1- alfE;

    if ((celula[ind].MCBuf-celula[ind].MliqiniBuf) < 0) hns = 1. - celula[ind].alfPigE;
    if (fabsl(celula[ind].MCBuf-celula[ind].MliqiniBuf) < (*vg1dSP).localtiny * 1e-5) {
      if (fabsl(celula[ind].alfPigE) < (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) < (*vg1dSP).localtiny
          && fabsl(alfE) > (*vg1dSP).localtiny && fabsl(1. - alfE) > (*vg1dSP).localtiny) hns = 1-alfE;
      else if (fabsl(alfE) < (*vg1dSP).localtiny && fabsl(1. - celula[ind - 1].alfPigD) < (*vg1dSP).localtiny
          && fabsl(celula[ind].alfPigE) > (*vg1dSP).localtiny && fabsl(1. - celula[ind].alfPigE) > (*vg1dSP).localtiny) hns = 1.
          - celula[ind].alfPigE;
      else hns = 0.5 * (1. - celula[ind].alfPigE + 1. - alfE);
    }
    if(hns<(*vg1dSP).localtiny || hns> 1.-(*vg1dSP).localtiny)  hns = 0.5 * (1. - celula[ind].alfPigE + 1. - alfE);

    double hol0 = hns;
    double alf0 = 1 - hol0;
    double razdx1 = celula[ind].dxR / (celula[ind].dxR + celula[ind].dx);
    double alf1;
    alf1 = celula[ind].alf;

    double alfneg;
    if (ind > 1) alfneg = alfE;
    else if (ind > 0) alfneg = alfE;
    else alfneg = celula[ind].alf;

    double pmed;
    double pmed0;

    pmed = presE;
    if (ind > 0) pmed0 = presE;
    else pmed0 = presE;
    double tmed = tempE;

    double tmed0;
    tmed0 = tmed;


    double rlm;
    double viscl1;
    double tensup1;
    //if (celula[ind].MliqiniBuf < 0.) { testeBeta
    if (celula[ind].MC-celula[ind].MliqiniBuf < 0.) {
      rlm =celula[ind].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[ind].flui.TensSupFunc(pmed, tmed);
    } else {
      rlm = celula[ind ].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[ind ].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[ind ].flui.TensSupFunc(pmed, tmed);
    }

    double rgm;
    double viscg1;
    if ((celula[ind].MCBuf-celula[ind].MliqiniBuf) < 0.) {
      rgm = celula[ind].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind].flui.ViscGas(pmed, tmed);
    } else {
      rgm = celula[ind ].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[ind ].flui.ViscGas(pmed, tmed);
    }

    //if(cel[ind+1].estadoPig==1)tensup1=0.07;

    double ug1 = (celula[ind].MCBuf - celula[ind].MliqiniBuf) / rgm;
    double ul1 = celula[ind].MliqiniBuf / rlm;
    double dia1 = celula[ind].duto.a;
    if (ind > 0 && ug1 >= 0) dia1 = celula[ind - 1].duto.a;
    double A1 = M_PI * dia1 * dia1 / 4.;

    double rmed = hns * rlm + (1 - hns) * rgm;
    double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
    double nrey = dia1 * rmed * (fabsl(ug1) / A1 + fabsl(ul1) / A1) / visc;

    int xarr1 = 1;
    double dtot = celula[ind].dxL + celula[ind].dx + 0 * celula[ind].dxR;
    double razL = celula[ind].dxL;
    double raz = celula[ind].dx;
    double razR = 0 * celula[ind].dxR;
    double ang = (razL * celula[ind].dutoL.teta + raz * celula[ind].duto.teta + razR * celula[ind].dutoR.teta)
        / dtot;
    double sinal = 1.;
    if (ang < 0.) sinal = -1.;
    double xc0 = 2.;
    double xud = sinal * 0.0246 * cosl(ang)
        + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
    if (nrey > 1e-30) {
      if (fabsl(0 * ang + 1 * celula[ind].duto.teta) < 45. * M_PI / 180. && hol0 < 0.99 && hol0 > 0.01
          && ind < ncel - 1) {
        double ug0;
        double ul0;

        /*         ug1=(cel[ind].MC-cel[ind].Mliqini)/cel[ind].flui.MasEspGas(pmed,tmed);
         ul1=cel[ind].Mliqini/
         ((1-betI)*cel[ind].flui.MasEspLiq(pmed,tmed)+betI*cel[ind].fluicol.MasEspFlu(pmed,tmed));*/
        ug1 = (celula[ind].MCBuf - celula[ind].MliqiniBuf) / rgm;
        ul1 = celula[ind].MliqiniBuf / rlm;

        ug0 = (celula[ind].MCBuf - celula[ind ].MliqiniBuf) / celula[ind ].flui.MasEspGas(pmed0, tmed0);
        ul0 = celula[ind ].MliqiniBuf
            / (celula[ind ].flui.MasEspLiq(pmed0, tmed0));

        estratificado testamapa(dia1, ul1, ug1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, celula[ind].duto.rug / dia1);

        /*if((*vg1dSP).lixo5>7141.){
         int para;
         para=0;
         }*/
        testamapa.mapaTD();
        //testamapa.mapa(dia1,1.-hol0);
        //double chute=testamapa.mapa2(celula[ind].chutearranjo);
        //celula[ind].chutearranjo=chute;
        xarr1 = testamapa.arr;
        //xarr1=1;
        if (xarr1 == -1) {
          celula[ind].arranjo = xarr1 = testamapa.arr;
          sinal = 1.;
          if (ang < 0.) sinal = -1.;
          double xc0C = 2. / (1 + pow(nrey / 1000., 2.))
              + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));
          double udC = sinal * 0.0246 * cosl(ang)
              + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
          double mult0, mult1;
          mult0 = 1.;
          if (ul0 < 0.) mult0 = 0.;
          mult1 = 0.;
          if (ul1 < 0.) mult1 = 1.;
          double alf0E = alfE;
          xc0 = 1.;

          c0 = xc0;

          c0 = xc0 = 1.;
          ud = xud;
          if ((fabsl(ug1) + fabsl(ul1)) / A1 > 0.5) {
            c0 = 1.04;
            ud = 0.466 * sinal;
          } else if ((fabsl(ug1) + fabsl(ul1)) / A1 < 0.1) {
            c0 = xc0C;
            ud = udC;
          } else {
            double raz = (0.5 - (fabsl(ug1) + fabsl(ul1)) / A1) / 0.4;
            c0 = ((1. - raz) * 1.04 + raz * xc0C);
            ud = ((1. - raz) * 0.466 * sinal + raz * udC);
          }

          if (EstadoMaster1 == 0) {
            if (contaMaster1 > 19) {
              c0 = xc0C;
              ud = udC;
            } else {
              c0 = (xc0C * contaMaster1 + celula[ind].c0 * (20. - contaMaster1)) / 20.;
              ud = (udC * contaMaster1 + celula[ind].ud * (20. - contaMaster1)) / 20.;
            }
          }
          if (celula[ind].transic > 0 && EstadoMaster1 == 1) {
            c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
            ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
          }
        }
      }
      if (xarr1 == 1) {

        arranjo testamapa2(dia1, ul1 / A1, ug1 / A1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, tensup1);
        xarr1 = testamapa2.verificaArr();

        double sinal = 1.;
        if (ang < 0.) sinal = -1.;
        c0 = 2. / (1 + pow(nrey / 1000., 2.))
            + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));

        ud = sinal * 0.0246 * cosl(ang) + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
        // if(alf0>0.9999 && xarr1!=-2){
        //c0=1.-(*vg1dSP).localtiny;
        //ud=0.;
        //xarr1=4;
        // }
        if (xarr1 == -2) {
          c0 = 1. + (1. - alf0) / (alf0 + 4. * sqrtl(rgm / rlm));
          ud = sinal * (1. - alf0)
              / (alf0 + 4. * sqrtl(rgm / rlm) * sqrtl(9.82 * dia1 * (rlm - rgm) * (1. - alf0)) / (0.015 * rgm));
        }

        if (celula[ind].transic > 0) {
          c0 = (c0 * celula[ind].transic + celula[ind].c0 * (20. - celula[ind].transic)) / 20.;
          ud = (ud * celula[ind].transic + celula[ind].ud * (20. - celula[ind].transic)) / 20.;
        }
        double udM = sinal * 0.0246 * cosl(celula[ind].dutoL.teta)
            + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(celula[ind].dutoL.teta);
        celula[ind].arranjo = xarr1;

      }
      celula[ind].c0Spare = c0;
      celula[ind].udSpare = ud;

    }
  }
  if(arq.escorregaTran==0){
     double ulsmed=celula[ind].QL/celula[ind].duto.area;
     double correcaoUd=1-(ulsmed-0.15)/0.35;
     double correcaoCo=c0-(c0-1)*(ulsmed-0.15)/0.35;
     if(correcaoUd>1.) correcaoUd=1.;
     if(correcaoUd<0.) correcaoUd=0.;
     if(correcaoCo>c0) correcaoCo=c0;
     if(correcaoCo<1) correcaoCo=1;
     c0=correcaoCo;
     ud=ud*correcaoUd;
  }
}

void  SProdVap::renova() {//alteracao7
	for (int i = 0; i <= ncel; i++) {
		if (i != 0 && i != ncel) {

			celula[i].pres = termolivreP[2 * i+1];
			celula[i].MC = termolivreP[2 * i];
			celula[i+1].presLini=celula[i+1].presL;
			celula[i+1].presL= celula[i-1].presR= celula[i].pres;
			celula[i-1].MRini=celula[i-1].MR;
			celula[i+1].ML=celula[i-1].MR=celula[i].MC;

			celula[i-1].MliqiniR0=celula[i-1].MliqiniR;// alteracao4
			celula[i].Mliqini=celula[i].term1*celula[i].MC+celula[i].term2;
			celula[i+1].MliqiniL=celula[i-1].MliqiniR=celula[i].Mliqini;
		}
		else if (i == 0) {
			celula[0].pres = termolivreP[1];
			celula[0].presaux=celula[0].pres;
			celula[0].MC = termolivreP[0];
			celula[1].presLini=celula[1].presL;
			celula[1].presL= celula[0].pres;
			celula[1].ML=celula[0].MC;
			celula[0].Mliqini = celula[i].term1*celula[i].MC+celula[i].term2;
			celula[1].MliqiniL=celula[0].Mliqini;
		}
		else {
			celula[ncel].pres = termolivreP[2 * ncel+1];
			celula[ncel].MC = termolivreP[2 * ncel];
			celula[ncel-1].presR= celula[ncel].pres;
			celula[ncel-1].MR=celula[ncel].MC;
			celula[ncel].MR=celula[ncel].MC;

			celula[i-1].MliqiniR0=celula[i-1].MliqiniR;// alteracao4
			celula[ncel].Mliqini=celula[ncel].term1*celula[ncel].MC+celula[ncel].term2;
			celula[ncel-1].MliqiniR=celula[ncel].Mliqini;
			celula[ncel].MliqiniR=celula[ncel].Mliqini;
		}
	}
}

///////////////////////////// alteracao7 //////////////////////////////////////////////
void  SProdVap::renovaBuffer() {

	celula[0].presBuf = termolivreP[1];
	celula[0].presauxBuf=celula[0].presBuf;
	celula[0].MCBuf = termolivreP[0];
	celula[1].presLiniBuf=celula[1].presLBuf;
	celula[1].presLBuf= celula[0].presBuf;
	celula[1].MLBuf=celula[0].MCBuf;
	celula[0].MliqiniBuf = celula[0].term1*celula[0].MCBuf+celula[0].term2;
	celula[1].MliqiniLBuf=celula[0].MliqiniBuf;


	int i=1;
	celula[i].presBuf = termolivreP[2 * i+1];
	celula[i].MCBuf = termolivreP[2 * i];
	celula[i+1].presLiniBuf=celula[i+1].presLBuf;
	celula[i+1].presLBuf= celula[i-1].presRBuf= celula[i].presBuf;
	celula[i-1].MRiniBuf=celula[i-1].MRBuf;
	celula[i+1].MLBuf=celula[i-1].MRBuf=celula[i].MCBuf;

	celula[i].MliqiniBuf=celula[i].term1*celula[i].MCBuf+celula[i].term2;
	celula[i+1].MliqiniLBuf=celula[i-1].MliqiniRBuf=celula[i].MliqiniBuf;

	celula[ncel].presBuf = termolivreP[2 * ncel+1];
	celula[ncel].MCBuf = termolivreP[2 * ncel];
	celula[ncel-1].presRBuf= celula[ncel].presBuf;
	celula[ncel-1].MRBuf=celula[ncel].MCBuf;
	celula[ncel].MRBuf=celula[ncel].MCBuf;

	celula[ncel].MliqiniBuf=celula[ncel].term1*celula[ncel].MCBuf+celula[ncel].term2;
	celula[ncel-1].MliqiniRBuf=celula[ncel].MliqiniBuf;
	celula[ncel].MliqiniRBuf=celula[ncel].MliqiniBuf;


}
///////////////////////////// alteracao7 //////////////////////////////////////////////
///////////////////////////// alteracao8 //////////////////////////////////////////////
void  SProdVap::renovaBufferCego() {

	celula[0].presBuf = celula[0].pres;
	celula[0].presauxBuf=celula[0].presaux;
	celula[0].MCBuf = celula[0].MC;
	celula[1].presLiniBuf=celula[1].presLini;
	celula[1].presLBuf= celula[1].presL;
	celula[1].MLBuf=celula[1].ML;
	celula[0].MliqiniBuf = celula[0].Mliqini;
	celula[1].MliqiniLBuf=celula[1].MliqiniL;


	int i=1;
	celula[i].presBuf = celula[i].pres;
	celula[i].MCBuf = celula[i].MC;
	celula[i+1].presLiniBuf=celula[i+1].presLini;
	celula[i+1].presLBuf=celula[i-1].presRBuf= celula[i].pres;
	celula[i-1].MRiniBuf=celula[i-1].MR;
	celula[i+1].MLBuf=celula[i-1].MRBuf=celula[i].MC;

	celula[i].MliqiniBuf=celula[i].Mliqini;
	celula[i+1].MliqiniLBuf=celula[i-1].MliqiniRBuf=celula[i].Mliqini;

	celula[ncel].presBuf = celula[ncel].pres;
	celula[ncel].MCBuf = celula[ncel].MC;
	celula[ncel-1].presRBuf= celula[ncel].pres;
	celula[ncel-1].MRBuf=celula[ncel].MC;

	celula[ncel].MliqiniBuf=celula[ncel].Mliqini;
	celula[ncel-1].MliqiniRBuf=celula[ncel].Mliqini;

    int fim=ncel-1;

    fontemassPRBuf=celula[fim+1].fontemassLR;

    fontemassGRBuf=celula[fim+1].fontemassGR;

}
///////////////////////////// alteracao8 //////////////////////////////////////////////

void SProdVap::renovaTemp() {
  for (int i = 0; i <= ncel; i++) {
    double rhol0;
    double boL;
    double rsL;
    if (i != 0 && i != ncel) {

    	if((*vg1dSP).lixo5>=7059.9 && i==ncel-1){
    		int para;
    		para=0;
    	}

      renovaFonte(i);
      celula[i + 1].fontemassLL = celula[i].fontemassLR;
      celula[i + 1].fontemassGL = celula[i].fontemassGR;
      //celula[i].tempini=celula[i].temp;
      //celula[i+1].tempL= celula[i-1].tempR= celula[i].temp;
      double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
      double razdxL = celula[i - 1].dx / (celula[i - 1].dx + celula[i - 1].dxL);
      double pmed = razdx * celula[i].pres + (1 - razdx) * celula[i].presL;
      double tmed = razdx * celula[i].temp + (1 - razdx) * celula[i - 1].temp;
      double tmedL = razdxL * celula[i].tempL + (1 - razdxL) * celula[i - 1].tempL;
      tmed = celula[i - 1].temp;
      if (celula[i].VTemper < 0.) tmed = celula[i].temp;
      tmedL = celula[i - 1].tempL;
      if (celula[i - 1].VTemper < 0.) tmedL = celula[i - 1].temp;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
      double dx = 0.5 * celula[i].dx;
      double dia = celula[i].duto.a;
      double area = 0.25 * M_PI * dia * dia;
      double si = celula[i].duto.peri;
      double alfmed = celula[i].alf;
      double rhog = celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp);
      double rhol =  celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp);
      double ugsmed = (celula[i].QG) / (area);
      double ulsmed = celula[i].QL / (area);
      double j = ugsmed + ulsmed;
      double ABSjL = (fabsl(celula[i - 1].QG) + fabsl(celula[i - 1].QL)) / celula[i - 1].duto.area;

      double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
      double viscmix = alfmed * celula[i].flui.ViscGas(celula[i].pres, celula[i].temp)
          + (1 - alfmed)
              * (celula[i].flui.ViscLiq(celula[i].pres, celula[i].temp));

      double re1;
      if (celula[i].duto.revest == 0) re1 = celula[i].Rey(celula[i].duto.a, j, rhomix, viscmix);
      else {
        double dhid = 4 * area / si;
        re1 = celula[i].Rey(dhid, j, rhomix, viscmix);
      }
      double f1 = celula[i].fric(re1, celula[i].duto.rug / dia);
      double gradfric = 0.5 * f1 * rhomix * (fabsl(j) * j) * si * dx / area;
      double gradhidro = 9.82 * sinl(celula[i].duto.teta) * rhomix * dx;

      celula[i].presaux = celula[i].pres + (gradfric + gradhidro - celula[i - 1].dpB) / 98066.5;
      celula[i - 1].presauxR = celula[i].presaux;
      celula[i + 1].presauxL = celula[i].presaux;
////////////////////////////////////////////////////////////////////////////////////////////////////////////

      ProVap flue;
      ProVap flud;
      double fwd;
      double fwe;

      if (celula[i].Mliqini < 0.) {
        flud = celula[i].flui;
      } else {
        flud = celula[i - 1].flui;
      }
      if (celula[i - 1].Mliqini < 0) {
        flue = celula[i - 1].flui;

      } else {
        if (i > 1) {
          flue = celula[i - 2].flui;
        } else {
          flue = celula[i - 1].flui;
        }
      }

      //if(celula[i-1].VTemper<0.) tmedL=celula[i-1].temp;
      double rl;
      double rg;
      double betI;

      rl = flud.MasEspLiq(celula[i].presaux, tmed);


      rg = flud.MasEspGas(celula[i].presaux, tmed);

      rhol = rl;
      celula[i].QL = celula[i].Mliqini / rhol;
      celula[i].QG = (celula[i].MC - celula[i].Mliqini) / rg;
      celula[i-1].QLRini=celula[i-1].QLR;// alteracao7
      celula[i - 1].QLR = celula[i].QL;
      celula[i + 1].QLL = celula[i].QL;

      double A1 = celula[i].dutoL.area;

      tmed = celula[i - 1].temp;
      if (celula[i].VTemper < 0.) tmed = celula[i].temp;
      tmedL = celula[i - 1].tempL;
      if (celula[i - 1].VTemper < 0.) tmedL = celula[i - 1].temp;

      celula[i - 1].TMModel = 1;
      if (TransMassModel != 3) {
        if (((celula[i - 1].alf < 0.001 || celula[i - 1].alf > 0.999
            || (celula[i - 1].alf < 0.999)) && ABSjL < 0.1)) celula[i - 1].TMModel = 3;
        else if (celula[i - 1].estadoPig == 1) celula[i - 1].TMModel = 3;
        else if (celula[i - 1].acsr.tipo == 2 || celula[i - 1].acsr.tipo == 3) celula[i - 1].TMModel = 3;
      }


      double rsf;
      double bswf;
      double rhostf;
      double rhogstf;
      double rhoastf;
      celula[i - 1].fontedissolv = 0.;

      celula[i].TMModelL = celula[i - 1].TMModel;
      celula[i - 1].FonteMudaFase = 0.;
      if (celula[i - 1].TMModel == 1) {
    	calcTransMassTermo(i-1);
        celula[i - 1].transmassR = celula[i - 1].FonteMudaFase;
        //celula[i - 1].transmassR /= celula[i - 1].dx;
        //celula[i - 1].transmassR += celula[i - 1].fontedissolv;
        celula[i].transmassL = celula[i - 1].transmassR;
        celula[i].DTransDxR = 0.;
        celula[i].DTransDxL = 0.;
        celula[i].DTransDt1 = 0.;
        celula[i].DTransDt0 = 0.;
        celula[i - 1].CoefDTR = 0.;
        celula[i - 1].CoefDTL = 0.;
      }
      if (celula[i - 1].TMModel == 3) {
        celula[i].transmassL = celula[i - 1].transmassR = 0.;
        celula[i].DTransDxR = 0.;
        celula[i].DTransDxL = 0.;
        celula[i].DTransDt1 = 0.;
        celula[i].DTransDt0 = 0.;
        celula[i - 1].CoefDTR = 0.;
        celula[i - 1].CoefDTL = 0.;
      }
      if (celula[i - 1].transmassR > 0 &&
    		  (celula[i].alfL > (1. - (*vg1dSP).localtiny))) {
        celula[i].transmassL = celula[i - 1].transmassR = -(*vg1dSP).localtiny;
        celula[i].DTransDxR = 0.;
        celula[i].DTransDxL = 0.;
        celula[i].DTransDt1 = 0.;
        celula[i].DTransDt0 = 0.;
        celula[i - 1].CoefDTR = 0.;
        celula[i - 1].CoefDTL = 0.;
      }
      if (celula[i - 1].transmassR < 0 && celula[i].alfL < (*vg1dSP).localtiny) {
        celula[i].transmassL = celula[i - 1].transmassR = (*vg1dSP).localtiny;
        celula[i].DTransDxR = 0.;
        celula[i].DTransDxL = 0.;
        celula[i].DTransDt1 = 0.;
        celula[i].DTransDt0 = 0.;
        celula[i - 1].CoefDTR = 0.;
        celula[i - 1].CoefDTL = 0.;
      }
      rhol0 = rhol;

    } else if (i == 0) {
      //celula[0].tempini=celula[0].temp;
      //celula[1].tempL= celula[0].temp;
		celula[0].QL=0.;
		celula[1].QLL=celula[0].QL;
		celula[0].QG=0.;
		renovaFonte(i);
		celula[0].transmassL=0.;
		celula[1].fontemassLL=celula[0].fontemassLR;
		celula[1].fontemassGL=celula[0].fontemassGR;
		//celula[1].transmassL=celula[0].transmassR=0.;
		rhol0 = celula[i].flui.MasEspLiq(celula[i].pres,celula[i].temp);
		if(arq.ConContEntrada>0){
		    double rhogC = celula[i].flui.MasEspGas(presE,tempE);
		    double rhopC = celula[i].flui.MasEspLiq(presE,tempE);
		    double rholC = rhopC;
			celula[0].QL=celula[0].Mliqini/rholC;
			celula[1].QLL=celula[0].QL;
			celula[0].QG=(celula[0].MC-celula[0].Mliqini)/rhogC;
		}
    } else {

      renovaFonte(i);

      double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
      double pmed = razdx * celula[i].pres + (1 - razdx) * celula[i].presL;
      double tmed = razdx * celula[i].temp + (1 - razdx) * celula[i - 1].temp;

      double rl = celula[i].flui.MasEspLiq(pmed, tmed);
      double rg = celula[i].flui.MasEspGas(pmed, tmed);
      double rhol = rl;
      celula[i].presaux = pmed;
      celula[i].QL = celula[i].Mliqini / rhol;
      celula[i-1].QLRini=celula[i-1].QLR;//alteracao7
      celula[i - 1].QLR = celula[i].QL;
      celula[i].QG = (celula[i].MC - celula[i].Mliqini) / rg;

      celula[i - 1].FonteMudaFase = 0.;
      if (celula[i - 1].TMModel == 1) {
    	calcTransMassTermo(i-1);
        celula[i - 1].transmassR = celula[i - 1].FonteMudaFase;
        //celula[i - 1].transmassR /= celula[i - 1].dx;
        //celula[i - 1].transmassR += celula[i - 1].fontedissolv;
        celula[i].transmassL = celula[i - 1].transmassR;
        celula[i].DTransDxR = 0.;
        celula[i].DTransDxL = 0.;
        celula[i].DTransDt1 = 0.;
        celula[i].DTransDt0 = 0.;
        celula[i - 1].CoefDTR = 0.;
        celula[i - 1].CoefDTL = 0.;
      }
      if (celula[i - 1].TMModel == 3) {
        celula[i].transmassL = celula[i - 1].transmassR = 0.;
        celula[i].DTransDxR = 0.;
        celula[i].DTransDxL = 0.;
        celula[i].DTransDt1 = 0.;
        celula[i].DTransDt0 = 0.;
        celula[i - 1].CoefDTR = 0.;
        celula[i - 1].CoefDTL = 0.;
      }

    }
  }

}

void SProdVap::renovaterm(int aflu) {
  for (int i = 0; i <= ncel; i++) {
    celula[i].presini = celula[i].pres;
    int imax=ncel;
    if(aflu==1) imax=ncel+1;
    if(i==ncel && aflu==1 && (*vg1dSP).lixo5>=1560){
    	int para;
    	para=0;
    }
    if (i != 0 && i != imax) {

      if(i<ncel){
        celula[i - 1].alfR = celula[i + 1].alfL = celula[i].alf;
      }
      else{
          celula[i - 1].alfR =  celula[i].alf;
      }
      int master = 1;
      if (celula[i - 1].acsr.tipo == 5 || celula[i - 1].acsr.tipo == 8) {
    	  //alteracao4
        if ((*celula[i].acsrL).tipo == 5 && (*celula[i].acsrL).chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i - 1].duto.area) master =
            0;//alteracao4
        if ((*celula[i].acsrL).tipo == 8 && fabsl((*celula[i].acsrL).bvol.freq) > 1) master = 0;
      }
      if (master == 1) {
        double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
        double pmed;
        if(i<ncel)pmed= celula[i].presaux;
        else pmed=celula[i].pres;
        double tmed;
        if(i<ncel)tmed= celula[i].temp * razdx + celula[i - 1].temp * (1. - razdx);
        else tmed=tGSup;
        //tmed = celula[i - 1].temp;
        if (celula[i].VTemper < 0.){
        	if(i<ncel)tmed = celula[i].temp;
        	else tmed = tGSup;
        }

        double rl;
        double viscl1;
        double tensup1;
        //if (celula[i].QL < 0.) { testeBeta
        if (celula[i].QG < 0.) {
          rl =  celula[i].flui.MasEspLiq(pmed, tmed);
          viscl1 = celula[i].flui.ViscLiq(pmed, tmed);
          tensup1 = celula[i].flui.TensSupFunc(pmed, tmed) ;
        } else {
          rl = celula[i - 1].flui.MasEspLiq(pmed, tmed);
          viscl1 = celula[i - 1].flui.ViscLiq(pmed, tmed);
          tensup1 = celula[i - 1].flui.TensSupFunc(pmed, tmed);
        }
        double rg;
        double viscg1;
        double amed;
        double hns;
        if (celula[i].QG >= 0) {
          amed = celula[i].dutoL.area;
          hns = 1. - celula[i].alfL;
          rg = celula[i - 1].flui.MasEspGas(pmed, tmed);
          viscg1 = celula[i - 1].flui.ViscGas(pmed, tmed);
        } else {
          rg = celula[i].flui.MasEspGas(pmed, tmed);
          viscg1 = celula[i].flui.ViscGas(pmed, tmed);
          amed = celula[i].duto.area;
          hns = 1. - celula[i].alf;
        }
        double ugs = celula[i].QG / (amed);
        double uls = celula[i].QL / (amed);
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        double dia1 = celula[i].duto.a;
        if (ugs >= 0) dia1 = celula[i - 1].duto.a;

        double rmed = hns * rl + (1 - hns) * rg;
        double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
        double nrey = dia1 * rmed * (fabsl(ugs) / amed + fabsl(uls) / amed) / visc;
        double ang = celula[i].duto.teta;
        if (i >= 2) {
          if (celula[i - 2].acsr.tipo == 5 && celula[i - 2].acsr.chk.AreaGarg <= (1e-3)) {
            if (celula[i].QG >= 0) ang = celula[i].duto.teta;
            else ang = celula[i].dutoR.teta;
          } else {
            if (celula[i].QG >= 0) ang = celula[i].dutoL.teta;
            else ang = celula[i].duto.teta;
          }
        }
        double sinal = 1.;
        if (ang < 0.) sinal = -1.;
        double xc0 = 2.;
        double xud = sinal * 0.0246 * cosl(ang)
            + 1.606 * pow(9.82 * tensup1 * (rl - rg) / (rl * rl), 0.25) * sin(ang);
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        double amedL = celula[i].dutoL.area;
        double razdxL = celula[i - 1].dx / (celula[i - 1].dx + celula[i - 1].dxL);
        double pmedL = celula[i - 1].presaux;
        double tmedL = celula[i - 1].temp * razdxL + celula[i - 1].tempL * (1. - razdxL);
        //double betIL=celula[i-1].betL;
        //if(celula[i-1].QL<0.)betIL=celula[i-1].bet;
        double betIL;
        double rgL = celula[i].flui.MasEspGas(pmedL, tmedL);
        double rlL = (1 - betIL) * celula[i].flui.MasEspLiq(pmedL, tmedL);
        double ugs0 = (celula[i].ML - celula[i].MliqiniL) / (rgL * amedL);
        double uls0 = (celula[i].MliqiniL) / (rlL * amedL);

        double amedR = celula[i].dutoR.area;
        double razdxR = celula[i ].dxR / (celula[i ].dxR + celula[i ].dx);
        double pmedR= celula[i].presauxR;
        double tmedR = celula[i].temp * razdxR + celula[i].tempL * (1. - razdxR);
        double rgR = celula[i].flui.MasEspGas(pmedR, tmedR);
        double rlR = celula[i].flui.MasEspLiq(pmedR, tmedR);
        double ugs1 = (celula[i].MR - celula[i].MliqiniR) / (rgR * amedR);
        double uls1 = (celula[i].MliqiniR) / (rlR * amedR);

        int bif = 1;

        if((*vg1dSP).lixo5>29900){
        	int para;
        	para=0;
        }

        if (celula[i - 1].alfPigD <= (*vg1dSP).localtiny && celula[i].alfPigE <= (*vg1dSP).localtiny
            && celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          //celula[i].transic=0;
          bif = 0;
          celula[i].c0 =1+0*xc0;
          celula[i].ud = 0*xud;
          celula[i].arranjo = 0;
        }
        else if (celula[i - 1].alfPigD >= (1. - (*vg1dSP).localtiny) && celula[i].alfPigE >= (1. - (*vg1dSP).localtiny)
            && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
                && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
          celula[i].term1 = 0.;
          celula[i].term2 = 0.;
          //celula[i].transic=1;
          bif = 0;
          celula[i].c0 =1+0*xc0;
          celula[i].ud = 0*xud;
          celula[i].arranjo = 0;
        }
        else if (celula[i - 1].acsr.tipo == 5 && celula[i - 1].acsr.chk.AreaGarg <= (1e-3)) {
          celula[i].term1 = 0.;
          celula[i].term2 = 0.;
          bif = 0;
          celula[i].c0 = 1+0*xc0;
          celula[i].ud = 0*xud;
          celula[i].arranjo = 0;
        }

        else if (ugs >= 0 && celula[i - 1].alfPigD <= (*vg1dSP).localtiny
            && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          //celula[i].transic=2;
          bif = 0;
          celula[i].c0 = 1+0*xc0;
          celula[i].ud = 0*xud;
          celula[i].arranjo = 0;//alteracao4
          if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - (*vg1dSP).localtiny) && uls < 0 && uls0 < 0 && celula[i].duto.teta >0) {
            celula[i].term1 = 0.;
            celula[i].term2 = 0.;
            //celula[i].transic=2;
            bif = 0;
          }//alteracao4
          if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - 10*(*vg1dSP).localtiny) && celula[i].duto.teta <0) {
            celula[i].term1 = 0.;
            celula[i].term2 = 0.;
            //celula[i].transic=2;
            bif = 1;
          }
          if (fabsl(ugs) <= 1e-15 && celula[i].alf >= celula[i + 1].alf && ugs1 < 0) {
            bif = 1;
          }
        }
        else if (ugs <= 0 && celula[i].alfPigE <= (*vg1dSP).localtiny
            && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          //celula[i].transic=3;
          bif = 0;
          celula[i].c0 = 1+0*xc0;
          celula[i].ud = 0*xud;
          celula[i].arranjo = 0;
          if (fabsl(ugs) <= 1e-15 && celula[i - 1].alfPigD > (1. - (*vg1dSP).localtiny) && uls > 0 && uls1 > 0 ) {
            celula[i].term1 = 0.;
            celula[i].term2 = 0.;
            //celula[i].transic=3;
            bif = 0;
          }
          if (fabsl(ugs) <= 1e-15 && celula[i].alfL >= celula[i - 1].alfL && ugs0 > 0) {
            bif = 1;
          }
        }
        else if (uls >= 0 && celula[i - 1].alfPigD >= 1. - 1*(*vg1dSP).localtiny
            && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
                && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
          celula[i].term1 = 0.;
          celula[i].term2 = 0.;
          //celula[i].transic=4;
          bif = 0;
          celula[i].c0 = 1+0*xc0;
          celula[i].ud = 0*xud;
          celula[i].arranjo = 0;
          if (fabsl(uls) <= 1e-15 && celula[i].alfPigE < (*vg1dSP).localtiny && uls1 < 0) {
            celula[i].term1 = 1.;
            celula[i].term2 = 0.;
            //celula[i].transic=4;
            bif = 0;
          } else if (fabsl(uls1) < (*vg1dSP).localtiny*1e-5) {
            if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].alfPigE < (*vg1dSP).localtiny && celula[i].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
              celula[i].term1 = 1.;
              celula[i].term2 = 0.;
              bif = 0;
            }
          }
          else if (fabsl(uls) < 1e-15  && uls1 < 0
              && ((celula[i].alfPigE <=( 1-10*(*vg1dSP).localtiny+.0 * celula[i ].alfPigER) &&
            		  celula[i].alfPigER < 1-1*(*vg1dSP).localtiny)
                  || celula[i].alfPigE <= 0.7)) bif = 1;

          //else if (fabsl(uls) < 1e-15 && celula[i].duto.teta > 0 && celula[i].alfPigE <= 0.999) bif = 1;
        }
        else if (uls <= 0 && celula[i].alfPigE >= 1. - (*vg1dSP).localtiny
            && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
                && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
          celula[i].term1 = 0.;
          celula[i].term2 = 0.;
          //celula[i].transic=5;
          bif = 0;
          celula[i].c0 = 1+0*xc0;
          celula[i].ud = 0*xud;
          celula[i].arranjo = 0;
          if (fabsl(uls) <= (*vg1dSP).localtiny*1e-5 && celula[i - 1].alfPigD < (*vg1dSP).localtiny && uls0 > 0) {
            celula[i].term1 = 1.;
            celula[i].term2 = 0.;
            //celula[i].transic=5;
            bif = 0;
          }

          if (fabsl(uls0) < (*vg1dSP).localtiny*1e-5) {
            if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i - 1].alfPigD < (*vg1dSP).localtiny
                && celula[i - 1].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
              celula[i].term1 = 1.;
              celula[i].term2 = 0.;
              bif = 0;
            }
          } else if (i>1 && fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && uls0 > 0//alteracao4
              && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 2].alfPigD && celula[i - 2].alfPigD < 0.99)
                  || celula[i - 1].alfPigD < 0.7)) bif = 1;
          else if (i>1 && fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && uls0 > 0//alteracao4
              && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 2].alfPigD && celula[i - 2].alfPigD < 0.99)
                  || celula[i - 1].alfPigD < 0.7)) bif = 1;
          else {
            if ( fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && uls0 > 0
                && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 1].alfL) || celula[i - 1].alfPigD < 0.7)) bif = 1;
            else if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && uls0 > 0
                && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 1].alfL) || celula[i - 1].alfPigD < 0.7)) bif = 1;
          }

        }

       // if(i==ncel && ugs<=0 && celula[i ].alf>(*vg1dSP).localtiny && celula[i].alf<1.-(*vg1dSP).localtiny) bif=1;

        if (bif == 1) {

          double c0;
          double ud;
          double alfmed;

          //if(aflu==0 || i<ncel){

            alfmed = celula[i - 1].alfPigD;
            if (ugs < 0) alfmed = celula[i].alfPigE;
            c0 = 1.2;
            double dmed = celula[i].duto.a;
            if (celula[i].MC >= 0) dmed = celula[i].dutoL.a;
            double sinal=1.;//alteracao4
            if(celula[i].duto.teta<0.)sinal=-1.;//alteracao4
            ud = sinal * 0.32 * sqrtl(9.82 * dmed);
            if(fabsl(rgR)/rlR>0.9){
          	  c0=1.;
          	  ud=0.;
            }
            CalcC0Ud(i, c0, ud);
            /*else if (((alfmed > (*vg1dSP).localtiny) && (alfmed < (1. - (*vg1dSP).localtiny))) || i==1) CalcC0Ud(i, c0, ud);
            else {
              c0 = 1.;
              ud = sinal*0.001;//alteracao4
              alfmed=0.5*(celula[i - 1].alfPigD+celula[i].alfPigE);//alteracao4
            }*/
          //}
          //else{
        //	  c0=1.;
        	//  ud=0.;
          //}
          celula[i].c0 = c0;
          celula[i].ud = ud;
          double num = (1. - alfmed * c0);
          double den = 1 + c0 * alfmed * ((rg / rl) - 1.);
          celula[i].term1 = num / den;
          celula[i].term2 = (-amed * alfmed * rg * ud) / den;
          if (celula[i - 1].acsr.tipo == 5
              && celula[i - 1].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i - 1].duto.area) {//alteracao4
            celula[i].term1 = 0.;
            celula[i].term2 = 0.;
          }

        }
        celula[i].term1L = celula[i - 1].term1;
        celula[i].term2L = celula[i - 1].term2;
        celula[i - 1].term1R = celula[i].term1;
        celula[i - 1].term2R = celula[i].term2;
      } else {
        celula[i].c0 = 1.;
        celula[i].ud = 0.;
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
        //celula[i].transic=-1;
        celula[i].term1L = celula[i - 1].term1;
        celula[i].term2L = celula[i - 1].term2;
        celula[i - 1].term1R = celula[i].term1;
        celula[i - 1].term2R = celula[i].term2;
      }
    } else if (i == 0) {
    	if(arq.ConContEntrada==0){
         celula[1].alfL = celula[0].alf;
         celula[0].alfL = celula[0].alf;
         celula[0].term1 = 0.;
         celula[0].term2 = 0.;
         celula[0].term1L = 0.;
         celula[0].term2L = 0.;
         //celula[i].transic=-2;
    	}
    	else{
    		if(titE<1){
    			int para;
    			para=0;
    		}

            double razdx =0.5;
            double pmed;
            pmed=presE;

            double tmed;
            if (celula[0].QL < 0.)
            	tmed = celula[i].temp;
            else tmed = tempE;

			  double rg=celula[0].flui.MasEspGas(presE,tempE);
			  double rl=celula[0].flui.MasEspLiq(presE,tempE);


			  double rlmist=rl;

            	  alfE=(-titE*rlmist/(titE*rg-rg-titE*rlmist))/(celula[0].c0);


            double betI;
            double viscl1;
            double tensup1;
            //if (celula[i].QL < 0.) { testeBeta
            if (celula[i].QG < 0.) {
              rl = celula[i].flui.MasEspLiq(pmed, tmed);
              viscl1 = celula[i].flui.ViscLiq(pmed, tmed);
              tensup1 =  celula[i].flui.TensSupFunc(pmed, tmed);
            } else {
              rl =  celula[i ].flui.MasEspLiq(pmed, tmed);
              viscl1 = celula[i ].flui.ViscLiq(pmed, tmed);
              tensup1 = celula[i ].flui.TensSupFunc(pmed, tmed);
            }

            double viscg1;
            double amed;
            double hns;
            if (celula[i].QG >= 0) {
              amed = celula[i].dutoL.area;
              rg = celula[i ].flui.MasEspGas(pmed, tmed);
              viscg1 = celula[i ].flui.ViscGas(pmed, tmed);
              hns = 1. - alfE;
            } else {
              rg = celula[i ].flui.MasEspGas(pmed, tmed);
              viscg1 = celula[i].flui.ViscGas(pmed, tmed);
              amed = celula[i].duto.area;
              hns = 1. - celula[i].alf;
            }
            double ugs = celula[i].QG / (amed);
            double uls = celula[i].QL / (amed);
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            double dia1 = celula[i].duto.a;

            double rmed = hns * rl + (1 - hns) * rg;
            double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
            double nrey = dia1 * rmed * (fabsl(ugs) / amed + fabsl(uls) / amed) / visc;
            double ang = celula[i].duto.teta;
            double sinal = 1.;
            if (ang < 0.) sinal = -1.;
            double xc0 = 2.;
            double xud = sinal * 0.0246 * cosl(ang)
                + 1.606 * pow(9.82 * tensup1 * (rl - rg) / (rl * rl), 0.25) * sin(ang);
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            double amedR = celula[i].dutoR.area;
            double razdxR = celula[i ].dxR / (celula[i ].dxR + celula[i ].dx);
            double pmedR= celula[i].presauxR;
            double tmedR = celula[i].temp * razdxR + celula[i].tempL * (1. - razdxR);


            double rgR = celula[i].flui.MasEspGas(pmedR, tmedR);
            double rlR =  celula[i].flui.MasEspLiq(pmedR, tmedR);
            double ugs1 = (celula[i].MR - celula[i].MliqiniR) / (rgR * amedR);
            double uls1 = (celula[i].MliqiniR) / (rlR * amedR);

            int bif = 1;

            if((*vg1dSP).lixo5>29900){
            	int para;
            	para=0;
            }

            if (alfE<(*vg1dSP).localtiny && celula[i].alfPigE <= (*vg1dSP).localtiny
                && celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5) {
              celula[i].term1 = 1.;
              celula[i].term2 = 0.;
              //celula[i].transic=0;
              bif = 0;
              celula[i].c0 =1+0*xc0;
              celula[i].ud = 0*xud;
              celula[i].arranjo = 0;
            }
            else if (alfE >= (1. - (*vg1dSP).localtiny) && celula[i].alfPigE >= (1. - (*vg1dSP).localtiny)
                && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
                    && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
              celula[i].term1 = 0.;
              celula[i].term2 = 0.;
              //celula[i].transic=1;
              bif = 0;
              celula[i].c0 =1+0*xc0;
              celula[i].ud = 0*xud;
              celula[i].arranjo = 0;
            }
            else if (ugs >= 0 && alfE <= (*vg1dSP).localtiny
                && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
              celula[i].term1 = 1.;
              celula[i].term2 = 0.;
              //celula[i].transic=2;
              bif = 0;
              celula[i].c0 = 1+0*xc0;
              celula[i].ud = 0*xud;
              celula[i].arranjo = 0;//alteracao4
              if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - (*vg1dSP).localtiny) && uls < 0 && celula[i].duto.teta >0) {
                celula[i].term1 = 0.;
                celula[i].term2 = 0.;
                //celula[i].transic=2;
                bif = 0;
              }//alteracao4
              if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - 1*(*vg1dSP).localtiny) && celula[i].duto.teta <0 && uls>0) {
                celula[i].term1 = 0.;
                celula[i].term2 = 0.;
                //celula[i].transic=2;
                bif = 1;
              }
              if (fabsl(ugs) <= 1e-15 && celula[i].alf >= alfE && uls < 0) {
                bif = 1;
              }
            }
            else if (ugs <= 0 && celula[i].alfPigE <= (*vg1dSP).localtiny
                && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
              celula[i].term1 = 1.;
              celula[i].term2 = 0.;
              //celula[i].transic=3;
              bif = 0;
              celula[i].c0 = 1+0*xc0;
              celula[i].ud = 0*xud;
              celula[i].arranjo = 0;
              if (fabsl(ugs) <= 1e-15 && alfE > (1. - (*vg1dSP).localtiny) && uls > 0) {
                celula[i].term1 = 0.;
                celula[i].term2 = 0.;
                //celula[i].transic=3;
                bif = 0;
              }
              if (fabsl(ugs) <= 1e-15 && alfE >(*vg1dSP).localtiny && uls > 0) {
                bif = 1;
              }
            }
            else if (uls >= 0 && alfE >= 1. - (*vg1dSP).localtiny
                && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
                    && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
              celula[i].term1 = 0.;
              celula[i].term2 = 0.;
              //celula[i].transic=4;
              bif = 0;
              celula[i].c0 = 1+0*xc0;
              celula[i].ud = 0*xud;
              celula[i].arranjo = 0;
              if (fabsl(uls) <= 1e-15 && celula[i].alfPigE < (*vg1dSP).localtiny && uls1 < 0) {
                celula[i].term1 = 1.;
                celula[i].term2 = 0.;
                //celula[i].transic=4;
                bif = 0;
              } else if (fabsl(uls1) < (*vg1dSP).localtiny*1e-5) {
                if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].alfPigE < (*vg1dSP).localtiny && celula[i].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
                  celula[i].term1 = 1.;
                  celula[i].term2 = 0.;
                  bif = 0;
                }
              }
              else if (fabsl(uls) < 1e-15  && uls1 < 0
                  && ((celula[i].alfPigE <=( 1-1*(*vg1dSP).localtiny+.0 * celula[i ].alfPigER) && celula[i].alfPigER < 1-1*(*vg1dSP).localtiny)
                      || celula[i].alfPigE <= 0.7)) bif = 1;

              //else if (fabsl(uls) < 1e-15 && celula[i].duto.teta > 0 && celula[i].alfPigE <= 0.999) bif = 1;
            }
            else if (uls <= 0 && celula[i].alfPigE >= 1. - (*vg1dSP).localtiny
                && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
                    && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
              celula[i].term1 = 0.;
              celula[i].term2 = 0.;
              //celula[i].transic=5;
              bif = 0;
              celula[i].c0 = 1+0*xc0;
              celula[i].ud = 0*xud;
              celula[i].arranjo = 0;
              if (fabsl(uls) <= (*vg1dSP).localtiny*1e-5 && alfE < (*vg1dSP).localtiny && uls1 < 0) {
                celula[i].term1 = 1.;
                celula[i].term2 = 0.;
                //celula[i].transic=5;
                bif = 0;
              }

              if (fabsl(uls1) < (*vg1dSP).localtiny*1e-5) {
                if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && alfE < (*vg1dSP).localtiny) {
                  celula[i].term1 = 1.;
                  celula[i].term2 = 0.;
                  bif = 0;
                }
              }
              else {
                if ( fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && ugs < 0
                    && ( alfE < 0.7)) bif = 1;
                else if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && ugs < 0
                    && (alfE < 0.7)) bif = 1;
              }

            }
            if(uls>0 && fabs(ugs)<=(*vg1dSP).localtiny*1e-5 && alfE>(*vg1dSP).localtiny && alfE<1-(*vg1dSP).localtiny) bif=1;
            if(ugs>0 && fabs(uls)<=(*vg1dSP).localtiny*1e-5 && alfE>(*vg1dSP).localtiny && alfE<1-(*vg1dSP).localtiny) bif=1;
            if(uls>=0 &&  alfE>1-(*vg1dSP).localtiny){
                celula[i].term1 = 0.;
                celula[i].term2 = 0.;
            	bif=0;
            }
            if(ugs>=0 && alfE<(*vg1dSP).localtiny){
                celula[i].term1 = 1.;
                celula[i].term2 = 0.;
            	bif=0;
            }

            if (bif == 1) {

              double c0;
              double ud;
              double alfmed;

              //if(aflu==0 || i<ncel){

                alfmed = alfE;
                if (ugs < 0) alfmed = celula[i].alfPigE;
                c0 = 1.2;
                double dmed = celula[i].duto.a;
                if (celula[i].MC >= 0) dmed = celula[i].dutoL.a;
                double sinal=1.;//alteracao4
                if(celula[i].duto.teta<0.)sinal=-1.;//alteracao4
                ud = sinal * 0.32 * sqrtl(9.82 * dmed);
                if(fabsl(rgR)/rlR>0.9){
              	  c0=1.;
              	  ud=0.;
                }
                CalcC0UdIni(i, c0, ud);
                /*else if (((alfmed > (*vg1dSP).localtiny) && (alfmed < (1. - (*vg1dSP).localtiny))) || i==1) CalcC0Ud(i, c0, ud);
                else {
                  c0 = 1.;
                  ud = sinal*0.001;//alteracao4
                  alfmed=0.5*(celula[i - 1].alfPigD+celula[i].alfPigE);//alteracao4
                }*/
              //}
              //else{
            //	  c0=1.;
            	//  ud=0.;
              //}
              celula[i].c0 = c0;
              celula[i].ud = ud;
              double num = (1. - alfmed * c0);
              double den = 1 + c0 * alfmed * ((rg / rl) - 1.);
              celula[i].term1 = num / den;
              celula[i].term2 = (-amed * alfmed * rg * ud) / den;

            }
            //celula[i].term1 = (1-titE);
            //celula[i].term2 = 0.;
            celula[1].term1L = celula[i ].term1;
            celula[1].term2L = celula[i].term2;


            celula[1].alfL = celula[0].alf;
            celula[0].alfL = alfE;

    	}

    } else if(aflu==0){
        //celula[ncel].alf=celula[ncel-1].alf;
        celula[ncel - 1].alfR = celula[ncel].alf;
        celula[ncel].alfR = celula[ncel].alf;

        double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
        double pmed = celula[i].presaux;
        double tmed = celula[i].temp * razdx + celula[i - 1].temp * (1. - razdx);

        double rg = celula[i].flui.MasEspGas(pmed, tmed);
        double rl =  celula[i].flui.MasEspLiq(pmed, tmed);
        double amed = celula[i].duto.area;
        if (celula[i].MC >= 0) amed = celula[i].dutoL.area;
        double ugs = celula[i].QG / (amed);
        double uls = celula[i].QL / (amed);

        double amedL = celula[i].dutoL.area;
        double razdxL = celula[i - 1].dx / (celula[i - 1].dx + celula[i - 1].dxL);
        double pmedL = celula[i - 1].presaux;
        double tmedL = celula[i - 1].temp * razdxL + celula[i - 1].temp * (1. - razdxL);

        double rgL = celula[i].flui.MasEspGas(pmedL, tmedL);
        double rlL = celula[i].flui.MasEspLiq(pmedL, tmedL);
        double ugs0 = (celula[i].ML - celula[i].MliqiniL) / (rgL * amedL);
        double uls0 = (celula[i].MliqiniL) / (rlL * amedL);

        double amedR = celula[i].dutoR.area;
        double razdxR = celula[i].dxR / (celula[i].dxR + celula[i].dx);
        double pmedR = celula[i].pres;
        double tmedR = celula[i].temp * razdxR + celula[i].temp * (1. - razdxR);
        double rgR = celula[i].flui.MasEspGas(pmedR, tmedR);
        double rlR = celula[i].flui.MasEspLiq(pmedR, tmedR);
        double ugs1 = (celula[i].MR - celula[i].MliqiniR) / (rgR * amedR);
        double uls1 = (celula[i].MliqiniR) / (rlR * amedR);

        int bif = 1;

        if (celula[i].alfL <= (*vg1dSP).localtiny && celula[i].alf <= (*vg1dSP).localtiny && celula[i].fontemassGL <= 0
          && celula[i].fontemassGR <= 0) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          //celula[i].transic=0;
          bif = 0;
          celula[i].c0 = 1. - (*vg1dSP).localtiny;
          celula[i].ud = 0.;
          celula[i].arranjo = 0;
        } else if (celula[i].alfL >= (1. - (*vg1dSP).localtiny) && celula[i].alf >= (1. - (*vg1dSP).localtiny) && celula[i].fontemassLL <= 0
          && celula[i].fontemassLR <= 0) {
          celula[i].term1 = 0.;
          celula[i].term2 = 0.;
          //celula[i].transic=1;
          bif = 0;
          celula[i].c0 = 1. - (*vg1dSP).localtiny;
          celula[i].ud = 0.;
          celula[i].arranjo = 0;
        } else if (ugs > 0 && celula[i].alfL <= (*vg1dSP).localtiny
          && (celula[i].fontemassGL <= 0. && celula[i].fontemassGR <= 0.)) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          //celula[i].transic=2;
          bif = 0;
          celula[i].c0 = 1. - (*vg1dSP).localtiny;
          celula[i].ud = 0.;
          celula[i].arranjo = 0;
          if (fabsl(ugs) <= 1e-15 && celula[i].alf > (1. - (*vg1dSP).localtiny) && uls < 0 && uls0 < 0) {
            celula[i].term1 = 0.;
            celula[i].term2 = 0.;
            //celula[i].transic=2;
            bif = 0;
          }
        } else if (ugs < 0 && celula[i].alf <= (*vg1dSP).localtiny
          && (celula[i].fontemassGL <= 0. && celula[i].fontemassGR <= 0.)) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          //celula[i].transic=3;
          bif = 0;
          celula[i].c0 = 1. - (*vg1dSP).localtiny;
          celula[i].ud = 0.;
          celula[i].arranjo = 0;
          if (fabsl(ugs) <= 1e-15 && celula[i].alfL > (1. - (*vg1dSP).localtiny) && uls > 0 && uls1 > 0) {
            celula[i].term1 = 0.;
            celula[i].term2 = 0.;
            //celula[i].transic=3;
            bif = 0;
          }
        }
        else if (uls >= 0 && celula[i - 1].alf >= 1. - (*vg1dSP).localtiny
           && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
               && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
           celula[i].term1 = 0.;
           celula[i].term2 = 0.;
           //celula[i].transic=4;
           bif = 0;
           celula[i].c0 = 1;
           celula[i].ud = 0;
           celula[i].arranjo = 0;
           if (fabsl(uls) <= 1e-15 && celula[i].alf < (*vg1dSP).localtiny && uls1 < 0) {
             celula[i].term1 = 1.;
             celula[i].term2 = 0.;
             //celula[i].transic=4;
             bif = 0;
           } else if (fabsl(uls1) < (*vg1dSP).localtiny*1e-5) {
             if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].alf < (*vg1dSP).localtiny && celula[i].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
               celula[i].term1 = 1.;
               celula[i].term2 = 0.;
               bif = 0;
             }
           }
           else if (fabsl(uls) < 1e-15 && uls1 < 0
             && celula[i].alf <=( 1-1*(*vg1dSP).localtiny) ) bif = 1;
         }

        else if (uls <= 0 && celula[i].alf >= 1. - (*vg1dSP).localtiny
           && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
               && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
           celula[i].term1 = 0.;
           celula[i].term2 = 0.;
           //celula[i].transic=5;
           bif = 0;
           celula[i].c0 = 1;
           celula[i].ud = 0;
           celula[i].arranjo = 0;
           if (fabsl(uls) <= (*vg1dSP).localtiny*1e-5 && celula[i - 1].alf < (*vg1dSP).localtiny && uls0 > 0) {
             celula[i].term1 = 1.;
             celula[i].term2 = 0.;
             //celula[i].transic=5;
             bif = 0;
           }
           if (fabsl(uls0) < (*vg1dSP).localtiny*1e-5) {
             if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i - 1].alf < (*vg1dSP).localtiny
               && celula[i - 1].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
               celula[i].term1 = 1.;
               celula[i].term2 = 0.;
               bif = 0;
             }
           } else if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && uls0 > 0
             && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 2].alfPigD && celula[i - 2].alfPigD < 0.99)
                 || celula[i - 1].alfPigD < 0.7)) bif = 1;
           else if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && uls0 > 0
             && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 2].alfPigD && celula[i - 2].alfPigD < 0.99)
                 || celula[i - 1].alfPigD < 0.7)) bif = 1;
           else {
             if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && uls0 > 0
               && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 1].alfL) || celula[i - 1].alfPigD < 0.7)) bif = 1;
             else if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && uls0 > 0
               && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 1].alfL) || celula[i - 1].alfPigD < 0.7)) bif = 1;
           }

         }

        else if (bif == 1) {
          double alfmed;
          alfmed = celula[i].alfL;
          double c0 = 1.2;
          double dmed = celula[i].duto.a;
          if (celula[i].MC >= 0) dmed = celula[i].dutoL.a;
         // CalcC0Ud(i, c0, ud);
          double sinal=1.;//alteracao4
          if(celula[i].duto.teta<0.)sinal=1.;//alteracao4
          double ud = sinal * 0.32 * sqrtl(9.82 * dmed);
          if(fabsl(rgR)/rlR>0.9){
        	  c0=1.;
        	  ud=0.;
          }
         /* else if (((alfmed > (*vg1dSP).localtiny) && (alfmed < (1. - (*vg1dSP).localtiny)))) CalcC0Ud(i, c0, ud);
          else {
            c0 = 1.;
            ud = sinal*0.001;//alteracao4
          }*/
          CalcC0Ud(i, c0, ud);
          celula[i].c0 = c0;
          celula[i].ud = ud;
          double num = (1. - alfmed * c0);
          double den = 1. + alfmed * (rg / rl) * c0 - alfmed * c0;
          celula[i].term1 = num / den;
          celula[i].term2 = (-amed * alfmed * rg * ud) / den;
          //celula[i].transic=-1;
        }
        celula[i].term1L = celula[i - 1].term1;
        celula[i].term2L = celula[i - 1].term2;
        celula[i - 1].term1R = celula[i].term1;
        celula[i - 1].term2R = celula[i].term2;
    }
  }
}

void SProdVap::renovatermAfluFim(){

	int i=ncel;

    double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
    //double pmed = razdx * celula[i].presBuf + (1 - razdx) * celula[i].presLBuf;
    double pmed = celula[i].presBuf ;
    double tmed = tGSup;
    tmed = celula[i - 1].temp;
    if (celula[i].VTemper < 0.) tmed = tGSup;

    double rl;
    double viscl1;
    double tensup1;
    //if (celula[i].MliqiniBuf < 0.) { testeBeta
    if (celula[i].MCBuf-celula[i].MliqiniBuf < 0.) {

      rl =  celula[i].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[i].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[i].flui.TensSupFunc(pmed, tmed);
    } else {

      rl = celula[i - 1].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[i - 1].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[i - 1].flui.TensSupFunc(pmed, tmed);
    }
    double rg;
    double viscg1;
    double amed;
    double hns;
    if ( (celula[i].MCBuf - celula[i].MliqiniBuf) >= 0) {
      amed = celula[i].dutoL.area;
      hns = 1. - celula[i].alfL;
      rg = celula[i - 1].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[i - 1].flui.ViscGas(pmed, tmed);
    } else {
      rg = celula[i].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[i].flui.ViscGas(pmed, tmed);
      amed = celula[i].duto.area;
      hns = 1. - celula[i].alf;
    }
    double ugs = (celula[i].MCBuf - celula[i].MliqiniBuf) / (rg * amed);
    double uls = (celula[i].MliqiniBuf) / (rl * amed);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double dia1 = celula[i].duto.a;
    if (ugs >= 0) dia1 = celula[i - 1].duto.a;

    double rmed = hns * rl + (1 - hns) * rg;
    double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
    double nrey = dia1 * rmed * (fabsl(ugs) / amed + fabsl(uls) / amed) / visc;
    double ang = celula[i].duto.teta;
    if (i >= 2) {
      if (celula[i - 2].acsr.tipo == 5 && celula[i - 2].acsr.chk.AreaGarg <= (1e-3)) {
        if ((celula[i].MCBuf - celula[i].MliqiniBuf) >= 0) ang = celula[i].duto.teta;
        else ang = celula[i].dutoR.teta;
      } else {
        if ((celula[i].MCBuf - celula[i].MliqiniBuf) >= 0) ang = celula[i].dutoL.teta;
        else ang = celula[i].duto.teta;
      }
    }
    double sinal = 1.;
    if (ang < 0.) sinal = -1.;
    double xc0 = 2.;
    double xud = sinal * 0.0246 * cosl(ang)
        + 1.606 * pow(9.82 * tensup1 * (rl - rg) / (rl * rl), 0.25) * sin(ang);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    double amedL = celula[i].dutoL.area;
    double razdxL = celula[i - 1].dx / (celula[i - 1].dx + celula[i - 1].dxL);
    double pmedL = celula[i - 1].presaux;
    double tmedL = celula[i - 1].temp * razdxL + celula[i - 1].tempL * (1. - razdxL);

    double rgL = celula[i].flui.MasEspGas(pmedL, tmedL);
    double rlL = celula[i].flui.MasEspLiq(pmedL, tmedL);
    double ugs0 = (celula[i].MLBuf - celula[i].MliqiniLBuf) / (rgL * amedL);
    double uls0 = (celula[i].MliqiniLBuf) / (rlL * amedL);

    double amedR = celula[i].dutoR.area;
    double razdxR = celula[i ].dxR / (celula[i ].dxR + celula[i ].dx);
    double pmedR= celula[i].presRBuf;
    double tmedR = celula[i].temp * razdxR + celula[i].tempL * (1. - razdxR);

    double rgR = celula[i].flui.MasEspGas(pmedR, tmedR);
    double rlR = celula[i].flui.MasEspLiq(pmedR, tmedR);
    double ugs1 = (celula[i].MRBuf - celula[i].MliqiniRBuf) / (rgR * amedR);
    double uls1 = (celula[i].MliqiniRBuf) / (rlR * amedR);

    int bif = 1;

    if((*vg1dSP).lixo5>29900){
    	int para;
    	para=0;
    }

    if (celula[i - 1].alfPigD <= (*vg1dSP).localtiny && celula[i].alfPigE <= (*vg1dSP).localtiny
        && celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5) {
      celula[i].term1 = 1.;
      celula[i].term2 = 0.;
      //celula[i].transic=0;
      bif = 0;
      celula[i].c0 =1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
    }
    else if (celula[i - 1].alfPigD >= (1. - (*vg1dSP).localtiny) && celula[i].alfPigE >= (1. - (*vg1dSP).localtiny)
        && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
            && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 0.;
      celula[i].term2 = 0.;
      //celula[i].transic=1;
      bif = 0;
      celula[i].c0 =1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
    }
    else if (celula[i - 1].acsr.tipo == 5 && celula[i - 1].acsr.chk.AreaGarg <= (1e-3)) {
      celula[i].term1 = 0.;
      celula[i].term2 = 0.;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
    }

    else if (ugs >= 0 && celula[i - 1].alfPigD <= (*vg1dSP).localtiny
        && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 1.;
      celula[i].term2 = 0.;
      //celula[i].transic=2;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;//alteracao4
      if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - (*vg1dSP).localtiny) && uls < 0 && uls0 < 0 && celula[i].duto.teta >0) {
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
        //celula[i].transic=2;
        bif = 0;
      }//alteracao4
      if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - 10*(*vg1dSP).localtiny) && celula[i].duto.teta <0) {
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
        //celula[i].transic=2;
        bif = 1;
      }
      if (fabsl(ugs) <= 1e-15 && celula[i].alf >= celula[i + 1].alf && ugs1 < 0) {
        bif = 1;
      }
    }
    else if (ugs <= 0 && celula[i].alfPigE <= (*vg1dSP).localtiny
        && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 1.;
      celula[i].term2 = 0.;
      //celula[i].transic=3;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
      if (fabsl(ugs) <= 1e-15 && celula[i - 1].alfPigD > (1. - (*vg1dSP).localtiny) && uls > 0 && uls1 > 0 ) {
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
        //celula[i].transic=3;
        bif = 0;
      }
      if (fabsl(ugs) <= 1e-15 && celula[i].alfL >= celula[i - 1].alfL && ugs0 > 0) {
        bif = 1;
      }
    }
    else if (uls >= 0 && celula[i - 1].alfPigD >= 1. - (*vg1dSP).localtiny
        && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
            && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 0.;
      celula[i].term2 = 0.;
      //celula[i].transic=4;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
      if (fabsl(uls) <= 1e-15 && celula[i].alfPigE < (*vg1dSP).localtiny && uls1 < 0) {
        celula[i].term1 = 1.;
        celula[i].term2 = 0.;
        //celula[i].transic=4;
        bif = 0;
      } else if (fabsl(uls1) < (*vg1dSP).localtiny*1e-5) {
        if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].alfPigE < (*vg1dSP).localtiny && celula[i].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          bif = 0;
        }
      }
      else if (fabsl(uls) < 1e-15  && uls1 < 0
          && ((celula[i].alfPigE <=( 1-1*(*vg1dSP).localtiny+.0 * celula[i ].alfPigER) && celula[i].alfPigER < 1-1*(*vg1dSP).localtiny)
              || celula[i].alfPigE <= 0.7)) bif = 1;

      //else if (fabsl(uls) < 1e-15 && celula[i].duto.teta > 0 && celula[i].alfPigE <= 0.999) bif = 1;
    }
    else if (uls <= 0 && celula[i].alfPigE >= 1. - (*vg1dSP).localtiny
        && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
            && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 0.;
      celula[i].term2 = 0.;
      //celula[i].transic=5;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
      if (fabsl(uls) <= (*vg1dSP).localtiny*1e-5 && celula[i - 1].alfPigD < (*vg1dSP).localtiny && uls0 > 0) {
        celula[i].term1 = 1.;
        celula[i].term2 = 0.;
        //celula[i].transic=5;
        bif = 0;
      }

      if (fabsl(uls0) < (*vg1dSP).localtiny*1e-5) {
        if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i - 1].alfPigD < (*vg1dSP).localtiny
            && celula[i - 1].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          bif = 0;
        }
      } else if (i>1 && fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && uls0 > 0//alteracao4
          && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 2].alfPigD && celula[i - 2].alfPigD < 0.99)
              || celula[i - 1].alfPigD < 0.7)) bif = 1;
      else if (i>1 && fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && uls0 > 0//alteracao4
          && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 2].alfPigD && celula[i - 2].alfPigD < 0.99)
              || celula[i - 1].alfPigD < 0.7)) bif = 1;
      else {
        if ( fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && uls0 > 0
            && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 1].alfL) || celula[i - 1].alfPigD < 0.7)) bif = 1;
        else if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && uls0 > 0
            && ((celula[i - 1].alfPigD <= 1.0 * celula[i - 1].alfL) || celula[i - 1].alfPigD < 0.7)) bif = 1;
      }

    }

    if(ugs<0 && celula[i ].alf>(*vg1dSP).localtiny && celula[i].alf<1.-(*vg1dSP).localtiny) bif=1;

    if (bif == 1) {

      double c0;
      double ud;
      double alfmed;

      //if(aflu==0 || i<ncel){

        alfmed = celula[i - 1].alfPigD;
        if (ugs < 0) alfmed = celula[i].alfPigE;
        c0 = 1.2;
        double dmed = celula[i].duto.a;
        if (celula[i].MC >= 0) dmed = celula[i].dutoL.a;
        double sinal=1.;//alteracao4
        if(celula[i].duto.teta<0.)sinal=-1.;//alteracao4
        ud = sinal * 0.32 * sqrtl(9.82 * dmed);
        if(fabsl(rgR)/rlR>0.9){
      	  c0=1.;
      	  ud=0.;
        }
        CalcC0UdBuf(i, c0, ud);
       /* else if (((alfmed > (*vg1dSP).localtiny) && (alfmed < (1. - (*vg1dSP).localtiny))) || i==1) CalcC0UdBuf(i, c0, ud);
        else {
          c0 = 1.;
          ud = sinal*0.001;//alteracao4
          alfmed=0.5*(celula[i - 1].alfPigD+celula[i].alfPigE);//alteracao4
        }*/
      //}
      //else{
    //	  c0=1.;
    	//  ud=0.;
      //}
      celula[i].c0 = c0;
      celula[i].ud = ud;
      double num = (1. - alfmed * c0);
      double den = 1 + c0 * alfmed * ((rg / rl) - 1.);
      celula[i].term1 = num / den;
      celula[i].term2 = (-amed * alfmed * rg * ud) / den;
      if (celula[i - 1].acsr.tipo == 5
          && celula[i - 1].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i - 1].duto.area) {//alteracao4
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
      }

    }
    celula[i].term1L = celula[i - 1].term1;
    celula[i].term2L = celula[i - 1].term2;
    celula[i - 1].term1R = celula[i].term1;
    celula[i - 1].term2R = celula[i].term2;
}

void SProdVap::renovatermColIni() {

	if(titE<1){
		int para;
		para=0;
	}

	int i=0;

    double razdx =0.5;
    double pmed;
    pmed=presE;

    double tmed;
    if (celula[0].MliqiniBuf  < 0.)
    	tmed = celula[i].temp;
    else tmed = tempE;


    double betI;
    double viscl1;
    double tensup1;
    //if (celula[i].QL < 0.) { testeBeta
	double rg=celula[0].flui.MasEspGas(presE,tempE);
	double rl=celula[0].flui.MasEspLiq(presE,tempE);

	double rlmist=rl;
    alfE=(-titE*rlmist/(titE*rg-rg-titE*rlmist))/(celula[0].c0);

    if (celula[i].MCBuf-celula[0].MliqiniBuf < 0.) {

      rl = celula[i].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[i].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[i].flui.TensSupFunc(pmed, tmed);
    } else {

      rl = celula[i ].flui.MasEspLiq(pmed, tmed);
      viscl1 = celula[i ].flui.ViscLiq(pmed, tmed);
      tensup1 = celula[i ].flui.TensSupFunc(pmed, tmed);
    }
    double viscg1;
    double amed;
    double hns;
    if (celula[i].MCBuf-celula[0].MliqiniBuf  >= 0) {
      amed = celula[i].dutoL.area;
      rg = celula[i ].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[i ].flui.ViscGas(pmed, tmed);
      hns = 1. - alfE;
    } else {
      rg = celula[i].flui.MasEspGas(pmed, tmed);
      viscg1 = celula[i].flui.ViscGas(pmed, tmed);
      amed = celula[i].duto.area;
      hns = 1. - celula[i].alf;
    }
    double ugs = (celula[i].MCBuf-celula[i].MliqiniBuf) / (rg*amed);
    double uls = celula[i].MliqiniBuf/ (rl*amed);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double dia1 = celula[i].duto.a;
    if (ugs >= 0) dia1 = celula[i - 1].duto.a;

    double rmed = hns * rl + (1 - hns) * rg;
    double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
    double nrey = dia1 * rmed * (fabsl(ugs) / amed + fabsl(uls) / amed) / visc;
    double ang = celula[i].duto.teta;
    double sinal = 1.;
    if (ang < 0.) sinal = -1.;
    double xc0 = 2.;
    double xud = sinal * 0.0246 * cosl(ang)
        + 1.606 * pow(9.82 * tensup1 * (rl - rg) / (rl * rl), 0.25) * sin(ang);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    double amedR = celula[i].dutoR.area;
    double razdxR = celula[i ].dxR / (celula[i ].dxR + celula[i ].dx);
    double pmedR= celula[i].presRBuf * razdxR + celula[i].presBuf * (1. - razdxR);
    double tmedR = celula[i].temp * razdxR + celula[i].tempL * (1. - razdxR);

    double rgR;
    rgR= celula[i].flui.MasEspGas(pmedR, tmedR);
    double rlR = celula[i].flui.MasEspLiq(pmedR, tmedR);
    double ugs1 = (celula[i].MRBuf - celula[i].MliqiniRBuf)/(rgR*amedR);
    double uls1 = (celula[i].MliqiniRBuf) / (rlR * amedR);

    int bif = 1;

    if((*vg1dSP).lixo5>29900){
    	int para;
    	para=0;
    }

    if (alfE<(*vg1dSP).localtiny && celula[i].alfPigE <= (*vg1dSP).localtiny
        && celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5) {
      celula[i].term1 = 1.;
      celula[i].term2 = 0.;
      //celula[i].transic=0;
      bif = 0;
      celula[i].c0 =1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
    }
    else if (alfE >= (1. - (*vg1dSP).localtiny) && celula[i].alfPigE >= (1. - (*vg1dSP).localtiny)
        && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
            && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 0.;
      celula[i].term2 = 0.;
      //celula[i].transic=1;
      bif = 0;
      celula[i].c0 =1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
    }
    else if (ugs >= 0 && alfE <= (*vg1dSP).localtiny
        && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 1.;
      celula[i].term2 = 0.;
      //celula[i].transic=2;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;//alteracao4
      if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - (*vg1dSP).localtiny) && uls < 0 && celula[i].duto.teta >0) {
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
        //celula[i].transic=2;
        bif = 0;
      }//alteracao4
      if (fabsl(ugs) <= 1e-15 && celula[i].alfPigE > (1. - 1*(*vg1dSP).localtiny) && celula[i].duto.teta <0 && uls>0) {
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
        //celula[i].transic=2;
        bif = 1;
      }
      if (fabsl(ugs) <= 1e-15 && celula[i].alf >= alfE && uls < 0) {
        bif = 1;
      }
    }
    else if (ugs <= 0 && celula[i].alfPigE <= (*vg1dSP).localtiny
        && (celula[i].fontemassGL <= (*vg1dSP).localtiny * 1e-5 && celula[i].fontemassGR <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 1.;
      celula[i].term2 = 0.;
      //celula[i].transic=3;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
      if (fabsl(ugs) <= 1e-15 && alfE > (1. - (*vg1dSP).localtiny) && uls > 0) {
        celula[i].term1 = 0.;
        celula[i].term2 = 0.;
        //celula[i].transic=3;
        bif = 0;
      }
      if (fabsl(ugs) <= 1e-15 && alfE >(*vg1dSP).localtiny && uls > 0) {
        bif = 1;
      }
    }
    else if (uls >= 0 && alfE >= 1. - (*vg1dSP).localtiny
        && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
            && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 0.;
      celula[i].term2 = 0.;
      //celula[i].transic=4;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
      if (fabsl(uls) <= 1e-15 && celula[i].alfPigE < (*vg1dSP).localtiny && uls1 < 0) {
        celula[i].term1 = 1.;
        celula[i].term2 = 0.;
        //celula[i].transic=4;
        bif = 0;
      } else if (fabsl(uls1) < (*vg1dSP).localtiny*1e-5) {
        if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].alfPigE < (*vg1dSP).localtiny && celula[i].fontemassGR >= (*vg1dSP).localtiny * 1e-5) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          bif = 0;
        }
      }
      else if (fabsl(uls) < 1e-15  && uls1 < 0
          && ((celula[i].alfPigE <=( 1-1*(*vg1dSP).localtiny+.0 * celula[i ].alfPigER) && celula[i].alfPigER < 1-1*(*vg1dSP).localtiny)
              || celula[i].alfPigE <= 0.7)) bif = 1;

      //else if (fabsl(uls) < 1e-15 && celula[i].duto.teta > 0 && celula[i].alfPigE <= 0.999) bif = 1;
    }
    else if (uls <= 0 && celula[i].alfPigE >= 1. - (*vg1dSP).localtiny
        && ((celula[i].fontemassLL) <= (*vg1dSP).localtiny * 1e-5
            && (celula[i].fontemassLR) <= (*vg1dSP).localtiny * 1e-5)) {
      celula[i].term1 = 0.;
      celula[i].term2 = 0.;
      //celula[i].transic=5;
      bif = 0;
      celula[i].c0 = 1+0*xc0;
      celula[i].ud = 0*xud;
      celula[i].arranjo = 0;
      if (fabsl(uls) <= (*vg1dSP).localtiny*1e-5 && alfE < (*vg1dSP).localtiny && uls1 < 0) {
        celula[i].term1 = 1.;
        celula[i].term2 = 0.;
        //celula[i].transic=5;
        bif = 0;
      }

      if (fabsl(uls1) < (*vg1dSP).localtiny*1e-5) {
        if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && alfE < (*vg1dSP).localtiny) {
          celula[i].term1 = 1.;
          celula[i].term2 = 0.;
          bif = 0;
        }
      }
      else {
        if ( fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta < 0.95 * M_PI / 2. && ugs < 0
            && ( alfE < 0.7)) bif = 1;
        else if (fabsl(uls) < (*vg1dSP).localtiny*1e-5 && celula[i].duto.teta >= 0.95 * M_PI / 2. && ugs < 0
            && (alfE < 0.7)) bif = 1;
      }

    }
    if(uls>0 && fabs(ugs)<=(*vg1dSP).localtiny*1e-5 && alfE>(*vg1dSP).localtiny && alfE<1-(*vg1dSP).localtiny) bif=1;
    if(ugs>0 && fabs(uls)<=(*vg1dSP).localtiny*1e-5 && alfE>(*vg1dSP).localtiny && alfE<1-(*vg1dSP).localtiny) bif=1;

    if (bif == 1) {

      double c0;
      double ud;
      double alfmed;

      //if(aflu==0 || i<ncel){

        alfmed = alfE;
        if (ugs < 0) alfmed = celula[i].alfPigE;
        c0 = 1.2;
        double dmed = celula[i].duto.a;
        if (celula[i].MC >= 0) dmed = celula[i].dutoL.a;
        double sinal=1.;//alteracao4
        if(celula[i].duto.teta<0.)sinal=-1.;//alteracao4
        ud = sinal * 0.32 * sqrtl(9.82 * dmed);
        if(fabsl(rgR)/rlR>0.9){
      	  c0=1.;
      	  ud=0.;
        }
        CalcC0UdIniBuf(i, c0, ud);
        /*else if (((alfmed > (*vg1dSP).localtiny) && (alfmed < (1. - (*vg1dSP).localtiny))) || i==1) CalcC0Ud(i, c0, ud);
        else {
          c0 = 1.;
          ud = sinal*0.001;//alteracao4
          alfmed=0.5*(celula[i - 1].alfPigD+celula[i].alfPigE);//alteracao4
        }*/
      //}
      //else{
    //	  c0=1.;
    	//  ud=0.;
      //}
      celula[i].c0 = c0;
      celula[i].ud = ud;
      double num = (1. - alfmed * c0);
      double den = 1 + c0 * alfmed * ((rg / rl) - 1.);
      celula[i].term1 = num / den;
      celula[i].term2 = (-amed * alfmed * rg * ud) / den;

    }
    celula[1].term1L = celula[i ].term1;
    celula[1].term2L = celula[i].term2;



}


void SProdVap::calcCCpres(double titRev, double alfRev) {//alteracao7

  double tESup = celula[ncel].temp;
  double alfSup = celula[ncel].alf;

  double masentrada = celula[ncel - 1].MR;
  double massgas = celula[ncel - 1].MR - celula[ncel - 1].MliqiniR;
  double maxSup = 0.;
  double chokemas = 0;

  double tit;
  if (massgas > 0 && celula[ncel - 1].MliqiniR < 0) tit = 1.;
  else if (massgas <= 0 && celula[ncel - 1].MliqiniR >= 0) tit = 0.;
  else if (masentrada < 0){
	  //tit = 1.;
   	if((*vg1dSP).chaverede==0 || noextremo==1)tit=1.;//alteracao7
   	else{//alteracao7
   		tit=titRev;
   		alfSup=alfRev;
   	}
  }
  else tit = fabsl(massgas / masentrada);
  if (tit > 1) tit = 1;
  if (presfim < pGSup){
	  //tit = 1.;
   	if((*vg1dSP).chaverede==0 || noextremo==1)tit=1.;//alteracao7
   	else{//alteracao7
   		tit=titRev;
   		alfSup=alfRev;
   	}
  }
  double rholp = celula[ncel].flui.MasEspLiq(celula[ncel].pres, celula[ncel].temp);
  double rholmix = rholp;
  double romix = alfSup * celula[ncel].flui.MasEspGas(celula[ncel].pres, celula[ncel].temp)
      + (1 - alfSup) * rholmix;

  double delp = (0.5 / 98066.5) * (1 / romix)
      * (1 / (chokeSup.AreaGarg * chokeSup.AreaGarg) - 1 / (celula[ncel - 1].duto.area * celula[ncel - 1].duto.area))
      * masentrada * masentrada;
  double masChk;

  double sinal = 1.;
  double pmon = presfim;

  double ypres = pGSup / presfim;
  if (ypres > 1.) {
    if (arq.chkv == 0) sinal = -1.;
    else sinal = 0.;
    //tit=1.;
	if((*vg1dSP).chaverede==0 || noextremo==1)tit=1.;//alteracao7
	else{//alteracao7
		tit=titRev;
		alfSup=alfRev;
	}
    pmon = pGSup;
    ypres = 1. / ypres;
  }

  masChk = chokeSup.vazmassSachdVap(ypres, pmon, tESup, alfSup, tit);
  maxSup = chokeSup.vazmaxSachdVap(pmon, tESup, alfSup, tit);

  int fluxcri = 1;
  if (fabsl(ypres) > fabsl(chokeSup.razpres)) {
    fluxcri = 0;
    maxSup = masChk;
  }
  if (chokeSup.AreaGarg < (1e-3) * celula[ncel - 1].duto.area) maxSup = 0.;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  double sinal2 = 1.;
  pmon = presfim * 1.0001;
  ypres = pGSup / pmon;
  if (ypres > 1.) {
    sinal2 = -1.;
    tit = 1;
    pmon = pGSup;
    ypres = 1. / ypres;
    if (arq.chkv == 0) sinal2 = -1.;
    else sinal2 = 0.;
  }

  double masChk2 = chokeSup.vazmassSachdVap(ypres, pmon, tESup, alfSup, tit);
  double maxSup2 = chokeSup.vazmaxSachdVap(pmon, tESup, alfSup, tit);

  fluxcri = 1;
  if (fabsl(ypres) > fabsl(chokeSup.razpres)) {
    fluxcri = 0;
    maxSup2 = masChk2;
  }
  if (chokeSup.AreaGarg < (1e-3) * celula[ncel - 1].duto.area) maxSup2 = 0.;
  double dmaxsup = (sinal2 * maxSup2 - sinal * maxSup) / (presfim * 0.0001);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  double masliq;
  double masgas;
  int abertoini = aberto;

  if (chokeSup.AreaGarg > (1e-3) * celula[ncel - 1].duto.area
      && ((((*vg1dSP).lixo5 > tMedMov || arq.perm == 2) && fabsl(presMedMov - pGSup) < 0.5 && fabsl(jMedMov) < 0.5)
          || (((*vg1dSP).lixo5 > tMedMov || arq.perm == 2) && fabsl(presMedMov - pGSup) < (0.05 * pGSup) && fabsl(jMedMov) < 5.
              && delp < 0.1 * pGSup) || (((*vg1dSP).lixo5 > tMedMov || arq.perm == 2) && (pGSup - presMedMov) > 0.01)
          || (fabsl(delp) < 0.1 && chokeSup.AreaGarg > (1e-3) * celula[ncel - 1].duto.area && alfSup > 0.95
              && (presfim - pGSup) < 0.5))) {
    aberto = 1;
    if (abertoini != aberto) tempoaberto = 1;
  } else {
    aberto = 0;
    if (tempoaberto > 60) tempoaberto = 0;
  }
  if (((tit > -0.01 && celula[ncel].alf > 0.01) || chokeSup.AreaGarg < (1e-3) * celula[ncel - 1].duto.area)
      && chokeSup.AreaGarg < 0.6 * celula[ncel - 1].duto.area
      && (aberto == 0 && (tempoaberto == 0 || tempoaberto > 60))) {

    aberto = 0;
    tempoaberto = 0;
    masliq = sinal * maxSup * (1. - tit);
    masgas = sinal * maxSup * tit;

    celula[ncel].DmasschokeG = -(1. - tit) * dmaxsup;
    celula[ncel].DmasschokeL = -tit * (rholp / rholmix) * dmaxsup;

    masChkSup = 1;
    celula[ncel].fontemassLR = -masliq * rholp / rholmix;
    celula[ncel].fontemassGR = -masgas;

  } else {
    if (tempoaberto != 0) tempoaberto++;
    if (chokeSup.AreaGarg >= 0.501 * celula[ncel - 1].duto.area) presfim = pGSup;
    else {
      double sens = 1.;
      if (masentrada < 0. && celula[ncel - 1].MliqiniR <= 0) sens = 0.;
      double delp;
      if (sens > 0 && celula[ncel].QG > 0) delp = chokeSup.dpVap(pGSup, tESup, alfSup, celula[ncel].QG,
          celula[ncel].QL);
      else if (sens > 0 && celula[ncel].QG < 0) delp = chokeSup.dpVap(pGSup, tESup, 0, 0 * celula[ncel].QG,
          celula[ncel].QL);
      else delp = 0.;
      presfim = pGSup + sens * delp / 98066.5;
    }
    masChkSup = 0;
  }

  double mastotchk = fabsl(celula[ncel].fontemassLR + celula[ncel].fontemassGR);


  if(masChkSup==0  && (*vg1dSP).chaverede==1){//alteracao7

		if(arq.chkv==0 || celula[ncel-1].MR>0.)
			sinal=1.;
		else sinal=0.;


	       celula[ncel].fontemassLR=-sinal*(celula[ncel-1].MliqiniR);
	       celula[ncel].fontemassGR=-sinal*(celula[ncel-1].MR-celula[ncel-1].MliqiniR);


  }
}

void  SProdVap::calcCCBuffer(double titRev, double alfRev){//alteracao7


	double tESup=celula[ncel].temp;
	double alfSup=celula[ncel].alf;

    double masentrada=celula[ncel-1].MRBuf;
    double massgas=celula[ncel-1].MRBuf-celula[ncel-1].MliqiniRBuf;
	double maxSup=0.;
	double chokemas=0;

    	double tit;
 	    if(massgas>0 && celula[ncel-1].MliqiniRBuf<0)tit=1.;
 	    else if(massgas<=0 && celula[ncel-1].MliqiniRBuf>=0)tit=0.;
 	    else if(masentrada<0){
 	    	if((*vg1dSP).chaverede==0 || noextremo==1)tit=1.;//alteracao7
 	    	else{//alteracao4
 	    		tit=titRev;
 	    		alfSup=alfRev;
 	    	}
 	    }
 	    else tit=fabsl(massgas/ masentrada);
 	    if(tit>1)tit=1;
 	    if(celula[ncel].presBuf<pGSup){
 	    	if((*vg1dSP).chaverede==0 || noextremo==1)tit=1.;//alteracao7
 	    	else{//alteracao4
 	    		tit=titRev;
 	    		alfSup=alfRev;
 	    	}
 	    }
 	    double rholp=celula[ncel].flui.MasEspLiq(celula[ncel].presBuf,celula[ncel].temp);
 	    double rholmix=rholp;
 	    double romix=alfSup*celula[ncel].flui.MasEspGas(celula[ncel].presBuf,celula[ncel].temp)+(1-alfSup)*rholmix;

 	    double delp=(0.5/98066.5)*(1/romix)*(1/(chokeSup.AreaGarg*chokeSup.AreaGarg)-
 	    		                                  1/(celula[ncel-1].duto.area*celula[ncel-1].duto.area))*masentrada*masentrada;
 	    double masChk;

 	    double sinal=1.;
 	    double pmon=celula[ncel].presBuf;

    	double ypres=pGSup/celula[ncel].presBuf;
    	if(ypres>1.){
    		if(arq.chkv==0) sinal=-1.;
    		else sinal=0.;
 	    	if((*vg1dSP).chaverede==0 || noextremo==1)tit=1.;//alteracao7
 	    	else{//alteracao7
 	    		tit=titRev;
 	    		alfSup=alfRev;
 	    	}
    		pmon=pGSup;
    		ypres=1./ypres;
    	}

     	masChk=chokeSup.vazmassSachdVap(ypres,pmon, tESup,alfSup,tit);
        maxSup=chokeSup.vazmaxSachdVap(pmon, tESup, alfSup,tit);

        int fluxcri=1;
        if(fabsl(ypres)>fabsl(chokeSup.razpres)){
                	fluxcri=0;
                	maxSup=masChk;
        }
        if(chokeSup.AreaGarg<(1e-3)*celula[ncel-1].duto.area ) maxSup=0.;

        double masliq;
        double masgas;
        int abertoini=aberto;

        if(chokeSup.AreaGarg>(1e-3)*celula[ncel-1].duto.area &&
        	((((*vg1dSP).lixo5>tMedMov || arq.perm==2) && fabsl(presMedMov-pGSup)<0.5&&fabsl(jMedMov)<0.5) ||
        	(((*vg1dSP).lixo5>tMedMov || arq.perm==2) && fabsl(presMedMov-pGSup)<(0.05*pGSup)&&fabsl(jMedMov)<5. && delp<0.1*pGSup)||
        	(((*vg1dSP).lixo5>tMedMov || arq.perm==2) && (pGSup-presMedMov)>0.01)||
			(fabsl(delp)<0.1&&chokeSup.AreaGarg>(1e-3)*celula[ncel-1].duto.area
        	&& alfSup>0.95&&(celula[ncel].presBuf-pGSup)<0.5))){
        	aberto=1;
        	if(abertoini!=aberto)tempoaberto=1;
        }
        else{
        	aberto=0;
        	if(tempoaberto>60)tempoaberto=0;
        }
        if(((tit>-0.01 && celula[ncel].alf>0.01)||chokeSup.AreaGarg<(1e-3)*celula[ncel-1].duto.area)&&
        		chokeSup.AreaGarg<0.6*celula[ncel-1].duto.area&&(aberto==0&&(tempoaberto==0||tempoaberto>60))){

    	    masliq=sinal*maxSup*(1.-tit);
            masgas=sinal*maxSup*tit;
        	fontemassPRBuf=-masliq*rholp/rholmix;
        	fontemassGRBuf=-masgas;

        }
   	    if(masChkSup==1 && (*vg1dSP).chaverede==1){//alteracao7
   	 	    celula[ncel].fontemassLR=fontemassPRBuf;
   	 	    celula[ncel].fontemassGR=fontemassGRBuf;

            celula[ncel].DmasschokeG=0.;
            celula[ncel].DmasschokeL=0.;
   	    }
   	    else if(masChkSup==0 && (*vg1dSP).chaverede==1){//alteracao4
    		if(arq.chkv==0 || celula[ncel-1].MRBuf>0.)
    			sinal=1.;
    		else sinal=0.;
            double rhomistBuf=
                  celula[ncel-1].flui.MasEspLiq(celula[ncel].presBuf,celula[ncel].temp);
            double QLbuf=celula[ncel-1].MliqiniRBuf/rhomistBuf;

 		      fontemassPRBuf=-sinal*(celula[ncel-1].MliqiniRBuf);
 		      fontemassGRBuf=-sinal*(celula[ncel-1].MRBuf-celula[ncel-1].MliqiniRBuf);
   	    }

}

void SProdVap::determinaDT() {
  if ((*vg1dSP).lixo5 < (*vg1dSP).localtiny && (*vg1dSP).chaverede==0) {//alteracao7
    arq.imprimeProfile(celula, flut, (*vg1dSP).lixo5, indTramo);
    arq.imprimeProfileTrans(celula, ncelperftransp, (*vg1dSP).lixo5, indTramo);
  }
  dt = arq.dtmax;
  if (chokeSup.AreaGarg >= 0.6 * celula[ncel - 1].duto.area) aberto = 1;
  //else aberto=0;
  if (fabsl(presfim - pGSup) < 0.5 && aberto == 0 && dt > 1. && chokeSup.AreaGarg < 0.6 * celula[ncel - 1].duto.area) dt =
      1.;
  for (int i = 0; i < arq.eventoabre; i++) {
    if ((*vg1dSP).lixo5 > arq.Tevento[i] - arq.dtmax && (*vg1dSP).lixo5 < arq.Tevento[i] + 30) {
      if (dt > menorDx / 100.) dt = menorDx / 100.;
    }
  }
  if (((*vg1dSP).lixo5 > tMedMov && (fabsl(presMedMov - presfim) > 4.))
      || (presfim < pGSup && masChkSup == 1 && chokeSup.AreaGarg > (1e-3) * celula[ncel - 1].duto.area)
      || (celula[ncel].alf <= 0.06 && chokeSup.AreaGarg < 0.6 * celula[ncel - 1].duto.area)) {
    double denominador = 10.;
    double progres = 0.95;

    if ((presfim < pGSup && masChkSup == 1)) {
      denominador = 100.;
      progres = 0.5;
      mult = pow(progres, fabsl(pGSup - presfim)) * mult;
    } else if (celula[ncel].alf <= 0.06 && chokeSup.AreaGarg < 0.6 * celula[ncel - 1].duto.area) {
      denominador = 1000.;
      progres = 0.5;
      mult = celula[ncel].alf * mult;
    } else mult = pow(progres, fabsl(presMedMov - presfim)) * mult;
    if (arq.dtmax * mult < menorDx / denominador) mult = (menorDx / denominador) / arq.dtmax;
    if (mult > 0.8) mult = 0.8;
  } else {
    mult = mult / 0.95;
    if (mult > 0.8) mult = 0.8;
  }

  for (int i = 0; i <= ncel; i++) {
    double jmix = 0.;
    double A1 = celula[i].duto.area;
    double dtaux;
    if (celula[i].alf < 1 - 1e-9) jmix += fabsl(
        celula[i].Mliqini
            / (A1 * (celula[i].rpC ) * (1. - 0 * celula[i].alf)));
    if (celula[i].alf > 1e-9) jmix += fabsl(
        (celula[i].MC - celula[i].Mliqini)
            / (A1 * celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp) * (0 + 1 * celula[i].alf)));
    if (fabsl(celula[i].VTemper) > jmix) jmix = fabsl(celula[i].VTemper);
    if (fabsl(jmix) > 1e-5) dtaux = 0.8 * celula[i].dx / jmix;
    else dtaux = dt;
    if (dtaux < dt) dt = dtaux;
  }		//alteracao2
  dt = mult * dt;
  if (dt > arq.dtmax) dt = arq.dtmax;
  if (arq.evento.size() > indevento) {
    if ((*vg1dSP).lixo5 < arq.evento[indevento] && ((*vg1dSP).lixo5 + dt) > (arq.evento[indevento] + 0.1)) {
      dt = arq.evento[indevento] - (*vg1dSP).lixo5;
      indevento++;
    } else if ((*vg1dSP).lixo5 < arq.evento[indevento] && ((*vg1dSP).lixo5 + dt) >= (arq.evento[indevento])) indevento++;
  }

  EstadoMaster1 = 1;
  for (int i = 0; i < nfechaM1; i++) {
    if ((*vg1dSP).lixo5 > fechaM1[i]) {
      for (int j = 0; j < nabreM1; j++) {
        if ((*vg1dSP).lixo5 < abreM1[j]) {
          EstadoMaster1 = 0;
          contaMaster1++;
          if (contaMaster1 > 19) contaMaster1 = 20;
          if ((*vg1dSP).lixo5 >= abreM1[j] - dt) contaMaster1 = 0;
          break;
        }
      }
    }
  }



  for (int i = 0; i <= ncel; i++) {
    celula[i].dt = dt;
    celula[i].dt2 = dt;
    celula[i].dtPig = dt;
  }

}

void SProdVap::EvoluiFrac(double alfrev) {//alteracao7

  for (int i = 0; i <= ncel; i++){

		if(i<ncel){//alteracao7
			celula[i].avancalf(reinicia, ncel);
		}
		if(i==ncel){
			if((*vg1dSP).chaverede==0 || noextremo ==1)
				celula[i].alf=celula[i-1].alf;

			if(celula[i].alf<0.05)celula[i].alf=0.05;
		}
  }
  for (int i = 0; i <= ncel; i++) {
    if (i < ncel && celula[i].estadoPig == 1) {
      celula[i].avancPig(reinicia);
      celula[i].avancalfPig();
    } else {
      celula[i].alfPigE = celula[i].alf;
      celula[i].alfPigD = celula[i].alf;
    }
  }
}

void  SProdVap::ReiniEvolFrac0(){//alteracao7
	for(int i=0; i<=ncel;i++){
		if(celula[i].dt<dt) dt=celula[i].dt;
		if(celula[i].dt2<dt) dt=celula[i].dt2;
		if(celula[i].dtPig<dt) dt=celula[i].dtPig;
	}
}

void SProdVap::ReiniEvolFrac() {
  for (int i = 0; i <= ncel; i++) {
    celula[i].dt = dt;
    celula[i].dt2 = dt;
    celula[i].dtPig = dt;
  }
  for (int i = 0; i <= ncel; i++) {
    celula[i].alf = celula[i].alfini;
    celula[i].razPig = celula[i].razPigini;
    celula[i].alfPigE = celula[i].alfPigEini;
    celula[i].alfPigD = celula[i].alfPigDini;
  }
}

void SProdVap::AtualizaPig() {
  for (int i = 1; i <= ncel; i++) {
    if (i < ncel && celula[i].estadoPig == 1) {
      if (celula[i].velPig >= 0) {
        if (celula[i].razPig >= 1. - (*vg1dSP).localtiny) {
          celula[i].estadoPig = 0;
          celula[i + 1].estadoPig = 1;
          celula[i].razPig = 0.;
          celula[i + 1].razPig = 0.;
          celula[i + 1].indpig = celula[i].indpig;
          celula[i].indpig = -1;
          celula[i + 1].velPig = celula[i].velPig;
          celula[i + 1].alfPigE = celula[i].alf;

        }
      } else {
        if (celula[i].razPig <= (*vg1dSP).localtiny) {
          celula[i].estadoPig = 0;
          celula[i - 1].estadoPig = 1;
          celula[i].razPig = 0.;
          celula[i - 1].razPig = 1.;
          celula[i - 1].indpig = celula[i].indpig;
          celula[i].indpig = -1;
          celula[i - 1].velPig = celula[i].velPig;
          celula[i - 1].alfPigD = celula[i].alf;
        }
      }
    }
  }
}

///////////////////////////// alteracao7 //////////////////////////////////////////////
void  SProdVap::SolveAcopPV(){
    for (int i = 0; i <= ncel; i++) {
		 celula[i].GeraLocal(presfim,masChkSup,ncel,arq.master1.razareaativ,presE, tempE,titE);
		 for (int j = 0; j < 5; j++) {
			   matglobP[2 * i][j - 2] = celula[i].local[0][j];
			   matglobP[2 * i + 1][j - 2] = celula[i].local[1][j];
			   termolivreP[2 * i] = celula[i].TL[0];
			   termolivreP[2 * i + 1] = celula[i].TL[1];
		 }
	 }

	 matglobP.GaussElimPP(termolivreP);
}
///////////////////////////// alteracao7 //////////////////////////////////////////////

void SProdVap::SolveTrans(double titRev, double alfRev) {//alteracao7
  double velmaxdesc=0;//alteracao5
	///////////////////////////// alteracao7 //////////////////////////////////////////////
	if((*vg1dSP).chaverede==0){
	   determinaDT();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	   reinicia=0;

	   arq.atualiza(noinicial,noextremo,chokeSup,celula,pGSup,temperatura,presE,tempE,titE,(*vg1dSP).lixo5);//alteracao4
	  //trackRGO=arq.trackRGO;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	   EvoluiFrac(alfRev);
	   if(reinicia==-1){
		   ReiniEvolFrac0();
		   ReiniEvolFrac();
		   EvoluiFrac(alfRev);
		   reinicia=0;
	   }
	   AtualizaPig();

	   calcCCpres(titRev,alfRev);
	   renovaterm();

	   SolveAcopPV();
  }
	///////////////////////////// alteracao7 //////////////////////////////////////////////

  renova();

  if(((*vg1dSP).chaverede==0 || noextremo==1) && arq.chkv==0){//alteracao7
	    if(celula[ncel-1].MliqiniR<0 ){

		      celula[ncel-1].MR=celula[ncel-1].MR-celula[ncel-1].MliqiniR;
		      celula[ncel-1].MliqiniR=0;
		      celula[ncel-1].term1R=0;
		      celula[ncel-1].term2R=0;

		      celula[ncel].MC=celula[ncel].MC-celula[ncel].Mliqini;
		      celula[ncel].Mliqini=0;
		      celula[ncel].term1=0;
		      celula[ncel].term2=0;
	    }
	    if(celula[ncel-1].QLR<0 && ( masChkSup==0 || aberto==1)){
		      celula[ncel-1].QLR=0;
		      celula[ncel].QL=0;
	    }
  }//alteracao7
  else if(arq.chkv==1 && masChkSup==0){//alteracao7
	    if(celula[ncel-1].MliqiniR<0 ){

		      celula[ncel-1].MR=0.;
		      celula[ncel-1].MliqiniR=0;
		      celula[ncel-1].term1R=0;
		      celula[ncel-1].term2R=0;

		      celula[ncel].MC=0.;
		      celula[ncel].Mliqini=0;
		      celula[ncel].term1=0;
		      celula[ncel].term2=0;
	    }
	    if(celula[ncel-1].QLR<0 && ( masChkSup==0 || aberto==1)){
		      celula[ncel-1].QLR=0;
		      celula[ncel].QL=0;
	    }
  }//alteracao7

  renovaTemp();

  double tempantiga;
  double buffer;
  tempantiga = celula[0].temp;
  celula[0].tempini = celula[0].temp;
	if(arq.ConContEntrada>0)//alteracao4
		celula[0].temp=tempE;
	else
	    celula[0].temp=temperatura;
  celula[1].tempL = celula[0].temp;
  for (int i = 1; i <= ncel; i++) {
    buffer = celula[i].temp;
    celula[i].tempini = celula[i].temp;
	if(i<ncel || (i==ncel && celula[i].VTemper>=0.)){//alteracao7
	  calcTempEntalp(i,tempantiga);
	}
    else{//alteracao7
		if((*vg1dSP).chaverede==0 || noextremo==1)celula[i].temp=celula[i].calor.Textern1;
		else celula[i].temp=tGSup;
	}
    tempantiga = buffer;
    if (i < ncel) celula[i + 1].tempL = celula[i].temp;
    celula[i - 1].tempR = celula[i].temp;
  }
  presfim = celula[ncel].pres;
  double mliqfim = celula[ncel - 1].MliqiniR;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  renovaalbetini();
  renovaMasEsp();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
  tVet.push_back(dt);
  presVet.push_back(presfim * dt);
  double jtemporario = celula[ncel - 1].Mliqini
      / (celula[ncel - 1].duto.area
          * ( celula[ncel - 1].rpC));
  jtemporario += (celula[ncel - 1].MC - celula[ncel - 1].Mliqini)
      / (celula[ncel - 1].duto.area * celula[ncel - 1].flui.MasEspGas(celula[ncel - 1].pres, celula[ncel - 1].temp));
  jVet.push_back(jtemporario * dt);
  alfVet.push_back(celula[ncel - 1].alf * dt);
  ktMedMov += dt;
  pTotal += presfim * dt;
  jTotal += jtemporario * dt;
  alfTotal += celula[ncel - 1].alf * dt;
  if ((*vg1dSP).lixo5 > tMedMov) {
    pTotal -= presVet.front();
    presVet.erase(presVet.begin());
    jTotal -= jVet.front();
    jVet.erase(jVet.begin());
    alfTotal -= alfVet.front();
    alfVet.erase(alfVet.begin());
    ktMedMov -= tVet.front();
    tVet.erase(tVet.begin());
    presMedMov = pTotal / ktMedMov;
    jMedMov = jTotal / ktMedMov;
    alfMedMov = alfTotal / ktMedMov;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////
  if ((*vg1dSP).lixo5 >= 6000) {
    int para;
    para = 0;
  }
  if((*vg1dSP).chaverede!=0)(*vg1dSP).lixo5=(*vg1dSP).lixo5R;//alteracao7
  if (arq.nperfisp > 0) {
    if (((*vg1dSP).lixo5 > (*vg1dSP).localtiny && (*vg1dSP).lixo5 <= arq.profp.tempo[kontaTempoProf] && (*vg1dSP).lixo5 + dt >= arq.profp.tempo[kontaTempoProf])) {
      arq.imprimeProfile(celula, flut, (*vg1dSP).lixo5, indTramo);
      arq.profp.tempo[kontaTempoProf] = (*vg1dSP).lixo5;
      kontaTempoProf++;
      if (kontaTempoProf >= arq.profp.n) kontaTempoProf--;
    }
  }
  if (arq.nperfistransp > 0) {
    if (((*vg1dSP).lixo5 > (*vg1dSP).localtiny && (*vg1dSP).lixo5 <= arq.proftransp.tempo[kontaTempoTransProf]
        && (*vg1dSP).lixo5 + dt >= arq.proftransp.tempo[kontaTempoTransProf])) {
      arq.imprimeProfileTrans(celula, ncelperftransp, (*vg1dSP).lixo5, indTramo);
      arq.proftransp.tempo[kontaTempoTransProf] = (*vg1dSP).lixo5;
      kontaTempoTransProf++;
      if (kontaTempoTransProf >= arq.proftransp.n) kontaTempoTransProf--;
    }
  }
  if (arq.ntendp > 0) {
    for (int i = 0; i < arq.ntendp; i++) {
      if (resettrend[i] == 0) {
        arq.imprimeTrend(celula, MatTrendP[i], (*vg1dSP).lixo5, i, ntrend[i]);
        ntrend[i]++;
      }
      resettrend[i] += dt;
      if (resettrend[i] > arq.trendp[i].dt) resettrend[i] = 0;
    }
  }
  if (arq.ntendtransp > 0) {
    for (int i = 0; i < arq.ntendtransp; i++) {
      if (resettrendtrans[i] == 0) {
        MatTrendTransP[i][ntrendtrans[i]][0] = (*vg1dSP).lixo5;
        int poscel = arq.trendtransp[i].posic;
        int poscam = arq.trendtransp[i].camada - 1;
        int posdiscre = arq.trendtransp[i].discre - 1;
        MatTrendTransP[i][ntrendtrans[i]][1] = celula[poscel].calor.Tcamada[poscam][posdiscre];
        ntrendtrans[i]++;
      }
      resettrendtrans[i] += dt;
      if (resettrendtrans[i] > arq.trendtransp[i].dt) resettrendtrans[i] = 0;
    }
  }
  (*vg1dSP).contador = kSP;
  cout << kSP << "  " << (*vg1dSP).lixo5 << " " << dt;
  for (int i = 0; i < arq.ntela; i++) {
    int posic = arq.tela[i].posic;
    if (arq.tela[i].col == 1) {
      switch (arq.tela[i].var) {
        case 1:
          cout << " " << celula[posic].pres;
          break;
        case 2:
          cout << " " << celula[posic].temp;
          break;
        case 3:
          cout << " " << celula[posic].alf;
          break;
        case 4:
          cout << " " << (celula[posic].QG / celula[posic].duto.area);
          break;
        case 5:
          cout << " " << (celula[posic].QL / celula[posic].duto.area);
          break;
      }
    }
  }
  cout << "\n";

  int MaxKontaImpres = 1000;

  while (fabsl(arq.logevento[contaLog].instante - (*vg1dSP).lixo5) < dt) {
    // current date/time based on current system
    time_t now = time(0);
    tm *ltm = localtime(&now);    ///////////Retirado de https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
    ofstream escreveIni(tmpLog.c_str(), ios_base::app);
    escreveIni << "************************************************************************************************"
        << "\n";
    escreveIni << "Evento Externo = ";
    escreveIni << arq.logevento[contaLog].instante << " ; ";
    escreveIni << arq.logevento[contaLog].duracao << " ; ";
    escreveIni << arq.logevento[contaLog].estIni << " ; ";
    escreveIni << arq.logevento[contaLog].estFim << " ; ";
    escreveIni << arq.logevento[contaLog].descricao << " ; ";
    escreveIni << "datahora = ";
    escreveIni << ltm->tm_mday << "/";
    escreveIni << 1 + ltm->tm_mon << "/";
    escreveIni << 1900 + ltm->tm_year << " ";
    escreveIni << 0 + ltm->tm_hour << ":";
    escreveIni << 0 + ltm->tm_min << ":";
    escreveIni << 0 + ltm->tm_sec;
    escreveIni << "\n";
    contaLog++;

    escreveIni.close();
  }
  if ((fabsl((*vg1dSP).lixo5 * (100. / 5.) / arq.tfinal - round((*vg1dSP).lixo5 * (100. / 5.) / arq.tfinal))
      < 0.5 * dt * (100 / 5.) / arq.tfinal) || KontaImprime > MaxKontaImpres) {
    KontaImprime = 0;
    ofstream escreveIni(tmpLog.c_str(), ios_base::app);
    escreveIni << "************************************************************************************************"
        << "\n";
    escreveIni << "Percentual alcancado = " << (*vg1dSP).lixo5 * (100.) / arq.tfinal << " % da simulacao alcancado" << "\n";
    escreveIni << "| Passo de Tempo = " << kSP << "| Tempo (s) = " << (*vg1dSP).lixo5 << "| Incremento de Tempo (s) = " << dt
        << " |" << "\n";
    for (int i = 0; i < arq.ntela; i++) {
      int posic = arq.tela[i].posic;
      if (arq.tela[i].col == 1) {
        switch (arq.tela[i].var) {
          case 1:
            escreveIni << " Pressao na Linha de Producao (kgf/cm2), Celula " << posic << " = " << celula[posic].pres
                << "\n";
            break;
          case 2:
            escreveIni << " Temperatura na Linha de Producao (C), Celula " << posic << " = " << celula[posic].temp
                << "\n";
            break;
          case 3:
            escreveIni << " Fracao de Vazio na Linha de Producao (-), Celula " << posic << " = " << celula[posic].alf
                << "\n";
            break;
          case 4:
            escreveIni << " Velocidade Superficial de Gas na Linha de Producao (m/s), Celula " << posic << " = "
                << (celula[posic].QG / celula[posic].duto.area) << "\n";
            break;
          case 5:
            escreveIni << " Velocidade Superficial de Liquido na Linha de Producao (m/s), Celula " << posic << " = "
                << (celula[posic].QL / celula[posic].duto.area) << "\n";
            break;
        }
      }
    }
    time_t now = time(0);
    tm *ltm = localtime(&now);    ///////////Retirado de https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
    escreveIni << "datahora = ";
    escreveIni << ltm->tm_mday << "/";
    escreveIni << 1 + ltm->tm_mon << "/";
    escreveIni << 1900 + ltm->tm_year << " ";
    escreveIni << 0 + ltm->tm_hour << ":";
    escreveIni << 0 + ltm->tm_min << ":";
    escreveIni << 0 + ltm->tm_sec;
    escreveIni << "\n";
  }

  kSP++;
  KontaImprime++;
  if((*vg1dSP).chaverede==0)(*vg1dSP).lixo5 += dt;//alteracao7
}

void  SProdVap::ImprimeTrend() {
  if(arq.ntendp>0){
      for(int i=0;i<arq.ntendp;i++){
        FullMtx<double> saidatrend(TrendLengthP[i],arq.nvartrendp[i]+1);
        for(int k=0; k<TrendLengthP[i];k++)
          for(int j=0;j<=arq.nvartrendp[i];j++)saidatrend[k][j]=MatTrendP[i][k][j];
        ostringstream saidaT;
        if(indTramo<0)
      saidaT << pathPrefixoArqSaida << "TENDP" << "-" << arq.trendp[i].posic << ".dat";
        else
        saidaT << pathPrefixoArqSaida << "Tramo" << indTramo << "-" << "TENDP" << "-" << arq.trendp[i].posic << ".dat";
      string tmp = saidaT.str();
      ofstream escreveTrend(tmp.c_str(), ios_base::out);
        //ofstream escreveTrend(arq.trendp[i].rotulo.c_str(), ios_base::out);
        double comprimento=0;
        int posicn=arq.trendp[i].posic;
        for(int k=0; k<=posicn; k++)comprimento+=arq.celp[k].dx;
      escreveTrend<<"# Comprimento a partir do Fundo de Poco (m) = "<<comprimento<<"\n";
      escreveTrend<<"# Rotulo = "<<arq.trendp[i].rotulo<<"\n";
      escreveTrend<<"# Indice da Celula = "<<arq.trendp[i].posic<<"\n";
        escreveTrend<<" Tempo (s) ;";
         if(arq.trendp[i].pres==1) escreveTrend<<" Pressao (kgf/cm2) ;";
         if(arq.trendp[i].temp==1) escreveTrend<<" Temperatura (C) ;";
         if(arq.trendp[i].hol==1) escreveTrend<<" Holdup (-) ;";
         if(arq.trendp[i].ugs==1) escreveTrend<<" Usg (m/s) ;";
         if(arq.trendp[i].uls==1) escreveTrend<<" Uls (m/s) ;";
         if(arq.trendp[i].ug==1) escreveTrend<<" Ug (m/s) ;";
         if(arq.trendp[i].ul==1) escreveTrend<<" Ul (m/s) ;";
         if(arq.trendp[i].arra==1) escreveTrend<<" Arranjo (-) ;";
         if(arq.trendp[i].viscl==1) escreveTrend<<" Viscosidade Liquido (cP) ;";
         if(arq.trendp[i].viscg==1) escreveTrend<<" Viscosidade Gas (cP) ;";
         if(arq.trendp[i].rhog==1) escreveTrend<<" Massa Especifica Gas (kg/m3) ;";
         if(arq.trendp[i].rhol==1) escreveTrend<<" Massa Especifica Liquido (kg/m3) ;";
         if(arq.trendp[i].masg==1) escreveTrend<<" Vazao Massica Gas (kg/s) ;";
         if(arq.trendp[i].masl==1) escreveTrend<<" Vazao Massica Liquido (kg/s) ;";
         if(arq.trendp[i].c0==1) escreveTrend<<" C0 (-) ;";
         if(arq.trendp[i].ud==1) escreveTrend<<" Ud (m/s) ;";
         if(arq.trendp[i].yco2==1) escreveTrend<<" Fracao Molar de CO2 (-) ;";
         if(arq.trendp[i].calor==1) escreveTrend<<" Fluxo de Calor (W/m) ;";
         if(arq.trendp[i].masstrans==1) escreveTrend<<" Transferencia de Massa entre Fases (kgf/(s m)) ;";
         escreveTrend<<"\n";
        //escreveTrend<<saidatrend;
        int nc=saidatrend.col();
        int nl=saidatrend.lin();
        for ( int i = 0; i<nl; i++){
          if(saidatrend[i][0]<=-9999)break;
                for(int j=0;j<nc;j++){
                  escreveTrend.width(20);
                  escreveTrend.precision(19);
                  escreveTrend<<saidatrend[i][j]<<" ; ";
                }
                escreveTrend<<"\n";
        }
      escreveTrend<<"\n";
      escreveTrend.close();
      arqRelatorioPerfis << tmp.c_str() << endl;
      arqRelatorioPerfis.flush();
      }
  }
  if(arq.ntendtransp>0){
      for(int i=0;i<arq.ntendtransp;i++){
        FullMtx<double> saidatrend(TrendLengthTransP[i],2);
        for(int k=0; k<TrendLengthTransP[i];k++)
          for(int j=0;j<2;j++)saidatrend[k][j]=MatTrendTransP[i][k][j];
        int poscel=arq.trendtransp[i].posic;
        int poscam=arq.trendtransp[i].camada-1;
        int posdiscre=arq.trendtransp[i].discre-1;
        ostringstream saidaT;
        if(indTramo<0)
      saidaT << pathPrefixoArqSaida << "TENDTRANSP" << "-" << poscel << "-" << poscam << "-" << posdiscre << ".dat";
        else
        saidaT << pathPrefixoArqSaida << "Tramo" << indTramo << "-" << "TENDTRANSP" << "-" << poscel << "-" << poscam << "-" << posdiscre << ".dat";
      string tmp = saidaT.str();
      ofstream escreveTrend(tmp.c_str(), ios_base::out);
        //ofstream escreveTrend(arq.trendtransp[i].rotulo.c_str(), ios_base::out);
      escreveTrend<<"# Rotulo = "<<arq.trendtransp[i].rotulo<<"\n";
        escreveTrend<<" Tempo (s) ; ";
        escreveTrend<<" Temperatura (C) ;";
        escreveTrend<<"\n";
        //escreveTrend<<saidatrend;
      int nc=saidatrend.col();
      int nl=saidatrend.lin();
      for ( int i = 0; i<nl; i++){
        if(saidatrend[i][0]<=-9999)break;
              for(int j=0;j<nc;j++){
                escreveTrend.width(20);
                escreveTrend.precision(19);
                escreveTrend<<saidatrend[i][j]<<" ; ";
              }
              escreveTrend<<"\n";
      }
        escreveTrend<<"\n";
        escreveTrend.close();
        arqRelatorioPerfis << tmp.c_str() << endl;
        arqRelatorioPerfis.flush();

      }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////



double SProdVap::marchaProdPerm1(double pchute) {

  int corrigechute = 1;

  if (celula[0].acsr.tipo == 0) celula[0].temp = arq.celp[0].textern;
  else if (celula[0].acsr.tipo == 12) celula[0].temp = celula[0].acsr.injmvap.temp;
  else if (celula[0].acsr.tipo == 11) celula[0].temp = celula[0].acsr.iprvap.Tres;
  else celula[0].temp = arq.celp[0].textern;
  celula[0].tempL = celula[0].temp;
  celula[1].tempL = celula[0].temp;
  celula[0].tempini = celula[0].temp;

  celula[0].ML = 0.;
  celula[0].MC = 0.;
  celula[1].ML = 0.;
  celula[0].MliqiniL = 0.;
  celula[0].Mliqini = 0.;
  celula[1].MliqiniL = 0.;
  celula[0].QLL = 0.;
  celula[0].QL = 0;
  celula[1].QLL = 0.;
  celula[0].QG = 0.;
  double masfim = 0.;
  double masfim0 = -10;
  iterperm = 0;

  //if(celula[0].acsr.tipo==3 && (celula[0].acsr.ipr.Pres-pchute)<(*vg1dSP).localtiny)
  //pchute=0.95*celula[0].acsr.ipr.Pres;

  double pchute0 = pchute;
  while (fabsl(masfim - masfim0) > 0.001 && iterperm < 3) {

    masfim0 = masfim;
    int i;
    corrigechute = 1;
    while (corrigechute == 1) {
      celula[0].presauxL = pchute;
      celula[0].presLini = pchute;
      celula[0].presL = pchute;
      celula[0].pres = pchute;
      celula[1].presL = pchute;
      celula[0].presini = pchute;
      celula[1].presLini = pchute;
      celula[0].presaux = pchute;
      celula[1].presauxL = pchute;
      if (iterperm == 0) {
    	if(celula[0].acsr.tipo != 1){
            celula[0].alf = 0.;
            celula[0].alfini = 0.;
    	}
    	else{
            celula[0].alf = 1.;
            celula[0].alfini = 1.;
    	}
        celula[1].alfL = celula[0].alf;
        celula[1].alfLini = celula[0].alf;
        celula[0].alfPigD = celula[0].alf;
        celula[0].alfPigDini = celula[0].alf;
        celula[0].alfPigE = celula[0].alf;
        celula[0].alfPigEini = celula[0].alf;
      } else {
        celula[0].alf = 0*0.5+1*celula[1].alf;
        celula[0].alfini = 0*0.5+1*celula[1].alf;
        celula[1].alfL = celula[0].alf;
        celula[1].alfLini = celula[0].alf;
        celula[0].alfPigD = celula[0].alf;
        celula[0].alfPigDini = celula[0].alf;
        celula[0].alfPigE = celula[0].alf;
        celula[0].alfPigEini = celula[0].alf;
      }
      i = 1;
      while (i <= ncel && celula[i - 1].pres >= 1. && fabsl(pchute - celula[0].pres) < (*vg1dSP).localtiny) {
if(i==199){
	int para=0;
	para++;
}
        calcHmixPerm(i);
        RenovaPresPermMon(i, 0);
        if (celula[i].presaux <= 1) return -1e10;
        atualizaPeriPmonProd(i);
        calcHmixPerm(i);
        RenovaMassPerm(i);
        //RenovaTempPerm(i, 0);
        calcTempEntalpPerm(i);
        atualizaPeriTempProd(i);
        RenovaPresPermJus(i, 0);
        if (celula[i].pres <= 1) return -1e10;
        atualizaPeriPjusProd(i);
        RenovaTransMassPerm(i - 1);
        if (arq.ordperm > 1) {
          double D0presaux = celula[i].presaux - celula[i - 1].pres;
          double D0pres = celula[i].pres - celula[i].presaux;
          double D0temp = celula[i].temp - celula[i - 1].temp;
          RenovaPresPermMon(i, 1);
          atualizaPeriPmonProd(i);
          RenovaMassPerm(i);
          //RenovaTempPerm(i, 1);
          calcTempEntalpPerm(i);
          RenovaPresPermJus(i, 1);
          celula[i].pres = 0.5 * (celula[i].presaux + D0pres + celula[i].pres);
          celula[i].presaux = 0.5 * (celula[i - 1].pres + D0presaux + celula[i].presaux);
          celula[i].temp = 0.5 * (celula[i - 1].temp + D0temp + celula[i].temp);
          atualizaPeriPjusProd(i);
          atualizaPeriPmonProd(i);
          atualizaPeriTempProd(i);
          RenovaMassPerm(i);
          RenovaTransMassPerm(i - 1);

        }
        celula[i].rpC = celula[i].rpCi = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp);
        celula[i].rpL = celula[i].rpLi = celula[i].flui.MasEspLiq(celula[i].presL, celula[i].tempL);
        celula[i - 1].rpR = celula[i - 1].rpRi = celula[i].rpC;
        i++;

        if (celula[i - 1].pres <= 1) return -1e10;
        else if ((celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.ipr.Pres - celula[i - 1].pres) > (*vg1dSP).localtiny))  return -1e10;

      }
      if (i == ncel + 1) corrigechute = 0;
    }
    masfim = celula[ncel - 1].MC;
    iterperm++;
    if (iterperm > 200) NumError("Convergência em marchaProdPerm1 atingiu m�ximo de itera��es");
  }

  return pGSup - celula[ncel].pres;
}

double SProdVap::marchaProdPerm2(double pchute) {

  int corrigechute = 1;

  if (celula[0].acsr.tipo == 0) celula[0].temp = arq.celp[0].textern;
  else if (celula[0].acsr.tipo == 12) celula[0].temp = celula[0].acsr.injmvap.temp;
  else if (celula[0].acsr.tipo == 11) celula[0].temp = celula[0].acsr.iprvap.Tres;
  else celula[0].temp = arq.celp[0].textern;
  celula[0].tempL = celula[0].temp;
  celula[1].tempL = celula[0].temp;
  celula[0].tempini = celula[0].temp;

  celula[0].ML = 0.;
  celula[0].MC = 0.;
  celula[1].ML = 0.;
  celula[0].MliqiniL = 0.;
  celula[0].Mliqini = 0.;
  celula[1].MliqiniL = 0.;
  celula[0].QLL = 0.;
  celula[0].QL = 0;
  celula[1].QLL = 0.;
  celula[0].QG = 0.;
  double masfim = 0.;
  double masfim0 = -10;
  iterperm = 0;

  //if(celula[0].acsr.tipo==3 && (celula[0].acsr.ipr.Pres-pchute)<(*vg1dSP).localtiny)
  //pchute=0.95*celula[0].acsr.ipr.Pres;

  double pchute0 = pchute;
  while (fabsl(masfim - masfim0) > 0.001) {
    masfim0 = masfim;
    int i;
    corrigechute = 1;
    while (corrigechute == 1) {
      celula[0].presauxL = pchute;
      celula[0].presLini = pchute;
      celula[0].presL = pchute;
      celula[0].pres = pchute;
      celula[1].presL = pchute;
      celula[0].presini = pchute;
      celula[1].presLini = pchute;
      celula[0].presaux = pchute;
      celula[1].presauxL = pchute;
      if (iterperm == 0) {
      	if(celula[0].acsr.tipo != 1){
              celula[0].alf = 0.;
              celula[0].alfini = 0.;
      	}
      	else{
              celula[0].alf = 1.;
              celula[0].alfini = 1.;
      	}
        celula[1].alfL = celula[0].alf;
        celula[1].alfLini = celula[0].alf;
        celula[0].alfPigD = celula[0].alf;
        celula[0].alfPigDini = celula[0].alf;
        celula[0].alfPigE = celula[0].alf;
        celula[0].alfPigEini = celula[0].alf;
      } else {
        celula[0].alf = celula[1].alf;
        celula[0].alfini = celula[1].alf;
        celula[1].alfL = celula[0].alf;
        celula[1].alfLini = celula[0].alf;
        celula[0].alfPigD = celula[0].alf;
        celula[0].alfPigDini = celula[0].alf;
        celula[0].alfPigE = celula[0].alf;
        celula[0].alfPigEini = celula[0].alf;
      }
      i = 1;
      while (i <= ncel && celula[i - 1].pres >= 1. && fabsl(pchute - celula[0].pres) < (*vg1dSP).localtiny) {
    	calcHmixPerm(i);
        RenovaPresPermMon(i, 0);
        atualizaPeriPmonProd(i);
        RenovaMassPerm(i);
        //RenovaTempPerm(i, 0);
        calcTempEntalpPerm(i);
        atualizaPeriTempProd(i);
        RenovaPresPermJus(i, 0);
        atualizaPeriPjusProd(i);
        RenovaTransMassPerm(i - 1);
        if (arq.ordperm > 1) {
          double D0presaux = celula[i].presaux - celula[i - 1].pres;
          double D0pres = celula[i].pres - celula[i].presaux;
          double D0temp = celula[i].temp - celula[i - 1].temp;
          RenovaPresPermMon(i, 1);
          //atualizaPeriPmonProd(i);
          calcTempEntalpPerm(i);
          RenovaMassPerm(i);
          //RenovaTempPerm(i,1);
          calcTempEntalpPerm(i);
          RenovaPresPermJus(i, 1);
          celula[i].pres = 0.5 * (celula[i].presaux + D0pres + celula[i].pres);
          celula[i].presaux = 0.5 * (celula[i - 1].pres + D0presaux + celula[i].presaux);
          celula[i].temp = 0.5 * (celula[i - 1].temp + D0temp + celula[i].temp);
          atualizaPeriPjusProd(i);
          atualizaPeriPmonProd(i);
          atualizaPeriTempProd(i);
          RenovaMassPerm(i);
          RenovaTransMassPerm(i - 1);

        }
        celula[i].rpC = celula[i].rpCi = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp);
        celula[i].rpL = celula[i].rpLi = celula[i].flui.MasEspLiq(celula[i].presL, celula[i].tempL);
        celula[i - 1].rpR = celula[i - 1].rpRi = celula[i].rpC;
        i++;
        if (celula[i - 1].pres <= 1 || ((i - 1) >= ncel && celula[i - 1].pres <= pGSup)) return -1e10;
        else if (celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.iprvap.Pres - celula[i - 1].pres) > (*vg1dSP).localtiny) return -1e10;
      }
      if (i == ncel + 1) corrigechute = 0;
    }
    masfim = celula[ncel - 1].MC;
    iterperm++;
    if (iterperm > 200) NumError("Convergência em marchaProdPerm2 atingiu m�ximo de itera��es");
  }

  double tESup = celula[ncel].temp;
  double alfSup = celula[ncel].alf;

  double masentrada = celula[ncel - 1].MR;
  double massgas = celula[ncel - 1].MR - celula[ncel - 1].MliqiniR;
  double maxSup = 0.;
  double chokemas = 0;

  double tit;
  presfim = celula[ncel].pres;
  double rholp = celula[ncel].flui.MasEspLiq(celula[ncel].pres, celula[ncel].temp);
  double rholmix = rholp;
  tit = fabsl(massgas / masentrada);

  double masChk;

  double ypres = pGSup / presfim;
  masChk = chokeSup.vazmassSachdVap(ypres, presfim, tESup, alfSup, tit);
  maxSup = chokeSup.vazmaxSachdVap(presfim, tESup, alfSup, tit);
  if (fabsl(ypres) > fabsl(chokeSup.razpres)) maxSup = masChk;

  return masfim - maxSup;
}

double SProdVap::buscaProdPfundoPerm(double chute) {
  double pchute = pGSup;
  double taux;
  double j = 0.;
  double rmis = 0.;
  double f1 = 0.;

  if (celula[0].acsr.tipo == 12 && fabsl(celula[0].acsr.injmvap.Mass) > 0.) {
    taux = arq.celp[0].textern;
    double visP = celula[0].flui.ViscLiq(pchute, taux);
    double visG = celula[0].flui.ViscGas(pchute, taux);
    double visMis = visP;
    double rP = celula[0].flui.MasEspLiq(pchute, taux);
    double rG = celula[0].flui.MasEspGas(pchute, taux);
    rmis = rP;
    double rlpA = celula[0].flui.MasEspLiq(1., 15.);
    //////////////////////////////////////////////////////////////////////////////////////////////
    double massic = celula[0].acsr.injmvap.Mass;
    if((*vg1dSP).chaverede==1)
    massic*=50;
    else massic*=50;
    double fracmasshidra = celula[0].flui.Titulo(pchute, taux);
    double massicP = (1. - fracmasshidra) * massic;
    double massicG = fracmasshidra * massic;
    j = (massicP / rP + massicG / rG) / celula[0].duto.area;
    double alfmis = (massicG / rG) / (massicP / rP + massicG / rG);
    rmis = (1 - alfmis) * rmis + alfmis * rG;
    visMis = (1 - alfmis) * visMis + alfmis * visG;
    double re;
    if (celula[0].duto.revest == 0) re = celula[0].Rey(celula[0].duto.a, j, rmis, visMis);
    else {
      double dhid = 4 * celula[0].duto.area / celula[0].duto.peri;
      re = celula[0].Rey(dhid, j, rmis, visMis);
    }
    f1 = celula[0].fric(re, celula[0].duto.rug / celula[0].duto.a);
  }
  double perdafric = (f1 * rmis * j * fabsl(j) / 2.) * celula[0].duto.peri / celula[0].duto.area;
  for (int i = ncel; i > 0; i--) {
    taux = arq.celp[i].textern;
    double rhol = celula[i].flui.MasEspLiq(pchute, taux);
    double rhog = celula[i].flui.MasEspGas(pchute, taux);
    double alfa = 0.;
    double rhomix = (1. - alfa) * rhol + alfa * rhog;
    double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
    pchute += ((rhomix * 9.81 * sinl(celula[i].duto.teta) * dxmed + perdafric * dxmed) / 98066.5);
    if (celula[i - 1].acsr.tipo == 7) pchute -= celula[i - 1].acsr.delp;
    if (celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.iprvap.Pres - pchute) > -(*vg1dSP).localtiny) pchute = 1.01
        * celula[i - 1].acsr.iprvap.Pres;
  }

  if(pchute>2000)pchute=2000.;
  if(chute>0) pchute=chute;
  double pchute2;
  double pchuteAux;
  double val;
  val = marchaProdPerm1(pchute);
  if (val < -0.9e10) {
    pchuteAux = pGSup;
    for (int i = ncel; i > 0; i--) {
      taux = arq.celp[i].textern;
      double rhol = 1000 + 0 * celula[i].flui.MasEspLiq(pchuteAux, taux);
      double rhog = celula[i].flui.MasEspGas(pchuteAux, taux);
      double alfa = 0.;
      double rhomix = (1. - alfa) * rhol + alfa * rhog;
      double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
      pchuteAux += (rhomix * 9.81 * sinl(celula[i].duto.teta) * dxmed + perdafric * dxmed) / 98066.5;
      if (celula[i - 1].acsr.tipo == 7) pchuteAux -= celula[i - 1].acsr.delp;
      if (celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.iprvap.Pres - pchuteAux) > -(*vg1dSP).localtiny)
    	  pchuteAux = 1.01 * celula[i - 1].acsr.iprvap.Pres;

    }

  } else if (val > 0.9e10) {
    pchuteAux = pGSup;
    for (int i = ncel; i > 0; i--) {
      taux = arq.celp[i].textern;
      double rhol = celula[i].flui.MasEspLiq(pchuteAux, taux);
      double rhog = celula[i].flui.MasEspGas(pchuteAux, taux);
      double alfa = 0.8;
      double rhomix = (1. - alfa) * rhol + alfa * rhog;
      double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
      pchuteAux += (rhomix * 9.81 * sinl(celula[i].duto.teta) * dxmed + perdafric * dxmed) / 98066.5;
      if (celula[i - 1].acsr.tipo == 7) pchuteAux -= celula[i - 1].acsr.delp;
      if (celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.iprvap.Pres - pchuteAux) > -(*vg1dSP).localtiny)
    	  pchuteAux = 1.01* celula[i - 1].acsr.iprvap.Pres;
    }
  }

  double mult1=0.9;
  double mult2 =1.1;
  int tipo=1;

  int kontaiter=0;

    while ((val < -0.9e10  || val > 0.9e10) && kontaiter<800) {
      pchute = 0.5 * (pchute + pchuteAux);
      if((fabsl(pchute-pchuteAux)/pchute)<0.001 && val < -0.9e10 ){
    	  pchute=1.05*pchute;
    	  if(celula[0].acsr.tipo == 11 && (celula[0].acsr.iprvap.Pres - pchute) > -(*vg1dSP).localtiny)
    		  pchute = 1.001 * celula[0].acsr.iprvap.Pres;

    	  pchuteAux=pchute;
      }

      val = marchaProdPerm1(pchute);
      kontaiter++;
    }

  kontaiter = 0;
  pchute2 = pchute;
  double chuteNeg;
  double chutePos;
  double chutelim = 0.;
  if(val<0.){
	chuteNeg=pchute;
  	while(val<0){
  		pchuteAux=pchute2;
  		pchute2*=0.9;
  		if(pchute2<=chutelim)pchute2=0.5*(pchuteAux+chutelim);
  		val=marchaProdPerm1(pchute2);
  		kontaiter++;
  		if(kontaiter>200){
  			if((*vg1dSP).chaverede==0)
  			NumError("Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm atingiu m�ximo de itera��es");
  		    else{
  		    	if((*vg1dSP).iterRede>0)return -1.1e6;
  		    	else return 1.e6;
  		    }
  		}
  		while(val<-0.9e10){
  			chutelim=pchute2;
  			pchute2=0.5*(pchute2+pchuteAux);
  			val=marchaProdPerm1(pchute2);
  			kontaiter++;
  			if(kontaiter>200){
  				if((*vg1dSP).chaverede==0)
  				NumError("Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm atingiu m�ximo de itera��es");
  	  		    else{
  	  		    	if((*vg1dSP).iterRede>0)return -1.1e6;
  	  		    	else return 1.e6;
  	  		    }
  			}
  		}
  	}
  	chutePos=pchute2;
  }
  else if(val>0.){
	chutePos=pchute;
  	while(val>0){
  		pchuteAux=pchute2;
  		pchute2*=1.1;
  		val=marchaProdPerm1(pchute2);
  		kontaiter++;
  		if(kontaiter>200){
  			if((*vg1dSP).chaverede==0)
  			NumError("Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm atingiu m�ximo de itera��es");
  		    else{
  		    	if((*vg1dSP).iterRede>0)return -1.1e6;
  		    	else return 1.e6;
  		    }
  		}
  		while(val>0.9e10){
  			chutelim=pchute2;
  			pchute2=0.5*(pchute2+pchuteAux);
  			val=marchaProdPerm1(pchute2);
  			kontaiter++;
  			if(kontaiter>200){
  				if((*vg1dSP).chaverede==0)
  				NumError("Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm atingiu m�ximo de itera��es");
  	  		    else{
  	  		    	if((*vg1dSP).iterRede>0)return -1.1e6;
  	  		    	else return 1.e6;
  	  		    }
  			}
  		}
  	}
  	chuteNeg=pchute2;
  }


  return zriddr(chuteNeg, chutePos, 1, 0);
}

double SProdVap::buscaProdPfundoPerm2(double chute) {
  double pchute = pGSup;
  double taux;
  double j = 0.;
  double rmis = 0.;
  double f1 = 0.;

  if (celula[0].acsr.tipo == 12 && fabsl(celula[0].acsr.injmvap.Mass) > 0.) {
    taux = arq.celp[0].textern;

    double visP = celula[0].flui.ViscLiq(pchute, taux);
    double visG = celula[0].flui.ViscGas(pchute, taux);
    double visMis = visP ;
    double rP = celula[0].flui.MasEspLiq(pchute, taux);
    double rG = celula[0].flui.MasEspGas(pchute, taux);
    rmis = rP;
    double rlpA = celula[0].flui.MasEspLiq(1., 15.);

    //////////////////////////////////////////////////////////////////////////////////////////////
    double massic = celula[0].acsr.injmvap.Mass;
    if((*vg1dSP).chaverede==1)
    massic *= 50;
    else massic*=50;
    double fracmasshidra = celula[0].flui.Titulo(pchute, taux);
    double massicP = (1. - fracmasshidra) * massic;
    double massicG = fracmasshidra * massic;
    j = (massicP / rP +  massicG / rG) / celula[0].duto.area;
    double alfmis = (massicG / rG) / (massicP / rP  + massicG / rG);
    rmis = (1 - alfmis) * rmis + alfmis * rG;
    visMis = (1 - alfmis) * visMis + alfmis * visG;
    double re;
    if (celula[0].duto.revest == 0) re = celula[0].Rey(celula[0].duto.a, j, rmis, visMis);
    else {
      double dhid = 4 * celula[0].duto.area / celula[0].duto.peri;
      re = celula[0].Rey(dhid, j, rmis, visMis);
    }
    f1 = celula[0].fric(re, celula[0].duto.rug / celula[0].duto.a);
  }
  double perdafric = (f1 * rmis * j * fabsl(j) / 2.) * celula[0].duto.peri / celula[0].duto.area;
  for (int i = ncel; i > 0; i--) {
    taux = arq.celp[i].textern;
    double rhol = celula[i].flui.MasEspLiq(pchute, taux);
    double rhog = celula[i].flui.MasEspGas(pchute, taux);
    double alfa = 0.;
    double rhomix = (1. - alfa) * rhol + alfa * rhog;
    double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
    pchute += (rhomix * 9.81 * sinl(celula[i].duto.teta) * dxmed + perdafric * dxmed) / 98066.5;
    //if(celula[i-1].acsr.tipo==7)pchute-=celula[i-1].acsr.delp;
    if (celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.iprvap.Pres - pchute) > -(*vg1dSP).localtiny) pchute = 1.05
        * celula[i - 1].acsr.iprvap.Pres;
  }

  if(pchute>2000)pchute=2000.;
  if(chute>0) pchute=chute;
  double pchute2;
  double pchuteAux;
  double val;
  val = marchaProdPerm2(pchute);

  if (val < -0.9e10) {
    pchuteAux = pGSup;
    for (int i = ncel; i > 0; i--) {
      taux = arq.celp[i].textern;
      double rhol = 1000 + 0 * celula[i].flui.MasEspLiq(pchuteAux, taux);
      double rhog = celula[i].flui.MasEspGas(pchuteAux, taux);
      double alfa = 0.;
      double rhomix = (1. - alfa) * rhol + alfa * rhog;
      double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
      pchuteAux += (rhomix * 9.81 * sinl(celula[i].duto.teta) * dxmed + perdafric * dxmed) / 98066.5;
      //if(celula[i-1].acsr.tipo==7)pchuteAux-=celula[i-1].acsr.delp;
      if (celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.iprvap.Pres - pchuteAux) > -(*vg1dSP).localtiny)
    	  pchuteAux = 1.01* celula[i - 1].acsr.iprvap.Pres;

    }
  } else if (val > 0.9e10) {
    pchuteAux = pGSup;
    for (int i = ncel; i > 0; i--) {
      taux = arq.celp[i].textern;
      double rhol = celula[i].flui.MasEspLiq(pchuteAux, taux);
      double rhog = celula[i].flui.MasEspGas(pchuteAux, taux);
      double alfa = 0.8;
      double rhomix = (1. - alfa) * rhol + alfa * rhog;
      double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
      pchuteAux += (rhomix * 9.81 * sinl(celula[i].duto.teta) * dxmed + perdafric * dxmed) / 98066.5;
      //if(celula[i-1].acsr.tipo==7)pchuteAux-=celula[i-1].acsr.delp;
      if (celula[i - 1].acsr.tipo == 11 && (celula[i - 1].acsr.iprvap.Pres - pchuteAux) > -(*vg1dSP).localtiny)
    	  pchuteAux = 1.15* celula[i - 1].acsr.iprvap.Pres;
    }
  }

  double mult1=0.9;
  double mult2 =1.1;

  int kontaiter=0;

  while ((val < -0.9e10  || val > 0.9e10) && kontaiter<800) {
    pchute = 0.5 * (pchute + pchuteAux);
    if((fabsl(pchute-pchuteAux)/pchute)<0.001 && val < -0.9e10 ){
  	  pchute=1.05*pchute;
  	  if(celula[0].acsr.tipo == 3 && (celula[0].acsr.iprvap.Pres - pchute) > -(*vg1dSP).localtiny)
  		  pchute = 1.001 * celula[0].acsr.iprvap.Pres;
  	  pchuteAux=pchute;
    }

    val = marchaProdPerm1(pchute);
    kontaiter++;
  }

  kontaiter = 0;
  pchute2 = pchute;
  double chutelim = 0.;
  double chuteNeg;
  double chutePos;
  if (val < 0.) {
	chuteNeg=pchute;
    while (val < 0) {
      pchuteAux = pchute2;
      if ((*vg1dSP).chaverede == 0) pchute2 *= 0.9;
      else pchute2 *= mult1;
      if (pchute2 <= chutelim) pchute2 = 0.5 * (pchuteAux + chutelim);
      if(celula[0].acsr.tipo == 11 && (celula[0].acsr.iprvap.Pres - pchute2) > -(*vg1dSP).localtiny)
    	  pchute2 = 1.001 * celula[0].acsr.iprvap.Pres;
      val = marchaProdPerm2(pchute2);
      kontaiter++;
      if (kontaiter > 200) {
        if ((*vg1dSP).chaverede == 0) NumError(
            "Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm2 atingiu m�ximo de itera��es");
        else {
          if ((*vg1dSP).iterRede > 0) return -1.1e6;
          else return 1.e6;
        }
      }
      while (val < -0.9e10) {
        chutelim = pchute2;
        pchute2 = 0.5 * (pchute2 + pchuteAux);
        if(celula[0].acsr.tipo == 3 && (celula[0].acsr.iprvap.Pres - pchute2) >-(*vg1dSP).localtiny)
        	pchute2 = 1.001 * celula[0].acsr.iprvap.Pres;
        //if(fabsl(pchute2-pchuteAux)<(*vg1dSP).localtiny)pchute2=0.9*pchuteAux;
        val = marchaProdPerm2(pchute2);
        kontaiter++;
        if (kontaiter > 200) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm2 atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }
      }
    }
    chutePos=pchute2;
  } else if (val > 0.) {
	  chutePos=pchute;
    while (val > 0) {
      pchuteAux = pchute2;
      if ((*vg1dSP).chaverede == 0) pchute2 *= 1.1;
      else pchute2 *= mult2;
      if(celula[0].acsr.tipo == 3 && (celula[0].acsr.iprvap.Pres - pchute2) > -(*vg1dSP).localtiny)
    	  pchute2 = 1.001 * celula[0].acsr.iprvap.Pres;
      val = marchaProdPerm2(pchute2);
      kontaiter++;
      if (kontaiter > 800) {
        if ((*vg1dSP).chaverede == 0) NumError(
            "Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm2 atingiu m�ximo de itera��es");
        else {
          if ((*vg1dSP).iterRede > 0) return -1.1e6;
          else return 1.e6;
        }
      }
      while (val > 0.9e10) {
        chutelim = pchute2;
        pchute2 = 0.5 * (pchute2 + pchuteAux);
        if(celula[0].acsr.tipo == 11 && (celula[0].acsr.iprvap.Pres - pchute2) > -(*vg1dSP).localtiny)
        	pchute2 = 1.001 * celula[0].acsr.iprvap.Pres;
        //if(fabsl(pchute2-pchuteAux)<(*vg1dSP).localtiny)pchute2=1.1*pchuteAux;
        val = marchaProdPerm2(pchute2);
        kontaiter++;

        if (kontaiter > 800) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPfundoPerm2 atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }

      }
    }
    chuteNeg=pchute2;
  }

  return zriddr(chuteNeg, chutePos, 1, 1);
}

double SProdVap::marchaProdPresPres1(double mchute) {

  int corrigechute = 1;

  if (celula[0].acsr.tipo == 12) {
    celula[0].temp = celula[0].acsr.injmvap.temp;
    celula[0].acsr.injmvap.Mass = mchute;
  }
  celula[0].tempL = celula[0].temp;
  celula[1].tempL = celula[0].temp;
  celula[0].tempini = celula[0].temp;

  celula[0].ML = 0.;
  celula[0].MC = 0.;
  celula[1].ML = 0.;
  celula[0].MliqiniL = 0.;
  celula[0].Mliqini = 0.;
  celula[1].MliqiniL = 0.;
  celula[0].QLL = 0.;
  celula[0].QL = 0;
  celula[1].QLL = 0.;
  celula[0].QG = 0.;

  int i;

  celula[0].presauxL = celula[0].pres;
  celula[0].presLini = celula[0].pres;
  celula[0].presL = celula[0].pres;
  //celula[0].pres=pchute;
  celula[1].presL = celula[0].pres;
  celula[0].presini = celula[0].pres;
  celula[1].presLini = celula[0].pres;
  celula[0].presaux = celula[0].pres;
  celula[1].presauxL = celula[0].pres;
  if(celula[0].acsr.tipo == 2){
     if (iterperm == 0) {
       celula[0].alf = 0.;
       celula[0].alfini = 0.;
       celula[1].alfL = celula[0].alf;
       celula[1].alfLini = celula[0].alf;
       celula[0].alfPigD = celula[0].alf;
       celula[0].alfPigDini = celula[0].alf;
       celula[0].alfPigE = celula[0].alf;
       celula[0].alfPigEini = celula[0].alf;
     } else {
       celula[0].alf = celula[1].alf;
       celula[0].alfini = celula[1].alf;
       celula[1].alfL = celula[0].alf;
       celula[1].alfLini = celula[0].alf;
       celula[0].alfPigD = celula[0].alf;
       celula[0].alfPigDini = celula[0].alf;
       celula[0].alfPigE = celula[0].alf;
       celula[0].alfPigEini = celula[0].alf;
     }
  }
  else{
	     if (iterperm == 0) {
	       celula[0].alf = 1.;
	       celula[0].alfini = 1.;
	       celula[1].alfL = celula[0].alf;
	       celula[1].alfLini = celula[0].alf;
	       celula[0].alfPigD = celula[0].alf;
	       celula[0].alfPigDini = celula[0].alf;
	       celula[0].alfPigE = celula[0].alf;
	       celula[0].alfPigEini = celula[0].alf;
	     } else {
	       celula[0].alf = celula[1].alf;
	       celula[0].alfini = celula[1].alf;
	       celula[1].alfL = celula[0].alf;
	       celula[1].alfLini = celula[0].alf;
	       celula[0].alfPigD = celula[0].alf;
	       celula[0].alfPigDini = celula[0].alf;
	       celula[0].alfPigE = celula[0].alf;
	       celula[0].alfPigEini = celula[0].alf;
	     }
  }
  i = 1;
  while (i <= ncel && celula[i - 1].pres >= 1.) {
	calcHmixPerm(i);
    RenovaPresPermMon(i, 0);
    atualizaPeriPmonProd(i);
    RenovaMassPerm(i);
    //RenovaTempPerm(i, 0);
    calcTempEntalpPerm(i);
    atualizaPeriTempProd(i);
    RenovaPresPermJus(i, 0);
    atualizaPeriPjusProd(i);
    RenovaTransMassPerm(i - 1);
    if (arq.ordperm > 1) {
      double D0presaux = celula[i].presaux - celula[i - 1].pres;
      double D0pres = celula[i].pres - celula[i].presaux;
      double D0temp = celula[i].temp - celula[i - 1].temp;
      RenovaPresPermMon(i, 1);
      atualizaPeriPmonProd(i);
      RenovaMassPerm(i);
      //RenovaTempPerm(i, 1);
      calcTempEntalpPerm(i);
      RenovaPresPermJus(i, 1);
      celula[i].pres = 0.5 * (celula[i].presaux + D0pres + celula[i].pres);
      celula[i].presaux = 0.5 * (celula[i - 1].pres + D0presaux + celula[i].presaux);
      celula[i].temp = 0.5 * (celula[i - 1].temp + D0temp + celula[i].temp);
      atualizaPeriPjusProd(i);
      atualizaPeriPmonProd(i);
      atualizaPeriTempProd(i);
      RenovaMassPerm(i);
      RenovaTransMassPerm(i - 1);

    }
    celula[i].rpC = celula[i].rpCi = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp);
    celula[i].rpL = celula[i].rpLi = celula[i].flui.MasEspLiq(celula[i].presL, celula[i].tempL);
    celula[i - 1].rpR = celula[i - 1].rpRi = celula[i].rpC;
    i++;
    if (celula[i - 1].pres <= 1) return -1e10;
  }

  iterperm++;

  return celula[ncel].pres - pGSup;
}

double SProdVap::buscaProdPresPresPerm(double chute) {

  double mchute = chute;
  double val = marchaProdPresPres1(mchute);
  int kontaiter=0;
  if (val < -0.9e10) {
    while (val < -0.9e10 && kontaiter<800) {
        mchute *= 0.9;
        val = marchaProdPresPres1(mchute);
        if (mchute < 1e-5) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }
      }
      val = marchaProdPresPres1(mchute);
      kontaiter++;
  }
  double mchuteAux;
  double mchute2 = mchute;
  double chutelim = 0.;
  kontaiter = 0;

  double chuteNeg;
  double chutePos;

  if (val < 0.) {
	chuteNeg=mchute;
    while (val < 0) {
      mchuteAux = mchute2;
      mchute2 *= 0.9;
      val = marchaProdPresPres1(mchute2);
      kontaiter++;
      if (kontaiter > 200) {
        if ((*vg1dSP).chaverede == 0) NumError(
            "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
        else {
          if ((*vg1dSP).iterRede > 0) return -1.1e6;
          else return 1.e6;
        }
      }
      while (val < -0.9e10) {
        chutelim = mchute2;
        mchute2 = 0.5 * (mchute2 + mchuteAux);
        val = marchaProdPresPres1(mchute2);
        kontaiter++;
        if (kontaiter > 200) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }
      }
    }
    chutePos=mchute2;
  } else if (val > 0.) {
	chutePos=mchute;
    while (val > 0) {
      mchuteAux = mchute2;
      mchute2 *= 1.1;

      val = marchaProdPresPres1(mchute2);
      kontaiter++;
      if (kontaiter > 200) {
        if ((*vg1dSP).chaverede == 0) NumError(
            "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm  atingiu m�ximo de itera��es");
        else {
          if ((*vg1dSP).iterRede > 0) return -1.1e6;
          else return 1.e6;
        }
      }
      while (val < -0.9e10) {
        chutelim = mchute2;
        mchute2 = 0.5 * (mchute2 + mchuteAux);

        val = marchaProdPresPres1(mchute2);
        kontaiter++;

        if (kontaiter > 200) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }

      }
    }
    chuteNeg=mchute2;
  }

  return zriddr(chuteNeg, chutePos, 2, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double SProdVap::marchaProdPresPres2(double mchute) {

  int corrigechute = 1;

 if (celula[0].acsr.tipo == 12) {
    celula[0].temp = celula[0].acsr.injmvap.temp;
    celula[0].acsr.injmvap.Mass = mchute;
  }
  celula[0].tempL = celula[0].temp;
  celula[1].tempL = celula[0].temp;
  celula[0].tempini = celula[0].temp;

  celula[0].ML = 0.;
  celula[0].MC = 0.;
  celula[1].ML = 0.;
  celula[0].MliqiniL = 0.;
  celula[0].Mliqini = 0.;
  celula[1].MliqiniL = 0.;
  celula[0].QLL = 0.;
  celula[0].QL = 0;
  celula[1].QLL = 0.;
  celula[0].QG = 0.;

  int i;

  celula[0].presauxL = celula[0].pres;
  celula[0].presLini = celula[0].pres;
  celula[0].presL = celula[0].pres;
  //celula[0].pres=pchute;
  celula[1].presL = celula[0].pres;
  celula[0].presini = celula[0].pres;
  celula[1].presLini = celula[0].pres;
  celula[0].presaux = celula[0].pres;
  celula[1].presauxL = celula[0].pres;
  if (iterperm == 0) {
    celula[0].alf = 0.;
    celula[0].alfini = 0.;
    celula[1].alfL = celula[0].alf;
    celula[1].alfLini = celula[0].alf;
    celula[0].alfPigD = celula[0].alf;
    celula[0].alfPigDini = celula[0].alf;
    celula[0].alfPigE = celula[0].alf;
    celula[0].alfPigEini = celula[0].alf;
  } else {
    celula[0].alf = celula[1].alf;
    celula[0].alfini = celula[1].alf;
    celula[1].alfL = celula[0].alf;
    celula[1].alfLini = celula[0].alf;
    celula[0].alfPigD = celula[0].alf;
    celula[0].alfPigDini = celula[0].alf;
    celula[0].alfPigE = celula[0].alf;
    celula[0].alfPigEini = celula[0].alf;
  }
  i = 1;
  while (i <= ncel && celula[i - 1].pres >= 1.) {
	calcHmixPerm(i);
    RenovaPresPermMon(i, 0);
    atualizaPeriPmonProd(i);
    RenovaMassPerm(i);
    //RenovaTempPerm(i, 0);
    calcTempEntalpPerm(i);
    atualizaPeriTempProd(i);
    RenovaPresPermJus(i, 0);
    atualizaPeriPjusProd(i);
    RenovaTransMassPerm(i - 1);
    if (arq.ordperm > 1) {
      double D0presaux = celula[i].presaux - celula[i - 1].pres;
      double D0pres = celula[i].pres - celula[i].presaux;
      double D0temp = celula[i].temp - celula[i - 1].temp;
      RenovaPresPermMon(i, 1);
      atualizaPeriPmonProd(i);
      RenovaMassPerm(i);
      //RenovaTempPerm(i, 1);
      calcTempEntalpPerm(i);
      RenovaPresPermJus(i, 1);
      celula[i].pres = 0.5 * (celula[i].presaux + D0pres + celula[i].pres);
      celula[i].presaux = 0.5 * (celula[i - 1].pres + D0presaux + celula[i].presaux);
      celula[i].temp = 0.5 * (celula[i - 1].temp + D0temp + celula[i].temp);
      atualizaPeriPjusProd(i);
      atualizaPeriPmonProd(i);
      atualizaPeriTempProd(i);
      RenovaMassPerm(i);
      RenovaTransMassPerm(i - 1);

    }
    celula[i].rpC = celula[i].rpCi = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp);
    celula[i].rpL = celula[i].rpLi = celula[i].flui.MasEspLiq(celula[i].presL, celula[i].tempL);
    celula[i - 1].rpR = celula[i - 1].rpRi = celula[i].rpC;

    i++;
    if (celula[i - 1].pres <= pGSup) return -1e10;
  }

  double tESup = celula[ncel].temp;
  double alfSup = celula[ncel].alf;

  double masentrada = celula[ncel - 1].MR;
  double massgas = celula[ncel - 1].MR - celula[ncel - 1].MliqiniR;
  double maxSup = 0.;
  double chokemas = 0;

  double tit;
  presfim = celula[ncel].pres;
  double rholp = celula[ncel].flui.MasEspLiq(celula[ncel].pres, celula[ncel].temp);
  double rholmix = rholp;
  tit = fabsl(massgas / masentrada);

  double masChk;

  double ypres = pGSup / presfim;
  masChk = chokeSup.vazmassSachdVap(ypres, presfim, tESup, alfSup, tit);
  maxSup = chokeSup.vazmaxSachdVap(presfim, tESup, alfSup, tit);
  if (fabsl(ypres) > fabsl(chokeSup.razpres)) maxSup = masChk;

  iterperm++;

  return maxSup - celula[ncel - 1].MR;
}

double SProdVap::buscaProdPresPresPerm2(double chute) {

  double mchute = chute;
  double val = marchaProdPresPres2(mchute);
  int kontaiter=0;
  if (val < -0.9e10) {
    while (val < -0.9e10 && kontaiter<400) {
        mchute *= 0.9;
        val = marchaProdPresPres2(mchute);
        if (mchute < 1e-5) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }
      val = marchaProdPresPres2(mchute);
      kontaiter++;
    }
  }
  double mchuteAux;
  double mchute2 = mchute;
  double chutelim = 0.;
  kontaiter = 0;

  double chuteNeg;
  double chutePos;

  if (val < 0.) {
	  chuteNeg=mchute;
    while (val < 0) {
      mchuteAux = mchute2;
      mchute2 *= 0.9;
      val = marchaProdPresPres1(mchute2);
      kontaiter++;
      if (kontaiter > 200) {
        if ((*vg1dSP).chaverede == 0) NumError(
            "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
        else {
          if ((*vg1dSP).iterRede > 0) return -1.1e6;
          else return 1.e6;
        }
      }
      while (val < -0.9e10) {
        chutelim = mchute2;
        mchute2 = 0.5 * (mchute2 + mchuteAux);
        val = marchaProdPresPres1(mchute2);
        kontaiter++;
        if (kontaiter > 200) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }
      }
    }
    chutePos=mchute2;
  } else if (val > 0.) {
	  chutePos=mchute;
    while (val > 0) {
      mchuteAux = mchute2;
      mchute2 *= 1.1;
      val = marchaProdPresPres1(mchute2);
      kontaiter++;
      if (kontaiter > 200) {
        if ((*vg1dSP).chaverede == 0) NumError(
            "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm  atingiu m�ximo de itera��es");
        else {
          if ((*vg1dSP).iterRede > 0) return -1.1e6;
          else return 1.e6;
        }
      }
      while (val < -0.9e10) {
        chutelim = mchute2;
        mchute2 = 0.5 * (mchute2 + mchuteAux);
        val = marchaProdPresPres1(mchute2);
        kontaiter++;

        if (kontaiter > 200) {
          if ((*vg1dSP).chaverede == 0) NumError(
              "Busca de valores iniciais para cálculo de zero de função em buscaProdPresPresPerm atingiu m�ximo de itera��es");
          else {
            if ((*vg1dSP).iterRede > 0) return -1.1e6;
            else return 1.e6;
          }
        }

      }
    }
    chuteNeg=mchute2;
  }
  return zriddr(chuteNeg, chutePos, 2, 1);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SProdVap::RenovaPresPermMon(int i, int RK) {

  double dx = 0.5 * celula[i].dxL;
  double dia = celula[i].dutoL.a;
  double area = 0.25 * M_PI * dia * dia;
  double si = celula[i].dutoL.peri;
  if (RK == 0) {
    double alfmed = celula[i - 1].alf;
    double rhog = celula[i - 1].flui.MasEspGas(celula[i - 1].pres, celula[i - 1].temp);
    double rhol = celula[i - 1].flui.MasEspLiq(celula[i - 1].pres, celula[i - 1].temp);
    double ugsmed = (celula[i - 1].MC - celula[i - 1].Mliqini) / (area * rhog);
    double ulsmed = celula[i - 1].Mliqini / (area * rhol);
    double j = ugsmed + ulsmed;

    double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    double viscmix = alfmed * celula[i - 1].flui.ViscGas(celula[i - 1].pres, celula[i - 1].temp)
        + (1 - alfmed)
            * (celula[i - 1].flui.ViscLiq(celula[i - 1].pres, celula[i - 1].temp));

    double re1;
    if (celula[i].dutoL.revest == 0) re1 = celula[i - 1].Rey(celula[i].dutoL.a, j, rhomix, viscmix);
    else {
      double dhid = 4 * area / si;
      re1 = celula[i - 1].Rey(dhid, j, rhomix, viscmix);
    }
    double f1 = celula[i - 1].fric(re1, celula[i].dutoL.rug / dia);
    double gradfric = 0.5 * f1 * rhomix * (fabsl(j) * j) * si * dx / area;
    double gradhidro = 9.82 * sinl(celula[i].dutoL.teta) * rhomix * dx;
    celula[i].presaux = celula[i - 1].pres - (gradfric + gradhidro) / 98066.5;
  } else {
    double alfmed = celula[i].alf;
    double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
    double tmed = razdx * celula[i].temp + (1. - razdx) * celula[i - 1].temp;
    double pmed = celula[i].presaux;
    double rhog = celula[i - 1].flui.MasEspGas(pmed, tmed);
    double rhol = celula[i - 1].flui.MasEspLiq(pmed, tmed);
    double ugsmed = (celula[i].QG) / (area);
    double ulsmed = celula[i].QL / (area);
    double j = ugsmed + ulsmed;

    double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    double viscmix = alfmed * celula[i - 1].flui.ViscGas(pmed, tmed)
        + (1 - alfmed)
            * (celula[i - 1].flui.ViscLiq(pmed, tmed));

    double re1;
    if (celula[i].dutoL.revest == 0) re1 = celula[i - 1].Rey(celula[i].dutoL.a, j, rhomix, viscmix);
    else {
      double dhid = 4 * area / si;
      re1 = celula[i - 1].Rey(dhid, j, rhomix, viscmix);
    }
    double f1 = celula[i - 1].fric(re1, celula[i].dutoL.rug / dia);
    double gradfric = 0.5 * f1 * rhomix * (fabsl(j) * j) * si * dx / area;
    double gradhidro = 9.82 * sinl(celula[i].dutoL.teta) * rhomix * dx;
    celula[i].presaux = celula[i - 1].pres - (gradfric + gradhidro) / 98066.5;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}




void SProdVap::RenovaPresPermJus(int i, int RK) {

  double dx;
  double dia;
  double area;
  double si;
  double alfmed;
  double betmed;
  double rhog;
  double rhol;
  double ugsmed;
  double ulsmed;
  double j;

  double rhomix;
  double viscmix;
  double re1;
  double f1;
  double gradfric;
  double gradhidro;

  double tmed;

  dx = 0.5 * celula[i].dx;
  dia = celula[i].duto.a;
  area = 0.25 * M_PI * dia * dia;
  si = celula[i].duto.peri;
  if (RK == 0) {
    alfmed = celula[i].alf;
    double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
    tmed = razdx * celula[i].temp + (1. - razdx) * celula[i - 1].temp;
    double pmed = celula[i].presaux + celula[i - 1].dpB / 98066.5;
    rhog = celula[i].flui.MasEspGas(pmed, tmed);
    rhol = celula[i].flui.MasEspLiq(pmed, tmed);
    ugsmed = (celula[i].QG) / (area);
    ulsmed = celula[i].QL / (area);
    j = ugsmed + ulsmed;

    rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    viscmix = alfmed * celula[i].flui.ViscGas(pmed, tmed)
        + (1 - alfmed)
            * (celula[i].flui.ViscLiq(pmed, tmed));

    if (celula[i].duto.revest == 0) re1 = celula[i].Rey(celula[i].duto.a, j, rhomix, viscmix);
    else {
      double dhid = 4 * area / si;
      re1 = celula[i].Rey(dhid, j, rhomix, viscmix);
    }
    f1 = celula[i].fric(re1, celula[i].duto.rug / dia);
    gradfric = 0.5 * f1 * rhomix * (fabsl(j) * j) * si * dx / area;
    gradhidro = 9.82 * sinl(celula[i].duto.teta) * rhomix * dx;

    celula[i].pres = pmed - (gradfric + gradhidro) / 98066.5;
  } else {
    alfmed = celula[i].alf;
    double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
    tmed = celula[i].temp;
    double pmed = celula[i].pres;
    rhog = celula[i].flui.MasEspGas(pmed, tmed);
    rhol = celula[i].flui.MasEspLiq(pmed, tmed);
    ugsmed = (celula[i].MC - celula[i].Mliqini) / (area * rhog);
    ulsmed = celula[i].Mliqini / (area * rhol);
    j = ugsmed + ulsmed;

    rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    viscmix = alfmed * celula[i].flui.ViscGas(pmed, tmed)
        + (1 - alfmed)
            * (celula[i].flui.ViscLiq(pmed, tmed));

    if (celula[i].duto.revest == 0) re1 = celula[i].Rey(celula[i].duto.a, j, rhomix, viscmix);
    else {
      double dhid = 4 * area / si;
      re1 = celula[i].Rey(dhid, j, rhomix, viscmix);
    }
    f1 = celula[i].fric(re1, celula[i].duto.rug / dia);
    gradfric = 0.5 * f1 * rhomix * (fabsl(j) * j) * si * dx / area;
    gradhidro = 9.82 * sinl(celula[i].duto.teta) * rhomix * dx;

    celula[i].pres = celula[i].presaux + celula[i - 1].dpB / 98066.5 - (gradfric + gradhidro) / 98066.5;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
}



void SProdVap::RenovaMassPerm(int i) {
  renovaFonte(i - 1);
  celula[i].fontemassLL = celula[i-1].fontemassLR;
  celula[i].fontemassGL = celula[i-1].fontemassGR;

  double tmed;
  if (iterperm != 0) tmed = (celula[i].dx * celula[i].temp + celula[i].dxL * celula[i].tempL)
      / (celula[i].dx + celula[i].dxL);
  else tmed = celula[i - 1].temp;

  celula[i].MC = celula[i - 1].MC + celula[i - 1].fontemassLR + celula[i - 1].fontemassGR;
  celula[i - 1].MR = celula[i].MC;
  if (i < ncel) celula[i + 1].ML = celula[i].MC;
  celula[i - 1].MRini = celula[i - 1].MR;

  double pmed = celula[i].presaux;
  double rhog = celula[i].flui.MasEspGas(pmed, tmed);
  double rhol= celula[i].flui.MasEspLiq(pmed, tmed);
  double raztit;
  double t0=celula[i].flui.TempFun(pmed, celula[i].entalp/raztit,HENT);

  if(iterperm!=0){
	  raztit=(celula[i].QG* celula[i].flui.MasEspGas(pmed, t0)+
			  celula[i].QL* celula[i].flui.MasEspLiq(pmed, t0))/celula[i].duto.area;
  }
  else{
	  raztit=(celula[i-1].QG* celula[i].flui.MasEspGas(pmed, t0)+
			  celula[i-1].QL* celula[i].flui.MasEspLiq(pmed, t0))/celula[i].duto.area;
  }
  double tit=celula[i].flui.Titulo(pmed, celula[i].entalp/raztit,HENT);
  celula[i].Mliqini = celula[i].MC*(1.-tit);
  celula[i].QL = celula[i].Mliqini / rhol;
  celula[i].QG = (celula[i].MC - celula[i].Mliqini) / rhog;

  if (celula[i].QG < (*vg1dSP).localtiny) {
    celula[i].alf = 0.;
    celula[i].alfini = celula[i].alf;
    celula[i - 1].alfR = celula[i].alf;
    celula[i - 1].alfRini = celula[i].alf;
    if (i < ncel) celula[i + 1].alfL = celula[i].alf;
    if (i < ncel) celula[i + 1].alfLini = celula[i].alf;
    celula[i].alfPigD = celula[i].alf;
    celula[i].alfPigDini = celula[i].alf;
    celula[i].alfPigE = celula[i].alf;
    celula[i].alfPigEini = celula[i].alf;
  } else if (celula[i].QL < (*vg1dSP).localtiny) {
    celula[i].alf = 1.;
    celula[i].alfini = celula[i].alf;
    celula[i - 1].alfR = celula[i].alf;
    celula[i - 1].alfRini = celula[i].alf;
    if (i < ncel) celula[i + 1].alfL = celula[i].alf;
    if (i < ncel) celula[i + 1].alfLini = celula[i].alf;
    celula[i].alfPigD = celula[i].alf;
    celula[i].alfPigDini = celula[i].alf;
    celula[i].alfPigE = celula[i].alf;
    celula[i].alfPigEini = celula[i].alf;
    celula[i].c0 = 1.;
    celula[i].ud = 0.;
  } else {
    double c0 = 1.;
    double ud = 0.;
    if(celula[i].QL > (*vg1dSP).localtiny){
      if (iterperm == 0) {
        if (fabsl(celula[i].QG + celula[i].QL) > (*vg1dSP).localtiny) celula[i].alf = celula[i].QG / (celula[i].QG + celula[i].QL);
        else celula[i].alf = 0.;
        celula[i].alfini = celula[i].alf;
        celula[i - 1].alfR = celula[i].alf;
        celula[i - 1].alfRini = celula[i].alf;
        if (i < ncel) celula[i + 1].alfL = celula[i].alf;
        if (i < ncel) celula[i + 1].alfLini = celula[i].alf;
        celula[i].alfPigD = celula[i].alf;
        celula[i].alfPigDini = celula[i].alf;
        celula[i].alfPigE = celula[i].alf;
        celula[i].alfPigEini = celula[i].alf;
      }
      if(fabsl(rhog)/rhol>0.9){
    	  c0=1.;
    	  ud=0.;
      }
      else if (fabsl(celula[i].QG) > (*vg1dSP).localtiny && fabsl(celula[i].QL) > (*vg1dSP).localtiny) CalcC0UdPerm(i, c0, ud);
      celula[i].c0 = c0;
      celula[i].ud = ud;
      double area = celula[i].duto.area;
      if (fabsl(celula[i].QG + celula[i].QL) > (*vg1dSP).localtiny) celula[i].alf = celula[i].QG
          / (c0 * (celula[i].QG + celula[i].QL) + ud * area);
      else celula[i].alf = 0.;
      if (celula[i].alf > (1 - (*vg1dSP).localtiny)) celula[i].alf = celula[i].QG / (celula[i].QG + celula[i].QL);
    }
    else{
  	  c0=1.;
  	  ud=0.;
  	  celula[i].alf = 1.;
    }
    celula[i].alfini = celula[i].alf;
    celula[i - 1].alfR = celula[i].alf;
    celula[i - 1].alfRini = celula[i].alf;
    if (i < ncel) celula[i + 1].alfL = celula[i].alf;
    if (i < ncel) celula[i + 1].alfLini = celula[i].alf;
    celula[i].alfPigD = celula[i].alf;
    celula[i].alfPigDini = celula[i].alf;
    celula[i].alfPigE = celula[i].alf;
    celula[i].alfPigEini = celula[i].alf;
  }

}


void SProdVap::CalcC0UdPerm(int ind, double& c0, double& ud) {

  c0 = 1.;
  ud = 0.;
  if (celula[ind].acsr.tipo != 4 || celula[ind].acsr.bcs.freqnova <= 1.) {
    double hns;
    double razdx = celula[ind].dx / (celula[ind].dx + celula[ind].dxL);
    double razdx0;
    if (ind > 0) razdx0 = celula[ind - 1].dx / (celula[ind - 1].dx + celula[ind - 1].dxL);
    else razdx0 = razdx;
    hns = 1. - celula[ind].alf;

    double hol0 = hns;
    double alf0 = 1 - hol0;
    double razdx1 = celula[ind].dxR / (celula[ind].dxR + celula[ind].dx);
    double alf1;
    alf1 = celula[ind].alf;

    double alfneg;
    if (ind > 1) alfneg = celula[ind - 2].alf;
    else if (ind > 0) alfneg = celula[ind - 1].alf;
    else alfneg = celula[ind].alf;

    double pmed;
    double pmed0;

    pmed = celula[ind].presaux;
    if (ind > 0) pmed0 = celula[ind - 1].presaux;
    else pmed0 = celula[ind].presaux;
    double tmed;
    if (iterperm != 0) tmed = razdx * celula[ind].temp + (1 - razdx) * celula[ind].tempL;
    else tmed = celula[ind].tempL;
    double tmed0;
    if (ind > 0) tmed0 = razdx0 * celula[ind - 1].temp + (1 - razdx0) * celula[ind - 1].tempL;
    else tmed0 = tmed;

    double rlm;
    double viscl1;
    double tensup1;
    rlm = celula[ind ].flui.MasEspLiq(pmed, tmed);
    viscl1 = celula[ind ].flui.ViscLiq(pmed, tmed);
    tensup1 = celula[ind ].flui.TensSupFunc(pmed, tmed);

    double rgm;
    double viscg1;
    rgm =celula[ind ].flui.MasEspGas(pmed, tmed);
    viscg1 = celula[ind ].flui.ViscGas(pmed, tmed);

    double ug1 = (celula[ind].MC - celula[ind].Mliqini) / rgm;
    double ul1 = celula[ind].Mliqini / rlm;
    double dia1 = celula[ind].duto.a;
    if (ind > 0 && ug1 >= 0) dia1 = celula[ind - 1].duto.a;
    double A1 = M_PI * dia1 * dia1 / 4.;

    double rmed = hns * rlm + (1 - hns) * rgm;
    double visc = (hns * viscl1 + (1 - hns) * viscg1) / pow(10., 3.);
    double nrey = dia1 * rmed * (fabsl(ug1) / A1 + fabsl(ul1) / A1) / visc;

    int xarr1 = 1;
    double dtot = celula[ind].dxL + celula[ind].dx + 0 * celula[ind].dxR;
    double razL = celula[ind].dxL;
    double raz = celula[ind].dx;
    double razR = 0 * celula[ind].dxR;
    double ang = (razL * celula[ind].dutoL.teta + raz * celula[ind].duto.teta + razR * celula[ind].dutoR.teta)
        / dtot;
    if (ind >= 2) {
      if (celula[ind - 2].acsr.tipo == 5 && celula[ind - 2].acsr.chk.AreaGarg <= (1e-3)) ang = celula[ind].duto.teta;
      else ang = celula[ind].dutoL.teta;
    }
    double sinal = 1.;
    if (ang < 0.) sinal = -1.;
    double xc0 = 2.;
    double xud = sinal * 0.0246 * cosl(ang)
        + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
    if (nrey > 1e-30) {
      if (fabsl(0 * ang + 1 * celula[ind].duto.teta) < 45. * M_PI / 180. && hol0 < 0.99 && hol0 > 0.01
          && ind < ncel - 1) {
        double ug0;
        double ul0;

        ug1 = (celula[ind].MC - celula[ind].Mliqini) / rgm;
        ul1 = celula[ind].Mliqini / rlm;

        ug0=ug1;
        ul0=ul1;
        if(ind>0){
            ug0 = (celula[ind - 1].MC - celula[ind - 1].Mliqini) / celula[ind].flui.MasEspGas(pmed0, tmed0);
            ul0 = celula[ind - 1].Mliqini
                 / (celula[ind].flui.MasEspLiq(pmed0, tmed0));
        }

        estratificado testamapa(dia1, ul1, ug1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, celula[ind].duto.rug / dia1);

        testamapa.mapaTD();

        xarr1 = testamapa.arr;

        if (xarr1 == -1) {
          if (((celula[ind].arranjo != xarr1) || celula[ind].transic > 0)) {
            if ((celula[ind].arranjo != xarr1) && celula[ind].transic > 0) celula[ind].transic = 0;
            celula[ind].transic++;
            if (celula[ind].transic > 19) celula[ind].transic = 0;
          }
          celula[ind].arranjo = xarr1 = testamapa.arr;
          if(ind>0){
              celula[ind - 1].arranjoR = testamapa.arr;
              celula[ind - 1].perdaEstratL = testamapa.fatorperdaLiq;
              celula[ind - 1].perdaEstratG = testamapa.fatorperdaGas;
          }
          sinal = 1.;
          if (ang < 0.) sinal = -1.;
          double xc0C = 2. / (1 + pow(nrey / 1000., 2.))
              + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));
          double udC = sinal * 0.0246 * cosl(ang)
              + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);
          double mult0, mult1;
          mult0 = 1.;
          if (ul0 < 0.) mult0 = 0.;
          mult1 = 0.;
          if (ul1 < 0.) mult1 = 1.;
          double alf0E = celula[ind].alf;
          if(ind>0)alf0E = celula[ind - 1].alf;
          xc0 = 1.;

          c0 = xc0;

          c0 = xc0 = 1.;
          ud = xud;
          if ((fabsl(ug1) + fabsl(ul1)) / A1 > 0.5) {
            c0 = 1.04;
            ud = 0.466 * sinal;
          } else if ((fabsl(ug1) + fabsl(ul1)) / A1 < 0.1) {
            c0 = xc0C;
            ud = udC;
          } else {
            double raz = (0.5 - (fabsl(ug1) + fabsl(ul1)) / A1) / 0.4;
            c0 = ((1. - raz) * 1.04 + raz * xc0C);
            ud = (1. - raz) * 0.466 * sinal + raz * udC;
          }

        }
      }
      if (xarr1 == 1) {

        arranjo testamapa2(dia1, ul1 / A1, ug1 / A1, rlm, rgm, viscl1 / pow(10., 3.), viscg1 / pow(10., 3.), hol0,
            celula[ind].duto.teta, tensup1);
        xarr1 = testamapa2.verificaArr();

        double sinal = 1.;
        if (ang < 0.) sinal = -1.;
        c0 = 2. / (1 + pow(nrey / 1000., 2.))
            + (1.2 - 0.2 * sqrtl(rgm / rlm) * (1 - expl(-18 * alf0))) / (1 + pow(1000. / nrey, 2.));

        ud = sinal * 0.0246 * cosl(ang) + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(ang);

        if (xarr1 == -2) {
          c0 = 1. + (1. - alf0) / (alf0 + 4. * sqrtl(rgm / rlm));
          ud = sinal * (1. - alf0)
              / (alf0 + 4. * sqrtl(rgm / rlm) * sqrtl(9.82 * dia1 * (rlm - rgm) * (1. - alf0)) / (0.015 * rgm));
        }
        double udM = sinal * 0.0246 * cosl(celula[ind].dutoL.teta)
            + 1.606 * pow(9.82 * tensup1 * (rlm - rgm) / (rlm * rlm), 0.25) * sin(celula[ind].dutoL.teta);
        celula[ind].arranjo = xarr1;
        if(ind>0)
        celula[ind - 1].arranjoR = xarr1;
      }
      celula[ind].c0Spare = c0;
      celula[ind].udSpare = ud;

    }
  }
  if(arq.escorregaPerm==0){
     double ulsmed=celula[ind].QL/celula[ind].duto.area;
     double correcaoUd=1-(ulsmed-0.15)/0.35;
     double correcaoCo=c0-(c0-1)*(ulsmed-0.15)/0.35;
     if(correcaoUd>1.) correcaoUd=1.;
     if(correcaoUd<0.) correcaoUd=0.;
     if(correcaoCo>c0) correcaoCo=c0;
     if(correcaoCo<1) correcaoCo=1;
     c0=correcaoCo;
     ud=ud*correcaoUd;
  }

}

 double SProdVap::calcHmixPerm(int i){

	  double dx = celula[i].dx;
	  double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
	  double dia = celula[i].duto.a;
	  double area = 0.25 * M_PI * dia * dia;
	  double alfmed = celula[i-1].alf;
	  double ugsL;
	  double ulsL;
	  double ugsR;
	  double ulsR;

      /*if(iterperm!=0){
	     ugsL = celula[i].QG / area;
         ulsL = celula[i].QL / area;
         ugsR = celula[i+1].QG / area;
         ulsR = celula[i+1].QL / area;
      }
      else{
 	     ugsL = celula[i-1].QG / area;
         ulsL = celula[i-1].QL / area;
         ugsR = celula[i-1].QG / area;
         ulsR = celula[i-1].QL / area;
      }*/

	  ugsL = celula[i-1].QG / area;
      ulsL = celula[i-1].QL / area;

      double ugsmed=(ugsL);
      double ulsmed=(ulsL);

      double presL= celula[i-1].presaux;
      /*double presR;
      //double presR= celula[i+1].presaux;
      if((celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 +arq.master1.razareaativ) * celula[i].duto.area) ||
    	  (celula[i].acsr.tipo == 4 && celula[i].acsr.bcs.freq>0) ||
		  (celula[i].acsr.tipo == 8 && celula[i].acsr.bvol.freq>0.) ||
		  (celula[i].acsr.tipo == 7 && fabsl(celula[i].acsr.delp)>0.) ){
    	  presR=celula[i].pres+(celula[i].pres-celula[i].presaux)*0.5;
      }*/


      //double tmedL =  celula[i-1].tempL;

      double tmedL;
      if (iterperm != 0) tmedL = (celula[i-1].dx * celula[i-1].temp + celula[i-1].dxL * celula[i-1].tempL)
          / (celula[i-1].dx + celula[i-1].dxL);
      else tmedL = celula[i - 1].temp;


      double rhogL=celula[i].flui.MasEspGas(presL,tmedL,TEMP);
      double rhopL=celula[i].flui.MasEspLiq(presL,tmedL,TEMP);
      double hgL=celula[i].flui.HeGas(presL,tmedL,TEMP);
      double hpL=celula[i].flui.HeLiq(presL,tmedL,TEMP);

    //  double rhogR=celula[i].flui.MasEspGas(presR,tmedR,TEMP);
    //  double rhopR=celula[i].flui.MasEspLiq(presR,tmedR,TEMP);
    //  double hgR=celula[i].flui.HeGas(presR,tmedR,TEMP);
    //  double hpR=celula[i].flui.HeLiq(presR,tmedR,TEMP);

      double fluxHL=rhogL*ugsL*hgL+rhopL*ulsL*hpL;
      //double fluxHR=rhogR*ugsR*hgR+rhopR*ulsR*hpR;
     // double delFlux=(fluxHR-fluxHL)/dx;
      double hidro=(rhogL*ugsmed+ulsmed*rhopL)*sinl(celula[i].duto.teta)*9.82;

      celula[i].calor.Tint = celula[i].tempL;
      celula[i].calor.Vint = ugsmed + ulsmed;
      celula[i].calor.dt = celula[i].dt;
      double cpl = celula[i].flui.CPlFunc(celula[i].presL, celula[i].tempL);
      double cpg = celula[i].flui.CPgFunc(celula[i].presL, celula[i].tempL);
      double condliq =  celula[i].flui.CondLiq(celula[i].presL, celula[i].tempL);
      celula[i].calor.kint = condliq * (1 - alfmed) +
     		  celula[i].flui.CondGas(celula[i].presL, celula[i].tempL) * alfmed;
      celula[i].calor.cpint = cpl * (1 - alfmed) + cpg * alfmed;
      celula[i].calor.rhoint = rhopL * (1 - alfmed) + rhogL * alfmed;
      double viscliq = celula[i].flui.ViscLiq(celula[i].presL, celula[i].tempL);
      celula[i].calor.viscint = viscliq * (1 - alfmed) * 1.e-3
           + celula[i].flui.ViscGas(celula[i].presL, celula[i].tempL) * alfmed * 1.e-3;
      double fluxcal = celula[i].calor.transtrans()/area;


      double fontemassG = 0.;
      double fontemassL = 0.;
      double tfonte = celula[i].temp;
      double hgF;
      double hlF;
      double hcF;

      if (celula[i].acsr.tipo == 12) {
        tfonte = celula[i].acsr.injmvap.temp;
        hgF = celula[i].flui.HeGas(celula[i].pres, tfonte,TEMP);
        hlF = celula[i].flui.HeLiq(celula[i].pres, tfonte,TEMP);
      } else if (celula[i].acsr.tipo == 11) {
        tfonte = celula[i].acsr.iprvap.Tres;
        hgF = celula[i].flui.HeGas(celula[i].pres, tfonte,TEMP);
        hlF = celula[i].flui.HeLiq(celula[i].pres, tfonte,TEMP);
        hcF=0;
      } else if (celula[i].acsrL != 0) {
          hgF = 0.;
          hlF = 0.;
      } else {
        hgF = 0.;
        hlF = 0.;
      }

      fontemassL = 0;
      fontemassG = 0;
      if (celula[i].fontemassLR > 0.) fontemassL = hlF*celula[i].fontemassLR / dx;
      if (fontemassG > 0.)fontemassG = hgF*celula[i].fontemassGR / dx;



	 double buffer=-(-fluxHL/dx+hidro-fluxcal-(fontemassG+fontemassL)/area)*dx;
	 celula[i].entalp=buffer;
	 return buffer;
}


 double SProdVap::calcTempEntalpPerm(int i){
	double dia = celula[i].duto.a;
	double area = 0.25 * M_PI * dia * dia;
    double ugsL = celula[i].QG / area;
    double ulsL = celula[i].QL / area;
    /*double ugsR;
    double ulsR;
    if(iterperm!=0){
      ugsR = celula[i+1].QG / area;
      ulsR = celula[i+1].QL / area;
    }
    else{
        ugsR = celula[i].QG / area;
        ulsR = celula[i].QL / area;
    }*/


    double ugs=ugsL;
    double uls=ulsL;

	double pres=celula[i].presaux;
	double t0;
	if (iterperm != 0) t0 = (celula[i].dx * celula[i].temp + celula[i].dxL * celula[i].tempL)
	      / (celula[i].dx + celula[i].dxL);
	else t0 = celula[i - 1].temp;

	double rhog=celula[i].flui.MasEspGas(pres, t0);
	double rhol=celula[i].flui.MasEspLiq(pres, t0);
	double raztit;
	raztit=ugsL*rhog+ulsL*rhol;
	t0=celula[i].flui.TempFun(pres, celula[i].entalp/raztit,HENT);
	//celula[i].temp=( tmed*(celula[i].dx + celula[i].dxL)-celula[i].dxL * celula[i-1].temp)/celula[i].dx ;
	//celula[i].temp=tmed;

    double t1=t0;
	double energint=celula[i].entalp;
	double energchute=celula[i].flui.entalpmix(pres,t0,uls,ugs);
	double difer=energint-energchute;
	double tneg;
	double tpos;
	if(difer>0.){
		tpos=t0;
		while(difer>0.){
			t1=0.9*t1;
			energchute=celula[i].flui.entalpmix(pres,t1,uls,ugs);
			difer=energint-energchute;
		}
		tneg=t1;
	}
	else{
		tneg=t0;
		while(difer<0){
			t1=1.1*t1;
			energchute=celula[i].flui.entalpmix(pres,t1,uls,ugs);
			difer=energint-energchute;
		}
		tpos=t1;
	}

	double xacc=1e-5;
	int maxit=100;
	double x1=tneg;
	double x2=tpos;
	double fl=energint-celula[i].flui.entalpmix(pres,x1,uls,ugs);
	double fh=energint-celula[i].flui.entalpmix(pres,x2,uls,ugs);
	if(fl>=0.){
		x1=x1-1*(x2-x1);
		fl=energint-celula[i].flui.entalpmix(pres,x1,uls,ugs);
	}
	else if(fh<=0.){
		x2=x2-1*(x1-x2);
		fh=energint-celula[i].flui.entalpmix(pres,x2,uls,ugs);
	}
	if((fl>0.0 && fh<0.0) || (fl<0.0 && fh>0.0)){
		double xl=x1;
		double xh=x2;
		double ans=-1.e20;
		for(int j=0;j<maxit;j++){
			double xm=0.5*(xl+xh);
			double fm=energint-celula[i].flui.entalpmix(pres,xm,uls,ugs);
			double s=sqrtl(fm*fm-fl*fh);
			if(s==0.0) celula[i].temp=ans;
			double xnew=xm+(xm-xl)*((fl>=fh ? 1.0 : -1.0)*fm/s);
			if(fabsl(xnew-ans)<=xacc ){
				celula[i].temp=ans;
			}
			ans=xnew;
			double fnew=energint-celula[i].flui.entalpmix(pres,ans,uls,ugs);
			if(fabsl(fnew)<=xacc ) celula[i].temp=ans;
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
			else celula[i].temp=-100000.;
			if(fabsl(xh-xl)<=xacc ) celula[i].temp=ans;

		}
		celula[i].temp=100000.;
	}
	else {
		if(fabsl(fl)<=xacc) return x1;
		if(fabsl(fh)<=xacc) return x2;
		celula[i].temp= -10000.;
	}

    if (celula[i].temp < -50.) celula[i].temp = -50.;
    if (celula[i].temp > 200.) celula[i].temp = 200.;

    celula[i].temp=( celula[i].temp*(celula[i].dx + celula[i].dxL)-celula[i].dxL * celula[i-1].temp)/
    		         celula[i].dx ;

}


void SProdVap::RenovaTransMassPerm(int i) {

	  double dx = celula[i].dx;
	  double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
	  double dia = celula[i].duto.a;
	  double area = 0.25 * M_PI * dia * dia;
	  double alfmed = celula[i].alf;
	  double ugsmed;

	  if (celula[i].alf > (*vg1dSP).localtiny) ugsmed = celula[i].QG / area;
	  else {
	    ugsmed = 0.;
	  }
	  double ulsmed;
	  if (celula[i].alf < 1. - (*vg1dSP).localtiny) ulsmed = celula[i].QL / area;
	  else {
	    ulsmed = 0.;
	  }
	  double rp = celula[i].flui.MasEspLiq(celula[i].pres, celula[i].temp,TEMP);
	  double rhol = rp;
	  double rhog = celula[i].flui.MasEspGas(celula[i].pres, celula[i].temp,TEMP);
	  double cpl = celula[i].flui.CPlFunc(celula[i].pres, celula[i].temp);
	  double cvl = cpl;
	  double cpg = celula[i].flui.CPgFunc(celula[i].pres, celula[i].temp);
	  double cvg = celula[i].flui.CVgFunc(celula[i].pres, celula[i].temp);
	  double jtl = celula[i].flui.JTlFunc(celula[i].pres, celula[i].temp);
	  double jtg = celula[i].flui.JTgFunc(celula[i].pres, celula[i].temp);
	  double hidro = (rhol * ulsmed + rhog * ugsmed) * area * 9.82 * sinl(celula[i].duto.teta);

	  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  celula[i].calor.Tint = celula[i].temp;
	  celula[i].calor.Vint = ugsmed + ulsmed;
	  celula[i].calor.dt = celula[i].dt;
	  double condliq = celula[i].flui.CondLiq(celula[i].pres, celula[i].temp);
	  celula[i].calor.kint = condliq * (1 - alfmed) +
			                 celula[i].flui.CondGas(celula[i].pres, celula[i].temp) * alfmed;
	  celula[i].calor.cpint = cpl * (1 - alfmed) + cpg * alfmed;
	  celula[i].calor.rhoint = rhol * (1 - alfmed) + rhog * alfmed;
	  double viscliq = celula[i].flui.ViscLiq(celula[i].pres, celula[i].temp);
	  celula[i].calor.viscint = viscliq * (1 - alfmed) * 1.e-3
	      + celula[i].flui.ViscGas(celula[i].pres, celula[i].temp) * alfmed * 1.e-3;
	  double fluxcal = celula[i].calor.transtrans();

	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  double coefTempo = (rhol * (1 - alfmed) * cvl + rhog * alfmed * cvg) * area;
	  double coefPresTempo;
	  coefPresTempo = -celula[i].flui.CPgFuncMod(celula[i].pres, celula[i].temp) * (rhog * alfmed * area);
	  double coefdxT = (rhol * ulsmed * cpl + rhog * ugsmed * cpg) * area;
	  double coefdxP = (rhol * ulsmed * jtl + rhog * ugsmed * jtg) * area;
	  double dpdx;
	  if (i < ncel) dpdx = 2. * (celula[i + 1].presaux - celula[i].pres) * 98066.5 / celula[i].dx;
	  else dpdx = 2. * (celula[i].pres - celula[i].presaux) * 98066.5 / celula[i].dx;
	  if (celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 +arq.master1.razareaativ) * celula[i].duto.area) dpdx = 2.
	      * (celula[i].pres - celula[i].presaux) * 98066.5 / celula[i].dx;
	  celula[i].VTemper = coefdxT / coefTempo;
	  double dtdx = (celula[i].temp - celula[i-1].temp) / dxmed;
	  if (i < ncel) if (celula[i].VTemper < 0) dtdx = (celula[i + 1].temp - celula[i].temp)
	      / (0.5 * (celula[i + 1].dx + celula[i].dx));
	  if (celula[i].acsr.tipo == 5 && celula[i].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i].duto.area
	      && celula[i].VTemper <= 0) dtdx = 0.;
	  if (celula[i - 1].acsr.tipo == 5 && celula[i - 1].acsr.chk.AreaGarg <= (1e-3 + arq.master1.razareaativ) * celula[i - 1].duto.area
	      && celula[i].VTemper >= 0) dtdx = 0.;
	  if ((i == 1 && celula[i].VTemper <= 0) || (i == ncel && celula[i].VTemper <= 0)) dtdx = 0.;
	  if (celula[i].acsr.tipo == 4 && celula[i].acsr.bcs.freqnova > 1.) {
	    dpdx = 0.;
	  }
	  if (celula[i].acsr.tipo == 8 && celula[i].acsr.bvol.freq > 1.) {
	    dpdx = (celula[i].pres - celula[i - 1].pres) * 98066.5 / dxmed;
	  }

	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  double cinetico=0;
	  double ugmed0=0;
	  double ulmed0=0;
	  double ugmed=0;
	  double ulmed;
	  if (celula[i].acsr.tipo == 0 && celula[i - 1].acsr.tipo == 0 && i>1) {

		double dxCin=celula[i-1].dx;
	    double diaaux = celula[i - 1].duto.a;
	    double areaaux = 0.25 * M_PI * diaaux * diaaux;

	    double alf;
	    if(celula[i].QL>0)alf=celula[i-1].alf;
	    else alf=celula[i].alf;
	    double alf0;
	    if(celula[i-1].QL>0)alf0=celula[i-2].alf;
	    else alf0=celula[i-1].alf;


	    if (alf0 > 1e-3){
	    	ugmed0 = celula[i-1].QG / (areaaux);
	    	ugmed0/=alf0;
	    }


	    if (alf0 < 1. - 1e-3){
	    	ulmed0 = celula[i-1].QL / (areaaux);
	    	ulmed0/=(1.-alf0);
	    }

	    if (alf > 1e-3){
	    	ugmed = ugsmed;
	    	ugmed/=alf;
	    }


	    if (alf< 1. - 1e-3){
	    	ulmed = ulsmed ;
	    	ulmed/=(1.-alf);
	    }

	    cinetico = (celula[i].MC-celula[i-1].Mliqini)*ugmed*(ugmed-ugmed0)/dxCin+celula[i-1].Mliqini*ulmed*(ulsmed-ulmed0)/dxCin;

	  }


	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  double fontemassG = 0.;
	    double fontemassL = 0.;
	    double tfonte = celula[i].temp;
	    double cpgF;
	    double razcpF;
	    double cplF;
	    if(celula[i].acsr.tipo==11){
	  	 tfonte = celula[i].acsr.iprvap.Tres;
	  	 cpgF = cpg;
	  	 razcpF = cpg/cvg;
	  	 cplF = cpl;

	    }
	    else if(celula[i].acsr.tipo==12){
	    	tfonte=celula[i].acsr.injmvap.temp;
	      cpgF=cpg;// alteracao7
	      razcpF=cpg/cvg;
	      cplF=cpl;
	    }
	    else if (celula[i].acsrL != 0) {
	        cpgF = 0.;
	        razcpF = 1.;
	        cplF = 0.;
	    } else {
	      cpgF = 0.;
	      razcpF = 1.;
	      cplF = 0.;
	    }

	    fontemassL = 0;
	    if (celula[i].fontemassLR > 0.) fontemassL = celula[i].fontemassLR / dx;
	    fontemassL *= (cplF) * (tfonte - celula[i].temp);

	    fontemassG = celula[i].fontemassGR / dx;
	    if (fontemassG > 0.) fontemassG *= (cpgF / razcpF) * (tfonte - celula[i].temp);
	    else fontemassG = 0;

	    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

	    double latente;
	    latente = (celula[i].flui.HeGas(celula[i].pres, celula[i].temp,TEMP)-
	  		  celula[i].flui.HeLiq(celula[i].pres, celula[i].temp,TEMP)) * celula[i].FonteMudaFase;

	    double alfinter;
	    double alfinterL;
	    if (ugsmed > 0) {
	      alfinter = celula[i].alf;
	      alfinterL = celula[i].alfL;
	    } else {
	      alfinter = celula[i].alfR;
	      alfinterL = celula[i].alf;
	    }
	    double delvel;
	    if (alfinter > (*vg1dSP).localtiny && alfinter < (1. - (*vg1dSP).localtiny)) delvel = ugsmed / alfinter - ulsmed / (1. - alfinter);
	    else if (alfinter > (*vg1dSP).localtiny) delvel = ugsmed;
	    else delvel = ulsmed;
	    double verifica = area * celula[i].pres * 98066.5* delvel * (alfinter - alfinterL) / celula[i].dx;

	    if (celula[i].temp < 0 || celula[i].temp > 100) {
	      int para;
	      para = 0.;
	    }

	  celula[i].FonteMudaFase = (
	      - (coefPresTempo * (celula[i].pres - celula[i].presini) * 98066.5 / celula[i].dt) - coefdxT * dtdx + coefdxP * dpdx
	      - cinetico - (hidro - 0. * verifica) + celula[i - 1].potB / dxmed + fontemassL + fontemassG + fluxcal);// / (coefTempo / celula[i].dt);

	  celula[i].FonteMudaFase /=latente;

	  celula[i].transmassR=celula[i].FonteMudaFase;
	  celula[i + 1].transmassL = celula[i].transmassR;

	}

void SProdVap::atualizaPeriPmonProd(int i) {
  if (celula[i].presaux < 0.99) celula[i].presaux = 0.99;
  celula[i - 1].presauxR = celula[i].presaux;
  if (i < ncel) celula[i + 1].presauxL = celula[i].presaux;

  celula[i - 1].dpB = 0.;
  celula[i - 1].potB = 0.;
  double tmed;
  if (iterperm != 0) tmed = (celula[i].dx * celula[i].temp + celula[i].dxL * celula[i].tempL)
      / (celula[i].dx + celula[i].dxL);
  else
  tmed = celula[i - 1].temp;
  if (celula[i - 1].acsr.tipo == 4 && celula[i - 1].acsr.bcs.freqnova > 1.) {
    double vazmix = celula[i - 1].QL + celula[i - 1].QG;
    double alf0 = celula[i - 1].alf;
    double rhomis = (alf0 * celula[i - 1].flui.MasEspGas(celula[i].presaux, tmed)
        + (1 - alf0)
            * ( celula[i - 1].flui.MasEspLiq(celula[i].presaux, tmed)));
    double vismis = alf0 * celula[i - 1].flui.ViscGas(celula[i].presaux, tmed)
        + (1 - alf0)
            * (celula[i - 1].flui.ViscLiq(celula[i].presaux, tmed));
    vazmix *= (86400 / 0.16);

    celula[i - 1].acsr.bcs.NovaVis(vismis, rhomis, vazmix);
    celula[i - 1].dpB = 0.3048 * celula[i - 1].acsr.bcs.Hvis * rhomis * 9.82;
    celula[i - 1].potB = (1. - celula[i - 1].acsr.bcs.Evis / 100.
        + 100. * (1. - celula[i - 1].acsr.bcs.eficM / 100.) / celula[i - 1].acsr.bcs.eficM)
        * celula[i - 1].acsr.bcs.Pvis * 745.7;

  } else if (celula[i - 1].acsr.tipo == 7) celula[i - 1].dpB = celula[i - 1].acsr.delp * 98066.5;
}
void SProdVap::atualizaPeriPjusProd(int i) {
  if (celula[i].pres < 0.99) celula[i].pres = 0.99;
  celula[i - 1].presR = celula[i].pres;
  if (i < ncel) {
    celula[i + 1].presL = celula[i].pres;
    celula[i + 1].presLini = celula[i + 1].presL;
  }
  celula[i].presini = celula[i].pres;
}
void SProdVap::atualizaPeriTempProd(int i) {
  celula[i - 1].tempR = celula[i].temp;
  if (i < ncel) celula[i + 1].tempL = celula[i].temp;
  celula[i].tempini = celula[i].temp;
}

void SProdVap::calcTempFim() {	    		           //alteracao 1

  if (arq.chokep.abertura[0] <= 0.6 && arq.chokep.abertura[0] > (*vg1dSP).localtiny && presfim<pGSup) {//alteracao8
    double masentrada = celula[ncel - 1].MR;
    double massgas = celula[ncel - 1].MR - celula[ncel - 1].MliqiniR;
    double rholp = celula[ncel].flui.MasEspLiq(celula[ncel].pres, celula[ncel].temp);
    double rholmix = rholp;
    double alfEF = celula[ncel].alf;
    double tit = fabsl(massgas / masentrada);


    double jtlM = celula[ncel].flui.JTlFunc(celula[ncel].pres, celula[ncel].temp);	  //alteraacao2
    double jtgM = celula[ncel].flui.JTgFunc(celula[ncel].pres, celula[ncel].temp);	    		           //alteracao2
    tempSup = celula[ncel].temp + ((1. - tit) * jtlM + tit * jtgM) * (celula[ncel].pres - celula[ncel].pres);	   //alteracao2

  } else tempSup = celula[ncel-1].temp;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double  SProdVap::multMarcha(double chute, int prod, int tipoCC){

	if(prod==1){
		  if(tipoCC==0) return marchaProdPerm1(chute);
		  else  return marchaProdPerm2(chute);
    }
    else{
			if(tipoCC==0) return marchaProdPresPres1(chute);
	}

}


double  SProdVap::zriddr(double x1,double x2,int prod, int tipoCC){
	double xacc=1e-5;
	int maxit=100;
	double fl=multMarcha(x1, prod, tipoCC);
	double fh=multMarcha(x2, prod, tipoCC);
	if(fl>=0.){
		x1=x1-1*(x2-x1);
		fl=multMarcha(x1, prod, tipoCC);
	}
	else if(fh<=0.){
		x2=x2-1*(x1-x2);
		fh=multMarcha(x2, prod, tipoCC);
	}
	if((fl>0.0 && fh<0.0) || (fl<0.0 && fh>0.0)){
		double xl=x1;
		double xh=x2;
		double ans=-1.e20;
		for(int j=0;j<maxit;j++){
			double xm=0.5*(xl+xh);
			double fm=multMarcha(xm, prod, tipoCC);
			double s=sqrtl(fm*fm-fl*fh);
			if(s==0.0) return ans;
			double xnew=xm+(xm-xl)*((fl>=fh ? 1.0 : -1.0)*fm/s);
			if(fabsl(xnew-ans)<=xacc ){
				return ans;
			}
			ans=xnew;
			double fnew=multMarcha(ans, prod, tipoCC);
			if(fabsl(fnew)<=xacc ) return ans;
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
			if(fabsl(xh-xl)<=xacc ) return ans;

		}
		 return 100000.;
	}
	else {
		if(fabsl(fl)<=xacc) return x1;
		if(fabsl(fh)<=xacc) return x2;
		return -10000.;
	}
}


double SProdVap::hidroreverso(double hol, double vaz) {
  double pchute = pGSup;
  double taux;
  celula[ncel].pres = pchute;
  double j = 0.;
  double rmis = 0.;
  double f1 = 0.;

  if (vaz > 0.) {
    taux = arq.celp[0].textern;
    double visP = celula[0].flui.ViscLiq(pchute, taux);
    double visG = celula[0].flui.ViscGas(pchute, taux);
    double visMis = visP ;
    double rP = celula[0].flui.MasEspLiq(pchute, taux);
    double rG = celula[0].flui.MasEspGas(pchute, taux);
    rmis = rP;
    double rlpA = celula[0].flui.MasEspLiq(1., 15.);

    //////////////////////////////////////////////////////////////////////////////////////////////
    double massic = rlpA * vaz;
    double Rhogs = celula[0].flui.MasEspGas(1., 15);;    //cel[ind].acsr.injl.FluidoPro.MasEspGas(1, 15);
    double Rhols = rlpA;

    massic=vaz;
    celula[0].flui.verificaFase(pchute, taux);
    double fracmasshidra;
    if(celula[0].flui.fase==0)fracmasshidra=0.;
    else if(celula[0].flui.fase==1)fracmasshidra=1.;
    else fracmasshidra=0.5;
    double massicP = (1. - fracmasshidra) * massic;
    double massicG = fracmasshidra * massic;
    j = (massicP / rP + massicG / rG) / celula[0].duto.area;
    double alfmis = (massicG / rG) / (massicP / rP+ massicG / rG);
    rmis = (1 - alfmis) * rmis + alfmis * rG;
    visMis = (1 - alfmis) * visMis + alfmis * visG;
    double re;
    if (celula[0].duto.revest == 0) re = celula[0].Rey(celula[0].duto.a, j, rmis, visMis);
    else {
      double dhid = 4 * celula[0].duto.area / celula[0].duto.peri;
      re = celula[0].Rey(dhid, j, rmis, visMis);
    }
    f1 = celula[0].fric(re, celula[0].duto.rug / celula[0].duto.a);
  }
  double perdafric = (f1 * rmis * j * fabsl(j) / 2.) * celula[0].duto.peri / celula[0].duto.area;
  for (int i = ncel; i > 0; i--) {
    taux = arq.celp[i].textern;
    double rhol = celula[i].flui.MasEspLiq(pchute, taux);
    double rhog = celula[i].flui.MasEspGas(pchute, taux);
    double alfa = 1. - hol;
    double rhomix = (1. - alfa) * rhol + alfa * rhog;
    double dxmed = 0.5 * (celula[i].dx + celula[i - 1].dx);
    pchute += ((rhomix * 9.81 * sinl(celula[i].duto.teta) * dxmed + perdafric * dxmed) / 98066.5);
    if (celula[i - 1].acsr.tipo == 7) pchute -= celula[i - 1].acsr.delp;
    if (celula[i - 1].acsr.tipo == 3 && (celula[i - 1].acsr.ipr.Pres - pchute) < (*vg1dSP).localtiny) pchute = 0.99
        * celula[i - 1].acsr.ipr.Pres;

    celula[i - 1].dpB = 0.;    //alteracao 1
    if (celula[i - 1].acsr.tipo == 4 && celula[i - 1].acsr.bcs.freqnova > 1. && vaz >= 0.) {    //alteracao 1
      double vazmix = j * celula[i - 1].dutoL.area;
      double rhomis = celula[i - 1].flui.MasEspLiq(pchute, taux);
      double vismis = celula[i - 1].flui.ViscLiq(pchute, taux);
      vazmix *= (86400 / 0.16);
      celula[i - 1].acsr.bcs.NovaVis(vismis, rhomis, vazmix);
      celula[i - 1].dpB = 0.3048 * celula[i - 1].acsr.bcs.Hvis * rhomis * 9.82;
    }    //alteracao 1
    pchute -= celula[i - 1].dpB / 98066.5;    //alteracao 1
    celula[i - 1].pres = pchute;
  }
  return pchute;
}
