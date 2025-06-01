// scheduler.h

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>  /* for size_t */

/** Process control block for simulation. */
typedef struct {
    int pid;            /* process ID (informational) */
    int arrivalTime;    /* when it arrives in ready queue */
    int burstTime;      /* total CPU time required */

    /* Filled in by scheduler: */
    int remainingTime;  /* for RR */
    int startTime;      /* first time on CPU */
    int completionTime; /* when it finishes */
} Process;

/** Aggregated metrics returned by each algorithm. */
typedef struct {
    float avgTurnaround;  /* average(completion–arrival) */
    float avgWaiting;     /* average(turnaround–burst)    */
    float avgResponse;    /* average(start–arrival)       */
} Metrics;

/** First-Come-First-Served */
Metrics fcfs_metrics(const Process proc[], size_t n);

/** Non-preemptive Shortest-Job-First */
Metrics sjf_metrics(const Process proc[], size_t n);

/** Round-Robin with given time quantum */
Metrics rr_metrics(const Process proc[], size_t n, int timeQuantum);

#endif /* SCHEDULER_H */
