#include "SisProdTrendOutput.h"
#include "SisProdTrendOutputInternal.h"

#include "Leitura.h"
#include "SimulationContext.h"

#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>

namespace trendoutput {
namespace {

// ---------------------------------------------------------------- captions --

// Order is the file format: these are emitted left to right, and the row
// writers rely on the data columns arriving in exactly this sequence.
constexpr OptionalColumn<detTRENDP> kProductionColumns[] = {
    {&detTRENDP::pres, " Pressao (kgf/cm2) ;", " Pressure (kgf/cm2) ;"},
    {&detTRENDP::temp, " Temperatura (C) ;", " Temperature (C) ;"},
    {&detTRENDP::hol, " Holdup de liquido (-) ;", " Liquid holdup (-) ;"},
    {&detTRENDP::FVH, " Fracao Volumetrica Hidrato (-) ;", " Hydrate volumetric fraction (-) ;"},
    {&detTRENDP::bet, " Fracao vol. de liquido complementar (-) ;", " Complementary liquid vol. fraction (-) ;"},
    {&detTRENDP::ugs, " Velocidade superficial do gas (m/s) ;", " Gas superficial velocity (m/s) ;"},
    {&detTRENDP::uls, " Velocidade superficial do liquido (m/s) ;", " Liquid superficial velocity (m/s) ;"},
    {&detTRENDP::ug, " Velocidade do gas (m/s) ;", " Gas velocity (m/s) ;"},
    {&detTRENDP::ul, " Velocidade do liquido (m/s) ;", " Liquid velocity (m/s) ;"},
    {&detTRENDP::arra, " Indicador de arranjo de fases (-) ;", " Phase pattern indicator (-) ;"},
    {&detTRENDP::viscl, " Viscosidade do Liquido (cP) ;", " Liquid viscosity (cP) ;"},
    {&detTRENDP::viscg, " Viscosidade do Gas (cP) ;", " Gas viscosity (cP) ;"},
    {&detTRENDP::rhog, " Massa Especifica do Gas (kg/m3) ;", " Gas density (kg/m3) ;"},
    {&detTRENDP::rhol, " Massa Especifica do Liquido (kg/m3) ;", " Liquid density (kg/m3) ;"},
    {&detTRENDP::rhoMix, " Massa Especifica da Mistura (kg/m3) ;", " Mixture density (kg/m3) ;"},
    {&detTRENDP::masg, " Vazao Massica do Gas (kg/s) ;", " Gas mass flow rate (kg/s) ;"},
    {&detTRENDP::masl, " Vazao Massica do Liquido (kg/s) ;", " Liquid mass flow rate (kg/s) ;"},
    {&detTRENDP::c0, " Coeficiente de distribuição: C0 (-) ;", " Distribution coefficient: C0 (-) ;"},
    {&detTRENDP::ud, " Velocidade de escorregamento: Ud (m/s) ;", " Slip velocity: Ud (m/s) ;"},
    {&detTRENDP::RGO, " RGO (Sm3/Sm3) ;", " RGO (Sm3/Sm3) ;"},
    {&detTRENDP::deng, " Densidade do Gas (-) ;", " Gas specific gravity (-) ;"},
    {&detTRENDP::yco2, " Fracao Molar de CO2 (-) ;", " CO2 molar fraction (-) ;"},
    {&detTRENDP::calor, " Fluxo de calor entre escoamento e parede (W/m) ;", " Heat flow between flow and wall (W/m) ;"},
    {&detTRENDP::masstrans, " Transferencia de Massa entre Fases (kg / [s m]) ;", " Interphase mass transfer (kg / [s m]) ;"},
    {&detTRENDP::qlst, " Vazao volumetrica standard de oleo morto (Sm3/d) ;", " Standard dead oil volumetric flow rate (Sm3/d) ;"},
    {&detTRENDP::qlwst, " Vazao volumetrica standard de oleo morto + agua (Sm3/d) ;", " Standard dead oil + water volumetric flow rate (Sm3/d) ;"},
    {&detTRENDP::qlstTot, " Vazao volumetrica standard de oleo morto + agua + liquido complementar (Sm3/d) ;", " Standard dead oil + water + complementary liquid volumetric flow rate (Sm3/d) ;"},
    {&detTRENDP::qgst, " Vazao volumetrica standard de gas livre + dissolvido (Sm3/d) ;", " Standard free + dissolved gas volumetric flow rate (Sm3/d) ;"},
    {&detTRENDP::api, " Grau API (-) ;", " API gravity (-) ;"},
    {&detTRENDP::bsw, " BSW (-) ;", " BSW (-) ;"},
    {&detTRENDP::hidro, " Termo Hidrostatico (Pa/m) ;", " Hydrostatic term (Pa/m) ;"},
    {&detTRENDP::fric, " Termo Friccao (Pa/m) ;", " Friction term (Pa/m) ;"},
    {&detTRENDP::dengD, " Densidade Gas Dissolvido In Situ (-) ;", " In-situ dissolved gas specific gravity (-) ;"},
    {&detTRENDP::dengL, " Densidade Gas Livre In Situ (-) ;", " In-situ free gas specific gravity (-) ;"},
    {&detTRENDP::mlFonte, " Fonte massica - Liq. (Hidrocarb+Agua) (kg/s);", " Mass source - Liq. (Hydrocarbon+Water) (kg/s);"},
    {&detTRENDP::mgFonte, " Fonte massica - Gas (kg/s);", " Mass source - Gas (kg/s);"},
    {&detTRENDP::mcFonte, " Fonte massica - Liq. Complementar (kg/s);", " Mass source - Complementary liquid (kg/s);"},
    {&detTRENDP::dpB, " Incremento de pressao de Bombeio (kgf/cm2);", " Pump pressure increment (kgf/cm2);"},
    {&detTRENDP::potB, " Potencia de Bombeio (kW);", " Pump power (kW);"},
    {&detTRENDP::tempChokeJus, " Temperatura a Jusante do Choke de Superficie (C);", " Surface choke downstream temperature (C);"},
    {&detTRENDP::reyi, " Reynolds interno da mistura (-) ;", " Internal mixture Reynolds (-) ;"},
    {&detTRENDP::reye, " Reynolds externo (-) ;", " External Reynolds (-) ;"},
    {&detTRENDP::Fr, " Froud (-) ;", " Froude (-) ;"},
    {&detTRENDP::grashi, " Grashof interno da mistura (-) ;", " Internal mixture Grashof (-) ;"},
    {&detTRENDP::grashe, " Grashof externo (-) ;", " External Grashof (-) ;"},
    {&detTRENDP::nusi, " Nusselt interno da mistura (-) ;", " Internal mixture Nusselt (-) ;"},
    {&detTRENDP::nuse, " Nusselt externo (-) ;", " External Nusselt (-) ;"},
    {&detTRENDP::hi, " Coeficiente de pelicula interno da mistura (W/(m2.K)) ;", " Internal mixture film coefficient (W/(m2.K)) ;"},
    {&detTRENDP::he, " Coeficiente de pelicula externo (W/(m2.K)) ;", " External film coefficient (W/(m2.K)) ;"},
    {&detTRENDP::pri, " Prandtl interno da mistura (-) ;", " Internal mixture Prandtl (-) ;"},
    {&detTRENDP::pre, " Prandtl externo (-) ;", " External Prandtl (-) ;"},
    {&detTRENDP::Rs, " Razao de Solubilidade (-) ;", " Solubility ratio (-) ;"},
    {&detTRENDP::Bo, " Fator Volume de Formacao (-) ;", " Formation volume factor (-) ;"},
    {&detTRENDP::volMonM1PT, " Volume de liquido a montante da Master1, a PT, m3 ;", " Liquid volume upstream of Master1, at PT, m3 ;"},
    {&detTRENDP::volJusM1PT, " Volume de liquido a jusante da Master1, a PT, m3 ;", " Liquid volume downstream of Master1, at PT, m3 ;"},
    {&detTRENDP::volMonM1ST, " Volume de liquido a montante da Master1, standard, m3 ;", " Liquid volume upstream of Master1, standard, m3 ;"},
    {&detTRENDP::volJusM1ST, " Volume de liquido a jusante da Master1, standard, m3 ;", " Liquid volume downstream of Master1, standard, m3 ;"},
    {&detTRENDP::inventarioGas, " Inventario de Gas em toda a tubulação, standard, m3 ;", " Gas inventory in whole tubing, standard, m3 ;"},
    {&detTRENDP::inventarioLiq, " Inventario de Liquido em toda a tubulação, standard, m3 ;", " Liquid inventory in whole tubing, standard, m3 ;"},
    {&detTRENDP::diamInt, " Diametro Interno da tubulacao, m ;", " Tubing inner diameter, m ;"},
    {&detTRENDP::TempParede, " Temperatura Interna da Parede, C ;", " Internal wall temperature, C ;"},
    {&detTRENDP::subResfria, " Subresfriamento, C ;", " Subcooling, C ;"},
    {&detTRENDP::presAnulICV, " Pressao a montante da ICV, kgf/cm2 ;", " downstream ICV pressure, kgf/cm2 ;"},
    {&detTRENDP::caixaValvula, " Pressao caixa de valvula (kgf/cm2) C;", " Box valve pressure (kgf/cm2) C;"},
    {&detTRENDP::caixaValvula, " Temperatura caixa de valvula (C) C;", " Box valve temperature (C) C;"},
    {&detTRENDP::caixaValvula, " Fracao de vazio caixa de valvula (-) C;", " Box valve void fraction (-) C;"},
    {&detTRENDP::caixaValvula, " Fracao de liquido complementar caixa de valvula (-) C;", " Box valve Complementary liquid fraction (-) C;"},
    {&detTRENDP::caixaValvula, " Titulo caixa de valvula (-) C;", " Box valve gas mass fraction(-) C;"},
    {&detTRENDP::caixaValvula, " Massa de fluidos caixa de valvula (kg) C;", " Box valve fluid mass(kg) C;"},
    {&detTRENDP::caixaValvula, " Massa de liquido caixa de valvula (kg) C;", " Box valve liquid mass(kg) C;"},
    {&detTRENDP::caixaValvula, " Massa de gas caixa de valvula (kg) C;", " Box valve gas mass(kg) C;"},
};

constexpr OptionalColumn<detTRENDG> kGasLineColumns[] = {
    {&detTRENDG::pres, " Pressao (kgf/cm2) ;", " Pressure (kgf/cm2) ;"},
    {&detTRENDG::temp, " Temperatura (C) ;", " Temperature (C) ;"},
    {&detTRENDG::ugs, " Velocidade superficial do gas (m/s) ;", " Gas superficial velocity (m/s) ;"},
    {&detTRENDG::ug, " Velocidade do gas (m/s) ;", " Gas velocity (m/s) ;"},
    {&detTRENDG::tens, " Tensao Cisalhante (N/m2) ;", " Shear stress (N/m2) ;"},
    {&detTRENDG::viscg, " Viscosidade do Gas (cP) ;", " Gas viscosity (cP) ;"},
    {&detTRENDG::rhog, " Massa Especifica do Gas (kg/m3) ;", " Gas density (kg/m3) ;"},
    {&detTRENDG::masg, " Vazao Massica do Gas (kg/s) ;", " Gas mass flow rate (kg/s) ;"},
    {&detTRENDG::hidro, " Termo Hidrostatico (Pa/m) ;", " Hydrostatic term (Pa/m) ;"},
    {&detTRENDG::FVHG, " FVHG (-) ;", " FVHG (-) ;"},
    {&detTRENDG::fric, " Termo Friccao (Pa/m) ;", " Friction term (Pa/m) ;"},
    {&detTRENDG::calor, " Fluxo de calor entre escoamento e parede (W/m) ;", " Heat flow between flow and wall (W/m) ;"},
    {&detTRENDG::qgst, " Vazao volumetrica standard de Gas (Sm3/d) ;", " Standard gas volumetric flow rate (Sm3/d) ;"},
    {&detTRENDG::pEstagVGL, " Pressao de Estagnacao VGL (kgf/cm²) ;", " VGL stagnation pressure (kgf/cm2) ;"},
    {&detTRENDG::tEstagVGL, " Temperatura de Estagnacao VGL (C) ;", " VGL stagnation temperature (C) ;"},
    {&detTRENDG::pGargVGL, " Pressao na Garganta VGL (kgf/cm²) ;", " VGL throat pressure (kgf/cm2) ;"},
    {&detTRENDG::tGargVGL, " Temperatura na Garganta VGL (C) ;", " VGL throat temperature (C) ;"},
    {&detTRENDG::velgarg, " Velocidade na VGL (m/s) ;", " Velocity in VGL (m/s) ;"},
    {&detTRENDG::qVGL, " Vazao volumetrica na VGL (mÂ³/s) ;", " Volumetric flow rate in VGL (m3/s) ;"},
    {&detTRENDG::reyi, " Reynolds interno (-) ;", " Internal Reynolds (-) ;"},
    {&detTRENDG::reye, " Reynolds externo (-) ;", " External Reynolds (-) ;"},
    {&detTRENDG::grashi, " Grashof interno (-) ;", " Internal Grashof (-) ;"},
    {&detTRENDG::grashe, " Grashof externo (-) ;", " External Grashof (-) ;"},
    {&detTRENDG::nusi, " Nusselt interno (-) ;", " Internal Nusselt (-) ;"},
    {&detTRENDG::nuse, " Nusselt externo (-) ;", " External Nusselt (-) ;"},
    {&detTRENDG::hi, " Coeficiente de pelicula interno (W/[m2 K]) ;", " Internal film coefficient (W/[m2 K]) ;"},
    {&detTRENDG::he, " Coeficiente de pelicula externo (W/[m2 K]) ;", " External film coefficient (W/[m2 K]) ;"},
    {&detTRENDG::pri, " Prandtl interno (-) ;", " Internal Prandtl (-) ;"},
    {&detTRENDG::pre, " Prandtl externo (-) ;", " External Prandtl (-) ;"},
    {&detTRENDG::diamInt, " Diametro Interno da tubulacao, m ;", " Tubing inner diameter, m ;"},
    {&detTRENDG::TempParede, " Temperatura Interna da Parede, C ;", " Internal wall temperature, C ;"},
    {&detTRENDG::subResfria, " Subresfriamento, C ;", " Subcooling, C ;"},
};

// Written as one group when dadosParafina is set.
constexpr FixedColumn kWaxColumns[] = {
    {" TIAC (C) C;", " TIAC (C) C;"},
    {" Cp Parafina (J/[kg C]) C;", " Paraffin Cp (J/[kg C]) C;"},
    {" Condutividade Termica Parafina (W / [m K]) C;", " Paraffin thermal conductivity (W / [m K]) C;"},
    {" Massa Especifica Parafina (kg/m3) C;", " Paraffin density (kg/m3) C;"},
    {" Massa molar do Liquido Parafina (kg/mol) C;", " Paraffin liquid molar mass (kg/mol) C;"},
    {" Difusividade Massica Parafina (m2/s) C;", " Paraffin mass diffusivity (m2/s) C;"},
    {" Fluxo Massico de Parafina Total (kg/(m2-s)) C;", " Total paraffin mass flux (kg/(m2-s)) C;"},
    {" Fluxo Massico de Parafina por Difusao (kg/(m2-s)) C;", " Paraffin diffusive mass flux (kg/(m2-s)) C;"},
    {" Vazao Massica de Parafina por Difusao (kg/(s)) C;", " Paraffin mass rate (kg/(s)) C;"},
    {" Gradiente de concentracao de parafina (1/m) C;", " Paraffin concentration gradient (1/m) C;"},
    {" Condutividade do deposito (W/(m-K)) C;", " Deposit conductivity (W/(m-K)) C;"},
    {" Temperatura da Interface do deposito (C) C;", " Deposit interface temperature (C) C;"},
};

template <typename Trend, std::size_t Count>
void writeOptionalColumns(std::ostream &trendFile, const CaptionTranslator &translate,
                          const Trend &trend,
                          const OptionalColumn<Trend> (&columns)[Count]) {
    for (const OptionalColumn<Trend> &column : columns)
        if (trend.*column.enabled == 1)
            trendFile << translate(column.ptBrText, column.enText);
}

template <std::size_t Count>
void writeFixedColumns(std::ostream &trendFile, const CaptionTranslator &translate,
                       const FixedColumn (&columns)[Count]) {
    for (const FixedColumn &column : columns)
        trendFile << translate(column.ptBrText, column.enText);
}

// The three wave families the characteristic analysis reports.
constexpr int kWaveFamilyCount = 3;

void writeWaveCelerityColumns(std::ostream &trendFile, const CaptionTranslator &translate) {
    for (int waveFamily = 0; waveFamily < kWaveFamilyCount; waveFamily++) {
        trendFile << translate(" Celeridade, familia de onda ", " Celerity, wave family ") << waveFamily << translate(" m/s ;", " m/s ;");
    }
}

void writeEigenvectorColumns(std::ostream &trendFile, const CaptionTranslator &translate) {
    for (int waveFamily = 0; waveFamily < kWaveFamilyCount; waveFamily++) {
        for (int eigenvectorTerm = 0; eigenvectorTerm < kWaveFamilyCount; eigenvectorTerm++)
            trendFile << translate(" Componente do autovetor, condicao adiabatica, familia de onda = ", " Eigenvector component, adiabatic condition, wave family = ") << waveFamily << translate("termo = ", " term = ") << eigenvectorTerm << " ;";
    }
}

void writeFluctuationColumns(std::ostream &trendFile, const CaptionTranslator &translate) {
    for (int waveFamily = 0; waveFamily < kWaveFamilyCount; waveFamily++) {
        trendFile << translate(" Componente de flutuacao da familia de onda ", " Fluctuation component of wave family ") << waveFamily << " ;";
    }
}

template <typename Cell>
double lengthFromOrigin(const Cell *cells, int lastCellIndex) {
    double length = 0;
    // Accumulate in mesh order to preserve the floating-point result.
    for (int cell = 0; cell <= lastCellIndex; cell++)
        length += cells[cell].dx;
    return length;
}

template <typename Trend, typename Cell>
void writeLineTrendMetadata(std::ostream &trendFile, const CaptionTranslator &translate,
                            const TrendState &state, const Trend &trend,
                            const Cell *cells, const char *originPtBrText,
                            const char *originEnText) {
    const double length = lengthFromOrigin(cells, trend.posic);
    trendFile << translate("# Versao ", "# Version ") << state.engineVersion
              << translate(" # arquivo = ", " # file = ")
              << state.input.absoluteInputFilePath() << std::endl;
    trendFile << translate(originPtBrText, originEnText) << length << std::endl;
    trendFile << translate("# Rotulo = ", "# Label = ") << trend.rotulo << std::endl;
    trendFile << translate("# Indice da Celula = ", "# Cell index = ") << trend.posic << std::endl;
    if (state.branchIndex < 0 && state.input.AP == 1)
        trendFile << translate(" Sequencia AP ;", " SA sequence ;");
    trendFile << translate(" Tempo (s) ;", " Time (s) ;");
}

void writeCrossSectionCaptions(std::ostream &trendFile, const CaptionTranslator &translate,
                               const detTRENDTrans &trend) {
    trendFile << translate("# Rotulo = ", "# Label = ") << trend.rotulo << std::endl;
    trendFile << translate(" Tempo (s) ; ", " Time (s) ; ");
    trendFile << translate(" Temperatura (C) ;", " Temperature (C) ;");
}

// ------------------------------------------------------------- file names --

std::string lineTrendFileName(const TrendState &state, const char *prefix,
                         int position, int networkIndex) {
    std::ostringstream fileNameStream;
    fileNameStream << pathPrefixoArqSaida;
    if (state.branchIndex < 0)
        fileNameStream << prefix << (state.input.AP == 1 ? "-AP-" : "-") << position;
    else
        fileNameStream << "Tramo" << state.branchIndex << "-R-" << networkIndex
                       << "-" << prefix << "-" << position;
    fileNameStream << ".dat";
    return fileNameStream.str();
}

CrossSectionPosition crossSectionPositionOf(const detTRENDTrans &trend) {
    return {trend.comp, trend.camada - 1, trend.discre - 1};
}

std::string crossSectionTrendFileName(const TrendState &state, const char *prefix,
                                 const CrossSectionPosition &position) {
    std::ostringstream fileNameStream;
    fileNameStream << pathPrefixoArqSaida;
    if (state.branchIndex >= 0)
        fileNameStream << "Tramo" << state.branchIndex << "-";
    fileNameStream << prefix << "-" << position.positionMetres << "-" << position.layer
                   << "-" << position.discretization << ".dat";
    return fileNameStream.str();
}

// --------------------------------------------------------------- reporting --

void reportProducedFile(const TrendState &state, const std::string &fileName) {
    // unless this is an injection-well (POCO_INJETOR) simulation
    if (state.input.tipoSimulacao != tipoSimulacao_t::poco_injetor) {
        arqRelatorioPerfis << fileName.c_str() << std::endl;
        arqRelatorioPerfis.flush();
    }
}

// --------------------------------------------------------------- file output --

template <typename WriteContent>
void produceTrendFile(const TrendState &state, const std::string &fileName,
                      std::ios_base::openmode mode, bool blankLineBeforeClose,
                      WriteContent writeContent) {
    std::ofstream trendFile(fileName.c_str(), mode);
    writeContent(trendFile);
    if (blankLineBeforeClose)
        trendFile << std::endl;
    trendFile.close();
    reportProducedFile(state, fileName);
}

int pendingRowCount(const TrendSeries &series, int trendIndex) {
    return series.count[trendIndex] - series.countBase[trendIndex] + 1;
}

SampleWindow windowOf(const TrendSeries &series, int trendIndex, int columnCount) {
    return {series.samples[trendIndex], series.countBase[trendIndex],
            pendingRowCount(series, trendIndex), columnCount};
}

// Time values at or below this limit stop the current sample scan.
constexpr double kEndOfSamplesMarker = -9999;

// Field width and precision of every value written to a trend file.
constexpr int kValueWidth = 20;
constexpr int kValuePrecision = 19;

// The two columns every cross-section trend carries: time and temperature.
constexpr int kCrossSectionColumnCount = 2;

template <typename WriteCaptions>
void writeTrendHeaderFile(const TrendState &state, const std::string &fileName,
                          WriteCaptions writeCaptions, bool blankLineBeforeClose) {
    produceTrendFile(state, fileName, std::ios_base::out, blankLineBeforeClose,
                     [&](std::ofstream &trendFile) {
                         if (state.printPassCount == 1) {
                             writeCaptions(trendFile);
                             trendFile << std::endl;
                         }
                     });
}

void writeTrendRowsFile(const TrendState &state, const std::string &fileName,
                        const SampleWindow &window, bool supportsApSequence,
                        bool blankLineBeforeClose) {
    produceTrendFile(
        state, fileName, std::ios_base::app, blankLineBeforeClose,
        [&](std::ofstream &trendFile) {
            const bool apSequenceColumn =
                supportsApSequence && state.branchIndex < 0 && state.input.AP == 1;

            trendFile.precision(kValuePrecision);
            for (int rowIndex = 0; rowIndex < window.rowCount; rowIndex++) {
                const double *sampleRow = window.samples[window.firstRow + rowIndex];
                if (sampleRow[0] <= kEndOfSamplesMarker)
                    break;
                if (apSequenceColumn) {
                    trendFile.width(kValueWidth);
                    trendFile << (*state.globals).parametricCaseIndex << " ; ";
                }
                for (int columnIndex = 0; columnIndex < window.columnCount; columnIndex++) {
                    trendFile.width(kValueWidth);
                    trendFile << sampleRow[columnIndex] << " ; ";
                }
                trendFile << std::endl;
            }
        });
}

} // namespace

// ------------------------------------------------------------ entry points --

void writeProductionTrendHeader(const TrendState &state, int trendIndex, int networkIndex) {
    if (state.input.ntendp > 0) {
        const CaptionTranslator translate{state.input.idiomaSaida};
        const detTRENDP &trend = state.input.trendp[trendIndex];
        writeTrendHeaderFile(
            state, lineTrendFileName(state, "TENDP", trend.comp, networkIndex),
            [&](std::ofstream &trendFile) {
                writeLineTrendMetadata(trendFile, translate, state, trend,
                                       state.input.celp,
                                       "# Comprimento a partir do Fundo de Poco (m) = ",
                                       "# Length from Bottomhole (m) = ");
                writeOptionalColumns(trendFile, translate, trend, kProductionColumns);
                if (trend.dadosParafina == 1)
                    writeFixedColumns(trendFile, translate, kWaxColumns);
                if (trend.autoVal == 1)
                    writeWaveCelerityColumns(trendFile, translate);
                if (trend.autoVel == 1)
                    writeEigenvectorColumns(trendFile, translate);
                if (trend.flutuacao == 1)
                    writeFluctuationColumns(trendFile, translate);
            },
            /*blankLineBeforeClose=*/false);
    }
}

void writeProductionTrendRows(const TrendState &state, int trendIndex, int networkIndex) {
    if (state.input.ntendp > 0) {
        const detTRENDP &trend = state.input.trendp[trendIndex];
        writeTrendRowsFile(
            state, lineTrendFileName(state, "TENDP", trend.comp, networkIndex),
            windowOf(state.production, trendIndex, state.input.nvartrendp[trendIndex] + 1),
            /*supportsApSequence=*/true, /*blankLineBeforeClose=*/false);
    }
}

void writeGasLineTrendHeader(const TrendState &state, int trendIndex, int networkIndex) {
    if (state.input.ntendg > 0 && state.input.lingas > 0) {
        const CaptionTranslator translate{state.input.idiomaSaida};
        const detTRENDG &trend = state.input.trendg[trendIndex];
        writeTrendHeaderFile(
            state, lineTrendFileName(state, "TENDG", trend.comp, networkIndex),
            [&](std::ofstream &trendFile) {
                writeLineTrendMetadata(trendFile, translate, state, trend,
                                       state.input.celg,
                                       "# Comprimento a partir da Plataforma (m) = ",
                                       "# Length from Platform (m) = ");
                writeOptionalColumns(trendFile, translate, trend, kGasLineColumns);
            },
            /*blankLineBeforeClose=*/true);
    }
}

void writeGasLineTrendRows(const TrendState &state, int trendIndex, int networkIndex) {
    if (state.input.ntendg > 0 && state.input.lingas > 0) {
        const detTRENDG &trend = state.input.trendg[trendIndex];
        writeTrendRowsFile(
            state, lineTrendFileName(state, "TENDG", trend.comp, networkIndex),
            windowOf(state.gasLine, trendIndex, state.input.nvartrendg[trendIndex] + 1),
            /*supportsApSequence=*/true, /*blankLineBeforeClose=*/false);
    }
}

void writeProductionCrossSectionTrendHeader(const TrendState &state, int trendIndex) {
    if (state.input.ntendtransp > 0) {
        const CaptionTranslator translate{state.input.idiomaSaida};
        const detTRENDTrans &trend = state.input.trendtransp[trendIndex];
        writeTrendHeaderFile(
            state,
            crossSectionTrendFileName(state, "TENDTRANSP", crossSectionPositionOf(trend)),
            [&](std::ofstream &trendFile) { writeCrossSectionCaptions(trendFile, translate, trend); },
            /*blankLineBeforeClose=*/false);
    }
}

void writeProductionCrossSectionTrendRows(const TrendState &state, int trendIndex) {
    if (state.input.ntendtransp > 0) {
        const detTRENDTrans &trend = state.input.trendtransp[trendIndex];
        writeTrendRowsFile(
            state,
            crossSectionTrendFileName(state, "TENDTRANSP", crossSectionPositionOf(trend)),
            windowOf(state.productionCrossSection, trendIndex, kCrossSectionColumnCount),
            /*supportsApSequence=*/false, /*blankLineBeforeClose=*/true);
    }
}

void writeGasLineCrossSectionTrendHeader(const TrendState &state, int trendIndex) {
    if (state.input.ntendtransg > 0 && state.input.lingas > 0) {
        const CaptionTranslator translate{state.input.idiomaSaida};
        const detTRENDTrans &trend = state.input.trendtransg[trendIndex];
        writeTrendHeaderFile(
            state,
            crossSectionTrendFileName(state, "TENDTRANSG", crossSectionPositionOf(trend)),
            [&](std::ofstream &trendFile) { writeCrossSectionCaptions(trendFile, translate, trend); },
            /*blankLineBeforeClose=*/false);
    }
}

void writeGasLineCrossSectionTrendRows(const TrendState &state, int trendIndex) {
    if (state.input.ntendtransg > 0 && state.input.lingas > 0) {
        const detTRENDTrans &trend = state.input.trendtransg[trendIndex];
        writeTrendRowsFile(
            state,
            crossSectionTrendFileName(state, "TENDTRANSG", crossSectionPositionOf(trend)),
            windowOf(state.gasLineCrossSection, trendIndex, kCrossSectionColumnCount),
            /*supportsApSequence=*/false, /*blankLineBeforeClose=*/true);
    }
}

} // namespace trendoutput
