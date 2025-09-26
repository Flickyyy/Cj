/**
 * @file ssa.h
 * @brief Static Single Assignment (SSA) form implementation for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_IR_SSA_H
#define CJ_IR_SSA_H

#include "../common.h"
#include "instruction.h"
#include <map>
#include <set>

namespace cj {

/**
 * @brief SSA Variable representation
 */
class SSAVariable {
private:
    String name_;
    Size version_;
    BasicBlock* defining_block_;

public:
    SSAVariable() : name_(""), version_(0), defining_block_(nullptr) {}
    
    SSAVariable(const String& name, Size version, BasicBlock* block = nullptr)
        : name_(name), version_(version), defining_block_(block) {}
    
    const String& GetName() const { return name_; }
    Size GetVersion() const { return version_; }
    String GetSSAName() const { return name_ + "_" + std::to_string(version_); }
    BasicBlock* GetDefiningBlock() const { return defining_block_; }
    
    void SetDefiningBlock(BasicBlock* block) { defining_block_ = block; }
    
    bool operator==(const SSAVariable& other) const {
        return name_ == other.name_ && version_ == other.version_;
    }
    
    bool operator<(const SSAVariable& other) const {
        if (name_ != other.name_) return name_ < other.name_;
        return version_ < other.version_;
    }
};

/**
 * @brief Phi node for SSA form
 */
class PhiNode {
private:
    SSAVariable result_;
    Vector<std::pair<SSAVariable, BasicBlock*>> operands_;

public:
    PhiNode(const SSAVariable& result) : result_(result) {}
    
    void AddOperand(const SSAVariable& var, BasicBlock* block) {
        operands_.emplace_back(var, block);
    }
    
    const SSAVariable& GetResult() const { return result_; }
    const Vector<std::pair<SSAVariable, BasicBlock*>>& GetOperands() const { return operands_; }
    
    String ToString() const {
        String result = result_.GetSSAName() + " = phi(";
        for (size_t i = 0; i < operands_.size(); ++i) {
            if (i > 0) result += ", ";
            result += operands_[i].first.GetSSAName() + ":" + operands_[i].second->GetLabel();
        }
        result += ")";
        return result;
    }
};

/**
 * @brief SSA-enhanced Basic Block
 */
class SSABasicBlock : public BasicBlock {
private:
    Vector<PhiNode> phi_nodes_;
    std::set<BasicBlock*> dominance_frontier_;
    BasicBlock* immediate_dominator_;
    std::set<BasicBlock*> dominated_blocks_;

public:
    explicit SSABasicBlock(const String& label = "") 
        : BasicBlock(label), immediate_dominator_(nullptr) {}
    
    // Phi node management
    void AddPhiNode(const PhiNode& phi) { phi_nodes_.push_back(phi); }
    const Vector<PhiNode>& GetPhiNodes() const { return phi_nodes_; }
    Vector<PhiNode>& GetPhiNodes() { return phi_nodes_; }
    
    // Dominance information
    void SetImmediateDominator(BasicBlock* dom) { immediate_dominator_ = dom; }
    BasicBlock* GetImmediateDominator() const { return immediate_dominator_; }
    
    void AddDominatedBlock(BasicBlock* block) { dominated_blocks_.insert(block); }
    const std::set<BasicBlock*>& GetDominatedBlocks() const { return dominated_blocks_; }
    
    void AddToDominanceFrontier(BasicBlock* block) { dominance_frontier_.insert(block); }
    const std::set<BasicBlock*>& GetDominanceFrontier() const { return dominance_frontier_; }
    
    virtual String ToString() const override {
        String result = BasicBlock::ToString();
        
        // Add phi nodes at the beginning
        if (!phi_nodes_.empty()) {
            String phi_section = GetLabel() + " (phi nodes):\n";
            for (const auto& phi : phi_nodes_) {
                phi_section += "    " + phi.ToString() + "\n";
            }
            result = phi_section + result;
        }
        
        return result;
    }
};

/**
 * @brief SSA Construction and Analysis
 */
class SSABuilder {
private:
    IRFunction* function_;
    HashMap<String, Size> variable_versions_;
    HashMap<BasicBlock*, HashMap<String, SSAVariable>> block_variables_;
    HashMap<BasicBlock*, std::set<BasicBlock*>> dominance_tree_;
    HashMap<BasicBlock*, std::set<BasicBlock*>> dominance_frontiers_;
    
    // Analysis methods
    void ComputeDominanceTree();
    void ComputeDominanceFrontiers();
    void PlacePhiNodes();
    void RenameVariables();
    void RenameVariablesInBlock(BasicBlock* block, HashMap<String, SSAVariable>& current_vars);
    
    // Helper methods
    bool Dominates(BasicBlock* dominator, BasicBlock* block);
    std::set<BasicBlock*> GetVariableDefinitionBlocks(const String& variable);

public:
    explicit SSABuilder(IRFunction* function) : function_(function) {}
    
    /**
     * @brief Convert function to SSA form
     */
    void ConstructSSA();
    
    /**
     * @brief Convert function from SSA form back to normal form
     */
    void DeconstructSSA();
    
    /**
     * @brief Validate SSA form properties
     */
    bool ValidateSSA() const;
    
    /**
     * @brief Get variable version for a block
     */
    const SSAVariable* GetVariable(BasicBlock* block, const String& name) const;
    
    /**
     * @brief Print SSA information for debugging
     */
    void PrintSSAInfo() const;
};

/**
 * @brief SSA-based optimization passes
 */
namespace SSAOptimizations {
    
    /**
     * @brief Constant propagation in SSA form
     */
    bool ConstantPropagation(IRFunction* function);
    
    /**
     * @brief Dead code elimination in SSA form
     */
    bool DeadCodeElimination(IRFunction* function);
    
    /**
     * @brief Copy propagation in SSA form
     */
    bool CopyPropagation(IRFunction* function);
    
    /**
     * @brief Sparse conditional constant propagation
     */
    bool SparseConditionalConstantPropagation(IRFunction* function);
    
    /**
     * @brief Global value numbering
     */
    bool GlobalValueNumbering(IRFunction* function);
}

/**
 * @brief SSA Utilities
 */
namespace SSAUtils {
    
    /**
     * @brief Check if instruction defines a variable
     */
    bool DefinesVariable(const IRInstruction& instr, String& variable_name);
    
    /**
     * @brief Check if instruction uses a variable
     */
    bool UsesVariable(const IRInstruction& instr, Vector<String>& variable_names);
    
    /**
     * @brief Convert basic block to SSA basic block
     */
    UniquePtr<SSABasicBlock> ConvertToSSABlock(BasicBlock* block);
    
    /**
     * @brief Find all variables in function
     */
    std::set<String> FindAllVariables(IRFunction* function);
}

} // namespace cj

#endif // CJ_IR_SSA_H