#ifndef SISPRODTRENDOUTPUT_H_
#define SISPRODTRENDOUTPUT_H_

#include <string>

/// Writes buffered time series and input-mesh metadata for production and service lines.
/// Samples and configuration are read without modifying simulation state or advancing counters.
/// File names use pathPrefixoArqSaida. Each enabled call closes its file before recording its
/// name in arqRelatorioPerfis, except for injection-well simulations.

class Ler;
struct SimulationContext;

namespace trendoutput {

/// Sample buffers and scan counters for one family of time series.
struct TrendSeries {
    /// Samples indexed by [trend][row][column]; column zero contains time.
    double ***const &samples;
    /// Recorder count for each trend. The writer also inspects the row at this position
    /// so that the trailing sentinel can stop the scan.
    const int *const &count;
    /// First buffer row to inspect for each trend; maintained by the recorder.
    const int *const &countBase;
};

/// Views of configuration, samples and counters used by the trend writers.
/// References preserve access to the owner's current pointers, counters and output-pass count.
/// Output paths and the profile-report stream are supplied by the module's I/O globals.
struct TrendState {
    /// Input deck, output settings and mesh geometry -- SProd::arq.
    const Ler &input;
    const std::string &engineVersion;
    /// Shared globals; sequenciaAP is read only for an AP sequence column -- SProd::simulationContext.
    const SimulationContext *const &globals;
    /// Branch index; negative outside a network -- SProd::indTramo.
    const int &branchIndex;
    /// Output-pass count; captions are emitted only when it equals one -- SProd::trends.trendOutputCount.
    const int &printPassCount;

    /// Production samples and their recording and scan-base counters.
    TrendSeries production;
    /// Service-line samples and their recording and scan-base counters.
    TrendSeries gasLine;
    /// Production cross-section samples and their recording and scan-base counters.
    TrendSeries productionCrossSection;
    /// Service cross-section samples and their recording and scan-base counters.
    TrendSeries gasLineCrossSection;
};

/// Truncates the production-line file when production trends are enabled.
/// Writes input-mesh metadata and selected captions only when printPassCount equals one.
/// networkIndex is part of the file name only for a network branch.
void writeProductionTrendHeader(const TrendState &state, int trendIndex, int networkIndex);

/// Appends production time and data columns from countBase through count, stopping at the sentinel.
/// Adds the AP sequence column only outside a network when AP equals one.
/// Performs no file operation when production trends are disabled.
void writeProductionTrendRows(const TrendState &state, int trendIndex, int networkIndex);

/// Truncates the service-line file when service trends and the gas line are enabled.
/// Writes input-mesh metadata and selected captions only when printPassCount equals one.
/// Appends a final blank line even when no captions are emitted.
void writeGasLineTrendHeader(const TrendState &state, int trendIndex, int networkIndex);

/// Appends service time and data columns from countBase through count, stopping at the sentinel.
/// Adds the AP sequence column only outside a network when AP equals one.
/// Performs no file operation unless service trends and the gas line are enabled.
void writeGasLineTrendRows(const TrendState &state, int trendIndex, int networkIndex);

/// Truncates the production cross-section file when its trends are enabled.
/// Writes the label and the time and temperature captions only when printPassCount equals one.
/// The file name uses position in metres and zero-based layer and discretization indices.
void writeProductionCrossSectionTrendHeader(const TrendState &state, int trendIndex);

/// Appends production cross-section time and temperature from countBase through count,
/// stopping at the sentinel. Emits no AP sequence and appends a final blank line.
/// Performs no file operation when production cross-section trends are disabled.
void writeProductionCrossSectionTrendRows(const TrendState &state, int trendIndex);

/// Truncates the service cross-section file when its trends and the gas line are enabled.
/// Writes the label and the time and temperature captions only when printPassCount equals one.
/// The file name uses position in metres and zero-based layer and discretization indices.
void writeGasLineCrossSectionTrendHeader(const TrendState &state, int trendIndex);

/// Appends service cross-section time and temperature from countBase through count,
/// stopping at the sentinel. Emits no AP sequence and appends a final blank line.
/// Performs no file operation unless service cross-section trends and the gas line are enabled.
void writeGasLineCrossSectionTrendRows(const TrendState &state, int trendIndex);

} // namespace trendoutput

#endif // SISPRODTRENDOUTPUT_H_
