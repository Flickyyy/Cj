/**
 * @file token.h
 * @brief Token definitions for the CJ lexer
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_TOKEN_H
#define CJ_TOKEN_H

#include "../common.h"

namespace cj {

/**
 * @brief Enumeration of all token types in CJ
 */
enum class TokenType {
    // End of file
    EOF_TOKEN,
    
    // Literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    NIL_LITERAL,
    
    // Keywords
    IF,
    ELSE,
    ELIF,
    WHILE,
    FOR,
    BREAK,
    CONTINUE,
    FUNCTION,
    RETURN,
    VAR,
    CONST,
    CLASS,
    EXTENDS,
    NEW,
    THIS,
    SUPER,
    TRUE,
    FALSE,
    NIL,
    AND,
    OR,
    NOT,
    IN,
    TRY,
    CATCH,
    FINALLY,
    THROW,
    IMPORT,
    EXPORT,
    FROM,
    AS,
    
    // Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    
    // Assignment operators
    ASSIGN,         // =
    PLUS_ASSIGN,    // +=
    MINUS_ASSIGN,   // -=
    MULT_ASSIGN,    // *=
    DIV_ASSIGN,     // /=
    MOD_ASSIGN,     // %=
    
    // Comparison operators
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    
    // Logical operators
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||
    LOGICAL_NOT,    // !
    
    // Bitwise operators
    BIT_AND,        // &
    BIT_OR,         // |
    BIT_XOR,        // ^
    BIT_NOT,        // ~
    BIT_LSHIFT,     // <<
    BIT_RSHIFT,     // >>
    
    // Increment/Decrement
    INCREMENT,      // ++
    DECREMENT,      // --
    
    // Punctuation
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    ARROW,          // ->
    QUESTION,       // ?
    COLON,          // :
    
    // Brackets
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    
    // Comments
    LINE_COMMENT,
    BLOCK_COMMENT,
    
    // Whitespace (usually ignored)
    WHITESPACE,
    NEWLINE,
    
    // Special
    UNKNOWN,
    INVALID
};

/**
 * @brief Source location information
 */
struct SourceLocation {
    String filename;
    UInt32 line;
    UInt32 column;
    UInt32 offset;
    
    SourceLocation() : line(1), column(1), offset(0) {}
    SourceLocation(const String& file, UInt32 l, UInt32 c, UInt32 o)
        : filename(file), line(l), column(c), offset(o) {}
    
    String ToString() const;
};

/**
 * @brief Token class representing a lexical token
 */
class Token {
private:
    TokenType type_;
    String lexeme_;
    SourceLocation location_;

public:
    /**
     * @brief Construct a token
     */
    Token(TokenType type, const String& lexeme, const SourceLocation& location)
        : type_(type), lexeme_(lexeme), location_(location) {}
    
    Token() : type_(TokenType::UNKNOWN), lexeme_(""), location_() {}
    
    /**
     * @brief Getters
     */
    TokenType GetType() const { return type_; }
    const String& GetLexeme() const { return lexeme_; }
    const SourceLocation& GetLocation() const { return location_; }
    
    /**
     * @brief Type checking methods
     */
    bool IsType(TokenType type) const { return type_ == type; }
    bool IsLiteral() const;
    bool IsKeyword() const;
    bool IsOperator() const;
    bool IsBinaryOperator() const;
    bool IsUnaryOperator() const;
    bool IsAssignmentOperator() const;
    bool IsComparisonOperator() const;
    bool IsLogicalOperator() const;
    
    /**
     * @brief Utility methods
     */
    String ToString() const;
    String TypeToString() const;
    
    /**
     * @brief Get operator precedence (higher number = higher precedence)
     */
    int GetPrecedence() const;
    
    /**
     * @brief Check if operator is right associative
     */
    bool IsRightAssociative() const;
    
    /**
     * @brief Comparison operators
     */
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const { return !(*this == other); }
};

/**
 * @brief Utility functions for token types
 */
namespace TokenUtils {
    String TokenTypeToString(TokenType type);
    bool IsKeyword(const String& identifier);
    TokenType GetKeywordType(const String& identifier);
    bool IsValidIdentifierStart(char c);
    bool IsValidIdentifierChar(char c);
    bool IsDigit(char c);
    bool IsHexDigit(char c);
    bool IsOctalDigit(char c);
    bool IsBinaryDigit(char c);
    bool IsWhitespace(char c);
    bool IsNewline(char c);
}

} // namespace cj

#endif // CJ_TOKEN_H