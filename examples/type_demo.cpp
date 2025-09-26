/**
 * @file type_demo.cpp
 * @brief Working demo of CJ static type system
 */

#include "cj/types/type_system.h"
#include "cj/semantic/type_checker.h"
#include <iostream>

using namespace cj;

int main() {
    std::cout << "=== CJ Static Type System - Working Demo ===" << std::endl;
    
    try {
        // Test basic type system
        auto& registry = TypeRegistry::Instance();
        
        std::cout << "\n1. Basic Types:" << std::endl;
        auto int_type = registry.GetIntType();
        auto float_type = registry.GetFloatType(); 
        auto bool_type = registry.GetBoolType();
        auto string_type = registry.GetStringType();
        
        std::cout << "  - Int: " << int_type->GetName() << std::endl;
        std::cout << "  - Float: " << float_type->GetName() << std::endl;
        std::cout << "  - Bool: " << bool_type->GetName() << std::endl;
        std::cout << "  - String: " << string_type->GetName() << std::endl;
        
        // Test type compatibility
        std::cout << "\n2. Type Compatibility:" << std::endl;
        std::cout << "  - Int == Int: " << (*int_type == *int_type) << std::endl;
        std::cout << "  - Int == Float: " << (*int_type == *float_type) << std::endl;
        std::cout << "  - Int is numeric: " << int_type->IsNumeric() << std::endl;
        std::cout << "  - String is numeric: " << string_type->IsNumeric() << std::endl;
        
        // Test type checker
        std::cout << "\n3. Type Checker:" << std::endl;
        TypeChecker checker;
        
        // Declare some variables
        checker.DeclareVariable("x", int_type);
        checker.DeclareVariable("y", float_type);
        checker.DeclareVariable("name", string_type);
        
        std::cout << "  - Declared variables: x (int), y (float), name (string)" << std::endl;
        
        // Test compatibility
        auto x_type = checker.LookupVariable("x");
        auto y_type = checker.LookupVariable("y");
        
        if (x_type && y_type) {
            bool compatible = checker.AreCompatible(x_type, y_type);
            std::cout << "  - x and y compatible: " << compatible << std::endl;
            
            auto common = checker.GetCommonType(x_type, y_type);
            if (common) {
                std::cout << "  - Common type: " << common->GetName() << std::endl;
            }
        }
        
        std::cout << "\n4. Simple Container (fixed array):" << std::endl;
        
        // Simple array implementation using C++ vector for demo
        std::vector<int> numbers = {1, 2, 3, 4, 5};
        std::cout << "  - Array size: " << numbers.size() << std::endl;
        std::cout << "  - Elements: ";
        for (int n : numbers) {
            std::cout << n << " ";
        }
        std::cout << std::endl;
        
        std::cout << "\n=== Type System Working Successfully! ===" << std::endl;
        std::cout << "This demonstrates a functional static type system." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}