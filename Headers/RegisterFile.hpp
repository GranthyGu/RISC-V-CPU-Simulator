#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include <iostream>
#include <cstdint>
#include "Decoder.hpp"

namespace Granthy {
class Register {
  private:
    uint32_t registers[32] = {0};
    bool busy[32] = {false};
    int reordered_id[32] = {-1};
  public:
    Register() {
        for (int i = 0; i < 32; i++) {
            registers[i] = busy[i] = 0;
            reordered_id[i] = -1;
        }
    }
    uint32_t read_register(int index) {
        if (index == 0) {
            return 0;
        } else {
            return registers[index];
        }
    }
    void write_register(int index, uint32_t value) {
        if (index == 0) {
            return;
        } else {
            registers[index] = value;
        }
    }
    void set_reordered_id(int index_reg, int index_ROB) {
        reordered_id[index_reg] = index_ROB;
    }
    int get_reordered_id(int index_reg) {return reordered_id[index_reg];}
    void get_broadcast_from_ROB(uint32_t dest, operation opr, uint32_t value, uint32_t index) {
        if (opr == operation::Stop) {
            return;
        }
        if (reordered_id[dest] == index) {
            reordered_id[dest] = -1;
        }
        if (opr != operation::Sb && opr != operation::Sh && opr != operation::Sw &&
            opr != operation::Beq && opr != operation::Bge && opr != operation::Bgeu &&
            opr != operation::Blt && opr != operation::Bltu && opr != operation::Bne && dest != 0) {
            registers[dest] = value;
        }
    }
};
}

#endif      // REGISTER_FILE_HPP