/**
 * @file lexer.cpp
 * @brief Implementation of Lexer for CJ language
 */

#include "cj/lexer/lexer.h"
#include <fstream>
#include <sstream>

namespace cj {

Lexer::Lexer(std::istream& input, const String& filename, const LexerOptions& options)
    : filename_(filename), position_(0), line_(1), column_(1), options_(options), at_end_(false) {
    
    std::ostringstream buffer;
    buffer << input.rdbuf();
    source_ = buffer.str();
    
    if (!source_.empty()) {
        current_char_ = source_[0];
    } else {
        at_end_ = true;
    }
}

Token Lexer::NextToken() {
    while (!IsAtEnd()) {
        if (options_.skip_whitespace && IsWhitespace(current_char_)) {
            SkipWhitespace();
            continue;
        }
        
        if (IsNewline(current_char_)) {
            Advance();
            if (!options_.skip_whitespace) {
                return MakeToken(TokenType::NEWLINE);
            }
            continue;
        }
        
        // Handle comments
        if (current_char_ == '/' && Peek(1) == '/') {
            if (options_.skip_comments) {
                SkipLineComment();
                continue;
            } else {
                return ScanLineComment();
            }
        }
        
        if (current_char_ == '/' && Peek(1) == '*') {
            if (options_.skip_comments) {
                SkipBlockComment();
                continue;
            } else {
                return ScanBlockComment();
            }
        }
        
        // Handle string literals
        if (current_char_ == '"' || current_char_ == '\'') {
            return ScanStringLiteral();
        }
        
        // Handle numbers
        if (IsDigit(current_char_)) {
            return ScanNumberLiteral();
        }
        
        // Handle identifiers and keywords
        if (IsAlpha(current_char_)) {
            return ScanIdentifierOrKeyword();
        }
        
        // Handle operators and punctuation
        Token op_token = ScanOperator();
        if (op_token.GetType() != TokenType::UNKNOWN) {
            return op_token;
        }
        
        // Unknown character
        char unknown = current_char_;
        Advance();
        return MakeErrorToken("Unexpected character: " + String(1, unknown));
    }
    
    return MakeToken(TokenType::EOF_TOKEN);
}

Token Lexer::PeekToken() {
    Size saved_pos = position_;
    Size saved_line = line_;
    Size saved_column = column_;
    char saved_char = current_char_;
    bool saved_at_end = at_end_;
    
    Token token = NextToken();
    
    position_ = saved_pos;
    line_ = saved_line;
    column_ = saved_column;
    current_char_ = saved_char;
    at_end_ = saved_at_end;
    
    return token;
}

Vector<Token> Lexer::TokenizeAll() {
    Vector<Token> tokens;
    Token token;
    
    do {
        token = NextToken();
        tokens.push_back(token);
    } while (token.GetType() != TokenType::EOF_TOKEN);
    
    return tokens;
}

SourceLocation Lexer::GetCurrentLocation() const {
    return SourceLocation(filename_, static_cast<UInt32>(line_), static_cast<UInt32>(column_), 
                         static_cast<UInt32>(position_));
}

void Lexer::Advance() {
    if (IsAtEnd()) return;
    
    if (current_char_ == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    
    position_++;
    
    if (position_ >= source_.length()) {
        at_end_ = true;
        current_char_ = '\0';
    } else {
        current_char_ = source_[position_];
    }
}

char Lexer::Peek(Size offset) const {
    Size peek_pos = position_ + offset;
    if (peek_pos >= source_.length()) {
        return '\0';
    }
    return source_[peek_pos];
}

void Lexer::SkipWhitespace() {
    while (!IsAtEnd() && IsWhitespace(current_char_)) {
        Advance();
    }
}

void Lexer::SkipLineComment() {
    while (!IsAtEnd() && !IsNewline(current_char_)) {
        Advance();
    }
}

void Lexer::SkipBlockComment() {
    Advance(); // skip '/'
    Advance(); // skip '*'
    
    while (!IsAtEnd()) {
        if (current_char_ == '*' && Peek(1) == '/') {
            Advance(); // skip '*'
            Advance(); // skip '/'
            break;
        }
        Advance();
    }
}

Token Lexer::ScanLineComment() {
    SourceLocation start_loc = GetCurrentLocation();
    String lexeme;
    
    while (!IsAtEnd() && !IsNewline(current_char_)) {
        lexeme += current_char_;
        Advance();
    }
    
    return Token(TokenType::LINE_COMMENT, lexeme, start_loc);
}

Token Lexer::ScanBlockComment() {
    SourceLocation start_loc = GetCurrentLocation();
    String lexeme;
    
    lexeme += current_char_; // '/'
    Advance();
    lexeme += current_char_; // '*'
    Advance();
    
    while (!IsAtEnd()) {
        if (current_char_ == '*' && Peek(1) == '/') {
            lexeme += current_char_; // '*'
            Advance();
            lexeme += current_char_; // '/'
            Advance();
            break;
        }
        lexeme += current_char_;
        Advance();
    }
    
    return Token(TokenType::BLOCK_COMMENT, lexeme, start_loc);
}

Token Lexer::MakeToken(TokenType type, const String& lexeme) {
    String token_lexeme = lexeme.empty() ? String(1, current_char_) : lexeme;
    return Token(type, token_lexeme, GetCurrentLocation());
}

Token Lexer::MakeErrorToken(const String& message) {
    return Token(TokenType::INVALID, message, GetCurrentLocation());
}

Token Lexer::ScanStringLiteral() {
    SourceLocation start_loc = GetCurrentLocation();
    char quote_char = current_char_;
    String lexeme;
    
    Advance(); // Skip opening quote
    
    while (!IsAtEnd() && current_char_ != quote_char) {
        if (current_char_ == '\\') {
            lexeme += current_char_;
            Advance();
            if (!IsAtEnd()) {
                lexeme += current_char_;
                Advance();
            }
        } else {
            lexeme += current_char_;
            Advance();
        }
    }
    
    if (IsAtEnd()) {
        return Token(TokenType::INVALID, "Unterminated string", start_loc);
    }
    
    Advance(); // Skip closing quote
    return Token(TokenType::STRING_LITERAL, lexeme, start_loc);
}

Token Lexer::ScanNumberLiteral() {
    return ScanDecimalNumber();
}

Token Lexer::ScanDecimalNumber() {
    SourceLocation start_loc = GetCurrentLocation();
    String lexeme;
    bool is_float = false;
    
    // Scan integer part
    while (!IsAtEnd() && IsDigit(current_char_)) {
        lexeme += current_char_;
        Advance();
    }
    
    // Check for decimal point
    if (!IsAtEnd() && current_char_ == '.' && IsDigit(Peek(1))) {
        is_float = true;
        lexeme += current_char_;
        Advance();
        
        while (!IsAtEnd() && IsDigit(current_char_)) {
            lexeme += current_char_;
            Advance();
        }
    }
    
    // Check for exponent
    if (!IsAtEnd() && (current_char_ == 'e' || current_char_ == 'E')) {
        is_float = true;
        lexeme += current_char_;
        Advance();
        
        if (!IsAtEnd() && (current_char_ == '+' || current_char_ == '-')) {
            lexeme += current_char_;
            Advance();
        }
        
        while (!IsAtEnd() && IsDigit(current_char_)) {
            lexeme += current_char_;
            Advance();
        }
    }
    
    TokenType type = is_float ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL;
    return Token(type, lexeme, start_loc);
}

Token Lexer::ScanIdentifierOrKeyword() {
    SourceLocation start_loc = GetCurrentLocation();
    String lexeme;
    
    while (!IsAtEnd() && IsAlphaNumeric(current_char_)) {
        lexeme += current_char_;
        Advance();
    }
    
    TokenType type = TokenUtils::GetKeywordType(lexeme);
    return Token(type, lexeme, start_loc);
}

Token Lexer::ScanOperator() {
    SourceLocation start_loc = GetCurrentLocation();
    
    // Check for two-character operators first
    TokenType two_char_op = GetTwoCharOperator(current_char_, Peek(1));
    if (two_char_op != TokenType::UNKNOWN) {
        String lexeme;
        lexeme += current_char_;
        Advance();
        lexeme += current_char_;
        Advance();
        return Token(two_char_op, lexeme, start_loc);
    }
    
    // Check for single-character operators
    TokenType single_char_op = GetSingleCharOperator(current_char_);
    if (single_char_op != TokenType::UNKNOWN) {
        char op = current_char_;
        Advance();
        return Token(single_char_op, String(1, op), start_loc);
    }
    
    return Token(TokenType::UNKNOWN, "", start_loc);
}

TokenType Lexer::GetTwoCharOperator(char first, char second) const {
    if (first == '=' && second == '=') return TokenType::EQUAL;
    if (first == '!' && second == '=') return TokenType::NOT_EQUAL;
    if (first == '<' && second == '=') return TokenType::LESS_EQUAL;
    if (first == '>' && second == '=') return TokenType::GREATER_EQUAL;
    if (first == '&' && second == '&') return TokenType::LOGICAL_AND;
    if (first == '|' && second == '|') return TokenType::LOGICAL_OR;
    if (first == '+' && second == '=') return TokenType::PLUS_ASSIGN;
    if (first == '-' && second == '=') return TokenType::MINUS_ASSIGN;
    if (first == '*' && second == '=') return TokenType::MULT_ASSIGN;
    if (first == '/' && second == '=') return TokenType::DIV_ASSIGN;
    if (first == '%' && second == '=') return TokenType::MOD_ASSIGN;
    if (first == '+' && second == '+') return TokenType::INCREMENT;
    if (first == '-' && second == '-') return TokenType::DECREMENT;
    if (first == '<' && second == '<') return TokenType::BIT_LSHIFT;
    if (first == '>' && second == '>') return TokenType::BIT_RSHIFT;
    if (first == '*' && second == '*') return TokenType::POWER;
    if (first == '-' && second == '>') return TokenType::ARROW;
    
    return TokenType::UNKNOWN;
}

TokenType Lexer::GetSingleCharOperator(char c) const {
    switch (c) {
        case '+': return TokenType::PLUS;
        case '-': return TokenType::MINUS;
        case '*': return TokenType::MULTIPLY;
        case '/': return TokenType::DIVIDE;
        case '%': return TokenType::MODULO;
        case '=': return TokenType::ASSIGN;
        case '<': return TokenType::LESS;
        case '>': return TokenType::GREATER;
        case '!': return TokenType::LOGICAL_NOT;
        case '&': return TokenType::BIT_AND;
        case '|': return TokenType::BIT_OR;
        case '^': return TokenType::BIT_XOR;
        case '~': return TokenType::BIT_NOT;
        case '(': return TokenType::LEFT_PAREN;
        case ')': return TokenType::RIGHT_PAREN;
        case '{': return TokenType::LEFT_BRACE;
        case '}': return TokenType::RIGHT_BRACE;
        case '[': return TokenType::LEFT_BRACKET;
        case ']': return TokenType::RIGHT_BRACKET;
        case ';': return TokenType::SEMICOLON;
        case ',': return TokenType::COMMA;
        case '.': return TokenType::DOT;
        case '?': return TokenType::QUESTION;
        case ':': return TokenType::COLON;
        default: return TokenType::UNKNOWN;
    }
}

bool Lexer::IsAlpha(char c) const {
    return TokenUtils::IsValidIdentifierStart(c);
}

bool Lexer::IsAlphaNumeric(char c) const {
    return TokenUtils::IsValidIdentifierChar(c);
}

bool Lexer::IsDigit(char c) const {
    return TokenUtils::IsDigit(c);
}

bool Lexer::IsWhitespace(char c) const {
    return TokenUtils::IsWhitespace(c);
}

bool Lexer::IsNewline(char c) const {
    return TokenUtils::IsNewline(c);
}

namespace LexerFactory {

UniquePtr<Lexer> FromFile(const String& filename, const LexerOptions& options) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw LexerException("Cannot open file: " + filename);
    }
    return std::make_unique<Lexer>(file, filename, options);
}

UniquePtr<Lexer> FromString(const String& source, const String& filename, const LexerOptions& options) {
    return std::make_unique<Lexer>(source, filename, options);
}

UniquePtr<Lexer> FromStream(std::istream& stream, const String& filename, const LexerOptions& options) {
    return std::make_unique<Lexer>(stream, filename, options);
}

} // namespace LexerFactory

} // namespace cj