/**
 * @file value.h
 * @brief Value system for the CJ language runtime
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_VALUE_H
#define CJ_VALUE_H

#include "../common.h"
#include <variant>

namespace cj {

/**
 * @brief Enumeration of value types in CJ
 */
enum class ValueType {
    NIL,
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    ARRAY,
    OBJECT,
    FUNCTION,
    NATIVE_FUNCTION
};

/**
 * @brief Forward declarations for complex types
 */
class Object;
class Array;
class Function;
class NativeFunction;

/**
 * @brief Runtime value representation in CJ
 * 
 * Uses a variant to store different types of values efficiently.
 */
class Value {
public:
    using VariantType = std::variant<
        std::nullptr_t,     // NIL
        bool,               // BOOLEAN
        Int64,              // INTEGER
        Float64,            // FLOAT
        String,             // STRING
        SharedPtr<Array>,   // ARRAY
        SharedPtr<Object>,  // OBJECT
        SharedPtr<Function>, // FUNCTION
        SharedPtr<NativeFunction> // NATIVE_FUNCTION
    >;

private:
    VariantType value_;

public:
    /**
     * @brief Default constructor creates a NIL value
     */
    Value() : value_(nullptr) {}

    /**
     * @brief Construct from various types
     */
    Value(std::nullptr_t) : value_(nullptr) {}
    Value(bool b) : value_(b) {}
    Value(Int64 i) : value_(i) {}
    Value(Float64 f) : value_(f) {}
    Value(const String& s) : value_(s) {}
    Value(SharedPtr<Array> arr) : value_(arr) {}
    Value(SharedPtr<Object> obj) : value_(obj) {}
    Value(SharedPtr<Function> func) : value_(func) {}
    Value(SharedPtr<NativeFunction> nfunc) : value_(nfunc) {}

    /**
     * @brief Get the type of this value
     */
    ValueType GetType() const;

    /**
     * @brief Type checking methods
     */
    bool IsNil() const { return std::holds_alternative<std::nullptr_t>(value_); }
    bool IsBool() const { return std::holds_alternative<bool>(value_); }
    bool IsInt() const { return std::holds_alternative<Int64>(value_); }
    bool IsFloat() const { return std::holds_alternative<Float64>(value_); }
    bool IsString() const { return std::holds_alternative<String>(value_); }
    bool IsArray() const { return std::holds_alternative<SharedPtr<Array>>(value_); }
    bool IsObject() const { return std::holds_alternative<SharedPtr<Object>>(value_); }
    bool IsFunction() const { return std::holds_alternative<SharedPtr<Function>>(value_); }
    bool IsNativeFunction() const { return std::holds_alternative<SharedPtr<NativeFunction>>(value_); }
    bool IsCallable() const { return IsFunction() || IsNativeFunction(); }
    bool IsNumber() const { return IsInt() || IsFloat(); }

    /**
     * @brief Value extraction methods
     */
    bool AsBool() const;
    Int64 AsInt() const;
    Float64 AsFloat() const;
    const String& AsString() const;
    SharedPtr<Array> AsArray() const;
    SharedPtr<Object> AsObject() const;
    SharedPtr<Function> AsFunction() const;
    SharedPtr<NativeFunction> AsNativeFunction() const;

    /**
     * @brief Conversion methods
     */
    String ToString() const;
    bool ToBool() const;
    Float64 ToNumber() const;

    /**
     * @brief Comparison operators
     */
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const { return !(*this == other); }
    bool operator<(const Value& other) const;
    bool operator<=(const Value& other) const;
    bool operator>(const Value& other) const;
    bool operator>=(const Value& other) const;

    /**
     * @brief Arithmetic operators
     */
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;

    /**
     * @brief Logical operators
     */
    Value operator&&(const Value& other) const;
    Value operator||(const Value& other) const;
    Value operator!() const;

    /**
     * @brief Get the underlying variant
     */
    const VariantType& GetVariant() const { return value_; }
    VariantType& GetVariant() { return value_; }
};

/**
 * @brief Array implementation
 */
class Array {
private:
    Vector<Value> elements_;

public:
    Array() = default;
    explicit Array(Size size) : elements_(size) {}
    Array(std::initializer_list<Value> init) : elements_(init) {}

    Size GetSize() const { return elements_.size(); }
    bool Empty() const { return elements_.empty(); }
    
    Value& operator[](Size index) { return elements_[index]; }
    const Value& operator[](Size index) const { return elements_[index]; }
    
    void Push(const Value& value) { elements_.push_back(value); }
    Value Pop();
    
    Vector<Value>::iterator begin() { return elements_.begin(); }
    Vector<Value>::iterator end() { return elements_.end(); }
    Vector<Value>::const_iterator begin() const { return elements_.begin(); }
    Vector<Value>::const_iterator end() const { return elements_.end(); }
};

/**
 * @brief Object implementation (hash table)
 */
class Object {
private:
    HashMap<String, Value> properties_;

public:
    Object() = default;

    bool HasProperty(const String& key) const;
    Value GetProperty(const String& key) const;
    void SetProperty(const String& key, const Value& value);
    bool RemoveProperty(const String& key);
    
    Vector<String> GetKeys() const;
    Size GetSize() const { return properties_.size(); }
    bool Empty() const { return properties_.empty(); }
    
    HashMap<String, Value>::iterator begin() { return properties_.begin(); }
    HashMap<String, Value>::iterator end() { return properties_.end(); }
    HashMap<String, Value>::const_iterator begin() const { return properties_.begin(); }
    HashMap<String, Value>::const_iterator end() const { return properties_.end(); }
};

} // namespace cj

#endif // CJ_VALUE_H