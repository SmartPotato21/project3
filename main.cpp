#include <iostream>
#include "memory_manager.h"

int main() {
    MemoryManager mm(16, 4096);  // 16 frames, 4KB each

    std::string command;
    while (std::getline(std::cin, command)) {
        mm.processCommand(command);

        mm.print_memory_overview();
    }

    return 0;
}
