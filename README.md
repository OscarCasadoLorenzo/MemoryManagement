# Virtual Memory Management Simulator

## Best Fit & First Fit Partition Allocation in C++

**A C++ simulation exploring the behavior and efficiency of dynamic memory allocation strategies —Best Fit and First Fit— in a contiguous memory management system.**

---

## Project Overview

This project simulates dynamic memory management using variable-size partitions, focusing on the Best Fit and First Fit allocation algorithms. It models how an operating system manages physical memory, allocating and freeing memory for processes over time. The simulator enables analysis of allocation efficiency, fragmentation, and the impact of different strategies on memory usage.

**Why these algorithms?** Best Fit and First Fit are classic strategies for contiguous memory allocation. Comparing them provides insights into fragmentation, allocation success rates, and system throughput in dynamic environments.

**Insights:**

- Visualizes memory state at each allocation/deallocation event
- Highlights tradeoffs between allocation speed and fragmentation
- Enables reproducible experiments for systems-level analysis

---

## Key Concepts Implemented

- Dynamic (variable-size) memory partitioning
- Contiguous memory allocation
- Process arrival, execution, and completion simulation
- Best Fit allocation strategy
- First Fit allocation strategy
- Memory holes (free partitions) management
- Fragmentation analysis
- Simulation metrics: allocation success, memory usage, fragmentation

---

## Algorithms Explained

### Best Fit Allocation

Best Fit searches all available memory holes and allocates the process to the smallest hole that is large enough. This minimizes wasted space per allocation but can lead to many small, unusable holes (external fragmentation). Chosen for its theoretical efficiency in space utilization, it often performs poorly under heavy load due to fragmentation.

### First Fit Allocation

First Fit scans memory from the beginning and allocates the process to the first hole large enough. It is fast and simple, but can leave small holes at the start of memory, leading to fragmentation over time. Useful for its speed and low overhead.

### Why Compare Best Fit & First Fit?

The simulator demonstrates how allocation strategy affects fragmentation, allocation failures, and memory utilization. By observing both, users can analyze tradeoffs between speed and memory efficiency in dynamic workloads.

---

## Project Architecture

- **prac3.cpp**: Main simulation logic, memory management, process handling, and I/O
- **Input/Output Handler**: Reads process definitions from file, writes memory state to output
- **Memory Manager**: Allocates, deallocates, and merges memory partitions (holes)
- **Simulation Driver**: Advances time, manages process arrivals and completions

---

## Input & Output Description

### Inputs

- **Process file**: Each line defines a process as:
  `<ProcessName> <ArrivalTime> <MemoryRequired> <ExecutionTime>`
- **Total memory**: Set via command-line argument (default: 2000)
- **Algorithm selection**: First Fit or Best Fit (via command-line)

### Outputs

- **input-example.txt**: Logs memory state at each event:
  `Time [StartAddr1 State1 Size1] [StartAddr2 State2 Size2] ...`
  Example:
  `1 [0 P1 300] [300 P2 200] [500 hole 1500]`
- **Console output**: Status messages, errors, and completion notice

---

## How to Build & Run

### Requirements

- C++ compiler (g++ recommended, C++11 or later)
- Tested on macOS

### Build

```sh
g++ prac3.cpp -o memory_management
```

### Run

```sh
./memory_management <input-file> <output-file> <total-memory> <algorithm>
```

**Arguments:**

- `<input-file>`: Path to process definition file
- `<output-file>`: Output file for memory state (e.g., input-example.txt)
- `<total-memory>`: Total memory size (e.g., 2000)
- `<algorithm>`: `first` for First Fit, `best` for Best Fit

**Example:**

```sh
./memory_management input-example.txt input-example.txt 2000 best
```

---

## Generated Files

- **input-example.txt**: Chronological log of memory state after each allocation/deallocation. Each line shows time, partition start address, state (process or hole), and size.

---

## Experimental Results

The simulator enables observation of:

- Fragmentation patterns under each algorithm
- Allocation failures due to insufficient contiguous memory
- Memory utilization trends over time

---

## Limitations & Future Improvements

- Does not implement page replacement or virtual memory (pure contiguous allocation)
- No graphical visualization (text output only)
- Could be extended with:
  - Additional allocation strategies (Worst Fit, Next Fit)
  - Visualization tools
  - Support for paging and page replacement algorithms
  - Performance metrics (allocation time, fragmentation ratio)

---

## License

MIT License
