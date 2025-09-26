/**
 * @file lexer_demo.cpp
 * @brief Lexer demonstration program
 */

#include "cj/cj.h"
#include <iostream>
#include <iomanip>

using namespace cj;

int main() {
    std::cout << "CJ Lexer Demo" << std::endl;
    std::cout << "=============" << std::endl;
    
    const String code = R"(
        // This is a comment
        var greeting = "Hello, CJ!";
        const pi = 3.14159;
        var count = 42;
        
        function add(a, b) {
            return a + b;
        }
        
        var result = add(10, 20);
        
        if (result > 25) {
            print("Result is greater than 25");
        }
        
        /* Multi-line
           comment */
        var array = [1, 2, 3, 4, 5];
        var object = {
            name: "CJ",
            version: "0.1.0"
        };
    )";
    
    try {
        auto lexer = LexerFactory::FromString(code, "demo.cj");
        
        std::cout << "Source code:" << std::endl;
        std::cout << code << std::endl;
        std::cout << "\nTokens:" << std::endl;
        std::cout << "-------" << std::endl;
        
        Token token;
        int token_count = 0;
        
        do {
            token = lexer->NextToken();
            
            if (token.GetType() != TokenType::EOF_TOKEN) {
                std::cout << std::setw(3) << ++token_count << ": ";
                std::cout << std::setw(20) << token.TypeToString() << " ";
                std::cout << "\"" << token.GetLexeme() << "\" ";
                std::cout << "at " << token.GetLocation().ToString() << std::endl;
            }
        } while (token.GetType() != TokenType::EOF_TOKEN);
        
        std::cout << "\nTotal tokens: " << token_count << std::endl;
        
    } catch (const CJException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}