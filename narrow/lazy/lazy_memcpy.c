#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>

int passed;

void* ping(void *not_used) {
	float *pi;
	float *pix;
	
	float c;
	float b = 3.1415;

	pi = &b;
	pix = &c;

	sleep(1);
	asm (
		// Wait about 10s so we have a sufficient amount of context
		// switches so load_fp and load_vec overflow and MSR.FP, MSR.VEC,
		// and MSR.VSX are disabled.
		"       lis	 7, 0x1       ;"
		"       ori      7, 7, 0xBFFE ;"
		"       sldi     7, 7, 15     ;"
		"1:	addi     7, 7, -1     ;"
	        "       cmpdi    7, 0         ;"
	        "       bne      1b           ;"

		// Any floating-point instruction in here so we enter in TM
		// when MSR.FP=1. MSR.VEC and MSR.VSX are disabled.
	        // N.B. 'fmr' is *not touching* any previously set register,
		// i.e. it's not touching vs0 (previously set).

		// vs0 is *still* 0x5555555555555555FFFFFFFFFFFFFFFF, right?
		// So let's get in a transaction and cause a VSX unavailable exception.
		"2:	tbegin.               ;" // Begin HTM
		"       beq      3f           ;" // Failure handler
		"       lfs    0, 0(%[pi]) ;"
		"       tabort.	0	      ;"
		"       tend.                 ;" // End HTM
		"3:     nop                   ;" // Fall through to code below to check vs0

		// Immediately after a transaction failure we save vs0 to two
		// general purpose registers to check its value. We need to have the
		// same value as before we entered in transactional state.

		"stfs   0, 0(%[pix]) ;"
		:
		: [pi] "r" (pi), [pix] "r" (pix)
		: "r3", "r4", "vs33", "vs34", "vs0", "vs10", "fr10", "r7", "r5", "r6", "vs3"
		);

		printf("pi %f pix %f\n", *pi, *pix);
		return NULL;
}

void *pong(void *not_used)
{
	while(1) sched_yield(); // will be classed as a interactive-like thread
}

int main(int argc, char **argv)
{
	pthread_t t0, t1;
	pthread_attr_t attr;
	cpu_set_t cpuset;

	// Set only CPU 0 in the mask. Both threads will be bound to cpu 0
	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);

	// Init pthread attribute
	pthread_attr_init(&attr);

	// Set CPU 0 mask into the pthread attribute
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

	// 'pong' thread used to induce more context switches then usual on 'ping' thread
	pthread_create(&t1, &attr /* bound to cpu 0 */, pong, NULL);

	printf("Checking if FP/VSX register is sane after a VSX unavailable exception in TM...\n");

	pthread_create(&t0, &attr /* bound to cpu 0 as well */, ping, NULL);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	if (passed) { printf("result: passed\n"); exit(0); }
	else { printf("result: failed!\n"); exit(1); } 
}
