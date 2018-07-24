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

void register_workload(void *func, char *description)
{
	workloads[nr_workloads] = func;
	strncpy(&wname[nr_workloads][0], description, 1024);

	if (debug)
		printf("Registering workload %d (%s)\n",
			nr_workloads, description ?
			description : "no description available");

	nr_workloads++;
}

void list_workloads()
{
	int i;

	for (i = 0 ; i < nr_workloads ; i++){
		printf("%d:	: %s\n", i, (char *) &wname[i]);
	}
}

void set_workloads()
{
	memset(workloads, 0, MAX_WORKLOADS*sizeof(void *));
	nr_workloads = 0;

	register_workload(workload0, "syscall");
	register_workload(workload1, "recursion");
	register_workload(workload2, "nop");
	register_workload(workload3, "tabort.");
	register_workload(workload4, "quicksort");
	register_workload(workload5, "loop");
	register_workload(workload6, "infite loop");
	
	// Preparation for workload7. TODO: improve argument passing to
	// workloads like that.
	array = (unsigned long *) calloc(ARRAY_SIZE, sizeof(unsigned long));

        // Fill array with pseudo-random values.
        for (uint64_t i = 0; i < ARRAY_SIZE; i++) array[i] = rand();

        // Order array.
	register_workload(utpsm_qsort, "utpsm_qsort");       // utpsm_qsort

	register_workload(workload8, "illegal instruction"); // illegal instruction
	register_workload(workload9, "trap");                // trap
	register_workload(workload10, "dscr");                // dscr load restore

        printf("\n");
}

int main(int argc, char **argv)
{
	int runworkload = UNINIT;
	int infinityrun = 0;
        int nr_threads =  THREADS;
	// Parse opt
	int opt;
	// Install signal handler for all threads.
        struct sigaction sa;

	// Debug initially disabled
	debug = 0;

	printf("Hardware Transactional Memory Torture\n");
	printf("-------------------------------------\n");
	if (debug) {
		printf("! = Transaction Failed but the VSX + VRSAVE registers were preserved\n");
		printf(". = Transaction Succeeded (in that case VSX + VRSAVE registers are not checked\n\n");
	}


	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = signal_handler;
	sigaction(SIGTRAP, &sa, NULL);
	sigaction(SIGILL,  &sa, NULL);

	set_workloads();

	while ((opt = getopt(argc, argv, "vcfials:n:")) != -1) {
		if (opt == 'l') {
			//List all options
			printf("Listing all workloads\n");
			list_workloads();
			exit(0);
		} else if (opt == 'v') {
			debug = 1;
		} else if (opt == 'i') {
			infinityrun = 1;
		} else if (opt == 'f') {
			runworkload = FAIL;
		} else if (opt == 'c') {
			runworkload = NON_FAIL;
		} else if (opt == 's') {
			int w = atoi(optarg);
			if (w > nr_workloads) {
				printf("Workload %d does not exist\n\n These are the available workloads:\n", w);
				list_workloads();
				exit(2);
			}
			printf("Running just workload %d (%s) \n", w, (char *) &wname[w]);
			runworkload = atoi(optarg);
		} else if (opt == 'a') {
			printf("Running all workloads\n");
			runworkload = ALL;
		} else if (opt == 'n') {
			nr_threads = atoi(optarg);
		}
	}

	if (infinityrun && runworkload == UNINIT)
		runworkload = ALL;

	if (runworkload == UNINIT) {
		printf("HTM torture\n");
		printf(" -a	: Run all workloads\n");
		printf(" -s	: Run just a single workload\n");
		printf(" -l	: List all workloads\n");
		printf(" -i	: Run forever\n");
		printf(" -n	: Amount of threads\n");
		printf(" -c	: Only workloads that commit\n");
		printf(" -f	: Only workloads that fails\n");
		printf(" -v	: Be verbose\n");
		exit(1);
	}

	if (debug)
		printf("Thread per worload type: %d\n", nr_threads);

	do {
		if (runworkload == ALL) {
			printf("Starting all workloads\n");
			start_workers(0, nr_threads);
			start_workers(1, nr_threads);
			start_workers(2, nr_threads);
			start_workers(3, nr_threads);
			start_workers(4, nr_threads);
			start_workers(5, nr_threads);
			start_workers(6, nr_threads);
			start_workers(7, nr_threads);
			start_workers(8, nr_threads);
			start_workers(9, nr_threads);
			start_workers(10, nr_threads);
		} else if (runworkload == FAIL) {
			printf("Starting workloads which the transactions will fail\n");
			start_workers(0, nr_threads);
			start_workers(1, nr_threads);
			start_workers(3, nr_threads);
			start_workers(6, nr_threads);
			start_workers(8, nr_threads);
			start_workers(9, nr_threads);
		} else if (runworkload == NON_FAIL) {
			printf("Starting workloads which the transactions will commit\n");
			start_workers(2, nr_threads);
			start_workers(4, nr_threads);
			start_workers(5, nr_threads);
			start_workers(7, nr_threads);
			start_workers(10, nr_threads);
		} else {
			start_workers(runworkload, nr_threads);
		}
		join_workers();
	} while (infinityrun);

	printf("\nStatistics:\n");
	if (debug) {
		printf("TM fails  : %ld\n", tm_fails);
		printf("TM commits: %ld\n", tm_commit);
	}
	printf("Fail Percentage: %1.2f %% \n",
		(float) 100*tm_fails/(tm_fails + tm_commit));

}
