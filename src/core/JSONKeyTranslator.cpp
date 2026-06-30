/*
 * JSONKeyTranslator.cpp
 *
 * Implements the English→Portuguese key translation for Marlim3 JSON input
 * files. The translation map is loaded at compile-time from
 * marlim3/translations.json (embedded via translations_data.h).
 *
 * Key design decisions
 * --------------------
 * - Single source of truth: marlim3/translations.json drives both this C++
 *   translator and the Python translator in marlim3/_tramo/_keys.py.
 * - Flat map: the same English name translates to the same Portuguese name
 *   regardless of nesting level.
 * - Lazy initialization: the JSON is parsed once on first call.
 */

#include "JSONKeyTranslator.h"
#include "translations_data.h"
#include "rapidjson/document.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

using namespace rapidjson;

// Lazy-loaded translation maps (parsed once from embedded JSON)

struct TranslationMaps {
    std::unordered_map<std::string, std::string> keys;
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::string>> values;
};

static const TranslationMaps& getMaps() {
    static TranslationMaps maps;
    static bool loaded = false;
    if (!loaded) {
        Document doc;
        doc.Parse(TRANSLATIONS_JSON_RAW);

        const Value& keysObj = doc["keys"];
        for (Value::ConstMemberIterator it = keysObj.MemberBegin();
             it != keysObj.MemberEnd(); ++it) {
            maps.keys[it->name.GetString()] = it->value.GetString();
        }

        const Value& valsObj = doc["values"];
        for (Value::ConstMemberIterator it = valsObj.MemberBegin();
             it != valsObj.MemberEnd(); ++it) {
            std::unordered_map<std::string, std::string> inner;
            const Value& innerObj = it->value;
            for (Value::ConstMemberIterator jt = innerObj.MemberBegin();
                 jt != innerObj.MemberEnd(); ++jt) {
                inner[jt->name.GetString()] = jt->value.GetString();
            }
            maps.values[it->name.GetString()] = inner;
        }
        loaded = true;
    }
    return maps;
}

// Internal implementation

static void renameObjectKeys(Value& obj, Document::AllocatorType& alloc) {
    const TranslationMaps& maps = getMaps();
    // Collect keys that need renaming (avoid modifying map while iterating)
    std::vector<std::pair<std::string, std::string>> renames;
    renames.reserve(obj.MemberCount());

    for (Value::MemberIterator it = obj.MemberBegin();
         it != obj.MemberEnd(); ++it)
    {
        const std::string key(it->name.GetString());

        // Translate string values for specific keys (e.g. layerMeasurementType)
        auto vit = maps.values.find(key);
        if (vit != maps.values.end() && it->value.IsString()) {
            std::string val(it->value.GetString());
            std::transform(val.begin(), val.end(), val.begin(), ::toupper);
            auto vfound = vit->second.find(val);
            if (vfound != vit->second.end()) {
                it->value.SetString(vfound->second.c_str(),
                                    static_cast<SizeType>(vfound->second.size()),
                                    alloc);
            }
        }

        auto found = maps.keys.find(key);
        if (found != maps.keys.end() && found->second != key) {
            renames.emplace_back(key, found->second);
        }
    }

    // Apply renames: extract value, remove old member, add with new key name
    for (const auto& rename : renames) {
        const std::string& oldKey = rename.first;
        const std::string& newKey = rename.second;

        Value::MemberIterator it = obj.FindMember(oldKey.c_str());
        if (it == obj.MemberEnd()) {
            continue;
        }

        Value val(kNullType);
        val.Swap(it->value);
        obj.RemoveMember(it);

        Value nameStr(newKey.c_str(),
                      static_cast<SizeType>(newKey.size()), alloc);
        obj.AddMember(nameStr, val, alloc);
    }
}

static void translateValueRecursive(Value& v,
                                    Document::AllocatorType& alloc) {
    if (v.IsObject()) {
        for (Value::MemberIterator it = v.MemberBegin();
             it != v.MemberEnd(); ++it)
        {
            translateValueRecursive(it->value, alloc);
        }
        renameObjectKeys(v, alloc);
    } else if (v.IsArray()) {
        for (Value::ValueIterator it = v.Begin(); it != v.End(); ++it) {
            translateValueRecursive(*it, alloc);
        }
    }
}

// Public API

namespace JSONKeyTranslator {

void translateEnToPt(Value& node, Document::AllocatorType& allocator) {
    translateValueRecursive(node, allocator);
}

} // namespace JSONKeyTranslator
