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

        printf("\n");
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
	printf(". = Transaction Succeeded but the VSX + VRSAVE registers were preserved\n\n");

//	printf("Starting %"PRIu64 " threads\n", threads);
//	printf("---------------------------\n");

	set_workloads();

        init_workers();
        start_workers(0, 1);
        start_workers(1, 1);
        start_workers(2, 1);
        start_workers(3, 1);
        start_workers(4, 1);
        start_workers(5, 1);
        start_workers(6, 1);

        // Preparation for workload7. TODO: improve argument passing to workloads like that.
        array = (unsigned long *) calloc(ARRAY_SIZE, sizeof(unsigned long));

        // Fill array with pseudo-random values.
        for (uint64_t i = 0; i < ARRAY_SIZE; i++) array[i] = rand();

        // Order array
        start_workers(7, 1);

/*
        // Print ordered array
        for (int i = 0; i < ARRAY_SIZE; i++) printf("%ld\n", array[i]);
*/

//	for (int i = 0; i < REPEAT; i++)
//		start_threads(threads);

	return 0;
}
