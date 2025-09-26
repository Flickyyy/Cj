/**
 * @file virtual_machine.h
 * @brief Virtual Machine implementation for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_VIRTUAL_MACHINE_H
#define CJ_VIRTUAL_MACHINE_H

#include "../common.h"
#include "../types/value.h"
#include "../ir/instruction.h"
#include <stack>

namespace cj {

// Forward declarations
class GarbageCollector;
class JITCompiler;

/**
 * @brief VM execution modes
 */
enum class ExecutionMode {
    INTERPRET,      // Pure interpretation
    JIT,           // Just-in-time compilation
    HYBRID         // Mixed interpretation and JIT
};

/**
 * @brief VM configuration options
 */
struct VMOptions {
    ExecutionMode execution_mode = ExecutionMode::INTERPRET;
    Size initial_stack_size = 1024;
    Size max_stack_size = 1024 * 1024;
    Size initial_heap_size = 1024 * 1024;
    Size max_heap_size = 512 * 1024 * 1024;
    bool enable_gc = true;
    bool enable_jit = true;
    Size jit_threshold = 100;  // Function call count threshold for JIT
    bool debug_mode = false;
    bool trace_execution = false;
    
    VMOptions() = default;
};

/**
 * @brief Call frame for function calls
 */
struct CallFrame {
    IRFunction* function;
    Size instruction_pointer;
    Size base_pointer;
    Size local_count;
    
    CallFrame(IRFunction* func, Size ip, Size bp, Size locals)
        : function(func), instruction_pointer(ip), base_pointer(bp), local_count(locals) {}
};

/**
 * @brief Virtual Machine execution state
 */
enum class VMState {
    READY,
    RUNNING,
    PAUSED,
    FINISHED,
    ERROR,
    DEBUGGING
};

/**
 * @brief Virtual Machine for CJ bytecode execution
 */
class VirtualMachine {
private:
    VMOptions options_;
    VMState state_;
    
    // Execution stacks
    Vector<Value> stack_;
    Vector<CallFrame> call_stack_;
    Size stack_top_;
    
    // Global variables
    HashMap<String, Value> globals_;
    
    // Memory management
    UniquePtr<GarbageCollector> gc_;
    
    // JIT compilation
    UniquePtr<JITCompiler> jit_;
    HashMap<IRFunction*, Size> call_counts_;
    
    // Current execution context
    IRFunction* current_function_;
    Size instruction_pointer_;
    
    // Built-in functions
    HashMap<String, std::function<Value(const Vector<Value>&)>> natives_;
    
    // Error handling
    String last_error_;
    Vector<String> error_stack_;

public:
    /**
     * @brief Construct VM with options
     */
    explicit VirtualMachine(const VMOptions& options = VMOptions());
    
    /**
     * @brief Destructor
     */
    ~VirtualMachine();
    
    /**
     * @brief Execute a function
     */
    Value Execute(IRFunction* function, const Vector<Value>& args = {});
    
    /**
     * @brief Execute a single instruction
     */
    void ExecuteInstruction(const IRInstruction& instruction);
    
    /**
     * @brief VM control
     */
    void Start();
    void Pause();
    void Stop();
    void Reset();
    
    /**
     * @brief State management
     */
    VMState GetState() const { return state_; }
    bool IsRunning() const { return state_ == VMState::RUNNING; }
    bool HasError() const { return state_ == VMState::ERROR; }
    const String& GetLastError() const { return last_error_; }
    
    /**
     * @brief Stack operations
     */
    void Push(const Value& value);
    Value Pop();
    Value Peek(Size offset = 0) const;
    void SetStackValue(Size index, const Value& value);
    Value GetStackValue(Size index) const;
    
    Size GetStackSize() const { return stack_top_; }
    bool IsStackEmpty() const { return stack_top_ == 0; }
    
    /**
     * @brief Global variable management
     */
    void SetGlobal(const String& name, const Value& value);
    Value GetGlobal(const String& name) const;
    bool HasGlobal(const String& name) const;
    void RemoveGlobal(const String& name);
    
    /**
     * @brief Local variable management
     */
    void SetLocal(Size index, const Value& value);
    Value GetLocal(Size index) const;
    
    /**
     * @brief Function call management
     */
    void Call(IRFunction* function, Size arg_count);
    void Return(const Value& return_value = Value());
    
    /**
     * @brief Native function registration
     */
    void RegisterNative(const String& name, std::function<Value(const Vector<Value>&)> func);
    Value CallNative(const String& name, const Vector<Value>& args);
    
    /**
     * @brief Memory management
     */
    GarbageCollector* GetGC() { return gc_.get(); }
    void TriggerGC();
    Size GetMemoryUsage() const;
    
    /**
     * @brief JIT compilation
     */
    JITCompiler* GetJIT() { return jit_.get(); }
    void EnableJIT(bool enable);
    bool IsJITEnabled() const;
    
    /**
     * @brief Debugging support
     */
    void SetBreakpoint(IRFunction* function, Size instruction_index);
    void RemoveBreakpoint(IRFunction* function, Size instruction_index);
    void Step();
    void Continue();
    
    /**
     * @brief Execution tracing
     */
    void EnableTracing(bool enable);
    bool IsTracingEnabled() const;
    
    /**
     * @brief Statistics
     */
    Size GetInstructionCount() const;
    Size GetFunctionCallCount(IRFunction* function) const;
    
    /**
     * @brief Options
     */
    const VMOptions& GetOptions() const { return options_; }
    void SetOptions(const VMOptions& options) { options_ = options; }

private:
    // Instruction execution helpers
    void ExecuteArithmetic(OpCode opcode);
    void ExecuteComparison(OpCode opcode);
    void ExecuteLogical(OpCode opcode);
    void ExecuteBitwise(OpCode opcode);
    void ExecuteJump(const IRInstruction& instruction);
    void ExecuteCall(const IRInstruction& instruction);
    void ExecuteReturn();
    
    // Stack management helpers
    void GrowStack();
    void CheckStackOverflow();
    void CheckStackUnderflow(Size required);
    
    // Error handling
    void RuntimeError(const String& message);
    void SetError(const String& error);
    void ClearError();
    
    // Native function implementations
    void RegisterBuiltinNatives();
    static Value NativePrint(const Vector<Value>& args);
    static Value NativeType(const Vector<Value>& args);
    static Value NativeLen(const Vector<Value>& args);
    static Value NativeStr(const Vector<Value>& args);
    static Value NativeInt(const Vector<Value>& args);
    static Value NativeFloat(const Vector<Value>& args);
    static Value NativeBool(const Vector<Value>& args);
    
    // Utility methods
    bool IsTruthy(const Value& value) const;
    void TraceInstruction(const IRInstruction& instruction);
    String FormatStackTrace() const;
};

/**
 * @brief VM factory functions
 */
namespace VMFactory {
    /**
     * @brief Create VM with default options
     */
    UniquePtr<VirtualMachine> Create(const VMOptions& options = VMOptions());
    
    /**
     * @brief Create VM optimized for development
     */
    UniquePtr<VirtualMachine> CreateDebug();
    
    /**
     * @brief Create VM optimized for production
     */
    UniquePtr<VirtualMachine> CreateRelease();
}

} // namespace cj

#endif // CJ_VIRTUAL_MACHINE_H