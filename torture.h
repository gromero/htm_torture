#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <altivec.h>
#include <pthread.h>

#define _ asm
// Number of threads to randomly distribute each workload.
// It might be the case that for an equal or a small number
// of THREADS is set in comparison to the number of workloads,
// then it might be the case that some workload will not be
// set to run.
#define THREADS 6
#define REPEAT 1
#define MAX_WORKLOADS 6

extern void workload0();
extern void workload1();
extern void workload2();
extern void workload3();
extern void workload4();
extern void workload5();

// Array of workloads functions
void (*workloads[MAX_WORKLOADS])();

// not TLS (.bss)
int nr_workloads;

extern void start_threads(uint64_t threads);
extern void *worker(void *arg);


#define DEBUG
