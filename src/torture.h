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

extern void scall();
extern void fibonacci();
extern void nop();
extern void tabort();
extern void ownqsort();
extern void loop();
extern void iloop();
extern void utpsm_qsort();
extern void illegal();
extern void trap();
extern void dscr();

// Array of workloads functions
void (*workloads[MAX_WORKLOADS])();
// TODO: Create a structure to keep both together
char wname[MAX_WORKLOADS][1024];

// not TLS (.bss)
int nr_workloads;
long tm_fails;
long tm_commit;
int debug;

pthread_t thread_pool[THREADS*MAX_WORKLOADS*4096];
uint64_t  num_used_threads;

// Workload type
#define UNINIT -1
#define FAIL -2
#define NON_FAIL -3
#define ALL -4


void init_workers(void);
void start_workers(uint64_t, uint64_t);
void join_workers(void);

extern void signal_handler(int signo, siginfo_t *si, void *data);
extern void *worker(void *arg);

// #define DEBUG
