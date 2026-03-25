// ---

#ifndef _FA_HIDRATO
#define _FA_HIDRATO

#include <vector>
#include <string>
#include <tuple>
#include "SisProd.h"


class FA_Hidrato {
public:
    FA_Hidrato(const SProd& sistema);
    ~FA_Hidrato();

    //std::vector<long double> V_w;
    //long double V_w;

    void solverHidrato();

    bool checkHidrato(long double P, long double T);

    long double TurnerHidrato(long double P, long double T, long double P_eq, long double T_eq,
                              long double A_s, long double V_h, long double V_w,
                              long double r_d, long double r_p,
                              const std::string& estruturaHidratos,
                              long double A_s_input,
                              long double &A_s_eff_out);

    long double Euler1ordemGas(int i, long double j_G, long double taxaCinetica,
                               long double A, long double MM_g);

    long double Euler1ordemAgua(int i, long double j_W, long double taxaCinetica,
                                long double A, long double eta, long double MM_w);

    long double Euler1ordemHidrato(long double j_H, long double taxaCinetica,
                                   long double A, long double eta, long double MM_h, long double rho_h);

private:
    const SProd& sistema;

    std::vector<long double> temperaturaCurva;
    std::vector<long double> pressaoCurva;
    std::vector<long double> temperaturaCurvaDeslocada;
    std::vector<long double> pressaoCurvaDeslocada;

    long double K_Hamm_Etanol, K_Hamm_MEG;
    long double MM_H, MM_G, MM_W, W_Hamm;
    long double M_Etanol, M_MEG; //não é mais empregada, nem como input de entrada
    long double coefEsteq;
    long double rhoH;
    long double r_d, r_p; //k1_sI, k2_sI, k1_sII, k2_sII,
    string inibidor; //chris - Hidratos
    string estruturaHidratos;
    //long double V_w;
    //std::vector<long double> Vw;

    void carregarCurvaHidrato(const std::string& nomeArquivo);
    void salvarCurvaDeslocada(const std::string& nomeArquivo);

    //bool checkHidrato(long double P_atual, long double T_atual);
    long double interpolar(long double x, const std::vector<long double>& xData, const std::vector<long double>& yData);


    std::tuple<std::vector<long double>, std::vector<long double>> gerarCurvaComInibidor(
        const std::vector<long double>& tempBase,
        const std::vector<long double>& pressBase,
        long double K, long double M, long double w);
};

#endif

