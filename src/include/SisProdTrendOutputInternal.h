#ifndef SISPRODTRENDOUTPUTINTERNAL_H_
#define SISPRODTRENDOUTPUTINTERNAL_H_

#include "SisProdTrendOutput.h"
#include "OutputI18n.h"

#include <cstddef>
#include <ios>
#include <iosfwd>
#include <string>

struct detTRENDTrans;

namespace trendoutput {
namespace {

/// Selects a caption using the configured language; unsupported codes use Portuguese.
struct CaptionTranslator {
    int languageCode;

    /// Returns the Portuguese or English caption without copying its text.
    const char *operator()(const char *ptBrText, const char *enText) const {
        return output_i18n::tr(languageCode, ptBrText, enText);
    }
};

/// Caption pair selected when the corresponding descriptor member equals one.
template <typename Trend>
struct OptionalColumn {
    int Trend::*enabled;
    const char *ptBrText;
    const char *enText;
};

/// Caption pair belonging to a group enabled by its caller.
struct FixedColumn {
    const char *ptBrText;
    const char *enText;
};

/// File-name position in metres and zero-based wall-layer and discretization indices.
struct CrossSectionPosition {
    int positionMetres;
    int layer;
    int discretization;
};

/// Buffer view and limits used to scan one trend's candidate rows.
struct SampleWindow {
    double **samples;
    int firstRow;
    int rowCount;
    int columnCount;
};

/// Writes captions in table order for descriptor members whose value equals one.
template <typename Trend, std::size_t Count>
void writeOptionalColumns(std::ostream &trendFile, const CaptionTranslator &translate,
                          const Trend &trend,
                          const OptionalColumn<Trend> (&columns)[Count]);

/// Writes every caption in the supplied group, in table order.
template <std::size_t Count>
void writeFixedColumns(std::ostream &trendFile, const CaptionTranslator &translate,
                       const FixedColumn (&columns)[Count]);

/// Writes the three wave-family celerity captions in increasing family order.
void writeWaveCelerityColumns(std::ostream &trendFile, const CaptionTranslator &translate);

/// Writes the three components of each wave family, with family as the outer index.
void writeEigenvectorColumns(std::ostream &trendFile, const CaptionTranslator &translate);

/// Writes the three wave-family fluctuation captions in increasing family order.
void writeFluctuationColumns(std::ostream &trendFile, const CaptionTranslator &translate);

/// Returns the sum of input-mesh lengths from zero through lastCellIndex, in metres.
/// Returns zero for a negative lastCellIndex and accumulates in increasing index order.
template <typename Cell>
double lengthFromOrigin(const Cell *cells, int lastCellIndex);

/// Writes input-mesh distance, label, cell index, optional AP sequence caption and time caption.
/// The AP caption is present only outside a network when AP equals one.
template <typename Trend, typename Cell>
void writeLineTrendMetadata(std::ostream &trendFile, const CaptionTranslator &translate,
                            const TrendState &state, const Trend &trend,
                            const Cell *cells, const char *originPtBrText,
                            const char *originEnText);

/// Writes the label and the two cross-section captions: time and temperature.
void writeCrossSectionCaptions(std::ostream &trendFile, const CaptionTranslator &translate,
                               const detTRENDTrans &trend);

/// Builds a production or service file name using an integer position in metres.
/// Uses the output-path prefix and either the AP suffix or the branch and network indices.
std::string lineTrendFileName(const TrendState &state, const char *prefix,
                         int position, int networkIndex);

/// Returns the file-name position in metres and the zero-based layer and discretization indices.
/// The position is trend.comp; trend.posic is the separate mesh-cell index.
CrossSectionPosition crossSectionPositionOf(const detTRENDTrans &trend);

/// Builds a cross-section file name from the output-path prefix, optional branch and position.
std::string crossSectionTrendFileName(const TrendState &state, const char *prefix,
                                 const CrossSectionPosition &position);

/// Records the file name in arqRelatorioPerfis unless this is an injection-well simulation.
/// Emits a line terminator and synchronizes the report stream.
void reportProducedFile(const TrendState &state, const std::string &fileName);

/// Opens the file with the supplied mode, calls writeContent, optionally emits a blank line,
/// closes the file, and records its name in the profile report. The report operation follows
/// normal completion of writing and closing, including stream failures that do not throw.
template <typename WriteContent>
void produceTrendFile(const TrendState &state, const std::string &fileName,
                      std::ios_base::openmode mode, bool blankLineBeforeClose,
                      WriteContent writeContent);

/// Returns count minus countBase plus one: the number of candidate rows to inspect,
/// including the recording position where the trailing sentinel is expected.
int pendingRowCount(const TrendSeries &series, int trendIndex);

/// Returns the selected trend buffer, first scan row, inclusive scan size and column count.
SampleWindow windowOf(const TrendSeries &series, int trendIndex, int columnCount);

/// Truncates the file and calls writeCaptions only when printPassCount equals one.
/// The optional final blank line is emitted independently of the caption guard.
/// Closes the file before recording its name in the profile report.
template <typename WriteCaptions>
void writeTrendHeaderFile(const TrendState &state, const std::string &fileName,
                          WriteCaptions writeCaptions, bool blankLineBeforeClose);

/// Appends candidate rows until their scan limit or a column-zero value at or below the end marker.
/// When supported, AP outside a network prefixes each row with sequenciaAP. Values retain the
/// configured width and precision. The optional final blank line is emitted after the scan,
/// even when no sample row is written. Closes the file before reporting its name.
void writeTrendRowsFile(const TrendState &state, const std::string &fileName,
                        const SampleWindow &window, bool supportsApSequence,
                        bool blankLineBeforeClose);

} // namespace
} // namespace trendoutput

#endif // SISPRODTRENDOUTPUTINTERNAL_H_
