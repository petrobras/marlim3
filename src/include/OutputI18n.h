#ifndef OUTPUT_I18N_H_
#define OUTPUT_I18N_H_

namespace output_i18n {

constexpr int kLanguagePtBr = 0;
constexpr int kLanguageEn = 1;

inline const char* tr(const int languageCode, const char* ptBrText,
                      const char* enText) {
  return (languageCode == kLanguageEn) ? enText : ptBrText;
}

}  // namespace output_i18n

#endif  // OUTPUT_I18N_H_
