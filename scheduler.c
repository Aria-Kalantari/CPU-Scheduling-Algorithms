// scheduler.c

#include "scheduler.h"
#include <stdio.h>    /* perror, fprintf */
#include <stdlib.h>   /* malloc, calloc, realloc, free, exit */
#include <string.h>   /* memcpy */
#include <limits.h>   /* INT_MAX */

/* -------------------------------------------------------------------------- */
/*  INTERNAL HELPERS                                                          */
/* -------------------------------------------------------------------------- */

/* Compare by arrivalTime, then pid */
static int cmp_arrival_pid(const void *a, const void *b) {
    const Process *p = a, *q = b;
    if (p->arrivalTime != q->arrivalTime)
        return p->arrivalTime - q->arrivalTime;
    return p->pid - q->pid;
}

/* Make a private copy of the proc array */
static Process *duplicate_proc_array(const Process src[], size_t n) {
    Process *dst = malloc(n * sizeof(Process));
    if (!dst) { perror("malloc"); exit(EXIT_FAILURE); }
    memcpy(dst, src, n * sizeof(Process));
    return dst;
}

/* Package sums into Metrics */
static Metrics make_metrics(double sumTa, double sumWait,
                            double sumResp, size_t n) {
    Metrics m;
    m.avgTurnaround = (float)(sumTa   / n);
    m.avgWaiting    = (float)(sumWait / n);
    m.avgResponse   = (float)(sumResp / n);
    return m;
}

/* -------------------------------------------------------------------------- */
/*  FCFS                                                                      */
/* -------------------------------------------------------------------------- */

Metrics fcfs_metrics(const Process proc[], size_t n) {
    Process *p = duplicate_proc_array(proc, n);
    qsort(p, n, sizeof(Process), cmp_arrival_pid);

    double sumTa = 0, sumWait = 0, sumResp = 0;
    int current = 0;

    for (size_t i = 0; i < n; ++i) {
        if (current < p[i].arrivalTime)
            current = p[i].arrivalTime;  /* idle until arrival */

        p[i].startTime      = current;
        p[i].completionTime = current + p[i].burstTime;
        current             = p[i].completionTime;

        int ta = p[i].completionTime - p[i].arrivalTime;
        int wt = p[i].startTime      - p[i].arrivalTime;
        sumTa   += ta;
        sumWait += wt;
        sumResp += wt;  /* in FCFS, response == waiting */
    }

    free(p);
    return make_metrics(sumTa, sumWait, sumResp, n);
}

/* -------------------------------------------------------------------------- */
/*  NON-PREEMPTIVE SJF                                                        */
/* -------------------------------------------------------------------------- */

Metrics sjf_metrics(const Process proc[], size_t n) {
    Process *p = duplicate_proc_array(proc, n);
    int *done  = calloc(n, sizeof(int));  /* track completed */

    double sumTa = 0, sumWait = 0, sumResp = 0;
    size_t completed = 0;
    int current = 0;

    while (completed < n) {
        int best = -1, bestBurst = INT_MAX;
        for (size_t i = 0; i < n; ++i) {
            if (!done[i] && p[i].arrivalTime <= current) {
                if (p[i].burstTime < bestBurst ||
                    (p[i].burstTime == bestBurst &&
                     p[i].arrivalTime < p[best].arrivalTime)) {
                    best = (int)i;
                    bestBurst = p[i].burstTime;
                }
            }
        }
        if (best < 0) {
            /* no ready process → advance to next arrival */
            int nxt = INT_MAX;
            for (size_t i = 0; i < n; ++i)
                if (!done[i] && p[i].arrivalTime < nxt)
                    nxt = p[i].arrivalTime;
            current = nxt;
            continue;
        }

        /* execute best to completion */
        p[best].startTime      = current;
        p[best].completionTime = current + p[best].burstTime;
        current                = p[best].completionTime;
        done[best] = 1;
        ++completed;

        int ta = p[best].completionTime - p[best].arrivalTime;
        int wt = p[best].startTime      - p[best].arrivalTime;
        sumTa   += ta;
        sumWait += wt;
        sumResp += wt;  /* response == waiting */
    }

    free(done);
    free(p);
    return make_metrics(sumTa, sumWait, sumResp, n);
}

/* -------------------------------------------------------------------------- */
/*  ROUND-ROBIN with Dynamic Queue                                            */
/* -------------------------------------------------------------------------- */

typedef struct {
    int   *buf;
    size_t cap, head, tail;
} Queue;

static void q_grow(Queue *q) {
    size_t newCap = q->cap ? q->cap * 2 : 16;
    int *newBuf   = realloc(q->buf, newCap * sizeof(int));
    if (!newBuf) { perror("realloc"); exit(EXIT_FAILURE); }
    q->buf = newBuf;
    q->cap = newCap;
}

static Queue q_create(void) {
    return (Queue){ .buf = NULL, .cap = 0, .head = 0, .tail = 0 };
}

static int  q_empty(const Queue *q)          { return q->head == q->tail; }
static void q_push(Queue *q, int v)          { if (q->tail == q->cap) q_grow(q); q->buf[q->tail++] = v; }
static int  q_pop (Queue *q)                 { if (q_empty(q)) { fprintf(stderr,"underflow\n"); exit(1);} return q->buf[q->head++]; }
static void q_destroy(Queue *q)              { free(q->buf); }

Metrics rr_metrics(const Process proc[], size_t n, int quantum) {
    if (quantum <= 0) {
        fprintf(stderr,"Quantum must be > 0\n");
        exit(EXIT_FAILURE);
    }

    Process *p = duplicate_proc_array(proc, n);
    for (size_t i = 0; i < n; ++i) {
        p[i].remainingTime = p[i].burstTime;
        p[i].startTime     = -1;
    }
    qsort(p, n, sizeof(Process), cmp_arrival_pid);

    Queue rq = q_create();
    size_t nextArr = 0, finished = 0;
    int current = 0;
    double sumTa = 0, sumWait = 0, sumResp = 0;

    while (finished < n) {
        /* enqueue arrivals up to current */
        while (nextArr < n && p[nextArr].arrivalTime <= current)
            q_push(&rq, (int)nextArr++);

        if (q_empty(&rq)) {
            current = p[nextArr].arrivalTime;
            continue;
        }

        int idx     = q_pop(&rq);
        Process *cur = &p[idx];

        if (cur->startTime < 0) {
            cur->startTime = current;
            sumResp += (cur->startTime - cur->arrivalTime);
        }

        int slice = cur->remainingTime > quantum
                  ? quantum
                  : cur->remainingTime;
        cur->remainingTime -= slice;
        current           += slice;

        /* enqueue newcomers during this slice */
        while (nextArr < n && p[nextArr].arrivalTime <= current)
            q_push(&rq, (int)nextArr++);

        if (cur->remainingTime > 0)
            q_push(&rq, idx);
        else {
            cur->completionTime = current;
            int ta = cur->completionTime - cur->arrivalTime;
            int wt = ta - cur->burstTime;
            sumTa   += ta;
            sumWait += wt;
            ++finished;
        }
    }

    q_destroy(&rq);
    free(p);

    /* build averages */
    Metrics m = make_metrics(sumTa, sumWait, sumResp, n);

    return m;
}
