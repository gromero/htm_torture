#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <altivec.h>
#include <pthread.h>
#include <signal.h>

#define _ asm
// Number of threads to randomly distribute each workload.
// It might be the case that for an equal or a small number
// of THREADS is set in comparison to the number of workloads,
// then it might be the case that some workload will not be
// set to run.
#define THREADS 64
#define REPEAT 1
#define MAX_WORKLOADS 9

// This is the array size to be ordered by workload7: uptsm_qsort.
#define ARRAY_SIZE 64

// Thi is the array pointer to be ordered by worload7: uptsm_qsort.
extern unsigned long *array;

extern void workload0();
extern void workload1();
extern void workload2();
extern void workload3();
extern void workload4();
extern void workload5();
extern void workload6();
extern void utpsm_qsort();
extern void workload8();

// Array of workloads functions
void (*workloads[MAX_WORKLOADS])();

// not TLS (.bss)
int nr_workloads;

pthread_t thread_pool[THREADS];
uint64_t  num_used_threads;

void init_workers(void);
void start_workers(uint64_t, uint64_t);

extern void start_threads(uint64_t threads);
extern void *worker(void *arg);

#define DEBUG
