/**
 * @file type_system.h
 * @brief Static type system for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_TYPE_SYSTEM_H
#define CJ_TYPE_SYSTEM_H

#include "../common.h"
#include <variant>

namespace cj {

/**
 * @brief Type kinds in the CJ type system
 */
enum class TypeKind {
    VOID,       // void type
    BOOL,       // boolean type
    INT8,       // 8-bit integer
    INT16,      // 16-bit integer  
    INT32,      // 32-bit integer
    INT64,      // 64-bit integer
    UINT8,      // 8-bit unsigned integer
    UINT16,     // 16-bit unsigned integer
    UINT32,     // 32-bit unsigned integer
    UINT64,     // 64-bit unsigned integer
    FLOAT32,    // 32-bit floating point
    FLOAT64,    // 64-bit floating point
    STRING,     // UTF-8 string
    CHAR,       // UTF-8 character
    ARRAY,      // array type
    TUPLE,      // tuple type
    STRUCT,     // struct type
    ENUM,       // enum type
    UNION,      // union type
    FUNCTION,   // function type
    REFERENCE,  // reference type (no raw pointers)
    OPTIONAL,   // optional type (for null safety)
    GENERIC,    // generic type parameter
    NEVER       // never type (for functions that don't return)
};

/**
 * @brief Type mutability qualifiers
 */
enum class TypeQualifier {
    CONST,      // immutable by default
    MUTABLE     // explicitly mutable
};

// Forward declarations
class Type;
class ArrayType;
class TupleType;
class StructType;
class EnumType;
class UnionType;
class FunctionType;
class ReferenceType;
class OptionalType;
class GenericType;

/**
 * @brief Base class for all types in the CJ type system
 */
class Type {
protected:
    TypeKind kind_;
    TypeQualifier qualifier_;
    String name_;
    Size size_;
    Size alignment_;

public:
    Type(TypeKind kind, TypeQualifier qualifier = TypeQualifier::CONST)
        : kind_(kind), qualifier_(qualifier), size_(0), alignment_(0) {}
    
    virtual ~Type() = default;
    
    // Type identification
    TypeKind GetKind() const { return kind_; }
    TypeQualifier GetQualifier() const { return qualifier_; }
    const String& GetName() const { return name_; }
    
    // Type properties
    Size GetSize() const { return size_; }
    Size GetAlignment() const { return alignment_; }
    
    // Type checking
    bool IsConst() const { return qualifier_ == TypeQualifier::CONST; }
    bool IsMutable() const { return qualifier_ == TypeQualifier::MUTABLE; }
    bool IsPrimitive() const;
    bool IsNumeric() const;
    bool IsInteger() const;
    bool IsFloatingPoint() const;
    bool IsReference() const { return kind_ == TypeKind::REFERENCE; }
    bool IsOptional() const { return kind_ == TypeKind::OPTIONAL; }
    bool IsFunction() const { return kind_ == TypeKind::FUNCTION; }
    bool IsComposite() const;
    
    // Type operations
    virtual bool IsAssignableFrom(const Type& other) const;
    virtual bool IsCompatibleWith(const Type& other) const;
    virtual SharedPtr<Type> WithQualifier(TypeQualifier qualifier) const;
    virtual String ToString() const;
    
    // Type equivalence
    virtual bool operator==(const Type& other) const;
    bool operator!=(const Type& other) const { return !(*this == other); }
    
    // Casting
    template<typename T>
    const T* As() const {
        return dynamic_cast<const T*>(this);
    }
    
    template<typename T>
    T* As() {
        return dynamic_cast<T*>(this);
    }
};

/**
 * @brief Primitive types (void, bool, integers, floats, char)
 */
class PrimitiveType : public Type {
public:
    explicit PrimitiveType(TypeKind kind, TypeQualifier qualifier = TypeQualifier::CONST);
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
};

/**
 * @brief String type (UTF-8)
 */
class StringType : public Type {
public:
    explicit StringType(TypeQualifier qualifier = TypeQualifier::CONST);
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
};

/**
 * @brief Array type with compile-time known size
 */
class ArrayType : public Type {
private:
    SharedPtr<Type> element_type_;
    Size array_size_;
    
public:
    ArrayType(SharedPtr<Type> element_type, Size size, TypeQualifier qualifier = TypeQualifier::CONST);
    
    SharedPtr<Type> GetElementType() const { return element_type_; }
    Size GetArraySize() const { return array_size_; }
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Tuple type for multiple values
 */
class TupleType : public Type {
private:
    Vector<SharedPtr<Type>> element_types_;
    
public:
    TupleType(Vector<SharedPtr<Type>> element_types, TypeQualifier qualifier = TypeQualifier::CONST);
    
    const Vector<SharedPtr<Type>>& GetElementTypes() const { return element_types_; }
    Size GetElementCount() const { return element_types_.size(); }
    SharedPtr<Type> GetElementType(Size index) const;
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Struct field
 */
struct StructField {
    String name;
    SharedPtr<Type> type;
    Size offset;
    
    StructField(const String& n, SharedPtr<Type> t, Size o = 0)
        : name(n), type(t), offset(o) {}
};

/**
 * @brief Struct type for user-defined structures
 */
class StructType : public Type {
private:
    Vector<StructField> fields_;
    HashMap<String, Size> field_indices_;
    
public:
    StructType(const String& name, Vector<StructField> fields, TypeQualifier qualifier = TypeQualifier::CONST);
    
    const Vector<StructField>& GetFields() const { return fields_; }
    Size GetFieldCount() const { return fields_.size(); }
    const StructField* GetField(const String& name) const;
    Size GetFieldIndex(const String& name) const;
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Enum variant
 */
struct EnumVariant {
    String name;
    Int64 value;
    
