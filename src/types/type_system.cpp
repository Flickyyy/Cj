/**
 * @file type_system.cpp
 * @brief Implementation of the static type system for CJ language
 */

#include "cj/types/type_system.h"
#include "cj/ast/ast_node.h"
#include <sstream>
#include <algorithm>

namespace cj {

// Type implementation
bool Type::IsPrimitive() const {
    switch (kind_) {
        case TypeKind::VOID:
        case TypeKind::BOOL:
        case TypeKind::INT8:
        case TypeKind::INT16:
        case TypeKind::INT32:
        case TypeKind::INT64:
        case TypeKind::UINT8:
        case TypeKind::UINT16:
        case TypeKind::UINT32:
        case TypeKind::UINT64:
        case TypeKind::FLOAT32:
        case TypeKind::FLOAT64:
        case TypeKind::CHAR:
            return true;
        default:
            return false;
    }
}

bool Type::IsNumeric() const {
    return IsInteger() || IsFloatingPoint();
}

bool Type::IsInteger() const {
    switch (kind_) {
        case TypeKind::INT8:
        case TypeKind::INT16:
        case TypeKind::INT32:
        case TypeKind::INT64:
        case TypeKind::UINT8:
        case TypeKind::UINT16:
        case TypeKind::UINT32:
        case TypeKind::UINT64:
            return true;
        default:
            return false;
    }
}

bool Type::IsFloatingPoint() const {
    return kind_ == TypeKind::FLOAT32 || kind_ == TypeKind::FLOAT64;
}

bool Type::IsComposite() const {
    switch (kind_) {
        case TypeKind::ARRAY:
        case TypeKind::TUPLE:
        case TypeKind::STRUCT:
        case TypeKind::UNION:
            return true;
        default:
            return false;
    }
}

bool Type::IsAssignableFrom(const Type& other) const {
    // Default implementation: exact type match
    return *this == other;
}

bool Type::IsCompatibleWith(const Type& other) const {
    // Default implementation: same as assignability
    return IsAssignableFrom(other);
}

SharedPtr<Type> Type::WithQualifier(TypeQualifier qualifier) const {
    // This should be overridden by derived classes
    return nullptr;
}

String Type::ToString() const {
    std::ostringstream oss;
    if (qualifier_ == TypeQualifier::MUTABLE) {
        oss << "mut ";
    }
    oss << name_;
    return oss.str();
}

bool Type::operator==(const Type& other) const {
    return kind_ == other.kind_ && 
           qualifier_ == other.qualifier_ && 
           name_ == other.name_;
}

// PrimitiveType implementation
PrimitiveType::PrimitiveType(TypeKind kind, TypeQualifier qualifier) : Type(kind, qualifier) {
    switch (kind) {
        case TypeKind::VOID:
            name_ = "void";
            size_ = 0;
            alignment_ = 0;
            break;
        case TypeKind::BOOL:
            name_ = "bool";
            size_ = 1;
            alignment_ = 1;
            break;
        case TypeKind::INT8:
            name_ = "i8";
            size_ = 1;
            alignment_ = 1;
            break;
        case TypeKind::INT16:
            name_ = "i16";
            size_ = 2;
            alignment_ = 2;
            break;
        case TypeKind::INT32:
            name_ = "i32";
            size_ = 4;
            alignment_ = 4;
            break;
        case TypeKind::INT64:
            name_ = "i64";
            size_ = 8;
            alignment_ = 8;
            break;
        case TypeKind::UINT8:
            name_ = "u8";
            size_ = 1;
            alignment_ = 1;
            break;
        case TypeKind::UINT16:
            name_ = "u16";
            size_ = 2;
            alignment_ = 2;
            break;
        case TypeKind::UINT32:
            name_ = "u32";
            size_ = 4;
            alignment_ = 4;
            break;
        case TypeKind::UINT64:
            name_ = "u64";
            size_ = 8;
            alignment_ = 8;
            break;
        case TypeKind::FLOAT32:
            name_ = "f32";
            size_ = 4;
            alignment_ = 4;
            break;
        case TypeKind::FLOAT64:
            name_ = "f64";
            size_ = 8;
            alignment_ = 8;
            break;
        case TypeKind::CHAR:
            name_ = "char";
            size_ = 4; // UTF-8 can be up to 4 bytes
            alignment_ = 4;
            break;
        case TypeKind::NEVER:
            name_ = "never";
            size_ = 0;
            alignment_ = 0;
            break;
        default:
            throw std::invalid_argument("Invalid primitive type kind");
    }
}

String PrimitiveType::ToString() const {
    return Type::ToString();
}

bool PrimitiveType::IsAssignableFrom(const Type& other) const {
    if (other.GetKind() == kind_) {
        return true;
    }
    
    // Allow some numeric conversions
    if (IsNumeric() && other.IsNumeric()) {
        // For now, allow all numeric conversions
        // TODO: Add proper conversion rules
        return true;
    }
    
    return false;
}

// StringType implementation  
StringType::StringType(TypeQualifier qualifier) : Type(TypeKind::STRING, qualifier) {
    name_ = "string";
    size_ = sizeof(void*); // String is implemented as a pointer to string data
    alignment_ = alignof(void*);
}

String StringType::ToString() const {
    return Type::ToString();
}

bool StringType::IsAssignableFrom(const Type& other) const {
    return other.GetKind() == TypeKind::STRING;
}

// ArrayType implementation
ArrayType::ArrayType(SharedPtr<Type> element_type, Size size, TypeQualifier qualifier) 
    : Type(TypeKind::ARRAY, qualifier), element_type_(element_type), array_size_(size) {
    name_ = "[" + element_type->ToString() + "; " + std::to_string(size) + "]";
    size_ = element_type->GetSize() * size;
    alignment_ = element_type->GetAlignment();
}

String ArrayType::ToString() const {
    return Type::ToString();
}

bool ArrayType::IsAssignableFrom(const Type& other) const {
    if (other.GetKind() != TypeKind::ARRAY) {
        return false;
    }
    
    const ArrayType* other_array = other.As<ArrayType>();
    return other_array && 
           array_size_ == other_array->array_size_ &&
           element_type_->IsAssignableFrom(*other_array->element_type_);
}

bool ArrayType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const ArrayType* other_array = other.As<ArrayType>();
    return other_array &&
           array_size_ == other_array->array_size_ &&
           *element_type_ == *other_array->element_type_;
}

