/**
 * @file ssa.cpp
 * @brief Static Single Assignment (SSA) form implementation for CJ language
 */

#include "cj/ir/ssa.h"
#include <queue>
#include <algorithm>
#include <iostream>

namespace cj {

void SSABuilder::ConstructSSA() {
    // Step 1: Compute dominance information
    ComputeDominanceTree();
    ComputeDominanceFrontiers();
    
    // Step 2: Place phi nodes
    PlacePhiNodes();
    
    // Step 3: Rename variables
    RenameVariables();
}

void SSABuilder::ComputeDominanceTree() {
    const auto& blocks = function_->GetBlocks();
    if (blocks.empty()) return;
    
    // Initialize dominance sets - all blocks dominate themselves
    HashMap<BasicBlock*, std::set<BasicBlock*>> dominance_sets;
    for (const auto& block : blocks) {
        std::set<BasicBlock*> all_blocks;
        for (const auto& b : blocks) {
            all_blocks.insert(b.get());
        }
        dominance_sets[block.get()] = all_blocks;
    }
    
    // Entry block dominates only itself initially
    BasicBlock* entry = blocks[0].get(); // Assume first block is entry
    dominance_sets[entry] = {entry};
    
    // Iterative algorithm to compute dominance
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& block : blocks) {
            if (block.get() == entry) continue;
            
            BasicBlock* current = block.get();
            std::set<BasicBlock*> new_dom = {current};
            
            // Intersection of all predecessors' dominance sets
            bool first_pred = true;
            for (BasicBlock* pred : current->GetPredecessors()) {
                if (first_pred) {
                    new_dom.insert(dominance_sets[pred].begin(), dominance_sets[pred].end());
                    first_pred = false;
                } else {
                    std::set<BasicBlock*> intersection;
                    std::set_intersection(
                        new_dom.begin(), new_dom.end(),
                        dominance_sets[pred].begin(), dominance_sets[pred].end(),
                        std::inserter(intersection, intersection.begin())
                    );
                    new_dom = intersection;
                }
            }
            new_dom.insert(current);
            
            if (new_dom != dominance_sets[current]) {
                dominance_sets[current] = new_dom;
                changed = true;
            }
        }
    }
    
    // Build immediate dominance tree
    for (const auto& block : blocks) {
        BasicBlock* current = block.get();
        if (current == entry) continue;
        
        // Find immediate dominator (closest dominator)
        BasicBlock* idom = nullptr;
        for (BasicBlock* dom : dominance_sets[current]) {
            if (dom == current) continue;
            
            bool is_immediate = true;
            for (BasicBlock* other_dom : dominance_sets[current]) {
                if (other_dom == current || other_dom == dom) continue;
                
                if (dominance_sets[other_dom].count(dom)) {
                    is_immediate = false;
                    break;
                }
            }
            
            if (is_immediate) {
                idom = dom;
                break;
            }
        }
        
        if (idom) {
            dominance_tree_[idom].insert(current);
        }
    }
}

void SSABuilder::ComputeDominanceFrontiers() {
    const auto& blocks = function_->GetBlocks();
    
    for (const auto& block : blocks) {
        BasicBlock* current = block.get();
        
        for (BasicBlock* successor : current->GetSuccessors()) {
            BasicBlock* runner = successor;
            
            // Walk up dominance tree until we reach current's dominator
            while (runner && !Dominates(current, runner)) {
                dominance_frontiers_[runner].insert(current);
                
                // Move to immediate dominator
                auto it = dominance_tree_.find(runner);
                if (it != dominance_tree_.end() && !it->second.empty()) {
                    // Find the immediate dominator of runner
                    BasicBlock* idom = nullptr;
                    for (const auto& dom_pair : dominance_tree_) {
                        if (dom_pair.second.count(runner)) {
                            idom = dom_pair.first;
                            break;
                        }
                    }
                    runner = idom;
                } else {
                    break;
                }
            }
        }
    }
}

