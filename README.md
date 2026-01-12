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

- **main.cpp**: Main simulation logic, memory management, process handling, and I/O
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

- **Output file** (e.g., output-first-2000.txt): Logs memory state at each time step:
  `Time [StartAddr1 State1 Size1] [StartAddr2 State2 Size2] ...`
  Example:
  `1 [0 P1 300] [300 P2 200] [500 hole 1500]`
- **Console output**: Status messages, errors, and completion notice

- **Algorithm Metrics**: Comprehensive statistics displayed after simulation completion:
  - **Total Processes**: Number of processes in the input file
  - **Successful Allocations**: Processes successfully allocated memory
  - **Allocation Failures**: Processes that could not be allocated
  - **Allocation Success Rate**: Percentage of successful allocations
  - **Processes Completed**: Number of processes that finished execution
  - **Total Simulation Time**: Total time units the simulation ran
  - **Average Memory Utilization**: Average percentage of memory used during execution
  - **Throughput**: Processes completed per time unit
  - **Peak Memory Usage**: Maximum memory used at any point (absolute and percentage)
  - **Average Waiting Time**: Average time processes spent waiting in queue
  - **Minimum/Maximum Queue Length**: Queue congestion metrics showing system responsiveness
  - **Average Fragmentation**: Average amount of fragmented memory (in holes) over time
  - **Peak Fragmentation**: Maximum fragmentation reached during simulation
  - **Final Number of Memory Holes**: Holes remaining after simulation
  - **Average/Smallest/Largest Hole Size**: Distribution of final memory holes
  - **Total Fragmented Memory**: Total memory in holes at simulation end

---

## How to Build & Run

### Requirements

- C++ compiler (g++ recommended, C++11 or later)
- Tested on macOS

### Build

```sh
g++ main.cpp -o memory_management
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