// TupleType implementation
TupleType::TupleType(Vector<SharedPtr<Type>> element_types, TypeQualifier qualifier)
    : Type(TypeKind::TUPLE, qualifier), element_types_(std::move(element_types)) {
    
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < element_types_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << element_types_[i]->ToString();
    }
    oss << ")";
    name_ = oss.str();
    
    // Calculate size and alignment
    size_ = 0;
    alignment_ = 1;
    for (const auto& type : element_types_) {
        size_ = (size_ + type->GetAlignment() - 1) & ~(type->GetAlignment() - 1);
        size_ += type->GetSize();
        alignment_ = std::max(alignment_, type->GetAlignment());
    }
}

SharedPtr<Type> TupleType::GetElementType(Size index) const {
    if (index >= element_types_.size()) {
        return nullptr;
    }
    return element_types_[index];
}

String TupleType::ToString() const {
    return Type::ToString();
}

bool TupleType::IsAssignableFrom(const Type& other) const {
    if (other.GetKind() != TypeKind::TUPLE) {
        return false;
    }
    
    const TupleType* other_tuple = other.As<TupleType>();
    if (!other_tuple || element_types_.size() != other_tuple->element_types_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < element_types_.size(); ++i) {
        if (!element_types_[i]->IsAssignableFrom(*other_tuple->element_types_[i])) {
            return false;
        }
    }
    
    return true;
}

bool TupleType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const TupleType* other_tuple = other.As<TupleType>();
    if (!other_tuple || element_types_.size() != other_tuple->element_types_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < element_types_.size(); ++i) {
        if (*element_types_[i] != *other_tuple->element_types_[i]) {
            return false;
        }
    }
    
    return true;
}

// StructType implementation
StructType::StructType(const String& name, Vector<StructField> fields, TypeQualifier qualifier)
    : Type(TypeKind::STRUCT, qualifier), fields_(std::move(fields)) {
    name_ = name;
    
    // Build field index map and calculate layout
    size_ = 0;
    alignment_ = 1;
    
    for (size_t i = 0; i < fields_.size(); ++i) {
        field_indices_[fields_[i].name] = i;
        
        // Align field
        Size field_align = fields_[i].type->GetAlignment();
        size_ = (size_ + field_align - 1) & ~(field_align - 1);
        fields_[i].offset = size_;
        
        size_ += fields_[i].type->GetSize();
        alignment_ = std::max(alignment_, field_align);
    }
    
    // Align struct size to its alignment
    size_ = (size_ + alignment_ - 1) & ~(alignment_ - 1);
}

