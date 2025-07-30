#ifndef ALU_HPP
#define ALU_HPP

#include "Decoder.hpp"
#include "RS.hpp"
#include <iostream>

namespace Granthy {
struct ALU_input {
    operation opr;
    uint32_t value1 = 0;
    uint32_t value2 = 0;
    uint32_t imm = 0;
    int index;
    bool busy = false;
};
struct ALU_output {
    uint32_t output;
    int index;
    bool busy = false;
};
class ALU {
public:
    ALU_input input;
    ALU_output output;
public:
    ALU() = default;
    void do_operation() {
        if (output.busy || !input.busy) {
            return;
        }
        output.index = input.index;
        input.busy = false;
        output.busy = true;
        if (input.opr == operation::Add) {
            output.output = input.value1 + input.value2;
        } else if (input.opr == operation::Sub) {
            output.output = input.value1 - input.value2;
        } else if (input.opr == operation::And) {
            output.output = input.value1 & input.value2;
        } else if (input.opr == operation::Or) {
            output.output = input.value1 | input.value2;
        } else if (input.opr == operation::Xor) {
            output.output = input.value1 ^ input.value2;
        } else if (input.opr == operation::Sll) {
            output.output = input.value1 << input.value2;
        } else if (input.opr == operation::Srl) {
            output.output = input.value1 >> input.value2;
        } else if (input.opr == operation::Sra) {
            output.output = static_cast<uint32_t>(static_cast<int32_t>(input.value1) >> input.value2);
        } else if (input.opr == operation::Slt) {
            output.output = (static_cast<int32_t>(input.value1) < static_cast<int32_t>(input.value2));
        } else if (input.opr == operation::Sltu) {
            output.output = (input.value1 < input.value2);
        } else if (input.opr == operation::Addi) {
            output.output = input.value1 + input.imm;
        } else if (input.opr == operation::Andi) {
            output.output = input.value1 & input.imm;
        } else if (input.opr == operation::Ori) {
            output.output = input.value1 | input.imm;
        } else if (input.opr == operation::Xori) {
            output.output = input.value1 ^ input.imm;
        } else if (input.opr == operation::Slli) {
            output.output = input.value1 << input.imm;
        } else if (input.opr == operation::Srli) {
            output.output = input.value1 >> input.imm;
        } else if (input.opr == operation::Srai) {
            output.output = static_cast<uint32_t>(static_cast<int32_t>(input.value1) >> input.imm);
        } else if (input.opr == operation::Slti) {
            output.output = (static_cast<int32_t>(input.value1) < static_cast<int32_t>(input.imm));
        } else if (input.opr == operation::Sltiu) {
            output.output = (input.value1 < input.imm);
        } else if (input.opr == operation::Beq) {
            output.output = input.value1 == input.value2;
        } else if (input.opr == operation::Bge) {
            output.output = static_cast<int32_t>(input.value1) >= static_cast<int32_t>(input.value2);
        } else if (input.opr == operation::Bgeu) {
            output.output = input.value1 >= input.value2;
        } else if (input.opr == operation::Blt) {
            output.output = static_cast<int32_t>(input.value1) < static_cast<int32_t>(input.value2);
        } else if (input.opr == operation::Bne) {
            output.output = (input.value1) != (input.value2);
        } else if (input.opr == operation::Bltu) {
            output.output = input.value1 < input.value2;
        } else if (input.opr == operation::Jalr) {
            output.output = input.value1 + input.imm;
        } else if (input.opr == operation::Jal) {
            output.output = input.value1 + input.imm;
        } else if (input.opr == operation::Auipc) {
            output.output = input.value1 + input.imm;
        } else if (input.opr == operation::Lui) {
            output.output = input.imm;
        }
    }
    void set_input(const ALU_input& input_) {input = input_;}
    ALU_output get_output() {return output;}
    bool set_input_from_RS_output(RS_entry output_) {
        if (input.busy) {
            return false;
        } 
        ALU_input new_input;
        new_input.opr = output_.opr;
        new_input.value1 = output_.vj;
        new_input.value2 = output_.vk;
        new_input.imm = output_.imm;
        new_input.index = output_.index;
        new_input.busy = true;
        input = new_input;
        return true;
    }
    void flush() {
        input.busy = output.busy = false;
    }
};
}

#endif      // ALU_HPP
