#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>

struct Flags {
	int test_fp;
	int test_vec;
	int result;
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
	for(counter=0; counter < 1024*1024*512; counter++);

	if (flags.test_fp)
                // set MSR.FP = 1 before provoking a VSX unavailable in TM
		asm("fadd    10, 10, 10;");
	if (flags.test_vec)
		// set MSR.VEC = 1 before provoking a VSX unavailable in TM
		asm("vaddcuw 10, 10, 10;");

	// TODO: remove that comment when porting to kernel test harness
	// If MSR.VEC = 1 => VSX32/VMX0 is corrupted,
	// If MSR.FP  = 1 => VSX0/FP0 is corrupted,
	// If MSR.VEC=MSR.FP=1 => restore_math() called from trap 0x900 (Decrementer) will set also MSR.VSX=1 so
	// the VSX unavailable exception in TM never happens and HTM commits the change (xxmrghd) without failing.

	asm (
		"1: tbegin.            ;" // begin HTM
		"   beq      2f        ;" // failure handler
		"   xxmrghd  10, 10, 10;" // any VSX instruction in here
		"   tend.              ;" // end HTM
		"2: nop                ;" // fall through to failure handler below

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

	asm (   // Get vs32 value in high and low
		"mfvsrd  %[high], 32       ;"
		"xxsldwi       3, 32, 32, 2;"
		"mfvsrd   %[low], 3        ;"

		: [high] "=r"(high),
		  [low]  "=r"(low)
		);

	// Check VEC (vs32)
	if (high != 0x5555555555555555 || low != 0xFFFFFFFFFFFFFFFF) {
		printf("VEC corrupted!\n");
	} else
		printf("VEC ok\n");
}

void *pong(void *not_used)
{
	sleep(1); // wait thread get its name "pong"
	while(1) sched_yield(); // classed as a interactive-like thread
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

	flags.result=0;

	printf("Checking if FP/VEC/VSX registers are sane after a VSX exception...\n");

	flags.test_fp=0;
	flags.test_vec=0;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	flags.test_fp=1;
	flags.test_vec=0;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	flags.test_fp=0;
	flags.test_vec=1;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	flags.test_fp=1;
	flags.test_vec=1;
	pthread_create(&t0, &attr /* bind to cpu 0 */, ping, (void *) &flags);
	pthread_setname_np(t0, "ping"); // name it for systemtap convenience
	pthread_join(t0, NULL);

	if (flags.result) {
		printf("result: failed!\n");
		exit(1);
	} else {
		printf("result: sucess\n");
		exit(0);
	}
}
