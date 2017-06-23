#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>

int passed;

void* ping(void *not_used) {

	asm goto (
		// r3 = 0x5555555555555555
		"lis  3, 0x5555    ;"
		"ori  3, 3, 0x5555 ;"
		"sldi 3, 3, 32     ;"
		"oris 3, 3, 0x5555 ;"
		"ori  3, 3, 0x5555 ;"

		//r4 = 0xFFFFFFFFFFFFFFFF
		"lis  4, 0xFFFF    ;"
		"ori  4, 4, 0xFFFF ;"
		"sldi 4, 4, 32     ;"
		"oris 4, 4, 0xFFFF ;"
		"ori  4, 4, 0xFFFF ;"

		// vs33 and vs34 will just be used to construct vs0 from r3 and
                // r4. Both won't be used in any other place after that.
		"mtvsrd 33, 3      ;"
		"mtvsrd 34, 4      ;"

		// vs0 = (r3 || r4) = 0x5555555555555555FFFFFFFFFFFFFFFF
		"xxmrghd 0, 33, 34 ;"

		// Any floating-point instruction in here.
	        // N.B. 'fmr' is *not touching* any previously set register,
		// i.e. it's not touching vs0.
		"fmr    10, 10     ;"

		// Wait about 10s so we have a sufficient amount of context
		// switches so load_fp and load_vec overflow and MSR.FP, MSR.VEC,
		// and MSR.VSX are disabled.
		"       lis	 7, 0x1       ;"
		"       ori      7, 7, 0xBFFE ;"
		"       sldi     7, 7, 17     ;"
		"1:	addi     7, 7, -1     ;"
	        "       cmpdi    7, 0         ;"
	        "       bne      1b           ;"

		// vs0 is *still* 0x5555555555555555FFFFFFFFFFFFFFFF, right?
		// So let's get in a transaction and cause a VSX unavailable exception.
		"2:	tbegin.               ;" // Begin HTM
		"       beq      3f           ;" // Failure handler
		"       xxmrghd  10, 10, 10   ;" // VSX unavailable in TM. Any VSX instr
		"       tend.                 ;" // End HTM
		"3:     nop                   ;" // Fall through to code below to check vs0

		// Immediately after a transaction failure we save vs0 to two
		// general purpose registers to check its value. We need to have the
		// same value as before we entered in transactional state.

		// vs0 should be *still* 0x5555555555555555FFFFFFFFFFFFFFFF, right?

		// Save high half - MSB (64bit) - from vs0 after transaction to r5
		"mfvsrd  5, 0                 ;"

		// Save low half - LSB (64bit) - from vs0 after transaction to r6
		// We mess with vs3, but it's not important.
		"xxsldwi 3, 0, 0, 2           ;"
		"mfvsrd  6, 3                 ;"

		// N.B. r3 and r4 never changed since they were used to
		// construct the initial vs0 value, hence we can use them to do the
		// comparison. r3 and r4 will be destroy but it's not important.
		"and. 3, 3, 5                 ;" // compare r3 to r5
		"bne   %[value_mismatch]      ;"
		"and. 4, 4, 6                 ;" // compare r4 to r6
		"bne   %[value_mismatch]      ;"
		"b     %[value_ok]            ;"
		:
		:
		: "r3", "r4", "vs33", "vs34", "vs0", "vs10", "fr10", "r7", "r5", "r6", "vs3"
		: value_mismatch, value_ok
		);
value_mismatch:
		passed = 0;
		return NULL;
value_ok:
		passed = 1;
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
	pthread_join(t0, NULL);

	if (passed) { printf("result: passed\n"); exit(0); }
	else { printf("result: failed!\n"); exit(1); } 
}
