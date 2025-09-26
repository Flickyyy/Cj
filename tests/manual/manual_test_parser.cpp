/**
 * @file manual_test_parser.cpp
 * @brief Manual test for CJ parser
 */

#include "cj/cj.h"
#include <iostream>

using namespace cj;

int main() {
    std::cout << "CJ Parser Manual Test" << std::endl;
    std::cout << "=====================" << std::endl;
    
    const String test_code = R"(
        var x = 10 + 20 * 3;
        const message = "Hello, CJ!";
        
        if (x > 50) {
            print(message);
        } else {
            print("x is not greater than 50");
        }
    )";
    
    try {
        auto parser = ParserFactory::FromString(test_code, "test.cj");
        auto program = parser->ParseProgram();
        
        if (parser->HasErrors()) {
            std::cout << "Parser errors:" << std::endl;
            for (const auto& error : parser->GetErrors()) {
                std::cout << "  " << error << std::endl;
            }
            return 1;
        }
        
        std::cout << "Parsed program successfully!" << std::endl;
        std::cout << std::endl;
        
        // Pretty print the AST
        ASTPrettyPrinter printer;
        program->Accept(printer);
        
        std::cout << "AST:" << std::endl;
        std::cout << printer.GetOutput() << std::endl;
        
        std::cout << "Parser test completed successfully!" << std::endl;
        
    } catch (const CJException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}