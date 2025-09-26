/**
 * @file ast_node.h
 * @brief Abstract Syntax Tree node definitions for CJ
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_AST_NODE_H
#define CJ_AST_NODE_H

#include "../common.h"
#include "../lexer/token.h"
#include "../types/value.h"

namespace cj {

/**
 * @brief Enumeration of AST node types
 */
enum class ASTNodeType {
    // Program structure
    PROGRAM,
    STATEMENT_LIST,
    
    // Declarations
    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,
    CLASS_DECLARATION,
    
    // Statements
    EXPRESSION_STATEMENT,
    BLOCK_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    RETURN_STATEMENT,
    TRY_STATEMENT,
    THROW_STATEMENT,
    
    // Expressions
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    INDEX_EXPRESSION,
    CONDITIONAL_EXPRESSION,
    
    // Literals
    LITERAL_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    ARRAY_LITERAL,
    OBJECT_LITERAL,
    FUNCTION_LITERAL,
    
    // Special
    THIS_EXPRESSION,
    SUPER_EXPRESSION,
    NEW_EXPRESSION
};

/**
 * @brief Base class for all AST nodes
 */
class ASTNode {
protected:
    ASTNodeType type_;
    SourceLocation location_;

public:
    explicit ASTNode(ASTNodeType type, const SourceLocation& location = SourceLocation())
        : type_(type), location_(location) {}
    
    virtual ~ASTNode() = default;
    
    /**
     * @brief Get node type
     */
    ASTNodeType GetType() const { return type_; }
    
    /**
     * @brief Get source location
     */
    const SourceLocation& GetLocation() const { return location_; }
    
    /**
     * @brief Virtual methods for visitor pattern
     */
    virtual String ToString() const = 0;
    virtual void Accept(class ASTVisitor& visitor) = 0;
    
    /**
     * @brief Type checking helper
     */
    template<typename T>
    T* As() {
        return dynamic_cast<T*>(this);
    }
    
    template<typename T>
    const T* As() const {
        return dynamic_cast<const T*>(this);
    }
    
    template<typename T>
    bool Is() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }
};

// Forward declarations for specific node types
class Expression;
class Statement;
class Declaration;

/**
 * @brief Base class for expressions
 */
class Expression : public ASTNode {
public:
    explicit Expression(ASTNodeType type, const SourceLocation& location = SourceLocation())
        : ASTNode(type, location) {}
    
    virtual ~Expression() = default;
};

/**
 * @brief Base class for statements
 */
class Statement : public ASTNode {
public:
    explicit Statement(ASTNodeType type, const SourceLocation& location = SourceLocation())
        : ASTNode(type, location) {}
    
    virtual ~Statement() = default;
};

/**
 * @brief Base class for declarations
 */
class Declaration : public Statement {
public:
    explicit Declaration(ASTNodeType type, const SourceLocation& location = SourceLocation())
        : Statement(type, location) {}
    
    virtual ~Declaration() = default;
};

/**
 * @brief Program node (root of AST)
 */
class Program : public ASTNode {
private:
    Vector<UniquePtr<Statement>> statements_;

public:
    explicit Program(const SourceLocation& location = SourceLocation())
        : ASTNode(ASTNodeType::PROGRAM, location) {}
    
    void AddStatement(UniquePtr<Statement> stmt) {
        statements_.push_back(std::move(stmt));
    }
    
    const Vector<UniquePtr<Statement>>& GetStatements() const { return statements_; }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Literal expression
 */
class LiteralExpression : public Expression {
private:
    Value value_;

public:
    LiteralExpression(const Value& value, const SourceLocation& location = SourceLocation())
        : Expression(ASTNodeType::LITERAL_EXPRESSION, location), value_(value) {}
    
    const Value& GetValue() const { return value_; }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Identifier expression
 */
class IdentifierExpression : public Expression {
private:
    String name_;

public:
    IdentifierExpression(const String& name, const SourceLocation& location = SourceLocation())
        : Expression(ASTNodeType::IDENTIFIER_EXPRESSION, location), name_(name) {}
    
    const String& GetName() const { return name_; }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Binary expression
 */
class BinaryExpression : public Expression {
private:
    UniquePtr<Expression> left_;
    TokenType operator_;
    UniquePtr<Expression> right_;

public:
    BinaryExpression(UniquePtr<Expression> left, TokenType op, UniquePtr<Expression> right,
                    const SourceLocation& location = SourceLocation())
        : Expression(ASTNodeType::BINARY_EXPRESSION, location),
          left_(std::move(left)), operator_(op), right_(std::move(right)) {}
    
