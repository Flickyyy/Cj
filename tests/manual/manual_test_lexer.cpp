/**
 * @file manual_test_lexer.cpp
 * @brief Manual test for CJ lexer
 */

#include "cj/cj.h"
#include <iostream>

using namespace cj;

int main() {
    std::cout << "CJ Lexer Manual Test" << std::endl;
    std::cout << "====================" << std::endl;
    
    const String test_code = R"(
        var x = 42;
        const pi = 3.14159;
        
        function factorial(n) {
            if (n <= 1) {
                return 1;
            } else {
                return n * factorial(n - 1);
            }
        }
        
        var result = factorial(5);
        print("Factorial of 5 is: " + result);
    )";
    
    try {
        auto lexer = LexerFactory::FromString(test_code, "test.cj");
        auto tokens = lexer->TokenizeAll();
        
        std::cout << "Tokenized " << tokens.size() << " tokens:" << std::endl;
        std::cout << std::endl;
        
        for (const auto& token : tokens) {
            std::cout << token.ToString() << std::endl;
        }
        
        std::cout << std::endl << "Lexer test completed successfully!" << std::endl;
        
    } catch (const CJException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}