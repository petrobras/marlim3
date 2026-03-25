/*
 * Leitura.cpp
 *
 *  Created on: 18/03/2015
 *      Author: bwz1
 */
#include "Leitura.h"
//#include "schema.h"

using namespace rapidjson;

/*!
 * Iniciar todas as variaveis globais e de instancia.
 */
void Ler::iniciarVariaveis() {
	// comprimento total da linha de producao
	nCompTotalUnidadesP = 0;
	// comprimento total da linha de servico
	nCompTotalUnidadesG = 0;
	saidaClassica=0;
	dtmaxserie.parserie = 0;
	nfluP = 0;
	flup = 0;
	indFlup=0;
	nmaterial = 0;
	ncorte = 0;
	nduto = 0;
	nunidadep = 0;
	lingas = 0;
	nunidadeg = 0;
	ncelp = 0;
	ncelg = 0;
	nipr = 0;
	nvalvgas = 0;
	nfontechk = 0;
	ninjgas = 0;
	ninjliq = 0;
	ninjmass = 0;
	nfuro = 0;
	nbcs = 0;
	nbvol = 0;
	ndpreq = 0;
	npig = 0;
	master1.parserie = 0;
	master1.ncv=0;
	psep.parserie = 0;
	chokep.parserie = 0;
	chokep.ncv=0;
	nperfisp = 0;
	nperfisg = 0;
	nperfistransp = 0;
	nperfistransg = 0;
	ntendp = 0;
	ntendg = 0;
	ntendtransp = 0;
	ntendtransg = 0;
	nduto = 0;
	ntela = 0;

	zdranP = 0; //15-06-2018
	dzdpP = 0; //15-06-2018
	dzdtP = 0; //15-06-2018
	nform = 0; // 03-04-2018
	formacPoc = 0; // 03-04-2018

	cpg = 0;
	cpl = 0;
	drholdT = 0;

	RhoInj = 0;
	ViscInj = 0;
	CondInj = 0;
	CpInj = 0;
	DrhoDtInj = 0;

	npontos = 0;

	mat = 0;
	corte = 0;
	duto = 0;
	unidadeP = 0;
	celp = 0;
	unidadeG = 0;
	celg = 0;
	IPRS = 0;
	valvgl = 0;
	nvalv=0;
	valv=0;
	fonteg = 0;
	fontel = 0;
	fontem=0;
	porosoRad=0;
	poroso2D=0;
	furo=0;
	bcs = 0;
	bvol = 0;
	dpreq = 0;
	fontechk = 0;
	pig = 0;

	trendp = 0;
	nvartrendp = 0;
	trendg = 0;
	nvartrendg = 0;
	trendtransp = 0;
	trendtransg = 0;
	tela = 0;
	Tevento = 0;
	Teventof = 0;
	TeventoPXO = 0;
	escorregaPerm = 1;
	escorregaTran = 1;
	tabRSPB = 0;
	corrDeng=0;

	controDesc = 0; //alteracao5
	vazDescControl = 0.; //alteracao5
	presMaxDesc = 0.; //alteracao5
	presMinDesc = 0; //alteracao5
	presIniDesc = 0; //alteracao5
	tempDescarga = 0; //alteracao5
	presMaxDescG = 0; //alteracao5
	presMinDescG = 0; //alteracao5
	presIniDescG = 0; //alteracao5
	tempoLatenciaDesc = 0; //alteracoa5

	flashCompleto = 0;
	tabelaDinamica=0;

	CorreEstrat = 1;
	CorreDisper = 1;
	CorreAnular = 1;

	cicloAcopTerm=0;

	saidaTela=0;

	mapaArranjo=0;

	RelaxaDTChoke=0;
	CriterioConvergPerm=0.001;
	AceleraConvergPerm=0;
	escorregamentoCelulaContorno=1;
	correcaoContracorPerm=0;
	desligaDeriTransMassDTemp=0;

	TcorrecaoMassaEspLiq=0;
	VcorrecaoMassaEspLiq=0;
	Tempsonico=0;
	Vsonico=0;

	correcaoMassaEspLiq=0;
	acopColAnulPermForte=0;
	mudaArea=0;

	compLinServ=0;

	miniTabAtraso=0;
	miniTabDp=0;
	miniTabDt=0;
	tabVisc=0;

	valTempChokeJus=0;
	limTransMass=10.;

	nvecEmul=0;
	BSWVec=0;
	emulVec=0;

	nsnp=0;

	corrigeContSep=1;

	nTcorrecaoMassaEspLiq=0;
	nTsonico=0;

	taxaDespre=0.01;

	tipoFluido=0;
	npseudo=0;

	ConContEntrada=0;

	tempReves=-1000.;
	razCompGasReves=0.;

	correcao.parserieDT=0;
	correcao.parserieFric=0;
	correcao.parserieHidro=0;
	correcao.dPdLHidro =0;
	correcao.dPdLFric =0;
	correcao.dTdL =0;

	AS=0;
	paralelAS=0;

	nthrd=1;

	buscaFC=0.1;

	chutePerm=-1.;

	HISEP=0;

	reverso=0;

	nPoroRad=0;
	nPoro2D=0;
	/////////////////////////////////////////////////////////////////////////////////////////
	mono = 0;
	eventoabrePXO = 0;
	indfluPIni = 0;
	tfinal = 0;
	transiente = 0;
	latente = 0;
	condlatente = 1;
	ordperm = 0;
	dutosMRT = 0;
	tabg = 0;
	chkv = 0;
	dtmax = 0;
	tabp = 0;
	modelcp = 0;
	critcond = 0;
	areagargPXO = 0;
	trackRGO = 0;
	trackDeng = 0;
	anmG = 0;
	equilterm = 0;
	eventofecha = 0;
	pocinjec = 0;
	anmP = 0;
	transmass = 0;
	snaps = 0;
	modelJTL = 0;
	origemGeometria = origemGeometria_t::montante;
	sistemaSimulacao = sistemaSimulacao_t::multifasico;
	sentidoGeometriaSegueEscoamento = true;
	perm = 0;
	eventoabre = 0;
	nvarprofp = 0;
	nvarprofg = 0;

	descarga = 0; //15-06-2018
	salinDescarga = 0; //15-06-2018
	compInterDesc = 1e7; //15-06-2018
	compInterDescP = 1e7; //15-06-2018
	celdescarga = 1e7; //15-06-2018
	celdescargaP = 1e7; //15-06-2018

	modoSegrega=1;
	nsegrega=0;
	vecTSegrega=0;
	vecSegrega=0;

	estabCol=0;

	vg1dSP=0;

	modoXY=0;
	xProd0=0;
	yProd0=0;
	xServ0=0;
	yServ0=0;
	modoParafina=0;

	detalParafina.arquivo="";
	detalParafina.poroRey=0;
	detalParafina.valRey=0;
	detalParafina.C2C3=0;
	detalParafina.valC2=0.;
	detalParafina.valC3=0.;
	detalParafina.difus=0;
	detalParafina.multDifus=0.;
	detalParafina.rug=0.;
	// Default adjusting constants D, E and F: // Samuel - 10/10/25
	detalParafina.DViscWax = 18.12; // 1.;
	detalParafina.EViscWax = 405.1; // 1.;
	detalParafina.FViscWax= 7.876E+6; // 1.;
	// Multipliers for the constants D, E and F: // Samuel - 10/10/25
	detalParafina.DmultipWax = 1.;
	detalParafina.EmultipWax = 1.;
	detalParafina.FmultipWax = 1.;

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
	oInterpolatedWaxConcs=0;

	npseudoWax=0;
	iPresWax=0;
	iTempWax=0;

	modoDifus3D=0;
	threadP3D=1;
	modoDifus3DJson="";
	nacop=0;

	desligaPenalizaDT=0;
	controleDTvalv=0;

	indicaBeta=0;

	redeperm=1;

	existeEnvelope=0;

	nparEnvelope=0;
	envelopeHidra=0;
	calculaEnvelope=0;

	//Solver Hidratos - chris
    //Khamm=1297; //chris - Hidratos
	MMH=120;
	MMG=17.4;
	MMW=18;
	Whamm=0.0;
    //Methanol=46.07; //chris - Hidratos
   	//MMEG=62.1; //chris - Hidratos
   	//rhoH=917.0; //chris - Hidratos
	//rhoG //vem do Marlim 3
	//rhoW //vem do Marlim 3
	coefEsteq=6.0;
   	/*estruturaHidratosIk1=7.3548E+17;
	estruturaHidratosIk2=-13600;
    estruturaHidratosIIk1=1.54384E+25;
	estruturaHidratosIIk2=-16696;*/
	inibidor=""; //chris - Hidratos
	fracFWcarregada=1e-4; //chris - Hidratos
	estruturaHidratos=""; //chris - Hidratos
    rd=4e-5;
	rp=4e-5;

	tipoModeloDrift=1;
	JTLiquidoSimple=0.;

	/*nestabCol=0;
	vecTestabCol=0;
	vecestabCol=0;*/

	nintermi=0;
	intermi=0;

	pmin=0.01;
	tmin=-200;
}

/*!
 * Iniciar todas as variaveis globais e de instancia para o construtor default.
 */
void Ler::iniciarVariaveisConstrutorDefault() {
	saidaClassica=0;
	mono = 0;
	eventoabrePXO = 0;
	indfluPIni = 0;
	tfinal = 0;
	transiente = 0;
	latente = 0;
	condlatente = 1;
	ordperm = 0;
	dutosMRT = 0;
	tabg = 0;
	chkv = 0;
	dtmax = 0;
	tabp = 0;
	modelcp = 0;
	critcond = 0;
	areagargPXO = 0;
	trackRGO = 0;
	trackDeng = 0;
	anmG = 0;
	equilterm = 0;
	eventofecha = 0;
	pocinjec = 0;
	anmP = 0;
	transmass = 0;
	snaps = 0;
	modelJTL = 0;
	origemGeometria = origemGeometria_t::montante;
	sistemaSimulacao = sistemaSimulacao_t::multifasico;
	sentidoGeometriaSegueEscoamento = true;
	perm = 0;
	eventoabre = 0;
	nvarprofp = 0;
	nvarprofg = 0;

	descarga = 0; //15-06-2018
	salinDescarga = 0; //15-06-2018
	compInterDesc = 1e7; //15-06-2018
	compInterDescP = 1e7; //15-06-2018
	celdescarga = 1e7; //15-06-2018
	celdescargaP = 1e7; //15-06-2018

	escorregaPerm = 1;
	escorregaTran = 1;
	tabRSPB = 0;
	corrDeng=0;
	controDesc = 0; //alteracao5
	vazDescControl = 0.; //alteracao5
	presMaxDesc = 0.; //alteracao5
	presMinDesc = 0; //alteracao5
	presIniDesc = 0; //alteracao5
	tempDescarga = 0; //alteracao5
	presMaxDescG = 0; //alteracao5
	presMinDescG = 0; //alteracao5
	presIniDescG = 0; //alteracao5
	tempoLatenciaDesc = 0; //alteracoa5

	flashCompleto = 0;
	tabelaDinamica=0;

	CorreEstrat = 1;
	CorreDisper = 1;
	CorreAnular = 1;

	nperfisp = 0;
	nperfisg = 0;
	nperfistransp = 0;
	nperfistransg = 0;
	ntendp = 0;
	ntendg = 0;
	ntendtransp = 0;
	ntendtransg = 0;
	nduto = 0;
	ntela = 0;

	cicloAcopTerm=0;

	saidaTela=0;

	mapaArranjo=0;

	RelaxaDTChoke=0;
	CriterioConvergPerm=0.001;
	AceleraConvergPerm=0;
	escorregamentoCelulaContorno=1;
	correcaoContracorPerm=0;
	desligaDeriTransMassDTemp=0;

	valTempChokeJus=0;
	limTransMass=10.;

	nvecEmul=0;
	BSWVec=0;
	emulVec=0;
	nsnp=0;

	corrigeContSep=1;

	nTcorrecaoMassaEspLiq=0;
	nTsonico=0;

	taxaDespre=0.01;

	acopColAnulPermForte=0;

	tipoFluido=0;
	npseudo=0;

	tempReves=0.;
	razCompGasReves=0.;

	correcao.parserieDT=0;
	correcao.parserieFric=0;
	correcao.parserieHidro=0;
	correcao.dPdLHidro =0;
	correcao.dPdLFric =0;
	correcao.dTdL =0;

	AS=0;
	paralelAS=0;

	nthrd=1;
	buscaFC=0.1;

	chutePerm=-1.;

	HISEP=0;

	reverso=0;

	nPoroRad=0;

	modoSegrega=1;
	nsegrega=0;
	vecTSegrega=0;
	vecSegrega=0;

	estabCol=0;

	vg1dSP=0;

	modoXY=0;
	xProd0=0;
	yProd0=0;
	xServ0=0;
	yServ0=0;
	modoParafina=0;

	detalParafina.arquivo="";
	detalParafina.poroRey=0;
	detalParafina.valRey=0;
	detalParafina.C2C3=0;
	detalParafina.valC2=0.;
	detalParafina.valC3=0.;
	detalParafina.difus=0;
	detalParafina.multDifus=0.;
	detalParafina.rug=0.;
	// Default adjusting constants D, E and F: // Samuel - 10/10/25
	detalParafina.DViscWax = 18.12; // 1.;
	detalParafina.EViscWax = 405.1; // 1.;
	detalParafina.FViscWax= 7.876E+6; // 1.;
	// Multipliers for the constants D, E and F: // Samuel - 10/10/25
	detalParafina.DmultipWax = 1.;
	detalParafina.EmultipWax = 1.;
	detalParafina.FmultipWax = 1.;

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
	oInterpolatedWaxConcs=0;

	npseudoWax=0;
	iPresWax=0;
	iTempWax=0;

	modoDifus3D=0;
	threadP3D=1;
	modoDifus3DJson="";
	nacop=0;

	desligaPenalizaDT=0;
	controleDTvalv=0;

	indicaBeta=0;

	redeperm=1;

	existeEnvelope=0;

	nparEnvelope=0;

	envelopeHidra=0;

	calculaEnvelope=0;


	//Solver Hidratos - chris
    //Khamm=1297; //chris - Hidratos
	MMH=120;
	MMG=17.4;
	MMW=18;
	Whamm=0.0;
    //Methanol=46.07; //chris - Hidratos
   	//MMEG=62.1; //chris - Hidratos
   	//rhoH=917.0; //chris - Hidratos
	//rhoG //vem do Marlim 3
	//rhoW //vem do Marlim 3
	coefEsteq=6.0;
   	/*estruturaHidratosIk1=7.3548E+17;
	estruturaHidratosIk2=-13600;
    estruturaHidratosIIk1=1.54384E+25;
	estruturaHidratosIIk2=-16696;*/
	inibidor="";
	fracFWcarregada=1e-4; //chris - Hidratos
	estruturaHidratos="";
    rd=4e-5;
	rp=4e-5;

	tipoModeloDrift=1;
	JTLiquidoSimple=0.;
	/*nestabCol=0;
	vecTestabCol=0;
	vecestabCol=0;*/

	nintermi=0;
	intermi=0;

	pmin=0.01;
	tmin=-200;
}

/*!
 * Fields Construtor ligado ao objeto de log da aplicacao.
 */
Ler::Ler(const string IMPFILE, const string ARQUIVO_LOG,
		const tipoValidacaoJson_t VALIDACAO, const tipoSimulacao_t SIMULACAO, int vreverso,varGlob1D* Vvg1dSP, int vredeperm) {
	iniciarVariaveis();
	impfile = IMPFILE;
	arquivoLog = ARQUIVO_LOG;
	validacaoJson = VALIDACAO;
	tipoSimulacao = SIMULACAO;
	reverso=vreverso;
	vg1dSP=Vvg1dSP;
	redeperm=vredeperm;
	lerArq();
}

/*!
 * Copy Constructor.
 */
Ler::Ler(const Ler& vler) {
	iniciarVariaveis();
	impfile = vler.impfile;
	arquivoLog = vler.arquivoLog;
	validacaoJson = vler.validacaoJson;
	tipoSimulacao = vler.tipoSimulacao;
	reverso=vler.reverso;
	vg1dSP=vler.vg1dSP;
	lerArq();
}

/*!
 * Default Construtor.
 */
Ler::Ler() {
	iniciarVariaveis();
	iniciarVariaveisConstrutorDefault();
}

/*!
 * Assignment Operator
 */
Ler& Ler::operator =(const Ler& vler) {
	if (this != &vler) {
		if (dtmaxserie.parserie > 0) {
			delete[] dtmaxserie.tempo;
			delete[] dtmaxserie.valor;
		}

		if (nfluP > 0)
			delete[] flup;

		if (nform > 0)
			delete[] formacPoc;    //15-06-2018

		//delete [] varRGO.tempo;
		//delete [] varRGO.valor;

		if (nmaterial > 0)
			delete[] mat;

		if (ncorte > 0) {
			for (int i = 0; i < this->ncorte; i++) {
				delete[] corte[i].diam;
				delete[] corte[i].indmat;
				delete[] corte[i].discre;
			}
			delete[] corte;
		}

		if (nduto > 0)
			delete[] duto;

		if (nunidadep > 0) {
			for (int i = 0; i < nunidadep; i++) {
				delete[] unidadeP[i].dx;
				delete[] unidadeP[i].dxVar;
				for (int j = 0; j < 12; j++)
					delete[] unidadeP[i].var[j];
				delete[] unidadeP[i].var;
				if(modoDifus3D==1){
					delete [] unidadeP[i].difusTerm3D;
					delete [] unidadeP[i].difusTerm3DFE;
					delete [] unidadeP[i].difusTerm3DAcop;
				}
			}
			delete[] unidadeP;
		}

		if (nunidadeg > 0 && lingas > 0) {
			for (int i = 0; i < nunidadeg; i++) {
				delete[] unidadeG[i].dx;
				delete[] unidadeG[i].dxVar;
				for (int j = 0; j < 9; j++)
					delete[] unidadeG[i].var[j];
				delete[] unidadeG[i].var;
			}
			delete[] unidadeG;
			if(chokes.parserie){
			  delete [] chokes.tempo;
			  delete [] chokes.abertura;
			}
		}

		if (ncelp > 0)
			delete[] celp;

		if (ncelg > 0)
			delete[] celg;

		if(nparEnvelope>0){
		  for(int i=0; i<nparEnvelope;i++){
			  delete [] envelopeHidra[i];
		  }
		  delete [] envelopeHidra;
		}

		if (nipr > 0) {
			for (int i = 0; i < this->nipr; i++) {
				delete[] IPRS[i].pres;
				delete[] IPRS[i].tpres;
				delete[] IPRS[i].temp;
				delete[] IPRS[i].ttemp;
				if(IPRS[i].tipoIPR==0 || IPRS[i].tipoIPR==1){
					  delete [] IPRS[i].ip;
					  delete [] IPRS[i].tip;
				}
				else{
					  delete [] IPRS[i].qMax;
					  delete [] IPRS[i].tqMax;
				}
				delete[] IPRS[i].jp;
				delete[] IPRS[i].tjp;

			}
			delete[] IPRS;
		}
		if (ConContEntrada == 1) {
			delete[] CCPres.temperatura;
			delete[] CCPres.pres;
			delete[] CCPres.tit;
			delete[] CCPres.bet;
			delete[] CCPres.tempo;
		}
		else if( ConContEntrada==2){
	        delete [] CCVPres.temperatura;
	        delete [] CCVPres.pres;
	        delete [] CCVPres.mass;
	        delete [] CCVPres.bet;
	        delete [] CCVPres.tempo;
		}

		if (lingas > 0 && controDesc == 0) {
			delete[] gasinj.tempo;
			if (gasinj.tipoCC == 1)
				delete[] gasinj.vazgas;
			else if (gasinj.tipoCC == 0) {    //mudancaChute
				delete[] gasinj.presinj;
				//f (gasinj.chuteVaz == 1)
				delete[] gasinj.vazgas;
			}    //mudancaChute
			delete[] gasinj.temperatura;
		}

		if (nvalvgas > 0 && lingas > 0)
			delete[] valvgl;

		if(nvalv>0){
		  for(int i=0; i<this->nvalv;i++){
			  if(valv[i].ncv>0)delete [] valv[i].cvCurv;
			  delete [] valv[i].abertura;
			  delete [] valv[i].tempo;
		  }
		  delete [] valv;
		}


		if (nfontechk > 0) {
			for (int i = 0; i < this->nfontechk; i++) {
				delete[] fontechk[i].tempo;
				delete[] fontechk[i].abertura;
			}
			delete[] fontechk;
		}

		if (ninjgas > 0) {
			for (int i = 0; i < this->ninjgas; i++) {
				delete[] fonteg[i].temp;
				delete[] fonteg[i].vazgas;
				delete[] fonteg[i].vazcomp;
				delete[] fonteg[i].tempo;
			}
			delete[] fonteg;
		}

		if (ninjliq > 0) {
			for (int i = 0; i < this->ninjliq; i++) {
				delete[] fontel[i].temp;
				delete[] fontel[i].bet;
				delete[] fontel[i].vazliq;
				delete[] fontel[i].tempo;
			}
			delete[] fontel;
		}

		if(ninjmass>0){
		  for(int i=0; i<this->ninjmass;i++){
			  delete [] fontem[i].temp;
			  delete [] fontem[i].vazMasT;
			  delete [] fontem[i].vazMasC;
			  delete [] fontem[i].vazMasG;
			  delete [] fontem[i].tempo;
		  }
		  delete [] fontem;
		}

		if(nPoroRad>0){
		  delete [] porosoRad;
		}

		if(nPoro2D>0){
		  delete [] poroso2D;
		}

		if(nfuro>0){
		  for(int i=0; i<this->nfuro;i++){
			  delete [] furo[i].abertura;
			  delete [] furo[i].tempo;

			  delete [] furo[i].check;
			  delete [] furo[i].tempoChk;
		  }
		  delete [] furo;
		}

		if (nbcs > 0) {
			for (int i = 0; i < this->nbcs; i++) {
				delete[] bcs[i].tempo;
				delete[] bcs[i].freq;
				delete[] bcs[i].vaz;
				delete[] bcs[i].head;
				delete[] bcs[i].power;
				delete[] bcs[i].efic;
			}
			delete[] bcs;
		}

		if (nbvol > 0) {
			for (int i = 0; i < this->nbvol; i++) {
				delete[] bvol[i].tempo;
				delete[] bvol[i].freq;
			}
			delete[] bvol;
		}

		if (ndpreq > 0) {
			for (int i = 0; i < this->ndpreq; i++) {
				delete[] dpreq[i].dp;
				delete[] dpreq[i].tempo;
			}
			delete[] dpreq;
		}

		if (npig > 0)
			delete[] pig;

		int nevento = master1.parserie;
		if (master1.parserie > 0)
			delete[] master1.tempo;
		if (master1.parserie > 0)
			delete[] master1.abertura;
		if(master1.ncv>0)delete [] master1.cvCurv;

		if (lingas > 0) {
			delete[] master2.tempo;
			delete[] master2.abertura;
		}

		if (psep.parserie > 0) {
			delete[] psep.tempo;
			delete[] psep.pres;
		}

		if(correcao.parserieHidro>0) delete [] correcao.dPdLHidro;
		if(correcao.parserieFric>0) delete [] correcao.dPdLFric;
		if(correcao.parserieDT>0) delete [] correcao.dTdL;

		if (chokep.parserie > 0) {
			delete[] chokep.tempo;
			delete[] chokep.abertura;
			if(chokep.ncv>0)delete [] chokep.cvCurv;
		}

		if (nperfisp > 0)
			delete[] profp.tempo;
		if (nperfisg > 0)
			delete[] profg.tempo;
		if (nperfistransp > 0) {
			delete[] proftransp.posic;
			delete[] proftransp.tempo;
		}
		if (nperfistransg > 0 && lingas > 0) {
			delete[] proftransg.posic;
			delete[] proftransg.tempo;
		}
		if (ntendp > 0) {
			delete[] trendp;
			delete[] nvartrendp;
		}
		if (ntendg > 0) {
			delete[] trendg;
			delete[] nvartrendg;
		}
		if (ntendtransp > 0)
			delete[] trendtransp;
		if (ntendtransg > 0)
			delete[] trendtransg;
		if (nduto > 0)
			delete[] dutosMRT;
		//if(nacop>0) delete [] dutosAcop;

		if (nevento > 0)
			delete[] Tevento;
		if (nevento > 0)
			delete[] Teventof;
		//if(TeventoPXO) delete [] TeventoPXO;

		if (ntela > 0)
			delete[] tela;

		if (tabp > 0 && npontos > 0) {     //alteracao2
			for (int i = 0; i < npontos + 2; i++)
				delete[] zdranP[i];
			delete[] zdranP;
			for (int i = 0; i < npontos + 2; i++)
				delete[] dzdpP[i];
			delete[] dzdpP;
			for (int i = 0; i < npontos + 2; i++)
				delete[] dzdtP[i];
			delete[] dzdtP;
		}     //alteracao2
		double ndiv = npontos - 1;
		if (modelcp > 0 && cpg != 0 && cpl != 0 && npontos > 0) {
			for (int i = 0; i < ndiv + 2; i++) {
				delete[] cpg[i];
				delete[] cpl[i];
			}
			delete[] cpg;
			delete[] cpl;
		}
		if (modelJTL ==1 && drholdT != 0 && npontos > 0) {
			for (int i = 0; i < ndiv + 2; i++) {
				delete[] drholdT[i];
			}
			delete[] drholdT;
		}
		if (flashCompleto == 1) {
			for (int i = 0; i < nfluP; i++) {
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].rholF[j];
				delete[] flash[i].rholF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].rhogF[j];
				delete[] flash[i].rhogF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrholDpF[j];
				delete[] flash[i].DrholDpF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrhogDpF[j];
				delete[] flash[i].DrhogDpF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrholDtF[j];
				delete[] flash[i].DrholDtF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrhogDtF[j];
				delete[] flash[i].DrhogDtF;
				if (flash[i].TwoOrThree == 1) {
					for (int j = 0; j < npontos + 1; j++)
						delete[] flash[i].DrhowDtF[j];
					delete[] flash[i].DrhowDtF;
				}
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].RSF[j];
				delete[] flash[i].RSF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].cplF[j];
				delete[] flash[i].cplF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].cpgF[j];
				delete[] flash[i].cpgF;
				if (flash[i].TwoOrThree == 1) {
					for (int j = 0; j < npontos + 1; j++)
						delete[] flash[i].cpwF[j];
					delete[] flash[i].cpwF;
				}
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].HlF[j];
				delete[] flash[i].HlF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].HgF[j];
				delete[] flash[i].HgF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].sigOGF[j];
				delete[] flash[i].sigOGF;
				if (flash[i].TwoOrThree == 1) {
					for (int j = 0; j < npontos + 1; j++)
						delete[] flash[i].sigWGF[j];
					delete[] flash[i].sigWGF;
				}
				for (int j = 0; j < npontos + 1; j++)     //alteracao6
					delete[] flash[i].viscO[j];
				delete[] flash[i].viscO;
				for (int j = 0; j < npontos + 1; j++)  //alteracao6
					delete[] flash[i].viscG[j];
				delete[] flash[i].viscG;

				delete[] flash[i].PBF;
				delete[] flash[i].TBF;
			}
			delete[] flash;
		}
		if(flashCompleto==2){
			   /*&for(int i=0;i<nfluP;i++){
			    	delete[] compDet[i].fracMol;
			    	delete[] compDet[i].CpIGCoefs;
			    	delete[] compDet[i].TIndepPeneloux;
			    	delete[] compDet[i].fatAcent;
			    	delete[] compDet[i].kij;
			    	delete[] compDet[i].lij;
			    	delete[] compDet[i].masMol;
			    	delete[] compDet[i].parac;
			    	delete[] compDet[i].presCrit;
			    	delete[] compDet[i].tempCrit;
			    }*/
			    if(nfluP>0) delete[] compDet;
	    		/*delete  compDet.fracMol;
	    		delete  compDet.CpIGCoefs;
	    		delete  compDet.TIndepPeneloux;
	    		delete  compDet.fatAcent;
	    		delete  compDet.kij;
	    		delete  compDet.lij;
	    		delete  compDet.masMol;
	    		delete  compDet.parac;
	    		delete  compDet.presCrit;
	    		delete  compDet.tempCrit;*/
			    if(pocinjec == 0 && (lingas==1 || ninjgas>0))delete[] compLinServ;
		  }
		if (pocinjec > 0 && condpocinj.tipoFlui == 2) {
			if (RhoInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] RhoInj[i];
				delete[] RhoInj;
			}
			if (ViscInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] ViscInj[i];
				delete[] ViscInj;
			}
			if (CondInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] CondInj[i];
				delete[] CondInj;
			}
			if (CpInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] CpInj[i];
				delete[] CpInj;
			}
			if (DrhoDtInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] DrhoDtInj[i];
				delete[] DrhoDtInj;
			}
		}
	    if(nvecEmul>0){
	    	  delete [] BSWVec;
	    	  delete [] emulVec;
	    }

	    if(nsnp>0) delete [] tempsnp;

	    if(nTcorrecaoMassaEspLiq>0){
	    	delete [] VcorrecaoMassaEspLiq;
	    	delete [] TcorrecaoMassaEspLiq;
	    }

	    if(nTsonico>0){
	     	delete [] Vsonico;
	       	delete [] Tempsonico;
	    }

		if(nfluP>0){
			for(int i=0; i<nfluP;i++){
				if(flup[i].corrOM==7){
					if(tabVisc[i].parserie>0){
						delete [] tabVisc[i].visc;
						delete [] tabVisc[i].temp;
					}
					delete [] tabVisc;
				}
			}
		}

		if(nsegrega>0){
			delete [] vecTSegrega;
			delete [] vecSegrega;
		}

		if(oPressurePoints!=0) delete [] oPressurePoints;
		if(oCloudPointTemperatures!=0) delete [] oCloudPointTemperatures;
		if(oStructurePressures!=0) delete [] oStructurePressures;
		if(oStructureTemperatures!=0) delete [] oStructureTemperatures;
		/*if(oStructureTemperatures!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureTemperatures[i];
			delete [] oStructureTemperatures;
		}*/
		if(oStructureDensities!=0) delete [] oStructureTemperatures;
		/*if(oStructureDensities!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureDensities[i];
			delete [] oStructureDensities;
		}*/
		if(oStructureLiqMW!=0) delete [] oStructureLiqMW;
		/*if(oStructureLiqMW!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureLiqMW[i];
			delete [] oStructureLiqMW;
		}*/
		if(oStructureCPWax!=0) delete [] oStructureCPWax;
		/*if(oStructureCPWax!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureCPWax[i];
			delete [] oStructureCPWax;
		}*/
		if(oStructureThermCond!=0) delete [] oStructureThermCond;
		/*if(oStructureThermCond!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureThermCond[i];
			delete [] oStructureThermCond;
		}*/
		if(oStructureWaxConcs!=0) delete [] oStructureWaxConcs;
		/*if(oStructureWaxConcs!=0){
			for(int i=0; i<iPresWax;i++){
				for(int j=0; i< iTempWax; i++) delete [] oStructureWaxConcs[i][j];
				delete [] oStructureWaxConcs[i];
			}
			delete [] oStructureWaxConcs;
		}*/
		if(oMolecularWeightsOfWaxComponents!=0) delete [] oMolecularWeightsOfWaxComponents;
		if(oLiquidDensitiesOfWaxComponents!=0) delete [] oLiquidDensitiesOfWaxComponents;
		if(oInterpolatedWaxConcs!=0) delete [] oInterpolatedWaxConcs;
		/*if(nestabCol>0){
			delete [] vecTestabCol;
			delete [] vecestabCol;
		}*/
		if(logeventoEstat.size()>0)logeventoEstat.clear();
		if(logevento.size()>0)logevento.clear();
		if(evento.size()>0)evento.clear();
		if(celAcop.size()>0)celAcop.clear();
		if(geoAcop.size()>0)geoAcop.clear();

	    iniciarVariaveis();
	    vg1dSP=vler.vg1dSP;
	    impfile = vler.impfile;
		arquivoLog = vler.arquivoLog;
		validacaoJson = vler.validacaoJson;
		tipoSimulacao = vler.tipoSimulacao;
		reverso=vler.reverso;
		lerArq();
		if((*vg1dSP).chaverede==1)tabelaDinamica=vler.tabelaDinamica;
	}

	return *this;
}


void Ler::copiaSemJson(Ler& vler) {
	if (this != &vler) {
		if (dtmaxserie.parserie > 0) {
			delete[] dtmaxserie.tempo;
			delete[] dtmaxserie.valor;
		}

		if (nfluP > 0)
			delete[] flup;

		if (nform > 0)
			delete[] formacPoc;    //15-06-2018

		//delete [] varRGO.tempo;
		//delete [] varRGO.valor;

		if (nmaterial > 0)
			delete[] mat;

		if (ncorte > 0) {
			for (int i = 0; i < this->ncorte; i++) {
				delete[] corte[i].diam;
				delete[] corte[i].indmat;
				delete[] corte[i].discre;
			}
			delete[] corte;
		}

		if (nduto > 0)
			delete[] duto;

		if (nunidadep > 0) {
			for (int i = 0; i < nunidadep; i++) {
				delete[] unidadeP[i].dx;
				delete[] unidadeP[i].dxVar;
				for (int j = 0; j < 12; j++)
					delete[] unidadeP[i].var[j];
				delete[] unidadeP[i].var;
				if(modoDifus3D==1){
					delete [] unidadeP[i].difusTerm3D;
					delete [] unidadeP[i].difusTerm3DFE;
					delete [] unidadeP[i].difusTerm3DAcop;
				}
			}
			delete[] unidadeP;
		}

		if (nunidadeg > 0 && lingas > 0) {
			for (int i = 0; i < nunidadeg; i++) {
				delete[] unidadeG[i].dx;
				delete[] unidadeG[i].dxVar;
				for (int j = 0; j < 9; j++)
					delete[] unidadeG[i].var[j];
				delete[] unidadeG[i].var;
			}
			delete[] unidadeG;
			if(chokes.parserie){
			  delete [] chokes.tempo;
			  delete [] chokes.abertura;
			}
		}

		if (ncelp > 0)
			delete[] celp;

		if (ncelg > 0)
			delete[] celg;

		if(nparEnvelope>0){
		  for(int i=0; i<nparEnvelope;i++){
			  delete [] envelopeHidra[i];
		  }
		  delete [] envelopeHidra;
		}

		if (nipr > 0) {
			for (int i = 0; i < this->nipr; i++) {
				delete[] IPRS[i].pres;
				delete[] IPRS[i].tpres;
				delete[] IPRS[i].temp;
				delete[] IPRS[i].ttemp;
				if(IPRS[i].tipoIPR==0 || IPRS[i].tipoIPR==1){
					  delete [] IPRS[i].ip;
					  delete [] IPRS[i].tip;
				}
				else{
					  delete [] IPRS[i].qMax;
					  delete [] IPRS[i].tqMax;
				}
				delete[] IPRS[i].jp;
				delete[] IPRS[i].tjp;

			}
			delete[] IPRS;
		}
		if (ConContEntrada == 1) {
			delete[] CCPres.temperatura;
			delete[] CCPres.pres;
			delete[] CCPres.tit;
			delete[] CCPres.bet;
			delete[] CCPres.tempo;
		}
		else if( ConContEntrada==2){
	        delete [] CCVPres.temperatura;
	        delete [] CCVPres.pres;
	        delete [] CCVPres.mass;
	        delete [] CCVPres.bet;
	        delete [] CCVPres.tempo;
		}

		if (lingas > 0 && controDesc == 0) {
			delete[] gasinj.tempo;
			if (gasinj.tipoCC == 1)
				delete[] gasinj.vazgas;
			else if (gasinj.tipoCC == 0) {    //mudancaChute
				delete[] gasinj.presinj;
				//f (gasinj.chuteVaz == 1)
				delete[] gasinj.vazgas;
			}    //mudancaChute
			delete[] gasinj.temperatura;
		}

		if (nvalvgas > 0 && lingas > 0)
			delete[] valvgl;

		if(nvalv>0){
		  for(int i=0; i<this->nvalv;i++){
			  if(valv[i].ncv>0)delete [] valv[i].cvCurv;
			  delete [] valv[i].abertura;
			  delete [] valv[i].tempo;
		  }
		  delete [] valv;
		}


		if (nfontechk > 0) {
			for (int i = 0; i < this->nfontechk; i++) {
				delete[] fontechk[i].tempo;
				delete[] fontechk[i].abertura;
			}
			delete[] fontechk;
		}

		if (ninjgas > 0) {
			for (int i = 0; i < this->ninjgas; i++) {
				delete[] fonteg[i].temp;
				delete[] fonteg[i].vazgas;
				delete[] fonteg[i].vazcomp;
				delete[] fonteg[i].tempo;
			}
			delete[] fonteg;
		}

		if (ninjliq > 0) {
			for (int i = 0; i < this->ninjliq; i++) {
				delete[] fontel[i].temp;
				delete[] fontel[i].bet;
				delete[] fontel[i].vazliq;
				delete[] fontel[i].tempo;
			}
			delete[] fontel;
		}

		if(ninjmass>0){
		  for(int i=0; i<this->ninjmass;i++){
			  delete [] fontem[i].temp;
			  delete [] fontem[i].vazMasT;
			  delete [] fontem[i].vazMasC;
			  delete [] fontem[i].vazMasG;
			  delete [] fontem[i].tempo;
		  }
		  delete [] fontem;
		}

		if(nPoroRad>0){
		  delete [] porosoRad;
		}

		if(nPoro2D>0){
		  delete [] poroso2D;
		}

		if(nfuro>0){
		  for(int i=0; i<this->nfuro;i++){
			  delete [] furo[i].abertura;
			  delete [] furo[i].tempo;
		  }
		  delete [] furo;
		}

		if (nbcs > 0) {
			for (int i = 0; i < this->nbcs; i++) {
				delete[] bcs[i].tempo;
				delete[] bcs[i].freq;
				delete[] bcs[i].vaz;
				delete[] bcs[i].head;
				delete[] bcs[i].power;
				delete[] bcs[i].efic;
			}
			delete[] bcs;
		}

		if (nbvol > 0) {
			for (int i = 0; i < this->nbvol; i++) {
				delete[] bvol[i].tempo;
				delete[] bvol[i].freq;
			}
			delete[] bvol;
		}

		if (ndpreq > 0) {
			for (int i = 0; i < this->ndpreq; i++) {
				delete[] dpreq[i].dp;
				delete[] dpreq[i].tempo;
			}
			delete[] dpreq;
		}

		if (npig > 0)
			delete[] pig;

		int nevento = master1.parserie;
		if (master1.parserie > 0)
			delete[] master1.tempo;
		if (master1.parserie > 0)
			delete[] master1.abertura;
		if(master1.ncv>0)delete [] master1.cvCurv;

		if (lingas > 0) {
			delete[] master2.tempo;
			delete[] master2.abertura;
		}

		if (psep.parserie > 0) {
			delete[] psep.tempo;
			delete[] psep.pres;
		}

		if(correcao.parserieHidro>0) delete [] correcao.dPdLHidro;
		if(correcao.parserieFric>0) delete [] correcao.dPdLFric;
		if(correcao.parserieDT>0) delete [] correcao.dTdL;

		if (chokep.parserie > 0) {
			delete[] chokep.tempo;
			delete[] chokep.abertura;
			if(chokep.ncv>0)delete [] chokep.cvCurv;
		}

		if (nperfisp > 0)
			delete[] profp.tempo;
		if (nperfisg > 0)
			delete[] profg.tempo;
		if (nperfistransp > 0) {
			delete[] proftransp.posic;
			delete[] proftransp.tempo;
		}
		if (nperfistransg > 0 && lingas > 0) {
			delete[] proftransg.posic;
			delete[] proftransg.tempo;
		}
		if (ntendp > 0) {
			delete[] trendp;
			delete[] nvartrendp;
		}
		if (ntendg > 0) {
			delete[] trendg;
			delete[] nvartrendg;
		}
		if (ntendtransp > 0)
			delete[] trendtransp;
		if (ntendtransg > 0)
			delete[] trendtransg;
		if (nduto > 0)
			delete[] dutosMRT;
		//if(nacop>0) delete [] dutosAcop;

		if (nevento > 0)
			delete[] Tevento;
		if (nevento > 0)
			delete[] Teventof;
		//if(TeventoPXO) delete [] TeventoPXO;

		if (ntela > 0)
			delete[] tela;

		if (tabp > 0 && npontos > 0) {     //alteracao2
			for (int i = 0; i < npontos + 2; i++)
				delete[] zdranP[i];
			delete[] zdranP;
			for (int i = 0; i < npontos + 2; i++)
				delete[] dzdpP[i];
			delete[] dzdpP;
			for (int i = 0; i < npontos + 2; i++)
				delete[] dzdtP[i];
			delete[] dzdtP;
		}     //alteracao2
		double ndiv = npontos - 1;
		if (modelcp > 0 && cpg != 0 && cpl != 0 && npontos > 0) {
			for (int i = 0; i < ndiv + 2; i++) {
				delete[] cpg[i];
				delete[] cpl[i];
			}
			delete[] cpg;
			delete[] cpl;
		}
		if (modelJTL ==1 && drholdT != 0 && npontos > 0) {
			for (int i = 0; i < ndiv + 2; i++) {
				delete[] drholdT[i];
			}
			delete[] drholdT;
		}
		if (flashCompleto == 1) {
			for (int i = 0; i < nfluP; i++) {
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].rholF[j];
				delete[] flash[i].rholF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].rhogF[j];
				delete[] flash[i].rhogF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrholDpF[j];
				delete[] flash[i].DrholDpF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrhogDpF[j];
				delete[] flash[i].DrhogDpF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrholDtF[j];
				delete[] flash[i].DrholDtF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].DrhogDtF[j];
				delete[] flash[i].DrhogDtF;
				if (flash[i].TwoOrThree == 1) {
					for (int j = 0; j < npontos + 1; j++)
						delete[] flash[i].DrhowDtF[j];
					delete[] flash[i].DrhowDtF;
				}
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].RSF[j];
				delete[] flash[i].RSF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].cplF[j];
				delete[] flash[i].cplF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].cpgF[j];
				delete[] flash[i].cpgF;
				if (flash[i].TwoOrThree == 1) {
					for (int j = 0; j < npontos + 1; j++)
						delete[] flash[i].cpwF[j];
					delete[] flash[i].cpwF;
				}
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].HlF[j];
				delete[] flash[i].HlF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].HgF[j];
				delete[] flash[i].HgF;
				for (int j = 0; j < npontos + 1; j++)
					delete[] flash[i].sigOGF[j];
				delete[] flash[i].sigOGF;
				if (flash[i].TwoOrThree == 1) {
					for (int j = 0; j < npontos + 1; j++)
						delete[] flash[i].sigWGF[j];
					delete[] flash[i].sigWGF;
				}
				for (int j = 0; j < npontos + 1; j++)     //alteracao6
					delete[] flash[i].viscO[j];
				delete[] flash[i].viscO;
				for (int j = 0; j < npontos + 1; j++)  //alteracao6
					delete[] flash[i].viscG[j];
				delete[] flash[i].viscG;

				delete[] flash[i].PBF;
				delete[] flash[i].TBF;
			}
			delete[] flash;
		}
		if(flashCompleto==2){
			   /*&for(int i=0;i<nfluP;i++){
			    	delete[] compDet[i].fracMol;
			    	delete[] compDet[i].CpIGCoefs;
			    	delete[] compDet[i].TIndepPeneloux;
			    	delete[] compDet[i].fatAcent;
			    	delete[] compDet[i].kij;
			    	delete[] compDet[i].lij;
			    	delete[] compDet[i].masMol;
			    	delete[] compDet[i].parac;
			    	delete[] compDet[i].presCrit;
			    	delete[] compDet[i].tempCrit;
			    }*/
			    if(nfluP>0) delete[] compDet;
	    		/*delete  compDet.fracMol;
	    		delete  compDet.CpIGCoefs;
	    		delete  compDet.TIndepPeneloux;
	    		delete  compDet.fatAcent;
	    		delete  compDet.kij;
	    		delete  compDet.lij;
	    		delete  compDet.masMol;
	    		delete  compDet.parac;
	    		delete  compDet.presCrit;
	    		delete  compDet.tempCrit;*/
			    if(pocinjec == 0 && (lingas==1 || ninjgas>0))delete[] compLinServ;
		  }
		if (pocinjec > 0 && condpocinj.tipoFlui == 2) {
			if (RhoInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] RhoInj[i];
				delete[] RhoInj;
			}
			if (ViscInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] ViscInj[i];
				delete[] ViscInj;
			}
			if (CondInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] CondInj[i];
				delete[] CondInj;
			}
			if (CpInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] CpInj[i];
				delete[] CpInj;
			}
			if (DrhoDtInj) {
				for (int i = 0; i < ndiv + 2; i++)
					delete[] DrhoDtInj[i];
				delete[] DrhoDtInj;
			}
		}
	    if(nvecEmul>0){
	    	  delete [] BSWVec;
	    	  delete [] emulVec;
	    }

	    if(nsnp>0) delete [] tempsnp;

	    if(nTcorrecaoMassaEspLiq>0){
	    	delete [] VcorrecaoMassaEspLiq;
	    	delete [] TcorrecaoMassaEspLiq;
	    }

	    if(nTsonico>0){
	     	delete [] Vsonico;
	       	delete [] Tempsonico;
	    }

		if(nfluP>0){
			for(int i=0; i<nfluP;i++){
				if(flup[i].corrOM==7){
					if(tabVisc[i].parserie>0){
						delete [] tabVisc[i].visc;
						delete [] tabVisc[i].temp;
					}
					delete [] tabVisc;
				}
			}
		}

		if(nsegrega>0){
			delete [] vecTSegrega;
			delete [] vecSegrega;
		}

		if(oPressurePoints!=0) delete [] oPressurePoints;
		if(oCloudPointTemperatures!=0) delete [] oCloudPointTemperatures;
		if(oStructurePressures!=0) delete [] oStructurePressures;
		if(oStructureTemperatures!=0) delete [] oStructureTemperatures;
		/*if(oStructureTemperatures!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureTemperatures[i];
			delete [] oStructureTemperatures;
		}*/
		if(oStructureDensities!=0) delete [] oStructureTemperatures;
		/*if(oStructureDensities!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureDensities[i];
			delete [] oStructureDensities;
		}*/
		if(oStructureLiqMW!=0) delete [] oStructureLiqMW;
		/*if(oStructureLiqMW!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureLiqMW[i];
			delete [] oStructureLiqMW;
		}*/
		if(oStructureCPWax!=0) delete [] oStructureCPWax;
		/*if(oStructureCPWax!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureCPWax[i];
			delete [] oStructureCPWax;
		}*/
		if(oStructureThermCond!=0) delete [] oStructureThermCond;
		/*if(oStructureThermCond!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureThermCond[i];
			delete [] oStructureThermCond;
		}*/
		if(oStructureWaxConcs!=0) delete [] oStructureWaxConcs;
		/*if(oStructureWaxConcs!=0){
			for(int i=0; i<iPresWax;i++){
				for(int j=0; i< iTempWax; i++) delete [] oStructureWaxConcs[i][j];
				delete [] oStructureWaxConcs[i];
			}
			delete [] oStructureWaxConcs;
		}*/
		if(oMolecularWeightsOfWaxComponents!=0) delete [] oMolecularWeightsOfWaxComponents;
		if(oLiquidDensitiesOfWaxComponents!=0) delete [] oLiquidDensitiesOfWaxComponents;
		if(oInterpolatedWaxConcs!=0) delete [] oInterpolatedWaxConcs;
		/*if(nestabCol>0){
			delete [] vecTestabCol;
			delete [] vecestabCol;
		}*/
		if(logeventoEstat.size()>0)logeventoEstat.clear();
		if(logevento.size()>0)logevento.clear();
		if(evento.size()>0)evento.clear();
		if(celAcop.size()>0)celAcop.clear();
		if(geoAcop.size()>0)geoAcop.clear();

	    iniciarVariaveis();
	    vg1dSP= vler.vg1dSP;
	    impfile = vler.impfile;
		arquivoLog = vler.arquivoLog;
		validacaoJson = vler.validacaoJson;
		tipoSimulacao = vler.tipoSimulacao;
		reverso=vler.reverso;
		copiaArq(vler);
		if((*vg1dSP).chaverede==1)tabelaDinamica=vler.tabelaDinamica;
	}
}

/*!
 * Criar arquivo do schema MRT para validacao de simulacao transiente.
 */
//void Ler::writeSchemaTransiente() {
//	remove(ARQUIVO_SCHEMA_JSON);
//	// criar arquivo de schema MRT
//	ofstream schemaMrtStream(ARQUIVO_SCHEMA_JSON, ios_base::out);
//	schemaMrtStream << "{" << endl;
//	defineSchemaCabecalho(schemaMrtStream);
//	schemaMrtStream
//			<< "\"t_sistema\": {\"type\": \"string\", \"enum\": [\"MULTIFASICO\", \"multifasico\()},"
//			<< endl;
//	defineSchemaBasico(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	// ********************************************** entrada **********************************************
//	defineSchemaObservacao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaPropriedadeFormacao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaConfiguracaoInicialTransiente(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaTempo(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaTabela(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFluidoProducao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFluidoComplementar(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFluidoGas(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaMaterial(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaCamadaSecaoTransversal(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaSecaoTransversal(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaDiscretizacaoDuto(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaDutoProducao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaDutoServico(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaIpr(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaGasInj(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFonteGasLift(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFonteGas(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFonteLiquido(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaBcs(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaBombaVolumetrica(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaDeltaPressao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaMaster1(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaMaster2(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaSeparador(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaChokeSup(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaPig(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	// ********************************************** saida **********************************************
//	defineSchemaPerfilProducao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaPerfilServico(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaPerfisTransP(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaPerfisTransS(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaTendP(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaTendS(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaTendTransP(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaTela(schemaMrtStream);
//	schemaMrtStream << "}," << endl;
//	// ********************************************** esquema completo **********************************************
//	schemaMrtStream << "\"type\": \"object\"," << endl;
//	schemaMrtStream << "\"properties\": {" << endl;
//	schemaMrtStream << "\"sistema\": {\"$ref\": \"#/definitions/t_sistema\"},"
//			<< endl;
//	schemaMrtStream << "\"versao\": {\"$ref\": \"#/definitions/t_versao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"versaoJson\": {\"$ref\": \"#/definitions/t_versaoJson\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"observacao\": {\"$ref\": \"#/definitions/t_observacao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"configuracaoInicial\": {\"$ref\": \"#/definitions/t_configuracaoInicial\"},"
//			<< endl;
//	schemaMrtStream << "\"tempo\": {\"$ref\": \"#/definitions/t_tempo\"},"
//			<< endl;
//	schemaMrtStream << "\"tabela\": {\"$ref\": \"#/definitions/t_tabela\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"fluidosProducao\": {\"$ref\": \"#/definitions/t_fluidosProducao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"fluidoComplementar\": {\"$ref\": \"#/definitions/t_fluidoComplementar\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"fluidoGas\": {\"$ref\": \"#/definitions/t_fluidoGas\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"material\": {\"$ref\": \"#/definitions/t_materiais\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"secaoTransversal\": {\"$ref\": \"#/definitions/t_secoesTransversais\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"dutosProducao\": {\"$ref\": \"#/definitions/t_dutosProducao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"dutosServico\": {\"$ref\": \"#/definitions/t_dutosServico\"},"
//			<< endl;
//	schemaMrtStream << "\"ipr\": {\"$ref\": \"#/definitions/t_iprs\"}," << endl;
//	schemaMrtStream << "\"gasInj\": {\"$ref\": \"#/definitions/t_gasInj\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"fonteGasLift\": {\"$ref\": \"#/definitions/t_fontesGasLift\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"fonteGas\": {\"$ref\": \"#/definitions/t_fontesGas\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"fonteLiquido\": {\"$ref\": \"#/definitions/t_fontesLiquido\"},"
//			<< endl;
//	schemaMrtStream << "\"bcs\": {\"$ref\": \"#/definitions/t_bcs\"}," << endl;
//	schemaMrtStream
//			<< "\"bombaVolumetrica\": {\"$ref\": \"#/definitions/t_bombasVolumetricas\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"deltaPressao\": {\"$ref\": \"#/definitions/t_deltasPressao\"},"
//			<< endl;
//	schemaMrtStream << "\"master1\": {\"$ref\": \"#/definitions/t_master1\"},"
//			<< endl;
//	schemaMrtStream << "\"master2\": {\"$ref\": \"#/definitions/t_master2\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"separador\": {\"$ref\": \"#/definitions/t_separador\"},"
//			<< endl;
//	schemaMrtStream << "\"chokeSup\": {\"$ref\": \"#/definitions/t_chokeSup\"},"
//			<< endl;
//	schemaMrtStream << "\"pig\": {\"$ref\": \"#/definitions/t_pigs\"}," << endl;
//	schemaMrtStream
//			<< "\"perfilProducao\" : {\"$ref\": \"#/definitions/t_perfilProducao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"perfilServico\" : {\"$ref\": \"#/definitions/t_perfilServico\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"perfisTransP\": {\"$ref\": \"#/definitions/t_perfisTransP\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"perfisTransS\": {\"$ref\": \"#/definitions/t_perfisTransS\"},"
//			<< endl;
//	schemaMrtStream << "\"tendP\": {\"$ref\": \"#/definitions/t_tendP\"},"
//			<< endl;
//	schemaMrtStream << "\"tendS\": {\"$ref\": \"#/definitions/t_tendS\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"tendTransP\": {\"$ref\": \"#/definitions/t_tendTransP\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"tendTransS\": {\"$ref\": \"#/definitions/t_tendTransP\"},"
//			<< endl;
//	schemaMrtStream << "\"tela\": {\"$ref\": \"#/definitions/t_tela\"}" << endl;
//	schemaMrtStream << "}," << endl;
//	schemaMrtStream
//			<< "\"required\": [\"versao\", \"versaoJson\", \"configuracaoInicial\", \"tempo\", \"fluidosProducao\", \"material\", \"secaoTransversal\", \"dutosProducao\", \"master1\", \"separador\", \"chokeSup\()"
//			<< endl;
//	schemaMrtStream << "}" << endl;
//	// fechar o stream
//	schemaMrtStream.close();
//}

//---------------------------------------------------------------------------
// POCO INJETOR
// --------------------------------------------------------------------------

/*!
 * Criar arquivo do schema MRT para validacao de simulacao de poco injetor.
 */
//void Ler::writeSchemaInjecao() {
//	remove(ARQUIVO_SCHEMA_JSON);
//	// criar arquivo de schema MRT
//	ofstream schemaMrtStream(ARQUIVO_SCHEMA_JSON, ios_base::out);
//	schemaMrtStream << "{" << endl;
//	defineSchemaCabecalho(schemaMrtStream);
//	schemaMrtStream
//			<< "\"t_sistema\": {\"type\": \"string\", \"enum\": [\"INJETOR\",\"injetor\()},"
//			<< endl;
//	defineSchemaBasico(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	// ********************************************** entrada **********************************************
//	defineSchemaObservacao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaPropriedadeFormacao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaConfiguracaoInicialInjetor(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaCondicaoContPocInjec(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaTabela(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFluidoComplementar(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaMaterial(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaCamadaSecaoTransversal(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaSecaoTransversal(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaDiscretizacaoDuto(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaDutoProducao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaIpr(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaFonteLiquido(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	// ********************************************** saida **********************************************
//	defineSchemaPerfilProducao(schemaMrtStream);
//	schemaMrtStream << "," << endl;
//	defineSchemaPerfisTransP(schemaMrtStream);
//	schemaMrtStream << "}," << endl;
//	// ********************************************** esquema completo **********************************************
//	schemaMrtStream << "\"type\": \"object\"," << endl;
//	schemaMrtStream << "\"properties\": {" << endl;
//	schemaMrtStream << "\"sistema\": {\"$ref\": \"#/definitions/t_sistema\"},"
//			<< endl;
//	schemaMrtStream << "\"versao\": {\"$ref\": \"#/definitions/t_versao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"versaoJson\": {\"$ref\": \"#/definitions/t_versaoJson\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"observacao\": {\"$ref\": \"#/definitions/t_observacao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"configuracaoInicial\": {\"$ref\": \"#/definitions/t_configuracaoInicial\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"CondicaoContPocInjec\": {\"$ref\": \"#/definitions/t_CondicaoContPocInjec\"},"
//			<< endl;
//	schemaMrtStream << "\"tabela\": {\"$ref\": \"#/definitions/t_tabela\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"fluidoComplementar\": {\"$ref\": \"#/definitions/t_fluidoComplementar\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"material\": {\"$ref\": \"#/definitions/t_materiais\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"secaoTransversal\": {\"$ref\": \"#/definitions/t_secoesTransversais\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"dutosProducao\": {\"$ref\": \"#/definitions/t_dutosProducao\"},"
//			<< endl;
//	schemaMrtStream << "\"ipr\": {\"$ref\": \"#/definitions/t_iprs\"}," << endl;
//	schemaMrtStream
//			<< "\"fonteLiquido\": {\"$ref\": \"#/definitions/t_fontesLiquido\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"perfilProducao\" : {\"$ref\": \"#/definitions/t_perfilProducao\"},"
//			<< endl;
//	schemaMrtStream
//			<< "\"perfisTransP\": {\"$ref\": \"#/definitions/t_perfisTransP\"}"
//			<< endl;
//	schemaMrtStream << "}," << endl;
//	schemaMrtStream
//			<< "\"required\": [\"versao\", \"versaoJson\", \"configuracaoInicial\", \"CondicaoContPocInjec\", \"material\", \"secaoTransversal\", \"dutosProducao\()"
//			<< endl;
//	schemaMrtStream << "}" << endl;
//	// fechar o stream
//	schemaMrtStream.close();
//}

/*!
 * Realizar parse do arquivo de schema JSON para validacao dos parametros da simulacao.
 */
/*Document Ler::parseSchema() {
	// criar string para mensagem de falha
	char mensagemFalha[5000];
	// declarar o documento para o schema de validacao do formato do MRT
	Document schemaDoc;
	// criar arquivo de schema de validacao do formato do MRT
	FILE *schemaMrtInFile = NULL;
	// definir buffer de entrada para leitura do arquivo json
	char schemaMrtInBuf[65536];
	// realizar a leitura do arquivo de schema do MRT
	try {
		// atualizar logger com arquivo de schema do mrt
		logger.setNomeArqEntrada(ARQUIVO_SCHEMA_JSON);
		char *buf = new char[schema_schema_1_3_9_json_len + 1];
		memcpy(buf, schema_schema_1_3_9_json, schema_schema_1_3_9_json_len);
		buf[schema_schema_1_3_9_json_len] = 0;

		StringStream schemaMrtInStream(buf);
		// realizar o parse do schema de validacao do formato do MRT
		schemaDoc.ParseStream(schemaMrtInStream);
		// remover o arquivo de schema
		//remove(ARQUIVO_SCHEMA_JSON);
		// caso haja erros de parse
		if (schemaDoc.HasParseError()) {
			//cout << "ErrorOffset: " << determinarLinhaErro(schemaDoc.GetErrorOffset()) << endl;
			// transpor os dados da falha para a mensagem
			sprintf(mensagemFalha, "Posicao = %u", schemaDoc.GetErrorOffset());
			cout << " parse error: "
					<< GetParseError_Pt_BR(schemaDoc.GetParseError()) << " - "
					<< mensagemFalha << " - " << schemaDoc.GetErrorOffset()
					<< endl;
			// incluir falha no processo de parse
			logger.log_write_logs_and_exit(LOGGER_FALHA,
			LOG_ERR_PARSE_JSON_FORMAT_VALIDATION, "Parse Schema-MRT",
					mensagemFalha,
					GetParseError_Pt_BR(schemaDoc.GetParseError()),
					determinarLinhaErro(schemaDoc.GetErrorOffset()));
		}
	} catch (exception& e) {
		// incluir falha no processo de parse
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "Parse Schema-MRT", "", e.what());
	}
	// retorna o documento do schema do MRT
	return schemaDoc;
}*/

/*!
 * Realizar parse do arquivo JSON de parametros da simulacao.
 */
JSON_entrada Ler::parseEntrada() {
	// criar string para mensagem de falha
	char mensagemFalha[5000];
	// declarar o documento raiz do arquivo entrada
	JSON_entrada jsonDoc;
	// definir arquivo MRT de entrada da simulacao
	FILE *mrtInFile = NULL;
	// definir buffer de entrada da simulacao
	char mrtInBuf[65536];
	// realizar a leitura do arquivo MRT
	try {
		// atualizar a estrutura de resultado do parse do arquivo de entrada da simulacao
		logger.setNomeArqEntrada(impfile);
		// realizar o parse do MRT de entrada
		jsonDoc.load(impfile.c_str());
		// caso parse do MRT de entrada com erro
		if (jsonDoc.HasParseError()) {
			//cout << "ErrorOffset: " << determinarLinhaErro(jsonDoc.GetErrorOffset()) << endl;
			// transpor os dados  para a mensagem
			sprintf(mensagemFalha, "Posicao = %u", jsonDoc.GetErrorOffset());
			// incluir falha no processo de parse
			logger.log_write_logs_and_exit(LOGGER_FALHA,
			LOG_ERR_PARSE_JSON_FORMAT_VALIDATION, "Verificar formato JSON",
					mensagemFalha, GetParseError_Pt_BR(jsonDoc.GetParseError()),
					determinarLinhaErro(jsonDoc.GetErrorOffset()));
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "Verificar formato JSON", "", e.what());
	}
	// incluir info de conclusao do parse do mrt no log
	logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED,
			"Verificar formato JSON", "", "SUCESSO");
	// retorna o documento do MRT
	return jsonDoc;
}

/*!
 * Realizar validacao dos parametros da simulacao contra o schema JSON.
 */
void Ler::validateVsSchema(Document *schemaDoc,
		Document *jsonDoc) {
	// atualizar logger com arquivo MRT de entrda
	logger.setNomeArqEntrada(impfile);
	// realizar a validacao do arquivo MRT segundo o schema
	try {
		// compilar o documento do schema MRT para um SchemaDocument
		SchemaDocument schemaMrt(*schemaDoc);
		// criar validador a partir do SchemaDocument definido para o MRT
		SchemaValidator schemaValidator(schemaMrt);
		// caso o documento MRT lido nao seja validado
		if (!jsonDoc->Accept(schemaValidator)) {
			// definir buffer para validacao do MRT
			StringBuffer schemaValidationStrBuf;
			// obter a propriedade origem da falha
			schemaValidator.GetInvalidDocumentPointer().StringifyUriFragment(
					schemaValidationStrBuf);
			//cout << "linha: " << determinarLinhaErro(schemaValidationStrBuf) << endl;
			// incluir falha no processo de parse
			logger.log_write_logs_and_exit(LOGGER_FALHA,
			LOG_ERR_PARSE_SCHEMA_VALIDATION, "Validar MRT x Schema-MRT",
					schemaValidationStrBuf.GetString(),
					GetParseErrorCause_Pt_BR(
							schemaValidator.GetInvalidSchemaKeyword()),
					determinarLinhaErro(schemaValidationStrBuf.GetString()));
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "Validar MRT x Schema-MRT", "", e.what());
	}
	// incluir info de conclusao do parse do mrt no log
	logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED,
			"Validar MRT x Schema-MRT", "", "SUCESSO");
}

/*!
 * Verificar o valor da chave Ativo da propriedade do arquivo Json do MRT.
 * \tparam propriedade_json - Qualquer das propriedades do arquivo JSON.
 * \return bool Flag indicativo de atividade
 */
template<typename T> bool is_ativo(T& propriedade_json) {
	// caso a propriedade não esteja vazia e exista uma chave "ativo"
	if (propriedade_json.ativo().exists()) {
		// retorna o valor da chave "ativo" da propriedade
		return propriedade_json.ativo();
	} else {
		// TODO: alterar quando tratar os erros no JSON.
		// habilita a propriedade, caso não exista a chave "ativo"
		return true;
	}
}

/*
 * Verificar se algum elemento da propriedade do arquivo Json do MRT é array e se está ativo.
 * \tparam propriedade_json - Qualquer das propriedades do arquivo JSON.
 * \treturn bool
 */
template<typename T> bool contem_ativos(T& propriedade_json) {
	// percorrer os elementos do array
	for (int i = 0; i < (int) propriedade_json.size(); i++) {
		// caso o elemento esteja ativo
		if (is_ativo(propriedade_json[i])) {
			// retorna verdadeiro indicando que existe elemento "ativo" no array
			return true;
		}
	}
	// retorna falso indicando que nao existe elemento "ativo" no array
	return false;
}

/*!
 * Verificar se objeto esta "ativo".
 * \tparam jsonDoc Objeto JSON que se deseja verificar
 * \tparam chave Nome da chave para busca no objeto JSON
 */
template <typename T> bool isArrayPositivoCrescente(T& propriedade_json) {
	// criar variavel para indicar o valor do elemento anterior do array
	double valorAnterior = -1e-9;
	// percorrer os elementos do array
	for (int i = 0; i < (int) propriedade_json.size(); i++) {
		// caso o elemento seja maior que o anterior
		if (propriedade_json[i] <= valorAnterior) {
			// retorna falso indicando que existe elemento "decrescente" no array
			return false;
		}
		valorAnterior = propriedade_json[i];
	}
	// retorna true indicando que os elementos sao crescentes no array
	return true;
}

//template<typename T> bool contemChaveAtiva(T& jsonDoc, string chave) {
//	try {
//		// buscar chave no documento Json
//			Value& propriedade = jsonDoc[chave.c_str()];
//			if (propriedade.IsArray()) {
//				return contem_ativos(propriedade);
//			} else if (propriedade.IsObject()) {
//				return is_ativo(propriedade);
//			}
//	} catch (exception& e) {
//		// incluir falha
//		logger.log_write_logs_and_exit(LOGGER_FALHA,
//		LOG_ERR_UNEXPECTED_EXCEPTION, "contemChaveAtiva", "", e.what());
//	}
//	return false;
//}

#define contemChaveAtiva(jsonDoc, chave) \
	[&jsonDoc](JSONInstance* o){ \
		return is_ativo(jsonDoc.chave());\
	}(&jsonDoc)

#define contemChaveAtivaArray(jsonDoc, chave) \
	[&jsonDoc](JSONInstance* o){ \
		return contem_ativos(jsonDoc.chave());\
	}(&jsonDoc)

/*!
 * Percorrer o vetor de materiais até encontrar o identificador parametrizado.
 * \tparam: identificador Numero que identifica unicamente o material
 */
int Ler::identificarMaterial(int identificador) {
	try {
		// buscar identificador no array de materiais
		int i;
		for (i = 0; i < nmaterial; i++) {
			if (mat[i].id == identificador) {
				return i;
			}
		}
		if(i==nmaterial) logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "identificarMaterial", "",
						"Material não encontrado");
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "identificarMaterial", "", e.what());
	}
	return -1;
}

/*!
 * Percorrer o vetor de secoes transversais até encontrar o identificador parametrizado.
 * \tparam: identificador Numero que identifica unicamente a secao transversal
 */
int Ler::identificarCorte(int identificador) {
	try {
		// buscar identificador no array de secoes transversais
		int i;
		for (i = 0; i < ncorte; i++) {
			if (corte[i].id == identificador) {
				return i;
			}
		}
		if(i==ncorte) logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "identificarSecaoTransversal", "",
						"Secao Transversal não encontrada");
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "identificarCorte", "", e.what());
	}
	return -1;
}

/*!
 * Percorrer o vetor de formacoes (litologias) até encontrar o identificador parametrizado.
 * \tparam: identificador Numero que identifica unicamente a formacao
 */
int Ler::identificarFormacao(int identificador) {
	try {
		// buscar identificador no array de litologias
		int i;
		for (i = 0; i < nform; i++) {
			if (formacPoc[i].id == identificador) {
				return i;
			}
		}
		if(i==nform) logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "identificarFormacao", "",
						"formacao não encontrada");
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "identificarFormacao", "", e.what());
	}
	return -1;
}

/*!
 * Percorrer o vetor de fluidos de producao até encontrar o identificador parametrizado.
 * \tparam: identificador Numero que identifica unicamente o fluido de producao
 */
int Ler::identificarFluidoProducao(int identificador) {
	try {
		// buscar identificador no array de fluidos de producao
		int i;
		for (i = 0; i < nfluP; i++) {
			if (flup[i].id == identificador) {
				return i;
			}
		}
		if(i==nfluP) logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "identificarFluidoProducao", "",
						"fluido não encontrado");
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "identificarFluidoProducao", "",
				e.what());
	}
	return -1;
}

/*!
 * Percorrer o arquivo linha a linha para identificar a linha exata onde foi encontrado um erro.
 * \tparam: posicao do erro.
 */
int Ler::determinarLinhaErro(const char* pathChaveJson) {
	int contadorLinhas = 0;
	ifstream arquivoMrt;
	int posicaoChaveLinhaArquivo, posicaoSeparadorPosterior,
			contadorElementoArray;
	int posicaoSeparadorAnterior = 0;
	string linhaArquivo, propriedadeJson, chaveJson, auxiliar;
	bool flagFimBuscaPath = false;
	bool flagChaveIntermediaria = false;
	bool flagChaveEmArquivo = false;
	bool flagRepetirBuscaChave = true;

	// copiar o parametro de entrada para a variavel de pesquisa
	propriedadeJson = pathChaveJson;
	//cout << "propriedadeJson " << propriedadeJson << endl;

	// -----------------------------------------------
	try {
		// abrir o arquivo MRT para leitura
		arquivoMrt.open(impfile.c_str());
		// caso o arquivo Mrt esteja aberto
		if (arquivoMrt.is_open()) {
			// enquanto nao for fim do arquivo e ainda houver chaves a buscar
			while (!arquivoMrt.eof() && (flagRepetirBuscaChave)) {
				// sinalizar final da busca da chave em arquivo
				flagRepetirBuscaChave = false;

				// caso nao tenha terminado a busca das chaves no path
				if (!flagFimBuscaPath) {
					// indicar que nao eh elemento de array
					contadorElementoArray = 0;
					do {
						// iniciar o valor do flag que indica ter sido encontrada uma chave intermediaria
						flagChaveIntermediaria = false;
						// buscar o caracter separador de chaves Json
						posicaoSeparadorPosterior = propriedadeJson.find("/",
								posicaoSeparadorAnterior);
						//cout << "posicaoSeparadorAnterior " << posicaoSeparadorAnterior << endl;
						//cout << "posicaoSeparadorPosterior " << posicaoSeparadorPosterior << endl;
						// caso nao tenha encontrado um separador posterior
						if (posicaoSeparadorPosterior == string::npos) {
							// finalizar a busca pela chave por ser a ultima separacao do path
							flagFimBuscaPath = true;
							// recuperar a chave a partir da posicao do separador anterior
							auxiliar = propriedadeJson.substr(
									posicaoSeparadorAnterior);
							// converter para inteiro e detectar possivel elemento de array
							//string::size_type tamanhoStr;
							try {
								// eh elemento de array
								atoi(auxiliar.c_str());
							} catch (exception& e) {
								// eh uma chave
								chaveJson = auxiliar;
								// sinalizar busca de outra chave em arquivo
								flagRepetirBuscaChave = true;
							}
							//cout << "tamanhoStr " << tamanhoStr << endl;
							//cout << "auxiliar.size() " << auxiliar.size() << endl;
							//cout << "chaveJson = " << chaveJson << endl;
						} else {
							// caso tenha encontrado um separador posterior
							// recuperar a chave entre a posicao do separador anterior e a do atual
							auxiliar = propriedadeJson.substr(
									posicaoSeparadorAnterior,
									posicaoSeparadorPosterior
											- posicaoSeparadorAnterior);
							//cout << "auxiliar " << auxiliar << endl;
							// atualizar a posicao do separador para a proxima busca
							posicaoSeparadorAnterior = posicaoSeparadorPosterior
									+ 1;
							//cout << "nova posicaoSeparadorAnterior " << posicaoSeparadorAnterior << endl;
							// caso a chave atual nao seja a default
							if (auxiliar != "#") {
								// converter para inteiro e detectar possivel elemento de array
								string::size_type tamanhoStr;
								try {
									// eh elemento de array
									contadorElementoArray = atoi(
											auxiliar.c_str());
									// finalizar a busca pela chave definitiva
									// flagFimBuscaPath = true;
								} catch (exception& e) {
									// eh uma chave intermediaria
									// tamanhoStr = 0;
									chaveJson = auxiliar;
									// interromper a busca por ter encontrado uma chave intermediaria
									flagChaveIntermediaria = true;
								}
								// sinalizar busca de outra chave em arquivo
								flagRepetirBuscaChave = true;
								//cout << "tamanhoStr " << tamanhoStr << endl;
								//cout << "auxiliar.size() " << auxiliar.size() << endl;
								// caso seja numero - elemento de array
								/*              if (tamanhoStr == auxiliar.size()) {
								 cout << "break" << endl;
								 } else {
								 cout << "chaveJson = " << chaveJson << endl;
								 }
								 */}
						}
					} while ((!flagFimBuscaPath) && (!flagChaveIntermediaria));
				}

				// enquanto ainda houver chaves a buscar no arquivo
				while ((!arquivoMrt.eof()) && (flagRepetirBuscaChave)) {
					// incrementar o contador de linhas
					contadorLinhas++;
					// carregar a linha seguinte do arquivo
					getline(arquivoMrt, linhaArquivo);
					// caso encontre a chave na linha lida
					if ((posicaoChaveLinhaArquivo = linhaArquivo.find(chaveJson,
							0)) != string::npos) {
						// indicar encontro da chave no arquivo
						flagChaveEmArquivo = true;
						// interromper a busca
						// break;
						if (--contadorElementoArray <= 0) {
							flagRepetirBuscaChave = false;
						}
					}
				}
				// caso tenha chegado ao fim do arquivo sem encontrar a chave
				if (!flagChaveEmArquivo) {
					// zerar o contador de linhas
					contadorLinhas = 0;
				}

			}

			/*

			 // buscar o caracter separador de chaves Json
			 while ((posicaoSeparadorPosterior = propriedadeJson.find("/", posicaoSeparadorAnterior)) != string::npos) {
			 cout << "posicaoSeparadorAnterior " << posicaoSeparadorAnterior << endl;
			 cout << "posicaoSeparadorPosterior " << posicaoSeparadorPosterior << endl;
			 // recuperar a chave entre a posicao do separador anterior e a do atual
			 auxiliar = propriedadeJson.substr(posicaoSeparadorAnterior, posicaoSeparadorPosterior-posicaoSeparadorAnterior);
			 cout << "auxiliar " << auxiliar << endl;
			 // atualizar a posicao do separador para a proxima busca
			 posicaoSeparadorAnterior = posicaoSeparadorPosterior + 1;
			 cout << "nova posicaoSeparadorAnterior " << posicaoSeparadorAnterior << endl;
			 // caso a chave atual nao seja a default
			 if (auxiliar != "#") {
			 // converter para inteiro e detectar possivel elemento de array
			 string::size_type tamanhoStr;
			 try {
			 stoi(auxiliar, &tamanhoStr);
			 } catch (exception& e) {
			 tamanhoStr = 0;
			 }
			 cout << "tamanhoStr " << tamanhoStr << endl;
			 cout << "auxiliar.size() " << auxiliar.size() << endl;
			 // caso seja numero - elemento de array
			 if (tamanhoStr == auxiliar.size()) {
			 cout << "break" << endl;
			 // interromper a busca pela chave principal
			 flagFimBuscaPath = false;
			 break;
			 } else {
			 // eh uma chave intermediaria
			 chaveJson = auxiliar;
			 cout << "chaveJson = " << chaveJson << endl;
			 }
			 }
			 }
			 // caso tenha sido encontrado o final da propriedadeJson
			 if (flagFimBuscaPath) {
			 // recuperar a chave a partir da posicao do separador anterior
			 auxiliar = propriedadeJson.substr(posicaoSeparadorAnterior);
			 // converter para inteiro e detectar possivel elemento de array
			 string::size_type tamanhoStr;
			 try {
			 stoi(auxiliar, &tamanhoStr);
			 } catch (exception& e) {
			 // eh uma chave intermediaria
			 chaveJson = auxiliar;
			 }
			 cout << "tamanhoStr " << tamanhoStr << endl;
			 cout << "auxiliar.size() " << auxiliar.size() << endl;
			 cout << "chaveJson = " << chaveJson << endl;
			 }
			 */
			// fechar o arquivo MRT
			arquivoMrt.close();
		} else {
			// incluir falha
			logger.log_write_logs_and_exit(LOGGER_FALHA,
			LOG_ERR_UNEXPECTED_EXCEPTION, "determinarLinhaErro", "",
					"Arquivo MRT nao pode ser lido");
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "determinarLinhaErro", "", e.what());
	}
	return contadorLinhas;
}

/*!
 * Percorrer o arquivo caracter a caracter para identificar a linha exata onde foi encontrado um erro.
 * \tparam posicaoErro posicao em offset no arquivo JSON de entrada onde foi acuasado erro no formato JSON
 */
int Ler::determinarLinhaErro(int posicaoErro) {
	int contadorLinhas = 0;
	int contadorCaracteresLidos = 0;
	ifstream arquivoMrt;
	string linha;
	try {
		// abrir o arquivo MRT para leitura
		arquivoMrt.open(impfile.c_str());
		// caso o arquivo Mrt esteja aberto
		if (arquivoMrt.is_open()) {
			// enquanto nao for fim do arquivo
			while (!arquivoMrt.eof()) {
				// incrementar o contador de linhas
				contadorLinhas++;
				// carregar a linha seguinte do arquivo
				getline(arquivoMrt, linha);
				// atualizar o contador da quantidade de caracteres lidos (incluindo o '\n')
				contadorCaracteresLidos += linha.size() + 1;
				// caso a quantidade de caracteres lidos ultrapasse a posicao relativa do erro
				if (contadorCaracteresLidos > posicaoErro) {
					// interromper a leitura do arquivo
					break;
				}
			}
			// fechar o arquivo MRT
			arquivoMrt.close();
		} else {
			// incluir falha
			logger.log_write_logs_and_exit(LOGGER_FALHA,
			LOG_ERR_UNEXPECTED_EXCEPTION, "determinarLinhaErro", "",
					"Arquivo MRT nao pode ser lido");
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "determinarLinhaErro", "", e.what());
	}
	// retorna o numero da linha onde o erro foi identificado
	return contadorLinhas;
}

/*!
 * Verificar se o array da propriedade do MRT é array e se possui valores crescentes.
 * \tparam propriedade_json Qualquer das propriedades do arquivo JSON.
 * \treturn bool
 */
/*
 * Verificar se algum elemento da propriedade do arquivo Json do MRT é array e se está ativo.
 * \tparam propriedade_json - Qualquer das propriedades do arquivo JSON.
 * \treturn bool
 */
/*bool Ler::verificarUnicidade(int *identificadores,
		int tamanhoVetor, int maiorElemento) {

	// criar vetor para verificar a unicidade dos identificadores
	bool* unicidade = new bool[maiorElemento + 1];
	for (int i = 0; i <= maiorElemento; i++) {
		unicidade[i] = false;
	}
	// percorrer os elementos do array
	for (int i = 0; i < tamanhoVetor; i++) {
		// caso o elemento esteja ativo
		if (unicidade[identificadores[i]]) {
			// retorna false indicando que existem elementos "id" duplicados no vetor
			return false;
		} else {
			unicidade[identificadores[i]] = true;
		}
	}
	// retorna true indicando que nao existem elementos "id" duplicados no vetor
	return true;
}*/
bool Ler::verificarUnicidade(const std::vector<int>& identificadores) {
    if (identificadores.empty()) return true;

    // Encontrar o maior elemento
    int maiorElemento = *std::max_element(identificadores.begin(), identificadores.end());

    // Usar vector para unicidade (gerencia memória automaticamente)
    std::vector<bool> unicidade(maiorElemento + 1, false);

    // percorrer os elementos do array
    for (int id : identificadores) {
        if (unicidade[id]) {
            return false; // elemento duplicado encontrado
        } else {
            unicidade[id] = true;
        }
    }

    return true; // todos os elementos são únicos
}


// Função auxiliar para ler uma linha "lógica" com continuações com barra invertida (\)
std::string Ler::ler_linha_continua(std::ifstream& in) {
	std::string linha, temp;
	while (std::getline(in, temp)) {
		// Remove espaços e caracteres de fim de linha (como \r)
		temp.erase(std::find_if(temp.rbegin(), temp.rend(),
			[](unsigned char ch) { return !std::isspace(ch); }).base(), temp.end());

		if (!temp.empty() && temp.back() == '\\') {
			temp.pop_back(); // Remove o '\'
			linha += temp;
		} else {
			linha += temp;
			break;
		}
	}
	return linha;
}

/*!
 * Converter o elemento "configuracaoInicial" do arquivo Json do MRT em struct mrtjson::configuracao_inicial_mrt_t.
 *
 * \tparam Value&
 * \treturn mrtjson::configuracao_inicial_mrt_t
 */
void Ler::parse_configuracao_inicial(
		JSON_entrada_configuracaoInicial& configuracao_inicial_json) {

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/configuracaoInicial");

	// propriedade do fluido: 0 - black oil; 1 - hibrido
	int propFluido = 0;

	try {
		// poco injetor
		lingas = 0;
		equilterm = 1;
		latente = 0;
		condlatente = 1;
		modelcp = 0;
		modelJTL = 0;
		tabp = 0;
		tabg = 0;
		perm = 1;
		trackRGO = 1;
		trackDeng = 1;
		corrDeng=0;
		ordperm = 1;
		transiente = 0;
		transmass = 0;
		indfluPIni = 0;
		//sem flash
		tabRSPB = 0;
		propFluido = 0;
		flashCompleto = 0;
		tabelaDinamica=0;
		// avancado
		mono = 1e-4;
		critcond = 0.001;

		saidaTela=0;

		escorregaPerm = 0;
		escorregaTran = 0;
		mapaArranjo = 0;

		limTransMass=10.;
		CriterioConvergPerm=0.001;
		AceleraConvergPerm=0;
		escorregamentoCelulaContorno=0;
		correcaoContracorPerm=0;
		correcaoMassaEspLiq=0;
		desligaDeriTransMassDTemp=0;
		corrigeContSep=1;
		nTcorrecaoMassaEspLiq=1;
		nTsonico=1;
		taxaDespre=0.1;
		estabCol=0;
		acopColAnulPermForte=0;

		tipoFluido=0;
		descarga=0;
		perm = 1;
		snaps = 0;
		transiente =0;
		ConContEntrada = 0;
		mudaArea=0;

		miniTabAtraso=0;
		miniTabDp=10.;
		miniTabDt=10.;

		razCompGasReves=0.;
		tempReves=-1000.;

		AS=0;
		paralelAS=0;

		nthrd=1;
		buscaFC=0.1;

		chutePerm=-1.;

		HISEP=0;

		modoXY=0;
		xProd0=0;
		yProd0=0;
		xServ0=0;
		yServ0=0;

		controleDTvalv=0;

		//reduzAtrito=0;


		// indicador do sentido de calculo do comprimento da linha de producao
		origemGeometria = origemGeometria_t::montante; // sentido plataforma para fundo-poco
		/*if (configuracao_inicial_json.origemGeometria().exists()) {
			string valorLido(configuracao_inicial_json.origemGeometria());
			transform(valorLido.begin(), valorLido.end(), valorLido.begin(),
					::toupper);
			if (valorLido.compare("PLATAFORMA") == 0) {
				origemGeometria = origemGeometria_t::jusante;
			} else if (valorLido.compare("POCO") == 0) {
				origemGeometria = origemGeometria_t::montante;
			}
		}*/

		// indicador do sentido dos angulos dos dutos em relacao ao do escoamento
		sentidoGeometriaSegueEscoamento = true; // angulos do duto a favor do sentido do escoamento
		if (configuracao_inicial_json.sentidoGeometriaSegueEscoamento().exists()) {
			sentidoGeometriaSegueEscoamento = configuracao_inicial_json.sentidoGeometriaSegueEscoamento();
		}
		if(reverso==1)sentidoGeometriaSegueEscoamento=0;

		// indicador do sentido dos angulos dos dutos em relacao ao do escoamento
		saidaClassica = 1; // angulos do duto a favor do sentido do escoamento
		if (configuracao_inicial_json.saidaClassica().exists()) {
			saidaClassica = configuracao_inicial_json.saidaClassica();
		}

		if (configuracao_inicial_json.AS().exists())
			AS = configuracao_inicial_json.AS();
		if (configuracao_inicial_json.paralelizaAS().exists())
			paralelAS = configuracao_inicial_json.paralelizaAS();

		if (configuracao_inicial_json.HISEP().exists())
			HISEP = configuracao_inicial_json.HISEP();

		if (configuracao_inicial_json.chutePerm().exists())
			chutePerm = configuracao_inicial_json.chutePerm();

		// caso nao seja simulacao de poco injetor
		if (pocinjec == 0) {
			if (configuracao_inicial_json.linhaGas().exists())
				lingas = configuracao_inicial_json.linhaGas();
			//if (configuracao_inicial_json.redutorAtritoCalc().exists())
				//reduzAtrito = configuracao_inicial_json.redutorAtritoCalc();
			if (configuracao_inicial_json.saidaTela().exists())
				saidaTela = configuracao_inicial_json.saidaTela();
			if (configuracao_inicial_json.equilibrioTermico().exists())
				equilterm =
					configuracao_inicial_json.equilibrioTermico();
			if (configuracao_inicial_json.latente().exists())
				latente = configuracao_inicial_json.latente();
			if (configuracao_inicial_json.condlatente().exists())
				condlatente = configuracao_inicial_json.condlatente();
			if (configuracao_inicial_json.pvtsimArq().exists())
				pvtsimarq = pathArqExtEntrada
						+ string(configuracao_inicial_json.pvtsimArq());
			if (configuracao_inicial_json.modeloFluidoTabelaFlash().exists())
				flashCompleto =
						configuracao_inicial_json.modeloFluidoTabelaFlash();
			// caso simulacao com tabela flash - modeloFluidoTabelaFlash: true
			if (configuracao_inicial_json.modoParafina().exists())
				modoParafina=configuracao_inicial_json.modoParafina();

			if (flashCompleto == 0) {
				int verifComp=0;
				if (configuracao_inicial_json.modeloFluidoComposicional().exists())
					verifComp=configuracao_inicial_json.modeloFluidoComposicional();
				flashCompleto+=2*verifComp;
				if(flashCompleto==2){
					if (configuracao_inicial_json.modeloTabelaDinamica().exists())
						tabelaDinamica=configuracao_inicial_json.modeloTabelaDinamica();
				}
				if(flashCompleto == 0){
					// RN-088: Chave "modeloCp" requerida em caso de "modeloFluidoTabelaFlash"=false
					if (configuracao_inicial_json.modeloCp().exists())
						modelcp = configuracao_inicial_json.modeloCp();
					/*else
						logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"modeloCp requerido", chaveJson,
								"modeloFluidoTabelaFlash=false");*/
					if (configuracao_inicial_json.modeloJTL().exists())
						modelJTL = configuracao_inicial_json.modeloJTL();
					// RN-089: Chave "tabP" requerida em caso de "modeloFluidoTabelaFlash"=false
					if (configuracao_inicial_json.tabP().exists())
						tabp = configuracao_inicial_json.tabP();
					/*else
						logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "tabP requerido",
								chaveJson, "modeloFluidoTabelaFlash=false");*/
					// RN-090: Chave "trackRgo" requerida em caso de "modeloFluidoTabelaFlash"=falseMedSimpPresFront=0;
					if (configuracao_inicial_json.trackRgo().exists())
						trackRGO = configuracao_inicial_json.trackRgo();
					/*else
						logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"trackRgo requerido", chaveJson,
								"modeloFluidoTabelaFlash=false");*/
					// RN-091: Chave "trackDensidadeGas" requerida em caso de "modeloFluidoTabelaFlash"=false
					if (configuracao_inicial_json.trackDensidadeGas().exists())
						trackDeng =
								configuracao_inicial_json.trackDensidadeGas();
					/*else
						logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"trackDensidadeGas requerido", chaveJson,
								"modeloFluidoTabelaFlash=false");*/
					if (configuracao_inicial_json.correcaoDenGasLivreBlackOil().exists())
						corrDeng = configuracao_inicial_json.correcaoDenGasLivreBlackOil();
					if (configuracao_inicial_json.tabelaRSPB().exists())
						tabRSPB = configuracao_inicial_json.tabelaRSPB();
					if (configuracao_inicial_json.propFluido().exists())
						propFluido =
							configuracao_inicial_json.propFluido();
					// RN-061: Uma das chaves "latente" ou "modeloCp" ou "modeloJTL" requerida
					if (propFluido == 1 && latente == 0 && modelcp == 0
							&& modelJTL == 0)
						logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Chave requerida: latente, modeloCp ou modeloJTL",
								chaveJson, "propFluido");
					// RN-092: Chave "trackDensidadeGas" requerida em caso de "modeloFluidoTabelaFlash"=false
					/*if (configuracao_inicial_json.iniFluidoP().exists())
						indfluPIni =
								configuracao_inicial_json.iniFluidoP();
					else{
						if()
						logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"iniFluidoP requerido", chaveJson,
								"modeloFluidoTabelaFlash=false");
					}*/
				}
				else{
					if (pvtsimarq.length() == 0) {
						// RN-042: Chave "compArq" requerida em caso de "modeloFluidoComposicional"
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"arquivo pvtsim requerido", chaveJson,
								"modeloFluidoComposicional");
						// Caso o arquivo pvtsim inexista
					} else if (!ifstream(pvtsimarq)) {
						// criar variavel para o nome da propriedade json em processo de parse
						string chavePvtsimArq(chaveJson + "/pvtsimArq");
						// RN-043: Arquivo "pvtsimArq" inexistente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Arquivo inexistente", chavePvtsimArq, pvtsimarq);
					}
				}
			}
			// caso as opcoes flashCompleto, latente, modeloCp ou modeloJTL tenham sido marcadas
			if (flashCompleto == 1 || latente == 1 || modelcp == 1
					|| modelJTL == 1) {
				if (pvtsimarq.length() == 0) {
					// RN-042: Chave "pvtsimArq" requerida em caso de "modeloFluidoTabelaFlash", "latente" ou "modeloCp" ou "modeloJTL"
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"arquivo pvtsim requerido", chaveJson,
							"modeloFluidoTabelaFlash, latente, modeloCp, modeloJTL");
					// Caso o arquivo pvtsim inexista
				} else if (!ifstream(pvtsimarq)) {
					// criar variavel para o nome da propriedade json em processo de parse
					string chavePvtsimArq(chaveJson + "/pvtsimArq");
					// RN-043: Arquivo "pvtsimArq" inexistente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Arquivo inexistente", chavePvtsimArq, pvtsimarq);
				}
			}

			if (configuracao_inicial_json.tabG().exists())
				tabg = configuracao_inicial_json.tabG();
			if(flashCompleto!=0)tabg=0;

			escorregaPerm = 1;
			if (configuracao_inicial_json.escorregamentoPermanente().exists())
				escorregaPerm =
						configuracao_inicial_json.escorregamentoPermanente();
			escorregaTran = 1;
			if (configuracao_inicial_json.escorregamentoTransiente().exists())
				escorregaTran =
						configuracao_inicial_json.escorregamentoTransiente();

			mapaArranjo = 0;
			if (configuracao_inicial_json.mapaArranjo().exists())
				mapaArranjo =
						configuracao_inicial_json.mapaArranjo();

			tipoFluido = 0;
			if (configuracao_inicial_json.tipoFluido().exists())
				tipoFluido =
						configuracao_inicial_json.tipoFluido();
			if((*vg1dSP).tipoFluidoRedeGlob==1)tipoFluido=1;

			if (configuracao_inicial_json.tipoModeloDrift().exists())
				tipoModeloDrift =
						configuracao_inicial_json.tipoModeloDrift();
			if((*vg1dSP).chaverede==1 ){
				tipoModeloDrift=(*vg1dSP).tipoModeloDrift;
			}
			else (*vg1dSP).tipoModeloDrift=tipoModeloDrift;

			// condicao inicial: 0 – informada pelo usuario, 1 – permanente, 2 – restart, 3 – descarga de gas lift
			int condicaoInicial = 1;
			perm = 1;
			snaps = 0;
			descarga = 0;
			// caso a chave condicaoInicial esteja preenchida
			if (configuracao_inicial_json.condicaoInicial().exists())
				condicaoInicial =
						configuracao_inicial_json.condicaoInicial();
			switch (condicaoInicial) {
			case 0:
				perm = 0;
				break;
			case 1:
				perm = 1;
				// RN-063: Chave "ordemperm" requerida
				if (configuracao_inicial_json.ordemperm().exists())
					ordperm = configuracao_inicial_json.ordemperm();
				/*else
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Chave ordemperm requerida", chaveJson,
							"condicaoInicial = 1");*/
				break;
			case 2:
				snaps = 1;
				perm = 2;
				if (configuracao_inicial_json.SnapShotArq().exists())
					snapshotArqIn =
							pathArqExtEntrada
									+ string(configuracao_inicial_json.SnapShotArq());
				else
					snapshotArqIn = "teste1.snp";
				// Caso o arquivo snapshot inexista
				if (!ifstream(snapshotArqIn)) {
					// criar variavel para o nome da propriedade json em processo de parse
					string chaveSnapShotArq(chaveJson + "/SnapShotArq");
					// RN-048: Arquivo "SnapShotArq" inexistente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Arquivo inexistente", chaveSnapShotArq,
							snapshotArqIn);
				}
				break;
			case 3:
				descarga = 1;
				perm = 0;
				snaps = 0;
				// caso a chave SalinidadeFluido esteja preenchida
				if (configuracao_inicial_json.SalinidadeFluido().exists()) {
					salinDescarga =
							configuracao_inicial_json.SalinidadeFluido();
				} else {
					// RN-058: Chave "SalinidadeFluido" requerida em caso de "descarga" true
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"SalinidadeFluido requerida", chaveJson,
							"descarga");
				}
				// caso a chave comprimentoMedidoInterfaceLinhaGas esteja preenchida
				if (configuracao_inicial_json.comprimentoMedidoInterfaceLinhaGas().exists()) {
					compInterDesc =
							configuracao_inicial_json.comprimentoMedidoInterfaceLinhaGas();
				} else {
					// RN-059: Chave "comprimentoMedidoInterfaceLinhaGas" requerida em caso de "descarga" true
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"comprimentoMedidoInterfaceLinhaGas requerida",
							chaveJson, "descarga");
				}
				// caso a chave comprimentoMedidoInterfaceLinhaProd esteja preenchida
				if (configuracao_inicial_json.comprimentoMedidoInterfaceLinhaProd().exists()) {
					compInterDescP =
							configuracao_inicial_json.comprimentoMedidoInterfaceLinhaProd();
				} else {
					// RN-060: Chave "comprimentoMedidoInterfaceLinhaProd" requerida em caso de "descarga" true
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"comprimentoMedidoInterfaceLinhaProd requerida",
							chaveJson, "descarga");
				}
				controDesc = 0;
				// caso controle de descarga preenchido
				if (configuracao_inicial_json.controleDescarga().exists()) {
					controDesc =
							configuracao_inicial_json.controleDescarga();
					// caso controle de descarga ativado
					if (controDesc == 1) {
						// caso parametros de descarga preenchidos
						if (configuracao_inicial_json.parametrosDescarga().exists()) {
							// carregar os parametros para controle da descarga
							vazDescControl =
									configuracao_inicial_json.parametrosDescarga().vazaoLimiteDescarga()
											/ 86400;
							presMaxDesc =
									configuracao_inicial_json.parametrosDescarga().pressaoLimiteDescarga();
							presMinDesc =
									configuracao_inicial_json.parametrosDescarga().pressaoMinimaDescarga();
							presIniDesc =
									configuracao_inicial_json.parametrosDescarga().pressaoTrabalhoDescargaGas();
							presMaxDescG =
									configuracao_inicial_json.parametrosDescarga().pressaoLimiteDescargaGas();
							presMinDescG =
									configuracao_inicial_json.parametrosDescarga().pressaoMinimaDescargaGas();
							presIniDescG =
									configuracao_inicial_json.parametrosDescarga().pressaoInicialDescargaGas();
							tempDescarga =
									configuracao_inicial_json.parametrosDescarga().temperaturaDescarga();
							tempoLatenciaDesc =
									configuracao_inicial_json.parametrosDescarga().tempoLatencia();
						} else {
							// RN-063: Objeto "parametrosDescarga" requerido
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Objeto parametrosDescarga requerido",
									chaveJson, "controleDescarga=true");
						}
					}
				}
				break;
			default:
				// RN-065: Chave "condicaoInicial" requerida
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Condicao inicial incorreta", chaveJson,
						"condicaoInicial");
			}
			if (flashCompleto == 0 && configuracao_inicial_json.iniFluidoP().exists())
				indfluPIni =
						configuracao_inicial_json.iniFluidoP();
			else{
				if(condicaoInicial!=0){
					indfluPIni =0;
				}
				else
				logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"iniFluidoP requerido", chaveJson,
						"modeloFluidoTabelaFlash=false");
			}
			if((*vg1dSP).chaverede==0 /*|| configuracao_inicial_json.transiente().exists()*/){
				
				if(tipoModeloDrift==1){
					transiente = configuracao_inicial_json.transiente();
					(*vg1dSP).simulaTransiente=transiente;
				}
				else{ 
					if(configuracao_inicial_json.transiente().exists()){
						transiente = configuracao_inicial_json.transiente();
						if(transiente==1)
						logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
		  	      		    		"Tentando rodar transiente  com correlacoes caixa preta, transiente sera desconsiderado",
		  	      		          "", "");
					}
					transiente = 0;
					(*vg1dSP).simulaTransiente=transiente;
				}
			}
			else{ 
				if((*vg1dSP).tipoModeloDrift==1)
					transiente=(*vg1dSP).chaveredeT;
				else transiente=0;
			}

			if (configuracao_inicial_json.transferenciaMassa().exists())
				transmass =
					configuracao_inicial_json.transferenciaMassa();
			if (configuracao_inicial_json.CheckValve().exists())
				chkv = configuracao_inicial_json.CheckValve();
			else
				chkv = 1;

			if (configuracao_inicial_json.modoDifus3D().exists()){
				modoDifus3D=configuracao_inicial_json.modoDifus3D();
				if(configuracao_inicial_json.threadP3D().exists())threadP3D=configuracao_inicial_json.threadP3D();
				else threadP3D=1;
				modoDifus3DJson=configuracao_inicial_json.modoDifus3DJson();
			}

			if (configuracao_inicial_json.Avancado().exists()) {
				mono = 1e-4;
				if (configuracao_inicial_json.Avancado().CriterioMonofasico().exists())
				mono =
						configuracao_inicial_json.Avancado().CriterioMonofasico();
				if (configuracao_inicial_json.Avancado().CriterioCondensacao().exists())
				critcond = 1e-3;
				critcond =
						configuracao_inicial_json.Avancado().CriterioCondensacao();
			    if (configuracao_inicial_json.Avancado().taxaDespre().exists())
			      taxaDespre=configuracao_inicial_json.Avancado().taxaDespre();
			    else
				taxaDespre=0.01;
			    if (configuracao_inicial_json.Avancado().MedSimpPresFront().exists())
			    	MedSimpPresFront=configuracao_inicial_json.Avancado().MedSimpPresFront();
			    else MedSimpPresFront=0;
			    if (configuracao_inicial_json.Avancado().JTLiquidoSimple().exists())
			    	JTLiquidoSimple=configuracao_inicial_json.Avancado().JTLiquidoSimple();
			    else JTLiquidoSimple=0.;
			    if (configuracao_inicial_json.Avancado().RelaxaDTChoke().exists())
			    	RelaxaDTChoke=configuracao_inicial_json.Avancado().RelaxaDTChoke();
			    else RelaxaDTChoke=0;
			    if (configuracao_inicial_json.Avancado().desligaPenalizaDT().exists())
			    	desligaPenalizaDT=configuracao_inicial_json.Avancado().desligaPenalizaDT();
			    if (configuracao_inicial_json.Avancado().controleDTvalv().exists())
			    	controleDTvalv=configuracao_inicial_json.Avancado().controleDTvalv();
			    if (configuracao_inicial_json.Avancado().limTransMass().exists())
			    	limTransMass=configuracao_inicial_json.Avancado().limTransMass();
			    else limTransMass=10.;
			    if (configuracao_inicial_json.Avancado().CriterioDTMin().exists())
			    	(*vg1dSP).CritDTMin=configuracao_inicial_json.Avancado().CriterioDTMin();
			    if (configuracao_inicial_json.Avancado().CriterioBuscaFalsaCorda().exists())
			    	buscaFC=configuracao_inicial_json.Avancado().CriterioBuscaFalsaCorda();
			    if (configuracao_inicial_json.Avancado().CriterioConvergPerm().exists())
			    	CriterioConvergPerm=configuracao_inicial_json.Avancado().CriterioConvergPerm();
			    if (configuracao_inicial_json.Avancado().AceleraConvergPerm().exists())
			    	AceleraConvergPerm=configuracao_inicial_json.Avancado().AceleraConvergPerm();
			    if (configuracao_inicial_json.Avancado().escorregamentoCelulaContorno().exists())
			    	escorregamentoCelulaContorno=configuracao_inicial_json.Avancado().escorregamentoCelulaContorno();
			    if (configuracao_inicial_json.Avancado().correcaoContracorPerm().exists())
			    	correcaoContracorPerm=configuracao_inicial_json.Avancado().correcaoContracorPerm();
				/*nestabCol=0;
				if (configuracao_inicial_json.Avancado().tempoEstabCol().exists()){
					nestabCol= configuracao_inicial_json.Avancado().tempoEstabCol().size();
					vecTestabCol = new double[nestabCol];
					vecestabCol = new int[nestabCol];
					for (int i = 0; i < nestabCol; i++) {
						vecTestabCol[i]  = configuracao_inicial_json.Avancado().tempoEstabCol()[i];
						vecestabCol[i]  = configuracao_inicial_json.Avancado().estabCol()[i];
					}
				}
				else{
					nestabCol= 1;
					vecTestabCol = new double[nestabCol];
					vecestabCol = new int[nestabCol];
					for (int i = 0; i < nestabCol; i++) {
						vecTestabCol[i]  = 0;
						vecestabCol[i]  = 0;
					}
				}
				estabCol=vecestabCol[0];*/
			    if (configuracao_inicial_json.Avancado().estabCol().exists())
			    	estabCol=configuracao_inicial_json.Avancado().estabCol();
			    if (configuracao_inicial_json.Avancado().acopColAnulPermForte().exists())
			    	acopColAnulPermForte=configuracao_inicial_json.Avancado().acopColAnulPermForte();
			    if (configuracao_inicial_json.Avancado().mudaArea().exists())
			    	mudaArea=configuracao_inicial_json.Avancado().mudaArea();
			    if (configuracao_inicial_json.Avancado().nthrd().exists())
			    	nthrd=configuracao_inicial_json.Avancado().nthrd();
			    if (configuracao_inicial_json.Avancado().nthrdMatriz().exists())
			    	nthrdMatriz=configuracao_inicial_json.Avancado().nthrdMatriz();
			    if (configuracao_inicial_json.Avancado().correcaoModComp().exists() &&
			    		configuracao_inicial_json.Avancado().TcorrecaoModComp().exists()){
			    	nTcorrecaoMassaEspLiq =	configuracao_inicial_json.Avancado().correcaoModComp().size();
			    	TcorrecaoMassaEspLiq = new double[nTcorrecaoMassaEspLiq];
			    	VcorrecaoMassaEspLiq = new int[nTcorrecaoMassaEspLiq];
			    	for(int i=0;i<nTcorrecaoMassaEspLiq;i++){
			    		VcorrecaoMassaEspLiq[i]=configuracao_inicial_json.Avancado().correcaoModComp()[i];
			    		TcorrecaoMassaEspLiq[i]=configuracao_inicial_json.Avancado().TcorrecaoModComp()[i];
			    	}
			    	correcaoMassaEspLiq=VcorrecaoMassaEspLiq[0];
			    	if(correcaoMassaEspLiq==1)cicloAcopTerm=1;
			    	else cicloAcopTerm=0;
			    }
			    else{
			    	TcorrecaoMassaEspLiq = new double[1];
			    	VcorrecaoMassaEspLiq = new int[1];
			    	VcorrecaoMassaEspLiq[0]=0;
			    	TcorrecaoMassaEspLiq[0]=0.;
					correcaoMassaEspLiq=0;
					cicloAcopTerm=0;
			    }
			    if (configuracao_inicial_json.Avancado().sonico().exists() &&
			    		configuracao_inicial_json.Avancado().Tsonico().exists()){
			    	nTsonico =	configuracao_inicial_json.Avancado().sonico().size();
			    	Tempsonico = new double[nTsonico];
			    	Vsonico = new int[nTsonico];
			    	for(int i=0;i<nTsonico;i++){
			    		Vsonico[i]=configuracao_inicial_json.Avancado().sonico()[i];
			    		Tempsonico[i]=configuracao_inicial_json.Avancado().Tsonico()[i];
			    	}
			    }
			    else{
			    	Tempsonico = new double[1];
			    	Vsonico = new int[1];
			    	Vsonico[0]=0;
			    	Tempsonico[0]=0.;
			    }
			    if (configuracao_inicial_json.Avancado().desligaDeriTransMassDTemp().exists())
			    	desligaDeriTransMassDTemp=configuracao_inicial_json.Avancado().desligaDeriTransMassDTemp();
			    if (configuracao_inicial_json.Avancado().corrigeContSep().exists())
			    	corrigeContSep=configuracao_inicial_json.Avancado().corrigeContSep();
			    if (configuracao_inicial_json.Avancado().miniTabDinAtraso().exists())
			    	miniTabAtraso=configuracao_inicial_json.Avancado().miniTabDinAtraso();
			    if (configuracao_inicial_json.Avancado().miniTabDinDp().exists())
			    	miniTabDp=configuracao_inicial_json.Avancado().miniTabDinDp();
			    if (configuracao_inicial_json.Avancado().miniTabDinDt().exists())
			    	miniTabDt=configuracao_inicial_json.Avancado().miniTabDinDt();

			} else {
				mono = 1e-4;
				critcond = 1e-3;
				cicloAcopTerm=0;
				MedSimpPresFront=0;
				RelaxaDTChoke=0;
				limTransMass=10.;
				CriterioConvergPerm=0.001;
				AceleraConvergPerm=0;
				escorregamentoCelulaContorno=0;
				correcaoContracorPerm=0;
		    	TcorrecaoMassaEspLiq = new double[1];
		    	VcorrecaoMassaEspLiq = new int[1];
		    	VcorrecaoMassaEspLiq[0]=0;
		    	TcorrecaoMassaEspLiq[0]=0.;
				correcaoMassaEspLiq=0;
				desligaDeriTransMassDTemp=0;
				corrigeContSep=1;
				taxaDespre=0.01;
				estabCol=0;
				acopColAnulPermForte=0;
				nthrd=1;
		    	Tempsonico = new double[1];
		    	Vsonico = new int[1];
		    	Vsonico[0]=0;
		    	Tempsonico[0]=0.;
			}

			(*vg1dSP).ntrd=nthrd;
			(*vg1dSP).ntrd3DP=threadP3D;

			ConContEntrada = 0;  //alteracao4
			if (configuracao_inicial_json.condicaoPressao().exists()
					&& configuracao_inicial_json.condicaoPressao().ativo()) {
				ConContEntrada = 1;
				int indicaBeta=0;
				if (configuracao_inicial_json.condicaoPressao().razaoBeta().exists())indicaBeta=1;
				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((configuracao_inicial_json.condicaoPressao().tempo().size()
						!= configuracao_inicial_json.condicaoPressao().temperatura().size())
						|| (configuracao_inicial_json.condicaoPressao().tempo().size()
								!= configuracao_inicial_json.condicaoPressao().pressao().size())
						|| (configuracao_inicial_json.condicaoPressao().tempo().size()
								!= configuracao_inicial_json.condicaoPressao().titulo().size())
						|| (indicaBeta && configuracao_inicial_json.condicaoPressao().tempo().size()
								!= configuracao_inicial_json.condicaoPressao().razaoBeta().size())) {
					// RN-078: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves tempo, temperatura, pressao, titulo e razaoBeta de #/configuracao_inicial/condicaoPressao com tamanhos diferentes");
				} else {
					CCPres.parserie =
							configuracao_inicial_json.condicaoPressao().pressao().size();
					CCPres.indFluido=-1;
					if (configuracao_inicial_json.condicaoPressao().indFluido().exists())
						CCPres.indFluido =configuracao_inicial_json.condicaoPressao().indFluido();
					CCPres.temperatura = new double[CCPres.parserie];
					CCPres.pres = new double[CCPres.parserie];
					CCPres.tit = new double[CCPres.parserie];
					CCPres.bet = new double[CCPres.parserie];
					CCPres.tempo = new double[CCPres.parserie];
					for (int i = 0; i < CCPres.parserie; i++)
						CCPres.temperatura[i] =
								configuracao_inicial_json.condicaoPressao().temperatura()[i];
					for (int i = 0; i < CCPres.parserie; i++)
						CCPres.pres[i] =
								configuracao_inicial_json.condicaoPressao().pressao()[i];
					for (int i = 0; i < CCPres.parserie; i++)
						CCPres.tit[i] =
								configuracao_inicial_json.condicaoPressao().titulo()[i];
					for (int i = 0; i < CCPres.parserie; i++){
						if(indicaBeta){
							CCPres.bet[i] =
								configuracao_inicial_json.condicaoPressao().razaoBeta()[i];
							if(tipoFluido==1)CCPres.bet[i]/=(100.*1e6);
						}
						else CCPres.bet[i] =0.;
					}
					for (int i = 0; i < CCPres.parserie; i++)
						CCPres.tempo[i] =
								configuracao_inicial_json.condicaoPressao().tempo()[i];
				}  //alteracao4
			}
			else if (configuracao_inicial_json.condicaoVazPres().exists()
					&& configuracao_inicial_json.condicaoVazPres().ativo()) {
				ConContEntrada = 2;
				int indicaBeta=0;
				if (configuracao_inicial_json.condicaoVazPres().razaoBeta().exists())indicaBeta=1;
				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((configuracao_inicial_json.condicaoVazPres().tempo().size()
						!= configuracao_inicial_json.condicaoVazPres().temperatura().size())
						|| (configuracao_inicial_json.condicaoVazPres().tempo().size()
								!= configuracao_inicial_json.condicaoVazPres().pressao().size())
						|| (configuracao_inicial_json.condicaoVazPres().tempo().size()
								!= configuracao_inicial_json.condicaoVazPres().VazMass().size())
						|| (indicaBeta && configuracao_inicial_json.condicaoVazPres().tempo().size()
								!= configuracao_inicial_json.condicaoVazPres().razaoBeta().size())) {
					// RN-078: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves tempo, temperatura, pressao, vazao massica e razaoBeta de #/configuracao_inicial/condicaoVazPres com tamanhos diferentes");
				} else {
					CCVPres.parserie =
							configuracao_inicial_json.condicaoVazPres().pressao().size();
					CCVPres.temperatura = new double[CCVPres.parserie];
					CCVPres.pres = new double[CCVPres.parserie];
					CCVPres.mass = new double[CCVPres.parserie];
					CCVPres.bet = new double[CCVPres.parserie];
					CCVPres.tempo = new double[CCVPres.parserie];
					for (int i = 0; i < CCVPres.parserie; i++)
						CCVPres.temperatura[i] =
								configuracao_inicial_json.condicaoVazPres().temperatura()[i];
					for (int i = 0; i < CCVPres.parserie; i++)
						CCVPres.pres[i] =
								configuracao_inicial_json.condicaoVazPres().pressao()[i];
					for (int i = 0; i < CCVPres.parserie; i++)
						CCVPres.mass[i] =
								configuracao_inicial_json.condicaoVazPres().VazMass()[i];
					for (int i = 0; i < CCVPres.parserie; i++){
						if(indicaBeta){
							CCVPres.bet[i] =
								configuracao_inicial_json.condicaoVazPres().razaoBeta()[i];
							if(tipoFluido==1)CCVPres.bet[i]/=(100.*1e6);
						}
						else CCVPres.bet[i] =0.;
					}
					for (int i = 0; i < CCVPres.parserie; i++)
						CCVPres.tempo[i] =
								configuracao_inicial_json.condicaoVazPres().tempo()[i];
				}  //alteracao4
			}


			// correlacoes de escorregamento
			CorreEstrat = 1;
			CorreDisper = 1;
			CorreAnular = 1;
			// caso flag de correlacoes de escorregamento preenchido e com TRUE
			//if (configuracao_inicial_json.correlacoesEscorregamento().exists()
					//&& configuracao_inicial_json.correlacoesEscorregamento()) {
				// caso correlacoes por arranjo preenchido
				if (configuracao_inicial_json.correlacoesPorArranjo().exists()) {
					// carregar as correlacoes por arranjo
					CorreEstrat =
							configuracao_inicial_json.correlacoesPorArranjo().estratificado();
					CorreDisper =
							configuracao_inicial_json.correlacoesPorArranjo().bolhaGolfada();
					CorreAnular =
							configuracao_inicial_json.correlacoesPorArranjo().anularChurn();
				} /*else {
					// RN-087: Objeto "correlacoesPorArranjo" requerido
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Objeto correlacoesPorArranjo requerido", chaveJson,
							"correlacoesEscorregamento=true");
				}*/
			//}
		}
		else{
	    	TcorrecaoMassaEspLiq = new double[1];
	    	VcorrecaoMassaEspLiq = new int[1];
			if (configuracao_inicial_json.modeloFluidoTabelaFlash().exists())
				flashCompleto =
						configuracao_inicial_json.modeloFluidoTabelaFlash();
			if(flashCompleto==0){
				int verifComp=0;
				if (configuracao_inicial_json.modeloFluidoComposicional().exists())
					verifComp=configuracao_inicial_json.modeloFluidoComposicional();
				flashCompleto+=2*verifComp;
			}
			if (configuracao_inicial_json.ordemperm().exists())
								ordperm = configuracao_inicial_json.ordemperm();
		}
		if (configuracao_inicial_json.modoXY().exists())
			modoXY=configuracao_inicial_json.modoXY();
		if(modoXY==1){
			if (configuracao_inicial_json.xProdInicio().exists())
				xProd0=configuracao_inicial_json.xProdInicio();
			if (configuracao_inicial_json.yProdInicio().exists())
				yProd0=configuracao_inicial_json.yProdInicio();
			if(lingas>0){
				if (configuracao_inicial_json.xServInicio().exists())
					xServ0=configuracao_inicial_json.xServInicio();
				if (configuracao_inicial_json.yServInicio().exists())
					yServ0=configuracao_inicial_json.yServInicio();
			}
		}


		if (configuracao_inicial_json.tempReves().exists())tempReves=configuracao_inicial_json.tempReves();
		if (configuracao_inicial_json.razCompGasReves().exists())razCompGasReves=configuracao_inicial_json.razCompGasReves();

		if (configuracao_inicial_json.Formacao().exists()) {
			if (configuracao_inicial_json.Formacao().Propriedades().exists())
				nform =
					configuracao_inicial_json.Formacao().Propriedades().size(); //alteracao2
			else nform=1;
			double tempProd=365;
			if (configuracao_inicial_json.Formacao().TempoProducao().exists())
			tempProd=
					configuracao_inicial_json.Formacao().TempoProducao(); //alteracao2
			//else
				//logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										//"formacao rochosa - propriedades termicas", "tempo de producao", "inexistencia do tempo de producao");
			formacPoc = new detFormacao[nform];  //alteracao2
			// criar vetor de inteiros para armazenar os ids
			//int* identificadores = NULL;
			std::vector<int> identificadores;
			// criar variavel para o maior identificador encontrado
			int maiorIdentificador = -99999;
			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nform];
			identificadores.resize(nform);
			//formacPoc.tempo=configuracao_inicial_json.Formacao().tempo();  //alteracao2
			//formacPoc.cond=configuracao_inicial_json.Formacao().condutividade();  //alteracao2
			//formacPoc.cp=configuracao_inicial_json.Formacao().calorEspecifico();  //alteracao2
			//formacPoc.rho=configuracao_inicial_json.Formacao().massaEspecifica();  //alteracao2

			if (configuracao_inicial_json.Formacao().Propriedades().exists()){
				for (int i = 0; i < nform; i++) {  //alteracao2
					// obter maior identificador
					identificadores[i] =
						configuracao_inicial_json.Formacao().Propriedades()[i].id();
					// caso o identificador seja maior que o ultimo selecionado, substitui
					if (identificadores[i] > maiorIdentificador) {
						maiorIdentificador = identificadores[i];
					}
					// obter id da formacao
					formacPoc[i].id = identificadores[i];
					formacPoc[i].tempo = tempProd;  //alteracao2
					formacPoc[i].cond =
						configuracao_inicial_json.Formacao().Propriedades()[i].condutividade(); //alteracao2
					formacPoc[i].cp =
						configuracao_inicial_json.Formacao().Propriedades()[i].calorEspecifico(); //alteracao2
					formacPoc[i].rho =
						configuracao_inicial_json.Formacao().Propriedades()[i].massaEspecifica(); //alteracao2
				}  //alteracao2
			}
			else{
				// obter maior identificador
				identificadores[0] =0;
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[0] > maiorIdentificador) {
					maiorIdentificador = identificadores[0];
				}
				// obter id da formacao
				formacPoc[0].id = identificadores[0];
				formacPoc[0].tempo = tempProd;  //alteracao2
				formacPoc[0].cond =2.5; //alteracao2
				formacPoc[0].cp =1250.; //alteracao2
				formacPoc[0].rho =2000.; //alteracao2
			}
			   // verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// criar variavel para o nome da propriedade json em processo de parse
				string chaveFormacaoId(chaveJson + "/Formacao/Propridades");
				// RN-052: Unicidade da chave 'id' em '#/configuracaoInicial'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveFormacaoId, "Unicidade da chave 'id'");
			}
		} else {
			nform = 1;  //alteracao2
			formacPoc = new detFormacao[nform];  //alteracao2
			formacPoc[0].tempo = -100.;  //altercacao2
			formacPoc[0].cond = 0.;  //altercacao2
			formacPoc[0].cp = 0.;  //altercacao2
			formacPoc[0].rho = 0.;  //altercacao2
			formacPoc[0].id = 0;
			//nform = 0;  //altercacao2
		}

		// caso exista uma condicao de falha da aplicacao
		if (!logger.getStResultadoSimulacao().sucesso) {
			// gerar o arquivo de saida da simulacao e encerra a aplicacao
			logger.write_logs_and_exit();
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter o elemento "CondicaoContPocInjec" do arquivo Json do MRT em struct mrtjson::CONDCONPOCINJEC_mrt_t.
 *
 * \tparam Value&
 * \treturn mrtjson::CONDCONPOCINJEC_mrt_t
 */
void Ler::parse_condcont_pocinjec(JSON_entrada_CondicaoContPocInjec& condcont_json, int temFluido) {
	/*
	 "CondicaoContPocInjec" :{
	 "ativo": false, "condcontorno": 2, "PVTSim": "PVTSIM_MARLIM.tab", "usuario": "acarvalho", "salinidade": 20.0, "tempinj": 40.0, "vazLiq": 1000.0, "presInjec": 33.91, "presFundo": 339.0
	 }
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/CondicaoContPocInjec");
	// criar variavel para o usuario do Marlim Producao, caso esteja sendo usado pela interface em CSBASE
	string usuario("");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(condcont_json)) {
			// caso o elemento usuario exista
			if (condcont_json.usuario().exists()) {
				// obter o valor da chave
				usuario = string(condcont_json.usuario());
			}

			condpocinj.tipoFlui = condcont_json.tipoFluido();
			// caso o tipo do fluido seja 1 - agua salinizada
			if (condpocinj.tipoFlui == 1) {
				// caso a chave salinidade exista e seja double
				if (condcont_json.salinidade().exists()) {
					// obter o valor da chave
					condpocinj.salin = condcont_json.salinidade();
				} else {
					// RN-016: Chave "salinidade" requerida para tipo de fluido agua salinizada (tipofluido = 1)
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"salinidade requerida", chaveJson, "tipoFlui");
				}
				// caso o tipo do fluido seja 2 - CO2
			} else if (condpocinj.tipoFlui == 2) {
				// caso a chave PVTSim exista
				if(temFluido==1)flashCompleto=1;
				else flashCompleto=0;
				if (condcont_json.arquivoPvtsim().exists()) {
					// caso o usuario tenha sido informado
					if (usuario.length() > 0) {
						// obter o valor da chave incluindo o usuario no path
						condpocinj.pvtsimarqInj = pathArqExtEntrada + usuario
								+ BARRA
								+ string(condcont_json.arquivoPvtsim());
					} else {
						// obter o valor da chave
						condpocinj.pvtsimarqInj = pathArqExtEntrada
								+ string(condcont_json.arquivoPvtsim());
					}
					// Caso o arquivo pvtsim inexista
					if (!ifstream(condpocinj.pvtsimarqInj)) {
						// criar variavel para o nome da propriedade json em processo de parse
						string chavePvtsimArqInj(chaveJson + "/arquivoPvtsim");
						// RN-043: Arquivo "PVTSim" inexistente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Arquivo inexistente", chavePvtsimArqInj,
								condpocinj.pvtsimarqInj);
					}
				}
				else {

					// RN-017: Chave "PVTSim" requerida para tipo de fluido CO2 (tipofluido = 2)
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"arquivoPvtsim requerida", chaveJson,
							"tipoFlui CO2");
				}
			}
			else if(condpocinj.tipoFlui == 3){
				flashCompleto=2;
				tabp=0;
				// caso a chave PVTSim exista
				if (condcont_json.arquivoPvtsim().exists()) {
					// caso o usuario tenha sido informado
					if (usuario.length() > 0) {
						// obter o valor da chave incluindo o usuario no path
						condpocinj.pvtsimarqInj = pathArqExtEntrada + usuario
								+ BARRA
								+ string(condcont_json.arquivoPvtsim());
					} else {
						// obter o valor da chave
						condpocinj.pvtsimarqInj = pathArqExtEntrada
								+ string(condcont_json.arquivoPvtsim());
					}
					// Caso o arquivo pvtsim inexista
					if (!ifstream(condpocinj.pvtsimarqInj)) {
						// criar variavel para o nome da propriedade json em processo de parse
						string chavePvtsimArqInj(chaveJson + "/arquivoPvtsim");
						// RN-043: Arquivo "PVTSim" inexistente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Arquivo inexistente", chavePvtsimArqInj,
								condpocinj.pvtsimarqInj);
					}
				}
				else {

					// RN-017: Chave "PVTSim" requerida para tipo de fluido CO2 (tipofluido = 2)
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"arquivoPvtsim requerida", chaveJson,
							"tipoFlui CO2");
				}
			}
			//if (condpocinj.tipoFlui == 2 && flashCompleto==2)condpocinj.tipoFlui = 3;
			condpocinj.CC = condcont_json.condContorno();
			condpocinj.tempinj = condcont_json.tempInj();

			condpocinj.vazinj = 0;
			// caso condicao de contorno 0, 4 ou 5
			if (condpocinj.CC == 0 || condpocinj.CC == 4
					|| condpocinj.CC == 5) {
				// caso a chave vazliq inexista
				if (condcont_json.vazLiq().exists()) {
					condpocinj.vazinj = condcont_json.vazLiq();
				} else {
					// RN-018: Chave "vazLiq" requerida para condicoes de contorno (CC = 0, 4 ou 5)
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "vazLiq requerida",
							chaveJson, "CC");
				}
			}

			condpocinj.presinj = 0;
			// caso condicao de contorno 1, 3 ou 4
			if (condpocinj.CC == 1 || condpocinj.CC == 3
					|| condpocinj.CC == 4) {
				// caso a chave presInjec inexista nas condicao de contorno 1, 3 ou 4
				if (condcont_json.presInjec().exists()) {
					condpocinj.presinj = condcont_json.presInjec();
				} else {
					// RN-019: Chave "presInjec" requerida para as condições de contorno (CC = 1, 3 ou 4)
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"presInjec requerida", chaveJson, "CC");
				}
			}

			condpocinj.presfundo = 0;
			// caso condicao de contorno 2, 3 ou 5
			if (condpocinj.CC == 2 || condpocinj.CC == 3
					|| condpocinj.CC == 5) {
				// caso a chave presFundo inexista nas condicao de contorno 2, 3 ou 5
				if (condcont_json.presFundo().exists()) {
					condpocinj.presfundo =
							condcont_json.presFundo();
				} else {
					// RN-020: Chave "presFundo" requerida para as condições de contorno (CC = 2, 3 ou 5)
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"presFundo requerida", chaveJson, "CC");
				}
			}
		}
		// caso exista uma condicao de falha da aplicacao ate esta etapa
		if (!logger.getStResultadoSimulacao().sucesso) {
			// gerar o arquivo de saida da simulacao e encerra a aplicacao
			logger.write_logs_and_exit();
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter o elemento "tempo" do arquivo Json do MRT em struct detTMAX.
 *
 * \tparam Objeto JSON correspondente a detTMAX
 */
void Ler::parse_tempo(JSON_entrada_tempo& tempo_json) {
	/*
	 "tempo": {
	 "tempoFinal": 50001,
	 "tempos": [0 , 1000.0 , 1060 , 29000 , 29060 , 31000 , 31060],
	 "dtmax": [4 , 4 , 4 , 4 , 4 , 4 , 4]
	 }
	 */
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/tempo");
	try {
		tfinal = tempo_json.tempoFinal();
		if(tempo_json.dtmax().exists() && tempo_json.tempos().exists())	dtmaxserie.parserie = tempo_json.tempos().size();
		else dtmaxserie.parserie = 1;
		dtmaxserie.tempo = new double[dtmaxserie.parserie];
		dtmaxserie.valor = new double[dtmaxserie.parserie];
		nsegrega=0;
		if (tempo_json.tempoSegrega().exists()){
			nsegrega= tempo_json.tempoSegrega().size();
			vecTSegrega = new double[nsegrega];
			vecSegrega = new int[nsegrega];
			for (int i = 0; i < nsegrega; i++) {
				vecTSegrega[i]  = tempo_json.tempoSegrega()[i];
				vecSegrega[i]  = tempo_json.segrega()[i];
			}
		}
		else{
			nsegrega= 1;
			vecTSegrega = new double[nsegrega];
			vecSegrega = new int[nsegrega];
			for (int i = 0; i < nsegrega; i++) {
				vecTSegrega[i]  = 0;
				vecSegrega[i]  = 1;
			}
		}
		modoSegrega=vecSegrega[0];
		nsnp=0;
		tempsnp=0;
		if (tempo_json.gravaMomento().exists()){
			nsnp = tempo_json.gravaMomento().size();
			tempsnp = new double[nsnp];
		}
		for (int i = 0; i < nsnp; i++) {
			tempsnp[i]  = tempo_json.gravaMomento()[i];
		}
		// percorrer a lista de elementos dos arrays tempos e dtmax. else {
		// caso o vetor de tempos nao seja positivo e crescente
		if (!isArrayPositivoCrescente(tempo_json.tempos())) {
			// RN-008: chave tempos nao positiva e crescente
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					chaveJson, chaveJson,
					"Chave #/tempo/tempos com valores nao crescentes");
		}
		if(tempo_json.dtmax().exists() && tempo_json.tempos().exists()){
		// caso os tamanhos dos vetores das chaves tempos e dtmax difiram entre si
			if (tempo_json.tempos().size() != tempo_json.dtmax().size()) {
			// RN-007: chaves tempos e dtmax com tamanhos diferentes
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chaves #/tempo/tempos e #/tempo/dtmax com tamanhos diferentes");
			} else {
			// percorrer a lista de elementos dos arrays tempos e dtmax.
				for (int i = 0; i < dtmaxserie.parserie; i++) {
					dtmaxserie.tempo[i] = tempo_json.tempos()[i];
					dtmaxserie.valor[i] = tempo_json.dtmax()[i];
				}
				dtmax = dtmaxserie.valor[0];
			}
		}
		else{
			dtmaxserie.tempo[0] = 0.;
			dtmaxserie.valor[0] = 5.;
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter o elemento "tabela" do arquivo Json do MRT em struct .
 *
 * \tparam Objeto JSON correspondente à tabela
 */
void Ler::parse_tabela(JSON_entrada_tabela& tabela_json) {
	/*
	 * "tabela": { "ativo": false, "nPontos": 21, "pressaoMaxima": 600, "pressaominima": 2, "temperaturaMaxima": 200, "temperaturaMinima": -100 }
	 */
	// caso a propriedade "ativo" esteja habilitada
	if (is_ativo(tabela_json)) {
		// de-para do elemento "tabela" obtido do json para o struct correspondente
		// de-para do elemento "tabela" obtido do json para o struct correspondente
		tabent.npont = tabela_json.nPontos();
		tabent.npontB=tabent.npont;
		tabent.pmax = tabela_json.pressaoMaxima();
		tabent.pmin = tabela_json.pressaoMinima();
		tabent.tmax = tabela_json.temperaturaMaxima();
		tabent.tmin = tabela_json.temperaturaMinima();
		npontos = tabent.npont;
	}
}

void Ler::parse_parafina(JSON_entrada_parafina& parafina_json) {
	string chaveJson("#/parafina");
	detalParafina.arquivo="";
	detalParafina.poroRey=0;
	detalParafina.valRey=0;
	detalParafina.C2C3=0;
	detalParafina.valC2=1.;
	detalParafina.valC3=1.;
	detalParafina.difus=0;
	detalParafina.alteraViscFlu=0;
	detalParafina.multDifus=1.;
	detalParafina.rug=0.;
	detalParafina.multVis=1.;
	detalParafina.DmultipWax = 1.; // Samuel - 10/10/25
	detalParafina.EmultipWax = 1.; // Samuel - 10/10/25
	detalParafina.FmultipWax = 1.; // Samuel - 10/10/25

	if (parafina_json.arquivoWax().exists())
		detalParafina.arquivo = parafina_json.arquivoWax();
	else
		logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				chaveJson, chaveJson,
				"Chave #/parafina sem arquivo WAX");
	if (parafina_json.usuarioPorosidade().exists()){
		detalParafina.poroRey = parafina_json.usuarioPorosidade();
		if(detalParafina.poroRey==1){
			if (parafina_json.porosidade().exists())
				detalParafina.valRey = parafina_json.porosidade();
			else
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chave #/parafina sem porosidade");
		}
	}
	if (parafina_json.usuarioC2C3().exists()){
		detalParafina.C2C3 = parafina_json.usuarioC2C3();
		if(detalParafina.C2C3==1){

			if (parafina_json.c2().exists())
				detalParafina.valC2 = parafina_json.c2();
			else
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chave #/parafina sem C2");

			if (parafina_json.c3().exists())
				detalParafina.valC3 = parafina_json.c3();
			else
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chave #/parafina sem C3");
		}
	}
	if (parafina_json.usuarioDifus().exists()){
		detalParafina.difus = parafina_json.usuarioDifus();
		if(detalParafina.difus==1){
			if (parafina_json.difus().exists())
				detalParafina.multDifus = parafina_json.difus();
			else
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chave #/parafina sem multiplicador de difusividade");
		}
	}
	if (parafina_json.alteraViscFlu().exists()){
		detalParafina.alteraViscFlu = parafina_json.alteraViscFlu();
	}

	// Importante: Definir parafina_json.Multips_D_E_F().exists(), parafina_json.D_multip_Wax(), parafina_json.E_multip_Wax() e parafina_json.F_multip_Wax() // Samuel - 10/10/25
	if (parafina_json.DmultipWax().exists())detalParafina.DmultipWax = parafina_json.DmultipWax(); // Samuel - 10/10/25
	if (parafina_json.EmultipWax().exists())detalParafina.DmultipWax = parafina_json.EmultipWax(); // Samuel - 10/10/25
	if (parafina_json.FmultipWax().exists())detalParafina.DmultipWax = parafina_json.FmultipWax(); // Samuel - 10/10/25

	if (parafina_json.rugosidade().exists())
		detalParafina.rug = parafina_json.rugosidade();
	else
		logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				chaveJson, chaveJson,
				"Chave #/parafina sem rugosidade");

	if (parafina_json.multVis().exists())
		detalParafina.multVis = parafina_json.multVis();
}

/*!
 * Converter o elemento "fluidosProducao" do arquivo Json do MRT em struct flup.
 *
 * \tparam Objeto JSON correspondente à fluidos producao
 */
void Ler::parse_fluidos_producao(
		JSON_entrada_fluidosProducao& fluidos_producao_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fluidosProducao");

	double api = 20;
	double rgo = 0;
	double deng = 1;
	double bsw = 0;
	double denag = 1;
	double templ = 10;
	double lvisl = 2;
	double temph = 20;
	double lvish = 1;
	int tipoemul = 0;
	double bswCorte = 1;
	double yco2 = 0;
	int corrC = 1;
	int corrSat = 0;  //alteracao4
	double aemul = 0.;  //alteracao4
	double bemul = 0.;  //alteracao4
	int vcorrOM = 0;
	int vcorrOV = 0;
	int vcorrOS = 0;
	int fracMolarUsuario=0;
	double PHI100=0.765;
	int nvecBSW=0;
	// obter o tamanho do elemento do Json
	nfluP = 0;
	try {
		// percorre o array de fluidos de producao
		for (size_t i = 0; i < fluidos_producao_json.size(); i++) {
			// caso o fluido de producao esteja ativo
			if (is_ativo(fluidos_producao_json[i])) {
				nfluP++;
			}
		}

		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;
		// caso o tamanho do elemento seja maior que zero
		if (nfluP > 0) {
			flup = new ProFlu [nfluP];
			indFlup=new int [nfluP];
			tabVisc= new detTabVisc[nfluP];
			for(int iflu=0;iflu<nfluP;iflu++){
				tabVisc[iflu].parserie=0;
				tabVisc[iflu].temp=0;
				tabVisc[iflu].visc=0;
				indFlup[iflu]=0;
			}
			// loop para carga do vetor do JSON
			int indAtivo = -1;
			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nfluP];
			identificadores.resize(nfluP);
			if (flashCompleto == 0) {
				// loop para carga do vetor do JSON
				for (int i = 0; i < nfluP; i++) {
					// enquanto a propriedade "ativo" do fluido de producao esteja desabilitada, avança
					while (!is_ativo(fluidos_producao_json[++indAtivo]))
						;
					// obter maior identificador
					indFlup[i]=identificadores[i] =
							fluidos_producao_json[indAtivo].id();
					// caso o identificador seja maior que o último selecionado, substitui
					if (identificadores[i] > maiorIdentificador) {
						maiorIdentificador = identificadores[i];
					}
					// de-para do elemento "fluidosProducao" do json para o struct
					api = fluidos_producao_json[indAtivo].api();
					rgo = fluidos_producao_json[indAtivo].rgo();
					deng =
							fluidos_producao_json[indAtivo].densidadeGas();
					bsw = fluidos_producao_json[indAtivo].bsw();
					if(bsw>1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"BSW > 1", chaveJson,
								"BSW>1");
					}
					denag = 1;
					if (fluidos_producao_json[indAtivo].densidadeAgua().exists())
						denag =
								fluidos_producao_json[indAtivo].densidadeAgua();
					if(denag<1.0)denag=1.0;

					tipoemul = 0;
					if (fluidos_producao_json[indAtivo].tipoEmul().exists()) {
						tipoemul =
								fluidos_producao_json[indAtivo].tipoEmul();
						// caso modelo exponencial
						if (tipoemul == 4) {
							if (!fluidos_producao_json[indAtivo].coefAModeloExp().exists()
									|| !fluidos_producao_json[indAtivo].coefBModeloExp().exists()) {
								// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
										chaveJson, "tipoEmul=4");
							} else {
								aemul =
										fluidos_producao_json[indAtivo].coefAModeloExp();
								bemul =
										fluidos_producao_json[indAtivo].coefBModeloExp();
							}
						}
						else if(tipoemul == 5 && fluidos_producao_json[indAtivo].PHI100().exists())
							PHI100=fluidos_producao_json[indAtivo].PHI100();
						else if(tipoemul == 6 && nvecBSW==0){
							nvecBSW=1;
							if(!fluidos_producao_json[indAtivo].BSWVec().exists() ||
							   !fluidos_producao_json[indAtivo].emulVec().exists()){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' requeridas",
										chaveJson, "tipoEmul=6");
							}
							else{
								nvecEmul =fluidos_producao_json[indAtivo].BSWVec().size();
								if(nvecEmul!=fluidos_producao_json[indAtivo].emulVec().size()){
									logger.log(LOGGER_FALHA,
									LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
											"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
											chaveJson, "tipoEmul=6");
								}
								else{
									BSWVec = new double[nvecEmul];
									emulVec = new double[nvecEmul];
									for (int iemul = 0; iemul < nvecEmul; iemul++) {
										BSWVec[iemul] = fluidos_producao_json[indAtivo].BSWVec()[iemul];
										emulVec[iemul] = fluidos_producao_json[indAtivo].emulVec()[iemul];
									}
								}
							}
						}
					}
					bswCorte = 1.;
					if (fluidos_producao_json[indAtivo].bswCorte().exists())
						bswCorte =
								fluidos_producao_json[indAtivo].bswCorte();
					if (fluidos_producao_json[indAtivo].fracCO2().exists())
					yco2 =
							fluidos_producao_json[indAtivo].fracCO2();
					if (fluidos_producao_json[indAtivo].correlacaoCritica().exists())
					corrC =
							fluidos_producao_json[indAtivo].correlacaoCritica();

					if (fluidos_producao_json[indAtivo].modeloRsPb().exists()) {
						corrSat =
								fluidos_producao_json[indAtivo].modeloRsPb(); //alteracao4
						// caso modelo RsPb da Livia
						if (corrSat == 4 && tabRSPB==1) {
							// TODO: CORRIGIR ESSE TRECHO DE CÓDIGO NO FUTURO
							corrSat = -4;
							/*if (pvtsimarq.length() == 0) {
								// RN-042: Chave "pvtsimArq" requerida em caso de "latente" ou "modeloCp" ou "modeloJTL"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"arquivo pvtsim requerido", chaveJson,
										"modeloRsPb=4");
								// Caso o arquivo pvtsim inexista
							} else if (!ifstream(pvtsimarq)) {
								// criar variavel para o nome da propriedade json em processo de parse
								string chavePvtsimArq(chaveJson + "/pvtsimArq");
								// RN-043: Arquivo "pvtsimArq" inexistente
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Arquivo inexistente", chavePvtsimArq,
										pvtsimarq);
							}*/
						}
					}

					vcorrOM = 3;
					// caso modelo oleo morto preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoMorto().exists()) {
						//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
						vcorrOM =
								fluidos_producao_json[indAtivo].modeloOleoMorto();
					}
					if (vcorrOM == 0) {
						if (!fluidos_producao_json[indAtivo].temp1().exists()
								|| !fluidos_producao_json[indAtivo].visc1().exists()
								|| !fluidos_producao_json[indAtivo].temp2().exists()
								|| !fluidos_producao_json[indAtivo].visc2().exists()) {
							// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
									chaveJson,
									"modeloOleoMorto = 0 ou ausente");
						} else {
							templ =
									fluidos_producao_json[indAtivo].temp1();
							lvisl =
									fluidos_producao_json[indAtivo].visc1();
							temph =
									fluidos_producao_json[indAtivo].temp2();
							lvish =
									fluidos_producao_json[indAtivo].visc2();
						}
					}
					else if (vcorrOM == 7) {
						if (!fluidos_producao_json[indAtivo].tempOleoMorto().exists()
								|| !fluidos_producao_json[indAtivo].viscOleoMorto().exists()) {
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'tempOleoMorto', 'viscOleoMorto' requeridas",
									chaveJson,
									"modeloOleoMorto = 7 ou ausente");
						} else {
							tabVisc[i].parserie=fluidos_producao_json[indAtivo].tempOleoMorto().size();
							tabVisc[i].temp=new double[tabVisc[i].parserie];
							tabVisc[i].visc=new double[tabVisc[i].parserie];
							for(int itab=0;itab<tabVisc[i].parserie;itab++){
								tabVisc[i].temp[itab]=fluidos_producao_json[indAtivo].tempOleoMorto()[itab];
								tabVisc[i].visc[itab]=fluidos_producao_json[indAtivo].viscOleoMorto()[itab];
							}
						}
					}

					// caso modelo oleo vivo preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoVivo().exists())
						//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
						vcorrOV =
								fluidos_producao_json[indAtivo].modeloOleoVivo();

					// caso modelo oleo subsaturado preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoSubSaturado().exists())
						//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
						vcorrOS =
								fluidos_producao_json[indAtivo].modeloOleoSubSaturado();

					flup[i] = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul, PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, identificadores[i]); //alteracao4
					flup[i].zdranP = zdranP;  //alteracao2
					flup[i].dzdpP = dzdpP;  //alteracao2
					flup[i].dzdtP = dzdtP;  //alteracao2
					flup[i].npontos = npontos;
					flup[i].viscBlackOil = 1;
					flup[i].modelaAgua = 1;
					flup[i].corrDeng=corrDeng;
					flup[i].nvecEmul=nvecEmul;
					flup[i].BSWVec=BSWVec;
					flup[i].emulVec=emulVec;
					//flup[i].dStockTankVaporDensity=flup[i].FracMass(1.01, 20.);
					flup[i].dStockTankVaporMassFraction=flup[i].FracMass(1.01, 20.);
					flup[i].parserie=tabVisc[i].parserie;
					flup[i].viscTab=tabVisc[i].visc;
					flup[i].tempTab=tabVisc[i].temp;
					flup[i].JTLiquidoSimple=JTLiquidoSimple;
				}
			} else if(flashCompleto==1){
				///////////////////////////////////////////////////////////
				string dadosMRPre;
				if(pocinjec==1)dadosMRPre =condpocinj.pvtsimarqInj;
				else dadosMRPre = pvtsimarq;
				ifstream lendoPVTSimPre(dadosMRPre.c_str(), ios_base::in);
				string chave;
				char* tenta;
				//tenta = new char[400];
				double testatok;
				char line[50000];

				// Lê linha com possível quebra
				//std::string linha_completa = ler_linha_continua(lendoPVTSimPre);
				//strcpy(line, linha_completa.c_str());

				//lendoPVTSimPre.get(line, 4000);
				//tenta = strtok(line, " ,()=");
				lendoPVTSimPre >> chave;
				while (chave != "PRESSURE") {
					lendoPVTSimPre >> chave;
					if(lendoPVTSimPre.eof( )){
						logger.log_write_logs_and_exit(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Arquivo PVTSIM com problemas", chaveJson,
								"pvtsimArq");
						break;
					}
				}

				//lendoPVTSimPre.get(line, 4000);

				// Lê novamente linha com quebra e monta linha unificada
				std::string linha_completa = ler_linha_continua(lendoPVTSimPre);
				strcpy(line, linha_completa.c_str());

				int ntab=0;
				tenta = strtok(line, " ,()=");
				chave=std::string(tenta);
				while(tenta != nullptr && (chave!="Pa" && chave!="PA")){
					ntab++;
					tenta = strtok(NULL, " ,)/");
					chave=std::string(tenta);

					if(lendoPVTSimPre.eof( )){
						logger.log_write_logs_and_exit(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Arquivo PVTSIM com problemas", chaveJson,
								"pvtsimArq");
					}

					if(chave=="\r"){
						tenta = strtok(NULL, " ,)/");
						if(lendoPVTSimPre.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
						tenta = strtok(NULL, " ,)/");
						if(lendoPVTSimPre.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
				}
				tabent.npont=ntab;

				tenta = strtok(NULL, " ,()=");
				chave=std::string(tenta);
				ntab=0;
				while(tenta != nullptr && chave!="BUBBLEPRESSURES"){
					tenta = strtok(NULL, " ,)/");
					chave=std::string(tenta);
					if(lendoPVTSimPre.eof( )){
						logger.log_write_logs_and_exit(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Arquivo PVTSIM com problemas", chaveJson,
								"pvtsimArq");
						break;
					}
				}


				tenta = strtok(NULL, " ,()/=");
				chave=std::string(tenta);
				while(tenta != nullptr && (chave!="Pa" && chave!="PA")){
					ntab++;
					tenta = strtok(NULL, " ,()/=");
					chave=std::string(tenta);

					if(lendoPVTSimPre.eof( )){
						logger.log_write_logs_and_exit(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Arquivo PVTSIM com problemas", chaveJson,
								"pvtsimArq");
					}

					if(chave=="\r"){
						tenta = strtok(NULL, " ,)/");
						if(lendoPVTSimPre.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
						tenta = strtok(NULL, " ,)/");
						if(lendoPVTSimPre.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
				}
				tabent.npontB=ntab;

				lendoPVTSimPre.close();
				//////////////////////////////////////////////////

				flash = new tabelaFlash[nfluP];
				for (int i = 0; i < nfluP; i++) {
					// enquanto a propriedade "ativo" do fluido de producao esteja desabilitada, avança
					while (!is_ativo(fluidos_producao_json[++indAtivo]))
						;
					// obter maior identificador
					indFlup[i]=identificadores[i] =
							fluidos_producao_json[indAtivo].id();
					// caso o identificador seja maior que o último selecionado, substitui
					if (identificadores[i] > maiorIdentificador) {
						maiorIdentificador = identificadores[i];
					}

					if(pocinjec==0) flash[i].arquivo = pvtsimarq;
					else flash[i].arquivo = condpocinj.pvtsimarqInj;

					bsw = fluidos_producao_json[indAtivo].bsw();
					if(bsw>1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"BSW > 1", chaveJson,
								"BSW>1");
					}
					denag = 1;
					if (fluidos_producao_json[indAtivo].densidadeAgua().exists())
						denag =
								fluidos_producao_json[indAtivo].densidadeAgua();
					if(denag<1.0)denag=1.0;

					int modelagua=1;
					if (fluidos_producao_json[indAtivo].modeloAguaBlackOil().exists())  //alteracao6
						modelagua =
								fluidos_producao_json[indAtivo].modeloAguaBlackOil();

					flash[i].visc = 0;  // modelo viscosidade tabela flash
					if (fluidos_producao_json[indAtivo].modeloViscBlackOil().exists())  //alteracao6
						flash[i].visc =
								fluidos_producao_json[indAtivo].modeloViscBlackOil(); //alteracao6
					if (flash[i].visc == 1) {  //modelo viscosidade black oil
						vcorrOM = 0;
						// caso modelo oleo morto preenchido
						if (fluidos_producao_json[indAtivo].modeloOleoMorto().exists()) {
							//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
							vcorrOM =
									fluidos_producao_json[indAtivo].modeloOleoMorto();
						}
						if (vcorrOM == 0) {
							if (!fluidos_producao_json[indAtivo].temp1().exists()
									|| !fluidos_producao_json[indAtivo].visc1().exists()
									|| !fluidos_producao_json[indAtivo].temp2().exists()
									|| !fluidos_producao_json[indAtivo].visc2().exists()) {
								// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
										chaveJson,
										"modeloOleoMorto = 0 ou ausente");
							} else {
								templ =
										fluidos_producao_json[indAtivo].temp1();
								lvisl =
										fluidos_producao_json[indAtivo].visc1();
								temph =
										fluidos_producao_json[indAtivo].temp2();
								lvish =
										fluidos_producao_json[indAtivo].visc2();
							}
						}
						else if (vcorrOM == 7) {
							if (!fluidos_producao_json[indAtivo].tempOleoMorto().exists()
									|| !fluidos_producao_json[indAtivo].viscOleoMorto().exists()) {
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'tempOleoMorto', 'viscOleoMorto' requeridas",
										chaveJson,
										"modeloOleoMorto = 7 ou ausente");
							} else {
								tabVisc[i].parserie=fluidos_producao_json[indAtivo].tempOleoMorto().size();
								tabVisc[i].temp=new double[tabVisc[i].parserie];
								tabVisc[i].visc=new double[tabVisc[i].parserie];
								for(int itab=0;itab<tabVisc[i].parserie;itab++){
									tabVisc[i].temp[itab]=fluidos_producao_json[indAtivo].tempOleoMorto()[itab];
									tabVisc[i].visc[itab]=fluidos_producao_json[indAtivo].viscOleoMorto()[itab];
								}
							}
						}

						if (fluidos_producao_json[indAtivo].modeloOleoVivo().exists())
							//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
							vcorrOV =
									fluidos_producao_json[indAtivo].modeloOleoVivo();
						if (fluidos_producao_json[indAtivo].modeloOleoSubSaturado().exists())
							//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
							vcorrOS =
									fluidos_producao_json[indAtivo].modeloOleoSubSaturado();
					} else {
						vcorrOM = 4;
						vcorrOV = 0;
						vcorrOS = 0;
					}

					tipoemul = 0;
					if (fluidos_producao_json[indAtivo].tipoEmul().exists())
						tipoemul =
								fluidos_producao_json[indAtivo].tipoEmul();
					bswCorte = 1.;
					if (fluidos_producao_json[indAtivo].bswCorte().exists())
						bswCorte =
								fluidos_producao_json[indAtivo].bswCorte();

					if (tipoemul == 4) {
						if (!fluidos_producao_json[indAtivo].coefAModeloExp().exists()
								|| !fluidos_producao_json[indAtivo].coefBModeloExp().exists()) {
							// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
									chaveJson, "tipoEmul=4");
						} else {
							aemul =
									fluidos_producao_json[indAtivo].coefAModeloExp();
							bemul =
									fluidos_producao_json[indAtivo].coefBModeloExp();
						}
					}
					else if(tipoemul == 5 && fluidos_producao_json[indAtivo].PHI100().exists())
						PHI100=fluidos_producao_json[indAtivo].PHI100();
					else if(tipoemul == 6 && nvecBSW==0){
						nvecBSW=1;
						if(!fluidos_producao_json[indAtivo].BSWVec().exists() ||
						   !fluidos_producao_json[indAtivo].emulVec().exists()){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'BSWVec', 'emulVec' requeridas",
									chaveJson, "tipoEmul=6");
						}
						else{
							nvecEmul =fluidos_producao_json[indAtivo].BSWVec().size();
							if(nvecEmul!=fluidos_producao_json[indAtivo].emulVec().size()){
									logger.log(LOGGER_FALHA,
											LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
											"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
											chaveJson, "tipoEmul=6");
							}
							else{
								BSWVec = new double[nvecEmul];
								emulVec = new double[nvecEmul];
								for (int iemul = 0; iemul < nvecEmul; iemul++) {
									BSWVec[iemul] = fluidos_producao_json[indAtivo].BSWVec()[iemul];
									emulVec[iemul] = fluidos_producao_json[indAtivo].emulVec()[iemul];
								}
							}
						}
					}

					flash[i].id =
							fluidos_producao_json[indAtivo].id();
					flash[i].rholF = new double*[tabent.npont + 1];
					flash[i].rhogF = new double*[tabent.npont + 1];
					flash[i].DrholDpF = new double*[tabent.npont + 1];
					flash[i].DrhogDpF = new double*[tabent.npont + 1];
					flash[i].DrholDtF = new double*[tabent.npont + 1];
					flash[i].DrhogDtF = new double*[tabent.npont + 1];
					flash[i].DrhowDtF = new double*[tabent.npont + 1];
					flash[i].RSF = new double*[tabent.npont + 1];
					flash[i].cplF = new double*[tabent.npont + 1];
					flash[i].cpgF = new double*[tabent.npont + 1];
					flash[i].cpwF = new double*[tabent.npont + 1];
					flash[i].HlF = new double*[tabent.npont + 1];
					flash[i].HgF = new double*[tabent.npont + 1];
					flash[i].sigOGF = new double*[tabent.npont + 1];
					flash[i].sigWGF = new double*[tabent.npont + 1];
					flash[i].viscO = new double*[tabent.npont + 1]; //alteracao6
					flash[i].viscG = new double*[tabent.npont + 1]; //alteracao6
					flash[i].PBF = new double[tabent.npontB];
					flash[i].TBF = new double[tabent.npontB];

					for (int k = 0; k < tabent.npont + 1; k++) {
						flash[i].rholF[k] = new double[tabent.npont + 1];
						flash[i].rhogF[k] = new double[tabent.npont + 1];
						flash[i].DrholDpF[k] =
								new double[tabent.npont + 1];
						flash[i].DrhogDpF[k] =
								new double[tabent.npont + 1];
						flash[i].DrholDtF[k] =
								new double[tabent.npont + 1];
						flash[i].DrhogDtF[k] =
								new double[tabent.npont + 1];
						flash[i].DrhowDtF[k] =
								new double[tabent.npont + 1];
						flash[i].RSF[k] = new double[tabent.npont + 1];
						flash[i].cplF[k] = new double[tabent.npont + 1];
						flash[i].cpgF[k] = new double[tabent.npont + 1];
						flash[i].cpwF[k] = new double[tabent.npont + 1];
						flash[i].HlF[k] = new double[tabent.npont + 1];
						flash[i].HgF[k] = new double[tabent.npont + 1];
						flash[i].sigOGF[k] = new double[tabent.npont + 1];
						flash[i].sigWGF[k] = new double[tabent.npont + 1];
						flash[i].viscO[k] = new double[tabent.npont + 1]; //alteracao6
						flash[i].viscG[k] = new double[tabent.npont + 1]; //alteracao6
					}

					for (int k = 0; k < tabent.npont + 1; k++) {
						for (int j = 0; j < tabent.npont + 1; j++) {
							flash[i].rholF[k][j] = 0;
							flash[i].rhogF[k][j] = 0;
							flash[i].DrholDpF[k][j] = 0;
							flash[i].DrhogDpF[k][j] = 0;
							flash[i].DrholDtF[k][j] = 0;
							flash[i].DrhogDtF[k][j] = 0;
							flash[i].DrhowDtF[k][j] = 0;
							flash[i].RSF[k][j] = 0;
							flash[i].cplF[k][j] = 0;
							flash[i].cpgF[k][j] = 0;
							flash[i].cpwF[k][j] = 0;
							flash[i].HlF[k][j] = 0;
							flash[i].HgF[k][j] = 0;
							flash[i].sigOGF[k][j] = 0;
							flash[i].sigWGF[k][j] = 0;
							flash[i].viscO[k][j] = 0;  //alteracao6
							flash[i].viscG[k][j] = 0;  //alteracao6
						}
					}
					for (int k = 0; k < tabent.npontB; k++) {
						flash[i].PBF[k] = 0;
						flash[i].TBF[k] = 0;
					}

					string dadosMR;
					if(pocinjec==0) dadosMR= pvtsimarq;
					else dadosMR=condpocinj.pvtsimarqInj;
					ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
					//string chave;
					//char* tenta;
					//tenta = new char[400];
					//double testatok;
					//char line[4000];
					//lendoPVTSim.get(line, 4000);

					// Lê novamente linha com quebra e monta linha unificada
					linha_completa = ler_linha_continua(lendoPVTSim);
					strcpy(line, linha_completa.c_str());

					tenta = strtok(line, " ,()=");
					while (tenta != nullptr && strcmp(tenta, "PHASE") != 0) {
						tenta = strtok(NULL, " ,()=");
						if(lendoPVTSim.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
					tenta = strtok(NULL, " ,()=");
					int threeOrtwo = 0;
					flash[i].TwoOrThree = 0;
					if (tenta != nullptr && strcmp(tenta, "THREE") == 0) {
						threeOrtwo = 1;
						flash[i].TwoOrThree = 1;
					}

					lendoPVTSim.close();

					lendoPVTSim.open(dadosMR.c_str(), ios_base::in);

					lendoPVTSim >> chave;
					while (chave != "GOR"){
						lendoPVTSim >> chave;
						if(lendoPVTSim.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					rgo = stof(chave);

					/*if (threeOrtwo == 1) {
						while (chave != "WC")
							lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						bsw = stof(chave);
					} else
						bsw = 0.;*/

					while (chave != "STDGASDENSITY"){
						lendoPVTSim >> chave;
						if(lendoPVTSim.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					deng = stof(chave);
					deng /= 1.225;

					while (chave != "STDOILDENSITY"){
						lendoPVTSim >> chave;
						if(lendoPVTSim.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					api = stof(chave);
					api /= 1000.;
					api = 141.5 / api - 131.5;

					/*if (threeOrtwo == 1) {
						while (chave != "STDWATDENSITY")
							lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						denag = stof(chave);
						denag /= 1000.;
						if (denag == 0)
							denag = 1.;
					} else
						denag = 1.;*/

					//double rgoSpare = fluidos_producao_json[indAtivo].rgo();
					//if(rgoSpare>rgo) rgo=rgoSpare;

					flup[i] = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul,PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, identificadores[i]);
					flup[i].indiceFlash = i;
					//flup[i].viscBlackOil = 1 - flash[i].visc;
					flup[i].viscBlackOil = flash[i].visc;
					flup[i].modelaAgua=modelagua;
					flup[i].JTLiquidoSimple=JTLiquidoSimple;

					int ndiv = tabent.npont - 1;
					int ndivB = tabent.npontB - 1;
					flup[i].npontos=tabent.npont;
					flup[i].npontosB=tabent.npontB;
					lendoPVTSim >> chave;
					int lacoleitura = ndiv;
					int lacoleituraB = ndivB;
					double valor;

					while (chave != "BUBBLEPRESSURES") {
						lendoPVTSim >> chave;
						if(lendoPVTSim.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
					//lendoPVTSim.get(line, 4000);
					linha_completa = ler_linha_continua(lendoPVTSim);
					strcpy(line, linha_completa.c_str());
					tenta = strtok(line, " ,()=");
					flash[i].PBF[0] = atof(tenta) * 0.00014503773800722;
					for (int kontaPVT = 1; kontaPVT <= lacoleituraB;
							kontaPVT++) {
						tenta = strtok(NULL, " ,");
						testatok = atof(tenta);
						flash[i].PBF[kontaPVT] = testatok * 0.00014503773800722;
					}
					tenta = strtok(NULL, " ,");
					chave = std::string(tenta);
					while (chave != "BUBBLETEMPERATURES") {
						//lendoPVTSim >> chave;
						tenta = strtok(NULL, " ,");
						chave = std::string(tenta);
						if(lendoPVTSim.eof( )){
							logger.log_write_logs_and_exit(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Arquivo PVTSIM com problemas", chaveJson,
									"pvtsimArq");
						}
					}
					//lendoPVTSim.get(line, 4000);
					//linha_completa = ler_linha_continua(lendoPVTSim);
					//strcpy(line, linha_completa.c_str());
					tenta = strtok(NULL, " ,()=");
					flash[i].TBF[0] = atof(tenta);
					for (int kontaPVT = 1; kontaPVT <= lacoleituraB;
							kontaPVT++) {
						tenta = strtok(NULL, " ,");
						testatok = atof(tenta);
						flash[i].TBF[kontaPVT] = testatok;
					}

					FullMtx<double> BolhaTemp(ndivB + 2, 2);
					for (int k = 0; k <= ndivB; k++) {
						BolhaTemp[k][0] = flash[i].PBF[k];
						BolhaTemp[k][1] = flash[i].TBF[k];
					}
					ostringstream saidaBolha;
					saidaBolha << pathPrefixoArqSaida << "perfilBolha.dat" << i;
					string tmp = saidaBolha.str();
					ofstream escreveMass(tmp.c_str(), ios_base::out);
					escreveMass << BolhaTemp;
					escreveMass.close();
					lendoPVTSim.close();

					geraTabFlash(i, 1);
					geraTabFlash(i, 2);
					geraTabFlash(i, 3);
					geraTabFlash(i, 4);
					geraTabFlash(i, 5);
					geraTabFlash(i, 6);
					geraTabFlash(i, 8);
					geraTabFlash(i, 9);
					geraTabFlash(i, 10);
					geraTabFlash(i, 12);
					geraTabFlash(i, 13);
					geraTabFlash(i, 14);
					geraTabFlash(i, 16);  //alteracao6
					geraTabFlash(i, 17);  //alteracao6
					if (threeOrtwo == 1) {
						geraTabFlash(i, 7);
						geraTabFlash(i, 11);
						geraTabFlash(i, 15);
					}
					flup[i].rhogF = flash[i].rhogF;

					int erroP=0;
					int erroT=0;
					vector<double> dP;
					vector<double> pchange;
					vector<int> indp;
					int ndp=0;
					vector<double> dT;
					vector<double> tchange;
					vector<int> indt;
					int ndt=0;
					double dp0=0.;
					double dp1;
					double dt0=0.;
					double dt1;

					int imin=1;
					while(flup[i].rhogF[imin][0]<=1.0332274497825)imin++;
					flup[i].indMinEquP=imin+1;
					int iant=imin+1;
					flup[i].pMinEqu=flup[i].rhogF[imin+1][0];
					flup[i].delpTab=flup[i].rhogF[imin+2][0]-flup[i].rhogF[imin+1][0];
					dp0=(flup[i].rhogF[ndiv+1][0]-flup[i].rhogF[imin+1][0])/(ndiv-imin);
					for(int j=imin+1;j<ndiv;j++){
						dp0=flup[i].rhogF[j+1][0]-flup[i].rhogF[j][0];
						dp1=flup[i].rhogF[j+2][0]-flup[i].rhogF[j+1][0];
						if(fabsl((dp0-dp1)/dp0)>1e-3){
							ndp++;
							pchange.push_back(flup[i].rhogF[j+1][0]);
							indp.push_back(j+1);
							dP.push_back((flup[i].rhogF[j+1][0]-flup[i].rhogF[iant][0])/(j+1-iant));
							iant=j+1;
							if(ndp>3)erroP=1;
						}
						else if(j==ndiv-1 && ndp>0){
							ndp++;
							pchange.push_back(flup[i].rhogF[ndiv+1][0]);
							indp.push_back(ndiv+1);
							dP.push_back((flup[i].rhogF[ndiv+1][0]-flup[i].rhogF[iant][0])/(ndiv+1-iant));
							if(ndp>3)erroP=1;

						}
					}
                    if(ndp==0){
                    	ndp++;
                    	pchange.push_back(flup[i].rhogF[ndiv+1][0]);
                    	indp.push_back(ndiv+1);
                    	dP.push_back((flup[i].rhogF[ndiv+1][0]-flup[i].rhogF[imin+1][0])/(ndiv-imin));
                    }
                    if(erroP==0){
                      flup[i].pchange=pchange;
                      flup[i].dpchange=dP;
                      flup[i].ipchange=indp;
                    }
					flup[i].delpTab=ndp;
					flup[i].erroP=erroP;

					imin=1;
					while(flup[i].rhogF[0][imin]<=16)imin++;
					flup[i].indMinEquT=imin+1;
					iant=imin+1;
					flup[i].tMinEqu=flup[i].rhogF[0][imin+1];
					flup[i].deltTab=flup[i].rhogF[0][imin+2]-flup[i].rhogF[0][imin+1];
					dt0=(flup[i].rhogF[0][ndiv+1]-flup[i].rhogF[0][imin+1])/(ndiv-imin);
					for(int j=imin+1;j<ndiv;j++){
						dt0=flup[i].rhogF[0][j+1]-flup[i].rhogF[0][j];
						dt1=flup[i].rhogF[0][j+2]-flup[i].rhogF[0][j+1];
						if(fabsl((dt0-dt1)/dt0)>1e-3){
							ndt++;
							tchange.push_back(flup[i].rhogF[0][j+1]);
							indt.push_back(j+1);
							dT.push_back((flup[i].rhogF[0][j+1]-flup[i].rhogF[0][iant])/(j+1-iant));
							iant=j+1;
							if(ndt>3)erroT=1;
						}
						else if(j==ndiv-1 && ndt>0){
							ndt++;
							tchange.push_back(flup[i].rhogF[0][ndiv+1]);
							indt.push_back(ndiv+1);
							dT.push_back((flup[i].rhogF[0][ndiv+1]-flup[i].rhogF[0][iant])/(ndiv+1-iant));
							if(ndt>3)erroT=1;

						}
					}
                    if(ndt==0){
                    	ndt++;
                    	tchange.push_back(flup[i].rhogF[0][ndiv+1]);
                    	indt.push_back(ndiv+1);
                    	dT.push_back((flup[i].rhogF[0][ndiv+1]-flup[i].rhogF[0][imin+1])/(ndiv-imin));
                    }
                    if(erroT==0){
                      flup[i].tchange=tchange;
                      flup[i].dtchange=dT;
                      flup[i].itchange=indt;
                    }
					flup[i].deltTab=ndt;
					flup[i].erroT=erroT;

					flup[i].rholF = flash[i].rholF;
					flup[i].DrhogDpF = flash[i].DrhogDpF;
					flup[i].DrholDpF = flash[i].DrholDpF;
					flup[i].DrhogDtF = flash[i].DrhogDtF;
					flup[i].DrholDtF = flash[i].DrholDtF;
					flup[i].DrhowDtF = flash[i].DrhowDtF;
                    flup[i].RSF = flash[i].RSF;
					flup[i].cpgF = flash[i].cpgF;
					flup[i].cplF = flash[i].cplF;
					flup[i].cpwF = flash[i].cpwF;
					flup[i].HgF = flash[i].HgF;
					flup[i].HlF = flash[i].HlF;
					flup[i].sigOGF = flash[i].sigOGF;
					flup[i].sigWGF = flash[i].sigWGF;
					flup[i].viscO = flash[i].viscO;
					flup[i].viscG = flash[i].viscG;
					flup[i].PBPVTSim = flash[i].PBF;
					flup[i].TBPVTSim = flash[i].TBF;

					/*for(int j=1;j>ndiv;j++){
						for(int k=2;k<ndiv+2;k++){
							if(flup[i].RSF[j][k]<0.){
								//flup[i].rholF[j][k]=1000*141.5/(131.5+api);
								flup[i].rholF[j][k]=flup[i].rholF[j][k-1];
								flup[i].DrholDtF[j][k]=0.;
								flup[i].DrholDpF[j][k]=0.;
							}
						}
					}*/

					flup[i].nvecEmul=nvecEmul;
					flup[i].BSWVec=BSWVec;
					flup[i].emulVec=emulVec;
					flup[i].parserie=tabVisc[i].parserie;
					flup[i].viscTab=tabVisc[i].visc;
					flup[i].tempTab=tabVisc[i].temp;

					rgo = fluidos_producao_json[indAtivo].rgo();
					if(rgo>flup[i].RGO){
						flup[i].RGO=rgo;
						flup[i].IRGO=flup[i].RGO*35.31467/6.29;
					}

					if(flash[i].rhogF[0][1]<20 && flash[i].rhogF[1][0]<1.01)
						//flup[i].dStockTankVaporDensity=flup[i].FracMass(1.01, 20.);
						flup[i].dStockTankVaporMassFraction=flup[i].FracMass(1.01, 20.);
					else
						//flup[i].dStockTankVaporDensity=0.5;
						flup[i].dStockTankVaporMassFraction=0.5;

				}

				//delete [] tenta;
			}
			else{

				/*api = fluidos_producao_json[indAtivo].api();
				rgo = fluidos_producao_json[indAtivo].rgo();
				deng =
						fluidos_producao_json[indAtivo].densidadeGas();
				bsw = fluidos_producao_json[indAtivo].bsw();
				if(bsw>1){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"BSW > 1", chaveJson,
							"BSW>1");
				}
				denag = 1;
				if (fluidos_producao_json[indAtivo].densidadeAgua().exists())
					denag =
							fluidos_producao_json[indAtivo].densidadeAgua();
				if(denag<1.0)denag=1.0;*/

				string dadosMRPre;
				if(pocinjec==1)dadosMRPre =condpocinj.pvtsimarqInj;
				else dadosMRPre = pvtsimarq;
				compDet = new composicional[nfluP];
				npseudo=0;
				int erro=0;
                GetMixtureComponentNumberFromExternalFile(dadosMRPre.c_str(),dadosMRPre.size(),0,
                		    &npseudo,&erro);
                if(erro!=0){
                	logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Leitura CTM", chaveJson, "Erro na leitura do CTM");
                }
                double fractemp[npseudo];
                for(int i=0; i<nfluP;i++){
                	// enquanto a propriedade "ativo" do fluido de producao esteja desabilitada, avança
                	while (!is_ativo(fluidos_producao_json[++indAtivo]));
 					// obter maior identificador
                	indFlup[i]=identificadores[i] =
							fluidos_producao_json[indAtivo].id();
					// caso o identificador seja maior que o último selecionado, substitui
					if (identificadores[i] > maiorIdentificador) {
						maiorIdentificador = identificadores[i];
					}

                	compDet[i].fracMol = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].fracMol[inpseudo]=0.;
                	compDet[i].TIndepPeneloux = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].TIndepPeneloux[inpseudo]=0.;
                	compDet[i].fatAcent = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].fatAcent[inpseudo]=0.;
                	compDet[i].masMol = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].masMol[inpseudo]=0.;
                	compDet[i].parac = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].parac[inpseudo]=0.;
                	compDet[i].presCrit = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].presCrit[inpseudo]=0.;
                	compDet[i].tempCrit = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].tempCrit[inpseudo]=0.;
                	compDet[i].CpIGCoefs = new double [4*npseudo];
                	for(int inpseudo=0;inpseudo<4*npseudo;inpseudo++)compDet[i].CpIGCoefs[inpseudo]=0.;
                	compDet[i].lij = new double [npseudo*npseudo];
                	for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet[i].lij[inpseudo]=0.;
                	compDet[i].kij = new double [npseudo*npseudo];
                	for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet[i].kij[inpseudo]=0.;

                	/*compDet.fracMol = new double [npseudo];
                	compDet.TIndepPeneloux = new double [npseudo];
                	compDet.fatAcent = new double [npseudo];
                	compDet.masMol = new double [npseudo];
                	compDet.parac = new double [npseudo];
                	compDet.presCrit = new double [npseudo];
                	compDet.tempCrit = new double [npseudo];
                	compDet.CpIGCoefs = new double [4*npseudo];
                	compDet.lij = new double [npseudo*npseudo];
                	compDet.kij = new double [npseudo*npseudo];*/

                	InputForViscosity modeloVisc;

                	if(i==0){
                		ReadMixtureComponentsFromExternalFile(dadosMRPre.c_str(), dadosMRPre.size(), 0,
                				&npseudo, &fractemp[0], compDet[i].masMol,
								compDet[i].tempCrit, compDet[i].presCrit, compDet[i].fatAcent,
								compDet[i].parac,compDet[i].kij,compDet[i].lij,compDet[i].TIndepPeneloux,
								&compDet[i].liqModel, compDet[i].CpIGCoefs,&modeloVisc, &erro);


                	}
                	else{
                		memcpy(compDet[i].TIndepPeneloux,compDet[0].TIndepPeneloux,npseudo*sizeof(double));
                		memcpy(compDet[i].fatAcent,compDet[0].fatAcent,npseudo*sizeof(double));
                		memcpy(compDet[i].masMol,compDet[0].masMol,npseudo*sizeof(double));
                		memcpy(compDet[i].parac,compDet[0].parac,npseudo*sizeof(double));
                		memcpy(compDet[i].presCrit,compDet[0].presCrit,npseudo*sizeof(double));
                		memcpy(compDet[i].tempCrit,compDet[0].tempCrit,npseudo*sizeof(double));
                		memcpy(compDet[i].CpIGCoefs,compDet[0].CpIGCoefs,4*npseudo*sizeof(double));
                		memcpy(compDet[i].lij,compDet[0].lij,npseudo*npseudo*sizeof(double));
                		memcpy(compDet[i].kij,compDet[0].kij,npseudo*npseudo*sizeof(double));

                	}
            		/*ReadMixtureComponentsFromExternalFile(dadosMRPre.c_str(), dadosMRPre.size(), 0,
            				&npseudo, &fractemp[0], compDet.masMol,
							compDet.tempCrit, compDet.presCrit, compDet.fatAcent,
							compDet.parac,compDet.kij,compDet.lij,compDet.TIndepPeneloux,
							&compDet.liqModel, compDet.CpIGCoefs,&modeloVisc, &erro);*/
              		int fracusu=0;
    				if (fluidos_producao_json[indAtivo].fracMolarUsuario().exists())
    						fracusu =fluidos_producao_json[indAtivo].fracMolarUsuario();
    				if(fracusu==1){
    					int nusu=fluidos_producao_json[indAtivo].fracMolar().size();
    					if(nusu!=npseudo){
    						logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
    								"Dimensao da composicao diferente da dimensao da composicao dada no CTM",
									chaveJson, "Erro no preenchimento da Fracao Molar");
    					}
    					double fracTot=0.;
    					for(int j=0;j<npseudo;j++){
    							compDet[i].fracMol[j]=fluidos_producao_json[indAtivo].fracMolar()[j]/100.;
    							fracTot+=compDet[i].fracMol[j];
    					}
    					for(int j=0;j<npseudo;j++)compDet[i].fracMol[j]/=fracTot;
    					/*for(int j=0;j<npseudo;j++)
    					    	compDet.fracMol[j]=fluidos_producao_json[indAtivo].fracMolar()[j];*/

    			    }
    				else /*memcpy(compDet.fracMol,&fractemp[0],npseudo*sizeof(double));*/
    					memcpy(compDet[i].fracMol,&fractemp[0],npseudo*sizeof(double));
    				double totFrac=0.;
    				for(int j=0;j<npseudo;j++)totFrac+=compDet[i].fracMol[j];
    				if(totFrac>1e-15)for(int j=0;j<npseudo;j++)compDet[i].fracMol[j]/=totFrac;
    				bsw = fluidos_producao_json[indAtivo].bsw();
    				if(bsw>1){
    							logger.log(LOGGER_FALHA,
    							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
    							"BSW > 1", chaveJson,
    							"BSW>1");
    					   	}
    				denag = 1;
    				if (fluidos_producao_json[indAtivo].densidadeAgua().exists())
    							denag =	fluidos_producao_json[indAtivo].densidadeAgua();
    				if(denag<1.0)denag=1.0;

					yco2 =
							fluidos_producao_json[indAtivo].fracCO2();
					corrC =
							fluidos_producao_json[indAtivo].correlacaoCritica();


					int modelovisc = 1;  // modelo viscosidade tabela flash
					//!!!!!!!!!!!!!!!!!!!!!!!!!!!momentaneamente, sera desconsiderada esta opção para composicional!!!!!!!!!!!!
					//if (fluidos_producao_json[indAtivo].modeloViscBlackOil().exists())  //alteracao6
						//modelovisc =
								//fluidos_producao_json[indAtivo].modeloViscBlackOil(); //alteracao6
					if (modelovisc == 1) {  //modelo viscosidade black oil
						vcorrOM = 0;
						// caso modelo oleo morto preenchido
						if (fluidos_producao_json[indAtivo].modeloOleoMorto().exists()) {
							//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
							vcorrOM =
									fluidos_producao_json[indAtivo].modeloOleoMorto();
						}
						if (vcorrOM == 0) {
							if (!fluidos_producao_json[indAtivo].temp1().exists()
									|| !fluidos_producao_json[indAtivo].visc1().exists()
									|| !fluidos_producao_json[indAtivo].temp2().exists()
									|| !fluidos_producao_json[indAtivo].visc2().exists()) {
								// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
										chaveJson,
										"modeloOleoMorto = 0 ou ausente");
							} else {
								templ =
										fluidos_producao_json[indAtivo].temp1();
								lvisl =
										fluidos_producao_json[indAtivo].visc1();
								temph =
										fluidos_producao_json[indAtivo].temp2();
								lvish =
										fluidos_producao_json[indAtivo].visc2();
							}
						}
						else if (vcorrOM == 7) {
							if (!fluidos_producao_json[indAtivo].tempOleoMorto().exists()
									|| !fluidos_producao_json[indAtivo].viscOleoMorto().exists()) {
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'tempOleoMorto', 'viscOleoMorto' requeridas",
										chaveJson,
										"modeloOleoMorto = 7 ou ausente");
							} else {
								tabVisc[i].parserie=fluidos_producao_json[indAtivo].tempOleoMorto().size();
								tabVisc[i].temp=new double[tabVisc[i].parserie];
								tabVisc[i].visc=new double[tabVisc[i].parserie];
								for(int itab=0;itab<tabVisc[i].parserie;itab++){
									tabVisc[i].temp[itab]=fluidos_producao_json[indAtivo].tempOleoMorto()[itab];
									tabVisc[i].visc[itab]=fluidos_producao_json[indAtivo].viscOleoMorto()[itab];
								}
							}
						}

						if (fluidos_producao_json[indAtivo].modeloOleoVivo().exists())
							//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
							vcorrOV =
									fluidos_producao_json[indAtivo].modeloOleoVivo();
						if (fluidos_producao_json[indAtivo].modeloOleoSubSaturado().exists())
							//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
							vcorrOS =
									fluidos_producao_json[indAtivo].modeloOleoSubSaturado();
					} else {
						vcorrOM = 4;
						vcorrOV = 0;
						vcorrOS = 0;
					}

					tipoemul = 0;
					if (fluidos_producao_json[indAtivo].tipoEmul().exists())
						tipoemul =
								fluidos_producao_json[indAtivo].tipoEmul();
					bswCorte = 1.;
					if (fluidos_producao_json[indAtivo].bswCorte().exists())
						bswCorte =
								fluidos_producao_json[indAtivo].bswCorte();

					if (tipoemul == 4) {
						if (!fluidos_producao_json[indAtivo].coefAModeloExp().exists()
								|| !fluidos_producao_json[indAtivo].coefBModeloExp().exists()) {
							// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
									chaveJson, "tipoEmul=4");
						} else {
							aemul =
									fluidos_producao_json[indAtivo].coefAModeloExp();
							bemul =
									fluidos_producao_json[indAtivo].coefBModeloExp();
						}
					}
					else if(tipoemul == 5 && fluidos_producao_json[indAtivo].PHI100().exists())
						PHI100=fluidos_producao_json[indAtivo].PHI100();
					else if(tipoemul == 6 && nvecBSW==0){
						nvecBSW=1;
						if(!fluidos_producao_json[indAtivo].BSWVec().exists() ||
						   !fluidos_producao_json[indAtivo].emulVec().exists()){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'BSWVec', 'emulVec' requeridas",
									chaveJson, "tipoEmul=6");
						}
						else{
							nvecEmul =fluidos_producao_json[indAtivo].BSWVec().size();
							if(nvecEmul!=fluidos_producao_json[indAtivo].emulVec().size()){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
										chaveJson, "tipoEmul=6");
							}
							else{
								BSWVec = new double[nvecEmul];
								emulVec = new double[nvecEmul];
								for (int iemul = 0; iemul < nvecEmul; iemul++) {
									BSWVec[iemul] = fluidos_producao_json[indAtivo].BSWVec()[iemul];
									emulVec[iemul] = fluidos_producao_json[indAtivo].emulVec()[iemul];
								}
							}
						}
					}


					/*vcorrOM = 0;
					// caso modelo oleo morto preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoMorto().exists()) {
						//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
						vcorrOM =
								fluidos_producao_json[indAtivo].modeloOleoMorto();
					}
					if (vcorrOM == 0) {
						if (!fluidos_producao_json[indAtivo].temp1().exists()
								|| !fluidos_producao_json[indAtivo].visc1().exists()
								|| !fluidos_producao_json[indAtivo].temp2().exists()
								|| !fluidos_producao_json[indAtivo].visc2().exists()) {
							// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
									chaveJson,
									"modeloOleoMorto = 0 ou ausente");
						} else {
							templ =
									fluidos_producao_json[indAtivo].temp1();
							lvisl =
									fluidos_producao_json[indAtivo].visc1();
							temph =
									fluidos_producao_json[indAtivo].temp2();
							lvish =
									fluidos_producao_json[indAtivo].visc2();
						}
					}

					// caso modelo oleo vivo preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoVivo().exists())
						//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
						vcorrOV =
								fluidos_producao_json[indAtivo].modeloOleoVivo();

					// caso modelo oleo subsaturado preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoSubSaturado().exists())
						//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
						vcorrOS =
								fluidos_producao_json[indAtivo].modeloOleoSubSaturado();*/
					if(pocinjec==1){
						rgo=1e6;
						deng =fluidos_producao_json[indAtivo].densidadeGas();
						api=40.;
					}

					flup[i] = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul, PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, identificadores[i],npseudo);


					flup[i].viscBlackOil = modelovisc;
					flup[i].modelaAgua=1;
					flup[i].JTLiquidoSimple=JTLiquidoSimple;
					//if(pocinjec==0)tabp=0;
					flup[i].tabelaDinamica=tabelaDinamica;
					if(tabelaDinamica==1)flup[i].flashCompleto=0;

					flup[i].TIndepPeneloux=compDet[i].TIndepPeneloux;
					flup[i].fatAcent=compDet[i].fatAcent;
					flup[i].masMol=compDet[i].masMol;
					flup[i].parac=compDet[i].parac;
					flup[i].presCrit=compDet[i].presCrit;
					flup[i].tempCrit=compDet[i].tempCrit;
					flup[i].CpIGCoefs=compDet[i].CpIGCoefs;
					flup[i].lij=compDet[i].lij;
					flup[i].kij=compDet[i].kij;
					flup[i].liqModel=compDet[i].liqModel;
					//memcpy(flup[i].fracMol,compDet[i].fracMol, npseudo*sizeof(double));
					for(int j=0;j<flup[i].npseudo;j++)flup[i].fracMol[j]=compDet[i].fracMol[j];
					flup[i].Pmol=0.;
					for(int j=0;j<flup[i].npseudo;j++)flup[i].Pmol+=flup[i].fracMol[j]*flup[i].masMol[j];
					flup[i].atualizaPropCompStandard();


					if (fluidos_producao_json[indAtivo].RGOCompUsuario().exists()) {

						int testeRGO=fluidos_producao_json[indAtivo].RGOCompUsuario();
						if(testeRGO==1){

							if (!fluidos_producao_json[indAtivo].rgo().exists()) {
										logger.log(LOGGER_FALHA,
										LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
																"Chave 'RGO' requerida",chaveJson, "rgo");
							}
							else rgo = fluidos_producao_json[indAtivo].rgo();

							double* GivenInitialLiqComposition = new double [npseudo];
					        for(int k=0;k<npseudo;k++)GivenInitialLiqComposition[k]=0;

				    		double* GivenInitialVapComposition = new double [npseudo];
				    		for(int k=0;k<npseudo;k++)GivenInitialVapComposition[k]=0;

				    		double* oGORAdjustedGlobalComp = new double [npseudo];
				    		double dAdjustedGlobalCompGOR;
				    		int iier;


							Marlim_AdjustGlobalCompositionForTargetGOR(flup[i].npseudo, flup[i].fracMol,
									flup[i].masMol,flup[i].tempCrit, flup[i].presCrit,
									flup[i].fatAcent, flup[i].TIndepPeneloux, flup[i].kij,
									flup[i].lij, flup[i].liqModel, flup[i].liqModel,
									flup[i].liqModel,
									flup[i].liqModel, 1.03322745279996 ,
									20., rgo, -1.0,
									GivenInitialLiqComposition, GivenInitialVapComposition,
									oGORAdjustedGlobalComp, &dAdjustedGlobalCompGOR,
							        &iier);
							 for(int k=0;k<npseudo;k++)flup[i].fracMol[k]=oGORAdjustedGlobalComp[k];
							 flup[i].atualizaPropCompStandard();
					         delete GivenInitialLiqComposition;
					         delete GivenInitialVapComposition;
					         delete oGORAdjustedGlobalComp;
						}
					}


					flup[i].parserie=tabVisc[i].parserie;
					flup[i].viscTab=tabVisc[i].visc;
					flup[i].tempTab=tabVisc[i].temp;

					flup[i].nvecEmul=nvecEmul;
					flup[i].BSWVec=BSWVec;
					flup[i].emulVec=emulVec;
					if(tabp==1){
						flup[i].zdranP = zdranP;  //alteracao2
						flup[i].dzdpP = dzdpP;  //alteracao2
						flup[i].dzdtP = dzdtP;  //alteracao2
						flup[i].npontos = npontos;
					}

					//flup[i].atualizaPropCompStandard();

					//flup[i].atualizaPropComp(90,10);
					/*FullMtx<double> fatores(3,flup[i].npseudo);
					for(int ipseu=0;ipseu<flup[i].npseudo;ipseu++){
						fatores[0][ipseu]=flup[i].fatAcent[ipseu];
						fatores[1][ipseu]=flup[i].masMol[ipseu];
						fatores[2][ipseu]=flup[i].masMol[ipseu];
					}
					cout<<fatores;
					cout<<dadosMRPre<<" "<<flup[i].iIER<<"   "<<flup[i].dStockTankVaporDensity<<"  "<<flup[i].dStockTankVaporMassFraction<<"\n";*/
					//flup[i].atualizaPropComp(50,50);
					/*flup[i].TIndepPeneloux=compDet.TIndepPeneloux;
					flup[i].fatAcent=compDet.fatAcent;
					flup[i].masMol=compDet.masMol;
					flup[i].parac=compDet.parac;
					flup[i].presCrit=compDet.presCrit;
					flup[i].tempCrit=compDet.tempCrit;
					flup[i].CpIGCoefs=compDet.CpIGCoefs;
					flup[i].lij=compDet.lij;
					flup[i].kij=compDet.kij;
					flup[i].liqModel=compDet.liqModel;
					memcpy(flup[i].fracMol,compDet.fracMol, npseudo*sizeof(double));*/
					//flup[i].atualizaPropCompStandard();
					//flup[i].atualizaPropComp(50,50);
					//double teste=flup[i].DZDT(50, 50);
					//teste=flup[i].BOFunc(50, 50);

                }
			}
			if(modoParafina==1){
				string dadosWax;
				dadosWax =detalParafina.arquivo;

				int erro=0;

	            GetWaxParameterExternalFileDimensions(dadosWax.c_str(), dadosWax.size(),0,
	            		&erro, &npseudoWax, &iPresWax, &iTempWax);

	            if(erro!=0){
	                logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Leitura CTM", chaveJson, "Erro na leitura do arquivo Wax");
	             }

	    		oPressurePoints=new double [iPresWax];

	    		oCloudPointTemperatures=new double [iPresWax];

	    		oStructurePressures=new double [iPresWax];

	    		oStructureTemperatures=new double [iPresWax*iTempWax];
				//for(int i=0;i<iPresWax;i++)oStructureTemperatures[i]=new double [iTempWax];

	    		oStructureDensities=new double [iPresWax*iTempWax];
				//for(int i=0;i<iPresWax;i++)oStructureDensities[i]=new double [iTempWax];

	    		oStructureLiqMW=new double [iPresWax*iTempWax];
				//for(int i=0;i<iPresWax;i++)oStructureLiqMW[i]=new double [iTempWax];

	    		oStructureCPWax=new double [iPresWax*iTempWax];
				//for(int i=0;i<iPresWax;i++)oStructureCPWax[i]=new double [iTempWax];

	    		oStructureThermCond=new double [iPresWax*iTempWax];
				//for(int i=0;i<iPresWax;i++)oStructureThermCond[i]=new double [iTempWax];

	    		oStructureWaxConcs=new double [iPresWax*iTempWax*npseudoWax];
	    		/*for(int i=0;i<iPresWax;i++){
	    			oStructureWaxConcs[i]=new double* [iTempWax];
	    			for(int j=0;j<iTempWax;j++)oStructureWaxConcs[i][j]=new double [npseudoWax];
	    		}*/

	    		oMolecularWeightsOfWaxComponents=new double[npseudoWax];

	    		oLiquidDensitiesOfWaxComponents=new double[npseudoWax];

	    		oInterpolatedWaxConcs=new double[npseudoWax];

	    		ReadWaxCalculationParametersFromExternalFile(dadosWax.c_str(), dadosWax.size(), 0,
	    				npseudoWax, iPresWax, iTempWax, &erro, oPressurePoints,
	    				oCloudPointTemperatures, oStructurePressures, oStructureTemperatures, oStructureDensities,
	    				oStructureLiqMW, oStructureCPWax, oStructureThermCond, oStructureWaxConcs,
	    				oMolecularWeightsOfWaxComponents, oLiquidDensitiesOfWaxComponents,&detalParafina.DViscWax,&detalParafina.EViscWax,
						&detalParafina.FViscWax);

	    		for(int i=0;i<nfluP;i++){
	    			flup[i].modoParafina=modoParafina;
	    			flup[i].iPresWax=iPresWax;
	    			flup[i].iTempWax=iTempWax;
	    			flup[i].npseudoWax=npseudoWax;
	    			flup[i].oPressurePoints=oPressurePoints;
	    			flup[i].oCloudPointTemperatures=oCloudPointTemperatures;
	    			flup[i].oStructurePressures=oStructurePressures;
	    			flup[i].oStructureTemperatures=oStructureTemperatures;
	    			flup[i].oStructureDensities=oStructureDensities;
	    			flup[i].oStructureLiqMW=oStructureLiqMW;
	    			flup[i].oStructureCPWax=oStructureCPWax;
	    			flup[i].oStructureThermCond=oStructureThermCond;
	    			flup[i].oStructureWaxConcs=oStructureWaxConcs;
	    			flup[i].oMolecularWeightsOfWaxComponents=oMolecularWeightsOfWaxComponents;
	    			flup[i].oLiquidDensitiesOfWaxComponents=oLiquidDensitiesOfWaxComponents;

	    			flup[i].npseudoWaxOut=flup[i].npseudoWax;
	    			//flup[i].oMolecularWeightsOfWaxComponentsOut=new double[npseudoWax];
	    			//flup[i].oInterpolatedWaxConcsTDerivOutput=new double[npseudoWax];

	    			flup[i].oMolecularWeightsOfWaxComponentsOut=new double[npseudoWax];
	    			flup[i].oInterpolatedWaxConcsTDerivOutput=new double[npseudoWax];
	    			flup[i].oLiquidDensitiesOfWaxComponentsOutput=new double[npseudoWax];
	    			flup[i].oInterpolatedWaxConcs=new double[npseudoWax];
	    			for(int j=0; j<npseudoWax;j++){
	    				flup[i].oMolecularWeightsOfWaxComponentsOut[j]=flup[i].oMolecularWeightsOfWaxComponents[j];
	    				flup[i].oInterpolatedWaxConcsTDerivOutput[j]=0.;
	    				flup[i].oInterpolatedWaxConcs[j]=oInterpolatedWaxConcs[j];
	    			}
	    		}



			}

			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-009: Unicidade da chave 'id' em '#/fluidosProducao'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}
/*!
 * Converter o elemento "fluidoComplementar" do arquivo Json do MRT em struct fluc.
 *
 * \tparam Objeto JSON correspondente à fluidos producao
 */
void Ler::parse_fluido_complementar(
		JSON_entrada_fluidoComplementar& fluido_complementar_json) {
	// caso a propriedade "ativo" esteja habilitada
	if (is_ativo(fluido_complementar_json)) {
		// de-para do elemento "fluido_complementar" do json para o struct fluc
		int tipoflui=0;
		if (fluido_complementar_json.tipoF().exists())
			tipoflui=fluido_complementar_json.tipoF();
		double masesp=0.;
		double compP = 0.;
		double compT = 0.;
		double tensup = 0.;
		double calesp = 0.;
		double condut = 0.;
		double templ = 0.;
		double lvisl = 0.;
		double temph = 0.;
		double lvish = 0.;
		double sal = 0.;
		if(tipoflui!=1){
			masesp =
				fluido_complementar_json.massaEspecifica();
			compP = fluido_complementar_json.compP();
			compT = fluido_complementar_json.compT();
			tensup = fluido_complementar_json.tensup();
			calesp =
				fluido_complementar_json.calorEspecifico();
			condut =
				fluido_complementar_json.condutividade();
			templ = fluido_complementar_json.temp1();
			lvisl = fluido_complementar_json.visc1();
			temph = fluido_complementar_json.temp2();
			lvish = fluido_complementar_json.visc2();
		}
		if(tipoflui==1 && fluido_complementar_json.salinidade().exists())sal = fluido_complementar_json.salinidade();

		fluc = ProFluCol(masesp, compP, compT, tensup, calesp,
				condut, sal, templ, lvisl, temph, lvish,0, tipoflui);
		fluc.npontos = npontos;
	}
}

/*!
 * Converter o elemento "fluidoGas" do arquivo Json do MRT em struct flug.
 *
 * \tparam Objeto JSON correspondente à fluidos producao
 */
void Ler::parse_fluido_gas(JSON_entrada_fluidoGas& fluido_gas_json) {
	double api = 60.0;
	double rgo = 10000000000000.0;
	double bsw = 0.0;
	double denag = 1.0;
	double templ = 10.0;
	double lvisl = 10;
	double temph = 50.0;
	double lvish = 50;
	int tipoemul = 0;
	double bswCorte = 0.0;
	double aemul = 0.;
	double bemul = 0.;
	double PHI100=0.;

	string chaveJson("#/fluidosGas");

	// caso a propriedade "ativo" esteja habilitada
	if (is_ativo(fluido_gas_json)) {
		if(flashCompleto!=2){
			// de-para do elemento "fluido_gas" do json para o struct flug
			double deng = fluido_gas_json.densidadeGas();
			double yco2 = 0.;
			if (fluido_gas_json.fracCO2().exists())yco2 = fluido_gas_json.fracCO2();
			int corrC = fluido_gas_json.correlacaoCritica();

			flug = ProFlu(vg1dSP, api, rgo, deng, bsw, denag, templ, lvisl,
				temph, lvish, tipoemul, aemul, bemul,PHI100, bswCorte, tabg, yco2,
				corrC);
			flug.zdranP = zdranP;  //15-06-2018
			flug.dzdpP = dzdpP;  //15-06-2018
			flug.dzdtP = dzdtP;  //15-06-2018
			flug.npontos = npontos;
		}
		else{
			tabg=0;
			flug.tab=tabg;
			flug = flup[0];
			if(lingas==1){
				tabg=1;
				flug.tab=tabg;
				flug.Deng = fluido_gas_json.densidadeGas();
				flug.yco2 = fluido_gas_json.fracCO2();
				flug.corrC = fluido_gas_json.correlacaoCritica();
				flug.RenovaFluido();
				flug.zdranP = zdranP;  //15-06-2018
				flug.dzdpP = dzdpP;  //15-06-2018
				flug.dzdtP = dzdtP;  //15-06-2018
				flug.npontos = npontos;
			}
			compLinServ = new double[flug.npseudo];
			for(int j=0;j<flug.npseudo;j++)flug.fracMol[j]=0.;

			/*int fracusu=0;
			if (fluido_gas_json.fracMolar().exists())fracusu=1;*/

     		int fracusu=0;
			if (fluido_gas_json.fracMolarUsuario().exists())
					fracusu =fluido_gas_json.fracMolarUsuario();

			if(fracusu==1){
				int nusu=fluido_gas_json.fracMolar().size();
				if(nusu!=flug.npseudo){
					logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Dimensao da composicao diferente da dimensao da composicao dada no CTM",
							chaveJson, "Erro no preenchimento da Fracao Molar");
				}
				double totFrac=0.;
				for(int j=0;j<flug.npseudo;j++){
					flug.fracMol[j]=fluido_gas_json.fracMolar()[j];
					totFrac+=flug.fracMol[j];
				}
				for(int j=0;j<flug.npseudo;j++)flug.fracMol[j]/=totFrac;

			}
			else{
				//flug.fracMol[0]=1.;
				//for(int j=1;j<flug.npseudo;j++)flug.fracMol[j]=0.;
				for(int j=1;j<flug.npseudo;j++)flug.fracMol[j]=flup[0].fracMol[j];
			}
			flug.atualizaPropCompStandard();
			//flug.atualizaPropComp(50,50,-1, NULL,NULL,pocinjec);

		}
	}
}

/*!
 * Converter o elemento "material" do arquivo Json do MRT em struct material.
 *
 * \tparam Objeto JSON correspondente ao material
 */
void Ler::parse_materiais(JSON_entrada_material& material_json) {
	/*
	 "material": [ { "id": 0, "ativo": true, "rotulo": "aco1", "condutividade": 55.0, "calorEspecifico": 465.0, "rho": 7833.0, "tipo": 0, "visc": 0.0 } ]
	 */
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/material");

	try {
		// obter o tamanho do elemento do Json
		nmaterial = 0;
		// percorre o array de materiais
		for (size_t i = 0; i < material_json.size(); i++) {
			// caso o material esteja ativo
			if (is_ativo(material_json[i]))
				nmaterial++;
		}

		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		// caso o tamanho do elemento seja maior que zero
		if (nmaterial > 0) {
			// vetor de materiais
			mat = new material[nmaterial];
			// loop para carga do vetor do JSON
			int indAtivo = -1;
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[nmaterial];
			identificadores.resize(nmaterial);
			for (int i = 0; i < nmaterial; i++) {
				// enquanto a propriedade "ativo" do material esteja desabilitada, avança
				while (!is_ativo(material_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] = material_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}
				// obter id do material
				mat[i].id = material_json[indAtivo].id();
				// obter tipo do material
				if (material_json[indAtivo].tipo().exists())
				mat[i].tipo = material_json[indAtivo].tipo();
				else mat[i].tipo =0;
				// TODO: Remover BACALHAU
				// caso poço injetor e material liquido, altera para água
				if (pocinjec == 1 && mat[i].tipo == 1)
					mat[i].tipo = 2;
				// de-para do elemento "material" do json para o struct material
				mat[i].cond = 0;
				mat[i].cp = 0;
				mat[i].rho = 0;
				mat[i].visc = 0;
				mat[i].beta = 0;
				// caso material solido ou fluido informado pelo usuario (tipo = 0, 1)
				if (mat[i].tipo <= 1) {
					if (!material_json[indAtivo].condutividade().exists()
							|| !material_json[indAtivo].calorEspecifico().exists()
							|| !material_json[indAtivo].rho().exists()) {
						// RN-050: Chaves "condutividade", "calorEspecifico", "rho" requeridas para tipo de material solido ou fluido informado pelo usuario (tipo = 0, 1)
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'condutividade', 'calorEspecifico', 'rho' requeridas",
								chaveJson, "tipo");
					} else {
						// caso as chaves "condutividade", "calorEspecifico", "rho", "visc" existam
						// de-para do elemento "material" do json para o struct material
						mat[i].cond =
								material_json[indAtivo].condutividade();
						mat[i].cp =
								material_json[indAtivo].calorEspecifico();
						mat[i].rho = material_json[indAtivo].rho();

						if((mat[i].cp<1e-15 || mat[i].rho<1e-15) && transiente==1 ){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"'calorEspecifico' ou 'rho' com valor menor ou igual a zero, em uma simula��o com transiente, nestes casos, � requerido valores maiores que zero para estas propriedades",
									chaveJson, "tipo");
						}
					}
					// caso fluido informado pelo usuario e a chave "visc" inexista
					if (mat[i].tipo == 1) {
						if (!material_json[indAtivo].visc().exists()) {
							// RN-051: Chave "visc" requerida para tipo de material fluido informado pelo usuario (tipo = 1)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"'visc' requerida", chaveJson, "tipo");
						}
						else if (!material_json[indAtivo].beta().exists()) {
							// RN-051: Chave "visc" requerida para tipo de material fluido informado pelo usuario (tipo = 1)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"'beta' requerido", chaveJson, "tipo");
						}
						else {
							mat[i].visc =
									material_json[indAtivo].visc();
							mat[i].beta =
									material_json[indAtivo].beta();
							if (mat[i].visc <=0) {
								// RN-051: Chave "visc" requerida para tipo de material fluido informado pelo usuario (tipo = 1)
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"'visc' com valor iogual a zero ou negativo em um material que foi definido como fluido",
										chaveJson, "tipo ou visc");
							}

						}
					}
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-010: Unicidade da chave 'id' em '#/material'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter o elemento "secaoTransversal" do arquivo Json do MRT em struct material.
 *
 * \tparam Objeto JSON correspondente a secão transversal / corte
 */
void Ler::parse_corte(JSON_entrada_secaoTransversal& corte_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/secaoTransversal");
	try {
		// obter o tamanho do elemento do Json
		ncorte = 0;
		// percorre o array de seções transversais
		for (size_t i = 0; i < corte_json.size(); i++) {
			// caso a secao transversal esteja ativo
			if (is_ativo(corte_json[i]))
				ncorte++;
		}

		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		// caso o tamanho do elemento seja maior que zero
		if (ncorte > 0) {
			// vetor de seções transversais
			corte = new corteduto[ncorte];
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[ncorte];
			identificadores.resize(ncorte);
			// loop para carga do vetor do JSON
			int indAtivo = -1;
			for (int i = 0; i < ncorte; i++) {
				// enquanto a propriedade "ativo" da secao transversal esteja desabilitada, avança
				while (!is_ativo(corte_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] = corte_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}
				// de-para do elemento do json para o struct
				corte[i].id = corte_json[indAtivo].id();
				if (corte_json[indAtivo].anular().exists())
				corte[i].anul = corte_json[indAtivo].anular();
				else corte[i].anul =0;
				if (corte[i].anul == 1) {
					corte[i].a =
							corte_json[indAtivo].diametroExterno();
					corte[i].b =
							corte_json[indAtivo].diametroInterno();
				} else {
					corte[i].a =
							corte_json[indAtivo].diametroInterno();
					corte[i].b = corte[i].a;
				}
				if(corte[i].a<1e-20){
					string chaveSecaoIndice(chaveJson +"   "+ to_string(indAtivo));

					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Diametro interno <= 0, indice = ", chaveSecaoIndice,
							"secao transversal");
				}
				corte[i].rug = corte_json[indAtivo].rugosidade();
				// obter vetor de camadas da secao transversal
				corte[i].ncam = (int) corte_json[indAtivo].camadas().size();
				// alocar memória para os elementos das camadas
				corte[i].diam = new double[corte[i].ncam];
				corte[i].indmat = new int[corte[i].ncam];
				corte[i].discre = new int[corte[i].ncam];
				// percorrer a lista de camadas da secao transversal
				for (int j = 0; j < corte[i].ncam; j++) {
					// indicador do tipo de medicao das camadas (espessura ou diametro)
					int medicaoCamadasDiametro = 1;
					if (corte_json[indAtivo].camadas()[j].tipoMedicaoCamada().exists()) {
						string tipoMedicaoCamada(corte_json[indAtivo].camadas()[j].tipoMedicaoCamada());
						transform(tipoMedicaoCamada.begin(),
								tipoMedicaoCamada.end(),
								tipoMedicaoCamada.begin(), ::toupper);
						if (tipoMedicaoCamada.compare("ESPESSURA") == 0)
							medicaoCamadasDiametro = 0;
					}
					string chaveCamada(chaveJson + "/camadas/" + to_string(j));
					if (medicaoCamadasDiametro == 1) {
						if (corte_json[indAtivo].camadas()[j].diametro().exists()) {
							corte[i].diam[j] =
									corte_json[indAtivo].camadas()[j].diametro();
						} else {
							// RN-046: Chave "diametro" requerida
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chave diametro requerida", chaveCamada,
									"tipoMedicaoCamada=DIAMETRO");
						}
					} else {
						if (corte_json[indAtivo].camadas()[j].espessura().exists()) {
							double espessura =
									corte_json[indAtivo].camadas()[j].espessura();
							if (j > 0)
								corte[i].diam[j] = corte[i].diam[j - 1]
										+ 2 * espessura;
							else
								corte[i].diam[j] = corte[i].a + 2 * espessura;
						} else {
							// RN-047: Chave "espessura" requerida
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chave espessura requerida", chaveCamada,
									"tipoMedicaoCamada=ESPESSURA");
						}
					}
					if (corte_json[indAtivo].camadas()[j].discretizacao().exists())
					corte[i].discre[j] =
							corte_json[indAtivo].camadas()[j].discretizacao();
					else corte[i].discre[j] =1;
					if(corte[i].discre[j] ==0)corte[i].discre[j]=1;
					if(corte[i].discre[j] <=0)
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Sem discretizacao valida da camada<=0", chaveCamada,
								"Discretizacao");
					corte[i].indmat[j] =
							identificarMaterial(
									corte_json[indAtivo].camadas()[j].idMaterial());

					// verificar a existencia do identificador do material
					if (corte[i].indmat[j] == -1) {
						// RN-021: A chave '#secaoTransversal/camadas/idMaterial' deve corresponder a um '#/material/id' existente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idMaterial",
								chaveCamada,
								"'idMaterial' deve corresponder a um '#/material/id' existente");
					}
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-011: Unicidade da chave 'id' em '#/secaoTransversal'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "dutosProducao" e "dutosServico" do arquivo Json do MRT em struct detduto.
 *
 * \tparam Objeto JSON correspondente à fluidos producao
 */
void Ler::parse_unidades_producao(
		JSON_entrada_dutosProducao& dutos_producao_json) {

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/dutosProducao/");

	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;
	try {
		if (nunidadep > 0) {
			unidadeP = new detalhaP[nunidadep];
			ncelp = 0;
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[nunidadep];
			identificadores.resize(nunidadep);
			// loop para carga do vetor de unidades de producao do JSON
			int indAtivo = -1;
			for (int i = 0; i < nunidadep; i++) {
				// enquanto a propriedade "ativo" do duto de producao esteja desabilitada, avança
				while (!is_ativo(dutos_producao_json[++indAtivo]))
					;

				// criar variavel para o nome da propriedade json em processo de parse
				string chaveDutoIndice(chaveJson + to_string(indAtivo));

				// obter maior identificador
				identificadores[i] =
						dutos_producao_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				int ind = i;
				// sentido plataforma para fundo-poco
				//if (origemGeometria == origemGeometria_t::plataforma)
				if (!sentidoGeometriaSegueEscoamento)
					ind = (nunidadep - 1) - i;

				if(tipoModeloDrift==0)unidadeP[ind].correlacaoMR2 = dutos_producao_json[indAtivo].correlacaoMR2();
				else unidadeP[ind].correlacaoMR2 =-1;

				double lxy=0.;
				if(modoXY==0){
					if (sentidoGeometriaSegueEscoamento /*|| origemGeometria == origemGeometria_t::*/) {
						duto[ind].ang =
								dutos_producao_json[indAtivo].angulo();
					} else {
						duto[ind].ang =
								dutos_producao_json[indAtivo].angulo()
										* (1.0-2*reverso);
					}
				}
				else{
					double x1=dutos_producao_json[indAtivo].xCoor();
					double y1=dutos_producao_json[indAtivo].yCoor();
					if(fabsl(x1-xProd0)>1e-15){
						double tang=(y1-yProd0)/(x1-xProd0);
						duto[ind].ang =atan(tang);
					}
					else{

						if(y1>yProd0)duto[ind].ang=M_PI/2.;
						else duto[ind].ang=-M_PI/2.;
					}
					if (!sentidoGeometriaSegueEscoamento && reverso==1)duto[ind].ang *=(1.0-2*reverso);
					lxy=sqrtl((x1-xProd0)*(x1-xProd0)+(y1-yProd0)*(y1-yProd0));
					xProd0=x1;
					yProd0=y1;
				}

				duto[ind].servico = 0;
				duto[ind].indcorte = identificarCorte(
						dutos_producao_json[indAtivo].idCorte());

				/*int ist;
				for(ist=0;ist<ncorte;ist++){
					if(corte[ist].id==duto[ind].indcorte){
						duto[ind].indcorte=ist;
						break;
					}
				}
				if(ist==ncorte)
					logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/indice de secao transversal na producao inexistente");*/


				// verificar a existencia do identificador da secao transversal
				if (duto[ind].indcorte == -1) {
					// RN-022: A chave '#/dutosProducao/idCorte' deve corresponder a uma '#/secaoTransversal/id' existente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idCorte",
							chaveDutoIndice,
							"'idCorte' deve corresponder a um '#/secaoTransversal/id' existente");
				}

				// de-para do elemento "dutosProducao" do json para o struct detalhaP
				unidadeP[ind].ind = ind;
				unidadeP[ind].duto = ind;
				unidadeP[ind].formacDuto = 0;
				unidadeP[ind].ambext = 0;
				unidadeP[ind].lito = 0;  // 03-04-2018
				unidadeP[ind].difusTerm2D = 0;
				unidadeP[ind].difusTerm2DJSON ="parametros.json" ;

				unidadeP[ind].dx=0;
				unidadeP[ind].dxVar=0;
				unidadeP[ind].var=0;
				unidadeP[ind].difusTerm3D=0;
				unidadeP[ind].difusTerm3DFE=0;
				unidadeP[ind].difusTerm3DAcop=0;

				if (dutos_producao_json[indAtivo].difusTerm2D().exists()){
					unidadeP[ind].difusTerm2D =dutos_producao_json[indAtivo].difusTerm2D();
					if(unidadeP[ind].difusTerm2D==1){
						if (dutos_producao_json[indAtivo].difusTerm2DJSON().exists())
							unidadeP[ind].difusTerm2DJSON =dutos_producao_json[indAtivo].difusTerm2DJSON();
					}
				}

				if (dutos_producao_json[indAtivo].idFormacao().exists()
						&& (dutos_producao_json[indAtivo].idFormacao()
								!= -1) ) {
					unidadeP[ind].formacDuto = 1;
					unidadeP[ind].lito =
							identificarFormacao(
									dutos_producao_json[indAtivo].idFormacao());
					/*int iroc;
					unidadeP[ind].lito =dutos_producao_json[indAtivo].idFormacao();
					for(iroc=0;iroc<nform;iroc++){
						if(formacPoc[iroc].id==unidadeP[ind].lito){
							unidadeP[ind].lito=iroc;
							break;
						}
					}
					if(iroc==nform)
						logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/indice de rocha inexistente");*/

					// verificar a existencia do identificador da formacao
					if (unidadeP[ind].lito == -1) {
						// RN-053: A chave '#/dutosProducao/idFormacao' deve corresponder a uma '#/configuracaoInicial/Formacao/Propriedades/id' existente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
					if((nform==0 && unidadeP[ind].lito>=0)  || unidadeP[ind].lito>nform){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
				}

				unidadeP[ind].dirconv=0;
				if (dutos_producao_json[indAtivo].direcaoConveccao().exists())
				unidadeP[ind].dirconv =
							dutos_producao_json[indAtivo].direcaoConveccao();
				unidadeP[ind].acopcol=0;
				if (dutos_producao_json[indAtivo].acoplamentoTermico().exists()){
					unidadeP[ind].acopcol =
							dutos_producao_json[indAtivo].acoplamentoTermico();
					if(unidadeP[ind].acopcol==1 && lingas==0){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "acoplamentoTermico",
								chaveDutoIndice,
								"'acoplamentoTermico' so deve ser aplicado caso exista uma linha de servico"
								" '#/configuracaoInicial/linGas' true");
					}
				}
				unidadeP[ind].acopcolRede=0;
				if (dutos_producao_json[indAtivo].acoplamentoTermicoRedeParalela().exists()){
					unidadeP[ind].acopcolRede =
							dutos_producao_json[indAtivo].acoplamentoTermicoRedeParalela();
					if(unidadeP[ind].acopcol==1 && unidadeP[ind].acopcolRede==1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "acoplamentoTermico",
								chaveDutoIndice,
								"'acoplamentoTermico' de linha de servico ligada em conjunto com acoplamento termico de rede paralela"
								" '#/acoplamentotermico' true");
					}
				}
				if(unidadeP[ind].acopcol==1 || unidadeP[ind].acopcolRede==1)unidadeP[ind].dirconv=1;

				// caso poço injetor e formacao desabilitada
				if (pocinjec == 1 && unidadeP[ind].formacDuto == 0) {
					// habilita ambiente externo automaticamente - workaround por causa da alteracao pendente da PUC
					unidadeP[ind].ambext = 1;
				} else if ((dutos_producao_json[indAtivo].ambienteExterno().exists() && (!dutos_producao_json[indAtivo].idFormacao().exists()
						|| dutos_producao_json[indAtivo].idFormacao()== -1)) && unidadeP[ind].acopcol==0) {
					// parse do ambiente externo
					unidadeP[ind].ambext =
							dutos_producao_json[indAtivo].ambienteExterno();
				}
				if(unidadeP[ind].ambext!=0 && unidadeP[ind].ambext!=1 && unidadeP[ind].ambext!=2){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "ambienteExterno",
							chaveDutoIndice,
							" tipo de 'ambienteExterno' inexistente");
				}

				unidadeP[ind].agrupaDiscre=1;
				if (dutos_producao_json[indAtivo].agrupamento().exists())
				unidadeP[ind].agrupaDiscre = dutos_producao_json[indAtivo].agrupamento();

				unidadeP[ind].dPdLHidro=1;
				unidadeP[ind].inddPdLHidro=-1;
				if (dutos_producao_json[indAtivo].dPdLHidro().exists() && correcao.parserieHidro>0){
					unidadeP[ind].inddPdLHidro=dutos_producao_json[indAtivo].dPdLHidro();
					unidadeP[ind].dPdLHidro = correcao.dPdLHidro[unidadeP[ind].inddPdLHidro];
				}
				unidadeP[ind].dPdLFric=1;
				unidadeP[ind].inddPdLFric=-1;
				if (dutos_producao_json[indAtivo].dPdLFric().exists() && correcao.parserieFric>0){
					unidadeP[ind].inddPdLFric=dutos_producao_json[indAtivo].dPdLFric();
					unidadeP[ind].dPdLFric = correcao.dPdLFric[unidadeP[ind].inddPdLFric];
				}
				unidadeP[ind].dTdL=1;
				unidadeP[ind].inddTdL=-1;
				if (dutos_producao_json[indAtivo].dTdL().exists() && correcao.parserieDT>0){
					unidadeP[ind].inddTdL=dutos_producao_json[indAtivo].dTdL();
					unidadeP[ind].dTdL = correcao.dTdL[unidadeP[ind].inddTdL];
				}
                if(unidadeP[ind].agrupaDiscre==1 && modoXY==0){
					unidadeP[ind].ncel = 0;
					for (int j = 0;j< (int) dutos_producao_json[indAtivo].discretizacao().size();
						j++) {
						unidadeP[ind].ncel +=
							dutos_producao_json[indAtivo].discretizacao()[j].nCelulas();
					}
					// quantidade de células total da unidade de producao
					ncelp += unidadeP[ind].ncel;
					// vetor de comprimentos de cada célula desta unidade de producao
					unidadeP[ind].dx = new double[unidadeP[ind].ncel];

					int ndx;
					double tempdx;
					int totaliza = 0;
					// percorre o vetor de discretizações
					for (int kontadiscre = 0;
							kontadiscre
								< (int) dutos_producao_json[indAtivo].discretizacao().size();
							kontadiscre++) {
						ndx =
							dutos_producao_json[indAtivo].discretizacao()[kontadiscre].nCelulas();
						tempdx =
							dutos_producao_json[indAtivo].discretizacao()[kontadiscre].comprimento();
						for (int j = totaliza; j < totaliza + ndx; j++)
						unidadeP[ind].dx[j] = tempdx;
						totaliza += ndx;
					}
                }
                else if(modoXY==0){
                	unidadeP[ind].ncel=(int) dutos_producao_json[indAtivo].dxCelula().size();
                	ncelp += unidadeP[ind].ncel;
                	unidadeP[ind].dx = new double[unidadeP[ind].ncel];
                	for(int j=0;j<unidadeP[ind].ncel;j++){
                		unidadeP[ind].dx[j] =dutos_producao_json[indAtivo].dxCelula()[j];
                		int para=0;
                	}
                }
                else{
                	unidadeP[ind].ncel=(int) dutos_producao_json[indAtivo].nCelulas_XY();
                	ncelp += unidadeP[ind].ncel;
                	unidadeP[ind].dx = new double[unidadeP[ind].ncel];
                	double dxAux=lxy/unidadeP[ind].ncel;
                	for(int j=0;j<unidadeP[ind].ncel;j++){
                		unidadeP[ind].dx[j] =dxAux;
                		int para=0;
                	}
                }

                if(unidadeP[ind].ncel<=0){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'discretizacao do segmento de duto', valores <= 0",
							chaveDutoIndice, "");
                }


				if (modoDifus3D==1){
					unidadeP[ind].difusTerm3D=new int[unidadeP[ind].ncel];
					unidadeP[ind].difusTerm3DFE=new double[unidadeP[ind].ncel];
					unidadeP[ind].difusTerm3DAcop =new string[unidadeP[ind].ncel];
					int para=0;
					para++;
					for(int i3=0;i3<unidadeP[ind].ncel;i3++){
						unidadeP[ind].difusTerm3D[i3]=0;
						unidadeP[ind].difusTerm3DFE[i3]=0.;
						unidadeP[ind].difusTerm3DAcop[i3]="";
					}
					if(dutos_producao_json[indAtivo].difusTerm3D().exists()){
						for(int i3=0;i3<unidadeP[ind].ncel;i3++){
							//unidadeP[ind].difusTerm3D.push_back(dutos_producao_json[indAtivo].difusTerm3D()[i3D]);
							unidadeP[ind].difusTerm3D[i3]=dutos_producao_json[indAtivo].difusTerm3D()[i3];
							if(unidadeP[ind].difusTerm3D[i3]==1){
								//unidadeP[ind].difusTerm3DAcop.push_back(dutos_producao_json[indAtivo].difusTerm3DAcop()[i3D]);
								unidadeP[ind].difusTerm3DFE[i3]=dutos_producao_json[indAtivo].difusTerm3DFE()[i3];
								unidadeP[ind].difusTerm3DAcop[i3]=dutos_producao_json[indAtivo].difusTerm3DAcop()[i3];
							}
							//else{
								//unidadeP[ind].difusTerm3DAcop.push_back("vazio");
							//}
						}
					}
					//else{
						//for(int i3D=0;i3D<unidadeP[ind].ncel;i3D++){
							//unidadeP[ind].difusTerm3D.push_back(0);
							//unidadeP[ind].difusTerm3DAcop.push_back("vazio");
						//}
					//}
				}


				// caso sentido plataforma para fundo-poco
				if (/*origemGeometria == origemGeometria_t::plataforma*/!sentidoGeometriaSegueEscoamento) {
					Vcr<double> dxAux(unidadeP[ind].ncel);
					for (int konta = 0; konta < unidadeP[ind].ncel; konta++)
						dxAux[konta] = unidadeP[ind].dx[konta];
					for (int konta = 0; konta < unidadeP[ind].ncel; konta++)
						unidadeP[ind].dx[konta] = dxAux[(unidadeP[ind].ncel - 1)
								- konta];
				}
				// comprimento da unidade
				unidadeP[ind].comp = 0.;
				for (int j = 0; j < unidadeP[ind].ncel; j++)
					unidadeP[ind].comp += unidadeP[ind].dx[j];
				if(modoXY==1){
					if(fabsl(unidadeP[ind].comp-lxy)/lxy>1e-15)	logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "modo de preenchimento X-Y",
									chaveDutoIndice,
									"'comprimento total da discretização diferente dos limites X e Y indicados"
									" '#/configuracaoInicial/modoXY' true");
				}

				// comprimento total da linha de producao
				nCompTotalUnidadesP += unidadeP[ind].comp;

				// tamanho do vetor das condições iniciais
				int npontsTeste=0;
				unidadeP[ind].nponts = 0;
				// caso uma das chaves 'condicoesIniciais', 'condicoesIniciais/compInter' inexista
				if ((unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0)) && (!dutos_producao_json[indAtivo].condicoesIniciais().exists()
						/*|| !dutos_producao_json[indAtivo].condicoesIniciais().compInter().exists()*/)) {
					// validar obrigatoriedade das condições iniciais e do vetor compInter
					// RN-069: Chaves 'condicoesIniciais', 'condicoesIniciais/compInter' requeridas
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'condicoesIniciais', 'condicoesIniciais/compInter' requeridas",
							chaveDutoIndice, "");
				} else if(unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0)){
					if(dutos_producao_json[indAtivo].condicoesIniciais().compInter().exists())
						unidadeP[ind].nponts =
							(int) dutos_producao_json[indAtivo].condicoesIniciais().compInter().size();
					else unidadeP[ind].nponts =1;
					if(unidadeP[ind].nponts==1)npontsTeste=2;
					else npontsTeste=unidadeP[ind].nponts;
					unidadeP[ind].dxVar = new double[npontsTeste];
					unidadeP[ind].var = new double*[12];
					for (int j = 0; j < 12; j++)
						unidadeP[ind].var[j] =
								new double[npontsTeste];
				}
				else{
					unidadeP[ind].nponts = 2;
					unidadeP[ind].dxVar = new double[unidadeP[ind].nponts];
					unidadeP[ind].var = new double*[12];
					for (int j = 0; j < 12; j++)
						unidadeP[ind].var[j] = new double[unidadeP[ind].nponts];
				}

				if ((perm == 0 && descarga == 0)
						|| ((*vg1dSP).chaverede == 1 && redeperm==0)) {
					// caso uma das chaves 'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' inexista
					if (!dutos_producao_json[indAtivo].condicoesIniciais().temp().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciais().pressao().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciais().holdup().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciais().bet().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciais().uls().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciais().ugs().exists()) {
						// RN-025: Chaves 'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' requeridas para simulacao nao permanente (#configuracaoInicial/permanente = 0) e sem descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' requeridas",
								chaveDutoIndice,
								"#/configuracaoInicial/permanente:false ou #/configuracaoInicial/GLDescarga");
					} else if (((int) dutos_producao_json[indAtivo].condicoesIniciais().temp().size()
							!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().pressao().size()
									!= unidadeP[ind].nponts)
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().holdup().size()
									!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().bet().size()
									!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().uls().size()
									!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().ugs().size()
									!= unidadeP[ind].nponts )) {
						// RN-067: Tamanhos dos arrays 'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' desiguais para simulacao nao permanente nem de descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'kExterna', 'calorEspecificoExterno', 'rhoExterno', 'viscExterna' com tamanhos menores que o de compInter",
								chaveDutoIndice,
								"#/configuracaoInicial/permanente e #/configuracaoInicial/GLDescarga");
					}
				}

				if (unidadeP[ind].acopcol == 0 /*|| descarga==1*/) { // alteracao2
					// caso a chave "tempExterna" inexista
					if (!dutos_producao_json[indAtivo].condicoesIniciais().tempExterna().exists()) {
						// RN-026: Chave "tempExterna" requerida
						// para duto sem acoplamento termico ou sem descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"tempExterna requerida", chaveDutoIndice,
								"descarga habilitada ou acoplamentoExterno desabilitado");
					} else if ((int) dutos_producao_json[indAtivo].condicoesIniciais().tempExterna().size()
							!= unidadeP[ind].nponts && ((int) dutos_producao_json[indAtivo].condicoesIniciais().tempExterna().size()>1
									&& unidadeP[ind].nponts>2)) {
						// RN-024: Tamanhos do array "tempExterna" diferente do array "compInter"
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'tempExterna' com tamanhos desiguais entre si",
								chaveDutoIndice,
								"condicoesIniciais/tempExterna");
					}

					if (unidadeP[ind].formacDuto
							== 0 && unidadeP[ind].acopcol == 0) {
						if (!dutos_producao_json[indAtivo].condicoesIniciais().velExterna().exists()){
							// RN-026: Chave "velExterna" requerida
							// para duto sem acoplamento termico, sem formacao e sem ambiente externo  (#dutosProducao/i/ambienteExterno = 0)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"velExterna requerida", chaveDutoIndice,
									"formacao e acoplamentoExterno desabilitados");
						} else if ((int) dutos_producao_json[indAtivo].condicoesIniciais().velExterna().size()
								!= unidadeP[ind].nponts && ((int) dutos_producao_json[indAtivo].condicoesIniciais().velExterna().size()>1
										&& unidadeP[ind].nponts>2)) {
							// RN-057: Tamanho do array "velExterna" diferente do array "compInter"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"'compInter', 'velExterna' com tamanhos desiguais entre si",
									chaveDutoIndice,
									"condicoesIniciais/velExterna");
						}

						if (unidadeP[ind].ambext == 0) {
							// caso chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" inexistam
							if (!dutos_producao_json[indAtivo].condicoesIniciais().kExterna().exists()
									|| !dutos_producao_json[indAtivo].condicoesIniciais().calorEspecificoExterno().exists()
									|| !dutos_producao_json[indAtivo].condicoesIniciais().rhoExterno().exists()
									|| !dutos_producao_json[indAtivo].condicoesIniciais().viscExterna().exists()) {
								// RN-026: Chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" requeridas
								// para duto sem acoplamento termico, sem formacao e sem ambiente externo (#dutosProducao/i/ambienteExterno = 0)
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"kExterna, calorEspecificoExterno, rhoExterno, viscExterna requeridas",
										chaveDutoIndice,
										"formacao, acoplamentoExterno e ambienteExterno desabilitados");
								// caso duto sem acoplamento termico, sem formacao e sem ambiente externo
							} else if ((((int) dutos_producao_json[indAtivo].condicoesIniciais().kExterna().size()
									!= unidadeP[ind].nponts )
									|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().calorEspecificoExterno().size()
											!= unidadeP[ind].nponts )
									|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().rhoExterno().size()
											!= unidadeP[ind].nponts )
									|| ((int) dutos_producao_json[indAtivo].condicoesIniciais().viscExterna().size()
											!= unidadeP[ind].nponts))) {
								// RN-027: Tamanhos dos arrays "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" desiguais para duto sem formacao (#dutosProducao/i/formacao = false)
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"'compInter', 'kExterna', 'calorEspecificoExterno', 'rhoExterno', 'viscExterna' com tamanhos desiguais entre si",
										chaveDutoIndice, "formacao");
							}
						}
					}
				}

				// caso exista uma condicao de falha da aplicacao ate esta etapa
					if (!logger.getStResultadoSimulacao().sucesso) {
						// gerar o arquivo de saida da simulacao e encerra a aplicacao
						logger.write_logs_and_exit();
					}

				// percorrer vetor de pontos no comprimento do duto para interpolacao
				for (int j = 0; j < unidadeP[ind].nponts; j++) {  // alteracao2
					int indj = j;
					// sentido plataforma para fundo-poco
					if ((!sentidoGeometriaSegueEscoamento && (unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0)))
							&& unidadeP[ind].nponts==npontsTeste/*origemGeometria == origemGeometria_t::plataforma*/) {
						indj = (unidadeP[ind].nponts - 1) - j;
						if(unidadeP[ind].nponts!=2){
							unidadeP[ind].dxVar[indj] =
									1.
											- dutos_producao_json[indAtivo].condicoesIniciais().compInter()[j];
						}
						else{
							unidadeP[ind].dxVar[0] =0.;
							unidadeP[ind].dxVar[1] =1.;
						}
					} else if((unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0))&& unidadeP[ind].nponts==npontsTeste){
						if(unidadeP[ind].nponts!=2)
							unidadeP[ind].dxVar[indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().compInter()[j];
						else{
							unidadeP[ind].dxVar[0] =0.;
							unidadeP[ind].dxVar[1] =1.;
						}
					}
					else{
						if(j==0)unidadeP[ind].dxVar[indj] =0.;
						else unidadeP[ind].dxVar[indj] =1.;
					}
					// ajuste de valores em caso de lixo casas decimais
					if (unidadeP[ind].dxVar[indj] < 0.0)
						unidadeP[ind].dxVar[indj] = 0.0;
					else if (unidadeP[ind].dxVar[indj] > 1.0)
						unidadeP[ind].dxVar[indj] = 1.0;

					if ((perm == 0 && descarga == 0)
							|| ((*vg1dSP).chaverede == 1 && redeperm==0)) {
						if(dutos_producao_json[indAtivo].condicoesIniciais().temp().size()>indj)
						unidadeP[ind].var[0][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().temp()[j];
						else unidadeP[ind].var[0][indj] =unidadeP[ind].var[0][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciais().pressao().size()>indj)
						unidadeP[ind].var[1][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().pressao()[j];
						else unidadeP[ind].var[1][indj] =unidadeP[ind].var[1][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciais().holdup().size()>indj)
						unidadeP[ind].var[2][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().holdup()[j];
						else unidadeP[ind].var[2][indj] =unidadeP[ind].var[2][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciais().bet().size()>indj)
						unidadeP[ind].var[3][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().bet()[j];
						else unidadeP[ind].var[3][indj] =unidadeP[ind].var[3][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciais().uls().size()>indj)
						unidadeP[ind].var[4][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().uls()[j];
						else unidadeP[ind].var[4][indj] =unidadeP[ind].var[4][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciais().ugs().size()>indj)
						unidadeP[ind].var[5][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().ugs()[j];
						else unidadeP[ind].var[5][indj] =unidadeP[ind].var[5][indj-1];

					} else {
						//if(flashCompleto != 1){
							unidadeP[ind].var[0][indj] = 10;
							unidadeP[ind].var[1][indj] = 10;
						//}
						//else{
							//unidadeP[ind].var[0][indj] = flash[0].rholF[0][2] ;
							//unidadeP[ind].var[1][indj] = flash[0].rholF[2][0];
						//}
						unidadeP[ind].var[2][indj] = 0.5;
						unidadeP[ind].var[3][indj] = 0.;
						unidadeP[ind].var[4][indj] = 0;
						unidadeP[ind].var[5][indj] = 0;
					}
					if (unidadeP[ind].acopcol == 0 || descarga == 1){
						if(dutos_producao_json[indAtivo].condicoesIniciais().tempExterna().size()>indj)
						unidadeP[ind].var[6][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().tempExterna()[j];
						else unidadeP[ind].var[6][indj] =unidadeP[ind].var[6][indj-1];
					}
					else
						unidadeP[ind].var[6][indj] = 0.;
					if (unidadeP[ind].formacDuto == 0
							&& (/*unidadeP[ind].ambext==0 && */unidadeP[ind].acopcol
									== 0)) {  // alteracao2
						if(dutos_producao_json[indAtivo].condicoesIniciais().velExterna().size()>indj)
						unidadeP[ind].var[7][indj] =
								dutos_producao_json[indAtivo].condicoesIniciais().velExterna()[j];
						else unidadeP[ind].var[7][indj] =unidadeP[ind].var[7][indj-1];

						if (unidadeP[ind].ambext == 0) {
							if(dutos_producao_json[indAtivo].condicoesIniciais().kExterna().size()>indj)
							unidadeP[ind].var[8][indj] =
									dutos_producao_json[indAtivo].condicoesIniciais().kExterna()[j];
							else unidadeP[ind].var[8][indj] =unidadeP[ind].var[8][indj-1];

							if(dutos_producao_json[indAtivo].condicoesIniciais().calorEspecificoExterno().size()>indj)
							unidadeP[ind].var[9][indj] =
									dutos_producao_json[indAtivo].condicoesIniciais().calorEspecificoExterno()[j];
							else unidadeP[ind].var[9][indj] =unidadeP[ind].var[9][indj-1];

							if(dutos_producao_json[indAtivo].condicoesIniciais().rhoExterno().size()>indj)
							unidadeP[ind].var[10][indj] =
									dutos_producao_json[indAtivo].condicoesIniciais().rhoExterno()[j];
							else unidadeP[ind].var[10][indj] =unidadeP[ind].var[10][indj-1];

							if(dutos_producao_json[indAtivo].condicoesIniciais().viscExterna().size()>indj)
							unidadeP[ind].var[11][indj] =
									dutos_producao_json[indAtivo].condicoesIniciais().viscExterna()[j];
							else unidadeP[ind].var[11][indj] =unidadeP[ind].var[11][indj-1];

						} else {
							unidadeP[ind].var[8][indj] = 0.;
							unidadeP[ind].var[9][indj] = 0.;
							unidadeP[ind].var[10][indj] = 0.;
							unidadeP[ind].var[11][indj] = 0.;
						}
					} else {
						unidadeP[ind].var[7][indj] = 0.;
						unidadeP[ind].var[8][indj] = 0.;
						unidadeP[ind].var[9][indj] = 0.;
						unidadeP[ind].var[10][indj] = 0.;
						unidadeP[ind].var[11][indj] = 0.;
					}
				}
				if(unidadeP[ind].nponts!=npontsTeste){
						unidadeP[ind].dxVar[0] = 0.0;
						unidadeP[ind].dxVar[1] = 1.0;
						for (int k=0; k<=11; k++){
							if(sentidoGeometriaSegueEscoamento){
								if(ind>0){
									unidadeP[ind].var[k][1] = unidadeP[ind].var[k][0];
									unidadeP[ind].var[k][0] = unidadeP[ind-1].var[k][1];
								}
								else{
									unidadeP[ind].var[k][1] = unidadeP[ind].var[k][0];
								}
							}
							else{
								if(ind<(nunidadep - 1)){
									unidadeP[ind].var[k][1] = unidadeP[ind+1].var[k][0];
									//unidadeP[ind].var[k][0] = unidadeP[ind].var[k][0];
								}
								else{
									unidadeP[ind].var[k][1] = unidadeP[ind].var[k][0];
								}
							}
						}
						unidadeP[ind].nponts++;
				}
			}


			unidadeP[0].Lini = 0.;
			for (int i = 1; i < nunidadep; i++) {
				unidadeP[i].Lini = unidadeP[i - 1].Lini + unidadeP[i - 1].comp;
			}

			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-012: Unicidade da chave 'id' em '#/dutosProducao'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_unidades_producaoAmb(
		JSON_entrada_dutosProducao& dutos_producao_json) {

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/dutosProducao/");

	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;
	try {
		if (nunidadep > 0) {
			unidadeP = new detalhaP[nunidadep];
			ncelp = 0;
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[nunidadep];
			identificadores.resize(nunidadep);
			// loop para carga do vetor de unidades de producao do JSON
			int indAtivo = -1;
			for (int i = 0; i < nunidadep; i++) {
				// enquanto a propriedade "ativo" do duto de producao esteja desabilitada, avança
				while (!is_ativo(dutos_producao_json[++indAtivo]))
					;

				// criar variavel para o nome da propriedade json em processo de parse
				string chaveDutoIndice(chaveJson + to_string(indAtivo));

				// obter maior identificador
				identificadores[i] =
						dutos_producao_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				int ind = i;
				// sentido plataforma para fundo-poco
				//if (origemGeometria == origemGeometria_t::plataforma)
				if (!sentidoGeometriaSegueEscoamento)
					ind = (nunidadep - 1) - i;

				if(tipoModeloDrift==0)unidadeP[ind].correlacaoMR2 = dutos_producao_json[indAtivo].correlacaoMR2();
				else unidadeP[ind].correlacaoMR2 =-1;

				double lxy;
				if(modoXY==0){
					if (sentidoGeometriaSegueEscoamento /*|| origemGeometria == origemGeometria_t::*/) {
						duto[ind].ang =
								dutos_producao_json[indAtivo].angulo();
					} else {
						duto[ind].ang =
								dutos_producao_json[indAtivo].angulo()
										* (1.0-2*reverso);
					}
				}
				else{
					double x1=dutos_producao_json[indAtivo].xCoor();
					double y1=dutos_producao_json[indAtivo].yCoor();
					if(fabsl(x1-xProd0)>1e-15){
						double tang=(y1-yProd0)/fabsl(x1-xProd0);
						duto[ind].ang =atan(tang);
					}
					else{

						if(y1>yProd0)duto[ind].ang=M_PI/2.;
						else duto[ind].ang=-M_PI/2.;
					}
					if (!sentidoGeometriaSegueEscoamento && reverso==1)duto[ind].ang *=(1.0-2*reverso);
					lxy=sqrtl((x1-xProd0)*(x1-xProd0)+(y1-yProd0)*(y1-yProd0));
					xProd0=x1;
					yProd0=y1;
				}

				duto[ind].servico = 0;
				duto[ind].indcorte = identificarCorte(
						dutos_producao_json[indAtivo].idCorte());

				/*int ist;
				for(ist=0;ist<ncorte;ist++){
					if(corte[ist].id==duto[ind].indcorte){
						duto[ind].indcorte=ist;
						break;
					}
				}
				if(ist==ncorte)
					logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/indice de secao transversal na producao inexistente");*/


				// verificar a existencia do identificador da secao transversal
				if (duto[ind].indcorte == -1) {
					// RN-022: A chave '#/dutosProducao/idCorte' deve corresponder a uma '#/secaoTransversal/id' existente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idCorte",
							chaveDutoIndice,
							"'idCorte' deve corresponder a um '#/secaoTransversal/id' existente");
				}

				// de-para do elemento "dutosProducao" do json para o struct detalhaP
				unidadeP[ind].ind = ind;
				unidadeP[ind].duto = ind;
				unidadeP[ind].formacDuto = 0;
				unidadeP[ind].ambext = 0;
				unidadeP[ind].lito = 0;  // 03-04-2018
				unidadeP[ind].difusTerm2D = 0;
				unidadeP[ind].difusTerm2DJSON ="parametros.json" ;
				unidadeP[ind].difusTerm3D = 0;
				unidadeP[ind].difusTerm3DFE = 0;
				unidadeP[ind].difusTerm3DAcop =0 ;
				unidadeP[ind].dx=0;
				unidadeP[ind].dxVar=0;
				unidadeP[ind].var=0;

				if (dutos_producao_json[indAtivo].difusTerm2D().exists()){
					unidadeP[ind].difusTerm2D =dutos_producao_json[indAtivo].difusTerm2D();
					if(unidadeP[ind].difusTerm2D==1){
						if (dutos_producao_json[indAtivo].difusTerm2DJSON().exists())
							unidadeP[ind].difusTerm2DJSON =dutos_producao_json[indAtivo].difusTerm2DJSON();
					}
				}

				if (dutos_producao_json[indAtivo].idFormacao().exists()
						&& (dutos_producao_json[indAtivo].idFormacao()
								!= -1) ) {
					unidadeP[ind].formacDuto = 1;
					unidadeP[ind].lito =
							identificarFormacao(
									dutos_producao_json[indAtivo].idFormacao());
					/*int iroc;
					unidadeP[ind].lito =dutos_producao_json[indAtivo].idFormacao();
					for(iroc=0;iroc<nform;iroc++){
						if(formacPoc[iroc].id==unidadeP[ind].lito){
							unidadeP[ind].lito=iroc;
							break;
						}
					}
					if(iroc==nform)
						logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/indice de rocha inexistente");*/

					// verificar a existencia do identificador da formacao
					if (unidadeP[ind].lito == -1) {
						// RN-053: A chave '#/dutosProducao/idFormacao' deve corresponder a uma '#/configuracaoInicial/Formacao/Propriedades/id' existente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
					if((nform==0 && unidadeP[ind].lito>=0)  || unidadeP[ind].lito>nform){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
				}

				unidadeP[ind].dirconv=0;
				if (dutos_producao_json[indAtivo].direcaoConveccao().exists())
				unidadeP[ind].dirconv =
							dutos_producao_json[indAtivo].direcaoConveccao();
				unidadeP[ind].acopcol=0;
				if (dutos_producao_json[indAtivo].acoplamentoTermico().exists()){
					unidadeP[ind].acopcol =
							dutos_producao_json[indAtivo].acoplamentoTermico();
					if(unidadeP[ind].acopcol==1 && lingas==0){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "acoplamentoTermico",
								chaveDutoIndice,
								"'acoplamentoTermico' so deve ser aplicado caso exista uma linha de servico"
								" '#/configuracaoInicial/linGas' true");
					}
				}
				unidadeP[ind].acopcolRede=0;
				if (dutos_producao_json[indAtivo].acoplamentoTermicoRedeParalela().exists()){
					unidadeP[ind].acopcolRede =
							dutos_producao_json[indAtivo].acoplamentoTermicoRedeParalela();
					if(unidadeP[ind].acopcol==1 && unidadeP[ind].acopcolRede==1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "acoplamentoTermico",
								chaveDutoIndice,
								"'acoplamentoTermico' de linha de servico ligada em conjunto com acoplamento termico de rede paralela"
								" '#/acoplamentotermico' true");
					}
				}
				if(unidadeP[ind].acopcol==1 || unidadeP[ind].acopcolRede==1)unidadeP[ind].dirconv=1;

				// caso poço injetor e formacao desabilitada
				if (pocinjec == 1 && unidadeP[ind].formacDuto == 0) {
					// habilita ambiente externo automaticamente - workaround por causa da alteracao pendente da PUC
					unidadeP[ind].ambext = 1;
				} else if ((dutos_producao_json[indAtivo].ambienteExterno().exists() && (!dutos_producao_json[indAtivo].idFormacao().exists()
						|| dutos_producao_json[indAtivo].idFormacao()== -1)) && unidadeP[ind].acopcol==0) {
					// parse do ambiente externo
					unidadeP[ind].ambext =
							dutos_producao_json[indAtivo].ambienteExterno();
				}
				if(unidadeP[ind].ambext!=0 && unidadeP[ind].ambext!=1 && unidadeP[ind].ambext!=2){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "ambienteExterno",
							chaveDutoIndice,
							" tipo de 'ambienteExterno' inexistente");
				}

				unidadeP[ind].agrupaDiscre=1;
				if (dutos_producao_json[indAtivo].agrupamento().exists())
				unidadeP[ind].agrupaDiscre = dutos_producao_json[indAtivo].agrupamento();

				unidadeP[ind].dPdLHidro=1;
				unidadeP[ind].inddPdLHidro=-1;
				if (dutos_producao_json[indAtivo].dPdLHidro().exists() && correcao.parserieHidro>0){
					unidadeP[ind].inddPdLHidro=dutos_producao_json[indAtivo].dPdLHidro();
					unidadeP[ind].dPdLHidro = correcao.dPdLHidro[unidadeP[ind].inddPdLHidro];
				}
				unidadeP[ind].dPdLFric=1;
				unidadeP[ind].inddPdLFric=-1;
				if (dutos_producao_json[indAtivo].dPdLFric().exists() && correcao.parserieFric>0){
					unidadeP[ind].inddPdLFric=dutos_producao_json[indAtivo].dPdLFric();
					unidadeP[ind].dPdLFric = correcao.dPdLFric[unidadeP[ind].inddPdLFric];
				}
				unidadeP[ind].dTdL=1;
				unidadeP[ind].inddTdL=-1;
				if (dutos_producao_json[indAtivo].dTdL().exists() && correcao.parserieDT>0){
					unidadeP[ind].inddTdL=dutos_producao_json[indAtivo].dTdL();
					unidadeP[ind].dTdL = correcao.dTdL[unidadeP[ind].inddTdL];
				}
                if(unidadeP[ind].agrupaDiscre==1 && modoXY==0){
					unidadeP[ind].ncel = 0;
					for (int j = 0;j< (int) dutos_producao_json[indAtivo].discretizacao().size();
						j++) {
						unidadeP[ind].ncel +=
							dutos_producao_json[indAtivo].discretizacao()[j].nCelulas();
					}
					// quantidade de células total da unidade de producao
					ncelp += unidadeP[ind].ncel;
					// vetor de comprimentos de cada célula desta unidade de producao
					unidadeP[ind].dx = new double[unidadeP[ind].ncel];

					int ndx;
					double tempdx;
					int totaliza = 0;
					// percorre o vetor de discretizações
					for (int kontadiscre = 0;
							kontadiscre
								< (int) dutos_producao_json[indAtivo].discretizacao().size();
							kontadiscre++) {
						ndx =
							dutos_producao_json[indAtivo].discretizacao()[kontadiscre].nCelulas();
						tempdx =
							dutos_producao_json[indAtivo].discretizacao()[kontadiscre].comprimento();
						for (int j = totaliza; j < totaliza + ndx; j++)
						unidadeP[ind].dx[j] = tempdx;
						totaliza += ndx;
					}
                }
                else if(modoXY==0){
                	unidadeP[ind].ncel=(int) dutos_producao_json[indAtivo].dxCelula().size();
                	ncelp += unidadeP[ind].ncel;
                	unidadeP[ind].dx = new double[unidadeP[ind].ncel];
                	for(int j=0;j<unidadeP[ind].ncel;j++){
                		unidadeP[ind].dx[j] =dutos_producao_json[indAtivo].dxCelula()[j];
                		int para=0;
                	}
                }
                else{
                	unidadeP[ind].ncel=(int) dutos_producao_json[indAtivo].nCelulas_XY();
                	ncelp += unidadeP[ind].ncel;
                	unidadeP[ind].dx = new double[unidadeP[ind].ncel];
                	double dxAux=lxy/unidadeP[ind].ncel;
                	for(int j=0;j<unidadeP[ind].ncel;j++){
                		unidadeP[ind].dx[j] =dxAux;
                		int para=0;
                	}
                }

                if(unidadeP[ind].ncel<=0){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'discretizacao do segmento de duto', valores <= 0",
							chaveDutoIndice, "");
                }


				if (modoDifus3D==1){
					unidadeP[ind].difusTerm3D=new int[unidadeP[ind].ncel];
					unidadeP[ind].difusTerm3DFE=new double[unidadeP[ind].ncel];
					unidadeP[ind].difusTerm3DAcop =new string[unidadeP[ind].ncel];
					int para=0;
					para++;
					for(int i3=0;i3<unidadeP[ind].ncel;i3++){
						unidadeP[ind].difusTerm3D[i3]=0;
						unidadeP[ind].difusTerm3DFE[i3]=0.;
						unidadeP[ind].difusTerm3DAcop[i3]="";
					}
					if(dutos_producao_json[indAtivo].difusTerm3D().exists()){
						for(int i3=0;i3<unidadeP[ind].ncel;i3++){
							//unidadeP[ind].difusTerm3D.push_back(dutos_producao_json[indAtivo].difusTerm3D()[i3D]);
							unidadeP[ind].difusTerm3D[i3]=dutos_producao_json[indAtivo].difusTerm3D()[i3];
							if(unidadeP[ind].difusTerm3D[i3]==1){
								//unidadeP[ind].difusTerm3DAcop.push_back(dutos_producao_json[indAtivo].difusTerm3DAcop()[i3D]);
								unidadeP[ind].difusTerm3DFE[i3]=dutos_producao_json[indAtivo].difusTerm3DFE()[i3];
								unidadeP[ind].difusTerm3DAcop[i3]=dutos_producao_json[indAtivo].difusTerm3DAcop()[i3];
							}
							//else{
								//unidadeP[ind].difusTerm3DAcop.push_back("vazio");
							//}
						}
					}
					//else{
						//for(int i3D=0;i3D<unidadeP[ind].ncel;i3D++){
							//unidadeP[ind].difusTerm3D.push_back(0);
							//unidadeP[ind].difusTerm3DAcop.push_back("vazio");
						//}
					//}
				}


				// caso sentido plataforma para fundo-poco
				if (/*origemGeometria == origemGeometria_t::plataforma*/!sentidoGeometriaSegueEscoamento) {
					Vcr<double> dxAux(unidadeP[ind].ncel);
					for (int konta = 0; konta < unidadeP[ind].ncel; konta++)
						dxAux[konta] = unidadeP[ind].dx[konta];
					for (int konta = 0; konta < unidadeP[ind].ncel; konta++)
						unidadeP[ind].dx[konta] = dxAux[(unidadeP[ind].ncel - 1)
								- konta];
				}
				// comprimento da unidade
				unidadeP[ind].comp = 0.;
				for (int j = 0; j < unidadeP[ind].ncel; j++)
					unidadeP[ind].comp += unidadeP[ind].dx[j];
				if(modoXY==1){
					if(fabsl(unidadeP[ind].comp-lxy)/lxy>1e-15)	logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "modo de preenchimento X-Y",
									chaveDutoIndice,
									"'comprimento total da discretização diferente dos limites X e Y indicados"
									" '#/configuracaoInicial/modoXY' true");
				}

				// comprimento total da linha de producao
				nCompTotalUnidadesP += unidadeP[ind].comp;

				// tamanho do vetor das condições iniciais
				int npontsTeste=0;
				unidadeP[ind].nponts = 0;
				// caso uma das chaves 'condicoesIniciais', 'condicoesIniciais/compInter' inexista
				if ((unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0)) && (!dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().exists()
						/*|| !dutos_producao_json[indAtivo].condicoesIniciais().compInter().exists()*/)) {
					// validar obrigatoriedade das condições iniciais e do vetor compInter
					// RN-069: Chaves 'condicoesIniciais', 'condicoesIniciais/compInter' requeridas
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'condicoesIniciais', 'condicoesIniciais/compInter' requeridas",
							chaveDutoIndice, "");
				} else if(unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0)){
					if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().compInter().exists())
						unidadeP[ind].nponts =
							(int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().compInter().size();
					else unidadeP[ind].nponts =1;
					if(unidadeP[ind].nponts==1)npontsTeste=2;
					else npontsTeste=unidadeP[ind].nponts;
					unidadeP[ind].dxVar = new double[npontsTeste];
					unidadeP[ind].var = new double*[12];
					for (int j = 0; j < 12; j++)
						unidadeP[ind].var[j] =
								new double[npontsTeste];
				}
				else{
					unidadeP[ind].nponts = 2;
					unidadeP[ind].dxVar = new double[unidadeP[ind].nponts];
					unidadeP[ind].var = new double*[12];
					for (int j = 0; j < 12; j++)
						unidadeP[ind].var[j] = new double[unidadeP[ind].nponts];
				}

				if ((perm == 0 && descarga == 0)
						|| ((*vg1dSP).chaverede == 1/* && redeperm==0*/)) {
					// caso uma das chaves 'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' inexista
					if (!dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().temp().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().pressao().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().holdup().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().bet().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().uls().exists()
							|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().ugs().exists()) {
						// RN-025: Chaves 'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' requeridas para simulacao nao permanente (#configuracaoInicial/permanente = 0) e sem descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' requeridas",
								chaveDutoIndice,
								"#/configuracaoInicial/permanente e #/configuracaoInicial/GLDescarga");
					} else if (((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().temp().size()
							!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().pressao().size()
									!= unidadeP[ind].nponts)
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().holdup().size()
									!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().bet().size()
									!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().uls().size()
									!= unidadeP[ind].nponts )
							|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().ugs().size()
									!= unidadeP[ind].nponts )) {
						// RN-067: Tamanhos dos arrays 'temp', 'pressao', 'holdup', 'bet', 'uls', 'ugs' desiguais para simulacao nao permanente nem de descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'kExterna', 'calorEspecificoExterno', 'rhoExterno', 'viscExterna' com tamanhos menores que o de compInter",
								chaveDutoIndice,
								"#/configuracaoInicial/permanente e #/configuracaoInicial/GLDescarga");
					}
				}

				if (unidadeP[ind].acopcol == 0 /*|| descarga==1*/) { // alteracao2
					// caso a chave "tempExterna" inexista
					if (!dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna().exists()) {
						// RN-026: Chave "tempExterna" requerida
						// para duto sem acoplamento termico ou sem descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"tempExterna requerida", chaveDutoIndice,
								"descarga habilitada ou acoplamentoExterno desabilitado");
					} else if ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna().size()
							!= unidadeP[ind].nponts && ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna().size()>1
									&& unidadeP[ind].nponts>2)) {
						// RN-024: Tamanhos do array "tempExterna" diferente do array "compInter"
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'tempExterna' com tamanhos desiguais entre si",
								chaveDutoIndice,
								"condicoesIniciais/tempExterna");
					}

					if (unidadeP[ind].formacDuto
							== 0 && unidadeP[ind].acopcol == 0) {
						if (!dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().velExterna().exists()){
							// RN-026: Chave "velExterna" requerida
							// para duto sem acoplamento termico, sem formacao e sem ambiente externo  (#dutosProducao/i/ambienteExterno = 0)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"velExterna requerida", chaveDutoIndice,
									"formacao e acoplamentoExterno desabilitados");
						} else if ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().velExterna().size()
								!= unidadeP[ind].nponts && ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().velExterna().size()>1
										&& unidadeP[ind].nponts>2)) {
							// RN-057: Tamanho do array "velExterna" diferente do array "compInter"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"'compInter', 'velExterna' com tamanhos desiguais entre si",
									chaveDutoIndice,
									"condicoesIniciais/velExterna");
						}

						if (unidadeP[ind].ambext == 0) {
							// caso chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" inexistam
							if (!dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().kExterna().exists()
									|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno().exists()
									|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno().exists()
									|| !dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna().exists()) {
								// RN-026: Chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" requeridas
								// para duto sem acoplamento termico, sem formacao e sem ambiente externo (#dutosProducao/i/ambienteExterno = 0)
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"kExterna, calorEspecificoExterno, rhoExterno, viscExterna requeridas",
										chaveDutoIndice,
										"formacao, acoplamentoExterno e ambienteExterno desabilitados");
								// caso duto sem acoplamento termico, sem formacao e sem ambiente externo
							} else if ((((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().kExterna().size()
									!= unidadeP[ind].nponts )
									|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno().size()
											!= unidadeP[ind].nponts )
									|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno().size()
											!= unidadeP[ind].nponts )
									|| ((int) dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna().size()
											!= unidadeP[ind].nponts))) {
								// RN-027: Tamanhos dos arrays "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" desiguais para duto sem formacao (#dutosProducao/i/formacao = false)
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"'compInter', 'kExterna', 'calorEspecificoExterno', 'rhoExterno', 'viscExterna' com tamanhos desiguais entre si",
										chaveDutoIndice, "formacao");
							}
						}
					}
				}

				// caso exista uma condicao de falha da aplicacao ate esta etapa
					if (!logger.getStResultadoSimulacao().sucesso) {
						// gerar o arquivo de saida da simulacao e encerra a aplicacao
						logger.write_logs_and_exit();
					}

				// percorrer vetor de pontos no comprimento do duto para interpolacao
				for (int j = 0; j < unidadeP[ind].nponts; j++) {  // alteracao2
					int indj = j;
					// sentido plataforma para fundo-poco
					if ((!sentidoGeometriaSegueEscoamento && (unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0)))
							&& unidadeP[ind].nponts==npontsTeste/*origemGeometria == origemGeometria_t::plataforma*/) {
						indj = (unidadeP[ind].nponts - 1) - j;
						if(unidadeP[ind].nponts!=2){
							unidadeP[ind].dxVar[indj] =
									1.
											- dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().compInter()[j];
						}
						else{
							unidadeP[ind].dxVar[0] =0.;
							unidadeP[ind].dxVar[1] =1.;
						}
					} else if((unidadeP[ind].acopcol == 0 || (perm == 0 && descarga == 0))&& unidadeP[ind].nponts==npontsTeste){
						if(unidadeP[ind].nponts!=2)
							unidadeP[ind].dxVar[indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().compInter()[j];
						else{
							unidadeP[ind].dxVar[0] =0.;
							unidadeP[ind].dxVar[1] =1.;
						}
					}
					else{
						if(j==0)unidadeP[ind].dxVar[indj] =0.;
						else unidadeP[ind].dxVar[indj] =1.;
					}
					// ajuste de valores em caso de lixo casas decimais
					if (unidadeP[ind].dxVar[indj] < 0.0)
						unidadeP[ind].dxVar[indj] = 0.0;
					else if (unidadeP[ind].dxVar[indj] > 1.0)
						unidadeP[ind].dxVar[indj] = 1.0;

					if ((perm == 0 && descarga == 0)
							|| ((*vg1dSP).chaverede == 1 /*&& redeperm==0*/)) {
						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().temp().size()>indj)
						unidadeP[ind].var[0][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().temp()[j];
						else unidadeP[ind].var[0][indj] =unidadeP[ind].var[0][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().pressao().size()>indj)
						unidadeP[ind].var[1][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().pressao()[j];
						else unidadeP[ind].var[1][indj] =unidadeP[ind].var[1][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().holdup().size()>indj)
						unidadeP[ind].var[2][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().holdup()[j];
						else unidadeP[ind].var[2][indj] =unidadeP[ind].var[2][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().bet().size()>indj)
						unidadeP[ind].var[3][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().bet()[j];
						else unidadeP[ind].var[3][indj] =unidadeP[ind].var[3][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().uls().size()>indj)
						unidadeP[ind].var[4][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().uls()[j];
						else unidadeP[ind].var[4][indj] =unidadeP[ind].var[4][indj-1];

						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().ugs().size()>indj)
						unidadeP[ind].var[5][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().ugs()[j];
						else unidadeP[ind].var[5][indj] =unidadeP[ind].var[5][indj-1];

					} else {
						//if(flashCompleto != 1){
							unidadeP[ind].var[0][indj] = 10;
							unidadeP[ind].var[1][indj] = 10;
						//}
						//else{
							//unidadeP[ind].var[0][indj] = flash[0].rholF[0][2] ;
							//unidadeP[ind].var[1][indj] = flash[0].rholF[2][0];
						//}
						unidadeP[ind].var[2][indj] = 0.5;
						unidadeP[ind].var[3][indj] = 0.;
						unidadeP[ind].var[4][indj] = 0;
						unidadeP[ind].var[5][indj] = 0;
					}
					if (unidadeP[ind].acopcol == 0 || descarga == 1){
						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna().size()>indj)
						unidadeP[ind].var[6][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna()[j];
						else unidadeP[ind].var[6][indj] =unidadeP[ind].var[6][indj-1];
					}
					else
						unidadeP[ind].var[6][indj] = 0.;
					if (unidadeP[ind].formacDuto == 0
							&& (/*unidadeP[ind].ambext==0 && */unidadeP[ind].acopcol
									== 0)) {  // alteracao2
						if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().velExterna().size()>indj)
						unidadeP[ind].var[7][indj] =
								dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().velExterna()[j];
						else unidadeP[ind].var[7][indj] =unidadeP[ind].var[7][indj-1];

						if (unidadeP[ind].ambext == 0) {
							if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().kExterna().size()>indj)
							unidadeP[ind].var[8][indj] =
									dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().kExterna()[j];
							else unidadeP[ind].var[8][indj] =unidadeP[ind].var[8][indj-1];

							if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno().size()>indj)
							unidadeP[ind].var[9][indj] =
									dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno()[j];
							else unidadeP[ind].var[9][indj] =unidadeP[ind].var[9][indj-1];

							if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno().size()>indj)
							unidadeP[ind].var[10][indj] =
									dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno()[j];
							else unidadeP[ind].var[10][indj] =unidadeP[ind].var[10][indj-1];

							if(dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna().size()>indj)
							unidadeP[ind].var[11][indj] =
									dutos_producao_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna()[j];
							else unidadeP[ind].var[11][indj] =unidadeP[ind].var[11][indj-1];

						} else {
							unidadeP[ind].var[8][indj] = 0.;
							unidadeP[ind].var[9][indj] = 0.;
							unidadeP[ind].var[10][indj] = 0.;
							unidadeP[ind].var[11][indj] = 0.;
						}
					} else {
						unidadeP[ind].var[7][indj] = 0.;
						unidadeP[ind].var[8][indj] = 0.;
						unidadeP[ind].var[9][indj] = 0.;
						unidadeP[ind].var[10][indj] = 0.;
						unidadeP[ind].var[11][indj] = 0.;
					}
				}
				if(unidadeP[ind].nponts!=npontsTeste){
						unidadeP[ind].dxVar[0] = 0.0;
						unidadeP[ind].dxVar[1] = 1.0;
						for (int k=0; k<=11; k++){
							if(ind>0){
								unidadeP[ind].var[k][1] = unidadeP[ind].var[k][0];
								unidadeP[ind].var[k][0] = unidadeP[ind-1].var[k][1];
							}
							else{
								unidadeP[ind].var[k][1] = unidadeP[ind].var[k][0];
							}
						}
						unidadeP[ind].nponts++;
				}
			}


			unidadeP[0].Lini = 0.;
			for (int i = 1; i < nunidadep; i++) {
				unidadeP[i].Lini = unidadeP[i - 1].Lini + unidadeP[i - 1].comp;
			}

			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-012: Unicidade da chave 'id' em '#/dutosProducao'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "dutosServico" do arquivo Json do MRT em struct detduto.
 *
 * \tparam Objeto JSON correspondente a dutos de servico
 */
void Ler::parse_unidades_servico(
		JSON_entrada_dutosServico& dutos_servico_json) {

	/*
	 "dutosServico": [{
	 "ativo": false, "id": 0, "idCorte": 1, "angulo": 0.9987, "rotulo": "riser2", "direcaoConveccao": 0, "acoplamentoTermico": 0,
	 "discretizacao": [{ "nCelulas": 50, "comprimento": 40.1931 }],
	 "condicoesIniciais": { "compInter": [0, 1], "temp": [4.0, 4.0], "pressao": [90, 25], "holdup": [0.0, 0.0], "bet": [0, 0], "uls": [0, 0], "ugs": [0, 0],
	 "tempExterna": [4.0, 25.0], "velExterna": [0.4, 0.4], "kExterna": [0.603, 0.603],
	 "calorEspecificoExterno": [4182, 4182], "rhoExterno": [1000.0, 1000.0], "viscExterna": [0.001, 0.001] }
	 }]
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/dutosServico/");
	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;

	try {
		if (nunidadeg > 0) {
			unidadeG = new detalhaG[nunidadeg];
			ncelg = 0;
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[nunidadeg];
			identificadores.resize(nunidadeg);
			// loop para carga do vetor de unidades de servico do JSON
			int indAtivo = -1;
			for (int i = 0; i < nunidadeg; i++) {
				// enquanto a propriedade "ativo" do duto de servico esteja desabilitada, avança
				while (!is_ativo(dutos_servico_json[++indAtivo]))
					;

				// criar variavel para o nome da propriedade json em processo de parse
				string chaveDutoIndice(chaveJson + to_string(indAtivo));

				// obter maior identificador
				identificadores[i] =
						dutos_servico_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				// de-para do elemento "dutosServico" do json para o struct detduto
				double lxy;
				if(modoXY==0){
					duto[i + nunidadep].ang =
							dutos_servico_json[indAtivo].angulo();
				}
				else{
					double x1=dutos_servico_json[indAtivo].xCoor();
					double y1=dutos_servico_json[indAtivo].yCoor();

					if(fabsl(x1-xServ0)>1e-15){
						double tang=(y1-yServ0)/fabsl(x1-xServ0);
						duto[i + nunidadep].ang =atan(tang);
					}
					else{

						if(y1>yServ0)duto[i + nunidadep].ang=M_PI/2.;
						else duto[i + nunidadep].ang=-M_PI/2.;
					}

					//double tang=(y1-yServ0)/(x1-xServ0);
					//duto[i + nunidadep].ang =atan(tang);
					lxy=sqrtl((x1-xServ0)*(x1-xServ0)+(y1-yServ0)*(y1-yServ0));
					xServ0=x1;
					yServ0=y1;
				}
				duto[i + nunidadep].servico = 1;
				duto[i + nunidadep].indcorte = identificarCorte(
						dutos_servico_json[indAtivo].idCorte());

				// verificar a existencia do identificador da secao transversal
				if (duto[i + nunidadep].indcorte == -1) {
					// RN-023: A chave '#/dutosServico/idCorte' deve corresponder a uma '#/secaoTransversal/id' existente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idCorte",
							chaveDutoIndice,
							"'idCorte' deve corresponder a um '#/secaoTransversal/id' existente");
				}

				// de-para do elemento "dutosServico" do json para o struct detalhaG
				unidadeG[i].ind = i;
				unidadeG[i].duto = i + nunidadep;
				unidadeG[i].formacDuto = 0;
				unidadeG[i].ambext = 0;
				unidadeG[i].lito = 0;  // 03-04-2018

				unidadeG[i].dx=0;
				unidadeG[i].dxVar=0;
				unidadeG[i].var=0;

				unidadeG[i].dPdLHidro=1;
				unidadeG[i].inddPdLHidro=-1;
				if (dutos_servico_json[indAtivo].dPdLHidro().exists() && correcao.parserieHidro>0){
					unidadeG[i].inddPdLHidro=dutos_servico_json[indAtivo].dPdLHidro();
					unidadeG[i].dPdLHidro = correcao.dPdLHidro[unidadeG[i].inddPdLHidro];
				}
				unidadeG[i].dPdLFric=1;
				unidadeG[i].inddPdLFric=-1;
				if (dutos_servico_json[indAtivo].dPdLFric().exists() && correcao.parserieFric>0){
					unidadeG[i].inddPdLFric=dutos_servico_json[indAtivo].dPdLFric();
					unidadeG[i].dPdLFric = correcao.dPdLFric[unidadeG[i].inddPdLFric];
				}
				unidadeG[i].dTdL=1;
				unidadeG[i].inddTdL=-1;
				if (dutos_servico_json[indAtivo].dTdL().exists() &&  correcao.parserieDT>0){
					unidadeG[i].inddTdL=dutos_servico_json[indAtivo].dTdL();
					unidadeG[i].dTdL = correcao.dTdL[unidadeG[i].inddTdL];
				}
				if (dutos_servico_json[indAtivo].idFormacao().exists()
						&& (dutos_servico_json[indAtivo].idFormacao()
								!= -1)) {
					unidadeG[i].formacDuto = 1;
					unidadeG[i].lito =
							identificarFormacao(
									dutos_servico_json[indAtivo].idFormacao());

					/*int iroc;
					for(iroc=0;iroc<nform;iroc++){
						if(formacPoc[iroc].id==unidadeG[i].lito){
							unidadeG[i].lito=iroc;
							break;
						}
					}
					if(iroc==nform)
						logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/indice de rocha inexistente");*/

					// verificar a existencia do identificador da formacao
					if (unidadeG[i].lito == -1) {
						// RN-054: A chave '#/dutosServico/idFormacao' deve corresponder a uma '#/configuracaoInicial/Formacao/Propriedades/id' existente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
					if((nform==0 && unidadeG[i].lito>=0)  || unidadeG[i].lito>nform){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
				}

				// caso poço injetor e formacao desabilitada
				if (pocinjec == 1 && unidadeG[i].formacDuto == 0) {
					// habilita ambiente externo automaticamente - workaround por causa da alteracao pendente da PUC
					unidadeG[i].ambext = 1;
				} else if (dutos_servico_json[indAtivo].ambienteExterno().exists() &&
						(!dutos_servico_json[indAtivo].idFormacao().exists()
						|| dutos_servico_json[indAtivo].idFormacao()== -1)) {
					// parse do ambiente externo
					unidadeG[i].ambext =
							dutos_servico_json[indAtivo].ambienteExterno();
				}
				if(unidadeG[i].ambext!=0 && unidadeG[i].ambext!=1 && unidadeG[i].ambext!=2){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "ambienteExterno",
							chaveDutoIndice,
							" tipo de 'ambienteExterno' inexistente");
				}

				if (i == 0)
					unidadeG[i].Lini = 0.;
				else
					unidadeG[i].Lini = unidadeG[i - 1].Lini
							+ unidadeG[i - 1].comp;

				unidadeG[i].acopcol =
						dutos_servico_json[indAtivo].acoplamentoTermico();

				unidadeG[i].agrupaDiscre=1;
				if (dutos_servico_json[indAtivo].agrupamento().exists())
					unidadeG[i].agrupaDiscre = dutos_servico_json[indAtivo].agrupamento();

				if(unidadeG[i].agrupaDiscre==1 && modoXY==0){
					unidadeG[i].ncel = 0;
					for (int j = 0;
						j
								< (int) dutos_servico_json[indAtivo].discretizacao().size();
						j++) {
						unidadeG[i].ncel +=
							dutos_servico_json[indAtivo].discretizacao()[j].nCelulas();
					}
					// quantidade de celulas total da unidade de servico
					ncelg += unidadeG[i].ncel;
					// vetor de comprimentos de cada célula desta unidade de producao
					unidadeG[i].dx = new double[unidadeG[i].ncel];
					int ndx;
					double tempdx;
					int totaliza = 0;
					// percorre o vetor de discretizações
					for (int kontadiscre = 0;
						kontadiscre
								< (int) dutos_servico_json[indAtivo].discretizacao().size();
						kontadiscre++) {
						ndx =
							dutos_servico_json[indAtivo].discretizacao()[kontadiscre].nCelulas();
						tempdx =
							dutos_servico_json[indAtivo].discretizacao()[kontadiscre].comprimento();
						for (int j = totaliza; j < totaliza + ndx; j++)
							unidadeG[i].dx[j] = tempdx;
						totaliza += ndx;
					}
				}
                else if(modoXY==0){
                	unidadeG[i].ncel=(int) dutos_servico_json[indAtivo].dxCelula().size();
                	ncelg += unidadeG[i].ncel;
                	unidadeG[i].dx = new double[unidadeG[i].ncel];
                	for(int j=0;j<unidadeG[i].ncel;j++)
                		unidadeG[i].dx[j] =dutos_servico_json[indAtivo].dxCelula()[j];
                }
                else{
                	unidadeG[i].ncel=(int) dutos_servico_json[indAtivo].nCelulas_XY();
                	ncelg += unidadeG[i].ncel;
                	unidadeG[i].dx = new double[unidadeG[i].ncel];
                	double dxAux=lxy/unidadeG[i].ncel;
                	for(int j=0;j<unidadeG[i].ncel;j++){
                		unidadeG[i].dx[j] =dxAux;
                		int para=0;
                	}
                }


                if(unidadeG[i].ncel<=0){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'discretizacao do segmento de duto', valores <= 0",
							chaveDutoIndice, "");
                }

				// comprimento da unidade
				unidadeG[i].comp = 0.;
				for (int j = 0; j < unidadeG[i].ncel; j++)
					unidadeG[i].comp += unidadeG[i].dx[j];
				if(modoXY==1){
					if(fabsl(unidadeG[i].comp-lxy)/lxy>1e-15)	logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "modo de preenchimento X-Y",
									chaveDutoIndice,
									"'comprimento total da discretização diferente dos limites X e Y indicados"
									" '#/configuracaoInicial/modoXY' true");
				}

				// comprimento total da linha de servico
				nCompTotalUnidadesG += unidadeG[i].comp;

				// tamanho do vetor das condições iniciais
				unidadeG[i].nponts = 0;
				int npontsTeste=0;
				// validar obrigatoriedade das condições iniciais e do vetor compInter
				if (!dutos_servico_json[indAtivo].condicoesIniciais().exists()
						/*|| !dutos_servico_json[indAtivo].condicoesIniciais().compInter().exists()*/) {
					// RN-028: Chaves 'condicoesIniciais', 'condicoesIniciais/compInter' requeridas para simulacao nao permanente (#configuracaoInicial/permanente = 0) e sem descarga
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'condicoesIniciais', 'condicoesIniciais/compInter' requeridas",
							chaveDutoIndice, "");
				} else {
					if(dutos_servico_json[indAtivo].condicoesIniciais().compInter().exists())
						unidadeG[i].nponts =
							(int) dutos_servico_json[indAtivo].condicoesIniciais().compInter().size();
					else unidadeG[i].nponts =1;
					if(unidadeG[i].nponts==1)npontsTeste=2;
					else npontsTeste=unidadeG[i].nponts;
					unidadeG[i].dxVar = new double[npontsTeste];
					unidadeG[i].var = new double*[9];
					for (int j = 0; j < 9; j++)
						unidadeG[i].var[j] =
								new double[npontsTeste];
				}

				// caso não permanente e sem descarga
				if ((perm == 0 && descarga == 0)
						|| ((*vg1dSP).chaverede == 1 /*&& redeperm==0*/)) {
					// caso uma das chaves "temp", "pressao", "vazaoMassicaGas" inexista
					if (!dutos_servico_json[indAtivo].condicoesIniciais().temp().exists()
							|| !dutos_servico_json[indAtivo].condicoesIniciais().pressao().exists()
							|| !dutos_servico_json[indAtivo].condicoesIniciais().vazaoMassicaGas().exists()) {
						// RN-029: Chaves "temp", "pressao", "vazaoMassicaGas" requeridas para simulacao nao permanente (#configuracaoInicial/permanente = 0)
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'temp', 'pressao', 'vazaoMassicaGas' requeridas",
								chaveDutoIndice,
								"#/configuracaoInicial/permanente");
					} else if (((int) dutos_servico_json[indAtivo].condicoesIniciais().temp().size()
							!= unidadeG[i].nponts)
							|| ((int) dutos_servico_json[indAtivo].condicoesIniciais().pressao().size()
									!= unidadeG[i].nponts)
							|| ((int) dutos_servico_json[indAtivo].condicoesIniciais().vazaoMassicaGas().size()
									!= unidadeG[i].nponts)) {
						// RN-068: Tamanhos dos arrays 'temp', 'pressao', 'vazaoMassicaGas' desiguais para simulacao nao permanente nem de descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'temp', 'pressao', 'vazaoMassicaGas' com tamanhos menores que o de compInter",
								chaveDutoIndice,
								"#/configuracaoInicial/condicaoInicial=1 ou 3");
					}
				}

				// caso a chave "tempExterna" inexista
				if (!dutos_servico_json[indAtivo].condicoesIniciais().tempExterna().exists()) {
					// RN-026: Chave "tempExterna" requerida
					// para duto sem acoplamento termico ou sem descarga
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"tempExterna requerida", chaveDutoIndice, "");
				} else if ((int) dutos_servico_json[indAtivo].condicoesIniciais().tempExterna().size()
						!= unidadeG[i].nponts) {
					// RN-024: Tamanhos do array "tempExterna" diferente do array "compInter"
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'compInter', 'tempExterna' com tamanhos desiguais entre si",
							chaveDutoIndice, "condicoesIniciais/tempExterna");
				}

				if (unidadeG[i].formacDuto == 0) {
					// caso a chave "velExterna" inexista
					if (!dutos_servico_json[indAtivo].condicoesIniciais().velExterna().exists()) {
						// RN-026: Chave "velExterna" requerida
						// para duto sem acoplamento termico, sem formacao e sem ambiente externo  (#dutosProducao/i/ambienteExterno = 0)
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"velExterna requerida", chaveDutoIndice,
								"formacao desabilitada");
						// caso formacao habilitada
						// ou duto sem acoplamento termico, sem formacao e sem ambiente externo
					} else if ((int) dutos_servico_json[indAtivo].condicoesIniciais().velExterna().size()
							!= unidadeG[i].nponts) {
						// RN-057: Tamanho do array "velExterna" diferente do array "compInter"
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'velExterna' com tamanhos desiguais entre si",
								chaveDutoIndice,
								"condicoesIniciais/velExterna");
					}
					if (unidadeG[i].ambext == 0) {
						// caso chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" inexistam
						if (!dutos_servico_json[indAtivo].condicoesIniciais().kExterna().exists()
								|| !dutos_servico_json[indAtivo].condicoesIniciais().calorEspecificoExterno().exists()
								|| !dutos_servico_json[indAtivo].condicoesIniciais().rhoExterno().exists()
								|| !dutos_servico_json[indAtivo].condicoesIniciais().viscExterna().exists()) {
							// RN-026: Chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" requeridas
							// para duto sem acoplamento termico e sem ambiente externo (#dutosServico/i/ambienteExterno = 0)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"kExterna, calorEspecificoExterno, rhoExterno, viscExterna requeridas",
									chaveDutoIndice,
									"formacao e ambienteExterno desabilitados");
							// caso duto sem acoplamento termico, sem formacao e sem ambiente externo
						} else if ((int) dutos_servico_json[indAtivo].condicoesIniciais().kExterna().size()
								!= unidadeG[i].nponts
								|| (int) dutos_servico_json[indAtivo].condicoesIniciais().calorEspecificoExterno().size()
										!= unidadeG[i].nponts
								|| (int) dutos_servico_json[indAtivo].condicoesIniciais().rhoExterno().size()
										!= unidadeG[i].nponts
								|| (int) dutos_servico_json[indAtivo].condicoesIniciais().viscExterna().size()
										!= unidadeG[i].nponts) {
							// RN-027: Tamanhos dos arrays "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" desiguais para duto sem formacao (#dutosProducao/i/formacao = false)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"'compInter', 'kExterna', 'calorEspecificoExterno', 'rhoExterno', 'viscExterna' com tamanhos desiguais entre si",
									chaveDutoIndice,
									"formacao e ambienteExterno");
						}
					}
				}

				// caso exista uma condicao de falha da aplicacao ate esta etapa
					if (!logger.getStResultadoSimulacao().sucesso) {
						// gerar o arquivo de saida da simulacao e encerra a aplicacao
						logger.write_logs_and_exit();
					}

				// percorrer vetor de pontos no comprimento do duto para interpolacao
				for (int j = 0; j < unidadeG[i].nponts; j++) {
					if( unidadeG[i].nponts==npontsTeste){
						if(unidadeG[i].nponts!=2){
						unidadeG[i].dxVar[j] =
							dutos_servico_json[indAtivo].condicoesIniciais().compInter()[j];
						}
						else{
							unidadeG[i].dxVar[0] =0.;
							unidadeG[i].dxVar[1] =1.;
						}
					}
					if ((perm == 0 && descarga == 0)
							|| ((*vg1dSP).chaverede == 1 /*&& redeperm==0*/)) { // alteracao2

						if(dutos_servico_json[indAtivo].condicoesIniciais().temp().size()>j)
						unidadeG[i].var[0][j] =
								dutos_servico_json[indAtivo].condicoesIniciais().temp()[j];
						else unidadeG[i].var[0][j] = unidadeG[i].var[0][j-1];

						if(dutos_servico_json[indAtivo].condicoesIniciais().pressao().size()>j)
						unidadeG[i].var[1][j] =
								dutos_servico_json[indAtivo].condicoesIniciais().pressao()[j];
						else unidadeG[i].var[1][j] = unidadeG[i].var[1][j-1];

						if(dutos_servico_json[indAtivo].condicoesIniciais().vazaoMassicaGas().size()>j)
						unidadeG[i].var[2][j] =
								dutos_servico_json[indAtivo].condicoesIniciais().vazaoMassicaGas()[j];
						else unidadeG[i].var[2][j] = unidadeG[i].var[2][j-1];

					} else {
						unidadeG[i].var[0][j] = 10;
						unidadeG[i].var[1][j] = 10;
						unidadeG[i].var[2][j] = 0.;
					}
					// caso sem acoplamento
					if(dutos_servico_json[indAtivo].condicoesIniciais().tempExterna().size()>j)
					unidadeG[i].var[3][j] =
							dutos_servico_json[indAtivo].condicoesIniciais().tempExterna()[j];
					else unidadeG[i].var[3][j] = unidadeG[i].var[3][j-1];

					if (unidadeG[i].formacDuto
							== 0 /*&& unidadeG[i].ambext==0*/) {  // alteracao0

						if(dutos_servico_json[indAtivo].condicoesIniciais().velExterna().size()>j)
						unidadeG[i].var[4][j] =
								dutos_servico_json[indAtivo].condicoesIniciais().velExterna()[j];
						else unidadeG[i].var[4][j] = unidadeG[i].var[4][j-1];


						if (unidadeG[i].ambext == 0) {

							if(dutos_servico_json[indAtivo].condicoesIniciais().kExterna().size()>j)
							unidadeG[i].var[5][j] =
									dutos_servico_json[indAtivo].condicoesIniciais().kExterna()[j];
							else unidadeG[i].var[5][j] = unidadeG[i].var[5][j-1];

							if(dutos_servico_json[indAtivo].condicoesIniciais().calorEspecificoExterno().size()>j)
							unidadeG[i].var[6][j] =
									dutos_servico_json[indAtivo].condicoesIniciais().calorEspecificoExterno()[j];
							else unidadeG[i].var[6][j] = unidadeG[i].var[6][j-1];

							if(dutos_servico_json[indAtivo].condicoesIniciais().rhoExterno().size()>j)
							unidadeG[i].var[7][j] =
									dutos_servico_json[indAtivo].condicoesIniciais().rhoExterno()[j];
							else unidadeG[i].var[7][j] = unidadeG[i].var[7][j-1];

							if(dutos_servico_json[indAtivo].condicoesIniciais().viscExterna().size()>j)
							unidadeG[i].var[8][j] =
									dutos_servico_json[indAtivo].condicoesIniciais().viscExterna()[j];
							else unidadeG[i].var[8][j] = unidadeG[i].var[8][j-1];

						} else {
							unidadeG[i].var[5][j] = 0.;
							unidadeG[i].var[6][j] = 0.;
							unidadeG[i].var[7][j] = 0.;
							unidadeG[i].var[8][j] = 0.;
						}
					} else {
						unidadeG[i].var[4][j] = 0.;
						unidadeG[i].var[5][j] = 0.;
						unidadeG[i].var[6][j] = 0.;
						unidadeG[i].var[7][j] = 0.;
						unidadeG[i].var[8][j] = 0.;
					}
				}
				if(unidadeG[i].nponts!=npontsTeste){
						unidadeG[i].dxVar[0] = 0.0;
						unidadeG[i].dxVar[1] = 1.0;
						for (int k=0; k<9; k++){
							if(i>0){
								unidadeG[i].var[k][1] = unidadeG[i].var[k][0];
								unidadeG[i].var[k][0] = unidadeG[i-1].var[k][1];
							}
							else{
								unidadeG[i].var[k][1] = unidadeG[i].var[k][0];
							}
						}
						unidadeG[i].nponts++;
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-013: Unicidade da chave 'id' em '#/dutosServico'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_unidades_servicoAmb(
		JSON_entrada_dutosServico& dutos_servico_json) {

	/*
	 "dutosServico": [{
	 "ativo": false, "id": 0, "idCorte": 1, "angulo": 0.9987, "rotulo": "riser2", "direcaoConveccao": 0, "acoplamentoTermico": 0,
	 "discretizacao": [{ "nCelulas": 50, "comprimento": 40.1931 }],
	 "condicoesIniciais": { "compInter": [0, 1], "temp": [4.0, 4.0], "pressao": [90, 25], "holdup": [0.0, 0.0], "bet": [0, 0], "uls": [0, 0], "ugs": [0, 0],
	 "tempExterna": [4.0, 25.0], "velExterna": [0.4, 0.4], "kExterna": [0.603, 0.603],
	 "calorEspecificoExterno": [4182, 4182], "rhoExterno": [1000.0, 1000.0], "viscExterna": [0.001, 0.001] }
	 }]
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/dutosServico/");
	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;

	try {
		if (nunidadeg > 0) {
			unidadeG = new detalhaG[nunidadeg];
			ncelg = 0;
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[nunidadeg];
			identificadores.resize(nunidadeg);
			// loop para carga do vetor de unidades de servico do JSON
			int indAtivo = -1;
			for (int i = 0; i < nunidadeg; i++) {
				// enquanto a propriedade "ativo" do duto de servico esteja desabilitada, avança
				while (!is_ativo(dutos_servico_json[++indAtivo]))
					;

				// criar variavel para o nome da propriedade json em processo de parse
				string chaveDutoIndice(chaveJson + to_string(indAtivo));

				// obter maior identificador
				identificadores[i] =
						dutos_servico_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				// de-para do elemento "dutosServico" do json para o struct detduto
				double lxy;
				if(modoXY==0){
					duto[i + nunidadep].ang =
							dutos_servico_json[indAtivo].angulo();
				}
				else{
					double x1=dutos_servico_json[indAtivo].xCoor();
					double y1=dutos_servico_json[indAtivo].yCoor();

					if(fabsl(x1-xProd0)>1e-15){
						double tang=(y1-yProd0)/(x1-xProd0);
						duto[i + nunidadep].ang =atan(tang);
					}
					else{

						if(y1>yProd0)duto[i + nunidadep].ang=M_PI/2.;
						else duto[i + nunidadep].ang=-M_PI/2.;
					}

					//double tang=(y1-yServ0)/(x1-xServ0);
					//duto[i + nunidadep].ang =atan(tang);
					lxy=sqrtl((x1-xServ0)*(x1-xServ0)+(y1-yServ0)*(y1-yServ0));
					xServ0=x1;
					yServ0=y1;
				}
				duto[i + nunidadep].servico = 1;
				duto[i + nunidadep].indcorte = identificarCorte(
						dutos_servico_json[indAtivo].idCorte());

				// verificar a existencia do identificador da secao transversal
				if (duto[i + nunidadep].indcorte == -1) {
					// RN-023: A chave '#/dutosServico/idCorte' deve corresponder a uma '#/secaoTransversal/id' existente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idCorte",
							chaveDutoIndice,
							"'idCorte' deve corresponder a um '#/secaoTransversal/id' existente");
				}

				// de-para do elemento "dutosServico" do json para o struct detalhaG
				unidadeG[i].ind = i;
				unidadeG[i].duto = i + nunidadep;
				unidadeG[i].formacDuto = 0;
				unidadeG[i].ambext = 0;
				unidadeG[i].lito = 0;  // 03-04-2018

				unidadeG[i].dx=0;
				unidadeG[i].dxVar=0;
				unidadeG[i].var=0;

				unidadeG[i].dPdLHidro=1;
				unidadeG[i].inddPdLHidro=-1;
				if (dutos_servico_json[indAtivo].dPdLHidro().exists() && correcao.parserieHidro>0){
					unidadeG[i].inddPdLHidro=dutos_servico_json[indAtivo].dPdLHidro();
					unidadeG[i].dPdLHidro = correcao.dPdLHidro[unidadeG[i].inddPdLHidro];
				}
				unidadeG[i].dPdLFric=1;
				unidadeG[i].inddPdLFric=-1;
				if (dutos_servico_json[indAtivo].dPdLFric().exists() && correcao.parserieFric>0){
					unidadeG[i].inddPdLFric=dutos_servico_json[indAtivo].dPdLFric();
					unidadeG[i].dPdLFric = correcao.dPdLFric[unidadeG[i].inddPdLFric];
				}
				unidadeG[i].dTdL=1;
				unidadeG[i].inddTdL=-1;
				if (dutos_servico_json[indAtivo].dTdL().exists() &&  correcao.parserieDT>0){
					unidadeG[i].inddTdL=dutos_servico_json[indAtivo].dTdL();
					unidadeG[i].dTdL = correcao.dTdL[unidadeG[i].inddTdL];
				}
				if (dutos_servico_json[indAtivo].idFormacao().exists()
						&& (dutos_servico_json[indAtivo].idFormacao()
								!= -1)) {
					unidadeG[i].formacDuto = 1;
					unidadeG[i].lito =
							identificarFormacao(
									dutos_servico_json[indAtivo].idFormacao());

					/*int iroc;
					for(iroc=0;iroc<nform;iroc++){
						if(formacPoc[iroc].id==unidadeG[i].lito){
							unidadeG[i].lito=iroc;
							break;
						}
					}
					if(iroc==nform)
						logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/indice de rocha inexistente");*/

					// verificar a existencia do identificador da formacao
					if (unidadeG[i].lito == -1) {
						// RN-054: A chave '#/dutosServico/idFormacao' deve corresponder a uma '#/configuracaoInicial/Formacao/Propriedades/id' existente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
					if((nform==0 && unidadeG[i].lito>=0)  || unidadeG[i].lito>nform){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "idFormacao",
								chaveDutoIndice,
								"'idFormacao' deve corresponder a um '#/configuracaoInicial/Formacao/Propriedades/id' existente");
					}
				}

				// caso poço injetor e formacao desabilitada
				if (pocinjec == 1 && unidadeG[i].formacDuto == 0) {
					// habilita ambiente externo automaticamente - workaround por causa da alteracao pendente da PUC
					unidadeG[i].ambext = 1;
				} else if (dutos_servico_json[indAtivo].ambienteExterno().exists() &&
						(!dutos_servico_json[indAtivo].idFormacao().exists()
						|| dutos_servico_json[indAtivo].idFormacao()== -1)) {
					// parse do ambiente externo
					unidadeG[i].ambext =
							dutos_servico_json[indAtivo].ambienteExterno();
				}
				if(unidadeG[i].ambext!=0 && unidadeG[i].ambext!=1 && unidadeG[i].ambext!=2){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "ambienteExterno",
							chaveDutoIndice,
							" tipo de 'ambienteExterno' inexistente");
				}

				if (i == 0)
					unidadeG[i].Lini = 0.;
				else
					unidadeG[i].Lini = unidadeG[i - 1].Lini
							+ unidadeG[i - 1].comp;

				unidadeG[i].acopcol =
						dutos_servico_json[indAtivo].acoplamentoTermico();

				unidadeG[i].agrupaDiscre=1;
				if (dutos_servico_json[indAtivo].agrupamento().exists())
					unidadeG[i].agrupaDiscre = dutos_servico_json[indAtivo].agrupamento();

				if(unidadeG[i].agrupaDiscre==1 && modoXY==0){
					unidadeG[i].ncel = 0;
					for (int j = 0;
						j
								< (int) dutos_servico_json[indAtivo].discretizacao().size();
						j++) {
						unidadeG[i].ncel +=
							dutos_servico_json[indAtivo].discretizacao()[j].nCelulas();
					}
					// quantidade de celulas total da unidade de servico
					ncelg += unidadeG[i].ncel;
					// vetor de comprimentos de cada célula desta unidade de producao
					unidadeG[i].dx = new double[unidadeG[i].ncel];
					int ndx;
					double tempdx;
					int totaliza = 0;
					// percorre o vetor de discretizações
					for (int kontadiscre = 0;
						kontadiscre
								< (int) dutos_servico_json[indAtivo].discretizacao().size();
						kontadiscre++) {
						ndx =
							dutos_servico_json[indAtivo].discretizacao()[kontadiscre].nCelulas();
						tempdx =
							dutos_servico_json[indAtivo].discretizacao()[kontadiscre].comprimento();
						for (int j = totaliza; j < totaliza + ndx; j++)
							unidadeG[i].dx[j] = tempdx;
						totaliza += ndx;
					}
				}
                else if(modoXY==0){
                	unidadeG[i].ncel=(int) dutos_servico_json[indAtivo].dxCelula().size();
                	ncelg += unidadeG[i].ncel;
                	unidadeG[i].dx = new double[unidadeG[i].ncel];
                	for(int j=0;j<unidadeG[i].ncel;j++)
                		unidadeG[i].dx[j] =dutos_servico_json[indAtivo].dxCelula()[j];
                }
                else{
                	unidadeG[i].ncel=(int) dutos_servico_json[indAtivo].nCelulas_XY();
                	ncelg += unidadeG[i].ncel;
                	unidadeG[i].dx = new double[unidadeG[i].ncel];
                	double dxAux=lxy/unidadeG[i].ncel;
                	for(int j=0;j<unidadeG[i].ncel;j++){
                		unidadeG[i].dx[j] =dxAux;
                		int para=0;
                	}
                }


                if(unidadeG[i].ncel<=0){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'discretizacao do segmento de duto', valores <= 0",
							chaveDutoIndice, "");
                }

				// comprimento da unidade
				unidadeG[i].comp = 0.;
				for (int j = 0; j < unidadeG[i].ncel; j++)
					unidadeG[i].comp += unidadeG[i].dx[j];
				if(modoXY==1){
					if(fabsl(unidadeG[i].comp-lxy)/lxy>1e-15)	logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "modo de preenchimento X-Y",
									chaveDutoIndice,
									"'comprimento total da discretização diferente dos limites X e Y indicados"
									" '#/configuracaoInicial/modoXY' true");
				}

				// comprimento total da linha de servico
				nCompTotalUnidadesG += unidadeG[i].comp;

				// tamanho do vetor das condições iniciais
				unidadeG[i].nponts = 0;
				int npontsTeste=0;
				// validar obrigatoriedade das condições iniciais e do vetor compInter
				if (!dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().exists()
						/*|| !dutos_servico_json[indAtivo].condicoesIniciais().compInter().exists()*/) {
					// RN-028: Chaves 'condicoesIniciais', 'condicoesIniciais/compInter' requeridas para simulacao nao permanente (#configuracaoInicial/permanente = 0) e sem descarga
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'condicoesIniciais', 'condicoesIniciais/compInter' requeridas",
							chaveDutoIndice, "");
				} else {
					if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().compInter().exists())
						unidadeG[i].nponts =
							(int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().compInter().size();
					else unidadeG[i].nponts =1;
					if(unidadeG[i].nponts==1)npontsTeste=2;
					else npontsTeste=unidadeG[i].nponts;
					unidadeG[i].dxVar = new double[npontsTeste];
					unidadeG[i].var = new double*[9];
					for (int j = 0; j < 9; j++)
						unidadeG[i].var[j] =
								new double[npontsTeste];
				}

				// caso não permanente e sem descarga
				if ((perm == 0 && descarga == 0)
						|| ((*vg1dSP).chaverede == 1 /*&& redeperm==0*/)) {
					// caso uma das chaves "temp", "pressao", "vazaoMassicaGas" inexista
					if (!dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().temp().exists()
							|| !dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().pressao().exists()
							|| !dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().vazaoMassicaGas().exists()) {
						// RN-029: Chaves "temp", "pressao", "vazaoMassicaGas" requeridas para simulacao nao permanente (#configuracaoInicial/permanente = 0)
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'temp', 'pressao', 'vazaoMassicaGas' requeridas",
								chaveDutoIndice,
								"#/configuracaoInicial/permanente");
					} else if (((int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().temp().size()
							!= unidadeG[i].nponts)
							|| ((int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().pressao().size()
									!= unidadeG[i].nponts)
							|| ((int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().vazaoMassicaGas().size()
									!= unidadeG[i].nponts)) {
						// RN-068: Tamanhos dos arrays 'temp', 'pressao', 'vazaoMassicaGas' desiguais para simulacao nao permanente nem de descarga
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'temp', 'pressao', 'vazaoMassicaGas' com tamanhos menores que o de compInter",
								chaveDutoIndice,
								"#/configuracaoInicial/condicaoInicial=1 ou 3");
					}
				}

				// caso a chave "tempExterna" inexista
				if (!dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna().exists()) {
					// RN-026: Chave "tempExterna" requerida
					// para duto sem acoplamento termico ou sem descarga
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"tempExterna requerida", chaveDutoIndice, "");
				} else if ((int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna().size()
						!= unidadeG[i].nponts) {
					// RN-024: Tamanhos do array "tempExterna" diferente do array "compInter"
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"'compInter', 'tempExterna' com tamanhos desiguais entre si",
							chaveDutoIndice, "condicoesIniciais/tempExterna");
				}

				if (unidadeG[i].formacDuto == 0) {
					// caso a chave "velExterna" inexista
					if (!dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().velExterna().exists()) {
						// RN-026: Chave "velExterna" requerida
						// para duto sem acoplamento termico, sem formacao e sem ambiente externo  (#dutosProducao/i/ambienteExterno = 0)
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"velExterna requerida", chaveDutoIndice,
								"formacao desabilitada");
						// caso formacao habilitada
						// ou duto sem acoplamento termico, sem formacao e sem ambiente externo
					} else if ((int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().velExterna().size()
							!= unidadeG[i].nponts) {
						// RN-057: Tamanho do array "velExterna" diferente do array "compInter"
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"'compInter', 'velExterna' com tamanhos desiguais entre si",
								chaveDutoIndice,
								"condicoesIniciais/velExterna");
					}
					if (unidadeG[i].ambext == 0) {
						// caso chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" inexistam
						if (!dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().kExterna().exists()
								|| !dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno().exists()
								|| !dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno().exists()
								|| !dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna().exists()) {
							// RN-026: Chaves "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" requeridas
							// para duto sem acoplamento termico e sem ambiente externo (#dutosServico/i/ambienteExterno = 0)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"kExterna, calorEspecificoExterno, rhoExterno, viscExterna requeridas",
									chaveDutoIndice,
									"formacao e ambienteExterno desabilitados");
							// caso duto sem acoplamento termico, sem formacao e sem ambiente externo
						} else if ((int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().kExterna().size()
								!= unidadeG[i].nponts
								|| (int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno().size()
										!= unidadeG[i].nponts
								|| (int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno().size()
										!= unidadeG[i].nponts
								|| (int) dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna().size()
										!= unidadeG[i].nponts) {
							// RN-027: Tamanhos dos arrays "kExterna", "calorEspecificoExterno", "rhoExterno", "viscExterna" desiguais para duto sem formacao (#dutosProducao/i/formacao = false)
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"'compInter', 'kExterna', 'calorEspecificoExterno', 'rhoExterno', 'viscExterna' com tamanhos desiguais entre si",
									chaveDutoIndice,
									"formacao e ambienteExterno");
						}
					}
				}

				// caso exista uma condicao de falha da aplicacao ate esta etapa
					if (!logger.getStResultadoSimulacao().sucesso) {
						// gerar o arquivo de saida da simulacao e encerra a aplicacao
						logger.write_logs_and_exit();
					}

				// percorrer vetor de pontos no comprimento do duto para interpolacao
				for (int j = 0; j < unidadeG[i].nponts; j++) {
					if( unidadeG[i].nponts==npontsTeste){
						if(unidadeG[i].nponts!=2){
						unidadeG[i].dxVar[j] =
							dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().compInter()[j];
						}
						else{
							unidadeG[i].dxVar[0] =0.;
							unidadeG[i].dxVar[1] =1.;
						}
					}
					if ((perm == 0 && descarga == 0)
							|| ((*vg1dSP).chaverede == 1 /*&& redeperm==0*/)) { // alteracao2

						if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().temp().size()>j)
						unidadeG[i].var[0][j] =
								dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().temp()[j];
						else unidadeG[i].var[0][j] = unidadeG[i].var[0][j-1];

						if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().pressao().size()>j)
						unidadeG[i].var[1][j] =
								dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().pressao()[j];
						else unidadeG[i].var[1][j] = unidadeG[i].var[1][j-1];

						if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().vazaoMassicaGas().size()>j)
						unidadeG[i].var[2][j] =
								dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().vazaoMassicaGas()[j];
						else unidadeG[i].var[2][j] = unidadeG[i].var[2][j-1];

					} else {
						unidadeG[i].var[0][j] = 10;
						unidadeG[i].var[1][j] = 10;
						unidadeG[i].var[2][j] = 0.;
					}
					// caso sem acoplamento
					if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna().size()>j)
					unidadeG[i].var[3][j] =
							dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().tempExterna()[j];
					else unidadeG[i].var[3][j] = unidadeG[i].var[3][j-1];


					if (unidadeG[i].formacDuto
							== 0 /*&& unidadeG[i].ambext==0*/) {  // alteracao0

						if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().velExterna().size()>j)
						unidadeG[i].var[4][j] =
								dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().velExterna()[j];
						else unidadeG[i].var[4][j] = unidadeG[i].var[4][j-1];


						if (unidadeG[i].ambext == 0) {

							if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().kExterna().size()>j)
							unidadeG[i].var[5][j] =
									dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().kExterna()[j];
							else unidadeG[i].var[5][j] = unidadeG[i].var[5][j-1];

							if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno().size()>j)
							unidadeG[i].var[6][j] =
									dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().calorEspecificoExterno()[j];
							else unidadeG[i].var[6][j] = unidadeG[i].var[6][j-1];

							if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno().size()>j)
							unidadeG[i].var[7][j] =
									dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().rhoExterno()[j];
							else unidadeG[i].var[7][j] = unidadeG[i].var[7][j-1];

							if(dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna().size()>j)
							unidadeG[i].var[8][j] =
									dutos_servico_json[indAtivo].condicoesIniciaisEAmbiente().viscExterna()[j];
							else unidadeG[i].var[8][j] = unidadeG[i].var[8][j-1];

						} else {
							unidadeG[i].var[5][j] = 0.;
							unidadeG[i].var[6][j] = 0.;
							unidadeG[i].var[7][j] = 0.;
							unidadeG[i].var[8][j] = 0.;
						}
					} else {
						unidadeG[i].var[4][j] = 0.;
						unidadeG[i].var[5][j] = 0.;
						unidadeG[i].var[6][j] = 0.;
						unidadeG[i].var[7][j] = 0.;
						unidadeG[i].var[8][j] = 0.;
					}
				}
				if(unidadeG[i].nponts!=npontsTeste){
						unidadeG[i].dxVar[0] = 0.0;
						unidadeG[i].dxVar[1] = 1.0;
						for (int k=0; k<9; k++){
							if(i>0){
								unidadeG[i].var[k][1] = unidadeG[i].var[k][0];
								unidadeG[i].var[k][0] = unidadeG[i-1].var[k][1];
							}
							else{
								unidadeG[i].var[k][1] = unidadeG[i].var[k][0];
							}
						}
						unidadeG[i].nponts++;
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-013: Unicidade da chave 'id' em '#/dutosServico'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_hidrato(JSON_entrada_hidrato& hidrato_json/*, */) {

	existeEnvelope=0;
	string chaveJson("#/hidrato");

	std::vector<std::pair<double, double>> envelopeHidraTemp;

	try {

		string chaveEnvelopeIndice(chaveJson);

		calculaEnvelope =0;
		if(hidrato_json.calculoInterno().exists())calculaEnvelope = hidrato_json.calculoInterno();

		if(calculaEnvelope==0){
			if(!hidrato_json.pressao().exists()){
				logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Pressao no envelope inexistente em ",
						chaveEnvelopeIndice,
						"Envelope de Hidrato");
			}
			else if(!hidrato_json.temperatura().exists()){
				logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Temperatura no envelope inexistente em ",
						chaveEnvelopeIndice,
						"Envelope de Hidrato");
			}
			else{
				nparEnvelope = hidrato_json.pressao().size();
				if(nparEnvelope!=hidrato_json.temperatura().size())logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Dimensqao do vetor de pressao diferemnte da dimensao do vetor temperatura ",
								chaveEnvelopeIndice,
								"Envelope de Hidrato");
			}
			if(nparEnvelope>0 || calculaEnvelope==1)existeEnvelope=1;


			for(int i=0;i<nparEnvelope;i++) {
				double pres=hidrato_json.pressao()[i];
				double temp=hidrato_json.temperatura()[i];
				envelopeHidraTemp.emplace_back(pres, temp);
			}
			if(nparEnvelope>0){
				std::sort(envelopeHidraTemp.begin(), envelopeHidraTemp.end());  // Ordena por coluna
				envelopeHidra = new double* [nparEnvelope];
				for(int i=0;i<nparEnvelope;i++) {
					envelopeHidra[i] = new double [2];
					envelopeHidra[i][0]=envelopeHidraTemp[i].first;
					envelopeHidra[i][1]=envelopeHidraTemp[i].second;
				}
			}
		}
		else{
		    //solverHidrato - chris

			if (hidrato_json.Hammerschmidt().exists()) {
				// carregar os parâmetros da chave
				/*Khamm =
						hidrato_json.Hammerschmidt().Khamm();*/ //chris - Hidratos
				MMH =
						hidrato_json.Hammerschmidt().MMH();
				MMG =
						hidrato_json.Hammerschmidt().MMG();
				MMW =
						hidrato_json.Hammerschmidt().MMW();
				Whamm =
						hidrato_json.Hammerschmidt().Whamm();
				/*Methanol =
						hidrato_json.Hammerschmidt().Methanol();
				MMEG =
						hidrato_json.Hammerschmidt().MMEG();*/ //chris - Hidratos
				inibidor =
						hidrato_json.Hammerschmidt().inibidor();
				fracFWcarregada =
						hidrato_json.Hammerschmidt().fracFWcarregada(); //chris - Hidratos

			} else {

				logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Objeto Hammerschmidt requerido", chaveJson,
						"solverHidrato=true");
			}

			if (hidrato_json.PropFluHidrato().exists()) {
				// carregar os parâmetros da chave

				/*rhoH =
						hidrato_json.PropFluHidrato().rhoH();*/ //chris - Hidratos
				coefEsteq =
						hidrato_json.PropFluHidrato().coefEsteq();
			} else {

				logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Objeto PropFluHidrato requerido", chaveJson,
						"solverHidrato=true");
			}


			if (hidrato_json.ModeloTurner().exists()) {
				// carregar os parâmetros da chave

				/*estruturaHidratosIk1 =
						hidrato_json.ModeloTurner().estruturaHidratosIk1();
				estruturaHidratosIk2 =
						hidrato_json.ModeloTurner().estruturaHidratosIk2();
				estruturaHidratosIIk1 =
						hidrato_json.ModeloTurner().estruturaHidratosIIk1();
				estruturaHidratosIIk2 =
						hidrato_json.ModeloTurner().estruturaHidratosIIk2();*/
				estruturaHidratos = //chris - Hidratos
						hidrato_json.ModeloTurner().estruturaHidratos();
				rd =
						hidrato_json.ModeloTurner().rd();
				rp =
						hidrato_json.ModeloTurner().rp();

			} else {

				logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Objeto ModeloTurner requerido", chaveJson,
						"solverHidrato=true");
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}


}

/*!
 * Converter o elemento "ipr" do arquivo Json do MRT em struct ipr.
 *
 * \tparam Objeto JSON correspondente a dutos de servico
 */
void Ler::parse_ipr(JSON_entrada_ipr& ipr_json/*, */) {
	/*
	 "ipr": [ {
	 "id": 0,
	 "pressaoEstatica": [187.0], "tempoPressaoEstatica": [0.0], "temperaturas": [83.0], "tempoTemperaturas": [0.0], "ip": [0.0], "tempoip": [0.0], "ii": [0.0], "tempoii": [0.0],
	 "indFluidoPro": 0, "comprimentoMedido": 0.0,
	 "ativo": true
	 } ]
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/ipr");
	try {
		// tamanho do vetor ipr
		nipr = 0;
		// percorre o array de IPRs
		for (size_t i = 0; i < ipr_json.size(); i++) {
			// caso o IPR esteja ativo
			if (is_ativo(ipr_json[i]))
				nipr++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (nipr > 0) {
			// declarar o vetor ipr
			IPRS = new detIPR[nipr];
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[nipr];
			identificadores.resize(nipr);
			// loop para parse das estruturas do ipr
			int indAtivo = -1;
			for (int i = 0; i < nipr; i++) {
				// enquanto a propriedade "ativo" do ipr esteja desabilitada, avança
				while (!is_ativo(ipr_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] = ipr_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				IPRS[i].comp =
						ipr_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				/*if ((origemGeometria == origemGeometria_t::plataforma
						&& sistemaSimulacao == sistemaSimulacao_t::multifasico)
						|| (origemGeometria == origemGeometria_t::poco
								&& sistemaSimulacao
										== sistemaSimulacao_t::injetor))*/
				if(!sentidoGeometriaSegueEscoamento)
					IPRS[i].comp = nCompTotalUnidadesP - IPRS[i].comp; //04-04-2018
				if (IPRS[i].comp < 0)
					IPRS[i].comp = 0.0;
				double lverif = IPRS[i].comp;
				IPRS[i].indcel = buscaIndiceMeioP(lverif);
				// parse do vetor de pressões estáticas
				IPRS[i].tipoIPR=1;
				if (ipr_json[indAtivo].tipoIPR().exists())IPRS[i].tipoIPR=ipr_json[indAtivo].tipoIPR();
				IPRS[i].seriep =
						(int) ipr_json[indAtivo].pressaoEstatica().size();
				if(IPRS[i].seriep>0){
					IPRS[i].pres = new double[IPRS[i].seriep];
					IPRS[i].tpres = new double[IPRS[i].seriep];
				}
				else{
					IPRS[i].pres = 0;
					IPRS[i].tpres = 0;
				}
				// caso os tamanhos dos vetores das chaves difiram entre si
				if (ipr_json[indAtivo].pressaoEstatica().size()
						!= ipr_json[indAtivo].tempoPressaoEstatica().size()) {
					// RN-070: chaves pressaoEstatica e tempoPressaoEstatica com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/ipr/pressaoEstatica e #/ipr/tempoPressaoEstatica com tamanhos diferentes");
				} else {
					// loop para parse do vetor de pressões estáticas
					for (int j = 0; j < IPRS[i].seriep; j++) {
						IPRS[i].pres[j] =
								ipr_json[indAtivo].pressaoEstatica()[j];
						IPRS[i].tpres[j] =
								ipr_json[indAtivo].tempoPressaoEstatica()[j];
					}
				}
				// parse do vetor de temperaturas
				IPRS[i].seriet =
						(int) ipr_json[indAtivo].temperaturas().size();
				if(IPRS[i].seriet>0){
					IPRS[i].temp = new double[IPRS[i].seriet];
					IPRS[i].ttemp = new double[IPRS[i].seriet];
				}
				else{
					IPRS[i].temp = 0;
					IPRS[i].ttemp = 0;
				}
				// caso os tamanhos dos vetores das chaves difiram entre si
				if (ipr_json[indAtivo].temperaturas().size()
						!= ipr_json[indAtivo].tempoTemperaturas().size()) {
					// RN-071: chaves temperaturas e tempoTemperaturas com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/ipr/temperaturas e #/ipr/tempoTemperaturas com tamanhos diferentes");
				} else {
					// loop para parse do vetor de temperaturas
					for (int j = 0; j < IPRS[i].seriet; j++) {
						IPRS[i].temp[j] =
								ipr_json[indAtivo].temperaturas()[j];
						IPRS[i].ttemp[j] =
								ipr_json[indAtivo].tempoTemperaturas()[j];
					}
				}
				IPRS[i].serieip=0;
				IPRS[i].ip=0;
				IPRS[i].tip=0;
				IPRS[i].serieqm=0;
				IPRS[i].qMax=0;
				IPRS[i].tqMax=0;
				if(IPRS[i].tipoIPR==0 || IPRS[i].tipoIPR==1){
					// parse do vetor de ip
					IPRS[i].serieip = (int) ipr_json[indAtivo].ip().size();
					IPRS[i].ip = new double[IPRS[i].serieip];
					IPRS[i].tip = new double[IPRS[i].serieip];
					// caso os tamanhos dos vetores das chaves difiram entre si
					if (ipr_json[indAtivo].ip().size()
						!= ipr_json[indAtivo].tempoip().size()) {
						// RN-072: chaves pressaoEstatica e tempoPressaoEstatica com tamanhos diferentes
						logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/ipr/ip e #/ipr/tempoip com tamanhos diferentes");
					} else {
						// loop para parse do vetor de ip
						for (int j = 0; j < IPRS[i].serieip; j++) {
							IPRS[i].ip[j] = ipr_json[indAtivo].ip()[j];
							IPRS[i].tip[j] =
									ipr_json[indAtivo].tempoip()[j];
						}
					}
				}
				else if(IPRS[i].tipoIPR==2){
					// parse do vetor de ip
					IPRS[i].serieqm = (int) ipr_json[indAtivo].qMax().size();
					IPRS[i].qMax = new double[IPRS[i].serieqm];
					IPRS[i].tqMax = new double[IPRS[i].serieqm];
					// caso os tamanhos dos vetores das chaves difiram entre si
					if ((ipr_json[indAtivo].qMax().size()
						!= ipr_json[indAtivo].tempoqMax().size()) || ipr_json[indAtivo].qMax().size()==0) {
						// RN-072: chaves pressaoEstatica e tempoPressaoEstatica com tamanhos diferentes
						if(ipr_json[indAtivo].qMax().size()!=0){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/ipr/qMax e #/ipr/tempoqMax com tamanhos diferentes");
						}
						else{
							logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/ipr/qMax vazia");
						}
					} else {
						// loop para parse do vetor de ip
						for (int j = 0; j < IPRS[i].serieqm; j++) {
							IPRS[i].qMax[j] = ipr_json[indAtivo].qMax()[j];
							IPRS[i].tqMax[j] =
									ipr_json[indAtivo].tempoqMax()[j];
						}
					}
				}
				// parse do vetor de jp
				IPRS[i].seriejp = (int) ipr_json[indAtivo].ii().size();
				if(IPRS[i].seriejp>0){
					IPRS[i].jp = new double[IPRS[i].seriejp];
					IPRS[i].tjp = new double[IPRS[i].seriejp];
				}
				else{
					IPRS[i].jp = 0;
					IPRS[i].tjp = 0;
				}
				// caso os tamanhos dos vetores das chaves difiram entre si
				if (ipr_json[indAtivo].ii().size()
						!= ipr_json[indAtivo].tempoii().size()) {
					// RN-073: chaves ii e tempoii com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/ipr/ii e #/ipr/tempoii com tamanhos diferentes");
				} else {
					// loop para parse do vetor de jp / ii
					for (int j = 0; j < IPRS[i].seriejp; j++) {
						IPRS[i].jp[j] = ipr_json[indAtivo].ii()[j];
						IPRS[i].tjp[j] =
								ipr_json[indAtivo].tempoii()[j];
					}
				}
				// parse do valor
				if (ipr_json[indAtivo].indFluidoPro().exists())
								IPRS[i].indfluP = ipr_json[indAtivo].indFluidoPro();
				else if (ipr_json[indAtivo].indiFluidoPro().exists())
				IPRS[i].indfluP = ipr_json[indAtivo].indiFluidoPro();
				else{
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/indicador de fluido de producao inexistente");
				}

				IPRS[i].indfluP = identificarFluidoProducao(
						IPRS[i].indfluP);
				/*if (ipr_json[indAtivo].indFluidoPro().exists())
				IPRS[i].indfluP = ipr_json[indAtivo].indFluidoPro();
				else if (ipr_json[indAtivo].indiFluidoPro().exists())
				IPRS[i].indfluP = ipr_json[indAtivo].indiFluidoPro();
				else{
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/indicador de fluido de producao inexistente");
				}
				int iflu;
				for(iflu=0;iflu<nfluP;iflu++){
					if(indFlup[iflu]==IPRS[i].indfluP){
						IPRS[i].indfluP=iflu;
						break;
					}
				}
				if(iflu==nfluP)
					logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/indice de fluido de producao inexistente");*/
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-014: Unicidade da chave 'id' em '#/ipr'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "gasInj" do arquivo Json do MRT em struct gasinj.
 *
 * \tparam Objeto JSON correspondente a gasInj.
 */
void Ler::parse_gasInj(JSON_entrada_gasInj& gasInj_json) {
	/*
	 "gasInj": {
	 "ativo": false,
	 "tipoCC": 1,
	 "temperatura": [40.0,40.0,40.0,40.0,40.0,40.0,40.0],
	 "pressaoInjecao": [160.0,160.0,160.0,160.0,160.0,160.0,160.0],
	 "vazaoGas": [150000.0,150000.0,150000.0,150000.0,0.0,0.0,150000.0],
	 "tempo": [0.0,3000.0,3060.0,7000.0,7065.0,30000.0,30065.0]
	 }
	 */
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/gasInj");
	try {
		if (is_ativo(gasInj_json)) {
			if (controDesc == 0) {  //alteracao5
				gasinj.tipoCC = gasInj_json.tipoCC();
				gasinj.parserie = (int) gasInj_json.temperatura().size();
				// caso os tamanhos dos vetores das chaves difiram entre si
				if (gasInj_json.temperatura().size()
						!= gasInj_json.tempo().size()) {
					// RN-074: chaves temperatura e tempo com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/gasInj/temperatura e #/gasInj/tempo com tamanhos diferentes");
				} else {
					gasinj.temperatura = new double[gasinj.parserie];
					gasinj.tempo = new double[gasinj.parserie];
					for (int i = 0; i < gasinj.parserie; i++) {
						gasinj.temperatura[i] =
								gasInj_json.temperatura()[i];
						gasinj.tempo[i] = gasInj_json.tempo()[i];
					}
				}
				if (gasinj.tipoCC == 1) {
					// caso o elemento vazaoGas nao exista
					if (!gasInj_json.vazaoGas().exists()) {
						// RN-085: chave vazaoGas obrigatoria
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chave #/gasInj/vazaoGas requerida para tipoCC 1");
					} else {
						// caso os tamanhos dos vetores das chaves difiram entre si
						if (gasInj_json.temperatura().size()
								!= gasInj_json.vazaoGas().size()) {
							// RN-075: chaves temperatura e tempo com tamanhos diferentes
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/gasInj/temperatura e #/gasInj/vazaoGas com tamanhos diferentes");
						} else {
							gasinj.vazgas = new double[gasinj.parserie];
							for (int i = 0; i < gasinj.parserie; i++) {
								gasinj.vazgas[i] =
										gasInj_json.vazaoGas()[i];
							}
						}
					}
				} else if (gasinj.tipoCC == 0) {
					// caso o elemento pressaoInjecao nao exista
					if (!gasInj_json.pressaoInjecao().exists()) {
						// RN-086: chave pressaoInjecao obrigatoria
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chave #/gasInj/pressaoInjecao requerida para tipoCC 0");
					} else {
						// caso os tamanhos dos vetores das chaves difiram entre si
						if (gasInj_json.temperatura().size()
								!= gasInj_json.pressaoInjecao().size()) {
							// RN-076: chaves temperatura e pressaoInjecao com tamanhos diferentes
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/gasInj/temperatura e #/gasInj/pressaoInjecao com tamanhos diferentes");
						} else {
							gasinj.presinj = new double[gasinj.parserie];
							for (int i = 0; i < gasinj.parserie; i++) {
								gasinj.presinj[i] =
										gasInj_json.pressaoInjecao()[i];
							}
						}
					}
					//mudancaChute
					gasinj.chuteVaz = 0;
					gasinj.vazgas = new double[1];
					if (gasInj_json.chuteVazaoInjecao().exists()) {
						gasinj.chuteVaz =
								gasInj_json.chuteVazaoInjecao();
						if (gasinj.chuteVaz == 1
								&& gasInj_json.vazaoGas().exists()
								&& gasInj_json.vazaoGas().size() > 0) {
							gasinj.vazgas[0] =
									gasInj_json.vazaoGas()[0];
						} else if (gasinj.chuteVaz == 1) {
							// RN-100: chaveVazaoInjecao sem valor indicado
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/gasInj/chuteVazaoInjecao e #/gasInj/vazaoGas");
						}
					}
					//mudancaChute
				}
				// caso exista uma condicao de falha da aplicacao ate esta etapa
					if (!logger.getStResultadoSimulacao().sucesso) {
						// gerar o arquivo de saida da simulacao e encerra a aplicacao
						logger.write_logs_and_exit();
					}
			} else {  //alteracao5
				gasinj.tipoCC = 0;
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "fonteGasLift" do arquivo Json do MRT em struct fonte.
 *
 * \tparam Objeto JSON correspondente a fonte_gaslift.
 */
void Ler::troca_gaslift(detVALVGL& valv1,
		detVALVGL& valv2) {
	valv1.compP = valv2.compP;
	valv1.compG = valv2.compG;
	valv1.cd = valv2.cd;
	valv1.cdLiq = valv2.cdLiq;
	valv1.diagarg = valv2.diagarg;
	valv1.frec = valv2.frec;
	valv1.frecLiq = valv2.frecLiq;
	valv1.pcali = valv2.pcali;
	valv1.tcali = valv2.tcali;  //alteravalpres
	valv1.diaexter = valv2.diaexter;  //alteravalpres
	valv1.posicG = valv2.posicG;
	valv1.posicP = valv2.posicP;
	valv1.razarea = valv2.razarea;
	valv1.tipo = valv2.tipo;
}

/*!
 * Converter os elementos "fonteGasLift" do arquivo Json do MRT em struct fonte.
 *
 * \tparam Objeto JSON correspondente a fonte_gaslift.
 */
void Ler::parse_fonte_gaslift(JSON_entrada_fonteGasLift& fonte_gaslift_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fonteGasLift");
	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;

	int colEanul=1;

	try {
		// tamanho do vetor de fontes de gás lift
		nvalvgas = 0;
		// percorre o array de fontes de gás lift
		for (size_t i = 0; i < fonte_gaslift_json.size(); i++) {
			// caso a fonte de gás lift esteja ativo
			if (is_ativo(fonte_gaslift_json[i]))
				nvalvgas++;
		}
		if (nvalvgas > 0) {
			valvgl = new detVALVGL[nvalvgas];
			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nvalvgas];
			identificadores.resize(nvalvgas);

			int indAtivo = -1;
			for (int i = 0; i < nvalvgas; i++) {
				// enquanto a propriedade "ativo" da fonte de gás lift esteja desabilitada, avança
				while (!is_ativo(fonte_gaslift_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						fonte_gaslift_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				valvgl[i].compP =
						fonte_gaslift_json[indAtivo].comprimentoMedidoProducao();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					valvgl[i].compP = nCompTotalUnidadesP - valvgl[i].compP; //04-04-2018
				if (valvgl[i].compP < 0.0)
					valvgl[i].compP = 0.0;
				if(fonte_gaslift_json[indAtivo].colunaEanular().exists())colEanul=fonte_gaslift_json[indAtivo].colunaEanular();
				if(colEanul==0)
					valvgl[i].compG =
						fonte_gaslift_json[indAtivo].comprimentoMedidoServico();
				else{
					double compAcopTerm=0.;
					int posCol=0;
					while(celp[posCol].acopcol==0 && posCol<ncelp){
						compAcopTerm+=celp[posCol].dx;
						posCol++;
					}
					int calcPossivel=0;
					if(celp[posCol].acopcol==1 && posCol<ncelp){
						calcPossivel=1;
						while(celp[posCol].acopcol==1){
							compAcopTerm+=celp[posCol].dx;
							posCol++;
						}
					}
					else{
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "calculo da posicao da valvula de gas lift no anular",
								chaveJson, "sem indicador de acoplamento na coluna");
					}

					double compAcopTermAnul=0.;
					int posAnul=0;
					while(celg[posAnul].acopcol==0 && posAnul<ncelg){
						compAcopTermAnul+=celg[posAnul].dx;
						posAnul++;
					}
					if(posAnul>=ncelg){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "calculo da posicao da valvula de gas lift no anular",
								chaveJson, "sem indicador de acoplamento no anular");
					}
					valvgl[i].compG=compAcopTermAnul+compAcopTerm-valvgl[i].compP;
				}
				double lverif = valvgl[i].compP;
				valvgl[i].posicP = buscaIndiceMeioP(lverif);
				lverif = valvgl[i].compG;
				valvgl[i].posicG = buscaIndiceMeioG(lverif);

				valvgl[i].tipo =0;
				if (fonte_gaslift_json[indAtivo].tipoValvula().exists())
				valvgl[i].tipo =
						fonte_gaslift_json[indAtivo].tipoValvula();
				valvgl[i].diagarg =
						fonte_gaslift_json[indAtivo].diametroOrificio();
				valvgl[i].cd =0.9;
				if (fonte_gaslift_json[indAtivo].cdvgl().exists())
				valvgl[i].cd =
						fonte_gaslift_json[indAtivo].cdvgl();

				if (fonte_gaslift_json[indAtivo].frecupera().exists())
					valvgl[i].frec =
							fonte_gaslift_json[indAtivo].frecupera();
				else
					valvgl[i].frec = 0.;

				if (fonte_gaslift_json[indAtivo].cdvLiq().exists())
					valvgl[i].cdLiq =
							fonte_gaslift_json[indAtivo].cdvLiq();
				else
					valvgl[i].cdLiq = 0.84;

				if (fonte_gaslift_json[indAtivo].frecuperaLiq().exists())
					valvgl[i].frecLiq =
							fonte_gaslift_json[indAtivo].frecuperaLiq();
				else
					valvgl[i].frecLiq = valvgl[i].frec;

				if (valvgl[i].tipo != 1) {
					valvgl[i].razarea = 0.1;
					valvgl[i].pcali = 1.;
					valvgl[i].tcali = 1.;  //alteravalpres
					valvgl[i].diaexter = 1.;  //alteravalpres
				} else {
					valvgl[i].frec = 0;
					valvgl[i].frecLiq = 0;
					// caso a chave razaoArea nao exista
					if (!fonte_gaslift_json[indAtivo].razaoArea().exists()) {
						// RN-044: chave "razaoArea" requerida em caso de valvula do tipo 1
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "razaoArea",
								chaveJson, "tipoValvula: 1");
					} else {
						valvgl[i].razarea =
								fonte_gaslift_json[indAtivo].razaoArea();
					}
					// caso a chave "pressaoCalibracao" nao exista
					if (!fonte_gaslift_json[indAtivo].pressaoCalibracao().exists()) {
						// RN-045: chave "pressaoCalibracao" requerida em caso de valvula do tipo 1
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"pressaoCalibracao", chaveJson,
								"tipoValvula: 1");
					} else {
						valvgl[i].pcali =
								fonte_gaslift_json[indAtivo].pressaoCalibracao();
					}
					// caso a chave "temperaturaCalibracao" nao exista
					if (!fonte_gaslift_json[indAtivo].temperaturaCalibracao().exists()) {  //alteravalpres
						// RN-101: chave "temperaturaCalibracao" requerida em caso de valvula do tipo 1
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"temperaturaCalibracao", chaveJson,
								"tipoValvula: 1");
					} else {
						valvgl[i].tcali =
								fonte_gaslift_json[indAtivo].temperaturaCalibracao();
					}
					// caso a chave "diametroExterno" nao exista
					if (!fonte_gaslift_json[indAtivo].diametroExterno().exists()) {	          //alteravalpres
						// RN-102: chave "diametroExterno" requerida em caso de valvula do tipo 1
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"diametroExterno", chaveJson, "tipoValvula: 1");
					} else {
						valvgl[i].diaexter =
								fonte_gaslift_json[indAtivo].diametroExterno();
					}
				}
			}

			if (nvalvgas > 1) {
				detVALVGL temp;
				int posi = valvgl[0].posicG;
				int imin = 0;
				int itroca;
				while (imin < nvalvgas - 1) {
					int troca = 0;
					for (int i = imin + 1; i < nvalvgas; i++) {
						if (posi > valvgl[i].posicG) {
							posi = valvgl[i].posicG;
							itroca = i;
							troca = -1;
						}
					}
					if (troca == -1) {
						troca_gaslift(temp, valvgl[imin]);
						troca_gaslift(valvgl[imin], valvgl[itroca]);
						troca_gaslift(valvgl[itroca], temp);
					}
					imin++;
					posi = valvgl[imin].posicG;
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-097: Unicidade da chave 'id' em '#/fonteGasLift'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter o elemento "fonteGas" do arquivo Json do MRT em struct fonteg.
 *
 * \tparam Objeto JSON correspondente a fonteGas
 */
void Ler::parse_fonte_gas(JSON_entrada_fonteGas& fonte_gas_json/*, */) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fonteGas");
	try {
		// tamanho do vetor fonteGas
		ninjgas = 0;
		// percorre o array de fontes de gás
		for (size_t i = 0; i < fonte_gas_json.size(); i++) {
			// caso a fonte de gás esteja ativo
			if (is_ativo(fonte_gas_json[i]))
				ninjgas++;
		}

		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		// caso o tamanho do elemento seja maior que zero
		if (ninjgas > 0) {
			// declarar o vetor fonteg
			fonteg = new detFONGAS[ninjgas];
			// iniciar vetor de inteiros para armazenar os ids
			//identificadores = new int[ninjgas];
			identificadores.resize(ninjgas);
			// loop para parse das estruturas do fonteGas
			int indAtivo = -1;
			for (int i = 0; i < ninjgas; i++) {
				// enquanto a propriedade "ativo" da fonte de gás esteja desabilitada, avança
				while (!is_ativo(fonte_gas_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] = fonte_gas_json[indAtivo].id();
				// caso o identificador seja maior que o último selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				fonteg[i].comp =
						fonte_gas_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					fonteg[i].comp = nCompTotalUnidadesP - fonteg[i].comp; //04-04-2018
				if (fonteg[i].comp < 0.0)
					fonteg[i].comp = 0.0;
				double lverif = fonteg[i].comp;
				fonteg[i].posicP = buscaIndiceMeioP(lverif);
				// parse do vetor de pressões estáticas
				fonteg[i].parserie =
						(int) fonte_gas_json[indAtivo].tempo().size();
				fonteg[i].temp = new double[fonteg[i].parserie];
				fonteg[i].vazgas = new double[fonteg[i].parserie];
				fonteg[i].vazcomp = new double[fonteg[i].parserie];
				fonteg[i].tempo = new double[fonteg[i].parserie];
				fonteg[i].seco=1;
				if (fonte_gas_json[indAtivo].seco().exists()){
					fonteg[i].seco =fonte_gas_json[indAtivo].seco();
					if(fonteg[i].seco==0){
						fonteg[i].indfluP =fonte_gas_json[indAtivo].indiFluidoPro();
						int iflu;
						for(iflu=0;iflu<nfluP;iflu++){
							if(indFlup[iflu]==fonteg[i].indfluP){
								fonteg[i].indfluP=iflu;
								break;
							}
						}
						if(iflu==nfluP)
							logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
										chaveJson,
										"Chaves #/indice de fluido de producao inexistente");
					}
				}
				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((fonte_gas_json[indAtivo].tempo().size()
						!= fonte_gas_json[indAtivo].temperatura().size())
						|| (fonte_gas_json[indAtivo].tempo().size()
								!= fonte_gas_json[indAtivo].vazaoGas().size())) {
					// RN-077: chaves tempo, temperatura e vazaoGas com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/fonteGas/tempo, #/fonteGas/temperatura e #/fonteGas/vazaoGas com tamanhos diferentes");
				} else {
					for (int j = 0; j < fonteg[i].parserie; j++) {
						fonteg[i].temp[j] =
								fonte_gas_json[indAtivo].temperatura()[j];
						fonteg[i].vazgas[j] =
								fonte_gas_json[indAtivo].vazaoGas()[j];
						fonteg[i].tempo[j] =
								fonte_gas_json[indAtivo].tempo()[j];
					}
					if (fonte_gas_json[indAtivo].vazaoFluidoComplementar().exists() && fonteg[i].seco==0){
						if (fonte_gas_json[indAtivo].tempo().size()
												!= fonte_gas_json[indAtivo].vazaoFluidoComplementar().size()){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/fonteGas/tempo, #/fonteGas/vazaoFluidoComplementar com tamanhos diferentes");
						}
						else{
							for (int j = 0; j < fonteg[i].parserie; j++){
								fonteg[i].vazcomp[j] =
									fonte_gas_json[indAtivo].vazaoFluidoComplementar()[j]/(100.*1e6);
							}
						}
					}
					else{
						for (int j = 0; j < fonteg[i].parserie; j++) fonteg[i].vazcomp[j]=0.;
					}
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-015: Unicidade da chave 'id' em '#/fonteGas'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter o elemento "valvula" do arquivo Json do MRT em struct .
 *
 * \tparam Objeto JSON correspondente à fonte de liquido
 */
void Ler::parse_valv(JSON_entrada_valvula& valvula_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/valvula");
	try {
		// tamanho do vetor valvula
		nvalv = 0;
		// percorre o array de valvula
		for (size_t i = 0; i < valvula_json.size(); i++) {
			// caso a valvula esteja ativo
			if (is_ativo(valvula_json[i]))
				nvalv++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (nvalv > 0) {
			int iIni;
			iIni = 0;
			// declarar o vetor valv
			valv = new detValv[nvalv];

			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nvalv];
			identificadores.resize(nvalv);
			// loop para parse das estruturas do valv
			int indAtivo = -1;
			for (int i = iIni; i < nvalv; i++) {
				// enquanto a propriedade "ativo" da valvula esteja desabilitada, avança
				while (!is_ativo(valvula_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						valvula_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				valv[i].curvaCV=0;
				valv[i].curvaDinamic=0;
				valv[i].ncv=0;
				if (valvula_json[indAtivo].curvaCV().exists())
					valv[i].curvaCV = valvula_json[indAtivo].curvaCV();
				if (valvula_json[indAtivo].curvaDinamic().exists())
					valv[i].curvaDinamic = valvula_json[indAtivo].curvaDinamic();
				if(valv[i].curvaCV==1){
					if (!valvula_json[indAtivo].x1().exists() || !valvula_json[indAtivo].cv1().exists()){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/valvula/x1 ou #/valvula/cv1 inexistente");
					}
					if (valvula_json[indAtivo].x1().size() != valvula_json[indAtivo].cv1().size()){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves dimensao de #/valvula/x1 diferente de  #/valvula/cv1");
					}
					valv[i].ncv =valvula_json[indAtivo].x1().size()-1;
					valv[i].cvCurv= new detCV [valv[i].ncv];
					for(int konta=0;konta<valv[i].ncv;konta++){
						valv[i].cvCurv[konta].x1=valvula_json[indAtivo].x1()[konta];
						valv[i].cvCurv[konta].x2=valvula_json[indAtivo].x1()[konta+1];
						valv[i].cvCurv[konta].cv1=valvula_json[indAtivo].cv1()[konta];
						valv[i].cvCurv[konta].cv2=valvula_json[indAtivo].cv1()[konta+1];
					}
				}

				valv[i].comp =
						valvula_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					valv[i].comp = nCompTotalUnidadesP - valv[i].comp; //04-04-2018
				if (valv[i].comp < 0.0)
					valv[i].comp = 0.0;
				double lverif = valv[i].comp;
				valv[i].posicP = buscaIndiceMeioP(lverif);

				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((valvula_json[indAtivo].tempo().size()
								!= valvula_json[indAtivo].abertura().size())) {
					// RN-078: chaves tempo e abertura com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/valvula/tempo e #/valvula/abertura com tamanhos diferentes");
				} else {
					valv[i].parserie =
							(int) valvula_json[indAtivo].tempo().size();
					valv[i].abertura = new double[valv[i].parserie];
					valv[i].tempo = new double[valv[i].parserie];

					// parse does vetores
					for (int j = 0; j < valv[i].parserie; j++) {
						valv[i].abertura[j] =
								valvula_json[indAtivo].abertura()[j];
						valv[i].tempo[j] =
								valvula_json[indAtivo].tempo()[j];
					}
				}
				valv[i].cd=0.84;
				if (valvula_json[indAtivo].cd().exists())
				valv[i].cd =valvula_json[indAtivo].cd();
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-093: Unicidade da chave 'id' em '#/valvula'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter o elemento "fonteLiquido" do arquivo Json do MRT em struct .
 *
 * \tparam Objeto JSON correspondente à fonte de liquido
 */
void Ler::parse_fonte_liquido(JSON_entrada_fonteLiquido& fonte_liquido_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fonteLiquido");
	try {
		// tamanho do vetor fonte_liquido
		ninjliq = 0;
		// percorre o array de fontes de líquido
		for (size_t i = 0; i < fonte_liquido_json.size(); i++) {
			// caso a fonte de líquido esteja ativo
			if (is_ativo(fonte_liquido_json[i]))
				ninjliq++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (ninjliq > 0) {
			int iIni;
			if (pocinjec == 1) {
				ninjliq++;
				// declarar o vetor fontel
				fontel = new detFONLIQ[ninjliq];
				// incluir fonte de liquido adicional
				fontel[0].comp = 0;
				fontel[0].posicP = 0;
				fontel[0].parserie = 1;
				fontel[0].temp = new double[fontel[0].parserie];
				fontel[0].bet = new double[fontel[0].parserie];
				fontel[0].vazliq = new double[fontel[0].parserie];
				fontel[0].tempo = new double[fontel[0].parserie];
				fontel[0].temp[0] = condpocinj.tempinj;
				fontel[0].bet[0] = 1.;
				fontel[0].vazliq[0] = condpocinj.vazinj;
				fontel[0].tempo[0] = 0.;
				fontel[0].indfluP = 0.;
				iIni = 1;
			} else {
				iIni = 0;
				// declarar o vetor fontel
				fontel = new detFONLIQ[ninjliq];
			}

			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[ninjliq];
			identificadores.resize(ninjliq);

			// loop para parse das estruturas do fontel
			int indAtivo = -1;
			for (int i = iIni; i < ninjliq; i++) {
				// enquanto a propriedade "ativo" da fonte de líquido esteja desabilitada, avança
				while (!is_ativo(fonte_liquido_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						fonte_liquido_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				int indicaBeta=0;
				if (fonte_liquido_json[indAtivo].beta().exists())indicaBeta=1;

				fontel[i].comp =
						fonte_liquido_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					fontel[i].comp = nCompTotalUnidadesP - fontel[i].comp; //04-04-2018
				if (fontel[i].comp < 0.0)
					fontel[i].comp = 0.0;
				double lverif = fontel[i].comp;
				fontel[i].posicP = buscaIndiceMeioP(lverif);

				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((fonte_liquido_json[indAtivo].tempo().size()
						!= fonte_liquido_json[indAtivo].temperatura().size())
						|| (indicaBeta && fonte_liquido_json[indAtivo].tempo().size()
								!= fonte_liquido_json[indAtivo].beta().size())
						|| (fonte_liquido_json[indAtivo].tempo().size()
								!= fonte_liquido_json[indAtivo].vazaoLiquido().size())) {
					// RN-078: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/fonteLiquido/tempo, #/fonteLiquido/temperatura, #/fonteLiquido/beta e #/fonteLiquido/vazaoLiquido com tamanhos diferentes");
				} else {
					fontel[i].parserie =
							(int) fonte_liquido_json[indAtivo].tempo().size();
					fontel[i].temp = new double[fontel[i].parserie];
					fontel[i].bet = new double[fontel[i].parserie];
					fontel[i].vazliq = new double[fontel[i].parserie];
					fontel[i].tempo = new double[fontel[i].parserie];

					// parse does vetores
					for (int j = 0; j < fontel[i].parserie; j++) {
						fontel[i].temp[j] =
								fonte_liquido_json[indAtivo].temperatura()[j];
						if(indicaBeta)
						fontel[i].bet[j] =
								fonte_liquido_json[indAtivo].beta()[j];
						else fontel[i].bet[j] =0.;
						fontel[i].vazliq[j] =
								fonte_liquido_json[indAtivo].vazaoLiquido()[j];
						fontel[i].tempo[j] =
								fonte_liquido_json[indAtivo].tempo()[j];
					}
				}
				fontel[i].indfluP =fonte_liquido_json[indAtivo].indiFluidoPro();
				fontel[i].indfluP = identificarFluidoProducao(fontel[i].indfluP);
				/*fontel[i].indfluP =fonte_liquido_json[indAtivo].indiFluidoPro();

				int iflu;
				for(iflu=0;iflu<nfluP;iflu++){
					if(indFlup[iflu]==fontel[i].indfluP){
						fontel[i].indfluP=iflu;
						break;
					}
				}
				if(iflu==nfluP)
					logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/indice de fluido de producao inexistente");*/


			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-093: Unicidade da chave 'id' em '#/fonteLiquido'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_fonte_massa(JSON_entrada_fonteMassa& fonte_massa_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fonteMassa");
	try {
		// tamanho do vetor fonte_liquido

	    if(ConContEntrada!=2)ninjmass = 0;
	    else ninjmass = 1;
		// percorre o array de fontes de líquido
		for (size_t i = 0; i < fonte_massa_json.size(); i++) {
			// caso a fonte de líquido esteja ativo
			if (is_ativo(fonte_massa_json[i]))
				ninjmass++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (ninjmass > 0) {
			int iIni;
			if(ConContEntrada!=2)iIni = 0;
			else iIni=1;
			// declarar o vetor fontel
			fontem = new detFONMASS[ninjmass];

			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[ninjmass];
			identificadores.resize(ninjmass);

			if(ConContEntrada==2){
				fontem[0].condiTermo=0;
				fontem[0].comp =0;
				fontem[0].indfluP =indfluPIni;
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					fontem[0].comp = nCompTotalUnidadesP - fontem[0].comp; //04-04-2018
				if (fontem[0].comp < 0.0)
					fontem[0].comp = 0.0;
				double lverif = fontem[0].comp;
				fontem[0].posicP = buscaIndiceMeioP(lverif);
				fontem[0].parserie =CCVPres.parserie;
				fontem[0].temp = new double[fontem[0].parserie];
				fontem[0].vazMasT = new double[fontem[0].parserie];
				fontem[0].vazMasC = new double[fontem[0].parserie];
				fontem[0].vazMasG = new double[fontem[0].parserie];
				fontem[0].tempo = new double[fontem[0].parserie];
				for (int j = 0; j < fontem[0].parserie; j++) {
					fontem[0].temp[j] =CCVPres.temperatura[j];
					fontem[0].vazMasT[j] =CCVPres.mass[j];
					fontem[0].vazMasC[j] =CCVPres.mass[j]*CCVPres.bet[j];
					if(tipoFluido==1){
						fontem[0].vazMasC[j] *=(fluc.MasEspFlu(1.01, 20.)/flup[fontem[0].indfluP].Deng*1.225);
						//fontem[0].vazMasC[j]/=(100.*1e6);
					}
					fontem[0].vazMasG[j] =0.;
					fontem[0].tempo[j] =CCVPres.tempo[j];
				}
			}

			// loop para parse das estruturas do fontel
			int indAtivo = -1;
			for (int i = iIni; i < ninjmass; i++) {
				// enquanto a propriedade "ativo" da fonte de líquido esteja desabilitada, avança
				while (!is_ativo(fonte_massa_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						fonte_massa_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				fontem[i].condiTermo=fonte_massa_json[indAtivo].tipoTermo();

				fontem[i].comp =
						fonte_massa_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					fontem[i].comp = nCompTotalUnidadesP - fontem[i].comp; //04-04-2018
				if (fontem[i].comp < 0.0)
					fontem[i].comp = 0.0;
				double lverif = fontem[i].comp;
				fontem[i].posicP = buscaIndiceMeioP(lverif);

				indicaBeta=0;
				if (fonte_massa_json[indAtivo].vazaoMassC().exists())indicaBeta=1;

				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((fonte_massa_json[indAtivo].tempo().size()
						!= fonte_massa_json[indAtivo].temperatura().size())
						|| (fonte_massa_json[indAtivo].tempo().size()
								!= fonte_massa_json[indAtivo].vazaoMassT().size())
						|| (fonte_massa_json[indAtivo].tempo().size()
								!= fonte_massa_json[indAtivo].vazaoMassG().size() && fontem[i].condiTermo==1)
						|| (indicaBeta && fonte_massa_json[indAtivo].tempo().size()
								!= fonte_massa_json[indAtivo].vazaoMassC().size())) {
					// RN-078: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/fonteMassa/tempo, #/fonteMassa/temperatura, #/fonteMassa/vazaoMassaT,  "
							"#/fonteMassa/vazaoMassaG e  #/fonteMassa/vazaoMassaC com tamanhos diferentes");
				} else {
					fontem[i].parserie =
							(int) fonte_massa_json[indAtivo].tempo().size();
					fontem[i].temp = new double[fontem[i].parserie];
					fontem[i].vazMasT = new double[fontem[i].parserie];
					fontem[i].vazMasC = new double[fontem[i].parserie];
					fontem[i].vazMasG = new double[fontem[i].parserie];
					fontem[i].tempo = new double[fontem[i].parserie];



					// parse does vetores
					for (int j = 0; j < fontem[i].parserie; j++) {
						fontem[i].temp[j] =
								fonte_massa_json[indAtivo].temperatura()[j];
						fontem[i].vazMasT[j] =
								fonte_massa_json[indAtivo].vazaoMassT()[j];
						if(indicaBeta)
						fontem[i].vazMasC[j] =
								fonte_massa_json[indAtivo].vazaoMassC()[j];
						else fontem[i].vazMasC[j] =0.;
						if(fontem[i].condiTermo==1)
						fontem[i].vazMasG[j] =
								fonte_massa_json[indAtivo].vazaoMassG()[j];
						else
							fontem[i].vazMasG[j] =0.;
						fontem[i].tempo[j] =
								fonte_massa_json[indAtivo].tempo()[j];
					}
				}
				fontem[i].indfluP =fonte_massa_json[indAtivo].indiFluidoPro();
				fontem[i].indfluP = identificarFluidoProducao(fontem[i].indfluP);
				/*fontem[i].indfluP =fonte_massa_json[indAtivo].indiFluidoPro();

				int iflu;
				for(iflu=0;iflu<nfluP;iflu++){
					if(indFlup[iflu]==fontem[i].indfluP){
						fontem[i].indfluP=iflu;
						break;
					}
				}
				if(iflu==nfluP)
					logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
								chaveJson,
								"Chaves #/indice de fluido de producao inexistente");*/

			}
			// verificar a unicidade dos identificadores
			if (ConContEntrada!=2 && !verificarUnicidade(identificadores)) {
				// RN-093: Unicidade da chave 'id' em '#/fonteLiquido'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_fonte_PoroRadial(JSON_entrada_fontePoroRadial& fonte_poroRadial_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fontePoroRadial");
	try {
		// tamanho do vetor fonte_liquido
		nPoroRad = 0;
		// percorre o array de fontes de líquido
		for (size_t i = 0; i < fonte_poroRadial_json.size(); i++) {
			// caso a fonte de líquido esteja ativo
			if (is_ativo(fonte_poroRadial_json[i]))
				nPoroRad++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (nPoroRad > 0) {
			int iIni;
			iIni = 0;
			// declarar o vetor fontel
			porosoRad = new detPoroRad[nPoroRad];

			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nPoroRad];
			identificadores.resize(nPoroRad);

			// loop para parse das estruturas do fontel
			int indAtivo = -1;
			for (int i = iIni; i < nPoroRad; i++) {
				// enquanto a propriedade "ativo" da fonte de líquido esteja desabilitada, avança
				while (!is_ativo(fonte_poroRadial_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						fonte_poroRadial_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				porosoRad[i].comp =
						fonte_poroRadial_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					porosoRad[i].comp = nCompTotalUnidadesP - porosoRad[i].comp; //04-04-2018
				if (porosoRad[i].comp < 0.0)
					porosoRad[i].comp = 0.0;
				double lverif = porosoRad[i].comp;
				porosoRad[i].posicP = buscaIndiceMeioP(lverif);

				porosoRad[i].nomeArquivoEntrada =fonte_poroRadial_json[indAtivo].arquivo();
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-093: Unicidade da chave 'id' em '#/fonteLiquido'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_fonte_Poro2D(JSON_entrada_fontePoro2D& fonte_poro2D_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fontePoro2D");
	try {
		// tamanho do vetor fonte_liquido
		nPoro2D = 0;
		// percorre o array de fontes de líquido
		for (size_t i = 0; i < fonte_poro2D_json.size(); i++) {
			// caso a fonte de líquido esteja ativo
			if (is_ativo(fonte_poro2D_json[i]))
				nPoro2D++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (nPoro2D > 0) {
			int iIni;
			iIni = 0;
			// declarar o vetor fontel
			poroso2D = new detPoro2D[nPoro2D];

			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nPoro2D];
			identificadores.resize(nPoro2D);

			// loop para parse das estruturas do fontel
			int indAtivo = -1;
			for (int i = iIni; i < nPoro2D; i++) {
				// enquanto a propriedade "ativo" da fonte de líquido esteja desabilitada, avança
				while (!is_ativo(fonte_poro2D_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						fonte_poro2D_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				poroso2D[i].comp =
						fonte_poro2D_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					poroso2D[i].comp = nCompTotalUnidadesP - poroso2D[i].comp; //04-04-2018
				if (poroso2D[i].comp < 0.0)
					poroso2D[i].comp = 0.0;
				double lverif = poroso2D[i].comp;
				poroso2D[i].posicP = buscaIndiceMeioP(lverif);

				poroso2D[i].nomeArquivoEntrada =fonte_poro2D_json[indAtivo].arquivo();
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-093: Unicidade da chave 'id' em '#/fonteLiquido'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
			if (!logger.getStResultadoSimulacao().sucesso) {
				// gerar o arquivo de saida da simulacao e encerra a aplicacao
				logger.write_logs_and_exit();
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}


/*void Ler::parse_furo(Value& furo_json) {
	string chaveJson("#/furo");
	try {
		// tamanho do vetor fonte_liquido
		nfuro = 0;
		nfuro=furo_json.Size();;
		// percorre o array de fontes de líquido
		// criar vetor de inteiros para armazenar os ids
		int* identificadores = NULL;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (nfuro > 0) {
			int iIni;
		    iIni = 0;
				// declarar o vetor fontel
		    furo = new detFURO[nfuro];

			// criar vetor de inteiros para armazenar os ids
			identificadores = new int[nfuro];

			// loop para parse das estruturas do fontel
			int indAtivo = -1;
			for (int i = iIni; i < nfuro; i++) {
				// enquanto a propriedade "ativo" da fonte de líquido esteja desabilitada, avança
				//while (!is_ativo(furo_json[++indAtivo]))
					//;
				indAtivo=i;
				// obter maior identificador
				identificadores[i] =
						furo_json[indAtivo]["id"].GetInt();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				furo[i].comp =
						furo_json[indAtivo]["comprimentoMedido"].GetDouble();
				// sentido plataforma para fundo-poco
				if (origemGeometria == origemGeometria_t::plataforma)
					furo[i].comp = nCompTotalUnidadesP - furo[i].comp; //04-04-2018
				if (furo[i].comp < 0.0)
					furo[i].comp = 0.0;
				double lverif = furo[i].comp;
				furo[i].posicP = buscaIndiceMeioP(lverif);

				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((furo_json[indAtivo]["tempo"].Capacity()
						!= furo_json[indAtivo]["temperatura"].Capacity())
						|| (furo_json[indAtivo]["tempo"].Capacity()
								!= furo_json[indAtivo]["pressao"].Capacity())
						|| (furo_json[indAtivo]["tempo"].Capacity()
								!= furo_json[indAtivo]["abertura"].Capacity())) {
					// RN-078: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/furo/tempo, #/furo/temperatura, #/furo/pressao e #/furo/abertura com tamanhos diferentes");
				} else {
					furo[i].parserie =
							(int) furo_json[indAtivo]["tempo"].Size();
					furo[i].temp = new double[furo[i].parserie];
					furo[i].pres = new double[furo[i].parserie];
					furo[i].abertura= new double[furo[i].parserie];
					furo[i].tempo = new double[furo[i].parserie];

					// parse does vetores
					for (int j = 0; j < furo[i].parserie; j++) {
						furo[i].temp[j] =
								furo_json[indAtivo]["temperatura"][j].GetDouble();
						furo[i].pres[j] =
								furo_json[indAtivo]["pressao"][j].GetDouble();
						furo[i].abertura[j] =
								furo_json[indAtivo]["abertura"][j].GetDouble();
						furo[i].tempo[j] =
								furo_json[indAtivo]["tempo"][j].GetDouble();
					}
				}
				//fontel[i].indfluP = identificarFluidoProducao(
						//fonte_liquido_json[indAtivo]["indiFluidoPro"].GetInt());
				furo[i].tipoFlu =furo_json[indAtivo]["tipoFluido"].GetInt();
				if(furo[i].tipoFlu==0)
				furo[i].indFlu =furo_json[indAtivo]["indiFluidoPro"].GetInt();
				else furo[i].indFlu=-1;
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores, nfuro,
					maiorIdentificador)) {
				// RN-093: Unicidade da chave 'id' em '#/fonteLiquido'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
			if (!logger.getStResultadoSimulacao().sucesso) {
				// gerar o arquivo de saida da simulacao e encerra a aplicacao
				logger.write_logs_and_exit();
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}*/

void Ler::parse_furo(JSON_entrada_fontePressao &fontePressao_json) {
	string chaveJson("#/fontePressao");
	try {
		// tamanho do vetor fonte_liquido
		nfuro = 0;
		// percorre o array de fontes de líquido
		for (size_t i = 0; i < fontePressao_json.size(); i++) {
		 // caso a fonte de líquido esteja ativo
		 if (is_ativo(fontePressao_json[i]))
		 nfuro++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int *identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (nfuro > 0) {
			int iIni;
			iIni = 0;
			// declarar o vetor fontel
			furo = new detFURO[nfuro];

			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nfuro];
			identificadores.resize(nfuro);

			// loop para parse das estruturas do fontel
			int indAtivo = -1;
			for (int i = iIni; i < nfuro; i++) {
				// enquanto a propriedade "ativo" da fonte de líquido esteja desabilitada, avança
				while (!is_ativo(fontePressao_json[++indAtivo]))
				;
				indAtivo = i;//??????????????????????????ESTA CERTO???????????????????????????????????????????
				// obter maior identificador
				identificadores[i] = fontePressao_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				furo[i].comp = fontePressao_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					furo[i].comp = nCompTotalUnidadesP - furo[i].comp; //04-04-2018
				if (furo[i].comp < 0.0)
					furo[i].comp = 0.0;
				double lverif = furo[i].comp;
				furo[i].posicP = buscaIndiceMeioP(lverif);

				furo[i].TipoAbertura=0;
				if (fontePressao_json[indAtivo].TipoAbertura().exists()){
					furo[i].TipoAbertura=fontePressao_json[indAtivo].TipoAbertura();
				}

				furo[i].beta=0;
				if (fontePressao_json[indAtivo].beta().exists()){
					furo[i].beta=fontePressao_json[indAtivo].beta();
				}

				furo[i].titAmb=0;
				if (fontePressao_json[indAtivo].titAmb().exists()){
					furo[i].titAmb=fontePressao_json[indAtivo].titAmb();
				}

				// caso os tamanhos dos vetores das chaves difiram entre si
				if (fontePressao_json[indAtivo].tempo().size() != fontePressao_json[indAtivo].abertura().size()) {
					// RN-078: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson, chaveJson, "Chaves #/fontePressao/tempo e #/fontePressao/abertura com tamanhos diferentes");
				} else {
					furo[i].parserie = (int) fontePressao_json[indAtivo].tempo().size();
					furo[i].abertura = new double[furo[i].parserie];
					furo[i].tempo = new double[furo[i].parserie];

					// parse does vetores
					for (int j = 0; j < furo[i].parserie; j++) {
						furo[i].abertura[j] = fontePressao_json[indAtivo].abertura()[j];
						furo[i].tempo[j] = fontePressao_json[indAtivo].tempo()[j];
					}
				}
				//fontel[i].indfluP = identificarFluidoProducao(
				//fonte_liquido_json[indAtivo]["indiFluidoPro"].GetInt());
				furo[i].temp=10;
				furo[i].temp = fontePressao_json[indAtivo].temperatura();
				furo[i].pres=1.;
				furo[i].pres = fontePressao_json[indAtivo].pressao();
				furo[i].cd=0.84;
				if(fontePressao_json[indAtivo].cd().exists())
				furo[i].cd = fontePressao_json[indAtivo].cd();
				furo[i].check=0;
				furo[i].tempoChk =0;
				if(fontePressao_json[indAtivo].check().exists()){
					//furo[i].check = fontePressao_json[indAtivo].check();
					// caso os tamanhos dos vetores das chaves difiram entre si
					if (fontePressao_json[indAtivo].tempoChk().size() != fontePressao_json[indAtivo].check().size()) {
						// RN-078: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson, chaveJson, "Chaves #/fontePressao/tempoChk e #/fontePressao/check com tamanhos diferentes");
					} else {
						furo[i].parserieChk = (int) fontePressao_json[indAtivo].tempoChk().size();
						furo[i].check = new int[furo[i].parserieChk];
						furo[i].tempoChk = new double[furo[i].parserieChk];

						// parse does vetores
						for (int j = 0; j < furo[i].parserieChk; j++) {
							furo[i].check[j] = fontePressao_json[indAtivo].check()[j];
							furo[i].tempoChk[j] = fontePressao_json[indAtivo].tempoChk()[j];
						}
					}
				}
				else{
					furo[i].parserieChk = 1;
					furo[i].check = new int[furo[i].parserieChk];
					furo[i].tempoChk = new double[furo[i].parserieChk];

					// parse does vetores
					for (int j = 0; j < furo[i].parserieChk; j++) {
						furo[i].check[j] = 0;
						furo[i].tempoChk[j] = 0.;
					}
				}
				furo[i].ambGas=0;
				if(fontePressao_json[indAtivo].ambGas().exists())
				furo[i].ambGas = fontePressao_json[indAtivo].ambGas();
				furo[i].tipoFlu =1;
				if(fontePressao_json[indAtivo].tipoFluido().exists())
				furo[i].tipoFlu = fontePressao_json[indAtivo].tipoFluido();
				if (furo[i].tipoFlu == 0){
					furo[i].indFlu = fontePressao_json[indAtivo].indiFluidoPro();
					furo[i].indFlu = identificarFluidoProducao(furo[i].indFlu);

					/*furo[i].indFlu = fontePressao_json[indAtivo].indiFluidoPro();

					int iflu;
					for(iflu=0;iflu<nfluP;iflu++){
						if(indFlup[iflu]==furo[i].indFlu){
							furo[i].indFlu=iflu;
							break;
						}
					}
					if(iflu==nfluP)
						logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
									chaveJson,
									"Chaves #/indice de fluido de producao inexistente");*/

				}
				else{
					furo[i].indFlu = 0;
					//fluc.maritimo=1.;
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-093: Unicidade da chave 'id' em '#/fonteLiquido'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception &e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/* Converter os elementos "bcs" do arquivo Json do MRT em struct bcs.
 *
 * \tparam Objeto JSON correspondente a bcs.
 */
void Ler::parse_bcs(JSON_entrada_bcs& bcs_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/bcs");
	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;
	try {
		// tamanho do vetor de bcs
		nbcs = 0;
		// percorre o array de bcs
		for (size_t i = 0; i < bcs_json.size(); i++) {
			// caso o bcs esteja ativo
			if (is_ativo(bcs_json[i]))
				nbcs++;
		}
		if (nbcs > 0) {
			bcs = new detBCS[nbcs];
			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nbcs];
			identificadores.resize(nbcs);
			// loop para parse das estruturas do bcs
			int indAtivo = -1;
			for (int i = 0; i < nbcs; i++) {
				// enquanto a propriedade "ativo" do bcs esteja desabilitada, avança
				while (!is_ativo(bcs_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] = bcs_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				bcs[i].comp =
						bcs_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					bcs[i].comp = nCompTotalUnidadesP - bcs[i].comp; //04-04-2018
				if (bcs[i].comp < 0.0)
					bcs[i].comp = 0.0;
				double lverif = bcs[i].comp;
				bcs[i].posicP = buscaIndiceFrontP(lverif);

				// caso os tamanhos dos vetores das chaves difiram entre si
				if (bcs_json[indAtivo].tempo().size()
						!= bcs_json[indAtivo].frequencia().size()) {
					// RN-080: chaves tempo, temperatura, beta e vazaoLiquido com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/bcs/tempo, #/bcs/frequencia com tamanhos diferentes");
				} else {
					bcs[i].parserie = (int) bcs_json[indAtivo].tempo().size();
					bcs[i].tempo = new double[bcs[i].parserie];
					bcs[i].freq = new double[bcs[i].parserie];
					for (int j = 0; j < bcs[i].parserie; j++) {
						bcs[i].tempo[j] =
								bcs_json[indAtivo].tempo()[j];
						bcs[i].freq[j] =
								bcs_json[indAtivo].frequencia()[j];
					}
				}

				// caso os tamanhos dos vetores das chaves difiram entre si
				if ((bcs_json[indAtivo].vazao().size()
						!= bcs_json[indAtivo].head().size())
						|| (bcs_json[indAtivo].vazao().size()
								!= bcs_json[indAtivo].potencia().size())
						|| (bcs_json[indAtivo].vazao().size()
								!= bcs_json[indAtivo].eficiencia().size())) {
					// RN-081: chaves vazao, head, potencia e eficiencia com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/bcs/vazao, #/bcs/head, #/bcs/potencia e #/bcs/eficiencia com tamanhos diferentes");
				} else {
					bcs[i].ncurva = (int) bcs_json[indAtivo].vazao().size();
					bcs[i].vaz = new double[bcs[i].ncurva];
					bcs[i].head = new double[bcs[i].ncurva];
					bcs[i].power = new double[bcs[i].ncurva];
					bcs[i].efic = new double[bcs[i].ncurva];
					int nestagFab=1;
					if (bcs_json[indAtivo].nestagFab().exists())
						nestagFab = bcs_json[indAtivo].nestagFab();
					bcs[i].correcHI=1;
					if (bcs_json[indAtivo].correcHI().exists())
						bcs[i].correcHI = bcs_json[indAtivo].correcHI();
					for (int j = 0; j < bcs[i].ncurva; j++) {
						bcs[i].vaz[j] =
								bcs_json[indAtivo].vazao()[j];
						bcs[i].head[j] =
								bcs_json[indAtivo].head()[j]/nestagFab;
						bcs[i].power[j] =
								bcs_json[indAtivo].potencia()[j]/nestagFab;
						bcs[i].efic[j] =
								bcs_json[indAtivo].eficiencia()[j];
					}
				}
				bcs[i].freqref = bcs_json[indAtivo].freqref();
				bcs[i].nestag = bcs_json[indAtivo].nestag();
				bcs[i].eficM = 100.;
				if (bcs_json[indAtivo].EficienciaMotor().exists())
					bcs[i].eficM =
							bcs_json[indAtivo].EficienciaMotor();
				bcs[i].freqMinima = 0.;
				if (bcs_json[indAtivo].FrequenciaMinima().exists())
					bcs[i].freqMinima =
							bcs_json[indAtivo].FrequenciaMinima();
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-094: Unicidade da chave 'id' em '#/bcs'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "bombaVolumetrica" do arquivo Json do MRT em struct bvol.
 *
 * \tparam Objeto JSON correspondente a bombaVolumetrica.
 */
void Ler::parse_bomba_volumetrica(
		JSON_entrada_bombaVolumetrica& bomba_volumetrica_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/bombaVolumetrica");
	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;
	try {
		// tamanho do array de bombas volumétricas
		nbvol = 0;
		// percorre o array de bombas volumétricas
		for (size_t i = 0; i < bomba_volumetrica_json.size(); i++) {
			// caso a bomba volumétrica esteja ativo
			if (is_ativo(bomba_volumetrica_json[i]))
				nbvol++;
		}
		if (nbvol > 0) {
			bvol = new detBVOL[nbvol];
			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[nbvol];
			identificadores.resize(nbvol);
			// loop para parse das estruturas de bombas volumétricas
			int indAtivo = -1;
			for (int i = 0; i < nbvol; i++) {
				// enquanto a propriedade "ativo" da bomba volumétrica esteja desabilitada, avança
				while (!is_ativo(bomba_volumetrica_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						bomba_volumetrica_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				bvol[i].comp =
						bomba_volumetrica_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					bvol[i].comp = nCompTotalUnidadesP - bvol[i].comp; //04-04-2018
				if (bvol[i].comp < 0.0)
					bvol[i].comp = 0.0;
				double lverif = bvol[i].comp;
				bvol[i].posicP = buscaIndiceFrontP(lverif);

				// caso os tamanhos dos vetores das chaves difiram entre si
				if (bomba_volumetrica_json[indAtivo].tempo().size()
						!= bomba_volumetrica_json[indAtivo].frequencia().size()) {
					// RN-083: chaves tempo, frequencia com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/bombaVolumetrica/tempo, #/bombaVolumetrica/frequencia com tamanhos diferentes");
				} else {
					bvol[i].parserie =
							(int) bomba_volumetrica_json[indAtivo].tempo().size();
					bvol[i].tempo = new double[bvol[i].parserie];
					bvol[i].freq = new double[bvol[i].parserie];
					for (int j = 0; j < bvol[i].parserie; j++) {
						bvol[i].tempo[j] =
								bomba_volumetrica_json[indAtivo].tempo()[j];
						bvol[i].freq[j] =
								bomba_volumetrica_json[indAtivo].frequencia()[j];
					}
				}
				bvol[i].capacidade =
						bomba_volumetrica_json[indAtivo].capacidade()
								/ 1.0e6;
				bvol[i].npoli =
						bomba_volumetrica_json[indAtivo].fatorpoli();
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-095: Unicidade da chave 'id' em '#/bombaVolumetrica'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "deltaPressao" do arquivo Json do MRT em struct dpreq.
 *
 * \tparam Objeto JSON correspondente a deltaPressao.
 */
void Ler::parse_delta_pressao(JSON_entrada_deltaPressao& delta_pressao_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/deltaPressao");
	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;
	try {
		// tamanho do array de deltas de pressão
		ndpreq = 0;
		// percorre o array de deltas de pressão
		for (size_t i = 0; i < delta_pressao_json.size(); i++) {
			// caso o delta de pressão esteja ativo
			if (is_ativo(delta_pressao_json[i]))
				ndpreq++;
		}
		if (ndpreq > 0) {
			dpreq = new detDPREQ[ndpreq];
			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[ndpreq];
			identificadores.resize(ndpreq);
			// loop para parse das estruturas de deltas de pressão
			int indAtivo = -1;
			for (int i = 0; i < ndpreq; i++) {
				// enquanto a propriedade "ativo" do delta de pressão esteja desabilitada, avança
				while (!is_ativo(delta_pressao_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						delta_pressao_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				dpreq[i].comp =
						delta_pressao_json[indAtivo].comprimentoMedido();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					dpreq[i].comp = nCompTotalUnidadesP - dpreq[i].comp; //04-04-2018
				if (dpreq[i].comp < 0.0)
					dpreq[i].comp = 0.0;
				double lverif = dpreq[i].comp;
				dpreq[i].posicP = buscaIndiceFrontP(lverif);

				if (delta_pressao_json[indAtivo].tipoCompGas().exists())
					dpreq[i].tipoCompGas=delta_pressao_json[indAtivo].tipoCompGas();
				else dpreq[i].tipoCompGas=0;
				dpreq[i].fatPoli=1.;
				if(dpreq[i].tipoCompGas==1){
					if (delta_pressao_json[indAtivo].fatPoli().exists())
						dpreq[i].fatPoli=delta_pressao_json[indAtivo].fatPoli();
					else dpreq[i].tipoCompGas=0;
				}
				if (delta_pressao_json[indAtivo].eficLiq().exists())
					dpreq[i].eficLiq=delta_pressao_json[indAtivo].eficLiq();
				else dpreq[i].eficLiq=100;
				if (delta_pressao_json[indAtivo].eficGas().exists())
					dpreq[i].eficGas=delta_pressao_json[indAtivo].eficGas();
				else dpreq[i].eficGas=100;

				// caso os tamanhos dos vetores das chaves difiram entre si
				if (delta_pressao_json[indAtivo].tempo().size()
						!= delta_pressao_json[indAtivo].deltaPressao().size()) {
					// RN-083: chaves tempo, deltaPressao com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/deltaPressao/tempo, #/deltaPressao/deltaPressao com tamanhos diferentes");
				} else {
					dpreq[i].parserie =
							(int) delta_pressao_json[indAtivo].tempo().size();
					dpreq[i].tempo = new double[dpreq[i].parserie];
					dpreq[i].dp = new double[dpreq[i].parserie];
					for (int j = 0; j < dpreq[i].parserie; j++) {
						dpreq[i].tempo[j] =
								delta_pressao_json[indAtivo].tempo()[j];
						dpreq[i].dp[j] =
								delta_pressao_json[indAtivo].deltaPressao()[j];
					}
				}
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-096: Unicidade da chave 'id' em '#/deltaPressao'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "master1" do arquivo Json do MRT em struct master1.
 *
 * \tparam Objeto JSON correspondente a master1.
 */
void Ler::parse_master1(JSON_entrada_master1& master1_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/master1");
	try {
		master1.curvaCV=0;
		master1.ncv=0;
		if (master1_json.curvaCV().exists())
			master1.curvaCV = master1_json.curvaCV(); //alteracao4
		if(master1.curvaCV==1){
			if (!master1_json.x1().exists() || !master1_json.cv1().exists()){
				logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
						chaveJson,
						"Chaves #/master1/x1 ou #/master1/cv1 inexistente");
			}
			if (master1_json.x1().size() != master1_json.cv1().size()){
				logger.log(LOGGER_FALHA,
				LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
						chaveJson,
						"Chaves dimensao de #/master1/x1 diferente de  #/master1/cv1");
			}
			master1.ncv =master1_json.x1().size()-1;
			master1.cvCurv= new detCV [master1.ncv];
			for(int konta=0;konta<master1.ncv;konta++){
				master1.cvCurv[konta].x1=master1_json.x1()[konta];
				master1.cvCurv[konta].x2=master1_json.x1()[konta+1];
				master1.cvCurv[konta].cv1=master1_json.cv1()[konta];
				master1.cvCurv[konta].cv2=master1_json.cv1()[konta+1];
			}
		}
		master1.razareaativ = 0.4;  //alteracao4
		if (master1_json.razaoAreaAtiva().exists())
			master1.razareaativ = master1_json.razaoAreaAtiva(); //alteracao4
		master1.comp = master1_json.comprimentoMedido();
		// sentido plataforma para fundo-poco
		if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/){
			master1.comp = nCompTotalUnidadesP - (master1.comp+0.01);  //04-04-2018
		}
		else{
			master1.comp-=0.01;
		}
		if (master1.comp < 0.0)
			master1.comp = 0.0;
		double lverif = master1.comp;
		master1.posic = buscaIndiceFrontP(lverif);

		// caso a master1 esteja depois da ultima celula da linha na simulacao transiente
		if (master1.posic >= ncelp - 1 && transiente) {
			// RN-099: chaves tempo, abertura com tamanhos diferentes
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					chaveJson, chaveJson,
					"Posicao da Master1 coincidente com Choke de Superficie.");
		}


		if (master1_json.tempo().exists() && master1_json.abertura().exists()){
		// caso os tamanhos dos vetores das chaves difiram entre si
			if (master1_json.tempo().size()
					!= master1_json.abertura().size()) {
			// RN-082: chaves tempo, abertura com tamanhos diferentes
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chaves #/master1/tempo, #/master1/abertura com tamanhos diferentes");
			} else {
				master1.parserie = (int) master1_json.abertura().size();
				master1.abertura = new double[master1.parserie];
				master1.tempo = new double[master1.parserie];
				for (int i = 0; i < master1.parserie; i++) {
					master1.abertura[i] = master1_json.abertura()[i];
					master1.tempo[i] = master1_json.tempo()[i];
				}
			}
		}
		else{
			master1.parserie = 1;
			master1.abertura = new double[master1.parserie];
			master1.tempo = new double[master1.parserie];
			for (int i = 0; i < master1.parserie; i++) {
				master1.abertura[i] = 1;
				master1.tempo[i] = 0.;
			}
		}
		eventoabre = 0;
		eventofecha = 0;
		Tevento = new double[master1.parserie];
		Teventof = new double[master1.parserie];

		// caso exista uma condicao de falha da aplicacao ate esta etapa
			if (!logger.getStResultadoSimulacao().sucesso) {
				// gerar o arquivo de saida da simulacao e encerra a aplicacao
				logger.write_logs_and_exit();
			}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "master2" do arquivo Json do MRT em struct master2.
 *
 * \tparam Objeto JSON correspondente a master2.
 */
void Ler::parse_master2(JSON_entrada_master2& master2_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/master2");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(master2_json)) {
			master2.curvaCV=0;
			master2.ncv=0;
			// caso os tamanhos dos vetores das chaves difiram entre si
			if (master2_json.tempo().size()
					!= master2_json.abertura().size()) {
				// RN-079: chaves tempo, abertura com tamanhos diferentes
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chaves #/master2/tempo, #/master2/abertura com tamanhos diferentes");
			} else {
				master2.parserie = (int) master2_json.abertura().size();
				master2.abertura = new double[master2.parserie];
				master2.tempo = new double[master2.parserie];
				master2.comp = master2_json.comprimentoMedido();

				double lverif = master2.comp;
				master2.posic = buscaIndiceFrontG(lverif);

				for (int i = 0; i < master2.parserie; i++) {
					master2.abertura[i] =
							master2_json.abertura()[i];
					master2.tempo[i] = master2_json.tempo()[i];
				}
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
		else{
			master2.curvaCV=0;
			master2.ncv=0;
			master2.parserie = 1;
			master2.abertura = new double[master2.parserie];
			master2.tempo = new double[master2.parserie];
			master2.comp = 0;
			double lverif = master2.comp;
			master2.posic = buscaIndiceFrontG(lverif);
			master2.abertura[0] =1;
			master2.tempo[0] = 0;
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "separador" do arquivo Json do MRT em struct PSEP.
 *
 * \tparam Objeto JSON correspondente a separador.
 */
void Ler::parse_separador(JSON_entrada_separador& separador_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/separador");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(separador_json)) {
			// caso os tamanhos dos vetores das chaves difiram entre si
			if (separador_json.tempo().size()
					!= separador_json.pressao().size()) {
				// RN-084: chaves tempo, abertura com tamanhos diferentes
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chaves #/separador/tempo, #/separador/pressao com tamanhos diferentes");
			} else {
				if (controDesc == 0) {  //alteracao5
					psep.parserie = (int) separador_json.pressao().size();
					psep.pres = new double[psep.parserie];
					psep.tempo = new double[psep.parserie];
					for (int i = 0; i < psep.parserie; i++) {
						psep.pres[i] = separador_json.pressao()[i];
						psep.tempo[i] = separador_json.tempo()[i];
					}
				} else {
					psep.parserie = 1;
					psep.pres = new double[psep.parserie];
					psep.tempo = new double[psep.parserie];

					for (int i = 0; i < psep.parserie; i++) {
						psep.pres[i] = 10.;
						psep.tempo[i] = 0.;
					}
				}
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_correcao(JSON_entrada_correcao& correcao_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/correcao");
	correcao.parserieHidro=0;
	correcao.parserieFric=0;
	correcao.parserieDT=0;
	correcao.dPdLHidro =0;
	correcao.dPdLFric =0;
	correcao.dTdL =0;
	try {
		if (is_ativo(correcao_json)) {
		// caso a propriedade "ativo" esteja habilitada
			if (correcao_json.dPdLHidro().exists()){
				correcao.parserieHidro = (int) correcao_json.dPdLHidro().size();
				correcao.dPdLHidro = new double[correcao.parserieHidro];
				for (int i = 0; i < correcao.parserieHidro; i++)correcao.dPdLHidro[i] = correcao_json.dPdLHidro()[i];
			}
			if (correcao_json.dPdLFric().exists()){
				correcao.parserieFric = (int) correcao_json.dPdLFric().size();
				correcao.dPdLFric = new double[correcao.parserieFric];
				for (int i = 0; i < correcao.parserieFric; i++)correcao.dPdLFric[i] = correcao_json.dPdLFric()[i];
			}
			if (correcao_json.dTdL().exists()){
				correcao.parserieDT = (int) correcao_json.dTdL().size();
				correcao.dTdL = new double[correcao.parserieDT];
				for (int i = 0; i < correcao.parserieDT; i++)correcao.dTdL[i] = correcao_json.dTdL()[i];
			}

			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "chokeSup" do arquivo Json do MRT em struct chokep.
 *
 * \tparam Objeto JSON correspondente a chokeSup
 */
void Ler::parse_chokeSup(JSON_entrada_chokeSup& chokeSup_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/chokeSup");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(chokeSup_json)) {
			chokep.curvaCV=0;
			chokep.curvaDinamic=0;
			chokep.ncv=0;
			if (chokeSup_json.curvaCV().exists())
				chokep.curvaCV = chokeSup_json.curvaCV();
			if (chokeSup_json.curvaDinamic().exists())
				chokep.curvaDinamic = chokeSup_json.curvaDinamic();
			if(chokep.curvaCV==1){
				if (!chokeSup_json.x1().exists() || !chokeSup_json.cv1().exists()){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/choke/x1 ou #/choke/cv1 inexistente");
				}
				if (chokeSup_json.x1().size() != chokeSup_json.cv1().size()){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves dimensao de #/choke/x1 diferente de  #/choke/cv1");
				}
				chokep.ncv =chokeSup_json.x1().size()-1;
				chokep.cvCurv= new detCV [chokep.ncv];
				for(int konta=0;konta<chokep.ncv;konta++){
					chokep.cvCurv[konta].x1=chokeSup_json.x1()[konta];
					chokep.cvCurv[konta].x2=chokeSup_json.x1()[konta+1];
					chokep.cvCurv[konta].cv1=chokeSup_json.cv1()[konta];
					chokep.cvCurv[konta].cv2=chokeSup_json.cv1()[konta+1];
				}
			}
			// caso os tamanhos dos vetores das chaves difiram entre si
			if (chokeSup_json.tempo().size()
					!= chokeSup_json.abertura().size()) {
				// RN-084: chaves tempo, abertura com tamanhos diferentes
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chaves #/chokeSup/tempo, #/chokeSup/abertura com tamanhos diferentes");
			} else {
				if (controDesc == 0) {  //alteracao5
					chokep.parserie = (int) chokeSup_json.abertura().size();
					chokep.abertura = new double[chokep.parserie];
					chokep.tempo = new double[chokep.parserie];
					chokep.cd =0.84;
					if (chokeSup_json.coeficienteDescarga().exists())
					chokep.cd =
							chokeSup_json.coeficienteDescarga();
					chokep.modelo = 0.;
					if (chokeSup_json.modelo().exists())
					chokep.modelo = chokeSup_json.modelo();

					for (int i = 0; i < chokep.parserie; i++) {
						chokep.abertura[i] =
								chokeSup_json.abertura()[i];
						chokep.tempo[i] = chokeSup_json.tempo()[i];
					}
				} else {
					chokep.parserie = 1;
					chokep.abertura = new double[chokep.parserie];
					chokep.tempo = new double[chokep.parserie];
					chokep.cd = 1;
					chokep.modelo = 0;

					for (int i = 0; i < chokep.parserie; i++) {
						chokep.abertura[i] = 0.95;
						chokep.tempo[i] = 0.;
					}
				}
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::parse_chokeInj(JSON_entrada_chokeInj& chokeInj_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/chokeInj");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(chokeInj_json)) {
			chokes.curvaCV=0;
			chokes.ncv=0;
			// caso os tamanhos dos vetores das chaves difiram entre si
			if (chokeInj_json.tempo().size()
					!= chokeInj_json.abertura().size()) {
				// RN-084: chaves tempo, abertura com tamanhos diferentes
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						chaveJson, chaveJson,
						"Chaves #/chokeInj/tempo, #/chokeInj/abertura com tamanhos diferentes");
			} else {
				if (controDesc == 0) {  //alteracao5
					chokes.parserie = (int) chokeInj_json.abertura().size();
					chokes.abertura = new double[chokes.parserie];
					chokes.tempo = new double[chokes.parserie];
					chokes.cd =0.9;
					if (chokeInj_json.coeficienteDescarga().exists())
					chokes.cd =
							chokeInj_json.coeficienteDescarga();
					chokes.modelo = 0;

					for (int i = 0; i < chokes.parserie; i++) {
						chokes.abertura[i] =
								chokeInj_json.abertura()[i];
						chokes.tempo[i] = chokeInj_json.tempo()[i];
					}
				} else {
					chokes.parserie = 1;
					chokes.abertura = new double[chokep.parserie];
					chokes.tempo = new double[chokep.parserie];
					chokes.cd = 1;
					chokes.modelo = 0;

					for (int i = 0; i < chokes.parserie; i++) {
						chokes.abertura[i] = 1.0;
						chokes.tempo[i] = 0.;
					}
				}
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "chokeSup" do arquivo Json do MRT em struct chokep.
 *
 * \tparam Objeto JSON correspondente a chokeSup
 */
void Ler::parse_fontechk(JSON_entrada_fonteChoke& fontechk_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fontechk");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(fontechk_json)) {
			nfontechk = 1;
			if (nfontechk > 0) {
				// caso os tamanhos dos vetores das chaves difiram entre si
				if (fontechk_json.tempo().size()
						!= fontechk_json.abertura().size()) {
					// RN-084: chaves tempo, abertura com tamanhos diferentes
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, chaveJson,
							chaveJson,
							"Chaves #/fontechk/tempo, #/fontechk/abertura com tamanhos diferentes");
				} else {
					fontechk = new detCHOKESUP[nfontechk];
					for (int i = 0; i < nfontechk; i++) {
						fontechk[i].curvaCV=0;
						fontechk[i].parserie = fontechk_json.abertura().size();
						fontechk[i].abertura =
								new double[fontechk[i].parserie];
						fontechk[i].tempo =
								new double[fontechk[i].parserie];
						fontechk[i].cd =
								fontechk_json.coeficienteDescarga();
						fontechk[i].modelo = fontechk_json.modelo();

						for (int j = 0; j < fontechk[i].parserie; j++) {
							fontechk[i].abertura[j] =
									fontechk_json.abertura()[j];
							fontechk[i].tempo[j] =
									fontechk_json.tempo()[j];
						}
					}
					// caso exista uma condicao de falha da aplicacao ate esta etapa
						if (!logger.getStResultadoSimulacao().sucesso) {
							// gerar o arquivo de saida da simulacao e encerra a aplicacao
							logger.write_logs_and_exit();
						}
				}
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "pig" do arquivo Json do MRT em struct pig.
 *
 * \tparam Objeto JSON correspondente a pig.
 */
void Ler::parse_pig(JSON_entrada_pig& pig_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/pig");
	// criar vetor de inteiros para armazenar os ids
	//int* identificadores = NULL;
	std::vector<int> identificadores;
	// criar variavel para o maior identificador encontrado
	int maiorIdentificador = -99999;
	try {
		// tamanho do array de pigs
		npig = 0;
		// percorre o array de pigs
		for (size_t i = 0; i < pig_json.size(); i++) {
			// caso o pig esteja ativo
			if (is_ativo(pig_json[i]))
				npig++;
		}
		if (npig > 0) {
			pig = new detPig[npig];
			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[npig];
			identificadores.resize(npig);
			// loop para parse das estruturas de pigs
			int indAtivo = -1;
			for (int i = 0; i < npig; i++) {
				// enquanto a propriedade "ativo" do pig esteja desabilitada, avança
				while (!is_ativo(pig_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] = pig_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				pig[i].compL = pig_json[indAtivo].lancador();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					pig[i].compL = nCompTotalUnidadesP - pig[i].compL; //04-04-2018
				if (pig[i].compL < 0.0)
					pig[i].compL = 0.0;
				pig[i].compR = pig_json[indAtivo].recebedor();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					pig[i].compR = nCompTotalUnidadesP - pig[i].compR; //04-04-2018
				if (pig[i].compR < 0.0)
					pig[i].compR = 0.0;
				pig[i].tempo = pig_json[indAtivo].tempo();
				double lverif = pig[i].compL;
				pig[i].lanca = buscaIndiceMeioP(lverif);
				lverif = pig[i].compR;
				pig[i].receb = buscaIndiceMeioP(lverif);
				pig[i].razarea = 0.;  //18-06-2018
				if (pig_json[i].folgaArea().exists())
					pig[i].razarea =
							pig_json[indAtivo].folgaArea(); //18-06-2018
				pig[i].delpres = 0.;  //18-06-2018
				if (pig_json[i].deltaPressao().exists())
					pig[i].delpres =
							pig_json[indAtivo].deltaPressao(); //18-06-2018
				pig[i].cdPig = 1.;  //18-06-2018
				if (pig_json[i].cdPig().exists())
					pig[i].cdPig = pig_json[indAtivo].cdPig(); //18-06-2018
			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-098: Unicidade da chave 'id' em '#/pig'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}


void Ler::parse_intermitencia(JSON_entrada_intermitenciaSevera& intermitencia_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/intermitenciaSevera");
	try {
		// tamanho do vetor fonte_liquido
		nintermi = 0;
		// percorre o array de fontes de líquido
		for (size_t i = 0; i < intermitencia_json.size(); i++) {
			// caso a fonte de líquido esteja ativo
			if (is_ativo(intermitencia_json[i]))
				nintermi++;
		}
		// criar vetor de inteiros para armazenar os ids
		//int* identificadores = NULL;
		std::vector<int> identificadores;
		// criar variavel para o maior identificador encontrado
		int maiorIdentificador = -99999;

		if (nintermi > 0) {
			int iIni;
			iIni = 0;
			// declarar o vetor fontel
			intermi = new detIntermi[nintermi];

			// criar vetor de inteiros para armazenar os ids
			//identificadores = new int[ninjliq];
			identificadores.resize(nintermi);

			// loop para parse das estruturas do fontel
			int indAtivo = -1;
			for (int i = iIni; i < nintermi; i++) {
				// enquanto a propriedade "ativo" da fonte de líquido esteja desabilitada, avança
				while (!is_ativo(intermitencia_json[++indAtivo]))
					;

				// obter maior identificador
				identificadores[i] =
						intermitencia_json[indAtivo].id();
				// caso o identificador seja maior que o ultimo selecionado, substitui
				if (identificadores[i] > maiorIdentificador) {
					maiorIdentificador = identificadores[i];
				}

				double comp =
						intermitencia_json[indAtivo].inicioTrechoAcumula();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					comp = nCompTotalUnidadesP - comp; //04-04-2018
				if (comp < 0.0)
					comp = 0.0;
				intermi[i].indInicioTrechoAcumula = buscaIndiceMeioP(comp);

				comp =intermitencia_json[indAtivo].fimTrechoAcumula();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					comp = nCompTotalUnidadesP - comp; //04-04-2018
				if (comp < 0.0)comp = 0.0;
				intermi[i].indFimTrechoAcumula = buscaIndiceMeioP(comp);

				comp =intermitencia_json[indAtivo].fimTrechoColuna();
				// sentido plataforma para fundo-poco
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					comp = nCompTotalUnidadesP - comp; //04-04-2018
				if (comp < 0.0)comp = 0.0;
				intermi[i].indFimTrechoColuna = buscaIndiceMeioP(comp);
				if (intermitencia_json[indAtivo].fracaoVazioPenetracao().exists())
				intermi[i].fracaoVazioPenetracao=intermitencia_json[indAtivo].fracaoVazioPenetracao();
				else intermi[i].fracaoVazioPenetracao=0.1;

				intermi[i].criterio=intermitencia_json[indAtivo].criterio();

			}
			// verificar a unicidade dos identificadores
			if (!verificarUnicidade(identificadores)) {
				// RN-093: Unicidade da chave 'id' em '#/intermitenciaSevera'
				// incluir falha
				logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"id", chaveJson, "Unicidade da chave 'id'");
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "perfil_producao" do arquivo Json do MRT em struct fonte.
 *
 * \tparam Objeto JSON correspondente a perfil_producao.
 */
void Ler::parse_perfil_producao(
		JSON_entrada_perfilProducao& perfil_producao_json) {

	/*
	 "perfilProducao": {
	 "ativo": true,
	 "pressao": true,
	 "temperatura": true,
	 "holdup": true,
	 "bet": true,
	 "ugs": true,
	 "uls": true,
	 "ug": false,
	 "ul": false,
	 "arra": true,
	 "viscosidadeLiquido": false,
	 "viscosidadeGas": false,
	 "rhog": false,
	 "rhol": 0.5,
	 "vasaoMassicaGas": true,
	 "vasaoMassicaLiquido": true,
	 "c0": true,
	 "ud": true,
	 "RGO": true,
	 "deng": true,
	 "yco2": true,
	 "calor": false,
	 "masstrans": false,
	 "cpgas": true,
	 "cpliq": true,
	 "QLstd": true,
	 "QGstd": true,
	 "api": true,
	 "bsw": true,
	 "tempo": [0.0,1000.0,7000.0,7100.0,7500.0,8000.0,10000.0,15000.0,20000.0,30000.0,30300.0,31000.0,31500.0,32000.0,33000.0,35000.0,40000.0,45000.0,48000.0,50000.0]
	 }
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/perfilProducao");
	try {
		nvarprofp = 0;
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(perfil_producao_json)) {
			// tamanho do array de tempos
			nperfisp = (int) perfil_producao_json.tempo().size();

			if (nperfisp > 0) {
				profp.n = nperfisp;
				profp.tempo = new double[profp.n];
				for (int i = 0; i < profp.n; i++) {
					profp.tempo[i] =
							perfil_producao_json.tempo()[i];
				}

				profp.pres = 0;
				profp.temp = 0;
				profp.hol = 0;

				profp.FVH = 0; //solver de Hidratos - chris

				profp.bet = 0;
				profp.ugs = 0;
				profp.uls = 0;
				profp.ug = 0;
				profp.ul = 0;
				profp.arra = 0;
				profp.viscl = 0;
				profp.viscg = 0;
				profp.rhog = 0;
				profp.rhol = 0;
				profp.rhoo = 0;
				profp.rhoa = 0;
				profp.rhoMix = 0;
				profp.RS = 0;
				profp.masg = 0;
				profp.masl = 0;
				profp.c0 = 0;
				profp.ud = 0;
				profp.RGO = 0;
				profp.deng = 0;
				profp.yco2 = 0;
				profp.calor = 0;
				profp.masstrans = 0;
				profp.cpg = 0;
				profp.cpl = 0;
				profp.qlst = 0;
				profp.qlwst = 0;
				profp.qlstTot = 0;
				profp.qgst = 0;
				profp.api = 0;
				profp.bsw = 0;
				profp.hidro = 0;
				profp.fric = 0;
				profp.Term1 = 0;
				profp.Term2 = 0;
				profp.dengD = 0;
				profp.dengL = 0;

				profp.reyi = 0;
				profp.reye = 0;
				profp.Fr = 0;
				profp.grashi = 0;
				profp.grashe = 0;
				profp.nusi = 0;
				profp.nuse = 0;
				profp.hi = 0;
				profp.he = 0;
				profp.pri = 0;
				profp.pre = 0;

				profp.Rs=0;
				profp.Bo=0;

				profp.tAmb=0;
				profp.PrG=0;
				profp.PrL=0;
				//nvarprofp+=10;

				profp.pseudoL=0;
				profp.pseudoG=0;
				profp.pseudoM=0;

				profp.TResi=0;
				profp.redAtrito=0;

				profp.angulo=0;
				profp.diamInt=0;
				profp.TempParede=0;

				profp.dadosParafina=0;

				profp.subResfria=0;



				if (perfil_producao_json.pressao().exists()) {
				    profp.pres = perfil_producao_json.pressao();
				    if (profp.pres == 1)
					nvarprofp++;
				}

				if (perfil_producao_json.temperatura().exists()) {
				    profp.temp = perfil_producao_json.temperatura();
				    if (profp.temp == 1)
					nvarprofp++;
				}


				if (perfil_producao_json.holdup().exists()) {
					profp.hol = perfil_producao_json.holdup();
					if (profp.hol == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.FVH().exists()) { //solver de Hidratos - chris
					profp.FVH = perfil_producao_json.FVH();
					if (profp.FVH == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.bet().exists()) {
					profp.bet = perfil_producao_json.bet();
					if (profp.bet == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.ugs().exists()) {
					profp.ugs = perfil_producao_json.ugs();
					if (profp.ugs == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.uls().exists()) {
					profp.uls = perfil_producao_json.uls();
					if (profp.uls == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.ug().exists()) {
					profp.ug = perfil_producao_json.ug();
					if (profp.ug == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.ul().exists()) {
					profp.ul = perfil_producao_json.ul();
					if (profp.ul == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.arra().exists()) {
					profp.arra = perfil_producao_json.arra();
					if (profp.arra == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.viscosidadeLiquido().exists()) {
					profp.viscl =
							perfil_producao_json.viscosidadeLiquido();
					if (profp.viscl == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.viscosidadeGas().exists()) {
					profp.viscg =
							perfil_producao_json.viscosidadeGas();
					if (profp.viscg == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.rhog().exists()) {
					profp.rhog = perfil_producao_json.rhog();
					if (profp.rhog == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.rhol().exists()) {
					profp.rhol = perfil_producao_json.rhol();
					if (profp.rhol == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.rhoo().exists()) {
					profp.rhoo = perfil_producao_json.rhoo();
					if (profp.rhoo == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.rhoa().exists()) {
					profp.rhoa = perfil_producao_json.rhoa();
					if (profp.rhoa == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.rhoMix().exists()) {
					profp.rhoMix = perfil_producao_json.rhoMix();
					if (profp.rhoMix == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.RS().exists()) {
					profp.RS = perfil_producao_json.RS();
					if (profp.RS == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.vazaoMassicaGas().exists()) {
					profp.masg =
							perfil_producao_json.vazaoMassicaGas();
					if (profp.masg == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.vazaoMassicaLiquido().exists()) {
					profp.masl =
							perfil_producao_json.vazaoMassicaLiquido();
					if (profp.masl == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.c0().exists()) {
					profp.c0 = perfil_producao_json.c0();
					if (profp.c0 == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.ud().exists()) {
					profp.ud = perfil_producao_json.ud();
					if (profp.ud == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.RGO().exists()) {
					profp.RGO = perfil_producao_json.RGO();
					if (profp.RGO == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.deng().exists()) {
					profp.deng = perfil_producao_json.deng();
					if (profp.deng == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.yco2().exists()) {
					profp.yco2 = perfil_producao_json.yco2();
					if (profp.yco2 == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.calor().exists()) {
					profp.calor = perfil_producao_json.calor();
					if (profp.calor == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.masstrans().exists()) {
					profp.masstrans =
							perfil_producao_json.masstrans();
					if (profp.masstrans == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.cpgas().exists()) {
					profp.cpg = perfil_producao_json.cpgas();
					if (profp.cpg == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.cpliq().exists()) {
					profp.cpl = perfil_producao_json.cpliq();
					if (profp.cpl == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.QLstd().exists()) {
					profp.qlst = perfil_producao_json.QLstd();
					if (profp.qlst == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.QLWstd().exists()) {
					profp.qlwst = perfil_producao_json.QLWstd();
					if (profp.qlwst == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.QLstdTotal().exists()) {
					profp.qlstTot =
							perfil_producao_json.QLstdTotal();
					if (profp.qlstTot == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.QGstd().exists()) {
					profp.qgst = perfil_producao_json.QGstd();
					if (profp.qgst == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.api().exists()) {
					profp.api = perfil_producao_json.api();
					if (profp.api == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.bsw().exists()) {
					profp.bsw = perfil_producao_json.bsw();
					if (profp.bsw == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.hidro().exists()) {
					profp.hidro = perfil_producao_json.hidro();
					if (profp.hidro == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.fric().exists()) {
					profp.fric = perfil_producao_json.fric();
					if (profp.fric == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.Term1().exists()) {
					profp.Term1 = perfil_producao_json.Term1();
					if (profp.Term1 == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.Term2().exists()) {
					profp.Term2 = perfil_producao_json.Term2();
					if (profp.Term2 == 1)
						nvarprofp++;
				}


				if (perfil_producao_json.dengD().exists()) {
					profp.dengD = perfil_producao_json.dengD();
					if (profp.dengD == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.dengL().exists()) {
					profp.dengL = perfil_producao_json.dengL();
					if (profp.dengL == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.ReyInterno().exists()) {
					profp.reyi = perfil_producao_json.ReyInterno();
					if (profp.reyi == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.ReyExterno().exists()) {
					profp.reye = perfil_producao_json.ReyExterno();
					if (profp.reye == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.GrashInterno().exists()) {
					profp.grashi = perfil_producao_json.GrashInterno();
					if (profp.grashi == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.GrashExterno().exists()) {
					profp.grashe = perfil_producao_json.GrashExterno();
					if (profp.grashe == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.NusselInterno().exists()) {
					profp.nusi = perfil_producao_json.NusselInterno();
					if (profp.nusi == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.NusselExterno().exists()) {
					profp.nuse = perfil_producao_json.NusselExterno();
					if (profp.nuse == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.PrandtlInterno().exists()) {
					profp.pri = perfil_producao_json.PrandtlInterno();
					if (profp.pri == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.PrandtlExterno().exists()) {
					profp.pre = perfil_producao_json.PrandtlExterno();
					if (profp.pre == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.Froud().exists()) {
					profp.Fr = perfil_producao_json.Froud();
					if (profp.Fr == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.Rs().exists()) {
					profp.Rs = perfil_producao_json.Rs();
					if (profp.Rs == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.Bo().exists()) {
					profp.Bo = perfil_producao_json.Bo();
					if (profp.Bo == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.Hint().exists()) {
					profp.hi = perfil_producao_json.Hint();
					if (profp.hi == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.Hext().exists()) {
					profp.he = perfil_producao_json.Hext();
					if (profp.he == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.temperaturaAmbiente().exists()) {
					profp.tAmb = perfil_producao_json.temperaturaAmbiente();
					if (profp.tAmb == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.PrandtlGas().exists()) {
					profp.PrG = perfil_producao_json.PrandtlGas();
					if (profp.PrG == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.PrandtlLiquido().exists()) {
					profp.PrL = perfil_producao_json.PrandtlLiquido();
					if (profp.PrL == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.pseudoLiquido().exists() && flashCompleto==2) {
					profp.pseudoL = perfil_producao_json.pseudoLiquido();
					if (profp.pseudoL == 1)
						nvarprofp+=flup[0].npseudo;
				}

				if (perfil_producao_json.pseudoGas().exists() && flashCompleto==2) {
					profp.pseudoG = perfil_producao_json.pseudoGas();
					if (profp.pseudoG == 1)
						nvarprofp+=flup[0].npseudo;
				}

				if (perfil_producao_json.pseudoMist().exists() && flashCompleto==2) {
					profp.pseudoM = perfil_producao_json.pseudoMist();
					if (profp.pseudoM == 1)
						nvarprofp+=flup[0].npseudo;
				}

				if (perfil_producao_json.TResi().exists()) {
					profp.TResi = perfil_producao_json.TResi();
					if (profp.TResi == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.RedutorAtrito().exists()) {
					profp.redAtrito = perfil_producao_json.RedutorAtrito();
					if (profp.redAtrito == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.angulo().exists()) {
					profp.angulo = perfil_producao_json.angulo();
					if (profp.angulo == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.diametroInterno().exists()) {
					profp.diamInt = perfil_producao_json.diametroInterno();
					if (profp.diamInt == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.tempParede().exists()) {
					profp.TempParede = perfil_producao_json.tempParede();
					if (profp.TempParede == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.subResfria().exists() && existeEnvelope==1) {
					profp.subResfria = perfil_producao_json.subResfria();
					if (profp.subResfria == 1)
						nvarprofp++;
				}

				if (perfil_producao_json.dadosParafina().exists() && modoParafina==1) {
					profp.dadosParafina = perfil_producao_json.dadosParafina();
					if (profp.dadosParafina == 1)
						nvarprofp+=12;
				}
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "perfil_servico" do arquivo Json do MRT em struct fonte.
 *
 * \tparam Objeto JSON correspondente a perfil_servico.
 */
void Ler::parse_perfil_servico(
		JSON_entrada_perfilServico& perfil_servico_json) {

	/*
	 "perfilServico": {
	 "ativo": true,
	 "pressao": true,
	 "temperatura": true,
	 "ugs": true,
	 "ug": false,
	 "tensaoCisalhamento": false,
	 "viscosidadeGas": false,
	 "rhog": false,
	 "vazaoMassicaGas": false,
	 "hidro": false,
	 "fric": false,
	 "calor": false,
	 "QGstd": true,
	 "tempo": [0.0,1000.0,7000.0,7100.0,7500.0,8000.0,10000.0,15000.0,20000.0,30000.0,35000.0,35100.0,36000.0,37000.0,40000.0,42000.0,44000.0,46000.0,48000.0,50000.0]
	 }
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/perfilServico");
	try {
		nvarprofg = 0;
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(perfil_servico_json)) {
			// tamanho do array de tempos
			nperfisg = (int) perfil_servico_json.tempo().size();
			if (nperfisg > 0) {
				profg.n = (int) perfil_servico_json.tempo().size();
				profg.tempo = new double[profg.n];
				for (int i = 0; i < profg.n; i++) {
					profg.tempo[i] =
							perfil_servico_json.tempo()[i];
				}

				profg.pres = 0;
				profg.temp = 0;
				profg.ugs = 0;
				profg.ug = 0;
				profg.tens = 0;
				profg.viscg = 0;
				profg.rhog = 0;
				profg.masg = 0;
				profg.hidro = 0;
				profg.FVHG = 0; //chris - Hidratos
				profg.calor = 0;
				profg.qgst = 0;

				profg.reyi = 0;
				profg.reye = 0;
				profg.grashi = 0;
				profg.grashe = 0;
				profg.nusi = 0;
				profg.nuse = 0;
				profg.hi = 0;
				profg.he = 0;
				profg.pri = 0;
				profg.pre = 0;

				profg.tAmb=0;

				profg.angulo=0;
				profg.diamInt=0;
				profg.TempParede=0;

				profg.subResfria=0;

				if (perfil_servico_json.pressao().exists()) {
				   profg.pres = perfil_servico_json.pressao();
				   if (profg.pres == 1)
					nvarprofg++;
				}

				if (perfil_servico_json.temperatura().exists()) {
				    profg.temp = perfil_servico_json.temperatura();
				    if (profg.temp == 1)
					nvarprofg++;
				}

				//nvarprofg+=10;

				if (perfil_servico_json.ugs().exists()) {
					profg.ugs = perfil_servico_json.ugs();
					if (profg.ugs == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.ug().exists()) {
					profg.ug = perfil_servico_json.ug();
					if (profg.ug == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.tensaoCisalhamento().exists()) {
					profg.tens =
							perfil_servico_json.tensaoCisalhamento();
					if (profg.tens == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.viscosidadeGas().exists()) {
					profg.viscg =
							perfil_servico_json.viscosidadeGas();
					if (profg.viscg == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.rhog().exists()) {
					profg.rhog = perfil_servico_json.rhog();
					if (profg.rhog == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.vazaoMassicaGas().exists()) {
					profg.masg =
							perfil_servico_json.vazaoMassicaGas();
					if (profg.masg == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.QGstd().exists()) {
					profg.qgst = perfil_servico_json.QGstd();
					if (profg.qgst == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.hidro().exists()) {
					profg.hidro = perfil_servico_json.hidro();
					if (profg.hidro == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.fric().exists()) {
					profg.fric = perfil_servico_json.fric();
					if (profg.fric == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.calor().exists()) {
					profg.calor = perfil_servico_json.calor();
					if (profg.calor == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.FVHG().exists()) { //chris - Hidratos
					profg.FVHG = perfil_servico_json.FVHG();
					if (profg.FVHG == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.ReyInterno().exists()) {
					profg.reyi = perfil_servico_json.ReyInterno();
					if (profg.reyi == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.ReyExterno().exists()) {
					profg.reye = perfil_servico_json.ReyExterno();
					if (profg.reye == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.GrashInterno().exists()) {
					profg.grashi = perfil_servico_json.GrashInterno();
					if (profg.grashi == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.GrashExterno().exists()) {
					profg.grashe = perfil_servico_json.GrashExterno();
					if (profg.grashe == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.NusselInterno().exists()) {
					profg.nusi = perfil_servico_json.NusselInterno();
					if (profg.nusi == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.NusselExterno().exists()) {
					profg.nuse = perfil_servico_json.NusselExterno();
					if (profg.nuse == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.PrandtlInterno().exists()) {
					profg.pri = perfil_servico_json.PrandtlInterno();
					if (profg.pri == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.PrandtlExterno().exists()) {
					profg.pre = perfil_servico_json.PrandtlExterno();
					if (profg.pre == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.Hint().exists()) {
					profg.hi = perfil_servico_json.Hint();
					if (profg.hi == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.Hext().exists()) {
					profg.he = perfil_servico_json.Hext();
					if (profg.he == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.temperaturaAmbiente().exists()) {
					profg.tAmb = perfil_servico_json.temperaturaAmbiente();
					if (profg.tAmb == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.angulo().exists()) {
					profg.angulo = perfil_servico_json.angulo();
					if (profg.angulo == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.diametroInterno().exists()) {
					profg.diamInt = perfil_servico_json.diametroInterno();
					if (profg.diamInt == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.tempParede().exists()) {
					profg.TempParede = perfil_servico_json.tempParede();
					if (profg.TempParede == 1)
						nvarprofg++;
				}

				if (perfil_servico_json.subResfria().exists() && existeEnvelope==1) {
					profg.subResfria = perfil_servico_json.subResfria();
					if (profg.subResfria == 1)
						nvarprofg++;
				}
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}


/*!
 * Converter os elementos "tendencia_producao" do arquivo Json do MRT em struct trendp.
 *
 * \tparam Objeto JSON correspondente a tendencia_producao.
 */
void Ler::parse_tendencia_producao(
		JSON_entrada_tendP& tendencia_producao_json) {
	/*
	 "tendP": [
	 {
	 "ativo": true,
	 "comprimentoMedido": 2,
	 "dt": 20.0,
	 "pressao": true,
	 "temperatura": true,
	 "holdup": true,
	 "bet": true,
	 "ugs": true,
	 "uls": true,
	 "ug": true,
	 "ul": true,
	 "arra": true,
	 "viscosidadeLiquido": true,
	 "viscosidadeGas": true,
	 "rhog": true,
	 "rhol": true,
	 "vazaoMassicaGas": true,
	 "vazaoMassicaLiquido": true,
	 "c0": true,
	 "ud": true,
	 "RGO": true,
	 "deng": true,
	 "yco2": true,
	 "calor": true,
	 "masstrans": true,
	 "QLstd": true,
	 "QGstd": true,
	 "api": true,
	 "bsw": true,
	 "rotulo": "Fundo"
	 }
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/tendP");
	try {
		// tamanho do array de tendencias de producao
		ntendp = 0;
		// percorre o array de tendencias de producao
		for (size_t i = 0; i < tendencia_producao_json.size(); i++) {
			// caso a tendencia de producao esteja ativo
			if (is_ativo(tendencia_producao_json[i]))
				ntendp++;
		}
		if (ntendp > 0) {
			trendp = new detTRENDP[ntendp];
			nvartrendp = new int[ntendp];
			// loop para parse das estruturas de tendencias de producao
			int indAtivo = -1;
			for (int i = 0; i < ntendp; i++) {
				// enquanto a propriedade "ativo" da tendencia de producao esteja desabilitada, avança
				while (!is_ativo(tendencia_producao_json[++indAtivo]))
					;

				trendp[i].pres = 0;
				trendp[i].temp = 0;
				trendp[i].hol = 0;
				trendp[i].FVH = 0; //chris - Hidratos
				trendp[i].bet = 0;
				trendp[i].ugs = 0;
				trendp[i].uls = 0;
				trendp[i].ug = 0;
				trendp[i].ul = 0;
				trendp[i].arra = 0;
				trendp[i].viscl = 0;
				trendp[i].viscg = 0;
				trendp[i].rhog = 0;
				trendp[i].rhol = 0;
				trendp[i].rhoMix = 0;
				trendp[i].masg = 0;
				trendp[i].masl = 0;
				trendp[i].c0 = 0;
				trendp[i].ud = 0;
				trendp[i].RGO = 0;
				trendp[i].deng = 0;
				trendp[i].yco2 = 0;
				trendp[i].calor = 0;
				trendp[i].masstrans = 0;
				trendp[i].qlst = 0;
				trendp[i].qlstTot = 0;
				trendp[i].qgst = 0;
				trendp[i].api = 0;
				trendp[i].bsw = 0;
				trendp[i].hidro = 0;
				trendp[i].fric = 0;
				trendp[i].dengD = 0;
				trendp[i].dengL = 0;

				trendp[i].mlFonte = 0;
				trendp[i].mgFonte = 0;
				trendp[i].mcFonte = 0;

				trendp[i].dpB = 0;
				trendp[i].potB = 0;

				trendp[i].tempChokeJus = 0;

				trendp[i].reyi = 0;
				trendp[i].reye = 0;
				trendp[i].Fr = 0;
				trendp[i].grashi = 0;
				trendp[i].grashe = 0;
				trendp[i].nusi = 0;
				trendp[i].nuse = 0;
				trendp[i].hi = 0;
				trendp[i].he = 0;
				trendp[i].pri = 0;
				trendp[i].pre = 0;

				trendp[i].Rs = 0;
				trendp[i].Bo = 0;

				trendp[i].volJusM1PT = 0;
				trendp[i].volMonM1PT = 0;
				trendp[i].volJusM1ST = 0;
				trendp[i].volMonM1ST = 0;

				trendp[i].autoVal = 0;
				trendp[i].autoVel = 0;
				trendp[i].flutuacao = 0;
				trendp[i].diamInt = 0;
				trendp[i].TempParede = 0;
				trendp[i].dadosParafina = 0;

				trendp[i].inventarioGas = 0;
				trendp[i].inventarioLiq = 0;

				trendp[i].subResfria=0;

				nvartrendp[i] = 0;
				double lComp =
						tendencia_producao_json[indAtivo].comprimentoMedido();
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					lComp = nCompTotalUnidadesP - lComp;
				trendp[i].comp=round(lComp);
				if (lComp < 0.0)
					lComp = 0.0;
				trendp[i].posic = buscaIndiceMeioP(lComp); // tendencia_producao_json[indAtivo].celula() - 1;
				if (trendp[i].posic < 0) {
					trendp[i].posic = 0;
				} else if (trendp[i].posic >= ncelp) {
					// RN-032: Chave 'comprimentoMedido' indica posicao inexistente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Chave 'comprimentoMedido' indica posicao inexistente",
							"#/tendP/" + to_string(i),
							"Somatorio '#/dutosProducao/0..n/discretizacao/nCelulas' = "
									+ to_string(ncelp));
				}
				if(transiente==1)
				trendp[i].dt =
						tendencia_producao_json[indAtivo].dt();
				else trendp[i].dt =1.;
				if(trendp[i].dt<=0 && transiente==1){
					logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							chaveJson, chaveJson,
							"Chaves #/tendP/dt,dt com valor<=0");
				}
				else if(trendp[i].dt<=0)trendp[i].dt=1;

				trendp[i].rotulo =
						tendencia_producao_json[indAtivo].rotulo();

				if (tendencia_producao_json[indAtivo].pressao().exists()) {
				    trendp[i].pres =
						tendencia_producao_json[indAtivo].pressao();
				    if (trendp[i].pres == 1)
					  nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].temperatura().exists()) {
				    trendp[i].temp =
						tendencia_producao_json[indAtivo].temperatura();
				    if (trendp[i].temp == 1)
					  nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].holdup().exists()) {
					trendp[i].hol =
							tendencia_producao_json[indAtivo].holdup();
					if (trendp[i].hol == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].FVH().exists()) {
					trendp[i].FVH =
							tendencia_producao_json[indAtivo].FVH();
					if (trendp[i].FVH == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].bet().exists()) {
					trendp[i].bet =
							tendencia_producao_json[indAtivo].bet();
					if (trendp[i].bet == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].ugs().exists()) {
					trendp[i].ugs =
							tendencia_producao_json[indAtivo].ugs();
					if (trendp[i].ugs == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].uls().exists()) {
					trendp[i].uls =
							tendencia_producao_json[indAtivo].uls();
					if (trendp[i].uls == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].ug().exists()) {
					trendp[i].ug =
							tendencia_producao_json[indAtivo].ug();
					if (trendp[i].ug == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].ul().exists()) {
					trendp[i].ul =
							tendencia_producao_json[indAtivo].ul();
					if (trendp[i].ul == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].arra().exists()) {
					trendp[i].arra =
							tendencia_producao_json[indAtivo].arra();
					if (trendp[i].arra == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].viscosidadeLiquido().exists()) {
					trendp[i].viscl =
							tendencia_producao_json[indAtivo].viscosidadeLiquido();
					if (trendp[i].viscl == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].viscosidadeGas().exists()) {
					trendp[i].viscg =
							tendencia_producao_json[indAtivo].viscosidadeGas();
					if (trendp[i].viscg == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].rhog().exists()) {
					trendp[i].rhog =
							tendencia_producao_json[indAtivo].rhog();
					if (trendp[i].rhog == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].rhol().exists()) {
					trendp[i].rhol =
							tendencia_producao_json[indAtivo].rhol();
					if (trendp[i].rhol == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].rhoMix().exists()) {
					trendp[i].rhoMix =
							tendencia_producao_json[indAtivo].rhoMix();
					if (trendp[i].rhoMix == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].vazaoMassicaGas().exists()) {
					trendp[i].masg =
							tendencia_producao_json[indAtivo].vazaoMassicaGas();
					if (trendp[i].masg == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].vazaoMassicaLiquido().exists()) {
					trendp[i].masl =
							tendencia_producao_json[indAtivo].vazaoMassicaLiquido();
					if (trendp[i].masl == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].c0().exists()) {
					trendp[i].c0 =
							tendencia_producao_json[indAtivo].c0();
					if (trendp[i].c0 == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].ud().exists()) {
					trendp[i].ud =
							tendencia_producao_json[indAtivo].ud();
					if (trendp[i].ud == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].RGO().exists()) {
					trendp[i].RGO =
							tendencia_producao_json[indAtivo].RGO();
					if (trendp[i].RGO == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].deng().exists()) {
					trendp[i].deng =
							tendencia_producao_json[indAtivo].deng();
					if (trendp[i].deng == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].yco2().exists()) {
					trendp[i].yco2 =
							tendencia_producao_json[indAtivo].yco2();
					if (trendp[i].yco2 == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].calor().exists()) {
					trendp[i].calor =
							tendencia_producao_json[indAtivo].calor();
					if (trendp[i].calor == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].masstrans().exists()) {
					trendp[i].masstrans =
							tendencia_producao_json[indAtivo].masstrans();
					if (trendp[i].masstrans == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].QLstd().exists()) {
					trendp[i].qlst =
							tendencia_producao_json[indAtivo].QLstd();
					if (trendp[i].qlst == 1)
						nvartrendp[i]++;
				}
				else if (tendencia_producao_json[indAtivo].QOstd().exists()) {
					trendp[i].qlst =
							tendencia_producao_json[indAtivo].QOstd();
					if (trendp[i].qlst == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].QLWstd().exists()) {
					trendp[i].qlwst =
							tendencia_producao_json[indAtivo].QLWstd();
					if (trendp[i].qlwst == 1)
						nvartrendp[i]++;
				}
				else if (tendencia_producao_json[indAtivo].QOWstd().exists()) {
					trendp[i].qlwst =
							tendencia_producao_json[indAtivo].QOWstd();
					if (trendp[i].qlwst == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].QLstdTotal().exists()) {
					trendp[i].qlstTot =
							tendencia_producao_json[indAtivo].QLstdTotal();
					if (trendp[i].qlstTot == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].QGstd().exists()) {
					trendp[i].qgst =
							tendencia_producao_json[indAtivo].QGstd();
					if (trendp[i].qgst == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].api().exists()) {
					trendp[i].api =
							tendencia_producao_json[indAtivo].api();
					if (trendp[i].api == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].bsw().exists()) {
					trendp[i].bsw =
							tendencia_producao_json[indAtivo].bsw();
					if (trendp[i].bsw == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].hidro().exists()) {
					trendp[i].hidro =
							tendencia_producao_json[indAtivo].hidro();
					if (trendp[i].hidro == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].fric().exists()) {
					trendp[i].fric =
							tendencia_producao_json[indAtivo].fric();
					if (trendp[i].fric == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].dengD().exists()) {
					trendp[i].dengD =
							tendencia_producao_json[indAtivo].dengD();
					if (trendp[i].dengD == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].dengL().exists()) {
					trendp[i].dengL =
							tendencia_producao_json[indAtivo].dengL();
					if (trendp[i].dengL == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].mlFonte().exists()) {
					trendp[i].mlFonte = tendencia_producao_json[indAtivo].mlFonte();
					if (trendp[i].mlFonte == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].mgFonte().exists()) {
					trendp[i].mgFonte = tendencia_producao_json[indAtivo].mgFonte();
					if (trendp[i].mgFonte == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].mcFonte().exists()) {
					trendp[i].mcFonte = tendencia_producao_json[indAtivo].mcFonte();
					if (trendp[i].mcFonte == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].deltaPBomba().exists()) {
					trendp[i].dpB = tendencia_producao_json[indAtivo].deltaPBomba();
					if (trendp[i].dpB == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].potenciaBomba().exists()) {
					trendp[i].potB = tendencia_producao_json[indAtivo].potenciaBomba();
					if (trendp[i].potB == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].tempChokeJus().exists()) {
					trendp[i].tempChokeJus = tendencia_producao_json[indAtivo].tempChokeJus();
					if (trendp[i].tempChokeJus == 1)
						nvartrendp[i]++;
				}
				if (tendencia_producao_json[indAtivo].ReyInterno().exists()) {
					trendp[i].reyi = tendencia_producao_json[indAtivo].ReyInterno();
					if (trendp[i].reyi == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].ReyExterno().exists()) {
					trendp[i].reye = tendencia_producao_json[indAtivo].ReyExterno();
					if (trendp[i].reye == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].GrashInterno().exists()) {
					trendp[i].grashi = tendencia_producao_json[indAtivo].GrashInterno();
					if (trendp[i].grashi == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].GrashExterno().exists()) {
					trendp[i].grashe = tendencia_producao_json[indAtivo].GrashExterno();
					if (trendp[i].grashe == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].NusselInterno().exists()) {
					trendp[i].nusi = tendencia_producao_json[indAtivo].NusselInterno();
					if (trendp[i].nusi == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].NusselExterno().exists()) {
					trendp[i].nuse = tendencia_producao_json[indAtivo].NusselExterno();
					if (trendp[i].nuse == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].PrandtlInterno().exists()) {
					trendp[i].pri = tendencia_producao_json[indAtivo].PrandtlInterno();
					if (trendp[i].pri == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].PrandtlExterno().exists()) {
					trendp[i].pre = tendencia_producao_json[indAtivo].PrandtlExterno();
					if (trendp[i].pre == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].Froud().exists()) {
					trendp[i].Fr = tendencia_producao_json[indAtivo].Froud();
					if (trendp[i].Fr == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].Rs().exists()) {
					trendp[i].Rs = tendencia_producao_json[indAtivo].Rs();
					if (trendp[i].Rs == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].Bo().exists()) {
					trendp[i].Bo = tendencia_producao_json[indAtivo].Bo();
					if (trendp[i].Bo == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].Hint().exists()) {
					trendp[i].hi = tendencia_producao_json[indAtivo].Hint();
					if (trendp[i].hi == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].Hext().exists()) {
					trendp[i].he = tendencia_producao_json[indAtivo].Hext();
					if (trendp[i].he == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].volMonM1PT().exists()) {
					trendp[i].volMonM1PT = tendencia_producao_json[indAtivo].volMonM1PT();
					if (trendp[i].volMonM1PT == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].volJusM1PT().exists()) {
					trendp[i].volJusM1PT = tendencia_producao_json[indAtivo].volJusM1PT();
					if (trendp[i].volJusM1PT == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].volMonM1ST().exists()) {
					trendp[i].volMonM1ST = tendencia_producao_json[indAtivo].volMonM1ST();
					if (trendp[i].volMonM1ST == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].volJusM1ST().exists()) {
					trendp[i].volJusM1ST = tendencia_producao_json[indAtivo].volJusM1ST();
					if (trendp[i].volJusM1ST == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].diametroInterno().exists()) {
					trendp[i].diamInt = tendencia_producao_json[indAtivo].diametroInterno();
					if (trendp[i].diamInt == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].dadosParafina().exists() && modoParafina==1) {
					trendp[i].dadosParafina = tendencia_producao_json[indAtivo].dadosParafina();
					if (trendp[i].dadosParafina == 1)
						nvartrendp[i]+=5;
				}

				if (tendencia_producao_json[indAtivo].tempParede().exists()) {
					trendp[i].TempParede = tendencia_producao_json[indAtivo].tempParede();
					if (trendp[i].TempParede == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].autoVal().exists()) {
					trendp[i].autoVal = tendencia_producao_json[indAtivo].autoVal();
					if (trendp[i].autoVal == 1)
						nvartrendp[i]+=3;
				}

				if (tendencia_producao_json[indAtivo].autoVel().exists()) {
					trendp[i].autoVel = tendencia_producao_json[indAtivo].autoVel();
					if (trendp[i].autoVel == 1)
						nvartrendp[i]+=9;
				}

				if (tendencia_producao_json[indAtivo].flutuacao().exists()) {
					trendp[i].flutuacao = tendencia_producao_json[indAtivo].flutuacao();
					if (trendp[i].flutuacao == 1)
						nvartrendp[i]+=3;
				}

				if (tendencia_producao_json[indAtivo].inventarioGas().exists()) {
					trendp[i].inventarioGas = tendencia_producao_json[indAtivo].inventarioGas();
					if (trendp[i].inventarioGas == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].inventarioLiq().exists()) {
					trendp[i].inventarioLiq = tendencia_producao_json[indAtivo].inventarioLiq();
					if (trendp[i].inventarioLiq == 1)
						nvartrendp[i]++;
				}

				if (tendencia_producao_json[indAtivo].subResfria().exists() && existeEnvelope==1) {
					trendp[i].subResfria = tendencia_producao_json[indAtivo].subResfria();
					if (trendp[i].subResfria == 1)
						nvartrendp[i]++;
				}
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "tendencia_servico" do arquivo Json do MRT em struct trendg.
 *
 * \tparam Objeto JSON correspondente a tendencia_servico.
 */
void Ler::parse_tendencia_servico(
		JSON_entrada_tendS& tendencia_servico_json) {

	/*
	 "tendG": [
	 {
	 "ativo": true,
	 "comprimentoMedido": 30,
	 "dt": 10.0,
	 "pressao": true,
	 "temperatura": true,
	 "ugs": true,
	 "ug": true,
	 "tensaoCisalhamento": false,
	 "viscosidadeGas": true,
	 "rhog": true,
	 "vazaoMassicaGas": false,
	 "hidro": false,
	 "fric": false,
	 "calor": false,
	 "QGstd": true,
	 "rotulo": "RotuloTendG"
	 }
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/tendG");
	try {
		// tamanho do array de tendencias de servico
		ntendg = 0;
		// percorre o array de tendencias de servico
		for (size_t i = 0; i < tendencia_servico_json.size(); i++) {
			// caso a tendencia de servico esteja ativo
			if (is_ativo(tendencia_servico_json[i]))
				ntendg++;
		}
		if (ntendg > 0) {
			trendg = new detTRENDG[ntendg];
			nvartrendg = new int[ntendg];
			// loop para parse das estruturas de tendencias de servico
			int indAtivo = -1;
			for (int i = 0; i < ntendg; i++) {
				// enquanto a propriedade "ativo" da tendencia de servico esteja desabilitada, avança
				while (!is_ativo(tendencia_servico_json[++indAtivo]))
					;

				nvartrendg[i] = 0;
				double lComp =
						tendencia_servico_json[indAtivo].comprimentoMedido();
				trendg[i].comp=round(lComp);
				trendg[i].posic = buscaIndiceMeioG(lComp); // tendencia_servico_json[indAtivo].celula() - 1
				if (trendg[i].posic < 0) {
					trendg[i].posic = 0;
				} else if (trendg[i].posic >= ncelg) {
					// RN-033: Chave 'comprimentoMedido' indica posicao inexistente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Chave 'comprimentoMedido' indica posicao inexistente",
							"#/tendG/" + to_string(i),
							"Somatorio '#/dutosServico/0..n/discretizacao/nCelulas' = "
									+ to_string(ncelg));
				}
				if(transiente==1)
				trendg[i].dt =
						tendencia_servico_json[indAtivo].dt();
				else trendg[i].dt =1.;
				if(trendg[i].dt<=0 && transiente==1){
					logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							chaveJson, chaveJson,
							"Chaves #/tendG/dt,dt com valor<=0");
				}
				else if(trendg[i].dt<=0)trendg[i].dt=1;
				trendg[i].rotulo =
						tendencia_servico_json[indAtivo].rotulo();

				trendg[i].pres = 0;
				trendg[i].temp = 0;
				trendg[i].ugs = 0;
				trendg[i].ug = 0;
				trendg[i].tens = 0;
				trendg[i].viscg = 0;
				trendg[i].rhog = 0;
				trendg[i].masg = 0;
				trendg[i].hidro = 0;
				trendg[i].fric = 0;
				trendg[i].FVHG = 0; //chris - Hidratos
				trendg[i].calor = 0;
				trendg[i].qgst = 0;
				trendg[i].pEstagVGL = 0;
				trendg[i].tEstagVGL = 0;
				trendg[i].pGargVGL = 0;
				trendg[i].tGargVGL = 0;
				trendg[i].qVGL = 0;
				trendg[i].velgarg = 0;

				trendg[i].reyi = 0;
				trendg[i].reye = 0;
				trendg[i].grashi = 0;
				trendg[i].grashe = 0;
				trendg[i].nusi = 0;
				trendg[i].nuse = 0;
				trendg[i].hi = 0;
				trendg[i].he = 0;
				trendg[i].pri = 0;
				trendg[i].pre = 0;
				trendg[i].diamInt = 0;
				trendg[i].TempParede = 0;
				trendg[i].subResfria=0;

				if (tendencia_servico_json[indAtivo].pressao().exists()) {
				    trendg[i].pres =
						tendencia_servico_json[indAtivo].pressao();
				    if (trendg[i].pres == 1)
					nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].temperatura().exists()) {
				    trendg[i].temp =
						tendencia_servico_json[indAtivo].temperatura();
				    if (trendg[i].temp == 1)
					nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].ugs().exists()) {
					trendg[i].ugs =
							tendencia_servico_json[indAtivo].ugs();
					if (trendg[i].ugs == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].ug().exists()) {
					trendg[i].ug =
							tendencia_servico_json[indAtivo].ug();
					if (trendg[i].ug == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].tensaoCisalhamento().exists()) {
					trendg[i].tens =
							tendencia_servico_json[indAtivo].tensaoCisalhamento();
					if (trendg[i].tens == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].viscosidadeGas().exists()) {
					trendg[i].viscg =
							tendencia_servico_json[indAtivo].viscosidadeGas();
					if (trendg[i].viscg == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].rhog().exists()) {
					trendg[i].rhog =
							tendencia_servico_json[indAtivo].rhog();
					if (trendg[i].rhog == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].vazaoMassicaGas().exists()) {
					trendg[i].masg =
							tendencia_servico_json[indAtivo].vazaoMassicaGas();
					if (trendg[i].masg == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].hidro().exists()) {
					trendg[i].hidro =
							tendencia_servico_json[indAtivo].hidro();
					if (trendg[i].hidro == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].fric().exists()) {
					trendg[i].fric =
							tendencia_servico_json[indAtivo].fric();
					if (trendg[i].fric == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].FVHG().exists()) { //chris - Hidratos
					trendg[i].FVHG =
							tendencia_servico_json[indAtivo].FVHG();
					if (trendg[i].FVHG == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].calor().exists()) {
					trendg[i].calor =
							tendencia_servico_json[indAtivo].calor();
					if (trendg[i].calor == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].QGstd().exists()) {
					trendg[i].qgst =
							tendencia_servico_json[indAtivo].QGstd();
					if (trendg[i].qgst == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].presEstagVGL().exists()) {
					trendg[i].pEstagVGL =
							tendencia_servico_json[indAtivo].presEstagVGL();
					if (trendg[i].pEstagVGL == 1)
						nvartrendg[i]++;
				}
				if (tendencia_servico_json[indAtivo].tempEstagVGL().exists()) {
					trendg[i].tEstagVGL =
							tendencia_servico_json[indAtivo].tempEstagVGL();
					if (trendg[i].tEstagVGL == 1)
						nvartrendg[i]++;
				}
				if (tendencia_servico_json[indAtivo].presGargVGL().exists()) {
					trendg[i].pGargVGL =
							tendencia_servico_json[indAtivo].presGargVGL();
					if (trendg[i].pGargVGL == 1)
						nvartrendg[i]++;
				}
				if (tendencia_servico_json[indAtivo].tempGargVGL().exists()) {
					trendg[i].tGargVGL =
							tendencia_servico_json[indAtivo].tempGargVGL();
					if (trendg[i].tGargVGL == 1)
						nvartrendg[i]++;
				}
				if (tendencia_servico_json[indAtivo].vazaoVGL().exists()) {
					trendg[i].qVGL =
							tendencia_servico_json[indAtivo].vazaoVGL();
					if (trendg[i].qVGL == 1)
						nvartrendg[i]++;
				}

				trendg[i].velgarg = 0;  //alteracao5
				if (tendencia_servico_json[indAtivo].VelocidadeMaximaGarganta().exists()) {
					trendg[i].velgarg =
							tendencia_servico_json[indAtivo].VelocidadeMaximaGarganta();
					if (trendg[i].velgarg == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].ReyInterno().exists()) {
					trendg[i].reyi = tendencia_servico_json[indAtivo].ReyInterno();
					if (trendg[i].reyi == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].ReyExterno().exists()) {
					trendg[i].reye = tendencia_servico_json[indAtivo].ReyExterno();
					if (trendg[i].reye == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].GrashInterno().exists()) {
					trendg[i].grashi = tendencia_servico_json[indAtivo].GrashInterno();
					if (trendg[i].grashi == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].GrashExterno().exists()) {
					trendg[i].grashe = tendencia_servico_json[indAtivo].GrashExterno();
					if (trendg[i].grashe == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].NusselInterno().exists()) {
					trendg[i].nusi = tendencia_servico_json[indAtivo].NusselInterno();
					if (trendg[i].nusi == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].NusselExterno().exists()) {
					trendg[i].nuse = tendencia_servico_json[indAtivo].NusselExterno();
					if (trendg[i].nuse == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].PrandtlInterno().exists()) {
					trendg[i].pri = tendencia_servico_json[indAtivo].PrandtlInterno();
					if (trendg[i].pri == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].PrandtlExterno().exists()) {
					trendg[i].pre = tendencia_servico_json[indAtivo].PrandtlExterno();
					if (trendg[i].pre == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].Hint().exists()) {
					trendg[i].hi = tendencia_servico_json[indAtivo].Hint();
					if (trendg[i].hi == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].Hext().exists()) {
					trendg[i].he = tendencia_servico_json[indAtivo].Hext();
					if (trendg[i].he == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].diametroInterno().exists()) {
					trendg[i].diamInt = tendencia_servico_json[indAtivo].diametroInterno();
					if (trendg[i].diamInt == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].tempParede().exists()) {
					trendg[i].TempParede = tendencia_servico_json[indAtivo].tempParede();
					if (trendg[i].TempParede == 1)
						nvartrendg[i]++;
				}

				if (tendencia_servico_json[indAtivo].subResfria().exists() && existeEnvelope==1) {
					trendg[i].subResfria = tendencia_servico_json[indAtivo].subResfria();
					if (trendg[i].subResfria == 1)
						nvartrendg[i]++;
				}
			}
// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "perfis_trans_p_json" do arquivo Json do MRT em struct proftransp.
 *
 * \tparam perfis_trans_producao_json Objeto JSON correspondente a perfis_trans_p_json.
 */
void Ler::parse_perfis_trans_producao(
		JSON_entrada_perfisTransP& perfis_trans_producao_json) {
	/*
	 "perfisTransP": {
	 "ativo": true,
	 "posicao": [1,150],
	 "tempo": [0.0,1000.0,3000.0,7000.0,9500.0,10000.0,10500.0,15000.0,20000.0,30000.0,30010.0,30050.0,30100.0,31000.0,31500.0,32000.0,35000.0,40000.0,45000.0,50000.0]
	 }
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/perfisTransP");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(perfis_trans_producao_json)) {
			nperfistransp =
					(int) perfis_trans_producao_json.comprimentoMedido().size();
			if (nperfistransp > 0) {
				proftransp.posic = new int[nperfistransp];
				proftransp.n = (int) perfis_trans_producao_json.tempo().size();
				proftransp.tempo = new double[proftransp.n];
				for (int i = 0; i < nperfistransp; i++) {
					double lComp =
							perfis_trans_producao_json.comprimentoMedido()[i];
					if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
						lComp = nCompTotalUnidadesP - lComp;
					if (lComp < 0.0)
						lComp = 0.0;
					proftransp.posic[i] = buscaIndiceMeioP(lComp);
					if (proftransp.posic[i] < 0) {
						proftransp.posic[i] = 0;
					} else if (proftransp.posic[i] >= ncelp) {
						// RN-037: Chave 'comprimentoMedido' indica posicao inexistente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Chave 'comprimentoMedido' indica celula inexistente",
								"#/perfisTransP/comprimentoMedido/"
										+ to_string(i),
								"Somatorio '#/dutosProducao/0..n/discretizacao/nCelulas' = "
										+ to_string(ncelp));
					}
				}

				// caso o vetor de tempos nao seja positivo e crescente
				if (!isArrayPositivoCrescente(
						perfis_trans_producao_json.tempo())) {
					// RN-038: chave tempo nao positiva e crescente
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"#/perfisTransP/tempo", "#/perfisTransP/tempo",
							"Chave contem valores nao crescentes");
				}

				for (int i = 0; i < proftransp.n; i++) {
					proftransp.tempo[i] =
							perfis_trans_producao_json.tempo()[i];
				}
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "perfis_trans_g_json" do arquivo Json do MRT em struct proftransg.
 *
 * \tparam Objeto JSON correspondente a perfis_trans_g_json.
 */
void Ler::parse_perfis_trans_servico(
		JSON_entrada_perfisTransS& perfis_trans_servico_json) {

	/*
	 "perfisTransG": {
	 "ativo": true,
	 "posicao": [210],
	 "tempo": [0.0,1000.0,3000.0,7000.0,9500.0,10000.0,10500.0,15000.0,20000.0,30000.0,30010.0,30050.0,30100.0,31000.0,31500.0,32000.0,35000.0,40000.0,45000.0,50000.0]
	 }
	 */
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/perfisTransG");
	try {
		// caso a propriedade "ativo" esteja habilitada
		if (is_ativo(perfis_trans_servico_json)) {
			nperfistransg =
					(int) perfis_trans_servico_json.comprimentoMedido().size();
			if (nperfistransg > 0) {
				proftransg.posic = new int[nperfistransg];
				proftransg.n = (int) perfis_trans_servico_json.tempo().size();
				proftransg.tempo = new double[proftransg.n];

				for (int i = 0; i < nperfistransg; i++) {
					double lComp =
							perfis_trans_servico_json.comprimentoMedido()[i];
					if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
						lComp = nCompTotalUnidadesP - lComp;
					if (lComp < 0.0)
						lComp = 0.0;
					proftransg.posic[i] = buscaIndiceMeioP(lComp);
					if (proftransg.posic[i] < 0) {
						proftransg.posic[i] = 0;
					} else if (proftransg.posic[i] >= ncelg) {
						// RN-039: Chave 'comprimentoMedido' indica posicao inexistente
						// incluir falha
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"Chave 'comprimentoMedido' indica celula inexistente",
								"#/perfisTransG/comprimentoMedido/"
										+ to_string(i),
								"Somatorio '#/dutosServico/0..n/discretizacao/nCelulas' = "
										+ to_string(ncelg));
					}
				}

				// caso o vetor de tempos nao seja positivo e crescente
				if (!isArrayPositivoCrescente(
						perfis_trans_servico_json.tempo())) {
					// RN-040: chave tempo nao positiva e crescente
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"#/perfisTransG/tempo", "#/perfisTransG/tempo",
							"Chave contem valores nao crescentes");
				}

				for (int i = 0; i < proftransg.n; i++) {
					proftransg.tempo[i] =
							perfis_trans_servico_json.tempo()[i];
				}
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "tendencia_trans_p_json" do arquivo Json do MRT em struct trendtransg.
 *
 * \tparam tendencia_trans_producao_json Objeto JSON correspondente a tendencia_trans_p_json.
 */
void Ler::parse_tendencia_trans_producao(
		JSON_entrada_tendTransP& tendencia_trans_producao_json) {
	/*
	 "tendTransP": [ { "ativo": true, "celula": 2, "camada": 2, "discretizacao": 1, "dt": 20.0, "rotulo": "FundoTemp" } ]
	 */
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/tendTransP");
	try {
		// tamanho do array de tendencias trans producao
		ntendtransp = 0;
		// percorre o array de tendencias trans producao
		for (size_t i = 0; i < tendencia_trans_producao_json.size();
				i++) {
			// caso a tendencia trans producao esteja ativo
			if (is_ativo(tendencia_trans_producao_json[i]))
				ntendtransp++;
		}
		if (ntendtransp > 0) {
			trendtransp = new detTRENDTrans[ntendtransp];
			// loop para parse das estruturas de tendencias trans producao
			int indAtivo = -1;
			for (int i = 0; i < ntendtransp; i++) {
				// enquanto a propriedade "ativo" da tendencia trans producao esteja desabilitada, avança
				while (!is_ativo(tendencia_trans_producao_json[++indAtivo]))
					;
				double lComp =
						tendencia_trans_producao_json[indAtivo].comprimentoMedido();
				if (!sentidoGeometriaSegueEscoamento/*origemGeometria == origemGeometria_t::plataforma*/)
					lComp = nCompTotalUnidadesP - lComp;
				trendtransp[i].comp=round(lComp);
				if (lComp < 0.0)
					lComp = 0.0;
				trendtransp[i].posic = buscaIndiceMeioP(lComp); //tendencia_trans_producao_json[indAtivo].celula() - 1
				if (trendtransp[i].posic < 0) {
					trendtransp[i].posic = 0;
				} else if (trendtransp[i].posic >= ncelp) {
					// RN-034: Chave 'comprimentoMedido' indica posicao inexistente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Chave 'comprimentoMedido' indica posicao inexistente",
							"#/tendTransP/" + to_string(i),
							"Somatorio '#/dutosProducao/0..n/discretizacao/nCelulas' = "
									+ to_string(ncelp));
				}
				trendtransp[i].camada =
						tendencia_trans_producao_json[indAtivo].camada()
								- 1;
				if (trendtransp[i].camada < 0)
					trendtransp[i].camada = 0;
				trendtransp[i].discre =
						tendencia_trans_producao_json[indAtivo].discretizacao();
				if(transiente==1)
				trendtransp[i].dt =
						tendencia_trans_producao_json[indAtivo].dt();
				else trendtransp[i].dt =1.;
				if(trendtransp[i].dt<=0 && transiente==1){
					logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							chaveJson, chaveJson,
							"Chaves #/trendtransp/dt,dt com valor<=0");
				}
				else if(trendtransp[i].dt<=0)trendtransp[i].dt=1;
				trendtransp[i].rotulo =
						tendencia_trans_producao_json[indAtivo].rotulo();
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
				if (!logger.getStResultadoSimulacao().sucesso) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "tendencia_trans_servico_json" do arquivo Json do MRT em struct trendtransg.
 *
 * \tparam tendencia_trans_servico_json Objeto JSON correspondente a tendencia_trans_servico_json.
 */
void Ler::parse_tendencia_trans_servico(
		JSON_entrada_tendTransS& tendencia_trans_servico_json) {
	/*
	 "tendTransS": [ { "ativo": true, "celula": 2, "camada": 2, "discretizacao": 1, "dt": 20.0, "rotulo": "FundoTemp" } ]
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/tendTransS");
	try {
		// tamanho do array de tendencias trans servico
		ntendtransg = 0;
		// percorre o array de tendencias trans servico
		for (size_t i = 0; i < tendencia_trans_servico_json.size(); i++) {
			// caso a tendencia trans servico esteja ativo
			if (is_ativo(tendencia_trans_servico_json[i]))
				ntendtransg++;
		}
		if (ntendtransg > 0) {
			trendtransg = new detTRENDTrans[ntendtransg];
			// loop para parse das estruturas de tendencias trans servico
			int indAtivo = -1;
			for (int i = 0; i < ntendtransg; i++) {
				// enquanto a propriedade "ativo" da tendencia trans servico esteja desabilitada, avança
				while (!is_ativo(tendencia_trans_servico_json[++indAtivo]))
					;
				double lComp =
						tendencia_trans_servico_json[indAtivo].comprimentoMedido();
				trendtransg[i].comp=round(lComp);
				trendtransg[i].posic = buscaIndiceMeioG(lComp); // tendencia_trans_servico_json[indAtivo].celula() - 1;
				if (trendtransg[i].posic < 0) {
					trendtransg[i].posic = 0;
				} else if (trendtransg[i].posic >= ncelg) {
					// RN-035: Chave 'comprimentoMedido' indica posicao inexistente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Chave 'comprimentoMedido' indica posicao inexistente",
							"#/tendTransS/" + to_string(i),
							"Somatorio '#/dutosServico/0..n/discretizacao/nCelulas' = "
									+ to_string(ncelg));
				}
				trendtransg[i].camada =
						tendencia_trans_servico_json[indAtivo].camada()
								- 1;
				if (trendtransg[i].camada < 0)
					trendtransg[i].camada = 0;
				trendtransg[i].discre =
						tendencia_trans_servico_json[indAtivo].discretizacao();
				if(transiente==1)
				trendtransg[i].dt =
						tendencia_trans_servico_json[indAtivo].dt();
				else trendtransg[i].dt =1.;
				if(trendtransg[i].dt<=0 && transiente==1){
					logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							chaveJson, chaveJson,
							"Chaves #/trendtransg/dt,dt com valor<=0");
				}
				else if(trendtransg[i].dt<=0)trendtransg[i].dt=1;
				trendtransg[i].rotulo =
						tendencia_trans_servico_json[indAtivo].rotulo();
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
			if (!logger.getStResultadoSimulacao().sucesso) {
				// gerar o arquivo de saida da simulacao e encerra a aplicacao
				logger.write_logs_and_exit();
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

/*!
 * Converter os elementos "tela" do arquivo Json do MRT em struct tela.
 *
 * \tparam tela_json Objeto JSON correspondente a tela.
 */
void Ler::parse_tela(JSON_entrada_tela& tela_json) {

	/*
	 "tela": [ { "ativo": true, "coluna": 1, "celula": 0, "variavel": 1 } ]
	 */

	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/tela");
	try {
		// tamanho do array de telas
		ntela = 0;
		// percorre o array de telas
		for (size_t i = 0; i < tela_json.size(); i++) {
			// caso a tela esteja ativo
			if (is_ativo(tela_json[i]))
				ntela++;
		}
		if (ntela > 0) {
			tela = new detTela[ntela];
			// loop para parse das estruturas de tela
			int indAtivo = -1;
			for (int i = 0; i < ntela; i++) {
				// enquanto a propriedade "ativo" da tela esteja desabilitada, avança
				while (!is_ativo(tela_json[++indAtivo]))
					;

				tela[i].col = tela_json[indAtivo].coluna();
				tela[i].posic = tela_json[indAtivo].celula() - 1;
				if (tela[i].posic < 0) {
					tela[i].posic = 0;
				} else if (tela[i].posic >= ncelp) {
					// RN-036: Chave 'comprimentoMedido' indica posicao inexistente
					// incluir falha
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"Chave 'comprimentoMedido' indica posicao inexistente",
							"#/tela/" + to_string(i),
							"Somatorio '#/dutosProducao/0..n/discretizacao/nCelulas' = "
									+ to_string(ncelp));
				}
				tela[i].var = tela_json[indAtivo].variavel();
			}
			// caso exista uma condicao de falha da aplicacao ate esta etapa
			if (!logger.getStResultadoSimulacao().sucesso) {
				// gerar o arquivo de saida da simulacao e encerra a aplicacao
				logger.write_logs_and_exit();
			}
		}
	} catch (exception& e) {
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "", chaveJson, e.what());
	}
}

void Ler::lerArq() {
	try {
		// configurar os nomes dos arquivos de entrada e log
		logger.changeResultadoSimulacao(impfile, arquivoLog);

		// Caso o arquivo de entrada inexista
		if (!ifstream(impfile)) {
			// incluir log de falha
			logger.log_write_logs_and_exit(LOGGER_FALHA,
			LOG_ERR_UNEXPECTED_EXCEPTION, "", "",
					"Arquivo de entrada inexistente");
		}

		// parse do documento raiz do arquivo MRT de entrada
		JSON_entrada jsonDoc = parseEntrada();
		// caso configurada opcao de termino apos validacao do formato do arquivo JSON
		if (validacaoJson == tipoValidacaoJson_t::json) {
			// gerar o arquivo de saida da simulacao e encerra a aplicacao
			logger.write_logs_and_exit();
		}

		// caso a validacao esteja ligada
		/*if (validacaoJson != tipoValidacaoJson_t::off) {
			// criar arquivo com schema de validacao do MRT
			//if (tipoSimulacao == tipoSimulacao_t::transiente) {
				// schema para simulacao transiente
				//writeSchemaTransiente();
			//} else if (tipoSimulacao == tipoSimulacao_t::poco_injetor) {
				// schema para simulacao de poco injecao
				//writeSchemaInjecao();
			//}
			// parse do schema do MRT
			Document schemaDoc = parseSchema();
			// realizar a validacao do arquivo MRT segundo o schema
			validateVsSchema(&schemaDoc, jsonDoc.getDocument());
		}

		// caso configurada opcao de termino apos validacao do formato do arquivo JSON
		if (validacaoJson == tipoValidacaoJson_t::schema) {
			// gravar arquivo de log e abandona a simulacao
			logger.write_logs_and_exit();
		}*/

		string sistemaLido(jsonDoc.sistema());
		transform(sistemaLido.begin(), sistemaLido.end(), sistemaLido.begin(),
				::toupper);
		if (sistemaLido.compare("MULTIFASICO") == 0) {
			if (tipoSimulacao != tipoSimulacao_t::transiente
					&& tipoSimulacao != tipoSimulacao_t::rede) {
				logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "", "#sistema",
						"Tipo de simulacao especificada na linha de comando incompativel com a do arquivo de entrada.");
			}
			sistemaSimulacao = sistemaSimulacao_t::multifasico;
		} else if (sistemaLido.compare("INJETOR") == 0) {
			if (tipoSimulacao != tipoSimulacao_t::poco_injetor
					&& tipoSimulacao != tipoSimulacao_t::rede) {
				logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "", "#sistema",
						"Tipo de simulacao especificada na linha de comando incompativel com a do arquivo de entrada.");
			}
			sistemaSimulacao = sistemaSimulacao_t::injetor;
		} else if (sistemaLido.compare("OLEODUTO") == 0) {
			if (tipoSimulacao != tipoSimulacao_t::rede) {
				logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "", "#sistema",
						"Tipo de simulacao especificada na linha de comando incompativel com a do arquivo de entrada.");
			}
			sistemaSimulacao = sistemaSimulacao_t::oleoduto;
		}

		// verificar a versao do esquema de validacao do JSON
		/*string versaoJsonLida(jsonDoc.versaoJson());
		string versaoJsonCorrente = VERSAO_SCHEMA_JSON;
		if (versaoJsonLida.compare(versaoJsonCorrente) != 0) {
			logger.log_write_logs_and_exit(LOGGER_FALHA,
			LOG_ERR_UNEXPECTED_EXCEPTION, "", "#versaoJson",
					"Versao de validacao do arquivo de entrada desatualizada! Este simulador faz uso da versao " VERSAO_SCHEMA_JSON);
		}*/

		// iniciar como simulacao de poco de producao
		pocinjec = 0;
		// caso seja simulacao do sistema de injecao
		if (sistemaSimulacao == sistemaSimulacao_t::injetor
				&& contemChaveAtiva(jsonDoc, CondicaoContPocInjec)) {
			// configurar simulacao de poco de injecao
			pocinjec = 1;
		}

		// parse da chave configuracaoInicial
		parse_configuracao_inicial(jsonDoc.configuracaoInicial());

		// caso seja simulacao do sistema de injecao
		int temFluido=0;
		if (pocinjec == 1 && contemChaveAtivaArray(jsonDoc, fluidosProducao)) temFluido=1;
		if (pocinjec == 1) {
			// parse da chave CondicaoContPocInjec do sistema de injecao
			parse_condcont_pocinjec(jsonDoc.CondicaoContPocInjec(),temFluido);
		} else {
			// caso seja simulacao do sistema de producao
			condpocinj.tipoFlui = 0;
		}

		// validacao das regras de negocio antes de ler os dados do MRT para as estruturas de dados do simulador

		// caso configurado o uso de tabela para o Z do gas em linha de producao ou de servico,
		// ou poco injetor com tipoFluido 2 - CO2, chave tabela eh requerida
		if ((tabp > 0 || tabg > 0 || modelcp > 0 || modelJTL ==1 || latente > 0
				/*|| (pocinjec == 1 && condpocinj.tipoFlui == 2)*/)
				&& (!contemChaveAtiva(jsonDoc, tabela))) {
			// RN-001: chave tabela requerida, caso configurado o uso de tabela para o Z do gas em linha de producao ou de servico, ou poco injetor com tipoFluido 2 - CO2
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"#/tabela", "#",
					"Chaves #/configuracaoInicial/tabP ou #/configuracaoInicial/tabG ");
		}

		if (modoParafina == 1) {
			// parse da chave CondicaoContPocInjec do sistema de injecao
			parse_parafina(jsonDoc.parafina());
		}

		// caso seja simulacao do sistema de producao e a chave "fluidosProducao" inexista ou esteja inativa
		if (pocinjec == 0 && !contemChaveAtivaArray(jsonDoc, fluidosProducao)) {
			// RN-002: chave fluidosProducao requerida, caso sistema de producao
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"#/fluidosProducao", "#",
					"Simulacao de sistema de producao requer fluido de producao");
		}
		// caso sistema de injecao com tipofluido 0 - a definir pelo usuario - e a chave "fluidoComplementar" inexista ou esteja inativa
		if (pocinjec == 1 && condpocinj.tipoFlui == 0
				&& !contemChaveAtiva(jsonDoc, fluidoComplementar)) {
			// RN-003: chave fluidoComplementar requerida, caso sistema de injecao comn tipofluido 0
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"#/fluidoComplementar", "#",
					"Simulacao de sistema de injecao com tipofluido 0 - a definir pelo usuario");
		}
		// caso possua linha de servico ou fonte de gas ou fonte de gas lift, fluido gas eh requerida
		// e a chave "fluidoGas" inexista ou esteja inativa
		if (((pocinjec == 0 && lingas == 1) || contemChaveAtivaArray(jsonDoc, fonteGas)
				|| contemChaveAtivaArray(jsonDoc, fonteGasLift))
				&& !contemChaveAtiva(jsonDoc, fluidoGas)) {
			// RN-004: chave fluidoGas requerida, caso possua linha de servico ou fonte de gas ou fonte de gas lift
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"#/fluidoGas", "#",
					"Chave requerida para linha de servico ou fonte de gas ou fonte de gas lift");
		}
		// flag para habilitar ou nao carga dos dados do MRT para a chave fonteGasLift
		bool enableFonteGasLift = true;
		// caso nao possua linha de servico e a chave "fonteGasLift" esteja ativa
		if ((pocinjec == 0 && lingas == 0) && contemChaveAtivaArray(jsonDoc, fonteGasLift)) {
			// RN-005: chave fonteGasLift indevida, caso sistema sem linha de servico
			logger.log(LOGGER_AVISO, LOG_WARN_PARSE_BUSINESS_RULE_VALIDATION,
					"#/fonteGasLift", "#",
					"Chave indevida para sistema sem linha de servico");
			enableFonteGasLift = false;
		}
		// caso possua linha de servico, em simulacao permanente, e a fonte de gas lift inexistente ou inativa
		if ((pocinjec == 0 && lingas == 1) && perm == 1
				&& !contemChaveAtivaArray(jsonDoc, fonteGasLift)) {
			// RN-006: chave fonteGasLift requerida, caso sistema com linha de servico
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"#/fonteGasLift", "#",
					"Chave requerida em simulacao permanente de sistema com linha de servico");
		}

		// obter os valores dos atributos do elemento "material" do Json
		parse_materiais(jsonDoc.material());
		// obter os valores dos atributos do elemento "secaoTransversal" do Json
		parse_corte(jsonDoc.secaoTransversal());

		// caso seja simulacao do sistema de producao
		if (pocinjec == 0 || transiente==0) {
			// obter os valores dos atributos do elemento "tempo" do Json
			parse_tempo(jsonDoc.tempo());
		} else {
			// definir valores default para os atributos do elemento "tempo" do MRT
			tfinal = 0.;
			dtmaxserie.parserie = 1;
			dtmaxserie.tempo = new double[dtmaxserie.parserie];
			dtmaxserie.valor = new double[dtmaxserie.parserie];
			dtmaxserie.tempo[0] = 0;
			dtmaxserie.valor[0] = 1.;
			dtmax = dtmaxserie.valor[0];
		}
		// caso a chave "tabela" exista ativa
		if (contemChaveAtiva(jsonDoc, tabela)
				&& (tabp > 0 || tabg > 0 || modelcp > 0 || modelJTL == 1
						|| latente > 0
						/*|| (pocinjec == 1 && (condpocinj.tipoFlui == 2))*/)) {
			// realizar o parse da chave tabela
			parse_tabela(jsonDoc.tabela());

			// caso os atributos "tabP", "tabG" da chave "configuracaoInicial" sejam true
			if (tabp > 0 || tabg > 0) {
				zdranP = new double*[npontos + 2];
				for (int i = 0; i < npontos + 2; i++)
					zdranP[i] = new double[npontos + 2];
				dzdpP = new double*[npontos + 2];
				for (int i = 0; i < npontos + 2; i++)
					dzdpP[i] = new double[npontos + 2];
				dzdtP = new double*[npontos + 2];
				for (int i = 0; i < npontos + 2; i++)
					dzdtP[i] = new double[npontos + 2];
			}
		}

		// caso a chave "fluidosProducao" exista e esteja inativa em simulacao nao poco_injetor
		if ((pocinjec == 0 || flashCompleto==1 || flashCompleto==2) && contemChaveAtivaArray(jsonDoc, fluidosProducao)) {
			// obter os valores dos atributos do elemento "fluidosProducao" do MRT
			parse_fluidos_producao(jsonDoc.fluidosProducao());
		} else {
			// definir valores default para os atributos do elemento "fluidosProducao"
			nfluP = 1;
			flup = new ProFlu [nfluP];
			flup[0] = ProFlu(vg1dSP, 20, 100., 0.7, 0., 1., 20, 10, 40,
					2, 0, 0.8, tabp, 0, 0);
			flup[0].npontos = npontos;
		}

		for (int i = 0; i < nfluP; i++) {
			flup[i].ModelCp = modelcp;
			flup[i].Modeljtl = modelJTL;
			flup[i].CalcLat = latente;
			flup[i].trackRGO = trackRGO;
			flup[i].trackDeng = trackDeng;
			if (contemChaveAtivaArray(jsonDoc, fonteGas))
				flup[i].ninjgas = (int) jsonDoc.fonteGas().size();
			else
				flup[i].ninjgas = 0.;
			flup[i].lingas = lingas;
			//if(flup[i].corrSat==3)flup[i].corrSat=-(flup[i].corrSat+1);
		}

		// caso exista elemento "fluidoComplementar"
		if ((jsonDoc.fluidoComplementar().exists() && contemChaveAtiva(jsonDoc, fluidoComplementar))
				/*&& jsonDoc.fluidoComplementar().massaEspecifica().exists()*/
				&& (pocinjec == 0 || (pocinjec == 1 && condpocinj.tipoFlui == 0))
				&& descarga == 0) {
			// obter os valores dos atributos do elemento "fluidoComplementar" do Json
			parse_fluido_complementar(jsonDoc.fluidoComplementar());
			fluc.descarga = descarga;
		} else {
			// definir valores default para os atributos do elemento "fluidoComplementar" do Json
			fluc = ProFluCol(1000., 0., 0., 1e-6, 4000., 1., 0.,
					1., 20., 40., 1.,0, 1);
			fluc.npontos = npontos;
			fluc.descarga = descarga;
			if (descarga == 1) {
				fluc.tipoF=1;
				fluc.salin = salinDescarga;
			}  //15-06-2018
			else if((pocinjec == 1 && condpocinj.tipoFlui == 1)){
				fluc.salin = condpocinj.salin;
				fluc.tipoF=1;
			}
		}

		// caso exista chave "fluidoGas" do Json
		if (jsonDoc.fluidoGas().exists() && contemChaveAtiva(jsonDoc, fluidoGas)
				&& (lingas == 1 || contemChaveAtivaArray(jsonDoc, fonteGas))
				&& pocinjec == 0) {
			// obter os valores dos atributos do elemento "fluidoGas" do Json
			parse_fluido_gas(jsonDoc.fluidoGas());
		} else {
			double api = 60.0;
			double rgo = 10000000000000.0;
			double bsw = 0.0;
			double denag = 1.0;
			double templ = 10.0;
			double lvisl = 10;
			double temph = 50.0;
			double lvish = 50;
			int tipoemul = 0;
			double bswCorte = 0.0;
			double aemul = 0.;
			double bemul = 0.;
			double PHI100=0.;
			double yco2 = 0.;
			double deng=0.7;
			int corrC = 1;
			if(flashCompleto!=2){

					flug = ProFlu(vg1dSP, api, rgo, deng, bsw, denag, templ, lvisl,
						temph, lvish, tipoemul, aemul, bemul,PHI100, bswCorte, tabg, yco2,
						corrC);
					flug.zdranP = zdranP;
					flug.dzdpP = dzdpP;
					flug.dzdtP = dzdtP;
					flug.npontos = npontos;
			}
			else{
					tabg=0;
					flug.tab=tabg;
					flug = flup[0];
					if(lingas==1){
						tabg=1;
						flug.tab=tabg;
						flug.Deng = deng;
						flug.yco2 = yco2;
						flug.corrC = corrC;
						flug.RenovaFluido();
						flug.zdranP = zdranP;
						flug.dzdpP = dzdpP;
						flug.dzdtP = dzdtP;
						flug.npontos = npontos;
					}
					compLinServ = new double[flug.npseudo];
					for(int j=1;j<flug.npseudo;j++)flug.fracMol[j]=flup[0].fracMol[j];
					flug.atualizaPropCompStandard();
			}
			//flug = ProFlu();
			//flug.npontos = npontos;
		}
		flug.ModelCp = modelcp;
		flug.Modeljtl = modelJTL;
		flug.CalcLat = latente;
		flug.trackRGO = trackRGO;
		flug.trackDeng = trackDeng;
		if (contemChaveAtivaArray(jsonDoc, fonteGas))
			flug.ninjgas = (int) jsonDoc.fonteGas().size();
		else
			flug.ninjgas = 0;
		flug.lingas = lingas;

		usaTabela = 0;
		if (flashCompleto == 1 /*|| tabRSPB>0*/)
			usaTabela = 1;  //alteracao8

		double TCMin = 0;  //15-06-2018
		double TCMax = 0;  //15-06-2018
		double PCMin = 0;  //15-06-2018
		double PCMax = 0;  //15-06-2018
		if (tabp > 0) {
			if(corrDeng==0){
			  TCMin = flup[0].TC;  //15-06-2018
			  TCMax = flup[0].TC;  //15-06-2018
			  PCMin = flup[0].PC;  //15-06-2018
			  PCMax = flup[0].PC;  //15-06-2018
			}
			else{
				double PC= 708.75 - 57.5* 0.5538;
				double TC= 169 + 314.0* 0.5538;
				TCMin = TC;
				TCMax = TC;
				PCMin = PC;
				PCMax = PC;
			}
			for (int i = 0; i < nfluP; i++) {  //15-06-2018
				if (flup[i].TC < TCMin)
					TCMin = flup[i].TC;
				if (flup[i].TC > TCMax)
					TCMax = flup[i].TC;
				if (flup[i].PC < PCMin)
					PCMin = flup[i].PC;
				if (flup[i].PC > PCMax)
					PCMax = flup[i].PC;
			}  //15-06-2018
		} else if (tabg > 0) {
			TCMin = flug.TC;  //15-06-2018
			TCMax = flug.TC;  //15-06-2018
			PCMin = flug.PC;  //15-06-2018
			PCMax = flug.PC;  //15-06-2018
		}
		if ((flug.ninjgas > 0 || lingas == 1) && tabg > 0) {  //15-06-2018
			if (flug.TC < TCMin)
				TCMin = flug.TC;
			if (flug.TC > TCMax)
				TCMax = flug.TC;
			if (flug.PC < PCMin)
				PCMin = flug.PC;
			if (flug.PC > PCMax)
				PCMax = flug.PC;
		}  //15-06-2018
		if (tabg > 0 || tabp > 0) {
			tabentCrit.npont = tabent.npont;  //15-06-2018
			tabentCrit.tmin = (tabent.tmin + 273.15)
					/ ((TCMax - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
			tabentCrit.tmax = (tabent.tmax + 273.15)
					/ ((TCMin - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
			tabentCrit.pmin = tabent.pmin * 0.9678411 * 14.69595 / PCMax; //15-06-2018
			tabentCrit.pmax = tabent.pmax * 0.9678411 * 14.69595 / PCMin; //15-06-2018
		}

		// caso nao seja simulacao de injecao
		if ((pocinjec == 0 && tabp > 0)) {  //15-06-2018
			flup[0].tabela(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
					tabentCrit.tmin);
			flup[0].tabelaDZDP(tabentCrit.pmax, tabentCrit.tmax,
					tabentCrit.pmin, tabentCrit.tmin);
			flup[0].tabelaDZDT(tabentCrit.pmax, tabentCrit.tmax,
					tabentCrit.pmin, tabentCrit.tmin);
		}  //15-06-2018
		else if (pocinjec == 0 && (flug.ninjgas > 0 || lingas == 1)
				&& tabg > 0) {
			flug.tabela(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
					tabentCrit.tmin);
			flug.tabelaDZDP(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
					tabentCrit.tmin);
			flug.tabelaDZDT(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
					tabentCrit.tmin);
		}

		if(perm==0){
			int iflu;
			for(iflu=0;iflu<nfluP;iflu++){
				if(indFlup[iflu]==indfluPIni){
					indfluPIni=iflu;
					break;
				}
			}
			if(iflu==nfluP)
				logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "#/configuracaoInicial",
						"#/configuracaoInicial",
							"Chaves #/indice de fluido de producao inexistente");
		}

		/*int npontosTeste=20;
		double pmaxTeste=300.;
		double pminTeste=1;
		double tmaxTeste=80.;
		double tminTeste=0.;
		double dtTeste=(tmaxTeste-tminTeste)/npontosTeste;
		double dpTeste=(pmaxTeste-pminTeste)/npontosTeste;
		FullMtx<double> flut(22,22,0.);
		for(int i=0;i<=npontosTeste;i++){
			flut[0][i+1]=pminTeste+i*dpTeste;
			flut[i+1][0]=tminTeste+i*dtTeste;
		}
		for(int i=0;i<=npontosTeste;i++){
			for(int j=0;j<=npontosTeste;j++){
				flut[i+1][j+1]=flup[0].RS(flut[0][i+1], flut[j+1][0]);
			}
		}
		ostringstream saidaLivia;
		saidaLivia << pathPrefixoArqSaida << "RS-Livia"<< ".dat";
		string tmp = saidaLivia.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << flut;
		escreveIni.close();*/

		// caso o atributo "tabG" do elemento "configuracaoInicial" seja true e exista elemento "fonteGas" não vazio
		/*
		 if (tabg > 0 && (lingas > 0 || contemChaveAtiva(jsonDoc, "fonteGas"))) {
		 flug.tabela(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 flug.tabelaDZDP(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 flug.tabelaDZDT(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 }

		 // caso "tabG" seja true e exista mais de um fluido de producao ou elemento "fonteGas" vazio
		 if (tabg > 0 && nfluP > 1
		 && (flup[iminDG].Deng < flug.Deng || (lingas == 0 && !contemChaveAtiva(jsonDoc, "fonteGas")))) {
		 ProFlu FluBaixaDeng(1., flup[iminDG].API, flup[iminDG].RGO, flup[iminDG].Deng, flup[iminDG].BSW,
		 flup[iminDG].Denag, flup[iminDG].TempL, flup[iminDG].LVisL, flup[iminDG].TempH, flup[iminDG].LVisH,
		 flup[iminDG].tipoemul, flup[iminDG].bswCorte, tabg, flup[iminDG].yco2, flup[iminDG].corrC);
		 FluBaixaDeng.npontos = npontos;
		 FluBaixaDeng.zdranG = zdranG;
		 FluBaixaDeng.zdranP = zdranP;
		 FluBaixaDeng.dzdpG = dzdpG;
		 FluBaixaDeng.dzdpP = dzdpP;
		 FluBaixaDeng.dzdtG = dzdtG;
		 FluBaixaDeng.dzdtP = dzdtP;
		 FluBaixaDeng.tabela(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 FluBaixaDeng.tabelaDZDP(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 FluBaixaDeng.tabelaDZDT(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 } else if (tabg > 0 && nfluP == 1 && (lingas == 0 && !contemChaveAtiva(jsonDoc, "fonteGas"))) {
		 ProFlu FluBaixaDeng(1., flup[0].API, flup[0].RGO, flup[0].Deng, flup[0].BSW, flup[0].Denag,
		 flup[0].TempL, flup[0].LVisL, flup[0].TempH, flup[0].LVisH, flup[0].tipoemul, flup[0].bswCorte, tabg,
		 flup[0].yco2, flup[0].corrC);
		 FluBaixaDeng.npontos = npontos;
		 FluBaixaDeng.zdranG = zdranG;
		 FluBaixaDeng.zdranP = zdranP;
		 FluBaixaDeng.dzdpG = dzdpG;
		 FluBaixaDeng.dzdpP = dzdpP;
		 FluBaixaDeng.dzdtG = dzdtG;
		 FluBaixaDeng.dzdtP = dzdtP;
		 FluBaixaDeng.tabela(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 FluBaixaDeng.tabelaDZDP(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 FluBaixaDeng.tabelaDZDT(tabent.pmax, tabent.tmax, tabent.pmin, tabent.tmin);
		 }
		 */
		// caso simulacao de poco de injecao e tipo de fluido CO2
		if (pocinjec == 1 && condpocinj.tipoFlui == 2 && flashCompleto!=1) {
			// gerar tabela de injecao de CO2
			geraTabInjCO2();
			fluc.RhoInj = RhoInj;
			fluc.ViscInj = ViscInj;
			fluc.CondInj = CondInj;
			fluc.CpInj = CpInj;
			fluc.DrhoDtInj = DrhoDtInj;
		}

		if (jsonDoc.correcao().exists()) parse_correcao(jsonDoc.correcao());

		if(jsonDoc.dutosProducao()[0].condicoesIniciais().exists())conINIAmbP=0;
		else if(jsonDoc.dutosProducao()[0].condicoesIniciaisEAmbiente().exists())conINIAmbP=1;

		if(lingas==1 && jsonDoc.dutosServico()[0].condicoesIniciais().exists())conINIAmbS=0;
		else if(lingas==1 && jsonDoc.dutosServico()[0].condicoesIniciaisEAmbiente().exists())conINIAmbS=1;

		// calcular a quantidade de dutos de producao
		nunidadep = 0;
		for (size_t i = 0; i < jsonDoc.dutosProducao().size(); i++) {
			// obter a propriedade do item
			JSON_entrada_dutosProducao_Item& propriedade = jsonDoc.dutosProducao()[i];
			// caso o duto de producao esteja ativo
			if (is_ativo(propriedade))
				nunidadep++;
		}

		// calcular a quantidade de dutos de servico
		nunidadeg = 0;
		if (lingas > 0 && contemChaveAtivaArray(jsonDoc, dutosServico)) {
			// percorre o array de dutos de servico
			for (size_t i = 0; i < jsonDoc.dutosServico().size(); i++) {
				// obter a propriedade do item
				JSON_entrada_dutosServico_Item& propriedade = jsonDoc.dutosServico()[i];
				// caso o duto de servico esteja ativo
				if (is_ativo(propriedade))
					nunidadeg++;
			}
		}

		// determinar a quantidade total de dutos
		nduto = nunidadep + nunidadeg;
		// instanciar vetor de dutos
		duto = new detduto[nduto];

		// parse unidades de producao
		if(conINIAmbP==0)parse_unidades_producao(jsonDoc.dutosProducao());
		else parse_unidades_producaoAmb(jsonDoc.dutosProducao());

		// gerar celulas de producao
		celp = new detcelp[ncelp];
		int tempncel = 0;
		anmP = 0 * unidadeP[0].dx[0];
		int para = 0;
		for (int i = 0; i < nunidadep; i++) {
			double posiccentro = 0.;
			double posicfronteira = 0.;
			int iniSegF = 0;
			int iniSegC = 0;
			for (int j = tempncel; j < (tempncel + unidadeP[i].ncel); j++) {
				int kontauni = j - tempncel;
				celp[j].duto = unidadeP[i].duto;
				celp[j].correlacaoMR2 = unidadeP[i].correlacaoMR2;
				celp[j].dirconv = unidadeP[i].dirconv;
				celp[j].acopcol = unidadeP[i].acopcol;
				celp[j].acopcolRede = unidadeP[i].acopcolRede;
				celp[j].dx = unidadeP[i].dx[kontauni];
				celp[j].formacCel = unidadeP[i].formacDuto;
				celp[j].lito = unidadeP[i].lito;  //03-04-2018
				celp[j].ambext = unidadeP[i].ambext;
				celp[j].dPdLHidro=unidadeP[i].dPdLHidro;
				celp[j].dPdLFric=unidadeP[i].dPdLFric;
				celp[j].dTdL=unidadeP[i].dTdL;
				celp[j].inddPdLHidro=unidadeP[i].inddPdLHidro;
				celp[j].inddPdLFric=unidadeP[i].inddPdLFric;
				celp[j].inddTdL=unidadeP[i].inddTdL;
				celp[j].difusTerm2D=unidadeP[i].difusTerm2D;
				celp[j].difusTerm2DJSON=unidadeP[i].difusTerm2DJSON;
				if(modoDifus3D==1){
					celp[j].difusTerm3D=unidadeP[i].difusTerm3D[kontauni];
					celp[j].difusTerm3DFE=unidadeP[i].difusTerm3DFE[kontauni];
					celp[j].difusTerm3DAcop=unidadeP[i].difusTerm3DAcop[kontauni];
					if(celp[j].difusTerm3D==1){
						acopDuto3D acopTemp;
						acopTemp.indCel=j;
						acopTemp.rotulo=celp[j].difusTerm3DAcop;
						acopTemp.comp=celp[j].dx;
						acopTemp.FE=celp[j].difusTerm3DFE;
						//acopTemp.duto=DadosGeo();
						celAcop.push_back(acopTemp);
						nacop++;
					}
				}
				else{
					celp[j].difusTerm3D=0;
					celp[j].difusTerm3DFE=0;
					celp[j].difusTerm3DAcop="vazio";
				}
				if (lingas > 0 && para == 0) {
					if (j > 1) {
						if (celp[j].acopcol == 0 && celp[j - 1].acopcol == 1)
							para = 1;
						//anmP+=celp[j-1].dx;
					}
					if (para == 0)
						anmP += celp[j].dx;
				}
				posiccentro = posicfronteira + celp[j].dx / 2.;
				posicfronteira += celp[j].dx;
				iniSegF = buscaIndiceUnidade(iniSegF, i, posicfronteira);
				iniSegC = buscaIndiceUnidade(iniSegC, i, posiccentro);
				double compC = (unidadeP[i].dxVar[iniSegC + 1]
						- unidadeP[i].dxVar[iniSegC]) * unidadeP[i].comp;
				double compF = (unidadeP[i].dxVar[iniSegF + 1]
						- unidadeP[i].dxVar[iniSegF]) * unidadeP[i].comp;
				celp[j].temp = unidadeP[i].var[0][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[0][iniSegC + 1]
										- unidadeP[i].var[0][iniSegC]) / compC;
				celp[j].pres = unidadeP[i].var[1][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[1][iniSegC + 1]
										- unidadeP[i].var[1][iniSegC]) / compC;
				celp[j].hol = unidadeP[i].var[2][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[2][iniSegC + 1]
										- unidadeP[i].var[2][iniSegC]) / compC;
				celp[j].bet = unidadeP[i].var[3][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[3][iniSegC + 1]
										- unidadeP[i].var[3][iniSegC]) / compC;
				celp[j].uls = unidadeP[i].var[4][iniSegF]
						+ (posicfronteira-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[4][iniSegF + 1]
										- unidadeP[i].var[4][iniSegF]) / compF;
				celp[j].ugs = unidadeP[i].var[5][iniSegF]
						+ (posicfronteira-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[5][iniSegF + 1]
										- unidadeP[i].var[5][iniSegF]) / compF;
				celp[j].textern = unidadeP[i].var[6][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[6][iniSegC + 1]
										- unidadeP[i].var[6][iniSegC]) / compC;
				celp[j].vextern = unidadeP[i].var[7][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[7][iniSegC + 1]
										- unidadeP[i].var[7][iniSegC]) / compC;
				celp[j].kextern = unidadeP[i].var[8][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[8][iniSegC + 1]
										- unidadeP[i].var[8][iniSegC]) / compC;
				celp[j].cpextern = unidadeP[i].var[9][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[9][iniSegC + 1]
										- unidadeP[i].var[9][iniSegC]) / compC;
				celp[j].rhoextern = unidadeP[i].var[10][iniSegC]
						+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[10][iniSegC + 1]
										- unidadeP[i].var[10][iniSegC]) / compC;
				celp[j].viscextern = unidadeP[i].var[11][iniSegC]
						+(posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
								* (unidadeP[i].var[11][iniSegC + 1]
										- unidadeP[i].var[11][iniSegC]) / compC;
			}
			tempncel += unidadeP[i].ncel;
		}

		celp[ncelp - 1].profundiF = 0;  //04-04-2018
		for (int i = ncelp - 1; i >= 0; i--) {  //04-04-2018
			int iDu = celp[i].duto;
			celp[i].profundiM = celp[i].profundiF
					+ 0.5 * celp[i].dx * sinl(duto[iDu].ang);
			if (i > 0)
				celp[i - 1].profundiF = celp[i].profundiF
						+ celp[i].dx * sinl(duto[iDu].ang);
		}  //04-04-2018

		celdescargaP = 1e7;  //15-06-2018
		if (descarga == 1)
			celdescargaP = buscaIndiceFrontP(compInterDescP);  //15-06-2018

		/* if(flashCompleto==1){
		 for(int i=0;i<nfluP;i++){
		 double comp=0;
		 for(int j=0;j<ncelp;j++){
		 if(flash[i].compini>=comp && flash[i].compini<comp+celp[j].dx)flash[i].celini=j;
		 else if((flash[i].compfim>=comp && flash[i].compfim<comp+celp[j].dx) ||
		 flash[i].compfim>=nCompTotalUnidadesP-1e-15)flash[i].celfim=j;
		 comp+=celp[j].dx;
		 }
		 }
		 }*/
		if(tempReves<-999)tempReves=celp[ncelp-1].textern;

		// carga das unidades de servico
		ncelg = 0;
		if (pocinjec == 0 && nunidadeg > 0) {

			if(conINIAmbS==0)parse_unidades_servico(jsonDoc.dutosServico());
			else if(conINIAmbS==1)parse_unidades_servicoAmb(jsonDoc.dutosServico());

			// gerar as células da linha de servico
			celg = new detcelg[ncelg];
			int tempncel = 0;
			int para = 0;
			anmG = unidadeG[0].dx[0];
			for (int i = 0; i < nunidadeg; i++) {
				double posiccentro = 0.;
				double posicfronteira = 0.;
				int iniSegF = 0;
				int iniSegC = 0;
				for (int j = tempncel; j < (tempncel + unidadeG[i].ncel); j++) {
					int kontauni = j - tempncel;
					celg[j].duto = unidadeG[i].duto;
					celg[j].acopcol = unidadeG[i].acopcol;
					celg[j].dx = unidadeG[i].dx[kontauni];
					celg[j].formacCel = unidadeG[i].formacDuto;
					celg[j].lito = unidadeG[i].lito;  //03-04-2018
					celg[j].ambext = unidadeG[i].ambext;
					celg[j].dPdLHidro=unidadeG[i].dPdLHidro;
					celg[j].dPdLFric=unidadeG[i].dPdLFric;
					celg[j].dTdL=unidadeG[i].dTdL;
					celg[j].inddPdLHidro=unidadeG[i].inddPdLHidro;
					celg[j].inddPdLFric=unidadeG[i].inddPdLFric;
					celg[j].inddTdL=unidadeG[i].inddTdL;
					if (para == 0) {
						if (j > 1) {
							if (celg[j].acopcol == 1
									&& celg[j - 1].acopcol == 0)
								para = 1;
							anmG += celg[j].dx;
						}
					}
					posiccentro = posicfronteira + celg[j].dx / 2.;
					posicfronteira += celg[j].dx;
					iniSegF = buscaIndiceUnidadeG(iniSegF, i, posicfronteira);
					iniSegC = buscaIndiceUnidadeG(iniSegC, i, posiccentro);
					double compC = (unidadeG[i].dxVar[iniSegC + 1]
							- unidadeG[i].dxVar[iniSegC]) * unidadeG[i].comp;
					double compF = (unidadeG[i].dxVar[iniSegF + 1]
							- unidadeG[i].dxVar[iniSegF]) * unidadeG[i].comp;
					celg[j].temp = unidadeG[i].var[0][iniSegC]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[0][iniSegC + 1]
											- unidadeG[i].var[0][iniSegC])
									/ compC;
					celg[j].pres = unidadeG[i].var[1][iniSegC]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[1][iniSegC + 1]
											- unidadeG[i].var[1][iniSegC])
									/ compC;
					celg[j].mg = unidadeG[i].var[2][iniSegC]
							+ (posicfronteira- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[2][iniSegC + 1]
											- unidadeG[i].var[2][iniSegC])
									/ compC;
					celg[j].textern = unidadeG[i].var[3][iniSegC]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[3][iniSegC + 1]
											- unidadeG[i].var[3][iniSegC])
									/ compC;
					celg[j].vextern = unidadeG[i].var[4][iniSegF]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[4][iniSegF + 1]
											- unidadeG[i].var[4][iniSegF])
									/ compF;
					celg[j].kextern = unidadeG[i].var[5][iniSegF]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[5][iniSegF + 1]
											- unidadeG[i].var[5][iniSegF])
									/ compF;
					celg[j].cpextern = unidadeG[i].var[6][iniSegC]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[6][iniSegC + 1]
											- unidadeG[i].var[6][iniSegC])
									/ compC;
					celg[j].rhoextern = unidadeG[i].var[7][iniSegC]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[7][iniSegC + 1]
											- unidadeG[i].var[7][iniSegC])
									/ compC;
					celg[j].viscextern = unidadeG[i].var[8][iniSegC]
							+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
									* (unidadeG[i].var[8][iniSegC + 1]
											- unidadeG[i].var[8][iniSegC])
									/ compC;
				}
				tempncel += unidadeG[i].ncel;
			}
		}
		else if(pocinjec == 0 && lingas==1){
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"#/dutosServico", "#",
					"Chave requerida para linha de servico");
		}

		if (pocinjec == 0 && lingas == 1) {  //04-04-2018
			celg[0].profundiF = 0;  //04-04-2018
			for (int i = 0; i < ncelg; i++) {  //04-04-2018
				int iDu = celg[i].duto;
				celg[i].profundiM = celg[i].profundiF
						+ 0.5 * celg[i].dx * sinl(-duto[iDu].ang);
				if (i < ncelg - 1)
					celg[i + 1].profundiF = celg[i].profundiF
							+ celg[i].dx * sinl(-duto[iDu].ang);
			}  //04-04-2018
		}  //04-04-2018

		celdescarga = 1e7;  //15-06-2018
		if (pocinjec == 0 && descarga == 1)
			celdescarga = buscaIndiceFrontG(compInterDesc);  //15-06-2018

		nparEnvelope = 0;
		existeEnvelope=0;
		calculaEnvelope=0;
		if (jsonDoc.hidrato().exists()) {
			parse_hidrato(jsonDoc.hidrato());
		}


		// parse ipr
		 // cout<<"estouaqui"<<"\n";
		nipr = 0;
		if (contemChaveAtivaArray(jsonDoc, ipr)) {
			parse_ipr(jsonDoc.ipr());
		}

		// parser do elemento "fonteGaslift"
		nvalvgas = 0;
		if (pocinjec == 0 && contemChaveAtivaArray(jsonDoc, fonteGasLift) && enableFonteGasLift) {
			parse_fonte_gaslift(jsonDoc.fonteGasLift());
		}

		// parser do elemento "fonte choke"
		nfontechk = 0;
		/*if (contemChaveAtiva(jsonDoc, fonteChoke)) {
			parse_fontechk(jsonDoc.fonteChoke());
		}*/

		// parse fonte de gas
		ninjgas = 0;
		// caso simulacao de producao
		if ((contemChaveAtivaArray(jsonDoc, fonteGas))) {
			parse_fonte_gas(jsonDoc.fonteGas());
		}
		else if(((flashCompleto==2 || flashCompleto==1) && pocinjec == 1)){
			ninjgas++;
			// declarar o vetor fontel com valores default
			fonteg = new detFONGAS[ninjgas];
			// incluir fonte de liquido adicional
			fonteg[0].comp = 0;
			fonteg[0].posicP = 0;
			fonteg[0].parserie = 1;
			fonteg[0].temp = new double[fonteg[0].parserie];
			fonteg[0].seco = 1;
			fonteg[0].vazgas = new double[fonteg[0].parserie];
			fonteg[0].vazcomp = new double[fonteg[0].parserie];
			fonteg[0].tempo = new double[fonteg[0].parserie];
			fonteg[0].temp[0] = condpocinj.tempinj;
			fonteg[0].vazgas[0] = condpocinj.vazinj;
			fonteg[0].vazcomp[0] = 0.;
			fonteg[0].tempo[0] = 0.;
			fonteg[0].indfluP = 0.;
		}

		// parse fonte de liquido
		ninjliq = 0;
		if (contemChaveAtivaArray(jsonDoc, fonteLiquido)) {
			parse_fonte_liquido(jsonDoc.fonteLiquido());
		} else if (pocinjec == 1 && flashCompleto!=2 && flashCompleto!=1) {
			// caso simulacao de injecao
			ninjliq++;
			// declarar o vetor fontel com valores default
			fontel = new detFONLIQ[ninjliq];
			// incluir fonte de liquido adicional
			fontel[0].comp = 0;
			fontel[0].posicP = 0;
			fontel[0].parserie = 1;
			fontel[0].temp = new double[fontel[0].parserie];
			fontel[0].bet = new double[fontel[0].parserie];
			fontel[0].vazliq = new double[fontel[0].parserie];
			fontel[0].tempo = new double[fontel[0].parserie];
			fontel[0].temp[0] = condpocinj.tempinj;
			fontel[0].bet[0] = 1.;
			fontel[0].vazliq[0] = condpocinj.vazinj;
			fontel[0].tempo[0] = 0.;
			fontel[0].indfluP = 0.;
		}

		// parse fonte de massa
		ninjmass = 0;
		if (contemChaveAtivaArray(jsonDoc, fonteMassa) || ConContEntrada==2) {
			parse_fonte_massa(jsonDoc.fonteMassa());
		}

		// parse fonte poroso radial
		nPoroRad = 0;
		if (contemChaveAtivaArray(jsonDoc, fontePoroRadial)) {
			parse_fonte_PoroRadial(jsonDoc.fontePoroRadial());
		}

		// parse fonte poroso 2D
		nPoro2D = 0;
		if (contemChaveAtivaArray(jsonDoc, fontePoro2D)) {
			parse_fonte_Poro2D(jsonDoc.fontePoro2D());
		}

		// iniciar os valores dos contadores dos equipamentos da simulacao
		nbcs = 0;
		nbvol = 0;
		ndpreq = 0;
		npig = 0;
		ntendp = 0;
		ntendg = 0;
		ntendtransp = 0;
		ntendtransg = 0;
		ntela = 0;

		// caso simulacao de producao
		if (pocinjec == 0) {
			// parser do elemento "valvula"
			if (contemChaveAtivaArray(jsonDoc, valvula)) {
				parse_valv(jsonDoc.valvula());
			}
			// parser do elemento "bcs"
			if (contemChaveAtivaArray(jsonDoc, bcs)) {
				parse_bcs(jsonDoc.bcs());
			}
			// parser do elemento "bombaVolumetrica"
			if (contemChaveAtivaArray(jsonDoc, bombaVolumetrica)) {
				parse_bomba_volumetrica(jsonDoc.bombaVolumetrica());
			}
			// parser do elemento "deltaPressao"
			if (contemChaveAtivaArray(jsonDoc, deltaPressao)) {
				parse_delta_pressao(jsonDoc.deltaPressao());
			}
			// parser do elemento "furo"
			if (contemChaveAtivaArray(jsonDoc, fontePressao)) {
				parse_furo(jsonDoc.fontePressao());
			}
			if (contemChaveAtivaArray(jsonDoc, intermitenciaSevera)) {
				parse_intermitencia(jsonDoc.intermitenciaSevera());
			}
			///forcacao de barra:
			/*nfuro=1;
			furo = new detFURO[nfuro];
			furo[0].comp = 7098;
			// sentido plataforma para fundo-poco
			if (origemGeometria == origemGeometria_t::plataforma)
				furo[0].comp = nCompTotalUnidadesP - furo[0].comp; //04-04-2018
			if (furo[0].comp < 0.0)
				furo[0].comp = 0.0;
			double lverif = furo[0].comp;
			furo[0].posicP = buscaIndiceMeioP(lverif);
			furo[0].parserie = 3;
			furo[0].abertura = new double[furo[0].parserie];
			furo[0].tempo = new double[furo[0].parserie];
			furo[0].abertura[0]=0;
			furo[0].tempo[0]=0;
			furo[0].abertura[1]=0;
			furo[0].tempo[1]=25000;
			furo[0].abertura[2]=0.05;
			furo[0].tempo[2]=25500;
			furo[0].temp=4;
			furo[0].pres=155;
			furo[0].cd=0.84;
			furo[0].tipoFlu =1;
			furo[0].indFlu = 0;
			fluc.maritimo=1.;*/

			// parse do elemento "master1"
			parse_master1(jsonDoc.master1());
			// parser do elemento "pig"
			if (contemChaveAtivaArray(jsonDoc, pig)) {
				parse_pig(jsonDoc.pig());
			}
			// parser do elemento "tendenciaProducao"
			if (jsonDoc.tendP().exists()) {
				parse_tendencia_producao(jsonDoc.tendP());
			}
			// parser do elemento "tendenciaTransProducao"
			if (jsonDoc.tendTransP().exists()) {
				parse_tendencia_trans_producao(jsonDoc.tendTransP());
			}
			// parser do elemento "Tela"
			if (contemChaveAtivaArray(jsonDoc, tela)) {
				parse_tela(jsonDoc.tela());
			}
			// caso possua linha de servico
			if (lingas > 0) {
				// parser do elemento "tendenciaServico"
				if (jsonDoc.tendS().exists()) {
					parse_tendencia_servico(jsonDoc.tendS());
				}
				// parser do elemento "tendenciaTransServico"
				if (jsonDoc.tendTransS().exists()) {
					parse_tendencia_trans_servico(jsonDoc.tendTransS());
				}
			}
		} else {
			// preencher dados da master1 com valores default
			master1.parserie = 1;
			master1.abertura = new double[master1.parserie];
			master1.tempo = new double[master1.parserie];
			master1.comp = 0.1;
			master1.posic = 0;
			master1.abertura[0] = 1;
			master1.tempo[0] = 0;
			eventoabre = 0;
			eventofecha = 0;
			Tevento = new double[master1.parserie];
			Teventof = new double[master1.parserie];

			int testaMaster = 1;
			int indMaster = 0;
			while (testaMaster) {
				testaMaster = 0;
				for (int i = 0; i < ninjliq; i++) {
					if (indMaster == fontel[i].posicP) {
						indMaster++;
						testaMaster = 1;
					}
				}
			}
			master1.posic = indMaster;
		}

		// iniciar os valores dos contadores dos perfis de servico da simulacao
		nperfisg = 0;
		nperfistransg = 0;
		// caso possua linha de servico
		if (lingas > 0) {
			// parse injecao de gas na plataforma
			if (contemChaveAtiva(jsonDoc, gasInj)) {
				parse_gasInj(jsonDoc.gasInj());
			}
			// parser do elemento "master2"
			if (contemChaveAtiva(jsonDoc, master2)) {
				parse_master2(jsonDoc.master2());
			}
			else{
				master2.curvaCV=0;
				master2.ncv=0;
				master2.parserie = 1;
				master2.abertura = new double[master2.parserie];
				master2.tempo = new double[master2.parserie];
				master2.comp = 0;
				double lverif = master2.comp;
				master2.posic = buscaIndiceFrontG(lverif);
				master2.abertura[0] =1;
				master2.tempo[0] = 0;
			}
			// parser do elemento "perfilServico"
			if (lingas > 0 && jsonDoc.perfilServico().exists()) {
				parse_perfil_servico(jsonDoc.perfilServico());
			}
			// parser do elemento "perfilTransS"
			if (lingas > 0 && jsonDoc.perfisTransS().exists()) {
				parse_perfis_trans_servico(jsonDoc.perfisTransS());
			}
		}

		// parse separador
		if (pocinjec == 0 && contemChaveAtiva(jsonDoc, separador)) {
			if(ConContEntrada!=2) parse_separador(jsonDoc.separador());
			else{
				psep.parserie = 1;
				psep.pres = new double[psep.parserie];
				psep.tempo = new double[psep.parserie];

				for (int i = 0; i < psep.parserie; i++) {
					psep.pres[i] = 10.;
					psep.tempo[i] = 0.;
				}
			}
		} else {
			// preencher dados do separador com valores default
			psep.parserie = 1;
			psep.pres = new double[psep.parserie];
			psep.tempo = new double[psep.parserie];
			if (pocinjec == 1)psep.pres[0] = condpocinj.presinj;
			else psep.pres[0] = -1.1e6;
			psep.tempo[0] = 0;
		}
		// parse chokeSup
		//if (pocinjec == 0 && contemChaveAtiva(jsonDoc, chokeSup)) {
		if (pocinjec == 0 && (jsonDoc.chokeSup().exists() && contemChaveAtiva(jsonDoc, chokeSup))) {
			parse_chokeSup(jsonDoc.chokeSup());
		} else {
			// preencher dados do chokeSup com valores default
			chokep.parserie = 1;
			chokep.abertura = new double[chokep.parserie];
			chokep.tempo = new double[chokep.parserie];
			chokep.cd = 1;
			chokep.modelo = 0;
			chokep.abertura[0] = 1;
			chokep.tempo[0] = 0;

			chokep.curvaCV=0;
			chokep.curvaDinamic=0;
			chokep.ncv=0;
		}
		if (pocinjec == 0&& jsonDoc.chokeInj().exists() && lingas > 0) {
			parse_chokeInj(jsonDoc.chokeInj());
		} else {
			// preencher dados do chokeSup com valores default
			chokes.curvaCV=0;
			chokes.ncv=0;
			chokes.parserie = 1;
			chokes.abertura = new double[chokes.parserie];
			chokes.tempo = new double[chokes.parserie];
			chokes.cd = 1;
			chokes.modelo = 0;
			chokes.abertura[0] = 1;
			chokes.tempo[0] = 0;

		}
		// parser do elemento "perfilProducao"
		nperfisp = 0;
		if (contemChaveAtiva(jsonDoc, perfilProducao)) {
			parse_perfil_producao(jsonDoc.perfilProducao());
		}
		// parser do elemento "perfilTransP"
		nperfistransp = 0;
		if (contemChaveAtiva(jsonDoc, perfisTransP)) {
			parse_perfis_trans_producao(jsonDoc.perfisTransP());
		}

		// caso exista uma condicao de falha da aplicacao
			if (!logger.getStResultadoSimulacao().sucesso) {
				// gerar o arquivo de saida da simulacao e encerra a aplicacao
				logger.write_logs_and_exit();
			} else {
				// incluir info sobre o fim do parse
				logger.log(LOGGER_INFO, LOG_INFO_PARSE_PROCESS_FINISHED,
					"Validar regras de negocio do MRT", "", "SUCESSO");
				// caso configurada opcao de termino apos validacao das regras de negocio do MRT
				if (validacaoJson == tipoValidacaoJson_t::rules) {
					// gerar o arquivo de saida da simulacao e encerra a aplicacao
					logger.write_logs_and_exit();
				}
			}
	} catch (exception& exc) {
		cout << "Unexpected exception raised: " << exc.what() << endl;
	} catch (...) {
		cout << "FATAL ERROR" << endl;
	}
}



void Ler::copiaArq(Ler& arqAntigo) {

	// configurar os nomes dos arquivos de entrada e log
	logger.changeResultadoSimulacao(arqAntigo.impfile, arqAntigo.arquivoLog);
	validacaoJson =arqAntigo.validacaoJson;
	tipoSimulacao=arqAntigo.tipoSimulacao;
	sistemaSimulacao = arqAntigo.sistemaSimulacao;
	pocinjec = arqAntigo.pocinjec;
	copia_configuracao_inicial(arqAntigo);
	copia_condcont_pocinjec(arqAntigo);
	copia_parafina(arqAntigo);
	copia_materiais(arqAntigo);
	copia_corte(arqAntigo);
	copia_tempo(arqAntigo);
	copia_tabela(arqAntigo);
	if (tabp > 0 || tabg > 0 || modelcp > 0 || modelJTL == 1 || latente > 0) {
		if (tabp > 0 || tabg > 0) {
			zdranP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
				zdranP[i] = new double[npontos + 2];
			dzdpP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
				dzdpP[i] = new double[npontos + 2];
			dzdtP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
				dzdtP[i] = new double[npontos + 2];
		}
	}
	copia_fluidos_producao(arqAntigo);
	copia_fluido_complementar(arqAntigo);
	fluc.descarga = descarga;
	copia_fluido_gas(arqAntigo);
	flug.ModelCp = modelcp;
	flug.Modeljtl = modelJTL;
	flug.CalcLat = latente;
	flug.trackRGO = trackRGO;
	flug.trackDeng = trackDeng;
	flug.ninjgas = arqAntigo.flug.ninjgas;
	flug.lingas = lingas;

	usaTabela = 0;
	if (flashCompleto == 1 /*|| tabRSPB>0*/)
		usaTabela = 1;  //alteracao8

	double TCMin = 0;  //15-06-2018
	double TCMax = 0;  //15-06-2018
	double PCMin = 0;  //15-06-2018
	double PCMax = 0;  //15-06-2018
	if (tabp > 0) {
		if(corrDeng==0){
		  TCMin = flup[0].TC;  //15-06-2018
		  TCMax = flup[0].TC;  //15-06-2018
		  PCMin = flup[0].PC;  //15-06-2018
		  PCMax = flup[0].PC;  //15-06-2018
		}
		else{
			double PC= 708.75 - 57.5* 0.5538;
			double TC= 169 + 314.0* 0.5538;
			TCMin = TC;
			TCMax = TC;
			PCMin = PC;
			PCMax = PC;
		}
		for (int i = 0; i < nfluP; i++) {  //15-06-2018
			if (flup[i].TC < TCMin)
				TCMin = flup[i].TC;
			if (flup[i].TC > TCMax)
				TCMax = flup[i].TC;
			if (flup[i].PC < PCMin)
				PCMin = flup[i].PC;
			if (flup[i].PC > PCMax)
				PCMax = flup[i].PC;
		}  //15-06-2018
	} else if (tabg > 0) {
		TCMin = flug.TC;  //15-06-2018
		TCMax = flug.TC;  //15-06-2018
		PCMin = flug.PC;  //15-06-2018
		PCMax = flug.PC;  //15-06-2018
	}
	if ((flug.ninjgas > 0 || lingas == 1) && tabg > 0) {  //15-06-2018
		if (flug.TC < TCMin)
			TCMin = flug.TC;
		if (flug.TC > TCMax)
			TCMax = flug.TC;
		if (flug.PC < PCMin)
			PCMin = flug.PC;
		if (flug.PC > PCMax)
			PCMax = flug.PC;
	}  //15-06-2018
	if (tabg > 0 || tabp > 0) {
		tabentCrit.npont = tabent.npont;  //15-06-2018
		tabentCrit.tmin = (tabent.tmin + 273.15)
				/ ((TCMax - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
		tabentCrit.tmax = (tabent.tmax + 273.15)
				/ ((TCMin - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
		tabentCrit.pmin = tabent.pmin * 0.9678411 * 14.69595 / PCMax; //15-06-2018
		tabentCrit.pmax = tabent.pmax * 0.9678411 * 14.69595 / PCMin; //15-06-2018
	}

	// caso nao seja simulacao de injecao
	if ((pocinjec == 0 && tabp > 0)) {  //15-06-2018
		flup[0].tabela(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
				tabentCrit.tmin);
		flup[0].tabelaDZDP(tabentCrit.pmax, tabentCrit.tmax,
				tabentCrit.pmin, tabentCrit.tmin);
		flup[0].tabelaDZDT(tabentCrit.pmax, tabentCrit.tmax,
				tabentCrit.pmin, tabentCrit.tmin);
	}  //15-06-2018
	else if (pocinjec == 0 && (flug.ninjgas > 0 || lingas == 1)
			&& tabg > 0) {
		flug.tabela(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
				tabentCrit.tmin);
		flug.tabelaDZDP(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
				tabentCrit.tmin);
		flug.tabelaDZDT(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
				tabentCrit.tmin);
	}
	indfluPIni=arqAntigo.indfluPIni;
	if (pocinjec == 1 && condpocinj.tipoFlui == 2 && flashCompleto!=1) {
		// gerar tabela de injecao de CO2
		geraTabInjCO2();
		fluc.RhoInj = RhoInj;
		fluc.ViscInj = ViscInj;
		fluc.CondInj = CondInj;
		fluc.CpInj = CpInj;
		fluc.DrhoDtInj = DrhoDtInj;
	}
	copia_correcao(arqAntigo);
	conINIAmbP=arqAntigo.conINIAmbP;
	conINIAmbS=arqAntigo.conINIAmbS;
	nunidadep = arqAntigo.nunidadep;
	nunidadeg = arqAntigo.nunidadeg;
	nduto = nunidadep + nunidadeg;
	duto = new detduto[nduto];
	copia_unidades_producao(arqAntigo);



	// gerar celulas de producao
	celp = new detcelp[ncelp];
	int tempncel = 0;
	anmP = 0 * unidadeP[0].dx[0];
	int para = 0;
	for (int i = 0; i < nunidadep; i++) {
		double posiccentro = 0.;
		double posicfronteira = 0.;
		int iniSegF = 0;
		int iniSegC = 0;
		for (int j = tempncel; j < (tempncel + unidadeP[i].ncel); j++) {
			int kontauni = j - tempncel;
			celp[j].duto = unidadeP[i].duto;
			celp[j].dirconv = unidadeP[i].dirconv;
			celp[j].acopcol = unidadeP[i].acopcol;
			celp[j].acopcolRede = unidadeP[i].acopcolRede;
			celp[j].dx = unidadeP[i].dx[kontauni];
			celp[j].formacCel = unidadeP[i].formacDuto;
			celp[j].lito = unidadeP[i].lito;  //03-04-2018
			celp[j].ambext = unidadeP[i].ambext;
			celp[j].dPdLHidro=unidadeP[i].dPdLHidro;
			celp[j].dPdLFric=unidadeP[i].dPdLFric;
			celp[j].dTdL=unidadeP[i].dTdL;
			celp[j].inddPdLHidro=unidadeP[i].inddPdLHidro;
			celp[j].inddPdLFric=unidadeP[i].inddPdLFric;
			celp[j].inddTdL=unidadeP[i].inddTdL;
			celp[j].difusTerm2D=unidadeP[i].difusTerm2D;
			celp[j].difusTerm2DJSON=unidadeP[i].difusTerm2DJSON;
			if(modoDifus3D==1){
				celp[j].difusTerm3D=unidadeP[i].difusTerm3D[kontauni];
				celp[j].difusTerm3DFE=unidadeP[i].difusTerm3DFE[kontauni];
				celp[j].difusTerm3DAcop=unidadeP[i].difusTerm3DAcop[kontauni];
				if(celp[j].difusTerm3D==1){
					acopDuto3D acopTemp;
					acopTemp.indCel=j;
					acopTemp.rotulo=celp[j].difusTerm3DAcop;
					acopTemp.comp=celp[j].dx;
					acopTemp.FE=celp[j].difusTerm3DFE;
					//acopTemp.duto=DadosGeo();
					celAcop.push_back(acopTemp);
					nacop++;
				}
			}
			else{
				celp[j].difusTerm3D=0;
				celp[j].difusTerm3DFE=0;
				celp[j].difusTerm3DAcop="vazio";
			}
			if (lingas > 0 && para == 0) {
				if (j > 1) {
					if (celp[j].acopcol == 0 && celp[j - 1].acopcol == 1)
						para = 1;
					//anmP+=celp[j-1].dx;
				}
				if (para == 0)
					anmP += celp[j].dx;
			}
			posiccentro = posicfronteira + celp[j].dx / 2.;
			posicfronteira += celp[j].dx;
			iniSegF = buscaIndiceUnidade(iniSegF, i, posicfronteira);
			iniSegC = buscaIndiceUnidade(iniSegC, i, posiccentro);
			double compC = (unidadeP[i].dxVar[iniSegC + 1]
					- unidadeP[i].dxVar[iniSegC]) * unidadeP[i].comp;
			double compF = (unidadeP[i].dxVar[iniSegF + 1]
					- unidadeP[i].dxVar[iniSegF]) * unidadeP[i].comp;
			celp[j].temp = unidadeP[i].var[0][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[0][iniSegC + 1]
									- unidadeP[i].var[0][iniSegC]) / compC;
			celp[j].pres = unidadeP[i].var[1][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[1][iniSegC + 1]
									- unidadeP[i].var[1][iniSegC]) / compC;
			if(isnan(celp[j].temp) || isnan(celp[j].pres) || celp[j].pres>1000000){
				int para;
				para=0;
			}
			celp[j].hol = unidadeP[i].var[2][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[2][iniSegC + 1]
									- unidadeP[i].var[2][iniSegC]) / compC;
			celp[j].bet = unidadeP[i].var[3][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[3][iniSegC + 1]
									- unidadeP[i].var[3][iniSegC]) / compC;
			celp[j].uls = unidadeP[i].var[4][iniSegF]
					+ (posicfronteira-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[4][iniSegF + 1]
									- unidadeP[i].var[4][iniSegF]) / compF;
			celp[j].ugs = unidadeP[i].var[5][iniSegF]
					+ (posicfronteira-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[5][iniSegF + 1]
									- unidadeP[i].var[5][iniSegF]) / compF;
			celp[j].textern = unidadeP[i].var[6][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[6][iniSegC + 1]
									- unidadeP[i].var[6][iniSegC]) / compC;
			celp[j].vextern = unidadeP[i].var[7][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[7][iniSegC + 1]
									- unidadeP[i].var[7][iniSegC]) / compC;
			celp[j].kextern = unidadeP[i].var[8][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[8][iniSegC + 1]
									- unidadeP[i].var[8][iniSegC]) / compC;
			celp[j].cpextern = unidadeP[i].var[9][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[9][iniSegC + 1]
									- unidadeP[i].var[9][iniSegC]) / compC;
			celp[j].rhoextern = unidadeP[i].var[10][iniSegC]
					+ (posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[10][iniSegC + 1]
									- unidadeP[i].var[10][iniSegC]) / compC;
			celp[j].viscextern = unidadeP[i].var[11][iniSegC]
					+(posiccentro-unidadeP[i].dxVar[iniSegF] * unidadeP[i].comp)
							* (unidadeP[i].var[11][iniSegC + 1]
									- unidadeP[i].var[11][iniSegC]) / compC;
		}
		tempncel += unidadeP[i].ncel;
	}

	celp[ncelp - 1].profundiF = 0;  //04-04-2018
	for (int i = ncelp - 1; i >= 0; i--) {  //04-04-2018
		int iDu = celp[i].duto;
		celp[i].profundiM = celp[i].profundiF
				+ 0.5 * celp[i].dx * sinl(duto[iDu].ang);
		if (i > 0)
			celp[i - 1].profundiF = celp[i].profundiF
					+ celp[i].dx * sinl(duto[iDu].ang);
	}  //04-04-2018

	celdescargaP = 1e7;  //15-06-2018
	if (descarga == 1)
		celdescargaP = buscaIndiceFrontP(compInterDescP);  //15-06-2018
	if(tempReves<-999)tempReves=celp[ncelp-1].textern;


	if (pocinjec == 0 && nunidadeg > 0) {
		copia_unidades_servico(arqAntigo);
		// gerar as celulas da linha de servico
		celg = new detcelg[ncelg];
		int tempncel = 0;
		int para = 0;
		anmG = unidadeG[0].dx[0];
		for (int i = 0; i < nunidadeg; i++) {
			double posiccentro = 0.;
			double posicfronteira = 0.;
			int iniSegF = 0;
			int iniSegC = 0;
			for (int j = tempncel; j < (tempncel + unidadeG[i].ncel); j++) {
				int kontauni = j - tempncel;
				celg[j].duto = unidadeG[i].duto;
				celg[j].acopcol = unidadeG[i].acopcol;
				celg[j].dx = unidadeG[i].dx[kontauni];
				celg[j].formacCel = unidadeG[i].formacDuto;
				celg[j].lito = unidadeG[i].lito;  //03-04-2018
				celg[j].ambext = unidadeG[i].ambext;
				celg[j].dPdLHidro=unidadeG[i].dPdLHidro;
				celg[j].dPdLFric=unidadeG[i].dPdLFric;
				celg[j].dTdL=unidadeG[i].dTdL;
				celg[j].inddPdLHidro=unidadeG[i].inddPdLHidro;
				celg[j].inddPdLFric=unidadeG[i].inddPdLFric;
				celg[j].inddTdL=unidadeG[i].inddTdL;
				if (para == 0) {
					if (j > 1) {
						if (celg[j].acopcol == 1
								&& celg[j - 1].acopcol == 0)
							para = 1;
						anmG += celg[j].dx;
					}
				}
				posiccentro = posicfronteira + celg[j].dx / 2.;
				posicfronteira += celg[j].dx;
				iniSegF = buscaIndiceUnidadeG(iniSegF, i, posicfronteira);
				iniSegC = buscaIndiceUnidadeG(iniSegC, i, posiccentro);
				double compC = (unidadeG[i].dxVar[iniSegC + 1]
						- unidadeG[i].dxVar[iniSegC]) * unidadeG[i].comp;
				double compF = (unidadeG[i].dxVar[iniSegF + 1]
						- unidadeG[i].dxVar[iniSegF]) * unidadeG[i].comp;
				celg[j].temp = unidadeG[i].var[0][iniSegC]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[0][iniSegC + 1]
										- unidadeG[i].var[0][iniSegC])
								/ compC;
				celg[j].pres = unidadeG[i].var[1][iniSegC]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[1][iniSegC + 1]
										- unidadeG[i].var[1][iniSegC])
								/ compC;
				celg[j].mg = unidadeG[i].var[2][iniSegC]
						+ (posicfronteira- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[2][iniSegC + 1]
										- unidadeG[i].var[2][iniSegC])
								/ compC;
				celg[j].textern = unidadeG[i].var[3][iniSegC]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[3][iniSegC + 1]
										- unidadeG[i].var[3][iniSegC])
								/ compC;
				celg[j].vextern = unidadeG[i].var[4][iniSegF]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[4][iniSegF + 1]
										- unidadeG[i].var[4][iniSegF])
								/ compF;
				celg[j].kextern = unidadeG[i].var[5][iniSegF]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[5][iniSegF + 1]
										- unidadeG[i].var[5][iniSegF])
								/ compF;
				celg[j].cpextern = unidadeG[i].var[6][iniSegC]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[6][iniSegC + 1]
										- unidadeG[i].var[6][iniSegC])
								/ compC;
				celg[j].rhoextern = unidadeG[i].var[7][iniSegC]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[7][iniSegC + 1]
										- unidadeG[i].var[7][iniSegC])
								/ compC;
				celg[j].viscextern = unidadeG[i].var[8][iniSegC]
						+ (posiccentro- unidadeG[i].dxVar[iniSegC] * unidadeG[i].comp)
								* (unidadeG[i].var[8][iniSegC + 1]
										- unidadeG[i].var[8][iniSegC])
								/ compC;
			}
			tempncel += unidadeG[i].ncel;
		}
	}

	if (pocinjec == 0 && lingas == 1) {  //04-04-2018
		celg[0].profundiF = 0;  //04-04-2018
		for (int i = 0; i < ncelg; i++) {  //04-04-2018
			int iDu = celg[i].duto;
			celg[i].profundiM = celg[i].profundiF
					+ 0.5 * celg[i].dx * sinl(-duto[iDu].ang);
			if (i < ncelg - 1)
				celg[i + 1].profundiF = celg[i].profundiF
						+ celg[i].dx * sinl(-duto[iDu].ang);
		}  //04-04-2018
	}  //04-04-2018

	celdescarga = 1e7;  //15-06-2018
	if (pocinjec == 0 && descarga == 1)
		celdescarga = buscaIndiceFrontG(compInterDesc);  //15-06-2018


	nparEnvelope = 0;
	existeEnvelope=0;
	calculaEnvelope=0;
	copia_hidrato(arqAntigo);
	copia_ipr(arqAntigo);
	copia_fonte_gaslift(arqAntigo);
	nfontechk = 0;
	ninjgas = 0;
	copia_fonte_gas(arqAntigo);
	copia_fonte_liquido(arqAntigo);
	copia_fonte_massa(arqAntigo);
	copia_fonte_PoroRadial(arqAntigo);
	copia_fonte_Poro2D(arqAntigo);
	copia_valv(arqAntigo);
	copia_bcs(arqAntigo);
	copia_bomba_volumetrica(arqAntigo);
	copia_delta_pressao(arqAntigo);
	copia_furo(arqAntigo);
	copia_master1(arqAntigo);
	copia_pig(arqAntigo);
	copia_tendencia_producao(arqAntigo);
	copia_tendencia_trans_producao(arqAntigo);
	copia_tela(arqAntigo);
	copia_tendencia_servico(arqAntigo);
	copia_tendencia_trans_servico(arqAntigo);
	copia_gasInj(arqAntigo);
	copia_master2(arqAntigo);
	copia_perfil_servico(arqAntigo);
	copia_perfis_trans_servico(arqAntigo);
	copia_separador(arqAntigo);
	copia_chokeSup(arqAntigo);
	copia_chokeInj(arqAntigo);
	copia_intermitencia(arqAntigo);
	copia_perfil_producao(arqAntigo);
	copia_perfis_trans_producao(arqAntigo);
}



void Ler::geraTabCp() {
	// Caso o arquivo pvtsim inexista
	if (!ifstream(pvtsimarq)) {
		// criar variavel para o nome da propriedade json
		string chavePvtsimArq("#/configuracaoInicial/pvtsimArq");
		// RN-043: Arquivo "pvtsimArq" inexistente
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "Arquivo inexistente", chavePvtsimArq,
				pvtsimarq);
	}
	string impfile;
	impfile = pvtsimarq;
	string dadosMR = impfile;
	ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
	string chave;
	char* tenta;
	//tenta = new char[400];
	double testatok;
	int ndiv = tabent.npont - 1;
	//double pteste = tabent.pmin;
	//double tteste = tabent.tmin;
	// double dpteste = (tabent.pmax - pteste) / ndiv;
	//double dtteste = (tabent.tmax - tteste) / ndiv;
	Vcr<double> presPVTSim(ndiv + 1);
	Vcr<double> tempPVTSim(ndiv + 1);
	FullMtx<double> CPGTemp(ndiv + 2, ndiv + 2);
	FullMtx<double> CPLTemp(ndiv + 2, ndiv + 2);
	char line[4000];
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	while (tenta != nullptr && strcmp(tenta, "PHASE") != 0) {
		tenta = strtok(NULL, " ,()=");
	}
	tenta = strtok(NULL, " ,()=");
	int lacoleitura = 10;
	if (tenta != nullptr && strcmp(tenta, "THREE") == 0)
		lacoleitura = 15;

	while (chave != "PRESSURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	presPVTSim[0] = atof(tenta) * 1.01971621e-5;
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		presPVTSim[kontaPVT] = atof(tenta) * 1.01971621e-5;
	}
	while (chave != "TEMPERATURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	tempPVTSim[0] = atof(tenta);
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		tempPVTSim[kontaPVT] = atof(tenta);
	}

	for (int i = 1; i <= ndiv + 1; i++) {
		CPGTemp[i][0] = presPVTSim[i - 1];
		CPLTemp[i][0] = presPVTSim[i - 1];
		for (int j = 1; j <= ndiv + 1; j++) {
			CPGTemp[0][j] = tempPVTSim[j - 1];
			CPLTemp[0][j] = tempPVTSim[j - 1];
			while (chave != "POINT")
				lendoPVTSim >> chave;
			char line[1000];
			lendoPVTSim.get(line, 4000);
			tenta = strtok(line, " ,()=");
			for (int kontaPVT = 0; kontaPVT < lacoleitura; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			CPGTemp[i][j] = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			CPLTemp[i][j] = testatok;
			//tteste += dtteste;
			if (i == ndiv + 1 && j == ndiv + 1)
				break;
			while (chave != "PVTTABLE")
				lendoPVTSim >> chave;
		}
		//pteste += dpteste;
		//tteste = tabent.tmin;
	}
	lendoPVTSim.close();
	cpg = new double*[ndiv + 2];
	cpl = new double*[ndiv + 2];
	for (int i = 0; i < ndiv + 2; i++) {
		cpg[i] = new double[ndiv + 2];
		cpl[i] = new double[ndiv + 2];
		for (int j = 0; j < ndiv + 2; j++) {
			cpg[i][j] = CPGTemp[i][j];
			cpl[i][j] = CPLTemp[i][j];
		}
	}
	ostringstream saidaCPG;
	saidaCPG << pathPrefixoArqSaida << "perfilCpg.dat";
	string tmp = saidaCPG.str();
	ofstream escreveCpg(tmp.c_str(), ios_base::out);
	escreveCpg << CPGTemp;
	escreveCpg.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}

	ostringstream saidaCPL;
	saidaCPL << pathPrefixoArqSaida << "perfilCpl.dat";
	tmp = saidaCPL.str();
	ofstream escreveCpl(tmp.c_str(), ios_base::out);
	escreveCpl << CPLTemp;
	escreveCpl.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}
	//delete [] tenta;
}

void Ler::geraTabDrholDt() {
	string impfile;
	impfile = pvtsimarq;
	string dadosMR = impfile;
	ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
	string chave;
	char* tenta;
	//tenta = new char[400];
	double testatok;
	int ndiv = tabent.npont - 1;
	//double pteste = tabent.pmin;
	//double tteste = tabent.tmin;
	//double dpteste = (tabent.pmax - pteste) / ndiv;
	//double dtteste = (tabent.tmax - tteste) / ndiv;

	Vcr<double> presPVTSim(ndiv + 1);
	Vcr<double> tempPVTSim(ndiv + 1);

	FullMtx<double> DrholDTTemp(ndiv + 2, ndiv + 2);
	char line[4000];
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	while (tenta != nullptr && strcmp(tenta, "PHASE") != 0) {
		tenta = strtok(NULL, " ,()=");
	}
	tenta = strtok(NULL, " ,()=");
	int lacoleitura = 6;
	if (tenta != nullptr && strcmp(tenta, "THREE") == 0)
		lacoleitura = 8;

	while (chave != "PRESSURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	presPVTSim[0] = atof(tenta) * 1.01971621e-5;
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		presPVTSim[kontaPVT] = atof(tenta) * 1.01971621e-5;
	}
	while (chave != "TEMPERATURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	tempPVTSim[0] = atof(tenta);
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		tempPVTSim[kontaPVT] = atof(tenta);
	}

	for (int i = 1; i <= ndiv + 1; i++) {
		DrholDTTemp[i][0] = presPVTSim[i - 1];
		for (int j = 1; j <= ndiv + 1; j++) {
			DrholDTTemp[0][j] = tempPVTSim[j - 1];
			while (chave != "POINT")
				lendoPVTSim >> chave;
			char line[1000];
			lendoPVTSim.get(line, 4000);
			tenta = strtok(line, " ,()=");
			for (int kontaPVT = 0; kontaPVT < lacoleitura; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			DrholDTTemp[i][j] = testatok;
			//tteste += dtteste;
			if (i == ndiv + 1 && j == ndiv + 1)
				break;
			while (chave != "PVTTABLE")
				lendoPVTSim >> chave;
		}
		//pteste += dpteste;
		//tteste = tabent.tmin;
	}
	lendoPVTSim.close();
	drholdT = new double*[ndiv + 2];
	for (int i = 0; i < ndiv + 2; i++) {
		drholdT[i] = new double[ndiv + 2];
		for (int j = 0; j < ndiv + 2; j++) {
			drholdT[i][j] = DrholDTTemp[i][j];
		}
	}
	ostringstream saidaDrholDt;
	saidaDrholDt << pathPrefixoArqSaida << "perfilDrhoLDT.dat";
	string tmp = saidaDrholDt.str();
	ofstream escreveDrholDt(tmp.c_str(), ios_base::out);
	escreveDrholDt << DrholDTTemp;
	escreveDrholDt.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}
	//delete [] tenta;
}

void Ler::geraTabInjCO2() {
	// Caso o arquivo pvtsim inexista
	if (!ifstream(condpocinj.pvtsimarqInj)) {
		// criar variavel para o nome da propriedade json
		string chavePvtsimArq("#/CondicaoContPocInjec/PVTSim");
		// RN-043: Arquivo "PVTSim" inexistente
		// incluir falha
		logger.log_write_logs_and_exit(LOGGER_FALHA,
		LOG_ERR_UNEXPECTED_EXCEPTION, "Arquivo inexistente", chavePvtsimArq,
				condpocinj.pvtsimarqInj);
	}

	string dadosMRPre;
	dadosMRPre =condpocinj.pvtsimarqInj;
	ifstream lendoPVTSimPre(dadosMRPre.c_str(), ios_base::in);
	string chave;
	char* tenta;
	//tenta = new char[400];
	double testatok;
	char line[4000];
	lendoPVTSimPre.get(line, 4000);
	tenta = strtok(line, " ,()=");
	lendoPVTSimPre >> chave;
	while (chave != "BUBBLEPRESSURES") {
		lendoPVTSimPre >> chave;
	}

	lendoPVTSimPre.get(line, 4000);
	int ntab=0;
	tenta = strtok(line, " ,()=");
	while(tenta != nullptr && std::string(tenta)!="Pa"){
		ntab++;
		tenta = strtok(NULL, " ,)/");
	}

	lendoPVTSimPre.close();
	tabent.npont=ntab;


	string impfile;
	impfile = condpocinj.pvtsimarqInj;
	string dadosMR = impfile;
	ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
	//char* tenta;
	//tenta = new char[400];
	//char line[4000];
	//double testatok;



	int ndiv = tabent.npont - 1;
	//double pteste = tabent.pmin;
	//double tteste = tabent.tmin;
	//double dpteste = (tabent.pmax - pteste) / ndiv;
	//double dtteste = (tabent.tmax - tteste) / ndiv;

	Vcr<double> presPVTSim(ndiv + 1);
	Vcr<double> tempPVTSim(ndiv + 1);

	FullMtx<double> RhoTemp(ndiv + 2, ndiv + 2);
	FullMtx<double> ViscTemp(ndiv + 2, ndiv + 2);
	FullMtx<double> CondTemp(ndiv + 2, ndiv + 2);
	FullMtx<double> CpTemp(ndiv + 2, ndiv + 2);
	FullMtx<double> DrhoDtTemp(ndiv + 2, ndiv + 2);
	double rhol;
	double rhog;
	double viscl;
	double viscg;
	double condl;
	double condg;
	double cplV;
	double cpgV;
	double DrhoDtlV;
	double DrhoDtgV;
	double rs;
	/*while (chave !="STDGASDENSITY")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=kg/m3");
	tenta = strtok(line, " ,()=");
    fluc.rholStd=atof(tenta);*/

	while (chave != "STDGASDENSITY")
		lendoPVTSim >> chave;
	lendoPVTSim >> chave;
	lendoPVTSim >> chave;
	fluc.rholStd = stof(chave);


	while (chave != "PRESSURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	presPVTSim[0] = atof(tenta) * 1.01971621e-5;
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		presPVTSim[kontaPVT] = atof(tenta) * 1.01971621e-5;
	}
	while (chave != "TEMPERATURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	tempPVTSim[0] = atof(tenta);
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		tempPVTSim[kontaPVT] = atof(tenta);
	}

	for (int i = 1; i <= ndiv + 1; i++) {
		RhoTemp[i][0] = presPVTSim[i - 1];
		ViscTemp[i][0] = presPVTSim[i - 1];
		CondTemp[i][0] = presPVTSim[i - 1];
		CpTemp[i][0] = presPVTSim[i - 1];
		DrhoDtTemp[i][0] = presPVTSim[i - 1];
		for (int j = 1; j <= ndiv + 1; j++) {
			RhoTemp[0][j] = tempPVTSim[j - 1];
			ViscTemp[0][j] = tempPVTSim[j - 1];
			CondTemp[0][j] = tempPVTSim[j - 1];
			CpTemp[0][j] = tempPVTSim[j - 1];
			DrhoDtTemp[0][j] = tempPVTSim[j - 1];
			while (chave != "POINT")
				lendoPVTSim >> chave;
			//char line[1000];
			lendoPVTSim.get(line, 4000);
			tenta = strtok(line, " ,()=");
			for (int kontaPVT = 0; kontaPVT < 1; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			rhog = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			rhol = testatok;
			for (int kontaPVT = 0; kontaPVT < 2; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			DrhoDtgV = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			DrhoDtlV = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			rs = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			viscg = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			viscl = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			cpgV = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			cplV = testatok;
			for (int kontaPVT = 0; kontaPVT < 2; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			condg = testatok;
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			condl = testatok;
			/*if(rs>=0.5){
			 RhoTemp[i][j]=rhog;
			 ViscTemp[i][j]=viscg;
			 CondTemp[i][j]=condg;
			 CpTemp[i][j]=cpgV;
			 DrhoDtTemp[i][j]=DrhoDtgV;
			 }
			 else{
			 RhoTemp[i][j]=rhol;
			 ViscTemp[i][j]=viscl;
			 CondTemp[i][j]=condl;
			 CpTemp[i][j]=cplV;
			 DrhoDtTemp[i][j]=DrhoDtlV;
			 }*/
			RhoTemp[i][j] = rs * rhog + (1 - rs) * rhol;
			ViscTemp[i][j] = rs * viscg + (1 - rs) * viscl;
			CondTemp[i][j] = rs * condg + (1 - rs) * condl;
			CpTemp[i][j] = rs * cpgV + (1 - rs) * cplV;
			DrhoDtTemp[i][j] = rs * DrhoDtgV + (1 - rs) * DrhoDtlV;
			//tteste += dtteste;
			if (i == ndiv + 1 && j == ndiv + 1)
				break;
			while (chave != "PVTTABLE")
				lendoPVTSim >> chave;
		}
		//pteste += dpteste;
		//tteste = tabent.tmin;
	}
	lendoPVTSim.close();
	RhoInj = new double*[ndiv + 2];
	ViscInj = new double*[ndiv + 2];
	CondInj = new double*[ndiv + 2];
	CpInj = new double*[ndiv + 2];
	DrhoDtInj = new double*[ndiv + 2];
	for (int i = 0; i < ndiv + 2; i++) {
		RhoInj[i] = new double[ndiv + 2];
		ViscInj[i] = new double[ndiv + 2];
		CondInj[i] = new double[ndiv + 2];
		CpInj[i] = new double[ndiv + 2];
		DrhoDtInj[i] = new double[ndiv + 2];
		for (int j = 0; j < ndiv + 2; j++) {
			RhoInj[i][j] = RhoTemp[i][j];
			ViscInj[i][j] = ViscTemp[i][j];
			CondInj[i][j] = CondTemp[i][j];
			CpInj[i][j] = CpTemp[i][j];
			DrhoDtInj[i][j] = DrhoDtTemp[i][j];
		}
	}

	ostringstream saidaRhoInj;
	saidaRhoInj << pathPrefixoArqSaida << "perfilRhoInj.dat";
	string tmp = saidaRhoInj.str();
	ofstream escreveRhoInj(tmp.c_str(), ios_base::out);
	escreveRhoInj << RhoTemp;
	escreveRhoInj.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}

	ostringstream saidaViscInj;
	saidaViscInj << pathPrefixoArqSaida << "perfilViscInj.dat";
	tmp = saidaViscInj.str();
	ofstream escreveViscInj(tmp.c_str(), ios_base::out);
	escreveViscInj << ViscTemp;
	escreveViscInj.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}

	ostringstream saidaCondInj;
	saidaCondInj << pathPrefixoArqSaida << "perfilCondInj.dat";
	tmp = saidaCondInj.str();
	ofstream escreveCondInj(tmp.c_str(), ios_base::out);
	escreveCondInj << CondTemp;
	escreveCondInj.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}

	ostringstream saidaCpInj;
	saidaCpInj << pathPrefixoArqSaida << "perfilCpInj.dat";
	tmp = saidaCpInj.str();
	ofstream escreveCpInj(tmp.c_str(), ios_base::out);
	escreveCpInj << CpTemp;
	escreveCpInj.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}

	ostringstream saidaDrhoDtInj;
	saidaCpInj << pathPrefixoArqSaida << "perfilDrhoDtInj.dat";
	tmp = saidaDrhoDtInj.str();
	ofstream escreveDrhoDtInj(tmp.c_str(), ios_base::out);
	escreveDrhoDtInj << DrhoDtTemp;
	escreveDrhoDtInj.close();
	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}
	//delete [] tenta;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void Ler::geraTabFlash(int flu, int var) {

	string pvtfile;
	if(pocinjec==1)pvtfile =condpocinj.pvtsimarqInj;
	else pvtfile = pvtsimarq;
	string dadosMR = pvtfile;
	ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
	string chave;
	char* tenta;
	//tenta = new char[400];
	double testatok;
	int ndiv = tabent.npont - 1;

	Vcr<double> presPVTSim(ndiv + 1);
	Vcr<double> tempPVTSim(ndiv + 1);

	FullMtx<double> VarTemp(ndiv + 2, ndiv + 2);

	char line[50000];
	//lendoPVTSim.get(line, 4000);
	std::string linha_completa = ler_linha_continua(lendoPVTSim);
	strcpy(line, linha_completa.c_str());
	tenta = strtok(line, " ,()=");
	while (tenta != nullptr && strcmp(tenta, "PHASE") != 0) {
		tenta = strtok(NULL, " ,()=");
	}
	tenta = strtok(NULL, " ,()=");
	int lacoleitura;

	int threeOrtwo = 0;
	if (strcmp(tenta, "THREE") == 0) {
		threeOrtwo = 1;
		if (var == 1)
			lacoleitura = 1;  //rhog
		else if (var == 2)
			lacoleitura = 2;  //rhol
		else if (var == 3)
			lacoleitura = 4;  //drhogdp
		else if (var == 4)
			lacoleitura = 5;  //drholdp
		else if (var == 5)
			lacoleitura = 7;  //dhhogdt
		else if (var == 6)
			lacoleitura = 8;  //drholdt
		else if (var == 7)
			lacoleitura = 9;  //drhowdt
		else if (var == 8)
			lacoleitura = 10;  //RS
		else if (var == 9)
			lacoleitura = 15;  //Cpg
		else if (var == 10)
			lacoleitura = 16;  //Cpl
		else if (var == 11)
			lacoleitura = 17;  //Cpw
		else if (var == 12)
			lacoleitura = 18;  //Hg
		else if (var == 13)
			lacoleitura = 19;  //Hl
		else if (var == 14)
			lacoleitura = 24;  //Sig g-l
		else if (var == 15)
			lacoleitura = 25;  //Sig g-w
		else if (var == 16)
			lacoleitura = 13;  //visc oleo//alteracao6
		else if (var == 17)
			lacoleitura = 12;  //visc gas//alteracao6
	} else {
		if (var == 1)
			lacoleitura = 1;  //rhog
		else if (var == 2)
			lacoleitura = 2;  //rhol
		else if (var == 3)
			lacoleitura = 3;  //drhogdp
		else if (var == 4)
			lacoleitura = 4;  //drholdp
		else if (var == 5)
			lacoleitura = 5;  //dhhogdt
		else if (var == 6)
			lacoleitura = 6;  //drholdt
		else if (var == 8)
			lacoleitura = 7;  //RS
		else if (var == 9)
			lacoleitura = 10;  //Cpg
		else if (var == 10)
			lacoleitura = 11;  //Cpl
		else if (var == 12)
			lacoleitura = 12;  //Hg
		else if (var == 13)
			lacoleitura = 13;  //Hl
		else if (var == 14)
			lacoleitura = 16;  //Sig g-l
		else if (var == 16)
			lacoleitura = 9;  //visc oleo//alteracao6
		else if (var == 17)
			lacoleitura = 8;  //visc gas//alteracao6
	}
	lendoPVTSim.close();


	lendoPVTSim.open(dadosMR.c_str(), ios_base::in);
	while (chave != "PRESSURE")
		lendoPVTSim >> chave;
	//lendoPVTSim.get(line, 4000);
	linha_completa = ler_linha_continua(lendoPVTSim);
	strcpy(line, linha_completa.c_str());
	tenta = strtok(line, " ,()=");
	presPVTSim[0] = atof(tenta) * 1.01971621e-5;
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		presPVTSim[kontaPVT] = atof(tenta) * 1.01971621e-5;
	}
	tenta = strtok(NULL, " ,()=");
	chave = std::string(tenta);
	while (chave != "TEMPERATURE"){
		//lendoPVTSim >> chave;
		tenta = strtok(NULL, " ,()=");
		chave = std::string(tenta);
	}
	//lendoPVTSim.get(line, 4000);
	//linha_completa = ler_linha_continua(lendoPVTSim);
	//strcpy(line, linha_completa.c_str());
	tenta = strtok(NULL, " ,()=");
	tempPVTSim[0] = atof(tenta);
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		tempPVTSim[kontaPVT] = atof(tenta);
	}
	if(var==1){
		tabent.pmin=presPVTSim[0];
		tabent.pmax=presPVTSim[ndiv];
		tabent.tmin=tempPVTSim[0];
		tabent.tmax=tempPVTSim[ndiv];
	}

	for (int i = 1; i <= ndiv + 1; i++) {
		VarTemp[i][0] = presPVTSim[i - 1];
		for (int j = 1; j <= ndiv + 1; j++) {
			VarTemp[0][j] = tempPVTSim[j - 1];
			//HLatTemp[i][j]=arq.flup.EntalpGasIsoT(pteste,tteste)-arq.flup.EntalpLiqIsoT(pteste,tteste);
			while (chave != "POINT")
				lendoPVTSim >> chave;
			//lendoPVTSim.get(line, 4000);
			linha_completa = ler_linha_continua(lendoPVTSim);
			strcpy(line, linha_completa.c_str());
			tenta = strtok(line, " ,()=");
			for (int kontaPVT = 0; kontaPVT < lacoleitura; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			VarTemp[i][j] = testatok;
			//tenta = strtok(NULL, " ,");
			//testatok = atof(tenta);
			//VarTemp[i][j] -= testatok;

			if (i == ndiv + 1 && j == ndiv + 1)
				break;
			while (chave != "PVTTABLE")
				lendoPVTSim >> chave;
		}

	}
	lendoPVTSim.close();
	for (int i = 0; i < ndiv + 2; i++) {
		for (int j = 0; j < ndiv + 2; j++) {
			if (threeOrtwo == 1) {
				if (var == 1)
					flash[flu].rhogF[i][j] = VarTemp[i][j];  //rhog
				else if (var == 2)
					flash[flu].rholF[i][j] = VarTemp[i][j];  //rhol
				else if (var == 3)
					flash[flu].DrhogDpF[i][j] = VarTemp[i][j]/**98066.14*/; //drhogdp
				else if (var == 4)
					flash[flu].DrholDpF[i][j] = VarTemp[i][j]/**98066.14*/; //drholdp
				else if (var == 5)
					flash[flu].DrhogDtF[i][j] = VarTemp[i][j];  //dhhogdt
				else if (var == 6)
					flash[flu].DrholDtF[i][j] = VarTemp[i][j];  //drholdt
				else if (var == 7)
					flash[flu].DrhowDtF[i][j] = VarTemp[i][j];  //drhowdt
				else if (var == 8)
					flash[flu].RSF[i][j] = VarTemp[i][j];  //RS
				else if (var == 9)
					flash[flu].cpgF[i][j] = VarTemp[i][j];  //Cpg
				else if (var == 10)
					flash[flu].cplF[i][j] = VarTemp[i][j];  //Cpl
				else if (var == 11)
					flash[flu].cpwF[i][j] = VarTemp[i][j];  //Cpw
				else if (var == 12)
					flash[flu].HgF[i][j] = VarTemp[i][j];  //Hg
				else if (var == 13)
					flash[flu].HlF[i][j] = VarTemp[i][j];  //Hl
				else if (var == 14)
					flash[flu].sigOGF[i][j] = VarTemp[i][j];  //Sig g-l
				else if (var == 15)
					flash[flu].sigWGF[i][j] = VarTemp[i][j];  //Sig g-w
				else if (var == 16)
					flash[flu].viscO[i][j] = VarTemp[i][j]; //visc oleo //alteracao6
				else if (var == 17)
					flash[flu].viscG[i][j] = VarTemp[i][j]; //visc gas //alteracao6

				if (var == 8 && i > 0 && j > 0) {
					double rhostd = 141.5 * 1000.
							/ (131.5 + flup[flu].API);
					double rgstd = 1.225 * flup[flu].Deng;
					flash[flu].RSF[i][j] = (flup[flu].RGO- (rhostd + rgstd * flup[flu].RGO)* flash[flu].RSF[i][j] / rgstd) * 35.31467
							/ 6.29;
					if(flash[flu].RSF[i][j]<0)flash[flu].RSF[i][j]=0.;
					//falcatrua-teste de consistencia de RS do PVTSIM
					//if(i>2 && flash[flu].RSF[i][j]<flash[flu].RSF[i-1][j])flash[flu].RSF[i][j]=flash[flu].RSF[i-1][j];
					VarTemp[i][j] = flash[flu].RSF[i][j] * 6.29 / 35.31467;
				}
				if ((var == 16 || var == 17) && i > 0 && j > 0) {  //alteracao6
					if (var == 16)
						flash[flu].viscO[i][j] *= 1000.;
					else
						flash[flu].viscG[i][j] *= 1000.;
					VarTemp[i][j] *= 1000.;
				}
			} else {
				if (var == 1)
					flash[flu].rhogF[i][j] = VarTemp[i][j];  //rhog
				else if (var == 2)
					flash[flu].rholF[i][j] = VarTemp[i][j];  //rhol
				else if (var == 3)
					flash[flu].DrhogDpF[i][j] = VarTemp[i][j]/**98066.5*/; //drhogdp
				else if (var == 4)
					flash[flu].DrholDpF[i][j] = VarTemp[i][j]/**98066.5*/; //drholdp
				else if (var == 5)
					flash[flu].DrhogDtF[i][j] = VarTemp[i][j];  //dhhogdt
				else if (var == 6)
					flash[flu].DrholDtF[i][j] = VarTemp[i][j];  //drholdt
				else if (var == 8)
					flash[flu].RSF[i][j] = VarTemp[i][j];  //RS
				else if (var == 9)
					flash[flu].cpgF[i][j] = VarTemp[i][j];  //Cpg
				else if (var == 10)
					flash[flu].cplF[i][j] = VarTemp[i][j];  //Cpl
				else if (var == 12)
					flash[flu].HgF[i][j] = VarTemp[i][j];  //Hg
				else if (var == 13)
					flash[flu].HlF[i][j] = VarTemp[i][j];  //Hl
				else if (var == 14)
					flash[flu].sigOGF[i][j] = VarTemp[i][j];  //Sig g-l
				else if (var == 16)
					flash[flu].viscO[i][j] = VarTemp[i][j]; //visc oleo //alteracao6
				else if (var == 17)
					flash[flu].viscG[i][j] = VarTemp[i][j]; //visc gas //alteracao6

				if (var == 8 && i > 0 && j > 0) {
					double rhostd = 141.5 * 1000.
							/ (131.5 + flup[flu].API);
					double rgstd = 1.225 * flup[flu].Deng;
					flash[flu].RSF[i][j] = (flup[flu].RGO
							- (rhostd + rgstd * flup[flu].RGO)
									* flash[flu].RSF[i][j] / rgstd) * 35.31467
							/ 6.29;
					if(flash[flu].RSF[i][j]<0)flash[flu].RSF[i][j]=0.;
					//falcatrua-teste de consistencia de RS do PVTSIM
					//if(i>2 && flash[flu].RSF[i][j]<flash[flu].RSF[i-1][j])flash[flu].RSF[i][j]=flash[flu].RSF[i-1][j];
					VarTemp[i][j] = flash[flu].RSF[i][j] * 6.29 / 35.31467;
				}
				if ((var == 16 || var == 17) && i > 0 && j > 0) {  //alteracao6
					if (var == 16)
						flash[flu].viscO[i][j] *= 1000.;
					else
						flash[flu].viscG[i][j] *= 1000.;
					VarTemp[i][j] *= 1000.;
				}
			}
		}

	}
	/*double del2e;
	 double del1e;
	 double del1d;
	 double del2d;
	 for (int i = 3; i < ndiv-1; i++) {
	 for (int j = 3; j < ndiv-1; j++){

	 if(var==1){
	 del2e=flash[flu].rhogF[i-1][j]-flash[flu].rhogF[i-2][j];
	 del1e=flash[flu].rhogF[i][j]-flash[flu].rhogF[i-1][j];
	 del1d=flash[flu].rhogF[i+1][j]-flash[flu].rhogF[i][j];
	 del2d=flash[flu].rhogF[i+2][j]-flash[flu].rhogF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].rhogF[i][j]=flash[flu].rhogF[i-1][j];
	 }
	 else if(var==2){
	 //flash[flu].rholF[i][j]= VarTemp[i][j];
	 del2e=flash[flu].rholF[i-1][j]-flash[flu].rholF[i-2][j];
	 del1e=flash[flu].rholF[i][j]-flash[flu].rholF[i-1][j];
	 del1d=flash[flu].rholF[i+1][j]-flash[flu].rholF[i][j];
	 del2d=flash[flu].rholF[i+2][j]-flash[flu].rholF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].rholF[i][j]=flash[flu].rholF[i-1][j];
	 }
	 else if(var==3){
	 //flash[flu].DrhogDpF[i][j]= VarTemp[i][j];
	 del2e=flash[flu].DrhogDpF[i-1][j]-flash[flu].DrhogDpF[i-2][j];
	 del1e=flash[flu].DrhogDpF[i][j]-flash[flu].DrhogDpF[i-1][j];
	 del1d=flash[flu].DrhogDpF[i+1][j]-flash[flu].DrhogDpF[i][j];
	 del2d=flash[flu].DrhogDpF[i+2][j]-flash[flu].DrhogDpF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].DrhogDpF[i][j]=flash[flu].DrhogDpF[i-1][j];
	 }
	 else if(var==4){
	 //flash[flu].DrholDpF[i][j]= VarTemp[i][j];
	 del2e=flash[flu].DrholDpF[i-1][j]-flash[flu].DrholDpF[i-2][j];
	 del1e=flash[flu].DrholDpF[i][j]-flash[flu].DrholDpF[i-1][j];
	 del1d=flash[flu].DrholDpF[i+1][j]-flash[flu].DrholDpF[i][j];
	 del2d=flash[flu].DrholDpF[i+2][j]-flash[flu].DrholDpF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].DrholDpF[i][j]=flash[flu].DrholDpF[i-1][j];
	 }
	 else if(var==5){
	 //flash[flu].DrhogDtF[i][j]= VarTemp[i][j];
	 del2e=flash[flu].DrhogDtF[i-1][j]-flash[flu].DrhogDtF[i-2][j];
	 del1e=flash[flu].DrhogDtF[i][j]-flash[flu].DrhogDtF[i-1][j];
	 del1d=flash[flu].DrhogDtF[i+1][j]-flash[flu].DrhogDtF[i][j];
	 del2d=flash[flu].DrhogDtF[i+2][j]-flash[flu].DrhogDtF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].DrhogDtF[i][j]=flash[flu].DrhogDtF[i-1][j];
	 }
	 else if(var==6){
	 //flash[flu].DrholDtF[i][j]= VarTemp[i][j];
	 del2e=flash[flu].rhogF[i-1][j]-flash[flu].rhogF[i-2][j];
	 del1e=flash[flu].rhogF[i][j]-flash[flu].rhogF[i-1][j];
	 del1d=flash[flu].rhogF[i+1][j]-flash[flu].rhogF[i][j];
	 del2d=flash[flu].rhogF[i+2][j]-flash[flu].rhogF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].rhogF[i][j]=flash[flu].rhogF[i-1][j];
	 }
	 else if(var==7){
	 //flash[flu].DrhowDtF[i][j]= VarTemp[i][j];
	 del2e=flash[flu].DrhowDtF[i-1][j]-flash[flu].DrhowDtF[i-2][j];
	 del1e=flash[flu].DrhowDtF[i][j]-flash[flu].DrhowDtF[i-1][j];
	 del1d=flash[flu].DrhowDtF[i+1][j]-flash[flu].DrhowDtF[i][j];
	 del2d=flash[flu].DrhowDtF[i+2][j]-flash[flu].DrhowDtF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].DrhowDtF[i][j]=flash[flu].DrhowDtF[i-1][j];
	 }
	 else if(var==8){
	 //flash[flu].RSF[i][j]= VarTemp[i][j];
	 del2e=flash[flu].RSF[i-1][j]-flash[flu].RSF[i-2][j];
	 del1e=flash[flu].RSF[i][j]-flash[flu].RSF[i-1][j];
	 del1d=flash[flu].RSF[i+1][j]-flash[flu].RSF[i][j];
	 del2d=flash[flu].RSF[i+2][j]-flash[flu].RSF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash[flu].RSF[i][j]=flash[flu].RSF[i-1][j];
	 }


	 }
	 }*/

	string variavel;
	if (var == 1)
		variavel = "rhog";
	else if (var == 2)
		variavel = "rhol";
	else if (var == 3)
		variavel = "drhogdp";
	else if (var == 4)
		variavel = "drholdp";
	else if (var == 5)
		variavel = "dhhogdt";
	else if (var == 6)
		variavel = "drholdt";
	else if (var == 7)
		variavel = "drhowdt";
	else if (var == 8)
		variavel = "RS";
	else if (var == 9)
		variavel = "Cpg";
	else if (var == 10)
		variavel = "Cpl";
	else if (var == 11)
		variavel = "Cpw";
	else if (var == 12)
		variavel = "Hg";
	else if (var == 13)
		variavel = "Hl";
	else if (var == 14)
		variavel = "Sig-g-l";
	else if (var == 15)
		variavel = "Sig-g-w";
	else if (var == 16)
		variavel = "visc oleo";  //alteracao6
	else if (var == 17)
		variavel = "visc gas";  //alteracao6
	ostringstream saidaLatente;
	saidaLatente << pathPrefixoArqSaida << "perfiFlash-" << flu << "-var-"
			<< variavel << ".dat";
	string tmp = saidaLatente.str();
	ofstream escreveMass(tmp.c_str(), ios_base::out);
	escreveMass << VarTemp;
	escreveMass.close();
	//delete [] tenta;
}

int Ler::buscaIndiceMeioP(double lverif) {

	double lteste = 0.;
	int kontacel = 0;
	while ((lteste + celp[kontacel].dx / 2.) < lverif) {
		lteste += celp[kontacel].dx;
		kontacel++;
		if (kontacel >= ncelp)
			//exit(-1);
			return ncelp - 1;
	}
	if (kontacel == 0)
		return kontacel;
	else {
		if (fabsl(lteste + celp[kontacel].dx / 2. - lverif)
				< fabsl(lteste - celp[kontacel - 1].dx / 2. - lverif))
			return kontacel;
		else
			return kontacel - 1;
	}
}
int Ler::buscaIndiceMeioG(double lverif) {

	double lteste = 0.;
	int kontacel = 0;
	while ((lteste + celg[kontacel].dx / 2.) < lverif) {
		lteste += celg[kontacel].dx;
		kontacel++;
		if (kontacel >= ncelg)
			//exit(-1);
			return ncelg - 1;
	}
	if (kontacel == 0)
		return kontacel;
	else {
		if (fabsl(lteste + celg[kontacel].dx / 2. - lverif)
				< fabsl(lteste - celg[kontacel - 1].dx / 2. - lverif))
			return kontacel;
		else
			return kontacel - 1;
	}
}
int Ler::buscaIndiceFrontP(double lverif) {

	double lteste = 0.;
	int kontacel = 0;
	while ((lteste + celp[kontacel].dx) < lverif) {
		lteste += celp[kontacel].dx;
		kontacel++;
		if (kontacel >= ncelp)
			//exit(-1);
			return ncelp - 1;
	}
	if (kontacel == 0)
		return kontacel;
	else {
		if (fabsl(lteste + celp[kontacel].dx - lverif)
				< fabsl(lteste - celp[kontacel - 1].dx - lverif))
			return kontacel;
		else
			return kontacel - 1;
	}
}
int Ler::buscaIndiceFrontG(double lverif) {

	double lteste = 0.;
	int kontacel = 0;
	while ((lteste + celg[kontacel].dx) < lverif) {
		lteste += celg[kontacel].dx;
		kontacel++;
		if (kontacel >= ncelg)
			//exit(-1);
			return ncelg - 1;
	}
	if (kontacel == 0)
		return kontacel;
	else {
		if (fabsl(lteste + celg[kontacel].dx - lverif)
				< fabsl(lteste - celg[kontacel - 1].dx - lverif))
			return kontacel;
		else
			return kontacel - 1;
	}
}

int Ler::buscaIndiceUnidade(int iniSeg, int nuni,
		double comp) {

	while (comp
			- (unidadeP[nuni].dxVar[iniSeg + 1] - unidadeP[nuni].dxVar[0])
					* unidadeP[nuni].comp > 1e-5) {
		iniSeg++;
		//if(fabsl(comp-(unidadeP[nuni].dxVar[iniSeg+1]-unidadeP[nuni].dxVar[0]))<1e-5 && iniSeg>=unidadeP[nuni].nponts-1)
		if (iniSeg >= unidadeP[nuni].nponts - 1) {
			// determinar mensagem de falha
			string falha = string("Indice ") + to_string(iniSeg)
					+ string(" maior/igual que o numero de pontos ")
					+ to_string(unidadeP[nuni].nponts - 1)
					+ string(" da unidade de producao ") + to_string(nuni);
			// incluir falha
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "",
					"", falha);
			// gerar o arquivo de saida da simulacao
			logger.writeOutputLog();
			// encerrar a aplicacao
			exit(EXIT_SUCCESS);
		}
	}
	return iniSeg;
}

int Ler::buscaIndiceUnidadeG(int iniSeg, int nuni,
		double comp) {

	while (comp
			- (unidadeG[nuni].dxVar[iniSeg + 1] - unidadeG[nuni].dxVar[0])
					* unidadeG[nuni].comp > 1e-5) {
		iniSeg++;
		if (iniSeg >= unidadeG[nuni].nponts - 1) {
			// determinar mensagem de falha
			string falha = string("Indice ") + to_string(iniSeg)
					+ string(" maior/igual que o numero de pontos ")
					+ to_string(unidadeG[nuni].nponts - 1)
					+ string(" da unidade de servico ") + to_string(nuni);
			// incluir falha
			logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION, "",
					"", falha);
			// gerar o arquivo de saida da simulacao
			logger.writeOutputLog();
			// encerrar a aplicacao
			exit(EXIT_SUCCESS);
		}
	}
	return iniSeg;
}

void Ler::listaevento(int inic, int extrem) {
	if (nipr > 0) {
		for (int i = 0; i < nipr; i++) {
			for (int j = 0; j < IPRS[i].seriep; j++)
				evento.push_back(IPRS[i].tpres[j]);
			for (int j = 0; j < IPRS[i].seriet; j++)
				evento.push_back(IPRS[i].ttemp[j]);
			for (int j = 0; j < IPRS[i].serieip; j++)
				evento.push_back(IPRS[i].tip[j]);
			for (int j = 0; j < IPRS[i].seriejp; j++)
				evento.push_back(IPRS[i].tjp[j]);
			for (int j = 0; j < IPRS[i].serieqm; j++)
				evento.push_back(IPRS[i].tqMax[j]);
		}
	}
	//if (lingas > 0) {
	// for (int i = 0; i < gasinj.parserie; i++)
	// evento.push_back(gasinj.tempo[i]);
	//}
	if (controDesc == 0) {  //alteracao5
		if (lingas > 0) {
			for (int i = 0; i < gasinj.parserie; i++)
				evento.push_back(gasinj.tempo[i]);
		}
	}
	if (ninjgas > 0) {
		for (int i = 0; i < ninjgas; i++)
			for (int j = 0; j < fonteg[i].parserie; j++)
				evento.push_back(fonteg[i].tempo[j]);
	}
	if (nvalv > 0) {
		for (int j = 0; j < nvalv; j++){
			for (int i = 0; i < valv[j].parserie; i++)
				evento.push_back(valv[j].tempo[i]);
		}
	}
	if (ninjliq > 0) {
		for (int i = 0; i < ninjliq; i++)
			for (int j = 0; j < fontel[i].parserie; j++)
				evento.push_back(fontel[i].tempo[j]);
	}
	for (int i = 0; i < master1.parserie; i++)
		evento.push_back(master1.tempo[i]);
	if (lingas > 0)
		for (int i = 0; i < master2.parserie; i++)
			evento.push_back(master2.tempo[i]);
	//for (int i = 0; i < psep.parserie; i++)
	//evento.push_back(psep.tempo[i]);
	if (controDesc == 0 && ((*vg1dSP).chaverede == 0 || extrem == 1)) {  //alteracao5
		for (int i = 0; i < psep.parserie; i++)
			evento.push_back(psep.tempo[i]);
	}
	for (int i = 0; i < chokep.parserie; i++)
		evento.push_back(chokep.tempo[i]);
	if(lingas>0 && chokes.parserie>0)
		for (int i = 0; i < chokes.parserie; i++)
			evento.push_back(chokes.tempo[i]);
	if (nbcs > 0) {
		for (int i = 0; i < nbcs; i++)
			for (int j = 0; j < bcs[i].parserie; j++)
				evento.push_back(bcs[i].tempo[j]);
	}
	if (nbvol > 0) {
		for (int i = 0; i < nbvol; i++)
			for (int j = 0; j < bvol[i].parserie; j++)
				evento.push_back(bvol[i].tempo[j]);
	}
	if (ndpreq > 0) {
		for (int i = 0; i < ndpreq; i++)
			for (int j = 0; j < dpreq[i].parserie; j++)
				evento.push_back(dpreq[i].tempo[j]);
	}
	if (npig > 0) {
		for (int i = 0; i < npig; i++)
			evento.push_back(pig[i].tempo);
	}
	//if(nperfisp>0)for(int i=0; i<profp.n; i++)evento.push_back(profp.tempo[i]);
	//if(nperfisg>0 && lingas>0)for(int i=0; i<profg.n; i++)evento.push_back(profg.tempo[i]);
	if (evento.size() > 0)
		sort(evento.begin(), evento.end());
	if (evento.size() > 1) {
		double replica = evento[0];
		for (int i = 1; i < evento.size(); i++) {
			if (fabsl(replica - evento[i]) < 1e-5) {
				evento.erase(evento.begin() + i);
				i--;
			} else
				replica = evento[i];
		}
	}
	if (((*vg1dSP).chaverede == 0 || inic == 1) && ConContEntrada == 1) {  //alteracao9
		for (int i = 0; i < CCPres.parserie; i++)
			evento.push_back(CCPres.tempo[i]);
	}
	if (((*vg1dSP).chaverede == 0 || inic == 1) && ConContEntrada == 2) {  //alteracao9
		for (int i = 0; i < CCVPres.parserie; i++)
			evento.push_back(CCVPres.tempo[i]);
	}

}

void Ler::geraevento(int inic, int extrem) {
	LogEvento e;
	if (nipr > 0) {
		for (int i = 0; i < nipr; i++) {
			if (IPRS[i].seriep > 1) {
				for (int j = 0; j < IPRS[i].seriep; j++) {
					if (j < IPRS[i].seriep - 1) {
						if (fabsl(IPRS[i].pres[j] - IPRS[i].pres[j + 1])
								> 1e-15) {
							e.instante = IPRS[i].tpres[j];
							e.duracao = IPRS[i].tpres[j + 1] - IPRS[i].tpres[j];
							if (IPRS[i].pres[j] > IPRS[i].pres[j + 1])
								e.descricao = "Diminuindo Pressao Estatica";
							else
								e.descricao = "Aumentando Pressao Estatica";
							e.estIni = IPRS[i].pres[j];
							e.estFim = IPRS[i].pres[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
			if (IPRS[i].seriet > 1) {
				for (int j = 0; j < IPRS[i].seriet; j++) {
					if (j < IPRS[i].seriet - 1) {
						if (fabsl(IPRS[i].temp[j] - IPRS[i].temp[j + 1])
								> 1e-15) {
							e.instante = IPRS[i].ttemp[j];
							e.duracao = IPRS[i].ttemp[j + 1] - IPRS[i].ttemp[j];
							if (IPRS[i].temp[j] > IPRS[i].temp[j + 1])
								e.descricao =
										"Diminuindo Temperatura de Reservatorio";
							else
								e.descricao =
										"Aumentando Temperatura de Reservatorio";
							e.estIni = IPRS[i].temp[j];
							e.estFim = IPRS[i].temp[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
			if (IPRS[i].serieip > 1) {
				for (int j = 0; j < IPRS[i].serieip; j++) {
					if (j < IPRS[i].serieip - 1) {
						if (fabsl(IPRS[i].ip[j] - IPRS[i].ip[j + 1]) > 1e-15) {
							e.instante = IPRS[i].tip[j];
							e.duracao = IPRS[i].tip[j + 1] - IPRS[i].tip[j];
							if (IPRS[i].ip[j] > IPRS[i].ip[j + 1])
								e.descricao =
										"Diminuindo indice de Produtividade";
							else
								e.descricao =
										"Aumentando indice de Produtividade";
							e.estIni = IPRS[i].ip[j];
							e.estFim = IPRS[i].ip[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
			if (IPRS[i].seriejp > 1) {
				for (int j = 0; j < IPRS[i].seriejp; j++) {
					if (j < IPRS[i].seriejp - 1) {
						if (fabsl(IPRS[i].jp[j] - IPRS[i].jp[j + 1]) > 1e-15) {
							e.instante = IPRS[i].tjp[j];
							e.duracao = IPRS[i].tjp[j + 1] - IPRS[i].tjp[j];
							if (IPRS[i].jp[j] > IPRS[i].jp[j + 1])
								e.descricao =
										"Diminuindo indice de Injetividade";
							else
								e.descricao =
										"Aumentando indice de Injetividade";
							e.estIni = IPRS[i].jp[j];
							e.estFim = IPRS[i].jp[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
			if (IPRS[i].serieqm > 1) {
				for (int j = 0; j < IPRS[i].serieqm; j++) {
					if (j < IPRS[i].serieqm - 1) {
						if (fabsl(IPRS[i].qMax[j] - IPRS[i].qMax[j + 1]) > 1e-15) {
							e.instante = IPRS[i].tqMax[j];
							e.duracao = IPRS[i].tqMax[j + 1] - IPRS[i].tqMax[j];
							if (IPRS[i].qMax[j] > IPRS[i].qMax[j + 1])
								e.descricao =
										"Diminuindo vazão máxima de IPR";
							else
								e.descricao =
										"Aumentando vazão máxima de IPR";
							e.estIni = IPRS[i].qMax[j];
							e.estFim = IPRS[i].qMax[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
		}
	}
	if (lingas > 0) {
		if (gasinj.parserie > 1) {
			for (int j = 0; j < gasinj.parserie; j++) {
				/* if (j < gasinj.parserie - 1) {
				 if (gasinj.tipoCC == 0) {
				 if (fabsl(gasinj.presinj[j] - gasinj.presinj[j + 1])
				 > 1e-15) {
				 e.instante = gasinj.tempo[j];
				 e.duracao = gasinj.tempo[j + 1] - gasinj.tempo[j];
				 if (gasinj.presinj[j] > gasinj.presinj[j + 1])
				 e.descricao =
				 "Diminuindo Pressao de Injecao de Gas";
				 else
				 e.descricao =
				 "Aumentando Pressao de Injecao de Gas";
				 e.estIni = gasinj.presinj[j];
				 e.estFim = gasinj.presinj[j + 1];
				 logevento.push_back(e);
				 logeventoEstat.push_back(e);
				 }
				 } else {
				 if (gasinj.tipoCC == 1) {
				 if (fabsl(gasinj.vazgas[j] - gasinj.vazgas[j + 1])
				 > 1e-15) {
				 e.instante = gasinj.tempo[j];
				 e.duracao = gasinj.tempo[j + 1] - gasinj.tempo[j];
				 if (gasinj.vazgas[j] > gasinj.vazgas[j + 1])
				 e.descricao =
				 "Diminuindo Vazao de Injecao de Gas";
				 else
				 e.descricao =
				 "Aumentando Vazao de Injecao de Gas";
				 e.estIni = gasinj.vazgas[j];
				 e.estFim = gasinj.vazgas[j + 1];
				 logevento.push_back(e);
				 logeventoEstat.push_back(e);
				 }
				 }
				 }
				 }*/
				if (controDesc == 0) {  //alteracao5
					if (j < gasinj.parserie - 1) {
						if (fabsl(gasinj.temperatura[j] - gasinj.temperatura[j + 1])
								> 1e-15) {
							e.instante = gasinj.tempo[j];
							e.duracao = gasinj.tempo[j + 1]
									- gasinj.tempo[j];
							if (gasinj.temperatura[j] > gasinj.temperatura[j + 1])
								e.descricao =
										"Diminuindo temperatura de Injecao de Gas";
							else
								e.descricao =
										"Aumentando temperatura de Injecao de Gas";
							e.estIni = gasinj.temperatura[j];
							e.estFim = gasinj.temperatura[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
						if (gasinj.tipoCC == 0) {
							if (fabsl(gasinj.presinj[j] - gasinj.presinj[j + 1])
									> 1e-15) {
								e.instante = gasinj.tempo[j];
								e.duracao = gasinj.tempo[j + 1]
										- gasinj.tempo[j];
								if (gasinj.presinj[j] > gasinj.presinj[j + 1])
									e.descricao =
											"Diminuindo Pressao de Injecao de Gas";
								else
									e.descricao =
											"Aumentando Pressao de Injecao de Gas";
								e.estIni = gasinj.presinj[j];
								e.estFim = gasinj.presinj[j + 1];
								logevento.push_back(e);
								logeventoEstat.push_back(e);
							}
						} else {

							if (gasinj.tipoCC == 1) {
								if (fabsl(
										gasinj.vazgas[j] - gasinj.vazgas[j + 1])
										> 1e-15) {
									e.instante = gasinj.tempo[j];
									e.duracao = gasinj.tempo[j + 1]
											- gasinj.tempo[j];
									if (gasinj.vazgas[j] > gasinj.vazgas[j + 1])
										e.descricao =
												"Diminuindo Vazao de Injecao de Gas";
									else
										e.descricao =
												"Aumentando Vazao de Injecao de Gas";
									e.estIni = gasinj.vazgas[j];
									e.estFim = gasinj.vazgas[j + 1];
									logevento.push_back(e);
									logeventoEstat.push_back(e);
								}
							}

						}
					}
				}
			}
		}
	}
	if (ninjgas > 0) {
		for (int i = 0; i < ninjgas; i++) {
			if (fonteg[i].parserie > 1) {
				for (int j = 0; j < fonteg[i].parserie; j++) {
					if (j < fonteg[i].parserie - 1) {
						if (fabsl(fonteg[i].vazgas[j] - fonteg[i].vazgas[j + 1])
								> 1e-15) {
							e.instante = fonteg[i].tempo[j];
							e.duracao = fonteg[i].tempo[j + 1]
									- fonteg[i].tempo[j];
							if (fonteg[i].vazgas[j] > fonteg[i].vazgas[j + 1])
								e.descricao = "Diminuindo Vazao de Gas, Fonte";
							else
								e.descricao = "Aumentando Vazao de Gas, Fonte";
							e.estIni = fonteg[i].vazgas[j];
							e.estFim = fonteg[i].vazgas[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
						if(fonteg[i].seco==0){
							if (fabsl(fonteg[i].vazcomp[j] - fonteg[i].vazcomp[j + 1])
									> 1e-15) {
								e.instante = fonteg[i].tempo[j];
								e.duracao = fonteg[i].tempo[j + 1]
															- fonteg[i].tempo[j];
								if (fonteg[i].vazcomp[j] > fonteg[i].vazcomp[j + 1])
									e.descricao = "Diminuindo Vazao de Fluido Complementar, Fonte";
								else
									e.descricao = "Aumentando Vazao de Fluido Complementar, Fonte";
								e.estIni = fonteg[i].vazcomp[j];
								e.estFim = fonteg[i].vazcomp[j + 1];
								logevento.push_back(e);
								logeventoEstat.push_back(e);
							}
						}
						if (fabsl(fonteg[i].temp[j] - fonteg[i].temp[j + 1])
								> 1e-15) {
							e.instante = fonteg[i].tempo[j];
							e.duracao = fonteg[i].tempo[j + 1]
									- fonteg[i].tempo[j];
							if (fonteg[i].temp[j] > fonteg[i].temp[j + 1])
								e.descricao = "Diminuindo temperatura do gas, Fonte";
							else
								e.descricao = "Aumentando temperatura do gas, Fonte";
							e.estIni = fonteg[i].temp[j];
							e.estFim = fonteg[i].temp[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
		}
	}
	if (ninjliq > 0) {
		for (int i = 0; i < ninjliq; i++) {
			if (fontel[i].parserie > 1) {
				for (int j = 0; j < fontel[i].parserie; j++) {
					if (j < fontel[i].parserie - 1) {
						if (fabsl(fontel[i].vazliq[j] - fontel[i].vazliq[j + 1])
								> 1e-15) {
							e.instante = fontel[i].tempo[j];
							e.duracao = fontel[i].tempo[j + 1]
									- fontel[i].tempo[j];
							if (fontel[i].vazliq[j] > fontel[i].vazliq[j + 1])
								e.descricao =
										"Diminuindo Vazao de Liquido, Fonte";
							else
								e.descricao =
										"Aumentando Vazao de Liquido, Fonte";
							e.estIni = fontel[i].vazliq[j];
							e.estFim = fontel[i].vazliq[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
						if (fabsl(fontel[i].temp[j] - fontel[i].temp[j + 1])
								> 1e-15) {
							e.instante = fontel[i].tempo[j];
							e.duracao = fontel[i].tempo[j + 1]
									- fontel[i].tempo[j];
							if (fontel[i].temp[j] > fontel[i].temp[j + 1])
								e.descricao =
										"Diminuindo temperatura de Liquido, Fonte";
							else
								e.descricao =
										"Aumentando temperatura de Liquido, Fonte";
							e.estIni = fontel[i].temp[j];
							e.estFim = fontel[i].temp[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}

						if (fabsl(fontel[i].bet[j] - fontel[i].bet[j + 1])
								> 1e-15) {
							e.instante = fontel[i].tempo[j];
							e.duracao = fontel[i].tempo[j + 1]
									- fontel[i].tempo[j];
							if (fontel[i].bet[j] > fontel[i].bet[j + 1])
								e.descricao =
										"Diminuindo fracao de Liquido complementar, Fonte";
							else
								e.descricao =
										"Aumentando fracao de Liquido complementar, Fonte";
							e.estIni = fontel[i].bet[j];
							e.estFim = fontel[i].bet[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
		}
	}
	if (ninjmass> 0) {
		for (int i = 0; i < ninjmass; i++) {
			if (fontem[i].parserie > 1) {
				for (int j = 0; j < fontem[i].parserie; j++) {
					if (j < fontem[i].parserie - 1) {
						if (fabsl(fontem[i].vazMasT[j] - fontem[i].vazMasT[j + 1])
								> 1e-15) {
							e.instante = fontem[i].tempo[j];
							e.duracao = fontem[i].tempo[j + 1]
									- fontem[i].tempo[j];
							if (fontem[i].vazMasT[j] > fontem[i].vazMasT[j + 1])
								e.descricao =
										"Diminuindo Vazao massica total, Fonte";
							else
								e.descricao =
										"Aumentando Vazao massica total, Fonte";
							e.estIni = fontem[i].vazMasT[j];
							e.estFim = fontem[i].vazMasT[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
						if(indicaBeta){
							if (fabsl(fontem[i].vazMasC[j] - fontem[i].vazMasC[j + 1])
									> 1e-15) {
								e.instante = fontem[i].tempo[j];
								e.duracao = fontem[i].tempo[j + 1]
															- fontem[i].tempo[j];
								if (fontem[i].vazMasC[j] > fontem[i].vazMasC[j + 1])
									e.descricao =
											"Diminuindo Vazao massica fluido complementar, Fonte";
								else
									e.descricao =
											"Aumentando Vazao massica fluido complementar, Fonte";
								e.estIni = fontem[i].vazMasC[j];
								e.estFim = fontem[i].vazMasC[j + 1];
								logevento.push_back(e);
								logeventoEstat.push_back(e);
							}
						}
						if(fontem[i].condiTermo==1){
							if (fabsl(fontem[i].vazMasG[j] - fontem[i].vazMasG[j + 1])
									> 1e-15) {
								e.instante = fontem[i].tempo[j];
								e.duracao = fontem[i].tempo[j + 1]
										- fontem[i].tempo[j];
								if (fontem[i].vazMasG[j] > fontem[i].vazMasG[j + 1])
									e.descricao =
											"Diminuindo Vazao massica de Gas, Fonte";
								else
									e.descricao =
											"Aumentando Vazao massica de Gas, Fonte";
								e.estIni = fontem[i].vazMasG[j];
								e.estFim = fontem[i].vazMasG[j + 1];
								logevento.push_back(e);
								logeventoEstat.push_back(e);
							}
						}
					}
				}
			}
		}
	}
	if (nvalv > 0) {
		for (int i = 0; i < nvalv; i++) {
			for (int j = 0; j < valv[i].parserie; j++) {
				if (j < valv[i].parserie - 1) {
					if (fabsl(valv[i].abertura[j] - valv[i].abertura[j + 1])
							> 1e-15) {
						e.instante = valv[i].tempo[j];
						e.duracao = valv[i].tempo[j + 1] - valv[i].tempo[j];
						if (valv[i].abertura[j] > valv[i].abertura[j + 1])
							e.descricao = "Fechando Valvula";
						else
							e.descricao = "Abrindo Valvula";
						e.estIni = valv[i].abertura[j];
						e.estFim = valv[i].abertura[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
	if (nfuro > 0) {
		for (int i = 0; i < nfuro; i++) {
			for (int j = 0; j < furo[i].parserie; j++) {
				if (j < furo[i].parserie - 1) {
					if (fabsl(furo[i].abertura[j] - furo[i].abertura[j + 1])
							> 1e-15) {
						e.instante = furo[i].tempo[j];
						e.duracao = furo[i].tempo[j + 1] - furo[i].tempo[j];
						if (furo[i].abertura[j] > furo[i].abertura[j + 1])
							e.descricao = "Fechando Vazamento";
						else
							e.descricao = "Abrindo Vazamento";
						e.estIni = furo[i].abertura[j];
						e.estFim = furo[i].abertura[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
	if (master1.parserie > 1) {
		for (int j = 0; j < master1.parserie; j++) {
			if (j < master1.parserie - 1) {
				if (fabsl(master1.abertura[j] - master1.abertura[j + 1])
						> 1e-15) {
					e.instante = master1.tempo[j];
					e.duracao = master1.tempo[j + 1] - master1.tempo[j];
					if (master1.abertura[j] > master1.abertura[j + 1])
						e.descricao = "Fechando Master1";
					else
						e.descricao = "Abrindo Master1";
					e.estIni = master1.abertura[j];
					e.estFim = master1.abertura[j + 1];
					logevento.push_back(e);
					logeventoEstat.push_back(e);
				}
			}
		}
	}
	if (lingas > 0) {
		if (master2.parserie > 1) {
			for (int j = 0; j < master2.parserie; j++) {
				if (j < master2.parserie - 1) {
					if (fabsl(master2.abertura[j] - master2.abertura[j + 1])
							> 1e-15) {
						e.instante = master2.tempo[j];
						e.duracao = master2.tempo[j + 1] - master2.tempo[j];
						if (master2.abertura[j] > master2.abertura[j + 1])
							e.descricao = "Fechando Master2";
						else
							e.descricao = "Abrindo Master2";
						e.estIni = master2.abertura[j];
						e.estFim = master2.abertura[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}
	}
	/*if (psep.parserie > 1) {
	 for (int j = 0; j < psep.parserie; j++) {
	 if (j < psep.parserie - 1) {
	 if (fabsl(psep.pres[j] - psep.pres[j + 1]) > 1e-15) {
	 e.instante = psep.tempo[j];
	 e.duracao = psep.tempo[j + 1] - psep.tempo[j];
	 if (psep.pres[j] > psep.pres[j + 1])
	 e.descricao = "Diminuindo Pressao do Separador";
	 else
	 e.descricao = "Aumentando Pressao do Separador";
	 e.estIni = psep.pres[j];
	 e.estFim = psep.pres[j + 1];
	 logevento.push_back(e);
	 logeventoEstat.push_back(e);
	 }
	 }
	 }
	 }

	 if (chokep.parserie > 1) {
	 for (int j = 0; j < chokep.parserie; j++) {
	 if (j < chokep.parserie - 1) {
	 if (fabsl(chokep.abertura[j + 1] - chokep.abertura[j]) > 1e-15) {
	 e.instante = chokep.tempo[j];
	 e.duracao = chokep.tempo[j + 1] - chokep.tempo[j];
	 if (chokep.abertura[j] > chokep.abertura[j + 1])
	 e.descricao = "Fechando Choke de Superficie";
	 else
	 e.descricao = "Abrindo Choke de Superficie";
	 e.estIni = chokep.abertura[j];
	 e.estFim = chokep.abertura[j + 1];
	 logevento.push_back(e);
	 logeventoEstat.push_back(e);
	 }
	 }
	 }
	 }*/
	if (controDesc == 0 && ((*vg1dSP).chaverede == 0 || extrem == 1)) {  //alteracao5)
		if (psep.parserie > 1) {
			for (int j = 0; j < psep.parserie; j++) {
				if (j < psep.parserie - 1) {
					if (fabsl(psep.pres[j] - psep.pres[j + 1]) > 1e-15) {
						e.instante = psep.tempo[j];
						e.duracao = psep.tempo[j + 1] - psep.tempo[j];
						if (psep.pres[j] > psep.pres[j + 1])
							e.descricao = "Diminuindo Pressao do Separador";
						else
							e.descricao = "Aumentando Pressao do Separador";
						e.estIni = psep.pres[j];
						e.estFim = psep.pres[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}

		/*if (chokep.parserie > 1) {
			for (int j = 0; j < chokep.parserie; j++) {
				if (j < chokep.parserie - 1) {
					if (fabsl(chokep.abertura[j + 1] - chokep.abertura[j])
							> 1e-15) {
						e.instante = chokep.tempo[j];
						e.duracao = chokep.tempo[j + 1] - chokep.tempo[j];
						if (chokep.abertura[j] > chokep.abertura[j + 1])
							e.descricao = "Fechando Choke de Superficie";
						else
							e.descricao = "Abrindo Choke de Superficie";
						e.estIni = chokep.abertura[j];
						e.estFim = chokep.abertura[j + 1];
						logevento.push_back(e);
						logeventoEstat.push_back(e);
					}
				}
			}
		}*/
	}
	if (controDesc == 0 ) {  //alteracao5)
	  if (chokep.parserie > 1) {
		for (int j = 0; j < chokep.parserie; j++) {
			if (j < chokep.parserie - 1) {
				if (fabsl(chokep.abertura[j + 1] - chokep.abertura[j])
						> 1e-15) {
					e.instante = chokep.tempo[j];
					e.duracao = chokep.tempo[j + 1] - chokep.tempo[j];
					if (chokep.abertura[j] > chokep.abertura[j + 1])
						e.descricao = "Fechando Choke de Superficie";
					else
						e.descricao = "Abrindo Choke de Superficie";
					e.estIni = chokep.abertura[j];
					e.estFim = chokep.abertura[j + 1];
					logevento.push_back(e);
					logeventoEstat.push_back(e);
				}
			}
		}
	  }
	}

	if (controDesc == 0) {
	   if(lingas>0 && chokes.parserie>0){
		   if (chokes.parserie > 1) {
			   for (int j = 0; j < chokes.parserie; j++) {
				   if (j < chokes.parserie - 1) {
					   if (fabsl(chokes.abertura[j + 1] - chokes.abertura[j])
							> 1e-15) {
						   e.instante = chokes.tempo[j];
						   e.duracao = chokes.tempo[j + 1] - chokes.tempo[j];
						   if (chokes.abertura[j] > chokes.abertura[j + 1])
							e.descricao = "Fechando Choke de Injecao";
						   else
							e.descricao = "Abrindo Choke de Injecao";
						   e.estIni = chokes.abertura[j];
						   e.estFim = chokes.abertura[j + 1];
						   logevento.push_back(e);
						   logeventoEstat.push_back(e);
					   }
				   }
			   }
		   }
	   }
	}
	if (nbcs > 0) {
		for (int i = 0; i < nbcs; i++) {
			if (bcs[i].parserie > 1) {
				for (int j = 0; j < bcs[i].parserie; j++) {
					if (j < bcs[i].parserie - 1) {
						if (fabsl(bcs[i].freq[j + 1] - bcs[i].freq[j])
								> 1e-15) {
							e.instante = bcs[i].tempo[j];
							e.duracao = bcs[i].tempo[j + 1] - bcs[i].tempo[j];
							if (bcs[i].freq[j] > bcs[i].freq[j + 1])
								e.descricao = "Desacelerando BCS";
							else
								e.descricao = "Acelerando BCS";
							e.estIni = bcs[i].freq[j];
							e.estFim = bcs[i].freq[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
		}
	}
	if (nbvol > 0) {
		for (int i = 0; i < nbvol; i++) {
			if (bvol[i].parserie > 1) {
				for (int j = 0; j < bvol[i].parserie; j++) {
					if (j < bvol[i].parserie - 1) {
						if (fabsl(bvol[i].freq[j + 1] - bvol[i].freq[j])
								> 1e-15) {
							e.instante = bvol[i].tempo[j];
							e.duracao = bvol[i].tempo[j + 1] - bvol[i].tempo[j];
							if (bvol[i].freq[j] > bvol[i].freq[j + 1])
								e.descricao = "Desacelerando Bomba Volumetrica";
							else
								e.descricao = "Acelerando Bomba Volumetrica";
							e.estIni = bvol[i].freq[j];
							e.estFim = bvol[i].freq[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
		}
	}
	if (ndpreq > 0) {
		for (int i = 0; i < ndpreq; i++) {
			if (dpreq[i].parserie > 1) {
				for (int j = 0; j < dpreq[i].parserie; j++) {
					if (j < dpreq[i].parserie - 1) {
						if (fabsl(dpreq[i].dp[j] - dpreq[i].dp[j + 1])
								> 1e-15) {
							e.instante = dpreq[i].tempo[j];
							e.duracao = dpreq[i].tempo[j + 1]
									- dpreq[i].tempo[j];
							if (dpreq[i].dp[j] > dpreq[i].dp[j + 1])
								e.descricao =
										"Diminuindo Delta P (pressao requerida)";
							else
								e.descricao =
										"Aumentando Delta P (pressao requerida)";
							e.estIni = dpreq[i].dp[j];
							e.estFim = dpreq[i].dp[j + 1];
							logevento.push_back(e);
							logeventoEstat.push_back(e);
						}
					}
				}
			}
		}
	}
	if (npig > 0) {
		for (int i = 0; i < npig; i++) {
			e.instante = pig[i].tempo;
			e.duracao = 0.;
			e.descricao = "Lancando PIG";
			e.estIni = 0.;
			e.estFim = 0.;
			logevento.push_back(e);
			logeventoEstat.push_back(e);
		}
	}
	if (nperfisp > 0) {
		for (int i = 0; i < nperfisp; i++) {
			e.instante = profp.tempo[i];
			e.duracao = 0.;
			e.descricao = "Gravando Perfil Linha de Producao";
			e.estIni = 0.;
			e.estFim = 0.;
			logevento.push_back(e);
		}
	}
	if (nperfisg > 0 && lingas > 0) {
		for (int i = 0; i < nperfisg; i++) {
			e.instante = profg.tempo[i];
			e.duracao = 0.;
			e.descricao = "Gravando Perfil Linha de Gas";
			e.estIni = 0.;
			e.estFim = 0.;
			logevento.push_back(e);
		}
	}
	if (((*vg1dSP).chaverede == 0 || inic == 1) && ConContEntrada == 1) {  //alteracao9
		for (int i = 0; i < CCPres.parserie; i++) {
			if ((fabsl(CCPres.pres[i] - CCPres.pres[i + 1]) > 1e-15)
					|| (fabsl(CCPres.temperatura[i] - CCPres.temperatura[i + 1])
							> 1e-15)
					|| (fabsl(CCPres.tit[i] - CCPres.tit[i + 1]) > 1e-15)
					|| (fabsl(CCPres.bet[i] - CCPres.bet[i + 1]) > 1e-15)) {
				e.instante = CCPres.tempo[i];
				e.duracao = CCPres.tempo[i + 1] - CCPres.tempo[i];
				if (fabsl(CCPres.pres[i] - CCPres.pres[i + 1]) > (*vg1dSP).localtiny
						|| fabsl(
								CCPres.temperatura[i]
										- CCPres.temperatura[i + 1]) > (*vg1dSP).localtiny
						|| fabsl(CCPres.tit[i] - CCPres.tit[i + 1]) > (*vg1dSP).localtiny
						|| fabsl(CCPres.bet[i] - CCPres.bet[i + 1]) > (*vg1dSP).localtiny)
					e.descricao = "Alterando Coondicao de Entrada da Tubulacao";
				e.estIni = CCPres.tempo[i];
				e.estFim = CCPres.tempo[i + 1];
				logevento.push_back(e);
				logeventoEstat.push_back(e);
			}
		}
	}
	if (((*vg1dSP).chaverede == 0 || inic == 1) && ConContEntrada == 2) {  //alteracao9
		for (int i = 0; i < CCVPres.parserie; i++) {
			if ((fabsl(CCVPres.pres[i] - CCVPres.pres[i + 1]) > 1e-15)
					|| (fabsl(CCVPres.temperatura[i] - CCVPres.temperatura[i + 1])
							> 1e-15)
					|| (fabsl(CCVPres.mass[i] - CCVPres.mass[i + 1]) > 1e-15)
					|| (fabsl(CCVPres.bet[i] - CCVPres.bet[i + 1]) > 1e-15)) {
				e.instante = CCVPres.tempo[i];
				e.duracao = CCVPres.tempo[i + 1] - CCVPres.tempo[i];
				if (fabsl(CCVPres.pres[i] - CCVPres.pres[i + 1]) > (*vg1dSP).localtiny
						|| fabsl(
								CCVPres.temperatura[i]
										- CCVPres.temperatura[i + 1]) > (*vg1dSP).localtiny
						|| fabsl(CCVPres.mass[i] - CCVPres.mass[i + 1]) > (*vg1dSP).localtiny
						|| fabsl(CCVPres.bet[i] - CCVPres.bet[i + 1]) > (*vg1dSP).localtiny)
					e.descricao = "Alterando Coondicao de Entrada da Tubulacao";
				e.estIni = CCVPres.tempo[i];
				e.estFim = CCVPres.tempo[i + 1];
				logevento.push_back(e);
				logeventoEstat.push_back(e);
			}
		}
	}
	if (logevento.size() > 0)
		sort(logevento.begin(), logevento.end());
	if (logeventoEstat.size() > 0)
		sort(logeventoEstat.begin(), logeventoEstat.end());

	// caso nao seja simulacao POCO_INJETOR
	if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
		ostringstream saidaLog;
		saidaLog << "LogEvento" << ".dat";
		string tmp = saidaLog.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		/* escreveIni<<" Tempo ; ";
		 escreveIni<<" Duracao ; ";
		 escreveIni<<" Estado Inicial ; ";
		 escreveIni<<" Estado Final ; ";
		 escreveIni<<" Evento ; "<<"\n";*/
		escreveIni.close();
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();

		int neve = logeventoEstat.size();
		ostringstream saidaExt;
		saidaExt << "EventosExternos" << ".dat";
		tmp = saidaExt.str();
		ofstream escreveExt(tmp.c_str(), ios_base::out);
		escreveExt << " Tempo ; ";
		escreveExt << " Duracao ; ";
		escreveExt << " Estado Inicial ; ";
		escreveExt << " Estado Final ; ";
		escreveExt << " Evento ; " << "\n";
		if (neve > 0) {
			for (int i = 0; i < neve; i++) {
				escreveExt << logeventoEstat[i].instante << " ; ";
				escreveExt << logeventoEstat[i].duracao << " ; ";
				escreveExt << logeventoEstat[i].estIni << " ; ";
				escreveExt << logeventoEstat[i].estFim << " ; ";
				escreveExt << logeventoEstat[i].descricao << " ; " << "\n";
			}
		}
		escreveExt.close();
		arqRelatorioPerfis << tmp.c_str() << endl;
		arqRelatorioPerfis.flush();
	}
}

void Ler::geraduto() {
	dutosMRT = new DadosGeo [nduto];

	for (int i = 0; i < nduto; i++) {
		int cam = corte[duto[i].indcorte].ncam;
		int icor = duto[i].indcorte;


		double* vk;
		vk = new double[cam];
		for (int j = 0; j < cam; j++)
			vk[j] = mat[corte[icor].indmat[j]].cond;
		double* vcp;
		vcp = new double[cam];
		for (int j = 0; j < cam; j++)
			vcp[j] = mat[corte[icor].indmat[j]].cp;
		double* vrhoc;
		vrhoc = new double[cam];
		for (int j = 0; j < cam; j++)
			vrhoc[j] = mat[corte[icor].indmat[j]].rho;
		double* vvisc;
		vvisc = new double[cam];
		for (int j = 0; j < cam; j++)
			vvisc[j] = mat[corte[icor].indmat[j]].visc;
		double* vbeta;
		vbeta = new double[cam];
		for (int j = 0; j < cam; j++)
			vbeta[j] = mat[corte[icor].indmat[j]].beta;
		int* vtipomat;
		vtipomat = new int[cam];
		for (int j = 0; j < cam; j++)
			vtipomat[j] = mat[corte[icor].indmat[j]].tipo;
		int* vindmat;
		vindmat = new int[cam];
		for (int j = 0; j < cam; j++)
			vindmat[j] = corte[icor].indmat[j];

		dutosMRT[i] = DadosGeo(corte[icor].a, corte[icor].b,
				duto[i].ang, corte[icor].rug, corte[icor].anul,
				corte[icor].ncam, vk, corte[icor].diam, vcp, vrhoc, vvisc,
				vbeta,vtipomat,vindmat);
		delete[] vk;
		delete[] vcp;
		delete[] vrhoc;
		delete[] vvisc;
		delete[] vbeta;
		delete[] vtipomat;
		delete[] vindmat;
	}
}

/*void Ler::geraduto() {
	dutosMRT = new DadosGeo [nduto];

	for (int i = 0; i < nduto; i++) {
		int cam = corte[duto[i].indcorte].ncam;
		int icor = duto[i].indcorte;

		double* vk;
		vk = new double[cam];
		double* vcp;
		vcp = new double[cam];
		double* vrhoc;
		vrhoc = new double[cam];
		double* vvisc;
		vvisc = new double[cam];
		double* vbeta;
		vbeta = new double[cam];
		int* vtipomat;
		vtipomat = new int[cam];
		int* vindmat;
		vindmat = new int[cam];

		for (int j = 0; j < cam; j++){


			int indicador;
			for(indicador=0;indicador<nmaterial;indicador++){
				if(mat[indicador].id==corte[icor].indmat[j])break;
			}
			if(indicador==nmaterial){
				logger.log_write_logs_and_exit(LOGGER_FALHA,
				LOG_ERR_UNEXPECTED_EXCEPTION, "", "#secao transversal",
						"indice de material nao encontrado.");
			}

			vk[j] = mat[indicador].cond;

			vcp[j] = mat[indicador].cp;

			vrhoc[j] = mat[indicador].rho;

			vvisc[j] = mat[indicador].visc;

			vbeta[j] = mat[indicador].beta;

			vtipomat[j] = mat[indicador].tipo;

			vindmat[j] = indicador;
		}

		dutosMRT[i] = DadosGeo(corte[icor].a, corte[icor].b,
				duto[i].ang, corte[icor].rug, corte[icor].anul,
				corte[icor].ncam, vk, corte[icor].diam, vcp, vrhoc, vvisc,
				vbeta,vtipomat,vindmat);
		delete[] vk;
		delete[] vcp;
		delete[] vrhoc;
		delete[] vvisc;
		delete[] vbeta;
		delete[] vtipomat;
		delete[] vindmat;
	}
}*/

void Ler::novatrans(TransCal& transfer,
		DadosGeo& geom, int* discre, double tint,
		double text, double vext, double vint, long dircon,
		double dt, double ki, double cpi, double rhoi,
		double visci, double ke, double cpe, double rhoe,
		double visce, int vformacao, int litologia, int ambext,
		double pres, double dx) {  // alteracao0 // alteracao2

	int* vncamada;
	vncamada = new int[geom.ncamadas];
	for (int i = 0; i < geom.ncamadas; i++) {
		if (geom.tipomat[i] != 0)
			vncamada[i] = 1;
		else
			vncamada[i] = discre[i];
	}
	double* vdrcamada;
	vdrcamada = new double[geom.ncamadas];
	for (int i = 0; i < geom.ncamadas; i++) {
		if (i > 0)
			vdrcamada[i] = 0.5 * (geom.diamC[i] - geom.diamC[i - 1])
					/ vncamada[i];
		else
			vdrcamada[0] = 0.5 * (geom.diamC[0] - geom.a) / vncamada[0];
	}
	double** vTcamada;
	vTcamada = new double*[geom.ncamadas];
	for (int i = 0; i < geom.ncamadas; i++) {
		vTcamada[i] = new double[vncamada[i] + 1];
	}

	for (int i = 0; i < geom.ncamadas; i++) {
		for (int j = 0; j <= vncamada[i]; j++)
			//vTcamada[i][j] = tint;  // alteracao0
			vTcamada[i][j] = text;
	}
	int perm = -1;

	if (ambext == 1) {
		int para;
		para = 0;
	}

	int kL = litologia;  //alteracao2

	if (equilterm == 1 && fabsl(vint)<1e-15)vint=0.1;

	transfer = TransCal(vg1dSP,geom, perm, vncamada, vdrcamada, vTcamada,
			tint, text, 0., vint, vext, dircon, dt, ki, cpi, rhoi, visci, ke,
			cpe, rhoe, visce, vformacao, ambext, formacPoc[kL].tempo,
			formacPoc[kL].cond, formacPoc[kL].cp, formacPoc[kL].rho, pres); // alteracao2 // alteracao0
	if (equilterm == 1)
		transfer.transperm();
	delete[] vncamada;
	delete[] vdrcamada;
	for (int i = 0; i < geom.ncamadas; i++)
		delete[] vTcamada[i];
	delete[] vTcamada;

}

void Ler::geracelp(Cel* celula) {

	if(celp[0].pres>1000000){
		int para;
		para=0;
	}
	if(isnan(celp[0].temp)){
		int para;
		para=0;
	}

	//int ColunaAnulaIni=99;
	//int ColunaAnulaFim=9;
	if(flashCompleto==1){
		pmin=flash[0].rholF[1][0];
		tmin=flash[0].rholF[0][1];
		for (int i = 0; i < ncelp; i++){
			if(celp[i].pres<pmin)celp[i].pres=pmin;
			if(celp[i].temp<tmin)celp[i].temp=tmin;
		}
	}

	for (int i = 0; i < ncelp; i++) {

		//if(flashCompleto==1){
		//for(int j=0; j<nfluP; j++){
		//if(i>=flash[j].celini && i<flash[j].celfim) indfluPIni=j;
		//}
		//}
		if (flashCompleto == 1)
			indfluPIni = 0;

		int iduto = celp[i].duto;
		int idutoL;
		if (i > 0)
			idutoL = celp[i - 1].duto;
		else
			idutoL = celp[i].duto;
		int idutoR;
		if (i < ncelp - 1)
			idutoR = celp[i + 1].duto;
		else
			idutoR = celp[i].duto;
		int icorte = duto[iduto].indcorte;
		double vdx;
		double vdxL;
		double vdxR;
		double vazio;
		double vazioL;
		double vazioR;
		double bet;
		double betL;
		double betR;
		double bet0;
		double betN;
		double betP;
		if(flashCompleto==1){
			if(celp[i].pres<pmin)celp[i].pres=pmin;
			if(celp[i].temp<tmin)celp[i].temp=tmin;
		}
		double pmed = celp[i].pres;
		double tmed = celp[i].temp;
		double pmedL;
		double tmedL;
		double pmedR;
		double tmedR;
		double presaux;
		double presauxL;
		double presauxR;
		double taux;
		double tauxL;
		double tauxR;
		double razdx;
		double razdxL;
		double razdxR;
		double vQL;
		double vQG;
		double vMC;
		double vMliq;
		double vML;
		double vMliqL;
		double vMR;
		double vMliqR;
		double vrhol;
		double vrhog;
		if(flashCompleto==2){
			flup[indfluPIni].atualizaPropComp(pmed,tmed,-1,NULL,NULL,pocinjec);
		}
		if (i > 1 && i < ncelp - 1) {
			vdx = celp[i].dx;
			vdxL = celp[i - 1].dx;
			vdxR = celp[i + 1].dx;
			razdx = celp[i].dx / (celp[i].dx + celp[i - 1].dx);
			razdxL = celp[i - 1].dx / (celp[i - 1].dx + celp[i - 2].dx);
			razdxR = celp[i + 1].dx / (celp[i + 1].dx + celp[i].dx);
			pmedL = celp[i - 1].pres;
			tmedL = celp[i - 1].temp;
			pmedR = celp[i + 1].pres;
			tmedR = celp[i + 1].temp;
			presaux = razdx * celp[i].pres + (1. - razdx) * celp[i - 1].pres;
			taux = razdx * celp[i].temp + (1. - razdx) * celp[i - 1].temp;
			presauxL = razdxL * celp[i - 1].pres
					+ (1. - razdxL) * celp[i - 2].pres;
			tauxL = razdxL * celp[i - 1].temp
					+ (1. - razdxL) * celp[i - 2].temp;
			presauxR = razdxR * celp[i + 1].pres + (1. - razdxR) * celp[i].pres;
			tauxR = razdxR * celp[i + 1].temp + (1. - razdxR) * celp[i].temp;
			vQL = celp[i].uls * dutosMRT[iduto].area;
			vQG = celp[i].ugs * dutosMRT[iduto].area;
			vazio = 1 - celp[i].hol;
			vazioL = 1 - celp[i - 1].hol;
			vazioR = 1 - celp[i + 1].hol;
			bet = celp[i].bet;
			betL = celp[i - 1].bet;
			betR = celp[i + 1].bet;
			bet0 = betL;
			if (vQL <= 0)
				bet0 = bet;
			betN = celp[i - 2].bet;
			if (celp[i - 1].uls <= 0)
				betN = betL;
			betP = bet;
			if (celp[i + 1].uls <= 0)
				betP = betR;
			vrhol = (1 - bet0) * flup[indfluPIni].MasEspLiq(presaux, taux)
					+ bet0 * fluc.MasEspFlu(presaux, taux);
			vrhog = flup[indfluPIni].MasEspGas(presaux, taux);
			vMC = vrhol * vQL + vrhog * vQG;
			vMliq = vrhol * vQL;
			vML = (((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * celp[i - 1].uls
					+ flup[indfluPIni].MasEspGas(presauxL, tauxL)
							* celp[i - 1].ugs) * dutosMRT[idutoL].area;
			vMliqL = ((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * celp[i - 1].uls
					* dutosMRT[idutoL].area;
			vMR = (((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * celp[i + 1].uls
					+ flup[indfluPIni].MasEspGas(presauxR, tauxR)
							* celp[i + 1].ugs) * dutosMRT[idutoR].area;
			vMliqR = ((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * celp[i + 1].uls
					* dutosMRT[idutoR].area;
		} else if (i == 1) {
			vdx = celp[i].dx;
			vdxL = celp[i - 1].dx;
			vdxR = celp[i + 1].dx;
			razdx = celp[i].dx / (celp[i].dx + celp[i - 1].dx);
			razdxL = 1.;
			razdxR = celp[i + 1].dx / (celp[i + 1].dx + celp[i].dx);
			pmedL = celp[i - 1].pres;
			tmedL = celp[i - 1].temp;
			pmedR = celp[i + 1].pres;
			tmedR = celp[i + 1].temp;
			presaux = razdx * celp[i].pres + (1. - razdx) * celp[i - 1].pres;
			taux = razdx * celp[i].temp + (1. - razdx) * celp[i - 1].temp;
			presauxL = razdxL * celp[i - 1].pres;
			tauxL = razdxL * celp[i - 1].temp;
			presauxR = razdxR * celp[i + 1].pres + (1. - razdxR) * celp[i].pres;
			tauxR = razdxR * celp[i + 1].temp + (1. - razdxR) * celp[i].temp;
			vQL = celp[i].uls * dutosMRT[iduto].area;
			vQG = celp[i].ugs * dutosMRT[iduto].area;
			vazio = 1 - celp[i].hol;
			vazioL = 1 - celp[i - 1].hol;
			vazioR = 1 - celp[i + 1].hol;
			bet = celp[i].bet;
			betL = celp[i - 1].bet;
			betR = celp[i + 1].bet;
			bet0 = betL;
			if (vQL <= 0)
				bet0 = bet;
			betN = betL;
			betP = bet;
			if (celp[i + 1].uls <= 0)
				betP = betR;
			vrhol = (1 - bet0) * flup[indfluPIni].MasEspLiq(presaux, taux)
					+ bet0 * fluc.MasEspFlu(presaux, taux);
			vrhog = flup[indfluPIni].MasEspGas(presaux, taux);
			vMC = vrhol * vQL + vrhog * vQG;
			vMliq = vrhol * vQL;
			vML = (((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * celp[i - 1].uls
					+ flup[indfluPIni].MasEspGas(presauxL, tauxL)
							* celp[i - 1].ugs) * dutosMRT[idutoL].area;
			vMliqL = ((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * celp[i - 1].uls
					* dutosMRT[idutoL].area;
			vMR = (((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * celp[i + 1].uls
					+ flup[indfluPIni].MasEspGas(presauxR, tauxR)
							* celp[i + 1].ugs) * dutosMRT[idutoR].area;
			vMliqR = ((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * celp[i + 1].uls
					* dutosMRT[idutoR].area;
		} else if (i == 0) {
			vdx = celp[i].dx;
			vdxL = celp[i].dx;
			vdxR = celp[i + 1].dx;
			razdx = 1.;
			razdxL = 1.;
			razdxR = celp[i + 1].dx / (celp[i + 1].dx + celp[i].dx);
			pmedL = celp[i].pres;
			tmedL = celp[i].temp;
			pmedR = celp[i + 1].pres;
			tmedR = celp[i + 1].temp;
			presaux = razdx * celp[i].pres;
			taux = razdx * celp[i].temp;
			presauxL = razdxL * celp[i].pres;
			tauxL = razdxL * celp[i].temp;
			presauxR = razdxR * celp[i + 1].pres + (1. - razdxR) * celp[i].pres;
			tauxR = razdxR * celp[i + 1].temp + (1. - razdxR) * celp[i].temp;
			vQL = celp[i].uls * dutosMRT[iduto].area;
			vQG = celp[i].ugs * dutosMRT[iduto].area;
			vazio = 1 - celp[i].hol;
			vazioL = 1 - celp[i].hol;
			vazioR = 1 - celp[i + 1].hol;
			bet = celp[i].bet;
			betL = celp[i].bet;
			betR = celp[i + 1].bet;
			bet0 = betL;
			if (vQL <= 0)
				bet0 = bet;
			betN = betL;
			betP = bet;
			if (celp[i + 1].uls <= 0)
				betP = betR;
			vrhol = (1 - bet0) * flup[indfluPIni].MasEspLiq(presaux, taux)
					+ bet0 * fluc.MasEspFlu(presaux, taux);
			vrhog = flup[indfluPIni].MasEspGas(presaux, taux);
			vMC = vrhol * vQL + vrhog * vQG;
			vMliq = vrhol * vQL;
			vML = (((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * 0.
					+ flup[indfluPIni].MasEspGas(presauxL, tauxL) * 0.)
					* dutosMRT[idutoL].area;
			vMliqL = ((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * 0
					* dutosMRT[idutoL].area;
			vMR = (((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * celp[i + 1].uls
					+ flup[indfluPIni].MasEspGas(presauxR, tauxR)
							* celp[i + 1].ugs) * dutosMRT[idutoR].area;
			vMliqR = ((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * celp[i + 1].uls
					* dutosMRT[idutoR].area;
		} else {
			vdx = celp[i].dx;
			vdxL = celp[i - 1].dx;
			vdxR = celp[i].dx;
			razdx = celp[i].dx / (celp[i].dx + celp[i - 1].dx);
			razdxL = celp[i - 1].dx / (celp[i - 1].dx + celp[i - 2].dx);
			razdxR = 0.;
			pmedL = celp[i - 1].pres;
			tmedL = celp[i - 1].temp;
			pmedR = celp[i].pres;
			tmedR = celp[i].temp;
			presaux = razdx * celp[i].pres + (1. - razdx) * celp[i - 1].pres;
			taux = razdx * celp[i].temp + (1. - razdx) * celp[i - 1].temp;
			presauxL = razdxL * celp[i - 1].pres
					+ (1. - razdxL) * celp[i - 2].pres;
			tauxL = razdxL * celp[i - 1].temp
					+ (1. - razdxL) * celp[i - 2].temp;
			presauxR = (1. - razdxR) * celp[i].pres;
			tauxR = (1. - razdxR) * celp[i].temp;
			vQL = celp[i].uls * dutosMRT[iduto].area;
			vQG = celp[i].ugs * dutosMRT[iduto].area;
			vazio = 1 - celp[i].hol;
			vazioL = 1 - celp[i - 1].hol;
			vazioR = 1 - celp[i].hol;
			bet = celp[i].bet;
			betL = celp[i - 1].bet;
			betR = celp[i].bet;
			bet0 = betL;
			if (vQL <= 0)
				bet0 = bet;
			betN = betL;
			betP = bet;
			if (celp[i + 1].uls <= 0)
				betP = betR;
			vrhol = (1 - bet0) * flup[indfluPIni].MasEspLiq(presaux, taux)
					+ bet0 * fluc.MasEspFlu(presaux, taux);
			vrhog = flup[indfluPIni].MasEspGas(presaux, taux);
			vMC = vrhol * vQL + vrhog * vQG;
			vMliq = vrhol * vQL;
			vML = (((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * celp[i - 1].uls
					+ flup[indfluPIni].MasEspGas(presauxL, tauxL)
							* celp[i - 1].ugs) * dutosMRT[idutoL].area;
			vMliqL = ((1 - betN) * flup[indfluPIni].MasEspLiq(presauxL, tauxL)
					+ betN * fluc.MasEspFlu(presauxL, tauxL)) * celp[i - 1].uls
					* dutosMRT[idutoL].area;
			vMR = (((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * 0
					+ flup[indfluPIni].MasEspGas(presauxR, tauxR) * 0)
					* dutosMRT[idutoR].area;
			vMliqR = ((1 - betP) * flup[indfluPIni].MasEspLiq(presauxR, tauxR)
					+ betP * fluc.MasEspFlu(presauxR, tauxR)) * 0
					* dutosMRT[idutoR].area;
		}
		double jmix;
		jmix = celp[i].ugs + celp[i].uls;
		TransCal entrada;

		double vhol = celp[i].hol;
		vrhol = (1 - bet)
				* flup[indfluPIni].MasEspLiq(celp[i].pres, celp[i].temp)
				+ bet * fluc.MasEspFlu(celp[i].pres, celp[i].temp);
		double vrhomix = celp[i].hol * vrhol + (1 - celp[i].hol) * vrhog;
		double vkint = ((1 - bet)
				* flup[indfluPIni].CondLiq(celp[i].pres, celp[i].temp)
				+ bet * fluc.CondLiq(celp[i].pres, celp[i].temp)) * vhol
				+ flup[indfluPIni].CondGas(celp[i].pres, celp[i].temp)
						* (1 - vhol);
		double vcpint;
		if (fabsl(jmix) > 1e-10)
			vcpint = (((1 - bet)
					* flup[indfluPIni].CalorLiq(celp[i].pres, celp[i].temp)
					+ bet * fluc.CalorLiq(celp[i].pres, celp[i].temp))
					* fabsl(celp[i].uls) * vrhol
					+ flup[indfluPIni].CalorGas(celp[i].pres, celp[i].temp)
							* fabsl(celp[i].ugs) * vrhog)
					/ (fabsl(celp[i].uls) * vrhol + fabsl(celp[i].ugs) * vrhog);
		else
			vcpint = ((1 - bet)
					* flup[indfluPIni].CalorLiq(celp[i].pres, celp[i].temp)
					+ bet * fluc.CalorLiq(celp[i].pres, celp[i].temp)) * vhol
					+ flup[indfluPIni].CalorGas(celp[i].pres, celp[i].temp)
							* (1. - vhol);
		double vviscint = ((1 - bet)
				* flup[indfluPIni].ViscOleo(celp[i].pres, celp[i].temp)
				+ bet * fluc.VisFlu(celp[i].pres, celp[i].temp)) * vhol
				+ flup[indfluPIni].ViscGas(celp[i].pres, celp[i].temp)
						* (1. - vhol);

		if (i == 26) {
			int para;
			para = 0;
		}
        double dxC;
        if(i==0)dxC=celp[i].dx;
        else dxC=0.5*(celp[i].dx+celp[i-1].dx);
		novatrans(entrada, dutosMRT[iduto], corte[icorte].discre, celp[i].temp,
				celp[i].textern, celp[i].vextern, jmix, celp[i].dirconv, dtmax,
				vkint, vcpint, vrhomix, vviscint, celp[i].kextern,
				celp[i].cpextern, celp[i].rhoextern, celp[i].viscextern,
				celp[i].formacCel, celp[i].lito, celp[i].ambext,
				1.0197 + celp[i].profundiM * 1000 * 9.81 / 98066.52, dxC);
		celula[i] = Cel(vg1dSP,dutosMRT[idutoL], dutosMRT[iduto],
				dutosMRT[idutoR], flup[indfluPIni], fluc, tmedL, tmed, tmedR,
				tmed, pmedL, pmed, pmedR, vML, vMC, vMR, vMliqL, vMliq, vMliqR,
				vazioL, vazio, vazioR, betL, bet, betR, vdxL, vdx, vdxR, dtmax,
				i, entrada);
		celula[i].correlacaoMR2=celp[i].correlacaoMR2;
		//enterramento
		if(celp[i].difusTerm2D==1){
			celula[i].calor.difus2D=1;
			double vCondGlob=celula[i].calor.condParede();
			double vCondLoc=celula[i].calor.condParedeLocal();
			double vhE=celula[i].calor.hExt();
			double vhInt=celula[i].calor.hInt();
			double vdiamI=celula[i].calor.geom.a;
			int camadaExt=celula[i].calor.geom.ncamadas-1;
			double vdiamE=celula[i].calor.geom.diamC[camadaExt];
			celula[i].calor.poisson2D=solverP(vg1dSP,celp[i].difusTerm2DJSON,vCondGlob,vCondLoc,vhE,vhInt,celula[i].calor.Tint,
					celula[i].calor.Textern1,vdiamI,vdiamE,i);
		}
		if(modoDifus3D==1){
			for(int iacop=0; iacop<nacop;iacop++){
				if(celAcop[iacop].indCel==i){
					geoAcop.push_back(celula[i].duto);
				}
			}
		}
		celula[i].mudaArea=mudaArea;
		celula[i].indGeom=icorte;
		celula[i].dPdLHidro=celp[i].dPdLHidro;
		celula[i].dPdLFric=celp[i].dPdLFric;
		celula[i].dTdLCor=celp[i].dTdL;
		celula[i].inddPdLHidro=celp[i].inddPdLHidro;
		celula[i].inddPdLFric=celp[i].inddPdLFric;
		celula[i].inddTdLCor=celp[i].inddTdL;
		celula[i].miniTabAtraso=miniTabAtraso;
		if (i > 0){
			celula[i - 1].fluiR = &celula[i].flui;
			celula[i].fluiL = &celula[i - 1].flui;
		}
		else{
			celula[i].fluiL = &celula[i].flui;
		}

	}

	//for(int i=ncelp-2; i<ncelp;i++) celula[i].flui.tab=0;

}

void Ler::geracelg(CelG* celula) {

	double ul;
	double u=0.;
	double ur=0.;
	double dxl;
	double dx=0.;
	double dxr=0.;
	for (int i = 0; i < ncelg; i++) {
		int iduto = celg[i].duto;
		int idutol;
		if (i == 0)
			idutol = iduto;
		else
			idutol = celg[i - 1].duto;
		int idutor;
		if (i == ncelg - 1)
			idutor = iduto;
		else
			idutor = celg[i + 1].duto;

		int icorte = duto[iduto].indcorte;
		double area;
		area = dutosMRT[iduto].area;

		TransCal entrada;

		double vrhog = flug.MasEspGas(celg[i].pres, celg[i].temp);
		double vkint = flug.CondGas(celg[i].pres, celg[i].temp);
		double vcpint = flug.ViscGas(celg[i].pres, celg[i].temp);
		double vviscint = flug.ViscGas(celg[i].pres, celg[i].temp);
		double jmix;
		jmix = celg[i].mg / (vrhog * area);

		if (i == 0) {
			u = vrhog * area;
			ul = u;
			ur = flug.MasEspGas(celg[i + 1].pres, celg[i + 1].temp)
					* dutosMRT[idutor].area;
			dx = celg[i].dx;
			dxl = dx;
			dxr = celg[i + 1].dx;
		} else if (i == ncelg - 1) {
			ul = u;
			u = ur;
			dxl = dx;
			dx = dxr;
		} else {
			ul = u;
			u = ur;
			ur = flug.MasEspGas(celg[i + 1].pres, celg[i + 1].temp)
					* dutosMRT[idutor].area;
			dxl = dx;
			dx = dxr;
			dxr = celg[i + 1].dx;
		}

        double dxC;
        if(i==0)dxC=celg[i].dx;
        else dxC=0.5*(celg[i].dx+celg[i-1].dx);

		novatrans(entrada, dutosMRT[iduto], corte[icorte].discre, celg[i].temp,
				celg[i].textern, celg[i].vextern, jmix, 0, dtmax, vkint, vcpint,
				vrhog, vviscint, celg[i].kextern, celg[i].cpextern,
				celg[i].rhoextern, celg[i].viscextern, celg[i].formacCel,
				celg[i].lito, celg[i].ambext,
				1.0197 + celg[i].profundiM * 1000 * 9.81 / 98066.52,dxC); // alteracao0 // alteracao2
		entrada.coluna = celg[i].acopcol;

		if (i > 0 && i < ncelg - 1)
			celula[i] = CelG(dutosMRT[idutol], dutosMRT[iduto],
					dutosMRT[idutor], flug, ChokeGas(),
					celg[i - 1].temp, celg[i].temp, celg[i + 1].temp,
					celg[i - 1].pres, celg[i].pres, celg[i + 1].pres,
					celg[i - 1].mg, celg[i].mg, celg[i + 1].mg, u, ur, ul, dx,
					dxr, dxl, dtmax, i, 0, gasinj.tipoCC, entrada);
		if (i == 0)
			celula[i] = CelG(dutosMRT[idutol], dutosMRT[iduto],
					dutosMRT[idutor], flug, ChokeGas(),
					celg[i].temp, celg[i].temp, celg[i + 1].temp, celg[i].pres,
					celg[i].pres, celg[i + 1].pres, celg[i].mg, celg[i].mg,
					celg[i + 1].mg, u, ur, ul, dx, dxr, dxl, dtmax, i, 0,
					gasinj.tipoCC, entrada);
		if (i == ncelg - 1)
			celula[i] = CelG(dutosMRT[idutol], dutosMRT[iduto],
					dutosMRT[idutor], flug, ChokeGas(),
					celg[i - 1].temp, celg[i].temp, celg[i].temp,
					celg[i - 1].pres, celg[i].pres, celg[i].pres,
					celg[i - 1].mg, celg[i].mg, celg[i].mg, u, ur, ul, dx, dxr,
					dxl, dtmax, i, 0, gasinj.tipoCC, entrada);
		if (descarga == 1)
			celula[i].salinidade = salinDescarga;  //15-06-2018

		celula[i].indGeom=icorte;

		celula[i].dPdLHidro=celg[i].dPdLHidro;
		celula[i].dPdLFric=celg[i].dPdLFric;
		celula[i].dTdLCor=celg[i].dTdL;
		celula[i].inddPdLHidro=celg[i].inddPdLHidro;
		celula[i].inddPdLFric=celg[i].inddPdLFric;
		celula[i].inddTdLCor=celg[i].inddTdL;

	}
}

int Ler::coluanulini() {
	int acopla = celp[0].acopcol;
	for (int i = 1; i < ncelp; i++) {
		if (acopla == 1 && celp[i].acopcol == 0) {
			return i - 1;
		}
		if (acopla == 1 && celp[i].acopcol == 1 && i == ncelp - 1) {
			return i;
		}
		acopla = celp[i].acopcol;
	}
	return -1; // TODO: tratar erro
}
int Ler::coluanulfim() {
	int acopla = celp[0].acopcol;
	for (int i = 1; i < ncelp; i++) {
		if (acopla == 0 && celp[i].acopcol == 1) {
			return i;
		}
		acopla = celp[i].acopcol;
	}
	return -1; // TODO: tratar erro
}
int Ler::anulcoluini() {
	int acopla = celg[0].acopcol;
	if (acopla)
		return 0; // casos de linha de servico sem linha maritima
	for (int i = 1; i < ncelg; i++) {
		if (acopla == 0 && celg[i].acopcol == 1) {
			return i;
		}
		acopla = celg[i].acopcol;
	}
	return -1; // TODO: tratar erro
}
int Ler::anulcolufim() {
	int acopla = celg[0].acopcol;
	for (int i = 1; i < ncelg; i++) {
		if (acopla == 1 && celg[i].acopcol == 0) {
			return i - 1;
		} else if (acopla == 1 && celg[i].acopcol == 1 && i == ncelg - 1) {
			return i;
		}
		acopla = celg[i].acopcol;
	}
	return -1;
}

int Ler::acopPriRedeParalelaini() {
	int acopla = celp[0].acopcolRede;
	for (int i = 1; i < ncelp; i++) {
		if (acopla == 1 && celp[i].acopcolRede == 0) {
			return i - 1;
		}
		if (acopla == 1 && celp[i].acopcolRede == 1 && i == ncelp - 1) {
			return i;
		}
		acopla = celp[i].acopcolRede;
	}
	return -1; // TODO: tratar erro
}
int Ler::acopPriRedeParalelafim() {
	int acopla = celp[0].acopcolRede;
	for (int i = 1; i < ncelp; i++) {
		if (acopla == 0 && celp[i].acopcolRede == 1) {
			return i;
		}
		acopla = celp[i].acopcolRede;
	}
	return -1; // TODO: tratar erro
}
int Ler::acopSecRedeParalelaini() {
	int acopla = celp[0].acopcolRede;
	if (acopla)
		return 0; // casos de linha de servico sem linha maritima
	for (int i = 1; i < ncelp; i++) {
		if (acopla == 0 && celp[i].acopcolRede == 1) {
			return i;
		}
		acopla = celp[i].acopcolRede;
	}
	return -1; // TODO: tratar erro
}
int Ler::acopSecRedeParalelafim() {
	int acopla = celp[0].acopcolRede;
	for (int i = 1; i < ncelp; i++) {
		if (acopla == 1 && celp[i].acopcolRede == 0) {
			return i - 1;
		} else if (acopla == 1 && celp[i].acopcolRede == 1 && i == ncelp - 1) {
			return i;
		}
		acopla = celp[i].acopcolRede;
	}
	return -1;
}

void Ler::geraipr(Cel* celula) {
	//IPR iprMRT();

	for (int i = 0; i < nipr; i++) {

		//if(flashCompleto==1){
		//for(int j=0; j<nfluP; j++){
		//if(IPRS[i].indcel>=flash[j].celini && IPRS[i].indcel<flash[j].celfim) IPRS[i].indfluP=j;
		//}
		//}
		//if (flashCompleto == 1)
			//IPRS[i].indfluP = 0;
		double ip=0.;
		double qm=0.;
		if(IPRS[i].tipoIPR==0 || IPRS[i].tipoIPR==1) ip=IPRS[i].ip[0];
		else if(IPRS[i].tipoIPR==2)qm=IPRS[i].qMax[0];

		if(flashCompleto==2){
			flup[IPRS[i].indfluP].atualizaPropComp(celula[IPRS[i].indcel].pres,celula[IPRS[i].indcel].temp,-1,NULL,NULL,pocinjec);
		}
		IPR iprMRT(IPRS[i].pres[0], IPRS[i].temp[0], ip,
				IPRS[i].jp[0],qm, flup[IPRS[i].indfluP],IPRS[i].tipoIPR);
		celula[IPRS[i].indcel].acsr.tipo = 3;
		celula[IPRS[i].indcel].acsr.ipr = iprMRT;
		if (IPRS[i].indcel < (ncelp - 1))
			celula[IPRS[i].indcel + 1].acsrL = &celula[IPRS[i].indcel].acsr;
	}

}

void Ler::gerafgasVGL(Cel* celula) {
	//InjGas injgasMRT();

	for (int i = 0; i < nvalvgas; i++) {
		int iposp = valvgl[i].posicP;
		int iposg = valvgl[i].posicG;
		if(flashCompleto==2){
			flug.atualizaPropComp(celula[iposp].pres,celula[iposp].temp,-1,NULL,NULL,1);
		}
		InjGas injgasMRT(0., celg[iposg].temp, flug);
		injgasMRT.seco=1;
		celula[iposp].acsr.tipo = 1;
		celula[iposp].acsr.injg = injgasMRT;
	}

}

void Ler::gerafgasFonte(Cel* celula) {
	//InjGas injgasMRT();

	for (int i = 0; i < ninjgas; i++) {
			int iposp = fonteg[i].posicP;
			ProFlu flutemp;
			if(pocinjec==0){
				if(fonteg[i].seco==1) flutemp=flug;
				else flutemp= flup[fonteg[i].indfluP];
			}
			else flutemp= flup[0];
			if(flashCompleto==2){
				flutemp.atualizaPropComp(celula[iposp].pres,celula[iposp].temp,-1,NULL,NULL,fonteg[i].seco);
			}
			if(fonteg[i].seco==1){
				InjGas injgasMRT(fonteg[i].vazgas[0], fonteg[i].temp[0],flutemp);
				injgasMRT.seco=fonteg[i].seco;
				celula[iposp].acsr.tipo = 1;
				celula[iposp].acsr.injg = injgasMRT;
			}
			else{
				InjGas injgasMRT(fonteg[i].vazgas[0], fonteg[i].temp[0],flutemp,fluc,fonteg[i].vazcomp[0]);
				injgasMRT.seco=fonteg[i].seco;
				celula[iposp].acsr.tipo = 1;
				celula[iposp].acsr.injg = injgasMRT;
			}
	}

}

void Ler::gerafliqFonte(Cel* celula) {
	//InjGas injgasMRT();

	for (int i = 0; i < ninjliq; i++) {

		//if(flashCompleto==1){
		//for(int j=0; j<nfluP; j++){
		//if(fontel[i].posicP>=flash[j].celini && fontel[i].posicP<flash[j].celfim) fontel[i].indfluP=j;
		//}
		//}
		//if (flashCompleto == 1)
			//fontel[i].indfluP = 0;

		int iposp = fontel[i].posicP;
		if(flashCompleto==2){
			flup[fontel[i].indfluP].atualizaPropComp(celula[iposp].pres,celula[iposp].temp,-1,NULL,NULL,pocinjec);
		}
		InjLiq injliqMRT(fontel[i].vazliq[0], fontel[i].temp[0],
				fontel[i].bet[0], flup[fontel[i].indfluP], fluc);
		celula[iposp].acsr.tipo = 2;
		celula[iposp].acsr.injl = injliqMRT;
	}


}

void Ler::gerafmassFonte(Cel* celula) {
	//InjGas injgasMRT();

	for (int i = 0; i < ninjmass; i++) {

		//if(flashCompleto==1){
		//for(int j=0; j<nfluP; j++){
		//if(fontel[i].posicP>=flash[j].celini && fontel[i].posicP<flash[j].celfim) fontel[i].indfluP=j;
		//}
		//}
		//if (flashCompleto == 1)
			//fontel[i].indfluP = 0;

		int iposp = fontem[i].posicP;
		double vmp;
		double vmc;
		double vmg;
		if(fontem[i].condiTermo==1){
			vmc=fontem[i].vazMasC[0];
			vmg=fontem[i].vazMasG[0];
			vmp=fontem[i].vazMasT[0]-vmc-vmg;
		}
		else{
			double tit;
			tit=flup[fontem[i].indfluP].FracMassHidra(celula[iposp].pres, celula[iposp].temp);
			vmc=fontem[i].vazMasC[0];
			vmg=(fontem[i].vazMasT[0]-vmc)*tit;
			vmp=fontem[i].vazMasT[0]-vmc-vmg;
		}
		if(flashCompleto==2){
			flup[fontem[i].indfluP].atualizaPropComp(celula[iposp].pres,celula[iposp].temp,-1,NULL,NULL,pocinjec);
		}
		InjMult injmultMRT(vmp, vmc, vmg,fontem[i].temp[0], flup[fontem[i].indfluP], fluc);
		injmultMRT.condTermo=fontem[i].condiTermo;
		celula[iposp].acsr.tipo = 10;
		celula[iposp].acsr.injm = injmultMRT;
	}

}

void Ler::gerafPoroRadFonte(Cel* celula) {

	for (int i = 0; i < nPoroRad; i++) {

		int iposp = porosoRad[i].posicP;
		PorosRadSimp poroRadMR3(porosoRad[i].nomeArquivoEntrada,vg1dSP);
		celula[iposp].acsr.tipo = 15;
		celula[iposp].acsr.radialPoro = poroRadMR3;
		celula[iposp].acsr.radialPoro.posicMarlim=porosoRad[i].comp;
	}

}

void Ler::gerafPoro2DFonte(Cel* celula) {

	for (int i = 0; i < nPoro2D; i++) {

		int iposp = poroso2D[i].posicP;
		solverPoro poro2DMR3(vg1dSP,poroso2D[i].nomeArquivoEntrada);
		celula[iposp].acsr.tipo = 16;
		celula[iposp].acsr.poroso2D = poro2DMR3;
		celula[iposp].acsr.poroso2D.posicMarlim=poroso2D[i].comp;
	}

}


void Ler::geraFuro(Cel* celula) {

	for (int i = 0; i < nfuro; i++) {

		int iposp = furo[i].posicP;

		if(furo[i].TipoAbertura==1){
			for(int j=0; j<furo[i].parserie;j++)
				furo[i].abertura[j]=0.25*M_PI*furo[i].abertura[j]*furo[i].abertura[j]/celula[iposp].duto.area;
		}

		if(flashCompleto==2){
			flup[furo[i].indFlu].atualizaPropComp(celula[iposp].pres,celula[iposp].temp,-1,NULL,NULL,pocinjec);
		}
		ProFlu fluAmb;
		fluAmb=flup[furo[i].indFlu];
		fontemaschk furoMRT(0.,0.,0.,0.,furo[i].titAmb,furo[i].cd, furo[i].abertura[0],
				            celula[iposp].duto.area, 0.,0.,furo[i].beta, celula[iposp].temp, celula[iposp].pres,
				            furo[i].temp, furo[i].pres,flup[0],fluAmb, fluc, furo[i].check[0],furo[i].ambGas);
		celula[iposp].acsr.tipo = 9;
		celula[iposp].acsr.fontechk = furoMRT;
	}

}

void Ler::gerafBCS(Cel* celula) {
	//InjGas injgasMRT();

	for (int i = 0; i < nbcs; i++) {
		int iposp = bcs[i].posicP;
		BomCentSub bcsMRT(bcs[i].ncurva, bcs[i].vaz, bcs[i].head,
				bcs[i].power, bcs[i].efic, bcs[i].freqref, bcs[i].nestag,
				bcs[i].eficM, bcs[i].freqMinima);
		celula[iposp].acsr.tipo = 4;
		celula[iposp].acsr.bcs = bcsMRT;
	}

}

void Ler::gerafBVOL(Cel* celula) {
	//InjGas injgasMRT();

	for (int i = 0; i < nbvol; i++) {
		int iposp = bvol[i].posicP;
		BomVol bvolMRT(bvol[i].capacidade, bvol[i].freq[0],
				bvol[i].npoli);
		celula[iposp].acsr.tipo = 8;
		celula[iposp].acsr.bvol = bvolMRT;
		celula[iposp + 1].acsrL = &celula[iposp].acsr;
	}

}

void Ler::funcRazCV(double abertura,detCV* cvCurv,int ncurvaCV,double cdchk,
		            double AreaTub, double& razarea){
	double cv;

	int ini;
	for(int i=0;i<ncurvaCV;i++){
		ini=-1;
		if(abertura>=cvCurv[i].x1 && abertura<cvCurv[i].x2){
			ini=i;
			break;
		}
	}
	if(ini==-1){
		abertura=1;
		ini=ncurvaCV-1;
		double ang=(cvCurv[ini].cv2-cvCurv[ini].cv1)/(cvCurv[ini].x2-cvCurv[ini].x1);
		cv=cvCurv[ini].cv1+ang*(abertura-cvCurv[ini].x1);
	}
	else{
		double ang=(cvCurv[ini].cv2-cvCurv[ini].cv1)/(cvCurv[ini].x2-cvCurv[ini].x1);
		cv=cvCurv[ini].cv1+ang*(abertura-cvCurv[ini].x1);
	}
	double term=cv/(58860.75*cdchk*AreaTub);
	razarea= sqrtl(term*term/(1.+term*term));
}


void Ler::geraDPReq(Cel* celula) {
	//InjGas injgasMRT();

	for (int i = 0; i < ndpreq; i++) {
		int iposp = dpreq[i].posicP;
		celula[iposp].acsr.tipo = 7;
		celula[iposp].acsr.delp = dpreq[i].dp[0];
		celula[iposp].acsr.tipoCompGas = dpreq[i].tipoCompGas;
		celula[iposp].acsr.fatPoli = dpreq[i].fatPoli;
		celula[iposp].acsr.eficLiq = dpreq[i].eficLiq;
		celula[iposp].acsr.eficGas = dpreq[i].eficGas;
	}

}

void Ler::geraValv(Cel* celula) {
	for (int i = 0; i < nvalv; i++) {
		int posicM = valv[i].posicP;
		double area = celula[posicM].duto.area;
		double razarea=1.;
		if(valv[i].curvaCV==1){
			funcRazCV(1.,valv[i].cvCurv,valv[i].ncv,valv[i].cd,area, razarea);
		}
		choke valvula(area, razarea * area);
		celula[posicM].acsr.tipo = 5;
		celula[posicM].acsr.chk = valvula;
		if (posicM < ncelp - 1)
			celula[posicM + 1].acsrL = &celula[posicM].acsr;
		double abre = valv[i].abertura[0];
		for (int j = 1; j < valv[i].parserie; j++) {
			if (abre == 0 && valv[i].abertura[j] == 1) {
				Tevento[eventoabre] = valv[i].tempo[j - 1];
				eventoabre++;
			}
			abre = valv[i].abertura[j];
		}
		if (eventoabre == 0)
			Tevento[0] = -1000.;

		double fecha = valv[i].abertura[0];
		for (int j = 1; j < valv[i].parserie; j++) {
			if (fecha == 1 && valv[i].abertura[j] == 0) {
				Teventof[eventofecha] = valv[i].tempo[j - 1];
				eventofecha++;
			}
			fecha = valv[i].abertura[j];
		}
		if (eventofecha == 0)
			Teventof[0] = -1000.;
	}

}

void Ler::geraMaster1(Cel* celula) {

	int posicM = master1.posic;
	double area = celula[posicM].duto.area;
	//celula[posicM].flui.tab=0;
	//celula[posicM+1].flui.tab=0;
	double razarea=1.;
	if(master1.curvaCV==1){
		funcRazCV(1.,master1.cvCurv,master1.ncv,1.,area, razarea);
	}
	choke master(area, razarea * area);
	celula[posicM].acsr.tipo = 5;
	celula[posicM].acsr.chk = master;
	if (posicM < ncelp - 1)
		celula[posicM + 1].acsrL = &celula[posicM].acsr;
	double abre = master1.abertura[0];
	for (int i = 1; i < master1.parserie; i++) {
		if (abre == 0 && master1.abertura[i] == 1) {
			Tevento[eventoabre] = master1.tempo[i - 1];
			eventoabre++;
		}
		abre = master1.abertura[i];
	}
	if (eventoabre == 0)
		Tevento[0] = -1000.;

	double fecha = master1.abertura[0];
	for (int i = 1; i < master1.parserie; i++) {
		if (fecha == 1 && master1.abertura[i] == 0) {
			Teventof[eventofecha] = master1.tempo[i - 1];
			eventofecha++;
		}
		fecha = master1.abertura[i];
	}
	if (eventofecha == 0)
		Teventof[0] = -1000.;

}

void Ler::geraMaster2(CelG* celula) {

	int posicM = master2.posic;
	double area = celula[posicM].duto.area;
	ChokeGas master(flug,area, 1.,0.84);
	celula[posicM].chkcell = master;
	celula[posicM].labelchk = 1;
	if (posicM < ncelg - 1)
		celula[posicM + 1].fechamon = 0;

}

void Ler::gerapresfim(double& presfim,
		double& pGSup) {

	pGSup = psep.pres[0];
	presfim = psep.pres[0];

}

void Ler::gerachokesup(choke& chokeSup) {
	int indduto = celp[ncelp - 1].duto;
	int indcorte = duto[indduto].indcorte;
	double dia = corte[indcorte].a;
	//celp[ncelp].
	double area = M_PI * dia * dia / 4;
	if(chokep.curvaCV==1){
		double razarea;
		double abertura;
		if(chokep.curvaDinamic==1){
			double entradaPres=chokep.abertura[0];
			double pJus=psep.pres[0];
			double Atub=0.018241469;
			double Aatua=0.072965877;
			double kMola=10.;
			abertura=-pJus*Atub/(kMola)+entradaPres*Aatua/(kMola)-kMola/(kMola);
		}
		else abertura=chokep.abertura[0];
		funcRazCV(abertura,chokep.cvCurv,chokep.ncv,chokep.cd,
				  area, razarea);
		chokeSup.AreaGarg = razarea * area;
	}
	else
		chokeSup.AreaGarg = chokep.abertura[0] * area;
	chokeSup.AreaTub = area;
	chokeSup.cdchk = chokep.cd;
}

void Ler::gerachokeinj(ChokeGas& chokeInj) {
	int indduto = celg[0].duto;
	int indcorte = duto[indduto].indcorte;
	double dia = corte[indcorte].a;
	//celp[ncelp].
	double area = M_PI * dia * dia / 4;
	chokeInj.areagarg = chokes.abertura[0] * area;
	chokeInj.cd = chokes.cd;
	if(gasinj.tipoCC==0)
	chokeInj.presEstag=gasinj.presinj[0];
	else chokeInj.presEstag=celg[0].pres;
	chokeInj.presGarg=celg[0].pres;
	chokeInj.tempEstag=gasinj.temperatura[0];
	chokeInj.tipo=0;
	chokeInj.flui=flug;
}

void Ler::indraz(int& ind, double& raz,
		double tempo, int parserie, double* serietemp) {

	for (int i = 0; i <= parserie - 1; i++) {
		if (i < parserie - 1) {
			if (tempo >= serietemp[i] && tempo < serietemp[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - serietemp[i])
								/ (serietemp[i + 1] - serietemp[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == parserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void Ler::atualiza(int inicio, int extrem,int anel,
		choke& chokeSup,ChokeGas& chokeInj, Cel* celula,
		CelG* celulaG, double& pGSup,
		double& temperatura, double& presiniG, double& tempiniG,
		double& presE, double& tempE, double& titE,
		double& betaE, double tempo, double dt) {  //alteracao7
    int ind;
	double raz;
	int celpos = master1.posic;
	indraz(ind, raz, tempo, master1.parserie, master1.tempo);
	double aberinf = master1.abertura[ind];
	double abersup;
	if (ind < master1.parserie - 1)
		abersup = master1.abertura[ind + 1];
	else
		abersup = master1.abertura[ind];

	if(master1.curvaCV==1){
		double razarea;
		funcRazCV((aberinf * raz + (1 - raz) * abersup),master1.cvCurv,master1.ncv,1.,
				celula[celpos].duto.area, razarea);
		celula[celpos].acsr.chk.AreaGarg = razarea * celula[celpos].duto.area;
	}
	else
		celula[celpos].acsr.chk.AreaGarg = (aberinf * raz + (1 - raz) * abersup)
											* celula[celpos].duto.area;
	//if(raz<0.99999999)celula[celpos].acsr.chk.AreaGarg=abersup*celula[celpos].duto.area;

	if(nvalv>0){
		for(int i=0;i<nvalv;i++){
			int celpos = valv[i].posicP;
			indraz(ind, raz, tempo, valv[i].parserie, valv[i].tempo);
			double aberinf = valv[i].abertura[ind];
			double abersup;
			if (ind < valv[i].parserie - 1)
				abersup = valv[i].abertura[ind + 1];
			else
				abersup = valv[i].abertura[ind];
			double abertura;
			if(valv[i].curvaDinamic==1){
				double entradaPres=aberinf * raz + (1 - raz) * abersup;
				double pJus=celula[valv[i].posicP+1].pres;
				double Atub=0.018241469;
				double Aatua=0.072965877;
				double kMola=10.;
				double camort=0.;
				if(tempo>100){
					camort=0./dt;
				}
				else valv[i].xini=1.;
				abertura=-pJus*Atub/(camort+kMola)+entradaPres*Aatua/(camort+kMola)-kMola/(camort+kMola)+camort*valv[i].xini/(camort+kMola);
				//if(abertura<0)abertura=0.05;
				double variavel=0.04*(rand()%100)/100.;
				abertura+=(variavel-0.02);
				valv[i].xini=abertura;
			}
			else{
				abertura=aberinf * raz + (1 - raz) * abersup;
				//double variavel=0.04*(rand()%100)/100.;
				//abertura+=(variavel-0.02);
				//abertura=abertura-0.02*sinl(2*M_PI*tempo/60.);
			}
			if(valv[i].curvaCV==1){
				double razarea;
				funcRazCV(abertura,valv[i].cvCurv,valv[i].ncv,valv[i].cd,
						celula[celpos].duto.area, razarea);
				celula[celpos].acsr.chk.AreaGarg = razarea * celula[celpos].duto.area;
			}
			else
				celula[celpos].acsr.chk.AreaGarg = abertura	* celula[celpos].duto.area;
		}
	}



	//////////////////////////////////////////////////////////////////////////////////////////
	indraz(ind, raz, tempo, nTcorrecaoMassaEspLiq, TcorrecaoMassaEspLiq);
	double estadoinf = VcorrecaoMassaEspLiq[ind];
	double estadosup;
	if (ind < nTcorrecaoMassaEspLiq - 1)
		estadosup = VcorrecaoMassaEspLiq[ind + 1];
	else
		estadosup = VcorrecaoMassaEspLiq[ind];
	correcaoMassaEspLiq=(estadoinf * raz + (1 - raz) * estadosup);
	//////////////////////////////////////////////////////////////////////////////////////////

	if (ConContEntrada ==1 && ((*vg1dSP).chaverede == 0 || inicio == 1 || (*vg1dSP).chaveRedeParalela!=0)) {  //alteracao4
		indraz(ind, raz, tempo, CCPres.parserie, CCPres.tempo);
		double inf = CCPres.pres[ind];
		double sup;
		if (ind < CCPres.parserie - 1)
			sup = CCPres.pres[ind + 1];
		else
			sup = CCPres.pres[ind];
		presE = (inf * raz + (1 - raz) * sup);

		inf = CCPres.temperatura[ind];
		if (ind < CCPres.parserie - 1)
			sup = CCPres.temperatura[ind + 1];
		else
			sup = CCPres.temperatura[ind];
		tempE = (inf * raz + (1 - raz) * sup);

		inf = CCPres.tit[ind];
		if (ind < CCPres.parserie - 1)
			sup = CCPres.tit[ind + 1];
		else
			sup = CCPres.tit[ind];
		titE = (inf * raz + (1 - raz) * sup);

		inf = CCPres.bet[ind];
		if (ind < CCPres.parserie - 1)
			sup = CCPres.bet[ind + 1];
		else
			sup = CCPres.bet[ind];
		betaE = (inf * raz + (1 - raz) * sup);

		if(titE<0){
			  double rgST=celula[0].flui.Deng*1.225;
			  double roST=141.5*1000./(131.5+celula[0].flui.API);
			  double rg=celula[0].flui.MasEspGas(presE,tempE);
			  double rl=celula[0].flui.MasEspLiq(presE,tempE);
			  double titH=celula[0].flui.FracMassHidra(presE,tempE);
			  double rcST=celula[0].fluicol.MasEspFlu(1.01, 20.);
			  double rc=celula[0].fluicol.MasEspFlu(presE,tempE);
			  double rlMix=betaE*rc+(1.-betaE)*rl;
			  double val1=((1.-betaE)*rl*titH/(1.-titH));
			  titE=val1/(rlMix+val1);
		}


	}
	else if (ConContEntrada ==2 && ((*vg1dSP).chaverede == 0 || inicio == 1)) {  //alteracao4
		indraz(ind, raz, tempo, CCVPres.parserie, CCVPres.tempo);
		double inf = CCVPres.pres[ind];
		double sup;
		if (ind < CCVPres.parserie - 1)
			sup = CCVPres.pres[ind + 1];
		else
			sup = CCVPres.pres[ind];
		presE = (inf * raz + (1 - raz) * sup);

	}

	if (tempo > 5000.) {
		int para;
		para = 0;
	}
	indraz(ind, raz, tempo, dtmaxserie.parserie, dtmaxserie.tempo);
	double Tinf = dtmaxserie.valor[ind];
	double Tsup;
	if (ind < dtmaxserie.parserie - 1)
		Tsup = dtmaxserie.valor[ind + 1];
	else
		Tsup = dtmaxserie.valor[ind];
	dtmax = Tinf * raz + (1 - raz) * Tsup;

	indraz(ind, raz, tempo,nsegrega, vecTSegrega);
	int segInf = vecSegrega[ind];
	int segSup;
	if (ind < nsegrega - 1)
		segSup = vecSegrega[ind + 1];
	else
		segSup = vecSegrega[ind];
	modoSegrega = segInf * raz + (1 - raz) * segSup;

	/*indraz(ind, raz, tempo,nestabCol, vecTestabCol);
	int estabInf = vecestabCol[ind];
	int estabSup;
	if (ind < nestabCol - 1)
		estabSup = vecestabCol[ind + 1];
	else
		estabSup = vecestabCol[ind];
	estabCol = estabInf * raz + (1 - raz) * estabSup;*/

	/*indraz(ind, raz, tempo, varRGO.parserie, varRGO.tempo);
	 double RGOinf=varRGO.valor[ind];
	 double RGOsup;
	 if(ind<varRGO.parserie-1)RGOsup=varRGO.valor[ind+1];
	 else RGOsup=varRGO.valor[ind];
	 trackRGO=RGOinf*raz+(1-raz)*RGOsup;*/

	if (lingas > 0) {
		celpos = master2.posic;
		indraz(ind, raz, tempo, master2.parserie, master2.tempo);
		aberinf = master2.abertura[ind];
		if (ind < master2.parserie - 1)
			abersup = master2.abertura[ind + 1];
		else
			abersup = master2.abertura[ind];
		celulaG[celpos].chkcell.areagarg = (aberinf * raz + (1 - raz) * abersup)
				* celulaG[celpos].duto.area;
		//if(raz<0.99999999)celulaG[celpos].chkcell.areagarg=abersup*celulaG[celpos].duto.area;
	}
	if (ninjgas > 0) {
		double vazinf;
		double vazsup;
		double tempinf;
		double tempsup;
		for (int i = 0; i < ninjgas; i++) {
			celpos = fonteg[i].posicP;
			indraz(ind, raz, tempo, fonteg[i].parserie, fonteg[i].tempo);
			vazinf = fonteg[i].vazgas[ind];
			tempinf = fonteg[i].temp[ind];
			if (ind < fonteg[i].parserie - 1) {
				vazsup = fonteg[i].vazgas[ind + 1];
				tempsup = fonteg[i].temp[ind + 1];
			} else {
				vazsup = fonteg[i].vazgas[ind];
				tempsup = fonteg[i].temp[ind];
			}
			celula[celpos].acsr.injg.QGas = vazinf * raz + (1 - raz) * vazsup;
			if(celula[celpos].acsr.injg.seco==0){
				vazinf = fonteg[i].vazcomp[ind];
				if (ind < fonteg[i].parserie - 1) {
					vazsup = fonteg[i].vazcomp[ind + 1];
				} else {
					vazsup = fonteg[i].vazcomp[ind];
				}
				celula[celpos].acsr.injg.razCompGas = vazinf * raz + (1 - raz) * vazsup;
			}
			celula[celpos].acsr.injg.temp = tempinf * raz + (1 - raz) * tempsup;
			if (celpos == 0)
				temperatura = celula[celpos].acsr.injg.temp;

		}
	}
	if (ninjliq > 0) {
		double vazinf;
		double vazsup;
		double betinf;
		double betsup;
		double tempinf;
		double tempsup;
		for (int i = 0; i < ninjliq; i++) {
			celpos = fontel[i].posicP;
			indraz(ind, raz, tempo, fontel[i].parserie, fontel[i].tempo);
			vazinf = fontel[i].vazliq[ind];
			betinf = fontel[i].bet[ind];
			tempinf = fontel[i].temp[ind];
			if (ind < fontel[i].parserie - 1) {
				vazsup = fontel[i].vazliq[ind + 1];
				betsup = fontel[i].bet[ind + 1];
				tempsup = fontel[i].temp[ind + 1];
			} else {
				vazsup = fontel[i].vazliq[ind];
				betsup = fontel[i].bet[ind];
				tempsup = fontel[i].temp[ind];
			}
			celula[celpos].acsr.injl.QLiq = 1
					* (vazinf * raz + (1 - raz) * vazsup);
			celula[celpos].acsr.injl.bet = 1
					* (betinf * raz + (1 - raz) * betsup);
			celula[celpos].acsr.injl.temp = 1
					* (tempinf * raz + (1 - raz) * tempsup);
			if (celpos == 0)
				temperatura = celula[celpos].acsr.injl.temp;

		}
	}

	if (ninjmass > 0) {
		double vazTinf;
		double vazTsup;
		double vazCinf;
		double vazCsup;
		double vazGinf=0.;
		double vazGsup=0.;
		double massT;
		double massC;
		double massG;
		double tit;
		double tempinf;
		double tempsup;
		for (int i = 0; i < ninjmass; i++) {
			celpos = fontem[i].posicP;
			indraz(ind, raz, tempo, fontem[i].parserie, fontem[i].tempo);
			vazTinf = fontem[i].vazMasT[ind];
			vazCinf = fontem[i].vazMasC[ind];
			if(fontem[i].condiTermo==1)vazGinf = fontem[i].vazMasG[ind];
			tempinf = fontem[i].temp[ind];
			if (ind < fontem[i].parserie - 1) {
				vazTsup = fontem[i].vazMasT[ind + 1];
				vazCsup=fontem[i].vazMasC[ind + 1];
				if(fontem[i].condiTermo==1)vazGsup = fontem[i].vazMasG[ind+1];
				tempsup = fontem[i].temp[ind + 1];
			} else {
				vazTsup = fontem[i].vazMasT[ind];
				vazCsup=fontem[i].vazMasC[ind];
				if(fontem[i].condiTermo==1)vazGsup = fontem[i].vazMasG[ind];
				tempsup = fontem[i].temp[ind];
			}
			massC=celula[celpos].acsr.injm.MassC = 1
					* (vazCinf * raz + (1 - raz) * vazCsup);
			massT=1
					* (vazTinf * raz + (1 - raz) * vazTsup);
			if(fontem[i].condiTermo==0){
				tit=celula[celpos].acsr.injm.FluidoPro.FracMassHidra(celula[celpos].pres, celula[celpos].temp);
				massG=celula[celpos].acsr.injm.MassG=(massT-massC)*tit;
				celula[celpos].acsr.injm.MassP=massT-massC-massG;
			}
			else{
				massG=celula[celpos].acsr.injm.MassG= 1
						* (vazGinf * raz + (1 - raz) * vazGsup);
				celula[celpos].acsr.injm.MassP=massT-massC-massG;
			}
			celula[celpos].acsr.injm.temp = 1
					* (tempinf * raz + (1 - raz) * tempsup);
			if (celpos == 0)
				temperatura = celula[celpos].acsr.injm.temp;

		}
	}

	if (nbcs > 0) {
		double freqinf;
		double freqsup;
		for (int i = 0; i < nbcs; i++) {
			celpos = bcs[i].posicP;
			indraz(ind, raz, tempo, bcs[i].parserie, bcs[i].tempo);
			freqinf = bcs[i].freq[ind];
			if (ind < bcs[i].parserie - 1)
				freqsup = bcs[i].freq[ind + 1];
			else
				freqsup = bcs[i].freq[ind];
			celula[celpos].acsr.bcs.freqnova = freqinf * raz
					+ (1 - raz) * freqsup;
		}
	}

	if (nbvol > 0) {
		double freqinf;
		double freqsup;
		for (int i = 0; i < nbvol; i++) {
			celpos = bvol[i].posicP;
			indraz(ind, raz, tempo, bvol[i].parserie, bvol[i].tempo);
			freqinf = bvol[i].freq[ind];
			if (ind < bvol[i].parserie - 1)
				freqsup = bvol[i].freq[ind + 1];
			else
				freqsup = bvol[i].freq[ind];
			celula[celpos].acsr.bvol.freq = freqinf * raz + (1 - raz) * freqsup;
		}
	}

	if (ndpreq > 0) {
		double dpinf;
		double dpsup;
		for (int i = 0; i < ndpreq; i++) {
			celpos = dpreq[i].posicP;
			indraz(ind, raz, tempo, dpreq[i].parserie, dpreq[i].tempo);
			dpinf = dpreq[i].dp[ind];
			if (ind < dpreq[i].parserie - 1)
				dpsup = dpreq[i].dp[ind + 1];
			else
				dpsup = dpreq[i].dp[ind];
			celula[celpos].acsr.delp = dpinf * raz + (1 - raz) * dpsup;
		}
	}

	if (npig > 0) {
		for (int i = 0; i < npig; i++) {
			if (tempo < (pig[i].tempo + 0.1)
					&& tempo > (pig[i].tempo - (*vg1dSP).localtiny)) {
				int celpos = pig[i].lanca;
				celula[celpos].estadoPig = 1;
				celula[celpos].razPig = 0.5;
				celula[celpos].razPigini = 0.5;
				celula[celpos].velPig = (celula[celpos].QL + celula[celpos].QG)
						/ celula[celpos].duto.area;
				celula[celpos].velPigini = celula[celpos].velPig;
				celula[celpos].indpig = i;  //alteracao2
			}
		}
	}

	if (nfuro > 0){
		for (int i = 0; i < nfuro; i++) {
		   celpos = furo[i].posicP;
	       indraz(ind, raz, tempo, furo[i].parserie, furo[i].tempo);
	       aberinf = furo[i].abertura[ind];
	       if (ind < furo[i].parserie - 1)
		    abersup = furo[i].abertura[ind + 1];
	       else
		     abersup = furo[i].abertura[ind];
	       celula[celpos].acsr.fontechk.chk.AreaGarg = (aberinf * raz + (1 - raz) * abersup)
			* celula[celpos].duto.area;

	       indraz(ind, raz, tempo, furo[i].parserieChk, furo[i].tempoChk);
	       aberinf = furo[i].check[ind];
	       if (ind < furo[i].parserieChk - 1)
		    abersup = furo[i].check[ind + 1];
	       else
		     abersup = furo[i].check[ind];
	       celula[celpos].acsr.fontechk.check = int(aberinf * raz + (1 - raz) * abersup);
		}
    }

	if ((*vg1dSP).chaverede == 0 || extrem == 1) {  //alteracao7
		if (controDesc == 0) {  //alteracao5
			indraz(ind, raz, tempo, psep.parserie, psep.tempo);
			double presinf = psep.pres[ind];
			double pressup;
			if (ind < psep.parserie - 1)
				pressup = psep.pres[ind + 1];
			else
				pressup = psep.pres[ind];
			pGSup = presinf * raz + (1 - raz) * pressup;
		}
	}

	if (controDesc == 0){
	     celpos = ncelp - 1;
	     indraz(ind, raz, tempo, chokep.parserie, chokep.tempo);
	     aberinf = chokep.abertura[ind];
	     if (ind < chokep.parserie - 1)
		    abersup = chokep.abertura[ind + 1];
	     else
		     abersup = chokep.abertura[ind];
			double abertura;
			if(chokep.curvaDinamic==1){
				double entradaPres=aberinf * raz + (1 - raz) * abersup;
				double pJus=pGSup;
				double Atub=0.018241469;
				double Aatua=0.072965877;
				double kMola=10.;
				abertura=-pJus*Atub/(kMola)+entradaPres*Aatua/(kMola)-kMola/(kMola);
			}
			else abertura=aberinf * raz + (1 - raz) * abersup;
			if(chokep.curvaCV==1){
				double razarea;
				funcRazCV(abertura,chokep.cvCurv,chokep.ncv,chokep.cd,
							celula[celpos].duto.area, razarea);
				chokeSup.AreaGarg = razarea * celula[celpos].duto.area;
			}
			else
				chokeSup.AreaGarg = (aberinf * raz + (1 - raz) * abersup)
										* celula[celpos].duto.area;
    }

	if (controDesc == 0 && lingas>0){
	     celpos = 0;
	     indraz(ind, raz, tempo, chokes.parserie, chokes.tempo);
	     aberinf = chokes.abertura[ind];
	     if (ind < chokes.parserie - 1)
		    abersup = chokes.abertura[ind + 1];
	     else
		     abersup = chokes.abertura[ind];
	     chokeInj.areagarg = (aberinf * raz + (1 - raz) * abersup)
			* celulaG[celpos].duto.area;
    }

	//if(controDesc==0){  //alteracao5
	//indraz(ind, raz, tempo, psep.parserie, psep.tempo);
	//double presinf = psep.pres[ind];
	//double pressup;
	//if (ind < psep.parserie - 1)
	//pressup = psep.pres[ind + 1];
	//else
	//pressup = psep.pres[ind];
	//pGSup = presinf * raz + (1 - raz) * pressup;
	//}

	/*indraz(ind, raz, tempo, psep.parserie, psep.tempo);
	 double presinf = psep.pres[ind];
	 double pressup;
	 if (ind < psep.parserie - 1)
	 pressup = psep.pres[ind + 1];
	 else
	 pressup = psep.pres[ind];
	 pGSup = presinf * raz + (1 - raz) * pressup;*/
	//pGSup=10.;

	/*for (int i = 0; i < ninjgas; i++) {
		celpos = fonteg[i].posicP;
		indraz(ind, raz, tempo, fonteg[i].parserie, fonteg[i].tempo);
		double vazinf = fonteg[i].vazgas[ind];
		double vazsup;
		if (ind < fonteg[i].parserie - 1)
			vazsup = fonteg[i].vazgas[ind + 1];
		else
			vazsup = fonteg[i].vazgas[ind];
		double tempinf = fonteg[i].temp[ind];
		double tempsup;
		if (ind < fonteg[i].parserie - 1)
			tempsup = fonteg[i].temp[ind + 1];
		tempsup = fonteg[i].temp[ind];
		celula[celpos].acsr.injg.QGas = vazinf * raz + (1 - raz) * vazsup;
		celula[celpos].acsr.injg.temp = tempinf * raz + (1 - raz) * tempsup;
		if (celpos == 0)
			temperatura = celula[celpos].acsr.injg.temp;

	}*/

	for (int i = 0; i < nipr; i++) {
		celpos = IPRS[i].indcel;

		indraz(ind, raz, tempo, IPRS[i].seriep, IPRS[i].tpres);
		double inf = IPRS[i].pres[ind];
		double sup;
		if (ind < IPRS[i].seriep - 1)
			sup = IPRS[i].pres[ind + 1];
		else
			sup = IPRS[i].pres[ind];
		celula[celpos].acsr.ipr.Pres = inf * raz + (1 - raz) * sup;

		indraz(ind, raz, tempo, IPRS[i].seriet, IPRS[i].ttemp);
		inf = IPRS[i].temp[ind];
		if (ind < IPRS[i].seriet - 1)
			sup = IPRS[i].temp[ind + 1];
		else
			sup = IPRS[i].temp[ind];
		celula[celpos].acsr.ipr.Tres = inf * raz + (1 - raz) * sup;

		if(IPRS[i].tipoIPR==0 || IPRS[i].tipoIPR==1){
			indraz(ind, raz, tempo, IPRS[i].serieip, IPRS[i].tip);
				inf = IPRS[i].ip[ind];
			if (ind < IPRS[i].serieip - 1)
				sup = IPRS[i].ip[ind + 1];
			else
				sup = IPRS[i].ip[ind];
			celula[celpos].acsr.ipr.ip = inf * raz + (1 - raz) * sup;
		}
		else if(IPRS[i].tipoIPR==2){
			indraz(ind, raz, tempo, IPRS[i].serieqm, IPRS[i].tqMax);
				inf = IPRS[i].qMax[ind];
			if (ind < IPRS[i].serieqm - 1)
				sup = IPRS[i].qMax[ind + 1];
			else
				sup = IPRS[i].qMax[ind];
			celula[celpos].acsr.ipr.qMax = inf * raz + (1 - raz) * sup;
		}

		indraz(ind, raz, tempo, IPRS[i].seriejp, IPRS[i].tjp);
		inf = IPRS[i].jp[ind];
		if (ind < IPRS[i].seriejp - 1)
			sup = IPRS[i].jp[ind + 1];
		else
			sup = IPRS[i].jp[ind];
		celula[celpos].acsr.ipr.ij = inf * raz + (1 - raz) * sup;
		if (celpos == 0)
			temperatura = celula[celpos].acsr.ipr.Tres;

	}

	if (lingas > 0) {
		/*if (gasinj.tipoCC == 1) {
		 indraz(ind, raz, tempo, gasinj.parserie, gasinj.tempo);
		 double vazinf = gasinj.vazgas[ind];
		 double tinf = gasinj.temperatura[ind];
		 double vazsup;
		 double tsup;
		 if (ind < gasinj.parserie - 1) {
		 vazsup = gasinj.vazgas[ind + 1];
		 tsup = gasinj.temperatura[ind + 1];
		 } else {
		 vazsup = gasinj.vazgas[ind];
		 tsup = gasinj.temperatura[ind];
		 }
		 //celulaG[0].massfonteCH =(vazinf*raz+(1-raz)*vazsup)*flug.MasEspGas(1.,15.)/86400;
		 celulaG[0].massfonteCH = (vazinf * raz + (1 - raz) * vazsup)
		 * (flug.Deng * 1.225) / 86400;
		 tempiniG = tinf * raz + (1 - raz) * tsup;
		 } else if (gasinj.tipoCC == 0) {
		 indraz(ind, raz, tempo, gasinj.parserie, gasinj.tempo);
		 double presinf = gasinj.presinj[ind];
		 double tinf = gasinj.temperatura[ind];
		 double pressup;
		 double tsup;
		 if (ind < gasinj.parserie - 1) {
		 pressup = gasinj.presinj[ind + 1];
		 tsup = gasinj.temperatura[ind + 1];
		 } else {
		 pressup = gasinj.presinj[ind];
		 tsup = gasinj.temperatura[ind];
		 }
		 presiniG = (presinf * raz + (1 - raz) * pressup);
		 tempiniG = tinf * raz + (1 - raz) * tsup;
		 }*/
		if (controDesc == 0 && anel==-1) {  //alteracao5
			if (gasinj.tipoCC == 1) {
				indraz(ind, raz, tempo, gasinj.parserie, gasinj.tempo);
				double vazinf = gasinj.vazgas[ind];
				double tinf = gasinj.temperatura[ind];
				double vazsup;
				double tsup;
				if (ind < gasinj.parserie - 1) {
					vazsup = gasinj.vazgas[ind + 1];
					tsup = gasinj.temperatura[ind + 1];
				} else {
					vazsup = gasinj.vazgas[ind];
					tsup = gasinj.temperatura[ind];
				}
				//celulaG[0].massfonteCH =(vazinf*raz+(1-raz)*vazsup)*flug.MasEspGas(1.,15.)/86400;
				celulaG[0].massfonteCH = (vazinf * raz + (1 - raz) * vazsup)
						* (flug.Deng * 1.225) / 86400;
				tempiniG = tinf * raz + (1 - raz) * tsup;
			} else if (gasinj.tipoCC == 0) {
				indraz(ind, raz, tempo, gasinj.parserie, gasinj.tempo);
				double presinf = gasinj.presinj[ind];
				double tinf = gasinj.temperatura[ind];
				double pressup;
				double tsup;
				if (ind < gasinj.parserie - 1) {
					pressup = gasinj.presinj[ind + 1];
					tsup = gasinj.temperatura[ind + 1];
				} else {
					pressup = gasinj.presinj[ind];
					tsup = gasinj.temperatura[ind];
				}
				presiniG = (presinf * raz + (1 - raz) * pressup);
				tempiniG = tinf * raz + (1 - raz) * tsup;
			}
		} else if(anel==-1)
			tempiniG = tempDescarga;

	}

}


void Ler::atualizaSonico(double tempo,int& vExpli){
    int ind=0;
	double raz;
	indraz(ind, raz, tempo, nTsonico, Tempsonico);
	double inf = Vsonico[ind];
	double sup;
	if (ind < nTsonico - 1)
		sup = Vsonico[ind + 1];
	else
		sup = Vsonico[ind];

	vExpli = (inf * raz + (1 - raz) * sup);
}


double Ler::fqlst(Cel* const celula,int i, double tempo){
	double bo;
	double ba;
	double fwi;
	double bswi;
	double beti;
	double rsi;
	double rhogst;
	double rhoast;
	double rhoost;
	if (celula[i].QL > 0. && i > 0) {
		//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
	    double razdx;
	    razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
	    double tmed=(1-razdx)*celula[i].temp+razdx*celula[i-1].temp;
	    double pmed=(1-razdx)*celula[i].pres+razdx*celula[i-1].pres;
		int r=1;
		if(tempo<=1e-10)r=0;
		bo = celula[i - r].flui.BOFunc(pmed,tmed);
		ba = celula[i - r].flui.BAFunc(pmed,tmed);
		bswi = celula[i - r].flui.BSW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		beti = celula[i - r].bet;
		//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
		rsi = celula[i - r].flui.RS(pmed,tmed) * (6.29 / 35.31467);
		rhogst = celula[i - r].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i - r].flui.API);
		rhoast = 1000 * celula[i - r].flui.Denag;
	} else {
		//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].temp);
		bo = celula[i].flui.BOFunc(celula[i].pres, celula[i].temp);
		ba = celula[i].flui.BAFunc(celula[i].pres, celula[i].temp);
		bswi = celula[i].flui.BSW;
		//fwi=celula[i].FW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		beti = celula[i].bet;
		//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
		rsi = celula[i].flui.RS(celula[i].pres, celula[i].temp)
				* (6.29 / 35.31467);
		rhogst = celula[i].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i].flui.API);
		rhoast = 1000 * celula[i].flui.Denag;
	}
	//double qostd=celula[i].Mliqini*((1-fwi)*(1.-beti)*86400/bo)/(rpi*(1-beti)+rci*beti);
	double qostd = 0.;
	if(celula[i].flui.dStockTankVaporMassFraction<1.-1e-15)qostd = celula[i].QL * (1 - fwi) * (1. - beti)
					* 86400 / bo;
	//flut[i][k] = qostd*(1.+bswi/(1-bswi));
	//double qostd=celula[i].Mliqini*(1-beti)*86400/(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
	return qostd;
}
double Ler::fqlstTot(Cel* const celula,int i, double tempo){
	double bo;
	double ba;
	double fwi;
	double bswi;
	double beti;
	double rsi;
	double rhogst;
	double rhoast;
	double rhoost;
	if (celula[i].QL > 0. && i > 0) {
		//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
	    double razdx;
	    razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
	    double tmed=(1-razdx)*celula[i].temp+razdx*celula[i-1].temp;
	    double pmed=(1-razdx)*celula[i].pres+razdx*celula[i-1].pres;
		int r=1;
		if(tempo<=1e-10)r=0;
		bo = celula[i - r].flui.BOFunc(pmed,tmed);
		ba = celula[i - r].flui.BAFunc(pmed,tmed);
		bswi = celula[i - r].flui.BSW;
		//fwi=celula[i-1].FW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		beti = celula[i - r].bet;
		//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
		rsi = celula[i - r].flui.RS(pmed,tmed) * (6.29 / 35.31467);
		rhogst = celula[i - r].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i - r].flui.API);
		rhoast = 1000 * celula[i - r].flui.Denag;
	} else {
		bo = celula[i].flui.BOFunc(celula[i].pres,celula[i].temp);
		ba = celula[i].flui.BAFunc(celula[i].pres,celula[i].temp);
		bswi = celula[i].flui.BSW;
		//fwi=celula[i-1].FW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].teba*bswi - bswi * bo);
		beti = celula[i].bet;
		//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
		rsi = celula[i].flui.RS(celula[i].pres, celula[i].temp)
				* (6.29 / 35.31467);
		rhogst = celula[i].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i].flui.API);
		rhoast = 1000 * celula[i].flui.Denag;
	}
	//double qostd=celula[i].Mliqini*((1-fwi)*(1.-beti)*86400/bo)/(rpi*(1-beti)+rci*beti);
	double qostd =0.;
	if(celula[i].flui.dStockTankVaporMassFraction<1.-1e-15)qostd= celula[i].QL * (1 - fwi) * (1. - beti)
			* 86400 / bo;
	double qowstd = qostd;
	if (bswi < 1 - 1e-15)
		qowstd /= (1 - bswi);
	double qlsC = 0.;
	if (beti > 1e-15)
		qlsC = 86400 * celula[i].QL * beti
				* celula[i].fluicol.MasEspFlu(
						celula[i].pres, celula[i].temp)
				/ celula[i].fluicol.MasEspFlu(1., 15);
	//flut[i][k] = qostd*(1.+bswi/(1-bswi));
	//double qostd=celula[i].Mliqini*(1-beti)*86400/(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
	return qowstd + qlsC;
}
double Ler::fqlwst(Cel* const celula,int i, double tempo){
	double bo;
	double ba;
	double fwi;
	double bswi;
	double beti;
	double rsi;
	double rhogst;
	double rhoast;
	double rhoost;
	if (celula[i].QL > 0. && i > 0) {
		//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
	    double razdx;
	    razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
	    double tmed=(1-razdx)*celula[i].temp+razdx*celula[i-1].temp;
	    double pmed=(1-razdx)*celula[i].pres+razdx*celula[i-1].pres;
		int r=1;
		if(tempo<=1e-10)r=0;
		bo = celula[i - r].flui.BOFunc(pmed,tmed);
		ba = celula[i - r].flui.BAFunc(pmed,tmed);
		bswi = celula[i - r].flui.BSW;
		//fwi=celula[i-1].FW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		beti = celula[i - r].bet;
		//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
		rsi = celula[i - r].flui.RS(pmed,tmed) * (6.29 / 35.31467);
		rhogst = celula[i - r].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i - r].flui.API);
		rhoast = 1000 * celula[i - r].flui.Denag;
	} else {
		//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].temp);
		bo = celula[i].flui.BOFunc(celula[i].pres, celula[i].temp);
		ba = celula[i].flui.BAFunc(celula[i].pres, celula[i].temp);
		bswi = celula[i].flui.BSW;
		//fwi=celula[i].FW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		beti = celula[i].bet;
		//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
		rsi = celula[i].flui.RS(celula[i].pres, celula[i].temp)
				* (6.29 / 35.31467);
		rhogst = celula[i].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i].flui.API);
		rhoast = 1000 * celula[i].flui.Denag;
	}
	//double qostd=celula[i].Mliqini*((1-fwi)*(1.-beti)*86400/bo)/(rpi*(1-beti)+rci*beti);
	double qostd=0.;
	if(celula[i].flui.dStockTankVaporMassFraction<1.-1e-15)qostd= celula[i].QL * (1 - fwi) * (1. - beti)
			* 86400 / bo;
	//flut[i][k] = qostd*(1.+bswi/(1-bswi));
	//double qostd=celula[i].Mliqini*(1-beti)*86400/(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
	if (celula[i].flui.BSW < 1 - 1e-9)
		return qostd / (1 - celula[i].flui.BSW);
	else
		return celula[i].QL;
}
double Ler::fqgst(Cel* const celula,int i, double tempo){
	double bo;
	double ba;
	double fwi;
	double bswi;
	double beti;
	double rsi;
	double dgi;
	double rhogst;
	double rhoast;
	double rhoost;
	if (celula[i].QL > 0. && i > 0) {
		//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
		//bo = celula[i - 1].flui.BOFunc(celula[i-1].pres,
				//celula[i - 1].temp);
		int r=1;
		if(tempo<=1e-10)r=0;
	    double razdx;
	    razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
	    double tmed=(1-razdx)*celula[i].temp+razdx*celula[i-1].temp;
	    double pmed=(1-razdx)*celula[i].pres+razdx*celula[i-1].pres;
	    //double tmed=celula[i-1].temp;
	    //double pmed=celula[i-1].presaux;
		bo= celula[i-r].flui.BOFunc(pmed,tmed);
		ba = celula[i-r].flui.BAFunc(pmed,tmed);
		bswi = celula[i-r].flui.BSW;
		//fwi=celula[i-1].FW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		beti = celula[i - r].bet;
		//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
		rsi = celula[i - r].flui.RS(pmed,tmed) * (6.29 / 35.31467);
		dgi = celula[i - r].flui.Deng;
		rhogst = celula[i - r].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i - r].flui.API);
		rhoast = 1000 * celula[i - r].flui.Denag;
	} else {
		//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].temp);
		bo = celula[i].flui.BOFunc(celula[i].pres, celula[i].temp);
		ba = celula[i].flui.BAFunc(celula[i].pres, celula[i].temp);
		bswi = celula[i].flui.BSW;
		//fwi=celula[i].FW;
		fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
		beti = celula[i].bet;
		//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
		rsi = celula[i].flui.RS(celula[i].pres, celula[i].temp)
				* (6.29 / 35.31467);
		dgi = celula[i].flui.Deng;
		rhogst = celula[i].flui.Deng * 1.225;
		rhoost = 1000 * 141.5 / (131.5 + celula[i].flui.API);
		rhoast = 1000 * celula[i].flui.Denag;
	}
	double qostd =0.;
	if(celula[i].flui.dStockTankVaporMassFraction<1.-1e-15){
		qostd= celula[i].QL * (1 - fwi) * (1. - beti)
			* 86400 / bo;
		//double qostd=celula[i].Mliqini*(1-beti)*86400*(1-bswi)/
		//(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
		return qostd * rsi
			+ (celula[i].MC - celula[i].Mliqini) * 86400
					/ (dgi * 1.225);
	}
	else{
		return celula[i].MC  * 86400/ (dgi * 1.225);
	}
}

double Ler::interpolaTempEnvelope(double pres){
	double temp=-10000.;

	if(pres<=envelopeHidra[0][0]){
 		logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
 			      		    		"Valor da pressao abaixo do limite inferior do envelope de hidratos cadastrado, valor assumido será a menor pressao",
 			      		          "", "");
 		return envelopeHidra[0][1];
	}
	else if(pres>=envelopeHidra[nparEnvelope-1][0]){
 		logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
 			      		    		"Valor da pressao acima do limite inferior do envelope de hidratos cadastrado, valor assumido será a maior pressao",
 			      		          "", "");
 		return envelopeHidra[nparEnvelope-1][1];
	}
	else{
		int menor=0;
		int maior=nparEnvelope-1;
		while(menor<=maior){
			int m=menor+floor((maior-menor)/2);
			if(envelopeHidra[m][0]<=pres && envelopeHidra[m+1][0]>=pres){
				double delP=pres-envelopeHidra[m][0];
				double delpMax=envelopeHidra[m+1][0]-envelopeHidra[m][0];
				return envelopeHidra[m][1]+(delP/delpMax)*(envelopeHidra[m+1][1]-envelopeHidra[m][1]);
			}
			else if(envelopeHidra[m][0]<pres) menor=m+1;
			else if(envelopeHidra[m][0]>pres)maior=m-1;
		}
	}

	return temp;
}

void Ler::imprimeProfile(Cel* const celula,
		FullMtx<double>& flut, double tempo, int indTramo, int nrede) {
	if (nperfisp > 0) {
		double comp = 0.;
		double altura = 0.;
		double alturaC = 0.;
		int unidade = 0;
		for (int i = 0; i < ncelp; i++) {
			flut[i][0]=comp;
			flut[i][1] = comp+celula[i].dx/2.;
			comp=flut[i][1];
			//altura += celula[i].dx/2. * sinl(celula[i].duto.teta);
			flut[i][2] = tempo;
			int k = 3;
			double pi=celula[i].pres;
			//double ti=(celula[i].temp*celula[i].dxL+celula[i].tempL*celula[i].dx)/
					       //(celula[i].dx+celula[i].dxL);
			double ti=celula[i].temp;
			double alfi=celula[i].alf;
			//if(i>0)alfi=(celula[i].alf*celula[i].dxL+celula[i].alfL*celula[i].dx)/
			//					       (celula[i].dx+celula[i].dxL);
			//else alfi=celula[i].alf;
			double FVHi=celula[i].FVH; //chris - hidratos
			double beti=celula[i].bet;
			//if(i>0)beti=(celula[i].bet*celula[i].dxL+celula[i].betL*celula[i].dx)/
			//								       (celula[i].dx+celula[i].dxL);
			//else beti=celula[i].bet;

			if (profp.pres == 1) {
				flut[i][k] = pi;
				k++;
			}
			if (profp.temp == 1) {
				flut[i][k] = ti;
				k++;
			}
			if (profp.hol == 1) {
				flut[i][k] = 1. - alfi;
				k++;
			}

			if (profp.FVH == 1) { //Modelagem Hidrato chris
				flut[i][k] = FVHi; //celula[i].FVH;
				k++;
			}

			if (profp.bet == 1) {
				flut[i][k] = beti;
				k++;
			}
			if (profp.ugs == 1) {
				/*if(i==ncelp-1)
					flut[i][k] = celula[i].QG / celula[i].duto.area;
				else if(i==0)flut[i][k] = celula[i+1].QG / celula[i].duto.area;
				else flut[i][k] = 0.5*(celula[i].QG+celula[i+1].QG) / celula[i].duto.area;*/
				flut[i][k] = celula[i].QG / celula[i].duto.area;
				k++;
			}
			if (profp.uls == 1) {
				/*if(i==ncelp-1)flut[i][k] = celula[i].QL / celula[i].duto.area;
				else if(i==0)flut[i+1][k] = celula[i].QL / celula[i].duto.area;
				else flut[i][k] = 0.5*(celula[i].QL+celula[i+1].QL) / celula[i].duto.area;*/
				flut[i][k] = celula[i].QL / celula[i].duto.area;
				k++;
			}
			if (profp.ug == 1) {
				/*if(i==ncelp-1){
					if (celula[i].alf > (*vg1dSP).localtiny)
						flut[i][k] = celula[i].QG
							/ (celula[i].duto.area * celula[i].alf);
					else
						flut[i][k] = 0;
				}
				else if(i==0){
					if (celula[i].alf > (*vg1dSP).localtiny)
						flut[i][k] = celula[i+1].QG
							/ (celula[i].duto.area * celula[i].alf);
					else
						flut[i][k] = 0;
				}
				else{
					if (celula[i].alf > (*vg1dSP).localtiny)
						flut[i][k] = 0.5*(celula[i].QG+celula[i+1].QG)
							/ (celula[i].duto.area * celula[i].alf);
					else
						flut[i][k] = 0;
				}*/
				if (celula[i].alf > (*vg1dSP).localtiny)
					flut[i][k] = celula[i].QG
						/ (celula[i].duto.area * celula[i].alf);
				else
					flut[i][k] = 0;
				k++;
			}
			if (profp.ul == 1) {
				/*if(i==ncelp-1){
					if (celula[i].alf < 1. - (*vg1dSP).localtiny)
						flut[i][k] = celula[i].QL
							/ (celula[i].duto.area * (1. - celula[i].alf));
					else
						flut[i][k] = 0;
				}
				else if(i==0){
					if (celula[i].alf < 1. - (*vg1dSP).localtiny)
						flut[i][k] = celula[i+1].QL
							/ (celula[i].duto.area * (1. - celula[i].alf));
					else
						flut[i][k] = 0;
				}
				else{
					if (celula[i].alf < 1. - (*vg1dSP).localtiny)
						flut[i][k] = 0.5*(celula[i].QL+celula[i+1].QL)
							/ (celula[i].duto.area * (1. - celula[i].alf));
					else
						flut[i][k] = 0;
				}*/
				if (celula[i].alf < 1. - (*vg1dSP).localtiny)
					flut[i][k] = celula[i].QL
						/ (celula[i].duto.area * (1. - celula[i].alf));
				else
					flut[i][k] = 0;
				k++;
			}
			if (profp.arra == 1) {
				flut[i][k] = celula[i].arranjo;
				k++;
			}
			if (profp.viscl == 1) {
				flut[i][k] = celula[i].flui.ViscOleo(pi,ti)*(1.-beti)+
						celula[i].fluicol.VisFlu(pi,ti)*beti;
				k++;
			}
			if (profp.viscg == 1) {
				flut[i][k] = celula[i].flui.ViscGas(celula[i].pres,
						celula[i].temp);
				k++;
			}
			if (profp.rhog == 1) {
				flut[i][k] = celula[i].flui.MasEspGas(pi,ti);
				//flut[i][k] = celula[i].flui.CalorGas(celula[i].pres,celula[i].temp);
				k++;
			}
			if (profp.rhol == 1) {
				flut[i][k] = celula[i].flui.MasEspLiq(pi,ti)*(1.-beti)+
						celula[i].fluicol.MasEspFlu(pi,ti)*beti;
				//flut[i][k] = celula[i].flui.CalorLiq(celula[i].pres,celula[i].temp);
				k++;
			}
			if (profp.rhoo == 1) {
							flut[i][k] = celula[i].flui.MasEspoleo(pi,ti);
							//flut[i][k] = celula[i].flui.CalorLiq(celula[i].pres,celula[i].temp);
							k++;
			}
			if (profp.rhoa == 1) {
							flut[i][k] = celula[i].flui.MasEspAgua(pi,ti);
							//flut[i][k] = celula[i].flui.CalorLiq(celula[i].pres,celula[i].temp);
							k++;
			}
			if (profp.rhoMix == 1) {
				flut[i][k] =alfi*celula[i].flui.MasEspGas(pi,ti)+ (1. - alfi)*(celula[i].flui.MasEspLiq(pi,ti)*(1.-beti)+
						celula[i].fluicol.MasEspFlu(pi,ti)*beti);
				//flut[i][k] = celula[i].flui.CalorLiq(celula[i].pres,celula[i].temp);
				k++;
			}
			if (profp.RS == 1) {
				flut[i][k] = celula[i].flui.RS(pi,ti)*6.29/35.31467;
				k++;
			}
			if (profp.masg == 1) {
				/*if(i==ncelp-1)flut[i][k] = celula[i].MC - celula[i].Mliqini;
				else if(i==0)flut[i][k] = celula[i+1].MC - celula[i+1].Mliqini;
				else flut[i][k] =0.5*(celula[i].MC - celula[i].Mliqini+
									  celula[i+1].MC - celula[i+1].Mliqini);*/
				flut[i][k] = celula[i].MC - celula[i].Mliqini;
				k++;
			}
			if (profp.masl == 1) {
				/*if(i==ncelp-1)flut[i][k] = celula[i].Mliqini;
				else if(i==0)flut[i][k] = celula[i+1].Mliqini;
				else flut[i][k] = 0.5*(celula[i].Mliqini+celula[i+1].Mliqini);*/
				flut[i][k] = celula[i].Mliqini;
				k++;
			}
			if (profp.c0 == 1) {
				flut[i][k] = celula[i].c0;
				k++;
			}
			if (profp.ud == 1) {
				flut[i][k] = celula[i].ud;
				k++;
			}
			if (profp.RGO == 1) {
				flut[i][k] = celula[i].flui.RGO;
				k++;
			}
			if (profp.deng == 1) {
				flut[i][k] = celula[i].flui.Deng;
				k++;
			}
			if (profp.yco2 == 1) {
				flut[i][k] = celula[i].flui.yco2;
				k++;
			}
			if (profp.calor == 1) {
				flut[i][k] = celula[i].calor.fluxIni;
				k++;
			}
			if (profp.masstrans == 1) {
				flut[i][k] = celula[i].FonteMudaFase;
				k++;
			}
			if (profp.cpg == 1) {
				//flut[i][k] = celula[i].flui.MasEspGas(celula[i].pres,celula[i].temp);
				flut[i][k] = celula[i].flui.CalorGas(pi,ti);
				k++;
			}
			if (profp.cpl == 1) {
				//flut[i][k] = celula[i].flui.MasEspLiq(celula[i].pres,celula[i].temp);
				flut[i][k] = celula[i].flui.CalorLiq(pi,ti)*(1.-beti)+
						celula[i].fluicol.CalorLiq(pi,ti)*beti;
				k++;
			}
			if (profp.qlst == 1) {
				/*if(i==ncelp-1)flut[i][k] = fqlst(celula,i,tempo);
				else if(i==0)flut[i][k] = fqlst(celula,i+1,tempo);
				else flut[i][k] = 0.5*(fqlst(celula,i,tempo)+fqlst(celula,i+1,tempo));*/
				flut[i][k] = fqlst(celula,i,tempo);
				k++;
			}

			if (profp.qlwst == 1) {
				/*if(i==ncelp-1)flut[i][k] = fqlwst(celula,i,tempo);
				else if(i==0)flut[i][k] = fqlwst(celula,i+1,tempo);
				else flut[i][k] =0.5*(fqlwst(celula,i,tempo)+fqlwst(celula,i+1,tempo));*/
				flut[i][k] = fqlwst(celula,i,tempo);
				k++;
			}

			if (profp.qlstTot == 1) {
				/*if(i==ncelp-1)flut[i][k] =fqlstTot(celula,i,tempo);
				else if(i==0)flut[i][k] =fqlstTot(celula,i+1,tempo);
				else flut[i][k] =0.5*(fqlstTot(celula,i,tempo)+fqlstTot(celula,i+1,tempo));*/
				flut[i][k] =fqlstTot(celula,i,tempo);
				k++;
			}

			if (profp.qgst == 1) {
				/*if(i==ncelp-1)flut[i][k] =fqgst(celula,i,tempo);
				else if(i==0)flut[i][k] =fqgst(celula,i+1,tempo);
				else flut[i][k] =0.5*(fqgst(celula,i,tempo)+fqgst(celula,i+1,tempo));*/
				flut[i][k] =fqgst(celula,i,tempo);
				k++;
			}
			if (profp.api == 1) {
				flut[i][k] = celula[i].flui.API;
				k++;
			}
			if (profp.bsw == 1) {
				flut[i][k] = celula[i].flui.BSW;
				k++;
			}
			if (profp.hidro == 1) {
				flut[i][k] = celula[i].termoHidro;
				k++;
			}
			if (profp.fric == 1) {
				flut[i][k] = celula[i].termoFric;
				k++;
			}
			if (profp.Term1== 1) {
				flut[i][k] = celula[i].term1;
				k++;
			}
			if (profp.Term2 == 1) {
				flut[i][k] = celula[i].term2;
				k++;
			}
			if (profp.dengD == 1) {
				flut[i][k] = celula[i].flui.Deng*celula[i].flui.rDgD;
				k++;
			}
			if (profp.dengL == 1) {
				flut[i][k] = celula[i].flui.Deng*celula[i].flui.rDgL;
				k++;
			}
			if (profp.reyi == 1) {
				flut[i][k] = celula[i].calor.reyi;
				k++;
			}
			if (profp.reye == 1) {
				flut[i][k] = celula[i].calor.reye;
				k++;
			}
			if (profp.Fr == 1) {
				flut[i][k] = ((celula[i].QG + celula[i].QL)/ celula[i].duto.area)/sqrtl(9.81*celula[i].duto.dia);
				k++;
			}
			if (profp.grashi == 1) {
				flut[i][k] = celula[i].calor.grashi;
				k++;
			}
			if (profp.grashe == 1) {
				flut[i][k] = celula[i].calor.grashe;
				k++;
			}
			if (profp.nusi == 1) {
				flut[i][k] = celula[i].calor.nusi;
				k++;
			}
			if (profp.nuse == 1) {
				flut[i][k] = celula[i].calor.nuse;
				k++;
			}
			if (profp.hi == 1) {
				flut[i][k] = celula[i].calor.hi;
				k++;
			}
			if (profp.he == 1) {
				flut[i][k] = celula[i].calor.he;
				k++;
			}
			if (profp.pri == 1) {
				flut[i][k] = celula[i].calor.pri;
				k++;
			}
			if (profp.pre == 1) {
				flut[i][k] = celula[i].calor.pre;
				k++;
			}
			if (profp.Rs == 1) {
				flut[i][k] = celula[i].flui.RS(pi,ti) * (6.29 / 35.31467);
				k++;
			}
			if (profp.Bo == 1) {
				flut[i][k] = celula[i].flui.BOFunc(pi,ti);
				k++;
			}
			if (profp.tAmb == 1) {
				flut[i][k] = celula[i].calor.Textern1;
				k++;
			}
			if (profp.PrG == 1) {
				double viscP=celula[i].flui.ViscGas(celula[i].pres, celula[i].temp)/1000.;
				double cpP=celula[i].flui.CalorGas(celula[i].pres, celula[i].temp);
				double kP=celula[i].flui.CondGas(celula[i].pres, celula[i].temp);
				flut[i][k] = (viscP)/(kP/cpP);
				k++;
			}
			if (profp.PrL == 1) {
				double viscP=celula[i].flui.ViscOleo(celula[i].pres, celula[i].temp)/1000.;
				double cpP=celula[i].flui.CalorLiq(celula[i].pres, celula[i].temp);
				double kP=celula[i].flui.CondLiq(celula[i].pres, celula[i].temp);
				flut[i][k] = (viscP)/(kP/cpP);
				k++;
			}
			if (profp.pseudoL == 1 && flashCompleto==2) {
				for(int konta=0; konta<flup[0].npseudo; konta++){
					flut[i][k] = celula[i].flui.oCalculatedLiqComposition[konta];
					k++;
				}
			}
			if (profp.pseudoG == 1 && flashCompleto==2) {
				for(int konta=0; konta<flup[0].npseudo; konta++){
					flut[i][k] = celula[i].flui.oCalculatedVapComposition[konta];
					k++;
				}
			}
			if (profp.pseudoM == 1 && flashCompleto==2) {
				for(int konta=0; konta<flup[0].npseudo; konta++){
					flut[i][k] = celula[i].flui.fracMol[konta];
					k++;
				}
			}
			if (profp.TResi == 1) {
				flut[i][k] = celula[i].fluicol.TR;
				k++;
			}
			if (profp.redAtrito == 1) {
				flut[i][k] = celula[i].dR;
				k++;
			}
			if (profp.angulo == 1) {
				flut[i][k] = celula[i].duto.teta;
				k++;
			}
			if (profp.diamInt == 1) {
				flut[i][k] = celula[i].duto.a;
				k++;
			}
			if (profp.TempParede == 1) {
				flut[i][k] = celula[i].calor.Tcamada[0][0];
				k++;
			}
			if (profp.subResfria == 1) {
				double tempH=interpolaTempEnvelope(celula[i].pres);
				flut[i][k] = celula[i].temp-tempH;
				k++;
			}
			if (profp.dadosParafina == 1) {
				double pres=celula[i].pres;
				double temp=celula[i].calor.Tcamada[0][0];
				celula[i].flui.atualizaPropParafina(pres, temp);
				flut[i][k] = celula[i].flui.dCloudPointTOutput;
				flut[i][k+1] = celula[i].flui.dInterpolatedCPWaxOutput;
				flut[i][k+2] = celula[i].flui.dInterpolatedThermCondOutput;
				flut[i][k+3] = celula[i].flui.dInterpolatedDensOutput;
				flut[i][k+4] = celula[i].flui.dInterpolatedLiqMWOutput;
				flut[i][k+5] = (celula[i].diamInterIni-celula[i].duto.a)/2.;
				flut[i][k+6] = celula[i].C2;
				flut[i][k+7] = celula[i].C3;
				flut[i][k+8] = celula[i].porosoPar;
				flut[i][k+9] = celula[i].MW_wax;
				flut[i][k+10] = celula[i].rhoWaxLiq;
				flut[i][k+11] = celula[i].Sum_dCwaxdT;
				k+=12;
			}
			if (unidade < nunidadep - 1) {
				if (comp >= unidadeP[unidade + 1].Lini)
					unidade++;
			}
			flut[i][k] = unidade;
			k++;
			flut[i][k] = altura;
			k++;
			alturaC=altura;
			comp += celula[i].dx/2.;
			//altura += celula[i].dx/2. * sinl(celula[i].duto.teta);
			altura += celula[i].dx * sinl(celula[i].duto.teta);
			flut[i][k] = alturaC+0.5*celula[i].dx * sinl(celula[i].duto.teta);
			k++;

		}
		ostringstream saidaP;
		int numero = roundl(tempo);
		if (indTramo < 0 && AS==0)
			saidaP << pathPrefixoArqSaida << "PERFISP" << "-" << numero
					<< ".dat";
		else if (indTramo < 0 && AS==1)
			saidaP << pathPrefixoArqSaida << "PERFISP"<<"-seqAS-"<<(*vg1dSP).sequenciaAS << "-" << numero
					<< ".dat";
		else
			saidaP << pathPrefixoArqSaida << "Tramo" << indTramo<<"-R-"<<nrede << "-"
					<< "PERFISP" << "-" << numero << ".dat";

	      time_t hoje = time(0);
	      struct tm * agora = localtime(&hoje);
	      char buffer[80];
	      strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", agora);

	      std::string dataStr(buffer);


		string tmp = saidaP.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << "# Passo de tempo = " << (*vg1dSP).contador << " # Posicao ANM= "<< master1.comp<< " # data e hora da simulacao "<< dataStr<< " # Versao "<< versao<<"\n";
		escreveIni << " Comprimento (m) Fronteira F;";
		escreveIni << " Comprimento (m) Centro Volume C;";
		escreveIni << " Tempo (s) ;";
		if (profp.pres == 1)
			escreveIni << " Pressao (kgf/cm2) C;";
		if (profp.temp == 1)
			escreveIni << " Temperatura (C) C;";
		if (profp.hol == 1)
			escreveIni << " Holdup de liquido (-) C;";

		if (profp.FVH == 1) //solver de Hidratos - chris
			escreveIni << " FVH (-) C;";

		if (profp.bet == 1)
			escreveIni << " Fracao vol. de liquido complementar (-) C;";
		if (profp.ugs == 1)
			escreveIni << "Velocidade superficial de gas (m/s) F;";
		if (profp.uls == 1)
			escreveIni << " Velocidade superficial do liquido (m/s) F;";
		if (profp.ug == 1)
			escreveIni << " Velocidade de gas (m/s) F;";
		if (profp.ul == 1)
			escreveIni << " Velocidade do liquido (m/s) F;";
		if (profp.arra == 1)
			escreveIni << " Indicador de arranjo de fases (-) F;";
		if (profp.viscl == 1)
			escreveIni << " Viscosidade do Liquido (cP) C;";
		if (profp.viscg == 1)
			escreveIni << " Viscosidade do Gas (cP) C;";
		if (profp.rhog == 1)
			escreveIni << " Massa Especifica do Gas (kg/m3) C;";
		if (profp.rhol == 1)
			escreveIni << " Massa Especifica do Liquido (kg/m3) C;";
		if (profp.rhoo == 1)
			escreveIni << " Massa Especifica do Oleo (kg/m3) C;";
		if (profp.rhoa == 1)
			escreveIni << " Massa Especifica da Agua (kg/m3) C;";
		if (profp.rhoMix == 1)
			escreveIni << " Massa Especifica da Mistura (kg/m3) C;";
		if (profp.RS == 1)
			escreveIni << " Razao de Solubilidade (Sm3/Sm3) C;";
		if (profp.masg == 1)
			escreveIni << " Vazao Massica do Gas (kg/s) F;";
		if (profp.masl == 1)
			escreveIni << " Vazao Massica do Liquido (kg/s) F;";
		if (profp.c0 == 1)
			escreveIni << " Coeficiente de distribuição: C0 (-) F;";
		if (profp.ud == 1)
			escreveIni << " Velocidade de escorregamento: Ud (m/s) F;";
		if (profp.RGO == 1)
			escreveIni << " RGO (Sm3/Sm3) C;";
		if (profp.deng == 1)
			escreveIni << " Densidade do Gas (-) C;";
		if (profp.yco2 == 1)
			escreveIni << " Fracao Molar de CO2 (-) C;";
		if (profp.calor == 1)
			escreveIni << " Fluxo de calor entre escoamento e parede (W/m) C;";
		if (profp.masstrans == 1)
			escreveIni << " Transferencia de Massa entre Fases (kg / [s m]) C;";
		if (profp.cpg == 1)
			escreveIni
					<< " Calor Especifico a pressao constante do Gas (J/[kg C]) C;";
		if (profp.cpl == 1)
			escreveIni
					<< " Calor Especifico a pressao constante do Liquido (J/[kg C]) C;";
		if (profp.qlst == 1)
			escreveIni << " Vazao volumetrica standard de oleo morto (Sm3/d) F;";
		if (profp.qlwst == 1)
			escreveIni << " Vazao volumetrica standard de oleo morto + agua (Sm3/d) F;";
		if (profp.qlstTot == 1)
			escreveIni << " Vazao volumetrica standard de oleo morto + agua + liquido complementar (Sm3/d) F;";
		if (profp.qgst == 1)
			escreveIni << " Vazao volumetrica standard de gas livre + dissolvido (Sm3/d) F;";
		if (profp.api == 1)
			escreveIni << " Grau API (-) C;";
		if (profp.bsw == 1)
			escreveIni << " BSW (-) C;";
		if (profp.hidro == 1)
			escreveIni << " Termo Hidrostatico (Pa/m) F;";
		if (profp.fric == 1)
			escreveIni << " Termo Fricao (Pa/m) F;";
		if (profp.Term1 == 1)
			escreveIni << " Termo de relacao de vazao massica 1 (-) F;";
		if (profp.Term2 == 1)
			escreveIni << " Termo de relacao de vazao massica 2 (kg/s) F;";
		if (profp.dengD == 1)
			escreveIni << " Densidade Gas Dissolvido In Situ(-) C;";
		if (profp.dengL == 1)
			escreveIni << " Densidade Gas Livre In Situ(-) C;";
		if (profp.reyi == 1)
			escreveIni << " Reynolds interno da mistura (-) F;";
		if (profp.reye == 1)
			escreveIni << " Reynolds externo (-) F;";
		if (profp.Fr == 1)
			escreveIni << " Froud (-) F;";
		if (profp.grashi == 1)
			escreveIni << " Grashof interno da mistura (-) F;";
		if (profp.grashe == 1)
			escreveIni << " Grashof externo (-) F;";
		if (profp.nusi == 1)
			escreveIni << " Nusselt interno da mistura (-) F;";
		if (profp.nuse == 1)
			escreveIni << " Nusselt externo (-) F;";
		if (profp.hi == 1)
			escreveIni << " Coeficiente de pelicula interno da mistura (W / [m2 K]) F;";
		if (profp.he == 1)
			escreveIni << " Coeficiente de pelicula externo (W / [m2 K]) F;";
		if (profp.pri == 1)
			escreveIni << " Prandtl interno da mistura (-) F;";
		if (profp.pre == 1)
			escreveIni << " Prandtl externo (-) F;";
		if (profp.Rs == 1)
			escreveIni << " Razao de Solubilidade (-) F;";
		if (profp.Bo == 1)
			escreveIni << " Fator Volume de Formacao (-) C;";
		if (profp.tAmb == 1)
			escreveIni << " Temperatura Ambiente (C) C;";
		if (profp.PrG == 1)
			escreveIni << " Prandtl do Gas (-) C;";
		if (profp.PrL == 1)
			escreveIni << " Prandtl do Liquido (-) C;";
		if (profp.pseudoL == 1 && flashCompleto==2) {
			for(int konta=0; konta<flup[0].npseudo; konta++){
				escreveIni << " Fracoes molares no Liquido "<<konta<<";";
			}
		}
		if (profp.pseudoG == 1 && flashCompleto==2) {
			for(int konta=0; konta<flup[0].npseudo; konta++){
				escreveIni << " Fracoes molares no Gas "<<konta<<";";
			}
		}
		if (profp.pseudoM == 1 && flashCompleto==2) {
			for(int konta=0; konta<flup[0].npseudo; konta++){
				escreveIni << " Fracoes molares no Liquido + Gas  "<<konta<<";";
			}
		}
		if (profp.TResi == 1)
			escreveIni << " Tempo de Residencia-Fluido Complementar (s) C;";
		if (profp.redAtrito == 1)
			escreveIni << " Fator de reducao de friccao (-) C;";
		if (profp.angulo == 1)
			escreveIni << " Angulo (radiano) C;";
		if (profp.diamInt == 1)
			escreveIni << " Diametro Interno (m) C;";
		if (profp.TempParede == 1)
			escreveIni << " Temperatura Parede Interna (C) C;";
		if(profp.subResfria==1)
			escreveIni << " Subresfriamento (C) C;";
		if (profp.dadosParafina == 1){
			escreveIni << " TIAC (C) C;";
			escreveIni << " Cp Parafina (J/[kg C]) C;";
			escreveIni << " Condutividade Termica Parafina (W / [m K]) C;";
			escreveIni << " Massa Especifica Parafina (kg/m3) C;";
			escreveIni << " Massa molar do Liquido Parafina (kg/mol) C;";
			escreveIni << " Espessura Radial de Parafina Depositada (m) C;";
			escreveIni << " Termo C2 C;";
			escreveIni << " Termo C3 C;";
			escreveIni << " Porosidade Parafina (-) C;";
			escreveIni << " Massa Molecular Parafina (kgMol) C;";
			escreveIni << " Massa específica Parafina na fase liquida (kg/m3) C;";
			escreveIni << " Derivada da concentracao da parafina com a temnperatura na fase liquida (1/K) C;";
		}
		escreveIni << " id do duto;";
		escreveIni << " Elevacao (m) F;";
		escreveIni << " Elevacao (m) C;";
		escreveIni << "\n";

		escreveIni << flut;
		escreveIni.close();
		// caso nao seja simulacao POCO_INJETOR
		if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
			arqRelatorioPerfis << tmp.c_str() << endl;
			arqRelatorioPerfis.flush();
		}
	}
}

void Ler::imprimeProfileG(CelG* const celula,
		FullMtx<double>& flut, double tempo, int indTramo, int nrede) {
	if (nperfisg > 0) {
		double comp = 0;
		double altura =0.;
		double alturaC =0.;
		int unidade = 0;
		for (int i = 0; i < ncelg; i++) {
			// Comprimento a partir da Plataforma (m)
			flut[i][0] = comp;
			flut[i][1] = comp+celula[i].dx0/2.;
			comp=flut[i][1];
			//altura+= celula[i].dx0 * sinl(celula[i].duto.teta)/2.;
			// Comprimento a partir do fundo do Poco (m)
			flut[i][2] = anmP + (anmG - flut[i][0]);
			flut[i][3] = tempo;
			int k = 4;

			//double pi=(celula[i].pres*celula[i].dxL+celula[i].presL*celula[i].dx0)/
			//	       (celula[i].dx0+celula[i].dxL);
			//double ti=(celula[i].temp*celula[i].dxL+celula[i].tempL*celula[i].dx0)/
			//		       (celula[i].dx0+celula[i].dxL);

			double pi=celula[i].pres;
			double ti=celula[i].temp;

			if (profg.pres == 1) {
				flut[i][k] = pi;
				k++;
			}
			if (profg.temp == 1) {
				flut[i][k] = ti;
				k++;
			}
			if (profg.ugs == 1) {
				if (i != *(celula[i].celInter) - 1)
					flut[i][k] = celula[i].VGasR / celula[i].u1L;  //15-06-2018
				else
					flut[i][k] = celula[i].VGasR / celula[i - 1].u1L; //15-06-2018
				k++;
			}
			if (profg.ug == 1) {
				if (i != *(celula[i].celInter) - 1)
					flut[i][k] = celula[i].VGasR / celula[i].u1L;  //15-06-2018
				else
					flut[i][k] = celula[i].VGasR / celula[i - 1].u1L; //15-06-2018
				k++;
			}
			if (profg.tens == 1) {
				flut[i][k] = -1;
				k++;
			}
			if (profg.viscg == 1) {
				if (i < *(celula[i].celInter))
					flut[i][k] = celula[i].flui.ViscGas(pi,ti);  //15-06-2018
				else
					flut[i][k] = celula[i].VisFlu(pi,ti);  //15-06-2018
				k++;
			}
			if (profg.rhog == 1) {
				//flut[i][k] = celula[i].u1L / celula[i].duto.area;
				if (i < *(celula[i].celInter))
					flut[i][k] = celula[i].flui.MasEspGas(pi,ti);  //15-06-2018
				else {
					if (i > *(celula[i].celInter))
						flut[i][k] = celula[i].MasEspFlu(pi,ti);  //15-06-2018
					else
						flut[i][k] = (1. - celula[i].razInter)
								* celula[i].MasEspFlu(pi,ti)
								+ celula[i].razInter
										* celula[i].flui.MasEspGas(pi,ti); //15-06-2018
				}
				k++;
			}
			if (profg.masg == 1) {
				flut[i][k] = celula[i].VGasR;
				k++;
			}
			if (profg.hidro == 1) {
				flut[i][k] = celula[i].termoHidro;
				k++;
			}
			if (profg.fric == 1) {
				flut[i][k] = celula[i].termoFric;
				k++;
			}
			if (profg.FVHG == 1) {
				flut[i][k] = celula[i].FVHG; //chris - Hidratos
				k++;
			}
			if (profg.calor == 1) {
				flut[i][k] = celula[i].calor.fluxIni;
				k++;
			}
			if (profg.qgst == 1) {  //15-06-2018
				double dgi;
				if (i < *(celula[i].celInter)) {
					if (celula[i].VGasR > 0. && i > 0)
						dgi = celula[i - 1].flui.Deng;
					else
						dgi = celula[i].flui.Deng;
					flut[i][k] = (celula[i].VGasR) * 86400 / (dgi * 1.225);
				} else {
					flut[i][k] = (celula[i].VGasR) * 86400
							/ celula[i].MasEspFlu(1.0, 15.6);
				}

				k++;
			}  //15-06-2018
			if (profg.reyi == 1) {
				flut[i][k] = celula[i].calor.reyi;
				k++;
			}
			if (profg.reye == 1) {
				flut[i][k] = celula[i].calor.reye;
				k++;
			}
			if (profg.grashi == 1) {
				flut[i][k] = celula[i].calor.grashi;
				k++;
			}
			if (profg.grashe == 1) {
				flut[i][k] = celula[i].calor.grashe;
				k++;
			}
			if (profg.nusi == 1) {
				flut[i][k] = celula[i].calor.nusi;
				k++;
			}
			if (profg.nuse == 1) {
				flut[i][k] = celula[i].calor.nuse;
				k++;
			}
			if (profg.hi == 1) {
				flut[i][k] = celula[i].calor.hi;
				k++;
			}
			if (profg.he == 1) {
				flut[i][k] = celula[i].calor.he;
				k++;
			}
			if (profg.pri == 1) {
				flut[i][k] = celula[i].calor.pri;
				k++;
			}
			if (profg.pre == 1) {
				flut[i][k] = celula[i].calor.pre;
				k++;
			}
			if (profg.tAmb == 1) {
				flut[i][k] = celula[i].calor.Textern1;
				k++;
			}
			if (profg.angulo == 1) {
				flut[i][k] = celula[i].duto.teta;
				k++;
			}
			if (profg.diamInt == 1) {
				flut[i][k] = celula[i].duto.a;
				k++;
			}
			if (profg.TempParede == 1) {
				flut[i][k] = celula[i].calor.Tcamada[0][0];
				k++;
			}
			if (profg.subResfria == 1) {
				double tempH=interpolaTempEnvelope(celula[i].pres);
				flut[i][k] = celula[i].temp-tempH;
				k++;
			}
			if (unidade < nunidadeg - 1) {
				if (comp >= unidadeG[unidade + 1].Lini)
					unidade++;
			}
			flut[i][k] = unidade;
			k++;
			flut[i][k] = altura;
			k++;
			alturaC=altura;
			comp += celula[i].dx0/2.;
			//altura += celula[i].dx0 * sinl(celula[i].duto.teta)/2.;
			altura += celula[i].dx0 * sinl(celula[i].duto.teta);
			alturaC+=0.5*celula[i].dx0 * sinl(celula[i].duto.teta);
			flut[i][k] = alturaC;
			k++;
		}
		ostringstream saidaG;
		int aproxtempo = roundl(tempo);
		if (indTramo < 0 && AS==0)
			saidaG << pathPrefixoArqSaida << "PERFISG" << "-" << aproxtempo
					<< ".dat";
		else if (indTramo < 0 && AS==1)
			saidaG << pathPrefixoArqSaida << "PERFISG" <<"-seqAS-"<<(*vg1dSP).sequenciaAS << "-" << aproxtempo
			<< ".dat";
		else
			saidaG << pathPrefixoArqSaida << "Tramo" << indTramo<<"-R-"<<nrede << "-"
					<< "PERFISG" << "-" << aproxtempo << ".dat";

	      time_t hoje = time(0);
	      struct tm * agora = localtime(&hoje);
	      char buffer[80];
	      strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", agora);

	      std::string dataStr(buffer);



		string tmp = saidaG.str();
		ofstream escreveIni(tmp.c_str(), ios_base::out);
		escreveIni << "# Passo de tempo = " << (*vg1dSP).contador << " # data e hora da simulacao "<< dataStr<< " # Versao "<< versao<<"\n";
		escreveIni << " Comprimento (m) Fronteira F;";
		escreveIni << " Comprimento (m) Centro Volume C;";
		escreveIni << " comprimento_fundoPoco (m) F;";
		escreveIni << " Tempo (s) ;";
		if (profg.pres == 1)
			escreveIni << " Pressao (kgf/cm2) C;";
		if (profg.temp == 1)
			escreveIni << " Temperatura (C) C;";
		if (profg.ugs == 1)
			escreveIni << " Velocidade superficial do gas (m/s) F;";
		if (profg.ug == 1)
			escreveIni << " Velocidade do gas (m/s) F;";
		if (profg.tens == 1)
			escreveIni << " Tensao Cisalhante (N/m2) F;";
		if (profg.viscg == 1)
			escreveIni << " Viscosidade do Gas (cP) C;";
		if (profg.rhog == 1)
			escreveIni << " Massa Especifica Gas (kg/m3) C;";
		if (profg.masg == 1)
			escreveIni << " Vazao Massica do Gas (kg/s) F;";
		if (profg.hidro == 1)
			escreveIni << " Termo Hidrostatico (Pa/m) F;";
		if (profg.fric == 1)
			escreveIni << " Termo friccao (Pa/m) F;";
		if (profg.FVHG == 1)
			escreveIni << " FVHG (-) C;"; //chris - Hidratos
		if (profg.calor == 1)
			escreveIni << " Fluxo de Calor (W/m) C;";
		if (profg.qgst == 1)
			escreveIni << " Vazao volumetrica standard de gas (Sm3/(d)) F;";
		if (profg.reyi == 1)
			escreveIni << " Reynolds interno (-) F;";
		if (profg.reye == 1)
			escreveIni << " Reynolds externo (-) F;";
		if (profg.grashi == 1)
			escreveIni << " Grashof interno (-) F;";
		if (profg.grashe == 1)
			escreveIni << " Grashof externo (-) F;";
		if (profg.nusi == 1)
			escreveIni << " Nusselt interno (-) F;";
		if (profg.nuse == 1)
			escreveIni << " Nusselt externo (-) F;";
		if (profg.hi == 1)
			escreveIni << " Coeficiente de película interno (W / [m2 K]) F;";
		if (profg.he == 1)
			escreveIni << " Coeficiente de película externo (W / [m2 K]) F;";
		if (profg.pri == 1)
			escreveIni << " Prandtl interno(-) F;";
		if (profg.pre == 1)
			escreveIni << " Prandtl externo (-) F;";
		if (profg.tAmb == 1)
			escreveIni << " Temperatura Ambiente (C) C;";
		if (profg.angulo == 1)
			escreveIni << " Angulo (radiano) C;";
		if (profg.diamInt == 1)
			escreveIni << " DiametroInterno (m) C;";
		if (profg.TempParede == 1)
			escreveIni << " Temperatura Interna da Parede (C) C;";
		escreveIni << " id do duto;";
		escreveIni << " Profundidade (m) F;";
		escreveIni << " Profundidade (m) C;";
		escreveIni << "\n";
		escreveIni << flut;
		escreveIni.close();
		// caso nao seja simulacao POCO_INJETOR
		if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
			arqRelatorioPerfis << tmp.c_str() << endl;
			arqRelatorioPerfis.flush();
		}
	}
}

void Ler::resumoPermanente(Cel* const celula, CelG* const celulaG, double pGsup, double presiniG,int indTramo, int nrede){
	ostringstream saidaR;
	if (indTramo < 0 && AS==0)
		saidaR << pathPrefixoArqSaida << "resumoPermanente" << ".dat";
	else if (indTramo < 0 && AS==1)
		saidaR << pathPrefixoArqSaida << "resumoPermanente"<<"-seqAS-"<<(*vg1dSP).sequenciaAS <<".dat";
	else
		saidaR << pathPrefixoArqSaida << "Tramo" << indTramo<<"-R-"<<nrede << "-"
				<< "resumoPermanente" << ".dat";
	string tmp = saidaR.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Vazao volumetrica standard de oleo morto (Sm3/d);";
	escreveIni << " Vazao volumetrica standard de oleo morto + agua (Sm3/d);";
	escreveIni << " Vazao volumetrica standard de oleo morto + agua + liquido complementar (Sm3/d);";
	escreveIni << " Vazao volumetrica standard de gas livre + dissolvido (Sm3/d);";
	escreveIni << " P_INI_TUB (kgf/cm2);";
	escreveIni << " P_ANM (kgf/cm2);";
	escreveIni << " P_MON_CKP (kgf/cm2);";
	escreveIni << " P_JUS_CKP (kgf/cm2);";
	if(lingas==1){
		escreveIni << " Vazao volumetrica standard de gas injetado (Sm3/(d));";
		escreveIni << " Pressao de injeção a montante do choke (kgf/cm2);";
		escreveIni << " Pressao de injeção a jusante do choke (kgf/cm2);";
	}
	if(nbcs>0){
		for(int ibcs=0;ibcs<nbcs;ibcs++){
			escreveIni << " Incremento de pressao na BCS "<<ibcs<<", (kgf/cm2);";
			escreveIni << " Potência da BCS (considerando apenas eficiencia hidraulica) "<<ibcs<<", (HP);";
			escreveIni << " Potência da BCS (considerando apenas eficiencia do motor) "<<ibcs<<", (HP);";
			escreveIni << " Head da BCS "<<ibcs<<", (m);";
			escreveIni << " Vazao de oleo in situ na succao da BCS "<<ibcs<<", (m3/s);";
			escreveIni << " Vazao de oleo + agua in situ na succao da BCS "<<ibcs<<", (m3/s);";
			escreveIni << " Vazao de gas in situ na succao da BCS "<<ibcs<<", (m3/s);";
			escreveIni << " Vazao de oleo in situ na descarga da BCS "<<ibcs<<", (m3/s);";
			escreveIni << " Vazao de oleo + agua in situ na descarga da BCS "<<ibcs<<", (m3/s);";
			escreveIni << " Vazao de gas in situ na descarga da BCS "<<ibcs<<", (m3/s);";
		}
	}
	if(nintermi>0){
		for(int iintermi=0;iintermi<nintermi;iintermi++){
			/*
			 * 	double cotaAcumulo;
	double cotaColuna;
	double zMed;
	double compAcumula;
	double volumeAcumula;
	double alfMedAcumula;
	double area0;
	double tempMedAcumula;
	double RGas;
	double rhog0;
	double rhol0;
	double rholR;
	double rhogMedAcumula;
	double rholMedAcumula;
	double pMedAcumula;
	double pJus;
	double ugs0;
	double uls0;
	double massGas0;
	double massLiq0;
	double uslCol;
	double alfPenetra;
	double qualidadeArranjo;
			 */
			escreveIni << " Criterio do teste de Intermitencia Severa "<<iintermi<<" ;";
			escreveIni << " Qualidade do Arranjo estratificado para Trecho de Acumulacao "<<iintermi<<", (-);";
			escreveIni << " Fator de compressibilidade medio zona de acumulo "<<iintermi<<", (-);";
			escreveIni << " Temperatura media zona de acumulo "<<iintermi<<", (C);";
			escreveIni << " Fracao de Vazio media zona de acumulo "<<iintermi<<", (-);";
			escreveIni << " Vazao massica de gas inicio da zona de acumulo "<<iintermi<<", (kg/s);";
			escreveIni << " Vazao massica de liquido inicio da zona de acumulo "<<iintermi<<", (kg/s);";
			escreveIni << " Velocidade superficial de gas inicio da zona de acumulo "<<iintermi<<", (m/s);";
			escreveIni << " Velocidade superficial de liquido inicio da zona de acumulo "<<iintermi<<", (m/s);";
			escreveIni << " Indicador de estabilidade "<<iintermi<<", ;";
		}
	}
	escreveIni << "\n";

	escreveIni << fqlst(celula,ncelp - 1,0)<<" ; ";
	escreveIni << fqlwst(celula,ncelp - 1,0)<<" ; ";
	escreveIni << fqlstTot(celula,ncelp - 1,0)<<" ; ";
	escreveIni << fqgst(celula,ncelp - 1,0)<<" ; ";
	escreveIni << celula[0].pres<<" ; ";
	int posicM = master1.posic;
	escreveIni << celula[posicM].pres<<" ; ";
	escreveIni << celula[ncelp-1].pres<<" ; ";
	escreveIni << pGsup<<" ; ";
	if(lingas==1){
		double dgi;
		dgi = celulaG[0].flui.Deng;
		escreveIni << (celulaG[1].VGasR) * 86400 / (dgi * 1.225)<<" ; ";
		escreveIni << presiniG<<" ; ";
		escreveIni << celulaG[0].pres<<" ; ";
	}
	if(nbcs>0){
		for(int ibcs=0;ibcs<nbcs;ibcs++){
			int iposp = bcs[ibcs].posicP;
			escreveIni << celula[iposp].dpB<<" ; ";
			escreveIni << celula[iposp].potB<<" ; ";
			escreveIni << celula[iposp].potBT<<" ; ";
			escreveIni << celula[iposp].acsr.bcs.Hvis<<" ; ";
			escreveIni << celula[iposp].QL*(1-celula[iposp].FW)<<" ; ";
			escreveIni << celula[iposp].QL<<" ; ";
			escreveIni << celula[iposp].QG<<" ; ";
			escreveIni << celula[iposp+1].QL*(1-celula[iposp+1].FW)<<" ; ";
			escreveIni << celula[iposp+1].QL<<" ; ";
			escreveIni << celula[iposp+1].QG<<" ; ";

		}
	}
	if(nintermi>0){
		std::vector<string> model = {"Boe", "Taitel-Barnea"};
		critInterSev* criterioItermitencia;
		criterioItermitencia = new critInterSev[nintermi];
		for(int i=0; i<nintermi;i++){
			  criterioItermitencia[i]=critInterSev(celula, intermi[i].indInicioTrechoAcumula,
					  intermi[i].indFimTrechoAcumula,  intermi[i].indFimTrechoColuna, intermi[i].criterio,
					  intermi[i].fracaoVazioPenetracao);
			  double estabi=criterioItermitencia[i].testaCriterio(intermi[i].criterio, celula);
			  for(int iintermi=0;iintermi<nintermi;iintermi++){
					escreveIni << model[criterioItermitencia[i].criterio]<<" ; ";
					escreveIni << criterioItermitencia[i].qualidadeArranjo<<" ; ";
					escreveIni << criterioItermitencia[i].zMed<<" ; ";
					escreveIni << criterioItermitencia[i].tempMedAcumula<<" ; ";
					escreveIni << criterioItermitencia[i].alfMedAcumula<<" ; ";
					escreveIni << criterioItermitencia[i].massGas0<<" ; ";
					escreveIni << criterioItermitencia[i].massLiq0<<" ; ";
					escreveIni << criterioItermitencia[i].ugs0<<" ; ";
					escreveIni << criterioItermitencia[i].uls0<<" ; ";
					if(estabi>0) escreveIni << "estavel"<<" ; ";
					else escreveIni << "instavel"<<" ; ";
			  }
		}
		delete [] criterioItermitencia;
	}
	escreveIni.close();
}

void Ler::imprimeProfileTrans(
		Cel* const celula, int* length, double tempo,
		int indTramo, int nrede) {
	if (nperfistransp > 0) {
		for (int i = 0; i < nperfistransp; i++) {
			int n = length[i];
			int posicn = proftransp.posic[i];
			double comprimento = 0;
			for (int k = 0; k <= posicn; k++)
				comprimento += celp[k].dx;
			FullMtx<double> saida(n, 4);
			saida = celula[posicn].calor.perfil();
			ostringstream saidaP;
			int numero = roundl(tempo);
			if (indTramo < 0)
				saidaP << pathPrefixoArqSaida << "PERFISTRANSP" << "-" << numero
						<< "-" << proftransp.posic[i] << ".dat";
			else
				saidaP << pathPrefixoArqSaida << "Tramo" << indTramo<<"-R-"<<nrede << "-"
						<< "PERFISTRANSP" << "-" << numero << "-"
						<< proftransp.posic[i] << ".dat";
			string tmp = saidaP.str();

			ofstream escreveIni(tmp.c_str(), ios_base::out);
			escreveIni << "# Passo de tempo = " << (*vg1dSP).contador << "\n";
			escreveIni << "# Tempo (s) = " << tempo << "\n";
			escreveIni << "# Comprimento a partir do Fundo de Poco(m) = "
					<< comprimento << "\n";
			escreveIni << " Raio (m) ;";
			escreveIni << " Raio (Polegada) ;";
			escreveIni << " Temperatura (C) ;";
			escreveIni << " Fluxo de Calor (W/m2) ;";
			escreveIni << "\n";
			escreveIni << saida;
			escreveIni.close();
			// caso nao seja simulacao POCO_INJETOR
			if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
				arqRelatorioPerfis << tmp.c_str() << endl;
				arqRelatorioPerfis.flush();
			}
		}
	}
}

void Ler::imprimeProfileTransG(
		CelG* const celula, int* length, double tempo,
		int indTramo, int nrede) {
	if (nperfistransg > 0) {
		for (int i = 0; i < nperfistransg; i++) {
			int posicn = proftransg.posic[i];
			double comprimento = 0;
			for (int k = 0; k <= posicn; k++)
				comprimento += celg[k].dx;
			int n = length[i];
			FullMtx<double> saida(n, 4);
			saida = celula[posicn].calor.perfil();
			ostringstream saidaG;
			int numero = roundl(tempo);
			if (indTramo < 0)
				saidaG << pathPrefixoArqSaida << "PERFISTRANSG" << "-" << numero
						<< "-" << proftransg.posic[i] << ".dat";
			else
				saidaG << pathPrefixoArqSaida << "Tramo"<<"-R-"<<nrede << indTramo << "-"
						<< "PERFISTRANSG" << "-" << numero << "-"
						<< proftransg.posic[i] << ".dat";
			string tmp = saidaG.str();
			ofstream escreveIni(tmp.c_str(), ios_base::out);
			escreveIni << "# Passo de tempo = " << (*vg1dSP).contador << "\n";
			escreveIni << "# Tempo (s) = " << tempo << "\n";
			escreveIni << "# Comprimento a partir da Plataforma (m) = "
					<< comprimento << "\n";
			escreveIni << " Raio (m) ;";
			escreveIni << " Raio (Polegada) ;";
			escreveIni << " Temperatura (C) ;";
			escreveIni << " Fluxo de Calor (W/m2) ;";
			escreveIni << "\n";
			escreveIni << saida;
			escreveIni.close();
			// caso nao seja simulacao POCO_INJETOR
			if (tipoSimulacao != tipoSimulacao_t::poco_injetor) {
				arqRelatorioPerfis << tmp.c_str() << endl;
				arqRelatorioPerfis.flush();
			}
		}
	}
}

void Ler::imprimeTrend(Cel* const celula,
		double** flut, double tempo, int trend, int linha) {
	if (ntendp > 0) {
		flut[linha][0] = tempo;
		int k = 1;

		int i = trendp[trend].posic;
		//if(tempo==0 && i==ncelp-1)i--;
		if(i==ncelp-1){
			int para;
			para=0;
		}
		if (trendp[trend].pres == 1) {
			flut[linha][k] = celula[i].pres;
			k++;
		}
		if (trendp[trend].temp == 1) {
			flut[linha][k] = celula[i].temp;
			k++;
		}
		if (trendp[trend].hol == 1) {
			flut[linha][k] = 1. - celula[i].alf;
			k++;
		}

		if (trendp[trend].FVH == 1) { //Modelagem Hidrato chris
			flut[linha][k] = celula[i].FVH;
			k++;
		}

		if (trendp[trend].bet == 1) {
			flut[linha][k] = celula[i].bet;
			k++;
		}
		if (trendp[trend].ugs == 1) {
			flut[linha][k] = celula[i].QG / celula[i].duto.area;
			k++;
		}
		if (trendp[trend].uls == 1) {
			flut[linha][k] = celula[i].QL / celula[i].duto.area;
			k++;
		}
		if (trendp[trend].ug == 1) {
			if (celula[i].alf > (*vg1dSP).localtiny)
				flut[linha][k] = celula[i].QG
						/ (celula[i].duto.area * celula[i].alf);
			else
				flut[linha][k] = 0;
			k++;
		}
		if (trendp[trend].ul == 1) {
			if (celula[i].alf < 1. - (*vg1dSP).localtiny)
				flut[linha][k] = celula[i].QL
						/ (celula[i].duto.area * (1. - celula[i].alf));
			else
				flut[linha][k] = 0;
			k++;
		}
		if (trendp[trend].arra == 1) {
			flut[linha][k] = celula[i].arranjo;
			k++;
		}
		if (trendp[trend].viscl == 1) {
			flut[linha][k] = celula[i].flui.ViscOleo(celula[i].pres,
					celula[i].temp);
			k++;
		}
		if (trendp[trend].viscg == 1) {
			flut[linha][k] = celula[i].flui.ViscGas(celula[i].pres,
					celula[i].temp);
			k++;
		}
		if (trendp[trend].rhog == 1) {
			flut[linha][k] = celula[i].flui.MasEspGas(celula[i].pres,celula[i].temp);
			k++;
		}
		if (trendp[trend].rhol == 1) {
			flut[linha][k] = (1-celula[i].bet)*celula[i].flui.MasEspLiq(celula[i].pres,celula[i].temp)+
					celula[i].fluicol.MasEspFlu(celula[i].pres,celula[i].temp)*celula[i].bet;
			k++;
		}
		if (trendp[trend].rhoMix == 1) {
			flut[linha][k] =celula[i].alf*celula[i].flui.MasEspGas(celula[i].pres,celula[i].temp)+
					(1. - celula[i].alf)*((1-celula[i].bet)*celula[i].flui.MasEspLiq(celula[i].pres,celula[i].temp)+
							celula[i].fluicol.MasEspFlu(celula[i].pres,celula[i].temp)*celula[i].bet);
			k++;
		}
		if (trendp[trend].masg == 1) {
			flut[linha][k] = celula[i].MC - celula[i].Mliqini;
			k++;
		}
		if (trendp[trend].masl == 1) {
			flut[linha][k] = celula[i].Mliqini;
			k++;
		}
		if (trendp[trend].c0 == 1) {
			flut[linha][k] = celula[i].c0;
			k++;
		}
		if (trendp[trend].ud == 1) {
			flut[linha][k] = celula[i].ud;
			k++;
		}
		if (trendp[trend].RGO == 1) {
			flut[linha][k] = celula[i].flui.RGO;
			k++;
		}
		if (trendp[trend].deng == 1) {
			flut[linha][k] = celula[i].flui.Deng;
			k++;
		}
		if (trendp[trend].yco2 == 1) {
			flut[linha][k] = celula[i].flui.yco2;
			k++;
		}
		if (trendp[trend].calor == 1) {
			flut[linha][k] = celula[i].calor.fluxIni;
			k++;
		}
		if (trendp[trend].masstrans == 1) {
			flut[linha][k] = celula[i].FonteMudaFase;
			k++;
		}
		if (trendp[trend].qlst == 1) {
			double bo;
			double ba;
			double fwi;
			double bswi;
			double beti;
			double rsi;
			double rhogst;
			double rhoast;
			double rhoost;
			double titStd;
			int m=i;
			//if(tempo<1e-15 && i==ncelp-1)m-=2;
			if (celula[trend].QL > 0. && i > 0) {
				//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
			    double razdx;
			    razdx = celula[m].dx / (celula[m].dx + celula[m].dxL);
			    double tmed=(1-razdx)*celula[m].temp+razdx*celula[m-1].temp;
			    double pmed=(1-razdx)*celula[m].pres+razdx*celula[m-1].pres;
				bo = celula[m - 1].flui.BOFunc(pmed,tmed);
				ba = celula[m - 1].flui.BAFunc(pmed,tmed);
				bswi = celula[m - 1].flui.BSW;
				//fwi=celula[i-1].FW;
				fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
				beti = celula[m - 1].bet;
				//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
				rsi = celula[m - 1].flui.RS(pmed,tmed) * (6.29 / 35.31467);
				rhogst = celula[m - 1].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m - 1].flui.API);
				rhoast = 1000 * celula[m - 1].flui.Denag;
				titStd=celula[m-1].flui.dStockTankVaporMassFraction;
			} else {
				//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].temp);
				bo = celula[m].flui.BOFunc(celula[m].pres, celula[m].temp);
				ba = celula[m].flui.BAFunc(celula[m].pres, celula[m].temp);
				bswi = celula[m].flui.BSW;
				//fwi=celula[i].FW;
				fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
				beti = celula[m].bet;
				//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
				rsi = celula[m].flui.RS(celula[m].pres, celula[m].temp)
						* (6.29 / 35.31467);
				rhogst = celula[m].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m].flui.API);
				rhoast = 1000 * celula[m].flui.Denag;
				titStd=celula[m].flui.dStockTankVaporMassFraction;
			}
			//double qostd=celula[i].Mliqini*((1-fwi)*(1.-beti)*86400/bo)/(rpi*(1-beti)+rci*beti);
			double qostd=0.;
			if(titStd<1.-1e-15)qostd= celula[m].QL * (1 - fwi) * (1. - beti) * 86400
					/ bo;
			//flut[i][k] = qostd*(1.+bswi/(1-bswi));
			//double qostd=celula[i].Mliqini*(1-beti)*86400/(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
			flut[linha][k] = qostd;
			k++;
		}
		if (trendp[trend].qlwst == 1) {
			double bo;
			double ba;
			double fwi;
			double bswi;
			double beti;
			double rsi;
			double rhogst;
			double rhoast;
			double rhoost;
			double titStd;
			int m=i;
			//if(tempo<1e-15 && i==ncelp-1)m-=2;
			if (celula[i].QL > 0. && i > 0) {
				//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
			    double razdx;
			    razdx = celula[m].dx / (celula[m].dx + celula[m].dxL);
			    double tmed=(1-razdx)*celula[m].temp+razdx*celula[m-1].temp;
			    double pmed=(1-razdx)*celula[m].pres+razdx*celula[m-1].pres;
				bo = celula[m - 1].flui.BOFunc(pmed,tmed);
				ba = celula[m - 1].flui.BAFunc(pmed,tmed);
				bswi = celula[m - 1].flui.BSW;
				//fwi=celula[i-1].FW;
				fwi = bswi*ba/ (bo + ba*bswi - bswi * bo);
				beti = celula[m - 1].bet;
				//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
				rsi = celula[m - 1].flui.RS(pmed,tmed) * (6.29 / 35.31467);
				rhogst = celula[m - 1].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m - 1].flui.API);
				rhoast = 1000 * celula[m - 1].flui.Denag;
				titStd=celula[m-1].flui.dStockTankVaporMassFraction;
			} else {
				//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].temp);
				bo = celula[m].flui.BOFunc(celula[m].pres, celula[m].temp);
				ba = celula[m].flui.BAFunc(celula[m].pres, celula[m].temp);
				bswi = celula[m].flui.BSW;
				//fwi=celula[i].FW;
				fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
				beti = celula[m].bet;
				//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
				rsi = celula[m].flui.RS(celula[m].pres, celula[m].temp)
						* (6.29 / 35.31467);
				rhogst = celula[m].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m].flui.API);
				rhoast = 1000 * celula[m].flui.Denag;
				titStd=celula[m].flui.dStockTankVaporMassFraction;
			}
			//double qostd=celula[i].Mliqini*((1-fwi)*(1.-beti)*86400/bo)/(rpi*(1-beti)+rci*beti);
			double qostd=0.;
			if(titStd<1.-1e-15)qostd = celula[m].QL * (1 - fwi) * (1. - beti) * 86400
					/ bo;
			//flut[i][k] = qostd*(1.+bswi/(1-bswi));
			//double qostd=celula[i].Mliqini*(1-beti)*86400/(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
//			flut[linha][k] = qostd;
			if (celula[m].flui.BSW < 1 - 1e-9)
				flut[linha][k] = qostd / (1 - celula[m].flui.BSW);
			else
				flut[linha][k] = celula[m].QL;
			k++;
		}
		if (trendp[trend].qlstTot == 1) {
			int m=i;
			//if(tempo<1e-15 && i==ncelp-1)m-=2;
			double bo;
			double ba;
			double fwi;
			double bswi;
			double beti;
			double rsi;
			double rhogst;
			double rhoast;
			double rhoost;
			double titStd;
			if (celula[i].QL > 0. && i > 0) {
				//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
			    double razdx;
			    razdx = celula[m].dx / (celula[m].dx + celula[m].dxL);
			    double tmed=(1-razdx)*celula[m].temp+razdx*celula[m-1].temp;
			    double pmed=(1-razdx)*celula[m].pres+razdx*celula[m-1].pres;
				bo = celula[m - 1].flui.BOFunc(pmed,tmed);
				ba = celula[m - 1].flui.BAFunc(pmed,tmed);
				bswi = celula[m - 1].flui.BSW;
				//fwi=celula[i-1].FW;
				fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
				beti = celula[m - 1].bet;
				//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
				rsi = celula[m - 1].flui.RS(pmed,tmed) * (6.29 / 35.31467);
				rhogst = celula[m - 1].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m - 1].flui.API);
				rhoast = 1000 * celula[m - 1].flui.Denag;
				titStd=celula[m-1].flui.dStockTankVaporMassFraction;
			} else {
				//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].temp);
				bo = celula[m].flui.BOFunc(celula[m].pres, celula[m].temp);
				ba = celula[m].flui.BAFunc(celula[m].pres, celula[m].temp);
				bswi = celula[m].flui.BSW;
				//fwi=celula[i].FW;
				fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
				beti = celula[m].bet;
				//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
				rsi = celula[m].flui.RS(celula[m].pres, celula[m].temp)
						* (6.29 / 35.31467);
				rhogst = celula[m].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m].flui.API);
				rhoast = 1000 * celula[m].flui.Denag;
				titStd=celula[m].flui.dStockTankVaporMassFraction;
			}
			//double qostd=celula[i].Mliqini*((1-fwi)*(1.-beti)*86400/bo)/(rpi*(1-beti)+rci*beti);
			double qostd=0.;
			if(titStd<1.-1e-15) qostd = celula[m].QL * (1 - fwi) * (1. - beti) * 86400
					/ bo;
			double qowstd = qostd;
			if (bswi < 1 - 1e-15)
				qowstd /= (1 - bswi);
			else qowstd=celula[m].QL * (1. - beti) * 86400/ba;
			double qlsC = 0.;
			if (beti > 1e-15)
				qlsC = 86400 * celula[m].QL * beti
						* celula[m - 1].fluicol.MasEspFlu(celula[m - 1].pres,
								celula[m - 1].temp)
						/ celula[m - 1].fluicol.MasEspFlu(1., 15);
			//flut[i][k] = qostd*(1.+bswi/(1-bswi));
			//double qostd=celula[i].Mliqini*(1-beti)*86400/(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
			flut[linha][k] = qowstd + qlsC;
			k++;
		}
		if (trendp[trend].qgst == 1) {
			double bo;
			double ba;
			double fwi;
			double bswi;
			double beti;
			double rsi;
			double dgi;
			double rhogst;
			double rhoast;
			double rhoost;
			double titStd;
			int m=i;
			//if(tempo<1e-15 && i==ncelp-1)m-=2;
			if (celula[i].QL > 0. && i > 0) {
				//bo= celula[i-1].flui.BOFunc(celula[i].presaux,celula[i-1].temp);
			    double razdx;
			    razdx = celula[m].dx / (celula[m].dx + celula[m].dxL);
			    double tmed=(1-razdx)*celula[m].temp+razdx*celula[m-1].temp;
			    double pmed=(1-razdx)*celula[m].pres+razdx*celula[m-1].pres;
				bo = celula[m - 1].flui.BOFunc(pmed,tmed);
				ba = celula[m - 1].flui.BAFunc(pmed,tmed);
				bswi = celula[m - 1].flui.BSW;
				//fwi=celula[i-1].FW;
				fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
				beti = celula[m - 1].bet;
				//rsi= celula[i-1].flui.RS(celula[i].presaux,celula[i-1].temp)*(6.29/35.31467);
				rsi = celula[m - 1].flui.RS(pmed,tmed) * (6.29 / 35.31467);
				dgi = celula[m - 1].flui.Deng;
				rhogst = celula[m - 1].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m - 1].flui.API);
				rhoast = 1000 * celula[m - 1].flui.Denag;
				titStd=celula[m-1].flui.dStockTankVaporMassFraction;
			} else {
				//bo= celula[i].flui.BOFunc(celula[i].presaux,celula[i].temp);
				bo = celula[m].flui.BOFunc(celula[m].pres, celula[m].temp);
				ba = celula[m].flui.BAFunc(celula[m].pres, celula[m].temp);
				bswi = celula[m].flui.BSW;
				//fwi=celula[i].FW;
				fwi = bswi*ba / (bo + ba*bswi - bswi * bo);
				beti = celula[m].bet;
				//rsi= celula[i].flui.RS(celula[i].presaux,celula[i].temp)*(6.29/35.31467);
				rsi = celula[m].flui.RS(celula[m].pres, celula[m].temp)
						* (6.29 / 35.31467);
				dgi = celula[m].flui.Deng;
				rhogst = celula[m].flui.Deng * 1.225;
				rhoost = 1000 * 141.5 / (131.5 + celula[m].flui.API);
				rhoast = 1000 * celula[m].flui.Denag;
				titStd=celula[m].flui.dStockTankVaporMassFraction;
			}
			double qostd=0.;
			if(titStd<1.-1e-15) qostd = celula[m].QL * (1 - fwi) * (1. - beti) * 86400
					/ bo;
			//double qostd=celula[i].Mliqini*(1-beti)*86400*(1-bswi)/
			//(rhogst*rsi*(1-bswi)+rhoost*(1-bswi)+rhoast*bswi);
			flut[linha][k] = qostd * rsi
					+ (celula[m].MC - celula[m].Mliqini) * 86400
							/ (dgi * 1.225);
			k++;
		}
		if (trendp[trend].api == 1) {
			flut[linha][k] = celula[i].flui.API;
			k++;
		}
		if (trendp[trend].bsw == 1) {
			flut[linha][k] = celula[i].flui.BSW;
			k++;
		}
		if (trendp[trend].hidro == 1) {
			flut[linha][k] = celula[i].termoHidro;
			k++;
		}
		if (trendp[trend].fric == 1) {
			flut[linha][k] = celula[i].termoFric;
			k++;
		}
		if (trendp[trend].dengD == 1) {
			flut[linha][k] = celula[i].flui.Deng*celula[i].flui.rDgD;
			k++;
		}
		if (trendp[trend].dengL == 1) {
			flut[linha][k] = celula[i].flui.Deng*celula[i].flui.rDgL;
			k++;
		}
		if (trendp[trend].mlFonte == 1) {
			flut[linha][k] = celula[i].fontemassLR;
			k++;
		}
		if (trendp[trend].mgFonte == 1) {
			flut[linha][k] = celula[i].fontemassGR;
			k++;
		}
		if (trendp[trend].mcFonte == 1) {
			flut[linha][k] = celula[i].fontemassCR;
			k++;
		}
		if (trendp[trend].dpB == 1) {
			flut[linha][k] = celula[i].dpB/98066.5;
			k++;
		}
		if (trendp[trend].potB == 1) {
			flut[linha][k] = celula[i].potB/1000.;
			k++;
		}
		if (trendp[trend].tempChokeJus == 1) {
			flut[linha][k] = valTempChokeJus;
			k++;
		}
		if (trendp[trend].reyi == 1) {
			flut[linha][k] = celula[i].calor.reyi;
			k++;
		}
		if (trendp[trend].reye == 1) {
			flut[linha][k] = celula[i].calor.reye;
			k++;
		}
		if (trendp[trend].Fr == 1) {
			flut[linha][k] =((celula[i].QG + celula[i].QL)/ celula[i].duto.area)/sqrtl(9.81*celula[i].duto.dia);
			k++;
		}
		if (trendp[trend].grashi == 1) {
			flut[linha][k] = celula[i].calor.grashi;
			k++;
		}
		if (trendp[trend].grashe == 1) {
			flut[linha][k] = celula[i].calor.grashe;
			k++;
		}
		if (trendp[trend].nusi == 1) {
			flut[linha][k] = celula[i].calor.nusi;
			k++;
		}
		if (trendp[trend].nuse == 1) {
			flut[linha][k] = celula[i].calor.nuse;
			k++;
		}
		if (trendp[trend].hi == 1) {
			flut[linha][k] = celula[i].calor.hi;
			k++;
		}
		if (trendp[trend].he == 1) {
			flut[linha][k] = celula[i].calor.he;
			k++;
		}
		if (trendp[trend].pri == 1) {
			flut[linha][k] = celula[i].calor.pri;
			k++;
		}
		if (trendp[trend].pre == 1) {
			flut[linha][k] = celula[i].calor.pre;
			k++;
		}
		if (trendp[trend].Rs == 1) {
			flut[linha][k] = celula[i].flui.RS(celula[i].pres,celula[i].temp)* (6.29 / 35.31467);
			k++;
		}
		if (trendp[trend].Bo == 1) {
			flut[linha][k] = celula[i].flui.BOFunc(celula[i].pres,celula[i].temp);
			k++;
		}

		if (trendp[trend].volMonM1PT == 1) {
			int posicM = master1.posic;
			double voltot=0.;
			for(int j=0; j<posicM;j++){
				voltot+=(1.-celula[j].alf)*celula[j].duto.area*celula[j].dx;
			}
			flut[linha][k] = voltot;
			k++;
		}
		if (trendp[trend].volJusM1PT == 1) {
			int posicM = master1.posic;
			double voltot=0.;
			for(int j=posicM; j<ncelp;j++){
				voltot+=(1.-celula[j].alf)*celula[j].duto.area*celula[j].dx;
			}
			flut[linha][k] = voltot;
			k++;
		}
		if (trendp[trend].volMonM1ST == 1) {
			int posicM = master1.posic;
			double voltot=0.;
			for(int j=0; j<posicM;j++){
				double pres=celula[j].pres;
				double temp=celula[j].temp;
				double bo=celula[j].flui.BOFunc(pres, temp);
				double ba=celula[j].flui.BAFunc(pres, temp);
				double fw=celula[j].FW;
				double beta=celula[j].bet;
				double rhocPT=celula[j].fluicol.MasEspFlu(pres, temp);
				double rhocST=celula[j].fluicol.rholStd;
				double hol=(1.-celula[j].alf);
				double vop=(1-beta)*(1-fw)*hol/bo;
				double voc=beta*hol*rhocPT/rhocST;
				double va=(1-beta)*(fw)*hol/ba;
				voltot+=(vop+voc+va)*celula[j].duto.area*celula[j].dx;
			}
			flut[linha][k] = voltot;
			k++;
		}
		if (trendp[trend].volJusM1ST == 1) {
			int posicM = master1.posic;
			double voltot=0.;
			for(int j=posicM; j<ncelp;j++){
				double pres=celula[j].pres;
				double temp=celula[j].temp;
				double bo=celula[j].flui.BOFunc(pres, temp);
				double ba=celula[j].flui.BAFunc(pres, temp);
				double fw=celula[j].FW;
				double beta=celula[j].bet;
				double rhocPT=celula[j].fluicol.MasEspFlu(pres, temp);
				double rhocST=celula[j].fluicol.rholStd;
				double hol=(1.-celula[j].alf);
				double vop=(1-beta)*(1-fw)*hol/bo;
				double voc=beta*hol*rhocPT/rhocST;
				double va=(1-beta)*(fw)*hol/ba;
				voltot+=(vop+voc+va)*celula[j].duto.area*celula[j].dx;
			}
			flut[linha][k] = voltot;
			k++;
		}
		if (trendp[trend].inventarioGas == 1) {
			double voltot=0.;
			for(int j=0; j<ncelp-1;j++){
				double pres=celula[j].pres;
				double temp=celula[j].temp;
				double bo=celula[j].flui.BOFunc(pres, temp);
				double ba=celula[j].flui.BAFunc(pres, temp);
				double fw=celula[j].FW;
				double beta=celula[j].bet;
				double tituloST=celula[j].flui.dStockTankVaporMassFraction;
				double rhogPT=celula[j].flui.MasEspGas(pres, temp);
				double rhogST=celula[j].flui.Deng*1.225;
				double rhooPT=celula[j].flui.MasEspoleo(pres, temp);
				double rhoaPT=celula[j].flui.MasEspAgua(pres, temp);
				double rhocPT=celula[j].fluicol.MasEspFlu(pres, temp);
				double rhocST=celula[j].fluicol.rholStd;
				double hol=(1.-celula[j].alf);
				double massg=(1.-hol)*rhogPT*celula[j].duto.area*celula[j].dx;
				double masspO=(1.-beta)*hol*(1.-fw)*rhooPT*celula[j].duto.area*celula[j].dx;
				double masspA=(1.-beta)*hol*fw*rhoaPT*celula[j].duto.area*celula[j].dx;
				double massc=beta*hol*rhocPT*celula[j].duto.area*celula[j].dx;
				double vg=(massg+masspO)*tituloST/rhogST;
				voltot+=vg;
			}
			flut[linha][k] = voltot;
			k++;
		}
		if (trendp[trend].inventarioLiq == 1) {
			double voltot=0.;
			for(int j=0; j<ncelp-1;j++){
				double pres=celula[j].pres;
				double temp=celula[j].temp;
				double bo=celula[j].flui.BOFunc(pres, temp);
				double ba=celula[j].flui.BAFunc(pres, temp);
				double fw=celula[j].FW;
				double beta=celula[j].bet;
				double tituloST=celula[j].flui.dStockTankVaporMassFraction;
				double rhogPT=celula[j].flui.MasEspGas(pres, temp);
				double rhogST=celula[j].flui.Deng*1.225;
				double rhooPT=celula[j].flui.MasEspoleo(pres, temp);
				double rhooST=celula[j].flui.MasEspoleo(1.01, 20.);
				double rhoaPT=celula[j].flui.MasEspAgua(pres, temp);
				double rhoaST=celula[j].flui.MasEspAgua(1.01, 20.);
				double rhocPT=celula[j].fluicol.MasEspFlu(pres, temp);
				double rhocST=celula[j].fluicol.rholStd;
				double hol=(1.-celula[j].alf);
				double massg=(1.-hol)*rhogPT*celula[j].duto.area*celula[j].dx;
				double masspO=(1.-beta)*hol*(1.-fw)*rhooPT*celula[j].duto.area*celula[j].dx;
				double masspA=(1.-beta)*hol*fw*rhoaPT*celula[j].duto.area*celula[j].dx;
				double massc=beta*hol*rhocPT*celula[j].duto.area*celula[j].dx;
				double vl=(massg+masspO)*(1.-tituloST)/rhooST+masspA/rhoaST+massc/rhocST;
				voltot+=vl;
			}
			flut[linha][k] = voltot;
			k++;
		}

		if (trendp[trend].diamInt == 1) {
			flut[linha][k] = celula[i].duto.a;
			k++;
		}

		if (trendp[trend].TempParede == 1) {
			flut[linha][k] = celula[i].calor.Tcamada[0][0];
			k++;
		}

		if (trendp[trend].subResfria == 1) {
			double tempH=interpolaTempEnvelope(celula[i].pres);
			flut[linha][k] = celula[i].temp-tempH;
			k++;
		}

		if (trendp[trend].dadosParafina == 1) {
			double pres=celula[i].pres;
			double temp=celula[i].calor.Tcamada[0][0];
			celula[i].flui.atualizaPropParafina(pres, temp);
			flut[linha][k] = celula[i].flui.dCloudPointTOutput;
			flut[linha][k+1] = celula[i].flui.dInterpolatedCPWaxOutput;
			flut[linha][k+2] = celula[i].flui.dInterpolatedThermCondOutput;
			flut[linha][k+3] = celula[i].flui.dInterpolatedDensOutput;
			flut[linha][k+4] = celula[i].flui.dInterpolatedLiqMWOutput;
			k+=5;
		}
		if(trendp[trend].autoVal == 1){
			double som=celula[i].somVel();
			double adCeleri=celula[i].termAdSomVel();
			for(int j=0;j<3;j++){
				double val;
				if(j==2){
					double alfI;
					if(celula[i].QG>0)alfI=celula[i].alfL;
					else alfI=celula[i].alf;
					if (alfI > (*vg1dSP).localtiny)
						val = celula[i].QG
								/ (celula[i].duto.area * alfI);
					else
						val = 0;
				}
				else if(j==0)val=adCeleri+som;
				else val=adCeleri-som;
				flut[linha][k] = val;
				k++;
			}
		}
		if(trendp[trend].autoVel == 1){

			double alfI;
			if(celula[i].QG>0)alfI=celula[i].alfL;
			else alfI=celula[i].alf;

			if(alfI>1e-15 && alfI<(1.-1e-15)){
				FullMtx<double> jac(3,3);
				jac=celula[i].Jacobiana();
				Vcr<double> autvec(3);

				for(int konta1=0;konta1<3;konta1++){
					autvec=celula[i].autoVec(konta1, jac);
					for(int konta2=0; konta2<3; konta2++){
						flut[linha][k] = autvec[konta2];
						k++;
					}
				}
			}
			else{
				for(int konta1=0;konta1<3;konta1++){
					for(int konta2=0; konta2<3; konta2++){
						flut[linha][k] = -1000000;
						k++;
					}
				}
			}
		}
		if(trendp[trend].flutuacao == 1){
			double alfI;
			if(celula[i].QG>0)alfI=celula[i].alfL;
			else alfI=celula[i].alf;

			if(alfI>1e-15 && alfI<(1.-1e-15)){
				Vcr<double> flutua(3);
				flutua=celula[i].flutua();
				for(int konta1=0;konta1<3;konta1++){
					flut[linha][k] = flutua[konta1];
					k++;
				}
			}
			else{
				for(int konta1=0;konta1<3;konta1++){
					flut[linha][k] = -1000000;
					k++;
				}
			}
		}

	}
}

/*!
 * imprimeTrendG
 * \tparam celula Vetor de celulas cujos valores serao copiados para a matriz de valores das variaveis de tendencia
 * \tparam flut Matriz de valores das variaveis de tendencia
 * \tparam tempo Tempo da simulacao
 * \tparam trend Indice do vetor de variaveis de tendencia a serem controladas
 * \tparam linha Linha da matriz de valores das variaveis de tendencia
 * \tparam velgaslift Valor da velocidade erosional na garganta da valvula de gas lift
 */
void Ler::imprimeTrendG(CelG* const celula,
		double** flut, double tempo, int trend, int linha,
		double velgaslift) {  //alteracao5
	if (ntendg > 0) {
		flut[linha][0] = tempo;
		int k = 1;

		int i = trendg[trend].posic;
		if (trendg[trend].pres == 1) {
			flut[linha][k] = celula[i].pres;
			k++;
		}
		if (trendg[trend].temp == 1) {
			flut[linha][k] = celula[i].temp;
			k++;
		}
		if (trendg[trend].ugs == 1) {
			flut[linha][k] = celula[i].VGasR / celula[i].u1L;
			k++;
		}
		if (trendg[trend].ug == 1) {
			flut[linha][k] = celula[i].VGasR / celula[i].u1L;
			k++;
		}
		if (trendg[trend].tens == 1) {
			flut[linha][k] = -1;
			k++;
		}
		if (trendg[trend].viscg == 1) {
			if (i < *(celula[i].celInter))
				flut[linha][k] = celula[i].flui.ViscGas(celula[i].pres,
						celula[i].temp);
			else
				flut[linha][k] = celula[i].VisFlu(celula[i].pres,
						celula[i].temp);
			k++;
		}
		if (trendg[trend].rhog == 1) {
			flut[linha][k] = celula[i].u1L / celula[i].duto.area;
			;
			k++;
		}
		if (trendg[trend].masg == 1) {
			flut[linha][k] = celula[i].VGasR;
			k++;
		}
		if (trendg[trend].hidro == 1) {
			flut[linha][k] = celula[i].termoHidro;
			k++;
		}
		if (trendg[trend].fric == 1) {
			flut[linha][k] = celula[i].termoFric;
			k++;
		}
		if (trendg[trend].FVHG == 1) {
			flut[linha][k] = celula[i].FVHG; //chris - Hidratos
			k++;
		}
		if (trendg[trend].calor == 1) {
			flut[linha][k] = celula[i].calor.fluxIni;
			k++;
		}
		if (trendg[trend].qgst == 1) {
			double dgi;
			if (celula[i].VGasR > 0. && i > 0)
				dgi = celula[i - 1].flui.Deng;
			else
				dgi = celula[i].flui.Deng;
			//flut[linha][k] = (celula[i].VGasR) * 86400
					/// celula[i].MasEspFlu(1.0, 20.);
			flut[linha][k] = (celula[i].VGasR) * 86400
								/ (dgi * 1.225);
			k++;
		}
		if (celula[i].vgl == 1 && trendg[trend].velgarg == 1) {    //alteracao5
			flut[linha][k] = celula[i].qGarg / celula[i].areaGarg;
			k++;
		}
		if (celula[i].vgl == 1 && trendg[trend].qVGL == 1) {    //alteracao5
			flut[linha][k] = celula[i].qGarg;
			k++;
		}
		if (celula[i].vgl == 1 && trendg[trend].pEstagVGL == 1) {   //alteracao5
			flut[linha][k] = celula[i].pEstag;
			k++;
		}
		if (celula[i].vgl == 1 && trendg[trend].tEstagVGL == 1) {   //alteracao5
			flut[linha][k] = celula[i].tEstag;
			k++;
		}
		if (celula[i].vgl == 1 && trendg[trend].pGargVGL == 1) {    //alteracao5
			flut[linha][k] = celula[i].pGarg;
			k++;
		}
		if (celula[i].vgl == 1 && trendg[trend].tGargVGL == 1) {    //alteracao5
			flut[linha][k] = celula[i].tGarg;
			k++;
		}
		if (trendg[trend].reyi == 1) {
			flut[linha][k] = celula[i].calor.reyi;
			k++;
		}
		if (trendg[trend].reye == 1) {
			flut[linha][k] = celula[i].calor.reye;
			k++;
		}
		if (trendg[trend].grashi == 1) {
			flut[linha][k] = celula[i].calor.grashi;
			k++;
		}
		if (trendg[trend].grashe == 1) {
			flut[linha][k] = celula[i].calor.grashe;
			k++;
		}
		if (trendg[trend].nusi == 1) {
			flut[linha][k] = celula[i].calor.nusi;
			k++;
		}
		if (trendg[trend].nuse == 1) {
			flut[linha][k] = celula[i].calor.nuse;
			k++;
		}
		if (trendg[trend].hi == 1) {
			flut[linha][k] = celula[i].calor.hi;
			k++;
		}
		if (trendg[trend].he == 1) {
			flut[linha][k] = celula[i].calor.he;
			k++;
		}
		if (trendg[trend].pri == 1) {
			flut[linha][k] = celula[i].calor.pri;
			k++;
		}
		if (trendg[trend].pre == 1) {
			flut[linha][k] = celula[i].calor.pre;
			k++;
		}
		if (trendg[trend].diamInt == 1) {
			flut[linha][k] = celula[i].duto.a;
			k++;
		}
		if (trendg[trend].TempParede == 1) {
			flut[linha][k] = celula[i].calor.Tcamada[0][0];
			k++;
		}

		if (trendg[trend].subResfria == 1) {
			double tempH=interpolaTempEnvelope(celula[i].pres);
			flut[linha][k] = celula[i].temp-tempH;
			k++;
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Ler::copia_configuracao_inicial(Ler& arqAntigo) {

		// poco injetor
		lingas = 0;
		equilterm = 1;
		latente = 0;
		condlatente = 1;
		modelcp = 0;
		modelJTL = 0;
		tabp = 0;
		tabg = 0;
		perm = 1;
		trackRGO = 1;
		trackDeng = 1;
		corrDeng=0;
		ordperm = 1;
		transiente = 0;
		transmass = 0;
		indfluPIni = 0;
		//sem flash
		tabRSPB = 0;
		flashCompleto = 0;
		tabelaDinamica=0;
		// avancado
		mono = 1e-4;
		critcond = 0.001;

		saidaTela=0;

		escorregaPerm = 0;
		escorregaTran = 0;
		mapaArranjo = 0;

		limTransMass=10.;
		CriterioConvergPerm=0.001;
		AceleraConvergPerm=0;
		escorregamentoCelulaContorno=0;
		correcaoContracorPerm=0;
		correcaoMassaEspLiq=0;
		desligaDeriTransMassDTemp=0;
		corrigeContSep=1;
		nTcorrecaoMassaEspLiq=1;
		nTsonico=1;
		taxaDespre=0.1;
		estabCol=0;
		acopColAnulPermForte=0;

		tipoFluido=0;
		descarga=0;
		perm = 1;
		snaps = 0;
		transiente =0;
		ConContEntrada = 0;
		mudaArea=0;

		miniTabAtraso=0;
		miniTabDp=10.;
		miniTabDt=10.;

		razCompGasReves=0.;
		tempReves=-1000.;

		AS=0;
		paralelAS=0;

		nthrd=1;
		buscaFC=0.1;

		chutePerm=-1.;

		HISEP=0;

		modoXY=0;
		xProd0=0;
		yProd0=0;
		xServ0=0;
		yServ0=0;

		//reduzAtrito=0;


		// indicador do sentido de calculo do comprimento da linha de producao
		origemGeometria = origemGeometria_t::montante; // sentido plataforma para fundo-poco
		// indicador do sentido dos angulos dos dutos em relacao ao do escoamento
		sentidoGeometriaSegueEscoamento = arqAntigo.sentidoGeometriaSegueEscoamento; // angulos do duto a favor do sentido do escoament
		if(reverso==1)sentidoGeometriaSegueEscoamento=0;

		// indicador do sentido dos angulos dos dutos em relacao ao do escoamento
		saidaClassica = arqAntigo.saidaClassica; // angulos do duto a favor do sentido do escoamento
		AS = arqAntigo.AS;
		paralelAS = arqAntigo.paralelAS;
		HISEP = arqAntigo.HISEP;
		chutePerm = arqAntigo.chutePerm;
		pocinjec =arqAntigo.pocinjec;

		// caso nao seja simulacao de poco injetor
		lingas = arqAntigo.lingas;
		saidaTela = arqAntigo.saidaTela;
		equilterm =arqAntigo.equilterm;
		latente = arqAntigo.latente;
		condlatente = arqAntigo.condlatente;
		pvtsimarq = arqAntigo.pvtsimarq;
		flashCompleto =arqAntigo.flashCompleto;
		modoParafina=arqAntigo.modoParafina;
		tabelaDinamica=arqAntigo.tabelaDinamica;
		modelcp = arqAntigo.modelcp;
		modelJTL = arqAntigo.modelJTL;
		JTLiquidoSimple=arqAntigo.JTLiquidoSimple;;
		tabp = arqAntigo.tabp;
		trackRGO = arqAntigo.trackRGO;
		trackDeng = arqAntigo.trackDeng;
		corrDeng = arqAntigo.corrDeng;
		tabRSPB = arqAntigo.tabRSPB;
		pvtsimarq=arqAntigo.pvtsimarq;
		tabg =arqAntigo.tabg;
		escorregaPerm = arqAntigo.escorregaPerm;
		escorregaTran = arqAntigo.escorregaTran;
		mapaArranjo = arqAntigo.mapaArranjo;
		tipoFluido = arqAntigo.tipoFluido;
		perm = arqAntigo.perm;
		snaps = arqAntigo.snaps;
		descarga = arqAntigo.descarga;
		ordperm = arqAntigo.ordperm;
		snapshotArqIn = arqAntigo.snapshotArqIn;
		salinDescarga = arqAntigo.salinDescarga;
		compInterDesc = arqAntigo.compInterDesc;
		controDesc = arqAntigo.controDesc;
		vazDescControl =arqAntigo.vazDescControl;
		presMaxDesc =arqAntigo.presMaxDesc;
		presMinDesc =arqAntigo.presMinDesc;
		presIniDesc =arqAntigo.presIniDesc;
		presMaxDescG =arqAntigo.presMaxDescG;
		presMinDescG =arqAntigo.presMinDescG;
		presIniDescG =arqAntigo.presIniDescG;
		tempDescarga =arqAntigo.tempDescarga;
		tempoLatenciaDesc =arqAntigo.tempoLatenciaDesc;
		indfluPIni = arqAntigo.indfluPIni;
        tipoModeloDrift=arqAntigo.tipoModeloDrift;
		if((*vg1dSP).chaverede==0) (*vg1dSP).tipoModeloDrift=tipoModeloDrift;
		transiente = arqAntigo.transiente;
		if((*vg1dSP).chaverede==0) (*vg1dSP).simulaTransiente=transiente;
		transmass = arqAntigo.transmass;
		chkv = arqAntigo.chkv;
		modoDifus3D=arqAntigo.modoDifus3D;
	    threadP3D=arqAntigo.threadP3D;
	    modoDifus3DJson=arqAntigo.modoDifus3DJson;
	    mono = arqAntigo.mono;
	    critcond = arqAntigo.critcond;
	    taxaDespre=arqAntigo.taxaDespre;
	    MedSimpPresFront=arqAntigo.MedSimpPresFront;
	    RelaxaDTChoke=arqAntigo.RelaxaDTChoke;
	    desligaPenalizaDT=arqAntigo.desligaPenalizaDT;
	    controleDTvalv=arqAntigo.controleDTvalv;
	    limTransMass=arqAntigo.limTransMass;
	    (*vg1dSP).CritDTMin=(*arqAntigo.vg1dSP).CritDTMin;
	    buscaFC=arqAntigo.buscaFC;
	    CriterioConvergPerm=arqAntigo.CriterioConvergPerm;
	    AceleraConvergPerm=arqAntigo.AceleraConvergPerm;
	    escorregamentoCelulaContorno=arqAntigo.escorregamentoCelulaContorno;
	    correcaoContracorPerm=arqAntigo.correcaoContracorPerm;
	    estabCol=arqAntigo.estabCol;
	    acopColAnulPermForte=arqAntigo.acopColAnulPermForte;
	    mudaArea=arqAntigo.mudaArea;
	    nthrd=arqAntigo.nthrd;
    	nTcorrecaoMassaEspLiq =	arqAntigo.nTcorrecaoMassaEspLiq;
    	TcorrecaoMassaEspLiq = new double[nTcorrecaoMassaEspLiq];
    	VcorrecaoMassaEspLiq = new int[nTcorrecaoMassaEspLiq];
    	for(int i=0;i<nTcorrecaoMassaEspLiq;i++){
    		VcorrecaoMassaEspLiq[i]=arqAntigo.VcorrecaoMassaEspLiq[i];
    		TcorrecaoMassaEspLiq[i]=arqAntigo.TcorrecaoMassaEspLiq[i];
    	}
    	correcaoMassaEspLiq=VcorrecaoMassaEspLiq[0];
    	cicloAcopTerm=arqAntigo.cicloAcopTerm;
    	nTsonico =	arqAntigo.nTsonico;
    	Tempsonico = new double[nTsonico];
    	Vsonico = new int[nTsonico];
    	for(int i=0;i<nTsonico;i++){
    		Vsonico[i]=arqAntigo.Vsonico[i];
    		Tempsonico[i]=arqAntigo.Tempsonico[i];
    	}
    	desligaDeriTransMassDTemp=arqAntigo.desligaDeriTransMassDTemp;
	    corrigeContSep=arqAntigo.corrigeContSep;
	    miniTabAtraso=arqAntigo.miniTabAtraso;
	    miniTabDp=arqAntigo.miniTabDp;
	    miniTabDt=arqAntigo.miniTabDt;
        (*vg1dSP).ntrd=nthrd;
		(*vg1dSP).ntrd3DP=threadP3D;
		ConContEntrada = arqAntigo.ConContEntrada;
		CCPres.parserie =arqAntigo.CCPres.parserie;
		CCPres.indFluido=arqAntigo.CCPres.indFluido;
		CCPres.temperatura = new double[CCPres.parserie];
		CCPres.pres = new double[CCPres.parserie];
		CCPres.tit = new double[CCPres.parserie];
		CCPres.bet = new double[CCPres.parserie];
		CCPres.tempo = new double[CCPres.parserie];
		for (int i = 0; i < CCPres.parserie; i++)
			CCPres.temperatura[i] =arqAntigo.CCPres.temperatura[i];
		for (int i = 0; i < CCPres.parserie; i++)
			CCPres.pres[i] =arqAntigo.CCPres.pres[i];
		for (int i = 0; i < CCPres.parserie; i++)
			CCPres.tit[i] =arqAntigo.CCPres.tit[i];
		for (int i = 0; i < CCPres.parserie; i++){
				CCPres.bet[i] =arqAntigo.CCPres.bet[i];
		}
		for (int i = 0; i < CCPres.parserie; i++)
			CCPres.tempo[i] =arqAntigo.CCPres.tempo[i];
		CCVPres.parserie =arqAntigo.CCVPres.parserie;
		CCVPres.temperatura = new double[CCVPres.parserie];
		CCVPres.pres = new double[CCVPres.parserie];
		CCVPres.mass = new double[CCVPres.parserie];
		CCVPres.bet = new double[CCVPres.parserie];
		CCVPres.tempo = new double[CCVPres.parserie];
		for (int i = 0; i < CCVPres.parserie; i++)
			CCVPres.temperatura[i] =arqAntigo.CCVPres.temperatura[i];
		for (int i = 0; i < CCVPres.parserie; i++)
			CCVPres.pres[i] =arqAntigo.CCVPres.pres[i];
		for (int i = 0; i < CCVPres.parserie; i++)
			CCVPres.mass[i] =arqAntigo.CCVPres.mass[i];
		for (int i = 0; i < CCVPres.parserie; i++){
				CCVPres.bet[i] =arqAntigo.CCVPres.bet[i];
		}
		for (int i = 0; i < CCVPres.parserie; i++)
			CCVPres.tempo[i] =arqAntigo.CCVPres.tempo[i];
		CorreEstrat = arqAntigo.CorreEstrat;
		CorreDisper = arqAntigo.CorreDisper;
		CorreAnular = arqAntigo.CorreAnular;
		modoXY=arqAntigo.modoXY;
		xProd0=arqAntigo.xProd0;
		yProd0=arqAntigo.yProd0;
		xServ0=arqAntigo.xServ0;
		yServ0=arqAntigo.yServ0;
		tempReves=arqAntigo.tempReves;
		razCompGasReves=arqAntigo.razCompGasReves;
		nform =arqAntigo.nform;
		formacPoc = new detFormacao[nform];
		for (int i = 0; i < nform; i++) {  //alteracao2
			formacPoc[i].id = arqAntigo.formacPoc[i].id;
			formacPoc[i].tempo = arqAntigo.formacPoc[i].tempo;  //alteracao2
			formacPoc[i].cond =arqAntigo.formacPoc[i].cond; //alteracao2
			formacPoc[i].cp =arqAntigo.formacPoc[i].cp; //alteracao2
			formacPoc[i].rho =arqAntigo.formacPoc[i].rho; //alteracao2
		}  //alteracao2


}

void Ler::copia_condcont_pocinjec(Ler& arqAntigo) {
			condpocinj.tipoFlui = arqAntigo.condpocinj.tipoFlui;
			condpocinj.salin =arqAntigo.condpocinj.salin;
			flashCompleto=arqAntigo.flashCompleto;
			condpocinj.pvtsimarqInj=arqAntigo.condpocinj.pvtsimarqInj;
			tabp=arqAntigo.tabp;
			condpocinj.CC = arqAntigo.condpocinj.CC;
			condpocinj.tempinj = arqAntigo.condpocinj.tempinj;
			condpocinj.vazinj =arqAntigo.condpocinj.vazinj;
			condpocinj.presinj =arqAntigo.condpocinj.presinj;
			condpocinj.presfundo = arqAntigo.condpocinj.presfundo;
}


void Ler::copia_tempo(Ler& arqAntigo) {
		tfinal = arqAntigo.tfinal;
		dtmaxserie.parserie = arqAntigo.dtmaxserie.parserie;
		dtmaxserie.tempo = new double[dtmaxserie.parserie];
		dtmaxserie.valor = new double[dtmaxserie.parserie];
		nsegrega=arqAntigo.nsegrega;
		vecTSegrega = new double[nsegrega];
		vecSegrega = new int[nsegrega];
		modoSegrega=vecSegrega[0];
		nsnp=arqAntigo.nsnp;
		if(nsnp>0)tempsnp = new double[nsnp];
		else tempsnp=0;
		for (int i = 0; i < nsnp; i++) {
			tempsnp[i]  = arqAntigo.tempsnp[i];
		}

		for (int i = 0; i < dtmaxserie.parserie; i++) {
			dtmaxserie.tempo[i] = arqAntigo.dtmaxserie.tempo[i];
			dtmaxserie.valor[i] = arqAntigo.dtmaxserie.valor[i];
		}
		dtmax = dtmaxserie.valor[0];

}


void Ler::copia_materiais(Ler& arqAntigo) {
	/*
	 "material": [ { "id": 0, "ativo": true, "rotulo": "aco1", "condutividade": 55.0, "calorEspecifico": 465.0, "rho": 7833.0, "tipo": 0, "visc": 0.0 } ]
	 */
	// criar variavel para o nome da propriedade json em processo de parse
		string chaveJson("#/material");

		// obter o tamanho do elemento do Json
		nmaterial = arqAntigo.nmaterial;
		// caso o tamanho do elemento seja maior que zero
		if (nmaterial > 0) {
			// vetor de materiais
			mat = new material[nmaterial];
			for (int i = 0; i < nmaterial; i++) {
				mat[i].id = arqAntigo.mat[i].id;
				mat[i].tipo = arqAntigo.mat[i].tipo;
				mat[i].cond = arqAntigo.mat[i].cond;
				mat[i].cp = arqAntigo.mat[i].cp;
				mat[i].rho = arqAntigo.mat[i].rho;
				mat[i].visc = arqAntigo.mat[i].visc;
				mat[i].beta = arqAntigo.mat[i].beta;
			}
		}
}


void Ler::copia_corte(Ler& arqAntigo) {
		// obter o tamanho do elemento do Json
		ncorte = arqAntigo.ncorte;

		// caso o tamanho do elemento seja maior que zero
		if (ncorte > 0) {
			// vetor de seções transversais
			corte = new corteduto[ncorte];
			for(int i=0; i< ncorte; i++){
				corte[i].id = arqAntigo.corte[i].id;
				corte[i].anul = arqAntigo.corte[i].anul;
				corte[i].a =arqAntigo.corte[i].a;
				corte[i].b =arqAntigo.corte[i].b;
				corte[i].rug =arqAntigo.corte[i].rug;
				corte[i].ncam = arqAntigo.corte[i].ncam;
				corte[i].diam = new double[corte[i].ncam];
				corte[i].indmat = new int[corte[i].ncam];
				corte[i].discre = new int[corte[i].ncam];
				for (int j = 0; j < corte[i].ncam; j++){
					corte[i].diam[j] =arqAntigo.corte[i].diam[j];
					corte[i].indmat[j] =arqAntigo.corte[i].indmat[j];
					corte[i].discre[j] = arqAntigo.corte[i].discre[j];
				}

			}
		}
}


void Ler::copia_tabela(Ler& arqAntigo) {

		tabent.npont = arqAntigo.tabent.npont;
		tabent.npontB=arqAntigo.tabent.npontB;
		tabent.pmax = arqAntigo.tabent.pmax;
		tabent.pmin = arqAntigo.tabent.pmin;
		tabent.tmax = arqAntigo.tabent.tmax;
		tabent.tmin = arqAntigo.tabent.tmin;
		npontos = arqAntigo.npontos;
}


void Ler::copia_parafina(Ler& arqAntigo) {

	detalParafina.arquivo=arqAntigo.detalParafina.arquivo;
	detalParafina.poroRey=arqAntigo.detalParafina.poroRey;
	detalParafina.valRey=arqAntigo.detalParafina.valRey;
	detalParafina.C2C3=arqAntigo.detalParafina.C2C3;
	detalParafina.valC2=arqAntigo.detalParafina.valC2;
	detalParafina.valC3=arqAntigo.detalParafina.valC3;
	detalParafina.difus=arqAntigo.detalParafina.difus;
	detalParafina.multDifus=arqAntigo.detalParafina.multDifus;
	detalParafina.rug=arqAntigo.detalParafina.rug;
	detalParafina.multVis=arqAntigo.detalParafina.multVis;
}


void Ler::copia_fluidos_producao(Ler& arqAntigo) {

	nfluP = arqAntigo.nfluP;
	if (nfluP > 0) {
		flup = new ProFlu [nfluP];
		indFlup=new int [nfluP];
		tabVisc= new detTabVisc[nfluP];
		for(int iflu=0;iflu<nfluP;iflu++){
			tabVisc[iflu].parserie=0;
			tabVisc[iflu].temp=0;
			tabVisc[iflu].visc=0;
			indFlup[iflu]=0;
		}

		nvecEmul = arqAntigo.nvecEmul;
		if(nvecEmul>0){
			BSWVec = new double[nvecEmul];
			emulVec = new double[nvecEmul];
			for (int i = 0; i < nvecEmul; i++) {
				BSWVec[i] = arqAntigo.BSWVec[i];
				emulVec[i] = arqAntigo.emulVec[i];
			}
		}
		else{
			BSWVec = 0;
			emulVec = 0;
		}

		if (flashCompleto == 0) {
			//flup = new ProFlu [nfluP];
			//indFlup=new int [nfluP];
			//tabVisc= new detTabVisc[nfluP];
			for(int iflu=0;iflu<nfluP;iflu++){
				tabVisc[iflu].parserie=arqAntigo.tabVisc[iflu].parserie;
				tabVisc[iflu].temp=0;
				tabVisc[iflu].visc=0;
				if(tabVisc[iflu].parserie>0){
					tabVisc[iflu].temp=new double[tabVisc[iflu].parserie];
					tabVisc[iflu].visc=new double[tabVisc[iflu].parserie];
					for(int itab=0;itab<tabVisc[iflu].parserie;itab++){
						tabVisc[iflu].temp[itab]=arqAntigo.tabVisc[iflu].temp[itab];
						tabVisc[iflu].visc[itab]=arqAntigo.tabVisc[iflu].visc[itab];
					}
				}
				indFlup[iflu]=arqAntigo.indFlup[iflu];

				//flup[iflu] = arqAntigo.flup[iflu]; //alteracao4
				flup[iflu] = ProFlu(vg1dSP, arqAntigo.flup[iflu].API, arqAntigo.flup[iflu].RGO, arqAntigo.flup[iflu].Deng,arqAntigo.flup[iflu].BSW,
						arqAntigo.flup[iflu].Denag, arqAntigo.flup[iflu].TempL, arqAntigo.flup[iflu].LVisL, arqAntigo.flup[iflu].TempH,
						arqAntigo.flup[iflu].LVisH, arqAntigo.flup[iflu].tipoemul, arqAntigo.flup[iflu].aemul,
						arqAntigo.flup[iflu].bemul,arqAntigo.flup[iflu].PHI100, arqAntigo.flup[iflu].bswCorte, arqAntigo.flup[iflu].tab,
						arqAntigo.flup[iflu].yco2,arqAntigo.flup[iflu].corrC,arqAntigo.flup[iflu].corrSat,
						arqAntigo.flup[iflu].corrOM,arqAntigo.flup[iflu].corrOV,arqAntigo.flup[iflu].corrOS, flashCompleto,
						arqAntigo.flup[iflu].id);
				flup[iflu].zdranP = zdranP;  //alteracao2
				flup[iflu].dzdpP = dzdpP;  //alteracao2
				flup[iflu].dzdtP = dzdtP;  //alteracao2
				flup[iflu].npontos = npontos;
				flup[iflu].viscBlackOil = 1;
				flup[iflu].modelaAgua = 1;
				flup[iflu].corrDeng=corrDeng;
				flup[iflu].nvecEmul=nvecEmul;
				flup[iflu].BSWVec=BSWVec;
				flup[iflu].emulVec=emulVec;
				flup[iflu].dStockTankVaporMassFraction=flup[iflu].FracMass(1.01, 20.);
				flup[iflu].parserie=tabVisc[iflu].parserie;
				flup[iflu].viscTab=tabVisc[iflu].visc;
				flup[iflu].tempTab=tabVisc[iflu].temp;
			}
		}
		else if(flashCompleto==1){
			tabent.npont=arqAntigo.tabent.npont;
			tabent.npontB=arqAntigo.tabent.npontB;
			flash = new tabelaFlash[nfluP];
			for (int i = 0; i < nfluP; i++) {

				tabVisc[i].parserie=arqAntigo.tabVisc[i].parserie;
				tabVisc[i].temp=0;
				tabVisc[i].visc=0;
				if(tabVisc[i].parserie>0){
					tabVisc[i].temp=new double[tabVisc[i].parserie];
					tabVisc[i].visc=new double[tabVisc[i].parserie];
					for(int itab=0;itab<tabVisc[i].parserie;itab++){
						tabVisc[i].temp[itab]=arqAntigo.tabVisc[i].temp[itab];
						tabVisc[i].visc[itab]=arqAntigo.tabVisc[i].visc[itab];
					}
				}

				indFlup[i]=arqAntigo.indFlup[i];
				flash[i].arquivo =arqAntigo.flash[i].arquivo;
				flash[i].visc =arqAntigo.flash[i].visc;

				flash[i].id =arqAntigo.flash[i].id;
				flash[i].rholF = new double*[tabent.npont + 1];
				flash[i].rhogF = new double*[tabent.npont + 1];
				flash[i].DrholDpF = new double*[tabent.npont + 1];
				flash[i].DrhogDpF = new double*[tabent.npont + 1];
				flash[i].DrholDtF = new double*[tabent.npont + 1];
				flash[i].DrhogDtF = new double*[tabent.npont + 1];
				flash[i].DrhowDtF = new double*[tabent.npont + 1];
				flash[i].RSF = new double*[tabent.npont + 1];
				flash[i].cplF = new double*[tabent.npont + 1];
				flash[i].cpgF = new double*[tabent.npont + 1];
				flash[i].cpwF = new double*[tabent.npont + 1];
				flash[i].HlF = new double*[tabent.npont + 1];
				flash[i].HgF = new double*[tabent.npont + 1];
				flash[i].sigOGF = new double*[tabent.npont + 1];
				flash[i].sigWGF = new double*[tabent.npont + 1];
				flash[i].viscO = new double*[tabent.npont + 1]; //alteracao6
				flash[i].viscG = new double*[tabent.npont + 1]; //alteracao6
				flash[i].PBF = new double[tabent.npontB];
				flash[i].TBF = new double[tabent.npontB];

				for (int k = 0; k < tabent.npont + 1; k++) {
					flash[i].rholF[k] = new double[tabent.npont + 1];
					flash[i].rhogF[k] = new double[tabent.npont + 1];
					flash[i].DrholDpF[k] =
							new double[tabent.npont + 1];
					flash[i].DrhogDpF[k] =
							new double[tabent.npont + 1];
					flash[i].DrholDtF[k] =
							new double[tabent.npont + 1];
					flash[i].DrhogDtF[k] =
							new double[tabent.npont + 1];
					flash[i].DrhowDtF[k] =
							new double[tabent.npont + 1];
					flash[i].RSF[k] = new double[tabent.npont + 1];
					flash[i].cplF[k] = new double[tabent.npont + 1];
					flash[i].cpgF[k] = new double[tabent.npont + 1];
					flash[i].cpwF[k] = new double[tabent.npont + 1];
					flash[i].HlF[k] = new double[tabent.npont + 1];
					flash[i].HgF[k] = new double[tabent.npont + 1];
					flash[i].sigOGF[k] = new double[tabent.npont + 1];
					flash[i].sigWGF[k] = new double[tabent.npont + 1];
					flash[i].viscO[k] = new double[tabent.npont + 1]; //alteracao6
					flash[i].viscG[k] = new double[tabent.npont + 1]; //alteracao6
				}

				for (int k = 0; k < tabent.npont + 1; k++) {
					for (int j = 0; j < tabent.npont + 1; j++) {
						flash[i].rholF[k][j] = arqAntigo.flash[i].rholF[k][j];
						flash[i].rhogF[k][j] = arqAntigo.flash[i].rhogF[k][j];
						flash[i].DrholDpF[k][j] = arqAntigo.flash[i].DrholDpF[k][j];
						flash[i].DrhogDpF[k][j] = arqAntigo.flash[i].DrhogDpF[k][j];
						flash[i].DrholDtF[k][j] = arqAntigo.flash[i].DrholDtF[k][j];
						flash[i].DrhogDtF[k][j] = arqAntigo.flash[i].DrhogDtF[k][j];
						flash[i].DrhowDtF[k][j] = arqAntigo.flash[i].DrhowDtF[k][j];
						flash[i].RSF[k][j] = arqAntigo.flash[i].RSF[k][j];
						flash[i].cplF[k][j] = arqAntigo.flash[i].cplF[k][j];
						flash[i].cpgF[k][j] = arqAntigo.flash[i].cpgF[k][j];
						flash[i].cpwF[k][j] = arqAntigo.flash[i].cpwF[k][j];
						flash[i].HlF[k][j] = arqAntigo.flash[i].HlF[k][j];
						flash[i].HgF[k][j] = arqAntigo.flash[i].HgF[k][j];
						flash[i].sigOGF[k][j] = arqAntigo.flash[i].sigOGF[k][j];
						flash[i].sigWGF[k][j] = arqAntigo.flash[i].sigOGF[k][j];
						flash[i].viscO[k][j] = arqAntigo.flash[i].viscO[k][j];  //alteracao6
						flash[i].viscG[k][j] = arqAntigo.flash[i].viscG[k][j];  //alteracao6
					}
				}
				for (int k = 0; k < tabent.npontB; k++) {
					flash[i].PBF[k] = arqAntigo.flash[i].PBF[k];
					flash[i].TBF[k] = arqAntigo.flash[i].TBF[k];
				}
				flash[i].TwoOrThree = arqAntigo.flash[i].TwoOrThree;

				flup[i] = ProFlu(vg1dSP, arqAntigo.flup[i].API, arqAntigo.flup[i].RGO, arqAntigo.flup[i].Deng,arqAntigo.flup[i].BSW,
						arqAntigo.flup[i].Denag, arqAntigo.flup[i].TempL, arqAntigo.flup[i].LVisL, arqAntigo.flup[i].TempH,
						arqAntigo.flup[i].LVisH, arqAntigo.flup[i].tipoemul, arqAntigo.flup[i].aemul,
						arqAntigo.flup[i].bemul,arqAntigo.flup[i].PHI100, arqAntigo.flup[i].bswCorte, arqAntigo.flup[i].tab,
						arqAntigo.flup[i].yco2,arqAntigo.flup[i].corrC,arqAntigo.flup[i].corrSat,
						arqAntigo.flup[i].corrOM,arqAntigo.flup[i].corrOV,arqAntigo.flup[i].corrOS, flashCompleto,
						arqAntigo.flup[i].id);
				flup[i].indiceFlash = i;
				flup[i].viscBlackOil = flash[i].visc;
				flup[i].modelaAgua=arqAntigo.flup[i].modelaAgua;

				int ndiv = tabent.npont - 1;
				int ndivB = tabent.npontB - 1;
				flup[i].npontos=tabent.npont;
				flup[i].npontosB=tabent.npontB;
				int lacoleitura = ndiv;
				int lacoleituraB = ndivB;

				flup[i].pMinEqu=arqAntigo.flup[i].pMinEqu;
				flup[i].delpTab=arqAntigo.flup[i].delpTab;
				flup[i].pchange=arqAntigo.flup[i].pchange;
				flup[i].dpchange=arqAntigo.flup[i].dpchange;
				flup[i].ipchange=arqAntigo.flup[i].ipchange;
				flup[i].delpTab=arqAntigo.flup[i].delpTab;
				flup[i].erroP=arqAntigo.flup[i].erroP;
				flup[i].indMinEquT=arqAntigo.flup[i].indMinEquT;
				flup[i].tMinEqu=arqAntigo.flup[i].tMinEqu;
				flup[i].deltTab=arqAntigo.flup[i].deltTab;
                flup[i].tchange=arqAntigo.flup[i].tchange;
                flup[i].dtchange=arqAntigo.flup[i].dtchange;
                flup[i].itchange=arqAntigo.flup[i].itchange;
				flup[i].deltTab=arqAntigo.flup[i].deltTab;
				flup[i].erroT=arqAntigo.flup[i].erroT;

				flup[i].rhogF = flash[i].rhogF;
				flup[i].rholF = flash[i].rholF;
				flup[i].DrhogDpF = flash[i].DrhogDpF;
				flup[i].DrholDpF = flash[i].DrholDpF;
				flup[i].DrhogDtF = flash[i].DrhogDtF;
				flup[i].DrholDtF = flash[i].DrholDtF;
				flup[i].DrhowDtF = flash[i].DrhowDtF;
                flup[i].RSF = flash[i].RSF;
				flup[i].cpgF = flash[i].cpgF;
				flup[i].cplF = flash[i].cplF;
				flup[i].cpwF = flash[i].cpwF;
				flup[i].HgF = flash[i].HgF;
				flup[i].HlF = flash[i].HlF;
				flup[i].sigOGF = flash[i].sigOGF;
				flup[i].sigWGF = flash[i].sigWGF;
				flup[i].viscO = flash[i].viscO;
				flup[i].viscG = flash[i].viscG;
				flup[i].PBPVTSim = flash[i].PBF;
				flup[i].TBPVTSim = flash[i].TBF;


				flup[i].nvecEmul=nvecEmul;
				flup[i].BSWVec=BSWVec;
				flup[i].emulVec=emulVec;
				flup[i].parserie=tabVisc[i].parserie;
				flup[i].viscTab=tabVisc[i].visc;
				flup[i].tempTab=tabVisc[i].temp;

				flup[i].dStockTankVaporMassFraction=arqAntigo.flup[i].dStockTankVaporMassFraction;
			}
		}
		else{
			compDet = new composicional[nfluP];
			npseudo=arqAntigo.npseudo;
			for (int i = 0; i < nfluP; i++) {

				tabVisc[i].parserie=arqAntigo.tabVisc[i].parserie;
				tabVisc[i].temp=0;
				tabVisc[i].visc=0;
				if(tabVisc[i].parserie>0){
					tabVisc[i].temp=new double[tabVisc[i].parserie];
					tabVisc[i].visc=new double[tabVisc[i].parserie];
					for(int itab=0;itab<tabVisc[i].parserie;itab++){
						tabVisc[i].temp[itab]=arqAntigo.tabVisc[i].temp[itab];
						tabVisc[i].visc[itab]=arqAntigo.tabVisc[i].visc[itab];
					}
				}

				compDet[i].liqModel=arqAntigo.compDet[i].liqModel;

				compDet[i].fracMol = new double [npseudo];
				for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].fracMol[inpseudo]=arqAntigo.compDet[i].fracMol[inpseudo];
				compDet[i].TIndepPeneloux = new double [npseudo];
				for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].TIndepPeneloux[inpseudo]=arqAntigo.compDet[i].TIndepPeneloux[inpseudo];
				compDet[i].fatAcent = new double [npseudo];
				for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].fatAcent[inpseudo]=arqAntigo.compDet[i].fatAcent[inpseudo];
				compDet[i].masMol = new double [npseudo];
				for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].masMol[inpseudo]=arqAntigo.compDet[i].masMol[inpseudo];
				compDet[i].parac = new double [npseudo];
				for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].parac[inpseudo]=arqAntigo.compDet[i].parac[inpseudo];
				compDet[i].presCrit = new double [npseudo];
				for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].presCrit[inpseudo]=arqAntigo.compDet[i].presCrit[inpseudo];
				compDet[i].tempCrit = new double [npseudo];
				for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet[i].tempCrit[inpseudo]=arqAntigo.compDet[i].tempCrit[inpseudo];
				compDet[i].CpIGCoefs = new double [4*npseudo];
				for(int inpseudo=0;inpseudo<4*npseudo;inpseudo++)compDet[i].CpIGCoefs[inpseudo]=arqAntigo.compDet[i].CpIGCoefs[inpseudo];
				compDet[i].lij = new double [npseudo*npseudo];
				for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet[i].lij[inpseudo]=arqAntigo.compDet[i].lij[inpseudo];
				compDet[i].kij = new double [npseudo*npseudo];
				for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet[i].kij[inpseudo]=arqAntigo.compDet[i].kij[inpseudo];

				flup[i] = ProFlu(vg1dSP, arqAntigo.flup[i].API, arqAntigo.flup[i].RGO, arqAntigo.flup[i].Deng,arqAntigo.flup[i].BSW,
						arqAntigo.flup[i].Denag, arqAntigo.flup[i].TempL, arqAntigo.flup[i].LVisL, arqAntigo.flup[i].TempH,
						arqAntigo.flup[i].LVisH, arqAntigo.flup[i].tipoemul, arqAntigo.flup[i].aemul,
						arqAntigo.flup[i].bemul,arqAntigo.flup[i].PHI100, arqAntigo.flup[i].bswCorte, arqAntigo.flup[i].tab,
						arqAntigo.flup[i].yco2,arqAntigo.flup[i].corrC,arqAntigo.flup[i].corrSat,
						arqAntigo.flup[i].corrOM,arqAntigo.flup[i].corrOV,arqAntigo.flup[i].corrOS, flashCompleto,
						arqAntigo.flup[i].id,npseudo);


				flup[i].viscBlackOil = arqAntigo.flup[i].viscBlackOil;
				flup[i].modelaAgua=arqAntigo.flup[i].modelaAgua;

				flup[i].tabelaDinamica=arqAntigo.flup[i].tabelaDinamica;

				flup[i].TIndepPeneloux=compDet[i].TIndepPeneloux;
				flup[i].fatAcent=compDet[i].fatAcent;
				flup[i].masMol=compDet[i].masMol;
				flup[i].parac=compDet[i].parac;
				flup[i].presCrit=compDet[i].presCrit;
				flup[i].tempCrit=compDet[i].tempCrit;
				flup[i].CpIGCoefs=compDet[i].CpIGCoefs;
				flup[i].lij=compDet[i].lij;
				flup[i].kij=compDet[i].kij;
				flup[i].liqModel=compDet[i].liqModel;
				for(int j=0;j<flup[i].npseudo;j++)flup[i].fracMol[j]=compDet[i].fracMol[j];
				flup[i].Pmol=0.;
				for(int j=0;j<flup[i].npseudo;j++)flup[i].Pmol+=flup[i].fracMol[j]*flup[i].masMol[j];
				flup[i].atualizaPropCompStandard();

				if(tabp==1){
					flup[i].zdranP = arqAntigo.flup[i].zdranP;  //alteracao2
					flup[i].dzdpP = arqAntigo.flup[i].dzdpP;  //alteracao2
					flup[i].dzdtP = arqAntigo.flup[i].dzdtP;  //alteracao2
					flup[i].npontos = arqAntigo.flup[i].npontos;
				}

			}
		}
		if(modoParafina==1){
			npseudoWax=arqAntigo.npseudoWax;
			iPresWax=arqAntigo.iPresWax;
			iTempWax=arqAntigo.iTempWax;

    		oPressurePoints=new double [iPresWax];
    		for(int iw=0; iw<iPresWax; iw++)oPressurePoints[iw]=arqAntigo.oPressurePoints[iw];

    		oCloudPointTemperatures=new double [iPresWax];
    		for(int iw=0; iw<iPresWax; iw++)oCloudPointTemperatures[iw]=arqAntigo.oCloudPointTemperatures[iw];

    		oStructurePressures=new double [iPresWax];
    		for(int iw=0; iw<iPresWax; iw++)oStructurePressures[iw]=arqAntigo.oStructurePressures[iw];

    		oStructureTemperatures=new double [iPresWax*iTempWax];
    		for(int iw=0; iw<iPresWax*iTempWax; iw++)oStructureTemperatures[iw]=arqAntigo.oStructureTemperatures[iw];

    		oStructureDensities=new double [iPresWax*iTempWax];
    		for(int iw=0; iw<iPresWax*iTempWax; iw++)oStructureDensities[iw]=arqAntigo.oStructureDensities[iw];

    		oStructureLiqMW=new double [iPresWax*iTempWax];
    		for(int iw=0; iw<iPresWax*iTempWax; iw++)oStructureLiqMW[iw]=arqAntigo.oStructureLiqMW[iw];

    		oStructureCPWax=new double [iPresWax*iTempWax];
    		for(int iw=0; iw<iPresWax*iTempWax; iw++)oStructureCPWax[iw]=arqAntigo.oStructureCPWax[iw];

    		oStructureThermCond=new double [iPresWax*iTempWax];
    		for(int iw=0; iw<iPresWax*iTempWax; iw++)oStructureThermCond[iw]=arqAntigo.oStructureThermCond[iw];

    		oStructureWaxConcs=new double [iPresWax*iTempWax*npseudoWax];
    		for(int iw=0; iw<iPresWax*iTempWax*npseudoWax; iw++)oStructureWaxConcs[iw]=arqAntigo.oStructureWaxConcs[iw];

    		oMolecularWeightsOfWaxComponents=new double[npseudoWax];
    		for(int iw=0; iw<npseudoWax; iw++)oMolecularWeightsOfWaxComponents[iw]=arqAntigo.oMolecularWeightsOfWaxComponents[iw];

    		oLiquidDensitiesOfWaxComponents=new double[npseudoWax];
    		for(int iw=0; iw<npseudoWax; iw++)oLiquidDensitiesOfWaxComponents[iw]=arqAntigo.oLiquidDensitiesOfWaxComponents[iw];

    		oInterpolatedWaxConcs=new double[npseudoWax];
    		for(int iw=0; iw<npseudoWax; iw++)oInterpolatedWaxConcs[iw]=arqAntigo.oInterpolatedWaxConcs[iw];

    		for(int i=0;i<nfluP;i++){
    			flup[i].modoParafina=modoParafina;
    			flup[i].iPresWax=iPresWax;
    			flup[i].iTempWax=iTempWax;
    			flup[i].npseudoWax=npseudoWax;
    			flup[i].oPressurePoints=oPressurePoints;
    			flup[i].oCloudPointTemperatures=oCloudPointTemperatures;
    			flup[i].oStructurePressures=oStructurePressures;
    			flup[i].oStructureTemperatures=oStructureTemperatures;
    			flup[i].oStructureDensities=oStructureDensities;
    			flup[i].oStructureLiqMW=oStructureLiqMW;
    			flup[i].oStructureCPWax=oStructureCPWax;
    			flup[i].oStructureThermCond=oStructureThermCond;
    			flup[i].oStructureWaxConcs=oStructureWaxConcs;
    			flup[i].oMolecularWeightsOfWaxComponents=oMolecularWeightsOfWaxComponents;
    			flup[i].oLiquidDensitiesOfWaxComponents=oLiquidDensitiesOfWaxComponents;

    			flup[i].npseudoWaxOut=flup[i].npseudoWax;

    			flup[i].oMolecularWeightsOfWaxComponentsOut=new double[npseudoWax];
    			flup[i].oInterpolatedWaxConcsTDerivOutput=new double[npseudoWax];
    			flup[i].oLiquidDensitiesOfWaxComponentsOutput=new double[npseudoWax];
    			flup[i].oInterpolatedWaxConcs=new double[npseudoWax];
    			for(int j=0; j<npseudoWax;j++){
    				flup[i].oMolecularWeightsOfWaxComponentsOut[j]=flup[i].oMolecularWeightsOfWaxComponents[j];
    				flup[i].oInterpolatedWaxConcsTDerivOutput[j]=0.;
    				flup[i].oInterpolatedWaxConcs[j]=oInterpolatedWaxConcs[j];
    			}
    		}
		}
	}

}

void Ler::copia_fluido_complementar(Ler& arqAntigo) {

	fluc = ProFluCol(arqAntigo.fluc.rholStd, arqAntigo.fluc.compresP, arqAntigo.fluc.compresT, arqAntigo.fluc.tenssup,
			arqAntigo.fluc.cp, arqAntigo.fluc.cond, arqAntigo.fluc.salin, arqAntigo.fluc.TempL, arqAntigo.fluc.LVisL,
			arqAntigo.fluc.TempH,arqAntigo.fluc.LVisH,0,arqAntigo.fluc.tipoF);
	fluc.npontos = arqAntigo.fluc.npontos;
}

void Ler::copia_fluido_gas(Ler& arqAntigo) {

	//flug=arqAntigo.flug;
	flug = ProFlu(vg1dSP, arqAntigo.flug.API, arqAntigo.flug.RGO, arqAntigo.flug.Deng,arqAntigo.flug.BSW,
			arqAntigo.flug.Denag, arqAntigo.flug.TempL, arqAntigo.flug.LVisL, arqAntigo.flug.TempH,
			arqAntigo.flug.LVisH, arqAntigo.flug.tipoemul, arqAntigo.flug.aemul,
			arqAntigo.flug.bemul,arqAntigo.flug.PHI100, arqAntigo.flug.bswCorte, arqAntigo.flug.tab,
			arqAntigo.flug.yco2,arqAntigo.flug.corrC,arqAntigo.flug.corrSat,
			arqAntigo.flug.corrOM,arqAntigo.flug.corrOV,arqAntigo.flug.corrOS, flashCompleto,
			arqAntigo.flug.id,npseudo);
	flug.tab=tabg;
	flug.zdranP = zdranP;  //15-06-2018
	flug.dzdpP = dzdpP;  //15-06-2018
	flug.dzdtP = dzdtP;  //15-06-2018
	if(flug.npseudo>0)compLinServ = new double[flug.npseudo];
	for(int j=0;j<flug.npseudo;j++)flug.fracMol[j]=arqAntigo.flug.fracMol[j];
}

void Ler::copia_unidades_producao(Ler& arqAntigo) {


	unidadeP = new detalhaP[nunidadep];
	ncelp=arqAntigo.ncelp;
	nCompTotalUnidadesP=arqAntigo.nCompTotalUnidadesP;
	for (int ind = 0; ind < nunidadep; ind++) {
		unidadeP[ind].correlacaoMR2 =arqAntigo.unidadeP[ind].correlacaoMR2;
		duto[ind].ang =arqAntigo.duto[ind].ang;
		duto[ind].indcorte = arqAntigo.duto[ind].indcorte;
		duto[ind].servico = arqAntigo.duto[ind].servico;
		unidadeP[ind].ind = arqAntigo.unidadeP[ind].ind;
		unidadeP[ind].duto = arqAntigo.unidadeP[ind].duto;
		unidadeP[ind].formacDuto = arqAntigo.unidadeP[ind].formacDuto;
		unidadeP[ind].ambext = arqAntigo.unidadeP[ind].ambext;
		unidadeP[ind].lito = arqAntigo.unidadeP[ind].lito;  // 03-04-2018
		unidadeP[ind].difusTerm2D = arqAntigo.unidadeP[ind].difusTerm2D;
		unidadeP[ind].difusTerm2DJSON =arqAntigo.unidadeP[ind].difusTerm2DJSON ;
		unidadeP[ind].difusTerm3D = 0;
		unidadeP[ind].difusTerm3DFE = 0;
		unidadeP[ind].difusTerm3DAcop =0;
		unidadeP[ind].dirconv=arqAntigo.unidadeP[ind].dirconv;
		unidadeP[ind].acopcol=arqAntigo.unidadeP[ind].acopcol;
		unidadeP[ind].acopcolRede=arqAntigo.unidadeP[ind].acopcolRede;
		unidadeP[ind].ambext = arqAntigo.unidadeP[ind].ambext;

		unidadeP[ind].agrupaDiscre=arqAntigo.unidadeP[ind].agrupaDiscre;

		unidadeP[ind].dPdLHidro=arqAntigo.unidadeP[ind].dPdLHidro;
		unidadeP[ind].inddPdLHidro=arqAntigo.unidadeP[ind].inddPdLHidro;
		unidadeP[ind].dPdLFric=arqAntigo.unidadeP[ind].dPdLFric;
		unidadeP[ind].inddPdLFric=arqAntigo.unidadeP[ind].inddPdLFric;
		unidadeP[ind].dTdL=arqAntigo.unidadeP[ind].dTdL;
		unidadeP[ind].inddTdL=arqAntigo.unidadeP[ind].inddTdL;
		unidadeP[ind].ncel = arqAntigo.unidadeP[ind].ncel;
		unidadeP[ind].dx = new double[unidadeP[ind].ncel];
    	for(int j=0;j<unidadeP[ind].ncel;j++){
    		unidadeP[ind].dx[j] =arqAntigo.unidadeP[ind].dx[j];
    	}

		if (modoDifus3D==1){
			unidadeP[ind].difusTerm3D=new int[unidadeP[ind].ncel];
			unidadeP[ind].difusTerm3DFE=new double[unidadeP[ind].ncel];
			unidadeP[ind].difusTerm3DAcop =new string[unidadeP[ind].ncel];

			for(int i3=0;i3<unidadeP[ind].ncel;i3++){
				unidadeP[ind].difusTerm3D[i3]=arqAntigo.unidadeP[ind].difusTerm3D[i3];
				unidadeP[ind].difusTerm3DFE[i3]=arqAntigo.unidadeP[ind].difusTerm3DFE[i3];
				unidadeP[ind].difusTerm3DAcop[i3]=arqAntigo.unidadeP[ind].difusTerm3DAcop[i3];
			}
		}

		unidadeP[ind].comp = arqAntigo.unidadeP[ind].comp;
		unidadeP[ind].nponts =arqAntigo.unidadeP[ind].nponts;
		int npontsTeste;
		if(unidadeP[ind].nponts==1)npontsTeste=2;
		else npontsTeste=unidadeP[ind].nponts;
		unidadeP[ind].nponts=npontsTeste;
		unidadeP[ind].dxVar = new double[npontsTeste];
		unidadeP[ind].var = new double*[12];
		for (int j = 0; j < 12; j++) unidadeP[ind].var[j] = new double[npontsTeste];

		for (int j = 0; j < npontsTeste; j++) {  // alteracao2
			unidadeP[ind].dxVar[j] =arqAntigo.unidadeP[ind].dxVar[j];
			for (int k=0; k<=11; k++){
				unidadeP[ind].var[k][j]=arqAntigo.unidadeP[ind].var[k][j];
			}

		}
		unidadeP[ind].Lini =arqAntigo.unidadeP[ind].Lini;

	}
}


void Ler::copia_unidades_servico(Ler& arqAntigo) {

	if (nunidadeg > 0) {
		unidadeG = new detalhaG[nunidadeg];
		ncelg=arqAntigo.ncelg;
		nCompTotalUnidadesG = arqAntigo.nCompTotalUnidadesG;
		for (int i = 0; i < nunidadeg; i++) {
			duto[i + nunidadep].ang = arqAntigo.duto[i + nunidadep].ang;
			duto[i + nunidadep].servico = arqAntigo.duto[i + nunidadep].servico;
			duto[i + nunidadep].indcorte = arqAntigo.duto[i + nunidadep].indcorte;

			unidadeG[i].ind = arqAntigo.unidadeG[i].ind;
			unidadeG[i].duto = arqAntigo.unidadeG[i].duto;
			unidadeG[i].formacDuto = arqAntigo.unidadeG[i].formacDuto;
			unidadeG[i].ambext = arqAntigo.unidadeG[i].ambext ;
			unidadeG[i].lito = arqAntigo.unidadeG[i].lito;  // 03-04-2018

			unidadeG[i].dPdLHidro=arqAntigo.unidadeG[i].dPdLHidro;
			unidadeG[i].inddPdLHidro=arqAntigo.unidadeG[i].inddPdLHidro;
			unidadeG[i].dPdLFric=arqAntigo.unidadeG[i].dPdLFric;
			unidadeG[i].inddPdLFric=arqAntigo.unidadeG[i].inddPdLFric;
			unidadeG[i].dTdL=arqAntigo.unidadeG[i].dTdL;
			unidadeG[i].inddTdL=arqAntigo.unidadeG[i].inddTdL;

			unidadeG[i].Lini = arqAntigo.unidadeG[i].Lini;
			unidadeG[i].acopcol =arqAntigo.unidadeG[i].acopcol;
			unidadeG[i].agrupaDiscre=arqAntigo.unidadeG[i].agrupaDiscre;
			unidadeG[i].ncel=arqAntigo.unidadeG[i].ncel;

			unidadeG[i].dx = new double[unidadeG[i].ncel];
           	for(int j=0;j<unidadeG[i].ncel;j++) unidadeG[i].dx[j] =arqAntigo.unidadeG[i].dx[j];
           	unidadeG[i].comp = arqAntigo.unidadeG[i].comp;

			int npontsTeste;
			unidadeG[i].nponts=arqAntigo.unidadeG[i].nponts;
           	if(unidadeG[i].nponts==1)npontsTeste=2;
			else npontsTeste=unidadeG[i].nponts;
           	unidadeG[i].nponts=npontsTeste;
			unidadeG[i].dxVar = new double[npontsTeste];
			unidadeG[i].var = new double*[9];
			for (int j = 0; j < 9; j++)
				unidadeG[i].var[j] = new double[npontsTeste];

			for (int j = 0; j < npontsTeste; j++) {
				unidadeG[i].dxVar[j] =arqAntigo.unidadeG[i].dxVar[j];
				for (int k=0; k<9; k++) unidadeG[i].var[k][j] = arqAntigo.unidadeG[i].var[k][j];
			}
		}

	}

}

void Ler::copia_hidrato(Ler& arqAntigo) {

	existeEnvelope=arqAntigo.existeEnvelope;
	calculaEnvelope =arqAntigo.calculaEnvelope;
	nparEnvelope = arqAntigo.nparEnvelope;
	if(nparEnvelope>0){
		envelopeHidra = new double* [nparEnvelope];
		for(int i=0;i<nparEnvelope;i++) {
			envelopeHidra[i] = new double [2];
			envelopeHidra[i][0]=arqAntigo.envelopeHidra[i][0];
			envelopeHidra[i][1]=arqAntigo.envelopeHidra[i][1];
		}
	}
	if(calculaEnvelope==1){

			MMH =arqAntigo.MMH;
			MMG =arqAntigo.MMG;
			MMW =arqAntigo.MMW;
			Whamm =arqAntigo.Whamm;
			coefEsteq =arqAntigo.coefEsteq;

			rd =arqAntigo.rd;
			rp =arqAntigo.rp;

	}

}

void Ler::copia_ipr(Ler& arqAntigo) {

	nipr=arqAntigo.nipr;
	IPRS = new detIPR[nipr];
	for (int i = 0; i < nipr; i++) {
		IPRS[i].comp =arqAntigo.IPRS[i].comp;
		IPRS[i].indcel =arqAntigo.IPRS[i].indcel;
		IPRS[i].tipoIPR=arqAntigo.IPRS[i].tipoIPR;
		IPRS[i].seriep = arqAntigo.IPRS[i].seriep;
		IPRS[i].seriet = arqAntigo.IPRS[i].seriet;
		IPRS[i].serieip=arqAntigo.IPRS[i].serieip;
		IPRS[i].serieqm=arqAntigo.IPRS[i].serieqm;
		IPRS[i].seriejp =arqAntigo.IPRS[i].seriejp;
		IPRS[i].indfluP=arqAntigo.IPRS[i].indfluP;

		IPRS[i].pres = 0;
		IPRS[i].tpres =0;
		IPRS[i].temp =0;
		IPRS[i].ttemp =0;
		IPRS[i].ip=0;
		IPRS[i].tip=0;
		IPRS[i].qMax=0;
		IPRS[i].tqMax=0;
		IPRS[i].jp = 0;
		IPRS[i].tjp = 0;
		if(IPRS[i].seriep>0){
			IPRS[i].pres = new double[IPRS[i].seriep];
			IPRS[i].tpres = new double[IPRS[i].seriep];
			for (int j = 0; j < IPRS[i].seriep; j++) {
				IPRS[i].pres[j] =arqAntigo.IPRS[i].pres[j];
				IPRS[i].tpres[j] =arqAntigo.IPRS[i].tpres[j];
			}
		}
		if(IPRS[i].seriet>0){
			IPRS[i].temp = new double[IPRS[i].seriet];
			IPRS[i].ttemp = new double[IPRS[i].seriet];
			for (int j = 0; j < IPRS[i].seriet; j++) {
				IPRS[i].temp[j] =arqAntigo.IPRS[i].temp[j];
				IPRS[i].ttemp[j] =arqAntigo.IPRS[i].ttemp[j];
			}
		}
		if(IPRS[i].serieip>0){
			IPRS[i].ip = new double[IPRS[i].serieip];
			IPRS[i].tip = new double[IPRS[i].serieip];
			for (int j = 0; j < IPRS[i].serieip; j++) {
				IPRS[i].ip[j] =arqAntigo.IPRS[i].ip[j];
				IPRS[i].tip[j] =arqAntigo.IPRS[i].tip[j];
			}
		}
		if(IPRS[i].serieqm>0){
			IPRS[i].qMax = new double[IPRS[i].serieqm];
			IPRS[i].tqMax = new double[IPRS[i].serieqm];
			for (int j = 0; j < IPRS[i].serieqm; j++) {
				IPRS[i].qMax[j] =arqAntigo.IPRS[i].qMax[j];
				IPRS[i].tqMax[j] =arqAntigo.IPRS[i].tqMax[j];
			}

		}
		if(IPRS[i].seriejp >0){
			IPRS[i].jp = new double[IPRS[i].seriejp];
			IPRS[i].tjp = new double[IPRS[i].seriejp];
			for (int j = 0; j < IPRS[i].seriejp; j++) {
				IPRS[i].jp[j] = arqAntigo.IPRS[i].jp[j];
				IPRS[i].tjp[j] =arqAntigo.IPRS[i].tjp[j];
			}
		}
	}
}

void Ler::copia_separador(Ler& arqAntigo) {
	psep.parserie = arqAntigo.psep.parserie;
	if (psep.parserie > 0) {  //alteracao5
		psep.pres = new double[psep.parserie];
		psep.tempo = new double[psep.parserie];
		for (int i = 0; i < psep.parserie; i++) {
			psep.pres[i] = arqAntigo.psep.pres[i];
			psep.tempo[i] = arqAntigo.psep.tempo[i];
		}
	} else {
		psep.pres = 0;
		psep.tempo = 0;
	}
}

void Ler::copia_correcao(Ler& arqAntigo) {

	correcao.parserieHidro=arqAntigo.correcao.parserieHidro;
	correcao.parserieFric=arqAntigo.correcao.parserieFric;
	correcao.parserieDT=arqAntigo.correcao.parserieDT;
	correcao.dPdLHidro =0;
	if(correcao.parserieHidro>0){
		correcao.dPdLHidro = new double[correcao.parserieHidro];
		for (int i = 0; i < correcao.parserieHidro; i++)correcao.dPdLHidro[i] = arqAntigo.correcao.dPdLHidro[i];
	}
	correcao.dPdLFric =0;
	if(correcao.parserieFric>0){
		correcao.dPdLFric = new double[correcao.parserieFric];
		for (int i = 0; i < correcao.parserieFric; i++)correcao.dPdLFric[i] = arqAntigo.correcao.dPdLFric[i];
	}
	correcao.dTdL =0;
	if(correcao.parserieDT>0){
		correcao.dTdL = new double[correcao.parserieDT];
		for (int i = 0; i < correcao.parserieDT; i++)correcao.dTdL[i] = arqAntigo.correcao.dTdL[i];
	}
}

void Ler::copia_chokeSup(Ler& arqAntigo) {

	chokep.curvaCV=arqAntigo.chokep.curvaCV;
	chokep.curvaDinamic=arqAntigo.chokep.curvaDinamic;
	chokep.ncv=arqAntigo.chokep.ncv;
	chokep.ncv =arqAntigo.chokep.ncv;
	if(chokep.ncv>0){
		chokep.cvCurv= new detCV [chokep.ncv];
		for(int konta=0;konta<chokep.ncv;konta++){
			chokep.cvCurv[konta].x1=arqAntigo.chokep.cvCurv[konta].x1;
			chokep.cvCurv[konta].x2=arqAntigo.chokep.cvCurv[konta].x2;
			chokep.cvCurv[konta].cv1=arqAntigo.chokep.cvCurv[konta].cv1;
			chokep.cvCurv[konta].cv2=arqAntigo.chokep.cvCurv[konta].cv2;
		}
	}
	else chokep.cvCurv= 0;

	chokep.parserie = arqAntigo.chokep.parserie;
	if(chokep.parserie>0){
		chokep.abertura = new double[chokep.parserie];
		chokep.tempo = new double[chokep.parserie];
		chokep.cd = arqAntigo.chokep.cd;
		chokep.modelo = arqAntigo.chokep.modelo;

		for (int i = 0; i < chokep.parserie; i++) {
			chokep.abertura[i] = arqAntigo.chokep.abertura[i];
			chokep.tempo[i] = arqAntigo.chokep.tempo[i];
		}
	}
	else{
		chokep.abertura = 0;
		chokep.tempo = 0;
		chokep.cd = 0;
		chokep.modelo = 0;
	}
}

void Ler::copia_chokeInj(Ler& arqAntigo) {
	chokes.curvaCV=arqAntigo.chokes.curvaCV;
	chokes.ncv=arqAntigo.chokes.ncv;
	chokes.parserie = arqAntigo.chokes.parserie;
	if(chokes.parserie>0){
		chokes.abertura = new double[chokes.parserie];
		chokes.tempo = new double[chokes.parserie];
		chokes.cd =arqAntigo.chokes.cd;
		chokes.modelo = arqAntigo.chokes.modelo;

		for (int i = 0; i < chokes.parserie; i++) {
			chokes.abertura[i] =arqAntigo.chokes.abertura[i];
			chokes.tempo[i] = arqAntigo.chokes.tempo[i];
		}
	}
	else{
		chokes.abertura = 0;
		chokes.tempo = 0;
		chokes.cd =0;
		chokes.modelo = 0;
	}
}

void Ler::copia_master1(Ler& arqAntigo) {

	master1.curvaCV=arqAntigo.master1.curvaCV;
	master1.ncv=arqAntigo.master1.ncv;
	master1.razareaativ = arqAntigo.master1.razareaativ ;
	master1.comp = arqAntigo.master1.comp;
	master1.posic = arqAntigo.master1.posic;

	if(master1.curvaCV==1){
		master1.cvCurv= new detCV [master1.ncv];
		for(int konta=0;konta<master1.ncv;konta++){
			master1.cvCurv[konta].x1=arqAntigo.master1.cvCurv[konta].x1;
			master1.cvCurv[konta].x2=arqAntigo.master1.cvCurv[konta].x2;
			master1.cvCurv[konta].cv1=arqAntigo.master1.cvCurv[konta].cv1;
			master1.cvCurv[konta].cv2=arqAntigo.master1.cvCurv[konta].cv2;
		}
	}

	master1.parserie = arqAntigo.master1.parserie;
	master1.abertura = new double[master1.parserie];
	master1.tempo = new double[master1.parserie];
	for (int i = 0; i < master1.parserie; i++) {
		master1.abertura[i] = arqAntigo.master1.abertura[i];
		master1.tempo[i] = arqAntigo.master1.tempo[i];
	}

	eventoabre = 0;
	eventofecha = 0;
	Tevento = new double[master1.parserie];
	Teventof = new double[master1.parserie];
}

void Ler::copia_master2(Ler& arqAntigo) {

	master2.curvaCV=arqAntigo.master2.curvaCV;
	master2.ncv=arqAntigo.master2.ncv;
	master2.parserie = arqAntigo.master2.parserie;
	if(master2.parserie>0){
		master2.abertura = new double[master2.parserie];
		master2.tempo = new double[master2.parserie];
		master2.comp =arqAntigo.master2.comp;
		master2.posic = arqAntigo.master2.posic;

		for (int i = 0; i < master2.parserie; i++) {
			master2.abertura[i] =arqAntigo.master2.abertura[i];
			master2.tempo[i] = arqAntigo.master2.tempo[i];
		}
	}
}

void Ler::copia_valv(Ler& arqAntigo) {

	nvalv=arqAntigo.nvalv;
	if(nvalv>0){
		valv = new detValv[nvalv];
		for(int i=0;i<nvalv;i++){
			valv[i].curvaCV=arqAntigo.valv[i].curvaCV;
			valv[i].curvaDinamic=arqAntigo.valv[i].curvaDinamic;
			valv[i].ncv=arqAntigo.valv[i].ncv;
			if(valv[i].curvaCV==1){
				valv[i].cvCurv= new detCV [valv[i].ncv];
				for(int konta=0;konta<valv[i].ncv;konta++){
					valv[i].cvCurv[konta].x1=arqAntigo.valv[i].cvCurv[konta].x1;
					valv[i].cvCurv[konta].x2=arqAntigo.valv[i].cvCurv[konta].x2;
					valv[i].cvCurv[konta].cv1=arqAntigo.valv[i].cvCurv[konta].cv1;
					valv[i].cvCurv[konta].cv2=arqAntigo.valv[i].cvCurv[konta].cv2;
				}
			}
			valv[i].comp =arqAntigo.valv[i].comp;
			valv[i].parserie =arqAntigo.valv[i].parserie;
			valv[i].abertura = new double[valv[i].parserie];
			valv[i].tempo = new double[valv[i].parserie];
			valv[i].cd=arqAntigo.valv[i].cd;
			// parse does vetores
			for (int j = 0; j < valv[i].parserie; j++) {
				valv[i].abertura[j] =arqAntigo.valv[i].abertura[j];
				valv[i].tempo[j] =arqAntigo.valv[i].tempo[j];
			}
		}
	}
}

void Ler::copia_gasInj(Ler& arqAntigo) {

	if(lingas==1){
			gasinj.tipoCC = arqAntigo.gasinj.tipoCC;
			gasinj.parserie = arqAntigo.gasinj.parserie;
			gasinj.temperatura = new double[gasinj.parserie];
			gasinj.tempo = new double[gasinj.parserie];
			for (int i = 0; i < gasinj.parserie; i++) {
				gasinj.temperatura[i] =arqAntigo.gasinj.temperatura[i];
				gasinj.tempo[i] = arqAntigo.gasinj.tempo[i];
			}
			if (gasinj.tipoCC == 1) {
				gasinj.vazgas = new double[gasinj.parserie];
				for (int i = 0; i < gasinj.parserie; i++) {
					gasinj.vazgas[i] =arqAntigo.gasinj.vazgas[i];
				}
			}
			else if (gasinj.tipoCC == 0) {
				gasinj.presinj = new double[gasinj.parserie];
				for (int i = 0; i < gasinj.parserie; i++) {
					gasinj.presinj[i] =arqAntigo.gasinj.presinj[i];
				}

				gasinj.chuteVaz = arqAntigo.gasinj.chuteVaz;
				gasinj.vazgas = new double[1];
				gasinj.vazgas[0] =arqAntigo.gasinj.vazgas[0];
			}
	}
}

void Ler::copia_fonte_gas(Ler& arqAntigo) {

	ninjgas = arqAntigo.ninjgas;
	if (ninjgas > 0) {
		fonteg = new detFONGAS[ninjgas];
		for (int i = 0; i < ninjgas; i++) {
			fonteg[i].comp =arqAntigo.fonteg[i].comp;
			fonteg[i].posicP = arqAntigo.fonteg[i].posicP;
			fonteg[i].parserie =arqAntigo.fonteg[i].parserie;
			fonteg[i].temp = new double[fonteg[i].parserie];
			fonteg[i].vazgas = new double[fonteg[i].parserie];
			fonteg[i].vazcomp = new double[fonteg[i].parserie];
			fonteg[i].tempo = new double[fonteg[i].parserie];
			fonteg[i].seco=arqAntigo.fonteg[i].seco;
			if(fonteg[i].seco==0){
				fonteg[i].indfluP =arqAntigo.fonteg[i].indfluP;
			}
			for (int j = 0; j < fonteg[i].parserie; j++) {
				fonteg[i].temp[j] =arqAntigo.fonteg[i].temp[j];
				fonteg[i].vazgas[j] =arqAntigo.fonteg[i].vazgas[j];
				fonteg[i].tempo[j] =arqAntigo.fonteg[i].tempo[j];
				fonteg[i].vazcomp[j] =arqAntigo.fonteg[i].vazcomp[j];
			}
		}
	}
}

void Ler::copia_fonte_liquido(Ler& arqAntigo) {

	ninjliq = arqAntigo.ninjliq;
	if (ninjliq > 0) {
		fontel = new detFONLIQ[ninjliq];
		for(int i=0; i<ninjliq;i++){
			fontel[i].comp = arqAntigo.fontel[i].comp;
			fontel[i].posicP = arqAntigo.fontel[i].posicP;
			fontel[i].indfluP = arqAntigo.fontel[i].indfluP;
			fontel[i].parserie = arqAntigo.fontel[i].parserie;
			fontel[i].temp = new double[fontel[i].parserie];
			fontel[i].bet = new double[fontel[i].parserie];
			fontel[i].vazliq = new double[fontel[i].parserie];
			fontel[i].tempo = new double[fontel[i].parserie];
			for (int j = 0; j < fontel[i].parserie; j++) {
				fontel[i].temp[j] =arqAntigo.fontel[i].temp[j];
				fontel[i].bet[j] =arqAntigo.fontel[i].bet[j];
				fontel[i].vazliq[j] =arqAntigo.fontel[i].vazliq[j];
				fontel[i].tempo[j] =arqAntigo.fontel[i].tempo[j];
			}
		}
	}
}

void Ler::copia_fonte_PoroRadial(Ler& arqAntigo) {
	nPoroRad = arqAntigo.nPoroRad;
	if (nPoroRad > 0) {
		porosoRad = new detPoroRad[nPoroRad];
		for (int i = 0; i < nPoroRad; i++) {
			porosoRad[i].comp =arqAntigo.porosoRad[i].comp;
			porosoRad[i].posicP =arqAntigo.porosoRad[i].posicP;
			porosoRad[i].nomeArquivoEntrada =arqAntigo.porosoRad[i].nomeArquivoEntrada;

		}
	}
}

void Ler::copia_fonte_Poro2D(Ler& arqAntigo) {

	nPoro2D = arqAntigo.nPoro2D;
	if (nPoro2D > 0) {
		poroso2D = new detPoro2D[nPoro2D];
		for (int i = 0; i < nPoro2D; i++) {
			poroso2D[i].comp =arqAntigo.poroso2D[i].comp;
			poroso2D[i].posicP =arqAntigo.poroso2D[i].posicP;
			poroso2D[i].nomeArquivoEntrada =arqAntigo.poroso2D[i].nomeArquivoEntrada;
		}
	}
}

void Ler::copia_fonte_massa(Ler& arqAntigo) {

	ninjmass = arqAntigo.ninjmass;
	if (ninjmass > 0) {
		fontem = new detFONMASS[ninjmass];
		for (int i = 0; i < ninjmass; i++) {
			fontem[i].condiTermo=arqAntigo.fontem[i].condiTermo;
			fontem[i].comp =arqAntigo.fontem[i].comp;
			fontem[i].posicP =arqAntigo.fontem[i].posicP;
			indicaBeta=arqAntigo.indicaBeta;
			fontem[i].indfluP =arqAntigo.fontem[i].indfluP;

			fontem[i].parserie =arqAntigo.fontem[i].parserie;
			fontem[i].temp = new double[fontem[i].parserie];
			fontem[i].vazMasT = new double[fontem[i].parserie];
			fontem[i].vazMasC = new double[fontem[i].parserie];
			fontem[i].vazMasG = new double[fontem[i].parserie];
			fontem[i].tempo = new double[fontem[i].parserie];

			for (int j = 0; j < fontem[i].parserie; j++) {
				fontem[i].temp[j] =arqAntigo.fontem[i].temp[j];
				fontem[i].vazMasT[j] =arqAntigo.fontem[i].vazMasT[j];
				fontem[i].vazMasC[j] =arqAntigo.fontem[i].vazMasC[j];
				fontem[i].vazMasG[j] =arqAntigo.fontem[i].vazMasG[j];
				fontem[i].tempo[j] =arqAntigo.fontem[i].tempo[j];
			}
		}
	}
}

void Ler::copia_furo(Ler& arqAntigo) {

	nfuro = arqAntigo.nfuro;
	if (nfuro > 0) {
		for (int i = 0; i < nfuro; i++) {
			furo[i].comp =arqAntigo.furo[i].comp;
			furo[i].posicP =arqAntigo.furo[i].posicP;
			furo[i].TipoAbertura=arqAntigo.furo[i].TipoAbertura;
			furo[i].beta=arqAntigo.furo[i].beta;
			furo[i].titAmb=arqAntigo.furo[i].titAmb;
			furo[i].parserie = arqAntigo.furo[i].parserie;
			furo[i].abertura = new double[furo[i].parserie];
			furo[i].tempo = new double[furo[i].parserie];
			for (int j = 0; j < furo[i].parserie; j++) {
				furo[i].abertura[j] = arqAntigo.furo[i].abertura[j];
				furo[i].tempo[j] = arqAntigo.furo[i].tempo[j];
			}

			furo[i].parserieChk = arqAntigo.furo[i].parserieChk;
			furo[i].check = new int[furo[i].parserieChk];
			furo[i].tempoChk = new double[furo[i].parserieChk];
			for (int j = 0; j < furo[i].parserieChk; j++) {
				furo[i].check[j] = arqAntigo.furo[i].check[j];
				furo[i].tempoChk[j] = arqAntigo.furo[i].tempoChk[j];
			}

			furo[i].temp=arqAntigo.furo[i].temp;
			furo[i].pres=arqAntigo.furo[i].pres;
			furo[i].cd=arqAntigo.furo[i].cd;
			furo[i].ambGas=arqAntigo.furo[i].ambGas;
			furo[i].tipoFlu =arqAntigo.furo[i].tipoFlu;
			furo[i].indFlu = arqAntigo.furo[i].indFlu;
		}
	}
}

void Ler::copia_fontechk(Ler& arqAntigo) {


	nfontechk = arqAntigo.nfontechk;
	if (nfontechk > 0) {
		fontechk = new detCHOKESUP[nfontechk];
		for (int i = 0; i < nfontechk; i++) {
				fontechk[i].curvaCV=arqAntigo.fontechk[i].curvaCV;
				fontechk[i].parserie = arqAntigo.fontechk[i].parserie;
				fontechk[i].abertura = new double[fontechk[i].parserie];
				fontechk[i].tempo = new double[fontechk[i].parserie];
				fontechk[i].cd =arqAntigo.fontechk[i].cd;
				fontechk[i].modelo = arqAntigo.fontechk[i].modelo;

				for (int j = 0; j < fontechk[i].parserie; j++) {
						fontechk[i].abertura[j] =arqAntigo.fontechk[i].abertura[j];
						fontechk[i].tempo[j] =arqAntigo.fontechk[i].tempo[j];
				}
		}
	}
}


void Ler::copia_pig(Ler& arqAntigo) {

	npig = arqAntigo.npig;
	if (npig > 0) {
		pig = new detPig[npig];
		for (int i = 0; i < npig; i++) {
			pig[i].compL = arqAntigo.pig[i].compL;
			pig[i].compR = arqAntigo.pig[i].compR;
			pig[i].tempo = arqAntigo.pig[i].tempo;
			pig[i].lanca = arqAntigo.pig[i].lanca;
			pig[i].receb = arqAntigo.pig[i].receb;
			pig[i].razarea = arqAntigo.pig[i].razarea;
			pig[i].delpres = arqAntigo.pig[i].delpres;
			pig[i].cdPig = arqAntigo.pig[i].cdPig;
		}
	}
}


void Ler::copia_bcs(Ler& arqAntigo) {

	nbcs = arqAntigo.nbcs;
	if (nbcs > 0) {
		bcs = new detBCS[nbcs];
		for (int i = 0; i < nbcs; i++) {
			bcs[i].comp = arqAntigo.bcs[i].comp;
			bcs[i].posicP = arqAntigo.bcs[i].posicP;
			bcs[i].parserie = arqAntigo.bcs[i].parserie;
			bcs[i].tempo = new double[bcs[i].parserie];
			bcs[i].freq = new double[bcs[i].parserie];
			for (int j = 0; j < bcs[i].parserie; j++) {
				bcs[i].tempo[j] =arqAntigo.bcs[i].tempo[j];
				bcs[i].freq[j] =arqAntigo.bcs[i].freq[j];
			}

			bcs[i].ncurva = arqAntigo.bcs[i].ncurva;
			bcs[i].vaz = new double[bcs[i].ncurva];
			bcs[i].head = new double[bcs[i].ncurva];
			bcs[i].power = new double[bcs[i].ncurva];
			bcs[i].efic = new double[bcs[i].ncurva];
			bcs[i].correcHI=arqAntigo.bcs[i].correcHI;
			for (int j = 0; j < bcs[i].ncurva; j++) {
				bcs[i].vaz[j] =arqAntigo.bcs[i].vaz[j];
				bcs[i].head[j] =arqAntigo.bcs[i].head[j];
				bcs[i].power[j] =arqAntigo.bcs[i].power[j];
				bcs[i].efic[j] =arqAntigo.bcs[i].efic[j];
			}
			bcs[i].freqref = arqAntigo.bcs[i].freqref;
			bcs[i].nestag = arqAntigo.bcs[i].nestag;
			bcs[i].eficM = arqAntigo.bcs[i].eficM;
			bcs[i].freqMinima = arqAntigo.bcs[i].freqMinima;
		}
	}
}


void Ler::copia_bomba_volumetrica(Ler& arqAntigo) {

	nbvol = arqAntigo.nbvol;
	if (nbvol > 0) {
		bvol = new detBVOL[nbvol];
		for (int i = 0; i < nbvol; i++) {
			bvol[i].comp = arqAntigo.bvol[i].comp;
			bvol[i].posicP = arqAntigo.bvol[i].posicP;
			bvol[i].parserie = arqAntigo.bvol[i].parserie;

			bvol[i].tempo = new double[bvol[i].parserie];
			bvol[i].freq = new double[bvol[i].parserie];
			for (int j = 0; j < bvol[i].parserie; j++) {
				bvol[i].tempo[j] = arqAntigo.bvol[i].tempo[j];
				bvol[i].freq[j] = arqAntigo.bvol[i].freq[j];
			}
			bvol[i].capacidade = arqAntigo.bvol[i].capacidade;
			bvol[i].npoli = arqAntigo.bvol[i].npoli;
		}
	}
}

void Ler::copia_delta_pressao(Ler& arqAntigo) {

	ndpreq = arqAntigo.ndpreq;
	if (ndpreq > 0) {
		dpreq = new detDPREQ[ndpreq];
		for (int i = 0; i < ndpreq; i++) {
			dpreq[i].comp = arqAntigo.dpreq[i].comp;
			dpreq[i].posicP = arqAntigo.dpreq[i].posicP;
			dpreq[i].tipoCompGas = arqAntigo.dpreq[i].tipoCompGas;
			dpreq[i].fatPoli = arqAntigo.dpreq[i].fatPoli;
			dpreq[i].eficLiq = arqAntigo.dpreq[i].eficLiq;
			dpreq[i].eficGas = arqAntigo.dpreq[i].eficGas;

			dpreq[i].parserie = arqAntigo.dpreq[i].parserie;
			dpreq[i].tempo = new double[dpreq[i].parserie];
			dpreq[i].dp = new double[dpreq[i].parserie];
			for (int j = 0; j < dpreq[i].parserie; j++) {
				dpreq[i].tempo[j] = arqAntigo.dpreq[i].tempo[j];
				dpreq[i].dp[j] = arqAntigo.dpreq[i].dp[j];
			}
		}
	}
}


void Ler::copia_fonte_gaslift(Ler& arqAntigo) {

	nvalvgas = arqAntigo.nvalvgas;
	if (nvalvgas > 0) {
		valvgl = new detVALVGL[nvalvgas];
		for (int i = 0; i < nvalvgas; i++) {
			valvgl[i].compP = arqAntigo.valvgl[i].compP;
			valvgl[i].compG = arqAntigo.valvgl[i].compG;
			valvgl[i].posicP = arqAntigo.valvgl[i].posicP;
			valvgl[i].posicG = arqAntigo.valvgl[i].posicG;
			valvgl[i].tipo = arqAntigo.valvgl[i].tipo;
			valvgl[i].diagarg = arqAntigo.valvgl[i].diagarg;
			valvgl[i].cd = arqAntigo.valvgl[i].cd;
			valvgl[i].frec = arqAntigo.valvgl[i].frec;
			valvgl[i].cdLiq = arqAntigo.valvgl[i].cdLiq;
			valvgl[i].frecLiq = arqAntigo.valvgl[i].frecLiq;
			valvgl[i].razarea = arqAntigo.valvgl[i].razarea;
			valvgl[i].pcali = arqAntigo.valvgl[i].pcali;
			valvgl[i].tcali = arqAntigo.valvgl[i].tcali;
			valvgl[i].diaexter = arqAntigo.valvgl[i].diaexter;
		}

	}
}

void Ler::copia_intermitencia(Ler& arqAntigo) {

	nintermi = arqAntigo.nintermi;
	if (nintermi> 0) {
		intermi = new detIntermi[nintermi];
		for(int i=0; i<nintermi;i++){
			intermi[i].indInicioTrechoAcumula=arqAntigo.intermi[i].indInicioTrechoAcumula;
			intermi[i].indFimTrechoAcumula=arqAntigo.intermi[i].indFimTrechoAcumula;
			intermi[i].indFimTrechoColuna=arqAntigo.intermi[i].indFimTrechoColuna;
			intermi[i].fracaoVazioPenetracao=arqAntigo.intermi[i].fracaoVazioPenetracao;
			intermi[i].criterio=arqAntigo.intermi[i].criterio;
		}
	}
}



void Ler::copia_perfil_producao(Ler& arqAntigo) {

	nvarprofp = arqAntigo.nvarprofp;
	nperfisp = arqAntigo.nperfisp;
	if (nperfisp > 0) {
		profp.n = nperfisp;
		profp.tempo = new double[profp.n];
		for (int i = 0; i < profp.n; i++) {
			profp.tempo[i] =arqAntigo.profp.tempo[i];
		}

		profp.pres = arqAntigo.profp.pres;
		profp.temp = arqAntigo.profp.temp;
		profp.hol = arqAntigo.profp.hol;
		profp.FVH = arqAntigo.profp.FVH; //chris - Hidratos
		profp.bet = arqAntigo.profp.bet;
		profp.ugs = arqAntigo.profp.ugs;
		profp.uls = arqAntigo.profp.uls;
		profp.ug = arqAntigo.profp.ug;
		profp.ul = arqAntigo.profp.ul;
		profp.arra = arqAntigo.profp.arra;
		profp.viscl = arqAntigo.profp.viscl;
		profp.viscg = arqAntigo.profp.viscg;
		profp.rhog = arqAntigo.profp.rhog;
		profp.rhol = arqAntigo.profp.rhol;
		profp.rhoo = arqAntigo.profp.rhoo;
		profp.rhoa = arqAntigo.profp.rhoa;
		profp.rhoMix = arqAntigo.profp.rhoMix;
		profp.RS = arqAntigo.profp.RS;
		profp.masg = arqAntigo.profp.masg;
		profp.masl = arqAntigo.profp.masl;
		profp.c0 = arqAntigo.profp.c0;
		profp.ud = arqAntigo.profp.ud;
		profp.RGO = arqAntigo.profp.RGO;
		profp.deng = arqAntigo.profp.deng;
		profp.yco2 = arqAntigo.profp.yco2;
		profp.calor = arqAntigo.profp.calor;
		profp.masstrans = arqAntigo.profp.masstrans;
		profp.cpg = arqAntigo.profp.cpg;
		profp.cpl = arqAntigo.profp.cpl;
		profp.qlst = arqAntigo.profp.qlst;
		profp.qlwst = arqAntigo.profp.qlwst;
		profp.qlstTot = arqAntigo.profp.qlstTot;
		profp.qgst = arqAntigo.profp.qgst;
		profp.api = arqAntigo.profp.api;
		profp.bsw = arqAntigo.profp.bsw;
		profp.hidro = arqAntigo.profp.hidro;
		profp.fric = arqAntigo.profp.fric;
		profp.Term1 = arqAntigo.profp.Term1;
		profp.Term2 = arqAntigo.profp.Term2;
		profp.dengD = arqAntigo.profp.dengD;
		profp.dengL = arqAntigo.profp.dengL;
		profp.reyi = arqAntigo.profp.reyi;
		profp.reye = arqAntigo.profp.reye;
		profp.Fr = arqAntigo.profp.Fr;
		profp.grashi = arqAntigo.profp.grashi;
		profp.grashe = arqAntigo.profp.grashe;
		profp.nusi = arqAntigo.profp.nusi;
		profp.nuse = arqAntigo.profp.nuse;
		profp.hi = arqAntigo.profp.hi;
		profp.he = arqAntigo.profp.he;
		profp.pri = arqAntigo.profp.pri;
		profp.pre = arqAntigo.profp.pre;
		profp.Rs=arqAntigo.profp.Rs;
		profp.Bo=arqAntigo.profp.Bo;
		profp.tAmb=arqAntigo.profp.tAmb;
		profp.PrG=arqAntigo.profp.PrG;
		profp.PrL=arqAntigo.profp.PrL;
		profp.pseudoL=arqAntigo.profp.pseudoL;
		profp.pseudoG=arqAntigo.profp.pseudoG;
		profp.pseudoM=arqAntigo.profp.pseudoM;
		profp.TResi=arqAntigo.profp.TResi;
		profp.redAtrito=arqAntigo.profp.redAtrito;
		profp.angulo=arqAntigo.profp.angulo;
		profp.diamInt=arqAntigo.profp.diamInt;
		profp.TempParede=arqAntigo.profp.TempParede;
		profp.dadosParafina=arqAntigo.profp.dadosParafina;
		profp.subResfria=arqAntigo.profp.subResfria;
	}
}

void Ler::copia_perfil_servico(Ler& arqAntigo) {

	nvarprofg = arqAntigo.nvarprofg;
	nperfisg = arqAntigo.nperfisg;
	if (nperfisg > 0) {
		profg.n = arqAntigo.profg.n;
		profg.tempo = new double[profg.n];
		for (int i = 0; i < profg.n; i++) {
			profg.tempo[i] =arqAntigo.profg.tempo[i];
		}

		profg.pres = arqAntigo.profg.pres;
		profg.temp = arqAntigo.profg.temp;
		profg.ugs = arqAntigo.profg.ugs;
		profg.ug = arqAntigo.profg.ug;
		profg.tens = arqAntigo.profg.tens;
		profg.viscg = arqAntigo.profg.viscg;
		profg.rhog = arqAntigo.profg.rhog;
		profg.masg = arqAntigo.profg.masg;
		profg.hidro = arqAntigo.profg.hidro;
		profg.fric = arqAntigo.profg.fric;
		profg.FVHG = arqAntigo.profg.FVHG; //chris - Hidratos
		profg.calor = arqAntigo.profg.calor;
		profg.qgst = arqAntigo.profg.qgst;
		profg.reyi = arqAntigo.profg.reyi;
		profg.reye = arqAntigo.profg.reye;
		profg.grashi = arqAntigo.profg.grashi;
		profg.grashe = arqAntigo.profg.grashe;
		profg.nusi = arqAntigo.profg.nusi;
		profg.nuse = arqAntigo.profg.nuse;
		profg.hi = arqAntigo.profg.hi;
		profg.he = arqAntigo.profg.he;
		profg.pri = arqAntigo.profg.pri;
		profg.pre = arqAntigo.profg.pre;
		profg.tAmb=arqAntigo.profg.tAmb;
		profg.angulo=arqAntigo.profg.angulo;
		profg.diamInt=arqAntigo.profg.diamInt;
		profg.TempParede=arqAntigo.profg.TempParede;
		profg.subResfria=arqAntigo.profg.subResfria;
	}
}

void Ler::copia_perfis_trans_producao(Ler& arqAntigo) {

	nperfistransp = arqAntigo.nperfistransp;
	if (nperfistransp > 0) {
		proftransp.posic = new int[nperfistransp];
		proftransp.n = arqAntigo.proftransp.n;
		proftransp.tempo = new double[proftransp.n];
		for (int i = 0; i < nperfistransp; i++) {
			proftransp.posic[i] = arqAntigo.proftransp.posic[i];
		}
		for (int i = 0; i < proftransp.n; i++) {
			proftransp.tempo[i] = arqAntigo.proftransp.tempo[i];
		}
	}
}

void Ler::copia_perfis_trans_servico(Ler& arqAntigo) {

	nperfistransg = arqAntigo.nperfistransg;
	if (nperfistransg > 0) {
		proftransg.posic = new int[nperfistransg];
		proftransg.n = arqAntigo.proftransg.n;
		proftransg.tempo = new double[proftransg.n];
		for (int i = 0; i < nperfistransg; i++) {
			proftransg.posic[i] = arqAntigo.proftransg.posic[i];
		}
		for (int i = 0; i < proftransg.n; i++) {
			proftransg.tempo[i] = arqAntigo.proftransg.tempo[i];
		}
	}
}


void Ler::copia_tendencia_producao(Ler& arqAntigo) {

	ntendp=arqAntigo.ntendp;
	if (ntendp > 0) {
			trendp = new detTRENDP[ntendp];
			nvartrendp = new int[ntendp];
			for (int i = 0; i < ntendp; i++) {

				trendp[i].pres = arqAntigo.trendp[i].pres;
				trendp[i].temp = arqAntigo.trendp[i].temp;
				trendp[i].hol = arqAntigo.trendp[i].hol;
				trendp[i].FVH = arqAntigo.trendp[i].FVH; //chris - Hidratos
				trendp[i].bet = arqAntigo.trendp[i].bet;
				trendp[i].ugs = arqAntigo.trendp[i].ugs;
				trendp[i].uls = arqAntigo.trendp[i].uls;
				trendp[i].ug = arqAntigo.trendp[i].ug;
				trendp[i].ul = arqAntigo.trendp[i].ul;
				trendp[i].arra = arqAntigo.trendp[i].arra;
				trendp[i].viscl = arqAntigo.trendp[i].viscl;
				trendp[i].viscg = arqAntigo.trendp[i].viscg;
				trendp[i].rhog = arqAntigo.trendp[i].rhog;
				trendp[i].rhol = arqAntigo.trendp[i].rhol;
				trendp[i].rhoMix = arqAntigo.trendp[i].rhoMix;
				trendp[i].masg = arqAntigo.trendp[i].masg;
				trendp[i].masl = arqAntigo.trendp[i].masl;
				trendp[i].c0 = arqAntigo.trendp[i].c0;
				trendp[i].ud = arqAntigo.trendp[i].ud;
				trendp[i].RGO = arqAntigo.trendp[i].RGO;
				trendp[i].deng = arqAntigo.trendp[i].deng;
				trendp[i].yco2 = arqAntigo.trendp[i].yco2;
				trendp[i].calor = arqAntigo.trendp[i].calor;
				trendp[i].masstrans = arqAntigo.trendp[i].masstrans;
				trendp[i].qlst = arqAntigo.trendp[i].qlst;
				trendp[i].qlstTot = arqAntigo.trendp[i].qlstTot;
				trendp[i].qgst = arqAntigo.trendp[i].qgst;
				trendp[i].api = arqAntigo.trendp[i].api;
				trendp[i].bsw = arqAntigo.trendp[i].bsw;
				trendp[i].hidro = arqAntigo.trendp[i].hidro;
				trendp[i].fric = arqAntigo.trendp[i].fric;
				trendp[i].dengD = arqAntigo.trendp[i].dengD;
				trendp[i].dengL = arqAntigo.trendp[i].dengL;

				trendp[i].mlFonte = arqAntigo.trendp[i].mlFonte;
				trendp[i].mgFonte = arqAntigo.trendp[i].mgFonte;
				trendp[i].mcFonte = arqAntigo.trendp[i].mcFonte;

				trendp[i].dpB = arqAntigo.trendp[i].dpB;
				trendp[i].potB = arqAntigo.trendp[i].potB;

				trendp[i].tempChokeJus = arqAntigo.trendp[i].tempChokeJus;

				trendp[i].reyi = arqAntigo.trendp[i].reyi;
				trendp[i].reye = arqAntigo.trendp[i].reye;
				trendp[i].Fr = arqAntigo.trendp[i].Fr;
				trendp[i].grashi = arqAntigo.trendp[i].grashi;
				trendp[i].grashe = arqAntigo.trendp[i].grashe;
				trendp[i].nusi = arqAntigo.trendp[i].nusi;
				trendp[i].nuse = arqAntigo.trendp[i].nuse;
				trendp[i].hi = arqAntigo.trendp[i].hi;
				trendp[i].he = arqAntigo.trendp[i].he;
				trendp[i].pri = arqAntigo.trendp[i].pri;
				trendp[i].pre = arqAntigo.trendp[i].pre;

				trendp[i].Rs = arqAntigo.trendp[i].Rs;
				trendp[i].Bo = arqAntigo.trendp[i].Bo;

				trendp[i].volJusM1PT = arqAntigo.trendp[i].volJusM1PT;
				trendp[i].volMonM1PT = arqAntigo.trendp[i].volMonM1PT;
				trendp[i].volJusM1ST = arqAntigo.trendp[i].volJusM1ST;
				trendp[i].volMonM1ST = arqAntigo.trendp[i].volMonM1ST;

				trendp[i].autoVal = arqAntigo.trendp[i].autoVal;
				trendp[i].autoVel = arqAntigo.trendp[i].autoVel;
				trendp[i].flutuacao = arqAntigo.trendp[i].flutuacao;
				trendp[i].diamInt = arqAntigo.trendp[i].diamInt;
				trendp[i].TempParede = arqAntigo.trendp[i].TempParede;
				trendp[i].dadosParafina = arqAntigo.trendp[i].dadosParafina;

				trendp[i].inventarioGas = arqAntigo.trendp[i].inventarioGas;
				trendp[i].inventarioLiq = arqAntigo.trendp[i].inventarioLiq;

				trendp[i].subResfria=arqAntigo.trendp[i].subResfria;

				nvartrendp[i] = arqAntigo.nvartrendp[i];
				trendp[i].comp=arqAntigo.trendp[i].comp;
				trendp[i].posic = arqAntigo.trendp[i].posic;
				trendp[i].dt =arqAntigo.trendp[i].dt;

				trendp[i].rotulo =arqAntigo.trendp[i].rotulo;
			}
	}
}


void Ler::copia_tendencia_servico(Ler& arqAntigo) {


	ntendg = arqAntigo.ntendg;
	if (ntendg > 0) {
		trendg = new detTRENDG[ntendg];
		nvartrendg = new int[ntendg];
		for (int i = 0; i < ntendg; i++) {
			nvartrendg[i] = arqAntigo.nvartrendg[i];
			trendg[i].comp=arqAntigo.trendg[i].comp;
			trendg[i].posic = arqAntigo.trendg[i].posic;
			trendg[i].dt = arqAntigo.trendg[i].dt;
			trendg[i].rotulo = arqAntigo.trendg[i].rotulo;

			trendg[i].pres = arqAntigo.trendg[i].pres;
			trendg[i].temp = arqAntigo.trendg[i].temp;
			trendg[i].ugs = arqAntigo.trendg[i].ugs;
			trendg[i].ug = arqAntigo.trendg[i].ug;
			trendg[i].tens = arqAntigo.trendg[i].tens;
			trendg[i].viscg = arqAntigo.trendg[i].viscg;
			trendg[i].rhog = arqAntigo.trendg[i].rhog;
			trendg[i].masg = arqAntigo.trendg[i].masg;
			trendg[i].hidro = arqAntigo.trendg[i].hidro;
			trendg[i].fric = arqAntigo.trendg[i].fric;
			trendg[i].FVHG = arqAntigo.trendg[i].FVHG; //chris - Hidratos
			trendg[i].calor = arqAntigo.trendg[i].calor;
			trendg[i].qgst = arqAntigo.trendg[i].qgst;
			trendg[i].pEstagVGL = arqAntigo.trendg[i].pEstagVGL;
			trendg[i].tEstagVGL = arqAntigo.trendg[i].tEstagVGL;
			trendg[i].pGargVGL = arqAntigo.trendg[i].pGargVGL;
			trendg[i].tGargVGL = arqAntigo.trendg[i].tGargVGL;
			trendg[i].qVGL = arqAntigo.trendg[i].qVGL;
			trendg[i].velgarg = arqAntigo.trendg[i].velgarg;
			trendg[i].reyi = arqAntigo.trendg[i].reyi;
			trendg[i].reye = arqAntigo.trendg[i].reye;
			trendg[i].grashi = arqAntigo.trendg[i].grashi;
			trendg[i].grashe = arqAntigo.trendg[i].grashe;
			trendg[i].nusi = arqAntigo.trendg[i].nusi;
			trendg[i].nuse = arqAntigo.trendg[i].nuse;
			trendg[i].hi = arqAntigo.trendg[i].hi;
			trendg[i].he = arqAntigo.trendg[i].he;
			trendg[i].pri = arqAntigo.trendg[i].pri;
			trendg[i].pre = arqAntigo.trendg[i].pre;
			trendg[i].diamInt = arqAntigo.trendg[i].diamInt;
			trendg[i].TempParede = arqAntigo.trendg[i].TempParede;
			trendg[i].subResfria=arqAntigo.trendg[i].subResfria;
		}
	}
}

void Ler::copia_tendencia_trans_producao(Ler& arqAntigo) {

	ntendtransp = arqAntigo.ntendtransp;
	if (ntendtransp > 0) {
		trendtransp = new detTRENDTrans[ntendtransp];
		for (int i = 0; i < ntendtransp; i++) {
			trendtransp[i].comp=arqAntigo.trendtransp[i].comp;
			trendtransp[i].posic = arqAntigo.trendtransp[i].posic;
			trendtransp[i].camada = arqAntigo.trendtransp[i].camada;
			trendtransp[i].discre = arqAntigo.trendtransp[i].discre;
			trendtransp[i].dt = arqAntigo.trendtransp[i].dt;
			trendtransp[i].rotulo = arqAntigo.trendtransp[i].rotulo;
		}
	}
}

void Ler::copia_tendencia_trans_servico(Ler& arqAntigo) {


	ntendtransg = arqAntigo.ntendtransg;
	if (ntendtransg > 0) {
		trendtransg = new detTRENDTrans[ntendtransg];
		for (int i = 0; i < ntendtransg; i++) {
			trendtransg[i].comp=arqAntigo.trendtransg[i].comp;
			trendtransg[i].posic = arqAntigo.trendtransg[i].posic;
			trendtransg[i].camada = arqAntigo.trendtransg[i].camada;
			trendtransg[i].discre = arqAntigo.trendtransg[i].discre;
			trendtransg[i].dt = arqAntigo.trendtransg[i].dt;
			trendtransg[i].rotulo = arqAntigo.trendtransg[i].rotulo;
		}
	}
}

void Ler::copia_tela(Ler& arqAntigo) {

	ntela = arqAntigo.ntela;
	if (ntela > 0) {
		tela = new detTela[ntela];
		for (int i = 0; i < ntela; i++) {
			tela[i].col = arqAntigo.tela[i].col;
			tela[i].posic = arqAntigo.tela[i].posic;
			tela[i].var = arqAntigo.tela[i].var;
		}
	}
}

//template class Ler;
