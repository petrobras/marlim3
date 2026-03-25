/*
 * PropFluCol.h
 *
 *  Created on: 21/12/2015
 *      Author: bwz1
 */

#ifndef PROPFLUCOL_H_
#define PROPFLUCOL_H_

//extern int injPoc;
//extern double** RhoInj;
//extern double** ViscInj;
//extern double** CondInj;
//extern double** CpInj;
//extern double** DrhoDtInj;
//extern int ndiv;

class ProFluCol{
	public:

    double rholStd;//massa especifica do liquido nas condicoes standard
    //, caracterizacao do fluido feita pelo usuario, kg/m3
    double compresP;//compressibilidade do liquido 1/Pa
    //, caracterizacao do fluido feita pelo usuario
    double compresT;//expansividade termica do liquido 1/K
    //, caracterizacao do fluido feita pelo usuario
    double tenssup;//tensao superficial N/m
    //, caracterizacao do fluido feita pelo usuario
    double cp;//calor especifico do liquido J/kgK
    //, caracterizacao do fluido feita pelo usuario
    double cond;//condutividade do liquido  W/mK
    //, caracterizacao do fluido feita pelo usuario
    double TempL;//7 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, temperatura C de um dos pontos do ASTM
    double LVisL;//8 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtempl,vlvisl)
    double TempH;//9 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, temperatura C de um dos pontos do ASTM
    double LVisH;//10 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtemph,vlvish)
    double salin;//salinidade da agua, quando o fluido complementar é agua ou fluido de completacao
    int injPoc;//indicador de que se está trabalhando com um fluido para poçoss de injeção
    //injPoc<=1. fluido definido pelo usuário, injPoc=2, agua (uso de correlacoes internas,
    //injPoc=3, gas rico em CO2 (uso de tabela)
    int descarga;//quando 1, indica que o fluido complementar está sendo usado como fluido de completação
    //para um modelo de descarga de GL
    int npontos;//numero de pontos da tabela de propriedades usada para o caso de fluido de injecao
    //ser um gas rico em CO2
    int tipoF;//utilizado quando se quer explicitar que o fluido e agua ou redutor de atrito
    double TR;
    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////Tabelas com as propriedades do gas de injecao rico em CO2///////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    double** RhoInj;
    double** ViscInj;
    double** CondInj;
    double** CpInj;
    double** DrhoDtInj;
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    ProFluCol(double=1000,double=0,double=0,double=1e-6,double=4000,double=1,double=0,
    		double=1,double=2,double=40,double=1,int=0, int vtipo=0); //construtor default
    ProFluCol(const ProFluCol&); //construtor por c�pia
    ProFluCol& operator=(const ProFluCol&);
	double operator[](int i)const;
	double MasEspFlu(double pres/*pressao kgf/cm2*/,
			double temp/*temperatura C*/) const;//retorna a massa especifica kg/m3
 	double VisFlu(double pres/*pressao kgf/cm2*/,
 			double temp/*temperatura C*/) const;//retorna a viscosidade cP
    double TensSuper(double pres/*pressao kgf/cm2*/,
    		double temp/*temperatura C*/)const{return tenssup;}//retorna a tensão superficial N/m
    double CalorLiq(double pres/*pressao kgf/cm2*/,
    		double temp/*temperatura C*/) const;//retorna o calor especifico J/kgK
    double CondLiq(double pres/*pressao kgf/cm2*/,
    		double temp/*temperatura C*/) const;//retorna a condutividade W/mK
    double DrhoDtFlu(double pres/*pressao kgf/cm2*/,
    		double temp/*temperatura C*/) const; //derivada da massa especifica com a temperatura,
    //kg/m3/K
    double interpolaVarInj(double pres/*pressao kgf/cm2*/,
    		double temp/*temperatura C*/,
			double** VarInj/*tabela com uma propriedade do gas rico em CO2*/) const;//funcao para interpolacao
    //de propriedades do fluido obtidos de um arquivo PVTSim, para o caso de fluido de injecao ser um gas rico em CO2
    double psia(const double p)const { return (p*0.9678411)*14.69595;} //p->kgf, transforma kgf para psia
    double Faren(const double t)const { return 1.8*t + 32;} //t em Celcius, transforma Celcius para Farenheit
    double calcDR(double ReyL);

};



#endif /* PROPFLUCOL_H_ */
