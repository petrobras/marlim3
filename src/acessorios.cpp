#include "acessorios.h"

acessorio::acessorio(const int vtipo):
   bcs(),
   //bvolvap(),
   mda(),
   chk(),
   ipr(),
   injg(),
   injl(),
   injm(),//alteracao7
   iprvap(),
   injmvap(),
   fluido(),
   bvol(),
   fontechk(),
   radialPoro(),
   poroso2D()
   {//construtor
   delp=0.;
   tipoCompGas=0;
   fatPoli=1.;
   eficLiq=1.;
   eficGas=1.;
   tipo=vtipo;
}

acessorio::acessorio(const acessorio& antigo){//construtor c�pia

   tipo=antigo.tipo;
   bcs=antigo.bcs;
   mda=antigo.mda;
   chk=antigo.chk;
   ipr=antigo.ipr;
   injg=antigo.injg;
   injl=antigo.injl;
   injm=antigo.injm;//alteracao7
   iprvap=antigo.iprvap;
   injmvap=antigo.injmvap;

   fluido=antigo.fluido;
   delp=antigo.delp;
   tipoCompGas=antigo.tipoCompGas;
   fatPoli=antigo.fatPoli;
   eficLiq=antigo.eficLiq;
   delp=antigo.delp;
   eficGas=antigo.eficGas;
   //bvolvap=antigo.bvolvap;
   fontechk=antigo.fontechk;
   radialPoro=antigo.radialPoro;
   poroso2D=antigo.poroso2D;
}

acessorio& acessorio::operator =(const acessorio& antigo){
   tipo=antigo.tipo;
   bcs=antigo.bcs;
   mda=antigo.mda;
   chk=antigo.chk;
   ipr=antigo.ipr;
   injg=antigo.injg;
   injl=antigo.injl;
   injm=antigo.injm;//alteracao7
   iprvap=antigo.iprvap;
   injmvap=antigo.injmvap;

   fluido=antigo.fluido;
   delp=antigo.delp;
   tipoCompGas=antigo.tipoCompGas;
   fatPoli=antigo.fatPoli;
   eficLiq=antigo.eficLiq;
   delp=antigo.delp;
   eficGas=antigo.eficGas;
   //bvolvap=antigo.bvolvap;
   bvol=antigo.bvol;
   fontechk=antigo.fontechk;
   radialPoro=antigo.radialPoro;
   poroso2D=antigo.poroso2D;
   return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//0-> sem acess�rio,1->Fonte de g�s, 2->IPR, 3-> BCS, 4-> choke, 5-> mudan�a de d�metro
/*template<class M,class double> void carrega(const M& acsr, acessorio& grupo){
  if(grupo.tipo==1)grupo.injg=const_cast<InjGas&>(acsr);
  else if(grupo.tipo==2)grupo.ipr=acsr;
  else if(grupo.tipo==3)grupo.bcs=acsr;
  else if(grupo.tipo==4)grupo.chk=acsr;
  else if(grupo.tipo==5)grupo.mda=acsr;
} */

//template class acessorio;

//void carrega(const IPR& acsr, acessorio& grupo){
//  grupo.ipr=acsr;
//}

