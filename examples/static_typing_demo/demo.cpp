/**
 * @file demo.cpp  
 * @brief Simple demo showing that the static type system actually works
 */

#include <iostream>
#include "cj/types/type_system.h"
#include "cj/stdlib/containers.h"

using namespace cj;
using namespace cj::stdlib;

int main() {
    std::cout << "=== CJ Static Type System Demo ===" << std::endl;
    
    try {
        // Test basic type system
        auto& registry = TypeRegistry::Instance();
        
        auto int_type = registry.GetInt32Type();
        std::cout << "✓ Int32 type: " << int_type->ToString() << std::endl;
        
        auto string_type = registry.GetStringType();  
        std::cout << "✓ String type: " << string_type->ToString() << std::endl;
        
        // Test array type
        auto array_type = registry.CreateArrayType(int_type, 5);
        std::cout << "✓ Array type: " << array_type->ToString() << std::endl;
        
        // Test containers (these are template-based and work)
        Vector<int> numbers = {1, 2, 3, 4, 5};
        std::cout << "✓ Vector with " << numbers.Size() << " elements" << std::endl;
        
        String text = "Hello World";
        std::cout << "✓ String: " << text.CStr() << " (length: " << text.Length() << ")" << std::endl;
        
        Stack<int> stack;
        stack.Push(42);
        std::cout << "✓ Stack top: " << stack.Top() << std::endl;
        
        std::cout << "\n=== Type System Headers Work! ===" << std::endl;
        std::cout << "But they're not integrated with the CJ language parser/VM yet." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}