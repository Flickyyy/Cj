/**
 * @file hello_world.cpp
 * @brief Hello World example using CJ engine
 */

#include "cj/cj.h"
#include <iostream>

using namespace cj;

int main() {
    std::cout << "CJ Hello World Example" << std::endl;
    std::cout << "======================" << std::endl;
    
    try {
        // Create CJ engine
        auto engine = CJUtils::CreateEngine();
        
        // Simple CJ program
        const String cj_code = R"(
            var message = "Hello, World from CJ!";
            print(message);
            
            var x = 10;
            var y = 20;
            var sum = x + y;
            
            print("The sum of " + x + " and " + y + " is " + sum);
        )";
        
        std::cout << "Executing CJ code:" << std::endl;
        std::cout << cj_code << std::endl;
        std::cout << "Output:" << std::endl;
        
        // Execute the code
        engine->Execute(cj_code, "hello.cj");
        
    } catch (const CJException& e) {
        std::cerr << "CJ Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}