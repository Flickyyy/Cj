/**
 * @file instruction.cpp
 * @brief Stub implementation of IR instructions for CJ language
 */

#include "cj/ir/instruction.h"

namespace cj {

String IRInstruction::ToString() const {
    String result = IRUtils::OpCodeToString(opcode_);
    if (!operands_.empty()) {
        result += " ";
        for (size_t i = 0; i < operands_.size(); ++i) {
            if (i > 0) result += ", ";
            result += operands_[i].ToString();
        }
    }
    return result;
}

Size IRInstruction::GetSize() const {
    return IRUtils::GetInstructionSize(opcode_);
}

bool IRInstruction::IsJump() const {
    switch (opcode_) {
        case OpCode::JUMP:
        case OpCode::JUMP_IF_TRUE:
        case OpCode::JUMP_IF_FALSE:
        case OpCode::JUMP_IF_NIL:
            return true;
        default:
            return false;
    }
}

bool IRInstruction::IsCall() const {
    return opcode_ == OpCode::CALL;
}

bool IRInstruction::ModifiesStack() const {
    return IRUtils::GetStackEffect(opcode_, operands_.size()) != 0;
}

BasicBlock* IRFunction::CreateBlock(const String& label) {
    String block_label = label.empty() ? "block_" + std::to_string(blocks_.size()) : label;
    auto block = make_unique<BasicBlock>(block_label);
    BasicBlock* block_ptr = block.get();
    
    blocks_.push_back(std::move(block));
    label_map_[block_label] = block_ptr;
    
    return block_ptr;
}

BasicBlock* IRFunction::GetBlock(const String& label) {
    auto it = label_map_.find(label);
    return it != label_map_.end() ? it->second : nullptr;
}

String IRFunction::ToString() const {
    String result = "function " + name_ + "(";
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (i > 0) result += ", ";
        result += parameters_[i];
    }
    result += ") {\n";
    
    for (const auto& block : blocks_) {
        result += block->ToString();
    }
    
    result += "}\n";
    return result;
}

void IRFunction::Optimize() {
    // Stub implementation - basic optimizations would go here
}

String BasicBlock::ToString() const {
    String result = label_ + ":\n";
    for (const auto& instruction : instructions_) {
        result += "    " + instruction.ToString() + "\n";
    }
    return result;
}

namespace IRUtils {

String OpCodeToString(OpCode opcode) {
    switch (opcode) {
        case OpCode::NOP: return "NOP";
        case OpCode::POP: return "POP";
        case OpCode::DUP: return "DUP";
        case OpCode::SWAP: return "SWAP";
        case OpCode::LOAD_CONST: return "LOAD_CONST";
        case OpCode::LOAD_NIL: return "LOAD_NIL";
        case OpCode::LOAD_TRUE: return "LOAD_TRUE";
        case OpCode::LOAD_FALSE: return "LOAD_FALSE";
        case OpCode::LOAD_LOCAL: return "LOAD_LOCAL";
        case OpCode::STORE_LOCAL: return "STORE_LOCAL";
        case OpCode::LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OpCode::STORE_GLOBAL: return "STORE_GLOBAL";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::MOD: return "MOD";
        case OpCode::POW: return "POW";
        case OpCode::NEG: return "NEG";
        case OpCode::EQ: return "EQ";
        case OpCode::NE: return "NE";
        case OpCode::LT: return "LT";
        case OpCode::LE: return "LE";
        case OpCode::GT: return "GT";
        case OpCode::GE: return "GE";
        case OpCode::LOGICAL_AND: return "LOGICAL_AND";
        case OpCode::LOGICAL_OR: return "LOGICAL_OR";
        case OpCode::LOGICAL_NOT: return "LOGICAL_NOT";
        case OpCode::JUMP: return "JUMP";
        case OpCode::JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::JUMP_IF_NIL: return "JUMP_IF_NIL";
        case OpCode::CALL: return "CALL";
        case OpCode::RETURN: return "RETURN";
        case OpCode::HALT: return "HALT";
        default: return "UNKNOWN";
    }
}

bool IsArithmeticOp(OpCode opcode) {
    switch (opcode) {
        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        case OpCode::MOD:
        case OpCode::POW:
        case OpCode::NEG:
            return true;
        default:
            return false;
    }
}

bool IsComparisonOp(OpCode opcode) {
    switch (opcode) {
        case OpCode::EQ:
        case OpCode::NE:
        case OpCode::LT:
        case OpCode::LE:
        case OpCode::GT:
        case OpCode::GE:
            return true;
        default:
            return false;
    }
}

bool IsLogicalOp(OpCode opcode) {
    switch (opcode) {
        case OpCode::LOGICAL_AND:
        case OpCode::LOGICAL_OR:
        case OpCode::LOGICAL_NOT:
            return true;
        default:
            return false;
    }
}

bool IsBitwiseOp(OpCode opcode) {
    switch (opcode) {
        case OpCode::BIT_AND:
        case OpCode::BIT_OR:
        case OpCode::BIT_XOR:
        case OpCode::BIT_NOT:
        case OpCode::BIT_LSHIFT:
        case OpCode::BIT_RSHIFT:
            return true;
        default:
            return false;
    }
}

Size GetInstructionSize(OpCode opcode) {
    // Simple implementation - all instructions are 1 byte + operands
    return 1;
}

int GetStackEffect(OpCode opcode, Size operand_count) {
    switch (opcode) {
        case OpCode::POP: return -1;
        case OpCode::DUP: return 1;
        case OpCode::SWAP: return 0;
        case OpCode::LOAD_CONST:
        case OpCode::LOAD_NIL:
        case OpCode::LOAD_TRUE:
        case OpCode::LOAD_FALSE:
        case OpCode::LOAD_LOCAL:
        case OpCode::LOAD_GLOBAL:
            return 1;
        case OpCode::STORE_LOCAL:
        case OpCode::STORE_GLOBAL:
            return -1;
        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        case OpCode::MOD:
        case OpCode::POW:
        case OpCode::EQ:
        case OpCode::NE:
        case OpCode::LT:
        case OpCode::LE:
        case OpCode::GT:
        case OpCode::GE:
        case OpCode::LOGICAL_AND:
        case OpCode::LOGICAL_OR:
            return -1; // Two operands -> one result
        case OpCode::NEG:
        case OpCode::LOGICAL_NOT:
            return 0; // One operand -> one result
        case OpCode::CALL:
            return -(static_cast<int>(operand_count)) + 1; // Pop args, push result
        case OpCode::RETURN:
            return -1;
        default:
            return 0;
    }
}

} // namespace IRUtils

} // namespace cj