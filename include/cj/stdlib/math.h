/**
 * @file math.h
 * @brief Standard library math functions for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_STDLIB_MATH_H
#define CJ_STDLIB_MATH_H

#include "../common.h"
#include <cmath>
#include <numeric>
#include <random>
#include <algorithm>

namespace cj {
namespace stdlib {
namespace math {

// Mathematical constants
constexpr Float64 PI = 3.14159265358979323846;
constexpr Float64 E = 2.71828182845904523536;
constexpr Float64 SQRT2 = 1.41421356237309504880;
constexpr Float64 SQRT3 = 1.73205080756887729353;
constexpr Float64 LN2 = 0.69314718055994530942;
constexpr Float64 LN10 = 2.30258509299404568402;

// Basic arithmetic functions
template<typename T>
constexpr T Abs(T value) {
    return value < T(0) ? -value : value;
}

template<typename T>
constexpr T Min(T a, T b) {
    return a < b ? a : b;
}

template<typename T>
constexpr T Max(T a, T b) {
    return a > b ? a : b;
}

template<typename T>
constexpr T Clamp(T value, T min_val, T max_val) {
    return Min(Max(value, min_val), max_val);
}

template<typename T>
constexpr T Sign(T value) {
    return value < T(0) ? T(-1) : (value > T(0) ? T(1) : T(0));
}

// Power and exponential functions
Float64 Pow(Float64 base, Float64 exponent);
Float32 Pow(Float32 base, Float32 exponent);

Float64 Sqrt(Float64 value);
Float32 Sqrt(Float32 value);

Float64 Cbrt(Float64 value);
Float32 Cbrt(Float32 value);

Float64 Exp(Float64 value);
Float32 Exp(Float32 value);

Float64 Exp2(Float64 value);
Float32 Exp2(Float32 value);

Float64 Expm1(Float64 value);
Float32 Expm1(Float32 value);

// Logarithmic functions
Float64 Log(Float64 value);
Float32 Log(Float32 value);

Float64 Log2(Float64 value);
Float32 Log2(Float32 value);

Float64 Log10(Float64 value);
Float32 Log10(Float32 value);

Float64 Log1p(Float64 value);
Float32 Log1p(Float32 value);

// Trigonometric functions
Float64 Sin(Float64 angle);
Float32 Sin(Float32 angle);

Float64 Cos(Float64 angle);
Float32 Cos(Float32 angle);

Float64 Tan(Float64 angle);
Float32 Tan(Float32 angle);

Float64 Asin(Float64 value);
Float32 Asin(Float32 value);

Float64 Acos(Float64 value);
Float32 Acos(Float32 value);

Float64 Atan(Float64 value);
Float32 Atan(Float32 value);

Float64 Atan2(Float64 y, Float64 x);
Float32 Atan2(Float32 y, Float32 x);

// Hyperbolic functions
Float64 Sinh(Float64 value);
Float32 Sinh(Float32 value);

Float64 Cosh(Float64 value);
Float32 Cosh(Float32 value);

Float64 Tanh(Float64 value);
Float32 Tanh(Float32 value);

Float64 Asinh(Float64 value);
Float32 Asinh(Float32 value);

Float64 Acosh(Float64 value);
Float32 Acosh(Float32 value);

Float64 Atanh(Float64 value);
Float32 Atanh(Float32 value);

// Rounding and remainder functions
Float64 Ceil(Float64 value);
Float32 Ceil(Float32 value);

Float64 Floor(Float64 value);
Float32 Floor(Float32 value);

Float64 Trunc(Float64 value);
Float32 Trunc(Float32 value);

Float64 Round(Float64 value);
Float32 Round(Float32 value);

Float64 Fmod(Float64 x, Float64 y);
Float32 Fmod(Float32 x, Float32 y);

Float64 Remainder(Float64 x, Float64 y);
Float32 Remainder(Float32 x, Float32 y);

// Floating point manipulation
Float64 Ldexp(Float64 x, int exp);
Float32 Ldexp(Float32 x, int exp);

Float64 Frexp(Float64 value, int* exp);
Float32 Frexp(Float32 value, int* exp);

Float64 Modf(Float64 value, Float64* integer_part);
Float32 Modf(Float32 value, Float32* integer_part);

Float64 Scalbn(Float64 x, int n);
Float32 Scalbn(Float32 x, int n);

// Floating point classification
bool IsFinite(Float64 value);
bool IsFinite(Float32 value);

bool IsInf(Float64 value);
bool IsInf(Float32 value);

bool IsNaN(Float64 value);
bool IsNaN(Float32 value);

bool IsNormal(Float64 value);
bool IsNormal(Float32 value);

// Comparison functions
bool IsEqual(Float64 a, Float64 b, Float64 epsilon = 1e-9);
bool IsEqual(Float32 a, Float32 b, Float32 epsilon = 1e-6f);

bool IsZero(Float64 value, Float64 epsilon = 1e-9);
bool IsZero(Float32 value, Float32 epsilon = 1e-6f);

// Degree/radian conversion
constexpr Float64 DegToRad(Float64 degrees) {
    return degrees * PI / 180.0;
}

constexpr Float32 DegToRad(Float32 degrees) {
    return degrees * static_cast<Float32>(PI) / 180.0f;
}

constexpr Float64 RadToDeg(Float64 radians) {
    return radians * 180.0 / PI;
}

constexpr Float32 RadToDeg(Float32 radians) {
    return radians * 180.0f / static_cast<Float32>(PI);
}

// Random number generation
class Random {
private:
    std::random_device rd_;
    std::mt19937 gen_;
    
public:
    Random() : gen_(rd_()) {}
    explicit Random(UInt32 seed) : gen_(seed) {}
    
    // Generate random integers
    Int32 NextInt();
    Int32 NextInt(Int32 max);
    Int32 NextInt(Int32 min, Int32 max);
    
    UInt32 NextUInt();
    UInt32 NextUInt(UInt32 max);
    UInt32 NextUInt(UInt32 min, UInt32 max);
    
    Int64 NextInt64();
    Int64 NextInt64(Int64 max);
    Int64 NextInt64(Int64 min, Int64 max);
    
    UInt64 NextUInt64();
    UInt64 NextUInt64(UInt64 max);
    UInt64 NextUInt64(UInt64 min, UInt64 max);
    
    // Generate random floating point numbers
    Float32 NextFloat();
    Float32 NextFloat(Float32 max);
    Float32 NextFloat(Float32 min, Float32 max);
    
    Float64 NextDouble();
    Float64 NextDouble(Float64 max);
    Float64 NextDouble(Float64 min, Float64 max);
    
    // Generate random boolean
    bool NextBool();
    bool NextBool(Float64 probability);
    
    // Generate random bytes
    void NextBytes(UInt8* buffer, Size count);
    
    // Seed the generator
    void Seed(UInt32 seed);
};

// Statistical functions
template<typename Iterator>
auto Mean(Iterator first, Iterator last) -> typename std::iterator_traits<Iterator>::value_type {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    if (first == last) return ValueType{};
    
    auto sum = std::accumulate(first, last, ValueType{});
    auto count = std::distance(first, last);
    return sum / static_cast<ValueType>(count);
}

template<typename Iterator>
auto Median(Iterator first, Iterator last) -> typename std::iterator_traits<Iterator>::value_type {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    auto size = std::distance(first, last);
    if (size == 0) return ValueType{};
    
    // Create a copy and sort it
    Vector<ValueType> sorted_data(first, last);
    std::sort(sorted_data.begin(), sorted_data.end());
    
    if (size % 2 == 0) {
        // Even number of elements - return average of middle two
        return (sorted_data[size / 2 - 1] + sorted_data[size / 2]) / ValueType{2};
    } else {
        // Odd number of elements - return middle element
        return sorted_data[size / 2];
    }
}

template<typename Iterator>
auto Variance(Iterator first, Iterator last) -> typename std::iterator_traits<Iterator>::value_type {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    auto size = std::distance(first, last);
    if (size <= 1) return ValueType{};
    
    auto mean = Mean(first, last);
    ValueType sum_sq_diff{};
    
    for (auto it = first; it != last; ++it) {
        auto diff = *it - mean;
        sum_sq_diff += diff * diff;
    }
    
    return sum_sq_diff / static_cast<ValueType>(size - 1);
}

template<typename Iterator>
auto StandardDeviation(Iterator first, Iterator last) -> typename std::iterator_traits<Iterator>::value_type {
    return Sqrt(Variance(first, last));
}

// Linear interpolation
template<typename T>
constexpr T Lerp(T a, T b, T t) {
    return a + t * (b - a);
}

// Smoothstep function
template<typename T>
constexpr T Smoothstep(T edge0, T edge1, T x) {
    T t = Clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

// Hash functions for numeric types
UInt32 Hash32(Int32 value);
UInt32 Hash32(UInt32 value);
UInt32 Hash32(Float32 value);

UInt64 Hash64(Int64 value);
UInt64 Hash64(UInt64 value);
UInt64 Hash64(Float64 value);

// Combine hash values
UInt32 CombineHash32(UInt32 seed, UInt32 value);
UInt64 CombineHash64(UInt64 seed, UInt64 value);

} // namespace math
} // namespace stdlib
} // namespace cj

#endif // CJ_STDLIB_MATH_H