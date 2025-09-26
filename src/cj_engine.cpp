/**
 * @file cj_engine.cpp
 * @brief Implementation of CJ Language Engine
 */

#include "cj/cj.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace cj {

CJEngine::CJEngine() {
    InitializeEngine();
}

CJEngine::CJEngine(const VMOptions& vm_opts, const GCOptions& gc_opts, const JITOptions& jit_opts)
    : vm_options_(vm_opts), gc_options_(gc_opts), jit_options_(jit_opts) {
    InitializeEngine();
}

CJEngine::~CJEngine() = default;

void CJEngine::InitializeEngine() {
    vm_ = cj_make_unique<VirtualMachine>(vm_options_);
    RegisterBuiltinFunctions();
}

Value CJEngine::Execute(const String& source, const String& filename) {
    try {
        auto program = Parse(source, filename);
        if (!program) {
            return Value();
        }
        
        auto ir_function = Compile(program.get());
        if (!ir_function) {
            return Value();
        }
        
        return vm_->Execute(ir_function.get());
    } catch (const CJException& e) {
        std::cerr << "CJ Error: " << e.what() << std::endl;
        return Value();
    }
}

Value CJEngine::ExecuteFile(const String& filename) {
    String source = ReadFile(filename);
    return Execute(source, filename);
}

UniquePtr<Program> CJEngine::Parse(const String& source, const String& filename) {
    auto parser = ParserFactory::FromString(source, filename, parser_options_);
    auto program = parser->ParseProgram();
    
    if (parser->HasErrors()) {
        for (const auto& error : parser->GetErrors()) {
            std::cerr << "Parse Error: " << error << std::endl;
        }
        return nullptr;
    }
    
    return program;
}

UniquePtr<Program> CJEngine::ParseFile(const String& filename) {
    String source = ReadFile(filename);
    return Parse(source, filename);
}

UniquePtr<IRFunction> CJEngine::Compile(Program* ast) {
    // Stub implementation - would compile AST to IR
    auto ir_function = cj_make_unique<IRFunction>("main");
    
    // Add a simple instruction sequence
    auto block = ir_function->CreateBlock("entry");
    block->AddInstruction(IRInstruction(OpCode::LOAD_NIL));
    block->AddInstruction(IRInstruction(OpCode::RETURN));
    
    return ir_function;
}

Value CJEngine::Evaluate(const String& expression) {
    return Execute(expression + ";");
}

void CJEngine::StartREPL() {
    std::cout << "CJ Language REPL v" << VERSION << std::endl;
    std::cout << "Type 'exit' to quit" << std::endl;
    
    String line;
    while (true) {
        std::cout << "cj> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        
        if (line == "exit" || line == "quit") {
            break;
        }
        
        if (line.empty()) {
            continue;
        }
        
        ExecuteREPLCommand(line);
    }
}

void CJEngine::ExecuteREPLCommand(const String& command) {
    try {
        Value result = Evaluate(command);
        if (!result.IsNil()) {
            std::cout << result.ToString() << std::endl;
        }
    } catch (const CJException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void CJEngine::SetGlobal(const String& name, const Value& value) {
    vm_->SetGlobal(name, value);
}

Value CJEngine::GetGlobal(const String& name) {
    return vm_->GetGlobal(name);
}

void CJEngine::RegisterNative(const String& name, std::function<Value(const Vector<Value>&)> func) {
    vm_->RegisterNative(name, func);
}

void CJEngine::RegisterBuiltinFunctions() {
    // Built-in functions are already registered in VM
}

String CJEngine::ReadFile(const String& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw CJException("Cannot open file: " + filename);
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void CJEngine::PrintStats() const {
    if (vm_->GetGC()) {
        vm_->GetGC()->PrintStats();
    }
    if (vm_->GetJIT()) {
        vm_->GetJIT()->DumpStats();
    }
}

void CJEngine::ResetStats() {
    if (vm_->GetGC()) {
        vm_->GetGC()->ResetStats();
    }
    if (vm_->GetJIT()) {
        vm_->GetJIT()->ResetStats();
    }
}

namespace CJUtils {

UniquePtr<CJEngine> CreateEngine() {
    return cj_make_unique<CJEngine>();
}

UniquePtr<CJEngine> CreateDebugEngine() {
    VMOptions vm_opts;
    vm_opts.debug_mode = true;
    vm_opts.trace_execution = true;
    vm_opts.enable_jit = false;
    
    GCOptions gc_opts;
    gc_opts.debug_gc = true;
    
    JITOptions jit_opts;
    jit_opts.debug_jit = true;
    jit_opts.optimization_level = JITOptLevel::NONE;
    
    return cj_make_unique<CJEngine>(vm_opts, gc_opts, jit_opts);
}

UniquePtr<CJEngine> CreateReleaseEngine() {
    VMOptions vm_opts;
    vm_opts.debug_mode = false;
    vm_opts.trace_execution = false;
    vm_opts.enable_jit = true;
    vm_opts.execution_mode = ExecutionMode::JIT;
    
    GCOptions gc_opts;
    gc_opts.debug_gc = false;
    gc_opts.algorithm = GCAlgorithm::GENERATIONAL;
    
    JITOptions jit_opts;
    jit_opts.debug_jit = false;
    jit_opts.optimization_level = JITOptLevel::AGGRESSIVE;
    
    return cj_make_unique<CJEngine>(vm_opts, gc_opts, jit_opts);
}

Value ExecuteCode(const String& code) {
    auto engine = CreateEngine();
    return engine->Execute(code);
}

Value ExecuteFile(const String& filename) {
    auto engine = CreateEngine();
    return engine->ExecuteFile(filename);
}

String FormatCode(const String& code) {
    // Stub implementation - would format CJ code
    return code;
}

bool ValidateSyntax(const String& code, Vector<String>& errors) {
    try {
        auto engine = CreateEngine();
        auto program = engine->Parse(code);
        return program != nullptr;
    } catch (const ParserException& e) {
        errors.push_back(e.what());
        return false;
    }
}

} // namespace CJUtils

} // namespace cj