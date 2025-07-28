#include "memory_manager.h"



MemoryManager::MemoryManager(int numFrames, int pageSize, ReplacementPolicy user_replacement_policy)
    : numFrames(numFrames), pageSize(pageSize), frameTable(numFrames, -1)
{
    replacement_policy = user_replacement_policy;

    for (int i = 0; i < numFrames; ++i) 
    {
        freeFrames.push_back(i);
    }
}

int MemoryManager::getFreeFrameFIFO() 
{
    if (freeFrames.size() == 0)
    {    
        int victim = FIFO_pid_order.front();
        FIFO_pid_order.pop_front();
        
        // remove the page table entry of the overwritten process
        processes[frameToPid[victim]].pageTable[frameToPage[victim]].valid = false;
    
        return victim;
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
    if (!lruStack.empty() && freeFrames.size() == 0) 
    {
        int victim = lruStack.back();
        lruStack.pop_back();


        return victim;
    }
    return getFreeFrameFIFO();
}

int MemoryManager::getFreeFrame(const std::string& pid)
{
    if (replacement_policy == ReplacementPolicy::FIFO)
    {
        return getFreeFrameFIFO();
    }
    else
    {
        return getFreeFrameLRU(pid);
    }

    return -1;
}

void MemoryManager::LRU_access(int frame_id)
{
    //check if in queue at the moment
    bool in_queue = false;
    for (int i = 0; i < lruStack.size(); i++)
    {   
        if (lruStack[i] == frame_id)
        {
            in_queue = true;
            lruStack.erase(lruStack.begin() + i);
            break;
        }
    }

    if(lruStack.size() != 0)
        lruStack.insert(lruStack.begin(), frame_id);
    else
        lruStack.push_back(frame_id);
    
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

        if (page_count_toalloc > numFrames)
        {
            std::cout << "process wont fit in the stack \n";
            return;
        } 

        processes.emplace(pid, Process(pid));
        for (int i = 0; i < page_count_toalloc; ++i) 
        {
            int frame = getFreeFrame(pid);
            LRU_access(frame);


            processes[pid].pageTable[i] = {true, frame, false, false};
            FIFO_pid_order.push_back(frame);    //add newest frame number to queue 
            
            frameToPid[frame] = pid;
            frameToPage[frame] = i;
        }
        std::cout << "Allocated " << page_count_toalloc << " pages to process " << pid << std::endl;
    } 
    else if (cmd == "access") 
    {
        int vaddr;
        iss >> pid >> vaddr >> rw;

        if (processes.find(pid) == processes.end()) 
        {
            std::cout << "Process does not exist \n";
            return;     //if process not allocated
        }

        
        


        int pageNum = vaddr / pageSize;
        int offset = vaddr % pageSize;


        if (processes[pid].pageTable.find(pageNum) == processes[pid].pageTable.end()) 
        {
            int frame = getFreeFrame(pid);
            processes[pid].pageTable[pageNum] = {true, frame, true, rw == "write"};

            frameToPid[frame] = pid;
            frameToPage[frame] = pageNum;
        }

        int frame = processes[pid].pageTable[pageNum].frameNumber;
        int paddr = frame * pageSize + offset;

        LRU_access(frame);


        std::cout << "Translated virtual address " << vaddr << " (" << pid << ") → physical address " << paddr << std::endl;
    } 
    else if (cmd == "free") 
    {
        iss >> pid;
        if (processes.find(pid) != processes.end()) 
        {
            for (auto &[page, pte] : processes[pid].pageTable) // translate processes' page table into f
            {
                if (!pte.valid) break;
                freeFrames.push_front(pte.frameNumber);

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
    else if (cmd == "view") 
    {
        print_memory_overview();
        
    }
}

void MemoryManager::print_memory_overview()         //view: print frames and their owners
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



