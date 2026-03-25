
//---

#include "FA_Hidratos_Servico.h"
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>

            std::string datahora_atualG() {
                std::time_t t = std::time(nullptr);
                std::tm* now = std::localtime(&t);

                std::ostringstream ss;
                ss << "datahora = "
                   << std::setfill('0')
                   << std::setw(2) << now->tm_mday << "/"
                   << std::setw(2) << now->tm_mon + 1 << "/"
                   << (now->tm_year + 1900) << " "
                   << std::setw(2) << now->tm_hour << ":"
                   << std::setw(2) << now->tm_min  << ":"
                   << std::setw(2) << now->tm_sec;

                return ss.str();
            }

//std::vector<long double> FVH_global; //chris - hidratos
constexpr long double G_MOL_TO_KG_MOL = 1.0 / 1000.0; //não precisa mais --> Chris: desde que o user ente com g/mol.

FA_Hidrato_Servico::FA_Hidrato_Servico(const SProd& sistemaRef) : sistema(sistemaRef) {


   // std::cout << "Inicializando FA_Hidrato_Servico com os dados do sistema de producao." << std::endl;
    carregarCurvaHidratoG("curva_base_hidrato_servico.txt");

    //std::cout << "\n--- Curva Base de Hidrato (Tbase [°C] vs Pbase [kgf/cm²]) ---\n";
    for (size_t i = 0; i < temperaturaCurva.size(); ++i) {
        //std::cout << temperaturaCurva[i] - 273.15 << "\t" << pressaoCurva[i] << "\n";
      //  std::cout << temperaturaCurva[i] << "\t" << pressaoCurva[i] << "\n";
    }

   // V_w.assign(sistema.ncel, 0.0L);   // cria vetor com um Vw por célula
    K_Hamm_Etanol = 1297; //sistema.arq.Khamm_Etanol;  //K_Etanol=1297;
    K_Hamm_MEG = 1500; //sistema.arq.Khamm_MEG; //K_MEG=1500;
    //MM_H = sistema.arq.MMH * G_MOL_TO_KG_MOL;
    //MM_G = sistema.arq.MMG * G_MOL_TO_KG_MOL;
    //MM_W = sistema.arq.MMW * G_MOL_TO_KG_MOL;
    MM_H = sistema.arq.MMH;  //* G_MOL_TO_KG_MOL; tem de ficar em g/mol mesmo
    MM_G = sistema.arq.MMG;  //* G_MOL_TO_KG_MOL; tem de ficar em g/mol mesmo
    MM_W = sistema.arq.MMW;  //* G_MOL_TO_KG_MOL; tem de ficar em g/mol mesmo
    W_Hamm = sistema.arq.Whamm;
    M_Etanol = 46.07;    //sistema.arq.Methanol; //chris - Hidratos - não é mais usado, nem como entrada
    M_MEG = 62.1;     //sistema.arq.MMEG; //chris - Hidratos - não é mais usado, nem como entrada

    //*** Linha de Servico --> chris - Hidratos
    fracFWcarregada =sistema.arq.fracFWcarregada;

    //rhoH = sistema.arq.rhoH; //agora é definido pelas estruturas sI ou sII
    long double rhoH_sI  = 930.0L;   // kg/m³
    long double rhoH_sII = 910.0L;   // kg/m³
    long double rhoW     = 1000.0L;  // kg/m³

    coefEsteq = sistema.arq.coefEsteq;

    /*k1_sI = sistema.arq.estruturaHidratosIk1;
    k2_sI = sistema.arq.estruturaHidratosIk2;
    k1_sII = sistema.arq.estruturaHidratosIIk1;
    k2_sII = sistema.arq.estruturaHidratosIIk2;*/
    inibidor=sistema.arq.inibidor;
    estruturaHidratos = sistema.arq.estruturaHidratos;
    r_d = sistema.arq.rd;
    r_p = sistema.arq.rp;

   /* std::cout << "[INFO] Aplicando a equacao de Hammerschmidt:\n";
    std::cout << "       T [oC] = (K * w) / (M * (1 - w))\n";
    std::cout << "       Onde:\n";
    std::cout << "            K = " << K_Hamm << "\n";
    std::cout << "            M = " << MM_W << " g/mol\n";   //" kg/mol\n";
    std::cout << "            w = " << W_Hamm << "\n"; */

    long double delta_T;
    if (inibidor=="Etanol") {
    delta_T = (K_Hamm_Etanol * W_Hamm) / (MM_W * (1.0 - W_Hamm));
    std::cout << "[HAMMERSCHMIDT] delta_T [oC] = " << delta_T << "\n";
    } else if (inibidor=="MEG") {
       delta_T = (K_Hamm_MEG * W_Hamm) / (MM_W * (1.0 - W_Hamm));
        std::cout << "[HAMMERSCHMIDT] delta_T [oC] = " << delta_T << "\n";
    } else {
    	cout << "O inibidor a ser aplicado é Etanol ou MEG" << endl;
    	system("exit");
    }

    temperaturaCurvaDeslocada.clear();
    pressaoCurvaDeslocada.clear();
    for (size_t i = 0; i < temperaturaCurva.size(); ++i) {
        temperaturaCurvaDeslocada.push_back(temperaturaCurva[i] - delta_T);
        //temperaturaCurvaDeslocada.push_back(temperaturaCurva[i] +10); //teste para forçar formação de hidrato
        //pressaoCurvaDeslocada.push_back(pressaoCurva[i]);
        pressaoCurvaDeslocada.push_back(pressaoCurva[i]); //teste para forçar formação de hidrato
    }

    salvarCurvaDeslocadaG("curva_hidrato_deslocada_servico.txt");

   // std::cout << "\n--- Curva Deslocada com Inibidor (T [°C] vs P [kgf/cm²]) ---\n";
    for (size_t i = 0; i < temperaturaCurvaDeslocada.size(); ++i) {
        //std::cout << temperaturaCurvaDeslocada[i] - 273.15 << "\t" << pressaoCurvaDeslocada[i] << "\n";
      //  std::cout << temperaturaCurvaDeslocada[i] << "\t" << pressaoCurvaDeslocada[i] << "\n";
    }
}

