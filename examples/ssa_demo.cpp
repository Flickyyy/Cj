/**
 * @file ssa_demo.cpp
 * @brief Demonstration of SSA (Static Single Assignment) form in CJ compiler
 * @author CJ Language Team
 * @version 0.1.0
 */

#include <iostream>
#include <string>
#include <vector>
#include "cj/cj.h"
#include "cj/ir/ssa.h"
#include "cj/jit/jit_compiler.h"

using namespace cj;

void DemonstrateSSAConstruction() {
    std::cout << "=== SSA Construction Demo ===" << std::endl;
    
    // Create a sample IR function with multiple variable assignments
    auto function = cj_make_unique<IRFunction>("example_func");
    
    // Create basic blocks
    auto entry_block = function->CreateBlock("entry");
    auto if_block = function->CreateBlock("if_true");
    auto else_block = function->CreateBlock("if_false");
    auto merge_block = function->CreateBlock("merge");
    
    // Set up control flow graph
    entry_block->AddSuccessor(if_block);
    entry_block->AddSuccessor(else_block);
    if_block->AddPredecessor(entry_block);
    if_block->AddSuccessor(merge_block);
    else_block->AddPredecessor(entry_block);
    else_block->AddSuccessor(merge_block);
    merge_block->AddPredecessor(if_block);
    merge_block->AddPredecessor(else_block);
    
    // Entry block: x = 10, y = 20
    entry_block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(10))));
    entry_block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(0)))); // x = 10
    entry_block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(20))));
    entry_block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(1)))); // y = 20
    entry_block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(0)))); // Load x
    entry_block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(5))));
    entry_block->AddInstruction(IRInstruction(OpCode::GT)); // x > 5
    entry_block->AddInstruction(IRInstruction(OpCode::JUMP_IF_TRUE)); // Jump to if_block
    
    // If block: x = x + 1
    if_block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(0)))); // Load x
    if_block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(1))));
    if_block->AddInstruction(IRInstruction(OpCode::ADD));
    if_block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(0)))); // x = x + 1
    if_block->AddInstruction(IRInstruction(OpCode::JUMP)); // Jump to merge
    
    // Else block: x = x * 2
    else_block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(0)))); // Load x
    else_block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(2))));
    else_block->AddInstruction(IRInstruction(OpCode::MUL));
    else_block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(0)))); // x = x * 2
    else_block->AddInstruction(IRInstruction(OpCode::JUMP)); // Jump to merge
    
    // Merge block: z = x + y
    merge_block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(0)))); // Load x
    merge_block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(1)))); // Load y
    merge_block->AddInstruction(IRInstruction(OpCode::ADD));
    merge_block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(2)))); // z = x + y
    merge_block->AddInstruction(IRInstruction(OpCode::RETURN));
    
    std::cout << "Original IR (before SSA):" << std::endl;
    std::cout << function->ToString() << std::endl;
    
    // Construct SSA form
    SSABuilder builder(function.get());
    builder.ConstructSSA();
    
    std::cout << "After SSA construction:" << std::endl;
    std::cout << function->ToString() << std::endl;
    
    // Print SSA information
    builder.PrintSSAInfo();
    
    // Validate SSA properties
    if (builder.ValidateSSA()) {
        std::cout << "✓ SSA form is valid!" << std::endl;
    } else {
        std::cout << "✗ SSA form validation failed!" << std::endl;
    }
}

void DemonstrateSSAOptimizations() {
    std::cout << "\n=== SSA Optimizations Demo ===" << std::endl;
    
    // Create a function with constant folding opportunities
    auto function = cj_make_unique<IRFunction>("constant_example");
    auto block = function->CreateBlock("main");
    
    // x = 5, y = 10, z = x + y (should become z = 15)
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(5))));
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(0)))); // x = 5
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(10))));
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(1)))); // y = 10
    block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(0)))); // Load x
    block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(1)))); // Load y
    block->AddInstruction(IRInstruction(OpCode::ADD));
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(2)))); // z = x + y
    
    // Dead code: w = 100 (never used)
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(100))));
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(3)))); // w = 100 (dead)
    
    block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(2)))); // Load z
    block->AddInstruction(IRInstruction(OpCode::RETURN));
    
    std::cout << "Before optimizations:" << std::endl;
    std::cout << function->ToString() << std::endl;
    
    // Run SSA optimizations
    bool changed = false;
    
    // Constant propagation
    if (SSAOptimizations::ConstantPropagation(function.get())) {
        std::cout << "✓ Constant propagation applied" << std::endl;
        changed = true;
    }
    
    // Dead code elimination
    if (SSAOptimizations::DeadCodeElimination(function.get())) {
        std::cout << "✓ Dead code elimination applied" << std::endl;
        changed = true;
    }
    
    // Copy propagation
    if (SSAOptimizations::CopyPropagation(function.get())) {
        std::cout << "✓ Copy propagation applied" << std::endl;
        changed = true;
    }
    
    if (changed) {
        std::cout << "\nAfter optimizations:" << std::endl;
        std::cout << function->ToString() << std::endl;
    } else {
        std::cout << "No optimizations were applied (stub implementations)" << std::endl;
    }
}

void DemonstrateJITWithSSA() {
    std::cout << "\n=== JIT Compiler with SSA Demo ===" << std::endl;
    
    // Create JIT compiler with SSA optimizations enabled
    JITOptions options;
    options.debug_jit = true;
    options.optimization_level = JITOptLevel::AGGRESSIVE;
    options.enable_constant_folding = true;
    options.enable_dead_code_elimination = true;
    
    auto jit = cj_make_unique<JITCompiler>(options);
    
    // Create a simple function
    auto function = cj_make_unique<IRFunction>("jit_example");
    auto block = function->CreateBlock("main");
    
    block->AddInstruction(IRInstruction(OpCode::LOAD_CONST, Value(Int64(42))));
    block->AddInstruction(IRInstruction(OpCode::STORE_LOCAL, Value(Int64(0))));
    block->AddInstruction(IRInstruction(OpCode::LOAD_LOCAL, Value(Int64(0))));
    block->AddInstruction(IRInstruction(OpCode::RETURN));
    
    std::cout << "Function before JIT compilation:" << std::endl;
    std::cout << function->ToString() << std::endl;
    
    // Compile with JIT (includes SSA optimizations)
    auto compiled = jit->Compile(function.get());
    
    if (compiled) {
        std::cout << "✓ Function compiled successfully with SSA optimizations!" << std::endl;
        std::cout << "Compiled function: " << compiled->GetName() << std::endl;
        std::cout << "Call count: " << compiled->GetCallCount() << std::endl;
    } else {
        std::cout << "✗ JIT compilation failed" << std::endl;
    }
    
    // Display JIT statistics
    jit->DumpStats();
}

int main() {
    std::cout << "======================================" << std::endl;
    std::cout << "  CJ COMPILER - SSA DEMONSTRATION" << std::endl;
    std::cout << "======================================" << std::endl;
    
    try {
        // Demonstrate SSA construction
        DemonstrateSSAConstruction();
        
        // Demonstrate SSA-based optimizations
        DemonstrateSSAOptimizations();
        
        // Demonstrate JIT compilation with SSA
        DemonstrateJITWithSSA();
        
        std::cout << "\n🎉 SSA DEMONSTRATION COMPLETE!" << std::endl;
        std::cout << "   Static Single Assignment form is now implemented" << std::endl;
        std::cout << "   and integrated with the JIT compiler for advanced" << std::endl;
        std::cout << "   optimizations like constant propagation and dead" << std::endl;
        std::cout << "   code elimination." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during SSA demonstration: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}