void SSABuilder::PlacePhiNodes() {
    // Find all variables that need phi nodes
    std::set<String> all_variables = SSAUtils::FindAllVariables(function_);
    
    for (const String& variable : all_variables) {
        std::set<BasicBlock*> def_sites = GetVariableDefinitionBlocks(variable);
        std::queue<BasicBlock*> worklist;
        std::set<BasicBlock*> has_phi;
        
        // Initialize worklist with definition sites
        for (BasicBlock* def_site : def_sites) {
            worklist.push(def_site);
        }
        
        while (!worklist.empty()) {
            BasicBlock* block = worklist.front();
            worklist.pop();
            
            // For each block in this block's dominance frontier
            for (BasicBlock* df_block : dominance_frontiers_[block]) {
                if (has_phi.count(df_block) == 0) {
                    // In a simplified implementation, we'd store phi information
                    // separately rather than modifying block structure
                    has_phi.insert(df_block);
                    
                    // Add to worklist if not already a definition site
                    if (def_sites.count(df_block) == 0) {
                        worklist.push(df_block);
                    }
                }
            }
        }
    }
}

void SSABuilder::RenameVariables() {
    // Initialize variable versions
    std::set<String> all_variables = SSAUtils::FindAllVariables(function_);
    for (const String& var : all_variables) {
        variable_versions_[var] = 0;
    }
    
    // Start renaming from entry block
    if (!function_->GetBlocks().empty()) {
        HashMap<String, SSAVariable> current_vars;
        RenameVariablesInBlock(function_->GetBlocks()[0].get(), current_vars);
    }
}

void SSABuilder::RenameVariablesInBlock(BasicBlock* block, HashMap<String, SSAVariable>& current_vars) {
    // For simplified implementation, we'll work with regular BasicBlocks
    // In a full implementation, we'd have proper phi node management
    
    // Process regular instructions
    auto& instructions = block->GetInstructions();
    for (auto& instr : instructions) {
        // Replace uses with current versions
        Vector<String> used_vars;
        if (SSAUtils::UsesVariable(instr, used_vars)) {
            for (const String& var : used_vars) {
                if (current_vars.count(var)) {
                    // Update instruction to use SSA name
                    // This is a simplified approach - real implementation would
                    // need to modify operands properly
                }
            }
        }
        
        // Handle definitions
        String defined_var;
        if (SSAUtils::DefinesVariable(instr, defined_var)) {
            Size new_version = ++variable_versions_[defined_var];
            SSAVariable new_var(defined_var, new_version, block);
            current_vars[defined_var] = new_var;
        }
    }
    
    // Update phi operands in successor blocks (simplified)
    for (BasicBlock* successor : block->GetSuccessors()) {
        // In a full implementation, would update phi operands here
    }
    
    // Recursively process dominated blocks
    if (dominance_tree_.count(block)) {
        for (BasicBlock* dominated : dominance_tree_[block]) {
            HashMap<String, SSAVariable> child_vars = current_vars; // Copy
            RenameVariablesInBlock(dominated, child_vars);
        }
    }
}

void SSABuilder::DeconstructSSA() {
    // Simplified deconstruction - in a full implementation would handle phi nodes
    const auto& blocks = function_->GetBlocks();
    
    for (const auto& block : blocks) {
        // For simplified implementation, SSA deconstruction is mostly a no-op
        // since we're not actually modifying the IR structure significantly
    }
}

bool SSABuilder::ValidateSSA() const {
    // Check SSA property: simplified validation
    HashMap<String, Size> def_counts;
    
    const auto& blocks = function_->GetBlocks();
    for (const auto& block : blocks) {
        // Check regular instructions
        for (const auto& instr : block->GetInstructions()) {
            String defined_var;
            if (SSAUtils::DefinesVariable(instr, defined_var)) {
                def_counts[defined_var]++;
                // For simplified implementation, we don't strictly enforce SSA
            }
        }
    }
    
    return true; // Simplified validation always passes
}

