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
    
    // For now, return basic types based on simple heuristics
    // This is a simplified implementation without full AST integration
    return registry_.GetIntType(); // Default for now
}

SharedPtr<Type> TypeChecker::CheckLiteral(const LiteralExpression* literal) {
    if (!literal) {
        AddError("Null literal expression");
        return registry_.GetVoidType();
    }
    
    // This is a simplified implementation
    // In a real implementation, we'd examine the literal value
    return registry_.GetIntType(); // Default for now
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

} // namespace cj