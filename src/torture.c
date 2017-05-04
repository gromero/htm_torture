#include <sys/ipc.h>
#include <sys/shm.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

// #include <inttypes.h>
// #include <htmintrin.h>
//#include <string.h>
// #include <sys/mman.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "torture.h"

void register_workload(void *func, char *description) {
	int i = 0;
	while (workloads[i] != NULL)
		i++;
	workloads[i] = func;
	#ifdef DEBUG
	printf("Registering workload %d (%s)\n", nr_workloads, description ? description : "no description available");
	#endif
	nr_workloads++;
}

void set_workloads() {
	memset(workloads, 0, MAX_WORKLOADS);
	nr_workloads = 0;

	register_workload(workload0, "syscall");             // syscall
	register_workload(workload1, "recursion");           // recursion, fib
	register_workload(workload2, "nop");                 // nop
	register_workload(workload3, "tabort.");             // tabort.
	register_workload(workload4, "quicksort");           // quicksort
	register_workload(workload5, "loop");                // loop
	register_workload(workload6, "infite loop");         // infite loop
	register_workload(utpsm_qsort, "utpsm_qsort");       // utpsm_qsort
	register_workload(workload8, "illegal instruction"); // illegal instruction
	register_workload(workload9, "trap");                // trap

        printf("\n");
}

void signal_handler(int signo, siginfo_t *si, void *data)
{
 // Set a local pointer to uc.
 // ucontext_t *uc = (ucontext_t *)data;

 // printf("* Received a SIGTRAP\n");
 // printf("* si->si_addr = %p\n", si->si_addr);
 // printf("* uc->uc_mcontext.regs->nip: %p\n", (void *) uc->uc_mcontext.regs->nip);

 // uc->uc_mcontext.regs->nip += 4; // Skip illegal instruction.
 // uc->uc_mcontext.gp_regs[32] += 4; // Same as above ;-)
}

	
int main(int argc, char **argv) {
	uint64_t threads;
	uint64_t repeat;

	if (argc == 2)
		threads = atoi(argv[1]);
	else
		threads = THREADS;

	printf("Hardware Transactional Memory Torture\n\n");
	printf("! = Transaction Failed but the VSX + VRSAVE registers were preserved\n");
	printf(". = Transaction Succeeded (in that case VSX + VRSAVE registers are not checked\n\n");

//	printf("Starting %"PRIu64 " threads\n", threads);
//	printf("---------------------------\n");

	// Install signal handler for all threads.
        struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = signal_handler;
	sigaction(SIGTRAP, &sa, NULL);
	sigaction(SIGILL,  &sa, NULL);


	set_workloads();




     int shmid;
     key_t key;
     char *shm, *s;

     key = 5678;

     if ((shmid = shmget(key, ARRAY_SIZE, IPC_CREAT | 0666)) < 0) {
         perror("shmget");
         exit(1);
     }
     
     if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
         perror("shmat");
         exit(1);
     }
     
     s = shm;


//      int nr_threads = THREADS / MAX_WORKLOADS;
//      printf("Thread per worload type: %d\n", nr_threads);

 //       while (1) {

        /**** WORKLOADS ****/

        start_worker_process(0, 2);
        start_worker_process(1, 2);
//          start_worker_process(7, 10); 
         start_worker_process(2, 2);
        start_worker_process(3, 2);
        start_worker_process(4, 2);
        start_worker_process(5, 2);
        start_worker_process(6, 2);

//        init_workers();

//         nr_threads = 450;

//        start_workers(0, nr_threads);
//        start_workers(1, nr_threads);
//        start_workers(2, nr_threads);
//        start_workers(3, nr_threads);
//        start_workers(4, nr_threads);
//        start_workers(5, nr_threads);
//        start_workers(6, nr_threads);

        // Preparation for workload7. TODO: improve argument passing to workloads like that.
//         array = (unsigned long *) calloc(ARRAY_SIZE, sizeof(unsigned long));
         array = (unsigned long *) s;   
        // Fill array with pseudo-random values.
         for (uint64_t i = 0; i < ARRAY_SIZE; i++) array[i] = rand();

        // Order array.
//        start_workers(7, nr_threads); //utpsm_qsort
        start_worker_process(7, 2);

       start_worker_process(8, 2);
        start_worker_process(9, 2);

//        start_workers(8, nr_threads); // Illegal instruction
//        start_workers(9, nr_threads); // trap


//        join_workers();

        /*******************/

//        } // while (1)
/*
        // Print ordered array
        for (int i = 0; i < ARRAY_SIZE; i++) printf("%ld\n", array[i]);
*/

//	for (int i = 0; i < REPEAT; i++)
//		start_threads(threads);

	return 0;
}
