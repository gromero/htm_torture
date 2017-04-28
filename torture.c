#include "torture.h"

void register_workload(void *func, char *description) {
	int i = 0;
	while (workloads[i] != NULL)
		i++;
	workloads[i] = func;
	nr_workloads++;
	#ifdef DEBUG
	printf("Registering workload %d (%s)\n", nr_workloads, description ? description : "no description available");
	#endif
}

void set_workloads() {
	memset(workloads, 0, MAX_WORKLOADS);
	nr_workloads = 0;

	register_workload(workload0, "syscall");             // syscall
	register_workload(workload1, "recursion");           // recursion, fib
	register_workload(workload2, "nop");                 // nop
	register_workload(workload3, "tabort.");             // tabort.
	register_workload(workload4, "quicksort");           // quicksort
	register_workload(workload5, "illegal instruction"); // illegal instruction

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

//	for (int i = 0; i < REPEAT; i++)
//		start_threads(threads);

	return 0;
}
