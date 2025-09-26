/**
 * @file jit_compiler.h
 * @brief Just-In-Time compiler for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_JIT_COMPILER_H
#define CJ_JIT_COMPILER_H

#include "../common.h"
#include "../ir/instruction.h"
#include "../types/value.h"
#include <functional>
#include <atomic>

namespace cj {

// Forward declarations
class VirtualMachine;

/**
 * @brief JIT compilation targets
 */
enum class JITTarget {
    X86_64,
    ARM64,
    WASM,
    NATIVE  // Use system's native architecture
};

/**
 * @brief JIT optimization levels
 */
enum class JITOptLevel {
    NONE,       // No optimization (O0)
    BASIC,      // Basic optimizations (O1)
    STANDARD,   // Standard optimizations (O2)
    AGGRESSIVE  // Aggressive optimizations (O3)
};

/**
 * @brief JIT compiler options
 */
struct JITOptions {
    JITTarget target = JITTarget::NATIVE;
    JITOptLevel optimization_level = JITOptLevel::STANDARD;
    bool enable_profiling = true;
    bool enable_inlining = true;
    bool enable_loop_unrolling = true;
    bool enable_constant_folding = true;
    bool enable_dead_code_elimination = true;
    bool enable_register_allocation = true;
    Size inline_threshold = 100;       // Maximum instructions to inline
    Size unroll_threshold = 10;        // Maximum loop iterations to unroll
    Size compilation_threshold = 100;  // Function call count to trigger compilation
    bool debug_jit = false;
    bool dump_ir = false;
    bool dump_asm = false;
    
    JITOptions() = default;
};

/**
 * @brief Compiled function representation
 */
class CompiledFunction {
private:
    String name_;
    void* native_code_;
    Size code_size_;
    std::function<Value(const Vector<Value>&)> entry_point_;
    Size call_count_;
    Float64 avg_execution_time_;
    bool is_optimized_;

public:
    CompiledFunction(const String& name, void* code, Size size,
                    std::function<Value(const Vector<Value>&)> entry)
        : name_(name), native_code_(code), code_size_(size), 
          entry_point_(entry), call_count_(0), avg_execution_time_(0.0), 
          is_optimized_(false) {}
    
    ~CompiledFunction();
    
    /**
     * @brief Execute compiled function
     */
    Value Execute(const Vector<Value>& args);
    
    /**
     * @brief Function properties
     */
    const String& GetName() const { return name_; }
    void* GetNativeCode() const { return native_code_; }
    Size GetCodeSize() const { return code_size_; }
    Size GetCallCount() const { return call_count_; }
    Float64 GetAverageExecutionTime() const { return avg_execution_time_; }
    bool IsOptimized() const { return is_optimized_; }
    void SetOptimized(bool optimized) { is_optimized_ = optimized; }
    
    /**
     * @brief Performance tracking
     */
    void RecordCall(Float64 execution_time);
};

/**
 * @brief JIT compilation statistics
 */
struct JITStats {
    std::atomic<Size> functions_compiled{0};
    std::atomic<Size> functions_optimized{0};
    std::atomic<Size> compilation_time_ms{0};
    std::atomic<Size> optimization_time_ms{0};
    std::atomic<Size> code_cache_size{0};
    std::atomic<Size> code_cache_hits{0};
    std::atomic<Size> code_cache_misses{0};
    std::atomic<Float64> speedup_ratio{1.0};
    
    void Reset() {
        functions_compiled = 0;
        functions_optimized = 0;
        compilation_time_ms = 0;
        optimization_time_ms = 0;
        code_cache_size = 0;
        code_cache_hits = 0;
        code_cache_misses = 0;
        speedup_ratio = 1.0;
    }
};

/**
 * @brief Just-In-Time compiler implementation
 */
class JITCompiler {
public:
    class CodeGenerator; // Forward declaration

private:
    JITOptions options_;
    VirtualMachine* vm_;
    
    // Compiled function cache
    HashMap<IRFunction*, UniquePtr<CompiledFunction>> compiled_functions_;
    HashMap<String, CompiledFunction*> function_cache_;
    
    // Code generation
    void* code_buffer_;
    Size code_buffer_size_;
    Size code_buffer_used_;
    
    // Optimization pipeline
    Vector<std::function<void(IRFunction*)>> optimizers_;
    
    // Profiling data
    HashMap<IRFunction*, Size> call_counts_;
    HashMap<IRFunction*, Vector<Float64>> execution_times_;
    
    // Statistics
    JITStats stats_;
    
    // Platform-specific code generator
    UniquePtr<CodeGenerator> code_gen_;

public:
    /**
     * @brief Construct JIT compiler
     */
    explicit JITCompiler(const JITOptions& options = JITOptions());
    
    /**
     * @brief Destructor
     */
    ~JITCompiler();
    
    /**
     * @brief Initialize JIT with VM reference
     */
    void Initialize(VirtualMachine* vm);
    
    /**
     * @brief Compile IR function to native code
     */
    CompiledFunction* Compile(IRFunction* function);
    
    /**
     * @brief Check if function should be compiled
     */
    bool ShouldCompile(IRFunction* function) const;
    
