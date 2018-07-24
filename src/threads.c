/*
 * Copyright (c) 2017 Breno Leitao, Gustavo Romero, IBM Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#include "torture.h"
void reinit_workers(void) {
	num_used_threads = 0;
}

void start_workers(uint64_t workload, uint64_t nr_workers)
{
	for (uint64_t i = 0; i < nr_workers; i++) {
		pthread_create(&thread_pool[num_used_threads],
			 NULL, &worker, (void *) workload);
		num_used_threads++;
	}
}

void join_workers(void)
{
	for (uint64_t i = 0; i < num_used_threads; i++)
		pthread_join(thread_pool[i], NULL);

	reinit_workers();
}
