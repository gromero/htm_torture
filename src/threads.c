#include "torture.h"
void reinit_workers(void) {
	num_used_threads=0;
}

void start_workers(uint64_t workload, uint64_t nr_workers)
{
	for (uint64_t i = 0; i < nr_workers; i++) {
		pthread_create(&thread_pool[num_used_threads], NULL, &worker, (void *) workload);
		num_used_threads++;
	}
}

void join_workers(void)
{
	for (uint64_t i = 0; i < num_used_threads; i++)
		pthread_join(thread_pool[i], NULL);

	reinit_workers();
}
