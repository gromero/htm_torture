#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <htmintrin.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>

#define LE 1UL // MSR LE == 1 => LE

int kill_myself = 0;

pthread_t t0_ping;
pthread_t t1_pong;

int trap_event = 0;
int le = 0;

void trap_signal_handler(int signo, siginfo_t *si, void *uc)
{

  ucontext_t *ucp = uc;
  uint64_t thread_endianness;

  /* Get thread endianness */
  thread_endianness = 1UL & ucp->uc_mcontext.gp_regs[PT_MSR];

  /*
   * Little-Endian Machine
   */

  if (le) {
    /* First trap event */
    if (trap_event == 0) {
      // Do nothing, since it is returning from this trap
      // event that endianness is flipped by the bug, so
      // we just let the process return from the signal
      // handler to check on the second trap event if
      // endianness is flipped or not.
    }
    /* Second trap event */
    if (trap_event == 1)
      // If (A) endiannes is still LE and since we got a trap
      // in TM, instruction (1) will be executed
      // on return (rollback to tbegin. + 4). As (1) does nothing in effect,
      // instruction (2) is executed again and so a 'trap' is executed (but not
      // in TM). On the other hand, if (B) endiannes is BE, i.e. flipped,
      // instruction (1) is treated as a b .+4, so instructions (3) and
      // (4) are executed ('tbegin.' and 'trap') instead and we get again
      // a trap (but in TM mode). In either case (A or B) we can then check
      // the MSR LE bit in the signal handler to see if endianness was
      // flipped (bug is present) or not on the first trap. Getting a trap in
      // non-TM mode (case A) or in TM mode (case B) are just worth noting
      // because after checking for endianness flip we manipulate NIP to
      // return a success or fail: NIP on trap + TM is the (tbegin. + 4)
      // address whilst NIP on trap - TM is the address of 'trap' instruction.
      if (thread_endianness == LE) {
        ucp->uc_mcontext.gp_regs[PT_NIP] += 16;  // Goto 'success'
      } else { // Thread endianness is BE, so it flipped inadvertently
        ucp->uc_mcontext.gp_regs[PT_MSR] |= 1UL; // Flip back to LE
        ucp->uc_mcontext.gp_regs[PT_NIP] += 4;   // Goto 'fail'
      }

  /*
   * Big-Endian Machine
   */

  } else {
    /* First trap event */
    if (trap_event == 0) {
      // Force thread endianness flip to LE
      ucp->uc_mcontext.gp_regs[PT_MSR] |= 1UL;
    /* Second trap event */
    } else if (trap_event == 1) {
      // Do nothing. If the bug is present on return from this
      // trap event endianness will flip back "automatically" to
      // BE, otherwise thread endianness will continue to be LE,
      // just as we set on first trap event above.
    /* A third trap event */
    } else {
      // If we reached out here it's because we got a third trap
      // event in TM, meaning that instruction (4) was
      // executed as 'trap', hence thread endianness
      // is still LE and so no bug flipped back to BE on return
      // from second trap event. Otherwise, bug flipped back to BE on
      // return from the second trap event and so instruction (4)
      // was executed as 'tdi 0, 0,-8065' (a skip so) and branch to
      // 'fail' is taken after instruction (5) to indicate failure
      // and we never reach out here.
      ucp->uc_mcontext.gp_regs[PT_MSR] &= ~1UL; // Flip back to BE
      ucp->uc_mcontext.gp_regs[PT_NIP] += 8;    // Goto 'success'
    }
  }

  trap_event++;
}

void *ping(void *not_used)
{

  uint64_t i;

  sleep(1);

  // Wait an amount of context switches so load_fp and load_vec overflows and
  // MSR_{FP|VEC|VSX} = 0
  for(i=0; i < 1024*1024*512; i++);

#ifdef MSR_FP
  // Set MSR_FP = 1 before entering in TM
  asm("fadd    10, 10, 10;");
#endif

#ifdef MSR_VEC
  // Set MSR_VEC = 1 before entering in TM
  asm("vaddcuw 10, 10, 10;");
#endif

  asm goto (
        "   tbegin.             ;" // (0) tbegin.
        "   tdi  0, 0, 0x48     ;" // (1) 'tdi' (skip) or 'b .+8'
        "   trap                ;" // (2) trap
        "  .long 0x1D05007C     ;" // (3) tbegin. in LE (BE machine)
        "  .long 0x0800e07f     ;" // (4) 'trap' in LE, 'tdi' (skip) in BE (BE machine)
        "   b %l[fail]          ;" // (5) MSR LE flipped back to BE, but it should not
        "   b %l[success]       ;" // (6) MSR LE is still LE, so ok

 : : : : fail, success);

fail:
  printf("yes :-(\n");
  goto bail_out;

success:
  printf("no! :-)\n");

bail_out:
  // Tell pong() to stop and return
  pthread_kill(t1_pong, SIGUSR1);
  return NULL;
}


void *pong(void *not_used)
{
  sleep(1);

  while (!kill_myself)
    // Induce a bunch of context switches in ping() thread that is by now just
    // sitting idle in the for() idle loop
    sched_yield();

  return NULL;
}



void usr1_signal_handler(int signo, siginfo_t *si, void *not_used)
{
  // On receiving a USR1 from ping(), just set kill_myself so pong() returns
  kill_myself = 1;
}

int main(int argc, char **argv)
{
  uint16_t k = 1;

  pthread_attr_t attr;
  cpu_set_t cpuset;

  struct sigaction trap_sa;

  trap_sa.sa_flags = SA_SIGINFO;
  trap_sa.sa_sigaction = trap_signal_handler;
  sigaction(SIGTRAP, &trap_sa, NULL);

  struct sigaction usr1_sa;

  usr1_sa.sa_flags = SA_SIGINFO;
  usr1_sa.sa_sigaction = usr1_signal_handler;
  sigaction(SIGUSR1, &usr1_sa, NULL);

  // Set only CPU 0 in the mask. Both threads will be bound to cpu 0.
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);

  // Init pthread attribute
  pthread_attr_init(&attr);

  // Bind thread ping() and pong() both to CPU 0 so they ping-pong
  // and fasten context switches on ping() thread, speeding up
  // load_fp and load_vec overflow.
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);


  // Detect machine endianness at run-time
  le = (int) *(uint8_t *)&k;

  if (le)
    printf("Little-Endian machine detected. Testing if endiannes flips inadvertently... ");
  else
    printf("Big-Endian machine detected. Testingif endiannes flips inadvertently... ");

  fflush(0);

  pthread_create(&t0_ping, &attr /* bind to cpu 0 */, ping, NULL);
  pthread_create(&t1_pong, &attr /* bind to cpu 0 */, pong, NULL);

  pthread_setname_np(t0_ping, "ping");
  pthread_setname_np(t1_pong, "pong");

  pthread_join(t0_ping, NULL);
  pthread_join(t1_pong, NULL);
}
