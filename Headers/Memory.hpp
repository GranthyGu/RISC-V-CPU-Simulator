#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <iostream>
#include <cstdint>
#include <string>
#include "Decoder.hpp"
#include "LSB.hpp"

enum Type {
    load, store,
};

struct instruction {
    uint32_t address;
    uint8_t input_data;
    uint8_t fetched_data;
    Type type;
    bool finished = false;
};

struct memory_input {
    uint32_t value = 0;
    uint32_t address = 0;
    operation opr;
    bool finished = false;
    int ROB_index = 0;
    int clk = -1;    // -1 means not busy, the operation done in clk == 3.
};

struct memory_output {
    int ROB_index = -1;
    uint32_t value = 0;
    bool busy = false;
};

class Memory {
public:
    uint32_t memory[0x2500] = {0};
    int current_address = 0;
    memory_input input;
    memory_input queue[8];  // To instore the value that before the operation "Store" did.
    memory_output output;
    instruction bytes[4];   // The operation on each byte, corresponding to the input.
    size_t queue_size = 0;
    size_t byte_num = 0;
    bool reset = false;     // reset = true, when the predection is false.
    uint32_t parse_address(std::string line) {
        std::string address = line.substr(1, 8);
        uint32_t address_ = std::stoul(address, nullptr, 16);
        return address_;
    }
    std::string skip_spaces(std::string str) {
        std::string new_str;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] != ' ') {
                new_str += str[i];
            }
        }
        return new_str;
    }
    void decode_the_operation() {       // Construct the bytes.
        if (input.opr == operation::Lb) {
            bytes[0].type = Type::load;
            bytes[0].address = input.address;
            bytes[0].finished = false;
            byte_num = 1;
        } else if (input.opr == operation::Lbu) {
            bytes[0].type = Type::load;
            bytes[0].address = input.address;
            bytes[0].finished = false;
            byte_num = 1;
        } else if (input.opr == operation::Lh) {
            bytes[0].type = Type::load;
            bytes[0].address = input.address + 1;
            bytes[0].finished = false;
            bytes[1].type = Type::load;
            bytes[1].address = input.address;
            bytes[1].finished = false;
            byte_num = 2;
        } else if (input.opr == operation::Lhu) {
            bytes[0].type = Type::load;
            bytes[0].address = input.address + 1;
            bytes[0].finished = false;
            bytes[1].type = Type::load;
            bytes[1].address = input.address;
            bytes[1].finished = false;
            byte_num = 2;
        } else if (input.opr == operation::Lw) {
            bytes[0].type = Type::load;
            bytes[0].address = input.address + 3;
            bytes[0].finished = false;
            bytes[1].type = Type::load;
            bytes[1].address = input.address + 2;
            bytes[1].finished = false;
            bytes[2].type = Type::load;
            bytes[2].address = input.address + 1;
            bytes[2].finished = false;
            bytes[3].type = Type::load;
            bytes[3].address = input.address;
            bytes[3].finished = false;
            byte_num = 4;
        } else if (input.opr == operation::Sb) {
            bytes[0].type = Type::store;
            bytes[0].address = input.address;
            bytes[0].input_data = input.value & 0xFF;
            bytes[0].finished = false;
            byte_num = 1;
        } else if (input.opr == operation::Sh) {
            bytes[0].type = Type::store;
            bytes[0].address = input.address + 1;
            bytes[0].input_data = (input.value >> 8) & 0xFF;
            bytes[0].finished = false;
            bytes[1].type = Type::store;
            bytes[1].address = input.address;
            bytes[1].input_data = (input.value) & 0xFF;
            bytes[1].finished = false;
            byte_num = 2;
        } else if (input.opr == operation::Sw) {
            bytes[0].type = Type::store;
            bytes[0].address = input.address + 3;
            bytes[0].input_data = (input.value >> 24) & 0xFF;
            bytes[0].finished = false;
            bytes[1].type = Type::store;
            bytes[1].address = input.address + 2;
            bytes[1].input_data = (input.value >> 16) & 0xFF;
            bytes[1].finished = false;
            bytes[2].type = Type::store;
            bytes[2].address = input.address + 1;
            bytes[2].input_data = (input.value >> 8) & 0xFF;
            bytes[2].finished = false;
            bytes[3].type = Type::store;
            bytes[3].address = input.address;
            bytes[3].input_data = (input.value) & 0xFF;
            bytes[3].finished = false;
        }
    }
    void check_complete(uint32_t result) {
        if (bytes[0].type == Type::store) {
            if (!reset) {
                memory_input new_reserved = {result, input.address, input.opr, false, -1, -1};
                queue[queue_size] = new_reserved;
                queue_size++;
            }
        } else {
            if (input.opr == operation::Lb) {
                result = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(static_cast<uint8_t>(result))));
            } else if (input.opr == operation::Lh) {
                result = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(static_cast<uint16_t>(result))));
            }
        }
        output.value = (bytes[0].type == Type::store) ? input.value : result;
        output.busy = true;
        output.ROB_index = input.ROB_index;
        input.clk = -1;     // Complete!
    }
