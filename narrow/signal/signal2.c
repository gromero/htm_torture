#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<htmintrin.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>

pthread_t ping_tid;
pthread_t pong_tid;

void *ping(void *not_used)
{

uint64_t high; // aux register to keep high 64bit (MSB) of a 128bit VSX register
uint64_t low;  // aux register to keep low 64bit (LSB) of a 128bit VSX register
uint64_t texasr; // will hold TEXASR value just after an HTM failure ;-)

sleep(1); // wait thread to be renamed to "ping". it's not necessary to reproduce the issue, but it's easier to install a selective stap probe using execname() == "ping".

asm (
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

   // VSR0  (VSX/FP )                 = 0x5555555555555555555FFFFFFFFFFFFFFFF
   "xxmrghd 0,  33, 34                    ;"

   // VSR32 (VSX) = VR0 (VMX/Altivec) = 0x5555555555555555555FFFFFFFFFFFFFFFF
   "xxmrghd 32, 33, 34                    ;"


   : // no  output
   : [high] "r" (high),
     [low]  "r" (low)
   :
   );

uint64_t i;
for(i=0; i<1024*1024*512; i++); // wait an amount of context switches so load_fp and load_vec overflows and MSR.[FP|VEC|VSX] = 0

#ifdef MSR_VEC
 asm("vaddcuw 10, 10, 10;"); // set MSR.VEC = 1 before provoking a VSX unavailable in transactional mode.
#endif

#ifdef MSR_FP
asm("fadd    10, 10, 10;"); // set MSR.FP = 1 before provoking a VSX unavailable in transaction mode.
#endif

/*** RESULTS BELOW ARE NO DETERMINISTIC SINCE THEY DEPEND UPON ASYNC EVENTS (Decrementer, 0x900) ***/
/*** IT MEANS THAT MOST OF THE TIME ALL REGISTERS (FP AND VEC) WILL BE RESTORED JUST FINE ***/
// If MSR.VEC = 1 => FP (VSX0/FP0) is corrupted,
// If MSR.FP  = 1 => VEC (VSX32/VMX0) is corrupted,
// If MSR.VEC=MSR.FP = 1 => no corruption occurs,
// If MSR.FP=MSR.FP = 0  => both FP and VEC (VSX0-63) are corrupted.

_do_htm:
asm goto (
   "1: tbegin.             ;" // begin HTM
   "   beq       3f        ;" // failure handler, taken after the VSX unavailable exception
   "   trap                ;" // get into singal handler
   "   tend.               ;" // end HTM
   "2: b 1b                ;" // failure handler just exits dumping all registers
   "3:     nop             ;" // Fall through to code below to check vs0
//              "       .long 0x0             ;" // Uncomment to generate a core dump

                // Immediately after a transaction failure we save vs0 to two
                // general purpose registers to check its value. We need to have the
                // same value as before we entered in transactional state.

                // vs0 should be *still* 0x5555555555555555FFFFFFFFFFFFFFFF, right?

                // Save high half - MSB (64bit) - from vs0 after transaction to r5
                "mfvsrd  5, 0                 ;"

                // Save low half - LSB (64bit) - from vs0 after transaction to r6
                // We mess with vs3, but it's not important, we don't use that.
                "xxsldwi 3, 0, 0, 2           ;"
                "mfvsrd  6, 3                 ;"

                // r3 = 0x5555555555555555
                "lis  9, 0x5555            ;"
                "ori  9, 9, 0x5555         ;"
                "sldi 9, 9, 32             ;"
                "oris 9, 9, 0x5555         ;"
                "ori  9, 9, 0x5555         ;"

                // r4 = 0xFFFFFFFFFFFFFFFF
                "lis  10, 0xFFFF               ;"
                "ori  10, 10, 0xFFFF           ;"
                "sldi 10, 10, 32               ;"
                "oris 10, 10, 0xFFFF           ;"
                "ori  10, 10, 0xFFFF           ;"

                // N.B. r3 and r4 never changed since they were used to
                // construct the initial vs0 value, hence we can use them to do the
                // comparison. r3 and r4 will be destroy but it's not important.
                "cmpd  9, 5             	;" // compare r9 (expected value) to r5
                "bne   %[value_mismatch]	;"
                "cmpd  10, 6            	;" // compare r10 (expected value) to r6
                "bne   %[value_mismatch]	;"
                "b     %[value_ok]      	;"
                :
                :
                : "r3", "r4", "vs33", "vs34", "vs0", "vs10", "fr10", "r7", "r5", "r6", "vs3", "r9", "r10"
                : value_mismatch, value_ok
                );
value_mismatch:
		printf("value_mismatch()\n");
		asm(".long 0x0;");
		pthread_kill(pong_tid, SIGKILL);
                return NULL;
value_ok:
		printf("value_ok()\n");
		asm(".long 0x0;");
		goto _do_htm;
//		pthread_kill(pong_tid, SIGILL);
                return NULL;

/*
texasr = __builtin_get_texasr();

if (_TEXASR_FAILURE_CODE(texasr) == 0xe0) {  // If TM_CAUSE_KVM_RESCHED ignore and retry transaction
  printf(".\n");
  goto _do_htm;
}

else { // otherwise it was a TM_CAUSE_RESCHED (ie aborted due to a context switch, so exit generating a core file)
  printf("\nTEXASR: %"PRIx64" \n", texasr);
  printf(": Failure with error: %lx\n\n",  _TEXASR_FAILURE_CODE(texasr));
  asm(".long 0x0;"); // exit dumping all registers state
}
*/
}


void *pong(void *not_used)
{
 pong_tid = pthread_self();
 sleep(1); // wait pthread to be renamed to "pong". it's not necessary to reproduce the issue, but it's easier to install a selective probe using execname() == "pong".
 while(1) { sched_yield(); } // pong
}


void signal_handler(int signo, siginfo_t *si, void *notused)
{
 // We don't skip trap on purpose here
 printf("Got a trap instruction in HTM...\n");
} 

int main(int argc, char **argv)
{
 pthread_t t0, t1;
 pthread_attr_t attr;
 cpu_set_t cpuset;

 struct sigaction sa;

 sa.sa_flags = SA_SIGINFO;
 sa.sa_sigaction = signal_handler;
 sigaction(SIGTRAP, &sa, NULL);

 // Set only CPU 0 in the mask. Both thread will be bind to cpu 0
 CPU_ZERO(&cpuset);
 CPU_SET(0, &cpuset);

 // init pthread attribute
 pthread_attr_init(&attr);

 // Set CPU 0 mask into the pthread attribute
 pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

 pthread_create(&t0, &attr /* bind to cpu 0 */, ping, NULL);
 pthread_create(&t1, &attr /* bind to cpu 0 */, pong, NULL);

 pthread_setname_np(t0, "ping"); // for systemtap probe install convenience
 pthread_setname_np(t1, "pong"); // for systemtap probe install convenience

 // threads will never leave after joint but thread performing HTM will issue an illegal instruction and exit
 pthread_join(t0, NULL);
 pthread_join(t1, NULL);
}
