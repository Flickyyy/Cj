/**
 * @file ast_node.cpp
 * @brief Implementation of AST nodes for CJ language
 */

#include "cj/ast/ast_node.h"
#include "cj/ast/ast_visitor.h"

namespace cj {

String Program::ToString() const {
    return "Program with " + std::to_string(statements_.size()) + " statements";
}

void Program::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String LiteralExpression::ToString() const {
    return "Literal(" + value_.ToString() + ")";
}

void LiteralExpression::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String IdentifierExpression::ToString() const {
    return "Identifier(" + name_ + ")";
}

void IdentifierExpression::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String BinaryExpression::ToString() const {
    return "Binary(" + left_->ToString() + " " + 
           TokenUtils::TokenTypeToString(operator_) + " " + 
           right_->ToString() + ")";
}

void BinaryExpression::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String UnaryExpression::ToString() const {
    return "Unary(" + TokenUtils::TokenTypeToString(operator_) + " " + 
           operand_->ToString() + ")";
}

void UnaryExpression::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String AssignmentExpression::ToString() const {
    return "Assignment(" + target_->ToString() + " " + 
           TokenUtils::TokenTypeToString(operator_) + " " + 
           value_->ToString() + ")";
}

void AssignmentExpression::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String CallExpression::ToString() const {
    return "Call(" + callee_->ToString() + " with " + 
           std::to_string(arguments_.size()) + " args)";
}

void CallExpression::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String VariableDeclaration::ToString() const {
    String result = is_const_ ? "Const(" : "Var(";
    result += name_;
    if (HasTypeAnnotation()) {
        result += ": " + type_annotation_;
    }
    if (initializer_) {
        result += " = " + initializer_->ToString();
    }
    result += ")";
    return result;
}

void VariableDeclaration::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String ExpressionStatement::ToString() const {
    return "ExpressionStmt(" + expression_->ToString() + ")";
}

void ExpressionStatement::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

String BlockStatement::ToString() const {
    return "Block with " + std::to_string(statements_.size()) + " statements";
}

void BlockStatement::Accept(ASTVisitor& visitor) {
    visitor.Visit(*this);
}

} // namespace cj