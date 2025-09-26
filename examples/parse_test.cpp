/**
 * @file parse_test.cpp
 * @brief Test program to verify static typing parser support
 */

#include "cj/cj.h"
#include <iostream>

using namespace cj;

int main() {
    std::cout << "=== CJ Static Typing Parser Test ===" << std::endl;
    
    try {
        CJEngine engine;
        
        std::cout << "\n1. Testing basic type annotation parsing..." << std::endl;
        
        // Test parsing variable declarations with type annotations
        String code = R"(
            var x: int = 42;
            var y: float = 3.14;
            var message: string = "Hello";
            var active: bool = true;
        )";
        
        auto program = engine.Parse(code, "test.cj");
        if (program) {
            std::cout << "   ✓ Successfully parsed code with type annotations" << std::endl;
            std::cout << "   Program: " << program->ToString() << std::endl;
        } else {
            std::cout << "   ❌ Failed to parse code with type annotations" << std::endl;
        }
        
        std::cout << "\n2. Testing variable without initializer..." << std::endl;
        
        String code2 = R"(
            var counter: int;
        )";
        
        auto program2 = engine.Parse(code2, "test2.cj");
        if (program2) {
            std::cout << "   ✓ Successfully parsed variable without initializer" << std::endl;
            std::cout << "   Program: " << program2->ToString() << std::endl;
        } else {
            std::cout << "   ❌ Failed to parse variable without initializer" << std::endl;
        }
        
        std::cout << "\n3. Testing mixed syntax (with and without types)..." << std::endl;
        
        String code3 = R"(
            var typed: int = 100;
            var inferred = 200;
        )";
        
        auto program3 = engine.Parse(code3, "test3.cj");
        if (program3) {
            std::cout << "   ✓ Successfully parsed mixed syntax" << std::endl;
            std::cout << "   Program: " << program3->ToString() << std::endl;
        } else {
            std::cout << "   ❌ Failed to parse mixed syntax" << std::endl;
        }
        
        std::cout << "\n🎉 Static typing parser test completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}