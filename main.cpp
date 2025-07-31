#include "Headers/CPU.hpp"
#include <iostream>

using namespace Granthy;

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
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