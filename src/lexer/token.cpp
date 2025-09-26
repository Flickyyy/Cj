/**
 * @file token.cpp
 * @brief Implementation of Token system for CJ lexer
 */

#include "cj/lexer/token.h"
#include <sstream>
#include <unordered_map>

namespace cj {

String SourceLocation::ToString() const {
    std::ostringstream oss;
    if (!filename.empty()) {
        oss << filename << ":";
    }
    oss << line << ":" << column;
    return oss.str();
}

bool Token::IsLiteral() const {
    switch (type_) {
        case TokenType::INTEGER_LITERAL:
        case TokenType::FLOAT_LITERAL:
        case TokenType::STRING_LITERAL:
        case TokenType::BOOLEAN_LITERAL:
        case TokenType::NIL_LITERAL:
            return true;
        default:
            return false;
    }
}

bool Token::IsKeyword() const {
    switch (type_) {
        case TokenType::IF:
        case TokenType::ELSE:
        case TokenType::ELIF:
        case TokenType::WHILE:
        case TokenType::FOR:
        case TokenType::BREAK:
        case TokenType::CONTINUE:
        case TokenType::FUNCTION:
        case TokenType::RETURN:
        case TokenType::VAR:
        case TokenType::CONST:
        case TokenType::CLASS:
        case TokenType::EXTENDS:
        case TokenType::NEW:
        case TokenType::THIS:
        case TokenType::SUPER:
        case TokenType::TRUE:
        case TokenType::FALSE:
        case TokenType::NIL:
        case TokenType::AND:
        case TokenType::OR:
        case TokenType::NOT:
        case TokenType::IN:
        case TokenType::TRY:
        case TokenType::CATCH:
        case TokenType::FINALLY:
        case TokenType::THROW:
        case TokenType::IMPORT:
        case TokenType::EXPORT:
        case TokenType::FROM:
        case TokenType::AS:
            return true;
        default:
            return false;
    }
}

bool Token::IsOperator() const {
    switch (type_) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
        case TokenType::POWER:
        case TokenType::ASSIGN:
        case TokenType::PLUS_ASSIGN:
        case TokenType::MINUS_ASSIGN:
        case TokenType::MULT_ASSIGN:
        case TokenType::DIV_ASSIGN:
        case TokenType::MOD_ASSIGN:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::LOGICAL_AND:
        case TokenType::LOGICAL_OR:
        case TokenType::LOGICAL_NOT:
        case TokenType::BIT_AND:
        case TokenType::BIT_OR:
        case TokenType::BIT_XOR:
        case TokenType::BIT_NOT:
        case TokenType::BIT_LSHIFT:
        case TokenType::BIT_RSHIFT:
        case TokenType::INCREMENT:
        case TokenType::DECREMENT:
            return true;
        default:
            return false;
    }
}

int Token::GetPrecedence() const {
    switch (type_) {
        case TokenType::LOGICAL_OR:
            return 1;
        case TokenType::LOGICAL_AND:
            return 2;
        case TokenType::BIT_OR:
            return 3;
        case TokenType::BIT_XOR:
            return 4;
        case TokenType::BIT_AND:
            return 5;
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
            return 6;
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
            return 7;
        case TokenType::BIT_LSHIFT:
        case TokenType::BIT_RSHIFT:
            return 8;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 9;
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
            return 10;
        case TokenType::POWER:
            return 11;
        default:
            return 0;
    }
}

bool Token::IsRightAssociative() const {
    switch (type_) {
        case TokenType::POWER:
        case TokenType::ASSIGN:
        case TokenType::PLUS_ASSIGN:
        case TokenType::MINUS_ASSIGN:
        case TokenType::MULT_ASSIGN:
        case TokenType::DIV_ASSIGN:
        case TokenType::MOD_ASSIGN:
            return true;
        default:
            return false;
    }
}

String Token::ToString() const {
    std::ostringstream oss;
    oss << TypeToString() << "(\"" << lexeme_ << "\") at " << location_.ToString();
    return oss.str();
}

String Token::TypeToString() const {
    return TokenUtils::TokenTypeToString(type_);
}

bool Token::operator==(const Token& other) const {
    return type_ == other.type_ && lexeme_ == other.lexeme_;
}

