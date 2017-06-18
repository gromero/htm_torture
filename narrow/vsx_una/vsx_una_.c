#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>

void *ping(void *not_used)
{

uint64_t high; // tmp register to keep high 64bit (MSB) of a 128bit VSX register
uint64_t low;  // tmp register to keep low 64bit (LSB) of a 128bit VSX register

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

   /********************************************************
    *** ADJUST VSX0 EXPECTED VALUE AFTER AN HTM FAILURE  ***
    ********************************************************/
   // VSR0  (VSX/FP )                 = 0x5555555555555555555FFFFFFFFFFFFFFFF
   "xxmrghd 0,  33, 34                    ;"

   /****************************************************************
    *** ADJUST VSX32 (VMX0) EXPECTED VALUE AFTER AN HTM FAILURE  ***
    ****************************************************************/
   // VSR32 (VSX) = VR0 (VMX/Altivec) = 0x5555555555555555555FFFFFFFFFFFFFFFF
   "xxmrghd 32, 33, 34                    ;"


   : // no  output
   : [high] "r" (high),
     [low]  "r" (low)
   :
   );

// Wait an amount of context switches so load_fp and load_vec overflow and MSR.[FP VEC VSX| = 0
uint64_t i;
for(i=0; i<1024*1024*512; i++);

#ifdef MSR_VEC
 asm("vaddcuw 10, 10, 10;"); // set MSR.VEC = 1 before provoking a VSX unavailable in transactional mode.
#endif

#ifdef MSR_FP
 asm("fadd    10, 10, 10;"); // set MSR.FP = 1 before provoking a VSX unavailable in transaction mode.
#endif


// If MSR.VEC = 1 => VSX32/VMX0 is corrupted,
// If MSR.FP  = 1 => VSX0/FP0 is corrupted,
// If MSR.VEC=MSR.FP=1 => restore_math() called from trap 0x900 (Decrementer) will set also MSR.VSX=1 so
// the VSX unavailable exception in TM never happens and HTM commits the change (xxmrghd) without failing.

asm(
   "1: tbegin.             ;" // begin HTM
   "   beq       2f        ;" // failure handler, taken after the VSX unavailable exception
   "   xxmrghd  10, 10, 10 ;" // any VSX instruction in here
   "   tend.               ;" // end HTM
   "2: .long 0x0           ;" // failure handler just exits dumping all registers
   );

}

void *pong(void *not_used)
{
 // pong
 while(1)
  sched_yield();
}


int main(int argc, char **argv)
{
 pthread_t t0, t1;
 pthread_attr_t attr;
 cpu_set_t cpuset;

 // Set only CPU 0 in the mask. Both threads will be bound to cpu 0.
 CPU_ZERO(&cpuset);
 CPU_SET(0, &cpuset);

 // Init pthread attribute.
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
