/**
 * @file string_utils.h
 * @brief String utility functions for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_STDLIB_STRING_UTILS_H
#define CJ_STDLIB_STRING_UTILS_H

#include "../common.h"
#include "containers.h"

namespace cj {
namespace stdlib {
namespace string_utils {

/**
 * @brief String manipulation functions
 */
String ToUpper(const String& str);
String ToLower(const String& str);
String Trim(const String& str);
String TrimLeft(const String& str);
String TrimRight(const String& str);

/**
 * @brief String search and replace
 */
Size Find(const String& str, const String& substr, Size pos = 0);
Size FindLast(const String& str, const String& substr);
String Replace(const String& str, const String& from, const String& to);
String ReplaceAll(const String& str, const String& from, const String& to);

/**
 * @brief String splitting and joining
 */
Vector<String> Split(const String& str, const String& delimiter);
Vector<String> Split(const String& str, char delimiter);
String Join(const Vector<String>& strings, const String& separator);

/**
 * @brief String validation
 */
bool StartsWith(const String& str, const String& prefix);
bool EndsWith(const String& str, const String& suffix); 
bool Contains(const String& str, const String& substr);
bool IsEmpty(const String& str);
bool IsWhitespace(const String& str);

/**
 * @brief String formatting
 */
String Repeat(const String& str, Size count);
String PadLeft(const String& str, Size width, char pad_char = ' ');
String PadRight(const String& str, Size width, char pad_char = ' ');
String Center(const String& str, Size width, char pad_char = ' ');

} // namespace string_utils
} // namespace stdlib
} // namespace cj

#endif // CJ_STDLIB_STRING_UTILS_H