/**
 * @file ssa.h
 * @brief Static Single Assignment (SSA) form representation for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_MIDDLEND_SSA_H
#define CJ_MIDDLEND_SSA_H

#include "../common.h"
#include "../types/type_system.h"
#include <unordered_set>

namespace cj {
namespace middlend {

// Forward declarations
class SSAValue;
class SSAInstruction;
class SSABasicBlock;
class SSAFunction;
class SSAModule;

/**
 * @brief SSA value representing a register or constant
 */
class SSAValue {
public:
    enum class Kind {
        REGISTER,
        CONSTANT,
        PARAMETER,
        UNDEF
    };

private:
    Kind kind_;
    SharedPtr<Type> type_;
    String name_;
    UInt32 id_;
    Vector<SSAInstruction*> uses_;
    SSAInstruction* definition_;
    
    static UInt32 next_id_;

public:
    SSAValue(Kind kind, SharedPtr<Type> type, const String& name = "");
    virtual ~SSAValue() = default;
    
    // Accessors
    Kind GetKind() const { return kind_; }
    SharedPtr<Type> GetType() const { return type_; }
    const String& GetName() const { return name_; }
    UInt32 GetId() const { return id_; }
    
    // Use-def chains
    const Vector<SSAInstruction*>& GetUses() const { return uses_; }
    SSAInstruction* GetDefinition() const { return definition_; }
    
    void AddUse(SSAInstruction* instruction);
    void RemoveUse(SSAInstruction* instruction);
    void SetDefinition(SSAInstruction* instruction);
    
    // Replace all uses of this value with another value
    void ReplaceAllUsesWith(SSAValue* new_value);
    
    // Utility methods
    bool HasUses() const { return !uses_.empty(); }
    Size GetUseCount() const { return uses_.size(); }
    bool IsConstant() const { return kind_ == Kind::CONSTANT; }
    bool IsParameter() const { return kind_ == Kind::PARAMETER; }
    bool IsRegister() const { return kind_ == Kind::REGISTER; }
    
    String ToString() const;
};

/**
 * @brief SSA constant value
 */
class SSAConstant : public SSAValue {
private:
    union ConstantValue {
        bool bool_val;
        Int64 int_val;
        Float64 float_val;
        const char* string_val;
        void* ptr_val;
        
        ConstantValue() : ptr_val(nullptr) {}
    } value_;

public:
    // Constructors for different constant types
    SSAConstant(bool value, SharedPtr<Type> type);
    SSAConstant(Int64 value, SharedPtr<Type> type);
    SSAConstant(Float64 value, SharedPtr<Type> type);
    SSAConstant(const String& value, SharedPtr<Type> type);
    SSAConstant(void* value, SharedPtr<Type> type); // For null pointers
    
    // Getters
    bool GetBoolValue() const;
    Int64 GetIntValue() const;
    Float64 GetFloatValue() const;
    const char* GetStringValue() const;
    void* GetPointerValue() const;
    
    String ToString() const override;
};

/**
 * @brief SSA instruction opcodes
 */
enum class SSAOpcode {
    // Arithmetic
    ADD, SUB, MUL, DIV, MOD,
    FADD, FSUB, FMUL, FDIV,
    
    // Bitwise
    AND, OR, XOR, NOT, SHL, SHR,
    
    // Comparison
    EQ, NE, LT, LE, GT, GE,
    FEQ, FNE, FLT, FLE, FGT, FGE,
    
    // Memory
    LOAD, STORE, ALLOCA,
    GEP, // Get Element Pointer
    
    // Control flow
    BR, COND_BR, SWITCH, RET,
    PHI, // Phi node for SSA form
    
    // Function calls
    CALL, TAIL_CALL,
    
    // Type conversions
    TRUNC, ZEXT, SEXT, // Integer conversions
    FPTRUNC, FPEXT,    // Float conversions
    FPTOSI, FPTOUI,    // Float to integer
    SITOFP, UITOFP,    // Integer to float
    BITCAST, PTRTOINT, INTTOPTR,
    
    // Aggregate operations
    EXTRACT_VALUE, INSERT_VALUE,
    EXTRACT_ELEMENT, INSERT_ELEMENT,
    
    // Special
    UNREACHABLE, FENCE
};

/**
 * @brief SSA instruction
 */
class SSAInstruction {
protected:
    SSAOpcode opcode_;
    SharedPtr<Type> type_;
    Vector<SSAValue*> operands_;
    SSAValue* result_;
    SSABasicBlock* parent_;
    String debug_info_;

public:
    SSAInstruction(SSAOpcode opcode, SharedPtr<Type> type, Vector<SSAValue*> operands = {});
    virtual ~SSAInstruction() = default;
    
    // Accessors
    SSAOpcode GetOpcode() const { return opcode_; }
    SharedPtr<Type> GetType() const { return type_; }
    const Vector<SSAValue*>& GetOperands() const { return operands_; }
    SSAValue* GetOperand(Size index) const;
    Size GetOperandCount() const { return operands_.size(); }
    
    SSAValue* GetResult() const { return result_; }
    SSABasicBlock* GetParent() const { return parent_; }
    
