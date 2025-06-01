#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "scheduler.h"

#define TOLERANCE 0.1

// Helper function to compare floats within a tolerance.
int floatEquals(float a, float b, float tol) {
    return fabs(a - b) <= tol;
}

// Runs a single test case.
void run_test_case(const char *testName, Process procs[], int n, int quantum,
                   Metrics expectedFcfs, Metrics expectedSjf, Metrics expectedRr) {
    printf("==== %s ====\n", testName);

    Metrics mFcfs = fcfs_metrics(procs, n);
    Metrics mSjf  = sjf_metrics(procs, n);
    Metrics mRr   = rr_metrics(procs, n, quantum);

    // FCFS Results
    printf("FCFS: Calculated: Turnaround: %.2f, Waiting: %.2f, Response: %.2f\n",
           mFcfs.avgTurnaround, mFcfs.avgWaiting, mFcfs.avgResponse);
    printf("      Expected:   Turnaround: %.2f, Waiting: %.2f, Response: %.2f\n",
           expectedFcfs.avgTurnaround, expectedFcfs.avgWaiting, expectedFcfs.avgResponse);
    assert(floatEquals(mFcfs.avgTurnaround, expectedFcfs.avgTurnaround, TOLERANCE));
    assert(floatEquals(mFcfs.avgWaiting,    expectedFcfs.avgWaiting,    TOLERANCE));
    assert(floatEquals(mFcfs.avgResponse,   expectedFcfs.avgResponse,   TOLERANCE));

    // SJF Results
    printf("SJF:  Calculated: Turnaround: %.2f, Waiting: %.2f, Response: %.2f\n",
           mSjf.avgTurnaround, mSjf.avgWaiting, mSjf.avgResponse);
    printf("      Expected:   Turnaround: %.2f, Waiting: %.2f, Response: %.2f\n",
           expectedSjf.avgTurnaround, expectedSjf.avgWaiting, expectedSjf.avgResponse);
    assert(floatEquals(mSjf.avgTurnaround, expectedSjf.avgTurnaround, TOLERANCE));
    assert(floatEquals(mSjf.avgWaiting,    expectedSjf.avgWaiting,    TOLERANCE));
    assert(floatEquals(mSjf.avgResponse,   expectedSjf.avgResponse,   TOLERANCE));

    // RR Results
    printf("RR (Quantum = %d): Calculated: Turnaround: %.2f, Waiting: %.2f, Response: %.2f\n",
           quantum, mRr.avgTurnaround, mRr.avgWaiting, mRr.avgResponse);
    printf("         Expected:   Turnaround: %.2f, Waiting: %.2f, Response: %.2f\n",
           expectedRr.avgTurnaround, expectedRr.avgWaiting, expectedRr.avgResponse);
    assert(floatEquals(mRr.avgTurnaround, expectedRr.avgTurnaround, TOLERANCE));
    assert(floatEquals(mRr.avgWaiting,    expectedRr.avgWaiting,    TOLERANCE));
    assert(floatEquals(mRr.avgResponse,   expectedRr.avgResponse,   TOLERANCE));

    printf(">>> %s PASSED.\n\n", testName);
}

