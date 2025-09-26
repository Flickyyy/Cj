/**
 * @file common.h
 * @brief Common definitions and utilities for the CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_COMMON_H
#define CJ_COMMON_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>

namespace cj {

// Version information
constexpr const char* VERSION = "0.1.0";
constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

// Common types
using Int8 = std::int8_t;
using Int16 = std::int16_t;
using Int32 = std::int32_t;
using Int64 = std::int64_t;
using UInt8 = std::uint8_t;
using UInt16 = std::uint16_t;
using UInt32 = std::uint32_t;
using UInt64 = std::uint64_t;
using Float32 = float;
using Float64 = double;
using Size = std::size_t;

// Smart pointer aliases
template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// Container aliases
template<typename T>
using Vector = std::vector<T>;

template<typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using String = std::string;

// Forward declarations
class Token;
class ASTNode;
class IRInstruction;
class VM;
class GarbageCollector;
class JITCompiler;

// Exception classes
class CJException : public std::runtime_error {
public:
    explicit CJException(const String& message) : std::runtime_error(message) {}
};

class LexerException : public CJException {
public:
    explicit LexerException(const String& message) : CJException("Lexer Error: " + message) {}
};

class ParserException : public CJException {
public:
    explicit ParserException(const String& message) : CJException("Parser Error: " + message) {}
};

class RuntimeException : public CJException {
public:
    explicit RuntimeException(const String& message) : CJException("Runtime Error: " + message) {}
};

class JITException : public CJException {
public:
    explicit JITException(const String& message) : CJException("JIT Error: " + message) {}
};

// Utility macros
#define CJ_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            throw CJException("Assertion failed: " + String(message)); \
        } \
    } while(0)

#ifdef DEBUG
#define CJ_DEBUG_ASSERT(condition, message) CJ_ASSERT(condition, message)
#else
#define CJ_DEBUG_ASSERT(condition, message) ((void)0)
#endif

// Memory management utilities
template<typename T, typename... Args>
UniquePtr<T> cj_make_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
SharedPtr<T> cj_make_shared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace cj

#endif // CJ_COMMON_H