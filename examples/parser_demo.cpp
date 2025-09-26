/**
 * @file parser_demo.cpp
 * @brief Parser demonstration program
 */

#include "cj/cj.h"
#include <iostream>

using namespace cj;

int main() {
    std::cout << "CJ Parser Demo" << std::endl;
    std::cout << "==============" << std::endl;
    
    const String code = R"(
        var x = 10;
        var y = 20;
        var sum = x + y * 2;
        
        const greeting = "Hello, " + "World!";
        
        if (sum > 30) {
            var message = "Sum is large";
        } else {
            var message = "Sum is small";  
        }
    )";
    
    try {
        std::cout << "Source code:" << std::endl;
        std::cout << code << std::endl;
        
        // Parse the code
        auto parser = ParserFactory::FromString(code, "demo.cj");
        auto program = parser->ParseProgram();
        
        if (parser->HasErrors()) {
            std::cout << "\nParser Errors:" << std::endl;
            for (const auto& error : parser->GetErrors()) {
                std::cout << "  " << error << std::endl;
            }
            return 1;
        }
        
        std::cout << "\nParsing successful!" << std::endl;
        
        // Generate AST representation
        ASTPrettyPrinter printer;
        program->Accept(printer);
        
        std::cout << "\nAbstract Syntax Tree:" << std::endl;
        std::cout << "---------------------" << std::endl;
        std::cout << printer.GetOutput() << std::endl;
        
        // Collect symbols
        SymbolCollector collector;
        program->Accept(collector);
        
        std::cout << "Variables found:" << std::endl;
        for (const auto& var : collector.GetVariables()) {
            std::cout << "  " << var << std::endl;
        }
        
        std::cout << "\nParser demo completed!" << std::endl;
        
    } catch (const CJException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}