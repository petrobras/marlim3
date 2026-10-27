#include "acessorios.h"

acessorio::acessorio(const int vtipo):
   bcs(),
   multibcs(),
   mda(),
   chk(),
   ipr(),
   injg(),
   injl(),
   injm(),
   iprvap(),
   injmvap(),
   fluido(),
   bvol(),
   fontechk(),
   radialPoro(),
   poroso2D()
   {//construtor
   delp=0.;
   dpB=0.;
   potB=0.;
   potBT=0.;
   potTermo=0;
   tipoCompGas=0;
   fatPoli=1.;
   eficLiq=1.;
   eficGas=1.;

   indJusRecic=-1;
   indMonRecic=-1;

   tipo=vtipo;
}

acessorio::acessorio(const acessorio& antigo){//construtor c�pia

   tipo=antigo.tipo;
   bcs=antigo.bcs;
   multibcs=antigo.multibcs;
   mda=antigo.mda;
   chk=antigo.chk;
   ipr=antigo.ipr;
   injg=antigo.injg;
   injl=antigo.injl;
   injm=antigo.injm;
   iprvap=antigo.iprvap;
   injmvap=antigo.injmvap;

   fluido=antigo.fluido;
   delp=antigo.delp;
   dpB=antigo.dpB;
   potB=antigo.potB;
   potBT=antigo.potBT;
   potTermo=antigo.potTermo;
   tipoCompGas=antigo.tipoCompGas;
   fatPoli=antigo.fatPoli;
   eficLiq=antigo.eficLiq;
   delp=antigo.delp;
   eficGas=antigo.eficGas;
   fontechk=antigo.fontechk;
   radialPoro=antigo.radialPoro;
   poroso2D=antigo.poroso2D;

   indJusRecic=antigo.indJusRecic;
   indMonRecic=antigo.indMonRecic;
}

acessorio& acessorio::operator =(const acessorio& antigo){
   tipo=antigo.tipo;
   bcs=antigo.bcs;
   multibcs=antigo.multibcs;
   mda=antigo.mda;
   chk=antigo.chk;
   ipr=antigo.ipr;
   injg=antigo.injg;
   injl=antigo.injl;
   injm=antigo.injm;
   iprvap=antigo.iprvap;
   injmvap=antigo.injmvap;

   fluido=antigo.fluido;
   delp=antigo.delp;
   tipoCompGas=antigo.tipoCompGas;
   fatPoli=antigo.fatPoli;
   eficLiq=antigo.eficLiq;
   delp=antigo.delp;
   dpB=antigo.dpB;
   potB=antigo.potB;
   potBT=antigo.potBT;
   potTermo=antigo.potTermo;
   eficGas=antigo.eficGas;
   bvol=antigo.bvol;
   fontechk=antigo.fontechk;
   radialPoro=antigo.radialPoro;
   poroso2D=antigo.poroso2D;

   indJusRecic=antigo.indJusRecic;
   indMonRecic=antigo.indMonRecic;
   return *this;
}
//0-> sem acess�rio,1->Fonte de g�s, 2->IPR, 3-> BCS, 4-> choke, 5-> mudan�a de d�metro
