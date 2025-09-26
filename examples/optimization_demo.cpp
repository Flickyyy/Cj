/**
 * @file optimization_demo.cpp
 * @brief Demonstration of improved JIT compiler optimizations
 */

#include <iostream>
#include "cj/ir/instruction.h"
#include "cj/jit/jit_compiler.h"

using namespace cj;

int main() {
    std::cout << "=== JIT Optimization Improvements Demo ===" << std::endl;
    
    // Create a function with optimization opportunities
    auto function = cj_make_unique<IRFunction>("optimization_test");
    auto block = function->CreateBlock("main");
    
    std::cout << "\nCreating IR with optimization opportunities..." << std::endl;
    
    // Add instructions that can be optimized:
    // 1. Constant folding: 5 + 10 = 15
    // 2. Multiplication by 1 (peephole)
    // 3. Addition with 0 (peephole)
    // 4. Dead code after return
    
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(5))));
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(10))));
    block->AddInstruction(IRInstruction(OpCode::ADD));  // Should be folded to 15
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(0))));
    
    block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(0))));
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(1))));
    block->AddInstruction(IRInstruction(OpCode::MUL));  // x * 1 - should be optimized away
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(1))));
    
    block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(1))));
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(0))));
    block->AddInstruction(IRInstruction(OpCode::ADD));  // x + 0 - should be optimized away
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(2))));
    
    block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(2))));
    block->AddInstruction(IRInstruction(OpCode::RETURN));
    
    // Dead code after return
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(999))));
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(3))));
    
    std::cout << "Original IR:" << std::endl;
    std::cout << function->ToString() << std::endl;
    
    // Create JIT compiler with optimizations enabled
    JITOptions options;
    options.debug_jit = true;
    options.optimization_level = JITOptLevel::AGGRESSIVE;
    options.enable_constant_folding = true;
    options.enable_dead_code_elimination = true;
    
    auto jit = cj_make_unique<JITCompiler>(options);
    
    std::cout << "\n=== Running JIT Optimizations ===" << std::endl;
    
    // Run traditional optimizations (non-SSA)
    jit->TestConstantFolding(function.get());
    jit->TestPeepholeOptimization(function.get());  
    jit->TestDeadCodeElimination(function.get());
    jit->TestRegisterAllocation(function.get());
    
    std::cout << "\nAfter traditional optimizations:" << std::endl;
    std::cout << function->ToString() << std::endl;
    
    // Display statistics
    std::cout << "\n=== Optimization Results ===" << std::endl;
    jit->DumpStats();
    
    std::cout << "\n✅ Optimization demo completed!" << std::endl;
    std::cout << "The improved JIT compiler now performs:" << std::endl;
    std::cout << "  • Constant folding (arithmetic at compile time)" << std::endl;
    std::cout << "  • Peephole optimizations (multiply by 1, add 0)" << std::endl;
    std::cout << "  • Dead code elimination (unreachable code)" << std::endl;
    std::cout << "  • Register allocation analysis (usage statistics)" << std::endl;
    
    return 0;
}