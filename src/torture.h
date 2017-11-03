#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
#define THREADS 450
#define REPEAT 1
#define MAX_WORKLOADS 20

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
extern void workload9();
extern void workload10();

// Array of workloads functions
void (*workloads[MAX_WORKLOADS])();
// TODO: Create a structure to keep both together
char wname[MAX_WORKLOADS][1024];

// not TLS (.bss)
int nr_workloads;

pthread_t thread_pool[THREADS*MAX_WORKLOADS];
uint64_t  num_used_threads;

// Workload type
#define FAIL -1
#define NON_FAIL -2
#define ALL -255
#define UNINIT ~0


void init_workers(void);
void start_workers(uint64_t, uint64_t);
void join_workers(void);

extern void signal_handler(int signo, siginfo_t *si, void *data);
extern void *worker(void *arg);

// #define DEBUG