const StructField* StructType::GetField(const String& name) const {
    auto it = field_indices_.find(name);
    if (it != field_indices_.end()) {
        return &fields_[it->second];
    }
    return nullptr;
}

Size StructType::GetFieldIndex(const String& name) const {
    auto it = field_indices_.find(name);
    return it != field_indices_.end() ? it->second : SIZE_MAX;
}

String StructType::ToString() const {
    return Type::ToString();
}

bool StructType::IsAssignableFrom(const Type& other) const {
    // Structs are only assignable from the same struct type
    return other.GetKind() == TypeKind::STRUCT && name_ == other.GetName();
}

bool StructType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const StructType* other_struct = other.As<StructType>();
    if (!other_struct || fields_.size() != other_struct->fields_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < fields_.size(); ++i) {
        if (fields_[i].name != other_struct->fields_[i].name ||
            *fields_[i].type != *other_struct->fields_[i].type) {
            return false;
        }
    }
    
    return true;
}

// EnumType implementation
EnumType::EnumType(const String& name, Vector<EnumVariant> variants, 
                   SharedPtr<Type> underlying_type, TypeQualifier qualifier)
    : Type(TypeKind::ENUM, qualifier), variants_(std::move(variants)), underlying_type_(underlying_type) {
    name_ = name;
    
    // Default underlying type is i32
    if (!underlying_type_) {
        underlying_type_ = TypeRegistry::Instance().GetInt32Type();
    }
    
    size_ = underlying_type_->GetSize();
    alignment_ = underlying_type_->GetAlignment();
    
    // Build variant index map
    for (size_t i = 0; i < variants_.size(); ++i) {
        variant_indices_[variants_[i].name] = i;
    }
}

const EnumVariant* EnumType::GetVariant(const String& name) const {
    auto it = variant_indices_.find(name);
    if (it != variant_indices_.end()) {
        return &variants_[it->second];
    }
    return nullptr;
}

String EnumType::ToString() const {
    return Type::ToString();
}

bool EnumType::IsAssignableFrom(const Type& other) const {
    // Enums are only assignable from the same enum type
    return other.GetKind() == TypeKind::ENUM && name_ == other.GetName();
}

bool EnumType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const EnumType* other_enum = other.As<EnumType>();
    if (!other_enum || variants_.size() != other_enum->variants_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < variants_.size(); ++i) {
        if (variants_[i].name != other_enum->variants_[i].name ||
            variants_[i].value != other_enum->variants_[i].value) {
            return false;
        }
    }
    
    return *underlying_type_ == *other_enum->underlying_type_;
}

// FunctionType implementation
FunctionType::FunctionType(Vector<FunctionParameter> parameters, SharedPtr<Type> return_type, 
                          bool is_variadic, TypeQualifier qualifier)
    : Type(TypeKind::FUNCTION, qualifier), parameters_(std::move(parameters)), 
      return_type_(return_type), is_variadic_(is_variadic) {
    
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters_[i].type->ToString();
    }
    if (is_variadic_) {
        if (!parameters_.empty()) oss << ", ";
        oss << "...";
    }
    oss << ") -> " << return_type_->ToString();
    name_ = oss.str();
    
    size_ = sizeof(void*); // Function pointer size
    alignment_ = alignof(void*);
}

String FunctionType::ToString() const {
    return Type::ToString();
}

bool FunctionType::IsAssignableFrom(const Type& other) const {
    if (other.GetKind() != TypeKind::FUNCTION) {
        return false;
    }
    
    const FunctionType* other_func = other.As<FunctionType>();
    if (!other_func || 
        parameters_.size() != other_func->parameters_.size() ||
        is_variadic_ != other_func->is_variadic_) {
        return false;
    }
    
    // Check parameter types (contravariant)
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (!other_func->parameters_[i].type->IsAssignableFrom(*parameters_[i].type)) {
            return false;
        }
    }
    
    // Check return type (covariant)
    return return_type_->IsAssignableFrom(*other_func->return_type_);
}

