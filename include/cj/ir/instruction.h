/**
 * @file instruction.h
 * @brief Intermediate Representation instruction definitions for CJ
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_IR_INSTRUCTION_H
#define CJ_IR_INSTRUCTION_H

#include "../common.h"
#include "../types/value.h"

namespace cj {

/**
 * @brief IR instruction operation codes
 */
enum class OpCode : UInt8 {
    // Stack operations
    NOP,            // No operation
    POP,            // Pop top value from stack
    DUP,            // Duplicate top value
    SWAP,           // Swap top two values
    
    // Constants
    LOAD_CONST,     // Load constant value
    LOAD_NIL,       // Load nil value
    LOAD_TRUE,      // Load true
    LOAD_FALSE,     // Load false
    
    // Variables
    LOAD_LOCAL,     // Load local variable
    STORE_LOCAL,    // Store to local variable
    LOAD_GLOBAL,    // Load global variable
    STORE_GLOBAL,   // Store to global variable
    
    // Arithmetic operations
    ADD,            // Addition
    SUB,            // Subtraction
    MUL,            // Multiplication
    DIV,            // Division
    MOD,            // Modulo
    POW,            // Power
    NEG,            // Negation
    
    // Bitwise operations
    BIT_AND,        // Bitwise AND
    BIT_OR,         // Bitwise OR
    BIT_XOR,        // Bitwise XOR
    BIT_NOT,        // Bitwise NOT
    BIT_LSHIFT,     // Left shift
    BIT_RSHIFT,     // Right shift
    
    // Comparison operations
    EQ,             // Equal
    NE,             // Not equal
    LT,             // Less than
    LE,             // Less than or equal
    GT,             // Greater than
    GE,             // Greater than or equal
    
    // Logical operations
    LOGICAL_AND,    // Logical AND
    LOGICAL_OR,     // Logical OR
    LOGICAL_NOT,    // Logical NOT
    
    // Type operations
    TYPEOF,         // Get type of value
    IS_NIL,         // Check if nil
    IS_BOOL,        // Check if boolean
    IS_INT,         // Check if integer
    IS_FLOAT,       // Check if float
    IS_STRING,      // Check if string
    IS_ARRAY,       // Check if array
    IS_OBJECT,      // Check if object
    IS_FUNCTION,    // Check if function
    
    // Control flow
    JUMP,           // Unconditional jump
    JUMP_IF_TRUE,   // Jump if top of stack is true
    JUMP_IF_FALSE,  // Jump if top of stack is false
    JUMP_IF_NIL,    // Jump if top of stack is nil
    
    // Function operations
    CALL,           // Call function
    RETURN,         // Return from function
    MAKE_CLOSURE,   // Create closure
    
    // Array operations
    MAKE_ARRAY,     // Create array
    ARRAY_GET,      // Get array element
    ARRAY_SET,      // Set array element
    ARRAY_LEN,      // Get array length
    ARRAY_PUSH,     // Push to array
    ARRAY_POP,      // Pop from array
    
    // Object operations
    MAKE_OBJECT,    // Create object
    OBJECT_GET,     // Get object property
    OBJECT_SET,     // Set object property
    OBJECT_HAS,     // Check if object has property
    OBJECT_DEL,     // Delete object property
    
    // String operations
    STRING_CONCAT,  // Concatenate strings
    STRING_LEN,     // Get string length
    STRING_GET,     // Get string character
    
    // Class operations
    MAKE_CLASS,     // Create class
    INHERIT,        // Set up inheritance
    GET_SUPER,      // Get super class
    
    // Exception handling
    TRY,            // Try block
    CATCH,          // Catch block
    FINALLY,        // Finally block
    THROW,          // Throw exception
    
    // Debugging
    PRINT,          // Print value (debug)
    BREAKPOINT,     // Debugger breakpoint
    
    // Memory management
    GC_COLLECT,     // Force garbage collection
    
    // Halt
    HALT            // Stop execution
};

/**
 * @brief IR instruction representation
 */
class IRInstruction {
private:
    OpCode opcode_;
    Vector<Value> operands_;
    Size line_number_;

public:
    /**
     * @brief Construct instruction with opcode
     */
    explicit IRInstruction(OpCode opcode, Size line = 0)
        : opcode_(opcode), line_number_(line) {}
    
    /**
     * @brief Construct instruction with operands
     */
    IRInstruction(OpCode opcode, const Vector<Value>& operands, Size line = 0)
        : opcode_(opcode), operands_(operands), line_number_(line) {}
    
    /**
     * @brief Construct instruction with single operand
     */
    IRInstruction(OpCode opcode, const Value& operand, Size line = 0)
        : opcode_(opcode), operands_{operand}, line_number_(line) {}
    