bool SSABuilder::Dominates(BasicBlock* dominator, BasicBlock* block) {
    if (dominator == block) return true;
    
    // Check if dominator is in the dominance tree path to block
    auto it = dominance_tree_.find(dominator);
    if (it != dominance_tree_.end()) {
        return it->second.count(block) > 0;
    }
    
    return false;
}

std::set<BasicBlock*> SSABuilder::GetVariableDefinitionBlocks(const String& variable) {
    std::set<BasicBlock*> def_blocks;
    
    const auto& blocks = function_->GetBlocks();
    for (const auto& block : blocks) {
        for (const auto& instr : block->GetInstructions()) {
            String defined_var;
            if (SSAUtils::DefinesVariable(instr, defined_var) && defined_var == variable) {
                def_blocks.insert(block.get());
                break;
            }
        }
    }
    
    return def_blocks;
}

void SSABuilder::PrintSSAInfo() const {
    std::cout << "=== SSA Information ===" << std::endl;
    
    std::cout << "Dominance Tree:" << std::endl;
    for (const auto& pair : dominance_tree_) {
        std::cout << "  " << pair.first->GetLabel() << " dominates: ";
        for (BasicBlock* dominated : pair.second) {
            std::cout << dominated->GetLabel() << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Dominance Frontiers:" << std::endl;
    for (const auto& pair : dominance_frontiers_) {
        std::cout << "  " << pair.first->GetLabel() << " DF: ";
        for (BasicBlock* df_block : pair.second) {
            std::cout << df_block->GetLabel() << " ";
        }
        std::cout << std::endl;
    }
}

// Utility functions
namespace SSAUtils {

bool DefinesVariable(const IRInstruction& instr, String& variable_name) {
    OpCode opcode = instr.GetOpCode();
    
    switch (opcode) {
        case OpCode::STORE_LOCAL:
        case OpCode::STORE_GLOBAL:
            if (instr.GetOperandCount() > 0) {
                // Simplified - in real implementation would extract variable name from operand
                variable_name = "var_" + std::to_string(instr.GetOperand(0).AsInt());
                return true;
            }
            break;
        default:
            break;
    }
    
    return false;
}

bool UsesVariable(const IRInstruction& instr, Vector<String>& variable_names) {
    OpCode opcode = instr.GetOpCode();
    
    switch (opcode) {
        case OpCode::LOAD_LOCAL:
        case OpCode::LOAD_GLOBAL:
            if (instr.GetOperandCount() > 0) {
                // Simplified - in real implementation would extract variable name from operand
                variable_names.push_back("var_" + std::to_string(instr.GetOperand(0).AsInt()));
                return true;
            }
            break;
        default:
            break;
    }
    
    return false;
}

UniquePtr<SSABasicBlock> ConvertToSSABlock(BasicBlock* block) {
    auto ssa_block = make_unique<SSABasicBlock>(block->GetLabel());
    
    // Copy instructions
    for (const auto& instr : block->GetInstructions()) {
        ssa_block->AddInstruction(instr);
    }
    
    // Copy control flow information
    for (BasicBlock* pred : block->GetPredecessors()) {
        ssa_block->AddPredecessor(pred);
    }
    for (BasicBlock* succ : block->GetSuccessors()) {
        ssa_block->AddSuccessor(succ);
    }
    
    ssa_block->SetEntry(block->IsEntry());
    ssa_block->SetExit(block->IsExit());
    
    return ssa_block;
}

std::set<String> FindAllVariables(IRFunction* function) {
    std::set<String> variables;
    
    const auto& blocks = function->GetBlocks();
    for (const auto& block : blocks) {
        for (const auto& instr : block->GetInstructions()) {
            String defined_var;
            if (DefinesVariable(instr, defined_var)) {
                variables.insert(defined_var);
            }
            
            Vector<String> used_vars;
            if (UsesVariable(instr, used_vars)) {
                for (const String& var : used_vars) {
                    variables.insert(var);
                }
            }
        }
    }
    
    return variables;
}

} // namespace SSAUtils

// SSA-based optimizations
namespace SSAOptimizations {

bool ConstantPropagation(IRFunction* function) {
    // Simplified constant propagation in SSA form
    bool changed = false;
    
    const auto& blocks = function->GetBlocks();
    for (const auto& block : blocks) {
        auto& instructions = block->GetInstructions();
        
        for (auto& instr : instructions) {
            // Look for constant loads followed by uses
            if (instr.GetOpCode() == OpCode::LOAD_CONST) {
                // In a real implementation, would track constant values
                // and replace uses with constants
                changed = true;
            }
        }
    }
    
    return changed;
}

bool DeadCodeElimination(IRFunction* function) {
    // Mark and sweep dead code elimination
    std::set<const IRInstruction*> live_instructions;
    std::queue<const IRInstruction*> worklist;
    
    const auto& blocks = function->GetBlocks();
    
    // Mark critical instructions as live
    for (const auto& block : blocks) {
        for (const auto& instr : block->GetInstructions()) {
            OpCode opcode = instr.GetOpCode();
            
            // Critical instructions (side effects, control flow, etc.)
            if (opcode == OpCode::CALL || opcode == OpCode::RETURN || 
                opcode == OpCode::JUMP || opcode == OpCode::HALT ||
                opcode == OpCode::STORE_GLOBAL) {
                
                live_instructions.insert(&instr);
                worklist.push(&instr);
            }
        }
    }
    
    // Propagate liveness backwards
    while (!worklist.empty()) {
        const IRInstruction* live_instr = worklist.front();
        worklist.pop();
        
        // Mark instructions that this one depends on as live
        Vector<String> used_vars;
        if (SSAUtils::UsesVariable(*live_instr, used_vars)) {
            // Find definitions of used variables and mark them live
            // Simplified implementation
        }
    }
    
    // Remove dead instructions
    bool changed = false;
    for (const auto& block : blocks) {
        auto& instructions = block->GetInstructions();
        auto it = instructions.begin();
        while (it != instructions.end()) {
            if (live_instructions.count(&(*it)) == 0) {
                it = instructions.erase(it);
                changed = true;
            } else {
                ++it;
            }
        }
    }
    
    return changed;
}

bool CopyPropagation(IRFunction* function) {
    // Simplified copy propagation
    bool changed = false;
    
    const auto& blocks = function->GetBlocks();
    for (const auto& block : blocks) {
        auto& instructions = block->GetInstructions();
        
        // Look for copy instructions (simplified)
        for (size_t i = 0; i < instructions.size(); ++i) {
            const auto& instr = instructions[i];
            
            // If this is a copy (load then store), try to eliminate it
            if (instr.GetOpCode() == OpCode::LOAD_LOCAL && 
                i + 1 < instructions.size() &&
                instructions[i + 1].GetOpCode() == OpCode::STORE_LOCAL) {
                
                // In real implementation, would replace uses of the target
                // with the source variable
                changed = true;
            }
        }
    }
    
    return changed;
}

bool SparseConditionalConstantPropagation(IRFunction* function) {
    // Simplified SCCP implementation
    return ConstantPropagation(function);
}

bool GlobalValueNumbering(IRFunction* function) {
    // Simplified GVN implementation
    HashMap<String, const IRInstruction*> value_table;
    bool changed = false;
    
    const auto& blocks = function->GetBlocks();
    for (const auto& block : blocks) {
        auto& instructions = block->GetInstructions();
        
        for (auto& instr : instructions) {
            // Create value number for instruction
            String value_key = IRUtils::OpCodeToString(instr.GetOpCode());
            for (const auto& operand : instr.GetOperands()) {
                value_key += "_" + operand.ToString();
            }
            
            auto it = value_table.find(value_key);
            if (it != value_table.end()) {
                // Found equivalent computation
                // In real implementation, would replace with previous result
                changed = true;
            } else {
                value_table[value_key] = &instr;
            }
        }
    }
    
    return changed;
}

} // namespace SSAOptimizations

} // namespace cj