    /**
     * @brief Get compiled function
     */
    CompiledFunction* GetCompiledFunction(IRFunction* function);
    CompiledFunction* GetCompiledFunction(const String& name);
    
    /**
     * @brief Function profiling
     */
    void RecordCall(IRFunction* function);
    void RecordExecutionTime(IRFunction* function, Float64 time);
    Size GetCallCount(IRFunction* function) const;
    
    /**
     * @brief Optimization
     */
    void OptimizeFunction(IRFunction* function);
    void RecompileWithOptimizations(IRFunction* function);
    
    /**
     * @brief Cache management
     */
    void ClearCache();
    void InvalidateFunction(IRFunction* function);
    Size GetCacheSize() const { return compiled_functions_.size(); }
    
    /**
     * @brief Statistics
     */
    const JITStats& GetStats() const { return stats_; }
    void ResetStats() { stats_.Reset(); }
    
    /**
     * @brief Configuration
     */
    const JITOptions& GetOptions() const { return options_; }
    void SetOptions(const JITOptions& options) { options_ = options; }
    
    /**
     * @brief Code generation utilities
     */
    void* AllocateCodeMemory(Size size);
    void FreeCodeMemory(void* ptr, Size size);
    void FlushInstructionCache(void* ptr, Size size);
    
    /**
     * @brief Debugging
     */
    void DumpCompiledFunction(const String& name) const;
    void DumpStats() const;
    void EnableDebug(bool enable) { options_.debug_jit = enable; }

private:
    // Compilation pipeline
    void RunOptimizationPasses(IRFunction* function);
    void GenerateNativeCode(IRFunction* function, CompiledFunction* compiled);
    
    // Optimization passes
    void ConstantFolding(IRFunction* function);
    void DeadCodeElimination(IRFunction* function);
    void InlineSmallFunctions(IRFunction* function);
    void LoopUnrolling(IRFunction* function);
    void RegisterAllocation(IRFunction* function);
    void PeepholeOptimization(IRFunction* function);
    
    // Analysis passes
    void ControlFlowAnalysis(IRFunction* function);
    void DataFlowAnalysis(IRFunction* function);
    void LivenessAnalysis(IRFunction* function);
    
    // SSA-based passes
    void ConstructSSA(IRFunction* function);
    void DeconstructSSA(IRFunction* function);
    void SSAConstantPropagation(IRFunction* function);
    void SSADeadCodeElimination(IRFunction* function);
    
    // Code generation helpers
    void SetupCodeGenerator();
    bool IsHotFunction(IRFunction* function) const;
    bool ShouldInline(IRFunction* function) const;
    
    // Memory management
    void GrowCodeBuffer();
    void CompactCodeBuffer();
    
    // Platform detection
    JITTarget DetectNativeTarget() const;
    bool IsTargetSupported(JITTarget target) const;
    
    // Debug helpers
    void DebugLog(const String& message) const;
    String DisassembleFunction(CompiledFunction* function) const;
};

/**
 * @brief Platform-specific code generator interface
 */
class JITCompiler::CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    
    /**
     * @brief Generate native code for IR function
     */
    virtual void* GenerateCode(IRFunction* function, Size& code_size) = 0;
    
    /**
     * @brief Get target architecture
     */
    virtual JITTarget GetTarget() const = 0;
    
    /**
     * @brief Platform-specific optimizations
     */
    virtual void OptimizeForTarget(IRFunction* function) = 0;
    
    /**
     * @brief Disassemble generated code
     */
    virtual String Disassemble(void* code, Size size) const = 0;
};

/**
 * @brief x86-64 code generator
 */
class X86_64CodeGenerator : public JITCompiler::CodeGenerator {
public:
    void* GenerateCode(IRFunction* function, Size& code_size) override;
    JITTarget GetTarget() const override { return JITTarget::X86_64; }
    void OptimizeForTarget(IRFunction* function) override;
    String Disassemble(void* code, Size size) const override;

private:
    // x86-64 specific instruction encoding
    void EmitInstruction(OpCode opcode, const Vector<Value>& operands);
    void EmitMovImm(int reg, Int64 value);
    void EmitAdd(int dst, int src);
    void EmitCall(void* target);
    void EmitRet();
    
    // Register allocation for x86-64
    int AllocateRegister();
    void FreeRegister(int reg);
    void SpillRegister(int reg);
    
    Vector<UInt8> code_buffer_;
    HashMap<int, bool> register_usage_;
};

/**
 * @brief JIT factory functions
 */
namespace JITFactory {
    /**
     * @brief Create JIT compiler for target
     */
    UniquePtr<JITCompiler> Create(JITTarget target, const JITOptions& options = JITOptions());
    
    /**
     * @brief Create JIT compiler for native target
     */
    UniquePtr<JITCompiler> CreateNative(const JITOptions& options = JITOptions());
    
    /**
     * @brief Create JIT compiler optimized for development
     */
    UniquePtr<JITCompiler> CreateDebug();
    
    /**
     * @brief Create JIT compiler optimized for production
     */
    UniquePtr<JITCompiler> CreateRelease();
}

} // namespace cj

#endif // CJ_JIT_COMPILER_H