bool FunctionType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const FunctionType* other_func = other.As<FunctionType>();
    if (!other_func || 
        parameters_.size() != other_func->parameters_.size() ||
        is_variadic_ != other_func->is_variadic_) {
        return false;
    }
    
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (*parameters_[i].type != *other_func->parameters_[i].type) {
            return false;
        }
    }
    
    return *return_type_ == *other_func->return_type_;
}

// ReferenceType implementation
ReferenceType::ReferenceType(SharedPtr<Type> referenced_type, TypeQualifier qualifier)
    : Type(TypeKind::REFERENCE, qualifier), referenced_type_(referenced_type) {
    name_ = "&" + referenced_type->ToString();
    size_ = sizeof(void*);
    alignment_ = alignof(void*);
}

String ReferenceType::ToString() const {
    return Type::ToString();
}

bool ReferenceType::IsAssignableFrom(const Type& other) const {
    if (other.GetKind() != TypeKind::REFERENCE) {
        return false;
    }
    
    const ReferenceType* other_ref = other.As<ReferenceType>();
    return other_ref && referenced_type_->IsAssignableFrom(*other_ref->referenced_type_);
}

bool ReferenceType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const ReferenceType* other_ref = other.As<ReferenceType>();
    return other_ref && *referenced_type_ == *other_ref->referenced_type_;
}

// OptionalType implementation
OptionalType::OptionalType(SharedPtr<Type> value_type, TypeQualifier qualifier)
    : Type(TypeKind::OPTIONAL, qualifier), value_type_(value_type) {
    name_ = value_type->ToString() + "?";
    size_ = value_type->GetSize() + 1; // Add one byte for null flag
    alignment_ = value_type->GetAlignment();
}

String OptionalType::ToString() const {
    return Type::ToString();
}

bool OptionalType::IsAssignableFrom(const Type& other) const {
    if (other.GetKind() == TypeKind::OPTIONAL) {
        const OptionalType* other_opt = other.As<OptionalType>();
        return other_opt && value_type_->IsAssignableFrom(*other_opt->value_type_);
    }
    
    // Can assign the value type directly to optional
    return value_type_->IsAssignableFrom(other);
}

bool OptionalType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const OptionalType* other_opt = other.As<OptionalType>();
    return other_opt && *value_type_ == *other_opt->value_type_;
}

// GenericType implementation
GenericType::GenericType(const String& name, Vector<SharedPtr<Type>> constraints, TypeQualifier qualifier)
    : Type(TypeKind::GENERIC, qualifier), parameter_name_(name), constraints_(std::move(constraints)) {
    name_ = name;
    size_ = 0; // Generic types don't have concrete size
    alignment_ = 0;
}

String GenericType::ToString() const {
    return Type::ToString();
}

bool GenericType::IsAssignableFrom(const Type& other) const {
    // Check if other type satisfies all constraints
    for (const auto& constraint : constraints_) {
        if (!constraint->IsAssignableFrom(other)) {
            return false;
        }
    }
    return true;
}

bool GenericType::operator==(const Type& other) const {
    if (!Type::operator==(other)) {
        return false;
    }
    
    const GenericType* other_generic = other.As<GenericType>();
    if (!other_generic || constraints_.size() != other_generic->constraints_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < constraints_.size(); ++i) {
        if (*constraints_[i] != *other_generic->constraints_[i]) {
            return false;
        }
    }
    
    return true;
}

// TypeRegistry implementation
UniquePtr<TypeRegistry> TypeRegistry::instance_ = nullptr;

TypeRegistry::TypeRegistry() {
    InitializePrimitiveTypes();
}

void TypeRegistry::InitializePrimitiveTypes() {
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::VOID));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::BOOL));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::INT8));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::INT16));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::INT32));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::INT64));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::UINT8));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::UINT16));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::UINT32));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::UINT64));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::FLOAT32));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::FLOAT64));
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::CHAR));
    primitive_types_.push_back(cj_make_shared<StringType>());
    primitive_types_.push_back(cj_make_shared<PrimitiveType>(TypeKind::NEVER));
    
    // Register named types
    for (const auto& type : primitive_types_) {
        named_types_[type->GetName()] = type;
    }
}

TypeRegistry& TypeRegistry::Instance() {
    if (!instance_) {
        instance_ = cj_make_unique<TypeRegistry>();
    }
    return *instance_;
}