    // Modification
    void SetOperand(Size index, SSAValue* value);
    void AddOperand(SSAValue* value);
    void RemoveOperand(Size index);
    void SetParent(SSABasicBlock* parent) { parent_ = parent; }
    
    // Type checking
    bool HasResult() const { return result_ != nullptr; }
    bool IsTerminator() const;
    bool IsBranch() const;
    bool IsCall() const;
    bool IsMemoryOperation() const;
    
    // Utility
    String ToString() const;
    virtual String GetMnemonic() const;
    
    // Debug info
    void SetDebugInfo(const String& info) { debug_info_ = info; }
    const String& GetDebugInfo() const { return debug_info_; }
};

/**
 * @brief Phi instruction for SSA form
 */
class SSAPhiInstruction : public SSAInstruction {
private:
    Vector<std::pair<SSAValue*, SSABasicBlock*>> incoming_values_;

public:
    SSAPhiInstruction(SharedPtr<Type> type);
    
    void AddIncoming(SSAValue* value, SSABasicBlock* block);
    void RemoveIncoming(SSABasicBlock* block);
    
    Size GetIncomingCount() const { return incoming_values_.size(); }
    SSAValue* GetIncomingValue(Size index) const;
    SSABasicBlock* GetIncomingBlock(Size index) const;
    
    // Find incoming value for a specific block
    SSAValue* GetIncomingValueForBlock(SSABasicBlock* block) const;
    
    String ToString() const override;
};

/**
 * @brief SSA basic block
 */
class SSABasicBlock {
private:
    String name_;
    UInt32 id_;
    Vector<UniquePtr<SSAInstruction>> instructions_;
    Vector<SSABasicBlock*> predecessors_;
    Vector<SSABasicBlock*> successors_;
    SSAFunction* parent_;
    
    static UInt32 next_id_;

public:
    explicit SSABasicBlock(const String& name = "", SSAFunction* parent = nullptr);
    ~SSABasicBlock() = default;
    
    // Accessors
    const String& GetName() const { return name_; }
    UInt32 GetId() const { return id_; }
    SSAFunction* GetParent() const { return parent_; }
    void SetParent(SSAFunction* parent) { parent_ = parent; }
    
    // Instructions
    const Vector<UniquePtr<SSAInstruction>>& GetInstructions() const { return instructions_; }
    Size GetInstructionCount() const { return instructions_.size(); }
    SSAInstruction* GetInstruction(Size index) const;
    SSAInstruction* GetTerminator() const;
    
    void AddInstruction(UniquePtr<SSAInstruction> instruction);
    void InsertInstruction(Size index, UniquePtr<SSAInstruction> instruction);
    void RemoveInstruction(Size index);
    void RemoveInstruction(SSAInstruction* instruction);
    
    // Control flow graph
    const Vector<SSABasicBlock*>& GetPredecessors() const { return predecessors_; }
    const Vector<SSABasicBlock*>& GetSuccessors() const { return successors_; }
    
    void AddPredecessor(SSABasicBlock* block);
    void AddSuccessor(SSABasicBlock* block);
    void RemovePredecessor(SSABasicBlock* block);
    void RemoveSuccessor(SSABasicBlock* block);
    
    // Utility
    bool Empty() const { return instructions_.empty(); }
    bool IsTerminated() const;
    
    String ToString() const;
};

/**
 * @brief SSA function
 */
class SSAFunction {
private:
    String name_;
    SharedPtr<FunctionType> function_type_;
    Vector<UniquePtr<SSABasicBlock>> basic_blocks_;
    Vector<SSAValue*> parameters_;
    SSAModule* parent_;
    bool is_declaration_;

public:
    SSAFunction(const String& name, SharedPtr<FunctionType> type, SSAModule* parent = nullptr);
    ~SSAFunction() = default;
    
    // Accessors
    const String& GetName() const { return name_; }
    SharedPtr<FunctionType> GetFunctionType() const { return function_type_; }
    SSAModule* GetParent() const { return parent_; }
    void SetParent(SSAModule* parent) { parent_ = parent; }
    
    bool IsDeclaration() const { return is_declaration_; }
    void SetDeclaration(bool is_decl) { is_declaration_ = is_decl; }
    
    // Parameters
    const Vector<SSAValue*>& GetParameters() const { return parameters_; }
    Size GetParameterCount() const { return parameters_.size(); }
    SSAValue* GetParameter(Size index) const;
    
    void AddParameter(SSAValue* param);
    
    // Basic blocks
    const Vector<UniquePtr<SSABasicBlock>>& GetBasicBlocks() const { return basic_blocks_; }
    Size GetBasicBlockCount() const { return basic_blocks_.size(); }
    SSABasicBlock* GetBasicBlock(Size index) const;
    SSABasicBlock* GetEntryBlock() const;
    
    SSABasicBlock* CreateBasicBlock(const String& name = "");
    void AddBasicBlock(UniquePtr<SSABasicBlock> block);
    void RemoveBasicBlock(SSABasicBlock* block);
    
    // Utility
    bool Empty() const { return basic_blocks_.empty(); }
    
    // Validation
    bool Verify() const;
    
