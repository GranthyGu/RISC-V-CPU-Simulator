#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include <iostream>

class Register {
  private:
    unsigned int registers[32] = {0};
    bool busy[32] = {false};
    int reordered_id[32] = {0};
  public:
    Register() {
        for (int i = 0; i < 32; i++) {
            registers[i] = busy[i] = 0;
            reordered_id[i] = -1;
        }
    }
    unsigned int read_register(int index) {
        if (index == 0) {
            return 0;
        } else {
            return registers[index];
        }
    }
    void write_register(int index, unsigned int value) {
        if (index == 0) {
            return;
        } else {
            registers[index] = value;
        }
    }
};

#endif      // REGISTER_FILE_HPP