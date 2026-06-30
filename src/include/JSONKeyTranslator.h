/*
 * JSONKeyTranslator.h
 *
 * Translates English JSON keys to the internal Portuguese equivalents used by
 * Marlim3. Activated when the input JSON contains the root-level key:
 *
 *   "language": "en"
 *
 * The default language is "pt-br"; in that case all keys are read as-is.
 * When "en" is specified, every key in the document is looked up in a static
 * English→Portuguese map and renamed before the typed schema (JSON_entrada)
 * processes the document. Unknown keys are left untouched.
 */

#ifndef JSONKEYTRANSLATOR_H_
#define JSONKEYTRANSLATOR_H_

#include "rapidjson/document.h"

namespace JSONKeyTranslator {

    /**
     * Recursively walks a RapidJSON Value and renames all object keys found in
     * the English→Portuguese map. Arrays are iterated element-by-element.
     *
     * @param node      The Value to translate (modified in-place).
     * @param allocator The document allocator needed to create new key strings.
     */
    void translateEnToPt(rapidjson::Value& node,
                         rapidjson::Document::AllocatorType& allocator);

} // namespace JSONKeyTranslator

#endif /* JSONKEYTRANSLATOR_H_ */
