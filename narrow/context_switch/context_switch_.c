#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<htmintrin.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>

void *ping(void *not_used)
{

	uint64_t high;		// aux register to keep high 64bit (MSB) of a 128bit VSX register
	uint64_t low;		// aux register to keep low 64bit (LSB) of a 128bit VSX register
	uint64_t texasr;	// will hold TEXASR value just after an HTM failure ;-)

	sleep(1);		// wait thread to be renamed to "ping". it's not necessary to reproduce the issue, but it's easier to install a selective stap probe using execname() == "ping".

	asm(
		   // r3 = 0x5555555555555555
		   "lis  %[high], 0x5555                  ;"
		   "ori  %[high], %[high], 0x5555         ;"
		   "sldi %[high], %[high], 32             ;"
		   "oris %[high], %[high], 0x5555         ;"
		   "ori  %[high], %[high], 0x5555         ;"
		   // r4 = 0xFFFFFFFFFFFFFFFF
		   "lis  %[low], 0xFFFF                   ;"
		   "ori  %[low], %[low], 0xFFFF           ;"
		   "sldi %[low], %[low], 32               ;"
		   "oris %[low], %[low], 0xFFFF           ;"
		   "ori  %[low], %[low], 0xFFFF           ;"
		   "mtvsrd 33, %[high]                    ;"
		   "mtvsrd 34, %[low]                     ;"
   /********************************************************
    *** ADJUST VSX0 EXPECTED VALUE AFTER AN HTM FAILURE  ***
    ********************************************************/
		   // VSR0  (VSX/FP )                 = 0x5555555555555555555FFFFFFFFFFFFFFFF
		   "xxmrghd 0,  33, 34                    ;"
   /****************************************************************
    *** ADJUST VSX32 (VMX0) EXPECTED VALUE AFTER AN HTM FAILURE  ***
    ****************************************************************/
		   // VSR32 (VSX) = VR0 (VMX/Altivec) = 0x5555555555555555555FFFFFFFFFFFFFFFF
"xxmrghd 32, 33, 34                    ;":	// no  output
		: [high] "r"(high),[low] "r"(low)
		:
	);

	uint64_t i;
	for (i = 0; i < 1024 * 1024 * 512; i++);	// wait an amount of context switches so load_fp and load_vec overflows and MSR.[FP|VEC|VSX] = 0

#ifdef MSR_VEC
	asm("vaddcuw 10, 10, 10;");	// set MSR.VEC = 1 before provoking a VSX unavailable in transactional mode.
#endif

#ifdef MSR_FP
	asm("fadd    10, 10, 10;");	// set MSR.FP = 1 before provoking a VSX unavailable in transaction mode.
#endif

/*** RESULTS BELOW ARE NO DETERMINISTIC SINCE THEY DEPEND UPON ASYNC EVENTS (Decrementer, 0x900) ***/
/*** IT MEANS THAT MOST OF THE TIME ALL REGISTERS (FP AND VEC) WILL BE RESTORED JUST FINE ***/
// If MSR.VEC = 1 => FP (VSX0/FP0) is corrupted,
// If MSR.FP  = 1 => VEC (VSX32/VMX0) is corrupted,
// If MSR.VEC=MSR.FP = 1 => no corruption occurs,
// If MSR.FP=MSR.FP = 0  => both FP and VEC (VSX0-63) are corrupted.

      _do_htm:
	asm("1: tbegin.             ;"	// begin HTM
	    "   beq       3f        ;"	// failure handler, taken after the VSX unavailable exception
	    "   tend.               ;"	// end HTM
	    "2: b 1b                ;"	// failure handler just exits dumping all registers
	    "3: nop                 ;"	// continue to the failure handler below that check for the failure code
	    );

	texasr = __builtin_get_texasr();

	if (_TEXASR_FAILURE_CODE(texasr) == 0xe0) {	// If TM_CAUSE_KVM_RESCHED ignore and retry transaction
		printf(".\n");
		goto _do_htm;
	}

	else {			// otherwise it was a TM_CAUSE_RESCHED (ie aborted due to a context switch, so exit generating a core file)
		printf("\nTEXASR: %" PRIx64 " \n", texasr);
		printf(": Failure with error: %lx\n\n",
		       _TEXASR_FAILURE_CODE(texasr));
		asm(".long 0x0;");	// exit dumping all registers state
	}
}

void *pong(void *not_used)
{
	sleep(1);		// wait pthread to be renamed to "pong". it's not necessary to reproduce the issue, but it's easier to install a selective probe using execname() == "pong".
	while (1) {
		sched_yield();
	}			// pong
}


int main(int argc, char **argv)
{
	pthread_t t0, t1;
	pthread_attr_t attr;
	cpu_set_t cpuset;

	// Set only CPU 0 in the mask. Both thread will be bind to cpu 0
	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);

	// init pthread attribute
	pthread_attr_init(&attr);

	// Set CPU 0 mask into the pthread attribute
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

	pthread_create(&t0, &attr /* bind to cpu 0 */ , ping, NULL);
	pthread_create(&t1, &attr /* bind to cpu 0 */ , pong, NULL);

	pthread_setname_np(t0, "ping");	// for systemtap probe install convenience
	pthread_setname_np(t1, "pong");	// for systemtap probe install convenience

	// threads will never leave after joint but thread performing HTM will issue an illegal instruction and exit
	pthread_join(t0, NULL);
	pthread_join(t1, NULL);
}
