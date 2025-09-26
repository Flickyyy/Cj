/**
 * @file file_utils.cpp
 * @brief File utilities for CJ language
 */

#include "cj/common.h"
#include <fstream>
#include <sstream>

namespace cj {
namespace FileUtils {

String ReadFile(const String& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw CJException("Cannot open file: " + filename);
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void WriteFile(const String& filename, const String& contents) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw CJException("Cannot create file: " + filename);
    }
    
    file << contents;
}

bool FileExists(const String& filename) {
    std::ifstream file(filename);
    return file.good();
}

String GetFileExtension(const String& filename) {
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos == String::npos) {
        return "";
    }
    return filename.substr(dot_pos + 1);
}

String GetBaseName(const String& filename) {
    size_t slash_pos = filename.find_last_of("/\\");
    String basename = (slash_pos == String::npos) ? filename : filename.substr(slash_pos + 1);
    
    size_t dot_pos = basename.find_last_of('.');
    if (dot_pos != String::npos) {
        basename = basename.substr(0, dot_pos);
    }
    
    return basename;
}

String GetDirectory(const String& filename) {
    size_t slash_pos = filename.find_last_of("/\\");
    if (slash_pos == String::npos) {
        return ".";
    }
    return filename.substr(0, slash_pos);
}

} // namespace FileUtils
} // namespace cj