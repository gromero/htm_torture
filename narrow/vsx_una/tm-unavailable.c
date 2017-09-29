#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>


// Unavailable exceptions to test in HTM
#define FP_UNA_EXCEPTION	0
#define VEC_UNA_EXCEPTION	1
#define VSX_UNA_EXCEPTION	2

struct Flags {
	int test_fp;
	int test_vec;
	int result;
	int exception;
} flags;

void *ping(void *input)
{

	/* Temporary registers to keep high/low 64bit half of a VSX register */
	uint64_t high;
	uint64_t low;

	uint64_t counter;

	struct Flags *f = (struct Flags *) input;

	/* Wait a bit so thread can get its name "ping".
	 * This is not important to reproduce the issue but
	 * it's nice to have for systemtap observance ;-)
	 */
	sleep(1);

	asm (
		// high = 0x5555555555555555
		"lis  %[high], 0x5555         ;"
		"ori  %[high], %[high], 0x5555;"
		"sldi %[high], %[high], 32    ;"
		"oris %[high], %[high], 0x5555;"
		"ori  %[high], %[high], 0x5555;"

		// low = 0xFFFFFFFFFFFFFFFF
		"lis  %[low], 0xFFFF          ;"
		"ori  %[low], %[low], 0xFFFF  ;"
		"sldi %[low], %[low], 32      ;"
		"oris %[low], %[low], 0xFFFF  ;"
		"ori  %[low], %[low], 0xFFFF  ;"

		// prepare to merge low and high
		"mtvsrd 33, %[high]           ;"
		"mtvsrd 34, %[low]            ;"

		/********************************************************
		 *** ADJUST vs0 EXPECTED VALUE AFTER AN HTM FAILURE   ***
		 ********************************************************/
		// vs0 = 0x5555555555555555555FFFFFFFFFFFFFFFF
		"xxmrghd 0,  33, 34           ;"

		/********************************************************
		 *** ADJUST vs32 EXPECTED VALUE AFTER AN HTM FAILURE  ***
		 ********************************************************/
		// vs32 = 0x5555555555555555555FFFFFFFFFFFFFFFF
		"xxmrghd 32, 33, 34           ;"

		:
		: [high] "r" (high),
		  [low]  "r" (low)
		);

	printf("If MSR.FP=%d MSR.VEC=%d: ", flags.test_fp, flags.test_vec);

	/* Wait an amount of context switches so load_fp and load_vec
	 * overflow and MSR.FP, MSR.VEC, and MSR.VSX become zero (off).
	 */
	for (counter = 0; counter < 1024*1024*512; counter++)
		// Do nothing
		;

	if (flags.test_fp)
		// set MSR.FP = 1 before provoking a unavailable in TM
		// exception
		asm("fadd    10, 10, 10;");
	if (flags.test_vec)
		// set MSR.VEC = 1 before provoking a unavailable in TM
		// exception
		asm("vaddcuw 10, 10, 10;");

	if (flags.exception == FP_UNA_EXCEPTION) {
		asm (
			"1: tbegin.            ;" // begin HTM
			"   beq      2f        ;" // failure handler
			// Any FP instruction in here
			"   fadd    10, 10, 10 ;"
			"   tend.              ;" // end HTM
			// fall through to checking below
			"2: nop                ;"
		:::);

	} else if (flags.exception == VEC_UNA_EXCEPTION) {
		asm (
			"1: tbegin.            ;"
			"   beq      2f        ;"
			"   fadd    10, 10, 10 ;"
			// Any Vector instruction in here
			"   vaddcuw 10, 10, 10 ;"
			"   tend.              ;"
			"2: nop                ;"
	 :::);

	} else if (flags.exception == VSX_UNA_EXCEPTION) {
		asm (
			"1: tbegin.            ;"
			"   beq      2f        ;"
			// Any VSX instruction in here
			"   xxmrghd  10, 10, 10;"
			"   tend.              ;"
			"2: nop                ;"
			:::);
	} else {
		printf("No valid exception specified to test.\n");
		return NULL;
	}

	// Check vs0 (FP set) and vs32 (VEC set)
	asm (
		// Get vs0 value in high and low
		"mfvsrd  %[high], 0      ;"
		"xxsldwi       3, 0, 0, 2;"
		"mfvsrd   %[low], 3      ;"

		: [high] "=r"(high),
		  [low]  "=r"(low)
		);

	// Check FP (vs0)
	if (high != 0x5555555555555555 || low != 0xFFFFFFFFFFFFFFFF) {
		printf("FP corrupted! ");
		flags.result++;
	} else
		printf("FP ok ");

	asm (
		// Get vs32 value in high and low
		"mfvsrd  %[high], 32       ;"
		"xxsldwi       3, 32, 32, 2;"
		"mfvsrd   %[low], 3        ;"

		: [high] "=r"(high),
		  [low]  "=r"(low)
		);

	// Check VEC (vs32)
	if (high != 0x5555555555555555 || low != 0xFFFFFFFFFFFFFFFF)
		printf("VEC corrupted!\n");
	else
		printf("VEC ok\n");
}

void *pong(void *not_used)
{
	// wait thread get its name "pong"
	sleep(1);

	// classed as a interactive-like thread;
	while (1)
		sched_yield();
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

	pthread_create(&t1, &attr /* bind to cpu 0 */, pong, NULL);
	pthread_setname_np(t1, "pong"); // name it for systemtap convenience

	flags.result = 0;
	int exception; // FP = 0, VEC = 1, VSX = 2

	for (exception = 0; exception <= 2; exception++) {
		printf("Checking if FP/VEC registers are sane after");

	if (exception == FP_UNA_EXCEPTION)
		printf(" a FP unavailable exception...\n");

	else if (exception == VEC_UNA_EXCEPTION)
		printf(" a VEC unavailable exception...\n");

	else
		printf(" a VSX unavailable exception...\n");

	flags.exception = exception;

	flags.test_fp = 0;
	flags.test_vec = 0;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	flags.test_fp = 1;
	flags.test_vec = 0;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping");
	pthread_join(t0, NULL);

	flags.test_fp = 0;
	flags.test_vec = 1;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	flags.test_fp = 1;
	flags.test_vec = 1;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	}

	if (flags.result) {
		printf("result: failed!\n");
		exit(1);
	} else {
		printf("result: success\n");
		exit(0);
	}
}