namespace TokenUtils {

String TokenTypeToString(TokenType type) {
    static const std::unordered_map<TokenType, String> token_names = {
        {TokenType::EOF_TOKEN, "EOF"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"},
        {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::BOOLEAN_LITERAL, "BOOLEAN_LITERAL"},
        {TokenType::NIL_LITERAL, "NIL_LITERAL"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::ELIF, "ELIF"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::FOR, "FOR"},
        {TokenType::BREAK, "BREAK"},
        {TokenType::CONTINUE, "CONTINUE"},
        {TokenType::FUNCTION, "FUNCTION"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::VAR, "VAR"},
        {TokenType::CONST, "CONST"},
        {TokenType::CLASS, "CLASS"},
        {TokenType::EXTENDS, "EXTENDS"},
        {TokenType::NEW, "NEW"},
        {TokenType::THIS, "THIS"},
        {TokenType::SUPER, "SUPER"},
        {TokenType::TRUE, "TRUE"},
        {TokenType::FALSE, "FALSE"},
        {TokenType::NIL, "NIL"},
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::NOT, "NOT"},
        {TokenType::IN, "IN"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::MULTIPLY, "MULTIPLY"},
        {TokenType::DIVIDE, "DIVIDE"},
        {TokenType::MODULO, "MODULO"},
        {TokenType::POWER, "POWER"},
        {TokenType::ASSIGN, "ASSIGN"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::UNKNOWN, "UNKNOWN"},
        {TokenType::INVALID, "INVALID"}
    };
    
    auto it = token_names.find(type);
    return it != token_names.end() ? it->second : "UNKNOWN_TOKEN_TYPE";
}

bool IsKeyword(const String& identifier) {
    static const std::unordered_map<String, TokenType> keywords = {
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"elif", TokenType::ELIF},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"function", TokenType::FUNCTION},
        {"return", TokenType::RETURN},
        {"var", TokenType::VAR},
        {"const", TokenType::CONST},
        {"class", TokenType::CLASS},
        {"extends", TokenType::EXTENDS},
        {"new", TokenType::NEW},
        {"this", TokenType::THIS},
        {"super", TokenType::SUPER},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"nil", TokenType::NIL},
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"not", TokenType::NOT},
        {"in", TokenType::IN},
        {"try", TokenType::TRY},
        {"catch", TokenType::CATCH},
        {"finally", TokenType::FINALLY},
        {"throw", TokenType::THROW},
        {"import", TokenType::IMPORT},
        {"export", TokenType::EXPORT},
        {"from", TokenType::FROM},
        {"as", TokenType::AS}
    };
    
    return keywords.find(identifier) != keywords.end();
}

TokenType GetKeywordType(const String& identifier) {
    static const std::unordered_map<String, TokenType> keywords = {
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"elif", TokenType::ELIF},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"function", TokenType::FUNCTION},
        {"return", TokenType::RETURN},
        {"var", TokenType::VAR},
        {"const", TokenType::CONST},
        {"class", TokenType::CLASS},
        {"extends", TokenType::EXTENDS},
        {"new", TokenType::NEW},
        {"this", TokenType::THIS},
        {"super", TokenType::SUPER},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"nil", TokenType::NIL},
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"not", TokenType::NOT},
        {"in", TokenType::IN},
        {"try", TokenType::TRY},
        {"catch", TokenType::CATCH},
        {"finally", TokenType::FINALLY},
        {"throw", TokenType::THROW},
        {"import", TokenType::IMPORT},
        {"export", TokenType::EXPORT},
        {"from", TokenType::FROM},
        {"as", TokenType::AS}
    };
    
    auto it = keywords.find(identifier);
    return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
}

bool IsValidIdentifierStart(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool IsValidIdentifierChar(char c) {
    return IsValidIdentifierStart(c) || (c >= '0' && c <= '9');
}

bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

bool IsHexDigit(char c) {
    return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool IsOctalDigit(char c) {
    return c >= '0' && c <= '7';
}

bool IsBinaryDigit(char c) {
    return c == '0' || c == '1';
}

bool IsWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

bool IsNewline(char c) {
    return c == '\n';
}

} // namespace TokenUtils

} // namespace cj