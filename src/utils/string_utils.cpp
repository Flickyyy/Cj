/**
 * @file string_utils.cpp
 * @brief String utilities for CJ language
 */

#include "cj/common.h"
#include <algorithm>
#include <cctype>

namespace cj {
namespace StringUtils {

String Trim(const String& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == String::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

Vector<String> Split(const String& str, char delimiter) {
    Vector<String> result;
    size_t start = 0;
    size_t pos = str.find(delimiter);
    
    while (pos != String::npos) {
        result.push_back(str.substr(start, pos - start));
        start = pos + 1;
        pos = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

String Join(const Vector<String>& strings, const String& separator) {
    if (strings.empty()) return "";
    
    String result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += separator + strings[i];
    }
    return result;
}

bool StartsWith(const String& str, const String& prefix) {
    return str.size() >= prefix.size() && 
           str.substr(0, prefix.size()) == prefix;
}

bool EndsWith(const String& str, const String& suffix) {
    return str.size() >= suffix.size() && 
           str.substr(str.size() - suffix.size()) == suffix;
}

String ToLower(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

String ToUpper(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

} // namespace StringUtils
} // namespace cj