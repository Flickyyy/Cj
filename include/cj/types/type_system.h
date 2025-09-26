/**
 * @file type_system.h
 * @brief Simple static type system for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_TYPE_SYSTEM_H
#define CJ_TYPE_SYSTEM_H

#include "../common.h"
#include <memory>

namespace cj {

/**
 * @brief Basic type kinds in CJ
 */
enum class TypeKind {
    VOID,
    BOOL,
    INT,      // 32-bit signed integer
    FLOAT,    // 64-bit floating point
    STRING,   // UTF-8 string
    ARRAY,    // array type
    FUNCTION  // function type
};

/**
 * @brief Simple type representation
 */
class Type {
private:
    TypeKind kind_;
    String name_;

public:
    explicit Type(TypeKind kind) : kind_(kind) {
        switch (kind) {
            case TypeKind::VOID: name_ = "void"; break;
            case TypeKind::BOOL: name_ = "bool"; break;
            case TypeKind::INT: name_ = "int"; break;  
            case TypeKind::FLOAT: name_ = "float"; break;
            case TypeKind::STRING: name_ = "string"; break;
            case TypeKind::ARRAY: name_ = "array"; break;
            case TypeKind::FUNCTION: name_ = "function"; break;
        }
    }
    
    TypeKind GetKind() const { return kind_; }
    const String& GetName() const { return name_; }
    
    bool IsNumeric() const { 
        return kind_ == TypeKind::INT || kind_ == TypeKind::FLOAT; 
    }
    
    bool operator==(const Type& other) const { 
        return kind_ == other.kind_; 
    }
    
    bool operator!=(const Type& other) const { 
        return !(*this == other); 
    }
};

/**
 * @brief Type registry for basic types
 */
class TypeRegistry {
private:
    static UniquePtr<TypeRegistry> instance_;
    HashMap<TypeKind, SharedPtr<Type>> types_;

    TypeRegistry() {
        types_[TypeKind::VOID] = std::make_shared<Type>(TypeKind::VOID);
        types_[TypeKind::BOOL] = std::make_shared<Type>(TypeKind::BOOL);
        types_[TypeKind::INT] = std::make_shared<Type>(TypeKind::INT);
        types_[TypeKind::FLOAT] = std::make_shared<Type>(TypeKind::FLOAT);
        types_[TypeKind::STRING] = std::make_shared<Type>(TypeKind::STRING);
        types_[TypeKind::ARRAY] = std::make_shared<Type>(TypeKind::ARRAY);
        types_[TypeKind::FUNCTION] = std::make_shared<Type>(TypeKind::FUNCTION);
    }

public:
    static TypeRegistry& Instance() {
        if (!instance_) {
            instance_ = UniquePtr<TypeRegistry>(new TypeRegistry());
        }
        return *instance_;
    }
    
    SharedPtr<Type> GetType(TypeKind kind) {
        return types_[kind];
    }
    
    SharedPtr<Type> GetVoidType() { return GetType(TypeKind::VOID); }
    SharedPtr<Type> GetBoolType() { return GetType(TypeKind::BOOL); }
    SharedPtr<Type> GetIntType() { return GetType(TypeKind::INT); }
    SharedPtr<Type> GetFloatType() { return GetType(TypeKind::FLOAT); }
    SharedPtr<Type> GetStringType() { return GetType(TypeKind::STRING); }
    SharedPtr<Type> GetArrayType() { return GetType(TypeKind::ARRAY); }
    SharedPtr<Type> GetFunctionType() { return GetType(TypeKind::FUNCTION); }
};

/**
 * @brief Typed value that integrates with existing Value system
 */
class TypedValue {
private:
    SharedPtr<Type> type_;
    class Value* value_;  // Forward reference to existing Value class

public:
    TypedValue(SharedPtr<Type> type, class Value* value) 
        : type_(type), value_(value) {}
    
    SharedPtr<Type> GetType() const { return type_; }
    class Value* GetValue() const { return value_; }
    
    bool IsCompatibleWith(const TypedValue& other) const {
        return *type_ == *other.type_;
    }
};

} // namespace cj

#endif // CJ_TYPE_SYSTEM_H