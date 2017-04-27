#include "torture.h"

void register_workload(void *func){
	int i = 0;
	while (workloads[i] != NULL)
		i++;
	workloads[i] = func;
	nr_workloads++;
	#ifdef DEBUG
	printf("Registering workload %d\n", nr_workloads);
	#endif
}

void set_workloads(){
	memset(workloads, 0, MAX_WORKLOADS);
	nr_workloads = 0;

	register_workload(workload0);
	register_workload(workload1);
	register_workload(workload2);
	register_workload(workload3);
	register_workload(workload4);
	register_workload(workload5);
}

	
int main(int argc, char **argv) {
	uint64_t threads;
	uint64_t repeat;

	if (argc == 2)
		threads = atoi(argv[1]);
	else
		threads = THREADS;

	printf("Hardware Transaction Memory Torture\n");
	printf("! = Transaction Failed but the VSX + VRSAVE registers were preserved\n");
	printf(". = Transaction Succeeded but the VSX + VRSAVE registers were preserved\n\n");
	printf("Starting %"PRIu64 " threads\n", threads);
	printf("---------------------------\n");

	set_workloads();

	for (int i = 0; i < REPEAT; i++)
		start_threads(threads);

	return 0;
}