public:
    Memory() = default;
    uint32_t get_mem(uint32_t index) {
        return memory[index];
    }
    void flush() {
        reset = true;
        input.clk = -1;
    }
    void set_input(memory_input input_) {input = input_;}
    bool set_input_by_LSB(LSB_entry input_) {
        memory_input new_input;
        new_input.address = input_.target_address;
        new_input.clk = 0;
        new_input.finished = false;
        new_input.opr = input_.opr;
        new_input.ROB_index = input_.index_ROB;
        new_input.value = input_.value;
        input = new_input;
    }
    memory_output get_output() {return output;}
    void pop_queue() {
        if (queue_size == 0) {
            return;
        }
        for (int i = 0; i < queue_size - 1; i++) {
            queue[i] = queue[i + 1];
        }
        if (queue_size > 0) {
            queue_size--;
        }
    }
    void initialize() {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line[0] == '@') {
                current_address = parse_address(line);
                continue;
            } else {
                std::string info = skip_spaces(line);
                for (int i = 0; i < info.length() / 8; i++) {
                    std::string str_ = info.substr(i * 8, 8);
                    std::string str;
                    std::string str = str_.substr(6, 2) + str_.substr(4, 2) + 
                                str_.substr(2, 2) + str_.substr(0, 2);
                    uint32_t pos = std::stoul(str, nullptr, 16);
                    memory[current_address / 4] = pos;
                    current_address += 4;
                }
            }
        }
        current_address = 0;
    }
    uint8_t read_a_byte(uint32_t index) {
        int num = index / 4;
        int index_ = index % 4;
        uint8_t byte;
        uint32_t number = memory[num];
        if (index_ == 0) {
            byte = static_cast<uint8_t>((number << 24) >> 24);
        } else if (index_ == 1) {
            byte = static_cast<uint8_t>((number << 16) >> 24);
        } else if (index_ == 2) {
            byte = static_cast<uint8_t>((number << 8) >> 24);
        } else {
            byte = static_cast<uint8_t>(number >> 24);
        }
        return byte;
    }
    void write_a_byte(uint8_t value, uint32_t index) {
        int num = index / 4;
        int index_ = index % 4;
        uint32_t number = memory[num];
        if (index_ == 0) {
            number = (number & 0xFFFFFF00) | value;
        } else if (index_ == 1) {
            number = (number & 0xFFFF00FF) | (static_cast<uint32_t>(value) << 8);
        } else if (index_ == 2) {
            number = (number & 0xFF00FFFF) | (static_cast<uint32_t>(value) << 16);
        } else {
            number = (number & 0x00FFFFFF) | (static_cast<uint32_t>(value) << 24);
        }
        memory[num] = number; 
    }
    void do_operation() {
        if (reset) {         // Clear the input and the queue.
            if (queue_size == 0 && input.clk == -1) {
                reset = false;      // Already cleared all the inputs.
            }
            if (queue_size > 0 && input.clk == -1) {
                input = queue[queue_size - 1];
                queue_size--;
                input.ROB_index = -1;
                input.clk = 0;
                input.finished = false;
            }
        }
        if (input.clk == -1) {
            return;
        }
        if (input.clk == 0) {
            decode_the_operation();     // Decode the operation in the first clock time.
        }
        input.clk++;
        bool finished = true;
        // Then do the operation when clk == 3. Check the value of the clock first.
        for (int i = 0; i < byte_num; i++) {
            if (bytes[i].finished == false) {
                if (bytes[i].type == Type::load) {
                    if (input.clk < 3){      // Not the time to submit!
                        finished = false;
                        continue;
                    }
                    bytes[i].finished = true;
                    bytes[i].fetched_data = read_a_byte(bytes[i].address);
                } else if (bytes[i].type == Type::store) {
                    if (input.clk < 3){      // Not the time to submit!
                        finished = false;
                        continue;
                    }
                    bytes[i].fetched_data = read_a_byte(bytes[i].address);
                    bytes[i].finished = true;
                    write_a_byte(bytes[i].input_data, bytes[i].address);
                }
            }
        }
        input.finished = finished;
        if (!output.busy && input.finished) {
            uint32_t result = 0;
            for (int i = 0; i < byte_num; i++) {
                result = result << 8;
                result = result | static_cast<uint32_t>(bytes[i].fetched_data);
            }
            check_complete(result);
        }
        if (output.ROB_index == -1) {
            output.busy = false;
        }
    }
};

#endif      // MEMORY_HPP