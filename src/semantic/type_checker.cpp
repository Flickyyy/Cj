/**
 * @file type_checker.cpp
 * @brief Type checker implementation
 */

#include "cj/semantic/type_checker.h"
#include "cj/ast/ast_node.h"

namespace cj {

SharedPtr<Type> TypeChecker::CheckExpression(const ASTNode* node) {
    if (!node) {
        AddError("Null AST node");
        return registry_.GetVoidType();
    }
    
    // Check based on node type
    switch (node->GetType()) {
        case ASTNodeType::LITERAL_EXPRESSION:
            return CheckLiteral(static_cast<const LiteralExpression*>(node));
        case ASTNodeType::IDENTIFIER_EXPRESSION:
            return CheckVariable(static_cast<const IdentifierExpression*>(node));
        case ASTNodeType::BINARY_EXPRESSION:
            return CheckBinaryOp(static_cast<const BinaryExpression*>(node));
        default:
            // For now, return int as default for unknown expressions
            return registry_.GetIntType();
    }
}

SharedPtr<Type> TypeChecker::CheckLiteral(const LiteralExpression* literal) {
    if (!literal) {
        AddError("Null literal expression");
        return registry_.GetVoidType();
    }
    
    // Check the actual value type from the literal
    const Value& value = literal->GetValue();
    switch (value.GetType()) {
        case ValueType::INTEGER:
            return registry_.GetIntType();
        case ValueType::FLOAT:
            return registry_.GetFloatType();
        case ValueType::STRING:
            return registry_.GetStringType();
        case ValueType::BOOLEAN:
            return registry_.GetBoolType();
        case ValueType::NIL:
            return registry_.GetVoidType();
        default:
            return registry_.GetIntType(); // Default fallback
    }
}

SharedPtr<Type> TypeChecker::CheckBinaryOp(const BinaryExpression* binary) {
    if (!binary) {
        AddError("Null binary expression");
        return registry_.GetVoidType();
    }
    
    // For now, assume arithmetic operations return int
    return registry_.GetIntType();
}

SharedPtr<Type> TypeChecker::CheckVariable(const IdentifierExpression* identifier) {
    if (!identifier) {
        AddError("Null identifier expression");
        return registry_.GetVoidType();
    }
    
    // For now, assume all variables are int
    return registry_.GetIntType();
}

void TypeChecker::DeclareVariable(const String& name, SharedPtr<Type> type) {
    symbol_table_[name] = type;
}

SharedPtr<Type> TypeChecker::LookupVariable(const String& name) {
    auto it = symbol_table_.find(name);
    if (it != symbol_table_.end()) {
        return it->second;
    }
    return nullptr;
}

void TypeChecker::AddError(const String& error) {
    errors_.push_back(error);
}

bool TypeChecker::AreCompatible(SharedPtr<Type> lhs, SharedPtr<Type> rhs) {
    if (!lhs || !rhs) return false;
    return *lhs == *rhs;
}

SharedPtr<Type> TypeChecker::GetCommonType(SharedPtr<Type> lhs, SharedPtr<Type> rhs) {
    if (AreCompatible(lhs, rhs)) {
        return lhs;
    }
    
    // Simple numeric promotion
    if (lhs->IsNumeric() && rhs->IsNumeric()) {
        if (lhs->GetKind() == TypeKind::FLOAT || rhs->GetKind() == TypeKind::FLOAT) {
            return registry_.GetFloatType();
        }
        return registry_.GetIntType();
    }
    
    return nullptr;
}

SharedPtr<Type> TypeChecker::GetTypeFromString(const String& type_name) {
    if (type_name == "int") {
        return registry_.GetIntType();
    } else if (type_name == "float") {
        return registry_.GetFloatType();
    } else if (type_name == "string") {
        return registry_.GetStringType();
    } else if (type_name == "bool") {
        return registry_.GetBoolType();
    } else if (type_name == "void") {
        return registry_.GetVoidType();
    }
    return nullptr;
}

void TypeChecker::CheckVariableDeclaration(const VariableDeclaration* decl) {
    if (!decl) {
        AddError("Null variable declaration");
        return;
    }
    
    SharedPtr<Type> annotated_type = nullptr;
    SharedPtr<Type> inferred_type = nullptr;
    
    // Check if there's an explicit type annotation
    if (decl->HasTypeAnnotation()) {
        annotated_type = GetTypeFromString(decl->GetTypeAnnotation());
        if (!annotated_type) {
            AddError("Unknown type '" + decl->GetTypeAnnotation() + "' for variable '" + decl->GetName() + "'");
            return;
        }
    }
    
    // Check initializer if present
    if (decl->GetInitializer()) {
        inferred_type = CheckExpression(decl->GetInitializer());
        
        // If we have both annotation and initializer, they must be compatible
        if (annotated_type && inferred_type) {
            if (!AreCompatible(annotated_type, inferred_type)) {
                AddError("Type mismatch: variable '" + decl->GetName() + 
                        "' declared as '" + annotated_type->GetName() + 
                        "' but initialized with '" + inferred_type->GetName() + "'");
                return;
            }
        }
    }
    
    // Determine the final type for the variable
    SharedPtr<Type> final_type;
    if (annotated_type) {
        final_type = annotated_type;
    } else if (inferred_type) {
        final_type = inferred_type;
    } else {
        AddError("Cannot determine type for variable '" + decl->GetName() + "' (no type annotation or initializer)");
        return;
    }
    
    // Declare the variable in symbol table
    DeclareVariable(decl->GetName(), final_type);
}

} // namespace cj