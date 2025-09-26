/**
 * @file type_checker.h
 * @brief Type checker for CJ language
 */

#ifndef CJ_TYPE_CHECKER_H
#define CJ_TYPE_CHECKER_H

#include "../common.h"
#include "../types/type_system.h"
#include "../ast/ast_node.h"

namespace cj {

/**
 * @brief Type checking errors
 */
class TypeCheckError : public std::runtime_error {
public:
    explicit TypeCheckError(const String& message) : std::runtime_error(message) {}
};

/**
 * @brief Simple type checker for CJ
 */
class TypeChecker {
private:
    TypeRegistry& registry_;
    HashMap<String, SharedPtr<Type>> symbol_table_;
    Vector<String> errors_;

public:
    TypeChecker() : registry_(TypeRegistry::Instance()) {}
    
    // Type checking methods
    SharedPtr<Type> CheckExpression(const ASTNode* node);
    SharedPtr<Type> CheckLiteral(const class LiteralExpression* literal);
    SharedPtr<Type> CheckBinaryOp(const class BinaryExpression* binary);
    SharedPtr<Type> CheckVariable(const class IdentifierExpression* identifier);
    void CheckVariableDeclaration(const class VariableDeclaration* decl);
    
    // Symbol table management
    void DeclareVariable(const String& name, SharedPtr<Type> type);
    SharedPtr<Type> LookupVariable(const String& name);
    
    // Type utilities
    SharedPtr<Type> GetTypeFromString(const String& type_name);
    
    // Error handling
    void AddError(const String& error);
    const Vector<String>& GetErrors() const { return errors_; }
    bool HasErrors() const { return !errors_.empty(); }
    void ClearErrors() { errors_.clear(); }
    
    // Type compatibility
    bool AreCompatible(SharedPtr<Type> lhs, SharedPtr<Type> rhs);
    SharedPtr<Type> GetCommonType(SharedPtr<Type> lhs, SharedPtr<Type> rhs);
};

} // namespace cj

#endif // CJ_TYPE_CHECKER_H