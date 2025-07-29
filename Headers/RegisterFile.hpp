#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include <iostream>
#include <cstdint>
#include "ROB.hpp"

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
    void get_broadcast_from_ROB(ROB_output broadcast) {
        if (reordered_id[broadcast.info.dest] == broadcast.index) {
            reordered_id[broadcast.info.dest] = -1;
        }
        if (broadcast.info.opr != operation::Sb && broadcast.info.opr != operation::Sh && broadcast.info.opr != operation::Sw &&
            broadcast.info.opr != operation::Beq && broadcast.info.opr != operation::Bge && broadcast.info.opr != operation::Bgeu &&
            broadcast.info.opr != operation::Blt && broadcast.info.opr != operation::Bltu && broadcast.info.opr != operation::Bne) {
            registers[broadcast.info.dest] = broadcast.info.value;
        }
    }
};

#endif      // REGISTER_FILE_HPP