#include "Headers/CPU.hpp"
#include <iostream>

using namespace Granthy;

int main() {
    freopen("input.txt", "r", stdin);
    CPU simulator;
    simulator.initialize();
    while (true) {
        std::pair<bool, uint32_t> result = simulator.work();
        if (result.first) {
            std::cout << result.second << std::endl;
            break;
        }
    }
}