FA_Hidrato_Servico::~FA_Hidrato_Servico() {
}

void FA_Hidrato_Servico::carregarCurvaHidratoG(const std::string& nomeArquivo) {
}



void FA_Hidrato_Servico::salvarCurvaDeslocadaG(const std::string& nomeArquivo) {
}

std::tuple<std::vector<long double>, std::vector<long double>> FA_Hidrato_Servico::gerarCurvaComInibidorG(const std::vector<long double>& tempBase,
	    const std::vector<long double>& pressBase,
	    long double K, long double M, long double w) {
}


bool FA_Hidrato_Servico::checkHidratoG(long double P_atual, long double T_atual) {

    return false;
}

long double FA_Hidrato_Servico::interpolarG(long double x,
                                   const std::vector<long double>& xData,
                                   const std::vector<long double>& yData) {
return -100000000.;
}

long double FA_Hidrato_Servico::TurnerHidratoG(long double P, long double T, long double P_eq, long double T_eq,
        long double A_s, long double V_h, long double V_w,
        long double r_d, long double r_p, const std::string& estruturaHidratos,   long double A_s_input, long double &A_s_eff_out) {


    return -100000000.;
}

long double FA_Hidrato_Servico::Euler1ordemHidratoG(long double j_H, long double taxaCinetica,
                                           long double A, long double eta, long double MM_h, long double rho_h) {

	 return -100000000.;
}

long double FA_Hidrato_Servico::Euler1ordemGasG(int i, long double j_G, long double taxaCinetica,
                                       long double A, long double MM_g) {

    return -100000000.;
}

long double FA_Hidrato_Servico::Euler1ordemAguaG(int i, long double j_W, long double taxaCinetica,
                                        long double A, long double eta, long double MM_w) {
    return -100000000.;
}


void FA_Hidrato_Servico::solverHidratoG() {

}
