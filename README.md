# CPU Scheduling Algorithms

This project implements three fundamental CPU scheduling algorithms in C as part of an Operating Systems course project.

## Algorithms Implemented

1. **FCFS (First Come First Served)**  
2. **SJF (Shortest Job First – Non-Preemptive)**  
3. **Round Robin (RR)**  

Each algorithm computes:
- **Average Turnaround Time** (completion − arrival)  
- **Average Waiting Time** (turnaround − burst)  
- **Average Response Time** (start − arrival)  

## File Structure

```text
.
├── scheduler.h         # Declarations for structs and scheduling functions
├── scheduler.c         # Implementation of FCFS, SJF and RR algorithms
├── scheduler_test.c    # Test cases and assertions for validating correctness
├── Dockerfile          # (Optional) Dockerized build & run
└── README.md           # Project documentation (this file)
```

This compiles the source files and produces an executable named `scheduler_test`.

## How to Build & Run Using Docker

```bash
docker build -t cpu-scheduler .
docker run --rm cpu-scheduler
```

This will execute the scheduler tests and print the calculated vs. expected metrics for:
- FCFS
- SJF
- Round Robin (with specified quantum)

If all assertions pass, you will see:

```
>>> Test Case X PASSED.
...
ALL TESTS PASSED.
```

## Example Output (Truncated)

```
==== Test Case 1 ====
FCFS: Calculated: Turnaround: 15.00, Waiting: 7.33, Response: 7.33
      Expected:   Turnaround: 15.00, Waiting: 7.33, Response: 7.33
SJF:  Calculated: Turnaround: 15.00, Waiting: 7.33, Response: 7.33
      Expected:   Turnaround: 15.00, Waiting: 7.33, Response: 7.33
RR (Quantum = 4): Calculated: Turnaround: 19.33, Waiting: 11.67, Response: 3.00
                 Expected:   Turnaround: 19.33, Waiting: 11.67, Response: 3.00
>>> Test Case 1 PASSED.

… (tests 2 through 7) …

==== Test Case 8 ====
FCFS: Calculated: Turnaround: 17.00, Waiting: 11.17, Response: 11.17
SJF:  Calculated: Turnaround: 15.17, Waiting: 9.33,  Response: 9.33
RR (Quantum = 4): Calculated: Turnaround: 20.67, Waiting: 14.83, Response: 6.33
                 Expected:   Turnaround: 20.67, Waiting: 14.83, Response: 6.33
>>> Test Case 8 PASSED.

ALL TESTS PASSED.
```
