/**
 * @file cj.h
 * @brief Main header file for the CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_H
#define CJ_H

// Core includes
#include "common.h"
#include "types/value.h"

// Lexer and Parser
#include "lexer/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

// AST
#include "ast/ast_node.h"
#include "ast/ast_visitor.h"

// IR
#include "ir/instruction.h"

// Virtual Machine
#include "vm/virtual_machine.h"

// Garbage Collector
#include "gc/garbage_collector.h"

// JIT Compiler
#include "jit/jit_compiler.h"

/**
 * @brief Main CJ namespace
 */
namespace cj {

/**
 * @brief CJ Language Engine - High-level interface
 */
class CJEngine {
private:
    UniquePtr<VirtualMachine> vm_;
    LexerOptions lexer_options_;
    ParserOptions parser_options_;
    VMOptions vm_options_;
    GCOptions gc_options_;
    JITOptions jit_options_;
    
    Vector<String> include_paths_;
    HashMap<String, UniquePtr<Program>> loaded_modules_;
    
public:
    /**
     * @brief Construct CJ engine with default options
     */
    CJEngine();
    
    /**
     * @brief Construct CJ engine with custom options
     */
    CJEngine(const VMOptions& vm_opts, const GCOptions& gc_opts, const JITOptions& jit_opts);
    
    /**
     * @brief Destructor
     */
    ~CJEngine();
    
    /**
     * @brief Execute CJ source code
     */
    Value Execute(const String& source, const String& filename = "");
    
    /**
     * @brief Execute CJ file
     */
    Value ExecuteFile(const String& filename);
    
    /**
     * @brief Parse CJ source to AST
     */
    UniquePtr<Program> Parse(const String& source, const String& filename = "");
    
    /**
     * @brief Parse CJ file to AST
     */
    UniquePtr<Program> ParseFile(const String& filename);
    
    /**
     * @brief Compile AST to IR
     */
    UniquePtr<IRFunction> Compile(Program* ast);
    
    /**
     * @brief Evaluate expression
     */
    Value Evaluate(const String& expression);
    
    /**
     * @brief REPL interface
     */
    void StartREPL();
    void ExecuteREPLCommand(const String& command);
    
    /**
     * @brief Module system
     */
    void AddIncludePath(const String& path);
    Program* LoadModule(const String& module_name);
    void UnloadModule(const String& module_name);
    
    /**
     * @brief Global variable management
     */
    void SetGlobal(const String& name, const Value& value);
    Value GetGlobal(const String& name);
    
    /**
     * @brief Native function registration
     */
    void RegisterNative(const String& name, std::function<Value(const Vector<Value>&)> func);
    
    /**
     * @brief Engine configuration
     */
    void SetLexerOptions(const LexerOptions& options) { lexer_options_ = options; }
    void SetParserOptions(const ParserOptions& options) { parser_options_ = options; }
    void SetVMOptions(const VMOptions& options) { vm_options_ = options; }
    void SetGCOptions(const GCOptions& options) { gc_options_ = options; }
    void SetJITOptions(const JITOptions& options) { jit_options_ = options; }
    
    const LexerOptions& GetLexerOptions() const { return lexer_options_; }
    const ParserOptions& GetParserOptions() const { return parser_options_; }
    const VMOptions& GetVMOptions() const { return vm_options_; }
    const GCOptions& GetGCOptions() const { return gc_options_; }
    const JITOptions& GetJITOptions() const { return jit_options_; }
    
    /**
     * @brief Engine state
     */
    VirtualMachine* GetVM() { return vm_.get(); }
    
    /**
     * @brief Debugging support
     */
    void EnableDebugMode(bool enable);
    void SetBreakpoint(const String& filename, Size line);
    void RemoveBreakpoint(const String& filename, Size line);
    
    /**
     * @brief Statistics and profiling
     */
    void PrintStats() const;
    void ResetStats();

private:
    void InitializeEngine();
    void RegisterBuiltinFunctions();
    String ReadFile(const String& filename);
    String FindModule(const String& module_name);
};

/**
 * @brief Utility functions for common operations
 */
namespace CJUtils {
    /**
     * @brief Create a CJ engine with default settings
     */
    UniquePtr<CJEngine> CreateEngine();
    
    /**
     * @brief Create a CJ engine optimized for development
     */
    UniquePtr<CJEngine> CreateDebugEngine();
    
    /**
     * @brief Create a CJ engine optimized for production
     */
    UniquePtr<CJEngine> CreateReleaseEngine();
    
    /**
     * @brief Execute CJ code with default engine
     */
    Value ExecuteCode(const String& code);
    
    /**
     * @brief Execute CJ file with default engine
     */
    Value ExecuteFile(const String& filename);
    
    /**
     * @brief Format CJ code
     */
    String FormatCode(const String& code);
    
    /**
     * @brief Validate CJ syntax
     */
    bool ValidateSyntax(const String& code, Vector<String>& errors);
}

} // namespace cj

#endif // CJ_H