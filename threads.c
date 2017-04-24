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
