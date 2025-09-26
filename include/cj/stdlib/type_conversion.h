/**
 * @file type_conversion.h
 * @brief Type conversion utilities for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_STDLIB_TYPE_CONVERSION_H
#define CJ_STDLIB_TYPE_CONVERSION_H

#include "../common.h"
#include "../types/type_system.h"
#include "containers.h"
#include <sstream>
#include <type_traits>

namespace cj {
namespace stdlib {
namespace conversion {

/**
 * @brief Conversion result with error handling
 */
template<typename T>
class ConversionResult {
private:
    T value_;
    bool success_;
    String error_message_;

public:
    ConversionResult(const T& value) : value_(value), success_(true) {}
    ConversionResult(const String& error) : value_(T{}), success_(false), error_message_(error) {}
    
    bool IsSuccess() const { return success_; }
    bool IsError() const { return !success_; }
    
    const T& GetValue() const { 
        if (!success_) {
            throw std::runtime_error("Attempted to get value from failed conversion: " + error_message_);
        }
        return value_; 
    }
    
    const String& GetError() const { return error_message_; }
    
    T GetValueOr(const T& default_value) const {
        return success_ ? value_ : default_value;
    }
};

// String to numeric conversions
ConversionResult<Int8> ToInt8(const String& str);
ConversionResult<Int16> ToInt16(const String& str);
ConversionResult<Int32> ToInt32(const String& str);
ConversionResult<Int64> ToInt64(const String& str);

ConversionResult<UInt8> ToUInt8(const String& str);
ConversionResult<UInt16> ToUInt16(const String& str);
ConversionResult<UInt32> ToUInt32(const String& str);
ConversionResult<UInt64> ToUInt64(const String& str);

ConversionResult<Float32> ToFloat32(const String& str);
ConversionResult<Float64> ToFloat64(const String& str);

ConversionResult<bool> ToBool(const String& str);

// Numeric to string conversions
String ToString(Int8 value);
String ToString(Int16 value);
String ToString(Int32 value);
String ToString(Int64 value);

String ToString(UInt8 value);
String ToString(UInt16 value);
String ToString(UInt32 value);
String ToString(UInt64 value);

String ToString(Float32 value, int precision = -1);
String ToString(Float64 value, int precision = -1);

String ToString(bool value);

// Advanced string formatting
String ToString(Int64 value, int base); // base 2-36
String ToString(UInt64 value, int base); // base 2-36

String ToHex(UInt8 value, bool uppercase = false);
String ToHex(UInt16 value, bool uppercase = false);
String ToHex(UInt32 value, bool uppercase = false);
String ToHex(UInt64 value, bool uppercase = false);

String ToBinary(UInt8 value);
String ToBinary(UInt16 value);
String ToBinary(UInt32 value);
String ToBinary(UInt64 value);

String ToOctal(UInt64 value);

// Numeric type conversions with overflow checking
template<typename To, typename From>
ConversionResult<To> SafeCast(From value) {
    static_assert(std::is_arithmetic_v<From> && std::is_arithmetic_v<To>, 
                  "SafeCast only works with arithmetic types");
    
    // Check for overflow/underflow
    if constexpr (std::is_integral_v<From> && std::is_integral_v<To>) {
        if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
            if (value < 0) {
                return ConversionResult<To>("Negative value cannot be converted to unsigned type");
            }
        }
        
        if (value > static_cast<From>(std::numeric_limits<To>::max())) {
            return ConversionResult<To>("Value too large for target type");
        }
        
        if constexpr (std::is_signed_v<To>) {
            if (value < static_cast<From>(std::numeric_limits<To>::min())) {
                return ConversionResult<To>("Value too small for target type");
            }
        }
    }
    
    return ConversionResult<To>(static_cast<To>(value));
}

// Explicit casting functions (unsafe but explicit)
template<typename To, typename From>
To UnsafeCast(From value) {
    return static_cast<To>(value);
}

// Zero-extending and sign-extending casts
template<typename To, typename From>
To ZeroExtend(From value) {
    static_assert(std::is_integral_v<From> && std::is_integral_v<To>, 
                  "ZeroExtend only works with integral types");
    static_assert(sizeof(To) >= sizeof(From), 
                  "Target type must be at least as large as source type");
    return static_cast<To>(static_cast<typename std::make_unsigned_t<From>>(value));
}

template<typename To, typename From>
To SignExtend(From value) {
    static_assert(std::is_integral_v<From> && std::is_integral_v<To>, 
                  "SignExtend only works with integral types");
    static_assert(sizeof(To) >= sizeof(From), 
                  "Target type must be at least as large as source type");
    return static_cast<To>(static_cast<typename std::make_signed_t<From>>(value));
}

