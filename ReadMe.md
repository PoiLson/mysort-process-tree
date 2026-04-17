# mysort – Process-Based Parallel Sorting System

## About

This repository contains coursework developed for the *Operating Systems* course at the Department of Informatics and Telecommunications, National and Kapodistrian University of Athens (NKUA).

The project implements a process-based parallel sorting system using a hierarchical structure of cooperating processes.

## Main Idea

The goal of this project is to explore process creation, inter-process communication, and synchronization in Linux.

The system sorts records stored in a binary file by constructing a **k-ary process tree**, where each node performs part of the computation.

The architecture consists of:

- **Coordinator (root process)**
  - creates the process hierarchy
  - distributes work across child processes
  - collects and merges final results

- **Splitter/Merger processes**
  - divide the dataset into smaller subsets
  - spawn child processes
  - merge intermediate sorted results

- **Sorter processes (leaf nodes)**
  - execute independent sorting programs
  - sort subsets of the data
  - return results to their parent

The sorting is performed based on:

1. surname  
2. name (if needed)  
3. record ID (as a tie-breaker)

## System Design

The system uses core OS mechanisms:

- **Process creation** → `fork()`
- **Program execution** → `exec()`
- **Inter-process communication** → pipes / FIFOs
- **Synchronization & signaling** → `kill()`, `wait()`, signals (`SIGUSR1`, `SIGUSR2`)
- **File operations** → `open()`, `read()`, `write()`, `close()`

All processes operate **concurrently**, forming a cooperative computation model.

## Functionality

The system:

- constructs a process tree of depth 2
- distributes workload across processes
- uses two different sorting algorithms (via separate executables)
- merges partial results into a final sorted output
- reports:
  - sorted records
  - execution time per sorter
  - number of signals received (USR1 / USR2)

## Usage

The project includes a [`Makefile`](Makefile).

### Compile

```make program```

### Run

```make execute```

or manually:

```./build/main -i ./voterfiles/voters50.bin -k 4 -e1 ./build/sorting1 -e2 ./build/sorting2```

#### Parameters 

- ```-i``` → input binary file
- ```-k``` → number of child processes per node
- ```-e1``` → first sorting executable
- ```-e2``` → second sorting executable

## Data Format

Each record in the binary file consists of:

- Record ID (```int```)
- Last Name (```char[20]```)
- First Name (```char[20]```)
- Postal Code (```char[6]```)

## Technologies

- C
- Linux system calls
- Process management
- Inter-process communication (IPC)
- Parallel computation