    /**
     * @brief Getters
     */
    OpCode GetOpCode() const { return opcode_; }
    const Vector<Value>& GetOperands() const { return operands_; }
    Size GetLineNumber() const { return line_number_; }
    Size GetOperandCount() const { return operands_.size(); }
    
    /**
     * @brief Get specific operand
     */
    const Value& GetOperand(Size index) const {
        CJ_ASSERT(index < operands_.size(), "Operand index out of bounds");
        return operands_[index];
    }
    
    /**
     * @brief Add operand
     */
    void AddOperand(const Value& operand) {
        operands_.push_back(operand);
    }
    
    /**
     * @brief Set line number
     */
    void SetLineNumber(Size line) { line_number_ = line; }
    
    /**
     * @brief String representation
     */
    String ToString() const;
    
    /**
     * @brief Get instruction size in bytes
     */
    Size GetSize() const;
    
    /**
     * @brief Check if instruction has operands
     */
    bool HasOperands() const { return !operands_.empty(); }
    
    /**
     * @brief Check if instruction is a jump
     */
    bool IsJump() const;
    
    /**
     * @brief Check if instruction is a call
     */
    bool IsCall() const;
    
    /**
     * @brief Check if instruction modifies the stack
     */
    bool ModifiesStack() const;
};

/**
 * @brief Basic block in IR
 */
class BasicBlock {
private:
    String label_;
    Vector<IRInstruction> instructions_;
    Vector<BasicBlock*> predecessors_;
    Vector<BasicBlock*> successors_;
    bool is_entry_;
    bool is_exit_;

public:
    explicit BasicBlock(const String& label = "")
        : label_(label), is_entry_(false), is_exit_(false) {}
    
    /**
     * @brief Label management
     */
    const String& GetLabel() const { return label_; }
    void SetLabel(const String& label) { label_ = label; }
    
    /**
     * @brief Instruction management
     */
    void AddInstruction(const IRInstruction& instruction) {
        instructions_.push_back(instruction);
    }
    
    void AddInstruction(IRInstruction&& instruction) {
        instructions_.push_back(std::move(instruction));
    }
    
    const Vector<IRInstruction>& GetInstructions() const { return instructions_; }
    Vector<IRInstruction>& GetInstructions() { return instructions_; }
    
    /**
     * @brief Control flow graph
     */
    void AddPredecessor(BasicBlock* block) { predecessors_.push_back(block); }
    void AddSuccessor(BasicBlock* block) { successors_.push_back(block); }
    
    const Vector<BasicBlock*>& GetPredecessors() const { return predecessors_; }
    const Vector<BasicBlock*>& GetSuccessors() const { return successors_; }
    
    /**
     * @brief Block properties
     */
    bool IsEntry() const { return is_entry_; }
    void SetEntry(bool entry) { is_entry_ = entry; }
    
    bool IsExit() const { return is_exit_; }
    void SetExit(bool exit) { is_exit_ = exit; }
    
    /**
     * @brief Utility methods
     */
    Size GetSize() const { return instructions_.size(); }
    bool Empty() const { return instructions_.empty(); }
    
    String ToString() const;
};

/**
 * @brief IR function representation
 */
class IRFunction {
private:
    String name_;
    Vector<String> parameters_;
    Vector<UniquePtr<BasicBlock>> blocks_;
    HashMap<String, BasicBlock*> label_map_;
    Size local_count_;
    Size max_stack_size_;

public:
    explicit IRFunction(const String& name = "")
        : name_(name), local_count_(0), max_stack_size_(0) {}
    
    /**
     * @brief Function properties
     */
    const String& GetName() const { return name_; }
    void SetName(const String& name) { name_ = name; }
    
    const Vector<String>& GetParameters() const { return parameters_; }
    void AddParameter(const String& param) { parameters_.push_back(param); }
    
    Size GetLocalCount() const { return local_count_; }
    void SetLocalCount(Size count) { local_count_ = count; }
    
    Size GetMaxStackSize() const { return max_stack_size_; }
    void SetMaxStackSize(Size size) { max_stack_size_ = size; }
    
    /**
     * @brief Basic block management
     */
    BasicBlock* CreateBlock(const String& label = "");
    BasicBlock* GetBlock(const String& label);
    const Vector<UniquePtr<BasicBlock>>& GetBlocks() const { return blocks_; }
    
    /**
     * @brief Utility methods
     */
    String ToString() const;
    void Optimize();
};

/**
 * @brief Utility functions for IR instructions
 */
namespace IRUtils {
    String OpCodeToString(OpCode opcode);
    bool IsArithmeticOp(OpCode opcode);
    bool IsComparisonOp(OpCode opcode);
    bool IsLogicalOp(OpCode opcode);
    bool IsBitwiseOp(OpCode opcode);
    Size GetInstructionSize(OpCode opcode);
    int GetStackEffect(OpCode opcode, Size operand_count = 0);
}

} // namespace cj

#endif // CJ_IR_INSTRUCTION_H