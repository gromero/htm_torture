#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <altivec.h>
#include <pthread.h>

#define _ asm
#define THREADS 5
#define REPEAT 1024
#define MAX_WORKLOADS 5

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