int main(void) {
    // disable buffering so prints appear immediately
    setvbuf(stdout, NULL, _IONBF, 0);

    // --- Test Case 1 ---
    Process test1[3] = {
        {1, 0, 10, 0, 0, 0},
        {2, 1,  5, 0, 0, 0},
        {3, 2,  8, 0, 0, 0}
    };
    int n1 = 3, quantum1 = 4;
    Metrics expectedFcfs1 = {15.0, 7.33, 7.33};
    Metrics expectedSjf1  = expectedFcfs1;
    Metrics expectedRr1   = {19.33, 11.67, 3.0};
    run_test_case("Test Case 1", test1, n1, quantum1,
                  expectedFcfs1, expectedSjf1, expectedRr1);

    // --- Test Case 2 ---
    Process test2[3] = {
        {1, 2, 6, 0, 0, 0},
        {2, 5, 2, 0, 0, 0},
        {3, 8, 4, 0, 0, 0}
    };
    int n2 = 3, quantum2 = 2;
    Metrics expectedFcfs2 = {5.67, 1.67, 1.67};
    Metrics expectedSjf2  = expectedFcfs2;
    Metrics expectedRr2   = {5.67, 1.67, 1.0};
    run_test_case("Test Case 2", test2, n2, quantum2,
                  expectedFcfs2, expectedSjf2, expectedRr2);

    // --- Test Case 3 ---
    Process test3[4] = {
        {1, 0, 4, 0, 0, 0},
        {2, 1, 3, 0, 0, 0},
        {3, 2, 1, 0, 0, 0},
        {4, 3, 2, 0, 0, 0}
    };
    int n3 = 4, quantum3 = 2;
    Metrics expectedFcfs3 = {5.75, 3.25, 3.25};
    Metrics expectedSjf3  = {5.0, 2.5, 2.5};
    Metrics expectedRr3   = {6.25, 3.75, 1.75};
    run_test_case("Test Case 3", test3, n3, quantum3,
                  expectedFcfs3, expectedSjf3, expectedRr3);

    // --- Test Case 4 ---
    Process test4[5] = {
        {1, 0, 3, 0, 0, 0},
        {2, 2, 6, 0, 0, 0},
        {3, 4, 4, 0, 0, 0},  
        {4, 6, 5, 0, 0, 0},
        {5, 8, 2, 0, 0, 0}
    };
    int n4 = 5, quantum4 = 3;
    Metrics expectedFcfs4 = {8.60, 4.60, 4.60};
    Metrics expectedSjf4  = {7.60, 3.60, 3.60};
    Metrics expectedRr4   = {10.6, 6.6, 2.6};
    run_test_case("Test Case 4", test4, n4, quantum4,
                  expectedFcfs4, expectedSjf4, expectedRr4);

    // --- Test Case 5 ---
    Process test5[1] = {{1, 0, 5, 0, 0, 0}};
    int n5 = 1, quantum5 = 2;
    Metrics expectedFcfs5 = {5.0, 0.0, 0.0};
    run_test_case("Test Case 5 (Single)", test5, n5, quantum5,
                  expectedFcfs5, expectedFcfs5, expectedFcfs5);

    // --- Test Case 6 ---
    Process test6[2] = {{1,0,4,0,0,0},{2,0,6,0,0,0}};
    int n6 = 2, quantum6 = 3;
    Metrics expectedFcfs6 = {7.0, 2.0, 2.0};
    Metrics expectedRr6   = {8.5, 3.5, 1.5};
    run_test_case("Test Case 6", test6, n6, quantum6,
                  expectedFcfs6, expectedFcfs6, expectedRr6);

    // --- Test Case 7 ---
    Process test7[4] = {
        {1,0,8,0,0,0},
        {2,1,4,0,0,0},
        {3,2,9,0,0,0},
        {4,3,5,0,0,0}
    };
    int n7 = 4, quantum7 = 3;
    Metrics expectedFcfs7 = {15.25, 8.75, 8.75};
    Metrics expectedSjf7  = {14.25, 7.75, 7.75};
    Metrics expectedRr7   = {20.0, 13.5, 3.0};
    run_test_case("Test Case 7", test7, n7, quantum7,
                  expectedFcfs7, expectedSjf7, expectedRr7);

    // --- Test Case 8 ---
    Process test8[6] = {
        {1,0,10,0,0,0},
        {2,2,3,0,0,0},
        {3,4,7,0,0,0},
        {4,6,5,0,0,0},
        {5,7,2,0,0,0},
        {6,9,8,0,0,0}
    };
    int n8 = 6, quantum8 = 4;
    Metrics expectedFcfs8 = {17.0, 11.17, 11.17};
    Metrics expectedSjf8  = {15.17, 9.33, 9.33};
    Metrics expectedRr8   = {20.67, 14.83, 6.33};
    run_test_case("Test Case 8", test8, n8, quantum8,
                  expectedFcfs8, expectedSjf8, expectedRr8);

    printf("ALL TESTS PASSED.\n");
    return 0;
}