    const Expression* GetLeft() const { return left_.get(); }
    Expression* GetLeft() { return left_.get(); }
    const Expression* GetRight() const { return right_.get(); }
    Expression* GetRight() { return right_.get(); }
    TokenType GetOperator() const { return operator_; }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Unary expression
 */
class UnaryExpression : public Expression {
private:
    TokenType operator_;
    UniquePtr<Expression> operand_;

public:
    UnaryExpression(TokenType op, UniquePtr<Expression> operand,
                   const SourceLocation& location = SourceLocation())
        : Expression(ASTNodeType::UNARY_EXPRESSION, location),
          operator_(op), operand_(std::move(operand)) {}
    
    TokenType GetOperator() const { return operator_; }
    const Expression* GetOperand() const { return operand_.get(); }
    Expression* GetOperand() { return operand_.get(); }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Assignment expression
 */
class AssignmentExpression : public Expression {
private:
    UniquePtr<Expression> target_;
    TokenType operator_;
    UniquePtr<Expression> value_;

public:
    AssignmentExpression(UniquePtr<Expression> target, TokenType op, UniquePtr<Expression> value,
                        const SourceLocation& location = SourceLocation())
        : Expression(ASTNodeType::ASSIGNMENT_EXPRESSION, location),
          target_(std::move(target)), operator_(op), value_(std::move(value)) {}
    
    const Expression* GetTarget() const { return target_.get(); }
    Expression* GetTarget() { return target_.get(); }
    TokenType GetOperator() const { return operator_; }
    const Expression* GetValue() const { return value_.get(); }
    Expression* GetValue() { return value_.get(); }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Function call expression
 */
class CallExpression : public Expression {
private:
    UniquePtr<Expression> callee_;
    Vector<UniquePtr<Expression>> arguments_;

public:
    CallExpression(UniquePtr<Expression> callee, Vector<UniquePtr<Expression>> args,
                  const SourceLocation& location = SourceLocation())
        : Expression(ASTNodeType::CALL_EXPRESSION, location),
          callee_(std::move(callee)), arguments_(std::move(args)) {}
    
    const Expression* GetCallee() const { return callee_.get(); }
    Expression* GetCallee() { return callee_.get(); }
    const Vector<UniquePtr<Expression>>& GetArguments() const { return arguments_; }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Variable declaration
 */
class VariableDeclaration : public Declaration {
private:
    String name_;
    UniquePtr<Expression> initializer_;
    bool is_const_;

public:
    VariableDeclaration(const String& name, UniquePtr<Expression> init, bool is_const,
                       const SourceLocation& location = SourceLocation())
        : Declaration(ASTNodeType::VARIABLE_DECLARATION, location),
          name_(name), initializer_(std::move(init)), is_const_(is_const) {}
    
    const String& GetName() const { return name_; }
    const Expression* GetInitializer() const { return initializer_.get(); }
    Expression* GetInitializer() { return initializer_.get(); }
    bool IsConst() const { return is_const_; }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Expression statement
 */
class ExpressionStatement : public Statement {
private:
    UniquePtr<Expression> expression_;

public:
    ExpressionStatement(UniquePtr<Expression> expr, const SourceLocation& location = SourceLocation())
        : Statement(ASTNodeType::EXPRESSION_STATEMENT, location), expression_(std::move(expr)) {}
    
    const Expression* GetExpression() const { return expression_.get(); }
    Expression* GetExpression() { return expression_.get(); }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

/**
 * @brief Block statement
 */
class BlockStatement : public Statement {
private:
    Vector<UniquePtr<Statement>> statements_;

public:
    explicit BlockStatement(const SourceLocation& location = SourceLocation())
        : Statement(ASTNodeType::BLOCK_STATEMENT, location) {}
    
    void AddStatement(UniquePtr<Statement> stmt) {
        statements_.push_back(std::move(stmt));
    }
    
    const Vector<UniquePtr<Statement>>& GetStatements() const { return statements_; }
    
    String ToString() const override;
    void Accept(ASTVisitor& visitor) override;
};

} // namespace cj

#endif // CJ_AST_NODE_H