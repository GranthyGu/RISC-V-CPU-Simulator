#include "Headers/CPU.hpp"
#include <iostream>

using namespace Granthy;

int main() {
    CPU simulator;
    simulator.initialize();
    while (true) {
        std::pair<bool, uint32_t> result = simulator.work();
        if (result.first) {
            result.second &= 255;
            std::cout << result.second << std::endl;
            break;
        }
    }
}