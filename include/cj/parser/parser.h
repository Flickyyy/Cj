/**
 * @file parser.h
 * @brief Recursive descent parser for the CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_PARSER_H
#define CJ_PARSER_H

#include "../common.h"
#include "../lexer/lexer.h"
#include "../ast/ast_node.h"
#include <queue>

namespace cj {

/**
 * @brief Parser configuration options
 */
struct ParserOptions {
    bool allow_trailing_comma = true;
    bool strict_mode = false;
    bool recover_from_errors = true;
    Size max_expression_depth = 1000;
    
    ParserOptions() = default;
};

/**
 * @brief Recursive descent parser for CJ
 */
class Parser {
private:
    UniquePtr<Lexer> lexer_;
    ParserOptions options_;
    
    // Token management
    Token current_token_;
    std::queue<Token> token_buffer_;
    bool has_current_token_;
    
    // Error handling
    Vector<String> errors_;
    bool panic_mode_;

public:
    /**
     * @brief Construct parser with lexer
     */
    explicit Parser(UniquePtr<Lexer> lexer, const ParserOptions& options = ParserOptions())
        : lexer_(std::move(lexer)), options_(options), has_current_token_(false), panic_mode_(false) {
        Advance(); // Load first token
    }
    
    /**
     * @brief Parse the entire program
     */
    UniquePtr<Program> ParseProgram();
    
    /**
     * @brief Parse a single expression
     */
    UniquePtr<Expression> ParseExpression();
    
    /**
     * @brief Parse a single statement
     */
    UniquePtr<Statement> ParseStatement();
    
    /**
     * @brief Check if parsing was successful
     */
    bool HasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief Get parsing errors
     */
    const Vector<String>& GetErrors() const { return errors_; }
    
    /**
     * @brief Get parser options
     */
    const ParserOptions& GetOptions() const { return options_; }
    void SetOptions(const ParserOptions& options) { options_ = options; }

private:
    // Token management
    void Advance();
    Token Peek(Size offset = 0);
    bool Match(TokenType type);
    bool Check(TokenType type) const;
    Token Consume(TokenType type, const String& error_message);
    void Synchronize();
    
    // Parsing methods - Statements
    UniquePtr<Statement> ParseDeclaration();
    UniquePtr<Statement> ParseVariableDeclaration();
    UniquePtr<Statement> ParseFunctionDeclaration();
    UniquePtr<Statement> ParseClassDeclaration();
    UniquePtr<Statement> ParseExpressionStatement();
    UniquePtr<Statement> ParseBlockStatement();
    UniquePtr<Statement> ParseIfStatement();
    UniquePtr<Statement> ParseWhileStatement();
    UniquePtr<Statement> ParseForStatement();
    UniquePtr<Statement> ParseBreakStatement();
    UniquePtr<Statement> ParseContinueStatement();
    UniquePtr<Statement> ParseReturnStatement();
    UniquePtr<Statement> ParseTryStatement();
    UniquePtr<Statement> ParseThrowStatement();
    
    // Parsing methods - Expressions (precedence climbing)
    UniquePtr<Expression> ParseAssignment();
    UniquePtr<Expression> ParseLogicalOr();
    UniquePtr<Expression> ParseLogicalAnd();
    UniquePtr<Expression> ParseEquality();
    UniquePtr<Expression> ParseComparison();
    UniquePtr<Expression> ParseTerm();
    UniquePtr<Expression> ParseFactor();
    UniquePtr<Expression> ParseUnary();
    UniquePtr<Expression> ParseCall();
    UniquePtr<Expression> ParsePrimary();
    
    // Helper methods for expressions
    UniquePtr<Expression> FinishCall(UniquePtr<Expression> callee);
    UniquePtr<Expression> ParseArrayLiteral();
    UniquePtr<Expression> ParseObjectLiteral();
    UniquePtr<Expression> ParseFunctionLiteral();
    
    // Utility methods
    bool IsAtEnd() const;
    SourceLocation GetCurrentLocation() const;
    
    // Error handling
    void Error(const String& message);
    void ErrorAtCurrent(const String& message);
    void ErrorAtPrevious(const String& message);
    Token ErrorToken(const String& message);
    
    // Recovery methods
    void RecoverToStatement();
    void RecoverToExpression();
    bool CanRecover() const;
};

/**
 * @brief Parser factory functions
 */
namespace ParserFactory {
    /**
     * @brief Create parser from file
     */
    UniquePtr<Parser> FromFile(const String& filename, const ParserOptions& options = ParserOptions());
    
    /**
     * @brief Create parser from string
     */
    UniquePtr<Parser> FromString(const String& source, const String& filename = "",
                                const ParserOptions& options = ParserOptions());
    
    /**
     * @brief Create parser from lexer
     */
    UniquePtr<Parser> FromLexer(UniquePtr<Lexer> lexer, const ParserOptions& options = ParserOptions());
}

/**
 * @brief Utility class for parsing expressions with custom precedence
 */
class ExpressionParser {
private:
    Parser* parser_;
    
public:
    explicit ExpressionParser(Parser* parser) : parser_(parser) {}
    
    /**
     * @brief Parse expression with minimum precedence
     */
    UniquePtr<Expression> ParseWithPrecedence(int min_precedence);
    
    /**
     * @brief Get operator precedence
     */
    static int GetPrecedence(TokenType type);
    
    /**
     * @brief Check if operator is left associative
     */
    static bool IsLeftAssociative(TokenType type);
};

} // namespace cj

#endif // CJ_PARSER_H