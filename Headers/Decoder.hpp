#ifndef DECODER_HPP
#define DECODER_HPP

#include <iostream>
#include <string>

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

class Decoder {
  private:
    operation opr;
    unsigned int value_1;
    unsigned int value_2;
    unsigned int value_3;
    type judge_the_type(unsigned int code) {
        unsigned int opcode = (code << 25) >> 25;
        if (opcode == 51) {
            return type::R;
        } else if (opcode == 19) {
            unsigned int Funct3 = (code << 17) >> 29;
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
    Decoder() : value_1(0), value_2(0), value_3(0) {}
    Decoder(unsigned int code) {
        type oper_type = judge_the_type(code);
        if (oper_type == type::R) {
            unsigned int funct7 = code >> 25;
            unsigned int rs2 = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int rd = (code << 20) >> 27;
            if (funct3 == 0) {
                if (funct7 == 0) {
                    opr = operation::Add;
                } else {
                    opr = operation::Sub;
                }
            } else if (funct3 == 7) {
                opr = operation::And;
            } else if (funct3 == 6) {
                opr = operation::Or;
            } else if (funct3 == 4) {
                opr = operation::Xor;
            } else if (funct3 == 1) {
                opr = operation::Sll;
            } else if (funct3 == 5) {
                opr = operation::Srl;
            } else if (funct3 == 5) {
                if (funct7 == 0) {
                    opr = operation::Srl;
                } else {
                    opr = operation::Sra;
                }
            } else if (funct3 == 2) {
                opr = operation::Slt;
            } else if (funct3 == 3) {
                opr = operation::Sltu;
            }
            value_1 = rd;
            value_2 = rs1;
            value_3 = rs2;
        } else if (oper_type == type::I) {
            unsigned int opcode = (code << 25) >> 25;
            unsigned int imm = code >> 20;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int rd = (code << 20) >> 27;
            if (funct3 == 0) {
                if (opcode == 19) {
                    opr = operation::Addi;
                } else if (opcode == 3) {
                    opr = operation::Lb;
                } else if (opcode == 103) {
                    opr = operation::Jalr;
                } else {
                    if (imm == 0) {
                        opr = operation::Ebreak;
                    } else {
                        opr = operation::Ecall;
                    }
                }
            } else if (funct3 == 7) {
                opr = operation::Andi;
            } else if (funct3 == 6) {
                opr = operation::Ori;
            } else if (funct3 == 4) {
                if (opcode == 19) {
                    opr = operation::Xori;
                } else {
                    opr = operation::Lbu;
                }
            } else if (funct3 == 2) {
                if (opcode == 19) {
                    opr = operation::Slti;
                } else {
                    opr = operation::Lw;
                }
            } else if (funct3 == 3) {
                opr = operation::Sltiu;
            } else if (funct3 == 1) {
                opr = operation::Lh;
            } else if (funct3 == 5) {
                opr = operation::Lhu;
            }
            value_1 = rd;
            value_2 = rs1;
            value_3 = imm;
        } else if (oper_type == type::I_) {
            unsigned int funct7 = code >> 25;
            unsigned int imm = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int rd = (code << 20) >> 27;
            if (funct3 == 1) {
                opr = operation::Slli;
            } else {
                if (funct7 == 0) {
                    opr = operation::Srli;
                } else {
                    opr = operation::Srai;
                }
            }
            value_1 = rd;
            value_2 = rs1;
            value_3 = imm;
        } else if (oper_type == type::S) {
            unsigned int imm1 = code >> 25;
            unsigned int rs2 = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int imm2 = (code << 20) >> 27;
            unsigned int imm = (imm1 << 5) + imm2;
            if (funct3 == 0) {
                opr = operation::Sb;
            } else if (funct3 == 1) {
                opr = operation::Sh;
            } else if (funct3 == 2) {
                opr = operation::Sw;
            }
            value_1 = rs2;
            value_2 = imm;
            value_3 = rs1;
        } else if (oper_type == type::B) {
            unsigned int imm1 = code >> 31;
            unsigned int imm2 = (code << 1) >> 26;
            unsigned int rs2 = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int imm3 = (code << 20) >> 28;
            unsigned int imm4 = (code << 24) >> 31;
            unsigned int imm = (imm1 << 12) + (imm4 << 11) + (imm2 << 5) + (imm3 << 1);
            if (funct3 == 0) {
                opr = operation::Beq;
            } else if (funct3 == 5) {
                opr = operation::Bge;
            } else if (funct3 == 7) {
                opr = operation::Bgeu;
            } else if (funct3 == 4) {
                opr = operation::Blt;
            } else if (funct3 == 1) {
                opr = operation::Bne;
            }
            value_1 = rs1;
            value_2 = rs2;
            value_3 = imm;
        } else if (oper_type == type::J) {
            unsigned int imm1 = code >> 31;
            unsigned int imm2 = (code << 1) >> 21;
            unsigned int imm3 = (code << 11) >> 31;
            unsigned int imm4 = (code << 12) >> 24;
            unsigned int rd = (code << 20) >> 27;
            unsigned int imm = (imm1 << 20) + (imm4 << 12) + (imm3 << 11) + (imm2 << 1);
            opr = operation::Jal;
            value_1 = rd;
            value_2 = imm;
        } else if (oper_type == type::U) {
            unsigned int imm1 = code >> 12;
            unsigned int rd = (code << 20) >> 27;
            unsigned int imm = imm1 << 12;
            unsigned int opcode = (code << 25) >> 25;
            if (opcode == 23) {
                opr = operation::Auipc;
            } else {
                opr = operation::Lui;
            }
            value_1 = rd;
            value_2 = imm;
        }
    }
    void initialize(unsigned int code) {
        type oper_type = judge_the_type(code);
        if (oper_type == type::R) {
            unsigned int funct7 = code >> 25;
            unsigned int rs2 = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int rd = (code << 20) >> 27;
            if (funct3 == 0) {
                if (funct7 == 0) {
                    opr = operation::Add;
                } else {
                    opr = operation::Sub;
                }
            } else if (funct3 == 7) {
                opr = operation::And;
            } else if (funct3 == 6) {
                opr = operation::Or;
            } else if (funct3 == 4) {
                opr = operation::Xor;
            } else if (funct3 == 1) {
                opr = operation::Sll;
            } else if (funct3 == 5) {
                opr = operation::Srl;
            } else if (funct3 == 5) {
                if (funct7 == 0) {
                    opr = operation::Srl;
                } else {
                    opr = operation::Sra;
                }
            } else if (funct3 == 2) {
                opr = operation::Slt;
            } else if (funct3 == 3) {
                opr = operation::Sltu;
            }
            value_1 = rd;
            value_2 = rs1;
            value_3 = rs2;
        } else if (oper_type == type::I) {
            unsigned int opcode = (code << 25) >> 25;
            unsigned int imm = code >> 20;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int rd = (code << 20) >> 27;
            if (funct3 == 0) {
                if (opcode == 19) {
                    opr = operation::Addi;
                } else if (opcode == 3) {
                    opr = operation::Lb;
                } else if (opcode == 103) {
                    opr = operation::Jalr;
                } else {
                    if (imm == 0) {
                        opr = operation::Ebreak;
                    } else {
                        opr = operation::Ecall;
                    }
                }
            } else if (funct3 == 7) {
                opr = operation::Andi;
            } else if (funct3 == 6) {
                opr = operation::Ori;
            } else if (funct3 == 4) {
                if (opcode == 19) {
                    opr = operation::Xori;
                } else {
                    opr = operation::Lbu;
                }
            } else if (funct3 == 2) {
                if (opcode == 19) {
                    opr = operation::Slti;
                } else {
                    opr = operation::Lw;
                }
            } else if (funct3 == 3) {
                opr = operation::Sltiu;
            } else if (funct3 == 1) {
                opr = operation::Lh;
            } else if (funct3 == 5) {
                opr = operation::Lhu;
            }
            value_1 = rd;
            value_2 = rs1;
            value_3 = imm;
        } else if (oper_type == type::I_) {
            unsigned int funct7 = code >> 25;
            unsigned int imm = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int rd = (code << 20) >> 27;
            if (funct3 == 1) {
                opr = operation::Slli;
            } else {
                if (funct7 == 0) {
                    opr = operation::Srli;
                } else {
                    opr = operation::Srai;
                }
            }
            value_1 = rd;
            value_2 = rs1;
            value_3 = imm;
        } else if (oper_type == type::S) {
            unsigned int imm1 = code >> 25;
            unsigned int rs2 = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int imm2 = (code << 20) >> 27;
            unsigned int imm = (imm1 << 5) + imm2;
            if (funct3 == 0) {
                opr = operation::Sb;
            } else if (funct3 == 1) {
                opr = operation::Sh;
            } else if (funct3 == 2) {
                opr = operation::Sw;
            }
            value_1 = rs2;
            value_2 = imm;
            value_3 = rs1;
        } else if (oper_type == type::B) {
            unsigned int imm1 = code >> 31;
            unsigned int imm2 = (code << 1) >> 26;
            unsigned int rs2 = (code << 7) >> 27;
            unsigned int rs1 = (code << 12) >> 27;
            unsigned int funct3 = (code << 17) >> 29;
            unsigned int imm3 = (code << 20) >> 28;
            unsigned int imm4 = (code << 24) >> 31;
            unsigned int imm = (imm1 << 12) + (imm4 << 11) + (imm2 << 5) + (imm3 << 1);
            if (funct3 == 0) {
                opr = operation::Beq;
            } else if (funct3 == 5) {
                opr = operation::Bge;
            } else if (funct3 == 7) {
                opr = operation::Bgeu;
            } else if (funct3 == 4) {
                opr = operation::Blt;
            } else if (funct3 == 1) {
                opr = operation::Bne;
            }
            value_1 = rs1;
            value_2 = rs2;
            value_3 = imm;
        } else if (oper_type == type::J) {
            unsigned int imm1 = code >> 31;
            unsigned int imm2 = (code << 1) >> 21;
            unsigned int imm3 = (code << 11) >> 31;
            unsigned int imm4 = (code << 12) >> 24;
            unsigned int rd = (code << 20) >> 27;
            unsigned int imm = (imm1 << 20) + (imm4 << 12) + (imm3 << 11) + (imm2 << 1);
            opr = operation::Jal;
            value_1 = rd;
            value_2 = imm;
        } else if (oper_type == type::U) {
            unsigned int imm1 = code >> 12;
            unsigned int rd = (code << 20) >> 27;
            unsigned int imm = imm1 << 12;
            unsigned int opcode = (code << 25) >> 25;
            if (opcode == 23) {
                opr = operation::Auipc;
            } else {
                opr = operation::Lui;
            }
            value_1 = rd;
            value_2 = imm;
        }
    }
    operation get_operation() {
        return opr;
    }
    unsigned int get_value1() {
        return value_1;
    }
    unsigned int get_value2() {
        return value_2;
    }
    unsigned int get_value3() {
        return value_3;
    }
};


#endif      // DECODER_HPP