SharedPtr<Type> TypeRegistry::GetVoidType() { return named_types_["void"]; }
SharedPtr<Type> TypeRegistry::GetBoolType() { return named_types_["bool"]; }
SharedPtr<Type> TypeRegistry::GetInt8Type() { return named_types_["i8"]; }
SharedPtr<Type> TypeRegistry::GetInt16Type() { return named_types_["i16"]; }
SharedPtr<Type> TypeRegistry::GetInt32Type() { return named_types_["i32"]; }
SharedPtr<Type> TypeRegistry::GetInt64Type() { return named_types_["i64"]; }
SharedPtr<Type> TypeRegistry::GetUInt8Type() { return named_types_["u8"]; }
SharedPtr<Type> TypeRegistry::GetUInt16Type() { return named_types_["u16"]; }
SharedPtr<Type> TypeRegistry::GetUInt32Type() { return named_types_["u32"]; }
SharedPtr<Type> TypeRegistry::GetUInt64Type() { return named_types_["u64"]; }
SharedPtr<Type> TypeRegistry::GetFloat32Type() { return named_types_["f32"]; }
SharedPtr<Type> TypeRegistry::GetFloat64Type() { return named_types_["f64"]; }
SharedPtr<Type> TypeRegistry::GetStringType() { return named_types_["string"]; }
SharedPtr<Type> TypeRegistry::GetCharType() { return named_types_["char"]; }
SharedPtr<Type> TypeRegistry::GetNeverType() { return named_types_["never"]; }

SharedPtr<ArrayType> TypeRegistry::CreateArrayType(SharedPtr<Type> element_type, Size size) {
    return cj_make_shared<ArrayType>(element_type, size);
}

SharedPtr<TupleType> TypeRegistry::CreateTupleType(Vector<SharedPtr<Type>> element_types) {
    return cj_make_shared<TupleType>(std::move(element_types));
}

SharedPtr<StructType> TypeRegistry::CreateStructType(const String& name, Vector<StructField> fields) {
    auto type = cj_make_shared<StructType>(name, std::move(fields));
    RegisterType(name, type);
    return type;
}

SharedPtr<EnumType> TypeRegistry::CreateEnumType(const String& name, Vector<EnumVariant> variants) {
    auto type = cj_make_shared<EnumType>(name, std::move(variants));
    RegisterType(name, type);
    return type;
}

SharedPtr<FunctionType> TypeRegistry::CreateFunctionType(Vector<FunctionParameter> parameters, SharedPtr<Type> return_type) {
    return cj_make_shared<FunctionType>(std::move(parameters), return_type);
}

SharedPtr<ReferenceType> TypeRegistry::CreateReferenceType(SharedPtr<Type> referenced_type) {
    return cj_make_shared<ReferenceType>(referenced_type);
}

SharedPtr<OptionalType> TypeRegistry::CreateOptionalType(SharedPtr<Type> value_type) {
    return cj_make_shared<OptionalType>(value_type);
}

SharedPtr<GenericType> TypeRegistry::CreateGenericType(const String& name, Vector<SharedPtr<Type>> constraints) {
    return cj_make_shared<GenericType>(name, std::move(constraints));
}

void TypeRegistry::RegisterType(const String& name, SharedPtr<Type> type) {
    named_types_[name] = type;
}

SharedPtr<Type> TypeRegistry::LookupType(const String& name) const {
    auto it = named_types_.find(name);
    return it != named_types_.end() ? it->second : nullptr;
}

bool TypeRegistry::HasType(const String& name) const {
    return named_types_.find(name) != named_types_.end();
}

bool TypeRegistry::AreTypesCompatible(const Type& lhs, const Type& rhs) const {
    return lhs.IsCompatibleWith(rhs);
}

SharedPtr<Type> TypeRegistry::GetCommonType(const Type& lhs, const Type& rhs) const {
    if (lhs == rhs) {
        return cj_make_shared<Type>(lhs);
    }
    
    // For numeric types, promote to the larger type
    if (lhs.IsNumeric() && rhs.IsNumeric()) {
        if (lhs.GetSize() >= rhs.GetSize()) {
            return cj_make_shared<Type>(lhs);
        } else {
            return cj_make_shared<Type>(rhs);
        }
    }
    
    return nullptr; // No common type
}

SharedPtr<Type> TypeRegistry::InferType(const ASTNode& node) const {
    // This is a stub - would need full implementation based on AST node types
    // For now, return i32 as default
    return GetInt32Type();
}

} // namespace cj