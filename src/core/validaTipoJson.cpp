/*
 * validaJson.cpp
 *
 *  Created on: 20 de mar. de 2026
 *      Author: Eduardo
 */

#include "validaTipoJson.h"

void validadorTipo::valida_configuracao_inicial(Value &configuracao_inicial_json, std::vector<std::string> &erros, bool &sucesso) {

    // criar variavel para o nome da propriedade json em processo de valida
    string chaveJson("#/configuracaoInicial");

    // Validar se chokeSup é um objeto
    if (!configuracao_inicial_json.IsObject()) {
        sucesso = false;
        erros.push_back("configuracaoInicial: deve ser do tipo objeto");
        return;
    }

    if (configuracao_inicial_json.HasMember("sentidoGeometriaSegueEscoamento")) {
        if (!configuracao_inicial_json["sentidoGeometriaSegueEscoamento"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: sentidoGeometriaSegueEscoamento deve ser do tipo booleano");
        }
    }

    if (configuracao_inicial_json.HasMember("saidaClassica")) {
        if (!configuracao_inicial_json["saidaClassica"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: saidaClassica deve ser do tipo booleano");
        }
    }

    if (configuracao_inicial_json.HasMember("AP"))
        if (!configuracao_inicial_json["AP"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: AP deve ser do tipo booleano");
        }

    if (configuracao_inicial_json.HasMember("paralelizaAP"))
        if (!configuracao_inicial_json["paralelizaAP"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: paralelizaAP deve ser do tipo booleano");
        }

    if (configuracao_inicial_json.HasMember("arquivoAP"))
        if (!configuracao_inicial_json["arquivoAP"].IsString()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: arquivoAP deve ser do tipo string");
        }

    if (configuracao_inicial_json.HasMember("HISEP"))
        if (!configuracao_inicial_json["HISEP"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: HISEP deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("chutePerm"))
        if (!configuracao_inicial_json["chutePerm"].IsNumber()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: chutePerm deve ser do tipo number");
        }

    // caso nao seja simulacao de poco injetor
    if (configuracao_inicial_json.HasMember("linhaGas"))
        if (!configuracao_inicial_json["linhaGas"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: linhaGas deve ser do tipo Boolean");
        }

    if (configuracao_inicial_json.HasMember("saidaTela"))
        if (!configuracao_inicial_json["saidaTela"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: saidaTela deve ser do tipo Boolean");
        }

    if (configuracao_inicial_json.HasMember("equilibrioTermico"))
        if (!configuracao_inicial_json["equilibrioTermico"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: equilibrioTermico deve ser do tipo Boolean");
        }

    if (configuracao_inicial_json.HasMember("latente"))
        if (!configuracao_inicial_json["latente"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: latente deve ser do tipo Boolean");
        }

    if (configuracao_inicial_json.HasMember("condlatente"))
        if (!configuracao_inicial_json["condlatente"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: condlatente deve ser do tipo Boolean");
        }

    if (configuracao_inicial_json.HasMember("pvtsimArq"))
        if (!configuracao_inicial_json["pvtsimArq"].IsString()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: pvtsimArq deve ser do tipo string");
        }

    if (configuracao_inicial_json.HasMember("modeloFluidoTabelaFlash"))
        if (!configuracao_inicial_json["modeloFluidoTabelaFlash"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modeloFluidoTabelaFlash deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("modoParafina"))
        if (!configuracao_inicial_json["modoParafina"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modoParafina deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("modeloFluidoComposicional"))
        if (!configuracao_inicial_json["modeloFluidoComposicional"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modeloFluidoComposicional deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("modeloTabelaDinamica"))
        if (!configuracao_inicial_json["modeloTabelaDinamica"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modeloTabelaDinamica deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("modeloCp"))
        if (!configuracao_inicial_json["modeloCp"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modeloCp deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("modeloJTL"))
        if (!configuracao_inicial_json["modeloJTL"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modeloJTL deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("tabP"))
        if (!configuracao_inicial_json["tabP"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: tabP deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("trackRgo"))
        if (!configuracao_inicial_json["trackRgo"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: trackRgo deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("trackDensidadeGas"))
        if (!configuracao_inicial_json["trackDensidadeGas"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: trackDensidadeGas deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("correcaoDenGasLivreBlackOil"))
        if (!configuracao_inicial_json["correcaoDenGasLivreBlackOil"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: correcaoDenGasLivreBlackOil deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("tabelaRSPB"))
        if (!configuracao_inicial_json["tabelaRSPB"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: tabelaRSPB deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("propFluido"))
        if (!configuracao_inicial_json["propFluido"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: propFluido deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("tabG"))
        if (!configuracao_inicial_json["tabG"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: tabG deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("escorregamentoPermanente"))
        if (!configuracao_inicial_json["escorregamentoPermanente"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: escorregamentoPermanente deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("escorregamentoTransiente"))
        if (!configuracao_inicial_json["escorregamentoTransiente"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: escorregamentoTransiente deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("mapaArranjo"))
        if (!configuracao_inicial_json["mapaArranjo"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: mapaArranjo deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("tipoFluido"))
        if (!configuracao_inicial_json["tipoFluido"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: tipoFluido deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("tipoModeloDrift"))
        if (!configuracao_inicial_json["tipoModeloDrift"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: tipoModeloDrift deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("condicaoInicial"))
        if (!configuracao_inicial_json["condicaoInicial"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: condicaoInicial deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("ordemperm"))
        if (!configuracao_inicial_json["ordemperm"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: ordemperm deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("SnapShotArq"))
        if (!configuracao_inicial_json["SnapShotArq"].IsString()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: SnapShotArq deve ser do tipo string");
        }

    if (configuracao_inicial_json.HasMember("SalinidadeFluido"))
        if (!configuracao_inicial_json["SalinidadeFluido"].IsNumber()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: SalinidadeFluido deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("comprimentoMedidoInterfaceLinhaGas"))
        if (!configuracao_inicial_json["comprimentoMedidoInterfaceLinhaGas"].IsNumber()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: comprimentoMedidoInterfaceLinhaGas deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("comprimentoMedidoInterfaceLinhaProd"))
        if (!configuracao_inicial_json["comprimentoMedidoInterfaceLinhaProd"].IsNumber()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: comprimentoMedidoInterfaceLinhaProd deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("controleDescarga"))
        if (!configuracao_inicial_json["controleDescarga"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: controleDescarga deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("parametrosDescarga")) {
        if (!configuracao_inicial_json["parametrosDescarga"].IsObject()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: parametrosDescarga deve ser um objeto JSON");
        }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("vazaoLimiteDescarga"))
            if (!configuracao_inicial_json["parametrosDescarga"]["vazaoLimiteDescarga"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: vazaoLimiteDescarga deve ser do tipo number");
            }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("pressaoLimiteDescarga"))
            if (!configuracao_inicial_json["parametrosDescarga"]["pressaoLimiteDescarga"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: pressaoLimiteDescarga deve ser do tipo number");
            }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("pressaoMinimaDescarga"))
            if (!configuracao_inicial_json["parametrosDescarga"]["pressaoMinimaDescarga"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: pressaoMinimaDescarga deve ser do tipo number");
            }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("pressaoTrabalhoDescargaGas"))
            if (!configuracao_inicial_json["parametrosDescarga"]["pressaoTrabalhoDescargaGas"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: pressaoTrabalhoDescargaGas deve ser do tipo number");
            }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("pressaoLimiteDescargaGas"))
            if (!configuracao_inicial_json["parametrosDescarga"]["pressaoLimiteDescargaGas"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: pressaoLimiteDescargaGas deve ser do tipo number");
            }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("pressaoMinimaDescargaGas"))
            if (!configuracao_inicial_json["parametrosDescarga"]["pressaoMinimaDescargaGas"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: pressaoMinimaDescargaGas deve ser do tipo number");
            }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("temperaturaDescarga"))
            if (!configuracao_inicial_json["parametrosDescarga"]["temperaturaDescarga"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: temperaturaDescarga deve ser do tipo number");
            }

        if (configuracao_inicial_json["parametrosDescarga"].HasMember("tempoLatencia"))
            if (!configuracao_inicial_json["parametrosDescarga"]["tempoLatencia"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:parametrosDescarga: tempoLatencia deve ser do tipo number");
            }
    }

    if (configuracao_inicial_json.HasMember("iniFluidoP"))
        if (!configuracao_inicial_json["iniFluidoP"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: iniFluidoP deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("transiente"))
        if (!configuracao_inicial_json["transiente"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: transiente deve ser do tipo booleana");
        }

    if (configuracao_inicial_json.HasMember("transferenciaMassa"))
        if (!configuracao_inicial_json["transferenciaMassa"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: transferenciaMassa deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("CheckValve"))
        if (!configuracao_inicial_json["CheckValve"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: CheckValve deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("modoDifus3D"))
        if (!configuracao_inicial_json["modoDifus3D"].IsBool()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modoDifus3D deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("threadP3D"))
        if (!configuracao_inicial_json["threadP3D"].IsInt()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: threadP3D deve ser do tipo inteiro");
        }

    if (configuracao_inicial_json.HasMember("modoDifus3DJson"))
        if (!configuracao_inicial_json["modoDifus3DJson"].IsString()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: modoDifus3DJson deve ser do tipo string");
        }

    if (configuracao_inicial_json.HasMember("Avancado")) {
        if (!configuracao_inicial_json["Avancado"].IsObject()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: Avancado deve ser um objeto Json");
        }

        if (configuracao_inicial_json["Avancado"].HasMember("CriterioMonofasico"))
            if (!configuracao_inicial_json["Avancado"]["CriterioMonofasico"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: CriterioMonofasico deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("CriterioCondensacao"))
            if (!configuracao_inicial_json["Avancado"]["CriterioCondensacao"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: CriterioCondensaca deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("taxaDespre"))
            if (!configuracao_inicial_json["Avancado"]["taxaDespre"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: taxaDespre deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("MedSimpPresFront"))
            if (!configuracao_inicial_json["Avancado"]["MedSimpPresFront"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: MedSimpPresFront deve ser do tipo Booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("JTLiquidoSimple"))
            if (!configuracao_inicial_json["Avancado"]["JTLiquidoSimple"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: JTLiquidoSimple deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("RelaxaDTChoke"))
            if (!configuracao_inicial_json["Avancado"]["RelaxaDTChoke"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: RelaxaDTChoke deve ser do tipo Booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("desligaPenalizaDT"))
            if (!configuracao_inicial_json["Avancado"]["desligaPenalizaDT"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: desligaPenalizaDT deve ser do tipo Booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("controleDTvalv"))
            if (!configuracao_inicial_json["Avancado"]["controleDTvalv"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: controleDTvalv deve ser do tipo Booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("limTransMass"))
            if (!configuracao_inicial_json["Avancado"]["limTransMass"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: limTransMass deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("CriterioDTMin"))
            if (!configuracao_inicial_json["Avancado"]["CriterioDTMin"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: CriterioDTMin deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("CriterioBuscaFalsaCorda"))
            if (!configuracao_inicial_json["Avancado"]["CriterioBuscaFalsaCorda"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: CriterioBuscaFalsaCorda deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("CriterioConvergPerm"))
            if (!configuracao_inicial_json["Avancado"]["CriterioConvergPerm"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: CriterioConvergPerm deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("AceleraConvergPerm"))
            if (!configuracao_inicial_json["Avancado"]["AceleraConvergPerm"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: AceleraConvergPerm deve ser do tipo booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("escorregamentoCelulaContorno"))
            if (!configuracao_inicial_json["Avancado"]["escorregamentoCelulaContorno"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: escorregamentoCelulaContorno deve ser do tipo booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("correcaoContracorPerm"))
            if (!configuracao_inicial_json["Avancado"]["correcaoContracorPerm"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: correcaoContracorPerm deve ser do tipo booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("estabCol"))
            if (!configuracao_inicial_json["Avancado"]["estabCol"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: estabCol deve ser do tipo booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("acopColAnulPermForte"))
            if (!configuracao_inicial_json["Avancado"]["acopColAnulPermForte"].IsInt()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: acopColAnulPermForte deve ser do tipo inteiro");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("mudaArea"))
            if (!configuracao_inicial_json["Avancado"]["mudaArea"].IsInt()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: mudaArea deve ser do tipo inteiro");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("nthrd"))
            if (!configuracao_inicial_json["Avancado"]["nthrd"].IsInt()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: nthrd deve ser do tipo inteiro");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("nthrdMatriz"))
            if (!configuracao_inicial_json["Avancado"]["nthrdMatriz"].IsInt()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: nthrdMatriz deve ser do tipo inteiro");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("correcaoModComp")) {
            if (!configuracao_inicial_json["Avancado"]["correcaoModComp"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: correcaoModComp deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["Avancado"]["correcaoModComp"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["Avancado"]["correcaoModComp"][i].IsBool()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:Avancado: correcaoModComindice " + std::to_string(i) + " deve ser do tipo booleana");
                    }
                }
            }
        }

        if (configuracao_inicial_json["Avancado"].HasMember("TcorrecaoModComp")) {
            if (!configuracao_inicial_json["Avancado"]["TcorrecaoModComp"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: TcorrecaoModComp deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["Avancado"]["TcorrecaoModComp"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["Avancado"]["TcorrecaoModComp"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:Avancado: TcorrecaoModComp " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["Avancado"].HasMember("sonico")) {
            if (!configuracao_inicial_json["Avancado"]["sonico"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: sonico deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["Avancado"]["sonico"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["Avancado"]["sonico"][i].IsBool()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:Avancado: sonico " + std::to_string(i) + " deve ser do tipo booleana");
                    }
                }
            }
        }

        if (configuracao_inicial_json["Avancado"].HasMember("Tsonico")) {
            if (!configuracao_inicial_json["Avancado"]["Tsonico"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: Tsonico deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["Avancado"]["Tsonico"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["Avancado"]["Tsonico"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:Avancado: Tsonico " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["Avancado"].HasMember("desligaDeriTransMassDTemp"))
            if (!configuracao_inicial_json["Avancado"]["desligaDeriTransMassDTemp"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: desligaDeriTransMassDTemp deve ser do tipo Booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("corrigeContSep"))
            if (!configuracao_inicial_json["Avancado"]["corrigeContSep"].IsBool()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: corrigeContSep deve ser do tipo Booleana");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("miniTabDinAtraso"))
            if (!configuracao_inicial_json["Avancado"]["miniTabDinAtraso"].IsInt()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: miniTabDinAtraso deve ser do tipo inteiro");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("miniTabDinDp"))
            if (!configuracao_inicial_json["Avancado"]["miniTabDinDp"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: miniTabDinDp deve ser do tipo number");
            }

        if (configuracao_inicial_json["Avancado"].HasMember("miniTabDinDt"))
            if (!configuracao_inicial_json["Avancado"]["miniTabDinDt"].IsNumber()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Avancado: miniTabDinDt deve ser do tipo number");
            }
    }

    if (configuracao_inicial_json.HasMember("condicaoPressao")) {
        if (!configuracao_inicial_json["condicaoPressao"].IsObject()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: condicaoPressao deve ser um objeto Json");
        }

        if (configuracao_inicial_json["condicaoPressao"].HasMember("indFluido"))
            if (!configuracao_inicial_json["condicaoPressao"]["indFluido"].IsInt()) {
                sucesso = false;
            }

        if (configuracao_inicial_json["condicaoPressao"].HasMember("pressao")) {
            if (!configuracao_inicial_json["condicaoPressao"]["pressao"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: pressao deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoPressao"]["pressao"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoPressao"]["pressao"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoPressao: pressao " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoPressao"].HasMember("temperatura")) {
            if (!configuracao_inicial_json["condicaoPressao"]["temperatura"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: temperatura deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoPressao"]["temperatura"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoPressao"]["temperatura"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoPressao: temperatura " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoPressao"].HasMember("titulo")) {
            if (!configuracao_inicial_json["condicaoPressao"]["titulo"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: titulo deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoPressao"]["titulo"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoPressao"]["titulo"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoPressao: titulo " + std::to_string(i) + "  deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoPressao"].HasMember("razaoBeta")) {
            if (!configuracao_inicial_json["condicaoPressao"]["razaoBeta"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: razaoBeta deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoPressao"]["razaoBeta"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoPressao"]["razaoBeta"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoPressao: pressao " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoPressao"].HasMember("tempo")) {
            if (!configuracao_inicial_json["condicaoPressao"]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: tempo deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoPressao"]["tempo"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoPressao"]["tempo"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoPressao: tempo " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }
    }

    if (configuracao_inicial_json.HasMember("condicaoVazPres")) {
        if (!configuracao_inicial_json["condicaoVazPres"].IsObject()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: condicaoVazPressao deve ser um objeto Json");
        }

        if (configuracao_inicial_json["condicaoVazPres"].HasMember("pressao")) {
            if (!configuracao_inicial_json["condicaoVazPres"]["pressao"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: pressao deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoVazPres"]["pressao"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoVazPres"]["pressao"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoVazPressao: pressao " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoVazPres"].HasMember("VazMass")) {
            if (!configuracao_inicial_json["condicaoVazPres"]["VazMass"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: VazMass deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoVazPres"]["VazMass"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoVazPres"]["VazMass"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoVazPressao: VazMass " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoVazPres"].HasMember("temperatura")) {
            if (!configuracao_inicial_json["condicaoVazPres"]["temperatura"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoVazPressao: temperatura deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoVazPres"]["temperatura"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoVazPres"]["temperatura"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoVazPressao: temperatura " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoVazPres"].HasMember("titulo")) {
            if (!configuracao_inicial_json["condicaoVazPres"]["titulo"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoPressao: titulo deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoVazPres"]["titulo"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoVazPres"]["titulo"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoVazPressao: titulo " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoVazPres"].HasMember("razaoBeta")) {
            if (!configuracao_inicial_json["condicaoVazPres"]["razaoBeta"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoVazPressao: razaoBeta deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoVazPres"]["razaoBeta"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoVazPres"]["razaoBeta"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoVazPressao: razaoBeta " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }

        if (configuracao_inicial_json["condicaoVazPres"].HasMember("tempo")) {
            if (!configuracao_inicial_json["condicaoVazPres"]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:condicaoVazPressao: tempo deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["condicaoVazPres"]["tempo"].Size();
                for (int i = 0; i < nele; i++) {
                    if (!configuracao_inicial_json["condicaoVazPres"]["tempo"][i].IsNumber()) {
                        sucesso = false;
                        erros.push_back("Configuracao inicial:condicaoVazPressao: tempo " + std::to_string(i) + " deve ser do tipo number");
                    }
                }
            }
        }
    }

    if (configuracao_inicial_json.HasMember("correlacoesPorArranjo")) {
        if (!configuracao_inicial_json["correlacoesPorArranjo"].IsObject()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: correlacoesPorArranjo deve ser um objeto Json");
        }

        if (configuracao_inicial_json["correlacoesPorArranjo"].HasMember("estratificado"))
            if (!configuracao_inicial_json["correlacoesPorArranjo"]["estratificado"].IsInt()) {
                sucesso = false;
                erros.push_back("correlacoesPorArranjo:estratificado deve ser do tipo inteiro");
            }

        if (configuracao_inicial_json["correlacoesPorArranjo"].HasMember("bolhaGolfada"))
            if (!configuracao_inicial_json["correlacoesPorArranjo"]["bolhaGolfada"].IsInt()) {
                sucesso = false;
                erros.push_back("correlacoesPorArranjo:bolhaGolfada deve ser do tipo inteiro");
            }

        if (configuracao_inicial_json["correlacoesPorArranjo"].HasMember("anularChurn"))
            if (!configuracao_inicial_json["correlacoesPorArranjo"]["anularChurn"].IsInt()) {
                sucesso = false;
                erros.push_back("correlacoesPorArranjo:anularChurn deve ser do tipo inteiro");
            }
    }

    if (configuracao_inicial_json.HasMember("modoXY"))
        if (!configuracao_inicial_json["modoXY"].IsBool()) {
            sucesso = false;
            erros.push_back("correlacoesPorArranjo:modoXY deve ser do tipo Booleana");
        }

    if (configuracao_inicial_json.HasMember("xProdInicio"))
        if (!configuracao_inicial_json["xProdInicio"].IsNumber()) {
            sucesso = false;
            erros.push_back("correlacoesPorArranjo:xProdInicio deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("yProdInicio"))
        if (!configuracao_inicial_json["yProdInicio"].IsNumber()) {
            sucesso = false;
            erros.push_back("correlacoesPorArranjo:yProdInicio deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("xServInicio"))
        if (!configuracao_inicial_json["xServInicio"].IsNumber()) {
            sucesso = false;
            erros.push_back("correlacoesPorArranjo:xServInicio deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("yServInicio"))
        if (!configuracao_inicial_json["yServInicio"].IsNumber()) {
            sucesso = false;
            erros.push_back("correlacoesPorArranjo:yServInicio deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("tempReves"))
        if (!configuracao_inicial_json["tempReves"].IsNumber()) {
            sucesso = false;
            erros.push_back("correlacoesPorArranjo:tempReves deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("razCompGasReves"))
        if (!configuracao_inicial_json["razCompGasReves"].IsNumber()) {
            sucesso = false;
            erros.push_back("correlacoesPorArranjo:razCompGasReves deve ser do tipo number");
        }

    if (configuracao_inicial_json.HasMember("Formacao")) {
        if (!configuracao_inicial_json["Formacao"].IsObject()) {
            sucesso = false;
            erros.push_back("Configuracao inicial: Formacao deve ser um objeto Json");
        }

        if (configuracao_inicial_json["Formacao"].HasMember("TempoProducao"))
            if (!configuracao_inicial_json["Formacao"]["TempoProducao"].IsNumber()) {
                sucesso = false;
            }

        if (configuracao_inicial_json["Formacao"].HasMember("Propriedades")) {
            if (!configuracao_inicial_json["Formacao"]["Propriedades"].IsArray()) {
                sucesso = false;
                erros.push_back("Configuracao inicial:Propriedades: tempo deve ser do tipo array");
            } else {
                int nele = configuracao_inicial_json["Formacao"]["Propriedades"].Size();
                for (int i = 0; i < nele; i++) {
                    if (configuracao_inicial_json["Formacao"]["Propriedades"][i].HasMember("identificadores")) {
                        if (!configuracao_inicial_json["Formacao"]["Propriedades"][i]["identificadores"].IsInt()) {
                            sucesso = false;
                            erros.push_back("Formacao: identificadores " + std::to_string(i) + " deve ser do tipo number");
                        }
                    }
                    if (configuracao_inicial_json["Formacao"]["Propriedades"][i].HasMember("condutividade")) {
                        if (!configuracao_inicial_json["Formacao"]["Propriedades"][i]["condutividade"].IsNumber()) {
                            sucesso = false;
                            erros.push_back("Formacao: condutividade " + std::to_string(i) + " deve ser do tipo number");
                        }
                    }
                    if (configuracao_inicial_json["Formacao"]["Propriedades"][i].HasMember("calorEspecifico")) {
                        if (!configuracao_inicial_json["Formacao"]["Propriedades"][i]["calorEspecifico"].IsNumber()) {
                            sucesso = false;
                            erros.push_back("Formacao: calorEspecifico " + std::to_string(i) + " deve ser do tipo number");
                        }
                    }
                    if (configuracao_inicial_json["Formacao"]["Propriedades"][i].HasMember("massaEspecifica")) {
                        if (!configuracao_inicial_json["Formacao"]["Propriedades"][i]["massaEspecifica"].IsNumber()) {
                            sucesso = false;
                            erros.push_back("Formacao: massaEspecifica " + std::to_string(i) + " deve ser do tipo number");
                        }
                    }
                }
            }
        }
    }
}

void validadorTipo::valida_tempo(Value &tempo_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/tempo");

    // Validar campo obrigatório tempoFinal
    if (tempo_json.HasMember("tempoFinal")) {
        if (!tempo_json["tempoFinal"].IsNumber()) {
            sucesso = false;
            erros.push_back("tempo: tempoFinal deve ser do tipo number");
        }
    }

    // Validar array tempos (opcional)
    if (tempo_json.HasMember("tempos")) {
        if (!tempo_json["tempos"].IsArray()) {
            sucesso = false;
            erros.push_back("tempo: tempos deve ser do tipo array");
        } else {
            // Validar elementos do array tempos
            int nele = tempo_json["tempos"].Size();
            for (int i = 0; i < nele; i++) {
                if (!tempo_json["tempos"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("tempo: tempos indice " + std::to_string(i) + " deve ser do tipo number");
                }
            }
        }
    }

    // Validar array dtmax (opcional)
    if (tempo_json.HasMember("dtmax")) {
        if (!tempo_json["dtmax"].IsArray()) {
            sucesso = false;
            erros.push_back("tempo: dtmax deve ser do tipo array");
        } else {
            // Validar elementos do array dtmax
            int nele = tempo_json["dtmax"].Size();
            for (int i = 0; i < nele; i++) {
                if (!tempo_json["dtmax"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("tempo: dtmax indice " + std::to_string(i) + " deve ser do tipo number");
                }
            }
        }
    }

    // Validar array tempoSegrega (opcional)
    if (tempo_json.HasMember("tempoSegrega")) {
        if (!tempo_json["tempoSegrega"].IsArray()) {
            sucesso = false;
            erros.push_back("tempo: tempoSegrega deve ser do tipo array");
        } else {
            // Validar elementos do array tempoSegrega
            int nele = tempo_json["tempoSegrega"].Size();
            for (int i = 0; i < nele; i++) {
                if (!tempo_json["tempoSegrega"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("tempo: tempoSegrega indice " + std::to_string(i) + " deve ser do tipo number");
                }
            }
        }
    }

    // Validar array segrega (opcional)
    if (tempo_json.HasMember("segrega")) {
        if (!tempo_json["segrega"].IsArray()) {
            sucesso = false;
            erros.push_back("tempo: segrega deve ser do tipo array");
        } else {
            // Validar elementos do array segrega
            int nele = tempo_json["segrega"].Size();
            for (int i = 0; i < nele; i++) {
                if (!tempo_json["segrega"][i].IsInt()) {
                    sucesso = false;
                    erros.push_back("tempo: segrega indice " + std::to_string(i) + " deve ser do tipo inteiro");
                }
            }
        }
    }

    // Validar array gravaMomento (opcional)
    if (tempo_json.HasMember("gravaMomento")) {
        if (!tempo_json["gravaMomento"].IsArray()) {
            sucesso = false;
            erros.push_back("tempo: gravaMomento deve ser do tipo array");
        } else {
            // Validar elementos do array gravaMomento
            int nele = tempo_json["gravaMomento"].Size();
            for (int i = 0; i < nele; i++) {
                if (!tempo_json["gravaMomento"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("tempo: gravaMomento indice " + std::to_string(i) + " deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_tabela(Value &tabela_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/tabela");

    // Validar campo ativo (opcional)
    if (tabela_json.HasMember("ativo")) {
        if (!tabela_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("tabela: ativo deve ser do tipo booleano");
        }
    }

    // Validar campo nPontos (opcional)
    if (tabela_json.HasMember("nPontos")) {
        if (!tabela_json["nPontos"].IsInt()) {
            sucesso = false;
            erros.push_back("tabela: nPontos deve ser do tipo inteiro");
        }
    }

    // Validar campo pressaoMaxima (opcional)
    if (tabela_json.HasMember("pressaoMaxima")) {
        if (!tabela_json["pressaoMaxima"].IsNumber()) {
            sucesso = false;
            erros.push_back("tabela: pressaoMaxima deve ser do tipo number");
        }
    }

    // Validar campo pressaoMinima (opcional)
    if (tabela_json.HasMember("pressaoMinima")) {
        if (!tabela_json["pressaoMinima"].IsNumber()) {
            sucesso = false;
            erros.push_back("tabela: pressaoMinima deve ser do tipo number");
        }
    }

    // Validar campo temperaturaMaxima (opcional)
    if (tabela_json.HasMember("temperaturaMaxima")) {
        if (!tabela_json["temperaturaMaxima"].IsNumber()) {
            sucesso = false;
            erros.push_back("tabela: temperaturaMaxima deve ser do tipo number");
        }
    }

    // Validar campo temperaturaMinima (opcional)
    if (tabela_json.HasMember("temperaturaMinima")) {
        if (!tabela_json["temperaturaMinima"].IsNumber()) {
            sucesso = false;
            erros.push_back("tabela: temperaturaMinima deve ser do tipo number");
        }
    }
}

void validadorTipo::valida_materiais(Value &material_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/material");

    // Validar se material é um array
    if (!material_json.IsArray()) {
        sucesso = false;
        erros.push_back("material: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array material
    for (SizeType i = 0; i < material_json.Size(); i++) {
        string indiceMaterial = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!material_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceMaterial + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (material_json[i].HasMember("ativo")) {
            if (!material_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (material_json[i].HasMember("id")) {
            if (!material_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo tipo (opcional)
        if (material_json[i].HasMember("tipo")) {
            if (!material_json[i]["tipo"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/tipo: deve ser do tipo inteiro");
            }
        }

        // Validar campo condutividade (opcional)
        if (material_json[i].HasMember("condutividade")) {
            if (!material_json[i]["condutividade"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/condutividade: deve ser do tipo number");
            }
        }

        // Validar campo calorEspecifico (opcional)
        if (material_json[i].HasMember("calorEspecifico")) {
            if (!material_json[i]["calorEspecifico"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/calorEspecifico: deve ser do tipo number");
            }
        }

        // Validar campo rho (opcional)
        if (material_json[i].HasMember("rho")) {
            if (!material_json[i]["rho"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/rho: deve ser do tipo number");
            }
        }

        // Validar campo visc (opcional)
        if (material_json[i].HasMember("visc")) {
            if (!material_json[i]["visc"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/visc: deve ser do tipo number");
            }
        }

        // Validar campo beta (opcional)
        if (material_json[i].HasMember("beta")) {
            if (!material_json[i]["beta"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceMaterial + "/beta: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_corte(Value &corte_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/secaoTransversal");

    // Validar se secaoTransversal é um array
    if (!corte_json.IsArray()) {
        sucesso = false;
        erros.push_back("secaoTransversal: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array secaoTransversal
    for (SizeType i = 0; i < corte_json.Size(); i++) {
        string indiceCorte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!corte_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceCorte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (corte_json[i].HasMember("ativo")) {
            if (!corte_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceCorte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (corte_json[i].HasMember("id")) {
            if (!corte_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceCorte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo anular (opcional)
        if (corte_json[i].HasMember("anular")) {
            if (!corte_json[i]["anular"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceCorte + "/anular: deve ser do tipo booleano");
            }
        }

        // Validar campo diametroExterno (opcional)
        if (corte_json[i].HasMember("diametroExterno")) {
            if (!corte_json[i]["diametroExterno"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceCorte + "/diametroExterno: deve ser do tipo number");
            }
        }

        // Validar campo diametroInterno (opcional)
        if (corte_json[i].HasMember("diametroInterno")) {
            if (!corte_json[i]["diametroInterno"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceCorte + "/diametroInterno: deve ser do tipo number");
            }
        }

        // Validar campo rugosidade (opcional)
        if (corte_json[i].HasMember("rugosidade")) {
            if (!corte_json[i]["rugosidade"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceCorte + "/rugosidade: deve ser do tipo number");
            }
        }

        // Validar array camadas (opcional, mas crítico se presente)
        if (corte_json[i].HasMember("camadas")) {
            if (!corte_json[i]["camadas"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceCorte + "/camadas: deve ser do tipo array");
            } else {
                // Validar cada item do array camadas
                for (SizeType j = 0; j < corte_json[i]["camadas"].Size(); j++) {
                    string indiceCamada = indiceCorte + "/camadas[" + std::to_string(j) + "]";

                    // Validar se a camada é um objeto
                    if (!corte_json[i]["camadas"][j].IsObject()) {
                        sucesso = false;
                        erros.push_back(indiceCamada + ": deve ser do tipo objeto");
                        continue;
                    }

                    // Validar campo tipoMedicaoCamada (opcional)
                    if (corte_json[i]["camadas"][j].HasMember("tipoMedicaoCamada")) {
                        if (!corte_json[i]["camadas"][j]["tipoMedicaoCamada"].IsString()) {
                            sucesso = false;
                            erros.push_back(indiceCamada + "/tipoMedicaoCamada: deve ser do tipo string");
                        }
                    }

                    // Validar campo diametro (opcional)
                    if (corte_json[i]["camadas"][j].HasMember("diametro")) {
                        if (!corte_json[i]["camadas"][j]["diametro"].IsNumber()) {
                            sucesso = false;
                            erros.push_back(indiceCamada + "/diametro: deve ser do tipo number");
                        }
                    }

                    // Validar campo espessura (opcional)
                    if (corte_json[i]["camadas"][j].HasMember("espessura")) {
                        if (!corte_json[i]["camadas"][j]["espessura"].IsNumber()) {
                            sucesso = false;
                            erros.push_back(indiceCamada + "/espessura: deve ser do tipo number");
                        }
                    }

                    // Validar campo discretizacao (opcional)
                    if (corte_json[i]["camadas"][j].HasMember("discretizacao")) {
                        if (!corte_json[i]["camadas"][j]["discretizacao"].IsInt()) {
                            sucesso = false;
                            erros.push_back(indiceCamada + "/discretizacao: deve ser do tipo inteiro");
                        }
                    }

                    // Validar campo idMaterial (opcional)
                    if (corte_json[i]["camadas"][j].HasMember("idMaterial")) {
                        if (!corte_json[i]["camadas"][j]["idMaterial"].IsInt()) {
                            sucesso = false;
                            erros.push_back(indiceCamada + "/idMaterial: deve ser do tipo inteiro");
                        }
                    }
                }
            }
        }
    }
}

void validadorTipo::valida_fluidos_producao(Value &fluidos_producao_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fluidosProducao");

    // Validar se fluidosProducao é um array
    if (!fluidos_producao_json.IsArray()) {
        sucesso = false;
        erros.push_back("fluidosProducao: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fluidosProducao
    for (SizeType i = 0; i < fluidos_producao_json.Size(); i++) {
        string indiceFluido = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fluidos_producao_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFluido + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fluidos_producao_json[i].HasMember("ativo")) {
            if (!fluidos_producao_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fluidos_producao_json[i].HasMember("id")) {
            if (!fluidos_producao_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo api (opcional)
        if (fluidos_producao_json[i].HasMember("api")) {
            if (!fluidos_producao_json[i]["api"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/api: deve ser do tipo number");
            }
        }

        // Validar campo rgo (opcional)
        if (fluidos_producao_json[i].HasMember("rgo")) {
            if (!fluidos_producao_json[i]["rgo"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/rgo: deve ser do tipo number");
            }
        }

        // Validar campo densidadeGas (opcional)
        if (fluidos_producao_json[i].HasMember("densidadeGas")) {
            if (!fluidos_producao_json[i]["densidadeGas"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/densidadeGas: deve ser do tipo number");
            }
        }

        // Validar campo bsw (opcional)
        if (fluidos_producao_json[i].HasMember("bsw")) {
            if (!fluidos_producao_json[i]["bsw"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/bsw: deve ser do tipo number");
            }
        }

        // Validar campo densidadeAgua (opcional)
        if (fluidos_producao_json[i].HasMember("densidadeAgua")) {
            if (!fluidos_producao_json[i]["densidadeAgua"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/densidadeAgua: deve ser do tipo number");
            }
        }

        // Validar campo tipoEmul (opcional)
        if (fluidos_producao_json[i].HasMember("tipoEmul")) {
            if (!fluidos_producao_json[i]["tipoEmul"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/tipoEmul: deve ser do tipo inteiro");
            }
        }

        // Validar campo coefAModeloExp (opcional)
        if (fluidos_producao_json[i].HasMember("coefAModeloExp")) {
            if (!fluidos_producao_json[i]["coefAModeloExp"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/coefAModeloExp: deve ser do tipo number");
            }
        }

        // Validar campo coefBModeloExp (opcional)
        if (fluidos_producao_json[i].HasMember("coefBModeloExp")) {
            if (!fluidos_producao_json[i]["coefBModeloExp"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/coefBModeloExp: deve ser do tipo number");
            }
        }

        // Validar campo PHI100 (opcional)
        if (fluidos_producao_json[i].HasMember("PHI100")) {
            if (!fluidos_producao_json[i]["PHI100"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/PHI100: deve ser do tipo number");
            }
        }

        // Validar campo bswCorte (opcional)
        if (fluidos_producao_json[i].HasMember("bswCorte")) {
            if (!fluidos_producao_json[i]["bswCorte"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/bswCorte: deve ser do tipo number");
            }
        }

        // Validar array BSWVec (opcional)
        if (fluidos_producao_json[i].HasMember("BSWVec")) {
            if (!fluidos_producao_json[i]["BSWVec"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/BSWVec: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fluidos_producao_json[i]["BSWVec"].Size(); j++) {
                    if (!fluidos_producao_json[i]["BSWVec"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFluido + "/BSWVec[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array emulVec (opcional)
        if (fluidos_producao_json[i].HasMember("emulVec")) {
            if (!fluidos_producao_json[i]["emulVec"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/emulVec: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fluidos_producao_json[i]["emulVec"].Size(); j++) {
                    if (!fluidos_producao_json[i]["emulVec"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFluido + "/emulVec[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo fracCO2 (opcional)
        if (fluidos_producao_json[i].HasMember("fracCO2")) {
            if (!fluidos_producao_json[i]["fracCO2"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/fracCO2: deve ser do tipo number");
            }
        }

        // Validar campo correlacaoCritica (opcional)
        if (fluidos_producao_json[i].HasMember("correlacaoCritica")) {
            if (!fluidos_producao_json[i]["correlacaoCritica"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/correlacaoCritica: deve ser do tipo inteiro");
            }
        }

        // Validar campo modeloRsPb (opcional)
        if (fluidos_producao_json[i].HasMember("modeloRsPb")) {
            if (!fluidos_producao_json[i]["modeloRsPb"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/modeloRsPb: deve ser do tipo inteiro");
            }
        }

        // Validar campo modeloOleoMorto (opcional)
        if (fluidos_producao_json[i].HasMember("modeloOleoMorto")) {
            if (!fluidos_producao_json[i]["modeloOleoMorto"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/modeloOleoMorto: deve ser do tipo inteiro");
            }
        }

        // Validar campo temp1 (opcional)
        if (fluidos_producao_json[i].HasMember("temp1")) {
            if (!fluidos_producao_json[i]["temp1"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/temp1: deve ser do tipo number");
            }
        }

        // Validar campo visc1 (opcional)
        if (fluidos_producao_json[i].HasMember("visc1")) {
            if (!fluidos_producao_json[i]["visc1"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/visc1: deve ser do tipo number");
            }
        }

        // Validar campo temp2 (opcional)
        if (fluidos_producao_json[i].HasMember("temp2")) {
            if (!fluidos_producao_json[i]["temp2"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/temp2: deve ser do tipo number");
            }
        }

        // Validar campo visc2 (opcional)
        if (fluidos_producao_json[i].HasMember("visc2")) {
            if (!fluidos_producao_json[i]["visc2"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/visc2: deve ser do tipo number");
            }
        }

        // Validar array tempOleoMorto (opcional)
        if (fluidos_producao_json[i].HasMember("tempOleoMorto")) {
            if (!fluidos_producao_json[i]["tempOleoMorto"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/tempOleoMorto: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fluidos_producao_json[i]["tempOleoMorto"].Size(); j++) {
                    if (!fluidos_producao_json[i]["tempOleoMorto"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFluido + "/tempOleoMorto[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array viscOleoMorto (opcional)
        if (fluidos_producao_json[i].HasMember("viscOleoMorto")) {
            if (!fluidos_producao_json[i]["viscOleoMorto"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/viscOleoMorto: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fluidos_producao_json[i]["viscOleoMorto"].Size(); j++) {
                    if (!fluidos_producao_json[i]["viscOleoMorto"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFluido + "/viscOleoMorto[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo modeloOleoVivo (opcional)
        if (fluidos_producao_json[i].HasMember("modeloOleoVivo")) {
            if (!fluidos_producao_json[i]["modeloOleoVivo"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/modeloOleoVivo: deve ser do tipo inteiro");
            }
        }

        // Validar campo modeloOleoSubSaturado (opcional)
        if (fluidos_producao_json[i].HasMember("modeloOleoSubSaturado")) {
            if (!fluidos_producao_json[i]["modeloOleoSubSaturado"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/modeloOleoSubSaturado: deve ser do tipo inteiro");
            }
        }

        // Validar campo modeloViscBlackOil (opcional)
        if (fluidos_producao_json[i].HasMember("modeloViscBlackOil")) {
            if (!fluidos_producao_json[i]["modeloViscBlackOil"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/modeloViscBlackOil: deve ser do tipo inteiro");
            }
        }

        // Validar campo modeloAguaBlackOil (opcional)
        if (fluidos_producao_json[i].HasMember("modeloAguaBlackOil")) {
            if (!fluidos_producao_json[i]["modeloAguaBlackOil"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/modeloAguaBlackOil: deve ser do tipo inteiro");
            }
        }

        // Validar campo fracMolarUsuario (opcional)
        if (fluidos_producao_json[i].HasMember("fracMolarUsuario")) {
            if (!fluidos_producao_json[i]["fracMolarUsuario"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/fracMolarUsuario: deve ser do tipo booleano");
            }
        }

        // Validar array fracMolar (opcional)
        if (fluidos_producao_json[i].HasMember("fracMolar")) {
            if (!fluidos_producao_json[i]["fracMolar"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/fracMolar: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fluidos_producao_json[i]["fracMolar"].Size(); j++) {
                    if (!fluidos_producao_json[i]["fracMolar"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFluido + "/fracMolar[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo RGOCompUsuario (opcional)
        if (fluidos_producao_json[i].HasMember("RGOCompUsuario")) {
            if (!fluidos_producao_json[i]["RGOCompUsuario"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFluido + "/RGOCompUsuario: deve ser do tipo booleano");
            }
        }
    }
}

void validadorTipo::valida_fluido_complementar(Value &fluido_complementar_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fluidoComplementar");

    // Validar se fluidoComplementar é um objeto
    if (!fluido_complementar_json.IsObject()) {
        sucesso = false;
        erros.push_back("fluidoComplementar: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (fluido_complementar_json.HasMember("ativo")) {
        if (!fluido_complementar_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/ativo: deve ser do tipo booleano");
        }
    }

    // Validar campo massaEspecifica (opcional)
    if (fluido_complementar_json.HasMember("massaEspecifica")) {
        if (!fluido_complementar_json["massaEspecifica"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/massaEspecifica: deve ser do tipo number");
        }
    }

    // Validar campo compP (opcional)
    if (fluido_complementar_json.HasMember("compP")) {
        if (!fluido_complementar_json["compP"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/compP: deve ser do tipo number");
        }
    }

    // Validar campo compT (opcional)
    if (fluido_complementar_json.HasMember("compT")) {
        if (!fluido_complementar_json["compT"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/compT: deve ser do tipo number");
        }
    }

    // Validar campo tensup (opcional)
    if (fluido_complementar_json.HasMember("tensup")) {
        if (!fluido_complementar_json["tensup"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/tensup: deve ser do tipo number");
        }
    }

    // Validar campo calorEspecifico (opcional)
    if (fluido_complementar_json.HasMember("calorEspecifico")) {
        if (!fluido_complementar_json["calorEspecifico"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/calorEspecifico: deve ser do tipo number");
        }
    }

    // Validar campo condutividade (opcional)
    if (fluido_complementar_json.HasMember("condutividade")) {
        if (!fluido_complementar_json["condutividade"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/condutividade: deve ser do tipo number");
        }
    }

    // Validar campo temp1 (opcional)
    if (fluido_complementar_json.HasMember("temp1")) {
        if (!fluido_complementar_json["temp1"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/temp1: deve ser do tipo number");
        }
    }

    // Validar campo visc1 (opcional)
    if (fluido_complementar_json.HasMember("visc1")) {
        if (!fluido_complementar_json["visc1"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/visc1: deve ser do tipo number");
        }
    }

    // Validar campo temp2 (opcional)
    if (fluido_complementar_json.HasMember("temp2")) {
        if (!fluido_complementar_json["temp2"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/temp2: deve ser do tipo number");
        }
    }

    // Validar campo visc2 (opcional)
    if (fluido_complementar_json.HasMember("visc2")) {
        if (!fluido_complementar_json["visc2"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/visc2: deve ser do tipo number");
        }
    }

    // Validar campo salinidade (opcional)
    if (fluido_complementar_json.HasMember("salinidade")) {
        if (!fluido_complementar_json["salinidade"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/salinidade: deve ser do tipo number");
        }
    }

    // Validar campo tipoFluido (opcional)
    if (fluido_complementar_json.HasMember("tipoFluido")) {
        if (!fluido_complementar_json["tipoFluido"].IsInt()) {
            sucesso = false;
            erros.push_back("fluidoComplementar/tipoFluido: deve ser do tipo inteiro");
        }
    }
}

void validadorTipo::valida_fluido_gas(Value &fluido_gas_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fluidoGas");

    // Validar se fluidoGas é um objeto
    if (!fluido_gas_json.IsObject()) {
        sucesso = false;
        erros.push_back("fluidoGas: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (fluido_gas_json.HasMember("ativo")) {
        if (!fluido_gas_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("fluidoGas/ativo: deve ser do tipo booleano");
        }
    }

    // Validar campo densidadeGas (opcional)
    if (fluido_gas_json.HasMember("densidadeGas")) {
        if (!fluido_gas_json["densidadeGas"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoGas/densidadeGas: deve ser do tipo number");
        }
    }

    // Validar campo fracCO2 (opcional)
    if (fluido_gas_json.HasMember("fracCO2")) {
        if (!fluido_gas_json["fracCO2"].IsNumber()) {
            sucesso = false;
            erros.push_back("fluidoGas/fracCO2: deve ser do tipo number");
        }
    }

    // Validar campo correlacaoCritica (opcional)
    if (fluido_gas_json.HasMember("correlacaoCritica")) {
        if (!fluido_gas_json["correlacaoCritica"].IsInt()) {
            sucesso = false;
            erros.push_back("fluidoGas/correlacaoCritica: deve ser do tipo inteiro");
        }
    }

    // Validar campo fracMolarUsuario (opcional)
    if (fluido_gas_json.HasMember("fracMolarUsuario")) {
        if (!fluido_gas_json["fracMolarUsuario"].IsBool()) {
            sucesso = false;
            erros.push_back("fluidoGas/fracMolarUsuario: deve ser do tipo booleano");
        }
    }

    // Validar array fracMolar (opcional)
    if (fluido_gas_json.HasMember("fracMolar")) {
        if (!fluido_gas_json["fracMolar"].IsArray()) {
            sucesso = false;
            erros.push_back("fluidoGas/fracMolar: deve ser do tipo array");
        } else {
            // Validar elementos do array fracMolar
            for (SizeType i = 0; i < fluido_gas_json["fracMolar"].Size(); i++) {
                if (!fluido_gas_json["fracMolar"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("fluidoGas/fracMolar[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_unidades_producao(Value &dutos_producao_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/dutosProducao");

    // Validar se dutosProducao é um array
    if (!dutos_producao_json.IsArray()) {
        sucesso = false;
        erros.push_back("dutosProducao: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array dutosProducao
    for (SizeType i = 0; i < dutos_producao_json.Size(); i++) {
        string indiceDuto = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!dutos_producao_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceDuto + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (dutos_producao_json[i].HasMember("ativo")) {
            if (!dutos_producao_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (dutos_producao_json[i].HasMember("id")) {
            if (!dutos_producao_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo correlacaoMR2 (opcional)
        if (dutos_producao_json[i].HasMember("correlacaoMR2")) {
            if (!dutos_producao_json[i]["correlacaoMR2"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/correlacaoMR2: deve ser do tipo inteiro");
            }
        }

        // Validar campo angulo (opcional)
        if (dutos_producao_json[i].HasMember("angulo")) {
            if (!dutos_producao_json[i]["angulo"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/angulo: deve ser do tipo number");
            }
        }

        // Validar campo idCorte (opcional)
        if (dutos_producao_json[i].HasMember("idCorte")) {
            if (!dutos_producao_json[i]["idCorte"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/idCorte: deve ser do tipo inteiro");
            }
        }

        // Validar campo idFormacao (opcional)
        if (dutos_producao_json[i].HasMember("idFormacao")) {
            if (!dutos_producao_json[i]["idFormacao"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/idFormacao: deve ser do tipo inteiro");
            }
        }

        // Validar campo ambienteExterno (opcional)
        if (dutos_producao_json[i].HasMember("ambienteExterno")) {
            if (!dutos_producao_json[i]["ambienteExterno"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/ambienteExterno: deve ser do tipo inteiro");
            }
        }

        // Validar campo direcaoConveccao (opcional)
        if (dutos_producao_json[i].HasMember("direcaoConveccao")) {
            if (!dutos_producao_json[i]["direcaoConveccao"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/direcaoConveccao: deve ser do tipo inteiro");
            }
        }

        // Validar campo acoplamentoTermico (opcional)
        if (dutos_producao_json[i].HasMember("acoplamentoTermico")) {
            if (!dutos_producao_json[i]["acoplamentoTermico"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/acoplamentoTermico: deve ser do tipo inteiro");
            }
        }

        // Validar campo acoplamentoTermicoRedeParalela (opcional)
        if (dutos_producao_json[i].HasMember("acoplamentoTermicoRedeParalela")) {
            if (!dutos_producao_json[i]["acoplamentoTermicoRedeParalela"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/acoplamentoTermicoRedeParalela: deve ser do tipo inteiro");
            }
        }

        // Validar campo agrupamento (opcional)
        if (dutos_producao_json[i].HasMember("agrupamento")) {
            if (!dutos_producao_json[i]["agrupamento"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/agrupamento: deve ser do tipo booleano");
            }
        }

        // Validar array dxCelula (opcional)
        if (dutos_producao_json[i].HasMember("dxCelula")) {
            if (!dutos_producao_json[i]["dxCelula"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dxCelula: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < dutos_producao_json[i]["dxCelula"].Size(); j++) {
                    if (!dutos_producao_json[i]["dxCelula"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceDuto + "/dxCelula[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array discretizacao (opcional) - array de objetos
        if (dutos_producao_json[i].HasMember("discretizacao")) {
            if (!dutos_producao_json[i]["discretizacao"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/discretizacao: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < dutos_producao_json[i]["discretizacao"].Size(); j++) {
                    string indiceDiscret = indiceDuto + "/discretizacao[" + std::to_string(j) + "]";

                    if (!dutos_producao_json[i]["discretizacao"][j].IsObject()) {
                        sucesso = false;
                        erros.push_back(indiceDiscret + ": deve ser do tipo objeto");
                        continue;
                    }

                    // Validar nCelulas
                    if (dutos_producao_json[i]["discretizacao"][j].HasMember("nCelulas")) {
                        if (!dutos_producao_json[i]["discretizacao"][j]["nCelulas"].IsInt()) {
                            sucesso = false;
                            erros.push_back(indiceDiscret + "/nCelulas: deve ser do tipo inteiro");
                        }
                    }

                    // Validar comprimento
                    if (dutos_producao_json[i]["discretizacao"][j].HasMember("comprimento")) {
                        if (!dutos_producao_json[i]["discretizacao"][j]["comprimento"].IsNumber()) {
                            sucesso = false;
                            erros.push_back(indiceDiscret + "/comprimento: deve ser do tipo number");
                        }
                    }
                }
            }
        }

        // Validar objeto condicoesIniciaisEAmbiente (opcional)
        if (dutos_producao_json[i].HasMember("condicoesIniciaisEAmbiente")) {
            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"].IsObject()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/condicoesIniciaisEAmbiente: deve ser do tipo objeto");
            } else {
                string indiceCondAmb = indiceDuto + "/condicoesIniciaisEAmbiente";

                // Validar array compInter
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("compInter")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["compInter"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/compInter: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["compInter"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["compInter"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/compInter[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array temp
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("temp")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["temp"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/temp: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["temp"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["temp"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/temp[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array pressao
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("pressao")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["pressao"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/pressao: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["pressao"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["pressao"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/pressao[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array holdup
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("holdup")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["holdup"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/holdup: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["holdup"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["holdup"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/holdup[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array bet
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("bet")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["bet"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/bet: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["bet"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["bet"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/bet[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array uls
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("uls")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["uls"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/uls: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["uls"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["uls"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/uls[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array ugs
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("ugs")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["ugs"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/ugs: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["ugs"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["ugs"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/ugs[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array tempExterna
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("tempExterna")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["tempExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/tempExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["tempExterna"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["tempExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/tempExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array velExterna
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("velExterna")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["velExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/velExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["velExterna"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["velExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/velExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array kExterna
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("kExterna")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["kExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/kExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["kExterna"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["kExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/kExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array calorEspecificoExterno
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("calorEspecificoExterno")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["calorEspecificoExterno"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/calorEspecificoExterno: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["calorEspecificoExterno"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["calorEspecificoExterno"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/calorEspecificoExterno[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array rhoExterno
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("rhoExterno")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["rhoExterno"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/rhoExterno: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["rhoExterno"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["rhoExterno"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/rhoExterno[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array viscExterna
                if (dutos_producao_json[i]["condicoesIniciaisEAmbiente"].HasMember("viscExterna")) {
                    if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["viscExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/viscExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["viscExterna"].Size(); j++) {
                            if (!dutos_producao_json[i]["condicoesIniciaisEAmbiente"]["viscExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/viscExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }
            }
        }

        // Validar campos dPdLHidro, dPdLFric, dTdL (opcional)
        if (dutos_producao_json[i].HasMember("dPdLHidro")) {
            if (!dutos_producao_json[i]["dPdLHidro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dPdLHidro: deve ser do tipo inteiro");
            }
        }

        if (dutos_producao_json[i].HasMember("dPdLFric")) {
            if (!dutos_producao_json[i]["dPdLFric"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dPdLFric: deve ser do tipo inteiro");
            }
        }

        if (dutos_producao_json[i].HasMember("dTdL")) {
            if (!dutos_producao_json[i]["dTdL"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dTdL: deve ser do tipo inteiro");
            }
        }

        // Validar campos xCoor, yCoor, nCelulas_XY (opcional)
        if (dutos_producao_json[i].HasMember("xCoor")) {
            if (!dutos_producao_json[i]["xCoor"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/xCoor: deve ser do tipo number");
            }
        }

        if (dutos_producao_json[i].HasMember("yCoor")) {
            if (!dutos_producao_json[i]["yCoor"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/yCoor: deve ser do tipo number");
            }
        }

        if (dutos_producao_json[i].HasMember("nCelulas_XY")) {
            if (!dutos_producao_json[i]["nCelulas_XY"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/nCelulas_XY: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_unidades_servico(Value &dutos_servico_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/dutosServico");

    // Validar se dutosServico é um array
    if (!dutos_servico_json.IsArray()) {
        sucesso = false;
        erros.push_back("dutosServico: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array dutosServico
    for (SizeType i = 0; i < dutos_servico_json.Size(); i++) {
        string indiceDuto = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!dutos_servico_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceDuto + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (dutos_servico_json[i].HasMember("ativo")) {
            if (!dutos_servico_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (dutos_servico_json[i].HasMember("id")) {
            if (!dutos_servico_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo angulo (opcional)
        if (dutos_servico_json[i].HasMember("angulo")) {
            if (!dutos_servico_json[i]["angulo"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/angulo: deve ser do tipo number");
            }
        }

        // Validar campo idCorte (opcional)
        if (dutos_servico_json[i].HasMember("idCorte")) {
            if (!dutos_servico_json[i]["idCorte"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/idCorte: deve ser do tipo inteiro");
            }
        }

        // Validar campo idFormacao (opcional)
        if (dutos_servico_json[i].HasMember("idFormacao")) {
            if (!dutos_servico_json[i]["idFormacao"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/idFormacao: deve ser do tipo inteiro");
            }
        }

        // Validar campo ambienteExterno (opcional)
        if (dutos_servico_json[i].HasMember("ambienteExterno")) {
            if (!dutos_servico_json[i]["ambienteExterno"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/ambienteExterno: deve ser do tipo inteiro");
            }
        }

        // Validar campo direcaoConveccao (opcional)
        if (dutos_servico_json[i].HasMember("direcaoConveccao")) {
            if (!dutos_servico_json[i]["direcaoConveccao"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/direcaoConveccao: deve ser do tipo inteiro");
            }
        }

        // Validar campo acoplamentoTermico (opcional)
        if (dutos_servico_json[i].HasMember("acoplamentoTermico")) {
            if (!dutos_servico_json[i]["acoplamentoTermico"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/acoplamentoTermico: deve ser do tipo inteiro");
            }
        }

        // Validar campo agrupamento (opcional)
        if (dutos_servico_json[i].HasMember("agrupamento")) {
            if (!dutos_servico_json[i]["agrupamento"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/agrupamento: deve ser do tipo booleano");
            }
        }

        // Validar array dxCelula (opcional)
        if (dutos_servico_json[i].HasMember("dxCelula")) {
            if (!dutos_servico_json[i]["dxCelula"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dxCelula: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < dutos_servico_json[i]["dxCelula"].Size(); j++) {
                    if (!dutos_servico_json[i]["dxCelula"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceDuto + "/dxCelula[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array discretizacao (opcional) - array de objetos
        if (dutos_servico_json[i].HasMember("discretizacao")) {
            if (!dutos_servico_json[i]["discretizacao"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/discretizacao: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < dutos_servico_json[i]["discretizacao"].Size(); j++) {
                    string indiceDiscret = indiceDuto + "/discretizacao[" + std::to_string(j) + "]";

                    if (!dutos_servico_json[i]["discretizacao"][j].IsObject()) {
                        sucesso = false;
                        erros.push_back(indiceDiscret + ": deve ser do tipo objeto");
                        continue;
                    }

                    // Validar nCelulas
                    if (dutos_servico_json[i]["discretizacao"][j].HasMember("nCelulas")) {
                        if (!dutos_servico_json[i]["discretizacao"][j]["nCelulas"].IsInt()) {
                            sucesso = false;
                            erros.push_back(indiceDiscret + "/nCelulas: deve ser do tipo inteiro");
                        }
                    }

                    // Validar comprimento
                    if (dutos_servico_json[i]["discretizacao"][j].HasMember("comprimento")) {
                        if (!dutos_servico_json[i]["discretizacao"][j]["comprimento"].IsNumber()) {
                            sucesso = false;
                            erros.push_back(indiceDiscret + "/comprimento: deve ser do tipo number");
                        }
                    }
                }
            }
        }

        // Validar objeto condicoesIniciaisEAmbiente (opcional)
        if (dutos_servico_json[i].HasMember("condicoesIniciaisEAmbiente")) {
            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"].IsObject()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/condicoesIniciaisEAmbiente: deve ser do tipo objeto");
            } else {
                string indiceCondAmb = indiceDuto + "/condicoesIniciaisEAmbiente";

                // Validar array compInter
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("compInter")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["compInter"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/compInter: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["compInter"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["compInter"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/compInter[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array temp
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("temp")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["temp"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/temp: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["temp"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["temp"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/temp[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array pressao
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("pressao")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["pressao"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/pressao: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["pressao"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["pressao"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/pressao[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array vazaoMassicaGas
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("vazaoMassicaGas")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["vazaoMassicaGas"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/vazaoMassicaGas: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["vazaoMassicaGas"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["vazaoMassicaGas"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/vazaoMassicaGas[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array tempExterna
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("tempExterna")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["tempExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/tempExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["tempExterna"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["tempExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/tempExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array velExterna
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("velExterna")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["velExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/velExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["velExterna"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["velExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/velExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array kExterna
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("kExterna")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["kExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/kExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["kExterna"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["kExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/kExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array calorEspecificoExterno
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("calorEspecificoExterno")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["calorEspecificoExterno"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/calorEspecificoExterno: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["calorEspecificoExterno"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["calorEspecificoExterno"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/calorEspecificoExterno[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array rhoExterno
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("rhoExterno")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["rhoExterno"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/rhoExterno: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["rhoExterno"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["rhoExterno"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/rhoExterno[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }

                // Validar array viscExterna
                if (dutos_servico_json[i]["condicoesIniciaisEAmbiente"].HasMember("viscExterna")) {
                    if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["viscExterna"].IsArray()) {
                        sucesso = false;
                        erros.push_back(indiceCondAmb + "/viscExterna: deve ser do tipo array");
                    } else {
                        for (SizeType j = 0; j < dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["viscExterna"].Size(); j++) {
                            if (!dutos_servico_json[i]["condicoesIniciaisEAmbiente"]["viscExterna"][j].IsNumber()) {
                                sucesso = false;
                                erros.push_back(indiceCondAmb + "/viscExterna[" + std::to_string(j) + "]: deve ser do tipo number");
                            }
                        }
                    }
                }
            }
        }

        // Validar campos dPdLHidro, dPdLFric, dTdL (opcional)
        if (dutos_servico_json[i].HasMember("dPdLHidro")) {
            if (!dutos_servico_json[i]["dPdLHidro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dPdLHidro: deve ser do tipo inteiro");
            }
        }

        if (dutos_servico_json[i].HasMember("dPdLFric")) {
            if (!dutos_servico_json[i]["dPdLFric"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dPdLFric: deve ser do tipo inteiro");
            }
        }

        if (dutos_servico_json[i].HasMember("dTdL")) {
            if (!dutos_servico_json[i]["dTdL"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/dTdL: deve ser do tipo inteiro");
            }
        }

        // Validar campos xCoor, yCoor, nCelulas_XY (opcional)
        if (dutos_servico_json[i].HasMember("xCoor")) {
            if (!dutos_servico_json[i]["xCoor"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/xCoor: deve ser do tipo number");
            }
        }

        if (dutos_servico_json[i].HasMember("yCoor")) {
            if (!dutos_servico_json[i]["yCoor"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/yCoor: deve ser do tipo number");
            }
        }

        if (dutos_servico_json[i].HasMember("nCelulas_XY")) {
            if (!dutos_servico_json[i]["nCelulas_XY"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDuto + "/nCelulas_XY: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_condcont_pocinjec(Value &condcont_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/CondicaoContPocInjec");

    // Validar se CondicaoContPocInjec é um objeto
    if (!condcont_json.IsObject()) {
        sucesso = false;
        erros.push_back("CondicaoContPocInjec: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (condcont_json.HasMember("ativo")) {
        if (!condcont_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/ativo: deve ser do tipo booleano");
        }
    }

    // Validar campo usuario (opcional)
    if (condcont_json.HasMember("usuario")) {
        if (!condcont_json["usuario"].IsString()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/usuario: deve ser do tipo string");
        }
    }

    // Validar campo tipoFluido (opcional)
    if (condcont_json.HasMember("tipoFluido")) {
        if (!condcont_json["tipoFluido"].IsInt()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/tipoFluido: deve ser do tipo inteiro");
        }
    }

    // Validar campo salinidade (opcional)
    if (condcont_json.HasMember("salinidade")) {
        if (!condcont_json["salinidade"].IsNumber()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/salinidade: deve ser do tipo number");
        }
    }

    // Validar campo arquivoPvtsim (opcional)
    if (condcont_json.HasMember("arquivoPvtsim")) {
        if (!condcont_json["arquivoPvtsim"].IsString()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/arquivoPvtsim: deve ser do tipo string");
        }
    }

    // Validar campo condContorno (opcional)
    if (condcont_json.HasMember("condContorno")) {
        if (!condcont_json["condContorno"].IsInt()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/condContorno: deve ser do tipo inteiro");
        }
    }

    // Validar campo tempInj (opcional)
    if (condcont_json.HasMember("tempInj")) {
        if (!condcont_json["tempInj"].IsNumber()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/tempInj: deve ser do tipo number");
        }
    }

    // Validar campo vazLiq (opcional)
    if (condcont_json.HasMember("vazLiq")) {
        if (!condcont_json["vazLiq"].IsNumber()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/vazLiq: deve ser do tipo number");
        }
    }

    // Validar campo presInjec (opcional)
    if (condcont_json.HasMember("presInjec")) {
        if (!condcont_json["presInjec"].IsNumber()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/presInjec: deve ser do tipo number");
        }
    }

    // Validar campo presFundo (opcional)
    if (condcont_json.HasMember("presFundo")) {
        if (!condcont_json["presFundo"].IsNumber()) {
            sucesso = false;
            erros.push_back("CondicaoContPocInjec/presFundo: deve ser do tipo number");
        }
    }
}

void validadorTipo::valida_parafina(Value &parafina_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/parafina");

    // Validar se parafina é um objeto
    if (!parafina_json.IsObject()) {
        sucesso = false;
        erros.push_back("parafina: deve ser do tipo objeto");
        return;
    }

    // Validar campo arquivoWax (opcional)
    if (parafina_json.HasMember("arquivoWax")) {
        if (!parafina_json["arquivoWax"].IsString()) {
            sucesso = false;
            erros.push_back("parafina/arquivoWax: deve ser do tipo string");
        }
    }

    // Validar campo usuarioPorosidade (opcional)
    if (parafina_json.HasMember("usuarioPorosidade")) {
        if (!parafina_json["usuarioPorosidade"].IsBool()) {
            sucesso = false;
            erros.push_back("parafina/usuarioPorosidade: deve ser do tipo booleano");
        }
    }

    // Validar campo porosidade (opcional)
    if (parafina_json.HasMember("porosidade")) {
        if (!parafina_json["porosidade"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/porosidade: deve ser do tipo number");
        }
    }

    // Validar campo usuarioC2C3 (opcional)
    if (parafina_json.HasMember("usuarioC2C3")) {
        if (!parafina_json["usuarioC2C3"].IsBool()) {
            sucesso = false;
            erros.push_back("parafina/usuarioC2C3: deve ser do tipo booleano");
        }
    }

    // Validar campo c2 (opcional)
    if (parafina_json.HasMember("c2")) {
        if (!parafina_json["c2"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/c2: deve ser do tipo number");
        }
    }

    // Validar campo c3 (opcional)
    if (parafina_json.HasMember("c3")) {
        if (!parafina_json["c3"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/c3: deve ser do tipo number");
        }
    }

    // Validar campo usuarioDifus (opcional)
    if (parafina_json.HasMember("usuarioDifus")) {
        if (!parafina_json["usuarioDifus"].IsBool()) {
            sucesso = false;
            erros.push_back("parafina/usuarioDifus: deve ser do tipo booleano");
        }
    }

    // Validar campo alteraViscFlu (opcional)
    if (parafina_json.HasMember("alteraViscFlu")) {
        if (!parafina_json["alteraViscFlu"].IsBool()) {
            sucesso = false;
            erros.push_back("parafina/alteraViscFlu: deve ser do tipo booleano");
        }
    }

    // Validar campo difus (opcional)
    if (parafina_json.HasMember("difus")) {
        if (!parafina_json["difus"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/difus: deve ser do tipo number");
        }
    }

    // Validar campo rugosidade (opcional)
    if (parafina_json.HasMember("rugosidade")) {
        if (!parafina_json["rugosidade"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/rugosidade: deve ser do tipo number");
        }
    }

    // Validar campo multiplicadorViscosidade (opcional)
    if (parafina_json.HasMember("multiplicadorViscosidade")) {
        if (!parafina_json["multiplicadorViscosidade"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/multiplicadorViscosidade: deve ser do tipo number");
        }
    }

    // Validar campo DmultipWax (opcional)
    if (parafina_json.HasMember("DmultipWax")) {
        if (!parafina_json["DmultipWax"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/DmultipWax: deve ser do tipo number");
        }
    }

    // Validar campo EmultipWax (opcional)
    if (parafina_json.HasMember("EmultipWax")) {
        if (!parafina_json["EmultipWax"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/EmultipWax: deve ser do tipo number");
        }
    }

    // Validar campo FmultipWax (opcional)
    if (parafina_json.HasMember("FmultipWax")) {
        if (!parafina_json["FmultipWax"].IsNumber()) {
            sucesso = false;
            erros.push_back("parafina/FmultipWax: deve ser do tipo number");
        }
    }
}

void validadorTipo::valida_hidrato(Value &hidrato_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/hidrato");

    // Validar se hidrato é um objeto
    if (!hidrato_json.IsObject()) {
        sucesso = false;
        erros.push_back("hidrato: deve ser do tipo objeto");
        return;
    }

    // Validar campo calculoInterno (opcional)
    if (hidrato_json.HasMember("calculoInterno")) {
        if (!hidrato_json["calculoInterno"].IsBool()) {
            sucesso = false;
            erros.push_back("hidrato/calculoInterno: deve ser do tipo booleano");
        }
    }

    // Validar array pressao (opcional)
    if (hidrato_json.HasMember("pressao")) {
        if (!hidrato_json["pressao"].IsArray()) {
            sucesso = false;
            erros.push_back("hidrato/pressao: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < hidrato_json["pressao"].Size(); i++) {
                if (!hidrato_json["pressao"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("hidrato/pressao[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array temperatura (opcional)
    if (hidrato_json.HasMember("temperatura")) {
        if (!hidrato_json["temperatura"].IsArray()) {
            sucesso = false;
            erros.push_back("hidrato/temperatura: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < hidrato_json["temperatura"].Size(); i++) {
                if (!hidrato_json["temperatura"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("hidrato/temperatura[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar objeto Hammerschmidt (opcional)
    if (hidrato_json.HasMember("Hammerschmidt")) {
        if (!hidrato_json["Hammerschmidt"].IsObject()) {
            sucesso = false;
            erros.push_back("hidrato/Hammerschmidt: deve ser do tipo objeto");
        } else {
            string chaveHammer = "hidrato/Hammerschmidt";

            // Validar campo MMH (opcional)
            if (hidrato_json["Hammerschmidt"].HasMember("MMH")) {
                if (!hidrato_json["Hammerschmidt"]["MMH"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveHammer + "/MMH: deve ser do tipo number");
                }
            }

            // Validar campo MMG (opcional)
            if (hidrato_json["Hammerschmidt"].HasMember("MMG")) {
                if (!hidrato_json["Hammerschmidt"]["MMG"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveHammer + "/MMG: deve ser do tipo number");
                }
            }

            // Validar campo MMW (opcional)
            if (hidrato_json["Hammerschmidt"].HasMember("MMW")) {
                if (!hidrato_json["Hammerschmidt"]["MMW"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveHammer + "/MMW: deve ser do tipo number");
                }
            }

            // Validar campo Whamm (opcional)
            if (hidrato_json["Hammerschmidt"].HasMember("Whamm")) {
                if (!hidrato_json["Hammerschmidt"]["Whamm"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveHammer + "/Whamm: deve ser do tipo number");
                }
            }

            // Validar campo inibidor (opcional)
            if (hidrato_json["Hammerschmidt"].HasMember("inibidor")) {
                if (!hidrato_json["Hammerschmidt"]["inibidor"].IsString()) {
                    sucesso = false;
                    erros.push_back(chaveHammer + "/inibidor: deve ser do tipo string");
                }
            }

            // Validar campo fracFWcarregada (opcional)
            if (hidrato_json["Hammerschmidt"].HasMember("fracFWcarregada")) {
                if (!hidrato_json["Hammerschmidt"]["fracFWcarregada"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveHammer + "/fracFWcarregada: deve ser do tipo number");
                }
            }
        }
    }

    // Validar objeto PropFluHidrato (opcional)
    if (hidrato_json.HasMember("PropFluHidrato")) {
        if (!hidrato_json["PropFluHidrato"].IsObject()) {
            sucesso = false;
            erros.push_back("hidrato/PropFluHidrato: deve ser do tipo objeto");
        } else {
            string chaveProp = "hidrato/PropFluHidrato";

            // Validar campo coefEsteq (opcional)
            if (hidrato_json["PropFluHidrato"].HasMember("coefEsteq")) {
                if (!hidrato_json["PropFluHidrato"]["coefEsteq"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveProp + "/coefEsteq: deve ser do tipo number");
                }
            }
        }
    }

    // Validar objeto ModeloTurner (opcional)
    if (hidrato_json.HasMember("ModeloTurner")) {
        if (!hidrato_json["ModeloTurner"].IsObject()) {
            sucesso = false;
            erros.push_back("hidrato/ModeloTurner: deve ser do tipo objeto");
        } else {
            string chaveTurner = "hidrato/ModeloTurner";

            // Validar campo estruturaHidratos (opcional)
            if (hidrato_json["ModeloTurner"].HasMember("estruturaHidratos")) {
                if (!hidrato_json["ModeloTurner"]["estruturaHidratos"].IsString()) {
                    sucesso = false;
                    erros.push_back(chaveTurner + "/estruturaHidratos: deve ser do tipo string");
                }
            }

            // Validar campo rd (opcional)
            if (hidrato_json["ModeloTurner"].HasMember("rd")) {
                if (!hidrato_json["ModeloTurner"]["rd"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveTurner + "/rd: deve ser do tipo number");
                }
            }

            // Validar campo rp (opcional)
            if (hidrato_json["ModeloTurner"].HasMember("rp")) {
                if (!hidrato_json["ModeloTurner"]["rp"].IsNumber()) {
                    sucesso = false;
                    erros.push_back(chaveTurner + "/rp: deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_ipr(Value &ipr_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/ipr");

    // Validar se ipr é um array
    if (!ipr_json.IsArray()) {
        sucesso = false;
        erros.push_back("ipr: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array ipr
    for (SizeType i = 0; i < ipr_json.Size(); i++) {
        string indiceIpr = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!ipr_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceIpr + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (ipr_json[i].HasMember("ativo")) {
            if (!ipr_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (ipr_json[i].HasMember("id")) {
            if (!ipr_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (ipr_json[i].HasMember("comprimentoMedido")) {
            if (!ipr_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar campo tipoIPR (opcional)
        if (ipr_json[i].HasMember("tipoIPR")) {
            if (!ipr_json[i]["tipoIPR"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/tipoIPR: deve ser do tipo inteiro");
            }
        }

        // Validar array pressaoEstatica (opcional)
        if (ipr_json[i].HasMember("pressaoEstatica")) {
            if (!ipr_json[i]["pressaoEstatica"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/pressaoEstatica: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["pressaoEstatica"].Size(); j++) {
                    if (!ipr_json[i]["pressaoEstatica"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/pressaoEstatica[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array tempoPressaoEstatica (opcional)
        if (ipr_json[i].HasMember("tempoPressaoEstatica")) {
            if (!ipr_json[i]["tempoPressaoEstatica"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/tempoPressaoEstatica: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["tempoPressaoEstatica"].Size(); j++) {
                    if (!ipr_json[i]["tempoPressaoEstatica"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/tempoPressaoEstatica[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array temperaturas (opcional)
        if (ipr_json[i].HasMember("temperaturas")) {
            if (!ipr_json[i]["temperaturas"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/temperaturas: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["temperaturas"].Size(); j++) {
                    if (!ipr_json[i]["temperaturas"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/temperaturas[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array tempoTemperaturas (opcional)
        if (ipr_json[i].HasMember("tempoTemperaturas")) {
            if (!ipr_json[i]["tempoTemperaturas"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/tempoTemperaturas: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["tempoTemperaturas"].Size(); j++) {
                    if (!ipr_json[i]["tempoTemperaturas"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/tempoTemperaturas[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array ip (opcional)
        if (ipr_json[i].HasMember("ip")) {
            if (!ipr_json[i]["ip"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/ip: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["ip"].Size(); j++) {
                    if (!ipr_json[i]["ip"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/ip[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array tempoip (opcional)
        if (ipr_json[i].HasMember("tempoip")) {
            if (!ipr_json[i]["tempoip"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/tempoip: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["tempoip"].Size(); j++) {
                    if (!ipr_json[i]["tempoip"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/tempoip[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array qMax (opcional)
        if (ipr_json[i].HasMember("qMax")) {
            if (!ipr_json[i]["qMax"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/qMax: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["qMax"].Size(); j++) {
                    if (!ipr_json[i]["qMax"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/qMax[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array tempoqMax (opcional)
        if (ipr_json[i].HasMember("tempoqMax")) {
            if (!ipr_json[i]["tempoqMax"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/tempoqMax: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["tempoqMax"].Size(); j++) {
                    if (!ipr_json[i]["tempoqMax"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/tempoqMax[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array ii (opcional)
        if (ipr_json[i].HasMember("ii")) {
            if (!ipr_json[i]["ii"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/ii: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["ii"].Size(); j++) {
                    if (!ipr_json[i]["ii"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/ii[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array tempoii (opcional)
        if (ipr_json[i].HasMember("tempoii")) {
            if (!ipr_json[i]["tempoii"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/tempoii: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < ipr_json[i]["tempoii"].Size(); j++) {
                    if (!ipr_json[i]["tempoii"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceIpr + "/tempoii[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo indFluidoPro (opcional)
        if (ipr_json[i].HasMember("indFluidoPro")) {
            if (!ipr_json[i]["indFluidoPro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/indFluidoPro: deve ser do tipo inteiro");
            }
        }

        // Validar campo indiFluidoPro (opcional)
        if (ipr_json[i].HasMember("indiFluidoPro")) {
            if (!ipr_json[i]["indiFluidoPro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceIpr + "/indiFluidoPro: deve ser do tipo inteiro");
            }
        }
    }
}

void validadorTipo::valida_gasInj(Value &gasInj_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/gasInj");

    // Validar se gasInj é um objeto
    if (!gasInj_json.IsObject()) {
        sucesso = false;
        erros.push_back("gasInj: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (gasInj_json.HasMember("ativo")) {
        if (!gasInj_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("gasInj/ativo: deve ser do tipo booleano");
        }
    }

    // Validar campo tipoCC (opcional)
    if (gasInj_json.HasMember("tipoCC")) {
        if (!gasInj_json["tipoCC"].IsInt()) {
            sucesso = false;
            erros.push_back("gasInj/tipoCC: deve ser do tipo inteiro");
        }
    }

    // Validar array temperatura (opcional)
    if (gasInj_json.HasMember("temperatura")) {
        if (!gasInj_json["temperatura"].IsArray()) {
            sucesso = false;
            erros.push_back("gasInj/temperatura: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < gasInj_json["temperatura"].Size(); i++) {
                if (!gasInj_json["temperatura"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("gasInj/temperatura[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array tempo (opcional)
    if (gasInj_json.HasMember("tempo")) {
        if (!gasInj_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("gasInj/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < gasInj_json["tempo"].Size(); i++) {
                if (!gasInj_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("gasInj/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array vazaoGas (opcional)
    if (gasInj_json.HasMember("vazaoGas")) {
        if (!gasInj_json["vazaoGas"].IsArray()) {
            sucesso = false;
            erros.push_back("gasInj/vazaoGas: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < gasInj_json["vazaoGas"].Size(); i++) {
                if (!gasInj_json["vazaoGas"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("gasInj/vazaoGas[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array pressaoInjecao (opcional)
    if (gasInj_json.HasMember("pressaoInjecao")) {
        if (!gasInj_json["pressaoInjecao"].IsArray()) {
            sucesso = false;
            erros.push_back("gasInj/pressaoInjecao: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < gasInj_json["pressaoInjecao"].Size(); i++) {
                if (!gasInj_json["pressaoInjecao"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("gasInj/pressaoInjecao[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar campo chuteVazaoInjecao (opcional)
    if (gasInj_json.HasMember("chuteVazaoInjecao")) {
        if (!gasInj_json["chuteVazaoInjecao"].IsBool()) {
            sucesso = false;
            erros.push_back("gasInj/chuteVazaoInjecao: deve ser do tipo booleano");
        }
    }
}

void validadorTipo::valida_fonte_gaslift(Value &fonteGasLift_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fonteGasLift");

    // Validar se fonteGasLift é um array
    if (!fonteGasLift_json.IsArray()) {
        sucesso = false;
        erros.push_back("fonteGasLift: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fonteGasLift
    for (SizeType i = 0; i < fonteGasLift_json.Size(); i++) {
        string indiceFonte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fonteGasLift_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFonte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fonteGasLift_json[i].HasMember("ativo")) {
            if (!fonteGasLift_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo colunaEanular (opcional)
        if (fonteGasLift_json[i].HasMember("colunaEanular")) {
            if (!fonteGasLift_json[i]["colunaEanular"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/colunaEanular: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fonteGasLift_json[i].HasMember("id")) {
            if (!fonteGasLift_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedidoProducao (opcional)
        if (fonteGasLift_json[i].HasMember("comprimentoMedidoProducao")) {
            if (!fonteGasLift_json[i]["comprimentoMedidoProducao"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedidoProducao: deve ser do tipo number");
            }
        }

        // Validar campo comprimentoMedidoServico (opcional)
        if (fonteGasLift_json[i].HasMember("comprimentoMedidoServico")) {
            if (!fonteGasLift_json[i]["comprimentoMedidoServico"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedidoServico: deve ser do tipo number");
            }
        }

        // Validar campo tipoValvula (opcional)
        if (fonteGasLift_json[i].HasMember("tipoValvula")) {
            if (!fonteGasLift_json[i]["tipoValvula"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tipoValvula: deve ser do tipo inteiro");
            }
        }

        // Validar campo diametroOrificio (opcional)
        if (fonteGasLift_json[i].HasMember("diametroOrificio")) {
            if (!fonteGasLift_json[i]["diametroOrificio"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/diametroOrificio: deve ser do tipo number");
            }
        }

        // Validar campo cdvgl (opcional)
        if (fonteGasLift_json[i].HasMember("cdvgl")) {
            if (!fonteGasLift_json[i]["cdvgl"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/cdvgl: deve ser do tipo number");
            }
        }

        // Validar campo frecupera (opcional)
        if (fonteGasLift_json[i].HasMember("frecupera")) {
            if (!fonteGasLift_json[i]["frecupera"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/frecupera: deve ser do tipo number");
            }
        }

        // Validar campo cdvLiq (opcional)
        if (fonteGasLift_json[i].HasMember("cdvLiq")) {
            if (!fonteGasLift_json[i]["cdvLiq"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/cdvLiq: deve ser do tipo number");
            }
        }

        // Validar campo frecuperaLiq (opcional)
        if (fonteGasLift_json[i].HasMember("frecuperaLiq")) {
            if (!fonteGasLift_json[i]["frecuperaLiq"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/frecuperaLiq: deve ser do tipo number");
            }
        }

        // Validar campo razaoArea (opcional)
        if (fonteGasLift_json[i].HasMember("razaoArea")) {
            if (!fonteGasLift_json[i]["razaoArea"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/razaoArea: deve ser do tipo number");
            }
        }

        // Validar campo pressaoCalibracao (opcional)
        if (fonteGasLift_json[i].HasMember("pressaoCalibracao")) {
            if (!fonteGasLift_json[i]["pressaoCalibracao"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/pressaoCalibracao: deve ser do tipo number");
            }
        }

        // Validar campo temperaturaCalibracao (opcional)
        if (fonteGasLift_json[i].HasMember("temperaturaCalibracao")) {
            if (!fonteGasLift_json[i]["temperaturaCalibracao"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/temperaturaCalibracao: deve ser do tipo number");
            }
        }

        // Validar campo diametroExterno (opcional)
        if (fonteGasLift_json[i].HasMember("diametroExterno")) {
            if (!fonteGasLift_json[i]["diametroExterno"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/diametroExterno: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_fonte_gas(Value &fonteGas_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fonteGas");

    // Validar se fonteGas é um array
    if (!fonteGas_json.IsArray()) {
        sucesso = false;
        erros.push_back("fonteGas: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fonteGas
    for (SizeType i = 0; i < fonteGas_json.Size(); i++) {
        string indiceFonte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fonteGas_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFonte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fonteGas_json[i].HasMember("ativo")) {
            if (!fonteGas_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fonteGas_json[i].HasMember("id")) {
            if (!fonteGas_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (fonteGas_json[i].HasMember("comprimentoMedido")) {
            if (!fonteGas_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (fonteGas_json[i].HasMember("tempo")) {
            if (!fonteGas_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteGas_json[i]["tempo"].Size(); j++) {
                    if (!fonteGas_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array temperatura (opcional)
        if (fonteGas_json[i].HasMember("temperatura")) {
            if (!fonteGas_json[i]["temperatura"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/temperatura: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteGas_json[i]["temperatura"].Size(); j++) {
                    if (!fonteGas_json[i]["temperatura"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/temperatura[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array vazaoGas (opcional)
        if (fonteGas_json[i].HasMember("vazaoGas")) {
            if (!fonteGas_json[i]["vazaoGas"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/vazaoGas: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteGas_json[i]["vazaoGas"].Size(); j++) {
                    if (!fonteGas_json[i]["vazaoGas"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/vazaoGas[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array vazaoFluidoComplementar (opcional)
        if (fonteGas_json[i].HasMember("vazaoFluidoComplementar")) {
            if (!fonteGas_json[i]["vazaoFluidoComplementar"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/vazaoFluidoComplementar: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteGas_json[i]["vazaoFluidoComplementar"].Size(); j++) {
                    if (!fonteGas_json[i]["vazaoFluidoComplementar"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/vazaoFluidoComplementar[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo seco (opcional)
        if (fonteGas_json[i].HasMember("seco")) {
            if (!fonteGas_json[i]["seco"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/seco: deve ser do tipo booleano");
            }
        }

        // Validar campo indiFluidoPro (opcional)
        if (fonteGas_json[i].HasMember("indiFluidoPro")) {
            if (!fonteGas_json[i]["indiFluidoPro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/indiFluidoPro: deve ser do tipo inteiro");
            }
        }
    }
}

void validadorTipo::valida_fonte_liquido(Value &fonteLiquido_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fonteLiquido");

    // Validar se fonteLiquido é um array
    if (!fonteLiquido_json.IsArray()) {
        sucesso = false;
        erros.push_back("fonteLiquido: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fonteLiquido
    for (SizeType i = 0; i < fonteLiquido_json.Size(); i++) {
        string indiceFonte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fonteLiquido_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFonte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fonteLiquido_json[i].HasMember("ativo")) {
            if (!fonteLiquido_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fonteLiquido_json[i].HasMember("id")) {
            if (!fonteLiquido_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (fonteLiquido_json[i].HasMember("comprimentoMedido")) {
            if (!fonteLiquido_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (fonteLiquido_json[i].HasMember("tempo")) {
            if (!fonteLiquido_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteLiquido_json[i]["tempo"].Size(); j++) {
                    if (!fonteLiquido_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array temperatura (opcional)
        if (fonteLiquido_json[i].HasMember("temperatura")) {
            if (!fonteLiquido_json[i]["temperatura"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/temperatura: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteLiquido_json[i]["temperatura"].Size(); j++) {
                    if (!fonteLiquido_json[i]["temperatura"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/temperatura[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array beta (opcional)
        if (fonteLiquido_json[i].HasMember("beta")) {
            if (!fonteLiquido_json[i]["beta"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/beta: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteLiquido_json[i]["beta"].Size(); j++) {
                    if (!fonteLiquido_json[i]["beta"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/beta[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array vazaoLiquido (opcional)
        if (fonteLiquido_json[i].HasMember("vazaoLiquido")) {
            if (!fonteLiquido_json[i]["vazaoLiquido"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/vazaoLiquido: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteLiquido_json[i]["vazaoLiquido"].Size(); j++) {
                    if (!fonteLiquido_json[i]["vazaoLiquido"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/vazaoLiquido[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo indiFluidoPro (opcional)
        if (fonteLiquido_json[i].HasMember("indiFluidoPro")) {
            if (!fonteLiquido_json[i]["indiFluidoPro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/indiFluidoPro: deve ser do tipo inteiro");
            }
        }
    }
}

void validadorTipo::valida_fonte_massa(Value &fonteMassa_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fonteMassa");

    // Validar se fonteMassa é um array
    if (!fonteMassa_json.IsArray()) {
        sucesso = false;
        erros.push_back("fonteMassa: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fonteMassa
    for (SizeType i = 0; i < fonteMassa_json.Size(); i++) {
        string indiceFonte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fonteMassa_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFonte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fonteMassa_json[i].HasMember("ativo")) {
            if (!fonteMassa_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fonteMassa_json[i].HasMember("id")) {
            if (!fonteMassa_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo tipoTermo (opcional)
        if (fonteMassa_json[i].HasMember("tipoTermo")) {
            if (!fonteMassa_json[i]["tipoTermo"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tipoTermo: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (fonteMassa_json[i].HasMember("comprimentoMedido")) {
            if (!fonteMassa_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (fonteMassa_json[i].HasMember("tempo")) {
            if (!fonteMassa_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteMassa_json[i]["tempo"].Size(); j++) {
                    if (!fonteMassa_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array temperatura (opcional)
        if (fonteMassa_json[i].HasMember("temperatura")) {
            if (!fonteMassa_json[i]["temperatura"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/temperatura: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteMassa_json[i]["temperatura"].Size(); j++) {
                    if (!fonteMassa_json[i]["temperatura"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/temperatura[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array vazaoMassT (opcional)
        if (fonteMassa_json[i].HasMember("vazaoMassT")) {
            if (!fonteMassa_json[i]["vazaoMassT"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/vazaoMassT: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteMassa_json[i]["vazaoMassT"].Size(); j++) {
                    if (!fonteMassa_json[i]["vazaoMassT"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/vazaoMassT[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array vazaoMassC (opcional)
        if (fonteMassa_json[i].HasMember("vazaoMassC")) {
            if (!fonteMassa_json[i]["vazaoMassC"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/vazaoMassC: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteMassa_json[i]["vazaoMassC"].Size(); j++) {
                    if (!fonteMassa_json[i]["vazaoMassC"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/vazaoMassC[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array vazaoMassG (opcional)
        if (fonteMassa_json[i].HasMember("vazaoMassG")) {
            if (!fonteMassa_json[i]["vazaoMassG"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/vazaoMassG: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteMassa_json[i]["vazaoMassG"].Size(); j++) {
                    if (!fonteMassa_json[i]["vazaoMassG"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/vazaoMassG[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo indiFluidoPro (opcional)
        if (fonteMassa_json[i].HasMember("indiFluidoPro")) {
            if (!fonteMassa_json[i]["indiFluidoPro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/indiFluidoPro: deve ser do tipo inteiro");
            }
        }
    }
}

void validadorTipo::valida_furo(Value &fontePressao_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fontePressao");

    // Validar se fontePressao é um array
    if (!fontePressao_json.IsArray()) {
        sucesso = false;
        erros.push_back("fontePressao: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fontePressao
    for (SizeType i = 0; i < fontePressao_json.Size(); i++) {
        string indiceFonte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fontePressao_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFonte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fontePressao_json[i].HasMember("ativo")) {
            if (!fontePressao_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fontePressao_json[i].HasMember("id")) {
            if (!fontePressao_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo TipoAbertura (opcional)
        if (fontePressao_json[i].HasMember("TipoAbertura")) {
            if (!fontePressao_json[i]["TipoAbertura"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/TipoAbertura: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (fontePressao_json[i].HasMember("comprimentoMedido")) {
            if (!fontePressao_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar campo beta (opcional)
        if (fontePressao_json[i].HasMember("beta")) {
            if (!fontePressao_json[i]["beta"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/beta: deve ser do tipo number");
            }
        }

        // Validar campo titAmb (opcional)
        if (fontePressao_json[i].HasMember("titAmb")) {
            if (!fontePressao_json[i]["titAmb"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/titAmb: deve ser do tipo number");
            }
        }

        // Validar campo cd (opcional)
        if (fontePressao_json[i].HasMember("cd")) {
            if (!fontePressao_json[i]["cd"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/cd: deve ser do tipo number");
            }
        }

        // Validar campo temperatura (opcional)
        if (fontePressao_json[i].HasMember("temperatura")) {
            if (!fontePressao_json[i]["temperatura"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/temperatura: deve ser do tipo number");
            }
        }

        // Validar campo pressao (opcional)
        if (fontePressao_json[i].HasMember("pressao")) {
            if (!fontePressao_json[i]["pressao"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/pressao: deve ser do tipo number");
            }
        }

        // Validar array abertura (opcional)
        if (fontePressao_json[i].HasMember("abertura")) {
            if (!fontePressao_json[i]["abertura"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/abertura: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fontePressao_json[i]["abertura"].Size(); j++) {
                    if (!fontePressao_json[i]["abertura"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/abertura[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array tempo (opcional)
        if (fontePressao_json[i].HasMember("tempo")) {
            if (!fontePressao_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fontePressao_json[i]["tempo"].Size(); j++) {
                    if (!fontePressao_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo tipoFluido (opcional)
        if (fontePressao_json[i].HasMember("tipoFluido")) {
            if (!fontePressao_json[i]["tipoFluido"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tipoFluido: deve ser do tipo inteiro");
            }
        }

        // Validar campo indiFluidoPro (opcional)
        if (fontePressao_json[i].HasMember("indiFluidoPro")) {
            if (!fontePressao_json[i]["indiFluidoPro"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/indiFluidoPro: deve ser do tipo inteiro");
            }
        }

        // Validar array check (opcional)
        if (fontePressao_json[i].HasMember("check")) {
            if (!fontePressao_json[i]["check"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/check: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fontePressao_json[i]["check"].Size(); j++) {
                    if (!fontePressao_json[i]["check"][j].IsInt()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/check[" + std::to_string(j) + "]: deve ser do tipo inteiro");
                    }
                }
            }
        }

        // Validar array tempoChk (opcional)
        if (fontePressao_json[i].HasMember("tempoChk")) {
            if (!fontePressao_json[i]["tempoChk"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/tempoChk: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fontePressao_json[i]["tempoChk"].Size(); j++) {
                    if (!fontePressao_json[i]["tempoChk"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceFonte + "/tempoChk[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo ambienteGas (opcional)
        if (fontePressao_json[i].HasMember("ambienteGas")) {
            if (!fontePressao_json[i]["ambienteGas"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ambienteGas: deve ser do tipo booleano");
            }
        }
    }
}

void validadorTipo::valida_bcs(Value &bcs_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/bcs");

    // Validar se bcs é um array
    if (!bcs_json.IsArray()) {
        sucesso = false;
        erros.push_back("bcs: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array bcs
    for (SizeType i = 0; i < bcs_json.Size(); i++) {
        string indiceBcs = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!bcs_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceBcs + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (bcs_json[i].HasMember("ativo")) {
            if (!bcs_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo correcHI (opcional)
        if (bcs_json[i].HasMember("correcHI")) {
            if (!bcs_json[i]["correcHI"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/correcHI: deve ser do tipo booleano");
            }
        }

        // Validar campo correcHI (opcional)
        if (bcs_json[i].HasMember("fracTermMotorEfic")) {
            if (!bcs_json[i]["fracTermMotorEfic"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/correcHI: deve ser do tipo number");
            }
        }

        // Validar campo id (opcional)
        if (bcs_json[i].HasMember("id")) {
            if (!bcs_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (bcs_json[i].HasMember("comprimentoMedido")) {
            if (!bcs_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (bcs_json[i].HasMember("tempo")) {
            if (!bcs_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bcs_json[i]["tempo"].Size(); j++) {
                    if (!bcs_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array frequencia (opcional)
        if (bcs_json[i].HasMember("frequencia")) {
            if (!bcs_json[i]["frequencia"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/frequencia: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bcs_json[i]["frequencia"].Size(); j++) {
                    if (!bcs_json[i]["frequencia"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/frequencia[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array vazao (opcional)
        if (bcs_json[i].HasMember("vazao")) {
            if (!bcs_json[i]["vazao"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/vazao: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bcs_json[i]["vazao"].Size(); j++) {
                    if (!bcs_json[i]["vazao"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/vazao[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array potencia (opcional)
        if (bcs_json[i].HasMember("potencia")) {
            if (!bcs_json[i]["potencia"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/potencia: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bcs_json[i]["potencia"].Size(); j++) {
                    if (!bcs_json[i]["potencia"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/potencia[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array eficiencia (opcional)
        if (bcs_json[i].HasMember("eficiencia")) {
            if (!bcs_json[i]["eficiencia"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/eficiencia: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bcs_json[i]["eficiencia"].Size(); j++) {
                    if (!bcs_json[i]["eficiencia"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/eficiencia[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array head (opcional)
        if (bcs_json[i].HasMember("head")) {
            if (!bcs_json[i]["head"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/head: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bcs_json[i]["head"].Size(); j++) {
                    if (!bcs_json[i]["head"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/head[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo freqref (opcional)
        if (bcs_json[i].HasMember("freqref")) {
            if (!bcs_json[i]["freqref"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/freqref: deve ser do tipo number");
            }
        }

        // Validar campo nestag (opcional)
        if (bcs_json[i].HasMember("nestag")) {
            if (!bcs_json[i]["nestag"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/nestag: deve ser do tipo inteiro");
            }
        }

        // Validar campo nestagFab (opcional)
        if (bcs_json[i].HasMember("nestagFab")) {
            if (!bcs_json[i]["nestagFab"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/nestagFab: deve ser do tipo inteiro");
            }
        }

        // Validar campo EficienciaMotor (opcional)
        if (bcs_json[i].HasMember("EficienciaMotor")) {
            if (!bcs_json[i]["EficienciaMotor"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/EficienciaMotor: deve ser do tipo number");
            }
        }

        // Validar campo FrequenciaMinima (opcional)
        if (bcs_json[i].HasMember("FrequenciaMinima")) {
            if (!bcs_json[i]["FrequenciaMinima"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/FrequenciaMinima: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_multibcs(Value &multibcs_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/multibcs");

    // Validar se bcs é um array
    if (!multibcs_json.IsArray()) {
        sucesso = false;
        erros.push_back("multibcs: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array bcs
    for (SizeType i = 0; i < multibcs_json.Size(); i++) {
        string indiceBcs = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!multibcs_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceBcs + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (multibcs_json[i].HasMember("ativo")) {
            if (!multibcs_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo correcHI (opcional)
        if (multibcs_json[i].HasMember("correcHI")) {
            if (!multibcs_json[i]["correcHI"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/correcHI: deve ser do tipo booleano");
            }
        }

        // Validar campo correcHI (opcional)
        if (multibcs_json[i].HasMember("fracTermMotorEfic")) {
            if (!multibcs_json[i]["fracTermMotorEfic"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/correcHI: deve ser do tipo number");
            }
        }

        // Validar campo id (opcional)
        if (multibcs_json[i].HasMember("id")) {
            if (!multibcs_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (multibcs_json[i].HasMember("comprimentoMedido")) {
            if (!multibcs_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (multibcs_json[i].HasMember("tempo")) {
            if (!multibcs_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < multibcs_json[i]["tempo"].Size(); j++) {
                    if (!multibcs_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array frequencia (opcional)
        if (multibcs_json[i].HasMember("frequencia")) {
            if (!multibcs_json[i]["frequencia"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/frequencia: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < multibcs_json[i]["frequencia"].Size(); j++) {
                    if (!multibcs_json[i]["frequencia"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBcs + "/frequencia[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo freqref (opcional)
        if (multibcs_json[i].HasMember("freqref")) {
            if (!multibcs_json[i]["freqref"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/freqref: deve ser do tipo number");
            }
        }

        // Validar campo EficienciaMotor (opcional)
        if (multibcs_json[i].HasMember("EficienciaMotor")) {
            if (!multibcs_json[i]["EficienciaMotor"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/EficienciaMotor: deve ser do tipo number");
            }
        }

        // Validar campo FrequenciaMinima (opcional)
        if (multibcs_json[i].HasMember("FrequenciaMinima")) {
            if (!multibcs_json[i]["FrequenciaMinima"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/FrequenciaMinima: deve ser do tipo number");
            }
        }

        if (multibcs_json[i].HasMember("curva")) {
            if (!multibcs_json[i]["curva"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBcs + "/curva: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < multibcs_json[i]["curva"].Size(); j++) {
                    // Validar campo nestag (opcional)
                    if (multibcs_json[i]["curva"][j].HasMember("nestag")) {
                        if (!multibcs_json[i]["curva"][j]["nestag"].IsInt()) {
                            sucesso = false;
                            erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "][nestag]: deve ser do tipo inteiro");
                        }
                    }
                    // Validar campo nestagFab (opcional)
                    if (multibcs_json[i]["curva"][j].HasMember("nestagFab")) {
                        if (!multibcs_json[i]["curva"][j]["nestagFab"].IsInt()) {
                            sucesso = false;
                            erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "/nestagFab: deve ser do tipo inteiro");
                        }
                    }
                    // Validar array vazao (opcional)
                    if (multibcs_json[i]["curva"][j].HasMember("vazao")) {
                        if (!multibcs_json[i]["curva"][j]["vazao"].IsArray()) {
                            sucesso = false;
                            erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/vazao: deve ser do tipo array");
                        } else {
                            for (SizeType k = 0; k < multibcs_json[i]["curva"][j]["vazao"].Size(); k++) {
                                if (!multibcs_json[i]["curva"][j]["vazao"][k].IsNumber()) {
                                    sucesso = false;
                                    erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/vazao[" + std::to_string(k) + "]: deve ser do tipo number");
                                }
                            }
                        }
                    }
                    // Validar array potencia (opcional)
                    if (multibcs_json[i]["curva"][j].HasMember("potencia")) {
                        if (!multibcs_json[i]["curva"][j]["potencia"].IsArray()) {
                            sucesso = false;
                            erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/potencia: deve ser do tipo array");
                        } else {
                            for (SizeType k = 0; k < multibcs_json[i]["curva"][j]["potencia"].Size(); k++) {
                                if (!multibcs_json[i]["curva"][j]["potencia"][k].IsNumber()) {
                                    sucesso = false;
                                    erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/potencia[" + std::to_string(k) + "]: deve ser do tipo number");
                                }
                            }
                        }
                    }
                    // Validar array eficiencia (opcional)
                    if (multibcs_json[i]["curva"][j].HasMember("eficiencia")) {
                        if (!multibcs_json[i]["curva"][j]["eficiencia"].IsArray()) {
                            sucesso = false;
                            erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/eficiencia: deve ser do tipo array");
                        } else {
                            for (SizeType k = 0; k < multibcs_json[i]["curva"][j]["eficiencia"].Size(); k++) {
                                if (!multibcs_json[i]["curva"][j]["eficiencia"][k].IsNumber()) {
                                    sucesso = false;
                                    erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/eficiencia[" + std::to_string(k) + "]: deve ser do tipo number");
                                }
                            }
                        }
                    }
                    // Validar array head (opcional)
                    if (multibcs_json[i]["curva"][j].HasMember("head")) {
                        if (!multibcs_json[i]["curva"][j]["head"].IsArray()) {
                            sucesso = false;
                            erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/head: deve ser do tipo array");
                        } else {
                            for (SizeType k = 0; k < multibcs_json[i]["curva"][j]["head"].Size(); k++) {
                                if (!multibcs_json[i]["curva"][j]["head"][k].IsNumber()) {
                                    sucesso = false;
                                    erros.push_back(indiceBcs + "/curva[" + std::to_string(j) + "]" + "/head[" + std::to_string(k) + "]: deve ser do tipo number");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void validadorTipo::valida_valv(Value &valvula_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/valvula");

    // Validar se valvula é um array
    if (!valvula_json.IsArray()) {
        sucesso = false;
        erros.push_back("valvula: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array valvula
    for (SizeType i = 0; i < valvula_json.Size(); i++) {
        string indiceValvula = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!valvula_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceValvula + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (valvula_json[i].HasMember("ativo")) {
            if (!valvula_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (valvula_json[i].HasMember("id")) {
            if (!valvula_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo curvaCV (opcional)
        if (valvula_json[i].HasMember("curvaCV")) {
            if (!valvula_json[i]["curvaCV"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/curvaCV: deve ser do tipo inteiro");
            }
        }

        // Validar campo curvaDinamic (opcional)
        if (valvula_json[i].HasMember("curvaDinamic")) {
            if (!valvula_json[i]["curvaDinamic"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/curvaDinamic: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (valvula_json[i].HasMember("comprimentoMedido")) {
            if (!valvula_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (valvula_json[i].HasMember("tempo")) {
            if (!valvula_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < valvula_json[i]["tempo"].Size(); j++) {
                    if (!valvula_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceValvula + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array abertura (opcional)
        if (valvula_json[i].HasMember("abertura")) {
            if (!valvula_json[i]["abertura"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/abertura: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < valvula_json[i]["abertura"].Size(); j++) {
                    if (!valvula_json[i]["abertura"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceValvula + "/abertura[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo cd (opcional)
        if (valvula_json[i].HasMember("cd")) {
            if (!valvula_json[i]["cd"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/cd: deve ser do tipo number");
            }
        }

        // Validar array x1 (opcional)
        if (valvula_json[i].HasMember("x1")) {
            if (!valvula_json[i]["x1"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/x1: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < valvula_json[i]["x1"].Size(); j++) {
                    if (!valvula_json[i]["x1"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceValvula + "/x1[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array cv1 (opcional)
        if (valvula_json[i].HasMember("cv1")) {
            if (!valvula_json[i]["cv1"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceValvula + "/cv1: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < valvula_json[i]["cv1"].Size(); j++) {
                    if (!valvula_json[i]["cv1"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceValvula + "/cv1[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }
    }
}

void validadorTipo::valida_chokeSup(Value &chokeSup_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/chokeSup");

    // Validar se chokeSup é um objeto
    if (!chokeSup_json.IsObject()) {
        sucesso = false;
        erros.push_back("chokeSup: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (chokeSup_json.HasMember("ativo")) {
        if (!chokeSup_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("chokeSup/ativo: deve ser do tipo booleano");
        }
    }

    // Validar campo curvaCV (opcional)
    if (chokeSup_json.HasMember("curvaCV")) {
        if (!chokeSup_json["curvaCV"].IsInt()) {
            sucesso = false;
            erros.push_back("chokeSup/curvaCV: deve ser do tipo inteiro");
        }
    }

    // Validar campo curvaDinamic (opcional)
    if (chokeSup_json.HasMember("curvaDinamic")) {
        if (!chokeSup_json["curvaDinamic"].IsInt()) {
            sucesso = false;
            erros.push_back("chokeSup/curvaDinamic: deve ser do tipo inteiro");
        }
    }

    // Validar array tempo (opcional)
    if (chokeSup_json.HasMember("tempo")) {
        if (!chokeSup_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("chokeSup/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < chokeSup_json["tempo"].Size(); i++) {
                if (!chokeSup_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("chokeSup/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array abertura (opcional)
    if (chokeSup_json.HasMember("abertura")) {
        if (!chokeSup_json["abertura"].IsArray()) {
            sucesso = false;
            erros.push_back("chokeSup/abertura: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < chokeSup_json["abertura"].Size(); i++) {
                if (!chokeSup_json["abertura"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("chokeSup/abertura[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar campo coeficienteDescarga (opcional)
    if (chokeSup_json.HasMember("coeficienteDescarga")) {
        if (!chokeSup_json["coeficienteDescarga"].IsNumber()) {
            sucesso = false;
            erros.push_back("chokeSup/coeficienteDescarga: deve ser do tipo number");
        }
    }

    // Validar campo modelo (opcional)
    if (chokeSup_json.HasMember("modelo")) {
        if (!chokeSup_json["modelo"].IsInt()) {
            sucesso = false;
            erros.push_back("chokeSup/modelo: deve ser do tipo inteiro");
        }
    }

    // Validar array x1 (opcional)
    if (chokeSup_json.HasMember("x1")) {
        if (!chokeSup_json["x1"].IsArray()) {
            sucesso = false;
            erros.push_back("chokeSup/x1: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < chokeSup_json["x1"].Size(); i++) {
                if (!chokeSup_json["x1"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("chokeSup/x1[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array cv1 (opcional)
    if (chokeSup_json.HasMember("cv1")) {
        if (!chokeSup_json["cv1"].IsArray()) {
            sucesso = false;
            erros.push_back("chokeSup/cv1: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < chokeSup_json["cv1"].Size(); i++) {
                if (!chokeSup_json["cv1"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("chokeSup/cv1[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_master1(Value &master1_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/master1");

    // Validar se master1 é um objeto
    if (!master1_json.IsObject()) {
        sucesso = false;
        erros.push_back("master1: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (master1_json.HasMember("ativo")) {
        if (!master1_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("master1/ativo: deve ser do tipo booleano");
        }
    }

    // Validar campo curvaCV (opcional)
    if (master1_json.HasMember("curvaCV")) {
        if (!master1_json["curvaCV"].IsInt()) {
            sucesso = false;
            erros.push_back("master1/curvaCV: deve ser do tipo inteiro");
        }
    }

    // Validar campo razaoAreaAtiva (opcional)
    if (master1_json.HasMember("razaoAreaAtiva")) {
        if (!master1_json["razaoAreaAtiva"].IsNumber()) {
            sucesso = false;
            erros.push_back("master1/razaoAreaAtiva: deve ser do tipo number");
        }
    }

    // Validar campo comprimentoMedido (opcional)
    if (master1_json.HasMember("comprimentoMedido")) {
        if (!master1_json["comprimentoMedido"].IsNumber()) {
            sucesso = false;
            erros.push_back("master1/comprimentoMedido: deve ser do tipo number");
        }
    }

    // Validar array tempo (opcional)
    if (master1_json.HasMember("tempo")) {
        if (!master1_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("master1/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < master1_json["tempo"].Size(); i++) {
                if (!master1_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("master1/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array abertura (opcional)
    if (master1_json.HasMember("abertura")) {
        if (!master1_json["abertura"].IsArray()) {
            sucesso = false;
            erros.push_back("master1/abertura: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < master1_json["abertura"].Size(); i++) {
                if (!master1_json["abertura"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("master1/abertura[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array x1 (opcional)
    if (master1_json.HasMember("x1")) {
        if (!master1_json["x1"].IsArray()) {
            sucesso = false;
            erros.push_back("master1/x1: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < master1_json["x1"].Size(); i++) {
                if (!master1_json["x1"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("master1/x1[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array cv1 (opcional)
    if (master1_json.HasMember("cv1")) {
        if (!master1_json["cv1"].IsArray()) {
            sucesso = false;
            erros.push_back("master1/cv1: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < master1_json["cv1"].Size(); i++) {
                if (!master1_json["cv1"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("master1/cv1[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_master2(Value &master2_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/master2");

    // Validar se master2 é um objeto
    if (!master2_json.IsObject()) {
        sucesso = false;
        erros.push_back("master2: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (master2_json.HasMember("ativo")) {
        if (!master2_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("master2/ativo: deve ser do tipo booleano");
        }
    }

    // Validar campo comprimentoMedido (opcional)
    if (master2_json.HasMember("comprimentoMedido")) {
        if (!master2_json["comprimentoMedido"].IsNumber()) {
            sucesso = false;
            erros.push_back("master2/comprimentoMedido: deve ser do tipo number");
        }
    }

    // Validar array tempo (opcional)
    if (master2_json.HasMember("tempo")) {
        if (!master2_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("master2/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < master2_json["tempo"].Size(); i++) {
                if (!master2_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("master2/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array abertura (opcional)
    if (master2_json.HasMember("abertura")) {
        if (!master2_json["abertura"].IsArray()) {
            sucesso = false;
            erros.push_back("master2/abertura: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < master2_json["abertura"].Size(); i++) {
                if (!master2_json["abertura"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("master2/abertura[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_separador(Value &separador_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/separador");

    // Validar se separador é um objeto
    if (!separador_json.IsObject()) {
        sucesso = false;
        erros.push_back("separador: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (separador_json.HasMember("ativo")) {
        if (!separador_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("separador/ativo: deve ser do tipo booleano");
        }
    }

    // Validar array tempo (opcional)
    if (separador_json.HasMember("tempo")) {
        if (!separador_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("separador/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < separador_json["tempo"].Size(); i++) {
                if (!separador_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("separador/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array pressao (opcional)
    if (separador_json.HasMember("pressao")) {
        if (!separador_json["pressao"].IsArray()) {
            sucesso = false;
            erros.push_back("separador/pressao: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < separador_json["pressao"].Size(); i++) {
                if (!separador_json["pressao"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("separador/pressao[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_chokeInj(Value &chokeInj_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/chokeInj");

    // Validar se chokeInj é um objeto
    if (!chokeInj_json.IsObject()) {
        sucesso = false;
        erros.push_back("chokeInj: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (chokeInj_json.HasMember("ativo")) {
        if (!chokeInj_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("chokeInj/ativo: deve ser do tipo booleano");
        }
    }

    // Validar array tempo (opcional)
    if (chokeInj_json.HasMember("tempo")) {
        if (!chokeInj_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("chokeInj/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < chokeInj_json["tempo"].Size(); i++) {
                if (!chokeInj_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("chokeInj/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array abertura (opcional)
    if (chokeInj_json.HasMember("abertura")) {
        if (!chokeInj_json["abertura"].IsArray()) {
            sucesso = false;
            erros.push_back("chokeInj/abertura: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < chokeInj_json["abertura"].Size(); i++) {
                if (!chokeInj_json["abertura"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("chokeInj/abertura[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar campo coeficienteDescarga (opcional)
    if (chokeInj_json.HasMember("coeficienteDescarga")) {
        if (!chokeInj_json["coeficienteDescarga"].IsNumber()) {
            sucesso = false;
            erros.push_back("chokeInj/coeficienteDescarga: deve ser do tipo number");
        }
    }
}

void validadorTipo::valida_delta_pressao(Value &deltaPressao_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/deltaPressao");

    // Validar se deltaPressao é um array
    if (!deltaPressao_json.IsArray()) {
        sucesso = false;
        erros.push_back("deltaPressao: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array deltaPressao
    for (SizeType i = 0; i < deltaPressao_json.Size(); i++) {
        string indiceDelta = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!deltaPressao_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceDelta + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (deltaPressao_json[i].HasMember("ativo")) {
            if (!deltaPressao_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (deltaPressao_json[i].HasMember("id")) {
            if (!deltaPressao_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (deltaPressao_json[i].HasMember("comprimentoMedido")) {
            if (!deltaPressao_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (deltaPressao_json[i].HasMember("tempo")) {
            if (!deltaPressao_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < deltaPressao_json[i]["tempo"].Size(); j++) {
                    if (!deltaPressao_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceDelta + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array deltaPressao (opcional)
        if (deltaPressao_json[i].HasMember("deltaPressao")) {
            if (!deltaPressao_json[i]["deltaPressao"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/deltaPressao: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < deltaPressao_json[i]["deltaPressao"].Size(); j++) {
                    if (!deltaPressao_json[i]["deltaPressao"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceDelta + "/deltaPressao[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo tipoCompGas (opcional)
        if (deltaPressao_json[i].HasMember("tipoCompGas")) {
            if (!deltaPressao_json[i]["tipoCompGas"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/tipoCompGas: deve ser do tipo inteiro");
            }
        }

        // Validar campo fatPoli (opcional)
        if (deltaPressao_json[i].HasMember("fatPoli")) {
            if (!deltaPressao_json[i]["fatPoli"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/fatPoli: deve ser do tipo number");
            }
        }

        // Validar campo eficLiq (opcional)
        if (deltaPressao_json[i].HasMember("eficLiq")) {
            if (!deltaPressao_json[i]["eficLiq"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/eficLiq: deve ser do tipo number");
            }
        }

        // Validar campo eficGas (opcional)
        if (deltaPressao_json[i].HasMember("eficGas")) {
            if (!deltaPressao_json[i]["eficGas"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceDelta + "/eficGas: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_fonteCalor(Value &fonteCalor_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fonteCalor");

    // Validar se deltaPressao é um array
    if (!fonteCalor_json.IsArray()) {
        sucesso = false;
        erros.push_back("fonteCalor: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array deltaPressao
    for (SizeType i = 0; i < fonteCalor_json.Size(); i++) {
        string indiceCal = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fonteCalor_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceCal + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fonteCalor_json[i].HasMember("ativo")) {
            if (!fonteCalor_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceCal + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fonteCalor_json[i].HasMember("id")) {
            if (!fonteCalor_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceCal + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (fonteCalor_json[i].HasMember("comprimentoMedido")) {
            if (!fonteCalor_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceCal + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (fonteCalor_json[i].HasMember("tempo")) {
            if (!fonteCalor_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceCal + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteCalor_json[i]["tempo"].Size(); j++) {
                    if (!fonteCalor_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceCal + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array deltaPressao (opcional)
        if (fonteCalor_json[i].HasMember("calor")) {
            if (!fonteCalor_json[i]["deltaPressao"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceCal + "/fonteCalor: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < fonteCalor_json[i]["calor"].Size(); j++) {
                    if (!fonteCalor_json[i]["calor"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceCal + "/fonteCalor[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }
    }
}

void validadorTipo::valida_bomba_volumetrica(Value &bombaVolumetrica_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/bombaVolumetrica");

    // Validar se bombaVolumetrica é um array
    if (!bombaVolumetrica_json.IsArray()) {
        sucesso = false;
        erros.push_back("bombaVolumetrica: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array bombaVolumetrica
    for (SizeType i = 0; i < bombaVolumetrica_json.Size(); i++) {
        string indiceBomba = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!bombaVolumetrica_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceBomba + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (bombaVolumetrica_json[i].HasMember("ativo")) {
            if (!bombaVolumetrica_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceBomba + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (bombaVolumetrica_json[i].HasMember("id")) {
            if (!bombaVolumetrica_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceBomba + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (bombaVolumetrica_json[i].HasMember("comprimentoMedido")) {
            if (!bombaVolumetrica_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBomba + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar array tempo (opcional)
        if (bombaVolumetrica_json[i].HasMember("tempo")) {
            if (!bombaVolumetrica_json[i]["tempo"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBomba + "/tempo: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bombaVolumetrica_json[i]["tempo"].Size(); j++) {
                    if (!bombaVolumetrica_json[i]["tempo"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBomba + "/tempo[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar array frequencia (opcional)
        if (bombaVolumetrica_json[i].HasMember("frequencia")) {
            if (!bombaVolumetrica_json[i]["frequencia"].IsArray()) {
                sucesso = false;
                erros.push_back(indiceBomba + "/frequencia: deve ser do tipo array");
            } else {
                for (SizeType j = 0; j < bombaVolumetrica_json[i]["frequencia"].Size(); j++) {
                    if (!bombaVolumetrica_json[i]["frequencia"][j].IsNumber()) {
                        sucesso = false;
                        erros.push_back(indiceBomba + "/frequencia[" + std::to_string(j) + "]: deve ser do tipo number");
                    }
                }
            }
        }

        // Validar campo capacidade (opcional)
        if (bombaVolumetrica_json[i].HasMember("capacidade")) {
            if (!bombaVolumetrica_json[i]["capacidade"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBomba + "/capacidade: deve ser do tipo number");
            }
        }

        // Validar campo fatorpoli (opcional)
        if (bombaVolumetrica_json[i].HasMember("fatorpoli")) {
            if (!bombaVolumetrica_json[i]["fatorpoli"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceBomba + "/fatorpoli: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_pig(Value &pig_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/pig");

    // Validar se pig é um array
    if (!pig_json.IsArray()) {
        sucesso = false;
        erros.push_back("pig: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array pig
    for (SizeType i = 0; i < pig_json.Size(); i++) {
        string indicePig = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!pig_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indicePig + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (pig_json[i].HasMember("ativo")) {
            if (!pig_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indicePig + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (pig_json[i].HasMember("id")) {
            if (!pig_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indicePig + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo lancador (opcional)
        if (pig_json[i].HasMember("lancador")) {
            if (!pig_json[i]["lancador"].IsNumber()) {
                sucesso = false;
                erros.push_back(indicePig + "/lancador: deve ser do tipo number");
            }
        }

        // Validar campo recebedor (opcional)
        if (pig_json[i].HasMember("recebedor")) {
            if (!pig_json[i]["recebedor"].IsNumber()) {
                sucesso = false;
                erros.push_back(indicePig + "/recebedor: deve ser do tipo number");
            }
        }

        // Validar campo tempo (opcional)
        if (pig_json[i].HasMember("tempo")) {
            if (!pig_json[i]["tempo"].IsNumber()) {
                sucesso = false;
                erros.push_back(indicePig + "/tempo: deve ser do tipo number");
            }
        }

        // Validar campo folgaArea (opcional)
        if (pig_json[i].HasMember("folgaArea")) {
            if (!pig_json[i]["folgaArea"].IsNumber()) {
                sucesso = false;
                erros.push_back(indicePig + "/folgaArea: deve ser do tipo number");
            }
        }

        // Validar campo deltaPressao (opcional)
        if (pig_json[i].HasMember("deltaPressao")) {
            if (!pig_json[i]["deltaPressao"].IsNumber()) {
                sucesso = false;
                erros.push_back(indicePig + "/deltaPressao: deve ser do tipo number");
            }
        }

        // Validar campo cdPig (opcional)
        if (pig_json[i].HasMember("cdPig")) {
            if (!pig_json[i]["cdPig"].IsNumber()) {
                sucesso = false;
                erros.push_back(indicePig + "/cdPig: deve ser do tipo number");
            }
        }
    }
}

void validadorTipo::valida_correcao(Value &correcao_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/correcao");

    // Validar se correcao é um objeto
    if (!correcao_json.IsObject()) {
        sucesso = false;
        erros.push_back("correcao: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (correcao_json.HasMember("ativo")) {
        if (!correcao_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("correcao/ativo: deve ser do tipo booleano");
        }
    }

    // Validar array dPdLHidro (opcional)
    if (correcao_json.HasMember("dPdLHidro")) {
        if (!correcao_json["dPdLHidro"].IsArray()) {
            sucesso = false;
            erros.push_back("correcao/dPdLHidro: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < correcao_json["dPdLHidro"].Size(); i++) {
                if (!correcao_json["dPdLHidro"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("correcao/dPdLHidro[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array dPdLFric (opcional)
    if (correcao_json.HasMember("dPdLFric")) {
        if (!correcao_json["dPdLFric"].IsArray()) {
            sucesso = false;
            erros.push_back("correcao/dPdLFric: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < correcao_json["dPdLFric"].Size(); i++) {
                if (!correcao_json["dPdLFric"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("correcao/dPdLFric[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Validar array dTdL (opcional)
    if (correcao_json.HasMember("dTdL")) {
        if (!correcao_json["dTdL"].IsArray()) {
            sucesso = false;
            erros.push_back("correcao/dTdL: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < correcao_json["dTdL"].Size(); i++) {
                if (!correcao_json["dTdL"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("correcao/dTdL[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }
}

void validadorTipo::valida_intermitencia(Value &intermitenciaSevera_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/intermitenciaSevera");

    // Validar se intermitenciaSevera é um array
    if (!intermitenciaSevera_json.IsArray()) {
        sucesso = false;
        erros.push_back("intermitenciaSevera: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array intermitenciaSevera
    for (SizeType i = 0; i < intermitenciaSevera_json.Size(); i++) {
        string indiceIntermi = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!intermitenciaSevera_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceIntermi + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (intermitenciaSevera_json[i].HasMember("ativo")) {
            if (!intermitenciaSevera_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceIntermi + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (intermitenciaSevera_json[i].HasMember("id")) {
            if (!intermitenciaSevera_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceIntermi + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo inicioTrechoAcumula (opcional)
        if (intermitenciaSevera_json[i].HasMember("inicioTrechoAcumula")) {
            if (!intermitenciaSevera_json[i]["inicioTrechoAcumula"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceIntermi + "/inicioTrechoAcumula: deve ser do tipo number");
            }
        }

        // Validar campo fimTrechoAcumula (opcional)
        if (intermitenciaSevera_json[i].HasMember("fimTrechoAcumula")) {
            if (!intermitenciaSevera_json[i]["fimTrechoAcumula"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceIntermi + "/fimTrechoAcumula: deve ser do tipo number");
            }
        }

        // Validar campo fimTrechoColuna (opcional)
        if (intermitenciaSevera_json[i].HasMember("fimTrechoColuna")) {
            if (!intermitenciaSevera_json[i]["fimTrechoColuna"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceIntermi + "/fimTrechoColuna: deve ser do tipo number");
            }
        }

        // Validar campo fracaoVazioPenetracao (opcional)
        if (intermitenciaSevera_json[i].HasMember("fracaoVazioPenetracao")) {
            if (!intermitenciaSevera_json[i]["fracaoVazioPenetracao"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceIntermi + "/fracaoVazioPenetracao: deve ser do tipo number");
            }
        }

        // Validar campo criterio (opcional)
        if (intermitenciaSevera_json[i].HasMember("criterio")) {
            if (!intermitenciaSevera_json[i]["criterio"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceIntermi + "/criterio: deve ser do tipo inteiro");
            }
        }
    }
}

void validadorTipo::valida_fonte_PoroRadial(Value &fontePoroRadial_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fontePoroRadial");

    // Validar se fontePoroRadial é um array
    if (!fontePoroRadial_json.IsArray()) {
        sucesso = false;
        erros.push_back("fontePoroRadial: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fontePoroRadial
    for (SizeType i = 0; i < fontePoroRadial_json.Size(); i++) {
        string indiceFonte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fontePoroRadial_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFonte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fontePoroRadial_json[i].HasMember("ativo")) {
            if (!fontePoroRadial_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fontePoroRadial_json[i].HasMember("id")) {
            if (!fontePoroRadial_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (fontePoroRadial_json[i].HasMember("comprimentoMedido")) {
            if (!fontePoroRadial_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar campo arquivo (opcional)
        if (fontePoroRadial_json[i].HasMember("arquivo")) {
            if (!fontePoroRadial_json[i]["arquivo"].IsString()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/arquivo: deve ser do tipo string");
            }
        }
    }
}

void validadorTipo::valida_fonte_Poro2D(Value &fontePoro2D_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/fontePoro2D");

    // Validar se fontePoro2D é um array
    if (!fontePoro2D_json.IsArray()) {
        sucesso = false;
        erros.push_back("fontePoro2D: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array fontePoro2D
    for (SizeType i = 0; i < fontePoro2D_json.Size(); i++) {
        string indiceFonte = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!fontePoro2D_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceFonte + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (fontePoro2D_json[i].HasMember("ativo")) {
            if (!fontePoro2D_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo id (opcional)
        if (fontePoro2D_json[i].HasMember("id")) {
            if (!fontePoro2D_json[i]["id"].IsInt()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/id: deve ser do tipo inteiro");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (fontePoro2D_json[i].HasMember("comprimentoMedido")) {
            if (!fontePoro2D_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar campo arquivo (opcional)
        if (fontePoro2D_json[i].HasMember("arquivo")) {
            if (!fontePoro2D_json[i]["arquivo"].IsString()) {
                sucesso = false;
                erros.push_back(indiceFonte + "/arquivo: deve ser do tipo string");
            }
        }
    }
}

void validadorTipo::valida_perfil_producao(Value &perfilProducao_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/perfilProducao");

    // Validar se perfilProducao é um objeto
    if (!perfilProducao_json.IsObject()) {
        sucesso = false;
        erros.push_back("perfilProducao: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (perfilProducao_json.HasMember("ativo")) {
        if (!perfilProducao_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("perfilProducao/ativo: deve ser do tipo booleano");
        }
    }

    // Validar array tempo (opcional)
    if (perfilProducao_json.HasMember("tempo")) {
        if (!perfilProducao_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("perfilProducao/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < perfilProducao_json["tempo"].Size(); i++) {
                if (!perfilProducao_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("perfilProducao/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Lista de todos os campos booleanos do perfilProducao
    std::vector<std::string> camposBooleanos = {
        "pressao", "temperatura", "holdup", "FVH", "bet", "ugs", "uls", "ug", "ul", "arra",
        "viscosidadeLiquido", "viscosidadeGas", "rhog", "rhol", "rhoo", "rhoa", "rhoMix", "RS",
        "vazaoMassicaGas", "vazaoMassicaLiquido", "c0", "ud", "RGO", "deng", "yco2",
        "calor", "masstrans", "cpgas", "cpliq", "QLstd", "QLWstd", "QLstdTotal", "QGstd",
        "api", "bsw", "hidro", "fric", "Term1", "Term2", "dengD", "dengL",
        "ReyInterno", "ReyExterno", "GrashInterno", "GrashExterno",
        "NusselInterno", "NusselExterno", "PrandtlInterno", "PrandtlExterno",
        "Froud", "Rs", "Bo", "Hint", "Hext", "temperaturaAmbiente",
        "PrandtlGas", "PrandtlLiquido", "pseudoLiquido", "pseudoGas", "pseudoMist",
        "TResi", "RedutorAtrito", "angulo", "diametroInterno", "tempParede",
        "subResfria", "dadosParafina", "correlacaoBB"};

    // Validar cada campo booleano
    for (const auto &campo : camposBooleanos) {
        if (perfilProducao_json.HasMember(campo.c_str())) {
            if (!perfilProducao_json[campo.c_str()].IsBool()) {
                sucesso = false;
                erros.push_back("perfilProducao/" + campo + ": deve ser do tipo booleano");
            }
        }
    }
}

void validadorTipo::valida_perfil_servico(Value &perfilServico_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/perfilServico");

    // Validar se perfilServico é um objeto
    if (!perfilServico_json.IsObject()) {
        sucesso = false;
        erros.push_back("perfilServico: deve ser do tipo objeto");
        return;
    }

    // Validar campo ativo (opcional)
    if (perfilServico_json.HasMember("ativo")) {
        if (!perfilServico_json["ativo"].IsBool()) {
            sucesso = false;
            erros.push_back("perfilServico/ativo: deve ser do tipo booleano");
        }
    }

    // Validar array tempo (opcional)
    if (perfilServico_json.HasMember("tempo")) {
        if (!perfilServico_json["tempo"].IsArray()) {
            sucesso = false;
            erros.push_back("perfilServico/tempo: deve ser do tipo array");
        } else {
            for (SizeType i = 0; i < perfilServico_json["tempo"].Size(); i++) {
                if (!perfilServico_json["tempo"][i].IsNumber()) {
                    sucesso = false;
                    erros.push_back("perfilServico/tempo[" + std::to_string(i) + "]: deve ser do tipo number");
                }
            }
        }
    }

    // Lista de todos os campos booleanos do perfilServico
    std::vector<std::string> camposBooleanos = {
        "pressao", "temperatura", "ugs", "ug", "tensaoCisalhamento", "viscosidadeGas",
        "rhog", "vazaoMassicaGas", "QGstd", "fric", "calor", "hidro", "FVHG",
        "ReyInterno", "ReyExterno", "GrashInterno", "GrashExterno",
        "NusselInterno", "NusselExterno", "PrandtlInterno", "PrandtlExterno",
        "Hint", "Hext", "temperaturaAmbiente", "angulo", "diametroInterno",
        "tempParede", "subResfria"};

    // Validar cada campo booleano
    for (const auto &campo : camposBooleanos) {
        if (perfilServico_json.HasMember(campo.c_str())) {
            if (!perfilServico_json[campo.c_str()].IsBool()) {
                sucesso = false;
                erros.push_back("perfilServico/" + campo + ": deve ser do tipo booleano");
            }
        }
    }
}

void validadorTipo::valida_tendencia_producao(Value &tendP_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/tendP");

    // Validar se tendP é um array
    if (!tendP_json.IsArray()) {
        sucesso = false;
        erros.push_back("tendP: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array tendP
    for (SizeType i = 0; i < tendP_json.Size(); i++) {
        string indiceTend = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!tendP_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceTend + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (tendP_json[i].HasMember("ativo")) {
            if (!tendP_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceTend + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (tendP_json[i].HasMember("comprimentoMedido")) {
            if (!tendP_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceTend + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar campo dt (opcional)
        if (tendP_json[i].HasMember("dt")) {
            if (!tendP_json[i]["dt"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceTend + "/dt: deve ser do tipo number");
            }
        }

        // Validar campo rotulo (opcional)
        if (tendP_json[i].HasMember("rotulo")) {
            if (!tendP_json[i]["rotulo"].IsString()) {
                sucesso = false;
                erros.push_back(indiceTend + "/rotulo: deve ser do tipo string");
            }
        }

        // Lista de todos os campos booleanos do tendP
        std::vector<std::string> camposBooleanos = {
            "pressao", "temperatura", "holdup", "FVH", "bet", "ugs", "uls", "ug", "ul", "arra",
            "viscosidadeLiquido", "viscosidadeGas", "rhog", "rhol", "rhoMix",
            "vazaoMassicaGas", "vazaoMassicaLiquido", "c0", "ud", "RGO", "deng", "yco2",
            "calor", "masstrans", "QLstd", "QLWstd", "QOstd", "QOWstd", "QLstdTotal", "QGstd",
            "api", "bsw", "hidro", "fric", "dengD", "dengL",
            "ReyInterno", "ReyExterno", "GrashInterno", "GrashExterno",
            "NusselInterno", "NusselExterno", "PrandtlInterno", "PrandtlExterno",
            "Froud", "Rs", "Bo", "Hint", "Hext",
            "mlFonte", "mgFonte", "mcFonte", "tempChokeJus", "deltaPBomba", "potenciaBomba",
            "volMonM1PT", "volJusM1PT", "volMonM1ST", "volJusM1ST",
            "autoVal", "autoVel", "flutuacao", "diametroInterno", "tempParede",
            "dadosParafina", "inventarioGas", "inventarioLiq", "subResfria"};

        // Validar cada campo booleano
        for (const auto &campo : camposBooleanos) {
            if (tendP_json[i].HasMember(campo.c_str())) {
                if (!tendP_json[i][campo.c_str()].IsBool()) {
                    sucesso = false;
                    erros.push_back(indiceTend + "/" + campo + ": deve ser do tipo booleano");
                }
            }
        }
    }
}

void validadorTipo::valida_tendencia_servico(Value &tendS_json, std::vector<std::string> &erros, bool &sucesso) {
    // Criar variável para o nome da propriedade json em processo de validação
    string chaveJson("#/tendS");

    // Validar se tendS é um array
    if (!tendS_json.IsArray()) {
        sucesso = false;
        erros.push_back("tendS: deve ser do tipo array");
        return;
    }

    // Percorrer cada item do array tendS
    for (SizeType i = 0; i < tendS_json.Size(); i++) {
        string indiceTend = chaveJson + "[" + std::to_string(i) + "]";

        // Validar se o item é um objeto
        if (!tendS_json[i].IsObject()) {
            sucesso = false;
            erros.push_back(indiceTend + ": deve ser do tipo objeto");
            continue;
        }

        // Validar campo ativo (opcional)
        if (tendS_json[i].HasMember("ativo")) {
            if (!tendS_json[i]["ativo"].IsBool()) {
                sucesso = false;
                erros.push_back(indiceTend + "/ativo: deve ser do tipo booleano");
            }
        }

        // Validar campo comprimentoMedido (opcional)
        if (tendS_json[i].HasMember("comprimentoMedido")) {
            if (!tendS_json[i]["comprimentoMedido"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceTend + "/comprimentoMedido: deve ser do tipo number");
            }
        }

        // Validar campo dt (opcional)
        if (tendS_json[i].HasMember("dt")) {
            if (!tendS_json[i]["dt"].IsNumber()) {
                sucesso = false;
                erros.push_back(indiceTend + "/dt: deve ser do tipo number");
            }
        }

        // Validar campo rotulo (opcional)
        if (tendS_json[i].HasMember("rotulo")) {
            if (!tendS_json[i]["rotulo"].IsString()) {
                sucesso = false;
                erros.push_back(indiceTend + "/rotulo: deve ser do tipo string");
            }
        }

        // Lista de todos os campos booleanos do tendS
        std::vector<std::string> camposBooleanos = {
            "pressao", "temperatura", "ugs", "ug", "tensaoCisalhamento", "viscosidadeGas",
            "rhog", "vazaoMassicaGas", "hidro", "fric", "FVHG", "calor", "QGstd",
            "presEstagVGL", "tempEstagVGL", "presGargVGL", "tempGargVGL", "vazaoVGL",
            "VelocidadeMaximaGarganta", "ReyInterno", "ReyExterno",
            "GrashInterno", "GrashExterno", "NusselInterno", "NusselExterno",
            "PrandtlInterno", "PrandtlExterno", "Froud", "Hint", "Hext",
            "diametroInterno", "tempParede", "subResfria"};

        // Validar cada campo booleano
        for (const auto &campo : camposBooleanos) {
            if (tendS_json[i].HasMember(campo.c_str())) {
                if (!tendS_json[i][campo.c_str()].IsBool()) {
                    sucesso = false;
                    erros.push_back(indiceTend + "/" + campo + ": deve ser do tipo booleano");
                }
            }
        }
    }
}

void validadorTipo::validaGeral(Document &jsonDoc) {
    std::vector<std::string> erros;
    bool sucesso = true;

    // Valida configuracaoInicial
    if (jsonDoc.HasMember("configuracaoInicial")) {
        valida_configuracao_inicial(jsonDoc["configuracaoInicial"], erros, sucesso);
    }

    // Valida tempo
    if (jsonDoc.HasMember("tempo")) {
        valida_tempo(jsonDoc["tempo"], erros, sucesso);
    }

    // Valida tabela
    if (jsonDoc.HasMember("tabela")) {
        valida_tabela(jsonDoc["tabela"], erros, sucesso);
    }

    // Valida material
    if (jsonDoc.HasMember("material")) {
        valida_materiais(jsonDoc["material"], erros, sucesso);
    }

    // Valida secaoTransversal
    if (jsonDoc.HasMember("secaoTransversal")) {
        valida_corte(jsonDoc["secaoTransversal"], erros, sucesso);
    }

    // Valida fluidosProducao
    if (jsonDoc.HasMember("fluidosProducao")) {
        valida_fluidos_producao(jsonDoc["fluidosProducao"], erros, sucesso);
    }

    // Valida fluidoComplementar
    if (jsonDoc.HasMember("fluidoComplementar")) {
        valida_fluido_complementar(jsonDoc["fluidoComplementar"], erros, sucesso);
    }

    // Valida fluidoGas
    if (jsonDoc.HasMember("fluidoGas")) {
        valida_fluido_gas(jsonDoc["fluidoGas"], erros, sucesso);
    }

    // Valida dutosProducao
    if (jsonDoc.HasMember("dutosProducao")) {
        valida_unidades_producao(jsonDoc["dutosProducao"], erros, sucesso);
    }

    // Valida dutosServico
    if (jsonDoc.HasMember("dutosServico")) {
        valida_unidades_servico(jsonDoc["dutosServico"], erros, sucesso);
    }

    // Valida CondicaoContPocInjec
    if (jsonDoc.HasMember("CondicaoContPocInjec")) {
        valida_condcont_pocinjec(jsonDoc["CondicaoContPocInjec"], erros, sucesso);
    }

    // Valida parafina
    if (jsonDoc.HasMember("parafina")) {
        valida_parafina(jsonDoc["parafina"], erros, sucesso);
    }

    // Valida hidrato
    if (jsonDoc.HasMember("hidrato")) {
        valida_hidrato(jsonDoc["hidrato"], erros, sucesso);
    }

    // Valida ipr
    if (jsonDoc.HasMember("ipr")) {
        valida_ipr(jsonDoc["ipr"], erros, sucesso);
    }

    // Valida gasInj
    if (jsonDoc.HasMember("gasInj")) {
        valida_gasInj(jsonDoc["gasInj"], erros, sucesso);
    }

    // Valida fonteGasLift
    if (jsonDoc.HasMember("fonteGasLift")) {
        valida_fonte_gaslift(jsonDoc["fonteGasLift"], erros, sucesso);
    }

    // Valida fonteGas
    if (jsonDoc.HasMember("fonteGas")) {
        valida_fonte_gas(jsonDoc["fonteGas"], erros, sucesso);
    }

    // Valida fonteLiquido
    if (jsonDoc.HasMember("fonteLiquido")) {
        valida_fonte_liquido(jsonDoc["fonteLiquido"], erros, sucesso);
    }

    // Valida fonteMassa
    if (jsonDoc.HasMember("fonteMassa")) {
        valida_fonte_massa(jsonDoc["fonteMassa"], erros, sucesso);
    }

    // Valida fontePressao (parse_furo)
    if (jsonDoc.HasMember("fontePressao")) {
        valida_furo(jsonDoc["fontePressao"], erros, sucesso);
    }

    // Valida bcs
    if (jsonDoc.HasMember("bcs")) {
        valida_bcs(jsonDoc["bcs"], erros, sucesso);
    }

    // Valida multibcs
    if (jsonDoc.HasMember("multibcs")) {
        valida_bcs(jsonDoc["multibcs"], erros, sucesso);
    }

    // Valida valvula
    if (jsonDoc.HasMember("valvula")) {
        valida_valv(jsonDoc["valvula"], erros, sucesso);
    }

    // Valida chokeSup
    if (jsonDoc.HasMember("chokeSup")) {
        valida_chokeSup(jsonDoc["chokeSup"], erros, sucesso);
    }

    // Valida master1
    if (jsonDoc.HasMember("master1")) {
        valida_master1(jsonDoc["master1"], erros, sucesso);
    }

    // Valida master2
    if (jsonDoc.HasMember("master2")) {
        valida_master2(jsonDoc["master2"], erros, sucesso);
    }

    // Valida separador
    if (jsonDoc.HasMember("separador")) {
        valida_separador(jsonDoc["separador"], erros, sucesso);
    }

    // Valida chokeInj
    if (jsonDoc.HasMember("chokeInj")) {
        valida_chokeInj(jsonDoc["chokeInj"], erros, sucesso);
    }

    // Valida deltaPressao
    if (jsonDoc.HasMember("deltaPressao")) {
        valida_delta_pressao(jsonDoc["deltaPressao"], erros, sucesso);
    }

    // Valida fonteCalor
    if (jsonDoc.HasMember("fonteCalor")) {
        valida_delta_pressao(jsonDoc["fonteCalor"], erros, sucesso);
    }

    // Valida bombaVolumetrica
    if (jsonDoc.HasMember("bombaVolumetrica")) {
        valida_bomba_volumetrica(jsonDoc["bombaVolumetrica"], erros, sucesso);
    }

    // Valida pig
    if (jsonDoc.HasMember("pig")) {
        valida_pig(jsonDoc["pig"], erros, sucesso);
    }

    // Valida correcao
    if (jsonDoc.HasMember("correcao")) {
        valida_correcao(jsonDoc["correcao"], erros, sucesso);
    }

    // Valida intermitenciaSevera
    if (jsonDoc.HasMember("intermitenciaSevera")) {
        valida_intermitencia(jsonDoc["intermitenciaSevera"], erros, sucesso);
    }

    // Valida fontePoroRadial
    if (jsonDoc.HasMember("fontePoroRadial")) {
        valida_fonte_PoroRadial(jsonDoc["fontePoroRadial"], erros, sucesso);
    }

    // Valida fontePoro2D
    if (jsonDoc.HasMember("fontePoro2D")) {
        valida_fonte_Poro2D(jsonDoc["fontePoro2D"], erros, sucesso);
    }

    // Valida perfilProducao
    if (jsonDoc.HasMember("perfilProducao")) {
        valida_perfil_producao(jsonDoc["perfilProducao"], erros, sucesso);
    }

    // Valida perfilServico
    if (jsonDoc.HasMember("perfilServico")) {
        valida_perfil_servico(jsonDoc["perfilServico"], erros, sucesso);
    }

    // Valida tendP
    if (jsonDoc.HasMember("tendP")) {
        valida_tendencia_producao(jsonDoc["tendP"], erros, sucesso);
    }

    // Valida tendS
    if (jsonDoc.HasMember("tendS")) {
        valida_tendencia_servico(jsonDoc["tendS"], erros, sucesso);
    }

    // Se houver erros, registra no logger e encerra
    if (!sucesso) {
        std::string mensagem;
        int nele = erros.size();
        for (int i = 0; i < nele; i++)
            logger.log_write_logs_and_exit(LOGGER_FALHA, LOG_ERR_UNEXPECTED_EXCEPTION, "validacao de tipo " + erros[i], "jsonDoc", "erro de tipo");
    }
}
