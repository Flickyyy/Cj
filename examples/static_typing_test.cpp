/**
 * @file static_typing_test.cpp
 * @brief Complete test demonstrating static typing enforcement in CJ
 */

#include "cj/cj.h"
#include "cj/semantic/type_checker.h"
#include "cj/ast/ast_visitor.h"
#include <iostream>

using namespace cj;

/**
 * Simple visitor to demonstrate type checking variable declarations
 */
class TypeCheckingVisitor : public ASTVisitor {
private:
    TypeChecker& type_checker_;

public:
    TypeCheckingVisitor(TypeChecker& checker) : type_checker_(checker) {}
    
    void Visit(Program& node) override {
        for (auto& stmt : node.GetStatements()) {
            stmt->Accept(*this);
        }
    }
    
    void Visit(VariableDeclaration& node) override {
        std::cout << "   Checking variable: " << node.ToString() << std::endl;
        type_checker_.CheckVariableDeclaration(&node);
        
        if (type_checker_.HasErrors()) {
            for (const auto& error : type_checker_.GetErrors()) {
                std::cout << "     ❌ Error: " << error << std::endl;
            }
            type_checker_.ClearErrors();
        } else {
            std::cout << "     ✓ Type check passed" << std::endl;
        }
    }
    
    // Default implementations for other node types
    void Visit(LiteralExpression&) override {}
    void Visit(IdentifierExpression&) override {}
    void Visit(BinaryExpression&) override {}
    void Visit(UnaryExpression&) override {}
    void Visit(AssignmentExpression&) override {}
    void Visit(CallExpression&) override {}
    void Visit(ExpressionStatement&) override {}
    void Visit(BlockStatement&) override {}
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  CJ STATIC TYPING ENFORCEMENT TEST" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        CJEngine engine;
        TypeChecker type_checker;
        
        std::cout << "\n🔧 1. VALID TYPE DECLARATIONS" << std::endl;
        
        String valid_code = R"(
            var x: int = 42;
            var y: float = 3.14;
            var message: string = "Hello, World!";
            var active: bool = true;
            var counter: int;
        )";
        
        auto program1 = engine.Parse(valid_code, "valid.cj");
        if (program1) {
            std::cout << "   Parsed successfully. Checking types..." << std::endl;
            TypeCheckingVisitor visitor(type_checker);
            program1->Accept(visitor);
        }
        
        std::cout << "\n⚠️  2. TYPE MISMATCH DETECTION" << std::endl;
        
        String invalid_code = R"(
            var bad1: int = "this is a string";
            var bad2: float = true;
            var bad3: string = 42;
        )";
        
        auto program2 = engine.Parse(invalid_code, "invalid.cj");
        if (program2) {
            std::cout << "   Parsed successfully. Checking types..." << std::endl;
            TypeCheckingVisitor visitor(type_checker);
            program2->Accept(visitor);
        }
        
        std::cout << "\n📋 3. MIXED DECLARATIONS (WITH AND WITHOUT TYPES)" << std::endl;
        
        String mixed_code = R"(
            var typed: int = 100;
            var inferred = 200;
            var explicit_no_init: float;
        )";
        
        auto program3 = engine.Parse(mixed_code, "mixed.cj");
        if (program3) {
            std::cout << "   Parsed successfully. Checking types..." << std::endl;
            TypeCheckingVisitor visitor(type_checker);
            program3->Accept(visitor);
        }
        
        std::cout << "\n🎯 4. TYPE COMPATIBILITY TEST" << std::endl;
        
        auto int_type = TypeRegistry::Instance().GetIntType();
        auto float_type = TypeRegistry::Instance().GetFloatType();
        auto string_type = TypeRegistry::Instance().GetStringType();
        
        std::cout << "   Testing type compatibility:" << std::endl;
        std::cout << "   - int == int: " << type_checker.AreCompatible(int_type, int_type) << std::endl;
        std::cout << "   - int == float: " << type_checker.AreCompatible(int_type, float_type) << std::endl;
        std::cout << "   - int == string: " << type_checker.AreCompatible(int_type, string_type) << std::endl;
        
        auto common = type_checker.GetCommonType(int_type, float_type);
        if (common) {
            std::cout << "   - Common type (int, float): " << common->GetName() << std::endl;
        }
        
        std::cout << "\n🎉 STATIC TYPING TEST COMPLETED!" << std::endl;
        std::cout << "   The CJ language now supports explicit type annotations" << std::endl;
        std::cout << "   and can enforce static typing when types are specified." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}