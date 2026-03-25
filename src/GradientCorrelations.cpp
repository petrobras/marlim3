//Petroleo Brasileiro S.A.
#include "GradientCorrelations.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstring>
#include <algorithm>
#include <vector>
#include <stdexcept>

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================

/*
==============================================================================
frictionFactor: Calcula o fator de atrito monofásico de Moody usando a equação de Colebrook,
                em regime turbulento ou laminar.

Dicionário de variáveis:
- reynolds   : Número de Reynolds (adimensional). Entrada.
- roughness : Rugosidade relativa (adimensional). Entrada.
- frictionFactorValue  : Fator de atrito de Moody (adimensional). Saída (por referência).
- d    : Rugosidade relativa normalizada pelo fator 3.7 (roughness / 3.7)
- rd   : Produto de Reynolds e d (reynolds * d)
- lgd  : Logaritmo decimal de d (log10(d))
- a    : Termo auxiliar (log10(1.0 + 12.0 / rd))
- b    : Termo auxiliar  (log10(1.0 - 5.02 * (a + lgd) / rd))
- c    : Termo auxiliar  (log10(1.0 - 5.02 * (b + lgd) / rd))
==============================================================================
*/
void frictionFactor(double reynolds, double roughness, double& frictionFactorValue) {
    double rd, lgd, a, b, c, d;
    if (reynolds <= 2100.0) {
        frictionFactorValue = 64.0 / reynolds;
    } else {
        d   = roughness / 3.7;
        rd  = reynolds * d;
        lgd = log10(d);
        a   = log10(1.0 + 12.0 / rd);
        b   = log10(1.0 - 5.02 * (a + lgd) / rd);
        c   = log10(1.0 - 5.02 * (b + lgd) / rd);
        if ((a - b) > 1.0E-06) {
            frictionFactorValue = 0.25 / pow(pow((a - b), 2) / (a - 2.0 * b + c) - a - lgd, 2);
        } else {
            frictionFactorValue = 0.25 / pow(a + lgd, 2);
        } 
    }
}

/*
==============================================================================
interpolate: Interpolação linear ou quadrática usando busca binária

Dicionário de variáveis:
- xValues : Vetor com os valores da variável independente (X), tamanho numPoints
- yValues : Vetor com os valores da variável dependente (Y), tamanho numPoints
- xTarget : Valor de X para o qual se deseja interpolar Y
- degree  : Grau da interpolação (1 = linear, 2 = quadrática)
- numPoints : Número de pontos em xValues e yValues
- minIndex : Índice inferior do intervalo de busca
- maxIndex : Índice superior do intervalo de busca
- midIndex : Índice intermediário usado na busca binária
- a, b, c : Coeficientes do polinômio de interpolação
==============================================================================
*/
long double interpolate(const std::vector<double>& xValues, const std::vector<double>& yValues,
                   double xTarget, int degree, int numPoints) {
    // Checagens de tamanho
    if (numPoints < 2) throw std::invalid_argument("numPoints deve ser >= 2");
    if (xValues.size() < numPoints || yValues.size() < numPoints) 
        throw std::invalid_argument("Vetor menor que numPoints");
    
    // Fora do range: retorna o extremo correspondente
    if (xTarget <= xValues[0])
        return yValues[0];
    if (xTarget >= xValues[numPoints-1])
        return yValues[numPoints-1];
    
    // Busca binária do intervalo
    int minIndex = 0;                   // Índice inferior
    int maxIndex = numPoints - 1;       // Índice superior
    int midIndex;                       // Índice intermediário
    while ((maxIndex - minIndex) > 1) {
        midIndex = (minIndex + maxIndex) / 2;
        if (xTarget <= xValues[midIndex])
            maxIndex = midIndex;
        else
            minIndex = midIndex;
    }
    
    // Interpolação
    if (degree == 1) { // Linear
        // Cálculo dos coeficientes b e c para interpolação linear
        double b = (yValues[maxIndex] - yValues[minIndex]) / 
                   (xValues[maxIndex] - xValues[minIndex]);
        double c = yValues[minIndex] - b * xValues[minIndex];
        return b * xTarget + c;
    } else { // Quadrática
        if (minIndex == 0) maxIndex++;
        if (maxIndex > numPoints-1) maxIndex = numPoints-1;
        if (maxIndex < 2) throw std::out_of_range("Não há pontos suficientes para interpolação quadrática");
        
        // Cálculo dos coeficientes a, b e c para interpolação quadrática
        double a = ((yValues[maxIndex] - yValues[maxIndex-1])/(xValues[maxIndex] - xValues[maxIndex-1]) - 
                    (yValues[maxIndex-1] - yValues[maxIndex-2])/(xValues[maxIndex-1] - xValues[maxIndex-2])) / 
                   (xValues[maxIndex] - xValues[maxIndex-2]);
        double b = (yValues[maxIndex-1] - yValues[maxIndex-2])/(xValues[maxIndex-1] - xValues[maxIndex-2]) - 
                   a * (xValues[maxIndex-1] + xValues[maxIndex-2]);
        double c = yValues[maxIndex-1] - a * pow(xValues[maxIndex-1], 2) - b * xValues[maxIndex-1];
        return a * pow(xTarget, 2) + b * xTarget + c;
    }
}

/*
==============================================================================
interpolate2D: Interpolação bidimensional (2D) em matriz de valores, utilizando
               interpolação 1D (linear ou quadrática) nas linhas e depois nas colunas.

Dicionário de variáveis:
- verticalPoints   : Vetor vertical (primeira dimensão, tamanho numVertical)
- horizontalPoints : Vetor horizontal (segunda dimensão, tamanho numHorizontal)
- matrixValues     : Matriz de valores (flattened), tamanho numVertical*numHorizontal, 
                     organizada por linha (matrixValues[j*numVertical + i] corresponde 
                     ao valor na linha i, coluna j)
- numVertical      : Número de elementos na dimensão vertical (linhas)
- numHorizontal    : Número de elementos na dimensão horizontal (colunas)
- verticalDegree   : Grau de interpolação na vertical (1 = linear, 2 = quadrática)
- horizontalDegree : Grau de interpolação na horizontal (1 = linear, 2 = quadrática)
- verticalTarget   : Valor para interpolação na dimensão vertical (linha)
- horizontalTarget : Valor para interpolação na dimensão horizontal (coluna)
- columnData       : Vetor auxiliar para armazenar linha de matrixValues
- interpolatedRow  : Vetor auxiliar para armazenar valores interpolados nas linhas
==============================================================================
*/
long double interpolate2D(const std::vector<double>& verticalPoints,
                     const std::vector<double>& horizontalPoints, 
                     const std::vector<double>& matrixValues,
                     int numVertical, int numHorizontal, 
                     int verticalDegree, int horizontalDegree, 
                     double verticalTarget, double horizontalTarget) {
    if (verticalPoints.size() < numVertical || horizontalPoints.size() < numHorizontal || 
        matrixValues.size() < numVertical * numHorizontal)
        throw std::invalid_argument("Tamanhos dos vetores incompatíveis!");
    
    std::vector<double> interpolatedRow(numHorizontal); // Armazena valores interpolados das linhas
    
    // Para cada coluna (horizontal), interpola nas linhas (vertical)
    for (int j = 0; j < numHorizontal; ++j) {
        std::vector<double> columnData(numVertical); // Armazena a linha da matriz correspondente à coluna j
        for (int i = 0; i < numVertical; ++i) {
            int k = j * numVertical + i; // matrixValues organizado por linhas
            columnData[i] = matrixValues[k];
        }
        // Interpola na dimensão vertical para verticalTarget
        interpolatedRow[j] = interpolate(verticalPoints, columnData, verticalTarget, verticalDegree, numVertical);
    }
    
    // Agora interpola o vetor interpolatedRow na dimensão horizontal para horizontalTarget
    return interpolate(horizontalPoints, interpolatedRow, horizontalTarget, horizontalDegree, numHorizontal);
}

/*
==============================================================================
accelerateConvergence: Método para acelerar convergência em processos iterativos.
                      

Dicionário de variáveis:
- currentValue    : Valor corrente, atualizado na função
- previousValue   : Valor anterior, transferido à função
- convergeStatus  : Indicador de status de convergência
                    0 = início; 1 = iterando; 2 = convergiu
- tolerance       : Tolerância de convergência
- auxValueA       : Valor auxiliar para Wegstein 
- auxValueB       : Valor auxiliar para Wegstein 
- parameter       : Parâmetro auxiliar para cálculo
- tempValue       : Valor temporário para cálculo
==============================================================================
*/
void accelerateConvergence(double& currentValue, double& previousValue, unsigned char& convergeStatus, 
                          double tolerance, double& auxValueA, double& auxValueB) {
    double parameter, tempValue;
    if (convergeStatus == 0) {
        auxValueA = currentValue;
        auxValueB = previousValue;
        currentValue = previousValue;
        convergeStatus = 1;
    } else if (std::abs(currentValue - previousValue) < tolerance) {
        currentValue = previousValue;
        convergeStatus = 2;
    } else {
        parameter = auxValueA - currentValue + previousValue - auxValueB;
        if (std::abs(parameter) < 1.0E-10) {
            currentValue = (currentValue + auxValueB) / 2.0;
        } else {
            tempValue = (auxValueA * previousValue - auxValueB * currentValue) / parameter;
            auxValueA = currentValue;
            auxValueB = previousValue;
            currentValue = tempValue;
        }
    }
}


// ==============================================================================
// FUNÇÃO PARA EXECUÇÃO DE CORRELAÇÕES
// ==============================================================================