    String ToString() const;
};

/**
 * @brief SSA module (compilation unit)
 */
class SSAModule {
private:
    String name_;
    Vector<UniquePtr<SSAFunction>> functions_;
    Vector<SSAValue*> global_variables_;
    HashMap<String, SSAFunction*> function_map_;

public:
    explicit SSAModule(const String& name);
    ~SSAModule() = default;
    
    // Accessors
    const String& GetName() const { return name_; }
    
    // Functions
    const Vector<UniquePtr<SSAFunction>>& GetFunctions() const { return functions_; }
    Size GetFunctionCount() const { return functions_.size(); }
    SSAFunction* GetFunction(Size index) const;
    SSAFunction* GetFunction(const String& name) const;
    
    SSAFunction* CreateFunction(const String& name, SharedPtr<FunctionType> type);
    void AddFunction(UniquePtr<SSAFunction> function);
    void RemoveFunction(const String& name);
    
    // Global variables
    const Vector<SSAValue*>& GetGlobalVariables() const { return global_variables_; }
    void AddGlobalVariable(SSAValue* global);
    
    // Utility
    bool HasFunction(const String& name) const;
    
    // Validation
    bool Verify() const;
    
    String ToString() const;
};

/**
 * @brief SSA builder for constructing SSA form
 */
class SSABuilder {
private:
    SSAFunction* current_function_;
    SSABasicBlock* current_block_;
    SSAModule* module_;
    HashMap<String, SSAValue*> named_values_;

public:
    explicit SSABuilder(SSAModule* module);
    
    // Context management
    void SetCurrentFunction(SSAFunction* function);
    void SetCurrentBlock(SSABasicBlock* block);
    
    SSAFunction* GetCurrentFunction() const { return current_function_; }
    SSABasicBlock* GetCurrentBlock() const { return current_block_; }
    SSAModule* GetModule() const { return module_; }
    
    // Value management
    void SetNamedValue(const String& name, SSAValue* value);
    SSAValue* GetNamedValue(const String& name) const;
    
    // Instruction building
    SSAValue* CreateAdd(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    SSAValue* CreateSub(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    SSAValue* CreateMul(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    SSAValue* CreateDiv(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    
    SSAValue* CreateFAdd(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    SSAValue* CreateFSub(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    SSAValue* CreateFMul(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    SSAValue* CreateFDiv(SSAValue* lhs, SSAValue* rhs, const String& name = "");
    
    SSAValue* CreateICmp(SSAOpcode cmp, SSAValue* lhs, SSAValue* rhs, const String& name = "");
    SSAValue* CreateFCmp(SSAOpcode cmp, SSAValue* lhs, SSAValue* rhs, const String& name = "");
    
    SSAValue* CreateLoad(SharedPtr<Type> type, SSAValue* ptr, const String& name = "");
    void CreateStore(SSAValue* value, SSAValue* ptr);
    SSAValue* CreateAlloca(SharedPtr<Type> type, const String& name = "");
    
    void CreateBr(SSABasicBlock* dest);
    void CreateCondBr(SSAValue* condition, SSABasicBlock* true_dest, SSABasicBlock* false_dest);
    void CreateRet(SSAValue* value = nullptr);
    
    SSAValue* CreateCall(SSAFunction* function, const Vector<SSAValue*>& args, const String& name = "");
    
    SSAPhiInstruction* CreatePhi(SharedPtr<Type> type, const String& name = "");
    
    // Constants
    SSAConstant* GetBoolConstant(bool value);
    SSAConstant* GetIntConstant(Int64 value, SharedPtr<Type> type);
    SSAConstant* GetFloatConstant(Float64 value, SharedPtr<Type> type);
    SSAConstant* GetStringConstant(const String& value);
    SSAConstant* GetNullConstant(SharedPtr<Type> type);
    
private:
    void InsertInstruction(UniquePtr<SSAInstruction> instruction, const String& name = "");
};

/**
 * @brief SSA optimization passes
 */
namespace passes {

/**
 * @brief Dead code elimination pass
 */
class DeadCodeElimination {
public:
    static bool Run(SSAFunction* function);
private:
    static void MarkLiveInstructions(SSAFunction* function, std::unordered_set<SSAInstruction*>& live);
    static void RemoveDeadInstructions(SSAFunction* function, const std::unordered_set<SSAInstruction*>& live);
};

/**
 * @brief Constant folding pass
 */
class ConstantFolding {
public:
    static bool Run(SSAFunction* function);
private:
    static SSAValue* FoldBinaryOp(SSAOpcode opcode, SSAConstant* lhs, SSAConstant* rhs);
    static SSAValue* FoldUnaryOp(SSAOpcode opcode, SSAConstant* operand);
};

/**
 * @brief Common subexpression elimination
 */
class CommonSubexpressionElimination {
public:
    static bool Run(SSAFunction* function);
};

/**
 * @brief Simple loop invariant code motion
 */
class LoopInvariantCodeMotion {
public:
    static bool Run(SSAFunction* function);
};

} // namespace passes

} // namespace middlend
} // namespace cj

#endif // CJ_MIDDLEND_SSA_H