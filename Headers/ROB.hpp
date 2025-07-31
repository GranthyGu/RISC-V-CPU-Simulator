#ifndef ROB_HPP
#define ROB_HPP

#include <iostream>
#include <cstdint>
#include "Decoder.hpp"
#include "RegisterFile.hpp"
#include "Memory.hpp"
#include "ALU.hpp"

namespace Granthy {

struct table {
    operation opr;
    bool busy = false;
    bool finish = false;
    uint32_t value = 0;
    uint32_t cur_PC = 0;
    uint32_t dest = -1;
    int index;
};

struct ROB_output {     // Used to do broadcast.
    table info;
    int index;
};

class ROB {
public:
    table input;
    table buffer[MAX];    // Store the inputs.
    ROB_output output;
    int head_index = 0;
    int tail_index = 0;
    size_t size = 0;
public:
    ROB() = default;
    // A function to do operator in ROB, from input -> output.
    void do_operation() {
        if (input.busy && size < MAX) {
            buffer[tail_index] = input;
            input.busy = false;
            size++;
            tail_index = (tail_index + 1) % MAX;
        }
        if (!output.info.busy && buffer[head_index].finish && size > 0) {
            output.info = buffer[head_index];
            output.info.busy = true;
            output.index = head_index;
            size--;
            buffer[head_index].busy = false;
            head_index = (head_index + 1) % MAX;
        }
        return;
    }
    void set_input(const table& con) {input = con;}
    bool set_input_by_decoder(decoder_output decode, uint32_t cur_PC, Register& reg) {
        if (input.busy) {
            return false;
        }
        if (decode.opr == operation::Stop) {
            input.busy = true;
            input.opr = decode.opr;
            input.finish = true;
            return true;
        }
        table new_input;
        new_input.opr = decode.opr;
        new_input.busy = true;
        new_input.cur_PC = cur_PC;
        new_input.dest = decode.value_1;
        new_input.index = tail_index;
        input = new_input;
        return true;
    }
    void get_info_from_mem(memory_output mem_info) {
        int index = mem_info.ROB_index;
        buffer[index].finish = true;
        buffer[index].busy = false;
        buffer[index].value = mem_info.value;
    }
    void get_info_from_ALU(ALU_output ALU_info) {
        int index = ALU_info.index;
        buffer[index].busy = false;
        buffer[index].finish = true;
        buffer[index].value = ALU_info.output;
    }
    ROB_output get_output() {return output;}
    void flush() {
        input.busy = output.info.busy = false;
        for (int i = 0; i < MAX; i++) {
            buffer[i].busy = false;
        }
        size = 0;
        head_index = tail_index = 0;
    }
};
}
#endif