// Bit manipulation for conversions
template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
ReverseBits(T value) {
    T result = 0;
    constexpr int bits = sizeof(T) * 8;
    
    for (int i = 0; i < bits; ++i) {
        if (value & (T(1) << i)) {
            result |= T(1) << (bits - 1 - i);
        }
    }
    
    return result;
}

template<typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
ReverseBytes(T value) {
    T result = 0;
    constexpr int bytes = sizeof(T);
    
    for (int i = 0; i < bytes; ++i) {
        result = (result << 8) | ((value >> (i * 8)) & 0xFF);
    }
    
    return result;
}

// Endianness conversion
template<typename T>
T ToLittleEndian(T value) {
    static_assert(std::is_integral_v<T>, "ToLittleEndian only works with integral types");
    
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return value;
    #else
        return ReverseBytes(value);
    #endif
}

template<typename T>
T ToBigEndian(T value) {
    static_assert(std::is_integral_v<T>, "ToBigEndian only works with integral types");
    
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return value;
    #else
        return ReverseBytes(value);
    #endif
}

template<typename T>
T FromLittleEndian(T value) {
    return ToLittleEndian(value); // Same as ToLittleEndian
}

template<typename T>
T FromBigEndian(T value) {
    return ToBigEndian(value); // Same as ToBigEndian
}

// Array/container conversions
template<typename To, typename From>
Vector<To> ConvertVector(const Vector<From>& source) {
    Vector<To> result;
    result.Reserve(source.Size());
    
    for (const auto& item : source) {
        auto conversion = SafeCast<To>(item);
        if (conversion.IsError()) {
            throw std::runtime_error("Conversion failed: " + conversion.GetError());
        }
        result.PushBack(conversion.GetValue());
    }
    
    return result;
}

// String encoding conversions (UTF-8 is default)
namespace encoding {

/**
 * @brief Convert UTF-8 string to wide string (UTF-16/UTF-32)
 */
std::wstring ToWideString(const String& utf8_str);

/**
 * @brief Convert wide string to UTF-8
 */
String FromWideString(const std::wstring& wide_str);

/**
 * @brief Validate UTF-8 encoding
 */
bool IsValidUTF8(const String& str);

/**
 * @brief Count UTF-8 characters (not bytes)
 */
Size CountUTF8Characters(const String& str);

/**
 * @brief Extract UTF-8 character at position
 */
String GetUTF8Character(const String& str, Size position);

/**
 * @brief Convert string to uppercase (UTF-8 aware)
 */
String ToUpperCase(const String& str);

/**
 * @brief Convert string to lowercase (UTF-8 aware)
 */
String ToLowerCase(const String& str);

} // namespace encoding

// Formatter class for custom string formatting
class Formatter {
private:
    std::ostringstream oss_;

public:
    Formatter() = default;
    
    template<typename T>
    Formatter& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Special formatting for numeric types
    Formatter& Hex(UInt64 value, bool uppercase = false);
    Formatter& Binary(UInt64 value);
    Formatter& Octal(UInt64 value);
    Formatter& Scientific(Float64 value, int precision = 6);
    Formatter& Fixed(Float64 value, int precision = 6);
    
    // Padding and alignment
    Formatter& Width(int width);
    Formatter& Fill(char fill_char);
    Formatter& Left();
    Formatter& Right();
    Formatter& Center();
    
    String ToString() const;
    void Clear();
};

// Parsing utilities
namespace parse {

/**
 * @brief Parse a delimited string into a vector
 */
Vector<String> Split(const String& str, char delimiter);
Vector<String> Split(const String& str, const String& delimiter);

/**
 * @brief Join a vector of strings with a delimiter
 */
String Join(const Vector<String>& strings, const String& delimiter);

/**
 * @brief Trim whitespace from string
 */
String Trim(const String& str);
String TrimLeft(const String& str);
String TrimRight(const String& str);

/**
 * @brief Parse key-value pairs
 */
HashMap<String, String> ParseKeyValuePairs(const String& str, char pair_separator = ',', char key_value_separator = '=');

/**
 * @brief Parse JSON-like structure (simplified)
 */
class JsonValue; // Forward declaration - would be implemented separately

} // namespace parse

// Type introspection helpers
template<typename T>
String GetTypeName() {
    // This would need compiler-specific implementation or RTTI
    return "unknown";
}

// Specializations for known types
template<> String GetTypeName<Int8>();
template<> String GetTypeName<Int16>();
template<> String GetTypeName<Int32>();
template<> String GetTypeName<Int64>();
template<> String GetTypeName<UInt8>();
template<> String GetTypeName<UInt16>();
template<> String GetTypeName<UInt32>();
template<> String GetTypeName<UInt64>();
template<> String GetTypeName<Float32>();
template<> String GetTypeName<Float64>();
template<> String GetTypeName<bool>();
template<> String GetTypeName<String>();

} // namespace conversion
} // namespace stdlib
} // namespace cj

#endif // CJ_STDLIB_TYPE_CONVERSION_H