/*
==============================================================================
executarCorrelacao: Função que extrai parâmetros da estrutura celula
                    e executa uma correlação de escoamento especificada por número.

Parâmetros:
- celula              : Ponteiro para array de células
- i                   : Índice da célula atual
- inter               : Flag indicando interface (1) ou não (0)
- AceleraConvergPerm  : Flag de aceleração de convergência
- correlacao          : Número da correlação a ser executada
                        0 = Poettmann-Carpenter
                        1 = Baxendell-Thomas
                        2 = Fancher-Brown
                        3 = Hagedorn-Brown
                        4 = Duns-Ros
                        5 = Orkiszewski
                        6 = Beggs & Brill
                        7 = Mukherjee-Brill
                        8 = Aziz
                        9 = Gray
                        10 = Oliemans
                        11 = Dukler
                        12 = Beggs & Brill com correção de Palmer
                        13 = Dukler, Eaton e Flanigan 
                        14 = Dukler e Minami I
                        15 = Dukler e Minami II

==============================================================================
*/
void executarCorrelacao(Cel* celula, int i, int inter, int AceleraConvergPerm,
                       int correlacao,
                       double& holdup, double& frictionGrad, double& gravityGrad, double& totalGrad,
                       double& reynolds, unsigned char& flowType) {
    
    
    long double area;
    long double diameter;
    long double roughness;
    long double alfmed;
    long double betmed;
    long double rhog;
    long double rhol;
    long double ugsmed;
    long double ulsmed;
    long double tmed;
    long double pmed;
    long double j;
    long double rhomix;
    long double gasViscosity;
    long double liquidViscosity;
    long double viscmix;
    long double angle;
    long double velocity;
    long double temperature;  // Temperatura em °F
	long double compressibilityFactor;
	long double surfaceTension;
	long double productionRate;
	long double waterFraction;
	long double oilSurfaceTension;
	long double waterSurfaceTension;

    
    
    if(inter == 1) {
        angle = celula[i].dutoL.teta * 180 / M_PI;
    } else {
        angle = celula[i].duto.teta * 180 / M_PI;
    }
    
   
    if(inter == 1) {
        // Interface
        area = celula[i].dutoL.area;
        diameter = celula[i].dutoL.dia * 100.0 / (2.54*12);
        roughness = celula[i].dutoL.rug / celula[i].dutoL.dia;
        alfmed = celula[i - 1].alf;
        betmed = celula[i - 1].bet;
        tmed = celula[i - 1].temp;
        pmed = celula[i - 1].pres;

        compressibilityFactor=celula[i - 1].flui.Zdran(pmed, tmed);
        surfaceTension=celula[i - 1].flui.TensSuper(pmed, tmed)*1000.;
        productionRate=celula[i - 1].flui.IRGO;
        waterFraction=celula[i - 1].flui.BSW;
        oilSurfaceTension=celula[i - 1].flui.TensSuperOleo(pmed, tmed)*1000.;
        waterSurfaceTension=celula[i - 1].flui.TensSuperAgua(pmed, tmed)*1000.;

        rhog = celula[i - 1].flui.MasEspGas(pmed, tmed);
        rhol = (1.0 - betmed) * celula[i - 1].flui.MasEspLiq(pmed, tmed)
             + betmed * celula[i - 1].fluicol.MasEspFlu(celula[i - 1].pres, celula[i - 1].temp);
        liquidViscosity = ((1.0 - betmed) * celula[i - 1].flui.ViscOleo(pmed, tmed) 
                         + betmed * celula[i - 1].fluicol.VisFlu(pmed, tmed));
        gasViscosity = celula[i].flui.ViscGas(pmed, tmed);
        ugsmed = (celula[i - 1].MC - celula[i - 1].Mliqini) / (area * rhog);
        ulsmed = celula[i - 1].Mliqini / (area * rhol);
        j = ugsmed + ulsmed;
        velocity = 3.28084 * (ugsmed + ulsmed);  
        temperature = tmed * 9.0/5.0 + 32.0;     // Conversão °C para °F
    } else {
        // Não interface
        area = celula[i].duto.area;
        diameter = celula[i].duto.dia * 100.0 / (2.54*12);
        roughness = celula[i].duto.rug / celula[i].duto.dia;
        if(inter!=0){
        	alfmed = celula[i].alf;
        	betmed = celula[i].bet;
        }
        else{
           	alfmed = celula[i-1].alf;
            betmed = celula[i-1].bet;
        }
        long double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
        
        if(AceleraConvergPerm == 0 && inter!=0) {
            tmed = razdx * celula[i].temp + (1.0 - razdx) * celula[i - 1].temp;
        } else {
            tmed = celula[i - 1].temp;
        }
        
        pmed = celula[i].presaux + celula[i - 1].dpB / 98066.5;

        celula[i].flui.PcTcIS();
        compressibilityFactor=celula[i].flui.Zdran(pmed, tmed);
        surfaceTension=celula[i].flui.TensSuper(pmed, tmed)*1000.;
        productionRate=celula[i - 1].flui.IRGO;
        waterFraction=celula[i].flui.BSW;
        oilSurfaceTension=celula[i - 1].flui.TensSuperOleo(pmed, tmed)*1000.;
        waterSurfaceTension=celula[i - 1].flui.TensSuperAgua(pmed, tmed)*1000.;

        rhog = celula[i].rgCi;
        rhol = (1.0 - betmed) * celula[i].rpCi + betmed * celula[i].rcCi;
        ugsmed = celula[i].QG / area;
        ulsmed = celula[i].QL / area;
        j = ugsmed + ulsmed;
        velocity = 3.28084 * (ugsmed + ulsmed);  
        
        long double sinalJ = 1.0;
        if(fabsl(j) > 1e-15) {
            sinalJ = j / fabsl(j);
        }
        
        rhomix = alfmed * rhog + (1.0 - alfmed) * rhol;
        liquidViscosity = ((1.0 - betmed) * celula[i].flui.ViscOleo(pmed, tmed) 
                         + betmed * celula[i].fluicol.VisFlu(pmed, tmed));
        gasViscosity = celula[i].flui.ViscGas(pmed, tmed);
        viscmix = alfmed * celula[i].flui.ViscGas(pmed, tmed) + (1.0 - alfmed) * liquidViscosity;
        temperature = tmed * 9.0/5.0 + 32.0;     // Conversão °C para °F
    }
    
    
    pmed*=(0.9678411)*14.69595;
    long double liquidFraction;
    if(fabsl(j)>1e-15)liquidFraction= fabsl(ulsmed / j);
    else liquidFraction=1.-celula[i-1].alf;
    long double gasDensity = 0.06243 * rhog;      
    long double liquidDensity = 0.06243 * rhol;   
    
    // Variáveis adicionais para correlações que retornam mais parâmetros
    double accelGrad = 0.0;
    unsigned char criticalFlag = 0;
    unsigned char convergeFlag = 0;
    unsigned char flowPattern = 0;
    unsigned char palmerFlag = 0;
    unsigned char correlationFlag = 0;
    unsigned char transitionFlag = 0;
    
    // Inicializa holdup
    holdup = 0.0;
    
    
    if (correlacao == 0) {
        // 0 = Poettmann-Carpenter
        poettmannCarpenter(angle, diameter, velocity, liquidFraction,
                          gasDensity, liquidDensity, gasViscosity, liquidViscosity, roughness,
                          holdup, frictionGrad, gravityGrad, totalGrad, reynolds, flowType);
    } 
    else if (correlacao == 1) {
        // 1 = Baxendell-Thomas
        baxendellThomas(angle, diameter, velocity, liquidFraction,
                       gasDensity, liquidDensity, gasViscosity, liquidViscosity, roughness,
                       holdup, frictionGrad, gravityGrad, totalGrad, reynolds, flowType);
    }
    else if (correlacao == 2) {
        // 2 = Fancher-Brown
        fancherBrown(angle, diameter, productionRate, velocity, liquidFraction,
                    gasDensity, liquidDensity, gasViscosity, liquidViscosity, roughness,
                    holdup, frictionGrad, gravityGrad, totalGrad, reynolds, flowType);
    }
    else if (correlacao == 3) {
        // 3 = Hagedorn-Brown
        hagedornBrown(angle, diameter, roughness, pmed, velocity, liquidFraction,
                     gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                     temperature, compressibilityFactor,
                     holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                     reynolds, flowType, criticalFlag, convergeFlag);
    }
    else if (correlacao == 4) {
        // 4 = Duns-Ros
        dunsRos(angle, diameter, roughness, pmed, velocity, liquidFraction,
               gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
               temperature, compressibilityFactor,
               holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
               reynolds, flowType, criticalFlag, convergeFlag);
    }
    else if (correlacao == 5) {
        // 5 = Orkiszewski
        unsigned char isigFlag=0;
        orkiszewski(angle, diameter, roughness, pmed, velocity, liquidFraction,
                   gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                   temperature, compressibilityFactor, waterFraction,
                   holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                   reynolds, flowType, criticalFlag, convergeFlag, isigFlag);
    }
    else if (correlacao == 6) {
        // 6 = Beggs & Brill
        beggsAndBrill(angle, diameter, roughness, pmed, velocity, liquidFraction,
                     gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                     holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                     reynolds, flowType, palmerFlag, criticalFlag);
    }
    else if (correlacao == 7) {
        // 7 = Mukherjee-Brill
        mukherjeeeBrill(angle, diameter, roughness, pmed, velocity, liquidFraction,
                       gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                       surfaceTension, temperature, compressibilityFactor,
                       holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                       reynolds, flowPattern, criticalFlag, convergeFlag);
    }
    else if (correlacao == 8) {
        // 8 = Aziz
        aziz(angle, diameter, roughness, pmed, velocity, liquidFraction,
            gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
            holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
            reynolds, flowPattern, criticalFlag);
    }
    else if (correlacao == 9) {
        // 9 = Gray
        gray(angle, diameter, roughness, pmed, velocity, liquidFraction, waterFraction,
            gasDensity, liquidDensity, gasViscosity, liquidViscosity,
            oilSurfaceTension, waterSurfaceTension,
            holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
            reynolds, flowPattern, criticalFlag);
    }
    else if (correlacao == 10) {
        // 10 = Oliemans
        olie(angle, diameter, roughness, pmed, velocity, liquidFraction,
            gasDensity, liquidDensity, gasViscosity, liquidViscosity,
            surfaceTension, temperature, compressibilityFactor,
            holdup, frictionGrad, gravityGrad, totalGrad,
            reynolds, flowPattern, correlationFlag, criticalFlag);
    }
    else if (correlacao == 11) {
        // 11 = Dukler
        correlationFlag=0;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
    else if (correlacao == 12) {
        // 12 = Beggs & Brill com correção de Palmer
        palmerFlag = 1;
        beggsAndBrill(angle, diameter, roughness, pmed, velocity, liquidFraction,
                     gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                     holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                     reynolds, flowType, palmerFlag, criticalFlag);
    }
    else if (correlacao == 13) {
        // 13 = Dukler, Eaton e Flanigan
        correlationFlag=1;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
    else if (correlacao == 14) {
        // 14 = Dukler e Minami I
        correlationFlag=2;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
    else if (correlacao == 15) {
        // 15 = Dukler e Minami II
        correlationFlag=3;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
}


// ============================================================================
// CORRELAÇÕES DE ESCOAMENTO
// ============================================================================

/*
==============================================================================
poettmannCarpenter: Calcula gradientes de pressão em escoamento bifásico vertical
                    usando correlações de Poettmann e Carpenter.

Referências:
     1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984) 3-4 thru. 3-10.
     2.  Poettmann, F. H. and Carpenter, P. G.: "The Multiphase Flow
         of Gas, Oil, and Water Through Vertical Flow String with
         Application to the Design of Gas-Lift Installation,
         Drilling and Production Practice, API (1952) 257-317.

Dicionário de variáveis:
- angle         : Ângulo de inclinação do tubo (graus)
- diameter      : Diâmetro interno do tubo (polegadas)
- velocity      : Velocidade média do escoamento (ft/s)
- liquidFraction: Fração volumétrica de líquido
- gasDensity    : Densidade da fase gás (lb/ft³)
- liquidDensity : Densidade da fase líquida (lb/ft³)
- gasViscosity  : Viscosidade da fase gás (cP)
- liquidViscosity: Viscosidade da fase líquida (cP)
- roughness     : Rugosidade relativa do tubo (adimensional)
- frictionGrad  : Gradiente de pressão por fricção (psi/ft)
- gravityGrad   : Gradiente de pressão devido à gravidade (psi/ft)
- totalGrad     : Gradiente de pressão total (psi/ft)
- reynolds      : Número de Reynolds (adimensional)
- flowType      : Indicador do tipo de fluxo (1 = líquido, 2 = gás, 3 = misto)
- logFlowArray  : Array de valores de logaritmo de fluxo usado na correlação
- frictionArray : Array de valores da função correlacionada
- angleRad      : Ângulo em radianos
- mixtureRho    : Densidade aparente do escoamento, ponderada pela fração de líquido/gás
- frictionFactor: Fator de fricção
- xParam        : Parâmetro intermediário (produto de densidade, velocidade e diâmetro)
- logX          : Logaritmo de xParam 
==============================================================================
*/


void poettmannCarpenter(double angle, double diameter, double velocity, double liquidFraction,
                       double gasDensity, double liquidDensity, double gasViscosity, 
                       double liquidViscosity, double roughness, double& holdup,
                       double& frictionGrad, double& gravityGrad, double& totalGrad, double& reynolds, 
                       unsigned char& flowType) {



}

/*
==============================================================================
baxendellThomas: Calcula gradientes de pressão para escoamento bifásico vertical
                 usando correlação de Baxendell & Thomas.

Referências:
     1.  Baxendell, P. B. and Thomas, R.: "The Calculation of
         Pressure Gradients in High-Rate Flowing Wells," Journal
         of Petroleum Technology (Oct., 1961) 1023-1028.
     2.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984) 3-4 thru. 3-10.

Dicionário de variáveis:
- angle         : Ângulo de inclinação do tubo (graus)
- diameter      : Diâmetro interno do tubo (polegadas)
- velocity      : Velocidade média do escoamento (ft/s)
- liquidFraction: Fração volumétrica de líquido
- gasDensity    : Densidade do gás (lb/ft³)
- liquidDensity : Densidade do líquido (lb/ft³)
- gasViscosity  : Viscosidade do gás (cP)
- liquidViscosity: Viscosidade do líquido (cP)
- roughness     : Rugosidade relativa do tubo (adimensional)
- frictionGrad  : Gradiente de pressão por fricção (psi/ft)
- gravityGrad   : Gradiente de pressão devido à gravidade (psi/ft)
- totalGrad     : Gradiente de pressão total (psi/ft)
- reynolds      : Número de Reynolds (adimensional)
- flowType      : Indicador do tipo de fluxo (1 = líquido, 2 = gás, 3 = misto)
- logFlowArray  : Array de valores de logaritmo de fluxo usado na correlação (constante interna)
- baxendellArray: Array de valores da função correlacionada de Baxendell & Thomas (constante interna)
- angleRad      : Ângulo em radianos
- mixtureRho    : Densidade aparente do escoamento (ponderada pela fração de líquido/gás)
- frictionFactor: Fator de fricção
- xParam        : Parâmetro intermediário (produto de densidade, velocidade e diâmetro)
- logX          : Logaritmo de xParam 
==============================================================================
*/
void baxendellThomas(double angle, double diameter, double velocity, double liquidFraction, 
                    double gasDensity, double liquidDensity, double gasViscosity, 
                    double liquidViscosity, double roughness, double& holdup,
                    double& frictionGrad, double& gravityGrad, double& totalGrad, double& reynolds, 
                    unsigned char& flowType) {
    

}

/*
==============================================================================
fancherBrown: Calcula gradientes de pressão para escoamento bifásico vertical
              usando correlação de Fancher & Brown, com validação de entradas.

Referências:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984) 3-4 thru. 3-10.
      2.  Fancher, G. H. and Brown, K. E.: "Prediction of Pressure
          Gradients for Multiphase Flow in Tubing," Society of
          Petroleum Engineers Journal (March, 1963) 59-69.

Dicionário de variáveis:
- angle         : Ângulo de inclinação do tubo (graus)
- diameter      : Diâmetro interno do tubo (polegadas)
- productionRate: Taxa de produção Gás/Líquido (SCF/STB)
- velocity      : Velocidade média do escoamento (ft/s)
- liquidFraction: Fração volumétrica de líquido
- gasDensity    : Densidade do gás (lb/ft³)
- liquidDensity : Densidade do líquido (lb/ft³)
- gasViscosity  : Viscosidade do gás (cP)
- liquidViscosity: Viscosidade do líquido (cP)
- roughness     : Rugosidade relativa do tubo (adimensional)
- frictionGrad  : Gradiente de pressão por fricção (psi/ft)
- gravityGrad   : Gradiente de pressão devido à gravidade (psi/ft)
- totalGrad     : Gradiente de pressão total (psi/ft)
- reynolds      : Número de Reynolds (adimensional)
- flowType      : Indicador do tipo de fluxo (1 = líquido, 2 = gás, 3 = misto)
- rateArray     : Dados de GLR
- logFlowArray  : Array de valores de logaritmo de fluxo usado nas correlações (constante interna)
- fancherMatrix : Matriz de valores da função correlacionada Fancher & Brown
- angleRad      : Ângulo em radianos (calculado internamente)
- mixtureRho    : Densidade aparente do escoamento (ponderada pela fração de líquido/gás)
- frictionFactor: Fator de fricção calculado pela correlação ou função frictionFactor
- xParam        : Parâmetro intermediário (produto de densidade, velocidade e diâmetro)
- logX          : Logaritmo de xParam (usado nas correlações)
- tempArray     : Vetor auxiliar para interpolação 1D nos casos extremos de productionRate
- i, j          : Variáveis de controle de laço para seleção de linhas/colunas na matriz fancherMatrix
==============================================================================
*/
void fancherBrown(double angle, double diameter, double productionRate, double velocity, 
                 double liquidFraction, double gasDensity, double liquidDensity, 
                 double gasViscosity, double liquidViscosity, double roughness, 
                 double& holdup, double& frictionGrad, double& gravityGrad, double& totalGrad, 
                 double& reynolds, unsigned char& flowType) {
    
 
}

/*
==============================================================================
hagedornBrown: Calcula gradientes de pressão, holdup de líquido e propriedades
               de escoamento bifásico vertical pela correlação de Hagedorn & Brown.

Referências:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984) 3-11 thru. 3-19.
      2.  Duns, H., Jr. and Ros, N. C. J.: "Vertical Flow of Gas and
          Liquid Mixtures in Wells," Proc., 6th World Petroleum
          Congress (1963) 451.
      3.  Griffith, P. and Wallis, G. B.: "Two Phase Slug Flow,"
          Journal of Heat Transfer; Trans., ASME (Aug, 1961) 307-320.
      4.  Hagedorn, A. R. and Brown, K. E.: "Experimental Study of
          Pressure Gradients Occurring During Continuous Two Phase
          Flow in Small-Diameter Vertical Conduits," Journal of
          Petroleum Technology (April, 1965) 475-484.

Dicionário de variáveis:
- angle         : Ângulo de inclinação do tubo (graus)
- diameter      : Diâmetro interno do tubo (polegadas)
- roughness     : Rugosidade relativa do tubo
- pressure      : Pressão (psi)
- velocity      : Velocidade média do escoamento (ft/s)
- liquidFraction: Fração volumétrica de líquido
- gasDensity    : Densidade do gás (lb/ft³)
- liquidDensity : Densidade do líquido (lb/ft³)
- gasViscosity  : Viscosidade do gás (cP)
- liquidViscosity: Viscosidade do líquido (cP)
- liquidViscNumber: Parâmetro relacionado ao holdup de líquido
- liquidVelNumber: Parâmetro auxiliar para cálculo de holdup
- gasVelNumber  : Parâmetro auxiliar para cálculo de holdup
- diameterNumber: Parâmetro auxiliar para cálculo de holdup
- holdup        : Holdup de líquido calculado
- frictionGrad  : Gradiente de pressão por fricção (psi/ft)
- gravityGrad   : Gradiente de pressão gravitacional (psi/ft)
- accelGrad     : Gradiente de pressão aceleracional (psi/ft)
- totalGrad     : Gradiente de pressão total (psi/ft)
- reynolds      : Número de Reynolds
- flowType      : Indicador do tipo de fluxo (1=líquido, 2=gás, 3=bolha, 4=intermitente)
- griffithFlag  : Indicador de correlação Griffith & Wallis para fluxo bolha
- holdupFlag    : Indicador para ajuste de holdup mínimo
- criticalFlag  : Indicador de fluxo crítico
- calcInj       : Flag global para modo de cálculo
==============================================================================
*/
void hagedornBrown(double angle, double diameter, double roughness, double pressure, double velocity, 
                  double liquidFraction, double gasDensity, double liquidDensity, 
                  double gasViscosity, double liquidViscosity, double surfaceTension,
                  double temperature, double compressibilityFactor,
                  double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                  double& totalGrad, double& reynolds, unsigned char& flowType, 
                  unsigned char& criticalFlag, unsigned char& convergeFlag) {



}

/*
==============================================================================
dunsRos: Calcula gradientes de pressão, holdup de líquido e propriedades
         de escoamento bifásico vertical pela correlação de Duns & Ros.

Referências:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984) 3-20 thru. 3-33.
      2.  Duns, H., Jr. and Ros, N. C. J.: "Vertical Flow of Gas and
          Liquid Mixtures in Wells," Proc., 6th World Petroleum
          Congress (1963) 451.

Dicionário de variáveis:
- angle         : Ângulo de inclinação do tubo (graus)
- diameter      : Diâmetro interno do tubo (polegadas)
- roughness     : Rugosidade relativa do tubo
- pressure      : Pressão (psi)
- velocity      : Velocidade média do escoamento (ft/s)
- liquidFraction: Fração volumétrica de líquido
- gasDensity    : Densidade do gás (lb/ft³)
- liquidDensity : Densidade do líquido (lb/ft³)
- gasViscosity  : Viscosidade do gás (cP)
- liquidViscosity: Viscosidade do líquido (cP)
- surfaceTension: Tensão superficial (dyn/cm)
- liquidViscNumber: Número adimensional de viscosidade do líquido
- liquidVelNumber: Número adimensional de velocidade do líquido
- gasVelNumber  : Número adimensional de velocidade do gás
- diameterNumber: Número adimensional de diâmetro do tubo
- holdup        : Holdup de líquido calculado
- frictionGrad  : Gradiente de pressão por fricção (psi/ft)
- gravityGrad   : Gradiente de pressão gravitacional (psi/ft)
- accelGrad     : Gradiente de pressão aceleracional (psi/ft)
- totalGrad     : Gradiente de pressão total (psi/ft)
- reynolds      : Número de Reynolds do líquido
- flowType      : Indicador do tipo de fluxo (1=líquido, 2=gás, 3=bolha, 4=slug, 5=névoa, 6=transição)
- criticalFlag  : Indicador de fluxo crítico
- convergeFlag  : Indicador de convergência
==============================================================================
*/
void dunsRos(double angle, double diameter, double roughness, double pressure, double velocity, 
             double liquidFraction, double gasDensity, double liquidDensity, 
             double gasViscosity, double liquidViscosity, double surfaceTension,
             double temperature, double compressibilityFactor,
             double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
             double& totalGrad, double& reynolds, unsigned char& flowType, 
             unsigned char& criticalFlag, unsigned char& convergeFlag) {
    
    

}

/*
==============================================================================
orkiszewski: Calcula gradientes de pressão, holdup de líquido e propriedades
             de escoamento bifásico vertical pela correlação de Orkiszewski.

Referências:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984) 3-33 thru. 3-42.
      2.  Orkiszewski, J.: "Predicting Two Phase Pressure Drops in
          Vertical Pipes," Journal of Petroleum Technology
          (June, 1967) 829-838.

Dicionário de variáveis:
- numEntrCorr   : Número de entradas (ou cenário de correlação)
- angle         : Ângulo de inclinação do tubo (graus)
- diameter      : Diâmetro interno do tubo (polegadas)
- roughness     : Rugosidade relativa do tubo
- pressure      : Pressão (psi)
- velocity      : Velocidade média do escoamento (ft/s)
- liquidFraction: Fração volumétrica de líquido
- gasDensity    : Densidade do gás (lb/ft³)
- liquidDensity : Densidade do líquido (lb/ft³)
- gasViscosity  : Viscosidade do gás (cP)
- liquidViscosity: Viscosidade do líquido (cP)
- surfaceTension: Tensão superficial (dyn/cm)
- liquidVelNumber: Número adimensional de velocidade do líquido
- gasVelNumber  : Número adimensional de velocidade do gás
- waterFraction : Fração em massa de água na fase líquida
- holdup        : Holdup de líquido calculado
- frictionGrad  : Gradiente de pressão por fricção (psi/ft)
- gravityGrad   : Gradiente de pressão gravitacional (psi/ft)
- accelGrad     : Gradiente de pressão aceleracional (psi/ft)
- totalGrad     : Gradiente de pressão total (psi/ft)
- reynolds      : Número de Reynolds do líquido
- liquidDistrib : Coeficiente de distribuição de líquido entre fases
- gasSupVel     : Velocidade superficial de gás
- gasSupVelCorr : Velocidade superficial de gás corrigida para névoa
- liquidSupVel  : Velocidade superficial de líquido
- flowType      : Indicador do tipo de fluxo (1=líquido, 2=gás, 3=bolha, 4=slug, 5=névoa, 6=transição)
- criticalFlag  : Indicador de fluxo crítico
- convergeFlag  : Indicador de convergência
- isigFlag      : Flag para ajustes de liquidDistrib
==============================================================================
*/
void orkiszewski(double angle, double diameter, double roughness, 
                double pressure, double velocity, double liquidFraction, double gasDensity, 
                double liquidDensity, double gasViscosity, double liquidViscosity, 
                double surfaceTension, double temperature, double compressibilityFactor, double waterFraction,
                double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                double& totalGrad, double& reynolds, unsigned char& flowType, 
                unsigned char& criticalFlag, unsigned char& convergeFlag, unsigned char& isigFlag) {
    
   

}

/*
==============================================================================
beggsAndBrill: Calcula gradientes de pressão, holdup de líquido e propriedades
               de escoamento bifásico pela correlação de Beggs & Brill.
Referências:
      1.  Beggs, H.D. and Brill, J.P.: "A Study of Two-Phase Flow in
          Inclined Pipes," Journal of Petroleum Technology (May,1973),
          607-617.
      2.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984) 3-53 thru. 3-64.
Dicionário de variáveis:
- angle         : Ângulo de inclinação do tubo (graus)
- diameter      : Diâmetro interno do tubo (polegadas)
- roughness     : Rugosidade relativa do tubo
- pressure      : Pressão (psi)
- velocity      : Velocidade superficial total (ft/s)
- liquidFraction: Fração volumétrica de líquido
- gasDensity    : Densidade do gás (lb/ft³)
- liquidDensity : Densidade do líquido (lb/ft³)
- gasViscosity  : Viscosidade do gás (cP)
- liquidViscosity: Viscosidade do líquido (cP)
- liquidVelNumber: Número adimensional da velocidade do líquido
- holdup        : Holdup de líquido calculado
- frictionGrad  : Gradiente de pressão por fricção (psi/ft)
- gravityGrad   : Gradiente de pressão gravitacional (psi/ft)
- accelGrad     : Gradiente de pressão aceleracional (psi/ft)
- totalGrad     : Gradiente de pressão total (psi/ft)
- reynolds      : Número de Reynolds da mistura
- flowType      : Indicador do padrão de fluxo (1=líquido, 2=gás, 3=distribuído, 4=intermitente, 5=segregado, 6=transição)
- palmerFlag    : Indicador de uso de fator de correção de Palmer (0=sem correção, 1=com correção)
- criticalFlag  : Indicador de fluxo crítico (0=normal, 1=crítico)
Variáveis auxiliares:
- liquidSupVel  : Velocidade superficial de líquido (ft/s)
- gasSupVel     : Velocidade superficial de gás (ft/s)
- froudeNumber  : Número de Froude da mistura
- transitionFlag: Indicador de fluxo de transição
- transitionBoundary1, transitionBoundary2, transitionBoundary3, transitionBoundary4 : Limites de transição entre padrões de fluxo
- flowPatternBoundary : Limite da correlação para determinar padrão de fluxo
- horizontalHoldup : Holdup horizontal
- flowPatternCoefD, flowPatternCoefE, flowPatternCoefF, flowPatternCoefG : Coeficientes específicos do padrão de fluxo para fator C
- segregatedHoldup : Holdup de líquido de fluxo segregado
- intermittentHoldup : Holdup de líquido de fluxo intermitente
- aParam, bParam : Fatores para interpolação de transição
- cFactor       : Fator de correção de ângulo
- angleParameter : Parâmetro auxiliar para cálculo do fator de correção angular
- angleFactor   : Fator de correção de ângulo
- noSlipDensity : Densidade da mistura sem escorregamento
- slipDensity   : Densidade da mistura com escorregamento
- noSlipViscosity : Viscosidade da mistura sem escorregamento
- frictionFactorValue : Fator de atrito de Moody
- yParam, xParam, sParam : Parâmetros para cálculo do fator de atrito bifásico
- accelParam    : Termo de aceleração (adimensional)
- loopFlag      : Flag de controle para laço
==============================================================================
*/
void beggsAndBrill(double angle, double diameter, double roughness, double pressure, double velocity, 
                   double liquidFraction, double gasDensity, double liquidDensity, 
                   double gasViscosity, double liquidViscosity, double surfaceTension,
                   double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                   double& totalGrad, double& reynolds, unsigned char& flowType, 
                   unsigned char palmerFlag, unsigned char& criticalFlag
                   ) {
    
    // Calcular velocidade superficial do líquido e número adimensional
    double liquidSupVel = velocity * liquidFraction;
    double liquidVelNumber = 1.938 * pow(liquidDensity / surfaceTension, 0.25) * liquidSupVel;
    unsigned char transitionFlag = 0;
    double gasSupVel;
   
    criticalFlag = 0;

    
    // Converter ângulo para radianos
    double angleRad = angle * M_PI / 180.0;
    
    // Calcular velocidades superficiais e número de Froude da mistura
    gasSupVel = velocity - liquidSupVel;
    double froudeNumber = (velocity * velocity) / (g_in * diameter);
    
    double horizontalHoldup;
    double intermittentHoldup = 0.0;
    
    // Verificar fluxo monofásico
    flowType = 5;
    if (liquidFraction > 0.99999) flowType = 1;
    if (liquidFraction < 0.00001) flowType = 2;
    
    if (flowType <= 2) {
        holdup = liquidFraction;
    } else {
        // Determinar padrão de fluxo usando mapa de padrão revisado
        transitionFlag = 0;
        double transitionBoundary1 = 316.0 * pow(liquidFraction, 0.302);
        double transitionBoundary2 = 0.0009252 * pow(liquidFraction, -2.46842);
        double transitionBoundary3 = 0.1 * pow(liquidFraction, -1.45155);
        double transitionBoundary4 = 0.5 * pow(liquidFraction, -6.738);
        double flowPatternBoundary = transitionBoundary1;
        
        if (liquidFraction >= 0.01) {
            if (liquidFraction >= 0.4) flowPatternBoundary = transitionBoundary4;
            if ((froudeNumber >= transitionBoundary2) && (froudeNumber < transitionBoundary3)) transitionFlag = 1;
            if ((froudeNumber >= transitionBoundary3) && (froudeNumber < flowPatternBoundary)) flowType = 4;
            if (froudeNumber >= flowPatternBoundary) flowType = 3;
        } else {
            if (froudeNumber >= transitionBoundary1) flowType = 3;
        }
        
        // Determinar holdup horizontal e coeficientes do fator C
        double flowPatternCoefD, flowPatternCoefE, flowPatternCoefF, flowPatternCoefG;
        double segregatedHoldup = 0.0;
        bool loopFlag = true;
        
        do {
            loopFlag = true;
            switch (flowType) {
                case 3: // Fluxo distribuído
                	if(fabsl(froudeNumber)>1e-15)horizontalHoldup = 1.065 * pow(liquidFraction, 0.5824) * pow(froudeNumber, -0.0609);
                	else horizontalHoldup = liquidFraction;
                    flowPatternCoefD = 1.0;
                    flowPatternCoefE = 0.0;
                    flowPatternCoefF = 0.0;
                    flowPatternCoefG = 0.0;
                    break;
                case 4: // Fluxo intermitente
                	if(fabsl(froudeNumber)>1e-15)horizontalHoldup = 0.845 * pow(liquidFraction, 0.5351) * pow(froudeNumber, -0.0173);
                	else horizontalHoldup = liquidFraction;
                    flowPatternCoefD = 2.96;
                    flowPatternCoefE = 0.305;
                    flowPatternCoefF = -0.4473;
                    flowPatternCoefG = 0.0978;
                    break;
                case 5: // Fluxo segregado
                	if(fabsl(froudeNumber)>1e-15)horizontalHoldup = 0.98 * pow(liquidFraction, 0.4846) * pow(froudeNumber, -0.0868);
                	else horizontalHoldup = liquidFraction;
                    flowPatternCoefD = 0.011;
                    flowPatternCoefE = -3.768;
                    flowPatternCoefF = 3.539;
                    flowPatternCoefG = -1.614;
                    break;
            }
            
            // Restringir valor mínimo de horizontalHoldup
            if (horizontalHoldup < liquidFraction) horizontalHoldup = liquidFraction;
            
            // Verificar fluxo horizontal
            if (angleRad == 0.0) {
                holdup = horizontalHoldup;
            } else {
                // Fluxo inclinado: calcular fator C
                if (angleRad <= 0.0) {
                    // Coeficientes para fluxo descendente
                    flowPatternCoefD = 4.7;
                    flowPatternCoefE = -0.3692;
                    flowPatternCoefF = 0.1244;
                    flowPatternCoefG = -0.5056;
                }
                
                // Calcular fator C
                double cFactor;
                if(fabsl(froudeNumber)>1e-15)
                    cFactor = (1.0 - liquidFraction) * log(flowPatternCoefD * pow(liquidFraction, flowPatternCoefE) * pow(liquidVelNumber, flowPatternCoefF) * pow(froudeNumber, flowPatternCoefG));
                else cFactor =0.;
                if (cFactor < 0.0) cFactor = 0.0;
                
                // Calcular fator de correção de ângulo e holdup corrigido
                double angleParameter = sin(1.8 * angleRad);
                double angleFactor = 1.0 + cFactor * (angleParameter - 0.333 * angleParameter * angleParameter * angleParameter);
                
                // Verificar se angleFactor não é negativo
                if (angleFactor < 0.0) angleFactor = 0.0;
                
                holdup = horizontalHoldup * angleFactor;
                if (holdup > 1.0) holdup = 1.0;
                
                // Verificar se holdup é maior que 0
                if (holdup <= 0.0) holdup = 0.00001;
                
                // Aplicar fatores de correção de Palmer se desejado
                if (palmerFlag != 0) {
                    if (angle < 0.0) holdup = holdup * 0.685;
                    if (angle > 0.0) holdup = holdup * 0.924;
                }
            }
            
            // Verificar fluxo de transição
            if (transitionFlag == 1) {
                if (flowType >= 5) {
                    segregatedHoldup = holdup;
                    flowType = 4;
                    loopFlag = false;
                } else {
                    intermittentHoldup = holdup;
                    double aParam = (transitionBoundary3 - froudeNumber) / (transitionBoundary3 - transitionBoundary2);
                    double bParam = 1.0 - aParam;
                    holdup = segregatedHoldup * aParam + intermittentHoldup * bParam;
                    flowType = 6;
                }
            }
        } while (!loopFlag);
    }
    
    // Calcular propriedades da mistura
    double noSlipDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    double slipDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
    double noSlipViscosity = liquidViscosity * liquidFraction + gasViscosity * (1.0 - liquidFraction);
    
    // Calcular fator de atrito do diagrama de Moody
    reynolds = 1488.0 * noSlipDensity * velocity * diameter / noSlipViscosity;
    double frictionFactorValue;
    if(fabsl(reynolds)>1e-15)frictionFactor(reynolds, roughness, frictionFactorValue);
    else frictionFactorValue=0.;
    
    if (flowType > 2) {
        // Calcular fator de atrito bifásico
        double yParam = liquidFraction / (holdup * holdup);
        double xParam = log(yParam);
        double sParam = xParam / (-0.0523 + 3.182*xParam - 0.8725*xParam*xParam + 0.01853*xParam*xParam*xParam*xParam);
        if ((yParam > 1.0) && (yParam < 1.2)) sParam = log(2.2*yParam - 1.2);
        frictionFactorValue = frictionFactorValue * exp(sParam);
    }
    
    // Calcular gradientes de pressão
    frictionGrad = frictionFactorValue * noSlipDensity * velocity * velocity / (2.0 * g_in * diameter * 144.0);
    gravityGrad = slipDensity * sin(angleRad) / 144.0;
    double accelParam = slipDensity * velocity * gasSupVel / (g_in * pressure * 144.0);
    
    // Verificar fluxo crítico
    if (accelParam > 0.95) {
        accelParam = 0.95;
        criticalFlag = 1;
    }
    
    totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
    accelGrad = accelParam * totalGrad;
}

/*
==============================================================================
holdupInterpolation: Função auxiliar para correlação de Dukler
Esta subrotina realiza interpolação log-log entre o holdup de líquido sem escorregamento
e o de Dukler, em função do número de Reynolds.
Dicionário de variáveis:
- columnIndex  : Índice da coluna a ser usada na matriz de dados (1 a 11)
- inputValue   : Valor para o qual se deseja interpolar o holdup (entre 0 e 1)
- outputValue  : Holdup de líquido interpolado (saída)
Matrizes de dados:
- inputMatrix[9][11]: Matriz de valores de X usados para interpolação (cada coluna corresponde a um valor de columnIndex)
- outputMatrix[9][11]: Matriz de valores de holdup de líquido para cada X (cada coluna corresponde a um valor de columnIndex)
Variáveis auxiliares:
- interpolationFlag : Indicador se a interpolação já foi realizada (flag de parada)
- arrayIndex        : Índice ajustado para acesso baseado em zero (columnIndex-1)
- rowIndex          : Índice de linha para percorrer as matrizes
- lowerIndex, upperIndex : Índices para interpolação entre dois pontos
==============================================================================
*/
void holdupInterpolation(int columnIndex, double inputValue, double& outputValue) {
 
}

/*
==============================================================================
holdupLiquidFraction: Calcula o holdup de líquido usando correlações de Eaton/Dukler

Dicionário de variáveis:
- liquidFraction   : Fração volumétrica de líquido sem escorregamento (0-1)
- liquidViscosity  : Viscosidade do líquido (cP)
- liquidDensity    : Densidade do líquido (lb/ft³)
- gasDensity       : Densidade do gás (lb/ft³)
- diameter         : Diâmetro interno do tubo (polegadas)
- velocity         : Velocidade superficial total (ft/s)
- pressure         : Pressão (psi)
- liquidNumber     : Número adimensional de viscosidade do líquido
- liquidViscNumber : Número adimensional de velocidade do líquido
- gasVelNumber     : Número adimensional de velocidade do gás
- tubeNumber       : Número adimensional de diâmetro do tubo
- holdup           : Holdup de líquido calculado
- reynoldsMixture  : Número de Reynolds da mistura
- densityMixture   : Densidade da mistura
- correlationFlag  : Flag de escolha da correlação (0=Dukler, 1=Eaton)

Arrays internos:
- reynoldsValues[11] : Valores de Reynolds para interpolação
- eatonX[30], eatonXL[30], eatonY[30] : Arrays da correlação de Eaton

Variáveis auxiliares:
- holdupGuess      : Estimativa do holdup para iteração
- iterationCount   : Contador de iterações
- bnl              : Constante usada na correlação de Eaton
- xe               : Parâmetro logarítmico para interpolação Eaton
- e1, e2           : Valores de holdup para interpolação Dukler
- atmToPsi         : Pressão atmosférica padrão em psi
==============================================================================
*/
void holdupLiquidFraction(double liquidFraction, double liquidViscosity, double liquidDensity, 
                         double gasDensity, double diameter, double velocity, double pressure,
                         double liquidNumber, double liquidViscNumber, double gasVelNumber, double tubeNumber,
                         double& holdup, double& reynoldsMixture, double& densityMixture, 
                         unsigned char correlationFlag) {
    

}

/*
==============================================================================
holdupMinami: Calcula holdup de líquido usando correlações de Minami I e II
             com correção de inclinação de Beggs & Brill

Referências:
     1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984) 4-11 thru 4-17.
     2.  Beggs, H. D. and Brill, J. P.: "A Study of Two-Phase Flow 
         in Inclined Pipes," Journal of Petroleum Technology 
         (May, 1973) 607-617.
     3.  Minami, K. et al.: "Correlações para Cálculo de Holdup de
         Líquido em Escoamento Bifásico Horizontal e Inclinado,"
         Relatório Técnico PETROBRAS (1985).

Dicionário de variáveis:
- angle            : Ângulo de inclinação do tubo (graus)
- diameter         : Diâmetro interno do tubo (polegadas)
- pressure         : Pressão (psi)
- velocity         : Velocidade da mistura (ft/s)
- liquidFraction   : Fração volumétrica de líquido sem escorregamento (0-1)
- gasVelNumber     : Número adimensional de velocidade do gás
- liquidViscNumber : Número adimensional de velocidade do líquido
- tubeNumber       : Número adimensional de diâmetro do tubo
- liquidNumber     : Número adimensional de viscosidade do líquido
- correlationFlag  : Flag de correlação (2=Minami I, 3=Minami II)
- transitionFlag   : Flag de transição quando fora da faixa de aplicabilidade
- flowPattern      : Padrão de escoamento (1=líquido, 2=gás, 3=distribuído, 4=intermitente, 5=segregado, 6=transição)
- holdup           : Holdup de líquido calculado (saída)

Variáveis auxiliares:
- angleRad         : Ângulo em radianos
- froudeNumber     : Número de Froude da mistura
- horizontalHoldup : Holdup para escoamento horizontal
- correctionFactor : Fator de correção de ângulo de inclinação
- transitionParams : Parâmetros para região de transição
==============================================================================
*/
void holdupMinami(double angle, double diameter, double pressure, double velocity, 
                 double liquidFraction, double gasVelNumber, double liquidViscNumber, 
                 double tubeNumber, double liquidNumber, unsigned char correlationFlag,
                 unsigned char& transitionFlag, unsigned char& flowPattern, double& holdup) {
    
    const double atmToPsi = 14.69595;
    
    
    double angleRad = angle * M_PI / 180.0;
    double froudeNumber = velocity * velocity / (g_in * diameter);
    
    // Verificar escoamento monofásico
    flowPattern = 5; // Padrão: escoamento segregado
    if (liquidFraction > 0.99999) flowPattern = 1; // Líquido
    if (liquidFraction < 0.00001) flowPattern = 2; // Gás
    
    if (flowPattern <= 2) {
        holdup = liquidFraction;
    } else {
        // Selecionar correlação de holdup líquido para escoamento horizontal
        double horizontalHoldup;
        bool flagLoop = true;
        
        do {
            flagLoop = true;
            if (correlationFlag == 2) {
                // Correlação I (gás úmido)
                double yParam = pow(liquidFraction, 0.8945) * pow(tubeNumber, 0.0796) * pow(liquidViscNumber, -0.4076);
                if (yParam < 0.0026) {
                    holdup = liquidFraction;
                    return;
                }
                if (yParam > 0.15) {
                    transitionFlag = 1; // Fora da faixa de aplicabilidade da Correlação I
                    flagLoop = false;
                    correlationFlag = 3; // Mudar para Correlação II
                } else {
                    horizontalHoldup = 3.698 * yParam - 11.497 * yParam * yParam + 65.22 * pow(yParam, 4) - 0.00952;
                }
            } else {
                // Correlação II (geral)
                double eatonNumber = 1.84 * pow(liquidViscNumber, 0.575) * pow(pressure / atmToPsi, 0.05) * pow(liquidNumber, 0.1) /
                                   (gasVelNumber * pow(tubeNumber, 0.0277));
                if (eatonNumber >= 0.0001) {
                    horizontalHoldup = 1.0 - exp(-pow((log10(eatonNumber) + 9.21) / 8.7115, 4.3374));
                } else {
                    holdup = liquidFraction;
                    return;
                }
            }
        } while (!flagLoop);
        
        if (angle == 0.0) {
            holdup = horizontalHoldup;
        } else {
            // Calcular fator de correção de ângulo de inclinação por Beggs e Brill
            if (horizontalHoldup < liquidFraction) horizontalHoldup = liquidFraction;
            
            // Determinar regime de escoamento usando mapa de padrão de Beggs e Brill
            unsigned char transitionType = 0;
            double xl1 = 316.0 * pow(liquidFraction, 0.302);
            double xl2 = 0.0009252 * pow(liquidFraction, -2.46842);
            double xl3 = 0.1 * pow(liquidFraction, -1.45155);
            double xl4 = 0.5 * pow(liquidFraction, -6.738);
            double froudeTransition = xl1;
            
            if (liquidFraction >= 0.01) {
                if (liquidFraction > 0.4) froudeTransition = xl4;
                if ((froudeNumber >= xl2) && (froudeNumber < xl3)) transitionType = 1;
                if ((froudeNumber >= xl3) && (froudeNumber < froudeTransition)) flowPattern = 4; // Intermitente
                if (froudeNumber >= froudeTransition) flowPattern = 3; // Distribuído
            } else {
                if (froudeNumber >= xl1) flowPattern = 3; // Distribuído
            }
            
            // Determinar coeficientes para correção de inclinação
            double holdupSegregated = 0.0, holdupIntermittent = 0.0;
            bool transitionLoop = true;
            
            do {
                transitionLoop = true;
                double coeff_d, coeff_e, coeff_f, coeff_g;
                
                switch (flowPattern) {
                    case 3: // Escoamento distribuído
                        coeff_d = 1.0; coeff_e = 0.0; coeff_f = 0.0; coeff_g = 0.0;
                        break;
                    case 4: // Escoamento intermitente  
                        coeff_d = 2.96; coeff_e = 0.305; coeff_f = -0.4473; coeff_g = 0.0978;
                        break;
                    case 5: // Escoamento segregado
                        coeff_d = 0.011; coeff_e = -3.768; coeff_f = 3.539; coeff_g = -1.614;
                        break;
                }
                
                if (angleRad <= 0) {
                    // Coeficientes para escoamento descendente
                    coeff_d = 4.7; coeff_e = -0.3692; coeff_f = 0.1244; coeff_g = -0.5056;
                }
                
                double cFactor = (1.0 - liquidFraction) * log(coeff_d * pow(liquidFraction, coeff_e) * 
                                pow(liquidViscNumber, coeff_f) * pow(froudeNumber, coeff_g));
                if (cFactor < 0.0) cFactor = 0.0;
                
                // Calcular fator de correção de ângulo
                double sineTerm = sin(1.8 * angleRad);
                double correctionFactor = 1.0 + cFactor * (sineTerm - 0.333 * sineTerm * sineTerm * sineTerm);
                
                if (correctionFactor < 0.0) correctionFactor = 0.0;
                holdup = horizontalHoldup * correctionFactor;
                if (holdup > 1.0) holdup = 1.0;
                if (holdup <= 0.0) holdup = 0.00001;
                
                // Verificar escoamento de transição
                if (transitionType == 1) {
                    if (flowPattern == 5) {
                        holdupSegregated = holdup;
                        flowPattern = 4;
                        transitionLoop = false;
                    } else {
                        holdupIntermittent = holdup;
                        double weightA = (xl3 - froudeNumber) / (xl3 - xl2);
                        double weightB = 1.0 - weightA;
                        holdup = holdupSegregated * weightA + holdupIntermittent * weightB;
                        flowPattern = 6; // Transição
                    }
                }
            } while (!transitionLoop);
        }
    }
}

/*
==============================================================================
duklerCorrelation: Calcula holdup de líquido e gradiente de pressão usando
                  correlação de Dukler com gradiente de elevação de Flanigan

Referências:
     1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984) 4-11 thru 4-17.
     2.  Dukler, A.E. et al.: "Gas-Liquid Flow in Pipelines, I.
         Research Results," AGA-API Project NX-28 (May, 1969)
     3.  Duns, H., Jr. and Ros, N. C. J.: "Vertical Flow of Gas and
         Liquid Mixtures in Wells." Proceedings, Sixth World
         Petroleum Congress (1963), 451.
     4.  Eaton, B.A.: "The Prediction of Flow Patterns, Liquid Holdup
         and Pressure Losses Occurring During Continuous Two Phase
         Flow in Horizontal Pipelines," Ph.D. Dissertation, The
         University of Texas, Austin (1965)
     5.  Eaton, B.A. et al.: "The Prediction of Flow Patterns, Liquid
         Holdup and Pressure Losses Occurring During Continuous
         Two Phase Flow in Horizontal Pipelines," Transactions of
         AIME (1967), 815
     6.  Flanigan, O.: "Effect of Uphill Flow on Pressure Drop in De-
         sign of Two Phase Gathering Systems," Oil and Gas Journal
         (March 10, 1958), 56

Dicionário de variáveis:
- angle             : Ângulo de inclinação do tubo (graus)
- diameter          : Diâmetro interno do tubo (polegadas)
- roughness         : Rugosidade relativa (adimensional)
- pressure          : Pressão (psi)
- velocity          : Velocidade da mistura (ft/s)
- liquidFraction    : Fração volumétrica de líquido sem escorregamento (0-1)
- gasDensity        : Densidade do gás (lb/ft³)
- liquidDensity     : Densidade do líquido (lb/ft³)
- gasViscosity      : Viscosidade do gás (cP)
- liquidViscosity   : Viscosidade do líquido (cP)
- liquidNumber      : Número adimensional de viscosidade do líquido
- liquidViscNumber  : Número adimensional de velocidade do líquido
- gasVelNumber      : Número adimensional de velocidade do gás
- tubeNumber        : Número adimensional de diâmetro do tubo
- holdup            : Holdup de líquido calculado (saída)
- frictionGrad      : Gradiente de pressão por atrito (psi/ft)
- gravityGrad       : Gradiente de pressão gravitacional (psi/ft)
- accelGrad         : Gradiente de pressão por aceleração (psi/ft)
- totalGrad         : Gradiente de pressão total (psi/ft)
- reynolds          : Número de Reynolds (saída)
- flowPattern       : Padrão de escoamento (1=líquido, 2=gás, 3=bifásico)
- correlationFlag   : Flag de correlação de holdup (0=Dukler, 1=Eaton, 2=Minami I, 3=Minami II)
- transitionFlag    : Flag de transição fora da faixa de aplicabilidade
- criticalFlag      : Flag de fluxo crítico (1=crítico)

Arrays internos:
- phiFlanigan[16]   : Fator de correção de densidade para gradiente de elevação (Flanigan)
- vsgFlanigan[16]   : Velocidades superficiais de gás para correlação de Flanigan
- lambdaValues[15]  : Valores de fração de líquido para fator de atrito
- logLambda[15]     : Logaritmos das frações de líquido
- frictionRatio[15] : Razões de fator de atrito bifásico/monofásico
==============================================================================
*/
void duklerCorrelation(double angle, double diameter, double roughness, double pressure, 
                      double velocity, double liquidFraction, double gasDensity, 
                      double liquidDensity, double gasViscosity, double liquidViscosity,
                      double surfaceTension, double temperature, double compressibilityFactor,
                      double& holdup, double& frictionGrad, double& gravityGrad,
                      double& accelGrad, double& totalGrad, double& reynolds, unsigned char& flowPattern,
                      unsigned char correlationFlag, unsigned char& transitionFlag, 
                      unsigned char& criticalFlag) {
    
 
}

/*
==============================================================================
mukherjeeeBrillFlowPattern: Determina o padrão de escoamento bifásico usando
                           a correlação de Mukherjee & Brill

Dicionário de variáveis:
- liquidViscNumber  : Número adimensional de velocidade do líquido
- gasVelNumber      : Número adimensional de velocidade do gás
- liquidNumber      : Número adimensional de viscosidade do líquido
- angle             : Ângulo de inclinação do tubo (graus)
- flowPattern       : Padrão de escoamento determinado
                      3=bolhas, 4=golfadas, 5=névoa, 6=estratificado

Variáveis auxiliares:
- angleRad          : Ângulo em radianos
- sinAngle          : Seno do ângulo de inclinação
- absAngle          : Valor absoluto do ângulo
- gasVelMist        : Fronteira para escoamento de névoa
- liquidVelBubble   : Fronteira para escoamento de bolhas
- liquidVelStrat    : Fronteira para escoamento estratificado
- gasVelBubble      : Fronteira para regime de bolhas vs estratificado

Padrões de escoamento:
- 3: Escoamento de bolhas
- 4: Escoamento de golfadas (slug)
- 5: Escoamento de névoa (mist)
- 6: Escoamento estratificado
==============================================================================
*/
void mukherjeeeBrillFlowPattern(double liquidViscNumber, double gasVelNumber, double liquidNumber, 
                               double angle, unsigned char& flowPattern) {
    
    
 
}

/*
==============================================================================
mukherjeeeBrillHoldup: Calcula o holdup de líquido pela correlação de
                      Mukherjee & Brill

Dicionário de variáveis:
- liquidViscNumber  : Número adimensional de velocidade do líquido
- gasVelNumber      : Número adimensional de velocidade do gás
- liquidNumber      : Número adimensional de viscosidade do líquido
- angle             : Ângulo de inclinação do tubo (graus)
- flowPattern       : Padrão de escoamento (3=bolhas, 4=golfadas, 5=névoa, 6=estratificado)
- holdup            : Holdup de líquido calculado (saída)

Variáveis auxiliares:
- angleRad          : Ângulo em radianos
- sinAngle          : Seno do ângulo de inclinação
- exponentExp       : Expoente da expressão para holdup
- exponentResult    : Resultado da expressão exponencial
==============================================================================
*/
void mukherjeeeBrillHoldup(double liquidViscNumber, double gasVelNumber, double liquidNumber, 
                          double angle, unsigned char flowPattern, double& holdup) {
    

    

}

/*
==============================================================================
mukherjeeeBrillStratifiedFlow: Calcula gradientes de pressão para escoamento
                              estratificado pela correlação de Mukherjee & Brill

Dicionário de variáveis:
- angleRad          : Ângulo de inclinação do tubo (radianos)
- velocity          : Velocidade superficial total (ft/s)
- liquidFraction    : Fração volumétrica de líquido sem escorregamento (0-1)
- holdup            : Holdup de líquido calculado (0-1)
- diameter          : Diâmetro interno do tubo (polegadas)
- roughness         : Rugosidade relativa (adimensional)
- gasDensity        : Densidade do gás (lb/ft³)
- gasViscosity      : Viscosidade do gás (cP)
- frictionGrad      : Gradiente de pressão por atrito (psi/ft) - saída
- gravityGrad       : Gradiente de pressão gravitacional (psi/ft) - saída
- totalGrad         : Gradiente de pressão total (psi/ft) - saída
- convergeFlag      : Flag de convergência (0=convergiu, 1=não convergiu) - saída

Variáveis auxiliares:
- convergeStatus    : Status de convergência da iteração
- chordAngle        : Ângulo subtendido pela corda de líquido
- iterations        : Contador de iterações
- gamma             : Variável auxiliar para solução iterativa
- auxiliarX, auxiliarY : Variáveis auxiliares para aceleração de convergência
- hydraulicDiameter : Diâmetro hidráulico da fase gasosa
- effectiveDiameter : Diâmetro hidráulico efetivo
- liquidVelocity    : Velocidade superficial do líquido
- gasVelocity       : Velocidade superficial do gás
- realGasVelocity   : Velocidade real do gás
- reynolds          : Número de Reynolds da fase gasosa
- frictionFactorValue : Fator de atrito de Moody
==============================================================================
*/
void mukherjeeeBrillStratifiedFlow(double angleRad, double velocity, double liquidFraction, 
                                 double holdup, double diameter, double roughness, double gasDensity, 
                                 double gasViscosity, double& frictionGrad, double& gravityGrad, 
                                 double& totalGrad, unsigned char& convergeFlag) {
    
    
 
}

/*
==============================================================================
mukherjeeeBrill: Calcula holdup de líquido, padrão de escoamento e gradiente
                de pressão usando a correlação completa de Mukherjee & Brill

Referências:
     1.  Mukherjee, H. and Brill, J.P.: "Empirical Equations to Predict 
         Flow Patterns in Two Phase Inclined Flow," International Journal 
         of Multiphase Flow, Vol. 11, No. 3 (May-June 1985), 299-315.
     2.  Mukherjee, H.: "An Experimental Study of Inclined Two Phase Flow," 
         Ph.D. Dissertation, The University of Tulsa (1979).
     3.  Mukherjee, H. and Brill, J.P.: "Liquid Holdup Correlations for 
         Inclined Two Phase Flow," Journal of Petroleum Technology (May 1983), 1003-1008.
     4.  Mukherjee, H. and Brill, J.P.: "Pressure Drop Correlations for 
         Inclined Two Phase Flow," Journal of Energy Resources Technology, 
         Vol. 107 (Dec. 1985), 549-554.

Dicionário de variáveis:
- angle             : Ângulo de inclinação do tubo (graus)
- diameter          : Diâmetro interno do tubo (polegadas)
- roughness         : Rugosidade relativa (adimensional)
- pressure          : Pressão (psi)
- velocity          : Velocidade da mistura (ft/s)
- liquidFraction    : Fração volumétrica de líquido sem escorregamento (0-1)
- gasDensity        : Densidade do gás (lb/ft³)
- liquidDensity     : Densidade do líquido (lb/ft³)
- gasViscosity      : Viscosidade do gás (cP)
- liquidViscosity   : Viscosidade do líquido (cP)
- liquidViscNumber  : Número adimensional de velocidade do líquido
- gasVelNumber      : Número adimensional de velocidade do gás
- liquidNumber      : Número adimensional de viscosidade do líquido
- holdup            : Holdup de líquido calculado (saída)
- frictionGrad      : Gradiente de pressão por atrito (psi/ft)
- gravityGrad       : Gradiente de pressão gravitacional (psi/ft)
- accelGrad         : Gradiente de pressão por aceleração (psi/ft)
- totalGrad         : Gradiente de pressão total (psi/ft)
- reynolds          : Número de Reynolds (saída)
- flowPattern       : Padrão de escoamento (saída)
- criticalFlag      : Flag de fluxo crítico (saída)
- convergeFlag      : Flag de convergência (saída)

Arrays de dados:
- frictionRatio[9]  : Razão de fator de atrito para escoamento de névoa
- holdupRatio[9]    : Razão de holdup para escoamento de névoa

Subprogramas utilizados:
- mukherjeeeBrillFlowPattern    : Determina padrão de escoamento
- mukherjeeeBrillHoldup         : Calcula holdup de líquido
- mukherjeeeBrillStratifiedFlow : Calcula gradientes para escoamento estratificado
==============================================================================
*/
void mukherjeeeBrill(double angle, double diameter, double roughness, double pressure, 
                    double velocity, double liquidFraction, double gasDensity, 
                    double liquidDensity, double gasViscosity, double liquidViscosity,
                    double surfaceTension, double temperature, double compressibilityFactor,
                    double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                    double& totalGrad, double& reynolds, unsigned char& flowPattern,
                    unsigned char& criticalFlag, unsigned char& convergeFlag) {
    
    

}


/*
==============================================================================
azizRegimeMap: Predição de padrão de fluxo usando mapa de Govier, Redford e Dunn
               para correlação de Aziz et al.

Dicionário de variáveis:
- liquidSupVel  : Velocidade superficial do líquido (ft/s)
- gasSupVel     : Velocidade superficial do gás (ft/s)
- liquidDensity : Densidade do líquido (lbm/ft³)
- gasDensity    : Densidade do gás (lbm/ft³)
- surfaceTension: Tensão superficial (lbm/s²)
- liquidFraction: Fração de holdup do líquido sem escorregamento
- surfaceTensionWaterAir : Tensão superficial água-ar (lbm/s²)
- bubbleToSlugBoundary      : Fronteira bolha-pistão
- slugToTransitionBoundary  : Fronteira pistão-transição
- transitionToMistBoundary  : Fronteira transição-névoa
- flowMapCoordinateX        : Coordenada X no mapa de padrão de fluxo
- flowMapCoordinateY        : Coordenada Y no mapa de padrão de fluxo
- flowPattern   : Indicador de padrão de fluxo
                 1-Líquido, 2-Gás, 3-Bolhas, 4-Pistão, 5-Névoa, 6-Transição
==============================================================================
*/
void azizRegimeMap(double liquidSupVel, double gasSupVel, double liquidDensity, double gasDensity, 
                  double surfaceTension, double liquidFraction,
                  double& bubbleToSlugBoundary, double& slugToTransitionBoundary, double& transitionToMistBoundary,
                  double& flowMapCoordinateX, double& flowMapCoordinateY, unsigned char& flowPattern) {
  

}

/*
==============================================================================
azizDensity: Calcula holdup de líquido e densidade deslizante para correlação
                         de Aziz, Govier & Fogarasi

Dicionário de variáveis:
- gasSupVel      : Velocidade superficial do gás (ft/s)
- velocity       : Velocidade superficial da mistura (ft/s) 
- surfaceTension : Tensão superficial (unidades do código)
- liquidDensity  : Densidade do líquido (lbm/ft³)
- gasDensity     : Densidade do gás (lbm/ft³)
- diameter       : Diâmetro interno do tubo (ft)
- liquidViscosity: Viscosidade do líquido (cp)
- transitionToMistBoundary : Limite X - fronteira transição-névoa
- flowMapCoordinateX: Coordenada X no mapa de padrão de fluxo
- slugToTransitionBoundary: Limite X - fronteira pistão-transição
- flowPattern    : Indicador do padrão de fluxo
                   1-Líquido, 2-Gás, 3-Bolhas, 4-Pistão, 5-Névoa, 6-Transição

Saídas / Variáveis de retorno:
- holdup            : Fração de holdup do líquido
- bubbleLength      : Comprimento da bolha (ft) 
- slugLength        : Comprimento da golfada (ft) 
- slipDensity       : Densidade deslizante (lbm/ft³)
- slugLiquidDensity : Densidade deslizante da golfada de líquido (lbm/ft³)
==============================================================================
*/

void azizDensity(double gasSupVel, double velocity, double surfaceTension,
                 double liquidDensity, double gasDensity, double diameter, double liquidViscosity,
                 double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary, unsigned char flowPattern,
                 double& holdup, double& bubbleLength, double& slugLength,
                 double& slipDensity, double& slugLiquidDensity) {

 
}

/*
==============================================================================
azizFriction: calcula o gradiente de atrito segundo a correlação de Aziz et al.

Dicionário de variáveis:
- diameter                    : diâmetro interno do tubo (ft)
- velocity                    : velocidade superficial da mistura (ft/s)
- gasSupVel                   : velocidade superficial do gás (ft/s)
- liquidDensity               : densidade do líquido (lbm/ft³)
- gasDensity                  : densidade do gás (lbm/ft³)
- liquidViscosity             : viscosidade do líquido (cP)
- gasViscosity                : viscosidade do gás (cP)
- surfaceTension              : tensão superficial (unidades do código)
- roughness                   : rugosidade relativa (adimensional)
- mixtureDensity              : densidade da mistura (no-slip) (lbm/ft³)
- slugMixtureDensity          : densidade média da mistura na região slug (lbm/ft³)
- bubbleLength                : comprimento da bolha (ft)
- slugLength                  : comprimento da golfada (ft)
- transitionToMistBoundary    : coordenada X de transição (limite slug->mist)
- flowMapCoordinateX          : coordenada X usada no mapa de fluxo (adimensional)
- slugToTransitionBoundary    : coordenada X do limite slug->transição
- reynolds                    : número de Reynolds calculado (retornado por referência)
- flowPattern                 : indicador do padrão de fluxo (1=líquido,2=gás,3=bolhas,4=slug,5=névoa,6=transição)

Saídas / parâmetros de saída:
- frictionGrad                : gradiente de pressão por atrito (psi/ft)
- reynolds                    : número de Reynolds calculado (por referência


Variáveis locais principais:
- frictionFactorValue         : fator de atrito de Darcy/Moody (adimensional)
- reynoldsGas                 : número de Reynolds do gás (quando aplicado)
- mistRoughnessParameter      : parâmetro auxiliar usado para cálculo de rugosidade efetiva na névoa
- effectiveRoughness          : rugosidade relativa efetiva usada no cálculo do fator de atrito (adimensional)
- slugFrictionTerm            : gradiente de atrito como valor médio ponderado para golfadas(psi/ft)
- mistFrictionTerm            : gradiente de atrito como valor médio ponderado para névoa (psi/ft)
==============================================================================
*/

void azizFriction(double diameter, double velocity, double gasSupVel,
                  double liquidDensity, double gasDensity, double liquidViscosity,
                  double gasViscosity, double surfaceTension, double roughness,
                  double mixtureDensity, double slugMixtureDensity,
                  double bubbleLength, double slugLength,
                  double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary,
                  double& reynolds, unsigned char flowPattern,
                  double& frictionGrad) {




}

/*
==============================================================================
aziz: Implementa a lógica principal da correlação de Aziz, Govier & Fogarasi

Dicionário de variáveis:
- angle          : ângulo de inclinação do tubo (graus)
- diameter       : diâmetro interno do tubo (ft)
- roughness      : rugosidade relativa (adimensional)
- pressure       : pressão (psia)
- velocity       : velocidade superficial da mistura (ft/s)
- liquidFraction : fração de holdup no-slip (ENS)
- gasDensity     : densidade do gás (lbm/ft³)
- liquidDensity  : densidade do líquido (lbm/ft³)
- gasViscosity   : viscosidade do gás (cP)
- liquidViscosity: viscosidade do líquido (cP)
- surfaceTension : tensão superficial gás-líquido
- holdup               : fração de holdup calculada
- frictionGrad         : gradiente por fricção
- gravityGrad          : gradiente por gravidade 
- accelerationGrad     : gradiente por aceleração
- totalGrad            : gradiente total
- reynolds             : número de Reynolds calculado
- flowPattern          : padrão de fluxo
- criticalFlag         : indicador de escoamento crítico
==============================================================================
*/
void aziz(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity, double surfaceTension,
          double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad, double& totalGrad,
          double& reynolds, unsigned char& flowPattern, unsigned char& criticalFlag) {


 
}

/*
==============================================================================
olie: Correlação de Oliemans

Dicionário de variáveis:
- angle             : Ângulo de inclinação do tubo (graus).
- diameter          : Diâmetro interno do tubo (polegadas).
- roughness         : Rugosidade relativa (adimensional). 
- pressure          : Pressão absoluta no ponto (psia).
- velocity          : Velocidade média da mistura (ft/s).
- liquidFraction    : Fração volumétrica de líquido na mistura (0-1).
- gasDensity        : Densidade do gás (lb/ft³).
- liquidDensity     : Densidade do líquido (lb/ft³).
- gasViscosity      : Viscosidade do gás (cP).
- liquidViscosity   : Viscosidade do líquido (cP).
- liquidNumber      : Número adimensional de líquido.
- liquidViscNumber  : Número adimensional de viscosidade de líquido.
- gasVelNumber      : Número adimensional de velocidade de gás.
- tubeNumber        : Número adimensional do tubo.

- holdup            : Holdup de líquido calculado.
- frictionGrad      : Gradiente de pressão por atrito (psi/ft).
- gravityGrad       : Gradiente de pressão por elevação/peso (psi/ft).
- totalGrad         : Gradiente total de pressão (psi/ft).
- reynolds          : Número de Reynolds (adimensional).
- flowPattern       : Padrão de escoamento detectado (flag).
- correlationFlag   : Flag de escolha/correção de holdup / seleção de correlação.
- criticalFlag      : Flag crítico de estabilidade.
==============================================================================
*/
void olie(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity,
          double surfaceTension, double temperature, double compressibilityFactor,
          double& holdup, double& frictionGrad, double& gravityGrad,
          double& totalGrad, double& reynolds, unsigned char& flowPattern, unsigned char correlationFlag,
          unsigned char& criticalFlag) {



}

/*
==============================================================================
gray: Correlação de Gray para poços de gás com produção de líquido

Referências:
      1. Gray, H.E.: "Vertical Flow Correlation in Gas Wells"
         User's Manual for API 14B, Subsurface Controlled Safety Valve Sizing Computer Program
         Appendix B (June 1974).

Dicionário de variáveis:
- angle              : Ângulo de escoamento em relação à horizontal (graus)
- diameter           : Diâmetro interno da tubulação (polegadas)
- roughness          : Rugosidade relativa da tubulação (adimensional)
- pressure           : Pressão absoluta (psia)
- velocity           : Velocidade superficial da mistura (ft/s)
- liquidFraction     : Fração de holdup de líquido no-slip (adimensional, 0-1)
- waterFraction      : Fração mássica de água na fase líquida (adimensional, 0-1)
- gasDensity         : Densidade do gás (lb/ft³)
- liquidDensity      : Densidade do líquido (lb/ft³)
- gasViscosity       : Viscosidade do gás (cP)
- liquidViscosity    : Viscosidade do líquido (cP)
- oilSurfaceTension  : Tensão superficial do óleo (dyn/cm)
- waterSurfaceTension: Tensão superficial da água (dyn/cm)
- holdup             : Fração de holdup de líquido calculada (adimensional, 0-1). Saída.
- frictionGrad       : Gradiente de pressão por atrito (psi/ft). Saída.
- gravityGrad        : Gradiente de pressão gravitacional (psi/ft). Saída.
- accelerationGrad   : Gradiente de pressão de aceleração (psi/ft). Saída.
- totalGrad          : Gradiente de pressão total (psi/ft). Saída.
- reynolds           : Número de Reynolds (adimensional). Saída.
- flowPattern        : Indicador de padrão de escoamento. Saída.
                       1 - líquido monofásico, 2 - gás monofásico, 3 - bifásico
- criticalFlag       : Indicador de escoamento crítico. Saída.
                       0 - se accelParam <= 0.95, 1 - se accelParam > 0.95


Variáveis internas:
- angleRad           : Ângulo de escoamento em relação à horizontal (radianos)
- liquidSupVel       : Velocidade superficial do líquido (ft/s)
- gasSupVel          : Velocidade superficial do gás (ft/s)
- noSlipDensity      : Densidade da mistura no-slip (lb/ft³)
- slipDensity        : Densidade da mistura com escorregamento (lb/ft³)
- liquidSurfTension  : Tensão superficial do líquido (dyn/cm)
- effectiveRoughness : Rugosidade efetiva para escoamento bifásico (adimensional)
- velocityNumber     : Número adimensional de velocidade (Nv)
- pipeNumber         : Número adimensional do tubo (Np)
- liquidGasRatio     : Razão de velocidades líquido/gás (R)
- exponentB          : Expoente B da correlação de holdup
- pseudoRoughness    : Pseudo-rugosidade baseada em tensão superficial (ft)
- transitionRoughness: Rugosidade de transição calculada (ft)
- effectiveViscosity : Viscosidade efetiva da mistura (cP)
- frictionFactor     : Fator de atrito de Moody (adimensional)
- accelParam         : Termo de energia cinética/aceleração (adimensional)
==============================================================================
*/
void gray(double angle, double diameter, double roughness, double pressure, double velocity, 
          double liquidFraction, double waterFraction, double gasDensity, double liquidDensity, 
          double gasViscosity, double liquidViscosity, double oilSurfaceTension, double waterSurfaceTension,
          double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
          double& totalGrad, double& reynolds, unsigned char& flowPattern, unsigned char& criticalFlag) {
    

}

