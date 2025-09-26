/**
 * @file parser.cpp
 * @brief Stub implementation of Parser for CJ language
 */

#include "cj/parser/parser.h"

namespace cj {

UniquePtr<Program> Parser::ParseProgram() {
    auto program = cj_make_unique<Program>(GetCurrentLocation());
    
    while (!IsAtEnd() && current_token_.GetType() != TokenType::EOF_TOKEN) {
        try {
            auto stmt = ParseStatement();
            if (stmt) {
                program->AddStatement(std::move(stmt));
            }
        } catch (const ParserException& e) {
            Error(e.what());
            if (options_.recover_from_errors) {
                Synchronize();
            } else {
                break;
            }
        }
    }
    
    return program;
}

UniquePtr<Expression> Parser::ParseExpression() {
    return ParseAssignment();
}

UniquePtr<Statement> Parser::ParseStatement() {
    if (Match(TokenType::VAR) || Match(TokenType::CONST)) {
        return ParseVariableDeclaration();
    }
    return ParseExpressionStatement();
}

UniquePtr<Statement> Parser::ParseVariableDeclaration() {
    bool is_const = current_token_.GetType() == TokenType::CONST;
    Advance();
    
    Token name_token = Consume(TokenType::IDENTIFIER, "Expected variable name");
    
    UniquePtr<Expression> initializer = nullptr;
    if (Match(TokenType::ASSIGN)) {
        Advance();
        initializer = ParseExpression();
    }
    
    Consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return cj_make_unique<VariableDeclaration>(name_token.GetLexeme(), std::move(initializer), 
                                           is_const, name_token.GetLocation());
}

UniquePtr<Statement> Parser::ParseExpressionStatement() {
    auto expr = ParseExpression();
    Consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return cj_make_unique<ExpressionStatement>(std::move(expr), GetCurrentLocation());
}

UniquePtr<Expression> Parser::ParseAssignment() {
    return ParseLogicalOr();
}

UniquePtr<Expression> Parser::ParseLogicalOr() {
    return ParseLogicalAnd();
}

UniquePtr<Expression> Parser::ParseLogicalAnd() {
    return ParseEquality();
}

UniquePtr<Expression> Parser::ParseEquality() {
    auto expr = ParseComparison();
    
    while (Match(TokenType::EQUAL) || Match(TokenType::NOT_EQUAL)) {
        TokenType op = current_token_.GetType();
        Advance();
        auto right = ParseComparison();
        expr = cj_make_unique<BinaryExpression>(std::move(expr), op, std::move(right), GetCurrentLocation());
    }
    
    return expr;
}

UniquePtr<Expression> Parser::ParseComparison() {
    auto expr = ParseTerm();
    
    while (Match(TokenType::LESS) || Match(TokenType::LESS_EQUAL) ||
           Match(TokenType::GREATER) || Match(TokenType::GREATER_EQUAL)) {
        TokenType op = current_token_.GetType();
        Advance();
        auto right = ParseTerm();
        expr = cj_make_unique<BinaryExpression>(std::move(expr), op, std::move(right), GetCurrentLocation());
    }
    
    return expr;
}

UniquePtr<Expression> Parser::ParseTerm() {
    auto expr = ParseFactor();
    
    while (Match(TokenType::PLUS) || Match(TokenType::MINUS)) {
        TokenType op = current_token_.GetType();
        Advance();
        auto right = ParseFactor();
        expr = cj_make_unique<BinaryExpression>(std::move(expr), op, std::move(right), GetCurrentLocation());
    }
    
    return expr;
}

UniquePtr<Expression> Parser::ParseFactor() {
    auto expr = ParseUnary();
    
    while (Match(TokenType::MULTIPLY) || Match(TokenType::DIVIDE) || Match(TokenType::MODULO)) {
        TokenType op = current_token_.GetType();
        Advance();
        auto right = ParseUnary();
        expr = cj_make_unique<BinaryExpression>(std::move(expr), op, std::move(right), GetCurrentLocation());
    }
    
    return expr;
}

UniquePtr<Expression> Parser::ParseUnary() {
    if (Match(TokenType::LOGICAL_NOT) || Match(TokenType::MINUS)) {
        TokenType op = current_token_.GetType();
        Advance();
        auto operand = ParseUnary();
        return cj_make_unique<UnaryExpression>(op, std::move(operand), GetCurrentLocation());
    }
    
    return ParseCall();
}

UniquePtr<Expression> Parser::ParseCall() {
    return ParsePrimary();
}

UniquePtr<Expression> Parser::ParsePrimary() {
    if (Match(TokenType::INTEGER_LITERAL)) {
        Token token = current_token_;
        Advance();
        Int64 value = std::stoll(token.GetLexeme());
        return cj_make_unique<LiteralExpression>(Value(value), token.GetLocation());
    }
    
    if (Match(TokenType::FLOAT_LITERAL)) {
        Token token = current_token_;
        Advance();
        Float64 value = std::stod(token.GetLexeme());
        return cj_make_unique<LiteralExpression>(Value(value), token.GetLocation());
    }
    
    if (Match(TokenType::STRING_LITERAL)) {
        Token token = current_token_;
        Advance();
        return cj_make_unique<LiteralExpression>(Value(token.GetLexeme()), token.GetLocation());
    }
    
    if (Match(TokenType::TRUE) || Match(TokenType::FALSE)) {
        Token token = current_token_;
        Advance();
        bool value = token.GetType() == TokenType::TRUE;
        return cj_make_unique<LiteralExpression>(Value(value), token.GetLocation());
    }
    
    if (Match(TokenType::NIL)) {
        Token token = current_token_;
        Advance();
        return cj_make_unique<LiteralExpression>(Value(), token.GetLocation());
    }
    
    if (Match(TokenType::IDENTIFIER)) {
        Token token = current_token_;
        Advance();
        return cj_make_unique<IdentifierExpression>(token.GetLexeme(), token.GetLocation());
    }
    
    if (Match(TokenType::LEFT_PAREN)) {
        Advance();
        auto expr = ParseExpression();
        Consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw ParserException("Expected expression");
}

void Parser::Advance() {
    if (!IsAtEnd()) {
        current_token_ = lexer_->NextToken();
        has_current_token_ = true;
    }
}

bool Parser::Match(TokenType type) {
    return Check(type);
}

bool Parser::Check(TokenType type) const {
    return current_token_.GetType() == type;
}

Token Parser::Consume(TokenType type, const String& error_message) {
    if (Check(type)) {
        Token token = current_token_;
        Advance();
        return token;
    }
    
    throw ParserException(error_message);
}

void Parser::Synchronize() {
    Advance();
    
    while (!IsAtEnd()) {
        if (current_token_.GetType() == TokenType::SEMICOLON) {
            Advance();
            return;
        }
        
        switch (current_token_.GetType()) {
            case TokenType::CLASS:
            case TokenType::FUNCTION:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        Advance();
    }
}

bool Parser::IsAtEnd() const {
    return current_token_.GetType() == TokenType::EOF_TOKEN;
}

SourceLocation Parser::GetCurrentLocation() const {
    return current_token_.GetLocation();
}

void Parser::Error(const String& message) {
    errors_.push_back(message);
    panic_mode_ = true;
}

namespace ParserFactory {

UniquePtr<Parser> FromFile(const String& filename, const ParserOptions& options) {
    auto lexer = LexerFactory::FromFile(filename);
    return cj_make_unique<Parser>(std::move(lexer), options);
}

UniquePtr<Parser> FromString(const String& source, const String& filename, const ParserOptions& options) {
    auto lexer = LexerFactory::FromString(source, filename);
    return cj_make_unique<Parser>(std::move(lexer), options);
}

UniquePtr<Parser> FromLexer(UniquePtr<Lexer> lexer, const ParserOptions& options) {
    return cj_make_unique<Parser>(std::move(lexer), options);
}

} // namespace ParserFactory

} // namespace cj