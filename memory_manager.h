#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <deque>
#include <sstream>

struct PageTableEntry {
    bool valid;
    int frameNumber;
    bool referenceBit;
    bool modifiedBit;
};

class Process {
public:
    std::string pid;
    std::unordered_map<int, PageTableEntry> pageTable;
    std::deque<int> pageOrder;  // For FIFO
    std::vector<int> lruStack;  // For LRU
    Process() = default;
    Process(const std::string &pid) : pid(pid) {}
};

class MemoryManager {
private:
    int numFrames;
    int pageSize;
    std::vector<int> frameTable;
    std::unordered_map<std::string, Process> processes;
    std::deque<int> freeFrames;
    std::unordered_map<int, std::string> frameToPid;
    std::unordered_map<int, int> frameToPage;

    int getFreeFrameFIFO();
    
    int getFreeFrameLRU(const std::string& pid);

public:
    MemoryManager(int numFrames, int pageSize);
    void processCommand(const std::string &command);
    void print_memory_overview();
};

#endif