    EnumVariant(const String& n, Int64 v) : name(n), value(v) {}
};

/**
 * @brief Enum type
 */
class EnumType : public Type {
private:
    Vector<EnumVariant> variants_;
    HashMap<String, Size> variant_indices_;
    SharedPtr<Type> underlying_type_;
    
public:
    EnumType(const String& name, Vector<EnumVariant> variants, 
             SharedPtr<Type> underlying_type = nullptr, TypeQualifier qualifier = TypeQualifier::CONST);
    
    const Vector<EnumVariant>& GetVariants() const { return variants_; }
    Size GetVariantCount() const { return variants_.size(); }
    const EnumVariant* GetVariant(const String& name) const;
    SharedPtr<Type> GetUnderlyingType() const { return underlying_type_; }
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Function parameter
 */
struct FunctionParameter {
    String name;
    SharedPtr<Type> type;
    bool has_default;
    
    FunctionParameter(const String& n, SharedPtr<Type> t, bool has_def = false)
        : name(n), type(t), has_default(has_def) {}
};

/**
 * @brief Function type
 */
class FunctionType : public Type {
private:
    Vector<FunctionParameter> parameters_;
    SharedPtr<Type> return_type_;
    bool is_variadic_;
    
public:
    FunctionType(Vector<FunctionParameter> parameters, SharedPtr<Type> return_type, 
                bool is_variadic = false, TypeQualifier qualifier = TypeQualifier::CONST);
    
    const Vector<FunctionParameter>& GetParameters() const { return parameters_; }
    Size GetParameterCount() const { return parameters_.size(); }
    SharedPtr<Type> GetReturnType() const { return return_type_; }
    bool IsVariadic() const { return is_variadic_; }
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Reference type (replaces raw pointers)
 */
class ReferenceType : public Type {
private:
    SharedPtr<Type> referenced_type_;
    
public:
    ReferenceType(SharedPtr<Type> referenced_type, TypeQualifier qualifier = TypeQualifier::CONST);
    
    SharedPtr<Type> GetReferencedType() const { return referenced_type_; }
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Optional type for null safety
 */
class OptionalType : public Type {
private:
    SharedPtr<Type> value_type_;
    
public:
    OptionalType(SharedPtr<Type> value_type, TypeQualifier qualifier = TypeQualifier::CONST);
    
    SharedPtr<Type> GetValueType() const { return value_type_; }
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Generic type parameter
 */
class GenericType : public Type {
private:
    String parameter_name_;
    Vector<SharedPtr<Type>> constraints_;
    
public:
    GenericType(const String& name, Vector<SharedPtr<Type>> constraints = {}, 
               TypeQualifier qualifier = TypeQualifier::CONST);
    
    const String& GetParameterName() const { return parameter_name_; }
    const Vector<SharedPtr<Type>>& GetConstraints() const { return constraints_; }
    
    String ToString() const override;
    bool IsAssignableFrom(const Type& other) const override;
    bool operator==(const Type& other) const override;
};

/**
 * @brief Type registry for managing all types
 */
class TypeRegistry {
private:
    HashMap<String, SharedPtr<Type>> named_types_;
    Vector<SharedPtr<Type>> primitive_types_;
    
    static UniquePtr<TypeRegistry> instance_;
    
    TypeRegistry();
    void InitializePrimitiveTypes();
    
public:
    static TypeRegistry& Instance();
    
    // Primitive type accessors
    SharedPtr<Type> GetVoidType();
    SharedPtr<Type> GetBoolType();
    SharedPtr<Type> GetInt8Type();
    SharedPtr<Type> GetInt16Type();
    SharedPtr<Type> GetInt32Type();
    SharedPtr<Type> GetInt64Type();
    SharedPtr<Type> GetUInt8Type();
    SharedPtr<Type> GetUInt16Type();
    SharedPtr<Type> GetUInt32Type();
    SharedPtr<Type> GetUInt64Type();
    SharedPtr<Type> GetFloat32Type();
    SharedPtr<Type> GetFloat64Type();
    SharedPtr<Type> GetStringType();
    SharedPtr<Type> GetCharType();
    SharedPtr<Type> GetNeverType();
    
    // Type creation
    SharedPtr<ArrayType> CreateArrayType(SharedPtr<Type> element_type, Size size);
    SharedPtr<TupleType> CreateTupleType(Vector<SharedPtr<Type>> element_types);
    SharedPtr<StructType> CreateStructType(const String& name, Vector<StructField> fields);
    SharedPtr<EnumType> CreateEnumType(const String& name, Vector<EnumVariant> variants);
    SharedPtr<FunctionType> CreateFunctionType(Vector<FunctionParameter> parameters, SharedPtr<Type> return_type);
    SharedPtr<ReferenceType> CreateReferenceType(SharedPtr<Type> referenced_type);
    SharedPtr<OptionalType> CreateOptionalType(SharedPtr<Type> value_type);
    SharedPtr<GenericType> CreateGenericType(const String& name, Vector<SharedPtr<Type>> constraints = {});
    
    // Type registration
    void RegisterType(const String& name, SharedPtr<Type> type);
    SharedPtr<Type> LookupType(const String& name) const;
    bool HasType(const String& name) const;
    
    // Type compatibility checking
    bool AreTypesCompatible(const Type& lhs, const Type& rhs) const;
    SharedPtr<Type> GetCommonType(const Type& lhs, const Type& rhs) const;
    
    // Type inference helpers
    SharedPtr<Type> InferType(const class ASTNode& node) const;
};

} // namespace cj

#endif // CJ_TYPE_SYSTEM_H