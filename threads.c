#include "torture.h"

void start_threads(uint64_t threads){
	pthread_t thread[threads];

#ifdef DEBUG
	printf("Creating %"PRIu64" threads\n", threads);
#endif

	for (uint64_t i = 0; i < threads; i++)
		pthread_create(&thread[i], NULL, &worker,  NULL);

#ifdef DEBUG
	printf("Waiting %"PRIu64" threads to finish\n", threads);
#endif
	for (int i = 0; i < threads; i++)
		pthread_join(thread[i], NULL);
}

void init_workers(void) { num_used_threads=0; }

void start_workers(uint64_t workload, uint64_t nr_workers)
{
  for (uint64_t i = 0; i < nr_workers && num_used_threads < THREADS; i++) {
    pthread_create(&thread_pool[i], NULL, &worker, (void *) workload);
    pthread_join(thread_pool[i], NULL);
    num_used_threads++;
  }
}
