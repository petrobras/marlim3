#include "FA_Hidratos.h"
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

std::string datahora_atual() {
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);

    std::ostringstream ss;
    ss << "datahora = "
       << std::setfill('0')
       << std::setw(2) << now->tm_mday << "/"
       << std::setw(2) << now->tm_mon + 1 << "/"
       << (now->tm_year + 1900) << " "
       << std::setw(2) << now->tm_hour << ":"
       << std::setw(2) << now->tm_min << ":"
       << std::setw(2) << now->tm_sec;

    return ss.str();
}

constexpr double G_MOL_TO_KG_MOL = 1.0 / 1000.0;

FA_Hidrato::FA_Hidrato(const SProd &sistemaRef) : sistema(sistemaRef) {

    carregarCurvaHidrato("curva_base_hidrato.txt");

    for (size_t i = 0; i < temperaturaCurva.size(); ++i) {
    }

    K_Hamm_Etanol = 1297;   // sistema.arq.Khamm_Etanol;  //K_Etanol=1297;
    K_Hamm_MEG = 1500;      // sistema.arq.Khamm_MEG; //K_MEG=1500;
    MM_H = sistema.arq.MMH; //* G_MOL_TO_KG_MOL; tem de ficar em g/mol mesmo
    MM_G = sistema.arq.MMG; //* G_MOL_TO_KG_MOL; tem de ficar em g/mol mesmo
    MM_W = sistema.arq.MMW; //* G_MOL_TO_KG_MOL; tem de ficar em g/mol mesmo
    W_Hamm = sistema.arq.Whamm;
    M_Etanol = 46.07; // sistema.arq.Methanol;
    M_MEG = 62.1;     // sistema.arq.MMEG;

    coefEsteq = sistema.arq.coefEsteq;
    inibidor = sistema.arq.inibidor;
    estruturaHidratos = sistema.arq.estruturaHidratos;
    r_d = sistema.arq.rd;
    r_p = sistema.arq.rp;

    double delta_T = 0.0;
    if (inibidor == "Etanol") {
        delta_T = (K_Hamm_Etanol * W_Hamm) / (MM_W * (1.0 - W_Hamm));
        std::cout << "[HAMMERSCHMIDT] delta_T [oC] = " << delta_T << "\n";
    } else if (inibidor == "MEG") {
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
        pressaoCurvaDeslocada.push_back(pressaoCurva[i]); // teste para forçar formação de hidrato
    }

    salvarCurvaDeslocada("curva_hidrato_deslocada.txt");

    for (size_t i = 0; i < temperaturaCurvaDeslocada.size(); ++i) {
    }
}

FA_Hidrato::~FA_Hidrato() {
}

void FA_Hidrato::carregarCurvaHidrato(const std::string &nomeArquivo) {
}

void FA_Hidrato::salvarCurvaDeslocada(const std::string &nomeArquivo) {
}

std::tuple<std::vector<double>, std::vector<double>> FA_Hidrato::gerarCurvaComInibidor(const std::vector<double> &tempBase,
                                                                                       const std::vector<double> &pressBase,
                                                                                       double K, double M, double w) {
    return {};
}

bool FA_Hidrato::checkHidrato(double P_atual, double T_atual) {
    return false;
}

double FA_Hidrato::interpolar(double x,
                              const std::vector<double> &xData,
                              const std::vector<double> &yData) {
    return -100000000.;
}

double FA_Hidrato::TurnerHidrato(double P, double T, double P_eq, double T_eq,
                                 double A_s, double V_h, double V_w,
                                 double r_d, double r_p, const std::string &estruturaHidratos, double A_s_input, double &A_s_eff_out) {

    return -100000000.; // kg/s
}

//*** Euler assume a mesma Massa Molar de Hidrato, pois está é praticamente igual entre as estruturas sI (119.5g/mol)
// e sII (117.9g/mol)
double FA_Hidrato::Euler1ordemHidrato(double j_H, double taxaCinetica,
                                      double A, double eta, double MM_h, double rho_h) {
    return -100000000.;
}

double FA_Hidrato::Euler1ordemGas(int i, double j_G, double taxaCinetica,
                                  double A, double MM_g) {

    return -100000000.;
}

double FA_Hidrato::Euler1ordemAgua(int i, double j_W, double taxaCinetica,
                                   double A, double eta, double MM_w) {
    return -100000000.;
}

void FA_Hidrato::solverHidrato() {
}
