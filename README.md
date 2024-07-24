# Simplified Linux CFS Scheduling Policy Using Threads

The objective is to simulate a simplified scheduling policy based on Linux Completely Fair Scheduler (CFS) using threads

# Usage
1. Before executing and running the program, you will first need a text file or CSV to parse for process information. The format is as follows:
Each row contains information for one process: PID, process type, priority level, expected execution time (ms), affinity
2. Once text file or CSV file is generated, place the program and formatted data file in the same folder/location. 
3. Launch terminal and navigate (use cd) to where the program and data file is located
4. Compile on terminal by inputting into the terminal:
gcc cfs.c -o cfs
5. Run the program by inputting into the terminal:
./cfs

# Goal of Program
This project aims to emulate a simplified scheduling policy resembling the Linux Completely Fair Scheduler (CFS) through thread-based simulation. 
The process begins by parsing a text file containing key information for each task, including PID, process type, priority level, expected execution 
time (in ms), and affinity. Processes are then categorized based on their type—NORMAL, FIFO, or RR (Round Robin)—and assigned to one of the four 
available CPUs, each featuring three ready queues. The dispatcher orchestrates the movement of processes from the ready state to the running state, 
executing them according to their assigned scheduling policies. The simulation concludes when all processes complete execution, marking the end of 
the program. This simulation provides insight into the functioning of a simplified scheduling policy inspired by the Linux CFS using threads.

# Output
The final result of the program displays the scheduling and execution details of tasks processed by consumer threads. For each task, the output 
indicates its process type, priority, remaining execution time, and CPU affinity. The scheduling policies, including NORMAL, RR (Round Robin), and 
FIFO (First In First Out), are applied accordingly, with tasks either incrementing dynamic priority, executing within time slices, or following a first-
come-first-served approach. The output provides a concise overview of the multi-level queue scheduler's effective management and execution of diverse 
tasks based on their characteristics.

## Developed by Mahad Ahmed