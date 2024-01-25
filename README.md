# Operating System Scheduler 

This project extends the functionality of the OS Scheduler to include advanced CPU scheduling algorithms and dynamic memory allocation, providing a comprehensive simulation of an operating system. The implementation showcases strong proficiency in C programming, emphasizing intricate algorithmic design and effective data structure utilization for both CPU and memory management.

## Features

### CPU Scheduling Algorithms:

- Non-preemptive Highest Priority First (HPF).
- Shortest Remaining Time Next (SRTN).
- Round Robin (RR).

### Memory Allocation Policies:

- First Fit.
- Buddy Memory Allocation.

### Simulation Components:

1. **Process Generator:** Simulates process creation, reads input files, and initiates scheduler and clock processes.
2. **Clock Module:** Emulates an integer time clock.
3. **Scheduler:** Core component managing processes, states, and execution based on scheduling algorithms.
4. **Process:** Represents CPU-bound processes and notifies the scheduler upon termination.

## How to Use

### Compilation:

Compile the source files using your preferred C compiler.

### Execution:

Run the compiled executable.

### Input:

Provide process details in the `processes.txt` file.

### Output:

Review the generated output files (`scheduler.log`, `scheduler.perf`, `memory.log`) for detailed simulation results.
