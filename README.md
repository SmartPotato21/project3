# Simulated Memory Management System

## How to Compile
```
g++ -std=c++17 main.cpp memory_manager.cpp -o memory_sim
```

## How to Run
```
./memory_sim < sample_input.txt
```

## Project Features
- Supports memory allocation (`alloc <pid> <pages>`)
- Virtual-to-physical translation (`access <pid> <vaddr> <read/write>`)
- Memory deallocation (`free <pid>`)
- Frame PID owner table (`view`)
- Implements FIFO and LRU page replacement, pick via command line
