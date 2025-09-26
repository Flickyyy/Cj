/**
 * @file ast_visitor.h
 * @brief Visitor pattern implementation for AST traversal
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_AST_VISITOR_H
#define CJ_AST_VISITOR_H

#include "../common.h"

namespace cj {

// Forward declarations
class Program;
class LiteralExpression;
class IdentifierExpression;
class BinaryExpression;
class UnaryExpression;
class AssignmentExpression;
class CallExpression;
class VariableDeclaration;
class ExpressionStatement;
class BlockStatement;

/**
 * @brief Abstract base visitor for AST nodes
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Program
    virtual void Visit(Program& node) = 0;
    
    // Expressions
    virtual void Visit(LiteralExpression& node) = 0;
    virtual void Visit(IdentifierExpression& node) = 0;
    virtual void Visit(BinaryExpression& node) = 0;
    virtual void Visit(UnaryExpression& node) = 0;
    virtual void Visit(AssignmentExpression& node) = 0;
    virtual void Visit(CallExpression& node) = 0;
    
    // Statements
    virtual void Visit(VariableDeclaration& node) = 0;
    virtual void Visit(ExpressionStatement& node) = 0;
    virtual void Visit(BlockStatement& node) = 0;
};

/**
 * @brief Generic visitor that can return values
 */
template<typename T>
class ASTVisitorWithReturn {
public:
    virtual ~ASTVisitorWithReturn() = default;
    
    // Program
    virtual T Visit(Program& node) = 0;
    
    // Expressions
    virtual T Visit(LiteralExpression& node) = 0;
    virtual T Visit(IdentifierExpression& node) = 0;
    virtual T Visit(BinaryExpression& node) = 0;
    virtual T Visit(UnaryExpression& node) = 0;
    virtual T Visit(AssignmentExpression& node) = 0;
    virtual T Visit(CallExpression& node) = 0;
    
    // Statements
    virtual T Visit(VariableDeclaration& node) = 0;
    virtual T Visit(ExpressionStatement& node) = 0;
    virtual T Visit(BlockStatement& node) = 0;
};

/**
 * @brief Pretty printer visitor
 */
class ASTPrettyPrinter : public ASTVisitor {
private:
    String output_;
    int indent_level_;
    String indent_string_;

public:
    ASTPrettyPrinter(const String& indent = "  ") 
        : indent_level_(0), indent_string_(indent) {}
    
    String GetOutput() const { return output_; }
    void Clear() { output_.clear(); indent_level_ = 0; }
    
    // Visitor methods
    void Visit(Program& node) override;
    void Visit(LiteralExpression& node) override;
    void Visit(IdentifierExpression& node) override;
    void Visit(BinaryExpression& node) override;
    void Visit(UnaryExpression& node) override;
    void Visit(AssignmentExpression& node) override;
    void Visit(CallExpression& node) override;
    void Visit(VariableDeclaration& node) override;
    void Visit(ExpressionStatement& node) override;
    void Visit(BlockStatement& node) override;

private:
    void Indent();
    void AddLine(const String& line);
    void IncreaseIndent() { indent_level_++; }
    void DecreaseIndent() { if (indent_level_ > 0) indent_level_--; }
};

/**
 * @brief Visitor for walking the AST and collecting information
 */
class ASTWalker : public ASTVisitor {
public:
    ASTWalker() = default;
    
    // Visitor methods - default implementations traverse children
    void Visit(Program& node) override;
    void Visit(LiteralExpression& node) override;
    void Visit(IdentifierExpression& node) override;
    void Visit(BinaryExpression& node) override;
    void Visit(UnaryExpression& node) override;
    void Visit(AssignmentExpression& node) override;
    void Visit(CallExpression& node) override;
    void Visit(VariableDeclaration& node) override;
    void Visit(ExpressionStatement& node) override;
    void Visit(BlockStatement& node) override;

protected:
    // Override these methods in derived classes for specific behavior
    virtual void OnEnterNode(ASTNode& node) {}
    virtual void OnExitNode(ASTNode& node) {}
};

/**
 * @brief Visitor for collecting symbols and variable information
 */
class SymbolCollector : public ASTWalker {
private:
    struct Symbol {
        String name;
        ASTNode* declaration;
        bool is_const;
        
        Symbol() : name(""), declaration(nullptr), is_const(false) {}
        Symbol(const String& n, ASTNode* decl, bool c)
            : name(n), declaration(decl), is_const(c) {}
    };
    
    Vector<HashMap<String, Symbol>> scopes_;
    Vector<String> variables_;
    Vector<String> functions_;

public:
    SymbolCollector();
    
    void Visit(Program& node) override;
    void Visit(VariableDeclaration& node) override;
    void Visit(BlockStatement& node) override;
    void Visit(IdentifierExpression& node) override;
    
    const Vector<String>& GetVariables() const { return variables_; }
    const Vector<String>& GetFunctions() const { return functions_; }
    
    void Clear();

private:
    void PushScope();
    void PopScope();
    void AddSymbol(const String& name, ASTNode* declaration, bool is_const);
    Symbol* FindSymbol(const String& name);
};

} // namespace cj

#endif // CJ_AST_VISITOR_H