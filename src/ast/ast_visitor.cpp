/**
 * @file ast_visitor.cpp
 * @brief Implementation of AST visitors for CJ language
 */

#include "cj/ast/ast_visitor.h"
#include "cj/ast/ast_node.h"

namespace cj {

// ASTPrettyPrinter implementation
void ASTPrettyPrinter::Visit(Program& node) {
    AddLine("Program:");
    IncreaseIndent();
    for (const auto& stmt : node.GetStatements()) {
        stmt->Accept(*this);
    }
    DecreaseIndent();
}

void ASTPrettyPrinter::Visit(LiteralExpression& node) {
    AddLine("Literal: " + node.GetValue().ToString());
}

void ASTPrettyPrinter::Visit(IdentifierExpression& node) {
    AddLine("Identifier: " + node.GetName());
}

void ASTPrettyPrinter::Visit(BinaryExpression& node) {
    AddLine("BinaryExpression:");
    IncreaseIndent();
    AddLine("Left:");
    IncreaseIndent();
    node.GetLeft()->Accept(*this);
    DecreaseIndent();
    AddLine("Operator: " + TokenUtils::TokenTypeToString(node.GetOperator()));
    AddLine("Right:");
    IncreaseIndent();
    node.GetRight()->Accept(*this);
    DecreaseIndent();
    DecreaseIndent();
}

void ASTPrettyPrinter::Visit(UnaryExpression& node) {
    AddLine("UnaryExpression:");
    IncreaseIndent();
    AddLine("Operator: " + TokenUtils::TokenTypeToString(node.GetOperator()));
    AddLine("Operand:");
    IncreaseIndent();
    node.GetOperand()->Accept(*this);
    DecreaseIndent();
    DecreaseIndent();
}

void ASTPrettyPrinter::Visit(AssignmentExpression& node) {
    AddLine("AssignmentExpression:");
    IncreaseIndent();
    AddLine("Target:");
    IncreaseIndent();
    node.GetTarget()->Accept(*this);
    DecreaseIndent();
    AddLine("Operator: " + TokenUtils::TokenTypeToString(node.GetOperator()));
    AddLine("Value:");
    IncreaseIndent();
    node.GetValue()->Accept(*this);
    DecreaseIndent();
    DecreaseIndent();
}

void ASTPrettyPrinter::Visit(CallExpression& node) {
    AddLine("CallExpression:");
    IncreaseIndent();
    AddLine("Callee:");
    IncreaseIndent();
    node.GetCallee()->Accept(*this);
    DecreaseIndent();
    AddLine("Arguments:");
    IncreaseIndent();
    for (const auto& arg : node.GetArguments()) {
        arg->Accept(*this);
    }
    DecreaseIndent();
    DecreaseIndent();
}

void ASTPrettyPrinter::Visit(VariableDeclaration& node) {
    AddLine(String(node.IsConst() ? "ConstDeclaration: " : "VarDeclaration: ") + node.GetName());
    if (node.GetInitializer()) {
        IncreaseIndent();
        AddLine("Initializer:");
        IncreaseIndent();
        node.GetInitializer()->Accept(*this);
        DecreaseIndent();
        DecreaseIndent();
    }
}

void ASTPrettyPrinter::Visit(ExpressionStatement& node) {
    AddLine("ExpressionStatement:");
    IncreaseIndent();
    node.GetExpression()->Accept(*this);
    DecreaseIndent();
}

void ASTPrettyPrinter::Visit(BlockStatement& node) {
    AddLine("BlockStatement:");
    IncreaseIndent();
    for (const auto& stmt : node.GetStatements()) {
        stmt->Accept(*this);
    }
    DecreaseIndent();
}

void ASTPrettyPrinter::Indent() {
    for (int i = 0; i < indent_level_; ++i) {
        output_ += indent_string_;
    }
}

void ASTPrettyPrinter::AddLine(const String& line) {
    Indent();
    output_ += line + "\n";
}

// ASTWalker implementation
void ASTWalker::Visit(Program& node) {
    OnEnterNode(node);
    for (const auto& stmt : node.GetStatements()) {
        stmt->Accept(*this);
    }
    OnExitNode(node);
}

void ASTWalker::Visit(LiteralExpression& node) {
    OnEnterNode(node);
    OnExitNode(node);
}

void ASTWalker::Visit(IdentifierExpression& node) {
    OnEnterNode(node);
    OnExitNode(node);
}

void ASTWalker::Visit(BinaryExpression& node) {
    OnEnterNode(node);
    node.GetLeft()->Accept(*this);
    node.GetRight()->Accept(*this);
    OnExitNode(node);
}

void ASTWalker::Visit(UnaryExpression& node) {
    OnEnterNode(node);
    node.GetOperand()->Accept(*this);
    OnExitNode(node);
}

void ASTWalker::Visit(AssignmentExpression& node) {
    OnEnterNode(node);
    node.GetTarget()->Accept(*this);
    node.GetValue()->Accept(*this);
    OnExitNode(node);
}

void ASTWalker::Visit(CallExpression& node) {
    OnEnterNode(node);
    node.GetCallee()->Accept(*this);
    for (const auto& arg : node.GetArguments()) {
        arg->Accept(*this);
    }
    OnExitNode(node);
}

void ASTWalker::Visit(VariableDeclaration& node) {
    OnEnterNode(node);
    if (node.GetInitializer()) {
        node.GetInitializer()->Accept(*this);
    }
    OnExitNode(node);
}

void ASTWalker::Visit(ExpressionStatement& node) {
    OnEnterNode(node);
    node.GetExpression()->Accept(*this);
    OnExitNode(node);
}

void ASTWalker::Visit(BlockStatement& node) {
    OnEnterNode(node);
    for (const auto& stmt : node.GetStatements()) {
        stmt->Accept(*this);
    }
    OnExitNode(node);
}

// SymbolCollector implementation
SymbolCollector::SymbolCollector() {
    PushScope(); // Global scope
}

void SymbolCollector::Visit(Program& node) {
    ASTWalker::Visit(node);
}

void SymbolCollector::Visit(VariableDeclaration& node) {
    AddSymbol(node.GetName(), &node, node.IsConst());
    variables_.push_back(node.GetName());
    ASTWalker::Visit(node);
}

void SymbolCollector::Visit(BlockStatement& node) {
    PushScope();
    ASTWalker::Visit(node);
    PopScope();
}

void SymbolCollector::Visit(IdentifierExpression& node) {
    // Check if identifier is declared
    FindSymbol(node.GetName());
    ASTWalker::Visit(node);
}

void SymbolCollector::Clear() {
    scopes_.clear();
    variables_.clear();
    functions_.clear();
    PushScope(); // Global scope
}

void SymbolCollector::PushScope() {
    scopes_.emplace_back();
}

void SymbolCollector::PopScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

void SymbolCollector::AddSymbol(const String& name, ASTNode* declaration, bool is_const) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = Symbol(name, declaration, is_const);
    }
}

SymbolCollector::Symbol* SymbolCollector::FindSymbol(const String& name) {
    // Search from current scope to global scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto symbol_it = it->find(name);
        if (symbol_it != it->end()) {
            return &symbol_it->second;
        }
    }
    return nullptr;
}

} // namespace cj