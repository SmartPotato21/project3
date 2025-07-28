#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <deque>
#include <sstream>


enum class ReplacementPolicy {
    FIFO,
    LRU
};


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
    
    Process() = default;
    Process(const std::string &pid) : pid(pid) {}
};

class MemoryManager {
private:
    int numFrames;
    int pageSize;
    std::vector<int> frameTable;
    std::unordered_map<std::string, Process> processes;

    std::deque<int> FIFO_pid_order; 
    std::vector<int> lruStack;  // For LRU

    std::deque<int> freeFrames;
    std::unordered_map<int, std::string> frameToPid;
    std::unordered_map<int, int> frameToPage;

    int getFreeFrameFIFO();
    int getFreeFrameLRU(const std::string& pid);
    int getFreeFrame(const std::string& pid);


    void LRU_access(int frame_id);



public:
    MemoryManager(int numFrames, int pageSize, ReplacementPolicy replacement_policy);

    ReplacementPolicy replacement_policy;

    void processCommand(const std::string &command);
    void print_memory_overview();
};

#endif
