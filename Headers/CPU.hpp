#ifndef CPU_HPP
#define CPU_HPP

#include "RegisterFile.hpp"
#include "Decoder.hpp"
#include "Memory.hpp"
#include "ALU.hpp"
#include "LSB.hpp"
#include "Predictor.hpp"
#include "ROB.hpp"
#include "RS.hpp"
#include <iostream>

namespace Granthy {
class CPU {
private:
    Register reg;
    Decoder decoder;
    RS rs;
    ROB rob;
    ALU alu;
    LSB lsb;
    Memory memory;
    uint32_t PC = 0;
    uint32_t last_PC = 0;
    uint32_t clk = 0;
    bool wait = false;      // Use for Jalr
    std::pair<uint32_t, uint32_t> data_for_jalr;
    bool reset = false;
    int predict_correct = 0;
    int predict_wrong = 0;
    bool first_flush = false;
    bool stop = false;
    bool finish = false;
    void Wire_mem_decoder() {
        if (!decoder.input.busy) {
            uint32_t pos = PC / 4;
            uint32_t input_ = memory.get_mem(pos);
            decoder.set_input(input_);
        }
    }
    void Wire_decoder_ROB() {
        if (decoder.output.opr == operation::Stop) {
            decoder.output.busy = false;
            rob.set_input_by_decoder(decoder.output, -1, reg);
        }
        if (decoder.output.busy && !rob.input.busy) {
            rob.set_input_by_decoder(decoder.output, last_PC, reg);
        }
    }
    void Wire_decoder_RS() {
        if (decoder.output.busy && !rs.input.busy) {
            rs.set_input_by_decoder(decoder.output, reg, rob.input.index, rob.input.cur_PC);
        }
    }
    void Wire_decoder_LSB() {
        if (decoder.output.busy && !lsb.input.busy) {
            if (decoder.output.opr == operation::Lb || decoder.output.opr == operation::Lbu || 
                decoder.output.opr == operation::Lh || decoder.output.opr == operation::Lhu || 
                decoder.output.opr == operation::Lw || decoder.output.opr == operation::Sb || 
                decoder.output.opr == operation::Sh || decoder.output.opr == operation::Sw) {
                lsb.set_input_from_decoder(decoder.output, rob.input.index);
            }
            decoder.output.busy = false;
        }
    }
    void Wire_RS_LSB() {
        if (rs.output.busy) {
            if (rs.output.opr == operation::Lb || rs.output.opr == operation::Lbu || 
                rs.output.opr == operation::Lh || rs.output.opr == operation::Lhu || 
                rs.output.opr == operation::Lw || rs.output.opr == operation::Sb || 
                rs.output.opr == operation::Sh || rs.output.opr == operation::Sw) {
                lsb.set_by_RS(rs.output);
                rs.output.busy = false;
            }
        }
    }
    void Wire_RS_ALU() {
        if (rs.output.busy && !alu.input.busy) {
            if (rs.output.opr != operation::Lb && rs.output.opr != operation::Lbu &&
                rs.output.opr != operation::Lh && rs.output.opr != operation::Lhu &&
                rs.output.opr != operation::Lw && rs.output.opr != operation::Sb &&
                rs.output.opr != operation::Sh && rs.output.opr != operation::Sw) {
                alu.set_input_from_RS_output(rs.output);
                rs.output.busy = false;
            }
        }
    }
    void Wire_LSB_mem() {
        if (lsb.output.busy && memory.input.clk == -1) {
            memory.set_input_by_LSB(lsb.output);
            lsb.output.busy = false;
        }
    }
    void Wire_mem_ROB() {
        if (memory.output.busy) {
            rob.get_info_from_mem(memory.output);
            memory.output.busy = false;
        }
    }
    void Wire_ALU_ROB() {
        if (alu.output.busy) {
            rob.get_info_from_ALU(alu.output);
            alu.output.busy = false;
        }
    }
    void Wire_ROB_RS() {
        if (rob.output.info.busy) {
            rs.get_broadcast_from_ROB(rob.output.index, rob.output.info.dest, reg);
        }
    }
    void Wire_ROB_RF() {
        if (rob.output.info.busy) {
            reg.get_broadcast_from_ROB(rob.output.info.dest, rob.output.info.opr, rob.output.info.value, rob.output.index);
        }
    }
    void Wire_ROB_PC() {
        if (rob.output.info.busy) {
            rob.output.info.busy = false;
            ROB_output broadcast = rob.output;
            if (broadcast.info.opr == operation::Beq || broadcast.info.opr == operation::Bge || broadcast.info.opr == operation::Bgeu ||
                broadcast.info.opr == operation::Blt || broadcast.info.opr == operation::Bltu || broadcast.info.opr == operation::Bne) {
                if (broadcast.info.value == false) {   // Have some risks!
                    reset = true;
                    first_flush = true;
                    predict_wrong++;
                    PC = broadcast.info.cur_PC;
                } else {
                    memory.start_to_store = false;
                    memory.clear_the_queue();
                    predict_correct++;
                }
            } else if (broadcast.info.opr == operation::Jalr) {
                wait = false;
                uint32_t rs1 = data_for_jalr.first;
                uint32_t imm = data_for_jalr.second;
                last_PC = broadcast.info.cur_PC;
                PC = reg.read_register(rs1) + imm;
            } else if (broadcast.info.opr == operation::Stop) {
                finish = true;
            }
        }
    }
public:
    CPU() = default;
    uint32_t get_PC() {return PC;}
    void initialize() {
        memory.initialize();
    }
    std::pair<bool, uint32_t> work() {
        clk++;
        if (finish) {
            return std::pair<bool, uint32_t>(true, reg.read_register(10));
        }
        if (reset) {
            if (first_flush) {
                memory.flush();
                decoder.flush();
                rs.flush();
                alu.flush();
                rob.flush();
                lsb.flush();
                reg.flush();
                first_flush = false;
            } 
            if (memory.reset) {
                memory.do_operation();
            }
            if (!memory.reset) {
                reset = false;
                wait = false;
                last_PC = PC;
                PC += 4;
            }
        } else if (stop) {
            Wire_decoder_ROB();
            Wire_RS_LSB();
            Wire_RS_ALU();
            Wire_LSB_mem();
            Wire_mem_ROB();
            Wire_ALU_ROB();
            Wire_ROB_RF();
            Wire_ROB_RS();
            Wire_ROB_PC();
            rs.do_operation();
            alu.do_operation();
            memory.do_operation();
            rob.do_operation();
            lsb.do_operation();
        } else if (wait) {
            Wire_decoder_ROB();
            Wire_decoder_RS();
            Wire_decoder_LSB();
            Wire_RS_LSB();
            Wire_RS_ALU();
            Wire_LSB_mem();
            Wire_mem_ROB();
            Wire_ALU_ROB();
            Wire_ROB_RF();
            Wire_ROB_RS();
            Wire_ROB_PC();
            if (!wait || reset) {
                return std::pair<bool, uint32_t>(false, 0);
            }
            rs.do_operation();
            alu.do_operation();
            memory.do_operation();
            rob.do_operation();
            lsb.do_operation();
        } else {
            Wire_mem_decoder();
            Wire_decoder_ROB();
            Wire_decoder_RS();
            Wire_decoder_LSB();
            Wire_RS_LSB();
            Wire_RS_ALU();
            Wire_LSB_mem();
            Wire_mem_ROB();
            Wire_ALU_ROB();
            Wire_ROB_RF();
            Wire_ROB_RS();
            Wire_ROB_PC();
            if (reset) {
                return std::pair<bool, uint32_t>(false, 0);
            }
            decoder.do_operation();
            rs.do_operation();
            alu.do_operation();
            memory.do_operation();
            rob.do_operation();
            lsb.do_operation();
            std::cout << std::hex << PC << ' ' << memory.get_mem(0x11b0 / 4) << ' ' << reg.read_register(15) << std::endl;
            if (decoder.output.opr == operation::Beq || decoder.output.opr == operation::Bge || 
                decoder.output.opr == operation::Bgeu || decoder.output.opr == operation::Blt || 
                decoder.output.opr == operation::Bltu || decoder.output.opr == operation::Bne) {
                last_PC = PC;
                PC += decoder.get_value3();     // Predict that is right!
                memory.start_to_store = true;
            } else if (decoder.output.opr == operation::Jal) {
                last_PC = PC;
                PC += decoder.get_value2();
            } else if (decoder.output.opr == operation::Jalr) {
                wait = true;
                last_PC += 4;
                data_for_jalr = std::pair<uint32_t, uint32_t>(decoder.get_value2(), decoder.get_value3());
            } else if (decoder.output.opr == operation::Stop) {
                stop = true;
            } else {
                last_PC = PC;
                PC += 4;
            }
        }
        return std::pair<bool, uint32_t>(false, 0);
    }
};
}

#endif      // CPU_HPP