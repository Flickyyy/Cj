/**
 * @file virtual_machine.cpp
 * @brief Stub implementation of Virtual Machine for CJ language
 */

#include "cj/vm/virtual_machine.h"
#include "cj/gc/garbage_collector.h"
#include "cj/jit/jit_compiler.h"
#include <iostream>
#include <cmath>

namespace cj {

VirtualMachine::VirtualMachine(const VMOptions& options)
    : options_(options), state_(VMState::READY), stack_top_(0),
      current_function_(nullptr), instruction_pointer_(0) {
    
    stack_.resize(options_.initial_stack_size);
    
    if (options_.enable_gc) {
        GCOptions gc_opts;
        gc_opts.initial_heap_size = options_.initial_heap_size;
        gc_opts.max_heap_size = options_.max_heap_size;
        gc_ = make_unique<GarbageCollector>(gc_opts);
        gc_->Initialize(this);
    }
    
    if (options_.enable_jit) {
        JITOptions jit_opts;
        jit_opts.compilation_threshold = options_.jit_threshold;
        jit_ = make_unique<JITCompiler>(jit_opts);
        jit_->Initialize(this);
    }
    
    RegisterBuiltinNatives();
}

VirtualMachine::~VirtualMachine() = default;

Value VirtualMachine::Execute(IRFunction* function, const Vector<Value>& args) {
    if (!function) {
        RuntimeError("Cannot execute null function");
        return Value();
    }
    
    current_function_ = function;
    instruction_pointer_ = 0;
    state_ = VMState::RUNNING;
    
    // Push arguments onto stack
    for (const auto& arg : args) {
        Push(arg);
    }
    
    // Simple execution loop (stub implementation)
    const auto& blocks = function->GetBlocks();
    if (!blocks.empty()) {
        const auto& instructions = blocks[0]->GetInstructions();
        
        for (const auto& instruction : instructions) {
            if (state_ != VMState::RUNNING) break;
            
            ExecuteInstruction(instruction);
        }
    }
    
    state_ = VMState::FINISHED;
    
    // Return top of stack or nil
    return IsStackEmpty() ? Value() : Pop();
}

void VirtualMachine::ExecuteInstruction(const IRInstruction& instruction) {
    if (options_.trace_execution) {
        TraceInstruction(instruction);
    }
    
    switch (instruction.GetOpCode()) {
        case OpCode::NOP:
            break;
            
        case OpCode::LOAD_CONST:
            if (instruction.GetOperandCount() > 0) {
                Push(instruction.GetOperand(0));
            }
            break;
            
        case OpCode::LOAD_NIL:
            Push(Value());
            break;
            
        case OpCode::LOAD_TRUE:
            Push(Value(true));
            break;
            
        case OpCode::LOAD_FALSE:
            Push(Value(false));
            break;
            
        case OpCode::POP:
            if (!IsStackEmpty()) {
                Pop();
            }
            break;
            
        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        case OpCode::MOD:
        case OpCode::POW:
            ExecuteArithmetic(instruction.GetOpCode());
            break;
            
        case OpCode::EQ:
        case OpCode::NE:
        case OpCode::LT:
        case OpCode::LE:
        case OpCode::GT:
        case OpCode::GE:
            ExecuteComparison(instruction.GetOpCode());
            break;
            
        case OpCode::LOGICAL_AND:
        case OpCode::LOGICAL_OR:
        case OpCode::LOGICAL_NOT:
            ExecuteLogical(instruction.GetOpCode());
            break;
            
        case OpCode::RETURN:
            ExecuteReturn();
            break;
            
        case OpCode::HALT:
            state_ = VMState::FINISHED;
            break;
            
        default:
            RuntimeError("Unknown instruction: " + IRUtils::OpCodeToString(instruction.GetOpCode()));
            break;
    }
}

void VirtualMachine::Start() {
    state_ = VMState::RUNNING;
}

void VirtualMachine::Pause() {
    state_ = VMState::PAUSED;
}

void VirtualMachine::Stop() {
    state_ = VMState::FINISHED;
}

void VirtualMachine::Reset() {
    state_ = VMState::READY;
    stack_top_ = 0;
    call_stack_.clear();
    current_function_ = nullptr;
    instruction_pointer_ = 0;
    ClearError();
}

void VirtualMachine::Push(const Value& value) {
    CheckStackOverflow();
    stack_[stack_top_++] = value;
}

Value VirtualMachine::Pop() {
    CheckStackUnderflow(1);
    return stack_[--stack_top_];
}

Value VirtualMachine::Peek(Size offset) const {
    CheckStackUnderflow(offset + 1);
    return stack_[stack_top_ - 1 - offset];
}

void VirtualMachine::ExecuteArithmetic(OpCode opcode) {
    if (opcode == OpCode::NEG) {
        Value operand = Pop();
        if (operand.IsInt()) {
            Push(Value(-operand.AsInt()));
        } else if (operand.IsFloat()) {
            Push(Value(-operand.AsFloat()));
        } else {
            RuntimeError("Invalid operand for negation");
        }
        return;
    }
    
    Value right = Pop();
    Value left = Pop();
    
    // Simple arithmetic implementation
    if (left.IsNumber() && right.IsNumber()) {
        Float64 l = left.ToNumber();
        Float64 r = right.ToNumber();
        
        switch (opcode) {
            case OpCode::ADD:
                Push(Value(l + r));
                break;
            case OpCode::SUB:
                Push(Value(l - r));
                break;
            case OpCode::MUL:
                Push(Value(l * r));
                break;
            case OpCode::DIV:
                if (r == 0.0) {
                    RuntimeError("Division by zero");
                } else {
                    Push(Value(l / r));
                }
                break;
            case OpCode::MOD:
                if (r == 0.0) {
                    RuntimeError("Modulo by zero");
                } else {
                    Push(Value(std::fmod(l, r)));
                }
                break;
            case OpCode::POW:
                Push(Value(std::pow(l, r)));
                break;
            default:
                RuntimeError("Unknown arithmetic operation");
                break;
        }
    } else {
        RuntimeError("Invalid operands for arithmetic operation");
    }
}

void VirtualMachine::ExecuteComparison(OpCode opcode) {
    Value right = Pop();
    Value left = Pop();
    
    bool result = false;
    
    switch (opcode) {
        case OpCode::EQ:
            result = left == right;
            break;
        case OpCode::NE:
            result = left != right;
            break;
        case OpCode::LT:
            result = left < right;
            break;
        case OpCode::LE:
            result = left <= right;
            break;
        case OpCode::GT:
            result = left > right;
            break;
        case OpCode::GE:
            result = left >= right;
            break;
        default:
            RuntimeError("Unknown comparison operation");
            return;
    }
    
    Push(Value(result));
}

void VirtualMachine::ExecuteLogical(OpCode opcode) {
    switch (opcode) {
        case OpCode::LOGICAL_NOT: {
            Value operand = Pop();
            Push(Value(!operand.ToBool()));
            break;
        }
        case OpCode::LOGICAL_AND: {
            Value right = Pop();
            Value left = Pop();
            Push(Value(left.ToBool() && right.ToBool()));
            break;
        }
        case OpCode::LOGICAL_OR: {
            Value right = Pop();
            Value left = Pop();
            Push(Value(left.ToBool() || right.ToBool()));
            break;
        }
        default:
            RuntimeError("Unknown logical operation");
            break;
    }
}

void VirtualMachine::ExecuteReturn() {
    Value return_value = IsStackEmpty() ? Value() : Pop();
    
    if (!call_stack_.empty()) {
        CallFrame frame = call_stack_.back();
        call_stack_.pop_back();
        
        current_function_ = frame.function;
        instruction_pointer_ = frame.instruction_pointer;
        
        // Restore stack
        stack_top_ = frame.base_pointer;
    } else {
        state_ = VMState::FINISHED;
    }
    
    Push(return_value);
}

void VirtualMachine::CheckStackOverflow() {
    if (stack_top_ >= stack_.size()) {
        if (stack_.size() < options_.max_stack_size) {
            GrowStack();
        } else {
            RuntimeError("Stack overflow");
        }
    }
}

void VirtualMachine::CheckStackUnderflow(Size required) {
    if (stack_top_ < required) {
        RuntimeError("Stack underflow");
    }
}

void VirtualMachine::GrowStack() {
    Size new_size = std::min(stack_.size() * 2, options_.max_stack_size);
    stack_.resize(new_size);
}

void VirtualMachine::RuntimeError(const String& message) {
    SetError(message);
    state_ = VMState::ERROR;
}

void VirtualMachine::SetError(const String& error) {
    last_error_ = error;
    error_stack_.push_back(error);
}

void VirtualMachine::ClearError() {
    last_error_.clear();
    error_stack_.clear();
}

void VirtualMachine::RegisterBuiltinNatives() {
    RegisterNative("print", NativePrint);
    RegisterNative("type", NativeType);
    RegisterNative("len", NativeLen);
}

void VirtualMachine::RegisterNative(const String& name, std::function<Value(const Vector<Value>&)> func) {
    natives_[name] = func;
}

Value VirtualMachine::CallNative(const String& name, const Vector<Value>& args) {
    auto it = natives_.find(name);
    if (it != natives_.end()) {
        return it->second(args);
    }
    RuntimeError("Unknown native function: " + name);
    return Value();
}

Value VirtualMachine::NativePrint(const Vector<Value>& args) {
    for (const auto& arg : args) {
        std::cout << arg.ToString() << " ";
    }
    std::cout << std::endl;
    return Value();
}

Value VirtualMachine::NativeType(const Vector<Value>& args) {
    if (args.empty()) {
        return Value("nil");
    }
    
    switch (args[0].GetType()) {
        case ValueType::NIL: return Value("nil");
        case ValueType::BOOLEAN: return Value("boolean");
        case ValueType::INTEGER: return Value("integer");
        case ValueType::FLOAT: return Value("float");
        case ValueType::STRING: return Value("string");
        case ValueType::ARRAY: return Value("array");
        case ValueType::OBJECT: return Value("object");
        case ValueType::FUNCTION: return Value("function");
        case ValueType::NATIVE_FUNCTION: return Value("native_function");
    }
    return Value("unknown");
}

Value VirtualMachine::NativeLen(const Vector<Value>& args) {
    if (args.empty()) {
        return Value(0);
    }
    
    const Value& arg = args[0];
    if (arg.IsString()) {
        return Value(static_cast<Int64>(arg.AsString().length()));
    } else if (arg.IsArray()) {
        return Value(static_cast<Int64>(arg.AsArray()->GetSize()));
    }
    
    return Value(0);
}

void VirtualMachine::TraceInstruction(const IRInstruction& instruction) {
    std::cout << "[TRACE] " << instruction.ToString() << std::endl;
}

String VirtualMachine::FormatStackTrace() const {
    return "Stack trace not implemented";
}

namespace VMFactory {

UniquePtr<VirtualMachine> Create(const VMOptions& options) {
    return make_unique<VirtualMachine>(options);
}

UniquePtr<VirtualMachine> CreateDebug() {
    VMOptions options;
    options.debug_mode = true;
    options.trace_execution = true;
    options.enable_jit = false;
    return make_unique<VirtualMachine>(options);
}

UniquePtr<VirtualMachine> CreateRelease() {
    VMOptions options;
    options.debug_mode = false;
    options.trace_execution = false;
    options.enable_jit = true;
    options.execution_mode = ExecutionMode::JIT;
    return make_unique<VirtualMachine>(options);
}

} // namespace VMFactory

} // namespace cj