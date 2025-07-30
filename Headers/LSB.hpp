#ifndef LSB_HPP
#define LSB_HPP

#include "Decoder.hpp"
#include "RS.hpp"

namespace Granthy {
struct LSB_entry {
    operation opr;
    bool busy = false;
    uint32_t bytes = 0;     // Number of bytes to operate.
    uint32_t target_address = 0;
    uint32_t value = 0;     // It is the value to store. If load, value = 0.
    int index_ROB = 0;
    bool ready = false;
};

class LSB {
public:
    LSB_entry input;
    LSB_entry queue_[8];
    LSB_entry output;
    size_t size_ = 0;
    LSB() = default;
    void do_operation() {
        if (input.busy && size_ < 8) {
            queue_[size_] = input;
            size_++;
            input.busy = false;
        }
        if (!output.busy && size_ > 0) {
            output = queue_[0];
            if (queue_[0].ready) {
                output.busy = true;
                for (int i = 0; i < 7; i++) {
                    queue_[i] = queue_[i + 1];
                }
                queue_[size_ - 1].busy = false;
                size_--;
            }
        }
        return;
    }
    void set_input(LSB_entry input_) {input = input_;}
    LSB_entry get_output() {return output;}
    bool set_input_from_decoder(decoder_output decode, uint32_t ROB_order) {
        LSB_entry new_input;
        new_input.opr = decode.opr;
        new_input.index_ROB = ROB_order;
        new_input.busy = true;
        new_input.ready = false;
        if (decode.opr == operation::Lb || decode.opr == operation::Lbu || decode.opr == operation::Sb) {
            new_input.bytes = 1;
        } else if (decode.opr == operation::Lh || decode.opr == operation::Lhu || decode.opr == operation::Sh) {
            new_input.bytes = 2;
        } else if (decode.opr == operation::Lw || decode.opr == operation::Sw) {
            new_input.bytes = 4;
        }
        input = new_input;
        return true;
    }
    bool set_by_RS(RS_entry output_) {
        for (int i = 0; i < 8; i++) {
            if (queue_[i].index_ROB == output_.index) {
                queue_[i].ready = true;
                queue_[i].value = output_.vk;
                queue_[i].target_address = output_.vj + output_.imm;
                return true;
            }
        }
        return false;
    }
    void flush() {
        input.busy = output.busy = false;
        for (int i = 0; i < 8; i++) {
            queue_[i].busy = false;
        }
    }
};
}

#endif      // LSB_HPP