// Tencent is pleased to support the open source community by making RapidJSON available.
// 
// Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. All rights reserved.
//
// Licensed under the MIT License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, software distributed 
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
// CONDITIONS OF ANY KIND, either express or implied. See the License for the 
// specific language governing permissions and limitations under the License.

#ifndef RAPIDJSON_ERROR_EN_H_
#define RAPIDJSON_ERROR_EN_H_

#include "error.h"

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(switch-enum)
RAPIDJSON_DIAG_OFF(covered-switch-default)
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Maps error code of parsing into error message.
/*!
    \ingroup RAPIDJSON_ERRORS
    \param parseErrorCode Error code obtained in parsing.
    \return the error message.
    \note User can make a copy of this function for localization.
        Using switch-case is safer for future modification of error codes.
*/
inline const RAPIDJSON_ERROR_CHARTYPE* GetParseError_Pt_BR(ParseErrorCode parseErrorCode) {
    switch (parseErrorCode) {
        case kParseErrorNone:                           return RAPIDJSON_ERROR_STRING("Nenhum erro.");
        case kParseErrorDocumentEmpty:                  return RAPIDJSON_ERROR_STRING("O documento esta vazio.");
        case kParseErrorDocumentRootNotSingular:        return RAPIDJSON_ERROR_STRING("O documento root deve ser seguido por outros valores.");
        case kParseErrorValueInvalid:                   return RAPIDJSON_ERROR_STRING("Valor indevido ou inesperado.");
        case kParseErrorObjectMissName:                 return RAPIDJSON_ERROR_STRING("Falta um nome para um membro de objeto.");
        case kParseErrorObjectMissColon:                return RAPIDJSON_ERROR_STRING("Falta dois-pontos ':' depois de nome de membro de objeto.");
        case kParseErrorObjectMissCommaOrCurlyBracket:  return RAPIDJSON_ERROR_STRING("Falta virgula ',' ou fecha-chaves '}' depois de membro de objeto.");
        case kParseErrorArrayMissCommaOrSquareBracket:  return RAPIDJSON_ERROR_STRING("Falta virgula ',' ou fecha-colchetes ']' depois de elemento array.");
        case kParseErrorStringUnicodeEscapeInvalidHex:  return RAPIDJSON_ERROR_STRING("Digito hexadecimal incorreto depois de sequencia de escape '\\u' em string.");
        case kParseErrorStringUnicodeSurrogateInvalid:  return RAPIDJSON_ERROR_STRING("Par de substituicao em string invalido.");
        case kParseErrorStringEscapeInvalid:            return RAPIDJSON_ERROR_STRING("Caracter de escape invalido em string.");
        case kParseErrorStringMissQuotationMark:        return RAPIDJSON_ERROR_STRING("Falta fecha-aspas em string.");
        case kParseErrorStringInvalidEncoding:          return RAPIDJSON_ERROR_STRING("Codificacao invalida em string.");
        case kParseErrorNumberTooBig:                   return RAPIDJSON_ERROR_STRING("Numero muito grande para ser armazenado em double.");
        case kParseErrorNumberMissFraction:             return RAPIDJSON_ERROR_STRING("Falta a parte fracao em numero.");
        case kParseErrorNumberMissExponent:             return RAPIDJSON_ERROR_STRING("Falta expoente em numero.");
        case kParseErrorTermination:                    return RAPIDJSON_ERROR_STRING("Processo de parse terminado devido a erro no handler.");
        case kParseErrorUnspecificSyntaxError:          return RAPIDJSON_ERROR_STRING("Erro de sintaxe nao inespecifico.");
        default:                                        return RAPIDJSON_ERROR_STRING("Erro desconhecido.");
    }
}

//! Maps error code of parsing into error message.
/*!
    \ingroup RAPIDJSON_ERRORS
    \param parseErrorCode Error code obtained in parsing.
    \return the error message.
    \note User can make a copy of this function for localization.
        Using switch-case is safer for future modification of error codes.
*/
template <typename Ch>
inline SizeType StrLen(const Ch* s) {
    const Ch* p = s;
    while (*p) ++p;
    return SizeType(p - s);
}

template <typename Ch>
inline const RAPIDJSON_ERROR_CHARTYPE* GetParseErrorCause_Pt_BR(const Ch* s) {
    if (strcmp(s, "required") == 0) {
        return RAPIDJSON_ERROR_STRING("Nao encontrado um elemento obrigatorio para a propriedade referida.");
    } else if (strcmp(s, "enum") == 0) {
        return RAPIDJSON_ERROR_STRING("Valor indevido para a propriedade referida. Consulte valores permitidos.");
    } else if (strcmp(s, "uniqueItems") == 0) {
        return RAPIDJSON_ERROR_STRING("Elemento duplicado na propriedade referida.");
    } else if (strcmp(s, "type") == 0) {
        return RAPIDJSON_ERROR_STRING("Valor do elemento incompativel com o tipo de dado definido para a propriedade referida.");
    } else {
        return RAPIDJSON_ERROR_STRING(s);
    }
}

RAPIDJSON_NAMESPACE_END

#ifdef __clang__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_ERROR_EN_H_
