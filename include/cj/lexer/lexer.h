/**
 * @file lexer.h
 * @brief Lexical analyzer for the CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_LEXER_H
#define CJ_LEXER_H

#include "../common.h"
#include "token.h"
#include <iostream>
#include <sstream>

namespace cj {

/**
 * @brief Configuration options for the lexer
 */
struct LexerOptions {
    bool skip_whitespace = true;
    bool skip_comments = true;
    bool track_locations = true;
    bool case_sensitive = true;
    
    LexerOptions() = default;
};

/**
 * @brief Lexical analyzer for CJ source code
 */
class Lexer {
private:
    String source_;
    String filename_;
    Size position_;
    Size line_;
    Size column_;
    LexerOptions options_;
    
    // Current character
    char current_char_;
    bool at_end_;

public:
    /**
     * @brief Construct lexer with source code
     */
    explicit Lexer(const String& source, const String& filename = "", 
                   const LexerOptions& options = LexerOptions())
        : source_(source), filename_(filename), position_(0), line_(1), column_(1),
          options_(options), current_char_(0), at_end_(false) {
        if (!source_.empty()) {
            current_char_ = source_[0];
        } else {
            at_end_ = true;
        }
    }
    
    /**
     * @brief Construct lexer from input stream
     */
    explicit Lexer(std::istream& input, const String& filename = "",
                   const LexerOptions& options = LexerOptions());
    
    /**
     * @brief Get next token from input
     */
    Token NextToken();
    
    /**
     * @brief Peek at next token without consuming it
     */
    Token PeekToken();
    
    /**
     * @brief Tokenize entire input and return vector of tokens
     */
    Vector<Token> TokenizeAll();
    
    /**
     * @brief Check if we're at end of input
     */
    bool IsAtEnd() const { return at_end_; }
    
    /**
     * @brief Get current source location
     */
    SourceLocation GetCurrentLocation() const;
    
    /**
     * @brief Get lexer options
     */
    const LexerOptions& GetOptions() const { return options_; }
    void SetOptions(const LexerOptions& options) { options_ = options; }

private:
    /**
     * @brief Character navigation
     */
    void Advance();
    char Peek(Size offset = 0) const;
    void SkipWhitespace();
    void SkipLineComment();
    void SkipBlockComment();
    Token ScanLineComment();
    Token ScanBlockComment();
    
    /**
     * @brief Token creation helpers
     */
    Token MakeToken(TokenType type, const String& lexeme = "");
    Token MakeErrorToken(const String& message);
    
    /**
     * @brief Literal scanning methods
     */
    Token ScanStringLiteral();
    Token ScanNumberLiteral();
    Token ScanIdentifierOrKeyword();
    
    /**
     * @brief Character type checking
     */
    bool IsAlpha(char c) const;
    bool IsAlphaNumeric(char c) const;
    bool IsDigit(char c) const;
    bool IsHexDigit(char c) const;
    bool IsOctalDigit(char c) const;
    bool IsBinaryDigit(char c) const;
    bool IsWhitespace(char c) const;
    bool IsNewline(char c) const;
    
    /**
     * @brief Operator scanning
     */
    Token ScanOperator();
    TokenType GetTwoCharOperator(char first, char second) const;
    TokenType GetSingleCharOperator(char c) const;
    
    /**
     * @brief Keyword detection
     */
    TokenType CheckKeyword(const String& text) const;
    
    /**
     * @brief String escape sequence handling
     */
    String ProcessEscapeSequences(const String& str);
    char ProcessEscapeSequence(char c);
    
    /**
     * @brief Number parsing helpers
     */
    Token ScanDecimalNumber();
    Token ScanHexNumber();
    Token ScanOctalNumber();
    Token ScanBinaryNumber();
    Token ScanFloatNumber(const String& integer_part);
    
    /**
     * @brief Error handling
     */
    void ReportError(const String& message);
};

/**
 * @brief Lexer factory functions
 */
namespace LexerFactory {
    /**
     * @brief Create lexer from file
     */
    UniquePtr<Lexer> FromFile(const String& filename, const LexerOptions& options = LexerOptions());
    
    /**
     * @brief Create lexer from string
     */
    UniquePtr<Lexer> FromString(const String& source, const String& filename = "", 
                               const LexerOptions& options = LexerOptions());
    
    /**
     * @brief Create lexer from stream
     */
    UniquePtr<Lexer> FromStream(std::istream& stream, const String& filename = "",
                               const LexerOptions& options = LexerOptions());
}

} // namespace cj

#endif // CJ_LEXER_H