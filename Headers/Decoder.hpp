#ifndef DECODER_HPP
#define DECODER_HPP

#include <iostream>
#include <string>
#include <cstdint>

enum operation {
    Add, Sub, Addi,
    And, Andi, Or, Ori, Xor, Xori,
    Sll, Slli, Srl, Srli, Sra, Srai,
    Slt, Slti, Sltu, Sltiu,
    Lb, Lbu, Lh, Lhu, Lw,
    Sb, Sh, Sw,
    Beq, Bge, Bgeu, Blt, Bltu, Bne, Jal, Jalr,
    Auipc, Lui, Ebreak, Ecall,
    Mul,
};
enum type {
    R, I, I_, S, B, U, J,
};

struct decoder_input {
    bool busy = false;
    uint32_t code;
};

struct decoder_output {
    bool busy = false;
    operation opr;
    uint32_t value_1 = 0;
    uint32_t value_2 = 0;
    uint32_t value_3 = 0;
};

class Decoder {
public:
    decoder_input input;
    decoder_output output;
    type judge_the_type(uint32_t code) {
        uint32_t opcode = (code << 25) >> 25;
        if (opcode == 51) {
            return type::R;
        } else if (opcode == 19) {
            uint32_t Funct3 = (code << 17) >> 29;
            if (Funct3 == 1 || Funct3 == 5) {
                return type::I_;
            } else {
                return type::I;
            }
        } else if (opcode == 3 || opcode == 103 || opcode == 115) {
            return type::I;
        } else if (opcode == 35) {
            return type::S;
        } else if (opcode == 99) {
            return type::B;
        } else if (opcode == 111) {
            return type::J;
        } else if (opcode == 23 || opcode == 55) {
            return type::U;
        }
    }
public:
    Decoder() = default;
    void do_operation() {
        if (!output.busy && input.busy) {
            code = input.code;
            type oper_type = judge_the_type(code);
            if (oper_type == type::R) {
                uint32_t funct7 = code >> 25;
                uint32_t rs2 = (code << 7) >> 27;
                uint32_t rs1 = (code << 12) >> 27;
                uint32_t funct3 = (code << 17) >> 29;
                uint32_t rd = (code << 20) >> 27;
                if (funct3 == 0) {
                    if (funct7 == 0) {
                        output.opr = operation::Add;
                    } else {
                        output.opr = operation::Sub;
                    }
                } else if (funct3 == 7) {
                    output.opr = operation::And;
                } else if (funct3 == 6) {
                    output.opr = operation::Or;
                } else if (funct3 == 4) {
                    output.opr = operation::Xor;
                } else if (funct3 == 1) {
                    output.opr = operation::Sll;
                } else if (funct3 == 5) {
                    output.opr = operation::Srl;
                } else if (funct3 == 5) {
                    if (funct7 == 0) {
                        output.opr = operation::Srl;
                    } else {
                        output.opr = operation::Sra;
                    }
                } else if (funct3 == 2) {
                    output.opr = operation::Slt;
                } else if (funct3 == 3) {
                    output.opr = operation::Sltu;
                }
                output.value_1 = rd;
                output.value_2 = rs1;
                output.value_3 = rs2;
            } else if (oper_type == type::I) {
                uint32_t opcode = (code << 25) >> 25;
                uint32_t imm = code >> 20;
                uint32_t rs1 = (code << 12) >> 27;
                uint32_t funct3 = (code << 17) >> 29;
                uint32_t rd = (code << 20) >> 27;
                if (funct3 == 0) {
                    if (opcode == 19) {
                        output.opr = operation::Addi;
                    } else if (opcode == 3) {
                        output.opr = operation::Lb;
                    } else if (opcode == 103) {
                        output.opr = operation::Jalr;
                    } else {
                        if (imm == 0) {
                            output.opr = operation::Ebreak;
                        } else {
                            output.opr = operation::Ecall;
                        }
                    }
                } else if (funct3 == 7) {
                    output.opr = operation::Andi;
                } else if (funct3 == 6) {
                    output.opr = operation::Ori;
                } else if (funct3 == 4) {
                    if (opcode == 19) {
                        output.opr = operation::Xori;
                    } else {
                        output.opr = operation::Lbu;
                    }
                } else if (funct3 == 2) {
                    if (opcode == 19) {
                        output.opr = operation::Slti;
                    } else {
                        output.opr = operation::Lw;
                    }
                } else if (funct3 == 3) {
                    output.opr = operation::Sltiu;
                } else if (funct3 == 1) {
                    output.opr = operation::Lh;
                } else if (funct3 == 5) {
                    output.opr = operation::Lhu;
                }
                output.value_1 = rd;
                output.value_2 = rs1;
                output.value_3 = imm;
            } else if (oper_type == type::I_) {
                uint32_t funct7 = code >> 25;
                uint32_t imm = (code << 7) >> 27;
                uint32_t rs1 = (code << 12) >> 27;
                uint32_t funct3 = (code << 17) >> 29;
                uint32_t rd = (code << 20) >> 27;
                if (funct3 == 1) {
                    output.opr = operation::Slli;
                } else {
                    if (funct7 == 0) {
                        output.opr = operation::Srli;
                    } else {
                        output.opr = operation::Srai;
                    }
                }
                output.value_1 = rd;
                output.value_2 = rs1;
                output.value_3 = imm;
            } else if (oper_type == type::S) {
                uint32_t imm1 = code >> 25;
                uint32_t rs2 = (code << 7) >> 27;
                uint32_t rs1 = (code << 12) >> 27;
                uint32_t funct3 = (code << 17) >> 29;
                uint32_t imm2 = (code << 20) >> 27;
                uint32_t imm = (imm1 << 5) + imm2;
                if (funct3 == 0) {
                    output.opr = operation::Sb;
                } else if (funct3 == 1) {
                    output.opr = operation::Sh;
                } else if (funct3 == 2) {
                    output.opr = operation::Sw;
                }
                output.value_1 = rs2;
                output.value_3 = imm;
                output.value_2 = rs1;
            } else if (oper_type == type::B) {
                uint32_t imm1 = code >> 31;
                uint32_t imm2 = (code << 1) >> 26;
                uint32_t rs2 = (code << 7) >> 27;
                uint32_t rs1 = (code << 12) >> 27;
                uint32_t funct3 = (code << 17) >> 29;
                uint32_t imm3 = (code << 20) >> 28;
                uint32_t imm4 = (code << 24) >> 31;
                uint32_t imm = (imm1 << 12) + (imm4 << 11) + (imm2 << 5) + (imm3 << 1);
                if (funct3 == 0) {
                    output.opr = operation::Beq;
                } else if (funct3 == 5) {
                    output.opr = operation::Bge;
                } else if (funct3 == 7) {
                    output.opr = operation::Bgeu;
                } else if (funct3 == 4) {
                    output.opr = operation::Blt;
                } else if (funct3 == 1) {
                    output.opr = operation::Bne;
                }
                output.value_1 = rs1;
                output.value_2 = rs2;
                output.value_3 = imm;
            } else if (oper_type == type::J) {
                uint32_t imm1 = code >> 31;
                uint32_t imm2 = (code << 1) >> 21;
                uint32_t imm3 = (code << 11) >> 31;
                uint32_t imm4 = (code << 12) >> 24;
                uint32_t rd = (code << 20) >> 27;
                uint32_t imm = (imm1 << 20) + (imm4 << 12) + (imm3 << 11) + (imm2 << 1);
                output.opr = operation::Jal;
                output.value_1 = rd;
                output.value_2 = imm;
            } else if (oper_type == type::U) {
                uint32_t imm1 = code >> 12;
                uint32_t rd = (code << 20) >> 27;
                uint32_t imm = imm1 << 12;
                uint32_t opcode = (code << 25) >> 25;
                if (opcode == 23) {
                    output.opr = operation::Auipc;
                } else {
                    output.opr = operation::Lui;
                }
                output.value_1 = rd;
                output.value_2 = imm;
            }
            output.busy = true;
            input.busy = false;
        }
    }
    decoder_output get_output() {
        return output;
    }
    void set_input(uint32_t input_) {input = input_;}
    uint32_t get_value1() {
        return output.value_1;
    }
    uint32_t get_value2() {
        return output.value_2;
    }
    uint32_t get_value3() {
        return output.value_3;
    }
    void flush() {
        output.busy = false;
    }
};


#endif      // DECODER_HPP