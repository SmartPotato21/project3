#include "memory_manager.h"

MemoryManager::MemoryManager(int numFrames, int pageSize)
    : numFrames(numFrames), pageSize(pageSize), frameTable(numFrames, -1) {
    for (int i = 0; i < numFrames; ++i) 
    {
        freeFrames.push_back(i);
    }
}

int MemoryManager::getFreeFrameFIFO() 
{
    if (freeFrames.empty()) 
    {
    //     int victim = freeFrames.front();
    //     freeFrames.pop_front();
    //     return victim;
    } 
    else 
    {
        int frame = freeFrames.front();
        freeFrames.pop_front();
        return frame;
    }
}

int MemoryManager::getFreeFrameLRU(const std::string& pid) 
{
    for (auto& [_, proc] : processes) {
        if (!proc.lruStack.empty()) {
            int victim = proc.lruStack.front();
            proc.lruStack.erase(proc.lruStack.begin());
            return victim;
        }
    }
    return getFreeFrameFIFO();
}

void MemoryManager::processCommand(const std::string &command) 
{
    std::istringstream iss(command);
    std::string cmd, pid, rw;
    int val;

    iss >> cmd;
    if (cmd == "alloc") 
    {
        int page_count_toalloc;
        iss >> pid >> page_count_toalloc;

        if (page_count_toalloc > freeFrames.size())
        {
            std::cout << "process wont fit in the stack \n";
            return;
        } 

        processes.emplace(pid, Process(pid));

        for (int i = 0; i < page_count_toalloc; ++i) {
            int frame = getFreeFrameFIFO();
            processes[pid].pageTable[i] = {true, frame, false, false};
            processes[pid].pageOrder.push_back(i);
            frameToPid[frame] = pid;
            frameToPage[frame] = i;
        }
        std::cout << "Allocated " << page_count_toalloc << " pages to process " << pid << std::endl;
    } 
    else if (cmd == "access") 
    {
        int vaddr;
        iss >> pid >> vaddr >> rw;

        int pageNum = vaddr / pageSize;
        int offset = vaddr % pageSize;

        if (processes.find(pid) == processes.end() || processes[pid].pageTable.find(pageNum) == processes[pid].pageTable.end()) {
            
            int frame = getFreeFrameFIFO();
            processes[pid].pageTable[pageNum] = {true, frame, true, rw == "write"};
            processes[pid].pageOrder.push_back(pageNum);
            frameToPid[frame] = pid;
            frameToPage[frame] = pageNum;
        }

        int frame = processes[pid].pageTable[pageNum].frameNumber;
        int paddr = frame * pageSize + offset;

        std::cout << "Translated virtual address " << vaddr << " (" << pid << ") → physical address " << paddr << std::endl;
    } 
    else if (cmd == "free") 
    {
        iss >> pid;
        if (processes.find(pid) != processes.end()) {
            for (auto &[page, pte] : processes[pid].pageTable) {
                freeFrames.push_front(pte.frameNumber);
                std::cout << "Freed frame: " << pte.frameNumber << "\n";
                frameToPid.erase(pte.frameNumber);
            }
            processes.erase(pid);
            std::cout << "Freed memory for process " << pid << std::endl;
        }
        else
        {
            std::cout << "Process does not exist \n";
        }
    }
}

void MemoryManager::print_memory_overview()
{
    std::cout << "Frame:        Owner: \n";
    for (int i = 0; i < numFrames; i++)
    {
        if(frameToPid.find(i) != frameToPid.end())
        {
            std::cout << i << ":          " << frameToPid[i] << "\n";
        }
        else
        {

            std::cout << i << ":          " << "None" << "\n";
        }
    }
}



// TODO: FREE DOESNT FREE FRAMETOPID