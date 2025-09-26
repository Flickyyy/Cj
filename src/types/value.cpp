/**
 * @file value.cpp
 * @brief Implementation of Value system for CJ language
 */

#include "cj/types/value.h"
#include <sstream>
#include <stdexcept>

namespace cj {

ValueType Value::GetType() const {
    return static_cast<ValueType>(value_.index());
}

bool Value::AsBool() const {
    if (!IsBool()) {
        throw RuntimeException("Value is not a boolean");
    }
    return std::get<bool>(value_);
}

Int64 Value::AsInt() const {
    if (!IsInt()) {
        throw RuntimeException("Value is not an integer");
    }
    return std::get<Int64>(value_);
}

Float64 Value::AsFloat() const {
    if (!IsFloat()) {
        throw RuntimeException("Value is not a float");
    }
    return std::get<Float64>(value_);
}

const String& Value::AsString() const {
    if (!IsString()) {
        throw RuntimeException("Value is not a string");
    }
    return std::get<String>(value_);
}

SharedPtr<Array> Value::AsArray() const {
    if (!IsArray()) {
        throw RuntimeException("Value is not an array");
    }
    return std::get<SharedPtr<Array>>(value_);
}

SharedPtr<Object> Value::AsObject() const {
    if (!IsObject()) {
        throw RuntimeException("Value is not an object");
    }
    return std::get<SharedPtr<Object>>(value_);
}

String Value::ToString() const {
    switch (GetType()) {
        case ValueType::NIL:
            return "nil";
        case ValueType::BOOLEAN:
            return AsBool() ? "true" : "false";
        case ValueType::INTEGER:
            return std::to_string(AsInt());
        case ValueType::FLOAT:
            return std::to_string(AsFloat());
        case ValueType::STRING:
            return AsString();
        case ValueType::ARRAY:
            return "[Array]";
        case ValueType::OBJECT:
            return "{Object}";
        case ValueType::FUNCTION:
            return "<Function>";
        case ValueType::NATIVE_FUNCTION:
            return "<NativeFunction>";
    }
    return "<Unknown>";
}

bool Value::ToBool() const {
    switch (GetType()) {
        case ValueType::NIL:
            return false;
        case ValueType::BOOLEAN:
            return AsBool();
        case ValueType::INTEGER:
            return AsInt() != 0;
        case ValueType::FLOAT:
            return AsFloat() != 0.0;
        case ValueType::STRING:
            return !AsString().empty();
        case ValueType::ARRAY:
            return !AsArray()->Empty();
        case ValueType::OBJECT:
            return !AsObject()->Empty();
        default:
            return true;
    }
}

Float64 Value::ToNumber() const {
    switch (GetType()) {
        case ValueType::INTEGER:
            return static_cast<Float64>(AsInt());
        case ValueType::FLOAT:
            return AsFloat();
        case ValueType::BOOLEAN:
            return AsBool() ? 1.0 : 0.0;
        case ValueType::STRING: {
            try {
                return std::stod(AsString());
            } catch (...) {
                return 0.0;
            }
        }
        default:
            return 0.0;
    }
}

bool Value::operator==(const Value& other) const {
    if (GetType() != other.GetType()) {
        return false;
    }
    
    switch (GetType()) {
        case ValueType::NIL:
            return true;
        case ValueType::BOOLEAN:
            return AsBool() == other.AsBool();
        case ValueType::INTEGER:
            return AsInt() == other.AsInt();
        case ValueType::FLOAT:
            return AsFloat() == other.AsFloat();
        case ValueType::STRING:
            return AsString() == other.AsString();
        default:
            return false;  // Objects compared by reference
    }
}

// Array implementation
Value Array::Pop() {
    if (elements_.empty()) {
        throw RuntimeException("Cannot pop from empty array");
    }
    Value value = elements_.back();
    elements_.pop_back();
    return value;
}

// Object implementation
bool Object::HasProperty(const String& key) const {
    return properties_.find(key) != properties_.end();
}

Value Object::GetProperty(const String& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    return Value(); // Return nil
}

void Object::SetProperty(const String& key, const Value& value) {
    properties_[key] = value;
}

bool Object::RemoveProperty(const String& key) {
    return properties_.erase(key) > 0;
}

Vector<String> Object::GetKeys() const {
    Vector<String> keys;
    keys.reserve(properties_.size());
    for (const auto& pair : properties_) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Value::operator>(const Value& other) const {
    // Compare based on types and values
    if (GetType() != other.GetType()) {
        // Type-based comparison for different types
        return static_cast<int>(GetType()) > static_cast<int>(other.GetType());
    }
    
    switch (GetType()) {
        case ValueType::INTEGER:
            return AsInt() > other.AsInt();
        case ValueType::FLOAT:
            return AsFloat() > other.AsFloat();
        case ValueType::STRING:
            return AsString() > other.AsString();
        default:
            return false; // Other types not comparable
    }
}

bool Value::operator>=(const Value& other) const {
    return *this > other || *this == other;
}

bool Value::operator<(const Value& other) const {
    return !(*this >= other);
}

bool Value::operator<=(const Value& other) const {
    return !(*this > other);
}

} // namespace cj