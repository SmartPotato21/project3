#include <iostream>
#include "memory_manager.h"

int main() 
{
    std::cout << "Pick page replacement algorithm, FIFO or LRU: \n";
    std::string choice_str;
    std::cin >> choice_str;
    
    ReplacementPolicy rp;
    if(choice_str == "FIFO")
    {
        rp = ReplacementPolicy::FIFO;
        std::cout << "Picked FIFO\n";
    }
    else
    {
        rp = ReplacementPolicy::LRU;
        std::cout << "Picked LRU\n";
    }

    MemoryManager mm(256, 4096, rp);  // 256 frames, 4KB each

    std::string command;
    while (std::getline(std::cin, command)) {
        mm.processCommand(command);
    }

    return 0;
}
