#include "JSON_entrada.h"

JSON_entrada_configuracaoInicial_parametrosDescarga::JSON_entrada_configuracaoInicial_parametrosDescarga(){
	contents["vazaoLimiteDescarga"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_vazaoLimiteDescarga>();
	contents["pressaoLimiteDescarga"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoLimiteDescarga>();
	contents["pressaoMinimaDescarga"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoMinimaDescarga>();
	contents["pressaoTrabalhoDescargaGas"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoTrabalhoDescargaGas>();
	contents["pressaoLimiteDescargaGas"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoLimiteDescargaGas>();
	contents["pressaoMinimaDescargaGas"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoMinimaDescargaGas>();
	contents["pressaoInicialDescargaGas"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoInicialDescargaGas>();
	contents["temperaturaDescarga"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_temperaturaDescarga>();
	contents["tempoLatencia"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga_tempoLatencia>();
}

JSON_entrada_configuracaoInicial_parametrosDescarga_vazaoLimiteDescarga& JSON_entrada_configuracaoInicial_parametrosDescarga::vazaoLimiteDescarga(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_vazaoLimiteDescarga&>(*contents["vazaoLimiteDescarga"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoLimiteDescarga& JSON_entrada_configuracaoInicial_parametrosDescarga::pressaoLimiteDescarga(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoLimiteDescarga&>(*contents["pressaoLimiteDescarga"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoMinimaDescarga& JSON_entrada_configuracaoInicial_parametrosDescarga::pressaoMinimaDescarga(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoMinimaDescarga&>(*contents["pressaoMinimaDescarga"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoTrabalhoDescargaGas& JSON_entrada_configuracaoInicial_parametrosDescarga::pressaoTrabalhoDescargaGas(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoTrabalhoDescargaGas&>(*contents["pressaoTrabalhoDescargaGas"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoLimiteDescargaGas& JSON_entrada_configuracaoInicial_parametrosDescarga::pressaoLimiteDescargaGas(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoLimiteDescargaGas&>(*contents["pressaoLimiteDescargaGas"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoMinimaDescargaGas& JSON_entrada_configuracaoInicial_parametrosDescarga::pressaoMinimaDescargaGas(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoMinimaDescargaGas&>(*contents["pressaoMinimaDescargaGas"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoInicialDescargaGas& JSON_entrada_configuracaoInicial_parametrosDescarga::pressaoInicialDescargaGas(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_pressaoInicialDescargaGas&>(*contents["pressaoInicialDescargaGas"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_temperaturaDescarga& JSON_entrada_configuracaoInicial_parametrosDescarga::temperaturaDescarga(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_temperaturaDescarga&>(*contents["temperaturaDescarga"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga_tempoLatencia& JSON_entrada_configuracaoInicial_parametrosDescarga::tempoLatencia(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga_tempoLatencia&>(*contents["tempoLatencia"].get());
}

JSON_entrada_configuracaoInicial_Avancado::JSON_entrada_configuracaoInicial_Avancado(){
	contents["CriterioMonofasico"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_CriterioMonofasico>();
	contents["CriterioCondensacao"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_CriterioCondensacao>();
	contents["CriterioDTMin"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_CriterioDTMin>();
	contents["CriterioBuscaFalsaCorda"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_CriterioBuscaFalsaCorda>();
	contents["taxaDespre"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_taxaDespre>();
	contents["MedSimpPresFront"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_MedSimpPresFront>();
	contents["JTLiquidoSimple"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_JTLiquidoSimple>();
	contents["limTransMass"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_limTransMass>();
	contents["RelaxaDTChoke"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_RelaxaDTChoke>();
	contents["desligaPenalizaDT"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_desligaPenalizaDT>();
	contents["controleDTvalv"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_controleDTvalv>();
	contents["CriterioConvergPerm"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_CriterioConvergPerm>();
	contents["AceleraConvergPerm"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_AceleraConvergPerm>();
	contents["escorregamentoCelulaContorno"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_escorregamentoCelulaContorno>();
	contents["correcaoContracorPerm"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_correcaoContracorPerm>();
	//contents["tempoEstabCol"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_tempoEstabCol>();
	contents["estabCol"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_estabCol>();
	contents["TcorrecaoModComp"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_TcorrecaoModComp>();
	contents["correcaoModComp"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_correcaoModComp>();
	contents["desligaDeriTransMassDTemp"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_desligaDeriTransMassDTemp>();
	contents["corrigeContSep"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_corrigeContSep>();
	contents["acopColAnulPermForte"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_acopColAnulPermForte>();
	contents["mudaArea"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_mudaArea>();
	contents["nthrd"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_nthrd>();
	contents["nthrdMatriz"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_nthrdMatriz>();
	contents["miniTabDinAtraso"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_miniTabDinAtraso>();
	contents["miniTabDinDp"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_miniTabDinDp>();
	contents["miniTabDinDt"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_miniTabDinDt>();
	contents["Tsonico"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_Tsonico>();
	contents["sonico"] = make_shared<JSON_entrada_configuracaoInicial_Avancado_sonico>();
}


JSON_entrada_configuracaoInicial_Avancado_CriterioMonofasico& JSON_entrada_configuracaoInicial_Avancado::CriterioMonofasico(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_CriterioMonofasico&>(*contents["CriterioMonofasico"].get());
}

JSON_entrada_configuracaoInicial_Avancado_CriterioCondensacao& JSON_entrada_configuracaoInicial_Avancado::CriterioCondensacao(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_CriterioCondensacao&>(*contents["CriterioCondensacao"].get());
}

JSON_entrada_configuracaoInicial_Avancado_CriterioDTMin& JSON_entrada_configuracaoInicial_Avancado::CriterioDTMin(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_CriterioDTMin&>(*contents["CriterioDTMin"].get());
}

JSON_entrada_configuracaoInicial_Avancado_CriterioBuscaFalsaCorda& JSON_entrada_configuracaoInicial_Avancado::CriterioBuscaFalsaCorda(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_CriterioBuscaFalsaCorda&>(*contents["CriterioBuscaFalsaCorda"].get());
}

JSON_entrada_configuracaoInicial_Avancado_taxaDespre& JSON_entrada_configuracaoInicial_Avancado::taxaDespre(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_taxaDespre&>(*contents["taxaDespre"].get());
}

JSON_entrada_configuracaoInicial_Avancado_limTransMass& JSON_entrada_configuracaoInicial_Avancado::limTransMass(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_limTransMass&>(*contents["limTransMass"].get());
}

JSON_entrada_configuracaoInicial_Avancado_MedSimpPresFront& JSON_entrada_configuracaoInicial_Avancado::MedSimpPresFront(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_MedSimpPresFront&>(*contents["MedSimpPresFront"].get());
}

JSON_entrada_configuracaoInicial_Avancado_JTLiquidoSimple& JSON_entrada_configuracaoInicial_Avancado::JTLiquidoSimple(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_JTLiquidoSimple&>(*contents["JTLiquidoSimple"].get());
}

JSON_entrada_configuracaoInicial_Avancado_desligaPenalizaDT& JSON_entrada_configuracaoInicial_Avancado::desligaPenalizaDT(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_desligaPenalizaDT&>(*contents["desligaPenalizaDT"].get());
}

JSON_entrada_configuracaoInicial_Avancado_controleDTvalv& JSON_entrada_configuracaoInicial_Avancado::controleDTvalv(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_controleDTvalv&>(*contents["controleDTvalv"].get());
}

JSON_entrada_configuracaoInicial_Avancado_RelaxaDTChoke& JSON_entrada_configuracaoInicial_Avancado::RelaxaDTChoke(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_RelaxaDTChoke&>(*contents["RelaxaDTChoke"].get());
}

JSON_entrada_configuracaoInicial_Avancado_CriterioConvergPerm& JSON_entrada_configuracaoInicial_Avancado::CriterioConvergPerm(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_CriterioConvergPerm&>(*contents["CriterioConvergPerm"].get());
}

JSON_entrada_configuracaoInicial_Avancado_AceleraConvergPerm& JSON_entrada_configuracaoInicial_Avancado::AceleraConvergPerm(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_AceleraConvergPerm&>(*contents["AceleraConvergPerm"].get());
}

JSON_entrada_configuracaoInicial_Avancado_escorregamentoCelulaContorno& JSON_entrada_configuracaoInicial_Avancado::escorregamentoCelulaContorno(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_escorregamentoCelulaContorno&>(*contents["escorregamentoCelulaContorno"].get());
}

JSON_entrada_configuracaoInicial_Avancado_correcaoContracorPerm& JSON_entrada_configuracaoInicial_Avancado::correcaoContracorPerm(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_correcaoContracorPerm&>(*contents["correcaoContracorPerm"].get());
}

//JSON_entrada_configuracaoInicial_Avancado_tempoEstabCol& JSON_entrada_configuracaoInicial_Avancado::tempoEstabCol(){
	//return static_cast<JSON_entrada_configuracaoInicial_Avancado_tempoEstabCol&>(*contents["tempoEstabCol"].get());
//}

JSON_entrada_configuracaoInicial_Avancado_estabCol& JSON_entrada_configuracaoInicial_Avancado::estabCol(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_estabCol&>(*contents["estabCol"].get());
}

JSON_entrada_configuracaoInicial_Avancado_TcorrecaoModComp& JSON_entrada_configuracaoInicial_Avancado::TcorrecaoModComp(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_TcorrecaoModComp&>(*contents["TcorrecaoModComp"].get());
}

JSON_entrada_configuracaoInicial_Avancado_correcaoModComp& JSON_entrada_configuracaoInicial_Avancado::correcaoModComp(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_correcaoModComp&>(*contents["correcaoModComp"].get());
}

JSON_entrada_configuracaoInicial_Avancado_desligaDeriTransMassDTemp& JSON_entrada_configuracaoInicial_Avancado::desligaDeriTransMassDTemp(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_desligaDeriTransMassDTemp&>(*contents["desligaDeriTransMassDTemp"].get());
}

JSON_entrada_configuracaoInicial_Avancado_corrigeContSep& JSON_entrada_configuracaoInicial_Avancado::corrigeContSep(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_corrigeContSep&>(*contents["corrigeContSep"].get());
}

JSON_entrada_configuracaoInicial_Avancado_acopColAnulPermForte& JSON_entrada_configuracaoInicial_Avancado::acopColAnulPermForte(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_acopColAnulPermForte&>(*contents["acopColAnulPermForte"].get());
}

JSON_entrada_configuracaoInicial_Avancado_mudaArea& JSON_entrada_configuracaoInicial_Avancado::mudaArea(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_mudaArea&>(*contents["mudaArea"].get());
}

JSON_entrada_configuracaoInicial_Avancado_nthrd& JSON_entrada_configuracaoInicial_Avancado::nthrd(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_nthrd&>(*contents["nthrd"].get());
}

JSON_entrada_configuracaoInicial_Avancado_nthrdMatriz& JSON_entrada_configuracaoInicial_Avancado::nthrdMatriz(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_nthrdMatriz&>(*contents["nthrdMatriz"].get());
}

JSON_entrada_configuracaoInicial_Avancado_miniTabDinAtraso& JSON_entrada_configuracaoInicial_Avancado::miniTabDinAtraso(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_miniTabDinAtraso&>(*contents["miniTabDinAtraso"].get());
}

JSON_entrada_configuracaoInicial_Avancado_miniTabDinDp& JSON_entrada_configuracaoInicial_Avancado::miniTabDinDp(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_miniTabDinDp&>(*contents["miniTabDinDp"].get());
}

JSON_entrada_configuracaoInicial_Avancado_miniTabDinDt& JSON_entrada_configuracaoInicial_Avancado::miniTabDinDt(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_miniTabDinDt&>(*contents["miniTabDinDt"].get());
}

JSON_entrada_configuracaoInicial_Avancado_Tsonico& JSON_entrada_configuracaoInicial_Avancado::Tsonico(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_Tsonico&>(*contents["Tsonico"].get());
}

JSON_entrada_configuracaoInicial_Avancado_sonico& JSON_entrada_configuracaoInicial_Avancado::sonico(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado_sonico&>(*contents["sonico"].get());
}

JSON_entrada_configuracaoInicial_condicaoVazPres::JSON_entrada_configuracaoInicial_condicaoVazPres(){
	contents["ativo"] = make_shared<JSON_entrada_configuracaoInicial_condicaoVazPres_ativo>();
	contents["pressao"] = make_shared<JSON_entrada_configuracaoInicial_condicaoVazPres_pressao>();
	contents["temperatura"] = make_shared<JSON_entrada_configuracaoInicial_condicaoVazPres_temperatura>();
	contents["Vazao Massica"] = make_shared<JSON_entrada_configuracaoInicial_condicaoVazPres_VazMass>();
	contents["razaoBeta"] = make_shared<JSON_entrada_configuracaoInicial_condicaoVazPres_razaoBeta>();
	contents["tempo"] = make_shared<JSON_entrada_configuracaoInicial_condicaoVazPres_tempo>();
}

JSON_entrada_configuracaoInicial_condicaoVazPres_ativo& JSON_entrada_configuracaoInicial_condicaoVazPres::ativo(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoVazPres_ativo&>(*contents["ativo"].get());
}

JSON_entrada_configuracaoInicial_condicaoVazPres_pressao& JSON_entrada_configuracaoInicial_condicaoVazPres::pressao(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoVazPres_pressao&>(*contents["pressao"].get());
}

JSON_entrada_configuracaoInicial_condicaoVazPres_temperatura& JSON_entrada_configuracaoInicial_condicaoVazPres::temperatura(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoVazPres_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_configuracaoInicial_condicaoVazPres_VazMass& JSON_entrada_configuracaoInicial_condicaoVazPres::VazMass(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoVazPres_VazMass&>(*contents["Vazao Massica"].get());
}

JSON_entrada_configuracaoInicial_condicaoVazPres_razaoBeta& JSON_entrada_configuracaoInicial_condicaoVazPres::razaoBeta(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoVazPres_razaoBeta&>(*contents["razaoBeta"].get());
}

JSON_entrada_configuracaoInicial_condicaoVazPres_tempo& JSON_entrada_configuracaoInicial_condicaoVazPres::tempo(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoVazPres_tempo&>(*contents["tempo"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao::JSON_entrada_configuracaoInicial_condicaoPressao(){
	contents["ativo"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao_ativo>();
	contents["indFluido"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao_indFluido>();
	contents["pressao"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao_pressao>();
	contents["temperatura"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao_temperatura>();
	contents["titulo"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao_titulo>();
	contents["razaoBeta"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao_razaoBeta>();
	contents["tempo"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao_tempo>();
}

JSON_entrada_configuracaoInicial_condicaoPressao_ativo& JSON_entrada_configuracaoInicial_condicaoPressao::ativo(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao_ativo&>(*contents["ativo"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao_indFluido& JSON_entrada_configuracaoInicial_condicaoPressao::indFluido(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao_indFluido&>(*contents["indFluido"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao_pressao& JSON_entrada_configuracaoInicial_condicaoPressao::pressao(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao_pressao&>(*contents["pressao"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao_temperatura& JSON_entrada_configuracaoInicial_condicaoPressao::temperatura(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao_titulo& JSON_entrada_configuracaoInicial_condicaoPressao::titulo(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao_titulo&>(*contents["titulo"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao_razaoBeta& JSON_entrada_configuracaoInicial_condicaoPressao::razaoBeta(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao_razaoBeta&>(*contents["razaoBeta"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao_tempo& JSON_entrada_configuracaoInicial_condicaoPressao::tempo(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao_tempo&>(*contents["tempo"].get());
}

JSON_entrada_configuracaoInicial_correlacoesPorArranjo::JSON_entrada_configuracaoInicial_correlacoesPorArranjo(){
	contents["estratificado"] = make_shared<JSON_entrada_configuracaoInicial_correlacoesPorArranjo_estratificado>();
	contents["bolhaGolfada"] = make_shared<JSON_entrada_configuracaoInicial_correlacoesPorArranjo_bolhaGolfada>();
	contents["anularChurn"] = make_shared<JSON_entrada_configuracaoInicial_correlacoesPorArranjo_anularChurn>();
}

JSON_entrada_configuracaoInicial_correlacoesPorArranjo_estratificado& JSON_entrada_configuracaoInicial_correlacoesPorArranjo::estratificado(){
	return static_cast<JSON_entrada_configuracaoInicial_correlacoesPorArranjo_estratificado&>(*contents["estratificado"].get());
}

JSON_entrada_configuracaoInicial_correlacoesPorArranjo_bolhaGolfada& JSON_entrada_configuracaoInicial_correlacoesPorArranjo::bolhaGolfada(){
	return static_cast<JSON_entrada_configuracaoInicial_correlacoesPorArranjo_bolhaGolfada&>(*contents["bolhaGolfada"].get());
}

JSON_entrada_configuracaoInicial_correlacoesPorArranjo_anularChurn& JSON_entrada_configuracaoInicial_correlacoesPorArranjo::anularChurn(){
	return static_cast<JSON_entrada_configuracaoInicial_correlacoesPorArranjo_anularChurn&>(*contents["anularChurn"].get());
}

JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item::JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item(){
	contents["id"] = make_shared<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_id>();
	contents["condutividade"] = make_shared<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_condutividade>();
	contents["calorEspecifico"] = make_shared<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_calorEspecifico>();
	contents["massaEspecifica"] = make_shared<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_massaEspecifica>();
}

JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_id& JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item::id(){
	return static_cast<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_id&>(*contents["id"].get());
}

JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_condutividade& JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item::condutividade(){
	return static_cast<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_condutividade&>(*contents["condutividade"].get());
}

JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_calorEspecifico& JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item::calorEspecifico(){
	return static_cast<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_calorEspecifico&>(*contents["calorEspecifico"].get());
}

JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_massaEspecifica& JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item::massaEspecifica(){
	return static_cast<JSON_entrada_configuracaoInicial_Formacao_Propriedades_Item_massaEspecifica&>(*contents["massaEspecifica"].get());
}

JSON_entrada_configuracaoInicial_Formacao::JSON_entrada_configuracaoInicial_Formacao(){
	contents["Propriedades"] = make_shared<JSON_entrada_configuracaoInicial_Formacao_Propriedades>();
	contents["TempoProducao"] = make_shared<JSON_entrada_configuracaoInicial_Formacao_TempoProducao>();
}

JSON_entrada_configuracaoInicial_Formacao_Propriedades& JSON_entrada_configuracaoInicial_Formacao::Propriedades(){
	return static_cast<JSON_entrada_configuracaoInicial_Formacao_Propriedades&>(*contents["Propriedades"].get());
}

JSON_entrada_configuracaoInicial_Formacao_TempoProducao& JSON_entrada_configuracaoInicial_Formacao::TempoProducao(){
	return static_cast<JSON_entrada_configuracaoInicial_Formacao_TempoProducao&>(*contents["TempoProducao"].get());
}

JSON_entrada_configuracaoInicial::JSON_entrada_configuracaoInicial(){
	contents["origemGeometria"] = make_shared<JSON_entrada_configuracaoInicial_origemGeometria>();
	contents["sentidoGeometriaSegueEscoamento"] = make_shared<JSON_entrada_configuracaoInicial_sentidoGeometriaSegueEscoamento>();
	contents["saidaClassica"] = make_shared<JSON_entrada_configuracaoInicial_saidaClassica>();
	contents["linhaGas"] = make_shared<JSON_entrada_configuracaoInicial_linhaGas>();
	contents["saidaTela"] = make_shared<JSON_entrada_configuracaoInicial_saidaTela>();
	contents["equilibrioTermico"] = make_shared<JSON_entrada_configuracaoInicial_equilibrioTermico>();
	contents["latente"] = make_shared<JSON_entrada_configuracaoInicial_latente>();
	contents["condlatente"] = make_shared<JSON_entrada_configuracaoInicial_condlatente>();
	contents["pvtsimArq"] = make_shared<JSON_entrada_configuracaoInicial_pvtsimArq>();
	contents["modeloFluidoTabelaFlash"] = make_shared<JSON_entrada_configuracaoInicial_modeloFluidoTabelaFlash>();
	contents["modeloFluidoComposicional"] = make_shared<JSON_entrada_configuracaoInicial_modeloFluidoComposicional>();
	contents["modeloTabelaDinamica"] = make_shared<JSON_entrada_configuracaoInicial_modeloTabelaDinamica>();
	contents["modeloCp"] = make_shared<JSON_entrada_configuracaoInicial_modeloCp>();
	contents["modeloJTL"] = make_shared<JSON_entrada_configuracaoInicial_modeloJTL>();
	contents["tabP"] = make_shared<JSON_entrada_configuracaoInicial_tabP>();
	contents["AS"] = make_shared<JSON_entrada_configuracaoInicial_AS>();
	contents["paralelizaAS"] = make_shared<JSON_entrada_configuracaoInicial_paralelizaAS>();
	contents["trackRgo"] = make_shared<JSON_entrada_configuracaoInicial_trackRgo>();
	contents["trackDensidadeGas"] = make_shared<JSON_entrada_configuracaoInicial_trackDensidadeGas>();
	contents["correcaoDenGasLivreBlackOil"] = make_shared<JSON_entrada_configuracaoInicial_correcaoDenGasLivreBlackOil>();
	contents["tabelaRSPB"] = make_shared<JSON_entrada_configuracaoInicial_tabelaRSPB>();
	contents["propFluido"] = make_shared<JSON_entrada_configuracaoInicial_propFluido>();
	contents["iniFluidoP"] = make_shared<JSON_entrada_configuracaoInicial_iniFluidoP>();
	contents["tabG"] = make_shared<JSON_entrada_configuracaoInicial_tabG>();
	contents["escorregamentoPermanente"] = make_shared<JSON_entrada_configuracaoInicial_escorregamentoPermanente>();
	contents["escorregamentoTransiente"] = make_shared<JSON_entrada_configuracaoInicial_escorregamentoTransiente>();
	contents["mapaArranjo"] = make_shared<JSON_entrada_configuracaoInicial_mapaArranjo>();
	contents["condicaoInicial"] = make_shared<JSON_entrada_configuracaoInicial_condicaoInicial>();
	contents["ordemperm"] = make_shared<JSON_entrada_configuracaoInicial_ordemperm>();
	contents["SnapShotArq"] = make_shared<JSON_entrada_configuracaoInicial_SnapShotArq>();
	contents["HISEP"] = make_shared<JSON_entrada_configuracaoInicial_HISEP>(); //chris
	contents["SalinidadeFluido"] = make_shared<JSON_entrada_configuracaoInicial_SalinidadeFluido>();
	contents["comprimentoMedidoInterfaceLinhaGas"] = make_shared<JSON_entrada_configuracaoInicial_comprimentoMedidoInterfaceLinhaGas>();
	contents["comprimentoMedidoInterfaceLinhaProd"] = make_shared<JSON_entrada_configuracaoInicial_comprimentoMedidoInterfaceLinhaProd>();
	contents["controleDescarga"] = make_shared<JSON_entrada_configuracaoInicial_controleDescarga>();
	contents["parametrosDescarga"] = make_shared<JSON_entrada_configuracaoInicial_parametrosDescarga>();
	contents["transiente"] = make_shared<JSON_entrada_configuracaoInicial_transiente>();
	contents["transferenciaMassa"] = make_shared<JSON_entrada_configuracaoInicial_transferenciaMassa>();
	contents["CheckValve"] = make_shared<JSON_entrada_configuracaoInicial_CheckValve>();
	contents["Avancado"] = make_shared<JSON_entrada_configuracaoInicial_Avancado>();
	contents["condicaoPressao"] = make_shared<JSON_entrada_configuracaoInicial_condicaoPressao>();
	contents["condicaoVazPres"] = make_shared<JSON_entrada_configuracaoInicial_condicaoVazPres>();
	contents["correlacoesEscorregamento"] = make_shared<JSON_entrada_configuracaoInicial_correlacoesEscorregamento>();
	contents["correlacoesPorArranjo"] = make_shared<JSON_entrada_configuracaoInicial_correlacoesPorArranjo>();
	contents["Formacao"] = make_shared<JSON_entrada_configuracaoInicial_Formacao>();
	contents["tipoFluido"] = make_shared<JSON_entrada_configuracaoInicial_tipoFluido>();
	contents["tempReves"] = make_shared<JSON_entrada_configuracaoInicial_tempReves>();
	contents["razCompGasReves"] = make_shared<JSON_entrada_configuracaoInicial_razCompGasReves>();
	contents["chutePerm"] = make_shared<JSON_entrada_configuracaoInicial_chutePerm>();
	contents["modoXY"] = make_shared<JSON_entrada_configuracaoInicial_modoXY>();
	contents["xProdInicio"] = make_shared<JSON_entrada_configuracaoInicial_xProdInicio>();
	contents["yProdInicio"] = make_shared<JSON_entrada_configuracaoInicial_yProdInicio>();
	contents["xServInicio"] = make_shared<JSON_entrada_configuracaoInicial_xServInicio>();
	contents["yServInicio"] = make_shared<JSON_entrada_configuracaoInicial_yServInicio>();
	contents["modoParafina"] = make_shared<JSON_entrada_configuracaoInicial_modoParafina>();
	contents["tipoModeloDrift"] = make_shared<JSON_entrada_configuracaoInicial_tipoModeloDrift>();
	contents["modoDifus3D"] = make_shared<JSON_entrada_configuracaoInicial_modoDifus3D>();
	contents["threadP3D"] = make_shared<JSON_entrada_configuracaoInicial_threadP3D>();
	contents["modoDifus3DArq"] = make_shared<JSON_entrada_configuracaoInicial_modoDifus3DJson>();
	//contents["redutorAtritoCalc"] = make_shared<JSON_entrada_configuracaoInicial_redutorAtritoCalc>();
}

JSON_entrada_configuracaoInicial_origemGeometria& JSON_entrada_configuracaoInicial::origemGeometria(){
	return static_cast<JSON_entrada_configuracaoInicial_origemGeometria&>(*contents["origemGeometria"].get());
}

JSON_entrada_configuracaoInicial_sentidoGeometriaSegueEscoamento& JSON_entrada_configuracaoInicial::sentidoGeometriaSegueEscoamento(){
	return static_cast<JSON_entrada_configuracaoInicial_sentidoGeometriaSegueEscoamento&>(*contents["sentidoGeometriaSegueEscoamento"].get());
}

JSON_entrada_configuracaoInicial_saidaClassica& JSON_entrada_configuracaoInicial::saidaClassica(){
	return static_cast<JSON_entrada_configuracaoInicial_saidaClassica&>(*contents["saidaClassica"].get());
}

JSON_entrada_configuracaoInicial_linhaGas& JSON_entrada_configuracaoInicial::linhaGas(){
	return static_cast<JSON_entrada_configuracaoInicial_linhaGas&>(*contents["linhaGas"].get());
}

JSON_entrada_configuracaoInicial_saidaTela& JSON_entrada_configuracaoInicial::saidaTela(){
	return static_cast<JSON_entrada_configuracaoInicial_saidaTela&>(*contents["saidaTela"].get());
}

JSON_entrada_configuracaoInicial_equilibrioTermico& JSON_entrada_configuracaoInicial::equilibrioTermico(){
	return static_cast<JSON_entrada_configuracaoInicial_equilibrioTermico&>(*contents["equilibrioTermico"].get());
}

JSON_entrada_configuracaoInicial_latente& JSON_entrada_configuracaoInicial::latente(){
	return static_cast<JSON_entrada_configuracaoInicial_latente&>(*contents["latente"].get());
}

JSON_entrada_configuracaoInicial_condlatente& JSON_entrada_configuracaoInicial::condlatente(){
	return static_cast<JSON_entrada_configuracaoInicial_condlatente&>(*contents["condlatente"].get());
}

JSON_entrada_configuracaoInicial_pvtsimArq& JSON_entrada_configuracaoInicial::pvtsimArq(){
	return static_cast<JSON_entrada_configuracaoInicial_pvtsimArq&>(*contents["pvtsimArq"].get());
}

JSON_entrada_configuracaoInicial_modeloFluidoTabelaFlash& JSON_entrada_configuracaoInicial::modeloFluidoTabelaFlash(){
	return static_cast<JSON_entrada_configuracaoInicial_modeloFluidoTabelaFlash&>(*contents["modeloFluidoTabelaFlash"].get());
}

JSON_entrada_configuracaoInicial_modeloFluidoComposicional& JSON_entrada_configuracaoInicial::modeloFluidoComposicional(){
	return static_cast<JSON_entrada_configuracaoInicial_modeloFluidoComposicional&>(*contents["modeloFluidoComposicional"].get());
}

JSON_entrada_configuracaoInicial_modeloTabelaDinamica& JSON_entrada_configuracaoInicial::modeloTabelaDinamica(){
	return static_cast<JSON_entrada_configuracaoInicial_modeloTabelaDinamica&>(*contents["modeloTabelaDinamica"].get());
}

JSON_entrada_configuracaoInicial_modeloCp& JSON_entrada_configuracaoInicial::modeloCp(){
	return static_cast<JSON_entrada_configuracaoInicial_modeloCp&>(*contents["modeloCp"].get());
}

JSON_entrada_configuracaoInicial_modeloJTL& JSON_entrada_configuracaoInicial::modeloJTL(){
	return static_cast<JSON_entrada_configuracaoInicial_modeloJTL&>(*contents["modeloJTL"].get());
}

JSON_entrada_configuracaoInicial_tabP& JSON_entrada_configuracaoInicial::tabP(){
	return static_cast<JSON_entrada_configuracaoInicial_tabP&>(*contents["tabP"].get());
}

JSON_entrada_configuracaoInicial_AS& JSON_entrada_configuracaoInicial::AS(){
	return static_cast<JSON_entrada_configuracaoInicial_AS&>(*contents["AS"].get());
}

JSON_entrada_configuracaoInicial_paralelizaAS& JSON_entrada_configuracaoInicial::paralelizaAS(){
	return static_cast<JSON_entrada_configuracaoInicial_paralelizaAS&>(*contents["paralelizaAS"].get());
}

JSON_entrada_configuracaoInicial_trackRgo& JSON_entrada_configuracaoInicial::trackRgo(){
	return static_cast<JSON_entrada_configuracaoInicial_trackRgo&>(*contents["trackRgo"].get());
}

JSON_entrada_configuracaoInicial_trackDensidadeGas& JSON_entrada_configuracaoInicial::trackDensidadeGas(){
	return static_cast<JSON_entrada_configuracaoInicial_trackDensidadeGas&>(*contents["trackDensidadeGas"].get());
}

JSON_entrada_configuracaoInicial_correcaoDenGasLivreBlackOil& JSON_entrada_configuracaoInicial::correcaoDenGasLivreBlackOil(){
	return static_cast<JSON_entrada_configuracaoInicial_correcaoDenGasLivreBlackOil&>(*contents["correcaoDenGasLivreBlackOil"].get());
}

JSON_entrada_configuracaoInicial_tabelaRSPB& JSON_entrada_configuracaoInicial::tabelaRSPB(){
	return static_cast<JSON_entrada_configuracaoInicial_tabelaRSPB&>(*contents["tabelaRSPB"].get());
}

JSON_entrada_configuracaoInicial_propFluido& JSON_entrada_configuracaoInicial::propFluido(){
	return static_cast<JSON_entrada_configuracaoInicial_propFluido&>(*contents["propFluido"].get());
}

JSON_entrada_configuracaoInicial_iniFluidoP& JSON_entrada_configuracaoInicial::iniFluidoP(){
	return static_cast<JSON_entrada_configuracaoInicial_iniFluidoP&>(*contents["iniFluidoP"].get());
}

JSON_entrada_configuracaoInicial_tabG& JSON_entrada_configuracaoInicial::tabG(){
	return static_cast<JSON_entrada_configuracaoInicial_tabG&>(*contents["tabG"].get());
}

JSON_entrada_configuracaoInicial_escorregamentoPermanente& JSON_entrada_configuracaoInicial::escorregamentoPermanente(){
	return static_cast<JSON_entrada_configuracaoInicial_escorregamentoPermanente&>(*contents["escorregamentoPermanente"].get());
}

JSON_entrada_configuracaoInicial_escorregamentoTransiente& JSON_entrada_configuracaoInicial::escorregamentoTransiente(){
	return static_cast<JSON_entrada_configuracaoInicial_escorregamentoTransiente&>(*contents["escorregamentoTransiente"].get());
}

JSON_entrada_configuracaoInicial_mapaArranjo& JSON_entrada_configuracaoInicial::mapaArranjo(){
	return static_cast<JSON_entrada_configuracaoInicial_mapaArranjo&>(*contents["mapaArranjo"].get());
}


JSON_entrada_configuracaoInicial_condicaoInicial& JSON_entrada_configuracaoInicial::condicaoInicial(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoInicial&>(*contents["condicaoInicial"].get());
}

JSON_entrada_configuracaoInicial_ordemperm& JSON_entrada_configuracaoInicial::ordemperm(){
	return static_cast<JSON_entrada_configuracaoInicial_ordemperm&>(*contents["ordemperm"].get());
}

JSON_entrada_configuracaoInicial_SnapShotArq& JSON_entrada_configuracaoInicial::SnapShotArq(){
	return static_cast<JSON_entrada_configuracaoInicial_SnapShotArq&>(*contents["SnapShotArq"].get());
}

JSON_entrada_configuracaoInicial_HISEP& JSON_entrada_configuracaoInicial::HISEP(){
	return static_cast<JSON_entrada_configuracaoInicial_HISEP&>(*contents["HISEP"].get());
}

JSON_entrada_configuracaoInicial_SalinidadeFluido& JSON_entrada_configuracaoInicial::SalinidadeFluido(){
	return static_cast<JSON_entrada_configuracaoInicial_SalinidadeFluido&>(*contents["SalinidadeFluido"].get());
}

JSON_entrada_configuracaoInicial_comprimentoMedidoInterfaceLinhaGas& JSON_entrada_configuracaoInicial::comprimentoMedidoInterfaceLinhaGas(){
	return static_cast<JSON_entrada_configuracaoInicial_comprimentoMedidoInterfaceLinhaGas&>(*contents["comprimentoMedidoInterfaceLinhaGas"].get());
}

JSON_entrada_configuracaoInicial_comprimentoMedidoInterfaceLinhaProd& JSON_entrada_configuracaoInicial::comprimentoMedidoInterfaceLinhaProd(){
	return static_cast<JSON_entrada_configuracaoInicial_comprimentoMedidoInterfaceLinhaProd&>(*contents["comprimentoMedidoInterfaceLinhaProd"].get());
}

JSON_entrada_configuracaoInicial_controleDescarga& JSON_entrada_configuracaoInicial::controleDescarga(){
	return static_cast<JSON_entrada_configuracaoInicial_controleDescarga&>(*contents["controleDescarga"].get());
}

JSON_entrada_configuracaoInicial_parametrosDescarga& JSON_entrada_configuracaoInicial::parametrosDescarga(){
	return static_cast<JSON_entrada_configuracaoInicial_parametrosDescarga&>(*contents["parametrosDescarga"].get());
}

JSON_entrada_configuracaoInicial_transiente& JSON_entrada_configuracaoInicial::transiente(){
	return static_cast<JSON_entrada_configuracaoInicial_transiente&>(*contents["transiente"].get());
}

JSON_entrada_configuracaoInicial_transferenciaMassa& JSON_entrada_configuracaoInicial::transferenciaMassa(){
	return static_cast<JSON_entrada_configuracaoInicial_transferenciaMassa&>(*contents["transferenciaMassa"].get());
}

JSON_entrada_configuracaoInicial_CheckValve& JSON_entrada_configuracaoInicial::CheckValve(){
	return static_cast<JSON_entrada_configuracaoInicial_CheckValve&>(*contents["CheckValve"].get());
}

JSON_entrada_configuracaoInicial_Avancado& JSON_entrada_configuracaoInicial::Avancado(){
	return static_cast<JSON_entrada_configuracaoInicial_Avancado&>(*contents["Avancado"].get());
}

JSON_entrada_configuracaoInicial_condicaoPressao& JSON_entrada_configuracaoInicial::condicaoPressao(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoPressao&>(*contents["condicaoPressao"].get());
}

JSON_entrada_configuracaoInicial_condicaoVazPres& JSON_entrada_configuracaoInicial::condicaoVazPres(){
	return static_cast<JSON_entrada_configuracaoInicial_condicaoVazPres&>(*contents["condicaoVazPres"].get());
}

JSON_entrada_configuracaoInicial_correlacoesEscorregamento& JSON_entrada_configuracaoInicial::correlacoesEscorregamento(){
	return static_cast<JSON_entrada_configuracaoInicial_correlacoesEscorregamento&>(*contents["correlacoesEscorregamento"].get());
}

JSON_entrada_configuracaoInicial_correlacoesPorArranjo& JSON_entrada_configuracaoInicial::correlacoesPorArranjo(){
	return static_cast<JSON_entrada_configuracaoInicial_correlacoesPorArranjo&>(*contents["correlacoesPorArranjo"].get());
}

JSON_entrada_configuracaoInicial_Formacao& JSON_entrada_configuracaoInicial::Formacao(){
	return static_cast<JSON_entrada_configuracaoInicial_Formacao&>(*contents["Formacao"].get());
}

JSON_entrada_configuracaoInicial_tipoFluido& JSON_entrada_configuracaoInicial::tipoFluido(){
	return static_cast<JSON_entrada_configuracaoInicial_tipoFluido&>(*contents["tipoFluido"].get());
}

JSON_entrada_configuracaoInicial_tempReves& JSON_entrada_configuracaoInicial::tempReves(){
	return static_cast<JSON_entrada_configuracaoInicial_tempReves&>(*contents["tempReves"].get());
}

JSON_entrada_configuracaoInicial_razCompGasReves& JSON_entrada_configuracaoInicial::razCompGasReves(){
	return static_cast<JSON_entrada_configuracaoInicial_razCompGasReves&>(*contents["razCompGasReves"].get());
}

JSON_entrada_configuracaoInicial_chutePerm& JSON_entrada_configuracaoInicial::chutePerm(){
	return static_cast<JSON_entrada_configuracaoInicial_chutePerm&>(*contents["chutePerm"].get());
}

JSON_entrada_configuracaoInicial_modoXY& JSON_entrada_configuracaoInicial::modoXY(){
	return static_cast<JSON_entrada_configuracaoInicial_modoXY&>(*contents["modoXY"].get());
}

JSON_entrada_configuracaoInicial_xProdInicio& JSON_entrada_configuracaoInicial::xProdInicio(){
	return static_cast<JSON_entrada_configuracaoInicial_xProdInicio&>(*contents["xProdInicio"].get());
}

JSON_entrada_configuracaoInicial_yProdInicio& JSON_entrada_configuracaoInicial::yProdInicio(){
	return static_cast<JSON_entrada_configuracaoInicial_yProdInicio&>(*contents["yProdInicio"].get());
}

JSON_entrada_configuracaoInicial_xServInicio& JSON_entrada_configuracaoInicial::xServInicio(){
	return static_cast<JSON_entrada_configuracaoInicial_xServInicio&>(*contents["xServInicio"].get());
}

JSON_entrada_configuracaoInicial_yServInicio& JSON_entrada_configuracaoInicial::yServInicio(){
	return static_cast<JSON_entrada_configuracaoInicial_yServInicio&>(*contents["yServInicio"].get());
}

JSON_entrada_configuracaoInicial_modoParafina& JSON_entrada_configuracaoInicial::modoParafina(){
	return static_cast<JSON_entrada_configuracaoInicial_modoParafina&>(*contents["modoParafina"].get());
}

JSON_entrada_configuracaoInicial_tipoModeloDrift& JSON_entrada_configuracaoInicial::tipoModeloDrift(){
	return static_cast<JSON_entrada_configuracaoInicial_tipoModeloDrift&>(*contents["tipoModeloDrift"].get());
}

JSON_entrada_configuracaoInicial_modoDifus3D& JSON_entrada_configuracaoInicial::modoDifus3D(){
	return static_cast<JSON_entrada_configuracaoInicial_modoDifus3D&>(*contents["modoDifus3D"].get());
}

JSON_entrada_configuracaoInicial_threadP3D& JSON_entrada_configuracaoInicial::threadP3D(){
	return static_cast<JSON_entrada_configuracaoInicial_threadP3D&>(*contents["threadP3D"].get());
}

JSON_entrada_configuracaoInicial_modoDifus3DJson& JSON_entrada_configuracaoInicial::modoDifus3DJson(){
	return static_cast<JSON_entrada_configuracaoInicial_modoDifus3DJson&>(*contents["modoDifus3DArq"].get());
}

//JSON_entrada_configuracaoInicial_redutorAtritoCalc& JSON_entrada_configuracaoInicial::redutorAtritoCalc(){
	//return static_cast<JSON_entrada_configuracaoInicial_redutorAtritoCalc&>(*contents["redutorAtritoCalc"].get());
//}

JSON_entrada_tabela::JSON_entrada_tabela(){
	contents["ativo"] = make_shared<JSON_entrada_tabela_ativo>();
	contents["nPontos"] = make_shared<JSON_entrada_tabela_nPontos>();
	contents["pressaoMaxima"] = make_shared<JSON_entrada_tabela_pressaoMaxima>();
	contents["pressaoMinima"] = make_shared<JSON_entrada_tabela_pressaoMinima>();
	contents["temperaturaMaxima"] = make_shared<JSON_entrada_tabela_temperaturaMaxima>();
	contents["temperaturaMinima"] = make_shared<JSON_entrada_tabela_temperaturaMinima>();
}

JSON_entrada_tabela_ativo& JSON_entrada_tabela::ativo(){
	return static_cast<JSON_entrada_tabela_ativo&>(*contents["ativo"].get());
}

JSON_entrada_tabela_nPontos& JSON_entrada_tabela::nPontos(){
	return static_cast<JSON_entrada_tabela_nPontos&>(*contents["nPontos"].get());
}

JSON_entrada_tabela_pressaoMaxima& JSON_entrada_tabela::pressaoMaxima(){
	return static_cast<JSON_entrada_tabela_pressaoMaxima&>(*contents["pressaoMaxima"].get());
}

JSON_entrada_tabela_pressaoMinima& JSON_entrada_tabela::pressaoMinima(){
	return static_cast<JSON_entrada_tabela_pressaoMinima&>(*contents["pressaoMinima"].get());
}

JSON_entrada_tabela_temperaturaMaxima& JSON_entrada_tabela::temperaturaMaxima(){
	return static_cast<JSON_entrada_tabela_temperaturaMaxima&>(*contents["temperaturaMaxima"].get());
}

JSON_entrada_tabela_temperaturaMinima& JSON_entrada_tabela::temperaturaMinima(){
	return static_cast<JSON_entrada_tabela_temperaturaMinima&>(*contents["temperaturaMinima"].get());
}


JSON_entrada_parafina::JSON_entrada_parafina(){
	contents["arquivoWax"] = make_shared<JSON_entrada_parafina_arquivoWax>();
	contents["usuarioPorosidade"] = make_shared<JSON_entrada_parafina_usuarioPorosidade>();
	contents["porosidade"] = make_shared<JSON_entrada_parafina_porosidade>();
	contents["usuarioC2C3"] = make_shared<JSON_entrada_parafina_usuarioC2C3>();
	contents["c2"] = make_shared<JSON_entrada_parafina_c2>();
	contents["c3"] = make_shared<JSON_entrada_parafina_c3>();
	contents["usuarioDifus"] = make_shared<JSON_entrada_parafina_usuarioDifus>();
	contents["alteraViscFlu"] = make_shared<JSON_entrada_parafina_alteraViscFlu>();
	contents["difus"] = make_shared<JSON_entrada_parafina_difus>();
	contents["rugosidade"] = make_shared<JSON_entrada_parafina_rugosidade>();
	contents["multiplicadorViscosidade"] = make_shared<JSON_entrada_parafina_multVis>();
	contents["DmultipWax"] = make_shared<JSON_entrada_parafina_DmultipWax>();
	contents["EmultipWax"] = make_shared<JSON_entrada_parafina_EmultipWax>();
	contents["FmultipWax"] = make_shared<JSON_entrada_parafina_FmultipWax>();
}

JSON_entrada_parafina_arquivoWax& JSON_entrada_parafina::arquivoWax(){
	return static_cast<JSON_entrada_parafina_arquivoWax&>(*contents["arquivoWax"].get());
}

JSON_entrada_parafina_usuarioPorosidade& JSON_entrada_parafina::usuarioPorosidade(){
	return static_cast<JSON_entrada_parafina_usuarioPorosidade&>(*contents["usuarioPorosidade"].get());
}

JSON_entrada_parafina_porosidade& JSON_entrada_parafina::porosidade(){
	return static_cast<JSON_entrada_parafina_porosidade&>(*contents["porosidade"].get());
}

JSON_entrada_parafina_usuarioC2C3& JSON_entrada_parafina::usuarioC2C3(){
	return static_cast<JSON_entrada_parafina_usuarioC2C3&>(*contents["usuarioC2C3"].get());
}

JSON_entrada_parafina_c2& JSON_entrada_parafina::c2(){
	return static_cast<JSON_entrada_parafina_c2&>(*contents["c2"].get());
}

JSON_entrada_parafina_c3& JSON_entrada_parafina::c3(){
	return static_cast<JSON_entrada_parafina_c3&>(*contents["c3"].get());
}

JSON_entrada_parafina_usuarioDifus& JSON_entrada_parafina::usuarioDifus(){
	return static_cast<JSON_entrada_parafina_usuarioDifus&>(*contents["usuarioDifus"].get());
}

JSON_entrada_parafina_alteraViscFlu& JSON_entrada_parafina::alteraViscFlu(){
	return static_cast<JSON_entrada_parafina_alteraViscFlu&>(*contents["alteraViscFlu"].get());
}

JSON_entrada_parafina_difus& JSON_entrada_parafina::difus(){
	return static_cast<JSON_entrada_parafina_difus&>(*contents["difus"].get());
}

JSON_entrada_parafina_rugosidade& JSON_entrada_parafina::rugosidade(){
	return static_cast<JSON_entrada_parafina_rugosidade&>(*contents["rugosidade"].get());
}

JSON_entrada_parafina_multVis& JSON_entrada_parafina::multVis(){
	return static_cast<JSON_entrada_parafina_multVis&>(*contents["multiplicadorViscosidade"].get());
}

JSON_entrada_parafina_DmultipWax& JSON_entrada_parafina::DmultipWax(){
	return static_cast<JSON_entrada_parafina_DmultipWax&>(*contents["DmultipWax"].get());
}

JSON_entrada_parafina_EmultipWax& JSON_entrada_parafina::EmultipWax(){
	return static_cast<JSON_entrada_parafina_EmultipWax&>(*contents["EmultipWax"].get());
}

JSON_entrada_parafina_FmultipWax& JSON_entrada_parafina::FmultipWax(){
	return static_cast<JSON_entrada_parafina_FmultipWax&>(*contents["FmultipWax"].get());
}


JSON_entrada_fluidoGas::JSON_entrada_fluidoGas(){
	contents["ativo"] = make_shared<JSON_entrada_fluidoGas_ativo>();
	contents["densidadeGas"] = make_shared<JSON_entrada_fluidoGas_densidadeGas>();
	contents["fracCO2"] = make_shared<JSON_entrada_fluidoGas_fracCO2>();
	contents["correlacaoCritica"] = make_shared<JSON_entrada_fluidoGas_correlacaoCritica>();
	contents["fracMolarUsuario"] = make_shared<JSON_entrada_fluidoGas_fracMolarUsuario>();
	contents["fracMolar"] = make_shared<JSON_entrada_fluidoGas_fracMolar>();
}

JSON_entrada_fluidoGas_ativo& JSON_entrada_fluidoGas::ativo(){
	return static_cast<JSON_entrada_fluidoGas_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fluidoGas_densidadeGas& JSON_entrada_fluidoGas::densidadeGas(){
	return static_cast<JSON_entrada_fluidoGas_densidadeGas&>(*contents["densidadeGas"].get());
}

JSON_entrada_fluidoGas_fracCO2& JSON_entrada_fluidoGas::fracCO2(){
	return static_cast<JSON_entrada_fluidoGas_fracCO2&>(*contents["fracCO2"].get());
}

JSON_entrada_fluidoGas_correlacaoCritica& JSON_entrada_fluidoGas::correlacaoCritica(){
	return static_cast<JSON_entrada_fluidoGas_correlacaoCritica&>(*contents["correlacaoCritica"].get());
}

JSON_entrada_fluidoGas_fracMolarUsuario& JSON_entrada_fluidoGas::fracMolarUsuario(){
	return static_cast<JSON_entrada_fluidoGas_fracMolarUsuario&>(*contents["fracMolarUsuario"].get());
}

JSON_entrada_fluidoGas_fracMolar& JSON_entrada_fluidoGas::fracMolar(){
	return static_cast<JSON_entrada_fluidoGas_fracMolar&>(*contents["fracMolar"].get());
}

JSON_entrada_fluidosProducao_Item::JSON_entrada_fluidosProducao_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fluidosProducao_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_fluidosProducao_Item_id>();
	contents["api"] = make_shared<JSON_entrada_fluidosProducao_Item_api>();
	contents["rgo"] = make_shared<JSON_entrada_fluidosProducao_Item_rgo>();
	contents["densidadeGas"] = make_shared<JSON_entrada_fluidosProducao_Item_densidadeGas>();
	contents["bsw"] = make_shared<JSON_entrada_fluidosProducao_Item_bsw>();
	contents["densidadeAgua"] = make_shared<JSON_entrada_fluidosProducao_Item_densidadeAgua>();
	contents["tipoEmul"] = make_shared<JSON_entrada_fluidosProducao_Item_tipoEmul>();
	contents["coefAModeloExp"] = make_shared<JSON_entrada_fluidosProducao_Item_coefAModeloExp>();
	contents["coefBModeloExp"] = make_shared<JSON_entrada_fluidosProducao_Item_coefBModeloExp>();
	contents["PHI100"] = make_shared<JSON_entrada_fluidosProducao_Item_PHI100>();
	contents["BSWVec"] = make_shared<JSON_entrada_fluidosProducao_Item_BSWVec>();
	contents["emulVec"] = make_shared<JSON_entrada_fluidosProducao_Item_emulVec>();
	contents["bswCorte"] = make_shared<JSON_entrada_fluidosProducao_Item_bswCorte>();
	contents["fracCO2"] = make_shared<JSON_entrada_fluidosProducao_Item_fracCO2>();
	contents["correlacaoCritica"] = make_shared<JSON_entrada_fluidosProducao_Item_correlacaoCritica>();
	contents["modeloRsPb"] = make_shared<JSON_entrada_fluidosProducao_Item_modeloRsPb>();
	contents["modeloOleoMorto"] = make_shared<JSON_entrada_fluidosProducao_Item_modeloOleoMorto>();
	contents["temp1"] = make_shared<JSON_entrada_fluidosProducao_Item_temp1>();
	contents["visc1"] = make_shared<JSON_entrada_fluidosProducao_Item_visc1>();
	contents["temp2"] = make_shared<JSON_entrada_fluidosProducao_Item_temp2>();
	contents["visc2"] = make_shared<JSON_entrada_fluidosProducao_Item_visc2>();
	contents["tempOleoMorto"] = make_shared<JSON_entrada_fluidosProducao_Item_tempOleoMorto>();
	contents["viscOleoMorto"] = make_shared<JSON_entrada_fluidosProducao_Item_viscOleoMorto>();
	contents["modeloOleoVivo"] = make_shared<JSON_entrada_fluidosProducao_Item_modeloOleoVivo>();
	contents["modeloOleoSubSaturado"] = make_shared<JSON_entrada_fluidosProducao_Item_modeloOleoSubSaturado>();
	contents["modeloViscBlackOil"] = make_shared<JSON_entrada_fluidosProducao_Item_modeloViscBlackOil>();
	contents["modeloAguaBlackOil"] = make_shared<JSON_entrada_fluidosProducao_Item_modeloAguaBlackOil>();
	contents["fracMolarUsuario"] = make_shared<JSON_entrada_fluidosProducao_Item_fracMolarUsuario>();
	contents["fracMolar"] = make_shared<JSON_entrada_fluidosProducao_Item_fracMolar>();
	contents["RGOCompUsuario"] = make_shared<JSON_entrada_fluidosProducao_Item_RGOCompUsuario>();
}

JSON_entrada_fluidosProducao_Item_ativo& JSON_entrada_fluidosProducao_Item::ativo(){
	return static_cast<JSON_entrada_fluidosProducao_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fluidosProducao_Item_id& JSON_entrada_fluidosProducao_Item::id(){
	return static_cast<JSON_entrada_fluidosProducao_Item_id&>(*contents["id"].get());
}

JSON_entrada_fluidosProducao_Item_api& JSON_entrada_fluidosProducao_Item::api(){
	return static_cast<JSON_entrada_fluidosProducao_Item_api&>(*contents["api"].get());
}

JSON_entrada_fluidosProducao_Item_rgo& JSON_entrada_fluidosProducao_Item::rgo(){
	return static_cast<JSON_entrada_fluidosProducao_Item_rgo&>(*contents["rgo"].get());
}

JSON_entrada_fluidosProducao_Item_densidadeGas& JSON_entrada_fluidosProducao_Item::densidadeGas(){
	return static_cast<JSON_entrada_fluidosProducao_Item_densidadeGas&>(*contents["densidadeGas"].get());
}

JSON_entrada_fluidosProducao_Item_bsw& JSON_entrada_fluidosProducao_Item::bsw(){
	return static_cast<JSON_entrada_fluidosProducao_Item_bsw&>(*contents["bsw"].get());
}

JSON_entrada_fluidosProducao_Item_densidadeAgua& JSON_entrada_fluidosProducao_Item::densidadeAgua(){
	return static_cast<JSON_entrada_fluidosProducao_Item_densidadeAgua&>(*contents["densidadeAgua"].get());
}

JSON_entrada_fluidosProducao_Item_tipoEmul& JSON_entrada_fluidosProducao_Item::tipoEmul(){
	return static_cast<JSON_entrada_fluidosProducao_Item_tipoEmul&>(*contents["tipoEmul"].get());
}

JSON_entrada_fluidosProducao_Item_coefAModeloExp& JSON_entrada_fluidosProducao_Item::coefAModeloExp(){
	return static_cast<JSON_entrada_fluidosProducao_Item_coefAModeloExp&>(*contents["coefAModeloExp"].get());
}

JSON_entrada_fluidosProducao_Item_coefBModeloExp& JSON_entrada_fluidosProducao_Item::coefBModeloExp(){
	return static_cast<JSON_entrada_fluidosProducao_Item_coefBModeloExp&>(*contents["coefBModeloExp"].get());
}

JSON_entrada_fluidosProducao_Item_bswCorte& JSON_entrada_fluidosProducao_Item::bswCorte(){
	return static_cast<JSON_entrada_fluidosProducao_Item_bswCorte&>(*contents["bswCorte"].get());
}

JSON_entrada_fluidosProducao_Item_coefAModeloExp& JSON_entrada_fluidosProducao_Item::PHI100(){
	return static_cast<JSON_entrada_fluidosProducao_Item_PHI100&>(*contents["PHI100"].get());
}

JSON_entrada_fluidosProducao_Item_BSWVec& JSON_entrada_fluidosProducao_Item::BSWVec(){
	return static_cast<JSON_entrada_fluidosProducao_Item_BSWVec&>(*contents["BSWVec"].get());
}

JSON_entrada_fluidosProducao_Item_emulVec& JSON_entrada_fluidosProducao_Item::emulVec(){
	return static_cast<JSON_entrada_fluidosProducao_Item_emulVec&>(*contents["emulVec"].get());
}

JSON_entrada_fluidosProducao_Item_fracCO2& JSON_entrada_fluidosProducao_Item::fracCO2(){
	return static_cast<JSON_entrada_fluidosProducao_Item_fracCO2&>(*contents["fracCO2"].get());
}

JSON_entrada_fluidosProducao_Item_correlacaoCritica& JSON_entrada_fluidosProducao_Item::correlacaoCritica(){
	return static_cast<JSON_entrada_fluidosProducao_Item_correlacaoCritica&>(*contents["correlacaoCritica"].get());
}

JSON_entrada_fluidosProducao_Item_modeloRsPb& JSON_entrada_fluidosProducao_Item::modeloRsPb(){
	return static_cast<JSON_entrada_fluidosProducao_Item_modeloRsPb&>(*contents["modeloRsPb"].get());
}

JSON_entrada_fluidosProducao_Item_modeloOleoMorto& JSON_entrada_fluidosProducao_Item::modeloOleoMorto(){
	return static_cast<JSON_entrada_fluidosProducao_Item_modeloOleoMorto&>(*contents["modeloOleoMorto"].get());
}

JSON_entrada_fluidosProducao_Item_temp1& JSON_entrada_fluidosProducao_Item::temp1(){
	return static_cast<JSON_entrada_fluidosProducao_Item_temp1&>(*contents["temp1"].get());
}

JSON_entrada_fluidosProducao_Item_visc1& JSON_entrada_fluidosProducao_Item::visc1(){
	return static_cast<JSON_entrada_fluidosProducao_Item_visc1&>(*contents["visc1"].get());
}

JSON_entrada_fluidosProducao_Item_temp2& JSON_entrada_fluidosProducao_Item::temp2(){
	return static_cast<JSON_entrada_fluidosProducao_Item_temp2&>(*contents["temp2"].get());
}

JSON_entrada_fluidosProducao_Item_visc2& JSON_entrada_fluidosProducao_Item::visc2(){
	return static_cast<JSON_entrada_fluidosProducao_Item_visc2&>(*contents["visc2"].get());
}

JSON_entrada_fluidosProducao_Item_tempOleoMorto& JSON_entrada_fluidosProducao_Item::tempOleoMorto(){
	return static_cast<JSON_entrada_fluidosProducao_Item_tempOleoMorto&>(*contents["tempOleoMorto"].get());
}

JSON_entrada_fluidosProducao_Item_viscOleoMorto& JSON_entrada_fluidosProducao_Item::viscOleoMorto(){
	return static_cast<JSON_entrada_fluidosProducao_Item_viscOleoMorto&>(*contents["viscOleoMorto"].get());
}

JSON_entrada_fluidosProducao_Item_modeloOleoVivo& JSON_entrada_fluidosProducao_Item::modeloOleoVivo(){
	return static_cast<JSON_entrada_fluidosProducao_Item_modeloOleoVivo&>(*contents["modeloOleoVivo"].get());
}

JSON_entrada_fluidosProducao_Item_modeloOleoSubSaturado& JSON_entrada_fluidosProducao_Item::modeloOleoSubSaturado(){
	return static_cast<JSON_entrada_fluidosProducao_Item_modeloOleoSubSaturado&>(*contents["modeloOleoSubSaturado"].get());
}

JSON_entrada_fluidosProducao_Item_modeloViscBlackOil& JSON_entrada_fluidosProducao_Item::modeloViscBlackOil(){
	return static_cast<JSON_entrada_fluidosProducao_Item_modeloViscBlackOil&>(*contents["modeloViscBlackOil"].get());
}

JSON_entrada_fluidosProducao_Item_modeloAguaBlackOil& JSON_entrada_fluidosProducao_Item::modeloAguaBlackOil(){
	return static_cast<JSON_entrada_fluidosProducao_Item_modeloAguaBlackOil&>(*contents["modeloAguaBlackOil"].get());
}

JSON_entrada_fluidosProducao_Item_fracMolarUsuario& JSON_entrada_fluidosProducao_Item::fracMolarUsuario(){
	return static_cast<JSON_entrada_fluidosProducao_Item_fracMolarUsuario&>(*contents["fracMolarUsuario"].get());
}

JSON_entrada_fluidosProducao_Item_fracMolar& JSON_entrada_fluidosProducao_Item::fracMolar(){
	return static_cast<JSON_entrada_fluidosProducao_Item_fracMolar&>(*contents["fracMolar"].get());
}

JSON_entrada_fluidosProducao_Item_RGOCompUsuario& JSON_entrada_fluidosProducao_Item::RGOCompUsuario(){
	return static_cast<JSON_entrada_fluidosProducao_Item_RGOCompUsuario&>(*contents["RGOCompUsuario"].get());
}

JSON_entrada_fluidoComplementar::JSON_entrada_fluidoComplementar(){
	contents["ativo"] = make_shared<JSON_entrada_fluidoComplementar_ativo>();
	contents["massaEspecifica"] = make_shared<JSON_entrada_fluidoComplementar_massaEspecifica>();
	contents["compP"] = make_shared<JSON_entrada_fluidoComplementar_compP>();
	contents["compT"] = make_shared<JSON_entrada_fluidoComplementar_compT>();
	contents["tensup"] = make_shared<JSON_entrada_fluidoComplementar_tensup>();
	contents["calorEspecifico"] = make_shared<JSON_entrada_fluidoComplementar_calorEspecifico>();
	contents["condutividade"] = make_shared<JSON_entrada_fluidoComplementar_condutividade>();
	contents["temp1"] = make_shared<JSON_entrada_fluidoComplementar_temp1>();
	contents["visc1"] = make_shared<JSON_entrada_fluidoComplementar_visc1>();
	contents["temp2"] = make_shared<JSON_entrada_fluidoComplementar_temp2>();
	contents["visc2"] = make_shared<JSON_entrada_fluidoComplementar_visc2>();
	contents["salinidade"] = make_shared<JSON_entrada_fluidoComplementar_salinidade>();
	contents["tipoFluido"] = make_shared<JSON_entrada_fluidoComplementar_tipoF>();
}

JSON_entrada_fluidoComplementar_ativo& JSON_entrada_fluidoComplementar::ativo(){
	return static_cast<JSON_entrada_fluidoComplementar_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fluidoComplementar_massaEspecifica& JSON_entrada_fluidoComplementar::massaEspecifica(){
	return static_cast<JSON_entrada_fluidoComplementar_massaEspecifica&>(*contents["massaEspecifica"].get());
}

JSON_entrada_fluidoComplementar_compP& JSON_entrada_fluidoComplementar::compP(){
	return static_cast<JSON_entrada_fluidoComplementar_compP&>(*contents["compP"].get());
}

JSON_entrada_fluidoComplementar_compT& JSON_entrada_fluidoComplementar::compT(){
	return static_cast<JSON_entrada_fluidoComplementar_compT&>(*contents["compT"].get());
}

JSON_entrada_fluidoComplementar_tensup& JSON_entrada_fluidoComplementar::tensup(){
	return static_cast<JSON_entrada_fluidoComplementar_tensup&>(*contents["tensup"].get());
}

JSON_entrada_fluidoComplementar_calorEspecifico& JSON_entrada_fluidoComplementar::calorEspecifico(){
	return static_cast<JSON_entrada_fluidoComplementar_calorEspecifico&>(*contents["calorEspecifico"].get());
}

JSON_entrada_fluidoComplementar_condutividade& JSON_entrada_fluidoComplementar::condutividade(){
	return static_cast<JSON_entrada_fluidoComplementar_condutividade&>(*contents["condutividade"].get());
}

JSON_entrada_fluidoComplementar_temp1& JSON_entrada_fluidoComplementar::temp1(){
	return static_cast<JSON_entrada_fluidoComplementar_temp1&>(*contents["temp1"].get());
}

JSON_entrada_fluidoComplementar_visc1& JSON_entrada_fluidoComplementar::visc1(){
	return static_cast<JSON_entrada_fluidoComplementar_visc1&>(*contents["visc1"].get());
}

JSON_entrada_fluidoComplementar_temp2& JSON_entrada_fluidoComplementar::temp2(){
	return static_cast<JSON_entrada_fluidoComplementar_temp2&>(*contents["temp2"].get());
}

JSON_entrada_fluidoComplementar_visc2& JSON_entrada_fluidoComplementar::visc2(){
	return static_cast<JSON_entrada_fluidoComplementar_visc2&>(*contents["visc2"].get());
}

JSON_entrada_fluidoComplementar_salinidade& JSON_entrada_fluidoComplementar::salinidade(){
	return static_cast<JSON_entrada_fluidoComplementar_salinidade&>(*contents["salinidade"].get());
}

JSON_entrada_fluidoComplementar_tipoF& JSON_entrada_fluidoComplementar::tipoF(){
	return static_cast<JSON_entrada_fluidoComplementar_tipoF&>(*contents["tipoFluido"].get());
}

JSON_entrada_valvula_Item::JSON_entrada_valvula_Item(){
	contents["ativo"] = make_shared<JSON_entrada_valvula_Item_ativo>();
	contents["curvaCV"] = make_shared<JSON_entrada_valvula_Item_curvaCV>();
	contents["curvaDinamic"] = make_shared<JSON_entrada_valvula_Item_curvaDinamic>();
	contents["id"] = make_shared<JSON_entrada_valvula_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_valvula_Item_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_valvula_Item_tempo>();
	contents["abertura"] = make_shared<JSON_entrada_valvula_Item_abertura>();
	contents["cd"] = make_shared<JSON_entrada_valvula_Item_cd>();
	contents["x1"] = make_shared<JSON_entrada_valvula_Item_x1>();
	contents["cv1"] = make_shared<JSON_entrada_valvula_Item_cv1>();
}

JSON_entrada_valvula_Item_ativo& JSON_entrada_valvula_Item::ativo(){
	return static_cast<JSON_entrada_valvula_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_valvula_Item_curvaCV& JSON_entrada_valvula_Item::curvaCV(){
	return static_cast<JSON_entrada_valvula_Item_curvaCV&>(*contents["curvaCV"].get());
}

JSON_entrada_valvula_Item_curvaDinamic& JSON_entrada_valvula_Item::curvaDinamic(){
	return static_cast<JSON_entrada_valvula_Item_curvaDinamic&>(*contents["curvaDinamic"].get());
}

JSON_entrada_valvula_Item_id& JSON_entrada_valvula_Item::id(){
	return static_cast<JSON_entrada_valvula_Item_id&>(*contents["id"].get());
}

JSON_entrada_valvula_Item_comprimentoMedido& JSON_entrada_valvula_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_valvula_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_valvula_Item_tempo& JSON_entrada_valvula_Item::tempo(){
	return static_cast<JSON_entrada_valvula_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_valvula_Item_abertura& JSON_entrada_valvula_Item::abertura(){
	return static_cast<JSON_entrada_valvula_Item_abertura&>(*contents["abertura"].get());
}

JSON_entrada_valvula_Item_cd& JSON_entrada_valvula_Item::cd(){
	return static_cast<JSON_entrada_valvula_Item_cd&>(*contents["cd"].get());
}

JSON_entrada_valvula_Item_x1& JSON_entrada_valvula_Item::x1(){
	return static_cast<JSON_entrada_valvula_Item_x1&>(*contents["x1"].get());
}

JSON_entrada_valvula_Item_cv1& JSON_entrada_valvula_Item::cv1(){
	return static_cast<JSON_entrada_valvula_Item_cv1&>(*contents["cv1"].get());
}

JSON_entrada_fonteLiquido_Item::JSON_entrada_fonteLiquido_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fonteLiquido_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_fonteLiquido_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_fonteLiquido_Item_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_fonteLiquido_Item_tempo>();
	contents["temperatura"] = make_shared<JSON_entrada_fonteLiquido_Item_temperatura>();
	contents["beta"] = make_shared<JSON_entrada_fonteLiquido_Item_beta>();
	contents["vazaoLiquido"] = make_shared<JSON_entrada_fonteLiquido_Item_vazaoLiquido>();
	contents["indiFluidoPro"] = make_shared<JSON_entrada_fonteLiquido_Item_indiFluidoPro>();
}

JSON_entrada_fonteLiquido_Item_ativo& JSON_entrada_fonteLiquido_Item::ativo(){
	return static_cast<JSON_entrada_fonteLiquido_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fonteLiquido_Item_id& JSON_entrada_fonteLiquido_Item::id(){
	return static_cast<JSON_entrada_fonteLiquido_Item_id&>(*contents["id"].get());
}

JSON_entrada_fonteLiquido_Item_comprimentoMedido& JSON_entrada_fonteLiquido_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_fonteLiquido_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_fonteLiquido_Item_tempo& JSON_entrada_fonteLiquido_Item::tempo(){
	return static_cast<JSON_entrada_fonteLiquido_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_fonteLiquido_Item_temperatura& JSON_entrada_fonteLiquido_Item::temperatura(){
	return static_cast<JSON_entrada_fonteLiquido_Item_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_fonteLiquido_Item_beta& JSON_entrada_fonteLiquido_Item::beta(){
	return static_cast<JSON_entrada_fonteLiquido_Item_beta&>(*contents["beta"].get());
}

JSON_entrada_fonteLiquido_Item_vazaoLiquido& JSON_entrada_fonteLiquido_Item::vazaoLiquido(){
	return static_cast<JSON_entrada_fonteLiquido_Item_vazaoLiquido&>(*contents["vazaoLiquido"].get());
}

JSON_entrada_fonteLiquido_Item_indiFluidoPro& JSON_entrada_fonteLiquido_Item::indiFluidoPro(){
	return static_cast<JSON_entrada_fonteLiquido_Item_indiFluidoPro&>(*contents["indiFluidoPro"].get());
}

JSON_entrada_fonteMassa_Item::JSON_entrada_fonteMassa_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fonteMassa_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_fonteMassa_Item_id>();
	contents["tipoTermo"] = make_shared<JSON_entrada_fonteMassa_Item_tipoTermo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_fonteMassa_Item_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_fonteMassa_Item_tempo>();
	contents["temperatura"] = make_shared<JSON_entrada_fonteMassa_Item_temperatura>();
	contents["vazaoMassT"] = make_shared<JSON_entrada_fonteMassa_Item_vazaoMassT>();
	contents["vazaoMassC"] = make_shared<JSON_entrada_fonteMassa_Item_vazaoMassC>();
	contents["vazaoMassG"] = make_shared<JSON_entrada_fonteMassa_Item_vazaoMassG>();
	contents["indiFluidoPro"] = make_shared<JSON_entrada_fonteMassa_Item_indiFluidoPro>();
}

JSON_entrada_fonteMassa_Item_ativo& JSON_entrada_fonteMassa_Item::ativo(){
	return static_cast<JSON_entrada_fonteMassa_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fonteMassa_Item_id& JSON_entrada_fonteMassa_Item::id(){
	return static_cast<JSON_entrada_fonteMassa_Item_id&>(*contents["id"].get());
}

JSON_entrada_fonteMassa_Item_tipoTermo& JSON_entrada_fonteMassa_Item::tipoTermo(){
	return static_cast<JSON_entrada_fonteMassa_Item_tipoTermo&>(*contents["tipoTermo"].get());
}

JSON_entrada_fonteMassa_Item_comprimentoMedido& JSON_entrada_fonteMassa_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_fonteMassa_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_fonteMassa_Item_tempo& JSON_entrada_fonteMassa_Item::tempo(){
	return static_cast<JSON_entrada_fonteMassa_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_fonteMassa_Item_temperatura& JSON_entrada_fonteMassa_Item::temperatura(){
	return static_cast<JSON_entrada_fonteMassa_Item_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_fonteMassa_Item_vazaoMassT& JSON_entrada_fonteMassa_Item::vazaoMassT(){
	return static_cast<JSON_entrada_fonteMassa_Item_vazaoMassT&>(*contents["vazaoMassT"].get());
}

JSON_entrada_fonteMassa_Item_vazaoMassC& JSON_entrada_fonteMassa_Item::vazaoMassC(){
	return static_cast<JSON_entrada_fonteMassa_Item_vazaoMassC&>(*contents["vazaoMassC"].get());
}

JSON_entrada_fonteMassa_Item_vazaoMassG& JSON_entrada_fonteMassa_Item::vazaoMassG(){
	return static_cast<JSON_entrada_fonteMassa_Item_vazaoMassG&>(*contents["vazaoMassG"].get());
}

JSON_entrada_fonteMassa_Item_indiFluidoPro& JSON_entrada_fonteMassa_Item::indiFluidoPro(){
	return static_cast<JSON_entrada_fonteMassa_Item_indiFluidoPro&>(*contents["indiFluidoPro"].get());
}

JSON_entrada_fonteGas_Item::JSON_entrada_fonteGas_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fonteGas_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_fonteGas_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_fonteGas_Item_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_fonteGas_Item_tempo>();
	contents["temperatura"] = make_shared<JSON_entrada_fonteGas_Item_temperatura>();
	contents["vazaoGas"] = make_shared<JSON_entrada_fonteGas_Item_vazaoGas>();
	contents["vazaoFluidoComplementar"] = make_shared<JSON_entrada_fonteGas_Item_vazaoFluidoComplementar>();
	contents["seco"] = make_shared<JSON_entrada_fonteGas_Item_seco>();
	contents["indiFluidoPro"] = make_shared<JSON_entrada_fonteGas_Item_indiFluidoPro>();
}

JSON_entrada_fonteGas_Item_ativo& JSON_entrada_fonteGas_Item::ativo(){
	return static_cast<JSON_entrada_fonteGas_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fonteGas_Item_id& JSON_entrada_fonteGas_Item::id(){
	return static_cast<JSON_entrada_fonteGas_Item_id&>(*contents["id"].get());
}

JSON_entrada_fonteGas_Item_comprimentoMedido& JSON_entrada_fonteGas_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_fonteGas_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_fonteGas_Item_tempo& JSON_entrada_fonteGas_Item::tempo(){
	return static_cast<JSON_entrada_fonteGas_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_fonteGas_Item_temperatura& JSON_entrada_fonteGas_Item::temperatura(){
	return static_cast<JSON_entrada_fonteGas_Item_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_fonteGas_Item_vazaoGas& JSON_entrada_fonteGas_Item::vazaoGas(){
	return static_cast<JSON_entrada_fonteGas_Item_vazaoGas&>(*contents["vazaoGas"].get());
}

JSON_entrada_fonteGas_Item_vazaoFluidoComplementar& JSON_entrada_fonteGas_Item::vazaoFluidoComplementar(){
	return static_cast<JSON_entrada_fonteGas_Item_vazaoFluidoComplementar&>(*contents["vazaoFluidoComplementar"].get());
}

JSON_entrada_fonteGas_Item_seco& JSON_entrada_fonteGas_Item::seco(){
	return static_cast<JSON_entrada_fonteGas_Item_seco&>(*contents["seco"].get());
}

JSON_entrada_fonteGas_Item_indiFluidoPro& JSON_entrada_fonteGas_Item::indiFluidoPro(){
	return static_cast<JSON_entrada_fonteGas_Item_indiFluidoPro&>(*contents["indiFluidoPro"].get());
}


JSON_entrada_fontePoroRadial_Item::JSON_entrada_fontePoroRadial_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fontePoroRadial_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_fontePoroRadial_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_fontePoroRadial_Item_comprimentoMedido>();
	contents["arquivo"] = make_shared<JSON_entrada_fontePoroRadial_Item_arquivo>();
}

JSON_entrada_fontePoroRadial_Item_ativo& JSON_entrada_fontePoroRadial_Item::ativo(){
	return static_cast<JSON_entrada_fontePoroRadial_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fontePoroRadial_Item_id& JSON_entrada_fontePoroRadial_Item::id(){
	return static_cast<JSON_entrada_fontePoroRadial_Item_id&>(*contents["id"].get());
}

JSON_entrada_fontePoroRadial_Item_comprimentoMedido& JSON_entrada_fontePoroRadial_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_fontePoroRadial_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_fontePoroRadial_Item_arquivo& JSON_entrada_fontePoroRadial_Item::arquivo(){
	return static_cast<JSON_entrada_fontePoroRadial_Item_arquivo&>(*contents["arquivo"].get());
}

JSON_entrada_fontePoro2D_Item::JSON_entrada_fontePoro2D_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fontePoro2D_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_fontePoro2D_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_fontePoro2D_Item_comprimentoMedido>();
	contents["arquivo"] = make_shared<JSON_entrada_fontePoro2D_Item_arquivo>();
}

JSON_entrada_fontePoro2D_Item_ativo& JSON_entrada_fontePoro2D_Item::ativo(){
	return static_cast<JSON_entrada_fontePoro2D_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fontePoro2D_Item_id& JSON_entrada_fontePoro2D_Item::id(){
	return static_cast<JSON_entrada_fontePoro2D_Item_id&>(*contents["id"].get());
}

JSON_entrada_fontePoro2D_Item_comprimentoMedido& JSON_entrada_fontePoro2D_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_fontePoro2D_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_fontePoro2D_Item_arquivo& JSON_entrada_fontePoro2D_Item::arquivo(){
	return static_cast<JSON_entrada_fontePoro2D_Item_arquivo&>(*contents["arquivo"].get());
}



JSON_entrada_fonteGasLift_Item::JSON_entrada_fonteGasLift_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fonteGasLift_Item_ativo>();
	contents["colunaEanular"] = make_shared<JSON_entrada_fonteGasLift_Item_colunaEanular>();
	contents["id"] = make_shared<JSON_entrada_fonteGasLift_Item_id>();
	contents["comprimentoMedidoProducao"] = make_shared<JSON_entrada_fonteGasLift_Item_comprimentoMedidoProducao>();
	contents["comprimentoMedidoServico"] = make_shared<JSON_entrada_fonteGasLift_Item_comprimentoMedidoServico>();
	contents["tipoValvula"] = make_shared<JSON_entrada_fonteGasLift_Item_tipoValvula>();
	contents["diametroOrificio"] = make_shared<JSON_entrada_fonteGasLift_Item_diametroOrificio>();
	contents["cdvgl"] = make_shared<JSON_entrada_fonteGasLift_Item_cdvgl>();
	contents["frecupera"] = make_shared<JSON_entrada_fonteGasLift_Item_frecupera>();
	contents["cdvLiq"] = make_shared<JSON_entrada_fonteGasLift_Item_cdvLiq>();
	contents["frecuperaLiq"] = make_shared<JSON_entrada_fonteGasLift_Item_frecuperaLiq>();
	contents["razaoArea"] = make_shared<JSON_entrada_fonteGasLift_Item_razaoArea>();
	contents["pressaoCalibracao"] = make_shared<JSON_entrada_fonteGasLift_Item_pressaoCalibracao>();
	contents["temperaturaCalibracao"] = make_shared<JSON_entrada_fonteGasLift_Item_temperaturaCalibracao>();
	contents["diametroExterno"] = make_shared<JSON_entrada_fonteGasLift_Item_diametroExterno>();
}

JSON_entrada_fonteGasLift_Item_ativo& JSON_entrada_fonteGasLift_Item::ativo(){
	return static_cast<JSON_entrada_fonteGasLift_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fonteGasLift_Item_colunaEanular& JSON_entrada_fonteGasLift_Item::colunaEanular(){
	return static_cast<JSON_entrada_fonteGasLift_Item_colunaEanular&>(*contents["colunaEanular"].get());
}

JSON_entrada_fonteGasLift_Item_id& JSON_entrada_fonteGasLift_Item::id(){
	return static_cast<JSON_entrada_fonteGasLift_Item_id&>(*contents["id"].get());
}

JSON_entrada_fonteGasLift_Item_comprimentoMedidoProducao& JSON_entrada_fonteGasLift_Item::comprimentoMedidoProducao(){
	return static_cast<JSON_entrada_fonteGasLift_Item_comprimentoMedidoProducao&>(*contents["comprimentoMedidoProducao"].get());
}

JSON_entrada_fonteGasLift_Item_comprimentoMedidoServico& JSON_entrada_fonteGasLift_Item::comprimentoMedidoServico(){
	return static_cast<JSON_entrada_fonteGasLift_Item_comprimentoMedidoServico&>(*contents["comprimentoMedidoServico"].get());
}

JSON_entrada_fonteGasLift_Item_tipoValvula& JSON_entrada_fonteGasLift_Item::tipoValvula(){
	return static_cast<JSON_entrada_fonteGasLift_Item_tipoValvula&>(*contents["tipoValvula"].get());
}

JSON_entrada_fonteGasLift_Item_diametroOrificio& JSON_entrada_fonteGasLift_Item::diametroOrificio(){
	return static_cast<JSON_entrada_fonteGasLift_Item_diametroOrificio&>(*contents["diametroOrificio"].get());
}

JSON_entrada_fonteGasLift_Item_cdvgl& JSON_entrada_fonteGasLift_Item::cdvgl(){
	return static_cast<JSON_entrada_fonteGasLift_Item_cdvgl&>(*contents["cdvgl"].get());
}

JSON_entrada_fonteGasLift_Item_frecupera& JSON_entrada_fonteGasLift_Item::frecupera(){
	return static_cast<JSON_entrada_fonteGasLift_Item_frecupera&>(*contents["frecupera"].get());
}

JSON_entrada_fonteGasLift_Item_cdvLiq& JSON_entrada_fonteGasLift_Item::cdvLiq(){
	return static_cast<JSON_entrada_fonteGasLift_Item_cdvLiq&>(*contents["cdvLiq"].get());
}

JSON_entrada_fonteGasLift_Item_frecuperaLiq& JSON_entrada_fonteGasLift_Item::frecuperaLiq(){
	return static_cast<JSON_entrada_fonteGasLift_Item_frecuperaLiq&>(*contents["frecuperaLiq"].get());
}

JSON_entrada_fonteGasLift_Item_razaoArea& JSON_entrada_fonteGasLift_Item::razaoArea(){
	return static_cast<JSON_entrada_fonteGasLift_Item_razaoArea&>(*contents["razaoArea"].get());
}

JSON_entrada_fonteGasLift_Item_pressaoCalibracao& JSON_entrada_fonteGasLift_Item::pressaoCalibracao(){
	return static_cast<JSON_entrada_fonteGasLift_Item_pressaoCalibracao&>(*contents["pressaoCalibracao"].get());
}

JSON_entrada_fonteGasLift_Item_temperaturaCalibracao& JSON_entrada_fonteGasLift_Item::temperaturaCalibracao(){
	return static_cast<JSON_entrada_fonteGasLift_Item_temperaturaCalibracao&>(*contents["temperaturaCalibracao"].get());
}

JSON_entrada_fonteGasLift_Item_diametroExterno& JSON_entrada_fonteGasLift_Item::diametroExterno(){
	return static_cast<JSON_entrada_fonteGasLift_Item_diametroExterno&>(*contents["diametroExterno"].get());
}

JSON_entrada_material_Item::JSON_entrada_material_Item(){
	contents["ativo"] = make_shared<JSON_entrada_material_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_material_Item_id>();
	contents["tipo"] = make_shared<JSON_entrada_material_Item_tipo>();
	contents["condutividade"] = make_shared<JSON_entrada_material_Item_condutividade>();
	contents["calorEspecifico"] = make_shared<JSON_entrada_material_Item_calorEspecifico>();
	contents["rho"] = make_shared<JSON_entrada_material_Item_rho>();
	contents["visc"] = make_shared<JSON_entrada_material_Item_visc>();
	contents["beta"] = make_shared<JSON_entrada_material_Item_beta>();
}

JSON_entrada_material_Item_ativo& JSON_entrada_material_Item::ativo(){
	return static_cast<JSON_entrada_material_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_material_Item_id& JSON_entrada_material_Item::id(){
	return static_cast<JSON_entrada_material_Item_id&>(*contents["id"].get());
}

JSON_entrada_material_Item_tipo& JSON_entrada_material_Item::tipo(){
	return static_cast<JSON_entrada_material_Item_tipo&>(*contents["tipo"].get());
}

JSON_entrada_material_Item_condutividade& JSON_entrada_material_Item::condutividade(){
	return static_cast<JSON_entrada_material_Item_condutividade&>(*contents["condutividade"].get());
}

JSON_entrada_material_Item_calorEspecifico& JSON_entrada_material_Item::calorEspecifico(){
	return static_cast<JSON_entrada_material_Item_calorEspecifico&>(*contents["calorEspecifico"].get());
}

JSON_entrada_material_Item_rho& JSON_entrada_material_Item::rho(){
	return static_cast<JSON_entrada_material_Item_rho&>(*contents["rho"].get());
}

JSON_entrada_material_Item_visc& JSON_entrada_material_Item::visc(){
	return static_cast<JSON_entrada_material_Item_visc&>(*contents["visc"].get());
}

JSON_entrada_material_Item_beta& JSON_entrada_material_Item::beta(){
	return static_cast<JSON_entrada_material_Item_beta&>(*contents["beta"].get());
}

JSON_entrada_secaoTransversal_Item_camadas_Item::JSON_entrada_secaoTransversal_Item_camadas_Item(){
	contents["tipoMedicaoCamada"] = make_shared<JSON_entrada_secaoTransversal_Item_camadas_Item_tipoMedicaoCamada>();
	contents["diametro"] = make_shared<JSON_entrada_secaoTransversal_Item_camadas_Item_diametro>();
	contents["espessura"] = make_shared<JSON_entrada_secaoTransversal_Item_camadas_Item_espessura>();
	contents["discretizacao"] = make_shared<JSON_entrada_secaoTransversal_Item_camadas_Item_discretizacao>();
	contents["idMaterial"] = make_shared<JSON_entrada_secaoTransversal_Item_camadas_Item_idMaterial>();
}

JSON_entrada_secaoTransversal_Item_camadas_Item_tipoMedicaoCamada& JSON_entrada_secaoTransversal_Item_camadas_Item::tipoMedicaoCamada(){
	return static_cast<JSON_entrada_secaoTransversal_Item_camadas_Item_tipoMedicaoCamada&>(*contents["tipoMedicaoCamada"].get());
}

JSON_entrada_secaoTransversal_Item_camadas_Item_diametro& JSON_entrada_secaoTransversal_Item_camadas_Item::diametro(){
	return static_cast<JSON_entrada_secaoTransversal_Item_camadas_Item_diametro&>(*contents["diametro"].get());
}

JSON_entrada_secaoTransversal_Item_camadas_Item_espessura& JSON_entrada_secaoTransversal_Item_camadas_Item::espessura(){
	return static_cast<JSON_entrada_secaoTransversal_Item_camadas_Item_espessura&>(*contents["espessura"].get());
}

JSON_entrada_secaoTransversal_Item_camadas_Item_discretizacao& JSON_entrada_secaoTransversal_Item_camadas_Item::discretizacao(){
	return static_cast<JSON_entrada_secaoTransversal_Item_camadas_Item_discretizacao&>(*contents["discretizacao"].get());
}

JSON_entrada_secaoTransversal_Item_camadas_Item_idMaterial& JSON_entrada_secaoTransversal_Item_camadas_Item::idMaterial(){
	return static_cast<JSON_entrada_secaoTransversal_Item_camadas_Item_idMaterial&>(*contents["idMaterial"].get());
}

JSON_entrada_secaoTransversal_Item::JSON_entrada_secaoTransversal_Item(){
	contents["ativo"] = make_shared<JSON_entrada_secaoTransversal_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_secaoTransversal_Item_id>();
	contents["anular"] = make_shared<JSON_entrada_secaoTransversal_Item_anular>();
	contents["diametroExterno"] = make_shared<JSON_entrada_secaoTransversal_Item_diametroExterno>();
	contents["diametroInterno"] = make_shared<JSON_entrada_secaoTransversal_Item_diametroInterno>();
	contents["rugosidade"] = make_shared<JSON_entrada_secaoTransversal_Item_rugosidade>();
	contents["camadas"] = make_shared<JSON_entrada_secaoTransversal_Item_camadas>();
}

JSON_entrada_secaoTransversal_Item_ativo& JSON_entrada_secaoTransversal_Item::ativo(){
	return static_cast<JSON_entrada_secaoTransversal_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_secaoTransversal_Item_id& JSON_entrada_secaoTransversal_Item::id(){
	return static_cast<JSON_entrada_secaoTransversal_Item_id&>(*contents["id"].get());
}

JSON_entrada_secaoTransversal_Item_anular& JSON_entrada_secaoTransversal_Item::anular(){
	return static_cast<JSON_entrada_secaoTransversal_Item_anular&>(*contents["anular"].get());
}

JSON_entrada_secaoTransversal_Item_diametroExterno& JSON_entrada_secaoTransversal_Item::diametroExterno(){
	return static_cast<JSON_entrada_secaoTransversal_Item_diametroExterno&>(*contents["diametroExterno"].get());
}

JSON_entrada_secaoTransversal_Item_diametroInterno& JSON_entrada_secaoTransversal_Item::diametroInterno(){
	return static_cast<JSON_entrada_secaoTransversal_Item_diametroInterno&>(*contents["diametroInterno"].get());
}

JSON_entrada_secaoTransversal_Item_rugosidade& JSON_entrada_secaoTransversal_Item::rugosidade(){
	return static_cast<JSON_entrada_secaoTransversal_Item_rugosidade&>(*contents["rugosidade"].get());
}

JSON_entrada_secaoTransversal_Item_camadas& JSON_entrada_secaoTransversal_Item::camadas(){
	return static_cast<JSON_entrada_secaoTransversal_Item_camadas&>(*contents["camadas"].get());
}

JSON_entrada_tempo::JSON_entrada_tempo(){
	contents["tempoFinal"] = make_shared<JSON_entrada_tempo_tempoFinal>();
	contents["tempos"] = make_shared<JSON_entrada_tempo_tempos>();
	contents["dtmax"] = make_shared<JSON_entrada_tempo_dtmax>();
	contents["tempoSegrega"] = make_shared<JSON_entrada_tempo_tempoSegrega>();
	contents["segrega"] = make_shared<JSON_entrada_tempo_segrega>();
	contents["gravaMomento"] = make_shared<JSON_entrada_tempo_gravaMomento>();
}

JSON_entrada_tempo_tempoFinal& JSON_entrada_tempo::tempoFinal(){
	return static_cast<JSON_entrada_tempo_tempoFinal&>(*contents["tempoFinal"].get());
}

JSON_entrada_tempo_tempos& JSON_entrada_tempo::tempos(){
	return static_cast<JSON_entrada_tempo_tempos&>(*contents["tempos"].get());
}

JSON_entrada_tempo_dtmax& JSON_entrada_tempo::dtmax(){
	return static_cast<JSON_entrada_tempo_dtmax&>(*contents["dtmax"].get());
}

JSON_entrada_tempo_tempoSegrega& JSON_entrada_tempo::tempoSegrega(){
	return static_cast<JSON_entrada_tempo_tempoSegrega&>(*contents["tempoSegrega"].get());
}

JSON_entrada_tempo_segrega& JSON_entrada_tempo::segrega(){
	return static_cast<JSON_entrada_tempo_segrega&>(*contents["segrega"].get());
}

JSON_entrada_tempo_gravaMomento& JSON_entrada_tempo::gravaMomento(){
	return static_cast<JSON_entrada_tempo_gravaMomento&>(*contents["gravaMomento"].get());
}


//JSON_entrada_gravaMomento::JSON_entrada_gravaMomento(){
	//contents["ativo"] = make_shared<JSON_entrada_gravaMomento_ativo>();
//	contents["tempos"] = make_shared<JSON_entrada_gravaMomento_tempos>();
//}

//JSON_entrada_gravaMomento_ativo& JSON_entrada_gravaMomento::ativo(){
//	return static_cast<JSON_entrada_gravaMomento_ativo&>(*contents["ativo"].get());
//}
//JSON_entrada_gravaMomento_tempos& JSON_entrada_gravaMomento::tempos(){
//	return static_cast<JSON_entrada_gravaMomento_tempos&>(*contents["tempos"].get());
//}

JSON_entrada_dutosProducao_Item_discretizacao_Item::JSON_entrada_dutosProducao_Item_discretizacao_Item(){
	contents["nCelulas"] = make_shared<JSON_entrada_dutosProducao_Item_discretizacao_Item_nCelulas>();
	contents["comprimento"] = make_shared<JSON_entrada_dutosProducao_Item_discretizacao_Item_comprimento>();
}

JSON_entrada_dutosProducao_Item_discretizacao_Item_nCelulas& JSON_entrada_dutosProducao_Item_discretizacao_Item::nCelulas(){
	return static_cast<JSON_entrada_dutosProducao_Item_discretizacao_Item_nCelulas&>(*contents["nCelulas"].get());
}

JSON_entrada_dutosProducao_Item_discretizacao_Item_comprimento& JSON_entrada_dutosProducao_Item_discretizacao_Item::comprimento(){
	return static_cast<JSON_entrada_dutosProducao_Item_discretizacao_Item_comprimento&>(*contents["comprimento"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais::JSON_entrada_dutosProducao_Item_condicoesIniciais(){
	contents["compInter"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_compInter>();
	contents["temp"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_temp>();
	contents["pressao"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_pressao>();
	contents["holdup"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_holdup>();
	contents["bet"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_bet>();
	contents["uls"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_uls>();
	contents["ugs"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_ugs>();
	contents["tempExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_tempExterna>();
	contents["velExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_velExterna>();
	contents["kExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_kExterna>();
	contents["calorEspecificoExterno"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_calorEspecificoExterno>();
	contents["rhoExterno"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_rhoExterno>();
	contents["viscExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais_viscExterna>();
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_compInter& JSON_entrada_dutosProducao_Item_condicoesIniciais::compInter(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_compInter&>(*contents["compInter"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_temp& JSON_entrada_dutosProducao_Item_condicoesIniciais::temp(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_temp&>(*contents["temp"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_pressao& JSON_entrada_dutosProducao_Item_condicoesIniciais::pressao(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_pressao&>(*contents["pressao"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_holdup& JSON_entrada_dutosProducao_Item_condicoesIniciais::holdup(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_holdup&>(*contents["holdup"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_bet& JSON_entrada_dutosProducao_Item_condicoesIniciais::bet(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_bet&>(*contents["bet"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_uls& JSON_entrada_dutosProducao_Item_condicoesIniciais::uls(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_uls&>(*contents["uls"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_ugs& JSON_entrada_dutosProducao_Item_condicoesIniciais::ugs(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_ugs&>(*contents["ugs"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_tempExterna& JSON_entrada_dutosProducao_Item_condicoesIniciais::tempExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_tempExterna&>(*contents["tempExterna"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_velExterna& JSON_entrada_dutosProducao_Item_condicoesIniciais::velExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_velExterna&>(*contents["velExterna"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_kExterna& JSON_entrada_dutosProducao_Item_condicoesIniciais::kExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_kExterna&>(*contents["kExterna"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_calorEspecificoExterno& JSON_entrada_dutosProducao_Item_condicoesIniciais::calorEspecificoExterno(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_calorEspecificoExterno&>(*contents["calorEspecificoExterno"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_rhoExterno& JSON_entrada_dutosProducao_Item_condicoesIniciais::rhoExterno(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_rhoExterno&>(*contents["rhoExterno"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais_viscExterna& JSON_entrada_dutosProducao_Item_condicoesIniciais::viscExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_viscExterna&>(*contents["viscExterna"].get());
}


JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente(){
	contents["compInter"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_compInter>();
	contents["temp"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_temp>();
	contents["pressao"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_pressao>();
	contents["holdup"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_holdup>();
	contents["bet"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_bet>();
	contents["uls"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_uls>();
	contents["ugs"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_ugs>();
	contents["tempExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_tempExterna>();
	contents["velExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_velExterna>();
	contents["kExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_kExterna>();
	contents["calorEspecificoExterno"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_calorEspecificoExterno>();
	contents["rhoExterno"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_rhoExterno>();
	contents["viscExterna"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_viscExterna>();
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_compInter& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::compInter(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_compInter&>(*contents["compInter"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_temp& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::temp(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_temp&>(*contents["temp"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_pressao& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::pressao(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_pressao&>(*contents["pressao"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_holdup& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::holdup(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_holdup&>(*contents["holdup"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_bet& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::bet(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_bet&>(*contents["bet"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_uls& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::uls(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_uls&>(*contents["uls"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_ugs& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::ugs(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_ugs&>(*contents["ugs"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_tempExterna& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::tempExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_tempExterna&>(*contents["tempExterna"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_velExterna& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::velExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_velExterna&>(*contents["velExterna"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_kExterna& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::kExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais_kExterna&>(*contents["kExterna"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_calorEspecificoExterno& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::calorEspecificoExterno(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_calorEspecificoExterno&>(*contents["calorEspecificoExterno"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_rhoExterno& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::rhoExterno(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_rhoExterno&>(*contents["rhoExterno"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_viscExterna& JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente::viscExterna(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente_viscExterna&>(*contents["viscExterna"].get());
}


JSON_entrada_dutosProducao_Item::JSON_entrada_dutosProducao_Item(){
	contents["ativo"] = make_shared<JSON_entrada_dutosProducao_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_dutosProducao_Item_id>();
	contents["correlacaoMR2"] = make_shared<JSON_entrada_dutosProducao_Item_correlacaoMR2>();
	contents["angulo"] = make_shared<JSON_entrada_dutosProducao_Item_angulo>();
	contents["idCorte"] = make_shared<JSON_entrada_dutosProducao_Item_idCorte>();
	contents["idFormacao"] = make_shared<JSON_entrada_dutosProducao_Item_idFormacao>();
	contents["ambienteExterno"] = make_shared<JSON_entrada_dutosProducao_Item_ambienteExterno>();
	contents["direcaoConveccao"] = make_shared<JSON_entrada_dutosProducao_Item_direcaoConveccao>();
	contents["acoplamentoTermico"] = make_shared<JSON_entrada_dutosProducao_Item_acoplamentoTermico>();
	contents["acoplamentoTermicoRedeParalela"] = make_shared<JSON_entrada_dutosProducao_Item_acoplamentoTermicoRedeParalela>();
	contents["agrupamento"] = make_shared<JSON_entrada_dutosProducao_Item_agrupamento>();
	contents["dxCelula"] = make_shared<JSON_entrada_dutosProducao_Item_dxCelula>();
	contents["discretizacao"] = make_shared<JSON_entrada_dutosProducao_Item_discretizacao>();
	contents["condicoesIniciais"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciais>();
	contents["condicoesIniciaisEAmbiente"] = make_shared<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente>();
	contents["dPdLHidro"] = make_shared<JSON_entrada_dutosProducao_Item_dPdLHidro>();
	contents["dPdLFric"] = make_shared<JSON_entrada_dutosProducao_Item_dPdLFric>();
	contents["dTdL"] = make_shared<JSON_entrada_dutosProducao_Item_dTdL>();
	contents["difusTerm2D"] = make_shared<JSON_entrada_dutosProducao_Item_difusTerm2D>();
	contents["difusTerm2DJSON"] = make_shared<JSON_entrada_dutosProducao_Item_difusTerm2DJSON>();
	contents["difusTerm3D"] = make_shared<JSON_entrada_dutosProducao_Item_difusTerm3D>();
	contents["difusTerm3DFE"] = make_shared<JSON_entrada_dutosProducao_Item_difusTerm3DFE>();
	contents["difusTerm3DRotulo"] = make_shared<JSON_entrada_dutosProducao_Item_difusTerm3DAcop>();
	contents["xCoor"] = make_shared<JSON_entrada_dutosProducao_Item_xCoor>();
	contents["yCoor"] = make_shared<JSON_entrada_dutosProducao_Item_yCoor>();
	contents["nCelulas_XY"] = make_shared<JSON_entrada_dutosProducao_Item_nCelulas_XY>();
}

JSON_entrada_dutosProducao_Item_ativo& JSON_entrada_dutosProducao_Item::ativo(){
	return static_cast<JSON_entrada_dutosProducao_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_dutosProducao_Item_id& JSON_entrada_dutosProducao_Item::id(){
	return static_cast<JSON_entrada_dutosProducao_Item_id&>(*contents["id"].get());
}

JSON_entrada_dutosProducao_Item_angulo& JSON_entrada_dutosProducao_Item::angulo(){
	return static_cast<JSON_entrada_dutosProducao_Item_angulo&>(*contents["angulo"].get());
}

JSON_entrada_dutosProducao_Item_correlacaoMR2& JSON_entrada_dutosProducao_Item::correlacaoMR2(){
	return static_cast<JSON_entrada_dutosProducao_Item_correlacaoMR2&>(*contents["correlacaoMR2"].get());
}

JSON_entrada_dutosProducao_Item_idCorte& JSON_entrada_dutosProducao_Item::idCorte(){
	return static_cast<JSON_entrada_dutosProducao_Item_idCorte&>(*contents["idCorte"].get());
}

JSON_entrada_dutosProducao_Item_idFormacao& JSON_entrada_dutosProducao_Item::idFormacao(){
	return static_cast<JSON_entrada_dutosProducao_Item_idFormacao&>(*contents["idFormacao"].get());
}

JSON_entrada_dutosProducao_Item_ambienteExterno& JSON_entrada_dutosProducao_Item::ambienteExterno(){
	return static_cast<JSON_entrada_dutosProducao_Item_ambienteExterno&>(*contents["ambienteExterno"].get());
}

JSON_entrada_dutosProducao_Item_direcaoConveccao& JSON_entrada_dutosProducao_Item::direcaoConveccao(){
	return static_cast<JSON_entrada_dutosProducao_Item_direcaoConveccao&>(*contents["direcaoConveccao"].get());
}

JSON_entrada_dutosProducao_Item_acoplamentoTermico& JSON_entrada_dutosProducao_Item::acoplamentoTermico(){
	return static_cast<JSON_entrada_dutosProducao_Item_acoplamentoTermico&>(*contents["acoplamentoTermico"].get());
}

JSON_entrada_dutosProducao_Item_acoplamentoTermicoRedeParalela& JSON_entrada_dutosProducao_Item::acoplamentoTermicoRedeParalela(){
	return static_cast<JSON_entrada_dutosProducao_Item_acoplamentoTermicoRedeParalela&>(*contents["acoplamentoTermicoRedeParalela"].get());
}

JSON_entrada_dutosProducao_Item_agrupamento& JSON_entrada_dutosProducao_Item::agrupamento(){
	return static_cast<JSON_entrada_dutosProducao_Item_agrupamento&>(*contents["agrupamento"].get());
}

JSON_entrada_dutosProducao_Item_dxCelula& JSON_entrada_dutosProducao_Item::dxCelula(){
	return static_cast<JSON_entrada_dutosProducao_Item_dxCelula&>(*contents["dxCelula"].get());
}

JSON_entrada_dutosProducao_Item_discretizacao& JSON_entrada_dutosProducao_Item::discretizacao(){
	return static_cast<JSON_entrada_dutosProducao_Item_discretizacao&>(*contents["discretizacao"].get());
}

JSON_entrada_dutosProducao_Item_xCoor& JSON_entrada_dutosProducao_Item::xCoor(){
	return static_cast<JSON_entrada_dutosProducao_Item_xCoor&>(*contents["xCoor"].get());
}

JSON_entrada_dutosProducao_Item_yCoor& JSON_entrada_dutosProducao_Item::yCoor(){
	return static_cast<JSON_entrada_dutosProducao_Item_yCoor&>(*contents["yCoor"].get());
}

JSON_entrada_dutosProducao_Item_nCelulas_XY& JSON_entrada_dutosProducao_Item::nCelulas_XY(){
	return static_cast<JSON_entrada_dutosProducao_Item_nCelulas_XY&>(*contents["nCelulas_XY"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciais& JSON_entrada_dutosProducao_Item::condicoesIniciais(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciais&>(*contents["condicoesIniciais"].get());
}

JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente& JSON_entrada_dutosProducao_Item::condicoesIniciaisEAmbiente(){
	return static_cast<JSON_entrada_dutosProducao_Item_condicoesIniciaisEAmbiente&>(*contents["condicoesIniciaisEAmbiente"].get());
}

JSON_entrada_dutosServico_Item_discretizacao_Item::JSON_entrada_dutosServico_Item_discretizacao_Item(){
	contents["nCelulas"] = make_shared<JSON_entrada_dutosServico_Item_discretizacao_Item_nCelulas>();
	contents["comprimento"] = make_shared<JSON_entrada_dutosServico_Item_discretizacao_Item_comprimento>();
}

JSON_entrada_dutosServico_Item_discretizacao_Item_nCelulas& JSON_entrada_dutosServico_Item_discretizacao_Item::nCelulas(){
	return static_cast<JSON_entrada_dutosServico_Item_discretizacao_Item_nCelulas&>(*contents["nCelulas"].get());
}

JSON_entrada_dutosServico_Item_discretizacao_Item_comprimento& JSON_entrada_dutosServico_Item_discretizacao_Item::comprimento(){
	return static_cast<JSON_entrada_dutosServico_Item_discretizacao_Item_comprimento&>(*contents["comprimento"].get());
}

JSON_entrada_dutosProducao_Item_dPdLHidro& JSON_entrada_dutosProducao_Item::dPdLHidro(){
	return static_cast<JSON_entrada_dutosProducao_Item_dPdLHidro&>(*contents["dPdLHidro"].get());
}

JSON_entrada_dutosProducao_Item_dPdLFric& JSON_entrada_dutosProducao_Item::dPdLFric(){
	return static_cast<JSON_entrada_dutosProducao_Item_dPdLFric&>(*contents["dPdLFric"].get());
}

JSON_entrada_dutosProducao_Item_dTdL& JSON_entrada_dutosProducao_Item::dTdL(){
	return static_cast<JSON_entrada_dutosProducao_Item_dTdL&>(*contents["dTdL"].get());
}

JSON_entrada_dutosProducao_Item_difusTerm2D& JSON_entrada_dutosProducao_Item::difusTerm2D(){
	return static_cast<JSON_entrada_dutosProducao_Item_difusTerm2D&>(*contents["difusTerm2D"].get());
}

JSON_entrada_dutosProducao_Item_difusTerm2DJSON& JSON_entrada_dutosProducao_Item::difusTerm2DJSON(){
	return static_cast<JSON_entrada_dutosProducao_Item_difusTerm2DJSON&>(*contents["difusTerm2DJSON"].get());
}

JSON_entrada_dutosProducao_Item_difusTerm3D& JSON_entrada_dutosProducao_Item::difusTerm3D(){
	return static_cast<JSON_entrada_dutosProducao_Item_difusTerm3D&>(*contents["difusTerm3D"].get());
}

JSON_entrada_dutosProducao_Item_difusTerm3DFE& JSON_entrada_dutosProducao_Item::difusTerm3DFE(){
	return static_cast<JSON_entrada_dutosProducao_Item_difusTerm3DFE&>(*contents["difusTerm3DFE"].get());
}

JSON_entrada_dutosProducao_Item_difusTerm3DAcop& JSON_entrada_dutosProducao_Item::difusTerm3DAcop(){
	return static_cast<JSON_entrada_dutosProducao_Item_difusTerm3DAcop&>(*contents["difusTerm3DRotulo"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais::JSON_entrada_dutosServico_Item_condicoesIniciais(){
	contents["compInter"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_compInter>();
	contents["temp"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_temp>();
	contents["pressao"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_pressao>();
	contents["vazaoMassicaGas"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_vazaoMassicaGas>();
	contents["tempExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_tempExterna>();
	contents["velExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_velExterna>();
	contents["kExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_kExterna>();
	contents["calorEspecificoExterno"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_calorEspecificoExterno>();
	contents["rhoExterno"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_rhoExterno>();
	contents["viscExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais_viscExterna>();
}

JSON_entrada_dutosServico_Item_condicoesIniciais_compInter& JSON_entrada_dutosServico_Item_condicoesIniciais::compInter(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_compInter&>(*contents["compInter"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_temp& JSON_entrada_dutosServico_Item_condicoesIniciais::temp(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_temp&>(*contents["temp"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_pressao& JSON_entrada_dutosServico_Item_condicoesIniciais::pressao(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_pressao&>(*contents["pressao"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_vazaoMassicaGas& JSON_entrada_dutosServico_Item_condicoesIniciais::vazaoMassicaGas(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_vazaoMassicaGas&>(*contents["vazaoMassicaGas"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_tempExterna& JSON_entrada_dutosServico_Item_condicoesIniciais::tempExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_tempExterna&>(*contents["tempExterna"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_velExterna& JSON_entrada_dutosServico_Item_condicoesIniciais::velExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_velExterna&>(*contents["velExterna"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_kExterna& JSON_entrada_dutosServico_Item_condicoesIniciais::kExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_kExterna&>(*contents["kExterna"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_calorEspecificoExterno& JSON_entrada_dutosServico_Item_condicoesIniciais::calorEspecificoExterno(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_calorEspecificoExterno&>(*contents["calorEspecificoExterno"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_rhoExterno& JSON_entrada_dutosServico_Item_condicoesIniciais::rhoExterno(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_rhoExterno&>(*contents["rhoExterno"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais_viscExterna& JSON_entrada_dutosServico_Item_condicoesIniciais::viscExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais_viscExterna&>(*contents["viscExterna"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente(){
	contents["compInter"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_compInter>();
	contents["temp"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_temp>();
	contents["pressao"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_pressao>();
	contents["vazaoMassicaGas"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_vazaoMassicaGas>();
	contents["tempExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_tempExterna>();
	contents["velExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_velExterna>();
	contents["kExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_kExterna>();
	contents["calorEspecificoExterno"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_calorEspecificoExterno>();
	contents["rhoExterno"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_rhoExterno>();
	contents["viscExterna"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_viscExterna>();
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_compInter& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::compInter(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_compInter&>(*contents["compInter"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_temp& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::temp(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_temp&>(*contents["temp"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_pressao& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::pressao(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_pressao&>(*contents["pressao"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_vazaoMassicaGas& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::vazaoMassicaGas(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_vazaoMassicaGas&>(*contents["vazaoMassicaGas"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_tempExterna& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::tempExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_tempExterna&>(*contents["tempExterna"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_velExterna& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::velExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_velExterna&>(*contents["velExterna"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_kExterna& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::kExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_kExterna&>(*contents["kExterna"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_calorEspecificoExterno& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::calorEspecificoExterno(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_calorEspecificoExterno&>(*contents["calorEspecificoExterno"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_rhoExterno& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::rhoExterno(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_rhoExterno&>(*contents["rhoExterno"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_viscExterna& JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente::viscExterna(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente_viscExterna&>(*contents["viscExterna"].get());
}


JSON_entrada_dutosServico_Item::JSON_entrada_dutosServico_Item(){
	contents["ativo"] = make_shared<JSON_entrada_dutosServico_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_dutosServico_Item_id>();
	contents["angulo"] = make_shared<JSON_entrada_dutosServico_Item_angulo>();
	contents["idCorte"] = make_shared<JSON_entrada_dutosServico_Item_idCorte>();
	contents["idFormacao"] = make_shared<JSON_entrada_dutosServico_Item_idFormacao>();
	contents["ambienteExterno"] = make_shared<JSON_entrada_dutosServico_Item_ambienteExterno>();
	contents["direcaoConveccao"] = make_shared<JSON_entrada_dutosServico_Item_direcaoConveccao>();
	contents["acoplamentoTermico"] = make_shared<JSON_entrada_dutosServico_Item_acoplamentoTermico>();
	contents["agrupamento"] = make_shared<JSON_entrada_dutosServico_Item_agrupamento>();
	contents["dxCelula"] = make_shared<JSON_entrada_dutosServico_Item_dxCelula>();
	contents["discretizacao"] = make_shared<JSON_entrada_dutosServico_Item_discretizacao>();
	contents["condicoesIniciais"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciais>();
	contents["condicoesIniciaisEAmbiente"] = make_shared<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente>();
	contents["dPdLHidro"] = make_shared<JSON_entrada_dutosServico_Item_dPdLHidro>();
	contents["dPdLFric"] = make_shared<JSON_entrada_dutosServico_Item_dPdLFric>();
	contents["dTdL"] = make_shared<JSON_entrada_dutosServico_Item_dTdL>();
	contents["xCoor"] = make_shared<JSON_entrada_dutosServico_Item_xCoor>();
	contents["yCoor"] = make_shared<JSON_entrada_dutosServico_Item_yCoor>();
	contents["nCelulas_XY"] = make_shared<JSON_entrada_dutosServico_Item_nCelulas_XY>();
}

JSON_entrada_dutosServico_Item_ativo& JSON_entrada_dutosServico_Item::ativo(){
	return static_cast<JSON_entrada_dutosServico_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_dutosServico_Item_id& JSON_entrada_dutosServico_Item::id(){
	return static_cast<JSON_entrada_dutosServico_Item_id&>(*contents["id"].get());
}

JSON_entrada_dutosServico_Item_angulo& JSON_entrada_dutosServico_Item::angulo(){
	return static_cast<JSON_entrada_dutosServico_Item_angulo&>(*contents["angulo"].get());
}

JSON_entrada_dutosServico_Item_idCorte& JSON_entrada_dutosServico_Item::idCorte(){
	return static_cast<JSON_entrada_dutosServico_Item_idCorte&>(*contents["idCorte"].get());
}

JSON_entrada_dutosServico_Item_idFormacao& JSON_entrada_dutosServico_Item::idFormacao(){
	return static_cast<JSON_entrada_dutosServico_Item_idFormacao&>(*contents["idFormacao"].get());
}

JSON_entrada_dutosServico_Item_ambienteExterno& JSON_entrada_dutosServico_Item::ambienteExterno(){
	return static_cast<JSON_entrada_dutosServico_Item_ambienteExterno&>(*contents["ambienteExterno"].get());
}

JSON_entrada_dutosServico_Item_direcaoConveccao& JSON_entrada_dutosServico_Item::direcaoConveccao(){
	return static_cast<JSON_entrada_dutosServico_Item_direcaoConveccao&>(*contents["direcaoConveccao"].get());
}

JSON_entrada_dutosServico_Item_acoplamentoTermico& JSON_entrada_dutosServico_Item::acoplamentoTermico(){
	return static_cast<JSON_entrada_dutosServico_Item_acoplamentoTermico&>(*contents["acoplamentoTermico"].get());
}

JSON_entrada_dutosServico_Item_agrupamento& JSON_entrada_dutosServico_Item::agrupamento(){
	return static_cast<JSON_entrada_dutosServico_Item_agrupamento&>(*contents["agrupamento"].get());
}

JSON_entrada_dutosServico_Item_dxCelula& JSON_entrada_dutosServico_Item::dxCelula(){
	return static_cast<JSON_entrada_dutosServico_Item_dxCelula&>(*contents["dxCelula"].get());
}

JSON_entrada_dutosServico_Item_discretizacao& JSON_entrada_dutosServico_Item::discretizacao(){
	return static_cast<JSON_entrada_dutosServico_Item_discretizacao&>(*contents["discretizacao"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciais& JSON_entrada_dutosServico_Item::condicoesIniciais(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciais&>(*contents["condicoesIniciais"].get());
}

JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente& JSON_entrada_dutosServico_Item::condicoesIniciaisEAmbiente(){
	return static_cast<JSON_entrada_dutosServico_Item_condicoesIniciaisEAmbiente&>(*contents["condicoesIniciais"].get());
}

JSON_entrada_dutosServico_Item_dPdLHidro& JSON_entrada_dutosServico_Item::dPdLHidro(){
	return static_cast<JSON_entrada_dutosServico_Item_dPdLHidro&>(*contents["dPdLHidro"].get());
}

JSON_entrada_dutosServico_Item_dPdLFric& JSON_entrada_dutosServico_Item::dPdLFric(){
	return static_cast<JSON_entrada_dutosServico_Item_dPdLFric&>(*contents["dPdLFric"].get());
}

JSON_entrada_dutosServico_Item_dTdL& JSON_entrada_dutosServico_Item::dTdL(){
	return static_cast<JSON_entrada_dutosServico_Item_dTdL&>(*contents["dTdL"].get());
}

JSON_entrada_dutosServico_Item_xCoor& JSON_entrada_dutosServico_Item::xCoor(){
	return static_cast<JSON_entrada_dutosServico_Item_xCoor&>(*contents["xCoor"].get());
}

JSON_entrada_dutosServico_Item_yCoor& JSON_entrada_dutosServico_Item::yCoor(){
	return static_cast<JSON_entrada_dutosServico_Item_yCoor&>(*contents["yCoor"].get());
}

JSON_entrada_dutosServico_Item_nCelulas_XY& JSON_entrada_dutosServico_Item::nCelulas_XY(){
	return static_cast<JSON_entrada_dutosServico_Item_nCelulas_XY&>(*contents["nCelulas_XY"].get());
}


/*JSON_entrada_hidrato_Hammerschmidt_Khamm& JSON_entrada_hidrato_Hammerschmidt::Khamm(){
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_Khamm&>(*contents["Khamm"].get()); //hydvaloritem
}*/ //chris - Hidratos

JSON_entrada_hidrato_Hammerschmidt_MMH& JSON_entrada_hidrato_Hammerschmidt::MMH(){ //hydvaloritem
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_MMH&>(*contents["MMH"].get());
}

JSON_entrada_hidrato_Hammerschmidt_MMG& JSON_entrada_hidrato_Hammerschmidt::MMG(){ //hydvaloritem
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_MMG&>(*contents["MMG"].get());
}

JSON_entrada_hidrato_Hammerschmidt_MMW& JSON_entrada_hidrato_Hammerschmidt::MMW(){ //hydvaloritem
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_MMW&>(*contents["MMW"].get());
}

JSON_entrada_hidrato_Hammerschmidt_Whamm& JSON_entrada_hidrato_Hammerschmidt::Whamm(){
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_Whamm&>(*contents["Whamm"].get()); //hydvaloritem
}

JSONString& JSON_entrada_hidrato_Hammerschmidt::inibidor() { // chris - Hidratos
    return static_cast<JSONString&>(*contents["inibidor"].get());
}

JSON_entrada_hidrato_Hammerschmidt_fracFWcarregada& JSON_entrada_hidrato_Hammerschmidt::fracFWcarregada(){ //chris - Hidratos
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_fracFWcarregada&>(*contents["fracFWcarregada"].get()); //hydvaloritem
}

/*JSON_entrada_hidrato_Hammerschmidt_Methanol& JSON_entrada_hidrato_Hammerschmidt::Methanol(){ //hydvaloritem
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_Methanol&>(*contents["Methanol"].get());
}

JSON_entrada_hidrato_Hammerschmidt_MMEG& JSON_entrada_hidrato_Hammerschmidt::MMEG(){ //hydvaloritem
	return static_cast<JSON_entrada_hidrato_Hammerschmidt_MMEG&>(*contents["MMEG"].get());
}*/ //chris - Hidratos

JSON_entrada_hidrato_Hammerschmidt::JSON_entrada_hidrato_Hammerschmidt(){ //hydvalor
	//contents["Khamm"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_Khamm>(); //chris - Hidratos
	contents["MMH"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_MMH>();
	contents["MMG"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_MMG>();
	contents["MMW"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_MMW>();
	contents["Whamm"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_Whamm>();
//	contents["Methanol"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_Methanol>(); //chris - Hidratos
//	contents["MMEG"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_MMEG>();
	contents["inibidor"] = make_shared<JSONString>(); //chris - Hidratos
	contents["fracFWcarregada"] = make_shared<JSON_entrada_hidrato_Hammerschmidt_fracFWcarregada>(); //chris - Hidratos
}

/*JSON_entrada_hidrato_PropFluHidrato_rhoH& JSON_entrada_hidrato_PropFluHidrato::rhoH(){
	return static_cast<JSON_entrada_hidrato_PropFluHidrato_rhoH&>(*contents["rhoH"].get()); //hydvaloritem
}*/ //chris - Hidratos

JSON_entrada_hidrato_PropFluHidrato_coefEsteq& JSON_entrada_hidrato_PropFluHidrato::coefEsteq(){ //hydvaloritem
	return static_cast<JSON_entrada_hidrato_PropFluHidrato_coefEsteq&>(*contents["coefEsteq"].get());
}


JSON_entrada_hidrato_PropFluHidrato::JSON_entrada_hidrato_PropFluHidrato(){ //hydvalor
	//contents["rhoH"] = make_shared<JSON_entrada_hidrato_PropFluHidrato_rhoH>(); //chris - Hidratos
	contents["coefEsteq"] = make_shared<JSON_entrada_hidrato_PropFluHidrato_coefEsteq>();
}

JSON_entrada_hidrato_ModeloTurner::JSON_entrada_hidrato_ModeloTurner(){ //hydvalor
	/*contents["estruturaHidratosIk1"] = make_shared<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIk1>();
	contents["estruturaHidratosIk2"] = make_shared<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIk2>();
	contents["estruturaHidratosIIk1"] = make_shared<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIIk1>();
	contents["estruturaHidratosIIk2"] = make_shared<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIIk2>();*/
	//contents["estruturaHidratos"] = make_shared<JSON_entrada_hidrato_ModeloTurner_estruturaHidratos>();
	contents["estruturaHidratos"] = make_shared<JSONString>(); //chris - Hidratos
	contents["rd"] = make_shared<JSON_entrada_hidrato_ModeloTurner_rd>();
	contents["rp"] = make_shared<JSON_entrada_hidrato_ModeloTurner_rp>();
}

/*JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIk1& JSON_entrada_hidrato_ModeloTurner::estruturaHidratosIk1(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIk1&>(*contents["estruturaHidratosIk1"].get()); //hydvaloritem
}

JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIk2& JSON_entrada_hidrato_ModeloTurner::estruturaHidratosIk2(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIk2&>(*contents["estruturaHidratosIk2"].get()); //hydvaloritem
}

JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIIk1& JSON_entrada_hidrato_ModeloTurner::estruturaHidratosIIk1(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIIk1&>(*contents["estruturaHidratosIIk1"].get()); //hydvaloritem
}

JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIIk2& JSON_entrada_hidrato_ModeloTurner::estruturaHidratosIIk2(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner_estruturaHidratosIIk2&>(*contents["estruturaHidratosIIk2"].get()); //hydvaloritem
}*/

/*JSON_entrada_hidrato_ModeloTurner_estruturaHidratos& JSON_entrada_hidrato_ModeloTurner::estruturaHidratos(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner_estruturaHidratos&>(*contents["estruturaHidratos"].get()); //hydvaloritem
}*/


JSONString& JSON_entrada_hidrato_ModeloTurner::estruturaHidratos() { // chris - Hidratos
    return static_cast<JSONString&>(*contents["estruturaHidratos"].get());
}

JSON_entrada_hidrato_ModeloTurner_rd& JSON_entrada_hidrato_ModeloTurner::rd(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner_rd&>(*contents["rd"].get()); //hydvaloritem
}

JSON_entrada_hidrato_ModeloTurner_rp& JSON_entrada_hidrato_ModeloTurner::rp(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner_rp&>(*contents["rp"].get()); //hydvaloritem
}

JSON_entrada_hidrato::JSON_entrada_hidrato(){
	contents["calculoInterno"] = make_shared<JSON_entrada_hidrato_calculoInterno>();
	contents["pressao"] = make_shared<JSON_entrada_hidrato_pressao>();
	contents["temperatura"] = make_shared<JSON_entrada_hidrato_temperatura>();
	contents["Hammerschmidt"] = make_shared<JSON_entrada_hidrato_Hammerschmidt>();
	contents["PropFluHidrato"] = make_shared<JSON_entrada_hidrato_PropFluHidrato>();
	contents["ModeloTurner"] = make_shared<JSON_entrada_hidrato_ModeloTurner>();
}

JSON_entrada_hidrato_calculoInterno& JSON_entrada_hidrato::calculoInterno(){
	return static_cast<JSON_entrada_hidrato_calculoInterno&>(*contents["calculoInterno"].get());
}

JSON_entrada_hidrato_pressao& JSON_entrada_hidrato::pressao(){
	return static_cast<JSON_entrada_hidrato_pressao&>(*contents["pressao"].get());
}

JSON_entrada_hidrato_temperatura& JSON_entrada_hidrato::temperatura(){
	return static_cast<JSON_entrada_hidrato_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_hidrato_Hammerschmidt& JSON_entrada_hidrato::Hammerschmidt(){
	return static_cast<JSON_entrada_hidrato_Hammerschmidt&>(*contents["Hammerschmidt"].get());
}

JSON_entrada_hidrato_PropFluHidrato& JSON_entrada_hidrato::PropFluHidrato(){
	return static_cast<JSON_entrada_hidrato_PropFluHidrato&>(*contents["PropFluHidrato"].get());
}

JSON_entrada_hidrato_ModeloTurner& JSON_entrada_hidrato::ModeloTurner(){
	return static_cast<JSON_entrada_hidrato_ModeloTurner&>(*contents["ModeloTurner"].get());
}

JSON_entrada_ipr_Item::JSON_entrada_ipr_Item(){
	contents["ativo"] = make_shared<JSON_entrada_ipr_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_ipr_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_ipr_Item_comprimentoMedido>();
	contents["tipoIPR"] = make_shared<JSON_entrada_ipr_Item_tipoIPR>();
	contents["pressaoEstatica"] = make_shared<JSON_entrada_ipr_Item_pressaoEstatica>();
	contents["tempoPressaoEstatica"] = make_shared<JSON_entrada_ipr_Item_tempoPressaoEstatica>();
	contents["temperaturas"] = make_shared<JSON_entrada_ipr_Item_temperaturas>();
	contents["tempoTemperaturas"] = make_shared<JSON_entrada_ipr_Item_tempoTemperaturas>();
	contents["ip"] = make_shared<JSON_entrada_ipr_Item_ip>();
	contents["tempoip"] = make_shared<JSON_entrada_ipr_Item_tempoip>();
	contents["qMax"] = make_shared<JSON_entrada_ipr_Item_qMax>();
	contents["tempoqMax"] = make_shared<JSON_entrada_ipr_Item_tempoqMax>();
	contents["ii"] = make_shared<JSON_entrada_ipr_Item_ii>();
	contents["tempoii"] = make_shared<JSON_entrada_ipr_Item_tempoii>();
	contents["indFluidoPro"] = make_shared<JSON_entrada_ipr_Item_indFluidoPro>();
	contents["indiFluidoPro"] = make_shared<JSON_entrada_ipr_Item_indiFluidoPro>();
}

JSON_entrada_ipr_Item_ativo& JSON_entrada_ipr_Item::ativo(){
	return static_cast<JSON_entrada_ipr_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_ipr_Item_id& JSON_entrada_ipr_Item::id(){
	return static_cast<JSON_entrada_ipr_Item_id&>(*contents["id"].get());
}

JSON_entrada_ipr_Item_comprimentoMedido& JSON_entrada_ipr_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_ipr_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_ipr_Item_id& JSON_entrada_ipr_Item::tipoIPR(){
	return static_cast<JSON_entrada_ipr_Item_tipoIPR&>(*contents["tipoIPR"].get());
}

JSON_entrada_ipr_Item_pressaoEstatica& JSON_entrada_ipr_Item::pressaoEstatica(){
	return static_cast<JSON_entrada_ipr_Item_pressaoEstatica&>(*contents["pressaoEstatica"].get());
}

JSON_entrada_ipr_Item_tempoPressaoEstatica& JSON_entrada_ipr_Item::tempoPressaoEstatica(){
	return static_cast<JSON_entrada_ipr_Item_tempoPressaoEstatica&>(*contents["tempoPressaoEstatica"].get());
}

JSON_entrada_ipr_Item_temperaturas& JSON_entrada_ipr_Item::temperaturas(){
	return static_cast<JSON_entrada_ipr_Item_temperaturas&>(*contents["temperaturas"].get());
}

JSON_entrada_ipr_Item_tempoTemperaturas& JSON_entrada_ipr_Item::tempoTemperaturas(){
	return static_cast<JSON_entrada_ipr_Item_tempoTemperaturas&>(*contents["tempoTemperaturas"].get());
}

JSON_entrada_ipr_Item_ip& JSON_entrada_ipr_Item::ip(){
	return static_cast<JSON_entrada_ipr_Item_ip&>(*contents["ip"].get());
}

JSON_entrada_ipr_Item_tempoip& JSON_entrada_ipr_Item::tempoip(){
	return static_cast<JSON_entrada_ipr_Item_tempoip&>(*contents["tempoip"].get());
}

JSON_entrada_ipr_Item_qMax& JSON_entrada_ipr_Item::qMax(){
	return static_cast<JSON_entrada_ipr_Item_qMax&>(*contents["qMax"].get());
}

JSON_entrada_ipr_Item_tempoqMax& JSON_entrada_ipr_Item::tempoqMax(){
	return static_cast<JSON_entrada_ipr_Item_tempoqMax&>(*contents["tempoqMax"].get());
}


JSON_entrada_ipr_Item_ii& JSON_entrada_ipr_Item::ii(){
	return static_cast<JSON_entrada_ipr_Item_ii&>(*contents["ii"].get());
}

JSON_entrada_ipr_Item_tempoii& JSON_entrada_ipr_Item::tempoii(){
	return static_cast<JSON_entrada_ipr_Item_tempoii&>(*contents["tempoii"].get());
}

JSON_entrada_ipr_Item_indFluidoPro& JSON_entrada_ipr_Item::indFluidoPro(){
	return static_cast<JSON_entrada_ipr_Item_indFluidoPro&>(*contents["indFluidoPro"].get());
}

JSON_entrada_ipr_Item_indiFluidoPro& JSON_entrada_ipr_Item::indiFluidoPro(){
	return static_cast<JSON_entrada_ipr_Item_indiFluidoPro&>(*contents["indiFluidoPro"].get());
}

JSON_entrada_fonteChoke::JSON_entrada_fonteChoke(){
	contents["ativo"] = make_shared<JSON_entrada_fonteChoke_ativo>();
	contents["tempo"] = make_shared<JSON_entrada_fonteChoke_tempo>();
	contents["abertura"] = make_shared<JSON_entrada_fonteChoke_abertura>();
	contents["coeficienteDescarga"] = make_shared<JSON_entrada_fonteChoke_coeficienteDescarga>();
	contents["modelo"] = make_shared<JSON_entrada_fonteChoke_modelo>();
}

JSON_entrada_fonteChoke_ativo& JSON_entrada_fonteChoke::ativo(){
	return static_cast<JSON_entrada_fonteChoke_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fonteChoke_tempo& JSON_entrada_fonteChoke::tempo(){
	return static_cast<JSON_entrada_fonteChoke_tempo&>(*contents["tempo"].get());
}

JSON_entrada_fonteChoke_abertura& JSON_entrada_fonteChoke::abertura(){
	return static_cast<JSON_entrada_fonteChoke_abertura&>(*contents["abertura"].get());
}

JSON_entrada_fonteChoke_coeficienteDescarga& JSON_entrada_fonteChoke::coeficienteDescarga(){
	return static_cast<JSON_entrada_fonteChoke_coeficienteDescarga&>(*contents["coeficienteDescarga"].get());
}

JSON_entrada_fonteChoke_modelo& JSON_entrada_fonteChoke::modelo(){
	return static_cast<JSON_entrada_fonteChoke_modelo&>(*contents["modelo"].get());
}

JSON_entrada_bcs_Item::JSON_entrada_bcs_Item(){
	contents["ativo"] = make_shared<JSON_entrada_bcs_Item_ativo>();
	contents["correcHI"] = make_shared<JSON_entrada_bcs_Item_correcHI>();
	contents["id"] = make_shared<JSON_entrada_bcs_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_bcs_Item_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_bcs_Item_tempo>();
	contents["frequencia"] = make_shared<JSON_entrada_bcs_Item_frequencia>();
	contents["vazao"] = make_shared<JSON_entrada_bcs_Item_vazao>();
	contents["potencia"] = make_shared<JSON_entrada_bcs_Item_potencia>();
	contents["eficiencia"] = make_shared<JSON_entrada_bcs_Item_eficiencia>();
	contents["head"] = make_shared<JSON_entrada_bcs_Item_head>();
	contents["freqref"] = make_shared<JSON_entrada_bcs_Item_freqref>();
	contents["nestag"] = make_shared<JSON_entrada_bcs_Item_nestag>();
	contents["nestagFab"] = make_shared<JSON_entrada_bcs_Item_nestagFab>();
	contents["EficienciaMotor"] = make_shared<JSON_entrada_bcs_Item_EficienciaMotor>();
	contents["FrequenciaMinima"] = make_shared<JSON_entrada_bcs_Item_FrequenciaMinima>();
}

JSON_entrada_bcs_Item_ativo& JSON_entrada_bcs_Item::ativo(){
	return static_cast<JSON_entrada_bcs_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_bcs_Item_correcHI& JSON_entrada_bcs_Item::correcHI(){
	return static_cast<JSON_entrada_bcs_Item_correcHI&>(*contents["correcHI"].get());
}

JSON_entrada_bcs_Item_id& JSON_entrada_bcs_Item::id(){
	return static_cast<JSON_entrada_bcs_Item_id&>(*contents["id"].get());
}

JSON_entrada_bcs_Item_comprimentoMedido& JSON_entrada_bcs_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_bcs_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_bcs_Item_tempo& JSON_entrada_bcs_Item::tempo(){
	return static_cast<JSON_entrada_bcs_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_bcs_Item_frequencia& JSON_entrada_bcs_Item::frequencia(){
	return static_cast<JSON_entrada_bcs_Item_frequencia&>(*contents["frequencia"].get());
}

JSON_entrada_bcs_Item_vazao& JSON_entrada_bcs_Item::vazao(){
	return static_cast<JSON_entrada_bcs_Item_vazao&>(*contents["vazao"].get());
}

JSON_entrada_bcs_Item_potencia& JSON_entrada_bcs_Item::potencia(){
	return static_cast<JSON_entrada_bcs_Item_potencia&>(*contents["potencia"].get());
}

JSON_entrada_bcs_Item_eficiencia& JSON_entrada_bcs_Item::eficiencia(){
	return static_cast<JSON_entrada_bcs_Item_eficiencia&>(*contents["eficiencia"].get());
}

JSON_entrada_bcs_Item_head& JSON_entrada_bcs_Item::head(){
	return static_cast<JSON_entrada_bcs_Item_head&>(*contents["head"].get());
}

JSON_entrada_bcs_Item_freqref& JSON_entrada_bcs_Item::freqref(){
	return static_cast<JSON_entrada_bcs_Item_freqref&>(*contents["freqref"].get());
}

JSON_entrada_bcs_Item_nestag& JSON_entrada_bcs_Item::nestag(){
	return static_cast<JSON_entrada_bcs_Item_nestag&>(*contents["nestag"].get());
}

JSON_entrada_bcs_Item_nestagFab& JSON_entrada_bcs_Item::nestagFab(){
	return static_cast<JSON_entrada_bcs_Item_nestagFab&>(*contents["nestagFab"].get());
}

JSON_entrada_bcs_Item_EficienciaMotor& JSON_entrada_bcs_Item::EficienciaMotor(){
	return static_cast<JSON_entrada_bcs_Item_EficienciaMotor&>(*contents["EficienciaMotor"].get());
}

JSON_entrada_bcs_Item_FrequenciaMinima& JSON_entrada_bcs_Item::FrequenciaMinima(){
	return static_cast<JSON_entrada_bcs_Item_FrequenciaMinima&>(*contents["FrequenciaMinima"].get());
}

JSON_entrada_bombaVolumetrica_Item::JSON_entrada_bombaVolumetrica_Item(){
	contents["ativo"] = make_shared<JSON_entrada_bombaVolumetrica_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_bombaVolumetrica_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_bombaVolumetrica_Item_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_bombaVolumetrica_Item_tempo>();
	contents["frequencia"] = make_shared<JSON_entrada_bombaVolumetrica_Item_frequencia>();
	contents["capacidade"] = make_shared<JSON_entrada_bombaVolumetrica_Item_capacidade>();
	contents["fatorpoli"] = make_shared<JSON_entrada_bombaVolumetrica_Item_fatorpoli>();
}

JSON_entrada_bombaVolumetrica_Item_ativo& JSON_entrada_bombaVolumetrica_Item::ativo(){
	return static_cast<JSON_entrada_bombaVolumetrica_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_bombaVolumetrica_Item_id& JSON_entrada_bombaVolumetrica_Item::id(){
	return static_cast<JSON_entrada_bombaVolumetrica_Item_id&>(*contents["id"].get());
}

JSON_entrada_bombaVolumetrica_Item_comprimentoMedido& JSON_entrada_bombaVolumetrica_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_bombaVolumetrica_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_bombaVolumetrica_Item_tempo& JSON_entrada_bombaVolumetrica_Item::tempo(){
	return static_cast<JSON_entrada_bombaVolumetrica_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_bombaVolumetrica_Item_frequencia& JSON_entrada_bombaVolumetrica_Item::frequencia(){
	return static_cast<JSON_entrada_bombaVolumetrica_Item_frequencia&>(*contents["frequencia"].get());
}

JSON_entrada_bombaVolumetrica_Item_capacidade& JSON_entrada_bombaVolumetrica_Item::capacidade(){
	return static_cast<JSON_entrada_bombaVolumetrica_Item_capacidade&>(*contents["capacidade"].get());
}

JSON_entrada_bombaVolumetrica_Item_fatorpoli& JSON_entrada_bombaVolumetrica_Item::fatorpoli(){
	return static_cast<JSON_entrada_bombaVolumetrica_Item_fatorpoli&>(*contents["fatorpoli"].get());
}

JSON_entrada_deltaPressao_Item::JSON_entrada_deltaPressao_Item(){
	contents["ativo"] = make_shared<JSON_entrada_deltaPressao_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_deltaPressao_Item_id>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_deltaPressao_Item_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_deltaPressao_Item_tempo>();
	contents["deltaPressao"] = make_shared<JSON_entrada_deltaPressao_Item_deltaPressao>();
	contents["tipoCompGas"] = make_shared<JSON_entrada_deltaPressao_Item_tipoCompGas>();
	contents["fatPoli"] = make_shared<JSON_entrada_deltaPressao_Item_fatPoli>();
	contents["eficLiq"] = make_shared<JSON_entrada_deltaPressao_Item_eficLiq>();
	contents["eficGas"] = make_shared<JSON_entrada_deltaPressao_Item_eficGas>();
}

JSON_entrada_deltaPressao_Item_ativo& JSON_entrada_deltaPressao_Item::ativo(){
	return static_cast<JSON_entrada_deltaPressao_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_deltaPressao_Item_id& JSON_entrada_deltaPressao_Item::id(){
	return static_cast<JSON_entrada_deltaPressao_Item_id&>(*contents["id"].get());
}

JSON_entrada_deltaPressao_Item_comprimentoMedido& JSON_entrada_deltaPressao_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_deltaPressao_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_deltaPressao_Item_tempo& JSON_entrada_deltaPressao_Item::tempo(){
	return static_cast<JSON_entrada_deltaPressao_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_deltaPressao_Item_deltaPressao& JSON_entrada_deltaPressao_Item::deltaPressao(){
	return static_cast<JSON_entrada_deltaPressao_Item_deltaPressao&>(*contents["deltaPressao"].get());
}

JSON_entrada_deltaPressao_Item_tipoCompGas& JSON_entrada_deltaPressao_Item::tipoCompGas(){
	return static_cast<JSON_entrada_deltaPressao_Item_tipoCompGas&>(*contents["tipoCompGas"].get());
}

JSON_entrada_deltaPressao_Item_fatPoli& JSON_entrada_deltaPressao_Item::fatPoli(){
	return static_cast<JSON_entrada_deltaPressao_Item_fatPoli&>(*contents["fatPoli"].get());
}

JSON_entrada_deltaPressao_Item_eficLiq& JSON_entrada_deltaPressao_Item::eficLiq(){
	return static_cast<JSON_entrada_deltaPressao_Item_eficLiq&>(*contents["eficLiq"].get());
}

JSON_entrada_deltaPressao_Item_eficGas& JSON_entrada_deltaPressao_Item::eficGas(){
	return static_cast<JSON_entrada_deltaPressao_Item_eficGas&>(*contents["eficGas"].get());
}



JSON_entrada_master1::JSON_entrada_master1(){
	contents["ativo"] = make_shared<JSON_entrada_master1_ativo>();
	contents["curvaCV"] = make_shared<JSON_entrada_master1_curvaCV>();
	contents["razaoAreaAtiva"] = make_shared<JSON_entrada_master1_razaoAreaAtiva>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_master1_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_master1_tempo>();
	contents["abertura"] = make_shared<JSON_entrada_master1_abertura>();
	contents["x1"] = make_shared<JSON_entrada_master1_x1>();
	contents["cv1"] = make_shared<JSON_entrada_master1_cv1>();
}

JSON_entrada_master1_ativo& JSON_entrada_master1::ativo(){
	return static_cast<JSON_entrada_master1_ativo&>(*contents["ativo"].get());
}

JSON_entrada_master1_curvaCV& JSON_entrada_master1::curvaCV(){
	return static_cast<JSON_entrada_master1_curvaCV&>(*contents["curvaCV"].get());
}

JSON_entrada_master1_razaoAreaAtiva& JSON_entrada_master1::razaoAreaAtiva(){
	return static_cast<JSON_entrada_master1_razaoAreaAtiva&>(*contents["razaoAreaAtiva"].get());
}

JSON_entrada_master1_comprimentoMedido& JSON_entrada_master1::comprimentoMedido(){
	return static_cast<JSON_entrada_master1_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_master1_tempo& JSON_entrada_master1::tempo(){
	return static_cast<JSON_entrada_master1_tempo&>(*contents["tempo"].get());
}

JSON_entrada_master1_abertura& JSON_entrada_master1::abertura(){
	return static_cast<JSON_entrada_master1_abertura&>(*contents["abertura"].get());
}

JSON_entrada_master1_x1& JSON_entrada_master1::x1(){
	return static_cast<JSON_entrada_master1_x1&>(*contents["x1"].get());
}

JSON_entrada_master1_cv1& JSON_entrada_master1::cv1(){
	return static_cast<JSON_entrada_master1_cv1&>(*contents["cv1"].get());
}

JSON_entrada_master2::JSON_entrada_master2(){
	contents["ativo"] = make_shared<JSON_entrada_master2_ativo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_master2_comprimentoMedido>();
	contents["tempo"] = make_shared<JSON_entrada_master2_tempo>();
	contents["abertura"] = make_shared<JSON_entrada_master2_abertura>();
}

JSON_entrada_master2_ativo& JSON_entrada_master2::ativo(){
	return static_cast<JSON_entrada_master2_ativo&>(*contents["ativo"].get());
}

JSON_entrada_master2_comprimentoMedido& JSON_entrada_master2::comprimentoMedido(){
	return static_cast<JSON_entrada_master2_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_master2_tempo& JSON_entrada_master2::tempo(){
	return static_cast<JSON_entrada_master2_tempo&>(*contents["tempo"].get());
}

JSON_entrada_master2_abertura& JSON_entrada_master2::abertura(){
	return static_cast<JSON_entrada_master2_abertura&>(*contents["abertura"].get());
}

JSON_entrada_pig_Item::JSON_entrada_pig_Item(){
	contents["ativo"] = make_shared<JSON_entrada_pig_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_pig_Item_id>();
	contents["lancador"] = make_shared<JSON_entrada_pig_Item_lancador>();
	contents["recebedor"] = make_shared<JSON_entrada_pig_Item_recebedor>();
	contents["tempo"] = make_shared<JSON_entrada_pig_Item_tempo>();
	contents["folgaArea"] = make_shared<JSON_entrada_pig_Item_folgaArea>();
	contents["deltaPressao"] = make_shared<JSON_entrada_pig_Item_deltaPressao>();
	contents["cdPig"] = make_shared<JSON_entrada_pig_Item_cdPig>();
}

JSON_entrada_pig_Item_ativo& JSON_entrada_pig_Item::ativo(){
	return static_cast<JSON_entrada_pig_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_pig_Item_id& JSON_entrada_pig_Item::id(){
	return static_cast<JSON_entrada_pig_Item_id&>(*contents["id"].get());
}

JSON_entrada_pig_Item_lancador& JSON_entrada_pig_Item::lancador(){
	return static_cast<JSON_entrada_pig_Item_lancador&>(*contents["lancador"].get());
}

JSON_entrada_pig_Item_recebedor& JSON_entrada_pig_Item::recebedor(){
	return static_cast<JSON_entrada_pig_Item_recebedor&>(*contents["recebedor"].get());
}

JSON_entrada_pig_Item_tempo& JSON_entrada_pig_Item::tempo(){
	return static_cast<JSON_entrada_pig_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_pig_Item_folgaArea& JSON_entrada_pig_Item::folgaArea(){
	return static_cast<JSON_entrada_pig_Item_folgaArea&>(*contents["folgaArea"].get());
}

JSON_entrada_pig_Item_deltaPressao& JSON_entrada_pig_Item::deltaPressao(){
	return static_cast<JSON_entrada_pig_Item_deltaPressao&>(*contents["deltaPressao"].get());
}

JSON_entrada_pig_Item_cdPig& JSON_entrada_pig_Item::cdPig(){
	return static_cast<JSON_entrada_pig_Item_cdPig&>(*contents["cdPig"].get());
}

JSON_entrada_fontePressao_Item::JSON_entrada_fontePressao_Item(){
	contents["ativo"] = make_shared<JSON_entrada_fontePressao_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_fontePressao_Item_id>();
	contents["TipoAbertura"] = make_shared<JSON_entrada_fontePressao_Item_TipoAbertura>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_fontePressao_Item_comprimentoMedido>();
	contents["beta"] = make_shared<JSON_entrada_fontePressao_Item_beta>();
	contents["titAmb"] = make_shared<JSON_entrada_fontePressao_Item_titAmb>();
	contents["cd"] = make_shared<JSON_entrada_fontePressao_Item_cd>();
	contents["temperatura"] = make_shared<JSON_entrada_fontePressao_Item_temperatura>();
	contents["pressao"] = make_shared<JSON_entrada_fontePressao_Item_pressao>();
	contents["abertura"] = make_shared<JSON_entrada_fontePressao_Item_abertura>();
	contents["tempo"] = make_shared<JSON_entrada_fontePressao_Item_tempo>();
	contents["tipoFluido"] = make_shared<JSON_entrada_fontePressao_Item_tipoFluido>();
	contents["indiFluidoPro"] = make_shared<JSON_entrada_fontePressao_Item_indiFluidoPro>();
	contents["check"] = make_shared<JSON_entrada_fontePressao_Item_check>();
	contents["tempoChk"] = make_shared<JSON_entrada_fontePressao_Item_tempoChk>();
	contents["ambienteGas"] = make_shared<JSON_entrada_fontePressao_Item_ambGas>();
}

JSON_entrada_fontePressao_Item_ativo& JSON_entrada_fontePressao_Item::ativo(){
	return static_cast<JSON_entrada_fontePressao_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_fontePressao_Item_id& JSON_entrada_fontePressao_Item::id(){
	return static_cast<JSON_entrada_fontePressao_Item_id&>(*contents["id"].get());
}

JSON_entrada_fontePressao_Item_TipoAbertura& JSON_entrada_fontePressao_Item::TipoAbertura(){
	return static_cast<JSON_entrada_fontePressao_Item_TipoAbertura&>(*contents["TipoAbertura"].get());
}

JSON_entrada_fontePressao_Item_comprimentoMedido& JSON_entrada_fontePressao_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_fontePressao_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_fontePressao_Item_beta& JSON_entrada_fontePressao_Item::beta(){
	return static_cast<JSON_entrada_fontePressao_Item_beta&>(*contents["beta"].get());
}

JSON_entrada_fontePressao_Item_titAmb& JSON_entrada_fontePressao_Item::titAmb(){
	return static_cast<JSON_entrada_fontePressao_Item_titAmb&>(*contents["titAmb"].get());
}

JSON_entrada_fontePressao_Item_cd& JSON_entrada_fontePressao_Item::cd(){
	return static_cast<JSON_entrada_fontePressao_Item_cd&>(*contents["cd"].get());
}

JSON_entrada_fontePressao_Item_temperatura& JSON_entrada_fontePressao_Item::temperatura(){
	return static_cast<JSON_entrada_fontePressao_Item_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_fontePressao_Item_pressao& JSON_entrada_fontePressao_Item::pressao(){
	return static_cast<JSON_entrada_fontePressao_Item_pressao&>(*contents["pressao"].get());
}

JSON_entrada_fontePressao_Item_abertura& JSON_entrada_fontePressao_Item::abertura(){
	return static_cast<JSON_entrada_fontePressao_Item_abertura&>(*contents["abertura"].get());
}

JSON_entrada_fontePressao_Item_tempo& JSON_entrada_fontePressao_Item::tempo(){
	return static_cast<JSON_entrada_fontePressao_Item_tempo&>(*contents["tempo"].get());
}

JSON_entrada_fontePressao_Item_tipoFluido& JSON_entrada_fontePressao_Item::tipoFluido(){
	return static_cast<JSON_entrada_fontePressao_Item_tipoFluido&>(*contents["tipoFluido"].get());
}

JSON_entrada_fontePressao_Item_indiFluidoPro& JSON_entrada_fontePressao_Item::indiFluidoPro(){
	return static_cast<JSON_entrada_fontePressao_Item_indiFluidoPro&>(*contents["indiFluidoPro"].get());
}

JSON_entrada_fontePressao_Item_check& JSON_entrada_fontePressao_Item::check(){
	return static_cast<JSON_entrada_fontePressao_Item_check&>(*contents["check"].get());
}

JSON_entrada_fontePressao_Item_tempoChk& JSON_entrada_fontePressao_Item::tempoChk(){
	return static_cast<JSON_entrada_fontePressao_Item_tempoChk&>(*contents["tempoChk"].get());
}

JSON_entrada_fontePressao_Item_ambGas& JSON_entrada_fontePressao_Item::ambGas(){
	return static_cast<JSON_entrada_fontePressao_Item_ambGas&>(*contents["ambienteGas"].get());
}

JSON_entrada_tendP_Item::JSON_entrada_tendP_Item(){
	contents["ativo"] = make_shared<JSON_entrada_tendP_Item_ativo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_tendP_Item_comprimentoMedido>();
	contents["dt"] = make_shared<JSON_entrada_tendP_Item_dt>();
	contents["rotulo"] = make_shared<JSON_entrada_tendP_Item_rotulo>();
	contents["pressao"] = make_shared<JSON_entrada_tendP_Item_pressao>();
	contents["temperatura"] = make_shared<JSON_entrada_tendP_Item_temperatura>();
	contents["holdup"] = make_shared<JSON_entrada_tendP_Item_holdup>();
	contents["FVH"] = make_shared<JSON_entrada_tendP_Item_FVH>(); //solver Hidratos - chris
	contents["bet"] = make_shared<JSON_entrada_tendP_Item_bet>();
	contents["ugs"] = make_shared<JSON_entrada_tendP_Item_ugs>();
	contents["uls"] = make_shared<JSON_entrada_tendP_Item_uls>();
	contents["ug"] = make_shared<JSON_entrada_tendP_Item_ug>();
	contents["ul"] = make_shared<JSON_entrada_tendP_Item_ul>();
	contents["arra"] = make_shared<JSON_entrada_tendP_Item_arra>();
	contents["viscosidadeLiquido"] = make_shared<JSON_entrada_tendP_Item_viscosidadeLiquido>();
	contents["viscosidadeGas"] = make_shared<JSON_entrada_tendP_Item_viscosidadeGas>();
	contents["rhog"] = make_shared<JSON_entrada_tendP_Item_rhog>();
	contents["rhol"] = make_shared<JSON_entrada_tendP_Item_rhol>();
	contents["rhoMix"] = make_shared<JSON_entrada_tendP_Item_rhoMix>();
	contents["vazaoMassicaGas"] = make_shared<JSON_entrada_tendP_Item_vazaoMassicaGas>();
	contents["vazaoMassicaLiquido"] = make_shared<JSON_entrada_tendP_Item_vazaoMassicaLiquido>();
	contents["c0"] = make_shared<JSON_entrada_tendP_Item_c0>();
	contents["ud"] = make_shared<JSON_entrada_tendP_Item_ud>();
	contents["RGO"] = make_shared<JSON_entrada_tendP_Item_RGO>();
	contents["deng"] = make_shared<JSON_entrada_tendP_Item_deng>();
	contents["yco2"] = make_shared<JSON_entrada_tendP_Item_yco2>();
	contents["calor"] = make_shared<JSON_entrada_tendP_Item_calor>();
	contents["masstrans"] = make_shared<JSON_entrada_tendP_Item_masstrans>();
	contents["QLstd"] = make_shared<JSON_entrada_tendP_Item_QLstd>();
	contents["QLWstd"] = make_shared<JSON_entrada_tendP_Item_QLWstd>();
	contents["QOstd"] = make_shared<JSON_entrada_tendP_Item_QOstd>();
	contents["QOWstd"] = make_shared<JSON_entrada_tendP_Item_QOWstd>();
	contents["QLstdTotal"] = make_shared<JSON_entrada_tendP_Item_QLstdTotal>();
	contents["QGstd"] = make_shared<JSON_entrada_tendP_Item_QGstd>();
	contents["api"] = make_shared<JSON_entrada_tendP_Item_api>();
	contents["bsw"] = make_shared<JSON_entrada_tendP_Item_bsw>();
	contents["hidro"] = make_shared<JSON_entrada_tendP_Item_hidro>();
	contents["fric"] = make_shared<JSON_entrada_tendP_Item_fric>();
	contents["dengD"] = make_shared<JSON_entrada_tendP_Item_dengD>();
	contents["dengL"] = make_shared<JSON_entrada_tendP_Item_dengL>();
	contents["ReyInterno"] = make_shared<JSON_entrada_tendP_Item_ReyInterno>();
	contents["ReyExterno"] = make_shared<JSON_entrada_tendP_Item_ReyExterno>();
	contents["GrashInterno"] = make_shared<JSON_entrada_tendP_Item_GrashInterno>();
	contents["GrashExterno"] = make_shared<JSON_entrada_tendP_Item_GrashExterno>();
	contents["NusselInterno"] = make_shared<JSON_entrada_tendP_Item_NusselInterno>();
	contents["NusselExterno"] = make_shared<JSON_entrada_tendP_Item_NusselExterno>();
	contents["PrandtlInterno"] = make_shared<JSON_entrada_tendP_Item_PrandtlInterno>();
	contents["PrandtlExterno"] = make_shared<JSON_entrada_tendP_Item_PrandtlExterno>();
	contents["Froud"] = make_shared<JSON_entrada_tendP_Item_Froud>();
	contents["Rs"] = make_shared<JSON_entrada_tendP_Item_Rs>();
	contents["Bo"] = make_shared<JSON_entrada_tendP_Item_Bo>();
	contents["Hint"] = make_shared<JSON_entrada_tendP_Item_Hint>();
	contents["Hext"] = make_shared<JSON_entrada_tendP_Item_Hext>();
	contents["mlFonte"] = make_shared<JSON_entrada_tendP_Item_mlFonte>();
	contents["mgFonte"] = make_shared<JSON_entrada_tendP_Item_mgFonte>();
	contents["mcFonte"] = make_shared<JSON_entrada_tendP_Item_mcFonte>();
	contents["tempChokeJusante"] = make_shared<JSON_entrada_tendP_Item_tempChokeJus>();
	contents["deltaPBomba"] = make_shared<JSON_entrada_tendP_Item_deltaPBomba>();
	contents["potenciaBomba"] = make_shared<JSON_entrada_tendP_Item_potenciaBomba>();

	contents["volMonM1PT"] = make_shared<JSON_entrada_tendP_Item_volMonM1PT>();
	contents["volJusM1PT"] = make_shared<JSON_entrada_tendP_Item_volJusM1PT>();
	contents["volMonM1ST"] = make_shared<JSON_entrada_tendP_Item_volMonM1ST>();
	contents["volJusM1ST"] = make_shared<JSON_entrada_tendP_Item_volJusM1ST>();

	contents["autoVal"] = make_shared<JSON_entrada_tendP_Item_autoVal>();
	contents["autoVel"] = make_shared<JSON_entrada_tendP_Item_autoVel>();
	contents["flutuacao"] = make_shared<JSON_entrada_tendP_Item_flutuacao>();
	contents["diametroInterno"] = make_shared<JSON_entrada_tendP_Item_diametroInterno>();
	contents["tempParede"] = make_shared<JSON_entrada_tendP_Item_tempParede>();
	contents["dadosParafina"] = make_shared<JSON_entrada_tendP_Item_dadosParafina>();

	contents["inventarioGas"] = make_shared<JSON_entrada_tendP_Item_inventarioGas>();
	contents["inventarioLiq"] = make_shared<JSON_entrada_tendP_Item_inventarioLiq>();

	contents["subResfria"] = make_shared<JSON_entrada_tendP_Item_subResfria>();
}

JSON_entrada_tendP_Item_ativo& JSON_entrada_tendP_Item::ativo(){
	return static_cast<JSON_entrada_tendP_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_tendP_Item_comprimentoMedido& JSON_entrada_tendP_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_tendP_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_tendP_Item_dt& JSON_entrada_tendP_Item::dt(){
	return static_cast<JSON_entrada_tendP_Item_dt&>(*contents["dt"].get());
}

JSON_entrada_tendP_Item_rotulo& JSON_entrada_tendP_Item::rotulo(){
	return static_cast<JSON_entrada_tendP_Item_rotulo&>(*contents["rotulo"].get());
}

JSON_entrada_tendP_Item_pressao& JSON_entrada_tendP_Item::pressao(){
	return static_cast<JSON_entrada_tendP_Item_pressao&>(*contents["pressao"].get());
}

JSON_entrada_tendP_Item_temperatura& JSON_entrada_tendP_Item::temperatura(){
	return static_cast<JSON_entrada_tendP_Item_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_tendP_Item_holdup& JSON_entrada_tendP_Item::holdup(){
	return static_cast<JSON_entrada_tendP_Item_holdup&>(*contents["holdup"].get());
}

JSON_entrada_tendP_Item_FVH& JSON_entrada_tendP_Item::FVH(){
	return static_cast<JSON_entrada_tendP_Item_FVH&>(*contents["FVH"].get());
}

JSON_entrada_tendP_Item_bet& JSON_entrada_tendP_Item::bet(){
	return static_cast<JSON_entrada_tendP_Item_bet&>(*contents["bet"].get());
}

JSON_entrada_tendP_Item_ugs& JSON_entrada_tendP_Item::ugs(){
	return static_cast<JSON_entrada_tendP_Item_ugs&>(*contents["ugs"].get());
}

JSON_entrada_tendP_Item_uls& JSON_entrada_tendP_Item::uls(){
	return static_cast<JSON_entrada_tendP_Item_uls&>(*contents["uls"].get());
}

JSON_entrada_tendP_Item_ug& JSON_entrada_tendP_Item::ug(){
	return static_cast<JSON_entrada_tendP_Item_ug&>(*contents["ug"].get());
}

JSON_entrada_tendP_Item_ul& JSON_entrada_tendP_Item::ul(){
	return static_cast<JSON_entrada_tendP_Item_ul&>(*contents["ul"].get());
}

JSON_entrada_tendP_Item_arra& JSON_entrada_tendP_Item::arra(){
	return static_cast<JSON_entrada_tendP_Item_arra&>(*contents["arra"].get());
}

JSON_entrada_tendP_Item_viscosidadeLiquido& JSON_entrada_tendP_Item::viscosidadeLiquido(){
	return static_cast<JSON_entrada_tendP_Item_viscosidadeLiquido&>(*contents["viscosidadeLiquido"].get());
}

JSON_entrada_tendP_Item_viscosidadeGas& JSON_entrada_tendP_Item::viscosidadeGas(){
	return static_cast<JSON_entrada_tendP_Item_viscosidadeGas&>(*contents["viscosidadeGas"].get());
}

JSON_entrada_tendP_Item_rhog& JSON_entrada_tendP_Item::rhog(){
	return static_cast<JSON_entrada_tendP_Item_rhog&>(*contents["rhog"].get());
}

JSON_entrada_tendP_Item_rhol& JSON_entrada_tendP_Item::rhol(){
	return static_cast<JSON_entrada_tendP_Item_rhol&>(*contents["rhol"].get());
}

JSON_entrada_tendP_Item_rhoMix& JSON_entrada_tendP_Item::rhoMix(){
	return static_cast<JSON_entrada_tendP_Item_rhoMix&>(*contents["rhoMix"].get());
}

JSON_entrada_tendP_Item_vazaoMassicaGas& JSON_entrada_tendP_Item::vazaoMassicaGas(){
	return static_cast<JSON_entrada_tendP_Item_vazaoMassicaGas&>(*contents["vazaoMassicaGas"].get());
}

JSON_entrada_tendP_Item_vazaoMassicaLiquido& JSON_entrada_tendP_Item::vazaoMassicaLiquido(){
	return static_cast<JSON_entrada_tendP_Item_vazaoMassicaLiquido&>(*contents["vazaoMassicaLiquido"].get());
}

JSON_entrada_tendP_Item_c0& JSON_entrada_tendP_Item::c0(){
	return static_cast<JSON_entrada_tendP_Item_c0&>(*contents["c0"].get());
}

JSON_entrada_tendP_Item_ud& JSON_entrada_tendP_Item::ud(){
	return static_cast<JSON_entrada_tendP_Item_ud&>(*contents["ud"].get());
}

JSON_entrada_tendP_Item_RGO& JSON_entrada_tendP_Item::RGO(){
	return static_cast<JSON_entrada_tendP_Item_RGO&>(*contents["RGO"].get());
}

JSON_entrada_tendP_Item_deng& JSON_entrada_tendP_Item::deng(){
	return static_cast<JSON_entrada_tendP_Item_deng&>(*contents["deng"].get());
}

JSON_entrada_tendP_Item_yco2& JSON_entrada_tendP_Item::yco2(){
	return static_cast<JSON_entrada_tendP_Item_yco2&>(*contents["yco2"].get());
}

JSON_entrada_tendP_Item_calor& JSON_entrada_tendP_Item::calor(){
	return static_cast<JSON_entrada_tendP_Item_calor&>(*contents["calor"].get());
}

JSON_entrada_tendP_Item_masstrans& JSON_entrada_tendP_Item::masstrans(){
	return static_cast<JSON_entrada_tendP_Item_masstrans&>(*contents["masstrans"].get());
}

JSON_entrada_tendP_Item_QLstd& JSON_entrada_tendP_Item::QLstd(){
	return static_cast<JSON_entrada_tendP_Item_QLstd&>(*contents["QLstd"].get());
}

JSON_entrada_tendP_Item_QLWstd& JSON_entrada_tendP_Item::QLWstd(){
	return static_cast<JSON_entrada_tendP_Item_QLWstd&>(*contents["QLWstd"].get());
}

JSON_entrada_tendP_Item_QOstd& JSON_entrada_tendP_Item::QOstd(){
	return static_cast<JSON_entrada_tendP_Item_QOstd&>(*contents["QOstd"].get());
}

JSON_entrada_tendP_Item_QOWstd& JSON_entrada_tendP_Item::QOWstd(){
	return static_cast<JSON_entrada_tendP_Item_QOWstd&>(*contents["QOWstd"].get());
}

JSON_entrada_tendP_Item_QLstdTotal& JSON_entrada_tendP_Item::QLstdTotal(){
	return static_cast<JSON_entrada_tendP_Item_QLstdTotal&>(*contents["QLstdTotal"].get());
}

JSON_entrada_tendP_Item_QGstd& JSON_entrada_tendP_Item::QGstd(){
	return static_cast<JSON_entrada_tendP_Item_QGstd&>(*contents["QGstd"].get());
}

JSON_entrada_tendP_Item_api& JSON_entrada_tendP_Item::api(){
	return static_cast<JSON_entrada_tendP_Item_api&>(*contents["api"].get());
}

JSON_entrada_tendP_Item_bsw& JSON_entrada_tendP_Item::bsw(){
	return static_cast<JSON_entrada_tendP_Item_bsw&>(*contents["bsw"].get());
}

JSON_entrada_tendP_Item_hidro& JSON_entrada_tendP_Item::hidro(){
	return static_cast<JSON_entrada_tendP_Item_hidro&>(*contents["hidro"].get());
}

JSON_entrada_tendP_Item_fric& JSON_entrada_tendP_Item::fric(){
	return static_cast<JSON_entrada_tendP_Item_fric&>(*contents["fric"].get());
}

JSON_entrada_tendP_Item_dengD& JSON_entrada_tendP_Item::dengD(){
	return static_cast<JSON_entrada_tendP_Item_dengD&>(*contents["dengD"].get());
}

JSON_entrada_tendP_Item_dengL& JSON_entrada_tendP_Item::dengL(){
	return static_cast<JSON_entrada_tendP_Item_dengL&>(*contents["dengL"].get());
}

JSON_entrada_tendP_Item_ReyInterno& JSON_entrada_tendP_Item::ReyInterno(){
	return static_cast<JSON_entrada_tendP_Item_ReyInterno&>(*contents["ReyInterno"].get());
}

JSON_entrada_tendP_Item_ReyExterno& JSON_entrada_tendP_Item::ReyExterno(){
	return static_cast<JSON_entrada_tendP_Item_ReyExterno&>(*contents["ReyExterno"].get());
}

JSON_entrada_tendP_Item_GrashInterno& JSON_entrada_tendP_Item::GrashInterno(){
	return static_cast<JSON_entrada_tendP_Item_GrashInterno&>(*contents["GrashInterno"].get());
}

JSON_entrada_tendP_Item_GrashExterno& JSON_entrada_tendP_Item::GrashExterno(){
	return static_cast<JSON_entrada_tendP_Item_GrashExterno&>(*contents["GrashExterno"].get());
}

JSON_entrada_tendP_Item_NusselInterno& JSON_entrada_tendP_Item::NusselInterno(){
	return static_cast<JSON_entrada_tendP_Item_NusselInterno&>(*contents["NusselInterno"].get());
}

JSON_entrada_tendP_Item_NusselExterno& JSON_entrada_tendP_Item::NusselExterno(){
	return static_cast<JSON_entrada_tendP_Item_NusselExterno&>(*contents["NusselExterno"].get());
}

JSON_entrada_tendP_Item_PrandtlInterno& JSON_entrada_tendP_Item::PrandtlInterno(){
	return static_cast<JSON_entrada_tendP_Item_PrandtlInterno&>(*contents["PrandtlInterno"].get());
}

JSON_entrada_tendP_Item_PrandtlExterno& JSON_entrada_tendP_Item::PrandtlExterno(){
	return static_cast<JSON_entrada_tendP_Item_PrandtlExterno&>(*contents["PrandtlExterno"].get());
}

JSON_entrada_tendP_Item_Froud& JSON_entrada_tendP_Item::Froud(){
	return static_cast<JSON_entrada_tendP_Item_Froud&>(*contents["Froud"].get());
}

JSON_entrada_tendP_Item_Rs& JSON_entrada_tendP_Item::Rs(){
	return static_cast<JSON_entrada_tendP_Item_Rs&>(*contents["Rs"].get());
}

JSON_entrada_tendP_Item_Bo& JSON_entrada_tendP_Item::Bo(){
	return static_cast<JSON_entrada_tendP_Item_Bo&>(*contents["Bo"].get());
}

JSON_entrada_tendP_Item_Hint& JSON_entrada_tendP_Item::Hint(){
	return static_cast<JSON_entrada_tendP_Item_Hint&>(*contents["Hint"].get());
}

JSON_entrada_tendP_Item_Hext& JSON_entrada_tendP_Item::Hext(){
	return static_cast<JSON_entrada_tendP_Item_Hext&>(*contents["Hext"].get());
}

JSON_entrada_tendP_Item_mlFonte& JSON_entrada_tendP_Item::mlFonte(){
	return static_cast<JSON_entrada_tendP_Item_mlFonte&>(*contents["mlFonte"].get());
}

JSON_entrada_tendP_Item_mgFonte& JSON_entrada_tendP_Item::mgFonte(){
	return static_cast<JSON_entrada_tendP_Item_mgFonte&>(*contents["mgFonte"].get());
}

JSON_entrada_tendP_Item_mcFonte& JSON_entrada_tendP_Item::mcFonte(){
	return static_cast<JSON_entrada_tendP_Item_mcFonte&>(*contents["mcFonte"].get());
}

JSON_entrada_tendP_Item_tempChokeJus& JSON_entrada_tendP_Item::tempChokeJus(){
	return static_cast<JSON_entrada_tendP_Item_tempChokeJus&>(*contents["tempChokeJusante"].get());
}

JSON_entrada_tendP_Item_deltaPBomba& JSON_entrada_tendP_Item::deltaPBomba(){
	return static_cast<JSON_entrada_tendP_Item_deltaPBomba&>(*contents["deltaPBomba"].get());
}

JSON_entrada_tendP_Item_potenciaBomba& JSON_entrada_tendP_Item::potenciaBomba(){
	return static_cast<JSON_entrada_tendP_Item_potenciaBomba&>(*contents["potenciaBomba"].get());
}

JSON_entrada_tendP_Item_volMonM1PT& JSON_entrada_tendP_Item::volMonM1PT(){
	return static_cast<JSON_entrada_tendP_Item_volMonM1PT&>(*contents["volMonM1PT"].get());
}

JSON_entrada_tendP_Item_volJusM1PT& JSON_entrada_tendP_Item::volJusM1PT(){
	return static_cast<JSON_entrada_tendP_Item_volJusM1PT&>(*contents["volJusM1PT"].get());
}

JSON_entrada_tendP_Item_volMonM1ST& JSON_entrada_tendP_Item::volMonM1ST(){
	return static_cast<JSON_entrada_tendP_Item_volMonM1ST&>(*contents["volMonM1ST"].get());
}

JSON_entrada_tendP_Item_volJusM1ST& JSON_entrada_tendP_Item::volJusM1ST(){
	return static_cast<JSON_entrada_tendP_Item_volJusM1ST&>(*contents["volJusM1ST"].get());
}

JSON_entrada_tendP_Item_autoVal& JSON_entrada_tendP_Item::autoVal(){
	return static_cast<JSON_entrada_tendP_Item_autoVal&>(*contents["autoVal"].get());
}

JSON_entrada_tendP_Item_autoVel& JSON_entrada_tendP_Item::autoVel(){
	return static_cast<JSON_entrada_tendP_Item_autoVel&>(*contents["autoVel"].get());
}

JSON_entrada_tendP_Item_flutuacao& JSON_entrada_tendP_Item::flutuacao(){
	return static_cast<JSON_entrada_tendP_Item_flutuacao&>(*contents["flutuacao"].get());
}

JSON_entrada_tendP_Item_diametroInterno& JSON_entrada_tendP_Item::diametroInterno(){
	return static_cast<JSON_entrada_tendP_Item_diametroInterno&>(*contents["diametroInterno"].get());
}

JSON_entrada_tendP_Item_tempParede& JSON_entrada_tendP_Item::tempParede(){
	return static_cast<JSON_entrada_tendP_Item_tempParede&>(*contents["tempParede"].get());
}

JSON_entrada_tendP_Item_dadosParafina& JSON_entrada_tendP_Item::dadosParafina(){
	return static_cast<JSON_entrada_tendP_Item_dadosParafina&>(*contents["dadosParafina"].get());
}

JSON_entrada_tendP_Item_inventarioGas& JSON_entrada_tendP_Item::inventarioGas(){
	return static_cast<JSON_entrada_tendP_Item_inventarioGas&>(*contents["inventarioGas"].get());
}

JSON_entrada_tendP_Item_inventarioLiq& JSON_entrada_tendP_Item::inventarioLiq(){
	return static_cast<JSON_entrada_tendP_Item_inventarioLiq&>(*contents["inventarioLiq"].get());
}

JSON_entrada_tendP_Item_subResfria& JSON_entrada_tendP_Item::subResfria(){
	return static_cast<JSON_entrada_tendP_Item_subResfria&>(*contents["subResfria"].get());
}

JSON_entrada_tendTransP_Item::JSON_entrada_tendTransP_Item(){
	contents["ativo"] = make_shared<JSON_entrada_tendTransP_Item_ativo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_tendTransP_Item_comprimentoMedido>();
	contents["camada"] = make_shared<JSON_entrada_tendTransP_Item_camada>();
	contents["discretizacao"] = make_shared<JSON_entrada_tendTransP_Item_discretizacao>();
	contents["dt"] = make_shared<JSON_entrada_tendTransP_Item_dt>();
	contents["rotulo"] = make_shared<JSON_entrada_tendTransP_Item_rotulo>();
}

JSON_entrada_tendTransP_Item_ativo& JSON_entrada_tendTransP_Item::ativo(){
	return static_cast<JSON_entrada_tendTransP_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_tendTransP_Item_comprimentoMedido& JSON_entrada_tendTransP_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_tendTransP_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_tendTransP_Item_camada& JSON_entrada_tendTransP_Item::camada(){
	return static_cast<JSON_entrada_tendTransP_Item_camada&>(*contents["camada"].get());
}

JSON_entrada_tendTransP_Item_discretizacao& JSON_entrada_tendTransP_Item::discretizacao(){
	return static_cast<JSON_entrada_tendTransP_Item_discretizacao&>(*contents["discretizacao"].get());
}

JSON_entrada_tendTransP_Item_dt& JSON_entrada_tendTransP_Item::dt(){
	return static_cast<JSON_entrada_tendTransP_Item_dt&>(*contents["dt"].get());
}

JSON_entrada_tendTransP_Item_rotulo& JSON_entrada_tendTransP_Item::rotulo(){
	return static_cast<JSON_entrada_tendTransP_Item_rotulo&>(*contents["rotulo"].get());
}

JSON_entrada_tendS_Item::JSON_entrada_tendS_Item(){
	contents["ativo"] = make_shared<JSON_entrada_tendS_Item_ativo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_tendS_Item_comprimentoMedido>();
	contents["dt"] = make_shared<JSON_entrada_tendS_Item_dt>();
	contents["rotulo"] = make_shared<JSON_entrada_tendS_Item_rotulo>();
	contents["pressao"] = make_shared<JSON_entrada_tendS_Item_pressao>();
	contents["temperatura"] = make_shared<JSON_entrada_tendS_Item_temperatura>();
	contents["ugs"] = make_shared<JSON_entrada_tendS_Item_ugs>();
	contents["ug"] = make_shared<JSON_entrada_tendS_Item_ug>();
	contents["tensaoCisalhamento"] = make_shared<JSON_entrada_tendS_Item_tensaoCisalhamento>();
	contents["viscosidadeGas"] = make_shared<JSON_entrada_tendS_Item_viscosidadeGas>();
	contents["rhog"] = make_shared<JSON_entrada_tendS_Item_rhog>();
	contents["vazaoMassicaGas"] = make_shared<JSON_entrada_tendS_Item_vazaoMassicaGas>();
	contents["hidro"] = make_shared<JSON_entrada_tendS_Item_hidro>();
	contents["fric"] = make_shared<JSON_entrada_tendS_Item_fric>();
	contents["FVHG"] = make_shared<JSON_entrada_tendS_Item_FVHG>(); //chris - Hidratos
	contents["calor"] = make_shared<JSON_entrada_tendS_Item_calor>();
	contents["QGstd"] = make_shared<JSON_entrada_tendS_Item_QGstd>();
	contents["presEstagVGL"] = make_shared<JSON_entrada_tendS_Item_presEstagVGL>();
	contents["tempEstagVGL"] = make_shared<JSON_entrada_tendS_Item_tempEstagVGL>();
	contents["presGargVGL"] = make_shared<JSON_entrada_tendS_Item_presGargVGL>();
	contents["tempGargVGL"] = make_shared<JSON_entrada_tendS_Item_tempGargVGL>();
	contents["vazaoVGL"] = make_shared<JSON_entrada_tendS_Item_vazaoVGL>();
	contents["VelocidadeMaximaGarganta"] = make_shared<JSON_entrada_tendS_Item_VelocidadeMaximaGarganta>();
	contents["ReyInterno"] = make_shared<JSON_entrada_tendS_Item_ReyInterno>();
	contents["ReyExterno"] = make_shared<JSON_entrada_tendS_Item_ReyExterno>();
	contents["GrashInterno"] = make_shared<JSON_entrada_tendS_Item_GrashInterno>();
	contents["GrashExterno"] = make_shared<JSON_entrada_tendS_Item_GrashExterno>();
	contents["NusselInterno"] = make_shared<JSON_entrada_tendS_Item_NusselInterno>();
	contents["NusselExterno"] = make_shared<JSON_entrada_tendS_Item_NusselExterno>();
	contents["PrandtlInterno"] = make_shared<JSON_entrada_tendS_Item_PrandtlInterno>();
	contents["PrandtlExterno"] = make_shared<JSON_entrada_tendS_Item_PrandtlExterno>();
	contents["Froud"] = make_shared<JSON_entrada_tendS_Item_Froud>();
	contents["Hint"] = make_shared<JSON_entrada_tendS_Item_Hint>();
	contents["Hext"] = make_shared<JSON_entrada_tendS_Item_Hext>();
	contents["diametroInterno"] = make_shared<JSON_entrada_tendS_Item_diametroInterno>();
	contents["tempParede"] = make_shared<JSON_entrada_tendS_Item_tempParede>();
	contents["subResfria"] = make_shared<JSON_entrada_tendS_Item_subResfria>();
}

JSON_entrada_tendS_Item_ativo& JSON_entrada_tendS_Item::ativo(){
	return static_cast<JSON_entrada_tendS_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_tendS_Item_comprimentoMedido& JSON_entrada_tendS_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_tendS_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_tendS_Item_dt& JSON_entrada_tendS_Item::dt(){
	return static_cast<JSON_entrada_tendS_Item_dt&>(*contents["dt"].get());
}

JSON_entrada_tendS_Item_rotulo& JSON_entrada_tendS_Item::rotulo(){
	return static_cast<JSON_entrada_tendS_Item_rotulo&>(*contents["rotulo"].get());
}

JSON_entrada_tendS_Item_pressao& JSON_entrada_tendS_Item::pressao(){
	return static_cast<JSON_entrada_tendS_Item_pressao&>(*contents["pressao"].get());
}

JSON_entrada_tendS_Item_temperatura& JSON_entrada_tendS_Item::temperatura(){
	return static_cast<JSON_entrada_tendS_Item_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_tendS_Item_ugs& JSON_entrada_tendS_Item::ugs(){
	return static_cast<JSON_entrada_tendS_Item_ugs&>(*contents["ugs"].get());
}

JSON_entrada_tendS_Item_ug& JSON_entrada_tendS_Item::ug(){
	return static_cast<JSON_entrada_tendS_Item_ug&>(*contents["ug"].get());
}

JSON_entrada_tendS_Item_tensaoCisalhamento& JSON_entrada_tendS_Item::tensaoCisalhamento(){
	return static_cast<JSON_entrada_tendS_Item_tensaoCisalhamento&>(*contents["tensaoCisalhamento"].get());
}

JSON_entrada_tendS_Item_viscosidadeGas& JSON_entrada_tendS_Item::viscosidadeGas(){
	return static_cast<JSON_entrada_tendS_Item_viscosidadeGas&>(*contents["viscosidadeGas"].get());
}

JSON_entrada_tendS_Item_rhog& JSON_entrada_tendS_Item::rhog(){
	return static_cast<JSON_entrada_tendS_Item_rhog&>(*contents["rhog"].get());
}

JSON_entrada_tendS_Item_vazaoMassicaGas& JSON_entrada_tendS_Item::vazaoMassicaGas(){
	return static_cast<JSON_entrada_tendS_Item_vazaoMassicaGas&>(*contents["vazaoMassicaGas"].get());
}

JSON_entrada_tendS_Item_hidro& JSON_entrada_tendS_Item::hidro(){
	return static_cast<JSON_entrada_tendS_Item_hidro&>(*contents["hidro"].get());
}

JSON_entrada_tendS_Item_fric& JSON_entrada_tendS_Item::fric(){
	return static_cast<JSON_entrada_tendS_Item_fric&>(*contents["fric"].get());
}

JSON_entrada_tendS_Item_FVHG& JSON_entrada_tendS_Item::FVHG(){
	return static_cast<JSON_entrada_tendS_Item_FVHG&>(*contents["FVHG"].get());
} //chris - Hidratos

JSON_entrada_tendS_Item_calor& JSON_entrada_tendS_Item::calor(){
	return static_cast<JSON_entrada_tendS_Item_calor&>(*contents["calor"].get());
}

JSON_entrada_tendS_Item_QGstd& JSON_entrada_tendS_Item::QGstd(){
	return static_cast<JSON_entrada_tendS_Item_QGstd&>(*contents["QGstd"].get());
}

JSON_entrada_tendS_Item_presEstagVGL& JSON_entrada_tendS_Item::presEstagVGL(){
	return static_cast<JSON_entrada_tendS_Item_presEstagVGL&>(*contents["presEstagVGL"].get());
}

JSON_entrada_tendS_Item_tempEstagVGL& JSON_entrada_tendS_Item::tempEstagVGL(){
	return static_cast<JSON_entrada_tendS_Item_tempEstagVGL&>(*contents["tempEstagVGL"].get());
}

JSON_entrada_tendS_Item_presGargVGL& JSON_entrada_tendS_Item::presGargVGL(){
	return static_cast<JSON_entrada_tendS_Item_presGargVGL&>(*contents["presGargVGL"].get());
}

JSON_entrada_tendS_Item_tempGargVGL& JSON_entrada_tendS_Item::tempGargVGL(){
	return static_cast<JSON_entrada_tendS_Item_tempGargVGL&>(*contents["tempGargVGL"].get());
}

JSON_entrada_tendS_Item_vazaoVGL& JSON_entrada_tendS_Item::vazaoVGL(){
	return static_cast<JSON_entrada_tendS_Item_vazaoVGL&>(*contents["vazaoVGL"].get());
}

JSON_entrada_tendS_Item_VelocidadeMaximaGarganta& JSON_entrada_tendS_Item::VelocidadeMaximaGarganta(){
	return static_cast<JSON_entrada_tendS_Item_VelocidadeMaximaGarganta&>(*contents["VelocidadeMaximaGarganta"].get());
}

JSON_entrada_tendS_Item_ReyInterno& JSON_entrada_tendS_Item::ReyInterno(){
	return static_cast<JSON_entrada_tendS_Item_ReyInterno&>(*contents["ReyInterno"].get());
}

JSON_entrada_tendS_Item_ReyExterno& JSON_entrada_tendS_Item::ReyExterno(){
	return static_cast<JSON_entrada_tendS_Item_ReyExterno&>(*contents["ReyExterno"].get());
}

JSON_entrada_tendS_Item_GrashInterno& JSON_entrada_tendS_Item::GrashInterno(){
	return static_cast<JSON_entrada_tendS_Item_GrashInterno&>(*contents["GrashInterno"].get());
}

JSON_entrada_tendS_Item_GrashExterno& JSON_entrada_tendS_Item::GrashExterno(){
	return static_cast<JSON_entrada_tendS_Item_GrashExterno&>(*contents["GrashExterno"].get());
}

JSON_entrada_tendS_Item_NusselInterno& JSON_entrada_tendS_Item::NusselInterno(){
	return static_cast<JSON_entrada_tendS_Item_NusselInterno&>(*contents["NusselInterno"].get());
}

JSON_entrada_tendS_Item_NusselExterno& JSON_entrada_tendS_Item::NusselExterno(){
	return static_cast<JSON_entrada_tendS_Item_NusselExterno&>(*contents["NusselExterno"].get());
}

JSON_entrada_tendS_Item_PrandtlInterno& JSON_entrada_tendS_Item::PrandtlInterno(){
	return static_cast<JSON_entrada_tendS_Item_PrandtlInterno&>(*contents["PrandtlInterno"].get());
}

JSON_entrada_tendS_Item_PrandtlExterno& JSON_entrada_tendS_Item::PrandtlExterno(){
	return static_cast<JSON_entrada_tendS_Item_PrandtlExterno&>(*contents["PrandtlExterno"].get());
}

JSON_entrada_tendS_Item_Froud& JSON_entrada_tendS_Item::Froud(){
	return static_cast<JSON_entrada_tendS_Item_Froud&>(*contents["Froud"].get());
}

JSON_entrada_tendS_Item_Hint& JSON_entrada_tendS_Item::Hint(){
	return static_cast<JSON_entrada_tendS_Item_Hint&>(*contents["Hint"].get());
}

JSON_entrada_tendS_Item_Hext& JSON_entrada_tendS_Item::Hext(){
	return static_cast<JSON_entrada_tendS_Item_Hext&>(*contents["Hext"].get());
}

JSON_entrada_tendS_Item_diametroInterno& JSON_entrada_tendS_Item::diametroInterno(){
	return static_cast<JSON_entrada_tendS_Item_diametroInterno&>(*contents["diametroInterno"].get());
}

JSON_entrada_tendS_Item_tempParede& JSON_entrada_tendS_Item::tempParede(){
	return static_cast<JSON_entrada_tendS_Item_tempParede&>(*contents["tempParede"].get());
}

JSON_entrada_tendS_Item_subResfria& JSON_entrada_tendS_Item::subResfria(){
	return static_cast<JSON_entrada_tendS_Item_subResfria&>(*contents["subResfria"].get());
}


JSON_entrada_tendTransS_Item::JSON_entrada_tendTransS_Item(){
	contents["ativo"] = make_shared<JSON_entrada_tendTransS_Item_ativo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_tendTransS_Item_comprimentoMedido>();
	contents["camada"] = make_shared<JSON_entrada_tendTransS_Item_camada>();
	contents["discretizacao"] = make_shared<JSON_entrada_tendTransS_Item_discretizacao>();
	contents["dt"] = make_shared<JSON_entrada_tendTransS_Item_dt>();
	contents["rotulo"] = make_shared<JSON_entrada_tendTransS_Item_rotulo>();
}

JSON_entrada_tendTransS_Item_ativo& JSON_entrada_tendTransS_Item::ativo(){
	return static_cast<JSON_entrada_tendTransS_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_tendTransS_Item_comprimentoMedido& JSON_entrada_tendTransS_Item::comprimentoMedido(){
	return static_cast<JSON_entrada_tendTransS_Item_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_tendTransS_Item_camada& JSON_entrada_tendTransS_Item::camada(){
	return static_cast<JSON_entrada_tendTransS_Item_camada&>(*contents["camada"].get());
}

JSON_entrada_tendTransS_Item_discretizacao& JSON_entrada_tendTransS_Item::discretizacao(){
	return static_cast<JSON_entrada_tendTransS_Item_discretizacao&>(*contents["discretizacao"].get());
}

JSON_entrada_tendTransS_Item_dt& JSON_entrada_tendTransS_Item::dt(){
	return static_cast<JSON_entrada_tendTransS_Item_dt&>(*contents["dt"].get());
}

JSON_entrada_tendTransS_Item_rotulo& JSON_entrada_tendTransS_Item::rotulo(){
	return static_cast<JSON_entrada_tendTransS_Item_rotulo&>(*contents["rotulo"].get());
}

JSON_entrada_tela_Item::JSON_entrada_tela_Item(){
	contents["ativo"] = make_shared<JSON_entrada_tela_Item_ativo>();
	contents["coluna"] = make_shared<JSON_entrada_tela_Item_coluna>();
	contents["celula"] = make_shared<JSON_entrada_tela_Item_celula>();
	contents["variavel"] = make_shared<JSON_entrada_tela_Item_variavel>();
}

JSON_entrada_tela_Item_ativo& JSON_entrada_tela_Item::ativo(){
	return static_cast<JSON_entrada_tela_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_tela_Item_coluna& JSON_entrada_tela_Item::coluna(){
	return static_cast<JSON_entrada_tela_Item_coluna&>(*contents["coluna"].get());
}

JSON_entrada_tela_Item_celula& JSON_entrada_tela_Item::celula(){
	return static_cast<JSON_entrada_tela_Item_celula&>(*contents["celula"].get());
}

JSON_entrada_tela_Item_variavel& JSON_entrada_tela_Item::variavel(){
	return static_cast<JSON_entrada_tela_Item_variavel&>(*contents["variavel"].get());
}

JSON_entrada_gasInj::JSON_entrada_gasInj(){
	contents["ativo"] = make_shared<JSON_entrada_gasInj_ativo>();
	contents["tipoCC"] = make_shared<JSON_entrada_gasInj_tipoCC>();
	contents["temperatura"] = make_shared<JSON_entrada_gasInj_temperatura>();
	contents["tempo"] = make_shared<JSON_entrada_gasInj_tempo>();
	contents["vazaoGas"] = make_shared<JSON_entrada_gasInj_vazaoGas>();
	contents["pressaoInjecao"] = make_shared<JSON_entrada_gasInj_pressaoInjecao>();
	contents["chuteVazaoInjecao"] = make_shared<JSON_entrada_gasInj_chuteVazaoInjecao>();
}

JSON_entrada_gasInj_ativo& JSON_entrada_gasInj::ativo(){
	return static_cast<JSON_entrada_gasInj_ativo&>(*contents["ativo"].get());
}

JSON_entrada_gasInj_tipoCC& JSON_entrada_gasInj::tipoCC(){
	return static_cast<JSON_entrada_gasInj_tipoCC&>(*contents["tipoCC"].get());
}

JSON_entrada_gasInj_temperatura& JSON_entrada_gasInj::temperatura(){
	return static_cast<JSON_entrada_gasInj_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_gasInj_tempo& JSON_entrada_gasInj::tempo(){
	return static_cast<JSON_entrada_gasInj_tempo&>(*contents["tempo"].get());
}

JSON_entrada_gasInj_vazaoGas& JSON_entrada_gasInj::vazaoGas(){
	return static_cast<JSON_entrada_gasInj_vazaoGas&>(*contents["vazaoGas"].get());
}

JSON_entrada_gasInj_pressaoInjecao& JSON_entrada_gasInj::pressaoInjecao(){
	return static_cast<JSON_entrada_gasInj_pressaoInjecao&>(*contents["pressaoInjecao"].get());
}

JSON_entrada_gasInj_chuteVazaoInjecao& JSON_entrada_gasInj::chuteVazaoInjecao(){
	return static_cast<JSON_entrada_gasInj_chuteVazaoInjecao&>(*contents["chuteVazaoInjecao"].get());
}

JSON_entrada_perfilProducao::JSON_entrada_perfilProducao(){
	contents["ativo"] = make_shared<JSON_entrada_perfilProducao_ativo>();
	contents["tempo"] = make_shared<JSON_entrada_perfilProducao_tempo>();
	contents["pressao"] = make_shared<JSON_entrada_perfilProducao_pressao>();
	contents["temperatura"] = make_shared<JSON_entrada_perfilProducao_temperatura>();
	contents["holdup"] = make_shared<JSON_entrada_perfilProducao_holdup>();
	contents["FVH"] = make_shared<JSON_entrada_perfilProducao_FVH>();
	contents["bet"] = make_shared<JSON_entrada_perfilProducao_bet>();
	contents["ugs"] = make_shared<JSON_entrada_perfilProducao_ugs>();
	contents["uls"] = make_shared<JSON_entrada_perfilProducao_uls>();
	contents["ug"] = make_shared<JSON_entrada_perfilProducao_ug>();
	contents["ul"] = make_shared<JSON_entrada_perfilProducao_ul>();
	contents["arra"] = make_shared<JSON_entrada_perfilProducao_arra>();
	contents["viscosidadeLiquido"] = make_shared<JSON_entrada_perfilProducao_viscosidadeLiquido>();
	contents["viscosidadeGas"] = make_shared<JSON_entrada_perfilProducao_viscosidadeGas>();
	contents["rhog"] = make_shared<JSON_entrada_perfilProducao_rhog>();
	contents["rhol"] = make_shared<JSON_entrada_perfilProducao_rhol>();
	contents["rhoo"] = make_shared<JSON_entrada_perfilProducao_rhoo>();
	contents["rhoa"] = make_shared<JSON_entrada_perfilProducao_rhoa>();
	contents["rhoMix"] = make_shared<JSON_entrada_perfilProducao_rhoMix>();
	contents["RS"] = make_shared<JSON_entrada_perfilProducao_RS>();
	contents["vazaoMassicaGas"] = make_shared<JSON_entrada_perfilProducao_vazaoMassicaGas>();
	contents["vazaoMassicaLiquido"] = make_shared<JSON_entrada_perfilProducao_vazaoMassicaLiquido>();
	contents["c0"] = make_shared<JSON_entrada_perfilProducao_c0>();
	contents["ud"] = make_shared<JSON_entrada_perfilProducao_ud>();
	contents["RGO"] = make_shared<JSON_entrada_perfilProducao_RGO>();
	contents["deng"] = make_shared<JSON_entrada_perfilProducao_deng>();
	contents["yco2"] = make_shared<JSON_entrada_perfilProducao_yco2>();
	contents["calor"] = make_shared<JSON_entrada_perfilProducao_calor>();
	contents["masstrans"] = make_shared<JSON_entrada_perfilProducao_masstrans>();
	contents["cpgas"] = make_shared<JSON_entrada_perfilProducao_cpgas>();
	contents["cpliq"] = make_shared<JSON_entrada_perfilProducao_cpliq>();
	contents["QLstd"] = make_shared<JSON_entrada_perfilProducao_QLstd>();
	contents["QLWstd"] = make_shared<JSON_entrada_perfilProducao_QLWstd>();
	contents["QLstdTotal"] = make_shared<JSON_entrada_perfilProducao_QLstdTotal>();
	contents["QGstd"] = make_shared<JSON_entrada_perfilProducao_QGstd>();
	contents["api"] = make_shared<JSON_entrada_perfilProducao_api>();
	contents["bsw"] = make_shared<JSON_entrada_perfilProducao_bsw>();
	contents["hidro"] = make_shared<JSON_entrada_perfilProducao_hidro>();
	contents["fric"] = make_shared<JSON_entrada_perfilProducao_fric>();
	contents["Term1"] = make_shared<JSON_entrada_perfilProducao_Term1>();
	contents["Term2"] = make_shared<JSON_entrada_perfilProducao_Term2>();
	contents["dengD"] = make_shared<JSON_entrada_perfilProducao_dengD>();
	contents["dengL"] = make_shared<JSON_entrada_perfilProducao_dengL>();
	contents["ReyInterno"] = make_shared<JSON_entrada_perfilProducao_ReyInterno>();
	contents["ReyExterno"] = make_shared<JSON_entrada_perfilProducao_ReyExterno>();
	contents["GrashInterno"] = make_shared<JSON_entrada_perfilProducao_GrashInterno>();
	contents["GrashExterno"] = make_shared<JSON_entrada_perfilProducao_GrashExterno>();
	contents["NusselInterno"] = make_shared<JSON_entrada_perfilProducao_NusselInterno>();
	contents["NusselExterno"] = make_shared<JSON_entrada_perfilProducao_NusselExterno>();
	contents["PrandtlInterno"] = make_shared<JSON_entrada_perfilProducao_PrandtlInterno>();
	contents["PrandtlExterno"] = make_shared<JSON_entrada_perfilProducao_PrandtlExterno>();
	contents["Froud"] = make_shared<JSON_entrada_perfilProducao_Froud>();
	contents["Rs"] = make_shared<JSON_entrada_perfilProducao_Rs>();
	contents["Bo"] = make_shared<JSON_entrada_perfilProducao_Bo>();
	contents["Hint"] = make_shared<JSON_entrada_perfilProducao_Hint>();
	contents["Hext"] = make_shared<JSON_entrada_perfilProducao_Hext>();
	contents["temperaturaAmbiente"] = make_shared<JSON_entrada_perfilProducao_temperaturaAmbiente>();
	contents["PrandtlGas"] = make_shared<JSON_entrada_perfilProducao_PrandtlGas>();
	contents["PrandtlLiquido"] = make_shared<JSON_entrada_perfilProducao_PrandtlLiquido>();
	contents["pseudoLiquido"] = make_shared<JSON_entrada_perfilProducao_pseudoLiquido>();
	contents["pseudoGas"] = make_shared<JSON_entrada_perfilProducao_pseudoGas>();
	contents["pseudoMist"] = make_shared<JSON_entrada_perfilProducao_pseudoMist>();
	contents["tempoResiLiqComp"] = make_shared<JSON_entrada_perfilProducao_TResi>();
	contents["RedutorAtrito"] = make_shared<JSON_entrada_perfilProducao_RedutorAtrito>();
	contents["angulo"] = make_shared<JSON_entrada_perfilProducao_angulo>();
	contents["diametroInterno"] = make_shared<JSON_entrada_perfilProducao_diametroInterno>();
	contents["tempParede"] = make_shared<JSON_entrada_perfilProducao_tempParede>();
	contents["subResfria"] = make_shared<JSON_entrada_perfilProducao_subResfria>();
	contents["dadosParafina"] = make_shared<JSON_entrada_perfilProducao_dadosParafina>();
}

JSON_entrada_perfilProducao_ativo& JSON_entrada_perfilProducao::ativo(){
	return static_cast<JSON_entrada_perfilProducao_ativo&>(*contents["ativo"].get());
}

JSON_entrada_perfilProducao_tempo& JSON_entrada_perfilProducao::tempo(){
	return static_cast<JSON_entrada_perfilProducao_tempo&>(*contents["tempo"].get());
}

JSON_entrada_perfilProducao_pressao& JSON_entrada_perfilProducao::pressao(){
	return static_cast<JSON_entrada_perfilProducao_pressao&>(*contents["pressao"].get());
}

JSON_entrada_perfilProducao_temperatura& JSON_entrada_perfilProducao::temperatura(){
	return static_cast<JSON_entrada_perfilProducao_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_perfilProducao_holdup& JSON_entrada_perfilProducao::holdup(){
	return static_cast<JSON_entrada_perfilProducao_holdup&>(*contents["holdup"].get());
}

//*** chris - hidratos
JSON_entrada_perfilProducao_FVH& JSON_entrada_perfilProducao::FVH(){
	return static_cast<JSON_entrada_perfilProducao_FVH&>(*contents["FVH"].get());
}

JSON_entrada_perfilProducao_bet& JSON_entrada_perfilProducao::bet(){
	return static_cast<JSON_entrada_perfilProducao_bet&>(*contents["bet"].get());
}

JSON_entrada_perfilProducao_ugs& JSON_entrada_perfilProducao::ugs(){
	return static_cast<JSON_entrada_perfilProducao_ugs&>(*contents["ugs"].get());
}

JSON_entrada_perfilProducao_uls& JSON_entrada_perfilProducao::uls(){
	return static_cast<JSON_entrada_perfilProducao_uls&>(*contents["uls"].get());
}

JSON_entrada_perfilProducao_ug& JSON_entrada_perfilProducao::ug(){
	return static_cast<JSON_entrada_perfilProducao_ug&>(*contents["ug"].get());
}

JSON_entrada_perfilProducao_ul& JSON_entrada_perfilProducao::ul(){
	return static_cast<JSON_entrada_perfilProducao_ul&>(*contents["ul"].get());
}

JSON_entrada_perfilProducao_arra& JSON_entrada_perfilProducao::arra(){
	return static_cast<JSON_entrada_perfilProducao_arra&>(*contents["arra"].get());
}

JSON_entrada_perfilProducao_viscosidadeLiquido& JSON_entrada_perfilProducao::viscosidadeLiquido(){
	return static_cast<JSON_entrada_perfilProducao_viscosidadeLiquido&>(*contents["viscosidadeLiquido"].get());
}

JSON_entrada_perfilProducao_viscosidadeGas& JSON_entrada_perfilProducao::viscosidadeGas(){
	return static_cast<JSON_entrada_perfilProducao_viscosidadeGas&>(*contents["viscosidadeGas"].get());
}

JSON_entrada_perfilProducao_rhog& JSON_entrada_perfilProducao::rhog(){
	return static_cast<JSON_entrada_perfilProducao_rhog&>(*contents["rhog"].get());
}

JSON_entrada_perfilProducao_rhol& JSON_entrada_perfilProducao::rhol(){
	return static_cast<JSON_entrada_perfilProducao_rhol&>(*contents["rhol"].get());
}

JSON_entrada_perfilProducao_rhoo& JSON_entrada_perfilProducao::rhoo(){
	return static_cast<JSON_entrada_perfilProducao_rhoo&>(*contents["rhoo"].get());
}

JSON_entrada_perfilProducao_rhoa& JSON_entrada_perfilProducao::rhoa(){
	return static_cast<JSON_entrada_perfilProducao_rhoa&>(*contents["rhoa"].get());
}

JSON_entrada_perfilProducao_rhoMix& JSON_entrada_perfilProducao::rhoMix(){
	return static_cast<JSON_entrada_perfilProducao_rhoMix&>(*contents["rhoMix"].get());
}

JSON_entrada_perfilProducao_RS& JSON_entrada_perfilProducao::RS(){
	return static_cast<JSON_entrada_perfilProducao_RS&>(*contents["RS"].get());
}

JSON_entrada_perfilProducao_vazaoMassicaGas& JSON_entrada_perfilProducao::vazaoMassicaGas(){
	return static_cast<JSON_entrada_perfilProducao_vazaoMassicaGas&>(*contents["vazaoMassicaGas"].get());
}

JSON_entrada_perfilProducao_vazaoMassicaLiquido& JSON_entrada_perfilProducao::vazaoMassicaLiquido(){
	return static_cast<JSON_entrada_perfilProducao_vazaoMassicaLiquido&>(*contents["vazaoMassicaLiquido"].get());
}

JSON_entrada_perfilProducao_c0& JSON_entrada_perfilProducao::c0(){
	return static_cast<JSON_entrada_perfilProducao_c0&>(*contents["c0"].get());
}

JSON_entrada_perfilProducao_ud& JSON_entrada_perfilProducao::ud(){
	return static_cast<JSON_entrada_perfilProducao_ud&>(*contents["ud"].get());
}

JSON_entrada_perfilProducao_RGO& JSON_entrada_perfilProducao::RGO(){
	return static_cast<JSON_entrada_perfilProducao_RGO&>(*contents["RGO"].get());
}

JSON_entrada_perfilProducao_deng& JSON_entrada_perfilProducao::deng(){
	return static_cast<JSON_entrada_perfilProducao_deng&>(*contents["deng"].get());
}

JSON_entrada_perfilProducao_yco2& JSON_entrada_perfilProducao::yco2(){
	return static_cast<JSON_entrada_perfilProducao_yco2&>(*contents["yco2"].get());
}

JSON_entrada_perfilProducao_calor& JSON_entrada_perfilProducao::calor(){
	return static_cast<JSON_entrada_perfilProducao_calor&>(*contents["calor"].get());
}

JSON_entrada_perfilProducao_masstrans& JSON_entrada_perfilProducao::masstrans(){
	return static_cast<JSON_entrada_perfilProducao_masstrans&>(*contents["masstrans"].get());
}

JSON_entrada_perfilProducao_cpgas& JSON_entrada_perfilProducao::cpgas(){
	return static_cast<JSON_entrada_perfilProducao_cpgas&>(*contents["cpgas"].get());
}

JSON_entrada_perfilProducao_cpliq& JSON_entrada_perfilProducao::cpliq(){
	return static_cast<JSON_entrada_perfilProducao_cpliq&>(*contents["cpliq"].get());
}

JSON_entrada_perfilProducao_QLstd& JSON_entrada_perfilProducao::QLstd(){
	return static_cast<JSON_entrada_perfilProducao_QLstd&>(*contents["QLstd"].get());
}

JSON_entrada_perfilProducao_QLWstd& JSON_entrada_perfilProducao::QLWstd(){
	return static_cast<JSON_entrada_perfilProducao_QLWstd&>(*contents["QLWstd"].get());
}

JSON_entrada_perfilProducao_QLstdTotal& JSON_entrada_perfilProducao::QLstdTotal(){
	return static_cast<JSON_entrada_perfilProducao_QLstdTotal&>(*contents["QLstdTotal"].get());
}

JSON_entrada_perfilProducao_QGstd& JSON_entrada_perfilProducao::QGstd(){
	return static_cast<JSON_entrada_perfilProducao_QGstd&>(*contents["QGstd"].get());
}

JSON_entrada_perfilProducao_api& JSON_entrada_perfilProducao::api(){
	return static_cast<JSON_entrada_perfilProducao_api&>(*contents["api"].get());
}

JSON_entrada_perfilProducao_bsw& JSON_entrada_perfilProducao::bsw(){
	return static_cast<JSON_entrada_perfilProducao_bsw&>(*contents["bsw"].get());
}

JSON_entrada_perfilProducao_hidro& JSON_entrada_perfilProducao::hidro(){
	return static_cast<JSON_entrada_perfilProducao_hidro&>(*contents["hidro"].get());
}

JSON_entrada_perfilProducao_fric& JSON_entrada_perfilProducao::fric(){
	return static_cast<JSON_entrada_perfilProducao_fric&>(*contents["fric"].get());
}

JSON_entrada_perfilProducao_Term1& JSON_entrada_perfilProducao::Term1(){
	return static_cast<JSON_entrada_perfilProducao_Term1&>(*contents["Term1"].get());
}

JSON_entrada_perfilProducao_Term2& JSON_entrada_perfilProducao::Term2(){
	return static_cast<JSON_entrada_perfilProducao_Term2&>(*contents["Term2"].get());
}

JSON_entrada_perfilProducao_dengD& JSON_entrada_perfilProducao::dengD(){
	return static_cast<JSON_entrada_perfilProducao_dengD&>(*contents["dengD"].get());
}

JSON_entrada_perfilProducao_dengL& JSON_entrada_perfilProducao::dengL(){
	return static_cast<JSON_entrada_perfilProducao_dengL&>(*contents["dengL"].get());
}

JSON_entrada_perfilProducao_ReyInterno& JSON_entrada_perfilProducao::ReyInterno(){
	return static_cast<JSON_entrada_perfilProducao_ReyInterno&>(*contents["ReyInterno"].get());
}

JSON_entrada_perfilProducao_ReyExterno& JSON_entrada_perfilProducao::ReyExterno(){
	return static_cast<JSON_entrada_perfilProducao_ReyExterno&>(*contents["ReyExterno"].get());
}

JSON_entrada_perfilProducao_GrashInterno& JSON_entrada_perfilProducao::GrashInterno(){
	return static_cast<JSON_entrada_perfilProducao_GrashInterno&>(*contents["GrashInterno"].get());
}

JSON_entrada_perfilProducao_GrashExterno& JSON_entrada_perfilProducao::GrashExterno(){
	return static_cast<JSON_entrada_perfilProducao_GrashExterno&>(*contents["GrashExterno"].get());
}

JSON_entrada_perfilProducao_NusselInterno& JSON_entrada_perfilProducao::NusselInterno(){
	return static_cast<JSON_entrada_perfilProducao_NusselInterno&>(*contents["NusselInterno"].get());
}

JSON_entrada_perfilProducao_NusselExterno& JSON_entrada_perfilProducao::NusselExterno(){
	return static_cast<JSON_entrada_perfilProducao_NusselExterno&>(*contents["NusselExterno"].get());
}

JSON_entrada_perfilProducao_PrandtlInterno& JSON_entrada_perfilProducao::PrandtlInterno(){
	return static_cast<JSON_entrada_perfilProducao_PrandtlInterno&>(*contents["PrandtlInterno"].get());
}

JSON_entrada_perfilProducao_PrandtlExterno& JSON_entrada_perfilProducao::PrandtlExterno(){
	return static_cast<JSON_entrada_perfilProducao_PrandtlExterno&>(*contents["PrandtlExterno"].get());
}

JSON_entrada_perfilProducao_Froud& JSON_entrada_perfilProducao::Froud(){
	return static_cast<JSON_entrada_perfilProducao_Froud&>(*contents["Froud"].get());
}

JSON_entrada_perfilProducao_Rs& JSON_entrada_perfilProducao::Rs(){
	return static_cast<JSON_entrada_perfilProducao_Rs&>(*contents["Rs"].get());
}

JSON_entrada_perfilProducao_Bo& JSON_entrada_perfilProducao::Bo(){
	return static_cast<JSON_entrada_perfilProducao_Bo&>(*contents["Bo"].get());
}

JSON_entrada_perfilProducao_Hint& JSON_entrada_perfilProducao::Hint(){
	return static_cast<JSON_entrada_perfilProducao_Hint&>(*contents["Hint"].get());
}

JSON_entrada_perfilProducao_Hext& JSON_entrada_perfilProducao::Hext(){
	return static_cast<JSON_entrada_perfilProducao_Hext&>(*contents["Hext"].get());
}

JSON_entrada_perfilProducao_temperaturaAmbiente& JSON_entrada_perfilProducao::temperaturaAmbiente(){
	return static_cast<JSON_entrada_perfilProducao_temperaturaAmbiente&>(*contents["temperaturaAmbiente"].get());
}

JSON_entrada_perfilProducao_PrandtlGas& JSON_entrada_perfilProducao::PrandtlGas(){
	return static_cast<JSON_entrada_perfilProducao_PrandtlGas&>(*contents["PrandtlGas"].get());
}

JSON_entrada_perfilProducao_PrandtlLiquido& JSON_entrada_perfilProducao::PrandtlLiquido(){
	return static_cast<JSON_entrada_perfilProducao_PrandtlLiquido&>(*contents["PrandtlLiquido"].get());
}

JSON_entrada_perfilProducao_pseudoLiquido& JSON_entrada_perfilProducao::pseudoLiquido(){
	return static_cast<JSON_entrada_perfilProducao_pseudoLiquido&>(*contents["pseudoLiquido"].get());
}

JSON_entrada_perfilProducao_pseudoGas& JSON_entrada_perfilProducao::pseudoGas(){
	return static_cast<JSON_entrada_perfilProducao_pseudoGas&>(*contents["pseudoGas"].get());
}

JSON_entrada_perfilProducao_pseudoMist& JSON_entrada_perfilProducao::pseudoMist(){
	return static_cast<JSON_entrada_perfilProducao_pseudoMist&>(*contents["pseudoMist"].get());
}

JSON_entrada_perfilProducao_TResi& JSON_entrada_perfilProducao::TResi(){
	return static_cast<JSON_entrada_perfilProducao_TResi&>(*contents["tempoResiLiqComp"].get());
}

JSON_entrada_perfilProducao_RedutorAtrito& JSON_entrada_perfilProducao::RedutorAtrito(){
	return static_cast<JSON_entrada_perfilProducao_RedutorAtrito&>(*contents["RedutorAtrito"].get());
}

JSON_entrada_perfilProducao_angulo& JSON_entrada_perfilProducao::angulo(){
	return static_cast<JSON_entrada_perfilProducao_angulo&>(*contents["angulo"].get());
}

JSON_entrada_perfilProducao_diametroInterno& JSON_entrada_perfilProducao::diametroInterno(){
	return static_cast<JSON_entrada_perfilProducao_diametroInterno&>(*contents["diametroInterno"].get());
}

JSON_entrada_perfilProducao_tempParede& JSON_entrada_perfilProducao::tempParede(){
	return static_cast<JSON_entrada_perfilProducao_tempParede&>(*contents["tempParede"].get());
}

JSON_entrada_perfilProducao_subResfria& JSON_entrada_perfilProducao::subResfria(){
	return static_cast<JSON_entrada_perfilProducao_subResfria&>(*contents["subResfria"].get());
}

JSON_entrada_perfilProducao_dadosParafina& JSON_entrada_perfilProducao::dadosParafina(){
	return static_cast<JSON_entrada_perfilProducao_dadosParafina&>(*contents["dadosParafina"].get());
}

JSON_entrada_perfilServico::JSON_entrada_perfilServico(){
	contents["ativo"] = make_shared<JSON_entrada_perfilServico_ativo>();
	contents["tempo"] = make_shared<JSON_entrada_perfilServico_tempo>();
	contents["pressao"] = make_shared<JSON_entrada_perfilServico_pressao>();
	contents["temperatura"] = make_shared<JSON_entrada_perfilServico_temperatura>();
	contents["ugs"] = make_shared<JSON_entrada_perfilServico_ugs>();
	contents["ug"] = make_shared<JSON_entrada_perfilServico_ug>();
	contents["tensaoCisalhamento"] = make_shared<JSON_entrada_perfilServico_tensaoCisalhamento>();
	contents["viscosidadeGas"] = make_shared<JSON_entrada_perfilServico_viscosidadeGas>();
	contents["rhog"] = make_shared<JSON_entrada_perfilServico_rhog>();
	contents["vazaoMassicaGas"] = make_shared<JSON_entrada_perfilServico_vazaoMassicaGas>();
	contents["QGstd"] = make_shared<JSON_entrada_perfilServico_QGstd>();
	contents["fric"] = make_shared<JSON_entrada_perfilServico_fric>();
	contents["calor"] = make_shared<JSON_entrada_perfilServico_calor>();
	contents["hidro"] = make_shared<JSON_entrada_perfilServico_hidro>();
	contents["FVHG"] = make_shared<JSON_entrada_perfilServico_FVHG>(); //chris - Hidratos
	contents["ReyInterno"] = make_shared<JSON_entrada_perfilServico_ReyInterno>();
	contents["ReyExterno"] = make_shared<JSON_entrada_perfilServico_ReyExterno>();
	contents["GrashInterno"] = make_shared<JSON_entrada_perfilServico_GrashInterno>();
	contents["GrashExterno"] = make_shared<JSON_entrada_perfilServico_GrashExterno>();
	contents["NusselInterno"] = make_shared<JSON_entrada_perfilServico_NusselInterno>();
	contents["NusselExterno"] = make_shared<JSON_entrada_perfilServico_NusselExterno>();
	contents["PrandtlInterno"] = make_shared<JSON_entrada_perfilServico_PrandtlInterno>();
	contents["PrandtlExterno"] = make_shared<JSON_entrada_perfilServico_PrandtlExterno>();
	contents["Hint"] = make_shared<JSON_entrada_perfilServico_Hint>();
	contents["Hext"] = make_shared<JSON_entrada_perfilServico_Hext>();
	contents["temperaturaAmbiente"] = make_shared<JSON_entrada_perfilServico_temperaturaAmbiente>();
	contents["angulo"] = make_shared<JSON_entrada_perfilServico_angulo>();
	contents["diametroInterno"] = make_shared<JSON_entrada_perfilServico_diametroInterno>();
	contents["tempParede"] = make_shared<JSON_entrada_perfilServico_tempParede>();
	contents["subResfria"] = make_shared<JSON_entrada_perfilServico_subResfria>();
}

JSON_entrada_perfilServico_ativo& JSON_entrada_perfilServico::ativo(){
	return static_cast<JSON_entrada_perfilServico_ativo&>(*contents["ativo"].get());
}

JSON_entrada_perfilServico_tempo& JSON_entrada_perfilServico::tempo(){
	return static_cast<JSON_entrada_perfilServico_tempo&>(*contents["tempo"].get());
}

JSON_entrada_perfilServico_pressao& JSON_entrada_perfilServico::pressao(){
	return static_cast<JSON_entrada_perfilServico_pressao&>(*contents["pressao"].get());
}

JSON_entrada_perfilServico_temperatura& JSON_entrada_perfilServico::temperatura(){
	return static_cast<JSON_entrada_perfilServico_temperatura&>(*contents["temperatura"].get());
}

JSON_entrada_perfilServico_ugs& JSON_entrada_perfilServico::ugs(){
	return static_cast<JSON_entrada_perfilServico_ugs&>(*contents["ugs"].get());
}

JSON_entrada_perfilServico_ug& JSON_entrada_perfilServico::ug(){
	return static_cast<JSON_entrada_perfilServico_ug&>(*contents["ug"].get());
}

JSON_entrada_perfilServico_tensaoCisalhamento& JSON_entrada_perfilServico::tensaoCisalhamento(){
	return static_cast<JSON_entrada_perfilServico_tensaoCisalhamento&>(*contents["tensaoCisalhamento"].get());
}

JSON_entrada_perfilServico_viscosidadeGas& JSON_entrada_perfilServico::viscosidadeGas(){
	return static_cast<JSON_entrada_perfilServico_viscosidadeGas&>(*contents["viscosidadeGas"].get());
}

JSON_entrada_perfilServico_rhog& JSON_entrada_perfilServico::rhog(){
	return static_cast<JSON_entrada_perfilServico_rhog&>(*contents["rhog"].get());
}

JSON_entrada_perfilServico_vazaoMassicaGas& JSON_entrada_perfilServico::vazaoMassicaGas(){
	return static_cast<JSON_entrada_perfilServico_vazaoMassicaGas&>(*contents["vazaoMassicaGas"].get());
}

JSON_entrada_perfilServico_QGstd& JSON_entrada_perfilServico::QGstd(){
	return static_cast<JSON_entrada_perfilServico_QGstd&>(*contents["QGstd"].get());
}

JSON_entrada_perfilServico_fric& JSON_entrada_perfilServico::fric(){
	return static_cast<JSON_entrada_perfilServico_fric&>(*contents["fric"].get());
}

JSON_entrada_perfilServico_calor& JSON_entrada_perfilServico::calor(){
	return static_cast<JSON_entrada_perfilServico_calor&>(*contents["calor"].get());
}

JSON_entrada_perfilServico_hidro& JSON_entrada_perfilServico::hidro(){
	return static_cast<JSON_entrada_perfilServico_hidro&>(*contents["hidro"].get());
}

JSON_entrada_perfilServico_FVHG& JSON_entrada_perfilServico::FVHG(){
	return static_cast<JSON_entrada_perfilServico_FVHG&>(*contents["FVHG"].get());
} //chris - Hidratos

JSON_entrada_perfilServico_ReyInterno& JSON_entrada_perfilServico::ReyInterno(){
	return static_cast<JSON_entrada_perfilServico_ReyInterno&>(*contents["ReyInterno"].get());
}

JSON_entrada_perfilServico_ReyExterno& JSON_entrada_perfilServico::ReyExterno(){
	return static_cast<JSON_entrada_perfilServico_ReyExterno&>(*contents["ReyExterno"].get());
}

JSON_entrada_perfilServico_GrashInterno& JSON_entrada_perfilServico::GrashInterno(){
	return static_cast<JSON_entrada_perfilServico_GrashInterno&>(*contents["GrashInterno"].get());
}

JSON_entrada_perfilServico_GrashExterno& JSON_entrada_perfilServico::GrashExterno(){
	return static_cast<JSON_entrada_perfilServico_GrashExterno&>(*contents["GrashExterno"].get());
}

JSON_entrada_perfilServico_NusselInterno& JSON_entrada_perfilServico::NusselInterno(){
	return static_cast<JSON_entrada_perfilServico_NusselInterno&>(*contents["NusselInterno"].get());
}

JSON_entrada_perfilServico_NusselExterno& JSON_entrada_perfilServico::NusselExterno(){
	return static_cast<JSON_entrada_perfilServico_NusselExterno&>(*contents["NusselExterno"].get());
}

JSON_entrada_perfilServico_PrandtlInterno& JSON_entrada_perfilServico::PrandtlInterno(){
	return static_cast<JSON_entrada_perfilServico_PrandtlInterno&>(*contents["PrandtlInterno"].get());
}

JSON_entrada_perfilServico_PrandtlExterno& JSON_entrada_perfilServico::PrandtlExterno(){
	return static_cast<JSON_entrada_perfilServico_PrandtlExterno&>(*contents["PrandtlExterno"].get());
}

JSON_entrada_perfilServico_Hint& JSON_entrada_perfilServico::Hint(){
	return static_cast<JSON_entrada_perfilServico_Hint&>(*contents["Hint"].get());
}

JSON_entrada_perfilServico_Hext& JSON_entrada_perfilServico::Hext(){
	return static_cast<JSON_entrada_perfilServico_Hext&>(*contents["Hext"].get());
}

JSON_entrada_perfilServico_temperaturaAmbiente& JSON_entrada_perfilServico::temperaturaAmbiente(){
	return static_cast<JSON_entrada_perfilServico_temperaturaAmbiente&>(*contents["temperaturaAmbiente"].get());
}

JSON_entrada_perfilServico_angulo& JSON_entrada_perfilServico::angulo(){
	return static_cast<JSON_entrada_perfilServico_angulo&>(*contents["angulo"].get());
}

JSON_entrada_perfilServico_diametroInterno& JSON_entrada_perfilServico::diametroInterno(){
	return static_cast<JSON_entrada_perfilServico_diametroInterno&>(*contents["diametroInterno"].get());
}

JSON_entrada_perfilServico_tempParede& JSON_entrada_perfilServico::tempParede(){
	return static_cast<JSON_entrada_perfilServico_tempParede&>(*contents["tempParede"].get());
}

JSON_entrada_perfilServico_subResfria& JSON_entrada_perfilServico::subResfria(){
	return static_cast<JSON_entrada_perfilServico_subResfria&>(*contents["subResfria"].get());
}

JSON_entrada_perfisTransP::JSON_entrada_perfisTransP(){
	contents["ativo"] = make_shared<JSON_entrada_perfisTransP_ativo>();
	contents["tempo"] = make_shared<JSON_entrada_perfisTransP_tempo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_perfisTransP_comprimentoMedido>();
}

JSON_entrada_perfisTransP_ativo& JSON_entrada_perfisTransP::ativo(){
	return static_cast<JSON_entrada_perfisTransP_ativo&>(*contents["ativo"].get());
}

JSON_entrada_perfisTransP_tempo& JSON_entrada_perfisTransP::tempo(){
	return static_cast<JSON_entrada_perfisTransP_tempo&>(*contents["tempo"].get());
}

JSON_entrada_perfisTransP_comprimentoMedido& JSON_entrada_perfisTransP::comprimentoMedido(){
	return static_cast<JSON_entrada_perfisTransP_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_perfisTransS::JSON_entrada_perfisTransS(){
	contents["ativo"] = make_shared<JSON_entrada_perfisTransS_ativo>();
	contents["tempo"] = make_shared<JSON_entrada_perfisTransS_tempo>();
	contents["comprimentoMedido"] = make_shared<JSON_entrada_perfisTransS_comprimentoMedido>();
}

JSON_entrada_perfisTransS_ativo& JSON_entrada_perfisTransS::ativo(){
	return static_cast<JSON_entrada_perfisTransS_ativo&>(*contents["ativo"].get());
}

JSON_entrada_perfisTransS_tempo& JSON_entrada_perfisTransS::tempo(){
	return static_cast<JSON_entrada_perfisTransS_tempo&>(*contents["tempo"].get());
}

JSON_entrada_perfisTransS_comprimentoMedido& JSON_entrada_perfisTransS::comprimentoMedido(){
	return static_cast<JSON_entrada_perfisTransS_comprimentoMedido&>(*contents["comprimentoMedido"].get());
}

JSON_entrada_separador::JSON_entrada_separador(){
	contents["ativo"] = make_shared<JSON_entrada_separador_ativo>();
	contents["tempo"] = make_shared<JSON_entrada_separador_tempo>();
	contents["pressao"] = make_shared<JSON_entrada_separador_pressao>();
}

JSON_entrada_separador_ativo& JSON_entrada_separador::ativo(){
	return static_cast<JSON_entrada_separador_ativo&>(*contents["ativo"].get());
}

JSON_entrada_separador_tempo& JSON_entrada_separador::tempo(){
	return static_cast<JSON_entrada_separador_tempo&>(*contents["tempo"].get());
}

JSON_entrada_separador_pressao& JSON_entrada_separador::pressao(){
	return static_cast<JSON_entrada_separador_pressao&>(*contents["pressao"].get());
}

JSON_entrada_correcao::JSON_entrada_correcao(){
	contents["ativo"] = make_shared<JSON_entrada_correcao_ativo>();
	contents["dPdLHidro"] = make_shared<JSON_entrada_correcao_dPdLHidro>();
	contents["dPdLFric"] = make_shared<JSON_entrada_correcao_dPdLFric>();
	contents["dTdL"] = make_shared<JSON_entrada_correcao_dTdL>();
}

JSON_entrada_correcao_ativo& JSON_entrada_correcao::ativo(){
	return static_cast<JSON_entrada_correcao_ativo&>(*contents["ativo"].get());
}

JSON_entrada_correcao_dPdLHidro& JSON_entrada_correcao::dPdLHidro(){
	return static_cast<JSON_entrada_correcao_dPdLHidro&>(*contents["dPdLHidro"].get());
}

JSON_entrada_correcao_dPdLHidro& JSON_entrada_correcao::dPdLFric(){
	return static_cast<JSON_entrada_correcao_dPdLFric&>(*contents["dPdLFric"].get());
}

JSON_entrada_correcao_dTdL& JSON_entrada_correcao::dTdL(){
	return static_cast<JSON_entrada_correcao_dTdL&>(*contents["dTdL"].get());
}

JSON_entrada_chokeSup::JSON_entrada_chokeSup(){
	contents["ativo"] = make_shared<JSON_entrada_chokeSup_ativo>();
	contents["curvaCV"] = make_shared<JSON_entrada_chokeSup_curvaCV>();
	contents["curvaDinamic"] = make_shared<JSON_entrada_chokeSup_curvaDinamic>();
	contents["tempo"] = make_shared<JSON_entrada_chokeSup_tempo>();
	contents["abertura"] = make_shared<JSON_entrada_chokeSup_abertura>();
	contents["coeficienteDescarga"] = make_shared<JSON_entrada_chokeSup_coeficienteDescarga>();
	contents["modelo"] = make_shared<JSON_entrada_chokeSup_modelo>();
	contents["x1"] = make_shared<JSON_entrada_chokeSup_x1>();
	contents["cv1"] = make_shared<JSON_entrada_chokeSup_cv1>();
}

JSON_entrada_chokeSup_ativo& JSON_entrada_chokeSup::ativo(){
	return static_cast<JSON_entrada_chokeSup_ativo&>(*contents["ativo"].get());
}

JSON_entrada_chokeSup_curvaCV& JSON_entrada_chokeSup::curvaCV(){
	return static_cast<JSON_entrada_chokeSup_curvaCV&>(*contents["curvaCV"].get());
}

JSON_entrada_chokeSup_curvaDinamic& JSON_entrada_chokeSup::curvaDinamic(){
	return static_cast<JSON_entrada_chokeSup_curvaDinamic&>(*contents["curvaDinamic"].get());
}

JSON_entrada_chokeSup_tempo& JSON_entrada_chokeSup::tempo(){
	return static_cast<JSON_entrada_chokeSup_tempo&>(*contents["tempo"].get());
}

JSON_entrada_chokeSup_abertura& JSON_entrada_chokeSup::abertura(){
	return static_cast<JSON_entrada_chokeSup_abertura&>(*contents["abertura"].get());
}

JSON_entrada_chokeSup_coeficienteDescarga& JSON_entrada_chokeSup::coeficienteDescarga(){
	return static_cast<JSON_entrada_chokeSup_coeficienteDescarga&>(*contents["coeficienteDescarga"].get());
}

JSON_entrada_chokeSup_modelo& JSON_entrada_chokeSup::modelo(){
	return static_cast<JSON_entrada_chokeSup_modelo&>(*contents["modelo"].get());
}

JSON_entrada_chokeSup_x1& JSON_entrada_chokeSup::x1(){
	return static_cast<JSON_entrada_chokeSup_x1&>(*contents["x1"].get());
}

JSON_entrada_chokeSup_cv1& JSON_entrada_chokeSup::cv1(){
	return static_cast<JSON_entrada_chokeSup_cv1&>(*contents["cv1"].get());
}

JSON_entrada_chokeInj::JSON_entrada_chokeInj(){
	contents["ativo"] = make_shared<JSON_entrada_chokeInj_ativo>();
	contents["tempo"] = make_shared<JSON_entrada_chokeInj_tempo>();
	contents["abertura"] = make_shared<JSON_entrada_chokeInj_abertura>();
	contents["coeficienteDescarga"] = make_shared<JSON_entrada_chokeInj_coeficienteDescarga>();
}

JSON_entrada_chokeInj_ativo& JSON_entrada_chokeInj::ativo(){
	return static_cast<JSON_entrada_chokeInj_ativo&>(*contents["ativo"].get());
}

JSON_entrada_chokeInj_tempo& JSON_entrada_chokeInj::tempo(){
	return static_cast<JSON_entrada_chokeInj_tempo&>(*contents["tempo"].get());
}

JSON_entrada_chokeInj_abertura& JSON_entrada_chokeInj::abertura(){
	return static_cast<JSON_entrada_chokeInj_abertura&>(*contents["abertura"].get());
}

JSON_entrada_chokeInj_coeficienteDescarga& JSON_entrada_chokeInj::coeficienteDescarga(){
	return static_cast<JSON_entrada_chokeInj_coeficienteDescarga&>(*contents["coeficienteDescarga"].get());
}

JSON_entrada_CondicaoContPocInjec::JSON_entrada_CondicaoContPocInjec(){
	contents["ativo"] = make_shared<JSON_entrada_CondicaoContPocInjec_ativo>();
	contents["usuario"] = make_shared<JSON_entrada_CondicaoContPocInjec_usuario>();
	contents["tipoFluido"] = make_shared<JSON_entrada_CondicaoContPocInjec_tipoFluido>();
	contents["salinidade"] = make_shared<JSON_entrada_CondicaoContPocInjec_salinidade>();
	contents["arquivoPvtsim"] = make_shared<JSON_entrada_CondicaoContPocInjec_arquivoPvtsim>();
	contents["condContorno"] = make_shared<JSON_entrada_CondicaoContPocInjec_condContorno>();
	contents["tempInj"] = make_shared<JSON_entrada_CondicaoContPocInjec_tempInj>();
	contents["vazLiq"] = make_shared<JSON_entrada_CondicaoContPocInjec_vazLiq>();
	contents["presInjec"] = make_shared<JSON_entrada_CondicaoContPocInjec_presInjec>();
	contents["presFundo"] = make_shared<JSON_entrada_CondicaoContPocInjec_presFundo>();
}

JSON_entrada_CondicaoContPocInjec_ativo& JSON_entrada_CondicaoContPocInjec::ativo(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_ativo&>(*contents["ativo"].get());
}

JSON_entrada_CondicaoContPocInjec_usuario& JSON_entrada_CondicaoContPocInjec::usuario(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_usuario&>(*contents["usuario"].get());
}

JSON_entrada_CondicaoContPocInjec_tipoFluido& JSON_entrada_CondicaoContPocInjec::tipoFluido(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_tipoFluido&>(*contents["tipoFluido"].get());
}

JSON_entrada_CondicaoContPocInjec_salinidade& JSON_entrada_CondicaoContPocInjec::salinidade(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_salinidade&>(*contents["salinidade"].get());
}

JSON_entrada_CondicaoContPocInjec_arquivoPvtsim& JSON_entrada_CondicaoContPocInjec::arquivoPvtsim(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_arquivoPvtsim&>(*contents["arquivoPvtsim"].get());
}

JSON_entrada_CondicaoContPocInjec_condContorno& JSON_entrada_CondicaoContPocInjec::condContorno(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_condContorno&>(*contents["condContorno"].get());
}

JSON_entrada_CondicaoContPocInjec_tempInj& JSON_entrada_CondicaoContPocInjec::tempInj(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_tempInj&>(*contents["tempInj"].get());
}

JSON_entrada_CondicaoContPocInjec_vazLiq& JSON_entrada_CondicaoContPocInjec::vazLiq(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_vazLiq&>(*contents["vazLiq"].get());
}

JSON_entrada_CondicaoContPocInjec_presInjec& JSON_entrada_CondicaoContPocInjec::presInjec(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_presInjec&>(*contents["presInjec"].get());
}

JSON_entrada_CondicaoContPocInjec_presFundo& JSON_entrada_CondicaoContPocInjec::presFundo(){
	return static_cast<JSON_entrada_CondicaoContPocInjec_presFundo&>(*contents["presFundo"].get());
}

JSON_entrada_intermitenciaSevera_Item::JSON_entrada_intermitenciaSevera_Item(){
	contents["ativo"] = make_shared<JSON_entrada_intermitenciaSevera_Item_ativo>();
	contents["id"] = make_shared<JSON_entrada_intermitenciaSevera_Item_id>();
	contents["inicioTrechoAcumula"] = make_shared<JSON_entrada_intermitenciaSevera_Item_inicioTrechoAcumula>();
	contents["fimTrechoAcumula"] = make_shared<JSON_entrada_intermitenciaSevera_Item_fimTrechoAcumula>();
	contents["fimTrechoColuna"] = make_shared<JSON_entrada_intermitenciaSevera_Item_fimTrechoColuna>();
	contents["fracaoVazioPenetracao"] = make_shared<JSON_entrada_intermitenciaSevera_Item_fracaoVazioPenetracao>();
	contents["criterio"] = make_shared<JSON_entrada_intermitenciaSevera_Item_criterio>();
}

JSON_entrada_intermitenciaSevera_Item_ativo& JSON_entrada_intermitenciaSevera_Item::ativo(){
	return static_cast<JSON_entrada_intermitenciaSevera_Item_ativo&>(*contents["ativo"].get());
}

JSON_entrada_intermitenciaSevera_Item_id& JSON_entrada_intermitenciaSevera_Item::id(){
	return static_cast<JSON_entrada_intermitenciaSevera_Item_id&>(*contents["id"].get());
}

JSON_entrada_intermitenciaSevera_Item_inicioTrechoAcumula& JSON_entrada_intermitenciaSevera_Item::inicioTrechoAcumula(){
	return static_cast<JSON_entrada_intermitenciaSevera_Item_inicioTrechoAcumula&>(*contents["inicioTrechoAcumula"].get());
}

JSON_entrada_intermitenciaSevera_Item_fimTrechoAcumula& JSON_entrada_intermitenciaSevera_Item::fimTrechoAcumula(){
	return static_cast<JSON_entrada_intermitenciaSevera_Item_fimTrechoAcumula&>(*contents["fimTrechoAcumula"].get());
}

JSON_entrada_intermitenciaSevera_Item_fimTrechoColuna& JSON_entrada_intermitenciaSevera_Item::fimTrechoColuna(){
	return static_cast<JSON_entrada_intermitenciaSevera_Item_fimTrechoColuna&>(*contents["fimTrechoColuna"].get());
}

JSON_entrada_intermitenciaSevera_Item_fracaoVazioPenetracao& JSON_entrada_intermitenciaSevera_Item::fracaoVazioPenetracao(){
	return static_cast<JSON_entrada_intermitenciaSevera_Item_fracaoVazioPenetracao&>(*contents["fracaoVazioPenetracao"].get());
}

JSON_entrada_intermitenciaSevera_Item_criterio& JSON_entrada_intermitenciaSevera_Item::criterio(){
	return static_cast<JSON_entrada_intermitenciaSevera_Item_criterio&>(*contents["criterio"].get());
}

JSON_entrada::JSON_entrada(){
	contents["sistema"] = make_shared<JSON_entrada_sistema>();
	contents["versaoJson"] = make_shared<JSON_entrada_versaoJson>();
	contents["configuracaoInicial"] = make_shared<JSON_entrada_configuracaoInicial>();
	contents["tabela"] = make_shared<JSON_entrada_tabela>();
	contents["parafina"] = make_shared<JSON_entrada_parafina>();
	contents["fluidoGas"] = make_shared<JSON_entrada_fluidoGas>();
	contents["fluidosProducao"] = make_shared<JSON_entrada_fluidosProducao>();
	contents["fluidoComplementar"] = make_shared<JSON_entrada_fluidoComplementar>();
	contents["valvula"] = make_shared<JSON_entrada_valvula>();
	contents["fonteLiquido"] = make_shared<JSON_entrada_fonteLiquido>();
	contents["fonteMassa"] = make_shared<JSON_entrada_fonteMassa>();
	contents["fonteGas"] = make_shared<JSON_entrada_fonteGas>();
	contents["poroRadial"] = make_shared<JSON_entrada_fontePoroRadial>();
	contents["poro2D"] = make_shared<JSON_entrada_fontePoro2D>();
	contents["fonteGasLift"] = make_shared<JSON_entrada_fonteGasLift>();
	contents["material"] = make_shared<JSON_entrada_material>();
	contents["secaoTransversal"] = make_shared<JSON_entrada_secaoTransversal>();
	contents["tempo"] = make_shared<JSON_entrada_tempo>();
	contents["dutosProducao"] = make_shared<JSON_entrada_dutosProducao>();
	contents["dutosServico"] = make_shared<JSON_entrada_dutosServico>();
	contents["hidrato"] = make_shared<JSON_entrada_hidrato>();
	contents["ipr"] = make_shared<JSON_entrada_ipr>();
	contents["fonteChoke"] = make_shared<JSON_entrada_fonteChoke>();
	contents["bcs"] = make_shared<JSON_entrada_bcs>();
	contents["bombaVolumetrica"] = make_shared<JSON_entrada_bombaVolumetrica>();
	contents["deltaPressao"] = make_shared<JSON_entrada_deltaPressao>();
	contents["master1"] = make_shared<JSON_entrada_master1>();
	contents["master2"] = make_shared<JSON_entrada_master2>();
	contents["pig"] = make_shared<JSON_entrada_pig>();
	contents["fontePressao"] = make_shared<JSON_entrada_fontePressao>();
	contents["tendP"] = make_shared<JSON_entrada_tendP>();
	contents["tendTransP"] = make_shared<JSON_entrada_tendTransP>();
	contents["tendS"] = make_shared<JSON_entrada_tendS>();
	contents["tendTransS"] = make_shared<JSON_entrada_tendTransS>();
	contents["tela"] = make_shared<JSON_entrada_tela>();
	contents["gasInj"] = make_shared<JSON_entrada_gasInj>();
	contents["perfilProducao"] = make_shared<JSON_entrada_perfilProducao>();
	contents["perfilServico"] = make_shared<JSON_entrada_perfilServico>();
	contents["perfisTransP"] = make_shared<JSON_entrada_perfisTransP>();
	contents["perfisTransS"] = make_shared<JSON_entrada_perfisTransS>();
	contents["separador"] = make_shared<JSON_entrada_separador>();
	contents["correcao"] = make_shared<JSON_entrada_correcao>();
	contents["chokeSup"] = make_shared<JSON_entrada_chokeSup>();
	contents["chokeInj"] = make_shared<JSON_entrada_chokeInj>();
	contents["CondicaoContPocInjec"] = make_shared<JSON_entrada_CondicaoContPocInjec>();
	contents["intermitenciaSevera"] = make_shared<JSON_entrada_intermitenciaSevera>();
}

JSON_entrada_sistema& JSON_entrada::sistema(){
	return static_cast<JSON_entrada_sistema&>(*contents["sistema"].get());
}

JSON_entrada_versaoJson& JSON_entrada::versaoJson(){
	return static_cast<JSON_entrada_versaoJson&>(*contents["versaoJson"].get());
}

JSON_entrada_configuracaoInicial& JSON_entrada::configuracaoInicial(){
	return static_cast<JSON_entrada_configuracaoInicial&>(*contents["configuracaoInicial"].get());
}

JSON_entrada_tabela& JSON_entrada::tabela(){
	return static_cast<JSON_entrada_tabela&>(*contents["tabela"].get());
}

JSON_entrada_parafina& JSON_entrada::parafina(){
	return static_cast<JSON_entrada_parafina&>(*contents["parafina"].get());
}

JSON_entrada_fluidoGas& JSON_entrada::fluidoGas(){
	return static_cast<JSON_entrada_fluidoGas&>(*contents["fluidoGas"].get());
}

JSON_entrada_fluidosProducao& JSON_entrada::fluidosProducao(){
	return static_cast<JSON_entrada_fluidosProducao&>(*contents["fluidosProducao"].get());
}

JSON_entrada_fluidoComplementar& JSON_entrada::fluidoComplementar(){
	return static_cast<JSON_entrada_fluidoComplementar&>(*contents["fluidoComplementar"].get());
}

JSON_entrada_valvula& JSON_entrada::valvula(){
	return static_cast<JSON_entrada_valvula&>(*contents["valvula"].get());
}

JSON_entrada_fonteLiquido& JSON_entrada::fonteLiquido(){
	return static_cast<JSON_entrada_fonteLiquido&>(*contents["fonteLiquido"].get());
}

JSON_entrada_fonteMassa& JSON_entrada::fonteMassa(){
	return static_cast<JSON_entrada_fonteMassa&>(*contents["fonteMassa"].get());
}

JSON_entrada_fonteGas& JSON_entrada::fonteGas(){
	return static_cast<JSON_entrada_fonteGas&>(*contents["fonteGas"].get());
}

JSON_entrada_fontePoroRadial& JSON_entrada::fontePoroRadial(){
	return static_cast<JSON_entrada_fontePoroRadial&>(*contents["poroRadial"].get());
}

JSON_entrada_fontePoro2D& JSON_entrada::fontePoro2D(){
	return static_cast<JSON_entrada_fontePoro2D&>(*contents["poro2D"].get());
}

JSON_entrada_fonteGasLift& JSON_entrada::fonteGasLift(){
	return static_cast<JSON_entrada_fonteGasLift&>(*contents["fonteGasLift"].get());
}

JSON_entrada_material& JSON_entrada::material(){
	return static_cast<JSON_entrada_material&>(*contents["material"].get());
}

JSON_entrada_secaoTransversal& JSON_entrada::secaoTransversal(){
	return static_cast<JSON_entrada_secaoTransversal&>(*contents["secaoTransversal"].get());
}

JSON_entrada_tempo& JSON_entrada::tempo(){
	return static_cast<JSON_entrada_tempo&>(*contents["tempo"].get());
}

JSON_entrada_dutosProducao& JSON_entrada::dutosProducao(){
	return static_cast<JSON_entrada_dutosProducao&>(*contents["dutosProducao"].get());
}

JSON_entrada_dutosServico& JSON_entrada::dutosServico(){
	return static_cast<JSON_entrada_dutosServico&>(*contents["dutosServico"].get());
}

JSON_entrada_hidrato& JSON_entrada::hidrato(){
	return static_cast<JSON_entrada_hidrato&>(*contents["hidrato"].get());
}

JSON_entrada_ipr& JSON_entrada::ipr(){
	return static_cast<JSON_entrada_ipr&>(*contents["ipr"].get());
}

JSON_entrada_fonteChoke& JSON_entrada::fonteChoke(){
	return static_cast<JSON_entrada_fonteChoke&>(*contents["fonteChoke"].get());
}

JSON_entrada_bcs& JSON_entrada::bcs(){
	return static_cast<JSON_entrada_bcs&>(*contents["bcs"].get());
}

JSON_entrada_bombaVolumetrica& JSON_entrada::bombaVolumetrica(){
	return static_cast<JSON_entrada_bombaVolumetrica&>(*contents["bombaVolumetrica"].get());
}

JSON_entrada_deltaPressao& JSON_entrada::deltaPressao(){
	return static_cast<JSON_entrada_deltaPressao&>(*contents["deltaPressao"].get());
}

JSON_entrada_master1& JSON_entrada::master1(){
	return static_cast<JSON_entrada_master1&>(*contents["master1"].get());
}

JSON_entrada_master2& JSON_entrada::master2(){
	return static_cast<JSON_entrada_master2&>(*contents["master2"].get());
}

JSON_entrada_pig& JSON_entrada::pig(){
	return static_cast<JSON_entrada_pig&>(*contents["pig"].get());
}

JSON_entrada_fontePressao& JSON_entrada::fontePressao(){
	return static_cast<JSON_entrada_fontePressao&>(*contents["fontePressao"].get());
}

JSON_entrada_tendP& JSON_entrada::tendP(){
	return static_cast<JSON_entrada_tendP&>(*contents["tendP"].get());
}

JSON_entrada_tendTransP& JSON_entrada::tendTransP(){
	return static_cast<JSON_entrada_tendTransP&>(*contents["tendTransP"].get());
}

JSON_entrada_tendS& JSON_entrada::tendS(){
	return static_cast<JSON_entrada_tendS&>(*contents["tendS"].get());
}

JSON_entrada_tendTransS& JSON_entrada::tendTransS(){
	return static_cast<JSON_entrada_tendTransS&>(*contents["tendTransS"].get());
}

JSON_entrada_tela& JSON_entrada::tela(){
	return static_cast<JSON_entrada_tela&>(*contents["tela"].get());
}

JSON_entrada_gasInj& JSON_entrada::gasInj(){
	return static_cast<JSON_entrada_gasInj&>(*contents["gasInj"].get());
}

JSON_entrada_perfilProducao& JSON_entrada::perfilProducao(){
	return static_cast<JSON_entrada_perfilProducao&>(*contents["perfilProducao"].get());
}

JSON_entrada_perfilServico& JSON_entrada::perfilServico(){
	return static_cast<JSON_entrada_perfilServico&>(*contents["perfilServico"].get());
}

JSON_entrada_perfisTransP& JSON_entrada::perfisTransP(){
	return static_cast<JSON_entrada_perfisTransP&>(*contents["perfisTransP"].get());
}

JSON_entrada_perfisTransS& JSON_entrada::perfisTransS(){
	return static_cast<JSON_entrada_perfisTransS&>(*contents["perfisTransS"].get());
}

JSON_entrada_separador& JSON_entrada::separador(){
	return static_cast<JSON_entrada_separador&>(*contents["separador"].get());
}

JSON_entrada_correcao& JSON_entrada::correcao(){
	return static_cast<JSON_entrada_correcao&>(*contents["correcao"].get());
}

JSON_entrada_chokeSup& JSON_entrada::chokeSup(){
	return static_cast<JSON_entrada_chokeSup&>(*contents["chokeSup"].get());
}

JSON_entrada_chokeInj& JSON_entrada::chokeInj(){
	return static_cast<JSON_entrada_chokeInj&>(*contents["chokeInj"].get());
}

JSON_entrada_CondicaoContPocInjec& JSON_entrada::CondicaoContPocInjec(){
	return static_cast<JSON_entrada_CondicaoContPocInjec&>(*contents["CondicaoContPocInjec"].get());
}

JSON_entrada_intermitenciaSevera& JSON_entrada::intermitenciaSevera(){
	return static_cast<JSON_entrada_intermitenciaSevera&>(*contents["intermitenciaSevera"].get());
}
