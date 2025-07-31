#ifndef RS_HPP
#define RS_HPP

#include "Decoder.hpp"
#include "RegisterFile.hpp"

namespace Granthy {
struct RS_entry {
    bool busy = false;
    operation opr;
    size_t index = 0;
    uint32_t vj = 0;
    uint32_t vk = 0;
    int Qj = -1;
    int Qk = -1;
    size_t Dest = 0;
    uint32_t imm = 0;
};

class RS {
public:
    RS_entry input;
    RS_entry Reservation_station[2000];
    RS_entry output;    // To LSB or ALU.
    size_t size_ = 0;
    RS() = default;
    void do_operation() {
        if (input.busy && size_ < 2000) {
            size_++;
            for (int i = 0; i < 2000; i++) {
                if (!Reservation_station[i].busy) {
                    Reservation_station[i] = input;
                    Reservation_station[i].busy = true;
                    input.busy = false;
                    break;
                }
            }
        }
        if (!output.busy && size_ > 0) {
            for (int i = 0; i < 2000; i++) {
                if (Reservation_station[i].busy && 
                    Reservation_station[i].Qj == -1 && 
                    Reservation_station[i].Qk == -1) {
                    output = Reservation_station[i];
                    for (int j = i; j < 1999; j++) {
                        Reservation_station[j] = Reservation_station[j + 1];
                    }
                    Reservation_station[1999].busy = false;
                    size_--;
                    break;
                }
            }
        }
    }
    bool set_input_by_decoder(decoder_output decode, Register& reg, uint32_t ROB_order, uint32_t ROB_cur_PC) {
        if (input.busy) {
            return false;
        }
        RS_entry new_entry;
        new_entry.busy = true;
        new_entry.opr = decode.opr;
        new_entry.Dest = decode.value_1;
        new_entry.index = ROB_order;
        if (decode.opr == operation::Add || decode.opr == operation::Sub || decode.opr == operation::And 
            || decode.opr == operation::Or || decode.opr == operation::Xor || decode.opr == operation::Sll 
            || decode.opr == operation::Srl || decode.opr == operation::Sra || decode.opr == operation::Slt 
            || decode.opr == operation::Sltu) {
            int r1 = decode.value_2;
            int r2 = decode.value_3;
            if (reg.get_reordered_id(r1) != -1) {
                new_entry.Qj = reg.get_reordered_id(r1);
            } else {
                new_entry.vj = reg.read_register(r1);
            }
            if (reg.get_reordered_id(r2) != -1) {
                new_entry.Qk = reg.get_reordered_id(r2);
            } else {
                new_entry.vk = reg.read_register(r2);
            }
        } else if (decode.opr == operation::Addi || decode.opr == operation::Andi 
            || decode.opr == operation::Ori || decode.opr == operation::Xori || decode.opr == operation::Slli 
            || decode.opr == operation::Srli || decode.opr == operation::Srai || decode.opr == operation::Slti 
            || decode.opr == operation::Sltiu) {
            new_entry.imm = decode.value_3;
            int r1 = decode.value_2;
            if (reg.get_reordered_id(r1) != -1) {
                new_entry.Qj = reg.get_reordered_id(r1);
            } else {
                new_entry.vj = reg.read_register(r1);
            }
        } else if (decode.opr == operation::Lb || decode.opr == operation::Lbu || decode.opr == operation::Lh 
                || decode.opr == operation::Lhu || decode.opr == operation::Lw) {
            new_entry.imm = decode.value_3;
            int r1 = decode.value_2;
            if (reg.get_reordered_id(r1) != -1) {
                new_entry.Qj = reg.get_reordered_id(r1);
            } else {
                new_entry.vj = reg.read_register(r1);
            }
        } else if (decode.opr == operation::Beq || decode.opr == operation::Bge || decode.opr == operation::Bgeu 
                || decode.opr == operation::Blt || decode.opr == operation::Bltu || decode.opr == operation::Bne) {
            new_entry.imm = decode.value_3;
            int r1 = decode.value_1;
            int r2 = decode.value_2;
            if (reg.get_reordered_id(r1) != -1) {
                new_entry.Qj = reg.get_reordered_id(r1);
            } else {
                new_entry.vj = reg.read_register(r1);
            }
            if (reg.get_reordered_id(r2) != -1) {
                new_entry.Qk = reg.get_reordered_id(r2);
            } else {
                new_entry.vk = reg.read_register(r2);
            }
        } else if (decode.opr == operation::Auipc || decode.opr == operation::Lui) {
            new_entry.imm = decode.value_2;
            new_entry.vj = ROB_cur_PC;
        } else if (decode.opr == operation::Jalr) {
            new_entry.imm = 4;
            int r2 = decode.value_2;
            new_entry.vj = ROB_cur_PC;
            if (reg.get_reordered_id(r2) != -1) {
                new_entry.Qk = reg.get_reordered_id(r2);
            } else {
                new_entry.vk = reg.read_register(r2);
            }
        } else if (decode.opr == operation::Jal) {
            new_entry.imm = 4;
            new_entry.vj = ROB_cur_PC;
        } else if (decode.opr == operation::Sb || decode.opr == operation::Sh || decode.opr == operation::Sw ) {
            new_entry.imm = decode.value_3;
            int r1 = decode.value_2;
            int r2 = decode.value_1;
            if (reg.get_reordered_id(r1) != -1) {
                new_entry.Qj = reg.get_reordered_id(r1);
            } else {
                new_entry.vj = reg.read_register(r1);
            }
            if (reg.get_reordered_id(r2) != -1) {
                new_entry.Qk = reg.get_reordered_id(r2);
            } else {
                new_entry.vk = reg.read_register(r2);
            }
        }
        int dest = decode.value_1;
        if (decode.opr != operation::Sb && decode.opr != operation::Sh && decode.opr != operation::Sw &&
            decode.opr != operation::Beq && decode.opr != operation::Bge && decode.opr != operation::Bgeu &&
            decode.opr != operation::Blt && decode.opr != operation::Bltu && decode.opr != operation::Bne && dest != 0) {
            reg.set_reordered_id(dest, ROB_order);
        }
        input = new_entry;
        return true;
    }
    void get_broadcast_from_ROB(uint32_t index, uint32_t dest, Register reg) {
        if (input.Qj == index) {
            input.Qj = -1;
            input.vj = reg.read_register(dest);
        }
        if (input.Qk == index) {
            input.Qk = -1;
            input.vk = reg.read_register(dest);
        }
        for (int i = 0; i < 2000; i++) {
            if (Reservation_station[i].busy) {
                if (Reservation_station[i].Qj == index) {
                    Reservation_station[i].Qj = -1;
                    Reservation_station[i].vj = reg.read_register(dest);
                }
                if (Reservation_station[i].Qk == index) {
                    Reservation_station[i].Qk = -1;
                    Reservation_station[i].vk = reg.read_register(dest);
                }
            }
        }
    }
    void flush() {
        input.busy = false;
        output.busy = false;
        for (int i = 0; i < 2000; i++) {
            Reservation_station[i].busy = false;
        }
        size_ = 0;
    }
};
}

#endif      // RS_HPP
