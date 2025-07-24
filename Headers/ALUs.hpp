#ifndef ALU_HPP
#define ALU_HPP

#include "RegisterFile.hpp"
#include "Decoder.hpp"
#include <iostream>

class ALU {
private:
    Register reg;
    Decoder scanner;
public:
    ALU() = default;
    void operate(unsigned int code) {
        scanner.initialize(code);
        operation opr = scanner.get_operation();
        unsigned int value1 = scanner.get_value1();
        unsigned int value2 = scanner.get_value2();
        unsigned int value3 = scanner.get_value3();
        if (opr == operation::Add) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = rs1_value + rs2_value;
            reg.write_register(value1, value);
        } else if (opr == operation::Sub){
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = rs1_value - rs2_value;
            reg.write_register(value1, value);
        } else if (opr == operation::And) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = rs1_value & rs2_value;
            reg.write_register(value1, value);
        } else if (opr == operation::Or) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = rs1_value | rs2_value;
            reg.write_register(value1, value);
        } else if (opr == operation::Xor) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = rs1_value ^ rs2_value;
            reg.write_register(value1, value);
        } else if (opr == operation::Sll) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = rs1_value << rs2_value;
            reg.write_register(value1, value);
        } else if (opr == operation::Srl) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = rs1_value >> rs2_value;
            reg.write_register(value1, value);
        } else if (opr == operation::Sra) {
            int rs1_value = reg.read_register(value2);
            unsigned int rs2_value = reg.read_register(value3);
            unsigned int value = static_cast<unsigned int>(static_cast<int>(rs1_value) >> rs2_value);
            reg.write_register(value1, value);
        } else if (opr == operation::Slt) {
            int rs1_value = static_cast<int>(reg.read_register(value2));
            int rs2_value = static_cast<int>(reg.read_register(value3));
            int value = (rs1_value < rs2_value) ? 1 : 0;
            reg.write_register(value1, static_cast<unsigned int>(value));
        } else if (opr == operation::Sltu) {
            int rs1_value = (reg.read_register(value2));
            int rs2_value = (reg.read_register(value3));
            unsigned int value = (rs1_value < rs2_value) ? 1 : 0;
            reg.write_register(value1, value);
        } else if (opr == operation::Addi) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int imm = value3;
            unsigned int value = rs1_value + imm;
            reg.write_register(value1, value);
        } else if (opr == operation::Andi) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int imm = value3;
            unsigned int value = rs1_value & imm;
            reg.write_register(value1, value);
        } else if (opr == operation::Ori) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int imm = value3;
            unsigned int value = rs1_value | imm;
            reg.write_register(value1, value);
        } else if (opr == operation::Xori) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int imm = value3;
            unsigned int value = rs1_value ^ imm;
            reg.write_register(value1, value);
        } else if (opr == operation::Slli) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int imm = value3;
            unsigned int value = rs1_value << imm;
            reg.write_register(value1, value);
        } else if (opr == operation::Srli) {
            unsigned int rs1_value = reg.read_register(value2);
            unsigned int imm = value3;
            unsigned int value = rs1_value >> imm;
            reg.write_register(value1, value);
        } else if (opr == operation::Srai) {
            int rs1_value = static_cast<int>(reg.read_register(value2));
            int imm = static_cast<int>(value3);
            unsigned int value = static_cast<unsigned int>(rs1_value >> imm);
            reg.write_register(value1, value);
        } else if (opr == operation::Slti) {
            int rs1_value = static_cast<int>(reg.read_register(value2));
            int imm = static_cast<int>(value3);
            int value = (rs1_value < imm) ? 1 : 0;
            reg.write_register(value1, static_cast<unsigned int>(value));
        } else if (opr == operation::Sltiu) {
            int rs1_value = (reg.read_register(value2));
            int imm = (value3);
            unsigned int value = (rs1_value < imm) ? 1 : 0;
            reg.write_register(value1, value);
        } else if (opr == operation::Lb) {
            /* code */
        } else if (opr == operation::Lbu) {
            /* code */
        } else if (opr == operation::Lh) {
            /* code */
        } else if (opr == operation::Lhu) {
            /* code */
        } else if (opr == operation::Lw) {
            /* code */
        } else if (opr == operation::Sb) {
            /* code */
        } else if (opr == operation::Sh) {
            /* code */
        } else if (opr == operation::Sw) {
            /* code */
        } else if (opr == operation::Beq) {
            /* code */
        } else if (opr == operation::Bge) {
            /* code */
        } else if (opr == operation::Bgeu) {
            /* code */
        } else if (opr == operation::Blt) {
            /* code */
        } else if (opr == operation::Bltu) {
            /* code */
        } else if (opr == operation::Bne) {
            /* code */
        } else if (opr == operation::Jal) {
            /* code */
        } else if (opr == operation::Jalr) {
            /* code */
        } else if (opr == operation::Auipc) {
            /* code */
        } else if (opr == operation::Lui) {
            /* code */
        } else if (opr == operation::Ebreak) {
            /* code */
        } else if (opr == operation::Ecall) {
            /* code */
        } else if (opr == operation::Mul) {
            /* code */
        }
    }
};


#endif      // ALU_HPP
